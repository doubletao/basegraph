// SkinFrameWnd.cpp : implementation file
//

#include "stdafx.h"
#include "SkinFrameWnd.h"

#define  ID_CLOSE_BTN 110
#define  ID_MAX_BTN   111
#define  ID_MIN_BTN   112

extern std::map<int, CRect> g_mapMenuBarItemRect;

// CSkinFrameWnd

IMPLEMENT_DYNCREATE(CSkinFrameWnd,CFrameWnd)

void CSkinFrameWnd::LoadBmpRes()
{
}

CSkinFrameWnd::CSkinFrameWnd()
{
	m_nTitleHeight = 38;
	m_nFrameWidth = 3;

	LoadBmpRes();
	m_iXButtonHovering = -1;

	m_bNCHovering = FALSE;
	m_bNCTracking = FALSE;

	m_rcIcon = CRect(15,13,15+GetSystemMetrics(SM_CXSMICON),13+GetSystemMetrics(SM_CYSMICON));
}

CSkinFrameWnd::~CSkinFrameWnd()
{
}


BEGIN_MESSAGE_MAP(CSkinFrameWnd, CFrameWnd)
	ON_WM_NCPAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_NCHITTEST()
	ON_WM_GETMINMAXINFO()
	ON_WM_NCACTIVATE()
	ON_WM_ACTIVATE()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCRBUTTONDOWN()
	ON_WM_NCLBUTTONUP()
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCMOUSEHOVER()
	ON_WM_NCMOUSELEAVE()
	ON_WM_SIZE()
	ON_WM_SYSCOMMAND()
	ON_WM_NCCREATE()
	ON_WM_DESTROY()
	ON_WM_DRAWITEM()
	ON_WM_INITMENUPOPUP()
	ON_WM_MEASUREITEM()
	ON_WM_MOVE()
END_MESSAGE_MAP()


// CSkinFrameWnd message handlers

void CSkinFrameWnd::OnNcPaint()
{
	CWindowDC dc(this);

	CDC MemDC;

	// 获取位置
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	rcWindow.OffsetRect( -rcWindow.left, -rcWindow.top);

	DWORD	dwStyle	= GetStyle();
	DWORD	dwExStyle = GetExStyle();

	CRect rcClient;
	GetClientRect(&rcClient);

	//剪除掉客户区
	rcClient.OffsetRect(m_nFrameWidth, m_nTitleHeight);
	dc.ExcludeClipRect(rcClient.left, rcClient.top,
		rcClient.right, rcClient.bottom);

	MemDC.CreateCompatibleDC(&dc);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc,rcWindow.Width(),rcWindow.Height());
	MemDC.SelectObject(&bmp);
	MemDC.SetBkMode(TRANSPARENT);

	dc.BitBlt(0,0,rcWindow.Width(),rcWindow.Height(),&MemDC,0,0,SRCCOPY);

	dc.SelectClipRgn(NULL);

	MemDC.DeleteDC();
	bmp.DeleteObject();
}

void CSkinFrameWnd::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	CRect	rtWindow(lpncsp->rgrc[0]);
	CPoint	ptOffset(rtWindow.TopLeft());
	DWORD	dwStyle	= GetStyle();
	DWORD	dwExStyle = GetExStyle();

	m_nTitleHeight = 38;

	if (bCalcValidRects)   
	{   

		CRect& rc = (CRect&)lpncsp->rgrc[0];//get the client rectangle   
		rc.top += m_nTitleHeight;   
		rc.left += m_nFrameWidth;   
		rc.bottom -= m_nFrameWidth;   
		rc.right -= m_nFrameWidth;   
	}

	BOOL	bToolWindow = (dwExStyle & WS_EX_TOOLWINDOW);
	BOOL	bHaveCaption = (dwStyle & WS_CAPTION) == WS_CAPTION;
}

