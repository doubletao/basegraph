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
END_MESSAGE_MAP()

void C3DSceneViewWnd::RenderCountLock()
{
	EnterCriticalSection(&m_lockRendCount);
}

void C3DSceneViewWnd::RenderCountUnlock()
{
	LeaveCriticalSection(&m_lockRendCount);
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

DWORD C3DSceneViewWnd::PreDecoderThread(LPVOID pParam)
{
#if 0
	CSYCGSceneView* pThis = (CSYCGSceneView*)pParam;
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
				//seek逻辑
				CSYCGDocment* pDoc = SYCGEditorApp::GetInstance()->GetDocument();
				int hDevice = SYCGEditorApp::GetInstance()->GetDocument()->GetMainDevice();
				if (!hDevice)
					continue;

				pDoc->Lock();
				MAC_STARTTIME_TEST();
				int projectWidth = pDoc->GetProjectWidth();
				int projectHeigth = pDoc->GetProjectHeight();

				CSYCGSceneEnv* pSceneEnv = pDoc->GetCurSceneEnv();
				if (!pSceneEnv)
				{
					SY3DEngine_Lock(hDevice);
					for (ViewCallBackData *data = (ViewCallBackData*)g_viewlist.first; data; data = data->next)
					{
						int hSwapChainBuffer = SY3DEngine_GetSwapChainBuffer(hDevice, data->m_hSwapChain);
						SY3DEngine_ClearTexture(hDevice, hSwapChainBuffer, 0);
						SY3DEngine_Present(hDevice, data->m_hSwapChain);
					}
					SY3DEngine_UnLock(hDevice);
					pDoc->UnLock();
					continue;
				}
				double dCurFrame = pDoc->GetCurFrame();
				if (!m_pRenderParam)
				{
					m_pRenderParam = SYRenderer_Create(g_bGpuRender2D, pDoc->GetRenderPrecision());
					m_pRenderParam->RenderCallback.pRenderParam = m_pRenderParam;
				}
				//设置动静态方式
				if (pDoc->IsModifyStatic())
				{
					m_pRenderParam->bStaticRener = TRUE;
				}
				else
				{
					m_pRenderParam->bStaticRener = FALSE;
				}

				if (pDoc->IsModifyStaticChanged())
				{
					//如果动静态变化，需要强迫物件刷新
					pSceneEnv->SetAllChildChangeLevel(CSYCGEnv::Envchg_MaterialAnim);
					pDoc->ResetModifyStaticChanged();
				}

				//////////////////////////////////////////////////////////////////////////
				SYRenderer_SetCallbacks(m_pRenderParam, m_pRenderCallback, (UINT_PTR)&g_viewlist);
				SYRenderer_SetConfig(m_pRenderParam, pSceneEnv->GetDevice(), pSceneEnv, TRUE, 0);
				SYRenderer_SetFrame(m_pRenderParam, SY_DToI(dCurFrame), SY_DToI(dCurFrame));
				SYRenderer_CalcScene(m_pRenderParam); //仅仅根据当前设定的数据执行一次渲染，渲染完成后返回
				SYRenderParam* p = m_pRenderParam;
				int hDepth = SYTextureManager_QueryTexture(SYTexture_Depth, 1);
				for (ViewCallBackData *data = (ViewCallBackData*)g_viewlist.first; data; data = data->next)
				{
					//渲染场景
					if (!::IsWindowVisible(data->hViewWnd) || ::IsIconic(data->hViewWnd))
						continue;
					FillViewRenderParam(p, data);
					int nRenderType = SYCameraNormal;
					if (data->m_nWndIndex == SYWND_3DSCENEVIEW)
						nRenderType = pDoc->GetCameraType();
					pSceneEnv->SetRenderCameraEnv(data->m_nCameraMode);
					SY3DVisionCameraType nType = SY3DVison_NormalCamera;
					if (nRenderType == SYCameraLeftRight)
					{
						RenderSceneView_3DEyes(p, data, hDepth, 0);
					}
					else
					{
						CString viewName = data->szName;
						if (nRenderType == SYCameraNormal)
						{
							viewName += '0';
							nType = SY3DVison_NormalCamera;
						}
						else if (nRenderType == SYCameraLeft)
						{
							viewName += '1';
							nType = SY3DVison_LeftCamera;
						}
						else if (nRenderType == SYCameraRight)
						{
							viewName += '2';
							nType = SY3DVison_RightCamera;
						}
						RenderSceneView_Normal(p, data, viewName, nType, hDepth, 0);
					}
				}
				SYTextureManager_UnQueryTexture(hDepth);
				pDoc->UnLock();
				for (ViewCallBackData *data = (ViewCallBackData*)g_viewlist.first; data; data = data->next)
				{
					data->bViewInfoChange = FALSE;
					if (!::IsWindowVisible(data->hViewWnd) || ::IsIconic(data->hViewWnd))
						continue;
					SY3DEngine_Present(hDevice, data->m_hSwapChain, 0);
					//全部做完后，存一份窗口bmp
					if (data->hViewWnd && ::IsWindow(data->hViewWnd))
					{
						DeleteObject(pSceneEnv->m_hDupmedBmp);
						pSceneEnv->m_hDupmedBmp = SYCGlobalFunction::GetWndImage(data->hViewWnd);
						//然后通知工具背景变化了
						CSYBaseTool * pTool = SYTool_GetCurTool();
						if (pTool)
						{
							pTool->UpdateEditor(eUpdateEditorReason_BkImageRefresh, SYWND_3DSCENEVIEW, NULL, p->szViewName, 0, (LPARAM)data->hViewWnd);
						}
					}
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
			//
		}
		return TRUE;
	}
	else
	{
		return 0;
	}
#endif
	return 0;
}

C3DSceneViewWnd::C3DSceneViewWnd()
	: m_nRenderCount(0)
{
	InitializeCriticalSection(&m_lockRendCount);
}

C3DSceneViewWnd::~C3DSceneViewWnd()
{
	DeleteCriticalSection(&m_lockRendCount);
}

void C3DSceneViewWnd::RecalcLayout()
{
}

void C3DSceneViewWnd::OnPaint()
{
	CPaintDC dc(this);
	CRect rtClient;
	GetClientRect(rtClient);
	dc.FillSolidRect(rtClient, RGB(255, 0, 0));
}

int C3DSceneViewWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return CWnd::OnCreate(lpCreateStruct);
}

void C3DSceneViewWnd::OnDestroy()
{
	CWnd::OnDestroy();
}

void C3DSceneViewWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	if (!m_ViewInfo.m_hSwapChain)
	{
		m_ViewInfo.m_hSwapChain = DX3DEngine_CreateSwapChain(DX3DEngine_GetMainDevice(), m_hWnd, cx, cy);
	}
	else
	{
		DX3DEngine_ResizeSwapChain(DX3DEngine_GetMainDevice(), m_ViewInfo.m_hSwapChain, cx, cy);
	}
}

BOOL C3DSceneViewWnd::OnEraseBkgnd(CDC * pDC)
{
	return TRUE;
}
