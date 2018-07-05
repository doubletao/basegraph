#pragma once
#include "stdafx.h"
#include "UIControl\UIComboBox.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
#define CustomUIComboBoxClass "CustomUIComboBoxClass"
const int nCmbID = 10423;

BEGIN_MESSAGE_MAP(UIComboBox, CWnd)
	ON_WM_SETFOCUS()
	ON_CBN_SELCHANGE(nCmbID, &UIComboBox::OnCbnSelchange)
END_MESSAGE_MAP()

UIComboBox::UIComboBox()
	: m_pComboBox(nullptr)
{
}

UIComboBox::~UIComboBox()
{
	if (m_pComboBox)
	{
		if (::IsWindow(m_pComboBox->GetSafeHwnd()))
		{
			m_pComboBox->DestroyWindow();
		}
		delete m_pComboBox;
		m_pComboBox = nullptr;
	}
}

BOOL UIComboBox::RegisterControlClass()
{
	HINSTANCE	hInst = AfxGetInstanceHandle();
	return	RegisterControlClass(hInst);
}

BOOL UIComboBox::RegisterControlClass(HINSTANCE hInst)
{
	if (hInst == NULL)		hInst = AfxGetInstanceHandle();

	WNDCLASS wcls;
	// check to see if class already registered
	static const TCHAR szClass[] = _T(CustomUIComboBoxClass);
	if (::GetClassInfo(hInst, szClass, &wcls)) {
		// name already registered - ok if it was us
		return (wcls.lpfnWndProc == AfxWndProc);
	}

	memset(&wcls, sizeof(WNDCLASS), 0);

	// Use standard "static" control as a template.
	VERIFY(::GetClassInfo(NULL, _T("Static"), &wcls));

	wcls.lpfnWndProc = AfxWndProc;
	wcls.style |= CS_DBLCLKS;
	wcls.hInstance = hInst;
	wcls.lpszClassName = szClass;
	return AfxRegisterClass(&wcls);
}

void UIComboBox::OnSetFocus(CWnd * pOldWnd)
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		m_pComboBox->SetFocus();
	}
}

void UIComboBox::OnCbnSelchange()
{
	//通知上层，选中项变动
	EmitMsg(EmUIElementMsg_OnComboBoxSelChged, m_nID, (LPARAM)this);
}

bool UIComboBox::Create(int nX, int nY, int nWidth, int nHeight, int nID, DWORD dwStyleEs, HWND hParentWnd, UIElement * pParent)
{
	bool bRet = false;
	if (!m_bCreated)
	{
		CWnd * pParentWnd = CWnd::FromHandle(hParentWnd);
		if (pParentWnd)
		{
			WNDCLASS	wcls;
			HINSTANCE	hInst = AfxGetInstanceHandle();
			if (!::GetClassInfo(hInst, _T(CustomUIComboBoxClass), &wcls))
			{
				RegisterControlClass(hInst);
			}
			if (CWnd::Create(_T(CustomUIComboBoxClass), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | WS_CLIPSIBLINGS, CRect(), pParentWnd, nID, nullptr))
			{
				Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParent);
				m_pComboBox = new CustomComboBox;
				bRet = m_pComboBox->Create(WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL | dwStyleEs, CRect(0, 0, 200, 300), this, nCmbID);
				bRet = true;
			}
		}
	}
	return bRet;
}

void UIComboBox::RearrangeChildPos()
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		m_pComboBox->MoveWindow(0, 0, m_nWidth, m_nHeight);
	}
}

inline void UIComboBox::MoveTo(CRect rtNew, bool bRefresh)
{
	m_nX = rtNew.left;
	m_nY = rtNew.top;
	m_nWidth = rtNew.Width();
	m_nHeight = rtNew.Height();
	if (bRefresh)
	{
		RearrangeChildPos();
		Refresh();
	}
	MoveWindow(GetRealX(), GetRealY(), m_nWidth, m_nHeight);
}

inline void UIComboBox::MoveTo(int nX, int nY, bool bRefresh)
{
	m_nX = nX;
	m_nY = nY;
	if (bRefresh)
	{
		RearrangeChildPos();
		Refresh();
	}
	MoveWindow(GetRealX(), GetRealY(), m_nWidth, m_nHeight);
}

inline void UIComboBox::MoveTo(int nX, int nY, int nWidth, int nHeight, bool bRefresh)
{
	m_nX = nX;
	m_nY = nY;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	if (bRefresh)
	{
		RearrangeChildPos();
		Refresh();
	}
	MoveWindow(GetRealX(), GetRealY(), m_nWidth, m_nHeight);
}

void UIComboBox::SetVisible(bool bVisible, bool bRefresh)
{
	m_bVisible = bVisible;
	if (bRefresh)
	{
		if (bVisible)
		{
			ShowWindow(SW_SHOW);
		}
		else
		{
			ShowWindow(SW_HIDE);
		}
		Refresh();
	}
}