LRESULT CSkinFrameWnd::OnNcHitTest(CPoint point)
{
	LRESULT nRet = HTCAPTION;

	//如果是嵌入模式，拒绝响应
	if (m_bChildMode)
	{
		return HTNOWHERE;
	}

	CRect rtButton;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CPoint pt = point;
	pt.x -=rcWindow.left;
	pt.y -= rcWindow.top;

	//如果是嵌入模式，只响应菜单，从这里直接返回
	if(m_bChildMode)
	{
		if (m_rcIcon.PtInRect(pt))
		{
			return HTNOWHERE;
		}
		else
		{
			nRet = CFrameWnd::OnNcHitTest(point);
			if (nRet == HTCLOSE
				|| nRet == HTMAXBUTTON
				|| nRet == HTMINBUTTON)
			{
				nRet = HTNOWHERE;
			}
		}

		return nRet;
	}

	int iButton = TButtonHitTest(pt, rtButton);
	if(iButton != -1)
	{
		switch(iButton)
		{
		case ID_CLOSE_BTN:
			{
				return HTCLOSE;
			}
			break;
		case ID_MAX_BTN:
			{
				return HTMAXBUTTON;
			}
			break;
		case ID_MIN_BTN:
			{
				return HTMINBUTTON;
			}
			break;
		}
	}
	else if (m_rcIcon.PtInRect(pt))
	{
		return HTSYSMENU;
	}
	else
	{
		nRet = CFrameWnd::OnNcHitTest(point);
		if (nRet == HTCLOSE
			|| nRet == HTMAXBUTTON
			|| nRet == HTMINBUTTON)
		{
			nRet = HTCAPTION;
		}
	}

	return nRet;
}

void CSkinFrameWnd::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.y = m_nTitleHeight + m_nFrameWidth;
	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}

BOOL CSkinFrameWnd::OnNcActivate(BOOL bActive)
{
	OnNcPaint();
	return TRUE;
}

void CSkinFrameWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CFrameWnd::OnActivate(nState, pWndOther, bMinimized);

	OnNcPaint();
}

void CSkinFrameWnd::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	rcWindow.bottom = rcWindow.top + m_nTitleHeight;


	CPoint pt = point;
	CRect rtButton;

	if (!m_bChildMode)
	{
		if (rcWindow.PtInRect(pt))
		{
			if (IsZoomed())
			{
				ShowWindow(SW_RESTORE);
			}
			else
			{
				ShowWindow(SW_MAXIMIZE);
			}

			OnNcPaint();
			return;
		}
	}

	OnNcPaint();
}

void CSkinFrameWnd::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	CRect rtButton;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CPoint pt = point;
	pt.x -=rcWindow.left;
	pt.y -= rcWindow.top;

	//嵌入模式不响应单击
	if (!m_bChildMode)
	{

		if(m_rcIcon.PtInRect(pt)) 
		{
			CMenu *pSysMenu = GetSystemMenu(FALSE);
			if (pSysMenu)
			{
				pSysMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y,this, NULL);
			}
			return;
		}
		else if(nHitTest >= HTLEFT && nHitTest <= HTBOTTOMRIGHT || nHitTest == HTCAPTION)
		{		
			CFrameWnd::OnNcLButtonDown(nHitTest, point);
			return;
		}
		else if (nHitTest == HTMENU)
		{
			return;
		}
		else if (nHitTest == HTSYSMENU)
		{
			return;
		}

		CFrameWnd::OnNcLButtonDown(nHitTest, point);
	}
}

void CSkinFrameWnd::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
	CRect rtButton;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CPoint pt = point;
	pt.x -=rcWindow.left;
	pt.y -= rcWindow.top;

	int iButton = TButtonHitTest(pt, rtButton);
	if(iButton != -1)
	{
		switch(iButton)
		{
		case ID_CLOSE_BTN:
			{
				SendMessage(WM_SYSCOMMAND,SC_CLOSE,0);
			}
			break;
		case ID_MAX_BTN:
			{
				if (IsZoomed())
				{
					SendMessage(WM_SYSCOMMAND,SC_RESTORE,0);
				}
				else
				{
					SendMessage(WM_SYSCOMMAND,SC_MAXIMIZE,0);
				}
			}
			break;
		case ID_MIN_BTN:
			{
				SendMessage(WM_SYSCOMMAND,SC_MINIMIZE,0);
			}
			break;
		default:
			{
				OnNcPaint();
			}
			break;
		}
	}
	else
	{
		//CFrameWnd::OnNcLButtonUp(nHitTest, point);
	}
}

