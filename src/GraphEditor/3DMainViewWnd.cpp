#include "stdafx.h"
#include "3DMainViewWnd.h"
#include "GraphEditorCommonDef.h"

IMPLEMENT_DYNCREATE(C3DMainViewWnd, CWnd)

BEGIN_MESSAGE_MAP(C3DMainViewWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

C3DMainViewWnd::C3DMainViewWnd()
{
}

C3DMainViewWnd::~C3DMainViewWnd()
{
	for (size_t i = 0; i < m_vecSceneViewWnd.size(); i++)
	{
		C3DSceneViewWnd *& pViewWnd = m_vecSceneViewWnd[i];
		if (pViewWnd)
		{
			delete pViewWnd;
			pViewWnd = nullptr;
		}
	}
	m_vecSceneViewWnd.clear();
}

int C3DMainViewWnd::GetMsgDispatchID()
{
	return UIElementMsg_MainViewWnd;
}

void C3DMainViewWnd::OnDispatchMsg(int nMsgID, WPARAM wParam, LPARAM lParam, int nIDFrom)
{
	switch (nMsgID)
	{
	case WM_MSG_FrameChanged:
		if (nIDFrom != GetMsgDispatchID())
		{
			OnFrameChanged((int)wParam);
		}
		break;
	case WM_MSG_OnSelectChanged:
	case WM_MSG_SerRenderTask:
		if (nIDFrom != GetMsgDispatchID())
		{
			for (size_t i = 0; i < m_vecSceneViewWnd.size(); i++)
			{
				C3DSceneViewWnd *& pViewWnd = m_vecSceneViewWnd[i];
				if (pViewWnd)
				{
					pViewWnd->SendMessage(WM_MSG_SerRenderTask);
				}
			}
		}
	default:
		break;
	}
}

void C3DMainViewWnd::RecalcLayout()
{
	CRect rtClient;
	GetClientRect(rtClient);
	if (m_vecSceneViewWnd.size() > 0)
	{
		C3DSceneViewWnd *& pViewWnd = m_vecSceneViewWnd[0];
		if (pViewWnd)
		{
			pViewWnd->MoveWindow(rtClient);
		}
	}
}

void C3DMainViewWnd::OnFrameChanged(int nFrame)
{
	for (size_t i = 0; i < m_vecSceneViewWnd.size(); i++)
	{
		C3DSceneViewWnd *& pViewWnd = m_vecSceneViewWnd[i];
		if (pViewWnd)
		{
			pViewWnd->SendMessage(WM_MSG_SerRenderTask);
		}
	}
}

int C3DMainViewWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//目前暂时仅支持单视图，未来可以扩充多视图
	C3DSceneViewWnd * pSceneView = new C3DSceneViewWnd;
	if (pSceneView->Create(NULL, _T("主视图"), WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 200, 400), this, 0, NULL) == -1)
	{
		delete pSceneView;
		pSceneView = nullptr;
		OutputDebugString(_T("主回显窗创建失败\n"));
		return 0;
	}
	m_vecSceneViewWnd.push_back(pSceneView);

	//注册消息中心
	UIControl_RegistMsgDispatcher(this);

	return 0;
}

void C3DMainViewWnd::OnDestroy()
{
	CWnd::OnDestroy();
}

void C3DMainViewWnd::OnSize(UINT nType, int cx, int cy)
{
	RecalcLayout();
	CWnd::OnSize(nType, cx, cy);
}

BOOL C3DMainViewWnd::OnEraseBkgnd(CDC * pDC)
{
	return TRUE;
}
