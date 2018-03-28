#include "stdafx.h"
#include "UIControl\CustomEdit.h"
#include "UIControl\ThemeResoueceID.h"
#include "ThemeManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

inline BOOL IsValidWindow(CWnd* pWnd)
{
	return (pWnd && ::IsWindow(pWnd->m_hWnd));
}

template<class TYPE>
inline void	SafeDelete( TYPE *& ptr )	//申明为指针的引用
{
	if( ptr ) 
	{
		delete ptr;
		ptr = NULL;
	}
};

inline BOOL  DoTabKey(CWnd *pWnd)
{
	UINT uStyleEx;
 	if(!pWnd) return FALSE;
 	CWnd *pParent = pWnd->GetParent();
 	while(pParent != NULL) 
 	{
 		uStyleEx =pParent->GetExStyle();
 		if(uStyleEx & WS_EX_CONTROLPARENT) 
 		{
 			pParent = pParent->GetParent();
 			continue;
 		}
 		CWnd *pNext = pParent->GetNextDlgTabItem(pWnd);
 		if(pNext) pNext->SetFocus(); 
 		return TRUE;
 	}
 	return FALSE;
 }

 static inline COLORREF GetThemeColor(int nIndex)
 {
 	COLORREF clRet = ThemeManager::Instance().GetColor(nIndex);
 	return clRet;
 }

//****************************************************************************
//****************** CustomEdit class *******************************************


////////////////////////////////////////////////////////////////////////////////
//==============================================================================
//		Implementation
//==============================================================================
////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC( CustomEdit, CEdit )

//////////////////////////////////////////////////////////////////////
////////////////////// 静态成员 //////////////////////////////////////
WNDPROC CustomEdit::lpfnSuperEdit = NULL;
/************************************************** *****************************
 *------------------------------------------------------------------------------
 *		Constructors & destructor
 *------------------------------------------------------------------------------
 ******************************************************************************/

CustomEdit::CustomEdit()
{
	m_ctlBkBrush.CreateSolidBrush(GetThemeColor(ThemeColor_CustomEditBk));
	m_dwHighLightText = GetThemeColor( ThemeColor_CustomEditHighLightText );
	m_dwNormalText = GetThemeColor( ThemeColor_CustomEditNorText );
	m_dwDisableText = GetThemeColor( ThemeColor_CustomEditGrayText );
	m_dwNORActiveBorder = GetThemeColor(ThemeColor_CustomEditTopLeftBoard);
	m_dwNORInActiveBorder = GetThemeColor(ThemeColor_CustomEditBottomRightBoard);
	m_dwDISActiveBorder = GetThemeColor(ThemeColor_CustomEditTopLeftBoardGray);
	m_dwDISInActiveBorder = GetThemeColor(ThemeColor_CustomEditBottomRightBoardGray);

	m_dwStyle		 = 0;
	m_fAutoDelete	 = FALSE; //自动删除标志
	m_bIsKeyUp		 = FALSE;
	m_pVScrollBar	 = NULL;
	m_pHScrollBar	 = NULL;
	m_nVScrollWidth  = GetSystemMetrics(SM_CXVSCROLL);
	m_nHScrollHeight = GetSystemMetrics(SM_CYHSCROLL);
	m_bBeginMove	 = FALSE;

	HBITMAP	hBmp = NULL;

	BITMAP bmInfo;
    memset( &bmInfo, 0, sizeof( BITMAP ) );
	if(hBmp)
		::GetObject(hBmp,sizeof(bmInfo),&bmInfo);
	m_szTopLeftBmp.cx  = bmInfo.bmWidth;
	m_szTopLeftBmp.cy  = bmInfo.bmHeight;
}

/*******************************************************************************
 *------------------------------------------------------------------------------
 *		Out-of-line functions
 *------------------------------------------------------------------------------
 ******************************************************************************/
// 注册窗口类
BOOL CustomEdit::RegisterControlClass()
{
	HINSTANCE	hInst = AfxGetInstanceHandle();
	return	RegisterControlClass( hInst );
}

BOOL CustomEdit::RegisterControlClass( HINSTANCE	hInst )
{
	if (hInst == NULL)		hInst = AfxGetInstanceHandle();

	WNDCLASS wcls;
	// check to see if class already registered
	static const TCHAR szClass[] = _T(CustomEditClass);
	if ( ::GetClassInfo( hInst, szClass, &wcls ) ){
		// name already registered - ok if it was us
		return ( wcls.lpfnWndProc == ( WNDPROC )CustomEdit::WndProcHook );
	}

	// Use standard "button" control as a template.
	VERIFY( ::GetClassInfo( NULL, _T("edit"), &wcls ) );
	lpfnSuperEdit = wcls.lpfnWndProc;

	// set new values
	wcls.lpfnWndProc = CustomEdit::WndProcHook;
	wcls.hInstance = hInst;
	wcls.lpszClassName = szClass;
	if( AfxRegisterClass( &wcls ) == 0 )
		return	FALSE;
	return TRUE;
}
////////////////////////////////////crcreacr/////////////////////////////////////////
// Special create hooks

