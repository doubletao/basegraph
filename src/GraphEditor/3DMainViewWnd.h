#pragma once
#include "3DSceneViewWnd.h"

/*
 * 回显窗视图主窗口，内含多个子视图
*/

class C3DMainViewWnd : public CWnd
{
	DECLARE_DYNCREATE(C3DMainViewWnd)
	DECLARE_MESSAGE_MAP()
protected:
	std::vector<C3DSceneViewWnd *> m_vecSceneViewWnd;
public:
	C3DMainViewWnd();
	~C3DMainViewWnd();
protected:
	void RecalcLayout();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};