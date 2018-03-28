// ScrollBarEx.cpp: implementation of the CScrollBarEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIControl\CustomScrollBar.h"
#include "ThemeManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

class CScrollBarMemDC : public CDC
{
private:
	CBitmap*	m_bitmap;
	CBitmap*	m_oldBitmap;
	CDC*		m_pDC;
	CRect		m_rcBounds;
public:
	CScrollBarMemDC(CDC* pDC, const CRect& rcBounds) : CDC()
	{
		CreateCompatibleDC(pDC);
		m_bitmap = new CBitmap;
		m_bitmap->CreateCompatibleBitmap(pDC, rcBounds.Width(), rcBounds.Height());
		m_oldBitmap = SelectObject(m_bitmap);
		m_pDC = pDC;
		m_rcBounds = rcBounds;
	}
	~CScrollBarMemDC()
	{
		m_pDC->BitBlt(m_rcBounds.left, m_rcBounds.top, m_rcBounds.Width(), m_rcBounds.Height(),
			this, m_rcBounds.left, m_rcBounds.top, SRCCOPY);
		SelectObject(m_oldBitmap);
		if (m_bitmap != NULL)
			delete m_bitmap;
	}
	CScrollBarMemDC* operator->()
	{
		return this;
	}
};


#ifndef IDC_HAND
#define IDC_HAND MAKEINTRESOURCE(32649)		// From WINUSER.H
#endif

#define TIMER_LEFTUP_LBUTTON_PRESSED		2	
#define TIMER_RIGHTDN_LBUTTON_PRESSED		3	
#define TIMER_PAGEUP_LBUTTON_PRESSED		4	
#define TIMER_PAGEDN_LBUTTON_PRESSED		5

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CustomScrollBar, CLSASS_BASE)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


CustomScrollBar::CustomScrollBar()
	: m_bAutoHide(TRUE)
	, m_pImgThumb(nullptr)
	, m_pImgThumbHot(nullptr)
	, m_pImgHeadArrow(nullptr)
	, m_pImgHeadArrowHot(nullptr)
	, m_pImgEndArrow(nullptr)
	, m_pImgEndArrowHot(nullptr)
	, m_pImgBackground(nullptr)
	, m_pImgBackgroundHot(nullptr)
{
    m_pParent              = NULL;
    m_hCursor              = NULL;
    m_bHorizontal          = FALSE;
    m_rtBarClient          = CRect(-1,-1,-1,-1);
    m_rtHeadArrow		   = CRect(-1,-1,-1,-1);
    m_rtEndArrow		   = CRect(-1,-1,-1,-1);
    m_rtThumbClient        = CRect(-1,-1,-1,-1);
    m_iCounter             = 0;
    m_bDragging            = FALSE;
    m_ptDragOffset         = 0;
    m_bLastHand            = FALSE;
    m_bHover               = FALSE;
	m_fIsAutoCreate			= FALSE;
    DWORD si[] = {sizeof(SCROLLINFO), SIF_ALL, 0, 100, 10, 0, 0};
    memcpy(&m_si, si, sizeof(SCROLLINFO));
	m_nArrowSize = 20;

	m_bIsHeadArrowHot = m_bIsEndArrowHot = m_bIsThumbHot = m_bIsBackgroundHot = FALSE;
}

CustomScrollBar::~CustomScrollBar()
{
}

BOOL CustomScrollBar::PreCreateWindow(CREATESTRUCT& cs) 
{
	ASSERT( !m_fIsAutoCreate );	
	cs.lpszClass = AfxRegisterWndClass( CS_HREDRAW|CS_VREDRAW|CS_OWNDC );
	return CLSASS_BASE::PreCreateWindow(cs);
}

