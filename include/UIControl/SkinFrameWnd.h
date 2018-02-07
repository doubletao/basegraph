#pragma once

#include <map>
#include <vector>
#include "UIButton.h"

/*
框架窗口非客户区自绘类
*/


// CSkinFrameWnd frame

class BASEGRAPH_UICONTROL_API CSkinFrameWnd : public CFrameWnd
{
	DECLARE_DYNCREATE(CSkinFrameWnd)
protected:
	DECLARE_MESSAGE_MAP()
protected:
	CRect m_rcIcon;
	CRect m_rtTitleBar;
	std::vector<UIButton *> m_vecButton;
	BOOL   m_bNCTracking;
	int    m_iXButtonHovering;
	BOOL   m_bNCHovering;
	CRgn m_Rgn;
public:
	CSkinFrameWnd();
	virtual ~CSkinFrameWnd();
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
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	int SysButtonHitTest(CPoint pt);
	void AddSysButton(int nImageN, int nImageF, int nImageD, int nImageX);
};


