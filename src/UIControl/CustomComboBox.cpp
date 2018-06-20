
#include "stdafx.h"
#include "UIControl/CustomDropDownWnd.h"
#include "UIControl/CustomComboBox.h"
#include "ThemeManager.h"
#include <imm.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma comment(lib, "imm32.lib")
#define Custom_szComboBoxClass				"Custom_ComboBoxClass32"

//*********************************************************************
//************  CustomComboBox *******************************************
#define		IDC_DYCOMBOBOX_EDIT		20032
#define		IDC_DYCOMBOBOX_DROPWND		20033


WNDPROC CustomComboBox::lpfnSuperComboBox = NULL;

#define COMBO_TIMER_FLUSH_DROPBTN	1
#define COMBO_TIMER_CLEAR_IMEBUFFER	2

#define WM_FLUSH_WND (WM_USER + 101)

IMPLEMENT_DYNAMIC( CustomComboBox, CWnd )

CustomComboBox::CustomComboBox()
	: m_pComboBox(NULL)
	, m_pDropDownWnd(NULL)
	, m_pDropDownBtnU(nullptr)
	, m_pDropDownBtnD(nullptr)
	, m_pDropDownBtnF(nullptr)
	, m_pDropDownBtnX(nullptr)
	, m_nDropDownBtnWidth(20)
	, m_nDropDownBtnHeight(20)
{
	m_pDropDownWnd	= new CustomDropDownWnd;	
	m_pComboBox = new CustomEdit;

	m_nCtrlHeight = GetSystemMetrics(SM_CYSIZE) + 2;
	if(m_nCtrlHeight > 22)
		m_nCtrlHeight = 22;

	m_nCtrlWidth = GetSystemMetrics(SM_CXSIZE);
	if (m_nCtrlWidth > 20)
		m_nCtrlWidth = 20;
	
	m_nWndHeight	= 100;

	m_bIsDropDown	= FALSE;

	m_nHit = -1;

	m_strCurSel		= _T("");
	m_nCurSel		= -1;
	m_strKeyDown	= _T("");

	m_bAutoDelete = FALSE;

	m_nStyle = 0;

	m_nDroppedWidth = 0;		//	Dropped window width
	m_nTextFormat = DT_VCENTER|DT_SINGLELINE|DT_LEFT;

	m_bEnableRunTimeList = FALSE;
	m_szTopLeftBmp = CSize(0, 0);

	m_szTopLeftBmp.cx = 2;
	m_szTopLeftBmp.cy = 2;

	m_szArrowBtn = CSize(15, 16);
	m_szArrowBtn.cx = m_szArrowBtn.cy = 20;

	m_pDropDownBtnU = ThemeManager::Instance().GetImage(ThemeImage_ComboxDropDownBtn_U);
	m_pDropDownBtnD = ThemeManager::Instance().GetImage(ThemeImage_ComboxDropDownBtn_D);
	m_pDropDownBtnF = ThemeManager::Instance().GetImage(ThemeImage_ComboxDropDownBtn_F);
	m_pDropDownBtnX = ThemeManager::Instance().GetImage(ThemeImage_ComboxDropDownBtn_X);

	if (m_pDropDownBtnX)
	{
		m_nDropDownBtnWidth = m_pDropDownBtnX->GetWidth();
		m_nDropDownBtnHeight = m_pDropDownBtnX->GetHeight();
	}

	m_clGray		= ThemeManager::Instance().GetColor(ThemeColor_CustomDropGreyBk);
	m_clText		= ThemeManager::Instance().GetColor(ThemeColor_CustomDropText);
	m_clGrayText	= ThemeManager::Instance().GetColor(ThemeColor_CustomDropGreyText);
	m_clSelectText	= ThemeManager::Instance().GetColor(ThemeColor_CustomDropSelectText);
}

CustomComboBox::~CustomComboBox()
{
	SAFE_DELETE(m_pDropDownWnd );
	SAFE_DELETE(m_pComboBox);
}

// 注册窗口类
BOOL CustomComboBox::RegisterControlClass()
{
	HINSTANCE hInst = AfxGetInstanceHandle();

	return	RegisterControlClass( hInst );
}

BOOL CustomComboBox::RegisterControlClass( HINSTANCE	hInst )
{
	WNDCLASS wcls;
	// check to see if class already registered
	static const TCHAR szClass[] = _T(Custom_szComboBoxClass);
	if ( ::GetClassInfo( hInst, szClass, &wcls ) )
	{
		// name already registered - ok if it was us
		return ( wcls.lpfnWndProc == ( WNDPROC )CustomComboBox::WndProcHook );
	}

	// Use standard "Static" control as a template.
	VERIFY( ::GetClassInfo( NULL, _T( "Static" ), &wcls ) );

	//should be NULL.
	lpfnSuperComboBox = NULL;

	// set new values
	wcls.lpfnWndProc = WndProcHook;
	wcls.hInstance = hInst;
	wcls.lpszClassName = szClass;
	wcls.hCursor = ::LoadCursor(NULL,IDC_ARROW);
	if( RegisterClass( &wcls ) == 0 )
		return	FALSE;
	return TRUE;
}
BOOL CustomComboBox::UnregisterControlClass(HINSTANCE hInst)
{
	if (hInst == NULL)	  hInst = AfxGetInstanceHandle();
	return UnregisterClass(_T(Custom_szComboBoxClass), hInst);
}

/////////////////////////////////////////////////////////////////////////////
// Special create hooks

LRESULT CALLBACK EXPORT
CustomComboBox::WndProcHook( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	CustomComboBox* pWnd = ( CustomComboBox* )FromHandlePermanent( hWnd );

	if( !pWnd )
	{
		// create new item and attach it
		pWnd = new CustomComboBox;
		pWnd->Attach( hWnd );
		pWnd->m_bAutoDelete = TRUE;
		pWnd->PreSubclassWindow();
	}

	// set up wndproc to AFX one, and call it
	pWnd->m_pfnSuper = CustomComboBox::lpfnSuperComboBox;
	::SetWindowLongPtr( hWnd, GWLP_WNDPROC, (LONG)(LONG_PTR)AfxWndProc );

// then call it for this first message
	return ::CallWindowProc( AfxWndProc, hWnd, msg, wParam, lParam );
}

BEGIN_MESSAGE_MAP(CustomComboBox, CWnd)
	//{{AFX_MSG_MAP(CustomComboBox)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_NCCALCSIZE()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	ON_WM_NCPAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_DROPWND_SELECT , OnDropWndSelect )
	ON_MESSAGE( WM_DROPWND_DISFOCUS , OnDropWndKillFocus )
	ON_MESSAGE( CB_SETCURSEL,OnSetCurSel)
	ON_MESSAGE( CB_GETCURSEL,OnGetCurSel)
	ON_MESSAGE( CB_SELECTSTRING,OnSelectString)
	ON_MESSAGE(WM_SETTEXT, OnMsgSetText)
	ON_MESSAGE(WM_FLUSH_WND, OnFlushWnd)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CustomComboBox message handlers

int CustomComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	ModifyStyle(0,WS_CLIPCHILDREN,0);
	DWORD dwStyle = GetStyle();
	CRect rtControl;
	GetWindowRect(&rtControl);

	CreateChildWindows(dwStyle,rtControl);
	CRect rtClient;	
	GetClientRect( &rtClient );

	m_nWndHeight = rtControl.Height()-m_nCtrlHeight;// - rtClient.Height();//下拉窗口高度。
	int nScale = m_nWndHeight/(m_pDropDownWnd->GetItemHeight());
	m_nWndHeight = nScale* (m_pDropDownWnd->GetItemHeight()) + 2;
	SetWindowPos( NULL , rtClient.left , rtClient.top , rtClient.Width() , rtClient.top + m_nCtrlHeight,  SWP_NOMOVE|SWP_NOZORDER );
	
	return 1;
}

void CustomComboBox::OnDestroy() 
{
	CWnd::OnDestroy();
	
	// TODO: Add your message handler code here
	if (m_pDropDownWnd && ::IsWindow(m_pDropDownWnd->m_hWnd))
	{
		m_pDropDownWnd->DestroyWindow();
	}
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		m_pComboBox->DestroyWindow();
	}
}