LRESULT CALLBACK EXPORT
CustomEdit::WndProcHook( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	CustomEdit* pWnd = ( CustomEdit* )FromHandlePermanent( hWnd );

	if( !pWnd )
	{
		pWnd = new CustomEdit;
		pWnd->Attach( hWnd );
		pWnd->m_fAutoDelete = TRUE;
	}

	pWnd->m_pfnSuper = CustomEdit::lpfnSuperEdit;

	return ::CallWindowProc( AfxWndProc, hWnd, msg, wParam, lParam );
}

CustomEdit::~CustomEdit()
{
	m_ctlBkBrush.DeleteObject();
	m_textFont.DeleteObject();
	m_dwStyle = 0;
	int index = 0;
	SafeDelete(m_pVScrollBar);
	SafeDelete(m_pHScrollBar);
}

BEGIN_MESSAGE_MAP(CustomEdit, CEdit)
	//{{AFX_MSG_MAP(CustomEdit)
	ON_WM_NCPAINT()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
	ON_WM_NCCALCSIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CustomEdit message handlers

BOOL CustomEdit::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN)
	{
		BOOL b = GetKeyState(VK_CONTROL) & 0x80;
		if(b && (pMsg->wParam=='a' ||pMsg->wParam=='A'))
		{
			SetSel(0,-1);
			return TRUE;
		}

		if (!(GetStyle() & WS_TABSTOP) && pMsg->wParam == VK_TAB)
		{
			// 如果没有TabStop属性，则允许插入Tab字符
			CString str;
			GetWindowText(str);
			int nStart,nEnd;
			GetSel(nStart,nEnd);
			if(nStart!=nEnd)
			{
				str=str.Left(nStart)+str.Mid(nEnd);
			}
			str.Insert(nStart,_T("\t"));
			SetWindowText(str);
			SetSel(nStart+1,nStart+1);
			CWnd * pParent = GetParent();
			if (pParent)
			{
				pParent->SendMessage(EN_CHANGE, GetDlgCtrlID(), 0);
			}
			return TRUE;
		}
	}
	if(pMsg->message==WM_RBUTTONDOWN)
	{
		//return TRUE;
	}
	if (pMsg->message == WM_MOUSEWHEEL)
	{
		CRect rt;
		GetWindowRect(&rt);
		if (!rt.PtInRect(pMsg->pt))
		{
			// 鼠标在Edit以外不处理
			return FALSE;
		}
 		SendMessage(WM_MOUSEWHEEL, pMsg->wParam, pMsg->lParam);
 		return TRUE;
	}

	return CEdit::PreTranslateMessage(pMsg);
}

void CustomEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CEdit::OnChar(nChar, nRepCnt, nFlags);
	AdjustScrollPos();
}

void CustomEdit::OnNcPaint() 
{
	DWORD dwStyle = GetStyle();
	DWORD dwStyleEx = GetExStyle();
	
	if(((dwStyle & WS_BORDER) == WS_BORDER)
	|| ((dwStyleEx&WS_EX_STATICEDGE) ==WS_EX_STATICEDGE))
	{
		CDC* pDC = GetWindowDC();
		CRect rtClient;
		GetClientRect(&rtClient);
		CRect rtWindow;
		GetWindowRect(&rtWindow);
		rtWindow.OffsetRect(-rtWindow.left, -rtWindow.top);
		m_rtClient = rtWindow;
		ReleaseDC(pDC);
		pDC = NULL;
		return;
	}

	CDC* pDC = GetWindowDC();
	CRect rect;
	GetWindowRect(&rect);
	rect.OffsetRect(-rect.left, -rect.top);
	m_rtClient = rect;

	DWORD DlgEdgeColor[2];
	if( IsWindowEnabled() )
	{
		DlgEdgeColor[0] = m_dwNORActiveBorder;
		DlgEdgeColor[1] = m_dwNORInActiveBorder;
	}
	else
	{
		DlgEdgeColor[0] = m_dwDISActiveBorder;
		DlgEdgeColor[1] = m_dwDISInActiveBorder;
	}

	pDC->Draw3dRect(rect,DlgEdgeColor[0],DlgEdgeColor[1]);
	rect.InflateRect(-1,-1);
	COLORREF clBack = GetThemeColor(ThemeColor_CustomEditBk);

	LOGBRUSH lbr;
	clBack = m_ctlBkBrush.GetLogBrush(&lbr);
	clBack = lbr.lbColor;

	pDC->Draw3dRect(rect,clBack,clBack);

	ReleaseDC(pDC);
}

