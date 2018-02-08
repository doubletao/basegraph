#include "stdafx.h"
#include "UISimpleTimeLine.h"


UISimpleTimeLine::UISimpleTimeLine()
{
}


UISimpleTimeLine::~UISimpleTimeLine()
{
}

void UISimpleTimeLine::Initialize(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd)
{
	UIElement::Initialize(nX, nY, nWidth, nHeight, 0, hParentWnd);
	m_setMsgFilter.insert(WM_LBUTTONDOWN);
	m_setMsgFilter.insert(WM_LBUTTONUP);
	m_setMsgFilter.insert(WM_MOUSEMOVE);
}

void UISimpleTimeLine::InitializeObject()
{
}

void UISimpleTimeLine::ChildHandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
}

void UISimpleTimeLine::EnterLeaveChild(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
}

bool UISimpleTimeLine::Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd)
{
	auto InsertOneDrawer = [&](int nStateValue, int nImgValue)
	{
		UIImageDrawer * pImgDrawer = new UIImageDrawer(nImgValue);
		m_mapDrawer[nStateValue].push_back(pImgDrawer);

		UITextDrawer * pTextDrawer = new UITextDrawer(ThemeFont_SkinWndTitleText);
		pTextDrawer->SetColor(RGB(255, 255, 255));
		m_mapDrawer[nStateValue].push_back(pTextDrawer);
		m_strText = _T("简单时间线");
	};
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, 0, hParentWnd);
		InitializeObject();

		InsertOneDrawer(0, ThemeImage_ElementArea);
	}
	return true;
}

EmUIElementMsgRet UISimpleTimeLine::HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
	return UIElementMsgSuccBreak;
}
