#include "stdafx.h"
#include "UIControl/CustomDropDownWnd.h"
#include "ThemeManager.h"

#define Custom_szDropDownWndClass			"CustomDropDownWindow"

//************  CustomDropDownWnd ******************************************
long CustomDropDownWnd::sm_lRef = 0;
CustomDropDownWnd::CustomDropDownWnd()
{
	m_nItemHeight	 = 18;
	m_nTop			 = 0;
	m_bScrollBarShow = FALSE;
	
	m_pMessageWnd		= NULL;

	m_bIsAutoDelete		= FALSE;
	m_pVScrollBar	 = new CustomScrollBar;

	if(sm_lRef == 0)
		RegisterDropDownWndClass(AfxGetInstanceHandle());
	sm_lRef ++;
	m_bSortString	= FALSE;

	m_ptOldCursorPos.x = m_ptOldCursorPos.y = 0;
	m_nPreIndex = -1;

	m_nCurSel = -1;
}

CustomDropDownWnd::~CustomDropDownWnd()
{
	SAFE_DELETE(m_pVScrollBar);

	m_AllItemArray.RemoveAll();
	m_arrItemData.RemoveAll();
	m_ItemImagelist.DeleteImageList();
	
	sm_lRef--;
	if(sm_lRef==0)
		UnRegisterDropDownWndClass();
}

BEGIN_MESSAGE_MAP(CustomDropDownWnd, CWnd)
	//{{AFX_MSG_MAP(CustomDropDownWnd)
	ON_WM_CREATE()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_LBUTTONUP()
	ON_WM_DESTROY()
	ON_WM_RBUTTONDOWN()
	ON_WM_CAPTURECHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CustomDropDownWnd message handlers

int CustomDropDownWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CRect rtClient;
	GetClientRect( &rtClient );
	
	CRect rtVScroll = rtClient;
	rtVScroll.right = rtClient.right - 1;
	rtVScroll.left = rtVScroll.right - CRect(0,0,15,95).Width();
	rtVScroll.bottom = rtClient.bottom -1;

	if(m_pVScrollBar)
		m_pVScrollBar->Create(SBS_VERT | WS_CHILD | WS_VISIBLE|WS_CLIPSIBLINGS, this, rtVScroll, 11238 );
	m_pVScrollBar->ShowWindow( SW_HIDE );
	m_pVScrollBar->HideArrow();
	
	if(!m_pMessageWnd)
		m_pMessageWnd = GetOwner();//GetParent();

	return 0;
}

void CustomDropDownWnd::OnDestroy() 
{
	CWnd::OnDestroy();
	
	if(m_pVScrollBar && ::IsWindow(m_pVScrollBar->m_hWnd))	
		m_pVScrollBar->DestroyWindow();
}

void CustomDropDownWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int nOldTop = m_nTop;

	SCROLLINFO  info;
	int nMin, nMax;
	m_pVScrollBar->GetScrollInfo(&info);
	m_pVScrollBar->GetScrollRange(&nMin, &nMax);  

	switch(nSBCode) 
	{
	case SB_LEFT:
		m_nTop = nMin;
		break;
	case SB_RIGHT:
		m_nTop = nMax;
		break;
	case SB_LINELEFT:
		m_nTop --;
		break;
	case SB_LINERIGHT:
		m_nTop ++;
		break;
	case SB_PAGELEFT:
		m_nTop -= info.nPage;
		break;
	case SB_PAGERIGHT:
		m_nTop += info.nPage;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_nTop = nPos;
		break;
	case SB_ENDSCROLL:
		break;
	default:
		break;
	}

	if (m_nTop > nMax - max(int(info.nPage), 0))
		m_nTop = nMax - max(int(info.nPage), 0);
	if (m_nTop < nMin)
		m_nTop = nMin;

	m_pVScrollBar->SetScrollPos(m_nTop);

	CRect rtClient;
	GetClientRect( &rtClient );
	if(nOldTop != m_nTop)
		InvalidateRect( rtClient );
	
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);

	if( nSBCode == SB_ENDSCROLL )
	{
		SetCapture();
	}
}

void CustomDropDownWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	int nRow = -1;
	int nValue = HitTest( point , nRow );
	if( nValue == -1)//-1：鼠标点击位于窗口之外。
	{
		::ReleaseCapture();
		ShowWindow(SW_HIDE);
		m_pMessageWnd->SetCapture();
		
		if( m_pMessageWnd )
			m_pMessageWnd->SendMessage( WM_DROPWND_DISFOCUS , 0 , 0 );		
	}	
	else if( nValue == 2 )//2：鼠标点击位于滚动条窗口内。
	{
		::ReleaseCapture();
		m_pVScrollBar->SetCapture();
		ClientToScreen( &point );
		m_pVScrollBar->ScreenToClient( &point );
		m_pVScrollBar->SendMessage( WM_LBUTTONDOWN , nFlags ,MAKELPARAM(point.x, point.y));	
	}
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CustomDropDownWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	int nRow = -1;
	int nValue = HitTest( point , nRow );
	if(nValue == 1 || nValue == 0)//1：鼠标点击位于窗口内,0：点击在窗口内但所选条目超过了条目总范围。
	{			
		::ReleaseCapture();
		ShowWindow(SW_HIDE);
		m_pMessageWnd->SetCapture();		
		if( nRow != -1 )
		{
			m_nCurSel = nRow;
			if( m_pMessageWnd )
				m_pMessageWnd->SendMessage( WM_DROPWND_SELECT , m_nCurSel , 0 );	
		}
	}
	CWnd::OnLButtonUp(nFlags, point);
}

void CustomDropDownWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	CWnd::OnMouseMove(nFlags, point);
	
	if (!IsWindowVisible())
		return;
	
	int nRow = -1;
	int nValue = HitTest( point , nRow );

	if (nRow != -1)
	{
		if ((point.y - m_ptOldCursorPos.y) >= m_nItemHeight/3 ||
			(m_ptOldCursorPos.y - point.y) >= m_nItemHeight/3)
		{
			SetCurSel(nRow);
			m_ptOldCursorPos = point;
		}
	}
	else if(nValue == 2)
	{
		ClientToScreen( &point );
		m_pVScrollBar->ScreenToClient( &point );
		m_pVScrollBar->SendMessage( WM_MOUSEMOVE , nFlags ,MAKELPARAM(point.x, point.y));
	}
}

void CustomDropDownWnd::OnNcPaint() 
{
	CDC* pDC = GetWindowDC();
	DrawBorder(pDC);
	ReleaseDC(pDC);
}

void CustomDropDownWnd::DrawOneItem( CDC* pDC , int nItem )
{
	if( nItem < m_nTop || nItem >= ( m_nTop + m_nRows ) )
		return;

	CRect rtClient;
	GetClientRect( &rtClient );
	rtClient.DeflateRect( 1 , 0 );

	int nTop = rtClient.top + ( nItem - m_nTop )*m_nItemHeight;

	CRect rtText = rtClient;
	rtText.top = nTop;
	rtText.bottom = rtText.top + m_nItemHeight;
	if( m_bScrollBarShow )
		rtText.right -= CRect(0,0,15,95).Width();
	
	pDC->SetBkMode(TRANSPARENT);
	
	if( m_nCurSel == nItem )
	{
		CRect rtFill = rtText;
		rtFill.InflateRect(0,-1);
		pDC->SetTextColor(RGB(250,250,250));
		pDC->FillSolidRect(&rtFill, ::GetSysColor(COLOR_HIGHLIGHT));
	}
	else
	{
		pDC->SetTextColor(RGB(185,185,185));
	}

	rtText.left += 4;
	CString str = m_AllItemArray.GetAt( nItem );

	CFont* pDefFont = GetFont();
	if (pDefFont == NULL && (pDefFont = GetParent()->GetFont()) == NULL)
		pDefFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	CFont* pOldFont = pDC->SelectObject(pDefFont);

	CRect rcImage = rtText;
	IMAGEINFO ImageIf; ImageIf.rcImage = CRect(0,0,0,0);
	if(HIMAGELIST(m_ItemImagelist) && m_ItemImagelist.GetImageCount() != 0)
	{
		m_ItemImagelist.GetImageInfo(0,&ImageIf);
		rtText.left += (ImageIf.rcImage.right - ImageIf.rcImage.left);

		rcImage.right = rcImage.left + ImageIf.rcImage.right - ImageIf.rcImage.left;
		UINT uImageIndex;
		if(m_mapItemImage.Lookup(nItem,uImageIndex))
		{
			HICON hIcon = m_ItemImagelist.ExtractIcon(uImageIndex);
			IMAGEINFO ImgInfo;
			m_ItemImagelist.GetImageInfo(uImageIndex,&ImgInfo);
			CSize sz = CSize(ImgInfo.rcImage.right - ImgInfo.rcImage.left , ImgInfo.rcImage.bottom - ImgInfo.rcImage.top);
			pDC->DrawState(CPoint(rcImage.left,rcImage.top + (GetItemHeight() - sz.cy )/2),sz,hIcon,DSS_NORMAL,HBRUSH(NULL));
		}
	}
	pDC->DrawText( str , rtText , DT_VCENTER | DT_SINGLELINE|DT_LEFT );

	pDC->SelectObject(pOldFont);
}