HBRUSH CustomEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	HBRUSH hBkBrush = NULL;
	BOOL fIsFoucs = (GetFocus() == this);
	switch( nCtlColor ) 
	{
	case CTLCOLOR_EDIT:
		{
			DWORD dwStyle = GetStyle();
			if( !(dwStyle & ES_READONLY))
			{
				if( fIsFoucs )
				{
					pDC->SetTextColor( m_dwHighLightText);
				}
				else
				{
					pDC->SetTextColor( m_dwNormalText);
				}
				COLORREF clBKColor;
				if(IsWindowEnabled())
				{
					if(GetFocus() == this)
					{
						clBKColor = GetThemeColor(ThemeColor_CustomEditBkFocus);
					}
					else
					{
						clBKColor = GetThemeColor(ThemeColor_CustomEditBk);
					}
				}
				else
					clBKColor = GetThemeColor(ThemeColor_CustomEditGray);
				pDC->SetBkColor(clBKColor);
				if(m_ctlBkBrush.m_hObject)
					m_ctlBkBrush.DeleteObject();
				m_ctlBkBrush.CreateSolidBrush(clBKColor);
				hBkBrush = (HBRUSH)m_ctlBkBrush;
			}
		}
		break;
	case CTLCOLOR_STATIC:
		{
			pDC->SetTextColor( m_dwDisableText);
			pDC->SetBkColor(GetThemeColor(ThemeColor_CustomEditGray));

			if(m_ctlBkBrush.m_hObject)
				m_ctlBkBrush.DeleteObject();

			m_ctlBkBrush.CreateSolidBrush(GetThemeColor(ThemeColor_CustomEditGray));
			hBkBrush = (HBRUSH)m_ctlBkBrush;
		}
		break;
	}
	return hBkBrush;
}

void CustomEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);

	CWnd * pParent = GetParent();
	if (pParent)
	{
		pParent->SendMessage(WM_SYEDIT_MSG_END_EDIT, GetDlgCtrlID(), 0);
	}

	if(IsWindow(GetSafeHwnd())){
		Invalidate();
	}
}

void CustomEdit::OnSetFocus(CWnd* pOldWnd) 
{
	CEdit::OnSetFocus(pOldWnd);
	InvalidateRect( m_rtClient );
}

void CustomEdit::SetBkColor(COLORREF dwBkColor){
	CBrush brush;
	brush.CreateSolidBrush(dwBkColor);
	SetBKBrush(&brush);
	brush.DeleteObject();
}

void CustomEdit::SetBKBrush( CBrush* pBrush )
{
	if( pBrush == NULL )
		return;

	if( m_ctlBkBrush.m_hObject!=NULL )
		m_ctlBkBrush.DeleteObject();
	
	LOGBRUSH lgBsh;
	pBrush->GetLogBrush( &lgBsh );
	
	m_ctlBkBrush.CreateBrushIndirect( &lgBsh );

	if( ::IsWindow( m_hWnd ) )
	{
		Invalidate();
		UpdateWindow();
	}
}

void CustomEdit::SetBorderColor(COLORREF dwTopLeft, COLORREF dwBottomRight)
{
	m_dwNORActiveBorder = dwTopLeft;
	m_dwNORInActiveBorder = dwBottomRight;

	Invalidate();
}

void CustomEdit::SetTextColor(COLORREF dwNormal,COLORREF dwDisable,COLORREF dwHilight)
{
	m_dwNormalText = dwNormal;
	m_dwDisableText = dwDisable;
	m_dwHighLightText = dwHilight;

	Invalidate();
}

void CustomEdit::SetAutoDeleteFlag( BOOL bIsAutoDel )
{
	m_fAutoDelete = bIsAutoDel;
}

