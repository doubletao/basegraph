#pragma once
/*
* 回显窗视图子窗口，分布在回显窗视图主窗口中
*/

class C3DSceneViewWnd : public CWnd
{
	DECLARE_DYNCREATE(C3DSceneViewWnd)
	DECLARE_MESSAGE_MAP()
public:
	C3DSceneViewWnd();
	~C3DSceneViewWnd();
protected:
	void RecalcLayout();
	afx_msg void OnPaint();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};