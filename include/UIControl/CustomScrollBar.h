// ScrollBar.h: interface for the CScrollBar class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#define CustomScrollBarClass "CustomScrollBarClass"

#define CLSASS_BASE CWnd
class BASEGRAPH_UICONTROL_API CustomScrollBar : public CLSASS_BASE
{
public:
	CustomScrollBar();
	virtual ~CustomScrollBar();

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	static BOOL RegisterControlClass(HINSTANCE hInst = NULL);
	static BOOL UnregisterControlClass(HINSTANCE hInst = NULL);
	static LRESULT CALLBACK EXPORT
		WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	BOOL Create(DWORD dwStyle, CWnd* pParentWnd, HWND hWnd, UINT nId);
	BOOL Create(DWORD dwStyle, CWnd* pParentWnd, CRect& rect, UINT nId);

	BOOL GetScrollInfo(LPSCROLLINFO lpScrollInfo);
	BOOL SetScrollInfo(LPSCROLLINFO lpScrollInfo);
	int  GetScrollPos() const {return m_si.nPos;} 
	void SetScrollPos(int iPos);
	int  GetScrollPageSize() const {return m_si.nPage;} 
	BOOL GetScrollRange(int * pMinPos, int * pMaxPos);

public:
    void ScrollLeft();
    void ScrollRight();
    void ScrollPageUp();
    void ScrollPageDn();
    void ScrollToHome();
    void ScrollToEnd();
    void ScrollTo(CPoint point);

	void SetAutoHide(BOOL bAutoHide){m_bAutoHide = bAutoHide;}
	void HideArrow(){m_nArrowSize = 0;}

protected:
	BOOL		m_fIsAutoCreate;
    CWnd *		m_pParent;
    HCURSOR		m_hCursor;
    BOOL		m_bHorizontal;
    CRect		m_rtBarClient;
    CRect       m_rtHeadArrow;
    CRect       m_rtEndArrow;
    CRect       m_rtThumbClient;
    CRect       m_rtThumb;
    SCROLLINFO  m_si;
    int         m_iCounter;
    BOOL        m_bDragging;
    CPoint      m_ptDragOffset;
    BOOL        m_bLastHand;
    BOOL        m_bHover;
	BOOL		m_bAutoHide;//是否自动隐藏

	BOOL		m_bIsThumbHot;			//滚动条拖杆是否激活
	CImage		* m_pImgThumb;				// 滚动条拖杆图片
	CRect		m_rtThumbbmp;				// 滚动条拖杆图片大小
	CImage		* m_pImgThumbHot;			// 滚动条拖杆激活图片
	CRect		m_rtThumbHotbmp;			// 滚动条拖杆激活图片大小

	BOOL		m_bIsHeadArrowHot;		//其实箭头是否激活
	CImage		* m_pImgHeadArrow;			// 起始箭头图片
	CRect		m_rtHeadArrowbmp;			// 起始箭头图片大小
	CImage		* m_pImgHeadArrowHot;		// 起始箭头激活图片
	CRect		m_rtHeadArrowHotbmp;		// 起始箭头激活图片大小

	BOOL		m_bIsEndArrowHot;			//结尾箭头是否激活
	CImage		* m_pImgEndArrow;			// 结尾箭头图片
	CRect		m_rtEndArrowbmp;			// 结尾箭头图片大小
	CImage		* m_pImgEndArrowHot;		// 结尾箭头激活图片
	CRect		m_rtEndArrowHotbmp;		// 结尾箭头激活图片大小

	BOOL		m_bIsBackgroundHot;			//背底是否激活
	CImage		* m_pImgBackground;		// 背底图片
	CRect		m_rtBackgroundbmp;			//背景图片大小
	CImage		* m_pImgBackgroundHot;		// 背底激活图片
	CRect		m_rtBackgroundHotbmp;		//背景激活图片大小
	int			m_nArrowSize;

protected:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnDestroy();
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg LRESULT OnMouseHover(WPARAM wParam,LPARAM lParam);  
    afx_msg LRESULT OnMouseLeave(WPARAM wParam,LPARAM lParam);  
	afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()
       
    void ResetButton();
    void GetPageRect( CRect &rtPageUp, CRect &rtPageDn );
	void LoadScrollBmp(BOOL bHor);
};
