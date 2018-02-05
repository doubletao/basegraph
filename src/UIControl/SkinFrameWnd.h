#pragma once

#include <map>
#include <vector>

/*
框架窗口非客户区自绘类
*/


// CSkinFrameWnd frame

class CSkinFrameWnd : public CFrameWnd
{
	DECLARE_DYNCREATE(CSkinFrameWnd)

protected:
	CSkinFrameWnd();           // protected constructor used by dynamic creation
	virtual ~CSkinFrameWnd();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnNcMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnNcMouseLeave();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
public:
	void SetResIcon(CString strRes){m_strIcon = strRes;}
	void SetTitle(CString strTitle){m_strTitle = strTitle; OnNcPaint();}
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	int	   TButtonHitTest(CPoint point, CRect& rtButton);
	void	LoadBmpRes();
protected:
	BOOL m_bChildMode;
	int  m_nTitleHeight;
	int  m_nFrameWidth;

	int    m_iXButtonHovering;
	BOOL   m_bNCHovering;
	BOOL   m_bNCTracking;
	CRgn m_Rgn;
	CRect  m_rcIcon;
	CRect  m_rtTitleBar;

	CString		m_strTitle;

	CString m_strIcon;
};