//create edit box and drop list box.
BOOL CustomComboBox::CreateChildWindows(DWORD dwStyle,CRect rtControl)
{
	BOOL ret = TRUE;
	if(!::IsWindow(m_hWnd))
		return FALSE;
	//initial style
	if(  ( dwStyle & CBS_DROPDOWNLIST ) == CBS_DROPDOWNLIST )
		m_nStyle = 1;//不带edit框的comboBox.
	else
		m_nStyle = 0;//带有edit框的comboBox.

	if(!m_pDropDownWnd)
		m_pDropDownWnd = new CustomDropDownWnd;
	if (!m_pComboBox)
	{
		m_pComboBox = new CustomEdit;
	}

	//create window
	CRect rtWnd;
	GetWindowRect(&rtWnd);
	if(rtWnd != rtControl)
	{
		m_nWndHeight = rtControl.Height()-rtWnd.Height();
		int nScale = m_nWndHeight/(m_pDropDownWnd->GetItemHeight());
		m_nWndHeight = nScale* (m_pDropDownWnd->GetItemHeight()) + 2;
	}
	CRect rtDropWnd;
	GetDropWndRect( rtDropWnd);

	if(!::IsWindow(m_pDropDownWnd->m_hWnd))
	{
		BOOL bResult = m_pDropDownWnd->Create(WS_POPUP,rtDropWnd,this,IDC_DYCOMBOBOX_DROPWND);
		m_pDropDownWnd->m_pMessageWnd = this;
		::SetWindowLongPtr(m_pDropDownWnd->m_hWnd, GWL_STYLE, GetWindowLongPtr(m_pDropDownWnd->m_hWnd, GWL_STYLE)| WS_OVERLAPPED);
		m_pDropDownWnd->SetWindowPos(&CWnd::wndTop,0,0,0,0, SWP_NOSIZE | SWP_NOMOVE|SWP_NOACTIVATE);

		ClientToScreen( &rtDropWnd );
		m_pDropDownWnd->MoveWindow( rtDropWnd.left, rtDropWnd.top, rtDropWnd.Width(), rtDropWnd.Height());
		m_pDropDownWnd->ShowWindow( SW_HIDE );

		CFont* pDefFont = GetFont();
		if (pDefFont == NULL && (pDefFont = GetParent()->GetFont()) == NULL)
			pDefFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
		if (pDefFont != NULL)
			m_pDropDownWnd->SetFont(pDefFont);		
	}

	if (m_nStyle == 1)
	{
		if (!IsWindow(m_pComboBox->GetSafeHwnd()))
		{
			//BOOL bResult = m_pEdit->Create();
		}
	}

	return TRUE;
}

void CustomComboBox::CalcClientRect(CRect& rtClient)
{
	DWORD dwStyle = GetStyle() & 0x0000000f;
	if (dwStyle != CBS_DROPDOWN && dwStyle != CBS_DROPDOWNLIST)
		return;

	int nHeight = m_nDropDownBtnHeight;
	if (nHeight > rtClient.Height())
		rtClient.bottom = rtClient.top + nHeight;
}

void CustomComboBox::OnPaint() 
{
	CPaintDC	dc(this);
	CRect		rtClient;
	GetClientRect(&rtClient);
	CalcClientRect(rtClient);

	CRect rtTemp(0,0,rtClient.Width(),rtClient.Height());
	CDC MemDC;
	MemDC.CreateCompatibleDC(&dc);
	CBitmap MemBitmap;
	MemBitmap.CreateCompatibleBitmap(&dc,rtTemp.Width(),rtTemp.Height());
	HBITMAP hOldBmp = (HBITMAP)MemDC.SelectObject(MemBitmap.m_hObject);

	CFont* pDefFont = GetFont();
	if (pDefFont == NULL && (pDefFont = GetParent()->GetFont()) == NULL)
		pDefFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	CFont* pOldFont = MemDC.SelectObject(pDefFont);

	// 判断是否为空刷子
	BOOL bFillSolid = TRUE;
	{
		HBRUSH hBKBrush = (HBRUSH)(GetParent()->SendMessage(WM_CTLCOLORSTATIC, 
			(WPARAM)(MemDC.m_hDC), (LPARAM)m_hWnd));
		if (hBKBrush == NULL)
		{
			MemDC.SetBkMode(TRANSPARENT);
			MemDC.SetBkColor(ThemeManager::Instance().GetColor(ThemeColor_CustomDropNorBk));
			MemDC.SetTextColor(m_clText);
			m_brCombBK.CreateSolidBrush(ThemeManager::Instance().GetColor(ThemeColor_CustomComboxBk));
		}
		if (hBKBrush != NULL)
		{
			HBRUSH hHollowBrush = (HBRUSH)::GetStockObject(HOLLOW_BRUSH); 
			if (hBKBrush == hHollowBrush)
			{
				MemDC.BitBlt(rtTemp.left, rtTemp.top, rtTemp.Width(), rtTemp.Height(), 
					&dc, 0, 0, SRCCOPY);
				bFillSolid = FALSE;
			}
		}
	}
	DWORD dwStyleParent = GetParent()->GetStyle();
	//if (dwStyleParent & WS_CLIPCHILDREN)
	{
		if ( bFillSolid)
			//MemDC.FillSolidRect(&rtTemp,SYGetColorRes(SYCOLORRES_DLG_NORMAL_BK));
			MemDC.FillRect(rtTemp,&m_brCombBK);
	}

	DWORD dwStyle	= GetStyle();
	DWORD dwExStyle = GetExStyle();
	if ((dwStyle & 0x0000000f) == CBS_SIMPLE)
	{
 	}
	else if ((dwStyle & 0x0000000f) == CBS_DROPDOWN)
	{
		BOOL bEnable = IsWindowEnabled() ? TRUE : FALSE;
		CWnd* pFocusWnd = GetFocus();
		BOOL bFocus = FALSE;
		if (pFocusWnd != NULL)
		{
			if (pFocusWnd == this || pFocusWnd->GetParent() == this)
				bFocus = TRUE;
		}
		BOOL bIsPress = FALSE;
		if( m_nHit != -1)
			bIsPress = TRUE;

		CImage * pImage = m_pDropDownBtnU;
		if (!bEnable)
			pImage = m_pDropDownBtnX;
		else if (bIsPress||m_bIsDropDown)
			pImage = m_pDropDownBtnD;
		else if (bFocus)
			pImage = m_pDropDownBtnF;
		
		if (pImage)
		{
			CRect rtBtn(rtTemp.right - m_nDropDownBtnWidth, rtTemp.top, rtTemp.right, rtTemp.bottom);
			BufferFunction::DrawImageWithAplha(&MemDC, pImage, rtBtn, rtBtn, CRect(0, 0, m_nDropDownBtnWidth, m_nDropDownBtnHeight), CRect(), TRUE, 255, FALSE);
		}
	}
	else if ((dwStyle & 0x0000000f) == CBS_DROPDOWNLIST)
	{
		BOOL bEnable = IsWindowEnabled() ? TRUE : FALSE;
		CWnd* pFocusWnd = GetFocus();
		BOOL bFocus = FALSE;
		if (pFocusWnd != NULL)
		{
			if (pFocusWnd == this || pFocusWnd->GetParent() == this)
				bFocus = TRUE;
		}

		BOOL bIsPress = FALSE;
		if( m_nHit != -1)
			bIsPress = TRUE;
		CImage * pImage = m_pDropDownBtnU;
		if (!bEnable)
			pImage = m_pDropDownBtnX;
		else if (bIsPress||m_bIsDropDown)
			pImage = m_pDropDownBtnD;
		else if (bFocus)
			pImage = m_pDropDownBtnF;

		if (pImage)
		{
			CRect rtBtn(rtTemp.right - m_nDropDownBtnWidth, rtTemp.top, rtTemp.right, rtTemp.bottom);
			BufferFunction::DrawImageWithAplha(&MemDC, pImage, rtBtn, rtBtn, CRect(0, 0, m_nDropDownBtnWidth, m_nDropDownBtnHeight), CRect(), TRUE, 255, FALSE);
		}

		int	nDrawFlag = DT_SINGLELINE | DT_VCENTER;
		if (dwExStyle & WS_EX_RIGHT)
			nDrawFlag |= DT_RIGHT;
		else
			nDrawFlag |= DT_LEFT;
		if (GetStyle() & (CBS_OWNERDRAWFIXED | CBS_OWNERDRAWVARIABLE))
		{
			DRAWITEMSTRUCT drawitem;
			drawitem.CtlType	= ODT_COMBOBOX; 
			drawitem.CtlID		= GetDlgCtrlID(); 
			drawitem.itemID		= GetCurSel(); 
			drawitem.itemAction	= ODA_DRAWENTIRE; 
			drawitem.itemState	= ODS_COMBOBOXEDIT; 
			drawitem.hwndItem	= m_hWnd; 
			drawitem.hDC		= dc.m_hDC; 
			if (drawitem.itemID != CB_ERR)
				drawitem.itemData = GetItemData(drawitem.itemID);
			else
				drawitem.itemData = 0;
			drawitem.rcItem		= rtClient;
			if (GetFocus() == this)
				drawitem.itemState |= /*ODS_FOCUS |*/ ODS_SELECTED;
			dc.BitBlt(rtClient.left,rtClient.top,rtClient.Width(),rtClient.Height(),
			&MemDC,0,0,SRCCOPY);
			MemDC.SelectObject(hOldBmp);
			MemDC.SelectObject(pOldFont);
			MemDC.DeleteDC();
			GetOwner()->SendMessage(WM_DRAWITEM, drawitem.CtlID, (LPARAM)&drawitem);
			return;
		}
	}
	
	DrawText(&MemDC);

	dc.BitBlt(rtClient.left,rtClient.top,rtClient.Width(),rtClient.Height(),
		&MemDC,0,0,SRCCOPY);

	MemDC.SelectObject(hOldBmp);
	MemDC.SelectObject(pOldFont);
	MemDC.DeleteDC();
}

