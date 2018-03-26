#pragma once
#include "stdafx.h"
#include "UIControl\UIStatic.h"

UIStatic::UIStatic()
	: m_pTextDrawer(nullptr)
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
		m_pTextDrawer = new UITextDrawer(nFont);
		m_pTextDrawer->SetColor(nFontColor);
		m_mapDrawer[0].push_back(m_pTextDrawer);
		bRet = true;
	}
	return bRet;
}

EmUIElementMsgRet UIStatic::HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
	return UIElementMsgSuccContinue;
}

void UIStatic::SetText(LPCTSTR strText, int nFont, COLORREF nFontColor)
{
	m_strText = strText;
	if (m_pTextDrawer)
	{
		m_pTextDrawer->SetResID(nFont);
		m_pTextDrawer->SetColor(nFontColor);
	}
}
