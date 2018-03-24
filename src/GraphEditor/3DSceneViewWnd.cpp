#include "stdafx.h"
#include "3DSceneViewWnd.h"
#include "GraphEditorCommonDef.h"
#include "GraphEditor.h"

IMPLEMENT_DYNCREATE(C3DSceneViewWnd, CWnd)

const int CGSceneViewRenderPoolSize = 2;

BEGIN_MESSAGE_MAP(C3DSceneViewWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_MSG_SerRenderTask, AddOneRenderTask)
	ON_MESSAGE(WM_MSG_ExcuteRenderTask, OnSetRenderTask)
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

void C3DSceneViewWnd::RenderCountLock()
{
	EnterCriticalSection(&m_lockRendCount);
}

void C3DSceneViewWnd::RenderCountUnlock()
{
	LeaveCriticalSection(&m_lockRendCount);
}

LRESULT C3DSceneViewWnd::OnSetRenderTask(WPARAM wParam, LPARAM lParam)
{
	SetEvent(m_hSeekReadyEvent);
	return 1L;
}

LRESULT C3DSceneViewWnd::AddOneRenderTask(WPARAM, LPARAM)
{
	RenderCountLock();
	//待刷新数没有达到池上限则+1
	if (m_nRenderCount < CGSceneViewRenderPoolSize)
	{
		++m_nRenderCount;
		PostMessage(WM_MSG_ExcuteRenderTask);
	}
	RenderCountUnlock();
	return 0;
}

BOOL C3DSceneViewWnd::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	SetCursor(m_hCursor);
	return 0;
}

struct DataBaseLocker
{
	DataBaseLocker()
	{
		DataBase_LockData();
	}
	~DataBaseLocker()
	{
		DataBase_UnlockData();
	}
};

struct DXEngineLocker
{
	int m_nDevice;
	DXEngineLocker(int nDevice)
		: m_nDevice(nDevice)
	{
		DX3DEngine_BeginRender(nDevice);
	}
	~DXEngineLocker()
	{
		DX3DEngine_EndRender(m_nDevice);
	}
};