void CustomDropDownWnd::OnPaint() 
{	
	CPaintDC dc(this); // device context for painting
	CRect rtClient;
	GetClientRect( &rtClient );
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmpBK;
	bmpBK.CreateCompatibleBitmap(&dc,rtClient.Width(),rtClient.Height());
	CBitmap* Oldbmp = memDC.SelectObject(&bmpBK);
	
	CalRowsNum();
	if(m_nRows >= GetCount())
	{
		if (m_pVScrollBar->IsWindowVisible())
			m_pVScrollBar->ShowWindow(SW_HIDE);
		if (m_pVScrollBar->IsWindowEnabled())
			m_pVScrollBar->EnableWindow(FALSE);
		m_bScrollBarShow = FALSE;
	}

	COLORREF clBk = ThemeManager::Instance().GetColor(ThemeColor_CustomDropDownWndBk);
	memDC.FillSolidRect(rtClient, clBk);

	for( int i = 0 ; i < m_AllItemArray.GetSize() ; i ++ )
		DrawOneItem( &memDC, i );

	dc.BitBlt(rtClient.left,rtClient.top,rtClient.Width(),rtClient.Height(),&memDC,
		0,0,SRCCOPY);
	memDC.SelectObject(Oldbmp);
	memDC.DeleteDC();
	bmpBK.DeleteObject();
}

BOOL CustomDropDownWnd::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	dwStyle &=~WS_CHILD;
	dwStyle |=WS_POPUP|WS_CLIPSIBLINGS;
	BOOL bRet = CWnd::CreateEx( NULL,_T(Custom_szDropDownWndClass) , _T("DropDown") , dwStyle,
								rect , pParentWnd, NULL );

	if (!bRet)
	{
		DWORD dwCode = ::GetLastError();
		TRACE("Create dropdownwnd error code =%d\n", dwCode);
	}

	m_pMessageWnd = pParentWnd;

	return bRet;
}

int CustomDropDownWnd::GetCount() const
{
	return (int)m_AllItemArray.GetSize();
}

int CustomDropDownWnd::GetCurSel() const
{
	return m_nCurSel;
}

int CustomDropDownWnd::SetCurSel(int nSelect)
{
	int iOldSelect = m_nCurSel; 
	m_nCurSel = nSelect;

	ReDrawOneItem( iOldSelect );	
	ReDrawOneItem( m_nCurSel );

	return m_nCurSel;
}

