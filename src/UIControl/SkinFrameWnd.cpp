// SkinFrameWnd.cpp : implementation file
//

#include "stdafx.h"
#include "UIControl\SkinFrameWnd.h"
#include "ThemeManager.h"
#include <assert.h>

// CSkinFrameWnd

IMPLEMENT_DYNCREATE(CSkinFrameWnd, CFrameWnd)

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
END_MESSAGE_MAP()

CSkinFrameWnd::CSkinFrameWnd()
	: m_rtTitleBar(0)
	, m_bNCTracking(FALSE)
	, m_iXButtonHovering(-1)
	, m_bNCHovering(FALSE)
{
}

CSkinFrameWnd::~CSkinFrameWnd()
{
	for (size_t i = 0; i < m_vecButton.size(); i++)
	{
		UIButton *& pButton = m_vecButton[i];
		if (pButton)
		{
			delete pButton;
			pButton = nullptr;
		}
	}
}

void CSkinFrameWnd::OnNcPaint()
{
	CWindowDC dc(this);

	CImage * pImgBk = ThemeManager::Instance().GetImage(ThemeImage_SkinWndNcBkgrd);
	int nFrameWidth = ThemeManager::Instance().GetIntValue(ThemeInt_SkinWndFrameWidth);
	int nTitleHeight = ThemeManager::Instance().GetIntValue(ThemeInt_SkinWndTitleHeight);
	CImage * pImgIcon = ThemeManager::Instance().GetImage(ThemeImage_SkinFrameDefaultIcon);
	LOGFONT lfTitleFont = ThemeManager::Instance().GetFont(ThemeFont_SkinWndTitleText);
	COLORREF nFontColor = ThemeManager::Instance().GetColor(ThemeColor_SkinWndTitleText);

	if (nullptr != pImgBk
		&& nFrameWidth > 0
		&& nTitleHeight > 0
		&& nullptr != pImgIcon
		)
	{
		CDC MemDC;

		// 获取位置
		CRect rcWindow;
		GetWindowRect(&rcWindow);
		rcWindow.OffsetRect(-rcWindow.left, -rcWindow.top);

		CRect rcClient;
		GetClientRect(&rcClient);

		//剪除掉客户区
		rcClient.OffsetRect(nFrameWidth, nTitleHeight);
		dc.ExcludeClipRect(rcClient.left, rcClient.top,
			rcClient.right, rcClient.bottom);

		MemDC.CreateCompatibleDC(&dc);

		CBitmap bmp;
		bmp.CreateCompatibleBitmap(&dc, rcWindow.Width(), rcWindow.Height());
		MemDC.SelectObject(&bmp);
		MemDC.SetBkMode(TRANSPARENT);

		CRect destRect;

		//绘制底部
		destRect.left = 0;
		destRect.top = rcWindow.Height() - nFrameWidth;
		destRect.right = destRect.left + rcWindow.Width();
		destRect.bottom = destRect.top + nFrameWidth;
		if (destRect.Width() <= 0 || destRect.Height() <= 0)
		{
			return;
		}
		pImgBk->Draw(MemDC.GetSafeHdc(), destRect);

		//左边框
		destRect.left = 0;
		destRect.top = 0;
		destRect.right = destRect.left + nFrameWidth;
		destRect.bottom = destRect.top + rcWindow.Height();
		if (destRect.Width() <= 0 || destRect.Height() <= 0)
		{
			return;
		}
		pImgBk->Draw(MemDC.GetSafeHdc(), destRect);


		//右边框
		destRect.left = rcWindow.Width() - nFrameWidth;
		destRect.top = 0;
		destRect.right = destRect.left + nFrameWidth;
		destRect.bottom = destRect.top + rcWindow.Height();
		if (destRect.Width() <= 0 || destRect.Height() <= 0)
		{
			return;
		}
		pImgBk->Draw(MemDC.GetSafeHdc(), destRect);

		//绘制标题栏
		destRect.left = 0;
		destRect.top = 0;
		destRect.right = destRect.left + rcWindow.Width() + nFrameWidth;
		destRect.bottom = destRect.top + nTitleHeight;
		m_rtTitleBar = destRect;
		if (destRect.Width() <= 0 || destRect.Height() <= 0)
		{
			return;
		}
		pImgBk->Draw(MemDC.GetSafeHdc(), destRect);

		int nXOffset = 0;
		int nYOffset = 0;
		DWORD	dwStyle = GetStyle();
		if (dwStyle & WS_MAXIMIZE)
		{
			nXOffset = nFrameWidth;
			nYOffset = nFrameWidth * 2;
		}

		//绘制ICON
		int nIconWidth = pImgIcon->GetWidth();
		int nIconHeight = pImgIcon->GetHeight();
		int xIconPos = nFrameWidth + nXOffset;
		int yIconPos = (nTitleHeight + nFrameWidth - nIconHeight + nYOffset) / 2;
		m_rcIcon.SetRect(xIconPos, yIconPos, xIconPos + nIconWidth, yIconPos + nIconHeight);
		BufferFunction::DrawImageWithAplha(&MemDC, pImgIcon, m_rcIcon, m_rcIcon,
			CRect(0, 0, nIconWidth, nIconHeight), CRect(), TRUE, 255, FALSE);

		//绘制标题栏按钮
		CRect rcPaint;
		dc.GetClipBox(&rcPaint);
		for (size_t i = 0; i < m_vecButton.size(); i++)
		{
			UIButton *& pButton = m_vecButton[i];
			if (pButton)
			{
				CRect rcControl = pButton->GetRect();

				// 判断当前按钮是否需要重绘
				if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
				{
					continue;
				}

				pButton->DrawElement(&MemDC);
			}
		}

		//绘制标题
		CString strTitle;
		GetWindowText(strTitle);
		CFont font;
		font.CreateFontIndirect(&lfTitleFont);
		MemDC.SelectObject(&font);
		MemDC.SetTextColor(nFontColor);
		CRect rtTitle(xIconPos + nIconWidth + nFrameWidth + nXOffset, 0, rcWindow.Width(), nTitleHeight + nYOffset);
		MemDC.DrawText(strTitle, rtTitle, DT_SINGLELINE | DT_VCENTER | DT_LEFT);

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

		dc.BitBlt(0, 0, rcWindow.Width(), rcWindow.Height(), &MemDC, 0, 0, SRCCOPY);

		dc.SelectClipRgn(NULL);

		MemDC.DeleteDC();
		bmp.DeleteObject();
	}
}

