#pragma once
#include "stdafx.h"
#include "UIControl\UIEdit.h"

UIEdit::UIEdit()
	: m_pEdit(nullptr)
{
}

UIEdit::~UIEdit()
{
	if (m_pEdit)
	{
		if (::IsWindow(m_pEdit->GetSafeHwnd()))
		{
			m_pEdit->DestroyWindow();
		}
		delete m_pEdit;
		m_pEdit = nullptr;
	}
}

bool UIEdit::Create(int nX, int nY, int nWidth, int nHeight, int nID, int nEditID, HWND hParentWnd, UIElement * pParent)
{
	bool bRet = false;
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParent);
		CWnd * pWnd = CWnd::FromHandlePermanent(hParentWnd);
		if (pWnd)
		{
			m_pEdit = new CEdit;
			m_pEdit->CreateEx(WS_EX_CLIENTEDGE, _T("Edit"), _T("UIEditWnd"), WS_BORDER | WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | ES_CENTER, CRect(0, 0, 100, 100), pWnd, nEditID);
		}
		bRet = true;
	}
	return bRet;
}

void UIEdit::RearrangeChildPos()
{
	int nX = GetRealX();
	int nY = GetRealY();
	if (m_pEdit && ::IsWindow(m_pEdit->GetSafeHwnd()))
	{
		m_pEdit->MoveWindow(nX, nY, m_nWidth, m_nHeight);
	}
}

void UIEdit::SetText(CString strText)
{
	if (m_pEdit && ::IsWindow(m_pEdit->GetSafeHwnd()))
	{
		m_pEdit->SetWindowText(strText);
	}
}

CString UIEdit::GetText()
{
	CString strRet = _T("");
	if (m_pEdit && ::IsWindow(m_pEdit->GetSafeHwnd()))
	{
		m_pEdit->GetWindowText(strRet);
	}
	return strRet;
}
