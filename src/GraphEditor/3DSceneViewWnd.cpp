#include "stdafx.h"
#include "3DSceneViewWnd.h"

IMPLEMENT_DYNCREATE(C3DSceneViewWnd, CWnd)

BEGIN_MESSAGE_MAP(C3DSceneViewWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

C3DSceneViewWnd::C3DSceneViewWnd()
{
}

C3DSceneViewWnd::~C3DSceneViewWnd()
{
}

void C3DSceneViewWnd::RecalcLayout()
{
}

void C3DSceneViewWnd::OnPaint()
{
	CPaintDC dc(this);
	CRect rtClient;
	GetClientRect(rtClient);
	dc.FillSolidRect(rtClient, RGB(0, 0, 0));
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
}

BOOL C3DSceneViewWnd::OnEraseBkgnd(CDC * pDC)
{
	return TRUE;
}