CWnd * UIComboBox::GetNextDlgTabItem(CWnd * pWndCtl, BOOL bPrevious) const
{
	return nullptr;
}

void UIComboBox::GetWindowText(CString & rString)
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		m_pComboBox->GetWindowText(rString);
	}
}

void UIComboBox::SetWindowText(CString rString)
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		m_pComboBox->SetWindowText(rString);
	}
}

int UIComboBox::GetCount() const
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		return m_pComboBox->GetCount();
	}
	return 0;
}

int UIComboBox::GetCurSel() const
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		return m_pComboBox->GetCurSel();
	}
	return -1;
}

int UIComboBox::SetCurSel(int nSelect, BOOL bSendChangeMessageToParent)
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		return m_pComboBox->SetCurSel(nSelect, bSendChangeMessageToParent);
	}
	return -1;
}

int UIComboBox::FindStringExact(int nIndexStart, LPCTSTR lpszFind) const
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		return m_pComboBox->FindStringExact(nIndexStart, lpszFind);
	}
	return -1;
}

int UIComboBox::SelectString(int nStartAfter, LPCTSTR lpszString)
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		return m_pComboBox->SelectString(nStartAfter, lpszString);
	}
	return -1;
}

int UIComboBox::AddString(LPCTSTR lpszString)
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		return m_pComboBox->AddString(lpszString);
	}
	return -1;
}

int UIComboBox::AddStringEx(LPCTSTR lpszString)
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		return m_pComboBox->AddStringEx(lpszString);
	}
	return -1;
}

int UIComboBox::AddStringArray(const CStringArray & aryString)
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		return m_pComboBox->AddStringArray(aryString);
	}
	return -1;
}

int UIComboBox::DeleteString(UINT nIndex)
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		return m_pComboBox->DeleteString(nIndex);
	}
	return -1;
}

int UIComboBox::DelString(UINT nIndex)
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		return m_pComboBox->DelString(nIndex);
	}
	return -1;
}

int UIComboBox::InsertString(int nIndex, LPCTSTR lpszString)
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		return m_pComboBox->InsertString(nIndex, lpszString);
	}
	return -1;
}

int UIComboBox::ReplaceString(int nIndex, LPCTSTR lpszValue)
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		return m_pComboBox->ReplaceString(nIndex, lpszValue);
	}
	return -1;
}

void UIComboBox::ResetContent()
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		m_pComboBox->ResetContent();
	}
}

void UIComboBox::RereshContent()
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		m_pComboBox->RereshContent();
	}
}

int UIComboBox::FindString(int nStartAfter, LPCTSTR lpszString) const
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		return m_pComboBox->FindString(nStartAfter, lpszString);
	}
	return -1;
}

void UIComboBox::GetLBText(int nIndex, CString & rString) const
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		m_pComboBox->GetLBText(nIndex, rString);
	}
}

int UIComboBox::GetLBTextLen(int nIndex) const
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		return m_pComboBox->GetLBTextLen(nIndex);
	}
	return 0;
}

int UIComboBox::SetItemData(int nIndex, DWORD_PTR dwItemData)
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		return m_pComboBox->SetItemData(nIndex, dwItemData);
	}
	return 0;
}

DWORD_PTR UIComboBox::GetItemData(int nIndex) const
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		return m_pComboBox->GetItemData(nIndex);
	}
	return NULL;
}

CString UIComboBox::GetLBText(int nIndex)
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		return m_pComboBox->GetLBText(nIndex);
	}
	return _T("");
}

CString UIComboBox::GetEditText()
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		return m_pComboBox->GetEditText();
	}
	return _T("");
}

void UIComboBox::SetEditText(CString strText)
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		m_pComboBox->SetEditText(strText);
	}
}

UINT UIComboBox::GetTextFormat() const
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		return m_pComboBox->GetTextFormat();
	}
	return 0;
}

void UIComboBox::SetTextFormat(UINT nFormat)
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		m_pComboBox->SetTextFormat(nFormat);
	}
}

BOOL UIComboBox::EnableWindow(BOOL bEnable)
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		return m_pComboBox->EnableWindow(bEnable);
	}
	return FALSE;
}

BOOL UIComboBox::SetItemImage(int nItemIndex, UINT nImageIndex)
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		return m_pComboBox->SetItemImage(nItemIndex, nImageIndex);
	}
	return FALSE;
}

int UIComboBox::GetItemImage(int nItemIndex) const
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		return m_pComboBox->GetItemImage(nItemIndex);
	}
	return -1;
}

BOOL UIComboBox::IsDropDown()
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		return m_pComboBox->IsDropDown();
	}
	return FALSE;
}