void CSkinFrameWnd::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS * lpncsp)
{
	CRect	rtWindow(lpncsp->rgrc[0]);
	CPoint	ptOffset(rtWindow.TopLeft());
	DWORD	dwStyle = GetStyle();
	DWORD	dwExStyle = GetExStyle();
	int nFrameWidth = ThemeManager::Instance().GetIntValue(ThemeInt_SkinWndFrameWidth);
	int nTitleHeight = ThemeManager::Instance().GetIntValue(ThemeInt_SkinWndTitleHeight);

	if (bCalcValidRects)
	{

		CRect& rc = (CRect&)lpncsp->rgrc[0];//get the client rectangle   
		rc.top += nTitleHeight;
		rc.left += nFrameWidth;
		rc.bottom -= nFrameWidth;
		rc.right -= nFrameWidth;

		if (dwStyle & WS_MAXIMIZE)
		{
			rc.left += nFrameWidth;
			rc.right -= nFrameWidth;
			rc.bottom -= nFrameWidth * 2;
		}
	}
}

LRESULT CSkinFrameWnd::OnNcHitTest(CPoint point)
{
	LRESULT nRet = HTCAPTION;

	CRect rtButton;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CPoint pt = point;
	pt.x -= rcWindow.left;
	pt.y -= rcWindow.top;

	int iButton = SysButtonHitTest(pt);
	if (iButton != -1)
	{
		switch (iButton)
		{
		case 0:
		{
			return HTCLOSE;
		}
		break;
		case 1:
		{
			return HTMAXBUTTON;
		}
		break;
		case 2:
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

void CSkinFrameWnd::OnGetMinMaxInfo(MINMAXINFO * lpMMI)
{
	int nFrameWidth = ThemeManager::Instance().GetIntValue(ThemeInt_SkinWndFrameWidth);
	int nTitleHeight = ThemeManager::Instance().GetIntValue(ThemeInt_SkinWndTitleHeight);
	lpMMI->ptMinTrackSize.y = nTitleHeight + nFrameWidth;
	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}

BOOL CSkinFrameWnd::OnNcActivate(BOOL bActive)
{
	OnNcPaint();
	return TRUE;
}

void CSkinFrameWnd::OnActivate(UINT nState, CWnd * pWndOther, BOOL bMinimized)
{
	CFrameWnd::OnActivate(nState, pWndOther, bMinimized);
	OnNcPaint();
}

void CSkinFrameWnd::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	int nTitleHeight = ThemeManager::Instance().GetIntValue(ThemeInt_SkinWndTitleHeight);

	CRect rcWindow;
	GetWindowRect(&rcWindow);
	rcWindow.bottom = rcWindow.top + nTitleHeight;


	CPoint pt = point;
	CRect rtButton;
	int iButton = SysButtonHitTest(pt);
	if (iButton != -1)
	{
		m_vecButton[iButton]->HandleMsg(WM_LBUTTONUP, 0, 0);
		OnNcPaint();
		return;
	}

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

	OnNcPaint();
}

void CSkinFrameWnd::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	CRect rtButton;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CPoint pt = point;
	pt.x -= rcWindow.left;
	pt.y -= rcWindow.top;

	int iButton = SysButtonHitTest(pt);
	if (iButton != -1)
	{
		m_vecButton[iButton]->HandleMsg(WM_LBUTTONDOWN, MAKELPARAM(pt.x, pt.y), 0);
		OnNcPaint();
		return;
	}

	if (m_rcIcon.PtInRect(pt))
	{
		CMenu *pSysMenu = GetSystemMenu(FALSE);
		if (pSysMenu)
		{
			pSysMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this, NULL);
		}
		return;
	}
	else if (nHitTest >= HTLEFT && nHitTest <= HTBOTTOMRIGHT || nHitTest == HTCAPTION)
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

void CSkinFrameWnd::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
	CRect rtButton;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CPoint pt = point;
	pt.x -= rcWindow.left;
	pt.y -= rcWindow.top;

	int iButton = SysButtonHitTest(pt);
	if (iButton != -1)
	{
		m_vecButton[iButton]->HandleMsg(WM_LBUTTONUP, 0, 0);
		switch (iButton)
		{
		case 0:
		{
			SendMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
		}
		break;
		case 1:
		{
			if (IsZoomed())
			{
				SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
			}
			else
			{
				SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
			}
		}
		break;
		case 2:
		{
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
		}
		break;
		default:
		{
			OnNcPaint();
		}
		break;
		}
	}
}

