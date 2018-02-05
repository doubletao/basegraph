// SkinFrameWnd.cpp : implementation file
//

#include "stdafx.h"
#include "SkinDlgWnd.h"

#define  ID_CLOSE_BTN 110
#define  ID_MAX_BTN   111
#define  ID_MIN_BTN   112

#define _UNICODE

// CSkinDlgWnd

IMPLEMENT_DYNCREATE(CSkinDlgWnd,CDialog)

void CSkinDlgWnd::LoadBmpRes()
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

CSkinDlgWnd::CSkinDlgWnd()
	: m_rtMenuBar()
	, m_pImgBk(NULL)
{
	m_nTitleHeight = 38;
	m_nFrameWidth = 3;

	LoadBmpRes();
	m_iXButtonHovering = -1;

	m_bNCHovering = FALSE;
	m_bNCTracking = FALSE;

	m_rcIcon = CRect(15,13,15+GetSystemMetrics(SM_CXSMICON),13+GetSystemMetrics(SM_CYSMICON));
}

CSkinDlgWnd::CSkinDlgWnd(UINT nIDTemplate, CWnd* pParentWnd /*= NULL*/)
	: CDialog(nIDTemplate, pParentWnd)
	, m_pImgBk(NULL)
	, m_rtMenuBar()
{
	m_nTitleHeight = 38;
	m_nFrameWidth = 3;

	LoadBmpRes();
	m_iXButtonHovering = -1;

	m_bNCHovering = FALSE;
	m_bNCTracking = FALSE;

	m_rcIcon = CRect(15,13,15+GetSystemMetrics(SM_CXSMICON),13+GetSystemMetrics(SM_CYSMICON));
}

CSkinDlgWnd::~CSkinDlgWnd()
{
	if (m_pImgBk != NULL)
	{
		m_pImgBk->Detach();
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
}


BEGIN_MESSAGE_MAP(CSkinDlgWnd, CDialog)
	ON_WM_NCPAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_NCHITTEST()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_GETMINMAXINFO()
	ON_WM_NCACTIVATE()
	ON_WM_ACTIVATE()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONUP()
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCMOUSEHOVER()
	ON_WM_NCMOUSELEAVE()
	ON_WM_SIZE()
	ON_WM_SYSCOMMAND()
	ON_WM_NCCREATE()
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, &CSkinDlgWnd::OnToolTipText)
END_MESSAGE_MAP()


// CSkinDlgWnd message handlers

