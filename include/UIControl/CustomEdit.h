#pragma once

#include "UIControl\CustomScrollBar.h"

#define WM_SYEDIT_MSG_END_EDIT	WM_USER + 0x0014
#define CustomEditClass "CustomEditClass"

//****************************************************************************
class BASEGRAPH_UICONTROL_API CustomEdit : public CEdit
{
// Construction
	DECLARE_DYNAMIC(CustomEdit)
public:
	CustomEdit();

// Attributes
public:
	void SetBKBrush( CBrush* pBrush );
	void SetBkColor(COLORREF dwBkColor);

public:
// 注册窗口类
	static BOOL RegisterControlClass();
	static BOOL RegisterControlClass(HINSTANCE	hInst);

// 创建该种Edit的方法
	//建议style:
	//ES_MULTILINE|WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_BORDER|WS_VSCROLL|WS_HSCROLL|ES_WANTRETURN
	BOOL Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID );

	BOOL Create( LPCTSTR lpszCaption, DWORD dwStyle,  LPCRECT pRect = NULL, CWnd* pParentWnd = NULL,  UINT nID = 0);
	BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, 
						DWORD dwStyle,
						const RECT& rect,CWnd* pParentWnd, UINT nID, 
						CCreateContext* pContext = NULL);
	BOOL SubclassEdit( UINT nID, CWnd* pParent );
	BOOL AutoLoad( UINT nID, CWnd* pParent, BOOL fAutoDelete );

	void SetTextColor(COLORREF dwTextColor) { m_dwNormalText = dwTextColor; if(::IsWindow(m_hWnd))Invalidate();};
	void SetTextColor(COLORREF dwNormal,COLORREF dwDisable,COLORREF dwHilight);

	void SetBorderColor(COLORREF dwTopLeft, COLORREF dwBottomRight);

	CFont* SetFont(CFont* pFont, BOOL bRedraw = TRUE);
// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	public:
	virtual void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
// 该Edit类的Proc
	static LRESULT CALLBACK EXPORT WndProcHook(HWND, UINT, WPARAM, LPARAM);
	BOOL CreateScroolBar(BOOL bAdjustPos = FALSE);
	void AdjustScrollPos();
	void SetCursorArrowOnScrollBar(CPoint point);

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual ~CustomEdit();
	
	void SetAutoDeleteFlag( BOOL bIsAutoDel );
	// Generated message map functions
protected:
	afx_msg void OnNcPaint();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	COLORREF m_dwNORActiveBorder;   //活动边框
	COLORREF m_dwNORInActiveBorder; //非活动边框
	COLORREF m_dwDISActiveBorder;
	COLORREF m_dwDISInActiveBorder;
	COLORREF m_dwHighLightText; //加亮状态时字色
	COLORREF m_dwNormalText;    //一般状态时字色
	COLORREF m_dwDisableText;   //无效状态时字色
	CFont    m_textFont;

	DWORD	m_dwStyle;			//风格

	CBrush	m_BKBrush;
	CBrush	m_ctlBkBrush;

	CRect	m_rtClient;


	BOOL	m_fAutoDelete;	 // 该C++ CWnd object 需要自动删除
	BOOL	m_bIsKeyUp;
	CustomScrollBar *m_pVScrollBar;
	CustomScrollBar *m_pHScrollBar;
	//CDyStubWnd	 *m_pStub;
	int		m_nVScrollWidth; //垂直滚动条的宽度
	int		m_nHScrollHeight;//水平滚动条的高度
	BOOL	m_bBeginMove;
	CSize   m_szTopLeftBmp;
public:
	void ModifyTextRect();
private:
	static WNDPROC	lpfnSuperEdit;		//系统Edit的Proc
protected:
};
