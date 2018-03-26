#include "stdafx.h"
#include "UIPropertyView.h"

UIPropertyView::UIPropertyView()
	: m_pPropertyTabBar(nullptr)
	, m_pSpaceElement(nullptr)
{
}


UIPropertyView::~UIPropertyView()
{
}

void UIPropertyView::Release()
{
	UIElement::Release();
	m_pPropertyTabBar = nullptr;
}

void UIPropertyView::Initialize(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd)
{
	UIElement::Initialize(nX, nY, nWidth, nHeight, 0, hParentWnd);
	m_setMsgFilter.insert(WM_LBUTTONDOWN);
	m_setMsgFilter.insert(WM_LBUTTONUP);
	m_setMsgFilter.insert(WM_MOUSEMOVE);
}

void UIPropertyView::RearrangeChildPos()
{
	int nPropertyTabBarWdith = UIControl_GetIntValue(ThemeInt_PropretyViewTabWidth);
	if (m_pPropertyTabBar)
	{
		m_pPropertyTabBar->MoveTo(0, 0, nPropertyTabBarWdith, m_nHeight);
	}
}

void UIPropertyView::InitializeObject()
{
	m_pPropertyTabBar = new UIPropertyTabBar;
	m_pPropertyTabBar->Create(0, 0, 100, 100, m_hParentWnd, this);
	m_vecChildElement.push_back(m_pPropertyTabBar);

	m_pSpaceElement = new UIPropertyViewElement;
	m_pSpaceElement->Create(0, 0, 100, 100, m_hParentWnd, this);
	m_vecChildElement.push_back(m_pSpaceElement);
}

bool UIPropertyView::Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd)
{
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, 0, hParentWnd);
		InitializeObject();
	}
	return true;
}

EmUIElementMsgRet UIPropertyView::HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
	if (m_setMsgFilter.count(nMsg) > 0)
	{
		switch (nMsg)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			ChildHandleMsg(nMsg, lParam, wParam);
			break;
		default:
			break;
		}
		return UIElementMsgSuccBreak;
	}
	return UIElementMsgSuccContinue; 
}