BOOL CustomScrollBar::RegisterControlClass(HINSTANCE	hInst)
{
	if (hInst == NULL)		hInst = AfxGetInstanceHandle();

	WNDCLASS	wcls;
	// check to see if class already registered
	if (::GetClassInfo(hInst, _T(CustomScrollBarClass), &wcls)){
		// name already registered - ok if it was us
		return (wcls.lpfnWndProc == (WNDPROC)CustomScrollBar::WndProcHook);
	}

	memset(&wcls, sizeof(WNDCLASS), 0);

	// Use standard "static" control as a template.
	VERIFY(::GetClassInfo(NULL, _T("Static"), &wcls));

	// set new values
	wcls.style|=CS_DBLCLKS;
	wcls.lpfnWndProc = CustomScrollBar::WndProcHook;
	wcls.hInstance = hInst;
	wcls.lpszClassName = _T(CustomScrollBarClass);
	if(AfxRegisterClass(&wcls) == 0)
		return	FALSE;
	return TRUE;
}

BOOL CustomScrollBar::UnregisterControlClass(HINSTANCE hInst)
{
	if (hInst == NULL)		 hInst = AfxGetInstanceHandle();
	return UnregisterClass(_T(CustomScrollBarClass), hInst);
}

LRESULT CALLBACK EXPORT
	CustomScrollBar::WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	CustomScrollBar* pScrollCtrl = (CustomScrollBar*)FromHandlePermanent(hWnd);

	if(!pScrollCtrl){
		// create new item and attach it
		pScrollCtrl = new CustomScrollBar;
		pScrollCtrl->Attach(hWnd);
		pScrollCtrl->m_fIsAutoCreate = TRUE;
	}

	pScrollCtrl->m_pfnSuper = NULL;

	::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (DWORD_PTR)AfxWndProc);

	// then call it for this first message
#ifdef STRICT
	return ::CallWindowProc(AfxWndProc, hWnd, msg, wParam, lParam);
#else
	return ::CallWindowProc((FARPROC)AfxWndProc, hWnd, msg, wParam, lParam);
#endif
}

BOOL CustomScrollBar::Create(DWORD dwStyle, CWnd* pParentWnd, HWND hWnd, UINT nId)
{
    ASSERT(pParentWnd);
    ASSERT(IsWindow(pParentWnd->m_hWnd));
    ASSERT(::IsWindow(hWnd));
    
    CRect rect;
    ::GetWindowRect(hWnd, &rect);
    pParentWnd->ScreenToClient(&rect);
    
    ::ShowWindow(hWnd, SW_HIDE);

	m_bHorizontal = (dwStyle & SBS_VERT) ? FALSE : TRUE;
	LoadScrollBmp(m_bHorizontal);
    
	return Create(dwStyle, pParentWnd, rect, nId);
}

BOOL CustomScrollBar::Create(DWORD dwStyle, CWnd* pParentWnd, CRect& rect, UINT nId)
{
    if (pParentWnd == NULL || !IsWindow(pParentWnd->m_hWnd)) ASSERT(FALSE);
    
	WNDCLASS	wcls;
	HINSTANCE	hInst = AfxGetInstanceHandle();
	if (!::GetClassInfo(hInst, _T(CustomScrollBarClass), &wcls))
	{
		RegisterControlClass(hInst);
	}

    m_pParent = pParentWnd;
    m_hCursor = AfxGetApp()->LoadStandardCursor(IDC_HAND);
    m_bHorizontal = (dwStyle & SBS_VERT) ? FALSE : TRUE;
    
	LoadScrollBmp(m_bHorizontal);

//     return CLSASS_BASE::Create(AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW | CS_OWNDC, 0, 0, 0),
//         _T(""), dwStyle, rect, pParentWnd, nId);
	return CLSASS_BASE::Create(_T(CustomScrollBarClass),NULL, dwStyle|WS_CLIPSIBLINGS, rect, pParentWnd, nId, NULL);
}

BOOL CustomScrollBar::OnEraseBkgnd(CDC* pDC) 
{
    return TRUE;	
}