void CustomDropDownWnd::EnsureVisible()
{
	if(GetCount() <= m_nRows)
		return;
	if(m_nCurSel < m_nTop ||
		m_nCurSel >= m_nTop + m_nRows)
	{
		m_nTop = m_nCurSel - m_nRows/2;
		m_nTop = max(m_nTop,0);
		m_nTop = min(GetCount() - m_nRows ,m_nTop);
		if(m_pVScrollBar)
		{
			SCROLLINFO si;
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_POS|SIF_TRACKPOS;
			m_pVScrollBar->GetScrollInfo(&si);
			si.nPos = si.nTrackPos = m_nTop;
			m_pVScrollBar->SetScrollInfo(&si);
		}
	}	
}

void CustomDropDownWnd::EnsureVisibleEx(int nItem)
{
	if(GetCount() <= m_nRows)
		return;
	if(nItem <= m_nTop ||
		nItem >= m_nTop + m_nRows)
	{
		if(nItem<=m_nTop)
			m_nTop = nItem;
		if(nItem>=m_nTop +m_nRows)
			m_nTop = nItem -m_nRows+1;
		if(m_pVScrollBar)
		{
			SCROLLINFO si;
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_POS|SIF_TRACKPOS;
			m_pVScrollBar->GetScrollInfo(&si);
			si.nPos = si.nTrackPos = m_nTop;
			m_pVScrollBar->SetScrollInfo(&si);
		}
	}
}

int CustomDropDownWnd::AddString(LPCTSTR lpszString)
{
	if(m_bSortString)
	{
		int nIndex = AddStringSort(lpszString);
		if (nIndex <= m_nCurSel)
			m_nCurSel++;
	}
	else
		m_AllItemArray.Add( lpszString );	
	m_arrItemData.Add(0);
	CalRowsNum();
	SetVerScrollBarRange();
	return (int)m_AllItemArray.GetSize();
}

int CustomDropDownWnd::AddStringEx(LPCTSTR lpszString)
{
	int nIndex = GetCount();
	if(m_bSortString)
	{
		nIndex = AddStringSort(lpszString);
		m_arrItemData.InsertAt(nIndex, (DWORD)0);
		if (nIndex <= m_nCurSel)
			m_nCurSel++;
	}
	else
	{
		m_AllItemArray.Add( lpszString );	
		m_arrItemData.Add(0);
	}
	CalRowsNum();
	SetVerScrollBarRange();
	return nIndex;
}

int CustomDropDownWnd::AddStringArray(const CStringArray & aryString)
{
	m_AllItemArray.Append(aryString);
	for (int i = 0; i < aryString.GetSize(); i++)
	{
		m_arrItemData.Add(0);
	}
	CalRowsNum();
	SetVerScrollBarRange();
	return (int)m_AllItemArray.GetSize();
}

int CustomDropDownWnd::DeleteString(UINT nIndex)
{
	int i =(int)m_AllItemArray.GetSize();
	int j =(int)m_arrItemData.GetSize();
	if(nIndex<(UINT)0 || (UINT)i<nIndex ||(UINT)j<nIndex)
		return -1;
	m_AllItemArray.RemoveAt( nIndex );
	m_arrItemData.RemoveAt(nIndex);

	if(IsWindow(m_hWnd))
	{
		CalRowsNum();
		SetVerScrollBarRange();		
	}

	if(m_nCurSel == (int)nIndex)
	{
		if(m_nCurSel >= GetCount())
		{
			m_nCurSel = -1;
		}
	}	
	if((int)nIndex < m_nCurSel)
	{
		m_nCurSel--;
		
	}
	return nIndex;
}

int CustomDropDownWnd::InsertString(int nIndex, LPCTSTR lpszString)
{
	int nNewIdx = nIndex;
	if( nIndex < 0 || nIndex > GetCount() )
		nNewIdx = GetCount();

	m_AllItemArray.InsertAt( nNewIdx , lpszString );
	m_arrItemData.InsertAt(nNewIdx, (DWORD)0);

	CalRowsNum();
	SetVerScrollBarRange();

	if(nIndex <= m_nCurSel)
	{
		m_nCurSel++;		
	}
	
	return nNewIdx;
}

