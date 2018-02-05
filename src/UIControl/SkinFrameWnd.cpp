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
	CImage * pImage = NULL;

	CString strPath = SYPath_GetPath(SY_PATH_BIN);
	strPath += _T("\\CGSkin\\OtherRes\\");
	{
		//CString strFile = strPath + _T("dlg_minimize_button.png");
		CString strFile = _T("UI_UIIMAGE_THEME_DLG_MIN_");
		m_vecStrBtnImages.push_back(strFile);
	}

	{
		//CString strFile = strPath + _T("dlg_maxmize_button.png");
		CString strFile = _T("UI_UIIMAGE_THEME_DLG_MAX_");
		m_vecStrBtnImages.push_back(strFile);
	}

	{
		//CString strFile = strPath + _T("dlg_close_btn.png");
		CString strFile = _T("UI_UIIMAGE_THEME_DLG_CLOSE_");
		m_vecStrBtnImages.push_back(strFile);
	}

	{
		//CString strFile = strPath + _T("DialogBkg.png");
		SYIamgeResHandle BKRes = SYGetIamgeRes(_T("UI_UIIMAGE_THEME_DLG_BK"),FALSE);
		m_pImgBk = new CImage;
		m_pImgBk->Attach(BKRes.hImg);
	}
}

CSkinFrameWnd::CSkinFrameWnd()
	: m_pImgBk(NULL)
	, m_bChildMode(FALSE)
{
	m_nTitleHeight = 38;
	m_nFrameWidth = 3;

	LoadBmpRes();
	m_iXButtonHovering = -1;

	m_bNCHovering = FALSE;
	m_bNCTracking = FALSE;

	m_rcIcon = CRect(15,13,15+GetSystemMetrics(SM_CXSMICON),13+GetSystemMetrics(SM_CYSMICON));

	SYSubstituteMenu(AfxGetInstanceHandle());
}