void CSkinFrameWnd::OnNcRButtonDown(UINT nHitTest, CPoint point)
{
	CRect rtButton;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CPoint pt = point;
	pt.x -=rcWindow.left;
	pt.y -= rcWindow.top;
	if (m_rtTitleBar.PtInRect(pt))
	{
		CFrameWnd::OnNcLButtonUp(nHitTest, point);
	}
}

void CSkinFrameWnd::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	if (nHitTest == HTMENU)
	{
		OnNcPaint();
	}

	if (!m_bNCTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_NONCLIENT | TME_HOVER | TME_LEAVE;
		tme.dwHoverTime = 1;
		m_bNCTracking = _TrackMouseEvent(&tme);
	}

	if(nHitTest>=HTLEFT && nHitTest <= HTBOTTOMRIGHT || 
		nHitTest == HTCAPTION)
	{
		CFrameWnd::OnNcMouseMove(nHitTest, point);
	}

	CRect rtButton;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	point.x -=rcWindow.left;
	point.y -= rcWindow.top;
}

void CSkinFrameWnd::OnNcMouseHover(UINT nFlags, CPoint point)
{
	m_bNCHovering = TRUE;
}

void CSkinFrameWnd::OnNcMouseLeave()
{
	m_bNCTracking = FALSE;
	m_bNCHovering = FALSE;

	if(m_iXButtonHovering != -1)
	{
		m_iXButtonHovering = -1;
	}

	OnNcPaint();
}

void CSkinFrameWnd::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	if (nType != SIZE_MINIMIZED && nType != SIZE_MAXHIDE )
	{
		if (m_Rgn.GetSafeHandle())
		{
			m_Rgn.DeleteObject();
		}

		CRect rc;
		GetWindowRect(&rc); //获得窗口矩形
		rc -= rc.TopLeft();
		m_Rgn.CreateRoundRectRgn(rc.left, rc.top,   rc.right+1, rc.bottom+1, 5, 5); //根据窗口矩形创建一个圆角矩形
		SetWindowRgn(m_Rgn, TRUE); //根据圆角矩形指定的区域改变窗口的形状
	}

	CRect rcWnd;
	GetWindowRect(&rcWnd);
	rcWnd.OffsetRect( -rcWnd.left, -rcWnd.top);

	//按钮位置要右对齐，垂直居中
	int nY = 0;
	int nYEnd = 0;
	CRect rtClose;
	nY = (m_rtTitleBar.Height() - rtClose.Height()) / 2;
	nYEnd = nY + rtClose.Height();
	int nLeft = 10;
	CRect rClose(rcWnd.right - rtClose.Width() - nLeft, nY, rcWnd.right - nLeft, nYEnd);

	if (nType == SIZE_MAXIMIZED||
		nType == SIZE_RESTORED)
	{
		OnNcPaint();
	}

}

void CSkinFrameWnd::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID != SC_CLOSE)
	{
		OnNcPaint();
	}

	CFrameWnd::OnSysCommand(nID, lParam);
}

LRESULT CSkinFrameWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == 0x125)
	{
		OnNcPaint();
	}
	if (message== WM_NCLBUTTONDOWN)
	{
		OnNcPaint();
	}
	if (message ==0x00AE||//WM_NCUAHDRAWCAPTION
		message==0x00AF)//WM_NCUAHDRAWFRAME)
	{
		OnNcPaint();
		return 0;
	}

	return CFrameWnd::WindowProc(message, wParam, lParam);
}

int CSkinFrameWnd::TButtonHitTest(CPoint point, CRect & rtButton)
{
	return 0;
}

BOOL CSkinFrameWnd::OnNcCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (!CFrameWnd::OnNcCreate(lpCreateStruct))
		return FALSE;

	CRect rcWnd;
	GetWindowRect(&rcWnd);
	rcWnd.OffsetRect( -rcWnd.left, -rcWnd.top);

	CRect rcControl;
	rcControl.left = rcWnd.right- 35;
	rcControl.top = 5;
	rcControl.right = rcControl.left + 17;
	rcControl.bottom = rcControl.top + 17;

	return TRUE;
}