int CustomDropDownWnd::ReplaceString(int nIndex,LPCTSTR lpszValue)
{
	if(nIndex < 0 || nIndex > GetCount())
		return -1;//falure;
	m_AllItemArray.SetAt(nIndex,lpszValue);
	return nIndex;
}

void CustomDropDownWnd::ResetContent()
{
	m_AllItemArray.RemoveAll();
	m_arrItemData.RemoveAll();

	m_nRows = 0;
	m_nTop = 0;
	SetVerScrollBarRange();

	m_nCurSel = -1;
}

void CustomDropDownWnd::CalRowsNum()
{
	if(!IsWindow(m_hWnd))
		return;

	CRect rtClient;
	GetWindowRect(rtClient);
	rtClient.InflateRect(-1,-1);

	int Height = rtClient.Height();

	m_nRows = Height/m_nItemHeight; 
}

/*
Name: HitTest
Description: 获得鼠标点击位置所处的区域。
Param: pt: 鼠标点击的点位置。nRow：点击的条目所在行。
Return Value: -1：鼠标点击位于窗口之外。 1：鼠标点击位于窗口内。
			   2：鼠标点击位于滚动条窗口内。0：点击在窗口内但所选条目超过了条目总范围。
*/
int CustomDropDownWnd::HitTest( CPoint pt , int& nRow )
{
	nRow = -1;

	CRect rtClient;
	GetClientRect( &rtClient );

	if( rtClient.PtInRect( pt ) == FALSE )
		return -1;
	
	for( int i = 0 ; i < m_nRows ; i ++ )
	{
		CRect rtItem = rtClient;
		rtItem.top = rtClient.top + i*m_nItemHeight;
		rtItem.bottom = rtItem.top + m_nItemHeight;
		if( m_bScrollBarShow )
			rtItem.right -= CRect(0,0,15,95).Width();
		if( rtItem.PtInRect( pt ) )
		{
			nRow = m_nTop + i;
			if( nRow >= m_AllItemArray.GetSize() )
			{
				nRow = -1;
				return 0;
			}
			return 1 ;
		}
	}

	CRect rtVScroll = rtClient;
	rtVScroll.left = rtVScroll.right - CRect(0,0,15,95).Width();
	if( rtVScroll.PtInRect( pt ) )
	{
		return 2;
	}

	return 0;
}

void CustomDropDownWnd::SetVerScrollBarRange()
{
	if(!m_pVScrollBar||!::IsWindow(m_pVScrollBar->m_hWnd))
		return;

	if( GetCount()<= m_nRows ) 
	{
		m_nTop = 0;
		m_pVScrollBar->ShowWindow( SW_HIDE );
		m_pVScrollBar->EnableWindow( FALSE );

		m_bScrollBarShow = FALSE;
	} 
	else 
	{
		SCROLLINFO info;
		info.cbSize = sizeof(SCROLLINFO);
		info.fMask = SIF_RANGE|SIF_POS|SIF_PAGE;//SIF_PAGE|SIF_RANGE;
		m_pVScrollBar->GetScrollInfo(&info);
		info.nMin = 0;
		info.nPage = m_nRows;
		info.nMax = GetCount();		// -1可能会少显示最后一行

		info.nPos = info.nTrackPos = m_nTop;

		m_pVScrollBar->SetScrollInfo(&info);
		
		m_nTop = m_pVScrollBar->GetScrollPos();

		m_pVScrollBar->ShowWindow( SW_SHOW );
		m_pVScrollBar->EnableWindow( TRUE );
		m_bScrollBarShow = TRUE;

	}
}

void CustomDropDownWnd::ReDrawOneItem( int nItem )
{
	if( nItem < m_nTop || nItem >= ( m_nTop + m_nRows ) )
		return;

	CRect rtClient;
	GetClientRect( &rtClient );
	rtClient.DeflateRect( 1 , 0 );

	int nTop = rtClient.top + ( nItem - m_nTop )*m_nItemHeight;
	
	CRect rtText = rtClient;
	rtText.top = nTop;
	rtText.bottom = rtText.top + m_nItemHeight;

	if( m_bScrollBarShow )
		rtText.right -= CRect(0,0,15,95).Width();

	InvalidateRect( rtText );
}

