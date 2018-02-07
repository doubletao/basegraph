#pragma once

class BaseGraphMainFrame :
	public CSkinFrameWnd
{
public:
	BaseGraphMainFrame();
	virtual ~BaseGraphMainFrame();
protected:
	DECLARE_DYNAMIC(BaseGraphMainFrame)
	// 生成的消息映射函数
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()
};