void CustomComboBox::DrawText( CDC *pDC )
{
	CRect rtText;
	GetCtrlRect( rtText );
	rtText.right -= m_nCtrlWidth;
	rtText.DeflateRect(m_szTopLeftBmp.cx+2, m_szTopLeftBmp.cy);
	pDC->SetBkMode(TRANSPARENT);
	COLORREF clrOldText = pDC->GetTextColor();
	
	COLORREF clrText;
	clrText = m_clText;
	if( !IsWindowEnabled() )
		clrText = m_clGrayText;
	pDC->SetTextColor( clrText );
	pDC->DrawText( m_strCurSel, rtText , m_nTextFormat);

	pDC->SetTextColor(clrOldText);
}

void CustomComboBox::OnSetFocus(CWnd* pOldWnd) 
{
	//////////////////////////////////////////////////////////////////////////
	//do not call for the white combobox splash out
	//	CWnd::OnSetFocus(pOldWnd);
	//////////////////////////////////////////////////////////////////////////
	
	PostMessage(WM_FLUSH_WND);
}

void CustomComboBox::OnKillFocus(CWnd* pNewWnd) 
{
//	CWnd::OnKillFocus(pNewWnd);
	if(m_pDropDownWnd && ::IsWindow(m_pDropDownWnd->m_hWnd))
		m_pDropDownWnd->ShowWindow( SW_HIDE );
	m_bIsDropDown = FALSE;
	if (m_strImeBuffer.GetLength() > 0) m_strImeBuffer = _T("");

	if(::IsWindow(m_hWnd))
		PostMessage(WM_FLUSH_WND);

	if (pNewWnd != m_pDropDownWnd)
		GetParent()->SendMessage(WM_COMMAND, MAKELPARAM(GetDlgCtrlID(), CBN_KILLFOCUS), (LPARAM)GetSafeHwnd());
}


void CustomComboBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_nHit = HitTest( point );
	
	if( m_bIsDropDown )//下拉框已经显示。
	{
		m_pDropDownWnd->ShowWindow( SW_HIDE );
		CRect rtClient;
		GetClientRect( &rtClient );
		
		m_nHit = -1;
		m_bIsDropDown = FALSE;
 		
		SetCapture();
		SetFocus();
		return;
	}
	GetParent()->SendMessage( WM_COMMAND , 
							MAKEWPARAM( GetDlgCtrlID() , CBN_SETFOCUS), 
							( LPARAM )( m_hWnd ) ) ;
	if( m_nHit == 0 )//点中下拉按钮。
	{
		SetFocus();
		::ReleaseCapture();
		GetParent()->SendMessage( WM_COMMAND , 
							   MAKEWPARAM( GetDlgCtrlID() , CBN_DROPDOWN ) , 
							   ( LPARAM )( m_hWnd ) ) ;
		CRect rtClient;
		GetClientRect( &rtClient );

		CRect rtWindow;
		GetWindowRect(rtWindow);
		rtWindow.top = rtWindow.bottom;
		
		int nCount = m_pDropDownWnd->GetNums();

		int nHeight = m_pDropDownWnd->m_nItemHeight*nCount + 2;
		CRect rtDrop;
		GetDropWndRect(rtDrop);
		nHeight = nHeight > rtDrop.Height() ? rtDrop.Height():nHeight;
		rtWindow.bottom = rtWindow.top + nHeight;

		if(m_nCurSel ==-1)
		{
			CString strShow;
			GetWindowText(strShow);
			if(strShow.IsEmpty()==0)
			{
				strShow.MakeLower();
				CString strItem;
				for(int i=0; i<GetCount(); i++)
				{
					strItem = GetLBText(i);
					strItem.MakeLower();
					if(strShow.Compare(strItem) == 0)
					{
						SetCurSel(i, FALSE);
						break;
					}
				}
			}
		}
		m_bIsDropDown = TRUE;
		rtWindow.OffsetRect(0,1);
		m_pDropDownWnd->SetWindowPos(&CWnd::wndTopMost,
			rtWindow.left,rtWindow.top,rtWindow.Width(),rtWindow.Height(),
			SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOOWNERZORDER);

		m_pDropDownWnd->EnsureVisible();	
		m_pDropDownWnd->ReSetScrollWndPos();
		m_pDropDownWnd->SetCapture();
		
		DWORD dwStyle = GetStyle();
		if((dwStyle & CBS_DROPDOWNLIST)== CBS_DROPDOWNLIST)
			PostMessage(WM_FLUSH_WND);//背景色不能是选中颜色了
		return;
	}

	if( m_nHit == -1 )
	{
		ReleaseCapture();
	}

	//////////////////////////////////////////////////////////////////////////
	//do not call this line for autoload combobox.
	//otherwise the combolist will splash out irritably!!! fixed by gm
	//
	//CWnd::OnLButtonDown(nFlags, point);
	//
	//////////////////////////////////////////////////////////////////////////
}

BOOL CustomComboBox::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	WNDCLASS	wcls;
	HINSTANCE	hInst = AfxGetInstanceHandle();
	if (!::GetClassInfo(hInst, _T(Custom_szComboBoxClass), &wcls))
	{
		RegisterControlClass(hInst);
	}

	dwStyle &= ~(WS_VSCROLL|WS_HSCROLL);

	CRect rtWnd = rect;
	if(rtWnd.Height() < 40)
		rtWnd.bottom = rtWnd.top + 40;
	if((dwStyle & CBS_SORT) == CBS_SORT)
		m_pDropDownWnd->SetSortString(TRUE);
	return CWnd::Create( _T(Custom_szComboBoxClass), NULL, dwStyle/*|WS_CLIPSIBLINGS*/, rtWnd, pParentWnd, nID); 
}
//返回下拉列表的项目数
int CustomComboBox::GetCount() const
{
	if( m_pDropDownWnd )
		return m_pDropDownWnd->GetCount();
	return 0;
}

int CustomComboBox::GetCurSel() const
{
	if(!IsWindow(m_hWnd))
		return CB_ERR;
	return (int)::SendMessage(m_hWnd,CB_GETCURSEL,0,0L);
}