void CustomDropDownWnd::GetLBText(int nIndex, CString& rString) const
{
	if( nIndex < 0 || nIndex >= GetCount() )
		return;
	rString = m_AllItemArray.GetAt( nIndex );
}

int CustomDropDownWnd::GetLBTextLen(int nIndex) const
{
	if( nIndex < 0 || nIndex >= GetCount() )
		return 0;

	CString str = m_AllItemArray.GetAt( nIndex );

	return str.GetLength();
}

int CustomDropDownWnd::SetItemHeight( int nItemHeight )
{
	if( nItemHeight <= 0 )
		return 0;

	m_nItemHeight = nItemHeight;

	if( ::IsWindow( m_hWnd ) )   
		Invalidate();
	return m_nItemHeight;
}

int CustomDropDownWnd::GetItemHeight() const
{
	return m_nItemHeight;
}

void CustomDropDownWnd::DrawBorder( CDC* pDC )
{
	CRect rtClient;
	GetWindowRect(&rtClient);
	rtClient.OffsetRect(-rtClient.TopLeft());

	DWORD DlgEdgeColor[2];
	DlgEdgeColor[1] = RGB(0,0,0);
	DlgEdgeColor[0] = RGB(0,0,0);
	
	pDC->Draw3dRect(rtClient,DlgEdgeColor[0],DlgEdgeColor[1]);
}

void CustomDropDownWnd::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	long lData = ::GetWindowLongPtr(m_hWnd, GWLP_USERDATA);

	lpncsp->rgrc[0].left ++;
	lpncsp->rgrc[0].right --;
	lpncsp->rgrc[0].top ++;
	lpncsp->rgrc[0].bottom--;
	
	int nHeight = lpncsp->rgrc[0].bottom - lpncsp->rgrc[0].top;
	int nScale = nHeight/m_nItemHeight;
	if(lData == 0)
	{
		lpncsp->rgrc[0].bottom = lpncsp->rgrc[0].top + nScale*m_nItemHeight;
	}
	else
	{
		lpncsp->rgrc[0].top = lpncsp->rgrc[0].bottom - nScale*m_nItemHeight;
	}
	
	m_nRows = nScale;
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	if(m_pVScrollBar && ::IsWindow(m_pVScrollBar->m_hWnd))
	{
		m_pVScrollBar->GetScrollInfo(&si);
		si.nPage = nScale;
		m_nTop = si.nPos;
		m_pVScrollBar->SetScrollInfo(&si);
	}
	
	CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
	SetVerScrollBarRange();
}

int CustomDropDownWnd::SetItemData(int nIndex, DWORD_PTR dwItemData ){
	if(nIndex < 0 || nIndex >= GetCount())
		return LB_ERR; 
	m_arrItemData.SetAt(nIndex,dwItemData);
	return TRUE;
}

DWORD_PTR CustomDropDownWnd::GetItemData(int nIndex ) const {
	if(nIndex < 0 || nIndex >= GetCount())
		return (DWORD)LB_ERR;
	return m_arrItemData.GetAt(nIndex);
}

int CustomDropDownWnd::GetNums()
{
	int nums = (int)m_AllItemArray.GetSize();
	return nums;
}

void CustomDropDownWnd::ReSetScrollWndPos()
{
	if( m_bScrollBarShow == FALSE )
		return;

	CRect rtClient;
	GetWindowRect(rtClient);
	rtClient.InflateRect(-1,-1);
	rtClient.OffsetRect(-rtClient.TopLeft());

	CRect rtVScroll = rtClient;
	rtVScroll.right = rtClient.right ;
	rtVScroll.left = rtVScroll.right - CRect(0,0,15,95).Width();
	rtVScroll.bottom = rtClient.bottom;

	rtVScroll.top ++;
	rtVScroll.bottom --;
	m_pVScrollBar->SetWindowPos( NULL , rtVScroll.left , rtVScroll.top , rtVScroll.Width() , rtVScroll.Height(),  SWP_NOZORDER );
}