void CustomScrollBar::OnDestroy() 
{
    CLSASS_BASE::OnDestroy();
    if (m_hCursor)
    {
        DestroyCursor(m_hCursor);
        m_hCursor = NULL;
    }
}

void CustomScrollBar::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    CScrollBarMemDC memDc(&dc, &m_rtBarClient);

	//先将nPos转成坐标，算出Thumb矩形
	int nScrollGap = m_si.nMax - m_si.nMin - m_si.nPage;
	if (nScrollGap < 1)
	{
		nScrollGap = 1;
	}
	m_rtThumb = m_rtThumbClient;
	if (m_bHorizontal)
	{
		m_rtThumb.InflateRect(-1,0,-1,0);
		int iPos = (m_si.nPos-m_si.nMin) * (m_rtBarClient.Width() - m_nArrowSize*2 - m_rtThumb.Width()) / nScrollGap;
		m_rtThumb.OffsetRect(iPos, 0);
	}
	else
	{
		m_rtThumb.InflateRect(0,-1,0,-1);
		int iPos = (m_si.nPos-m_si.nMin) * (m_rtBarClient.Height() - m_nArrowSize*2 - m_rtThumb.Height()) / nScrollGap;
		m_rtThumb.OffsetRect(0, iPos);
	}

	//绘制过程是先涂整体背景，然后画两个箭头，最后画thumb
	m_rtBarClient;
	m_rtThumb;
	m_rtHeadArrow;
	m_rtEndArrow;

	CDC	 bmpDC;
	bmpDC.CreateCompatibleDC(&dc);

    //整体背景画个黑色
	if (m_bIsBackgroundHot)
	{
		if (m_pImgBackgroundHot)
		{
			m_pImgBackgroundHot->StretchBlt(memDc.GetSafeHdc(), m_rtBarClient.left, m_rtBarClient.top, m_rtBarClient.Width(), m_rtBarClient.Height(), 0, 0, m_rtBackgroundHotbmp.Width(), m_rtBackgroundHotbmp.Height(), SRCCOPY);
		}
		else
		{
			memDc->FillRect(m_rtBarClient, &CBrush(RGB(175, 175, 175)));
		}
	}
	else
	{
		if (m_pImgBackground)
		{
			m_pImgBackground->StretchBlt(memDc.GetSafeHdc(), m_rtBarClient.left, m_rtBarClient.top, m_rtBarClient.Width(), m_rtBarClient.Height(), 0, 0, m_rtBackgroundbmp.Width(), m_rtBackgroundbmp.Height(), SRCCOPY);
		}
		else
		{
			memDc->FillRect(m_rtBarClient, &CBrush(RGB(50, 50, 50)));
		}
	}

    //画两个箭头
	if (m_bIsHeadArrowHot)
	{
		if (m_pImgHeadArrowHot)
		{
			m_pImgHeadArrowHot->StretchBlt(memDc.GetSafeHdc(), m_rtHeadArrow.left, m_rtHeadArrow.top, m_rtHeadArrow.Width(), m_rtHeadArrow.Height(), 0, 0, m_rtHeadArrowHotbmp.Width(), m_rtHeadArrowHotbmp.Height(), SRCCOPY);
		}
		else
		{
			memDc->FillRect(m_rtHeadArrow, &CBrush(RGB(150, 150, 150)));
		}
	}
	else
	{
		if (m_pImgHeadArrow)
		{
			m_pImgHeadArrow->StretchBlt(memDc.GetSafeHdc(), m_rtHeadArrow.left, m_rtHeadArrow.top, m_rtHeadArrow.Width(), m_rtHeadArrow.Height(), 0, 0, m_rtHeadArrowbmp.Width(), m_rtHeadArrowbmp.Height(), SRCCOPY);
		}
		else
		{
			memDc->FillRect(m_rtHeadArrow, &CBrush(RGB(100, 100, 100)));
		}
	}
	if (m_bIsEndArrowHot)
	{
		if (m_pImgEndArrowHot)
		{
			m_pImgEndArrowHot->StretchBlt(memDc.GetSafeHdc(), m_rtEndArrow.left, m_rtEndArrow.top, m_rtEndArrow.Width(), m_rtEndArrow.Height(), 0, 0, m_rtEndArrowHotbmp.Width(), m_rtEndArrowHotbmp.Height(), SRCCOPY);
		}
		else
		{
			memDc->FillRect(m_rtEndArrow, &CBrush(RGB(150, 150, 150)));
		}
	}
	else
	{
		if (m_pImgEndArrow)
		{
			m_pImgEndArrow->StretchBlt(memDc.GetSafeHdc(), m_rtEndArrow.left, m_rtEndArrow.top, m_rtEndArrow.Width(), m_rtEndArrow.Height(), 0, 0, m_rtEndArrowbmp.Width(), m_rtEndArrowbmp.Height(), SRCCOPY);
		}
		else
		{
			memDc->FillRect(m_rtEndArrow, &CBrush(RGB(100, 100, 100)));
		}
	}

	//画Thumb
	if (m_bIsThumbHot)
	{
		if (m_pImgThumbHot)
		{
			m_pImgThumbHot->StretchBlt(memDc.GetSafeHdc(), m_rtThumb.left, m_rtThumb.top, m_rtThumb.Width(), m_rtThumb.Height(), 0, 0, m_rtThumbHotbmp.Width(), m_rtThumbHotbmp.Height(), SRCCOPY);
		}
		else
		{
			memDc->FillRect(m_rtThumb, &CBrush(RGB(150, 150, 150)));
		}
	}
	else
	{
		if (m_pImgThumb)
		{
			m_pImgThumb->StretchBlt(memDc.GetSafeHdc(), m_rtThumb.left, m_rtThumb.top, m_rtThumb.Width(), m_rtThumb.Height(), 0, 0, m_rtThumbbmp.Width(), m_rtThumbbmp.Height(), SRCCOPY);
		}
		else
		{
			memDc->FillRect(m_rtThumb, &CBrush(RGB(100, 100, 100)));
		}
	}

	bmpDC.DeleteDC();
}

