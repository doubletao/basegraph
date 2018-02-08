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
protected:
	DECLARE_DYNAMIC(BaseGraphMainFrame)
	// 生成的消息映射函数
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
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
	DECLARE_MESSAGE_MAP()
};

