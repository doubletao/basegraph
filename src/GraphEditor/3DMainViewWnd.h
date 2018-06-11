#pragma once
#include "3DSceneViewWnd.h"

/*
 * 回显窗视图主窗口，内含多个子视图
*/

class C3DMainViewWnd 
	: public CWnd
	, public IMsgDispatchObject
{
	DECLARE_DYNCREATE(C3DMainViewWnd)
	DECLARE_MESSAGE_MAP()
protected:
	std::vector<C3DSceneViewWnd *> m_vecSceneViewWnd;
public:
	C3DMainViewWnd();
	~C3DMainViewWnd();
public:
	virtual int GetMsgDispatchID();
	virtual void OnDispatchMsg(int nMsgID, WPARAM wParam, LPARAM lParam, int nIDFrom);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
protected:
	void RecalcLayout();
	void OnFrameChanged(int nFrame);
protected:
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};