void CustomEdit::PostNcDestroy()
{ 
	if( m_fAutoDelete )
	{
		delete this;
	}
}
/************************************************************************/
/*   Create创建带滚动条小结:add by caiguosen
	 
     dwStylle: 右垂直滚动条: WS_CHILD|WS_VISIBLE|ES_MULTILINE|WS_VSCROLL 
               左垂直滚动条: WS_CHILD|WS_VISIBLE|ES_MULTILINE|WS_VSCROLL|WS_EX_LEFTSCROLLBAR  
			     水平滚动条: WS_CHILD|WS_VISIBLE|ES_MULTILINE|WS_HSCROLL|ES_WANTRETURN
		 水平和右垂直滚动条: WS_CHILD|WS_VISIBLE|ES_MULTILINE|WS_HSCROLL|WS_VSCROLL|ES_WANTRETURN
		 水平和左垂直滚动条: WS_CHILD|WS_VISIBLE|ES_MULTILINE|WS_HSCROLL|WS_VSCROLL|WS_EX_LEFTSCROLLBAR|ES_WANTRETURN

      以上 ES_WANTRETURN属性均为非必要属性，其他必需                                    */
/************************************************************************/
 
BOOL CustomEdit::Create/*No.1*/( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID )
{
	return Create(_T(""), dwStyle, &rect, pParentWnd, nID);
}

BOOL CustomEdit::Create/*No.2*/( LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
		DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
		UINT nID, CCreateContext* /*pContext = NULL*/)
{   // the Create No.2 will call the Create No.3
	if(m_dwStyle == 0)
		m_dwStyle = dwStyle; 
	BOOL rt = Create( lpszWindowName, dwStyle, ( LPCRECT )&rect, pParentWnd, nID );
	return rt;
}

BOOL CustomEdit::Create/*No.3*/( LPCTSTR lpszCaption, DWORD dwStyle,LPCRECT pRect, CWnd* pParentWnd ,UINT nID)
{
	if(m_dwStyle == 0) m_dwStyle = dwStyle;

	WNDCLASS	wcls;
	HINSTANCE	hInst = AfxGetInstanceHandle();
	if (!::GetClassInfo(hInst, _T(CustomEditClass), &wcls))
	{
		RegisterControlClass(hInst);
	}

	BOOL bRet = CWnd::CreateEx( WS_EX_STATICEDGE ,  _T(CustomEditClass) , lpszCaption,dwStyle ,
			( const RECT& )*( pRect ? pRect : CRect( 0,0,0,0 ) ), pParentWnd, nID );

	if((m_dwStyle &WS_EX_LEFTSCROLLBAR) == WS_EX_LEFTSCROLLBAR)
	{
		ModifyStyle(WS_EX_LEFTSCROLLBAR, 0);
		ModifyStyleEx(0, WS_EX_LEFTSCROLLBAR);
	}
	LOGFONT lgFont = ThemeManager::Instance().GetFont(ThemeFont_CustomEditCtr);
	m_textFont.CreateFontIndirect(&lgFont);
	CreateScroolBar();
	ModifyTextRect();
	return bRet;
}

BOOL CustomEdit::SubclassEdit( UINT nID, CWnd* pParent )
{
	CWnd*	pDlg = CWnd::FromHandle( pParent->m_hWnd );
	ASSERT( pDlg );

	CEdit* pEdit = ( CEdit* )pDlg->GetDlgItem( nID );
	ASSERT( pEdit );
	m_dwStyle = pEdit->GetStyle();
	
	CustomEdit* pWnd = ( CustomEdit* )FromHandlePermanent( pEdit->m_hWnd );
	if( pWnd )
	{   
		pWnd->CreateScroolBar();
		pWnd->ModifyTextRect();
		return	FALSE;
	}

	return SubclassDlgItem( nID, pParent );
}

BOOL CustomEdit::AutoLoad( UINT nID, CWnd* pParent, BOOL fAutoDelete )
{
	if( SubclassEdit( nID , pParent ) == FALSE )
		return FALSE;
	
	ModifyTextRect();
	CreateScroolBar();

	m_fAutoDelete = fAutoDelete;
	return TRUE;
}

#ifdef _DEBUG
void CustomEdit::AssertValid() const
{CEdit::AssertValid(); }

void CustomEdit::Dump( CDumpContext& dc ) const
{CEdit::Dump( dc );}
#endif

//****************** CustomEdit class *******************************************
//****************************************************************************

int CustomEdit::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;
	if(m_textFont.m_hObject)
		SetFont(&m_textFont);

	return 0;
}