void CustomScrollBar::OnSize(UINT nType, int cx, int cy) 
{
    CLSASS_BASE::OnSize(nType, cx, cy);
	if (m_bAutoHide)
	{
		if (m_si.nMax-m_si.nMin <= (int)m_si.nPage)
		{
			ShowWindow(SW_HIDE);
		}
		else
		{
			ShowWindow(SW_SHOW);
		}
	}
    m_rtBarClient = CRect(0, 0, cx, cy);

    m_rtHeadArrow = m_rtEndArrow = m_rtThumbClient = m_rtBarClient;
    if (m_bHorizontal)
    {
        m_rtHeadArrow.right = m_rtHeadArrow.left + m_nArrowSize;
        m_rtEndArrow.left = m_rtEndArrow.right - m_nArrowSize;

        int iRange = m_si.nMax - m_si.nMin;
		if (iRange <= 0)
		{
			iRange = 1;
		}
        int iThumbSize = m_si.nPage * (m_rtBarClient.Width() - 2*m_nArrowSize) / iRange;
		if (iThumbSize > m_rtBarClient.Width() - 2*m_nArrowSize)
		{
			iThumbSize = m_rtBarClient.Width() - 2*m_nArrowSize;
		}
        if (m_rtBarClient.Width() > m_nArrowSize*2 + 15)
        {
            iThumbSize = max(iThumbSize, 10);
        }
        else
        {
            iThumbSize = 0;
        }
        m_rtThumbClient.left = m_rtHeadArrow.Width();
        m_rtThumbClient.right = m_rtThumbClient.left + iThumbSize;
    }
    else
    {
        m_rtHeadArrow.bottom = m_rtHeadArrow.top + m_nArrowSize;
        m_rtEndArrow.top = m_rtEndArrow.bottom - m_nArrowSize;

        int iRange = m_si.nMax - m_si.nMin;
		if (iRange <= 0)
		{
			iRange = 1;
		}
        int iThumbSize = m_si.nPage * (m_rtBarClient.Height() - 2*m_nArrowSize) / iRange;
		if (iThumbSize > m_rtBarClient.Height() - 2*m_nArrowSize)
		{
			iThumbSize = m_rtBarClient.Height() - 2*m_nArrowSize;
		}
        if (m_rtBarClient.Height() > m_nArrowSize*2 + 15)
        {
            iThumbSize = max(iThumbSize, 10);
        }
        else
        {
            iThumbSize = 0;
        }
        m_rtThumbClient.top = m_rtHeadArrow.Height();
        m_rtThumbClient.bottom = m_rtThumbClient.top +  iThumbSize;
    }

    InvalidateRect(NULL, FALSE);
}

