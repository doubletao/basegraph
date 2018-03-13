#include "stdafx.h"
#include "3DSceneViewWnd.h"
#include "GraphEditorCommonDef.h"

IMPLEMENT_DYNCREATE(C3DSceneViewWnd, CWnd)

const int CGSceneViewRenderPoolSize = 2;

BEGIN_MESSAGE_MAP(C3DSceneViewWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_SET_RENDERTASK, AddOneRenderTask)
	ON_MESSAGE(WM_EXCUTE_RENDERTASK, OnSetRenderTask)
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
		PostMessage(WM_EXCUTE_RENDERTASK);
	}
	RenderCountUnlock();
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
				RECT viewPort;
				pThis->GetClientRect(&viewPort);
				DX3DEngine_SetViewPort(hDevice, &viewPort);
				DX3DEngine_SetScissor(hDevice, &viewPort);
				OffsetRect(&viewPort, -viewPort.left / 2, -viewPort.top / 2);

				//上锁，要绘制了
				DataBaseLocker DataLocker;
				DXEngineLocker EngineLocker(hDevice);

				//取物件列表来
				std::vector<CString> vecObjID;
				if (DataBase_GetAllItemIDExceptSubItem(vecObjID))
				{
					//如果没有物件，则清屏
					if (vecObjID.size() < 1)
					{
						DX3DEngine_Lock(hDevice);
						int hSwapChainBuffer = DX3DEngine_GetSwapChainBuffer(hDevice, pThis->m_ViewInfo.m_hSwapChain);
						DX3DEngine_ClearTexture(hDevice, hSwapChainBuffer, RGBA(255, 125, 125, 255));
						DX3DEngine_Present(hDevice, pThis->m_ViewInfo.m_hSwapChain);
						DX3DEngine_UnLock(hDevice);
						DXTextureManager_UnQueryTexture(hTex);
						DXTextureManager_UnQueryTexture(hDepth);
						ResetEvent(pThis->m_hSeekReadyEvent);
						continue;
					}

					//设置帧号SetFrame
					int nCurFrame = DataRender_GetCurFrame();
					DataRender_SetFrame(nCurFrame);

					//遍历物件拿到bufer
					for (size_t i = 0; i < vecObjID.size(); i++)
					{
						CString strObjID = vecObjID[i];
						DataRender_UpdateRender(strObjID);
						if (false)
						{
							StuSimpleBuffer stuSimpleBuf;
							if (DataRender_GetBuffer(strObjID, stuSimpleBuf))
							{
								BufferFunction::WriteBitmap(_T("e:\\test.bmp"), stuSimpleBuf.nWidth, stuSimpleBuf.nHeight, 32, stuSimpleBuf.pBuffer);
							}
						}
						DXHalDef32 * pHalDef = nullptr;
						matrix4f worldMat;
						pThis->m_ViewInfo.m_fScale = 1.0f;
						if (DataRender_Get2DRenderMatrix(strObjID, pThis->m_ViewInfo.m_fScale, worldMat))
						{
							if (DataRender_GetDesHalDef(strObjID, pHalDef))
							{
								if (pHalDef)
								{
									//渲染bufer到3D
									DX3DEngine_RenderTexture(hDevice, pHalDef, &worldMat, &viewPort, FALSE, 1.0f);
									//DX3DEngine_CopyTexture(hDevice, hTex, viewPort.left, viewPort.top, viewPort.right - viewPort.left, viewPort.bottom - viewPort.top, pHalDef->m_hTex, 0, 0, pHalDef->m_rtBound.right - pHalDef->m_rtBound.left, pHalDef->m_rtBound.bottom - pHalDef->m_rtBound.top);
									if (false)
									{
										DX3DEngine_DumpTexture(hDevice, pHalDef->m_hTex, _T("e:\\test.bmp"));
										DX3DEngine_DumpRenderTarget(hDevice);
									}
								}
							}
						}
					}

					//刷新SwapChain
					if (false)
						DX3DEngine_DumpTexture(hDevice, hTex);
					int hSwapChainBuffer = DX3DEngine_GetSwapChainBuffer(hDevice, pThis->m_ViewInfo.m_hSwapChain);
					CRect rtClient;
					pThis->GetClientRect(rtClient);
					CRect rtDes = rtClient;
					//rtDes.OffsetRect(100, 0);

					if (rtClient.Width()>0)
					{
						if (false)
							DX3DEngine_DumpTexture(hDevice, hTex);
					}

					DX3DEngine_RGBA_To_BGRA(hDevice, hTex, &rtClient, hSwapChainBuffer, &rtDes);
					if (false)
						DX3DEngine_DumpTexture(hDevice, hSwapChainBuffer);
					DXTextureManager_UnQueryTexture(hTex);
					DXTextureManager_UnQueryTexture(hDepth);

					DX3DEngine_Present(hDevice, pThis->m_ViewInfo.m_hSwapChain, 0);

					OutputDebugString(_T("C3DSceneViewWnd::PreDecoderThread\n"));
				}
				pThis->RenderCountLock();
				//待刷新数自减
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

C3DSceneViewWnd::C3DSceneViewWnd()
	: m_nRenderCount(0)
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
}

void C3DSceneViewWnd::RecalcLayout()
{
}

void C3DSceneViewWnd::OnPaint()
{
	CPaintDC dc(this);
	PostMessage(WM_SET_RENDERTASK);
}

int C3DSceneViewWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CWnd::OnCreate(lpCreateStruct);

	m_thPreDecoderThread = CreateThread(NULL, 0, PreDecoderThread, this, 0, NULL);
	::SetThreadPriority(m_thPreDecoderThread, THREAD_PRIORITY_TIME_CRITICAL);

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

	OnPaint();
}

BOOL C3DSceneViewWnd::OnEraseBkgnd(CDC * pDC)
{
	return TRUE;
}