void CustomEdit::ModifyTextRect()
{
	if(m_textFont.m_hObject)
		SetFont(&m_textFont);
	
	DWORD dwStyle = GetStyle();
	if(!(dwStyle & ES_MULTILINE))
	{
		CFont* tempFont = NULL;
		if(m_textFont.m_hObject)
			tempFont = &m_textFont;

		LOGFONT logFont; tempFont->GetLogFont(&logFont);
		CRect rc1, rc2; int h1,h2;
		GetClientRect(&rc1);
		GetRect(&rc2); 
		h1 = rc1.Height();
		h2 = logFont.lfHeight+2; 
		rc2.top  = rc1.top + (h1 - h2)/2;
		rc2.bottom = rc2.top + h2;
		if (rc2.Height() > rc1.Height())
		{
			rc2 = rc1;
		}
		SetRect(rc2);
	}
	else 
	{
		DWORD dwExStyle	= GetExStyle();
		DWORD dwStyleV = ES_MULTILINE|WS_VSCROLL;//垂直滚动条
		DWORD dwStyleH = ES_MULTILINE|WS_HSCROLL;//水平滚动条
		DWORD dwExStyleLeftV    = WS_EX_LEFTSCROLLBAR;    //垂直左滚动条 
		CRect rtClient;
		CRect rt2;
		GetClientRect(&rtClient);
		rtClient.DeflateRect(2,2,2,2);
		CRect rtEdit = rtClient;
		if((dwStyle &dwStyleV) == dwStyleV 
			&& (dwStyle &dwStyleH) == dwStyleH)
		{//既有垂直又有水平
			if((dwExStyle & dwExStyleLeftV) == dwExStyleLeftV)
			{
				rtEdit.left   = rtEdit.left + m_nVScrollWidth+1;
				rtEdit.right  = rtEdit.right -1;
				rtEdit.bottom = rtEdit.bottom - m_nHScrollHeight;
			}
			else
			{
				rtEdit.left   = rtEdit.left +1;
				rtEdit.right  = rtEdit.right - m_nVScrollWidth+1;
				rtEdit.bottom = rtEdit.bottom - m_nHScrollHeight+1;
			}
		}
		else if((dwStyle &dwStyleV) == dwStyleV )
		{//只有垂直
			if((dwExStyle & dwExStyleLeftV) == dwExStyleLeftV)
			{
				rtEdit.left   = rtEdit.left + m_nVScrollWidth+1;
			}
			else
			{
				rtEdit.right  = rtEdit.right - m_nVScrollWidth+1;
			}
		}
		else if((dwStyle &dwStyleH) == dwStyleH)
		{//只有水平
			rtEdit.bottom = rtEdit.bottom - m_nHScrollHeight+1;//
		}
		SetRect(&rtEdit);
	}
}

void CustomEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(nChar == VK_DELETE)
	{
		AdjustScrollPos();
	}
	if(nChar == VK_RETURN)
	{
		DWORD dwStyle = GetStyle();
		if( ( dwStyle & ES_MULTILINE ) != ES_MULTILINE && 
			(dwStyle & ES_WANTRETURN) != ES_WANTRETURN) 
			DoTabKey(this);
	}
	else 	
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CustomEdit::PreSubclassWindow() 
{
	DWORD dwExStyle = GetExStyle();
	ModifyStyle(m_hWnd,WS_BORDER,0,1);
	if(dwExStyle&WS_EX_CLIENTEDGE)
		ModifyStyleEx(m_hWnd,WS_EX_CLIENTEDGE,WS_EX_STATICEDGE,1);
	CEdit::PreSubclassWindow();
}

void CustomEdit::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CEdit::OnVScroll(nSBCode, nPos, NULL);
	if( m_hWnd!=::GetFocus() ) SetFocus();
	SCROLLINFO si;
	GetScrollInfo(SB_VERT,&si,SIF_ALL);
	si.nMax++;
	if(m_pVScrollBar && ::IsWindow(m_pVScrollBar->m_hWnd))
		m_pVScrollBar->SetScrollInfo(&si);
	Invalidate(FALSE);
	UpdateWindow();
}

void CustomEdit::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CEdit::OnHScroll(nSBCode, nPos, NULL);
	if( m_hWnd!=::GetFocus() ) SetFocus();
	SCROLLINFO si;
	GetScrollInfo(SB_HORZ,&si,SIF_ALL);
	si.nMax++;
	if(m_pHScrollBar && ::IsWindow(m_pHScrollBar->m_hWnd))
		m_pHScrollBar->SetScrollInfo(&si);
	Invalidate(FALSE);
	UpdateWindow();
}

void CustomEdit::OnMouseMove(UINT nFlags, CPoint point) 
{
	SetCursorArrowOnScrollBar(point);
	if(m_bBeginMove == FALSE) 
		return;
	CRect rtClient;
	GetClientRect(&rtClient);
	int nUpDwon= 0, nLeftRight =0;
	if(point.y >rtClient.bottom )
		nUpDwon = 1;
	else if(point.y <rtClient.top )
		nUpDwon = -1;
	if(point.x > rtClient.right)
		nLeftRight = 1;
	else if(point.x <rtClient.left)
		nLeftRight = -1;
	if(nUpDwon !=0)
		OnVScroll(0,0,NULL);
	if(nLeftRight !=0)
		OnHScroll(0,0,NULL);
	CEdit::OnMouseMove(nFlags, point);
}