DWORD C3DSceneViewWnd::PreDecoderThread(LPVOID pParam)
{
	C3DSceneViewWnd * pThis = (C3DSceneViewWnd*)pParam;
	if (pThis)
	{
		//执行
		::CoInitialize(NULL);
		while (TRUE)
		{
			HANDLE handleEmpty[2];
			handleEmpty[0] = pThis->m_hExitPreDecoderEvent;
			handleEmpty[1] = pThis->m_hSeekReadyEvent;
			DWORD dwObjectStart = ::WaitForMultipleObjects(2, handleEmpty, FALSE, INFINITE);
			if (dwObjectStart == WAIT_OBJECT_0)
			{
				break;
			}
			else if (dwObjectStart == WAIT_OBJECT_0 + 1)
			{
				std::vector<CString> vecObjID;
				if (DataBase_GetAllItemIDExceptSubItem(vecObjID))
				{
					//拿到视图缩放
					float fScale = pThis->m_ViewInfo.m_fScale;
					
					//形成贴片
					pThis->UpdateAllItemToHal(vecObjID, fScale);

					//拿到默认设备
					int hDevice = DX3DEngine_GetMainDevice();
					if (!hDevice)
						continue;

					//创建纹理及深度buf
					int hDepth = DXTextureManager_QueryTexture(DXTexture_Depth, 1);
					int hTex = DXTextureManager_QueryTexture(DXTexture_Normal2DTex, 1);
					DX3DEngine_ClearTexture(hDevice, hTex, RGBA(0, 0, 0, 0));
					DX3DEngine_ClearDepth(hDevice, hDepth, 1);

					//设定渲染目标
					DX3DEngine_SetRenderTarget(hDevice, hTex, hDepth);

					//设置好视窗
					CRect rtClient;
					pThis->GetClientRect(rtClient);
					RECT viewPort = { rtClient.left, rtClient.top, rtClient.right, rtClient.bottom };
					DX3DEngine_SetViewPort(hDevice, &viewPort);
					DX3DEngine_SetScissor(hDevice, &viewPort);

					//定位渲染中心点
					OffsetRect(&viewPort, -(pThis->m_ViewInfo.m_nSceneOffsetX + pThis->m_ViewInfo.m_nSceneWidth / 2), -(pThis->m_ViewInfo.m_nSceneOffsetY + pThis->m_ViewInfo.m_nSceneHeight / 2));

					//上锁，要绘制了
					DataBaseLocker DataLocker;
					DXEngineLocker EngineLocker(hDevice);

					//绘制物件
					pThis->RenderAllItemToTarget(vecObjID, hDevice, hTex, hDepth, fScale, viewPort);

					//绘制周边黑场
					pThis->RenderVirtualZone(hDevice, rtClient);

					//绘制三个buffer
					pThis->RenderTripleBuffer(hDevice);

					//将画面刷到SwapChain上
					if (false)
						DX3DEngine_DumpTexture(hDevice, hTex);
					int hSwapChainBuffer = DX3DEngine_GetSwapChainBuffer(hDevice, pThis->m_ViewInfo.m_hSwapChain);
					CRect rtDes = rtClient;
					//rtDes.OffsetRect(100, 0);

					if (rtClient.Width() > 0)
					{
						if (false)
							DX3DEngine_DumpTexture(hDevice, hTex);
					}

					DX3DEngine_RGBA_To_BGRA(hDevice, hTex, &rtClient, hSwapChainBuffer, &rtDes);
					if (false)
						DX3DEngine_DumpTexture(hDevice, hSwapChainBuffer);

					//OutputDebugString(_T("C3DSceneViewWnd::PreDecoderThread\n"));

					//善后
					DXTextureManager_UnQueryTexture(hTex);
					DXTextureManager_UnQueryTexture(hDepth);

					//刷新
					DX3DEngine_Present(hDevice, pThis->m_ViewInfo.m_hSwapChain, 0);
				}
				//待刷新数自减
				pThis->RenderCountLock();
				if (pThis->m_nRenderCount > 0)
				{
					--pThis->m_nRenderCount;
				}
				else
				{
					pThis->m_nRenderCount = 0;
					ResetEvent(pThis->m_hSeekReadyEvent);
				}
				pThis->RenderCountUnlock();
			}
		}
		return TRUE;
	}
	else
	{
		return 0;
	}
	return 0;
}

void C3DSceneViewWnd::RenderAllItemToTarget(std::vector<CString> vecObjID, int hDevice, int hTex, int hDepth, float fScale, RECT &viewPort)
{
	//如果没有物件，则清屏
	if (vecObjID.size() < 1)
	{
		int hSwapChainBuffer = DX3DEngine_GetSwapChainBuffer(hDevice, m_ViewInfo.m_hSwapChain);
		DX3DEngine_ClearTexture(hDevice, hSwapChainBuffer, RGBA(255, 125, 125, 255));
		return;
	}

	//遍历物件，取出其矩阵，然后贴上去
	for (size_t i = 0; i < vecObjID.size(); i++)
	{
		CString strObjID = vecObjID[i];
		DXHalDef32 * pHalDef = nullptr;
		matrix4f worldMat;
		if (DataRender_Get2DRenderMatrix(strObjID, fScale, worldMat))
		{
			if (DataRender_GetDesHalDef(strObjID, pHalDef))
			{
				if (pHalDef)
				{
					//渲染bufer到3D
					DX3DEngine_RenderTexture(hDevice, pHalDef, &worldMat, &viewPort, FALSE, 1.0f);
					if (false)
					{
						DX3DEngine_DumpTexture(hDevice, pHalDef->m_hTex, _T("e:\\test.bmp"));
						DX3DEngine_DumpRenderTarget(hDevice);
					}
				}
			}
		}
	}
}