void CustomScrollBar::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
    OnLButtonDown(nFlags, point);
}

void CustomScrollBar::OnRButtonDown(UINT nFlags, CPoint point) 
{
    if (m_rtEndArrow.PtInRect(point))
    {
        ScrollToEnd();
    }
    else if (m_rtHeadArrow.PtInRect(point))
    {
        ScrollToHome();
    }
    else
    {
        CRect rtPageUp, rtPageDn;
        GetPageRect(rtPageUp, rtPageDn);
        if (rtPageUp.PtInRect(point) || rtPageDn.PtInRect(point))
        {
            ScrollTo(point);
        }
    }
}

void CustomScrollBar::OnLButtonDown(UINT nFlags, CPoint point)
{
    SetCapture();
    if (m_rtThumb.PtInRect(point))
    {
        m_ptDragOffset = point - m_rtThumb.CenterPoint();
        m_bDragging = TRUE;
		m_bIsThumbHot = TRUE;
    }

    CRect rtPageUp, rtPageDn;
    GetPageRect(rtPageUp, rtPageDn);

    if (m_rtHeadArrow.PtInRect(point))
    {
        m_bIsHeadArrowHot = TRUE;
        ScrollLeft();
        SetTimer(TIMER_LEFTUP_LBUTTON_PRESSED, 100, NULL);
        m_iCounter = 0;
    }
    else if (m_rtEndArrow.PtInRect(point))
    {
        m_bIsEndArrowHot = TRUE;
        ScrollRight();
        SetTimer(TIMER_RIGHTDN_LBUTTON_PRESSED, 100, NULL);
        m_iCounter = 0;
    }
    else if (rtPageUp.PtInRect(point))
    {
		m_bIsBackgroundHot = TRUE;
        ScrollPageUp();
        SetTimer(TIMER_PAGEUP_LBUTTON_PRESSED, 100, NULL);
        m_iCounter = 0;
    }
    else if (rtPageDn.PtInRect(point))
    {
		m_bIsBackgroundHot = TRUE;
        ScrollPageDn();
        SetTimer(TIMER_PAGEDN_LBUTTON_PRESSED, 100, NULL);
        m_iCounter = 0;
    }

    InvalidateRect(NULL, FALSE);

    CLSASS_BASE::OnLButtonDown(nFlags, point);
}

void CustomScrollBar::OnLButtonUp(UINT nFlags, CPoint point)
{
    ReleaseCapture();
	m_pParent->PostMessage(m_bHorizontal ? WM_HSCROLL : WM_VSCROLL, MAKELONG(SB_ENDSCROLL,0), (LPARAM)this);
    BOOL bDrag = m_bDragging;
    ResetButton();
    if (bDrag)
    {
        point.Offset(-m_ptDragOffset.x, -m_ptDragOffset.y);
        ScrollTo(point);
    }
    InvalidateRect(NULL, FALSE);

    CLSASS_BASE::OnLButtonUp(nFlags,point);
}