void CSkinDlgWnd::OnNcPaint()
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

	//绘制底部和边框
	destRect.X = 0;
	destRect.Y = rcWindow.Height() - m_nFrameWidth;
	destRect.Width = rcWindow.Width();
	destRect.Height = m_nFrameWidth;
	if (destRect.Width <= 0 || destRect.Height <= 0)
	{
		return;
	}
	m_pImgBk->Draw(MemDC.GetSafeHdc(), CRect(destRect.GetLeft(), destRect.GetTop(), destRect.GetRight(), destRect.GetBottom()));

	destRect.X = rcWindow.right-m_nTitleHeight;
	destRect.Y = rcWindow.bottom - m_nTitleHeight;
	destRect.Width = m_nTitleHeight;
	destRect.Height = m_nTitleHeight;
	if (destRect.Width <= 0 || destRect.Height <= 0)
	{
		return;
	}
	m_pImgBk->Draw(MemDC.GetSafeHdc(), CRect(destRect.GetLeft(), destRect.GetTop(), destRect.GetRight(), destRect.GetBottom()));

	destRect.X = 0;
	destRect.Y = rcWindow.Height() - m_nTitleHeight;
	destRect.Width = m_nTitleHeight;
	destRect.Height = m_nTitleHeight;
	if (destRect.Width <= 0 || destRect.Height <= 0)
	{
		return;
	}
	m_pImgBk->Draw(MemDC.GetSafeHdc(), CRect(destRect.GetLeft(), destRect.GetTop(), destRect.GetRight(), destRect.GetBottom()));

	//左边框
	destRect.X = 0;
	destRect.Y = m_nTitleHeight;
	destRect.Width = m_nFrameWidth;
	destRect.Height = rcWindow.Height() - 2*m_nTitleHeight;
	if (destRect.Width <= 0 || destRect.Height <= 0)
	{
		return;
	}
	m_pImgBk->Draw(MemDC.GetSafeHdc(), CRect(destRect.GetLeft(), destRect.GetTop(), destRect.GetRight(), destRect.GetBottom()));


	//右边框
	destRect.X = rcWindow.Width() - m_nFrameWidth;
	destRect.Y = m_nTitleHeight;
	destRect.Width = m_nFrameWidth;
	destRect.Height = rcWindow.Height() - 2*m_nTitleHeight;
	if (destRect.Width <= 0 || destRect.Height <= 0)
	{
		return;
	}
	m_pImgBk->Draw(MemDC.GetSafeHdc(), CRect(destRect.GetLeft(), destRect.GetTop(), destRect.GetRight(), destRect.GetBottom()));

	//绘制标题栏
	destRect.X = 0;
	destRect.Y = 0;
	destRect.Width = rcWindow.Width();
	destRect.Height = m_nTitleHeight;
	if (destRect.Width <= 0 || destRect.Height <= 0)
	{
		return;
	}
	m_pImgBk->Draw(MemDC.GetSafeHdc(), CRect(destRect.GetLeft(), destRect.GetTop(), destRect.GetRight(), destRect.GetBottom()));

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

	int xPos = 15;
	int yPos = 15;

	HICON hIcon = GetIcon(FALSE);
	if (hIcon)
	{
		DrawIconEx(MemDC,xPos, yPos-2,hIcon,GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0, 0, DI_NORMAL);

		xPos += GetSystemMetrics(SM_CXSMICON);
		xPos += 5;
	}

	CString strText;
	GetWindowText(strText);
	if (!strText.IsEmpty())
	{
		//绘制标题
		LOGFONT lfFont;
		memset(&lfFont, 0, sizeof(lfFont));
		lfFont.lfHeight = -12;
		//lfFont.lfWeight |= FW_BOLD;
		lstrcpy(lfFont.lfFaceName, _T("微软雅黑"));
		Gdiplus::Font font(dc.GetSafeHdc(), &lfFont);

		StringFormat stringFormat;
		stringFormat.SetFormatFlags( StringFormatFlagsNoWrap);
		stringFormat.SetAlignment(StringAlignmentNear);
		stringFormat.SetLineAlignment(StringAlignmentNear);
		CStringW strTitle(strText);
		SolidBrush brush((ARGB)Color::White);
		PointF point;

		point.X=(Gdiplus::REAL)xPos;
		point.Y=(Gdiplus::REAL)yPos;
		graphics.DrawString(strTitle, strTitle.GetLength(), &font, point,&stringFormat, &brush);

	}

	CRect rcPaint;
	dc.GetClipBox(&rcPaint);
	std::map<int,CDUIButton *>::iterator iter;
	for (iter = m_TitleBtn.begin(); iter != m_TitleBtn.end(); iter++)
	{
		CDUIButton* dcControl = iter->second;
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
	DWORD	dwStyle	= GetStyle();
	DWORD	dwExStyle = GetExStyle();
	CMenu* pMenu = GetMenu();
	if (pMenu != NULL && !(dwStyle & WS_CHILD) && !(dwExStyle & WS_EX_TOOLWINDOW))
	{
		int n = 0;
	}

	dc.BitBlt(0,0,rcWindow.Width(),rcWindow.Height(),&MemDC,0,0,SRCCOPY);

	dc.SelectClipRgn(NULL);
}

HBRUSH CSkinDlgWnd::OnCtlColor(CDC* pDC,  CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC,pWnd, nCtlColor);
	// TODO: Change any attributes of theDC here

	pDC->SetTextColor(RGB(185,185,185));
	if (pWnd->IsKindOf(RUNTIME_CLASS( CStatic )))
	{
		// CStatic
		pDC->SetBkMode(TRANSPARENT); ///选择透明背景模式
		return g_brBK;
	}
	else if (pWnd->IsKindOf(RUNTIME_CLASS( CListBox )) ||
			pWnd->IsKindOf(RUNTIME_CLASS( CEdit )) ||
			pWnd->IsKindOf(RUNTIME_CLASS( CComboBox )))
	{
		// CStatic
		pDC->SetBkMode(TRANSPARENT); ///选择透明背景模式
		return g_brEdtBK;
	}
	return hbr;
}

CWnd * CSkinDlgWnd::GetValidCtrlWnd(DWORD nID)
{
	CWnd * pWnd = GetDlgItem(nID);
	if (pWnd && ::IsWindow(pWnd->GetSafeHwnd()))
	{
		return pWnd;
	}
	return NULL;
}

BOOL CSkinDlgWnd::GetToolTips(UINT nID,CString &strTips)
{
	if (m_mapIDTips.count(nID) > 0)
	{
		strTips = m_mapIDTips[nID];
		return TRUE;
	}
	return FALSE;
}

void CSkinDlgWnd::ModifyTooltips(UINT nID, const CString &strTips)
{
	m_mapIDTips[nID] = strTips;
}

void CSkinDlgWnd::RemoveTooltips(UINT nID)
{
	if(m_mapIDTips.count(nID) > 0)
	{
		m_mapIDTips.erase(nID);
	}
}

