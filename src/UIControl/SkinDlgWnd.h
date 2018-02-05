#pragma once

#include "DUIButton.h"
#include <map>
#include <vector>

/*
框架窗口非客户区自绘类
*/


// CSkinDlgWnd frame

class SYCGCONTROLS_CLASS CSkinDlgWnd : public CDialog
{
	DECLARE_DYNCREATE(CSkinDlgWnd)
protected:
	CSkinDlgWnd();           // protected constructor used by dynamic creation
	CSkinDlgWnd(UINT nIDTemplate, CWnd* pParentWnd = NULL);           // protected constructor used by dynamic creation
	virtual ~CSkinDlgWnd();
     CDUIButton* AddTitleButton(LPRECT lpRect, CString strImage, UINT nCount = 3 , UINT nID = 0,LPCTSTR lpszCaption=NULL);
	 virtual BOOL PreTranslateMessage(MSG* pMsg);
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
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnNcMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnNcMouseLeave();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	HBRUSH  OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	CWnd * GetValidCtrlWnd(DWORD ID);
	BOOL GetToolTips(UINT nID,CString &strTips);
	void ModifyTooltips(UINT nID, const CString &strTips);
	void RemoveTooltips(UINT nID);

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	 int	   TButtonHitTest(CPoint point, CRect& rtButton);
	 void	LoadBmpRes();
protected:
	int  m_nTitleHeight;
	int  m_nFrameWidth;
	//标题按钮列表
	std::map<int,CDUIButton * > m_TitleBtn;
	int    m_iXButtonHovering;
	BOOL   m_bNCHovering;
	BOOL   m_bNCTracking;
	CRgn m_Rgn;
	CRect  m_rcIcon;

	CImage * m_pImgBk;
	//三个标题按钮图片
	CRect m_rcControl;
	std::vector<CString> m_vecStrBtnImages;
	CRect			m_rtMenuBar;		// 主菜单矩形
	CToolTipCtrl m_tt;
	std::map<UINT, CString> m_mapIDTips;
};


