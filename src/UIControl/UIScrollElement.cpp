#include "stdafx.h"
#include "UIControl\UIScrollElement.h"

void UIScrollElement::Initialize(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UIElement * pParent)
{
	UIElement::Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParent);
	m_nLogicWidth = nWidth;
	m_nLogicHeight = nHeight;
}

int UIScrollElement::GetRealX()
{
	int nRet = 0;
	if (m_pParentElement)
	{
		nRet = m_nX + m_pParentElement->GetRealX();
	}
	else
	{
		nRet = m_nX;
	}
	return nRet + m_nScrollX;
}

int UIScrollElement::GetRealY()
{
	int nRet = 0;
	if (m_pParentElement)
	{
		nRet = m_nY + m_pParentElement->GetRealY();
	}
	else
	{
		nRet = m_nY;
	}
	return nRet + m_nScrollY;
}

UIScrollElement::UIScrollElement()
	: m_nScrollX(0)
	, m_nScrollY(0)
	, m_nLogicWidth(0)
	, m_nLogicHeight(0)
{
}

UIScrollElement::~UIScrollElement()
{
}

bool UIScrollElement::SetScrollInfo(int nLogicWidth, int nLogicHeight, bool bRefresh/* = true*/)
{
	if (nLogicWidth >= m_nWidth && nLogicHeight >= m_nHeight)
	{
		m_nLogicWidth = nLogicWidth;
		m_nLogicHeight = nLogicHeight;
		if (bRefresh)
		{
			Refresh();
		}
	}
	return false;
}

int UIScrollElement::GetScrollXPos()
{
	return m_nScrollX;
}

int UIScrollElement::GetScrollYPos()
{
	return m_nScrollY;
}

bool UIScrollElement::SetScrollXPos(int nXPos, bool bRefresh/* = true*/)
{
	if (nXPos > m_nLogicWidth - m_nWidth)
	{
		nXPos = m_nLogicWidth - m_nWidth;
	}
	m_nScrollX = nXPos;
	if (bRefresh)
	{
		Refresh();
	}
	return true;
}

bool UIScrollElement::SetScrollYPos(int nYPos, bool bRefresh/* = true*/)
{
	if (nYPos > m_nLogicHeight - m_nHeight)
	{
		nYPos = m_nLogicHeight - m_nHeight;
	}
	m_nScrollY = nYPos;
	if (bRefresh)
	{
		Refresh();
	}
	return true;
}

int UIScrollElement::GetLogicWidth()
{
	return m_nLogicWidth;
}

int UIScrollElement::GetLogicHeight()
{
	return m_nLogicHeight;
}