void CSkinDlgWnd::OnPaint()
{
	CPaintDC dc(this);
	CRect		rtClient;
	GetClientRect(&rtClient);
	CDC			MemDC;
	MemDC.CreateCompatibleDC(&dc);
	CBitmap		MemBitmap;
	MemBitmap.CreateCompatibleBitmap(&dc, rtClient.Width(), rtClient.Height());
	CBitmap*	pOldMemBmp = MemDC.SelectObject(&MemBitmap);

	//如果指定了背景图，则加载背景图，否则涂成灰色
	if (g_brBK.GetSafeHandle() != NULL)
	{
		MemDC.FillRect(rtClient, &g_brBK);
	}
	else
	{
		MemDC.FillSolidRect(rtClient,RGB(60,60,60));
	}
	dc.BitBlt(rtClient.left,rtClient.top, rtClient.Width(), rtClient.Height(), 
		&MemDC, 0, 0, SRCCOPY);
	MemDC.SelectObject(pOldMemBmp);
	MemDC.DeleteDC();
}

BOOL CSkinDlgWnd::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
	UINT nID = (UINT)pNMHDR->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND)
	{
		nID = ::GetDlgCtrlID((HWND)nID);
		if(nID)
		{
			CString strTips(_T(""));
			if(GetToolTips(nID,strTips))
			{
				lstrcpyn(pTTT->szText, strTips, sizeof(pTTT->szText));
				return TRUE;
			}
		}
	}
	return(FALSE);
}

BOOL CSkinDlgWnd::OnInitDialog()
{
	CDialog::OnInitDialog();
	EnableToolTips(TRUE);
	m_tt.Create(this);
	m_tt.Activate(TRUE);
	for (std::map<UINT, CString>::iterator it = m_mapIDTips.begin(); it != m_mapIDTips.end(); it++)
	{
		assert(GetDlgItem(it->first));
		m_tt.AddTool(GetDlgItem(it->first), LPSTR_TEXTCALLBACK);//添加
	}
	return TRUE;
}

void CSkinDlgWnd::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	if (bCalcValidRects)   
	{   

		CRect& rc = (CRect&)lpncsp->rgrc[0];//get the client rectangle   
		rc.top += m_nTitleHeight;   
		rc.left += m_nFrameWidth;   
		rc.bottom -= m_nFrameWidth;   
		rc.right -= m_nFrameWidth;   
	}

	CRect	rtWindow(lpncsp->rgrc[0]);
	CPoint	ptOffset(rtWindow.TopLeft());
	DWORD	dwStyle	= GetStyle();
	DWORD	dwExStyle = GetExStyle();

	BOOL	bToolWindow = (dwExStyle & WS_EX_TOOLWINDOW);
	BOOL	bHaveCaption = (dwStyle & WS_CAPTION) == WS_CAPTION;

	CMenu* pMenu = GetMenu();
	if (bHaveCaption && !(dwStyle & WS_CHILD) && !bToolWindow && pMenu != NULL)
	{
		m_rtMenuBar	= rtWindow;
		m_rtMenuBar.OffsetRect( -ptOffset );
		m_rtMenuBar.OffsetRect(40, 0);

		if (dwStyle & (MFS_4THICKFRAME | MFS_THICKFRAME | WS_THICKFRAME))
		{
			m_rtMenuBar.left -= m_nFrameWidth;
			rtWindow.top -= m_nFrameWidth;
		}
	}
}

LRESULT CSkinDlgWnd::OnNcHitTest(CPoint point)
{
	LRESULT nRet = HTCAPTION;
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
		nRet = CDialog::OnNcHitTest(point);
		if (nRet == HTCLOSE
			|| nRet == HTMAXBUTTON
			|| nRet == HTMINBUTTON)
		{
			nRet = HTCAPTION;
		}
	}
	return nRet;
}

void CSkinDlgWnd::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	 lpMMI->ptMinTrackSize.y = m_nTitleHeight + m_nFrameWidth;
	CDialog::OnGetMinMaxInfo(lpMMI);
}

BOOL CSkinDlgWnd::OnNcActivate(BOOL bActive)
{
	 OnNcPaint();
	 return TRUE;
}

void CSkinDlgWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	OnNcPaint();
}

void CSkinDlgWnd::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
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

	//if (rcWindow.PtInRect(pt))
	//{
	//	if (IsZoomed())
	//	{
	//		ShowWindow(SW_RESTORE);
	//	}
	//	else
	//	{
	//		ShowWindow(SW_MAXIMIZE);
	//	}

	//	OnNcPaint();
	//	return;
	//}

	OnNcPaint();
}

void CSkinDlgWnd::OnNcLButtonDown(UINT nHitTest, CPoint point)
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
		CDialog::OnNcLButtonDown(nHitTest, point);
		return;
	}

	CDialog::OnNcLButtonDown(nHitTest, point);
}

