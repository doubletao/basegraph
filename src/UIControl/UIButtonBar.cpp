#include "stdafx.h"
#include "UIControl\UIButtonBar.h"
#include "UIControl\UICheckBtn.h"
#include "UIControl\UIImageDrawer.h"
#include "UIControl\ThemeResoueceID.h"
#include "UIControl\UIRectDrawer.h"

UIButtonBar::UIButtonBar()
{
}


UIButtonBar::~UIButtonBar()
{
}

void UIButtonBar::Initialize(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd)
{
	UIElement::Initialize(nX, nY, nWidth, nHeight, 0, hParentWnd);
	m_setMsgFilter.insert(WM_LBUTTONDOWN);
	m_setMsgFilter.insert(WM_LBUTTONUP);
	m_setMsgFilter.insert(WM_MOUSEMOVE);
}

void UIButtonBar::AddSplitLine(int nWidth, int nHeight, COLORREF clColor)
{
	UIButton * pEleRect = new UIButton;
	pEleRect->Create(0, 0, nWidth, nHeight, 0, m_hParentWnd, clColor, clColor, clColor, clColor, this);
	m_vecChildElement.push_back(pEleRect);
}

bool UIButtonBar::Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd)
{
	auto InsertOneDrawer = [&](int nStateValue, int nImgValue)
	{
		UIImageDrawer * pImgDrawer = new UIImageDrawer(nImgValue);
		m_mapDrawer[nStateValue].push_back(pImgDrawer);
	};
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, 0, hParentWnd);
		InitializeObject();

		InsertOneDrawer(0, ThemeImage_ElementArea);
	}
	return true;
}

EmUIElementMsgRet UIButtonBar::HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
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
