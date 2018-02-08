#include "stdafx.h"
#include "UIControl\UICheckBtnBar.h"
#include "UIControl\UICheckBtn.h"


UICheckBtnBar::UICheckBtnBar()
{
}


UICheckBtnBar::~UICheckBtnBar()
{
}

void UICheckBtnBar::Initialize(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd)
{
	UIElement::Initialize(nX, nY, nWidth, nHeight, 0, hParentWnd);
	m_setMsgFilter.insert(WM_LBUTTONDOWN);
	m_setMsgFilter.insert(WM_LBUTTONUP);
	m_setMsgFilter.insert(WM_MOUSEMOVE);
}

void UICheckBtnBar::ChildHandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
	for (size_t i = 0; i < m_vecChildElement.size(); i++)
	{
		UICheckBtn * pChild = (UICheckBtn *)m_vecChildElement[i];
		if (pChild && pChild->HitTest(lParam))
		{
			pChild->HandleMsg(nMsg, lParam, wParam);
		}
	}
}

void UICheckBtnBar::EnterLeaveChild(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
	for (size_t i = 0; i < m_vecChildElement.size(); i++)
	{
		UIElement *& pChild = m_vecChildElement[i];
		if (pChild && pChild->HitTest(lParam))
		{
			pChild->OnMouseHover();
		}
		else
		{
			pChild->OnMouseLeave();
		}
	}
}

bool UICheckBtnBar::Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd)
{
	Initialize(nX, nY, nWidth, nHeight, 0, hParentWnd);
	InitializeObject();
	return true;
}

EmUIElementMsgRet UICheckBtnBar::HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
	if (m_setMsgFilter.count(nMsg) > 0)
	{
		switch (nMsg)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			ChildHandleMsg(nMsg, lParam, wParam);
			break;
		case WM_MOUSEMOVE:
			EnterLeaveChild(nMsg, lParam, wParam);
			break;
		default:
			break;
		}
		return UIElementMsgSuccBreak;
	}
	return UIElementMsgSuccContinue;
}

void UICheckBtnBar::UnCkeckAll(int nExceptID/* = 0*/)
{
	for (size_t i = 0; i < m_vecChildElement.size(); i++)
	{
		UICheckBtn * pChild = (UICheckBtn *)m_vecChildElement[i];
		if (pChild)
		{
			if (pChild->GetMsgID() != nExceptID)
			{
				pChild->SetCheck(false);
			}
		}
	}
}