BOOL CustomEdit::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if(IsValidWindow(m_pVScrollBar))
		OnVScroll(0,0,NULL);
	BOOL bRet =  CEdit::OnMouseWheel(nFlags, zDelta, pt);
	if (FALSE==IsValidWindow(this))
		return TRUE;
	return bRet;
} 

void CustomEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetCursorArrowOnScrollBar(point);
	SetFocus();
	m_bBeginMove = TRUE;
	SetCapture();
	CEdit::OnLButtonDown(nFlags, point);
}

void CustomEdit::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();
	m_bBeginMove = FALSE;
	CEdit::OnLButtonUp(nFlags, point);
}

void CustomEdit::OnDestroy() 
{
	m_textFont.DeleteObject();
	CEdit::OnDestroy();
	if(IsValidWindow(m_pVScrollBar))
		m_pVScrollBar->DestroyWindow();
	if(IsValidWindow(m_pHScrollBar))
		m_pHScrollBar->DestroyWindow();
}

BOOL CustomEdit::CreateScroolBar(BOOL bAdjustPos /*= FALSE*/)
{
	DWORD dwStyle	= GetStyle();
	DWORD dwExStyle	= GetExStyle();
	DWORD dwStyleV = ES_MULTILINE|WS_VSCROLL;//垂直滚动条
	DWORD dwStyleH = ES_MULTILINE|WS_HSCROLL;//水平滚动条
	DWORD dwExStyleLeftV    = WS_EX_LEFTSCROLLBAR;    //垂直左滚动条 
	CRect rtClient;
	GetWindowRect(&rtClient);
	ScreenToClient(&rtClient);
	rtClient.OffsetRect(-rtClient.TopLeft());
	CRect rtVScrollBar  = rtClient, rtHScrollBar  = rtClient, rtStub = rtClient;
	BOOL bHScrollBar = FALSE, bVScrollBar = FALSE, bStub= FALSE;
	if((dwStyle &dwStyleV) == dwStyleV 
		&& (dwStyle &dwStyleH) == dwStyleH)
	{//既有垂直又有水平		
		bHScrollBar = TRUE;
		bVScrollBar = TRUE;
		bStub		= TRUE;
		ShowScrollBar(SB_BOTH,FALSE);
		ModifyStyle(0, WS_VSCROLL|WS_HSCROLL);
		if((dwExStyle & dwExStyleLeftV) == dwExStyleLeftV)
		{//左垂直
			rtVScrollBar.right  = rtVScrollBar.left + m_nVScrollWidth;
			rtHScrollBar.top	= rtHScrollBar.bottom - m_nHScrollHeight;

			rtStub.right = rtStub.left + m_nVScrollWidth    - m_szTopLeftBmp.cx;
			rtStub.top   = rtStub.bottom - m_nHScrollHeight - m_szTopLeftBmp.cy-2;

			rtVScrollBar.DeflateRect(0, 0, 0, 2*m_szTopLeftBmp.cy + m_nHScrollHeight-1);
			rtHScrollBar.OffsetRect(0, -2*m_szTopLeftBmp.cy); 
			rtHScrollBar.DeflateRect(m_nVScrollWidth-1, 0, 2*m_szTopLeftBmp.cy, 0);
		}
		else
		{//右垂直
			rtVScrollBar.left   = rtVScrollBar.right  - m_nVScrollWidth;
			rtVScrollBar.bottom = rtVScrollBar.bottom - m_nHScrollHeight;
			
			rtHScrollBar.right	= rtHScrollBar.right  - m_nVScrollWidth;
			rtHScrollBar.top	= rtHScrollBar.bottom - m_nHScrollHeight;
		
			rtStub.left  = rtStub.right - m_nVScrollWidth - 2*m_szTopLeftBmp.cx;
			rtStub.top   = rtStub.bottom - m_nHScrollHeight - 2*m_szTopLeftBmp.cy;
			
			rtVScrollBar.OffsetRect(-2*m_szTopLeftBmp.cx, 0);
			rtVScrollBar.DeflateRect(0,0, 0, 2*m_szTopLeftBmp.cy-1);
			rtHScrollBar.OffsetRect(0, -2*m_szTopLeftBmp.cy);
			rtHScrollBar.DeflateRect(1,0, 2*m_szTopLeftBmp.cx-1, 0);
		}
	}
	else if((dwStyle &dwStyleV) == dwStyleV )
	{//只有垂直
		bVScrollBar = TRUE;
		ShowScrollBar(SB_BOTH,FALSE);
		ModifyStyle(0, WS_VSCROLL);
		if((dwExStyle & dwExStyleLeftV) == dwExStyleLeftV)
		{
			rtVScrollBar.right  = rtVScrollBar.left + m_nVScrollWidth;
			rtVScrollBar.DeflateRect(0,0,0, 2*m_szTopLeftBmp.cy);
		}
		else
		{
			rtVScrollBar.right = rtVScrollBar.right -m_szTopLeftBmp.cx;
			rtVScrollBar.left  = rtVScrollBar.right - m_nVScrollWidth;
			rtVScrollBar.OffsetRect(-m_szTopLeftBmp.cx, 0);
			rtVScrollBar.DeflateRect(0,0,0, 2*m_szTopLeftBmp.cy);
		}
		
	}
	else if((dwStyle &dwStyleH) == dwStyleH)
	{//只有水平
		bHScrollBar = TRUE;
		ShowScrollBar(SB_BOTH,FALSE);
		ModifyStyle(0, WS_HSCROLL);
		rtHScrollBar.top	= rtHScrollBar.bottom - m_nHScrollHeight;
		rtHScrollBar.OffsetRect(0, -2*m_szTopLeftBmp.cy);
		rtHScrollBar.DeflateRect(0, 0, 2*m_szTopLeftBmp.cx, 0);
	}
	if(bVScrollBar)
	{
		if(!bAdjustPos)
		{
			m_pVScrollBar = new CustomScrollBar;
			m_pVScrollBar->SetAutoHide(FALSE);
			m_pVScrollBar->Create(WS_CHILD|WS_VISIBLE|SBS_VERT|SBS_RIGHTALIGN,this,rtVScrollBar,11111); 
			m_pVScrollBar->SetWindowPos(&CWnd::wndTop,0,0,0,0, SWP_NOSIZE | SWP_NOMOVE|SWP_NOACTIVATE);
		}
		else
			m_pVScrollBar->MoveWindow(&rtVScrollBar);
		SCROLLINFO si;
		GetScrollInfo(SB_VERT,&si,SIF_ALL);
		if(si.nMax> (int)si.nPage)
			m_pVScrollBar->SetScrollInfo(&si);
		else
			m_pVScrollBar->EnableWindow(FALSE);
	}
	if(bHScrollBar)
	{
		if(!bAdjustPos)
		{
			m_pHScrollBar = new CustomScrollBar;
			m_pHScrollBar->SetAutoHide(FALSE);
			m_pHScrollBar->Create(WS_CHILD|WS_VISIBLE|SBS_HORZ|SBS_RIGHTALIGN,this,rtHScrollBar,11112); 
			m_pHScrollBar->SetWindowPos(&CWnd::wndTop,0,0,0,0, SWP_NOSIZE | SWP_NOMOVE|SWP_NOACTIVATE);
		}
		else
			m_pHScrollBar->MoveWindow(&rtHScrollBar);
		SCROLLINFO si;
		GetScrollInfo(SB_HORZ,&si,SIF_ALL);
		if(si.nMax> (int)si.nPage)
			m_pHScrollBar->SetScrollInfo(&si);
		else
			m_pHScrollBar->EnableWindow(FALSE);
	}
	return 1L;
}