void CSkinDlgWnd::OnNcLButtonUp(UINT nHitTest, CPoint point)
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
		CDialog::OnNcLButtonUp(nHitTest, point);
	}
}

void CSkinDlgWnd::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	if (!m_bNCTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_NONCLIENT;
		tme.dwHoverTime = 1;
		m_bNCTracking = _TrackMouseEvent(&tme);
	}

	if(nHitTest>=HTLEFT && nHitTest <= HTBOTTOMRIGHT || 
		nHitTest == HTCAPTION)
	{
		CDialog::OnNcMouseMove(nHitTest, point);
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

void CSkinDlgWnd::OnNcMouseHover(UINT nFlags, CPoint point)
{
	m_bNCHovering = TRUE;
}

void CSkinDlgWnd::OnNcMouseLeave()
{
	m_bNCTracking = FALSE;
	m_bNCHovering = FALSE;

	if(m_iXButtonHovering != -1)
	{
		m_TitleBtn[m_iXButtonHovering]->MouseLeave();
		m_iXButtonHovering = -1;
	}
}

void CSkinDlgWnd::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

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
	CRect rtClose = m_rcControl;
	nY = (m_nTitleHeight - rtClose.Height()) / 2;
	nYEnd = nY + rtClose.Height();
	int nLeft = 10;
	if (m_TitleBtn.count(ID_CLOSE_BTN) > 0)
	{
		m_TitleBtn[ID_CLOSE_BTN]->GetRect(rtClose);
		nY = (m_nTitleHeight - rtClose.Height()) / 2;
		nYEnd = nY + rtClose.Height();

		CRect rClose(rcWnd.right - rtClose.Width() - nLeft, nY, rcWnd.right - nLeft, nYEnd);
		m_TitleBtn[ID_CLOSE_BTN]->SetRect(rClose);
	}
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

	if (nType == SIZE_MAXIMIZED||
		nType == SIZE_RESTORED)
	{
		OnNcPaint();
	}

}

void CSkinDlgWnd::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID != SC_CLOSE)
	{
		OnNcPaint();
	}

	CDialog::OnSysCommand(nID, lParam);
}

LRESULT CSkinDlgWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == 0x125)
	{
		OnNcPaint();
	}

	if (message ==0x00AE||//WM_NCUAHDRAWCAPTION
		message==0x00AF)//WM_NCUAHDRAWFRAME)
	{
		OnNcPaint();
		return 0;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CSkinDlgWnd::OnNcCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (!CDialog::OnNcCreate(lpCreateStruct))
		return FALSE;

	CRect rcWnd;
	GetWindowRect(&rcWnd);
	rcWnd.OffsetRect( -rcWnd.left, -rcWnd.top);

	m_rcControl.left = rcWnd.right- 35;
	m_rcControl.top = 5;
	m_rcControl.right = m_rcControl.left + 17;
	m_rcControl.bottom = m_rcControl.top + 17;

	DWORD	dwStyle	= lpCreateStruct->style;
	if ((dwStyle & WS_MINIMIZEBOX) != 0 &&
		(dwStyle & WS_SYSMENU) != 0)
	{
		AddTitleButton(m_rcControl,m_vecStrBtnImages[0],3,ID_MIN_BTN);
	}
	if ((dwStyle & WS_MAXIMIZEBOX) != 0 &&
		(dwStyle & WS_SYSMENU) != 0)
	{
		AddTitleButton(m_rcControl,m_vecStrBtnImages[1],3,ID_MAX_BTN);
	}
	if ((dwStyle & WS_SYSMENU) != 0)
	{
		AddTitleButton(m_rcControl,m_vecStrBtnImages[2],3,ID_CLOSE_BTN);
	}

	return TRUE;
}

//往对话框添加一个按钮
CDUIButton* CSkinDlgWnd::AddTitleButton(LPRECT lpRect, CString strImage, UINT nCount /*= 3*/ , UINT nID/* = 0*/,LPCTSTR lpszCaption/*=NULL*/)
{
	CDUIButton  * dcControl = new CDUIButton;
	dcControl->CreateButton(lpszCaption, lpRect, this,strImage,nCount, nID);
	m_TitleBtn[nID] = dcControl;

	return m_TitleBtn[nID];
}

BOOL CSkinDlgWnd::PreTranslateMessage(MSG* pMsg)
{
	if (::IsWindow(GetSafeHwnd()))
	{
		m_tt.RelayEvent(pMsg);
	}
	return CDialog::PreTranslateMessage(pMsg);
}

int CSkinDlgWnd::TButtonHitTest(CPoint point, CRect& rtButton)
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