void CustomScrollBar::OnMouseMove(UINT nFlags, CPoint point)
{
    // track mouse event
    TRACKMOUSEEVENT tme;   
    tme.cbSize=sizeof(TRACKMOUSEEVENT);
    tme.dwFlags=/*TME_HOVER |*/ TME_LEAVE;   
    tme.hwndTrack = m_hWnd;  
    tme.dwHoverTime = 1;
    _TrackMouseEvent(&tme); 
    
    // set cursor
    if (m_rtHeadArrow.PtInRect(point) || m_rtEndArrow.PtInRect(point))
    {
        if (m_bLastHand == FALSE)
        {
            SetCursor(m_hCursor);
            m_bLastHand = TRUE;
        }
    }
    else
    {
        if (m_bLastHand)
        {
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            m_bLastHand = FALSE;
        }
    }

    // drag thumb
    if (m_bDragging)
    {
        point.Offset(-m_ptDragOffset.x, -m_ptDragOffset.y);
        ScrollTo(point);
        InvalidateRect(NULL, FALSE);
    }
    else
    {
        if (m_rtThumb.PtInRect(point))
        {
            if (m_bHover == FALSE)
            {
                m_bIsThumbHot = TRUE;
                m_bHover = TRUE;
                InvalidateRect(m_rtThumb, FALSE);
            }
        }
        else
        {
            if (m_bHover == TRUE)
            {
                m_bIsThumbHot = FALSE;
                m_bHover = FALSE;
                InvalidateRect(m_rtThumb, FALSE);
            }
        }
    }
}

LRESULT CustomScrollBar::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
    return TRUE;
}

LRESULT CustomScrollBar::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    ResetButton();
    InvalidateRect(NULL, FALSE);
    return TRUE;
}

BOOL CustomScrollBar::GetScrollInfo(LPSCROLLINFO lpScrollInfo)
{
    *lpScrollInfo = m_si;
    return TRUE;
}

BOOL CustomScrollBar::SetScrollInfo(LPSCROLLINFO lpScrollInfo)
{
    m_si = *lpScrollInfo;
	if (::IsWindow(GetSafeHwnd()))
	{
		CRect rt;
		GetClientRect(rt);
		PostMessage(WM_SIZE, SIZE_RESTORED, MAKELPARAM(rt.Width(), rt.Height()));
	}
    return TRUE;
}

void CustomScrollBar::ScrollLeft()
{
    if (m_si.nPos > m_si.nMin)
    {
        m_si.nPos--;
        m_pParent->PostMessage(m_bHorizontal ? WM_HSCROLL : WM_VSCROLL, MAKELONG(SB_THUMBPOSITION,m_si.nPos), (LPARAM)this);
    }
}

void CustomScrollBar::ScrollRight()
{
    if (m_si.nPos < m_si.nMax)
    {
        m_si.nPos++;
        m_pParent->PostMessage(m_bHorizontal ? WM_HSCROLL : WM_VSCROLL, MAKELONG(SB_THUMBPOSITION,m_si.nPos), (LPARAM)this);
    }
}

void CustomScrollBar::ScrollPageUp()
{
    if (m_si.nPos > m_si.nMin)
    {
        m_si.nPos -= m_si.nPage - 1;
        m_si.nPos = max(m_si.nPos, m_si.nMin);
        m_pParent->PostMessage(m_bHorizontal ? WM_HSCROLL : WM_VSCROLL, MAKELONG(SB_THUMBPOSITION,m_si.nPos), (LPARAM)this);
    }
}

void CustomScrollBar::ScrollPageDn()
{
    if (m_si.nPos < m_si.nMax)
    {
        m_si.nPos += m_si.nPage - 1;
        m_si.nPos = min(m_si.nPos, m_si.nMax);
        m_pParent->PostMessage(m_bHorizontal ? WM_HSCROLL : WM_VSCROLL, MAKELONG(SB_THUMBPOSITION,m_si.nPos), (LPARAM)this);
    }
}