int CustomComboBox::FindStringExact( int nIndexStart, LPCTSTR lpszFind ) const
{
	int nIndex = CB_ERR;
	if (lpszFind == NULL)
		return nIndex;

	CString strItem;
	int nCnt = GetCount();
	if (nIndexStart == -1) nIndexStart = 0;
	for (int i = nIndexStart; i < nCnt; ++i)
	{
		GetLBText(i, strItem);
	
		if (strItem.CompareNoCase(lpszFind) == 0)
		{
			nIndex = i;
			break;
		}
	}

	return nIndex;
}

int CustomComboBox::SelectString( int nStartAfter, LPCTSTR lpszString )
{
	int nIndex = CB_ERR;
	if (lpszString == NULL)
		return nIndex;

	CString strItem;
	int nCnt = GetCount();
	if (nStartAfter == -1) nStartAfter = 0;
	for (int i = nStartAfter; i < nCnt; ++i)
	{
		GetLBText(i, strItem);
		
		//compare prefix, is case
		int nFind = strItem.Find(lpszString);
		if (nFind == 0)
		{
			nIndex = i;
			break;
		}
	}

	if (nIndex > CB_ERR)
	{
		SetCurSel(nIndex);
		//copies string to the edit control
		// ...
	}

	return nIndex;
}

LRESULT CustomComboBox::OnGetCurSel(WPARAM wParam,LPARAM lParam)
{
	int nSel = m_pDropDownWnd->GetCurSel();
	CString strSel = GetLBText(nSel);
	CString str;
	GetWindowText(str);
	if (str == strSel)
		return nSel;
	else
		return CB_ERR;
}

int CustomComboBox::SetCurSel(int nSelect, BOOL bSendChangeMessageToParent /*= FALSE*/)
{
	if( m_pDropDownWnd )
	{
		if(!IsWindow(m_hWnd))
			return CB_ERR;
		
		int iOldSel = m_nCurSel;
		int iRet = (int)SendMessage(CB_SETCURSEL,(WPARAM)nSelect,0L);
		if((m_nCurSel != iOldSel) )
			if(bSendChangeMessageToParent)		
				GetParent()->SendMessage( WM_COMMAND , 
								  MAKEWPARAM( GetDlgCtrlID() , CBN_SELCHANGE ) , 
								  ( LPARAM )( m_hWnd ) ) ;
		PostMessage(WM_FLUSH_WND);
		return iRet;
	}

	PostMessage(WM_FLUSH_WND);
	return CB_ERR;
}

LRESULT CustomComboBox::OnSetCurSel(WPARAM wParam,LPARAM lParam)
{
	int nSelect = (int)wParam;
	CRect rect;
	GetCtrlRect( rect );
	int ii = m_pDropDownWnd->SetCurSel( nSelect );
	if( ii != -1 )
	{
		m_pDropDownWnd->GetLBText( ii , m_strCurSel );
		InvalidateRect( rect );
		m_nCurSel = nSelect;

		CWnd::SetWindowText(m_strCurSel);
	}
	else
	{
		m_strCurSel = _T("");
		InvalidateRect( rect );
		m_nCurSel = -1;
	}
	return m_nCurSel;
}

LRESULT CustomComboBox::OnSelectString(WPARAM wParam,LPARAM lParam)
{
	LPCTSTR lpszValue = (LPCTSTR)lParam;

	if (!m_pDropDownWnd || !::IsWindow(m_pDropDownWnd->m_hWnd))
		return CB_ERR;
	
	int ii = m_pDropDownWnd->FindString(-1,lpszValue);
	if(ii != CB_ERR)
	{
		return OnSetCurSel((WPARAM)ii,0L);
	}
	SetEditText(lpszValue);
	return CB_ERR;
}

int CustomComboBox::AddString(LPCTSTR lpszString)
{
	CString strItem = lpszString;
	//strItem = DYRTranslate(strItem,m_dwDicID)
	if( m_pDropDownWnd )
	{
		return m_pDropDownWnd->AddString( strItem );
	}
	return -1;
}
int CustomComboBox::AddStringEx(LPCTSTR lpszString)
{
	CString strItem = lpszString;
	if( m_pDropDownWnd )
	{
		return m_pDropDownWnd->AddStringEx( strItem );
	}
	return -1;
}

int CustomComboBox::AddStringArray(const CStringArray & aryString)
{
	if( m_pDropDownWnd )
	{
		return m_pDropDownWnd->AddStringArray(aryString);
	}
	return -1;
}

int CustomComboBox::DelString(UINT nIndex)
{
	return DeleteString(nIndex);
}
//删除指定索引的项
int CustomComboBox::DeleteString(UINT nIndex)
{
	int iRet = -1;
	if( m_pDropDownWnd )
		iRet = m_pDropDownWnd->DeleteString( nIndex );
	if(m_nCurSel == (int)nIndex)
	{
		if(m_nCurSel >= GetCount())
		{
			m_nCurSel = -1;
			m_strCurSel = _T("");
		}
	}
	if((int)nIndex < m_nCurSel)
	{
		m_nCurSel--;
	}
	return iRet;
}
//插入指定索引的字符串
int CustomComboBox::InsertString(int nIndex, LPCTSTR lpszString)
{
	CString strItem = lpszString;
	int iRet = -1;
	if( m_pDropDownWnd )
		iRet = m_pDropDownWnd->InsertString( nIndex , strItem );
	//end add
	if((int)nIndex < m_nCurSel)
	{
		m_nCurSel++;		
	}
		
	return iRet;
}

int CustomComboBox::ReplaceString(int nIndex,LPCTSTR lpszValue)
{
	CString strItem = lpszValue; 
	if(m_pDropDownWnd)
		return m_pDropDownWnd->ReplaceString(nIndex,strItem);
	if(nIndex == m_nCurSel)
	{
		m_strCurSel = strItem;
	}
	return -1;
}

void CustomComboBox::ResetContent()
{
	if( m_pDropDownWnd )
		m_pDropDownWnd->ResetContent();
	if(!m_strCurSel.IsEmpty())
		m_strCurSel = _T("");
	m_nCurSel = -1;
	if(IsWindow(m_hWnd))
		PostMessage(WM_FLUSH_WND);
	
}

void CustomComboBox::RereshContent()
{
		m_pDropDownWnd->ReSetScrollWndPos();
 		m_pDropDownWnd->Invalidate();
}

int CustomComboBox::SetItemData(int nIndex, DWORD_PTR dwItemData )
{
	if(m_pDropDownWnd == NULL) return CB_ERR;
//	if(!::IsWindow(m_pDropDownWnd->m_hWnd))  return CB_ERR;
	return m_pDropDownWnd->SetItemData(nIndex, dwItemData);
}
DWORD_PTR CustomComboBox::GetItemData(int nIndex ) const 
{
	if(m_pDropDownWnd == NULL) return CB_ERR;
	return m_pDropDownWnd->GetItemData(nIndex);
}

//得到下拉列表的CRect
void CustomComboBox::GetDropWndRect( CRect &rtDropWnd )
{
	CRect rtClient;
	GetWindowRect(&rtClient);
	rtClient.OffsetRect(-rtClient.TopLeft());
	rtDropWnd = rtClient;
	rtDropWnd.top += rtClient.Height()/*m_nCtrlHeight*/;
	rtDropWnd.bottom = rtDropWnd.top + m_nWndHeight;
}

//获取下拉按钮的CRect
void CustomComboBox::GetDropBtnRect( CRect &rtDropBtn )
{
// 	int nHeightMin = 2 * m_szTopLeftBmp.cy + m_szArrowBtn.cy;
// 	CRect rtClient;
// 	GetClientRect(&rtClient);
// 	if (rtClient.Height() >= nHeightMin)
// 	{
// 		rtDropBtn.top = rtClient.top + (rtClient.Height() - m_szArrowBtn.cy) / 2;
// 		rtDropBtn.bottom = rtDropBtn.top + m_szArrowBtn.cy;
// 		rtDropBtn.right = rtClient.right - m_szTopLeftBmp.cx;
// 		rtDropBtn.left = rtDropBtn.right - m_szArrowBtn.cx;
// 	}
// 	else
// 	{
// 		rtDropBtn = rtClient;
// 		rtDropBtn.top	 = rtDropBtn.top + m_szTopLeftBmp.cy - 1;
// 		rtDropBtn.bottom = rtDropBtn.top + m_szArrowBtn.cy;
// 		rtDropBtn.right  = rtDropBtn.right - (m_szTopLeftBmp.cx - 1);
// 		rtDropBtn.left   = rtDropBtn.right - m_szArrowBtn.cx;
// 	}
}