LRESULT CustomEdit::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	LRESULT lr;
	if(message == WM_PASTE && IsClipboardFormatAvailable(CF_TEXT))
	{
		DWORD dwStyle = GetStyle();
		if((dwStyle & ES_MULTILINE) != ES_MULTILINE)
		{//对于多行的Edit不应该把回车替换成空格
			OpenClipboard();
			CString strTemp;
			HGLOBAL hglb = GetClipboardData(CF_UNICODETEXT);
			if(hglb != NULL)
			{
				strTemp = CString((LPWSTR)GlobalLock(hglb));
			}
			else
			{
				hglb = GetClipboardData(CF_TEXT);
				if(hglb != NULL)
				{
					LPSTR lpstr = (LPSTR)GlobalLock(hglb); 
					strTemp = CString(lpstr);
				}
			}
			if(hglb)
				GlobalUnlock(hglb);

			if (!strTemp.IsEmpty()) 
			{
				strTemp.Replace(10,32);
				strTemp.Replace(13,32);
				ReplaceSel(strTemp, TRUE);
			}

			CloseClipboard(); 
			return 0;
		}
	}
	if(message == WM_PASTE || message == WM_CUT)
	{
		DWORD dwStyle = GetStyle();
		if((dwStyle & ES_MULTILINE) == ES_MULTILINE)
		{
			lr = CEdit::DefWindowProc(message, wParam, lParam);
			AdjustScrollPos();
			return lr;
		}
	}
	lr = CEdit::DefWindowProc(message, wParam, lParam);
	if (message == WM_SETTEXT)
	{
		CString strText;
		GetWindowText(strText);
		DWORD dwStyle = GetStyle();
		if((dwStyle & ES_MULTILINE) == ES_MULTILINE)
		{
			AdjustScrollPos();
		}
	}
	else if (message == EM_SCROLLCARET)
	{
		AdjustScrollPos();
	}
	return lr;
}