CSkinFrameWnd::~CSkinFrameWnd()
{
	if (m_pImgBk != NULL)
	{
		delete m_pImgBk;
		m_pImgBk = NULL;
	}
	for(std::map<int,CDUIButton * >::iterator it = m_TitleBtn.begin(); it != m_TitleBtn.end(); it++)
	{
		if (it->second != NULL)
		{
			delete it->second;
			it->second = NULL;
		}
	}
	m_TitleBtn.clear();

	SYUnSubstituteMenu();
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

	if (m_pImgBk==NULL)
	{
		return;
	}

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

	Gdiplus::Graphics graphics(MemDC.GetSafeHdc());
	RectF destRect;

	//绘制底部
	destRect.X = 0;
	destRect.Y = rcWindow.Height() - m_nFrameWidth;
	destRect.Width = rcWindow.Width();
	destRect.Height = m_nFrameWidth;
	if (destRect.Width <= 0 || destRect.Height <= 0)
	{
		return;
	}
	m_pImgBk->Draw(MemDC.GetSafeHdc(), CRect(destRect.GetLeft(), destRect.GetTop(), destRect.GetRight(), destRect.GetBottom()));

	//左边框
	destRect.X = 0;
	destRect.Y = 0;//m_nTitleHeight;
	destRect.Width = m_nFrameWidth;
	destRect.Height = rcWindow.Height();
	if (destRect.Width <= 0 || destRect.Height <= 0)
	{
		return;
	}
	m_pImgBk->Draw(MemDC.GetSafeHdc(), CRect(destRect.GetLeft(), destRect.GetTop(), destRect.GetRight(), destRect.GetBottom()));


	//右边框
	destRect.X = rcWindow.Width() - m_nFrameWidth;
	destRect.Y = 0;//m_nTitleHeight;
	destRect.Width = m_nFrameWidth;
	destRect.Height = rcWindow.Height();
	if (destRect.Width <= 0 || destRect.Height <= 0)
	{
		return;
	}
	m_pImgBk->Draw(MemDC.GetSafeHdc(), CRect(destRect.GetLeft(), destRect.GetTop(), destRect.GetRight(), destRect.GetBottom()));

	//绘制标题栏
	destRect.X = 0;
	destRect.Y = 0;
	destRect.Width = rcWindow.Width() + m_nFrameWidth;
	destRect.Height = m_nTitleHeight;
	if (destRect.Width <= 0 || destRect.Height <= 0)
	{
		return;
	}
	m_pImgBk->Draw(MemDC.GetSafeHdc(), CRect(destRect.GetLeft() - m_nFrameWidth, destRect.GetTop(), destRect.GetRight() + m_nFrameWidth, destRect.GetBottom()));
	m_rtTitleBar.SetRect(destRect.GetLeft(), destRect.GetTop(), destRect.GetRight(), destRect.GetBottom());

	//子窗口模式，不绘制ICON、四周的蓝框、系统按钮及菜单
	if (!m_bChildMode)
	{
		SYIamgeResHandle imageResHandle;
		imageResHandle = SYGetIamgeRes(m_strIcon,FALSE);
		int nWidth = imageResHandle.rtSource.right - imageResHandle.rtSource.left;
		int nHeight = imageResHandle.rtSource.bottom - imageResHandle.rtSource.top;
		int xPos = 15;
		int yPos = (m_nTitleHeight + m_nFrameWidth - nHeight) / 2;
		CRect rtDes(xPos, yPos, xPos + nWidth, yPos + nHeight);
		DrawImageWithAplha(&MemDC, imageResHandle.hImg, rtDes, rtDes,
			imageResHandle.rtSource, imageResHandle.rtCorner, TRUE, 255, FALSE);

		//沿着窗口四周画一圈一个像素宽的蓝色矩形
		int nSavedDC = MemDC.SaveDC();
		COLORREF col = RGB(142, 170, 195);
		CPen pen(PS_SOLID, 1, col);
		MemDC.SelectObject(&pen);
		(CBrush*)MemDC.SelectStockObject(NULL_BRUSH);//空心的笔
		MemDC.RoundRect(&rcWindow, CPoint(6, 6));
		MemDC.RoundRect(&rcWindow, CPoint(8, 8));
		pen.DeleteObject();
		MemDC.RestoreDC(nSavedDC);

		CRect rcPaint;
		dc.GetClipBox(&rcPaint);
		std::map<int,CDUIButton *>::iterator iter;
		for (iter = m_TitleBtn.begin(); iter != m_TitleBtn.end(); iter++)
		{
			CDUIButton * dcControl = iter->second;
			CRect rcControl;
			dcControl->GetRect(&rcControl);

			// 判断当前按钮是否需要重绘
			if(!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
			{
				continue;
			}

			dcControl->DrawButton(MemDC.GetSafeHdc());
		}

		//绘制菜单
		//菜单前边加一个竖线
		//MemDC.FillSolidRect(CSYNewMenu::m_rtMenuBar.left - 3, CSYNewMenu::m_rtMenuBar.top, 1, m_rtTitleBar.Height(), RGB(30, 30, 30));
		CMenu* pMenu = GetMenu();
		if(pMenu && IsMenu(pMenu->m_hMenu))
		{
			MemDC.FillRect(CSYNewMenu::m_rtMenuBar, &CBrush(RGB(50, 50, 50)));
			for(int i = 0;i<(int)pMenu->GetMenuItemCount();i++)
			{
				CSYNewMenu::DrawMenuBarItem(this, pMenu, i, &MemDC, -1, CSYNewMenu::m_rtMenuBar);
			}
		}

		//绘制标题
		LOGFONT lfFont;
		memset(&lfFont, 0, sizeof(lfFont));
		lfFont.lfHeight = -12;
		//lfFont.lfWeight |= FW_BOLD;
		lstrcpy(lfFont.lfFaceName, _T("微软雅黑"));
		CFont font;
		font.CreateFontIndirect(&lfFont);

		MemDC.SelectObject(&font);

		// 绘制标题,左边
		MemDC.SetTextColor(RGB(200,200,200));
		CRect rtTitle(destRect.X, destRect.Y+6, destRect.Width, destRect.Height);
		if (pMenu)
		{
			rtTitle.left = CSYNewMenu::m_rtMenuBar.right + 100;
			rtTitle.right -= CSYNewMenu::m_rtMenuBar.right;
		}
		else
		{
			rtTitle.left = rtDes.right + 100;
			rtTitle.right -= rtDes.right;
		}
		MemDC.DrawText(m_strTitle, rtTitle, DT_SINGLELINE | DT_VCENTER | DT_LEFT);
	}

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

	if (m_bChildMode)
	{
		m_nTitleHeight = 1;
	}
	else
	{
		m_nTitleHeight = 38;
	}

	if (bCalcValidRects)   
	{   

		CRect& rc = (CRect&)lpncsp->rgrc[0];//get the client rectangle   
		rc.top += m_nTitleHeight;   
		rc.left += m_nFrameWidth;   
		rc.bottom -= m_nFrameWidth;   
		rc.right -= m_nFrameWidth;   

		CRect rtMenuBar(146, 1, 301, 1);
		if (dwStyle & WS_MAXIMIZE)
		{
			rc.left += m_nFrameWidth;
			rc.right -= m_nFrameWidth;
			rc.bottom -= m_nFrameWidth;
			rtMenuBar.OffsetRect(m_nFrameWidth, 0);
		}
		CSYNewMenu::m_rtMenuBar = rtMenuBar;
	}

	BOOL	bToolWindow = (dwExStyle & WS_EX_TOOLWINDOW);
	BOOL	bHaveCaption = (dwStyle & WS_CAPTION) == WS_CAPTION;

	CMenu* pMenu = GetMenu();
	if (bHaveCaption && !(dwStyle & WS_CHILD) && !bToolWindow && pMenu != NULL)
	{
		if (dwStyle & WS_MAXIMIZE)
		{
			CSYNewMenu::m_rtMenuBar.top = (m_nTitleHeight - CSYNewMenu::m_nItemHeight) / 2 + m_nFrameWidth;
		}
		else
		{
			CSYNewMenu::m_rtMenuBar.top = (m_nTitleHeight - CSYNewMenu::m_nItemHeight) / 2;
		}

		int nLastRight = CSYNewMenu::m_rtMenuBar.left;
		//计算并保存menuitem的大小
		for (int i = 0; i < m_MainMenu.GetMenuItemCount(); i++)
		{
			SIZE szItem = m_MainMenu.CalcMenuItemSize(i);
			CRect rtItem(0, 0, szItem.cx, szItem.cy);
			rtItem.OffsetRect(nLastRight, CSYNewMenu::m_rtMenuBar.top);
			MENUITEMINFO iteminfo;
			ZeroMemory(&iteminfo, sizeof(MENUITEMINFO));
			iteminfo.cbSize	= sizeof(MENUITEMINFO); 
			iteminfo.fMask	= MIIM_ID; 
			VERIFY(m_MainMenu.GetMenuItemInfo(i, &iteminfo, TRUE));
			g_mapMenuBarItemRect[iteminfo.wID] = rtItem;
			nLastRight = rtItem.right;

			rtItem.OffsetRect(rtWindow.left, rtWindow.top);
			((CSYNewMenu *)m_MainMenu.GetSubMenu(i))->SetItemRect(rtItem);
		}
		CSYNewMenu::m_rtMenuBar.right = nLastRight;
		CSYNewMenu::m_rtMenuBar.bottom = CSYNewMenu::m_rtMenuBar.top + CSYNewMenu::m_nItemHeight;
	}
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

	CMenu* pMenu = GetMenu();
	if(pMenu && IsMenu(pMenu->m_hMenu))
	{
		for (int i = 0; i < pMenu->GetMenuItemCount(); i++)
		{
			MENUITEMINFO iteminfo;
			ZeroMemory(&iteminfo, sizeof(MENUITEMINFO));
			iteminfo.cbSize	= sizeof(MENUITEMINFO); 
			iteminfo.fMask	= MIIM_ID;
			VERIFY(m_MainMenu.GetMenuItemInfo(i, &iteminfo, TRUE));
			if (g_mapMenuBarItemRect.count(iteminfo.wID) > 0 && g_mapMenuBarItemRect[iteminfo.wID].PtInRect(pt))
			{
				::HiliteMenuItem(m_hWnd, pMenu->m_hMenu, i, MF_BYPOSITION|MF_HILITE);
			}
			else
			{
				::HiliteMenuItem(m_hWnd, pMenu->m_hMenu, i, MF_BYPOSITION|MF_UNHILITE);
			}
		}
	}
	if(CSYNewMenu::m_rtMenuBar.PtInRect(pt))
	{
		return HTMENU;
	}

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
	int iButton = TButtonHitTest(pt, rtButton);
	if(iButton != -1)
	{
		m_TitleBtn[iButton]->LButtonDown();
		OnNcPaint();
		return;
	}

	if (CSYNewMenu::m_rtMenuBar.PtInRect(pt))
	{
		OnNcPaint();
		return;
	}

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
		int iButton = TButtonHitTest(pt, rtButton);
		if(iButton != -1)
		{
			m_TitleBtn[iButton]->LButtonDown();
			OnNcPaint();
			return;
		}

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
				m_TitleBtn[iButton]->LButtonUp();
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
	int iButton = TButtonHitTest(point, rtButton);
	if(iButton != m_iXButtonHovering)
	{
		if(m_iXButtonHovering != -1)
		{
			m_TitleBtn[m_iXButtonHovering]->MouseLeave();
			m_iXButtonHovering = -1;
		}
		if(iButton != -1)
		{
			m_iXButtonHovering = iButton;
			m_TitleBtn[m_iXButtonHovering]->MouseHover();
		}

		OnNcPaint();
	}	
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
		m_TitleBtn[m_iXButtonHovering]->MouseLeave();
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
	m_TitleBtn[ID_CLOSE_BTN]->GetRect(rtClose);
	nY = (m_rtTitleBar.Height() - rtClose.Height()) / 2;
	nYEnd = nY + rtClose.Height();
	int nLeft = 10;
	if (m_TitleBtn.count(ID_MIN_BTN) > 0)
	{
		CRect rMin(rcWnd.right - rtClose.Width() * 3 - nLeft, nY, rcWnd.right - rtClose.Width() * 2 - nLeft, nYEnd);
		m_TitleBtn[ID_MIN_BTN]->SetRect(rMin);
	}
	if (m_TitleBtn.count(ID_MAX_BTN) > 0)
	{
		CRect rMax(rcWnd.right - rtClose.Width() * 2 - nLeft, nY, rcWnd.right - rtClose.Width() - nLeft, nYEnd);
		m_TitleBtn[ID_MAX_BTN]->SetRect(rMax);
	}
	CRect rClose(rcWnd.right - rtClose.Width() - nLeft, nY, rcWnd.right - nLeft, nYEnd);
	m_TitleBtn[ID_CLOSE_BTN]->SetRect(rClose);

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
	AddTitleButton(rcControl,m_vecStrBtnImages[2],3,ID_CLOSE_BTN);
	AddTitleButton(rcControl,m_vecStrBtnImages[1],3,ID_MAX_BTN);
	AddTitleButton(rcControl,m_vecStrBtnImages[0],3,ID_MIN_BTN);

	HMENU hMainMenu = ::GetMenu(m_hWnd);
	if(hMainMenu && ::IsMenu(hMainMenu))
	{
		m_MainMenu.LoadMenu(hMainMenu,FALSE);
		SYInstallFrameHook(AfxGetInstanceHandle());
		SYHookFrameWnd(m_hWnd);
	}


	return TRUE;
}

void CSkinFrameWnd::OnDestroy()
{
	HMENU hMainMenu = ::GetMenu(m_hWnd);
	if(hMainMenu && ::IsMenu(hMainMenu))
	{
		SYUnHookFrameWnd(m_hWnd);
		SYUninstallFrameHook();
		m_MainMenu.UnLoadMenu();
	}
	CFrameWnd::OnDestroy();
}

void CSkinFrameWnd::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if(lpDrawItemStruct->CtlType == ODT_MENU)
	{
		if (m_MainMenu.IsMenu())
		{
			CSYNewMenu* pNewMenu = DYNAMIC_DOWNCAST(CSYNewMenu, CMenu::FromHandlePermanent((HMENU)lpDrawItemStruct->hwndItem));
			if (pNewMenu->IsMenuBar())
			{
				if (g_mapMenuBarItemRect.count(lpDrawItemStruct->itemID))
				{
					lpDrawItemStruct->rcItem = g_mapMenuBarItemRect[lpDrawItemStruct->itemID];
				}
			}
		}
	}
	CFrameWnd::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CSkinFrameWnd::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	CSYNewMenu::OnInitMenuPopup(m_hWnd,pPopupMenu,nIndex,bSysMenu);
}