//获得包括button在内的Edit的CRect
void CustomComboBox::GetCtrlRect( CRect &rtCtrl )
{
	CRect rtClient;
	GetWindowRect(&rtClient);
	rtClient.OffsetRect(-rtClient.TopLeft());
	//rtClient.bottom = rtClient.top + m_nCtrlHeight;
	rtCtrl = rtClient;	
}

void CustomComboBox::DrawDropBtn( CDC* pDC , BOOL bIsPress )
{

}

/*
Name : HitTest
Description: test the mouse position
Param: pt: the point the mouse hit.
Return Value: -1: the mouse is out of the range of the control; 
			0: in the drop down button range;
			1:in combox but not in drop down button range.
*/
int CustomComboBox::HitTest( CPoint pt )
{
	CRect rtClient;
	GetClientRect( &rtClient );
	if( rtClient.PtInRect( pt ) == FALSE )
		return -1;

	CRect rtDropBtn;
	GetCtrlRect( rtDropBtn );
	if( rtDropBtn.PtInRect( pt ) )
		return 0;

	return 1;
}

void CustomComboBox::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	ReleaseCapture();
	if( m_nHit == 0 )
	{
		CDC *pDC = GetDC();
		if( pDC )
		{
			DrawDropBtn( pDC  , FALSE );
			ReleaseDC(pDC);
		}

		m_nHit = -1;
	}
	PostMessage(WM_FLUSH_WND);

	if (m_strImeBuffer.GetLength() > 0)
		m_strImeBuffer = _T("");

	CWnd::OnLButtonUp(nFlags, point);
}

void CustomComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(m_bIsDropDown)
		return;
	CRect rtClient;
	GetClientRect(&rtClient);
	int iOldHit = m_nHit;
	if(rtClient.PtInRect(point))
	{
		m_nHit = HitTest(point);
//		m_uTimerarr[DY_COMBO_TIMER_1] = (UINT)SetTimer(1,10,NULL);
	}
	else
	{
		m_nHit = -1;
	}
	if(iOldHit == m_nHit)
		return;

// 	CRect rtDropBtn;
// 	GetDropBtnRect(rtDropBtn);
// 	InvalidateRect(rtDropBtn);
	
	//CWnd::OnMouseMove(nFlags, point);
}

void CustomComboBox::OnTimer(UINT_PTR nIDEvent) 
{
	if(nIDEvent == COMBO_TIMER_FLUSH_DROPBTN)
	{
		CRect rect;
		CPoint pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		GetClientRect(&rect);
		if(!rect.PtInRect(pt))
		{
			m_nHit = -1;
			KillTimer(nIDEvent);
// 			CRect rtDropBtn;
// 			GetDropBtnRect(rtDropBtn);
// 			InvalidateRect(rtDropBtn);
		}
	}
	else if (nIDEvent == COMBO_TIMER_CLEAR_IMEBUFFER)
	{
		KillTimer(nIDEvent);
		m_strImeBuffer = _T("");
	}

	CWnd::OnTimer(nIDEvent);
}


LRESULT	CustomComboBox::OnDropWndSelect(WPARAM wParam,LPARAM lParam)
{
	m_nCurSel = (int)wParam;
	m_pDropDownWnd->GetLBText((int)wParam, m_strCurSel);

	SetFocus();
	if( m_bIsDropDown )
	{
		CString strOldSel = m_strCurSel;
	//	int iOldSel = m_nCurSel;
		m_pDropDownWnd->ShowWindow( SW_HIDE );
	    GetParent()->SendMessage( WM_COMMAND , 
								  MAKEWPARAM( GetDlgCtrlID() , CBN_SELENDOK) , 
								  ( LPARAM )( m_hWnd ) ) ;
		GetParent()->SendMessage( WM_COMMAND , 
								  MAKEWPARAM( GetDlgCtrlID() , CBN_CLOSEUP) , 
								  ( LPARAM )( m_hWnd ) ) ;
		CRect rtClient;
		GetClientRect( &rtClient );
		//下面的2多加了，这么加后非客户区多了2个屏幕像素，导致FormView里刷新问题。cgs
		//		SetWindowPos( NULL , rtClient.left , rtClient.top , rtClient.Width() , 
		//			rtClient.top + m_nCtrlHeight /*+ 2*/ ,  SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW );
		m_bIsDropDown = FALSE;
		CRect rect;
		GetCtrlRect( rect );
		InvalidateRect( rect );
		if(GetCapture() == this)
			::ReleaseCapture();

		//微软的标准控件CcomboBox,对于同一个字符串也要发送Selchange消息的。cgs
		//	if(iOldSel != m_nCurSel)
		GetParent()->SendMessage( WM_COMMAND , 
			MAKEWPARAM( GetDlgCtrlID() , CBN_SELCHANGE ) , 
			( LPARAM )( m_hWnd ) ) ;
		//	else
		//		GetParent()->SendMessage( WM_COMMAND , 
		//				  MAKEWPARAM( GetDlgCtrlID() , CBN_SELCHANGE ) , 
		//				  ( LPARAM )NULL ) ;

		CRect rcDropdown;
		m_pDropDownWnd->GetWindowRect(&rcDropdown);
		CWnd *pParentWnd = GetParent();
		pParentWnd->ScreenToClient(&rcDropdown);
		pParentWnd->InvalidateRect(&rcDropdown);
	}
	PostMessage(WM_FLUSH_WND);
	return TRUE;
}

LRESULT	CustomComboBox::OnDropWndKillFocus(WPARAM wParam,LPARAM lParam)
{
	if( m_bIsDropDown )
	{
		m_nHit = -1;
		BOOL bSelOK = TRUE;
		int sel = m_pDropDownWnd->GetCurSel();
		if(sel < 0 || sel > m_pDropDownWnd->GetCount()) 
			bSelOK = FALSE;
		CString str; m_pDropDownWnd->GetLBText(sel,str);
		if(str.CompareNoCase(m_strCurSel) != 0)
			bSelOK = FALSE;

		m_pDropDownWnd->ShowWindow( SW_HIDE );
		m_pDropDownWnd->SetCurSel(m_nCurSel);

		GetParent()->SendMessage( WM_COMMAND , 
			MAKEWPARAM( GetDlgCtrlID() , bSelOK ? CBN_SELENDOK : CBN_SELENDCANCEL) , 
			( LPARAM )( m_hWnd ) ) ;
		
		
		GetParent()->SendMessage( WM_COMMAND , 
			MAKEWPARAM( GetDlgCtrlID() , CBN_CLOSEUP), 
			( LPARAM )( m_hWnd ) ) ;
		CRect rtClient;
		GetClientRect( &rtClient );
		//下面的2多加了，这么加后非客户区多了2个屏幕像素，导致FormView里刷新问题。cgs
	//	SetWindowPos( NULL , rtClient.left , rtClient.top , rtClient.Width() , 
	//		rtClient.top + m_nCtrlHeight /*+ 2*/ ,  SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW );
		m_bIsDropDown = FALSE;
		if(GetCapture() == this)
			::ReleaseCapture();
		
	}

	SetFocus();
	PostMessage(WM_FLUSH_WND);
	return TRUE;
}


void CustomComboBox::GetLBText(int nIndex, CString& rString) const
{
	if( m_pDropDownWnd )
		m_pDropDownWnd->GetLBText( nIndex , rString );
}

int CustomComboBox::GetLBTextLen(int nIndex) const
{
	if( m_pDropDownWnd )
		return m_pDropDownWnd->GetLBTextLen( nIndex );
	return 0;
}

CString CustomComboBox::GetLBText(int nIndex)
{
	CString strValue;
	GetLBText(nIndex,strValue);
	return strValue;
}

CString CustomComboBox::GetEditText()
{
	CString strValue;
	GetWindowText(strValue);
	return strValue;
}