//-------------------------------------------------------------------------------------------------
//	Function:		FindString()	
//	Description:	Find specified string in combobox. If the return value is greater than or equal
//					to 0, it is the zero-based index of the matching item. It is CB_ERR if the 
//					search was unsuccessful.
//-------------------------------------------------------------------------------------------------
int CustomDropDownWnd::FindString(int nStartAfter, LPCTSTR lpszString) const
{
	int nFindStart = (nStartAfter == -1)?0:nStartAfter;

	int nCount = (int)m_AllItemArray.GetSize();
	if(nCount <= 0)
		return CB_ERR;

	int i = nFindStart;
	for(; i < nCount; i++)
	{
		CString strString = m_AllItemArray[i];
		if(strString.CompareNoCase(lpszString) == 0)
			break;
	}

	return (i==nCount)?CB_ERR:i;
}

WNDPROC CustomDropDownWnd::m_lpSuperDropDownWnd = NULL;

LRESULT CALLBACK EXPORT
CustomDropDownWnd::WndProcHook(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	CustomDropDownWnd* pDropDownWnd = (CustomDropDownWnd*)FromHandlePermanent(hWnd);
	if(pDropDownWnd == NULL)//window has not been created
	{
		pDropDownWnd = new CustomDropDownWnd;
		pDropDownWnd->Attach(hWnd);
		pDropDownWnd->m_bIsAutoDelete = TRUE;		
	}
	pDropDownWnd->m_pfnSuper = CustomDropDownWnd::m_lpSuperDropDownWnd;
	::SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG)(LONG_PTR)AfxWndProc);
#ifdef STRICT
	return ::CallWindowProc(AfxWndProc, hWnd, uMsg, wParam, lParam);
#else
	return ::CallWindowProc((FARPROC)AfxWndProc, hWnd, uMsg, wParam, lParam);
#endif

}

BOOL CustomDropDownWnd::RegisterDropDownWndClass(HINSTANCE hInst)
{
	WNDCLASS wndcls;
	static TCHAR szClass[] = _T(Custom_szDropDownWndClass);

	if(::GetClassInfo(hInst,szClass,&wndcls))
	{
		return wndcls.lpfnWndProc == (WNDPROC)CustomDropDownWnd::WndProcHook;
	}

	VERIFY(::GetClassInfo(NULL,_T("Static"),&wndcls));
	m_lpSuperDropDownWnd = wndcls.lpfnWndProc;

	wndcls.lpszClassName = szClass;
	wndcls.lpfnWndProc = WndProcHook;
	wndcls.hInstance = hInst;
	wndcls.hCursor = ::LoadCursor(NULL,IDC_ARROW);
	if(RegisterClass(&wndcls) == 0)
		return FALSE;
	return TRUE;
}

BOOL CustomDropDownWnd::UnRegisterDropDownWndClass()
{
	return ::UnregisterClass(_T(Custom_szDropDownWndClass),AfxGetInstanceHandle());
}

BOOL CustomDropDownWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CWnd::PreCreateWindow(cs);
}

BOOL CustomDropDownWnd::SetImageList(CImageList* pImageList)
{
	if(!pImageList) return FALSE;
	m_ItemImagelist.DeleteImageList();
	m_ItemImagelist.Create(pImageList);
	if(pImageList->GetImageCount())
	{
		IMAGEINFO ImageInfo;
		m_ItemImagelist.GetImageInfo(0,&ImageInfo);
		int h = ImageInfo.rcImage.bottom - ImageInfo.rcImage.top;
		if(h > GetItemHeight())
			SetItemHeight(h);
	}
	return TRUE;
}

CImageList* CustomDropDownWnd::GetImageList()
{
	return &m_ItemImagelist;
}

