#include "stdafx.h"
#include "UIControl\UIScrollElement.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
void UIScrollElement::Initialize(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UIElement * pParent)
{
	UIElement::Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParent);
	m_nLogicWidth = nWidth;
	m_nLogicHeight = nHeight;
}

inline bool UIScrollElement::HitTest(CPoint point)
{
	//滚动窗口的hittest应当以窗口实际大小为判定基础，而不是逻辑大小
	int nX = GetRealX() + m_nScrollX;
	int nY = GetRealY() + m_nScrollY;
	CRect rtReal(nX, nY, nX + m_nWidth, nY + m_nHeight);
	if (false)
	{
		BOOL bTest = rtReal.PtInRect(point);
		int i = 0;
		i++;
	}
	if (rtReal.PtInRect(point))
	{
		return true;
	}
	return false;
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
	return nRet - m_nScrollX;
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
	return nRet - m_nScrollY;
}

inline CRect UIScrollElement::GetRect()
{
	int nX = GetRealX();
	int nY = GetRealY();
	return CRect(nX, nY, nX + m_nLogicWidth, nY + m_nLogicHeight);
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
	if (nXPos < 0)
	{
		nXPos = 0;
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
	if (nYPos < 0)
	{
		nYPos = 0;
	}
	m_nScrollY = nYPos;
	if (bRefresh)
	{
		Refresh();
	}
	return true;
}

void UIScrollElement::SetLogicWidth(int nWidth)
{
	//逻辑宽高一定要大于等于实际宽高
	if (nWidth < m_nWidth)
	{
		nWidth = m_nWidth;
	}
	m_nLogicWidth = nWidth;
}

void UIScrollElement::SetLogicHeight(int nHeight)
{
	//逻辑宽高一定要大于等于实际宽高
	if (nHeight < m_nHeight)
	{
		nHeight = m_nHeight;
	}
	m_nLogicHeight = nHeight;
}

int UIScrollElement::GetLogicWidth()
{
	return m_nLogicWidth;
}

int UIScrollElement::GetLogicHeight()
{
	return m_nLogicHeight;
}

float UIScrollElement::GetXRatio()
{
	float fRet = 0.0f;
	if (m_nWidth > 0)
	{
		fRet = m_nLogicWidth * 1.0f / m_nWidth;
	}
	return fRet;
}

float UIScrollElement::GetYRatio()
{
	float fRet = 0.0f;
	if (m_nHeight > 0)
	{
		fRet = m_nLogicHeight * 1.0f / m_nHeight;
	}
	return fRet;
}