void CustomComboBox::SetEditText(CString strText)
{
	CString strEdit;
	if(m_pDropDownWnd)
	{
		for(int i = 0;i<m_pDropDownWnd->GetCount();i++)
		{
			m_pDropDownWnd->GetLBText(i,strEdit);
			if(strEdit == strText)
			{
				m_pDropDownWnd->SetCurSel(i);
				return ;
			}
		}
	}
}

int CustomComboBox::SetItemHeight( int nItemHeight )
{
	if( nItemHeight <= 0 )
		return 0;
	
	m_nCtrlHeight = nItemHeight;
	m_nCtrlWidth = nItemHeight;
	if( m_pDropDownWnd )
		m_pDropDownWnd->SetItemHeight( m_nCtrlHeight );

	if(m_pDropDownWnd && ::IsWindow(m_pDropDownWnd->m_hWnd))
	{
		int nScale = m_nWndHeight/(m_pDropDownWnd->GetItemHeight());
		m_nWndHeight = nScale* (m_pDropDownWnd->GetItemHeight()) + 2;	
		CRect r;
		GetClientRect(r);
		r.top += m_nCtrlHeight;
		r.bottom = r.top + m_nWndHeight;
		m_pDropDownWnd->SetWindowPos(NULL,0,0,r.Width(),r.Height(),SWP_NOMOVE|SWP_NOZORDER);
		m_pDropDownWnd->CalRowsNum();
		m_pDropDownWnd->SetVerScrollBarRange();
	}

	return m_nCtrlHeight;
}

int CustomComboBox::GetItemHeight() const
{
	return m_nCtrlHeight;
}

void CustomComboBox::PostNcDestroy()
{
	if( m_bAutoDelete )
	{
		// needed to clean up the C++ CWnd object
		delete this;
		TRACE0("Auto Delete ComboBox!\n");
	}
}

BOOL CustomComboBox::SubclassComboBox( UINT nID, CWnd* pParent )
{
	CWnd*	pDlg = CWnd::FromHandle( pParent->m_hWnd );
	ASSERT( pDlg );

	CRect rtControl;
	CRect rtWnd;
	
	CComboBox* pCombo = ((CComboBox*)(pDlg->GetDlgItem(nID)));
	ASSERT(pCombo);

	pCombo->GetWindowRect(&rtWnd);
	pCombo->GetDroppedControlRect(&rtControl);

	m_nCtrlHeight = rtWnd.Height();

	pCombo->GetWindowText(m_strCurSel);
	m_nCurSel = pCombo->GetCurSel();

	DWORD dwStyle = pCombo->GetStyle();
	CustomComboBox* pWnd = ( CustomComboBox* )FromHandlePermanent( pCombo->m_hWnd );
	if( pWnd )//ddx_control the combox is CustomComboBox type.
	{
		// pWnd->ModifyStyle(0,WS_CLIPSIBLINGS);
		pWnd->CreateChildWindows(dwStyle,rtControl);

		if((dwStyle & CBS_SORT) == CBS_SORT)
			pWnd->m_pDropDownWnd->SetSortString(TRUE);

		//initial combobox data.
		for(int i = 0;i<pCombo->GetCount();i++)
		{
			CString strLBText;
			pCombo->GetLBText(i,strLBText);
			pWnd->AddString(strLBText);
		}
		pWnd->m_strCurSel = m_strCurSel;
		pWnd->m_nCurSel = pCombo->GetCurSel();

	//	pWnd->m_pDropDownWnd->SetToolTipCtrl(pWnd->m_pToolTip);
		
		return FALSE;
	}
	if((dwStyle & CBS_SORT) == CBS_SORT)
		m_pDropDownWnd->SetSortString(TRUE);
	return SubclassDlgItem( nID, pParent );
}

BOOL CustomComboBox::AutoLoad( UINT nID, CWnd* pParent, BOOL fAutoDelete )
{
	// first attach the CDYStatic to the dialog control
	if( SubclassComboBox( nID , pParent ) == FALSE )
		return FALSE;

	// ModifyStyle(0,WS_CLIPSIBLINGS);
	CRect rtControl;
	CRect rtWnd;
	
	CComboBox* pCombo = ((CComboBox*)(pParent->GetDlgItem(nID)));
	ASSERT(pCombo);

	pCombo->GetWindowRect(&rtWnd);
	pCombo->GetDroppedControlRect(&rtControl);

	DWORD dwStyle = pCombo->GetStyle();
	
	//non ddx control do 
	if(!CreateChildWindows(dwStyle,rtControl))
		return FALSE;
	//initial combobox data.
	for(int i = 0;i<pCombo->GetCount();i++)
	{
		CString strLBText;
		pCombo->GetLBText(i,strLBText);
		AddString(strLBText);
	}

	m_bAutoDelete = fAutoDelete;
	return TRUE;
}

void CustomComboBox::OnChangeEdit()
{
}

void CustomComboBox::GetWindowText( CString &rString )
{   
	rString = m_strCurSel;
}

/*
Name : OnCtlColor
Description: WM_CTLCOLOR 消息响应函数。用于控件中子控件画之前的刷子准备。
Param: pDC：子控件的dc 。 pWnd：子控件的窗口指针。 nCtlColor: 控件颜色类型也即控件类型，祥见msdn。
Return Value: 画子控件的刷子。
*/
HBRUSH CustomComboBox::OnCtlColor( CDC* pDC , CWnd* pWnd , UINT nCtlColor )
{
	HBRUSH hbr = CWnd::OnCtlColor( pDC , pWnd , nCtlColor );
	
	switch( nCtlColor )
	{
		case CTLCOLOR_EDIT:
			COLORREF clBkColor;
			if(IsWindowEnabled())
			{
				clBkColor = m_clSelectText;
			}
			else
			{
				clBkColor = m_clGray;
			}

			pDC->SetBkColor( clBkColor );
			if( IsWindowEnabled() )
			{
				COLORREF clTextColor = m_clText;				
				pDC->SetTextColor(clTextColor);
			}
			else
			{
				COLORREF clGrayText = m_clGrayText;
				pDC->SetTextColor( clGrayText );
			}

			if(m_brCombBK.m_hObject)
			{
				m_brCombBK.DeleteObject();
				m_brCombBK.CreateSolidBrush(clBkColor);
			}
			return  (HBRUSH)m_brCombBK;
			break;
		case CTLCOLOR_LISTBOX:
			return (HBRUSH)m_brCombBK;
			break;
		case CTLCOLOR_STATIC:
			{
				COLORREF clBkColor = m_clGray;
				pDC->SetBkColor(clBkColor);
				COLORREF clGrayText = m_clGrayText;
				pDC->SetTextColor( clGrayText );
				if(m_brCombBK.m_hObject)
				{
					m_brCombBK.DeleteObject();
					m_brCombBK.CreateSolidBrush(clBkColor);
				}
			}
		default:
			{
				COLORREF clBkColor = m_clGray;
				pDC->SetBkColor(clBkColor);
				COLORREF clText;
				if(IsWindowEnabled())
				{
					clText = m_clText;
				}
				else
				{
					clText = m_clGrayText;
				}
				pDC->SetTextColor( clText );
				return  (HBRUSH)m_brCombBK;
			}
	}
	return (HBRUSH)m_brCombBK;
}

void CustomComboBox::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	CRect rtDropWnd;
	GetDropWndRect( rtDropWnd );
	ClientToScreen( &rtDropWnd );
	if(m_pDropDownWnd && ::IsWindow(m_pDropDownWnd->m_hWnd))
	{
		m_pDropDownWnd->SetWindowPos(&CWnd::wndTop, rtDropWnd.left, rtDropWnd.top, rtDropWnd.Width(), rtDropWnd.Height(),0);
		m_pDropDownWnd->ReSetScrollWndPos();
	}

	CRect rtCtrl;
	GetCtrlRect( rtCtrl );
	rtCtrl.DeflateRect(m_szTopLeftBmp.cx+1, m_szTopLeftBmp.cy);
	rtCtrl.right -= m_nCtrlWidth;
}