void CustomScrollBar::ScrollToEnd()
{
    m_si.nPos = m_si.nMax;
    m_pParent->PostMessage(m_bHorizontal ? WM_HSCROLL : WM_VSCROLL, MAKELONG(SB_THUMBPOSITION,m_si.nPos), (LPARAM)this);
    InvalidateRect(NULL, FALSE);
}

void CustomScrollBar::ScrollToHome()
{
    m_si.nPos = m_si.nMin;
    m_pParent->PostMessage(m_bHorizontal ? WM_HSCROLL : WM_VSCROLL, MAKELONG(SB_THUMBPOSITION,m_si.nPos), (LPARAM)this);
    InvalidateRect(NULL, FALSE);
}

void CustomScrollBar::ScrollTo(CPoint point)
{
	if ((m_bHorizontal && 0 == m_rtBarClient.Width()-m_nArrowSize*2-m_rtThumb.Width())
		|| (!m_bHorizontal && 0 == m_rtBarClient.Height()-m_nArrowSize*2-m_rtThumb.Height()))
	{
		return;
	}
    point.Offset(-m_nArrowSize-m_rtThumb.Width()/2, -m_nArrowSize-m_rtThumb.Height()/2);

    if (m_bHorizontal)
    {
        m_si.nPos = point.x * (m_si.nMax - m_si.nMin - (int)m_si.nPage) / (m_rtBarClient.Width()-m_nArrowSize*2-m_rtThumb.Width()) + m_si.nMin;
    }
    else
    {
        m_si.nPos = point.y * (m_si.nMax-m_si.nMin - (int)m_si.nPage) / (m_rtBarClient.Height()-m_nArrowSize*2-m_rtThumb.Height()) + m_si.nMin;
    }
    m_si.nPos = max(m_si.nMin, m_si.nPos);
    m_si.nPos = min(m_si.nMax - (int)m_si.nPage, m_si.nPos);
    if (m_bDragging)
    {
        m_pParent->PostMessage(m_bHorizontal ? WM_HSCROLL : WM_VSCROLL, MAKELONG(SB_THUMBPOSITION,m_si.nPos), (LPARAM)this);
    }
    else
    {
        m_pParent->PostMessage(m_bHorizontal ? WM_HSCROLL : WM_VSCROLL, MAKELONG(SB_THUMBPOSITION,m_si.nPos), (LPARAM)this);
    }

    InvalidateRect(NULL, FALSE);
	UpdateWindow();
}

void CustomScrollBar::OnTimer(UINT_PTR nIDEvent)
{
    switch (nIDEvent)
    {
    case TIMER_LEFTUP_LBUTTON_PRESSED:
        m_iCounter++;
        if (m_iCounter > 5)
        {
            ScrollLeft();
        }
        break;
    case TIMER_RIGHTDN_LBUTTON_PRESSED:
        m_iCounter++;
        if (m_iCounter > 5)
        {
            ScrollRight();
        }
        break;
    case TIMER_PAGEUP_LBUTTON_PRESSED:
        m_iCounter++;
        if (m_iCounter > 5)
        {
            ScrollPageUp();
        }
        break;
    case TIMER_PAGEDN_LBUTTON_PRESSED:
        m_iCounter++;
        if (m_iCounter > 5)
        {
            ScrollPageDn();
        }
        break;
    }
    InvalidateRect(NULL, FALSE);
}

void CustomScrollBar::GetPageRect( CRect &rtPageUp, CRect &rtPageDn )
{
    if (m_bHorizontal)
    {
        rtPageUp.left       = m_rtHeadArrow.right;
        rtPageUp.top        = m_rtThumb.top;
        rtPageUp.right      = m_rtThumb.left;
        rtPageUp.bottom     = m_rtThumb.bottom;
        
        rtPageDn.left       = m_rtThumb.right;
        rtPageDn.top        = m_rtThumb.top;
        rtPageDn.right      = m_rtEndArrow.left;
        rtPageDn.bottom     = m_rtThumb.bottom;
    }
    else
    {
        rtPageUp.left       = m_rtThumb.left;
        rtPageUp.top        = m_rtHeadArrow.bottom;
        rtPageUp.right      = m_rtThumb.right;
        rtPageUp.bottom     = m_rtThumb.top;
        
        rtPageDn.left       = m_rtThumb.left;
        rtPageDn.top        = m_rtThumb.bottom;
        rtPageDn.right      = m_rtThumb.right;
        rtPageDn.bottom     = m_rtEndArrow.top;
    }
}