void CustomEdit::AdjustScrollPos()
{
	if(m_pVScrollBar && ::IsWindow(m_pVScrollBar->m_hWnd))
	{
		SCROLLINFO si;
		GetScrollInfo(SB_VERT,&si,SIF_ALL);
		si.nMax++;
		if(::IsWindowEnabled(m_pVScrollBar->m_hWnd))
		{
			if(si.nMax < (int)si.nPage)
				m_pVScrollBar->EnableWindow(FALSE);
		}
		else
		{
			if(si.nMax >= (int)si.nPage)
				m_pVScrollBar->EnableWindow(TRUE);
		}
		if(m_pVScrollBar && ::IsWindow(m_pVScrollBar->m_hWnd))
			m_pVScrollBar->SetScrollInfo(&si);
	}
	if(m_pHScrollBar && ::IsWindow(m_pHScrollBar->m_hWnd))
	{
		SCROLLINFO si;
		GetScrollInfo(SB_HORZ,&si,SIF_ALL);
		si.nMax++;
		if(::IsWindowEnabled(m_pHScrollBar->m_hWnd))
		{
			if(si.nMax < (int)si.nPage)
				m_pHScrollBar->EnableWindow(FALSE);
		}
		else
		{
			if(si.nMax >= (int)si.nPage)
				m_pHScrollBar->EnableWindow(TRUE);
		}
		if(m_pHScrollBar && ::IsWindow(m_pHScrollBar->m_hWnd))
			m_pHScrollBar->SetScrollInfo(&si);
	}
	Invalidate(FALSE);
}

void CustomEdit::OnSize(UINT nType, int cx, int cy) 
{
	CEdit::OnSize(nType, cx, cy);
	if((m_pVScrollBar && ::IsWindow(m_pVScrollBar->m_hWnd))
		||(m_pHScrollBar && ::IsWindow(m_pHScrollBar->m_hWnd)))
	{
		CreateScroolBar(TRUE);
		ModifyTextRect();
	}
}

void CustomEdit::SetCursorArrowOnScrollBar(CPoint point)
{//为了鼠标在ScrollBar上变成箭头形状
	if(m_pVScrollBar && ::IsWindow(m_pVScrollBar->m_hWnd) 
		&& !m_pVScrollBar->IsWindowEnabled())
	{
		CRect rtVScrollBar;
		m_pVScrollBar->GetWindowRect(&rtVScrollBar);
		ScreenToClient(&rtVScrollBar);
		if(rtVScrollBar.PtInRect(point))
			SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
	if(m_pHScrollBar && ::IsWindow(m_pHScrollBar->m_hWnd) 
		&& !m_pHScrollBar->IsWindowEnabled() )
	{
		CRect rtHScrollBar;
		m_pHScrollBar->GetWindowRect(&rtHScrollBar);
		ScreenToClient(&rtHScrollBar);
		if(rtHScrollBar.PtInRect(point))
			SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
}

void CustomEdit::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	CEdit::OnNcCalcSize(bCalcValidRects, lpncsp);
	DWORD dwStyle = GetStyle();
	DWORD dwStyleEx = GetExStyle();
	if(((dwStyle & WS_BORDER) == WS_BORDER)
		|| ((dwStyleEx&WS_EX_STATICEDGE) ==WS_EX_STATICEDGE))
	{
		CRect rtTemp = lpncsp->rgrc[0];
		rtTemp.DeflateRect(m_szTopLeftBmp.cx-1, m_szTopLeftBmp.cy-1);
		lpncsp->rgrc[0] = rtTemp;
	}
}

CFont* CustomEdit::SetFont(CFont* pFont, BOOL bRedraw /* = TRUE */)
{
	if(!pFont)
		return NULL;
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	if(m_textFont.m_hObject)
		m_textFont.DeleteObject();
	m_textFont.CreateFontIndirect(&lf);
	CEdit::SetFont(pFont, bRedraw);
	if (bRedraw)
	{
		UpdateWindow();
		Invalidate();
	}
	return &m_textFont;
}