void C3DSceneViewWnd::UpdateAllItemToHal(std::vector<CString> vecObjID, float fScale)
{
	//设置缩放
	DataRender_SetScale(fScale);
	//设置帧号SetFrame
	int nCurFrame = DataRender_GetCurFrame();
	DataRender_SetFrame(nCurFrame);
	//遍历并生成物件贴片
	for (size_t i = 0; i < vecObjID.size(); i++)
	{
		CString * pStrObjID = new CString(vecObjID[i]);
		m_ThreadPool.StartNewTask(UpdateItemToHalFunc, pStrObjID);
		//DataRender_UpdateRender(*pStrObjID);
		if (false)
		{
			StuSimpleBuffer stuSimpleBuf;
			if (DataRender_GetBuffer(*pStrObjID, stuSimpleBuf))
			{
				BufferFunction::WriteBitmap(_T("e:\\test.bmp"), stuSimpleBuf.nWidth, stuSimpleBuf.nHeight, 32, stuSimpleBuf.pBuffer);
			}
		}
	}
	int nTm = clock();
	m_ThreadPool.WaitAll();
	TRACE(_T("wait time = %d\n"), clock() - nTm);
}

DWORD C3DSceneViewWnd::UpdateItemToHalFunc(LPVOID pParam, HANDLE hEnd)
{
	CString * pStrObjID = (CString *)pParam;
	if (pStrObjID)
	{
		DataRender_UpdateRender(*pStrObjID);
		delete pStrObjID;
		pStrObjID = nullptr;
	}
	return 0;
}