BOOL CustomDropDownWnd::SetItemImage(int nItemIndex,UINT nImageIndex)
{
	if(nItemIndex < 0 || nItemIndex > GetNums() -1) 
		return FALSE;
	if(nImageIndex < 0 || nItemIndex >= m_ItemImagelist.GetImageCount())
		return FALSE;

	m_mapItemImage.SetAt(nItemIndex,nImageIndex);
	return TRUE;
}

int CustomDropDownWnd::GetItemImage(int nItemIndex ) const
{
	if(nItemIndex <= 0 || nItemIndex > m_AllItemArray.GetSize() - 1)
		return -1;
	UINT nImageIndex;
	if(!m_mapItemImage.Lookup(nItemIndex,nImageIndex))
		return -1;
	return nImageIndex;
}

int CustomDropDownWnd::AddStringSort(CString strInsert,BOOL bAsc/*=FALSE*/)
{
	BOOL bInsert =FALSE;
	int nIndex=-1;
	CString str1 = strInsert, str2=_T("");
	int i=0;
	for(; i< m_AllItemArray.GetSize(); i++)
	{
		str2 = m_AllItemArray[i];
		if(!bAsc)
		{
			str1.MakeUpper();
			str2.MakeUpper();
		}
		LPCSTR lpszStr1 , lpszStr2;
		char *buffer=NULL;	
		char *buffer2=NULL;
		USES_CONVERSION;
#ifdef _UNICODE
		int SendSize=WideCharToMultiByte(CP_ACP,0,str1,-1,NULL,0,NULL,NULL);
		buffer =new char[SendSize+1];
		int err = WideCharToMultiByte(CP_ACP,0,str1,-1,buffer,SendSize,NULL,NULL);
		buffer[SendSize] = '\0';
		lpszStr1 = buffer;	
		
		SendSize=WideCharToMultiByte(CP_ACP,0,str2,-1,NULL,0,NULL,NULL);
		buffer2 =new char[SendSize+1];
		err = WideCharToMultiByte(CP_ACP,0,str2,-1,buffer2,SendSize,NULL,NULL);
		buffer2[SendSize] = '\0';
		lpszStr2 = buffer2;	
		
#else
		lpszStr1 = str1.GetBuffer(str1.GetLength());
		lpszStr2 = str2.GetBuffer(str2.GetLength());
#endif
		if(strcmp(lpszStr1,lpszStr2) > 0)
		{
			
		}
		else
		{
			m_AllItemArray.InsertAt(i, strInsert);
			bInsert = TRUE;
			if(buffer) { delete [] buffer; buffer=NULL; }
			if(buffer2){ delete [] buffer2;buffer2=NULL; }
			break;
		}	
		if(buffer) { delete [] buffer;  buffer=NULL; }
		if(buffer2){ delete [] buffer2; buffer2=NULL; }
	}
	nIndex =i;
	if(bInsert)
	{
	}
	else if(i == 0)
		m_AllItemArray.InsertAt(0, strInsert);
	else if(i == m_AllItemArray.GetSize())
		m_AllItemArray.Add(strInsert);	
	return nIndex;
}

void CustomDropDownWnd::ShowTip(int nIndex)
{
}

void CustomDropDownWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	int nRow = -1;
	int nValue = HitTest( point , nRow );
	if( nValue == -1)//-1：鼠标点击位于窗口之外。
	{
		::ReleaseCapture();
		ShowWindow(SW_HIDE);
		m_pMessageWnd->SetCapture();
		
		if( m_pMessageWnd )
			m_pMessageWnd->SendMessage( WM_DROPWND_DISFOCUS , 0 , 0 );
	}	

	CWnd::OnRButtonDown(nFlags, point);
}

void CustomDropDownWnd::OnCaptureChanged(CWnd *pWnd) 
{
	POINT point;
	GetCursorPos(&point);
	this->ScreenToClient(&point);

	CPoint cpoint(point.x,point.y);
	int nRow = -1;
	int nValue = HitTest( cpoint , nRow );
	if( nValue == -1)//-1：鼠标点击位于窗口之外。
	{
		::ReleaseCapture();
		ShowWindow(SW_HIDE);
	}
	
	CWnd::OnCaptureChanged(pWnd);
}
