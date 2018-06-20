#pragma once
#include "stdafx.h"
#include "UIControl\UIEdit.h"

BEGIN_MESSAGE_MAP(UIEdit, CWnd)
	ON_WM_SETFOCUS()
	ON_MESSAGE(EmUIElementMsg_OnEditEnded, UIEdit::OnEndEdit)
END_MESSAGE_MAP()

#define CustomUIEditClass "CustomUIEditClass"

UIEdit::UIEdit()
	: m_pComboBox(nullptr)
{
}

UIEdit::~UIEdit()
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

BOOL UIEdit::RegisterControlClass()
{
	HINSTANCE	hInst = AfxGetInstanceHandle();
	return	RegisterControlClass(hInst);
}

BOOL UIEdit::RegisterControlClass(HINSTANCE hInst)
{
	if (hInst == NULL)		hInst = AfxGetInstanceHandle();

	WNDCLASS wcls;
	// check to see if class already registered
	static const TCHAR szClass[] = _T(CustomUIEditClass);
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

void UIEdit::OnSetFocus(CWnd * pOldWnd)
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		m_pComboBox->SetFocus();
		m_pComboBox->SetSel(0, -1, TRUE);
	}
}

LRESULT UIEdit::OnEndEdit(WPARAM, LPARAM)
{
	//通知上层，编辑结束了
	EmitMsg(EmUIElementMsg_OnEditEnded, m_nID, (LPARAM)this);
	return 0;
}

bool UIEdit::Create(int nX, int nY, int nWidth, int nHeight, int nID, int nEditID, DWORD dwStyleEs, HWND hParentWnd, UIElement * pParent)
{
	bool bRet = false;
	if (!m_bCreated)
	{
		CWnd * pParentWnd = CWnd::FromHandle(hParentWnd);
		if (pParentWnd)
		{
			WNDCLASS	wcls;
			HINSTANCE	hInst = AfxGetInstanceHandle();
			if (!::GetClassInfo(hInst, _T(CustomUIEditClass), &wcls))
			{
				RegisterControlClass(hInst);
			}
			if (CWnd::Create(_T(CustomUIEditClass), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | WS_CLIPSIBLINGS, CRect(), pParentWnd, nID, nullptr))
			{
				Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParent);
				m_pComboBox = new CustomEdit;
				m_pComboBox->Create(WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | dwStyleEs, CRect(), this, nEditID);
				bRet = true;
			}
		}
	}
	return bRet;
}

void UIEdit::RearrangeChildPos()
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		m_pComboBox->MoveWindow(0, 0, m_nWidth, m_nHeight);
	}
}

void UIEdit::SetText(CString strText)
{
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		m_pComboBox->SetWindowText(strText);
		//强制立刻刷新
		m_pComboBox->UpdateWindow();
	}
}

CString UIEdit::GetText()
{
	CString strRet = _T("");
	if (m_pComboBox && ::IsWindow(m_pComboBox->GetSafeHwnd()))
	{
		m_pComboBox->GetWindowText(strRet);
	}
	return strRet;
}

inline void UIEdit::MoveTo(CRect rtNew, bool bRefresh)
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

inline void UIEdit::MoveTo(int nX, int nY, bool bRefresh)
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

inline void UIEdit::MoveTo(int nX, int nY, int nWidth, int nHeight, bool bRefresh)
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

void UIEdit::SetVisible(bool bVisible, bool bRefresh)
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

CWnd * UIEdit::GetNextDlgTabItem(CWnd * pWndCtl, BOOL bPrevious) const
{
	return nullptr;
}