C3DSceneViewWnd::C3DSceneViewWnd()
	: m_nRenderCount(0)
	, m_nSceneOutBackTex(0)
	, m_fSceneOutBackTexAlpha(0.7f)
	, m_bHandDraging(false)
	, m_ptHandDragStart(0)
	, m_hCursor(nullptr)
	, m_hDefaultCur(nullptr)
	, m_hHandDragCurOpen(nullptr)
	, m_hHandDragCurClose(nullptr)
{
	InitializeCriticalSection(&m_lockRendCount);
	m_thPreDecoderThread = NULL;
	m_hExitPreDecoderEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hSeekReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

C3DSceneViewWnd::~C3DSceneViewWnd()
{
	DeleteCriticalSection(&m_lockRendCount);
	SetEvent(m_hExitPreDecoderEvent);
	WaitForSingleObject(m_thPreDecoderThread, INFINITE);
	if (m_thPreDecoderThread)
		CloseHandle(m_thPreDecoderThread);
	if (m_hExitPreDecoderEvent)
		CloseHandle(m_hExitPreDecoderEvent);
	if (m_hSeekReadyEvent)
		CloseHandle(m_hSeekReadyEvent);
	if (m_nSceneOutBackTex)
	{
		DX3DEngine_ReleaseTexture(DX3DEngine_GetMainDevice(), m_nSceneOutBackTex);
	}
	DeleteObject(m_hDefaultCur);
	DeleteObject(m_hHandDragCurOpen);
	DeleteObject(m_hHandDragCurClose);
}

void C3DSceneViewWnd::AdjustViewFit()
{
	CRect rtClient;
	GetClientRect(rtClient);
	float fScale = rtClient.Width() / (float)DataBase_GetSceneWidth();
	fScale = min(fScale, rtClient.Height() / (float)DataBase_GetSceneHeight());
	ResetViewInfo(fScale);
	point2f ptCenter = {0.0f, 0.0f};
	SetZoomScale(fScale, ptCenter);
}

void C3DSceneViewWnd::AdjustViewReal()
{
	point2f ptCenter = { 0.0f, 0.0f };
	SetZoomScale(1.0f, ptCenter);
}

void C3DSceneViewWnd::SetZoomScale(float fScale, point2f ptFixed)
{
	point2f ptFixedWnd = SceneToWnd(ptFixed);
	//四倍是以1920  1080编辑为标准的，对于比这个更小的分辨率倍数可以不超过4*1920  4*1080的尺寸
	int nSceneWidth = DataBase_GetSceneWidth();
	int nSceneHeight = DataBase_GetSceneHeight();
	float fMaxRate = 4 * min(1920.0f / (float)nSceneWidth, nSceneHeight * 1.0f / (float)nSceneHeight);
	if (fScale > fMaxRate)
	{
		fScale = fMaxRate;
	}
	if (fScale < 0.25)
	{
		fScale = 0.25;
	}
	m_ViewInfo.m_fScale = fScale;

	ResetScaledViewInfo(ptFixed, ptFixedWnd);
}

float C3DSceneViewWnd::GetZoomScale()
{
	return m_ViewInfo.m_fScale;
}

void C3DSceneViewWnd::ResetScaledViewInfo(point2f ptFixedScene, point2f ptFixedWnd)
{
	ResetViewInfo(m_ViewInfo.m_fScale);

	//通过调整偏移量来确保指定点尽量保持不动
	point2f ptCurWnd = SceneToWnd(ptFixedScene);
	//TRACE(_T("ptFixedWnd = (%f, %f), ptCurWnd = (%f, %f), ptFixedScene = (%f, %f)\n"), ptFixedWnd.x, ptFixedWnd.y, ptCurWnd.x, ptCurWnd.y, ptFixedScene.x, ptFixedScene.y);
	m_ViewInfo.m_nSceneOffsetX -= ptCurWnd.x - ptFixedWnd.x;
	m_ViewInfo.m_nSceneOffsetY -= ptCurWnd.y - ptFixedWnd.y;

	PostMessage(WM_MSG_SerRenderTask);//刷新
}

void C3DSceneViewWnd::ResetViewInfo(float fScale)
{
	m_ViewInfo.m_fScale = fScale;
	//固定信息的计算
	m_ViewInfo.m_nSceneWidth = (int)(DataBase_GetSceneWidth() * m_ViewInfo.m_fScale);
	m_ViewInfo.m_nSceneHeight = (int)(DataBase_GetSceneHeight() * m_ViewInfo.m_fScale);
	CRect rtClient;
	GetClientRect(rtClient);
	m_ViewInfo.m_nWndWidth = rtClient.Width();
	m_ViewInfo.m_nWndHeight = rtClient.Height();

	m_ViewInfo.m_nSceneOffsetX = (m_ViewInfo.m_nWndWidth - m_ViewInfo.m_nSceneWidth) / 2;
	m_ViewInfo.m_nSceneOffsetY = (m_ViewInfo.m_nWndHeight - m_ViewInfo.m_nSceneHeight) / 2;
}

bool C3DSceneViewWnd::DealHandDragMsg(UINT nMsg, UINT nFlags, CPoint point)
{
	bool bAlt = ::GetKeyState(VK_MENU) & 0x8000;
	switch (nMsg)
	{
	case WM_LBUTTONDOWN:
		if (bAlt)
		{
			m_ptHandDragStart = point;
			m_bHandDraging = true;
			SetHandCloseCursor();
			SetCapture();
			return true;
		}
		break;
	case WM_MOUSEMOVE:
		if (m_bHandDraging && bAlt)
		{
			HandDragView(point);
			return true;
		}
		break;
	case WM_LBUTTONUP:
		if (m_bHandDraging)
		{
			m_bHandDraging = false;
			m_ptHandDragStart = 0;
			if (bAlt)
			{
				SetHandOpenCursor();
			}
			ReleaseCapture();
			return true;
		}
		break;
	default:
		break;
	}
	if (!bAlt)
	{
		SetDefaultCursor();
	}
	return false;
}

void C3DSceneViewWnd::RecalcLayout()
{
}

point2f C3DSceneViewWnd::SceneToWnd(point2f ptScene)
{
	point2f ptWnd = ptScene;
	ptWnd.x = ptScene.x * m_ViewInfo.m_fScale + m_ViewInfo.m_nSceneOffsetX + m_ViewInfo.m_nSceneWidth / 2;
	ptWnd.y = m_ViewInfo.m_nSceneOffsetY + m_ViewInfo.m_nSceneHeight / 2 - ptScene.y * m_ViewInfo.m_fScale;
	return ptWnd;
}

point2f C3DSceneViewWnd::WndToScene(point2f ptWnd)
{
	point2f ptScene = ptWnd;
	ptScene.x = (ptWnd.x - m_ViewInfo.m_nSceneOffsetX - m_ViewInfo.m_nSceneWidth / 2) / m_ViewInfo.m_fScale;
	ptScene.y = -(ptWnd.y - m_ViewInfo.m_nSceneOffsetY - m_ViewInfo.m_nSceneHeight / 2) / m_ViewInfo.m_fScale;
	return ptScene;
}

point2f C3DSceneViewWnd::SceneToWnd(CPoint ptScene)
{
	point2f pt = { ptScene.x, ptScene.y };
	return SceneToWnd(pt);
}

point2f C3DSceneViewWnd::WndToScene(CPoint ptWnd)
{
	point2f pt = { ptWnd.x, ptWnd.y};
	return WndToScene(pt);
}

void C3DSceneViewWnd::RenderVirtualZone(int nDevice, const CRect& rtClient)
{
	int hTex = DXTextureManager_QueryTexture(DXTexture_Normal2DTex, 1);
	DX3DEngine_ClearTexture(nDevice, hTex, 0);

	RECT rtDraw = { m_ViewInfo.m_nSceneOffsetX,
		m_ViewInfo.m_nSceneOffsetY ,
		m_ViewInfo.m_nSceneOffsetX + m_ViewInfo.m_nSceneWidth
		,m_ViewInfo.m_nSceneOffsetY + m_ViewInfo.m_nSceneHeight };

	if (!m_nSceneOutBackTex)
	{
		CImage * pImgSceneLayoutBk = UIControl_GetImage(ThemeImage_SceneLayoutBk);
		m_nSceneOutBackTex = DX3DEngine_CreateTexFromBuffer(nDevice, (DWORD *)pImgSceneLayoutBk->GetBits(), pImgSceneLayoutBk->GetWidth(), pImgSceneLayoutBk->GetHeight(), DXTexture_Normal2DTex);
	}
	DrawEditBackBuffer(nDevice, rtClient, rtDraw, hTex, m_nSceneOutBackTex, m_fSceneOutBackTexAlpha);
	DX3DEngine_BeginRenderTexture(nDevice);
	DX3DEngine_RenderTexture(nDevice, hTex, 1, &rtClient, &rtClient, TRUE);
	DX3DEngine_EndRenderTexture(nDevice);
	DXTextureManager_UnQueryTexture(hTex);
}

void C3DSceneViewWnd::RenderTripleBuffer(int nDevice)
{
	RECT oldViewPort;
	DX3DEngine_GetViewPort(nDevice, &oldViewPort);

	int hCtrlHal = m_BufferInfo.GetControlTexture();
	int hEditHal = m_BufferInfo.GetEditTexture();
	int hUIHal = m_BufferInfo.GetUITexture();

	CRect rtClient;
	GetClientRect(rtClient);

	RenderCtrlBuffer(nDevice);//画安全框和选中框
	UpdateBufferToHal(nDevice, hCtrlHal, rtClient, m_BufferInfo.GetControlMemBuffer());
	RenderUIBuffer(nDevice);//画界面元素
	UpdateBufferToHal(nDevice, hEditHal, rtClient, m_BufferInfo.GetEditMemBuffer());

	RECT rtDst = rtClient;
	RECT rtCtrlSrc = rtDst;

	DX3DEngine_SetViewPort(nDevice, &rtCtrlSrc);
	DX3DEngine_BeginRenderTexture(nDevice);
	DX3DEngine_RenderTexture(nDevice, hCtrlHal, 1, &rtCtrlSrc, &rtDst, FALSE);
	DX3DEngine_RenderTexture(nDevice, hEditHal, 1, &rtCtrlSrc, &rtDst, FALSE);
	DX3DEngine_RenderTexture(nDevice, hUIHal, 1, &rtCtrlSrc, &rtDst, FALSE);
	DX3DEngine_EndRenderTexture(nDevice);

	if (0)
	{
		DX3DEngine_DumpRenderTarget(nDevice);
		DX3DEngine_DumpTexture(nDevice, hCtrlHal);
		DX3DEngine_DumpTexture(nDevice, hEditHal);
		DX3DEngine_DumpTexture(nDevice, hUIHal);
	}
	DX3DEngine_SetViewPort(nDevice, &oldViewPort);

	return;
}

void C3DSceneViewWnd::RenderCtrlBuffer(int nDevice)
{
	int nWidth = DataBase_GetSceneWidth();
	int nHeight = DataBase_GetSceneHeight();
	int nCtrlBuffer = m_BufferInfo.GetControlMemBuffer();
	//准备好DC
	CSYBufferDC bufferDC;
	//清空画布
	DataRender_Clear2DBuf(nCtrlBuffer, 0);
	// 先画出编辑控制线，后画选中。
	StuSimpleBuffer stuSimBuf;
	DataRender_Get2DBuf(nCtrlBuffer, stuSimBuf);
	bufferDC.AttachBuffer((DWORD *)stuSimBuf.pBuffer, stuSimBuf.nWidth, stuSimBuf.nHeight);
	bufferDC.SetZoomRef(m_ViewInfo.m_fScale);
	CPoint ptOffset(m_ViewInfo.m_nSceneOffsetX, m_ViewInfo.m_nSceneOffsetY);
	bufferDC.SetOffsetRef(ptOffset.x, ptOffset.y);
	//拿出安全框配置
	SafeLineSettingParam & param = CGraphEditorApp::GetBaseGraphFrameWnd()->GetSafeLineSettingParam();
	//画安全框
	if (param.bUse4_3SafeLine)
	{
		if (param.bDrawSafeLine1)
		{
			bufferDC.SetPenColor(param.nColorSafeLine1);
			float fRatio = param.fSafeLine1Percent;
			int nWidthTmp = nWidth;
			int nHeightTmp = nHeight;
			if ((float)nWidth / nHeight > 4.0f / 3)
			{
				nWidthTmp = (float)nHeight * 4 / 3;
			}
			else
			{
				nHeightTmp = (float)nWidth * 3 / 4;
			}
			CRect rtSafe(0, 0, nWidthTmp, nHeightTmp);
			rtSafe.right *= fRatio;
			rtSafe.bottom *= fRatio;
			rtSafe.OffsetRect((nWidth - rtSafe.Width()) / 2, (nHeight - rtSafe.Height()) / 2);
			bufferDC.DrawRect(rtSafe);
		}
		if (param.bDrawSafeLine2)
		{
			bufferDC.SetPenColor(param.nColorSafeLine2);
			float fRatio = param.fSafeLine2Percent;
			int nWidthTmp = nWidth;
			int nHeightTmp = nHeight;
			if ((float)nWidth / nHeight > 4.0f / 3)
			{
				nWidthTmp = (float)nHeight * 4 / 3;
			}
			else
			{
				nHeightTmp = (float)nWidth * 3 / 4;
			}
			CRect rtSafe(0, 0, nWidthTmp, nHeightTmp);
			rtSafe.right *= fRatio;
			rtSafe.bottom *= fRatio;
			rtSafe.OffsetRect((nWidth - rtSafe.Width()) / 2, (nHeight - rtSafe.Height()) / 2);
			bufferDC.DrawRect(rtSafe);
		}
	}
	else
	{
		if (param.bDrawSafeLine1)
		{
			bufferDC.SetPenColor(param.nColorSafeLine1);
			float fRatio = param.fSafeLine1Percent;
			CRect rtSafe(0, 0, nWidth, nHeight);
			rtSafe.right *= fRatio;
			rtSafe.bottom *= fRatio;
			rtSafe.OffsetRect((nWidth - rtSafe.Width()) / 2, (nHeight - rtSafe.Height()) / 2);
			bufferDC.DrawRect(rtSafe);
		}
		if (param.bDrawSafeLine2)
		{
			bufferDC.SetPenColor(param.nColorSafeLine2);
			float fRatio = param.fSafeLine2Percent;
			CRect rtSafe(0, 0, nWidth, nHeight);
			rtSafe.right *= fRatio;
			rtSafe.bottom *= fRatio;
			rtSafe.OffsetRect((nWidth - rtSafe.Width()) / 2, (nHeight - rtSafe.Height()) / 2);
			bufferDC.DrawRect(rtSafe);
		}
	}
	//画十字基准线
	if (param.bDrawCrossLine)
	{
		bufferDC.SetPenColor(param.nColorCrossLine);
		bufferDC.MoveTo(nWidth / 2, 0);
		bufferDC.LineTo(nWidth / 2, nHeight);
		bufferDC.MoveTo(0, nHeight / 2);
		bufferDC.LineTo(nWidth, nHeight / 2);
	}
}

void C3DSceneViewWnd::RenderUIBuffer(int nDevice)
{
}

BOOL C3DSceneViewWnd::UpdateBufferToHal(int hDevice, int hDesHal, RECT rtDirty, int n2DBufferID)
{
	if (false)
	{
		DataRender_DebugOutPut2DBuffer(n2DBufferID, _T("E:debug.bmp"));
	}
	StuSimpleBuffer stuBuffer;
	DataRender_Get2DBuf(n2DBufferID, stuBuffer);
	int hTrans = DXTextureManager_QueryTexture(DXTexture_MemTexture, 1);
	DX3DEngine_UpLoad(
		hDesHal,
		hDevice,
		hTrans,
		rtDirty.left,
		rtDirty.top,
		rtDirty.right - rtDirty.left,
		rtDirty.bottom - rtDirty.top,
		(DWORD *)stuBuffer.pBuffer,
		rtDirty.left,
		rtDirty.top,
		rtDirty.right - rtDirty.left,
		rtDirty.bottom - rtDirty.top,
		stuBuffer.nWidth,
		stuBuffer.nHeight);
	DXTextureManager_UnQueryTexture(hTrans);
	return 1;
}

void C3DSceneViewWnd::DrawEditBackBuffer(int hDevice, const CRect & rtInValid, const CRect & rtValid, int nRTTex, int hTex, float fAlpha)
{
	if (false)
	{
		TRACE(_T("rtInValid:(%d, %d, %d, %d)\n"), rtInValid.left, rtInValid.top, rtInValid.right, rtInValid.bottom);
		TRACE(_T("rtValid:(%d, %d, %d, %d)\n"), rtValid.left, rtValid.top, rtValid.right, rtValid.bottom);
	}
	int hOldRt, hOldDepth;
	if (0)
		DX3DEngine_DumpTexture(hDevice, hTex);
	DX3DEngine_GetRenderTarget(hDevice, &hOldRt, &hOldDepth);
	DX3DEngine_SetRenderTarget(hDevice, nRTTex, NULL);
	DX3DEngine_SetViewPort(hDevice, &rtInValid);
	DX3DEngine_BeginRenderTexture(hDevice);
	RECT srcRt = { 0,0,0,0 };
	DX3DEngine_GetTextureInfo(hDevice, hTex, (int*)&srcRt.right, (int*)&srcRt.bottom, NULL);
	//背景为纯色，采用拉伸绘制，比平铺效率高
	DX3DEngine_RenderTexture(hDevice, hTex, fAlpha, &srcRt, &rtInValid, FALSE, BOTH_FIELD, TRUE);
	DX3DEngine_EndRenderTexture(hDevice);
	if (0)
		DX3DEngine_DumpTexture(hDevice, nRTTex);
	DX3DEngine_SetViewPort(hDevice, &rtValid);
	DX3DEngine_ClearScreen(hDevice, 0x00000000, 1, DX_CLEAR_RENDERTARGET_ONLY);
	DX3DEngine_SetRenderTarget(hDevice, hOldRt, hOldDepth);
	if (0)
		DX3DEngine_DumpTexture(hDevice, nRTTex);
}

void C3DSceneViewWnd::HandDragView(CPoint pt)
{
	int nXOffet = pt.x - m_ptHandDragStart.x;
	int nYOffet = pt.y - m_ptHandDragStart.y;

	m_ViewInfo.m_nSceneOffsetX += nXOffet;
	m_ViewInfo.m_nSceneOffsetY += nYOffet;

	m_ptHandDragStart = pt;

	PostMessage(WM_MSG_SerRenderTask);//刷新
}

void C3DSceneViewWnd::SetDefaultCursor()
{
	if (!m_hDefaultCur)
	{
		m_hDefaultCur = LoadCursor(NULL, IDC_ARROW);
	}
	m_hCursor = m_hDefaultCur;
	SendMessage(WM_SETCURSOR);
}

void C3DSceneViewWnd::SetHandOpenCursor()
{
	if (!m_hHandDragCurOpen)
	{
		CString strFilePath = UIControl_GetThemePath() + UIControl_GetCString(ThemeCString_CursorOpenHandName);
		m_hHandDragCurOpen = LoadCursorFromFile(strFilePath);
	}
	m_hCursor = m_hHandDragCurOpen;
	SendMessage(WM_SETCURSOR);
}

void C3DSceneViewWnd::SetHandCloseCursor()
{
	if (!m_hHandDragCurClose)
	{
		CString strFilePath = UIControl_GetThemePath() + UIControl_GetCString(ThemeCString_CursorCloseHandName);
		m_hHandDragCurClose = LoadCursorFromFile(strFilePath);
	}
	m_hCursor = m_hHandDragCurClose;
	SendMessage(WM_SETCURSOR);
}

void C3DSceneViewWnd::OnPaint()
{
	CPaintDC dc(this);
	PostMessage(WM_MSG_SerRenderTask);//刷新
}

int C3DSceneViewWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CWnd::OnCreate(lpCreateStruct);

	m_BufferInfo.Initialize(DataBase_GetSceneWidth(), DataBase_GetSceneHeight());

	m_thPreDecoderThread = CreateThread(NULL, 0, PreDecoderThread, this, 0, NULL);
	::SetThreadPriority(m_thPreDecoderThread, THREAD_PRIORITY_TIME_CRITICAL);

	SetDefaultCursor();

	return 0;
}

