#pragma once
#include "UIObjectBar.h"

class BaseGraphMainFrame :
	public CSkinFrameWnd
{
private:
	UIObjectBar m_uiObjectBar;
public:
	BaseGraphMainFrame();
	virtual ~BaseGraphMainFrame();
	DECLARE_DYNAMIC(BaseGraphMainFrame)
	// 生成的消息映射函数
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg virtual void OnNcPaint();
	afx_msg virtual void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg virtual void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg virtual void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg virtual void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg virtual BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
	afx_msg LRESULT OnUIElementBtnClicked(WPARAM wParam = 0, LPARAM lParam = 0);
public:
	void OnBtnCreateTitle();
	void OnBtnCreateScroll();
	void OnBtnCreateImage();
};