void CSkinFrameWnd::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if(lpMeasureItemStruct->CtlType == ODT_MENU)
	{
		if(CSYNewMenu::OnMeasureItem(nIDCtl, lpMeasureItemStruct, m_hWnd))
			return;
	}	
	CFrameWnd::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CSkinFrameWnd::OnMove(int x, int y)
{
	CFrameWnd::OnMove(x, y);

	CRect	rtWindow;
	GetWindowRect(rtWindow);
	DWORD	dwStyle	= GetStyle();
	DWORD	dwExStyle = GetExStyle();
	BOOL	bToolWindow = (dwExStyle & WS_EX_TOOLWINDOW);
	BOOL	bHaveCaption = (dwStyle & WS_CAPTION) == WS_CAPTION;
	CMenu* pMenu = GetMenu();
	if (bHaveCaption && !(dwStyle & WS_CHILD) && !bToolWindow && pMenu != NULL)
	{
		if (dwStyle & WS_MAXIMIZE)
		{
			CSYNewMenu::m_rtMenuBar.top = (m_nTitleHeight - CSYNewMenu::m_nItemHeight) / 2 + m_nFrameWidth;
		}
		else
		{
			CSYNewMenu::m_rtMenuBar.top = (m_nTitleHeight - CSYNewMenu::m_nItemHeight) / 2;
		}

		int nLastRight = CSYNewMenu::m_rtMenuBar.left;
		//计算并保存menuitem的大小
		for (int i = 0; i < m_MainMenu.GetMenuItemCount(); i++)
		{
			SIZE szItem = m_MainMenu.CalcMenuItemSize(i);
			CRect rtItem(0, 0, szItem.cx, szItem.cy);
			rtItem.OffsetRect(nLastRight, CSYNewMenu::m_rtMenuBar.top);
			MENUITEMINFO iteminfo;
			ZeroMemory(&iteminfo, sizeof(MENUITEMINFO));
			iteminfo.cbSize	= sizeof(MENUITEMINFO); 
			iteminfo.fMask	= MIIM_ID; 
			VERIFY(m_MainMenu.GetMenuItemInfo(i, &iteminfo, TRUE));
			g_mapMenuBarItemRect[iteminfo.wID] = rtItem;
			nLastRight = rtItem.right;

			rtItem.OffsetRect(rtWindow.left, rtWindow.top);
			((CSYNewMenu *)m_MainMenu.GetSubMenu(i))->SetItemRect(rtItem);
		}
		CSYNewMenu::m_rtMenuBar.right = nLastRight;
		CSYNewMenu::m_rtMenuBar.bottom = CSYNewMenu::m_rtMenuBar.top + CSYNewMenu::m_nItemHeight;
	}
}

void CSkinFrameWnd::SetChildMode(BOOL bChildMode)
{
	m_bChildMode = bChildMode;
}

//往对话框添加一个按钮
CDUIButton* CSkinFrameWnd::AddTitleButton(LPRECT lpRect, CString strImage, UINT nCount /*= 3*/ , UINT nID/* = 0*/,LPCTSTR lpszCaption/*=NULL*/)
{
	CDUIButton * dcControl = new CDUIButton;
	dcControl->CreateButton(lpszCaption, lpRect, this,strImage,nCount, nID);
	m_TitleBtn[nID] = dcControl;

	return m_TitleBtn[nID];
}

int CSkinFrameWnd::TButtonHitTest(CPoint point, CRect& rtButton)
{
	std::map<int,CDUIButton *>::iterator iter;
	for (iter = m_TitleBtn.begin(); iter != m_TitleBtn.end(); iter++)
	{
		CDUIButton * dcControl = iter->second;
		if(dcControl->PtInButton(point))
		{
			dcControl->GetRect(&rtButton);
			return dcControl->GetControlId();
		}
	}
	return -1;
}