void CustomScrollBar::LoadScrollBmp(BOOL bHor)
{
	auto LoadImg = [](CImage *& pImg, CRect & rtImg, int nResID)
	{
		pImg = ThemeManager::Instance().GetImage(nResID);
		if (pImg)
		{
			rtImg.SetRect(0, 0, pImg->GetWidth(), pImg->GetHeight());
		}
	};

	//加载图片
	LoadImg(m_pImgThumb, m_rtThumbbmp, ThemeImage_ScrollThumb);
	LoadImg(m_pImgThumbHot, m_rtThumbHotbmp, ThemeImage_ScrollThumbHot);
	if (bHor)
	{
		LoadImg(m_pImgHeadArrow, m_rtHeadArrowbmp, ThemeImage_ScrollLeftArrow);
	}
	else
	{
		LoadImg(m_pImgHeadArrow, m_rtHeadArrowbmp, ThemeImage_ScrollTopArrow);
	}
	if (bHor)
	{
		LoadImg(m_pImgHeadArrowHot, m_rtHeadArrowHotbmp, ThemeImage_ScrollLeftArrowHot);
	}
	else
	{
		LoadImg(m_pImgHeadArrowHot, m_rtHeadArrowHotbmp, ThemeImage_ScrollTopArrowHot);
	}
	if (bHor)
	{
		LoadImg(m_pImgEndArrow, m_rtEndArrowbmp, ThemeImage_ScrollRightArrow);
	}
	else
	{
		LoadImg(m_pImgEndArrow, m_rtEndArrowbmp, ThemeImage_ScrollBottomArrow);
	}
	if (bHor)
	{
		LoadImg(m_pImgEndArrowHot, m_rtEndArrowHotbmp, ThemeImage_ScrollRightArrowHot);
	}
	else
	{
		LoadImg(m_pImgEndArrowHot, m_rtEndArrowHotbmp, ThemeImage_ScrollBottomArrowHot);
	}
	LoadImg(m_pImgBackground, m_rtBackgroundbmp, ThemeImage_ScrollBk);
	LoadImg(m_pImgBackgroundHot, m_rtBackgroundHotbmp, ThemeImage_ScrollBkHot);
}

void CustomScrollBar::ResetButton()
{
    m_bIsHeadArrowHot = m_bIsEndArrowHot = m_bIsThumbHot = m_bIsBackgroundHot = FALSE;
    m_bDragging = FALSE;
    m_bLastHand = FALSE;
    m_bHover = FALSE;

    KillTimer(TIMER_LEFTUP_LBUTTON_PRESSED);
    KillTimer(TIMER_RIGHTDN_LBUTTON_PRESSED);
    KillTimer(TIMER_PAGEUP_LBUTTON_PRESSED);
    KillTimer(TIMER_PAGEDN_LBUTTON_PRESSED);
}

void CustomScrollBar::SetScrollPos(int iPos)
{
    iPos = min(m_si.nMax, iPos);
    iPos = max(m_si.nMin, iPos);
    m_si.nPos = iPos;
    InvalidateRect(NULL, FALSE);
}

BOOL CustomScrollBar::GetScrollRange(int * pMinPos, int * pMaxPos)
{
	ASSERT(pMinPos);
	ASSERT(pMaxPos);
	*pMinPos = m_si.nMin;
	*pMaxPos = m_si.nMax;
	return TRUE;
}