void CSkinFrameWnd::OnNcRButtonDown(UINT nHitTest, CPoint point)
{
	CRect rtButton;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CPoint pt = point;
	pt.x -= rcWindow.left;
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

	if (nHitTest >= HTLEFT && nHitTest <= HTBOTTOMRIGHT ||
		nHitTest == HTCAPTION)
	{
		CFrameWnd::OnNcMouseMove(nHitTest, point);
	}

	CRect rtButton;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	point.x -= rcWindow.left;
	point.y -= rcWindow.top;
	int iButton = SysButtonHitTest(point);
	if (iButton != m_iXButtonHovering)
	{
		if (m_iXButtonHovering != -1)
		{
			m_vecButton[m_iXButtonHovering]->OnMouseLeave();
			m_iXButtonHovering = -1;
		}
		if (iButton != -1)
		{
			m_iXButtonHovering = iButton;
			m_vecButton[m_iXButtonHovering]->OnMouseHover();
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

	if (m_iXButtonHovering != -1)
	{
		m_vecButton[m_iXButtonHovering]->OnMouseLeave();
		m_iXButtonHovering = -1;
	}

	OnNcPaint();
}

void CSkinFrameWnd::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	if (nType != SIZE_MINIMIZED && nType != SIZE_MAXHIDE)
	{
		if (m_Rgn.GetSafeHandle())
		{
			m_Rgn.DeleteObject();
		}

		CRect rc;
		GetWindowRect(&rc); //获得窗口矩形
		rc -= rc.TopLeft();
		m_Rgn.CreateRoundRectRgn(rc.left, rc.top, rc.right + 1, rc.bottom + 1, 5, 5); //根据窗口矩形创建一个圆角矩形
		SetWindowRgn(m_Rgn, TRUE); //根据圆角矩形指定的区域改变窗口的形状
	}

	CRect rcWnd;
	GetWindowRect(&rcWnd);
	rcWnd.OffsetRect(-rcWnd.left, -rcWnd.top);

	//摆放三个系统按钮的位置
	CImage * pImg = ThemeManager::Instance().GetImage(ThemeImage_SkinWndMinizeBtn_N);
	if (pImg)
	{
		int nBtnWidth = pImg->GetWidth();
		int nBtnHeight = pImg->GetHeight();
		int nBtnGap = ThemeManager::Instance().GetIntValue(ThemeInt_SkinWndFrameSysBtnGap);
		int nTitleHeight = ThemeManager::Instance().GetIntValue(ThemeInt_SkinWndTitleHeight);
		int nFrameWidth = ThemeManager::Instance().GetIntValue(ThemeInt_SkinWndFrameWidth);
		int nY = (nTitleHeight - nBtnHeight) / 2;
		int nX = nFrameWidth;
		DWORD	dwStyle = GetStyle();
		if (dwStyle & WS_MAXIMIZE)
		{
			nY += nFrameWidth + 1;
			nX += nFrameWidth * 2 + 1;
		}
		UIButton * pCloseBtn = nullptr;
		UIButton * pMaxBtn = nullptr;
		UIButton * pMinBtn = nullptr;
		if (m_vecButton.size() > 0)
		{
			pCloseBtn = m_vecButton[0];
		}
		if (m_vecButton.size() > 1)
		{
			pMaxBtn = m_vecButton[1];
		}
		if (m_vecButton.size() > 2)
		{
			pMinBtn = m_vecButton[2];
		}
		if (pCloseBtn)
		{
			pCloseBtn->MoveTo(rcWnd.Width() - nBtnWidth - nX, nY);
		}
		if (pMaxBtn)
		{
			pMaxBtn->MoveTo(rcWnd.Width() - nBtnWidth * 2 - nX - nBtnGap, nY);
		}
		if (pMinBtn)
		{
			pMinBtn->MoveTo(rcWnd.Width() - nBtnWidth * 3 - nX - nBtnGap * 2, nY);
		}
	}

	if (nType == SIZE_MAXIMIZED ||
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

BOOL CSkinFrameWnd::OnNcCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (!CFrameWnd::OnNcCreate(lpCreateStruct))
		return FALSE;

	CRect rcWnd;
	GetWindowRect(&rcWnd);
	rcWnd.OffsetRect(-rcWnd.left, -rcWnd.top);

	CRect rcControl;
	rcControl.left = rcWnd.right - 35;
	rcControl.top = 5;
	rcControl.right = rcControl.left + 17;
	rcControl.bottom = rcControl.top + 17;

	AddSysButton(ThemeImage_SkinWndCloseBtn_N, ThemeImage_SkinWndCloseBtn_F, ThemeImage_SkinWndCloseBtn_D, ThemeImage_SkinWndCloseBtn_X);
	AddSysButton(ThemeImage_SkinWndMaxizeBtn_N, ThemeImage_SkinWndMaxizeBtn_F, ThemeImage_SkinWndMaxizeBtn_D, ThemeImage_SkinWndMaxizeBtn_X);
	AddSysButton(ThemeImage_SkinWndMinizeBtn_N, ThemeImage_SkinWndMinizeBtn_F, ThemeImage_SkinWndMinizeBtn_D, ThemeImage_SkinWndMinizeBtn_X);

	return TRUE;
}

LRESULT CSkinFrameWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == 0x125)
	{
		OnNcPaint();
	}
	if (message == WM_NCLBUTTONDOWN)
	{
		OnNcPaint();
	}
	if (message == 0x00AE ||//WM_NCUAHDRAWCAPTION
		message == 0x00AF)//WM_NCUAHDRAWFRAME)
	{
		OnNcPaint();
		return 0;
	}

	return CFrameWnd::WindowProc(message, wParam, lParam);
}

int CSkinFrameWnd::SysButtonHitTest(CPoint pt)
{
	for (size_t i = 0; i < m_vecButton.size(); i++)
	{
		UIButton *& pButton = m_vecButton[i];
		if (pButton)
		{
			if (pButton->HitTest(pt))
			{
				return (int)i;
			}
		}
	}
	return -1;
}

void CSkinFrameWnd::AddSysButton(int nImageN, int nImageF, int nImageD, int nImageX)
{
	CImage * pImg = ThemeManager::Instance().GetImage(ThemeImage_SkinWndMinizeBtn_N);
	if (pImg)
	{
		int nBtnWidth = pImg->GetWidth();
		int nBtnHeight = pImg->GetHeight();

		UIButton * pButton = nullptr;

		pButton = new UIButton;
		pButton->Create(0, 0, nBtnWidth, nBtnHeight, 0, GetSafeHwnd(), nImageN, nImageF, nImageD, nImageX);
		m_vecButton.push_back(pButton);
	}
}