//-------------------------------------------------------------------------------------------------
//	Function:		FindString()	
//	Description:	Find specified string in combobox. If the return value is greater than or equal
//					to 0, it is the zero-based index of the matching item. It is CB_ERR if the 
//					search was unsuccessful.
//-------------------------------------------------------------------------------------------------
int CustomComboBox::FindString(int nStartAfter, LPCTSTR lpszString) const
{
	CString strItem = lpszString;
//	strItem = DYRTranslate(strItem,m_dwDicID);
	if(m_pDropDownWnd)
		return m_pDropDownWnd->FindString(nStartAfter, strItem);
	return CB_ERR;
}

//-------------------------------------------------------------------------------------------------
//	Function:		SetDroppedWidth()	
//	Description:	Set Dropped width of this combobox . 
//					If successful, the new width of the list box, otherwise CB_ERR
//-------------------------------------------------------------------------------------------------
int CustomComboBox::SetDroppedWidth(UINT nWidth)
{
	//	new dropped width must larger than original 
	//	rect's width
	CRect rect;
	GetClientRect(&rect);
	if((UINT)rect.Width() > nWidth)
		return CB_ERR;

	m_nDroppedWidth = nWidth;

	return m_nDroppedWidth;
}

//-------------------------------------------------------------------------------------------------
//	Function:		GetDroppedWidth()	
//	Description:	Get Dropped width of this combobox .
//-------------------------------------------------------------------------------------------------
int CustomComboBox::GetDroppedWidth() const
{
	return m_nDroppedWidth;
}

//-------------------------------------------------------------------------------------------------
//	Function:		SetTextFormat()
//	Description:	set text format for text shown in combobox edit control
//-------------------------------------------------------------------------------------------------
void CustomComboBox::SetTextFormat(UINT nFormat)
{
	m_nTextFormat = nFormat;
}

//-------------------------------------------------------------------------------------------------
//	Function:		GetTextFormat()
//	Description:	get text format
//-------------------------------------------------------------------------------------------------
UINT CustomComboBox::GetTextFormat() const
{
	return m_nTextFormat;
}

BOOL CustomComboBox::EnableWindow(BOOL bEnable )
{
	CRect rect;
	GetCtrlRect( rect );
	rect.bottom += 2;
	InvalidateRect( rect );

	return CWnd::EnableWindow( bEnable );
}


/*
Name : SetWindowText
Description: 设定comboBox的文字标题。当为带有EDIT框的为设定EDIT框的内容。
Param:
Return Value:
*/
void CustomComboBox::SetWindowText( CString rString )
{
	if(!IsWindow(m_hWnd))
		return;

	m_strCurSel = rString;
	SendMessage( WM_PAINT );

	CWnd::SetWindowText(rString);
}

BOOL CustomComboBox::SetItemImage(int nItemIndex,UINT nImageIndex)
{
	return m_pDropDownWnd->SetItemImage(nItemIndex,nImageIndex);
}

int CustomComboBox::GetItemImage(int nItemIndex ) const
{
	return m_pDropDownWnd->GetItemImage(nItemIndex);
}

BOOL CustomComboBox::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	return FALSE;
	if(m_pDropDownWnd 
		&& IsWindow(m_pDropDownWnd->m_hWnd)
		&&::IsWindowVisible(m_pDropDownWnd->m_hWnd))
	{		
		int iTotal = GetCount();
		int iCurSel = m_pDropDownWnd->GetCurSel();
		if(iCurSel < -1) iCurSel =-1;
		if(iCurSel >= iTotal-1 )
			iCurSel = iTotal-1;
		if(zDelta < -10)
			iCurSel = min(iCurSel +1,iTotal-1);
		if(zDelta > 10)
			iCurSel = max(iCurSel -1,0);
		m_pDropDownWnd->SetCurSel(iCurSel);
		m_pDropDownWnd->EnsureVisibleEx(iCurSel);
		m_pDropDownWnd->Invalidate();
		m_pDropDownWnd->ShowTip(iCurSel);
	}
	else
	{
		int iTotal = GetCount();
		int iCurSel = GetCurSel();
		if(zDelta < -10)
			iCurSel = min(iCurSel +1,iTotal-1);
		if(zDelta > 10)
			iCurSel = max(iCurSel -1,0);
		
		SetCurSel(iCurSel, TRUE);
	}
	return TRUE;
	
	//do not call this line, because the combolist will splash irritable.
	//	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CustomComboBox::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN)	
	{
		m_strKeyDown	= m_strCurSel;
		int iSel		= m_nCurSel;
		if (pMsg->wParam == VK_UP)
		{
			//先判断自动搜索列表是不是可见状态
			if(m_pDropDownWnd 
				&& IsWindow(m_pDropDownWnd->m_hWnd)
				&&::IsWindowVisible(m_pDropDownWnd->m_hWnd))
			{
				//然后处理下拉列表状态
				int nDropSel = m_pDropDownWnd->GetCurSel();
				nDropSel--;
				if(nDropSel<0) 
					nDropSel =0;
				else if(nDropSel>=GetCount()-1)
					nDropSel =GetCount()-1;
				m_pDropDownWnd->SetCurSel(nDropSel);
				m_pDropDownWnd->EnsureVisibleEx(nDropSel) ;
				m_pDropDownWnd->Invalidate();
				m_pDropDownWnd->ShowTip(nDropSel);
			}
			else
			{
				//最后处理只有Edit的状态
				iSel--;
				iSel = max(iSel,0);
				SetCurSel(iSel, TRUE);
			}			
			return TRUE;
		}
		else if(pMsg->wParam == VK_DOWN)
		{
			//自动搜索列表是不是可见状态
			if(m_pDropDownWnd 
				&& IsWindow(m_pDropDownWnd->m_hWnd)
				&&::IsWindowVisible(m_pDropDownWnd->m_hWnd))
			{
				int nDropSel = m_pDropDownWnd->GetCurSel();
				nDropSel++;
				if(nDropSel<0) 
					nDropSel = 0;
				else if(nDropSel>=GetCount()-1) 
					nDropSel = GetCount()-1;
				m_pDropDownWnd->SetCurSel(nDropSel);
				m_pDropDownWnd->EnsureVisibleEx(nDropSel) ;
				m_pDropDownWnd->Invalidate();
				m_pDropDownWnd->ShowTip(nDropSel);
			}
			else
			{
				iSel++;
				iSel = min(iSel,GetCount()-1);
				SetCurSel(iSel, TRUE);
			}			
			return TRUE;
		}
		else if(pMsg->wParam == VK_RETURN)
		{
			//用回车选定某一条目
			if(m_pDropDownWnd 
				&& IsWindow(m_pDropDownWnd->m_hWnd)
				&&::IsWindowVisible(m_pDropDownWnd->m_hWnd))
			{		
				int iTotal = GetCount();
				int iCurSel = m_pDropDownWnd->GetCurSel();
				SetCurSel(iCurSel, TRUE);	
				ReleaseCapture();
				m_pDropDownWnd->ShowWindow(SW_HIDE);
//				m_pToolTip->ShowWindow(SW_HIDE);
				SendMessage( WM_DROPWND_SELECT , iCurSel , 0 );	
				return TRUE;
			}			
		}
		else if (pMsg->wParam == VK_ESCAPE)
		{
			if( m_bIsDropDown ) // 下拉框已经显示
			{
				m_pDropDownWnd->ShowWindow(SW_HIDE);
				CRect rtClient;
				GetClientRect( &rtClient );
				
				m_nHit = -1;
				m_bIsDropDown = FALSE;
				
				SetCapture();
				SetFocus();
				PostMessage(WM_FLUSH_WND);
				return TRUE;				
			}
		}
		else if (pMsg->wParam == VK_F4) // F4功能键显示下拉列表
		{
			CRect rect;
			GetCtrlRect(rect);
			OnLButtonDown(0, rect.CenterPoint());
			return TRUE;
		}
		BOOL bRet = CWnd::PreTranslateMessage(pMsg);
		PostMessage(WM_FLUSH_WND);
		return bRet;
	}
	else if(pMsg->message == WM_KEYUP)
	{
		DWORD dwStyle = GetStyle();
		if((dwStyle & CBS_DROPDOWNLIST ) != CBS_DROPDOWNLIST)
			if(pMsg->wParam != VK_UP && pMsg->wParam != VK_DOWN
				&& m_strKeyDown.IsEmpty()==0)
			{
				if(m_strKeyDown.Compare(m_strCurSel))
					m_nCurSel =-1;
				m_strKeyDown.Empty();
			}

	}
	else if(pMsg->message == WM_MOUSEMOVE)
	{

	}
	else if( pMsg->message == WM_SYSKEYDOWN )
	{
		if( pMsg->wParam == VK_DOWN || pMsg->wParam == VK_UP )
		{
			CRect rect;
			GetCtrlRect(rect);
			OnLButtonDown(0, rect.CenterPoint());
			return TRUE;
		}
	}
	else if (pMsg->message == WM_MOUSEWHEEL)
	{
		int zDelta = GET_WHEEL_DELTA_WPARAM(pMsg->wParam);
		if(m_pDropDownWnd 
			&& IsWindow(m_pDropDownWnd->m_hWnd)
			&&::IsWindowVisible(m_pDropDownWnd->m_hWnd))
		{		
			int iTotal = GetCount();
			int iCurSel = m_pDropDownWnd->GetCurSel();
			if(iCurSel < -1) iCurSel =-1;
			if(iCurSel >= iTotal-1 )
				iCurSel = iTotal-1;
			if(zDelta < -10)
				iCurSel = min(iCurSel +1,iTotal-1);
			if(zDelta > 10)
				iCurSel = max(iCurSel -1,0);
			m_pDropDownWnd->SetCurSel(iCurSel);
			m_pDropDownWnd->EnsureVisibleEx(iCurSel);
			m_pDropDownWnd->Invalidate();
			m_pDropDownWnd->ShowTip(iCurSel);
		}
		else
		{
			CRect rt;
			GetWindowRect(&rt);
			if (!rt.PtInRect(pMsg->pt))
			{
				// 非下拉时，rect以外消息不处理
				return FALSE;
			}
			int iTotal = GetCount();
			int iCurSel = GetCurSel();
			if(zDelta < -10)
				iCurSel = min(iCurSel +1,iTotal-1);
			if(zDelta > 10)
				iCurSel = max(iCurSel -1,0);

			SetCurSel(iCurSel, TRUE);
		}
		return TRUE;
	}
		
	return CWnd::PreTranslateMessage(pMsg);
}

