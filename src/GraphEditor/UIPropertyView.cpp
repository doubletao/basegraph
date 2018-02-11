#include "stdafx.h"
#include "UIPropertyView.h"

UIPropertyView::UIPropertyView()
	:m_pPropertyTabBar(nullptr)
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
}

void UIPropertyView::ChildHandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
	for (size_t i = 0; i < m_vecChildElement.size(); i++)
	{
		UIElement * pChild = m_vecChildElement[i];
		if (pChild)
		{
			pChild->HandleMsg(nMsg, lParam, wParam);
		}
	}
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
		m_strText = _T("ÊôÐÔ´°");
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
