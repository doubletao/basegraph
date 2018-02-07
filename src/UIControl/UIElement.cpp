#include "stdafx.h"
#include "UIControl\UIElement.h"


void UIElement::Release()
{
	for (size_t i = 0; i < m_vecChildElement.size(); i++)
	{
		UIElement * & pChild = m_vecChildElement[i];
		if (pChild)
		{
			delete pChild;
			pChild = nullptr;
		}
	}
	m_vecChildElement.clear();

	for (std::map<int, std::vector<UIDrawer *>>::iterator it = m_mapDrawer.begin(); it != m_mapDrawer.end(); it++)
	{
		std::vector<UIDrawer *> & vecDrawer = it->second;
		for (size_t i = 0; i < vecDrawer.size(); i++)
		{
			UIDrawer * & pDrawer = vecDrawer[i];
			if (pDrawer)
			{
				delete pDrawer;
				pDrawer = nullptr;
			}
		}
	}
	m_mapDrawer.clear();
	m_nX = 0;
	m_nY = 0;
	m_nWidth = 1;
	m_nHeight = 1;
	m_nID = 0;
	m_hParentWnd = nullptr;
	m_nState = 0;
	m_bCreated = false;
}

void UIElement::Initialize(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd/* = nullptr*/)
{
	m_nX = nX;
	m_nY = nY;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nID = nID;
	m_hParentWnd = hParentWnd;
	m_nState = 0;
	m_bCreated = true;
}

void UIElement::Refresh()
{
	InvalidateRect(m_hParentWnd, CRect(m_nX, m_nY, m_nWidth, m_nHeight), false);
}

UIElement::UIElement()
	: m_hParentWnd(nullptr)
	, m_nID(0)
	, m_nX(0)
	, m_nY(0)
	, m_nWidth(1)
	, m_nHeight(1)
	, m_nState(0)
	, m_strText(_T(""))
	, m_bCreated(false)
{
}


UIElement::~UIElement()
{
	Release();
}

EmUIElementMsgRet UIElement::HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
	return UIElementMsgSuccContinue;
}

bool UIElement::IsSucceed(EmUIElementMsgRet emMsgRet)
{
	if (emMsgRet | UIElementMsgSucc)
	{
		return true;
	}
	return false;
}

bool UIElement::NeedContinue(EmUIElementMsgRet emMsgRet)
{
	if (emMsgRet | UIElementMsgContinue)
	{
		return true;
	}
	return false;
}

bool UIElement::HitTest(CPoint point)
{
	if (false)
	{
		CRect rtTest = GetRect();
		BOOL bTest = rtTest.PtInRect(point);
		int i = 0;
		i++;
	}
	if (GetRect().PtInRect(point))
	{
		return true;
	}
	return false;
}

bool UIElement::HitTest(int nX, int nY)
{
	return HitTest(CPoint(nX, nY));
}

bool UIElement::HitTest(LPARAM lParam)
{
	return HitTest(LOWORD(lParam), HIWORD(lParam));
}

void UIElement::DrawElement(CDC * pDC, int nX/* = 0*/, int nY/* = 0*/)
{
	//先画自己
	if (m_mapDrawer.count(m_nState))
	{
		std::vector<UIDrawer *> & vecDrawer = m_mapDrawer[m_nState];
		for (size_t i = 0; i < vecDrawer.size(); i++)
		{
			UIDrawer * & pDrawer = vecDrawer[i];
			if (pDrawer)
			{
				pDrawer->Draw(pDC, nX + m_nX, nY + m_nY, m_nWidth, m_nHeight, m_strText);
			}
		}
	}
	//再画子
	for (size_t i = 0; i < m_vecChildElement.size(); i++)
	{
		UIElement * & pChild = m_vecChildElement[i];
		if (pChild)
		{
			pChild->DrawElement(pDC, nX + m_nX, nY + m_nY);
		}
	}
}

void UIElement::OnMouseHover()
{
}

void UIElement::OnMouseLeave()
{
}

CRect UIElement::GetRect()
{
	return CRect(m_nX, m_nY, m_nX + m_nWidth, m_nY + m_nHeight);
}

inline void UIElement::MoveTo(CRect rtNew)
{
	m_nX = rtNew.left;
	m_nY = rtNew.top;
	m_nWidth = rtNew.Width();
	m_nHeight = rtNew.Height();
	Refresh();
}

inline void UIElement::MoveTo(int nX, int nY)
{
	m_nX = nX;
	m_nY = nY;
	Refresh();
}

