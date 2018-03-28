#pragma once
#include "stdafx.h"
#include "UIControl\UIStatic.h"
#include "ThemeManager.h"

UIStatic::UIStatic()
	: m_nFont(0)
	, m_nFontColor(0)
{
}

UIStatic::~UIStatic()
{
}

bool UIStatic::Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UIElement * pParent, LPCTSTR strText, int nFont, COLORREF nFontColor)
{
	bool bRet = false;
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParent);
		m_strText = strText;
		m_nFont = nFont;
		m_nFontColor = nFontColor;
		bRet = true;
	}
	return bRet;
}

void UIStatic::SetText(LPCTSTR strText, int nFont, COLORREF nFontColor)
{
	m_strText = strText;
	m_nFont = nFont;
	m_nFontColor = nFontColor;
}

void UIStatic::DrawElement(CDC * pDC)
{
	if (!m_strText.IsEmpty())
	{
		int nX = GetRealX();
		int nY = GetRealY();
		LOGFONT lgfont = ThemeManager::Instance().GetFont(m_nFont);
		CFont font;
		font.CreateFontIndirect(&lgfont);
		CFont * pOldFont = pDC->SelectObject(&font);
		CRect rtText(nX, nY, nX + m_nWidth, nY + m_nHeight);
		pDC->SetTextColor(m_nFontColor);
		pDC->DrawText(m_strText, rtText, DT_SINGLELINE | DT_VCENTER | DT_LEFT);
		pDC->SelectObject(pOldFont);
	}
}