LRESULT CustomComboBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = __super::WindowProc(message, wParam, lParam);
	
	if (message == WM_IME_ENDCOMPOSITION) // 响应中文输入法
	{
		//IME输入自动匹配。
		//Step 1、获得IME输入缓冲
		CString strBuf;
		HIMC hIMC = ImmGetContext(m_hWnd);
		if (hIMC)
		{
			LONG nIMECharSize = ImmGetCompositionString(hIMC, GCS_RESULTSTR, 0, 0);
			if (nIMECharSize > 0)
			{
				int nStrLen = nIMECharSize + 1;
				LPTSTR szIMEInput = new TCHAR[nStrLen];
				memset(szIMEInput, 0, sizeof(TCHAR) * (nStrLen));
				ImmGetCompositionString(hIMC, GCS_RESULTSTR, szIMEInput, nIMECharSize);
				strBuf = szIMEInput;
				strBuf.MakeLower();

				delete[] szIMEInput;
			}
			ImmReleaseContext(m_hWnd, hIMC);
		}

		//Step 2、自动匹配
		if (!strBuf.IsEmpty())
		{
			DWORD dwStyle = GetStyle();
			int nSelIndex = -1;

			if ((dwStyle & CBS_DROPDOWNLIST) != 0)
			{
				CString strItem;
				int nCnt = GetCount();
				for (int i = 0; i < nCnt; ++i)
				{
					GetLBText(i, strItem);
					strItem.MakeLower();

					//从字符串开始处进行匹配
					int nFind = strItem.Find(strBuf);
					if (nFind == 0)
					{
						nSelIndex = i;
						break;
					}
				}
			}
			
			if (nSelIndex >= 0)
			{
				SetCurSel(nSelIndex);
				m_pDropDownWnd->SetCurSel(nSelIndex);
				m_pDropDownWnd->EnsureVisibleEx(nSelIndex) ;
				m_pDropDownWnd->Invalidate();
				m_pDropDownWnd->ShowTip(nSelIndex);

				CWnd* pParent = GetParent();
				if (pParent != NULL)
				{
					int nID = GetDlgCtrlID();
					pParent->SendMessage(WM_COMMAND, MAKEWPARAM(nID, CBN_SELCHANGE), (LPARAM)m_hWnd);
				}
			}
		}
	}
	else if(message == WM_KEYDOWN)	
	{
		BOOL bCtrl = GetKeyState(VK_CONTROL) & 0x8000;
		BOOL bAlt = GetKeyState(VK_MENU) & 0x8000;

		if ((wParam >= '0' && wParam <= '9')
		|| (wParam >= 'A' && wParam <= 'Z')
		&& !bCtrl && !bAlt)
		{
			//1. 自动匹配
			DWORD dwStyle = GetStyle();
			int nSelIndex = -1;

			if ((dwStyle & CBS_DROPDOWNLIST) != 0)
			{
				KillTimer(COMBO_TIMER_CLEAR_IMEBUFFER);

				m_strImeBuffer += (TCHAR)wParam;
				if (m_strImeBuffer.IsEmpty())
				{
					// 否则匹配首个
					return lResult;
				}

				CString strItem;
				int nCnt = GetCount();
				for (int i = 0; i < nCnt; ++i)
				{
					GetLBText(i, strItem);
					strItem.MakeUpper();

					//从字符串开始处进行匹配
					int nFind = strItem.Find(m_strImeBuffer);
					if (nFind == 0)
					{
						nSelIndex = i;
						break;
					}
				}

				if (nSelIndex == -1)
					m_strImeBuffer.SetAt(m_strImeBuffer.GetLength() - 1, '\0');
			}

			// 通知
			if (nSelIndex >= 0)
			{
				SetTimer(COMBO_TIMER_CLEAR_IMEBUFFER, 2000, NULL);
				SetCurSel(nSelIndex);
				m_pDropDownWnd->SetCurSel(nSelIndex);
				m_pDropDownWnd->EnsureVisibleEx(nSelIndex) ;
				m_pDropDownWnd->Invalidate();
				m_pDropDownWnd->ShowTip(nSelIndex);

				CWnd* pParent = GetParent();
				if (pParent != NULL)
				{
					int nID = GetDlgCtrlID();
					pParent->SendMessage(WM_COMMAND, MAKEWPARAM(nID, CBN_SELCHANGE), (LPARAM)m_hWnd);
				}
			}
		}
	}

	return lResult;
}


LRESULT CustomComboBox::OnMsgGetTextLength(WPARAM wP,LPARAM lP)
{
	CString rString;
	rString = m_strCurSel;
	
	return rString.GetLength();
}
LRESULT CustomComboBox::OnMsgGetText(WPARAM wP,LPARAM lP)
{
	CString rString;
	rString = m_strCurSel;
	int len =(int)wP;
	wP = (WPARAM)rString.GetLength();
	if(wP == 0)
		return 0;
	LPTSTR lpStrBuf = (LPTSTR)lP;
	_tcscpy_s(lpStrBuf,len,rString.GetBuffer(0));
	rString.ReleaseBuffer();
	return wP;
}
LRESULT CustomComboBox::OnMsgSetText(WPARAM wP,LPARAM lP)
{
	LPCTSTR lpString = LPCTSTR(lP);
	CString strRealText = lpString;
	m_strCurSel = strRealText;
	PostMessage(WM_FLUSH_WND);
	return 1L;
}

LRESULT CustomComboBox::OnFlushWnd(WPARAM wParam, LPARAM lParam)
{
	Invalidate();
	
	return 1L;
}

BOOL CustomComboBox::EnableRunTimeList(BOOL bEnable)
{
	BOOL bRet = m_bEnableRunTimeList;
	m_bEnableRunTimeList = bEnable;
	return bRet;
}
void CustomComboBox::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
}
void CustomComboBox::OnNcPaint() 
{
	//	CDC* pDC = GetWindowDC();
	//	ReleaseDC(pDC);
	//	pDC = NULL;
}

BOOL CustomComboBox::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}