void C3DSceneViewWnd::OnDestroy()
{
	CWnd::OnDestroy();
}

void C3DSceneViewWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (cx <= 0 || cy <= 0)
		return;

	if (!m_ViewInfo.m_hSwapChain)
	{
		m_ViewInfo.m_hSwapChain = DX3DEngine_CreateSwapChain(DX3DEngine_GetMainDevice(), m_hWnd, cx, cy);
	}
	else
	{
		DX3DEngine_ResizeSwapChain(DX3DEngine_GetMainDevice(), m_ViewInfo.m_hSwapChain, cx, cy);
	}

	AdjustViewFit();
}

BOOL C3DSceneViewWnd::OnEraseBkgnd(CDC * pDC)
{
	return TRUE;
}

BOOL C3DSceneViewWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	float fScale = GetZoomScale();
	fScale += (float)zDelta / 2000;

	ScreenToClient(&pt);
	point2f ptScene = WndToScene(pt);

	SetZoomScale(fScale, ptScene);
	return 0;
}

void C3DSceneViewWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CRect rtClient;
	GetClientRect(rtClient);
	float fScale = rtClient.Width() / (float)DataBase_GetSceneWidth();
	fScale = min(fScale, rtClient.Height() / (float)DataBase_GetSceneHeight());
	if (fabs(m_ViewInfo.m_fScale - fScale) < ABS_DOUBLE_0)
	{
		AdjustViewReal();
	}
	else
	{
		AdjustViewFit();
	}
}

void C3DSceneViewWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (DealHandDragMsg(WM_LBUTTONDOWN, nFlags, point))
	{
		return;
	}
	return CWnd::OnLButtonDown(nFlags, point);
}

void C3DSceneViewWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (DealHandDragMsg(WM_LBUTTONUP, nFlags, point))
	{
		return;
	}
	return CWnd::OnLButtonDown(nFlags, point);
}

void C3DSceneViewWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (DealHandDragMsg(WM_MOUSEMOVE, nFlags, point))
	{
		return;
	}
	return CWnd::OnLButtonDown(nFlags, point);
}
