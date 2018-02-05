#pragma once

#include "DUIButton.h"
#include <map>
#include <vector>
#include "SYNewMenu.h"

/*
框架窗口非客户区自绘类
*/


// CSkinFrameWnd frame

class SYCGCONTROLS_CLASS CSkinFrameWnd : public CFrameWnd
{
	DECLARE_DYNCREATE(CSkinFrameWnd)

protected:
	CSkinFrameWnd();           // protected constructor used by dynamic creation
	virtual ~CSkinFrameWnd();
	CDUIButton* AddTitleButton(LPRECT lpRect, CString strImage, UINT nCount = 3 , UINT nID = 0,LPCTSTR lpszCaption=NULL);
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
	afx_msg void OnDestroy();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnMove(int x, int y);
public:
	void SetChildMode(BOOL bChildMode);//设置为内嵌模式（内嵌模式下，标题栏不显示系统菜单，不响应单击双击事件，边缘不可拖动）
	BOOL IsChildMode(){return m_bChildMode;}
	void SetResIcon(CString strRes){m_strIcon = strRes;}
	void SetTitle(CString strTitle){m_strTitle = strTitle; OnNcPaint();};
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	int	   TButtonHitTest(CPoint point, CRect& rtButton);
	void	LoadBmpRes();
protected:
	BOOL m_bChildMode;
	int  m_nTitleHeight;
	int  m_nFrameWidth;
	std::map<int,CDUIButton *> m_TitleBtn;//标题按钮列表

	int    m_iXButtonHovering;
	BOOL   m_bNCHovering;
	BOOL   m_bNCTracking;
	CRgn m_Rgn;
	CRect  m_rcIcon;
	CRect  m_rtTitleBar;

	CImage * m_pImgBk;
	//三个标题按钮图片
	std::vector<CString> m_vecStrBtnImages;
	CString		m_strTitle;

	CString m_strIcon;

	CSYNewMenu		m_MainMenu;
};


