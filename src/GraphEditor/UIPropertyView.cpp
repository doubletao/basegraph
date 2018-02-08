#include "stdafx.h"
#include "UIPropertyView.h"
#include "UIPropertyTabBar.h"

UIPropertyView::UIPropertyView()
{
}


UIPropertyView::~UIPropertyView()
{
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
	if (m_vecChildElement.size() > 0)
	{
		UIPropertyTabBar * pPropertyTabBar = (UIPropertyTabBar *)m_vecChildElement[0];
		if (pPropertyTabBar)
		{
			pPropertyTabBar->MoveTo(0, 0, nPropertyTabBarWdith, m_nHeight);
		}
	}
}

void UIPropertyView::InitializeObject()
{
	UIPropertyTabBar * pPropertyTabBar = new UIPropertyTabBar;
	pPropertyTabBar->Create(0, 0, 100, 100, m_hParentWnd, this);
	m_vecChildElement.push_back(pPropertyTabBar);
}

void UIPropertyView::ChildHandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
}

void UIPropertyView::EnterLeaveChild(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
}

bool UIPropertyView::Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd)
{
	auto InsertOneDrawer = [&](int nStateValue, int nImgValue)
	{
		UIImageDrawer * pImgDrawer = new UIImageDrawer(nImgValue);
		m_mapDrawer[nStateValue].push_back(pImgDrawer);

		UITextDrawer * pTextDrawer = new UITextDrawer(ThemeFont_SkinWndTitleText);
		pTextDrawer->SetColor(RGB(255, 255, 255));
		m_mapDrawer[nStateValue].push_back(pTextDrawer);
		m_strText = _T(" Ù–‘¥∞");
	};
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, 0, hParentWnd);
		InitializeObject();

		InsertOneDrawer(0, ThemeImage_ElementArea);
	}
	return true;
}

EmUIElementMsgRet UIPropertyView::HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
	return UIElementMsgSuccBreak;
}
