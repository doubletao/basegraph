#include "stdafx.h"
#include "UIControl\UIButtonBar.h"
#include "UIControl\UICheckBtn.h"
#include "UIControl\UIImageDrawer.h"
#include "UIControl\ThemeResoueceID.h"
#include "UIControl\UIRectDrawer.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
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

EmUIElementMsgRet UIButtonBar::HandleMsg(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_setMsgFilter.count(nMsg) > 0)
	{
		switch (nMsg)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			ChildHandleMsg(nMsg, wParam, lParam);
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

void UIButtonBar::SetVisible(bool bVisible, bool bRefresh)
{
	//对于buttonbar来说，visible应该影响到其子物件
	m_bVisible = bVisible;
	for (auto it : m_vecChildElement)
	{
		if (it)
		{
			it->SetVisible(bVisible, bRefresh);
		}
	}
	if (bRefresh)
	{
		Refresh();
	}
}
