#include "stdafx.h"
#include "UITimeLine.h"
#include "GraphEditorCommonDef.h"

UITimeLine::UITimeLine()
	: m_pTimeCode(nullptr)
	, m_pScrollHor(nullptr)
{
}


UITimeLine::~UITimeLine()
{
}

void UITimeLine::Release()
{
	UIElement::Release();
	m_pTimeCode = nullptr;
	m_pScrollHor = nullptr;
}

void UITimeLine::Initialize(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd)
{
	UIElement::Initialize(nX, nY, nWidth, nHeight, 0, hParentWnd);
	m_setMsgFilter.insert(WM_LBUTTONDOWN);
	m_setMsgFilter.insert(WM_LBUTTONUP);
	m_setMsgFilter.insert(WM_MOUSEMOVE);
}

void UITimeLine::InitializeObject()
{
	m_pTimeCode = new UITLTimeCode;
	m_pTimeCode->Create(0, 0, 100, 100, m_hParentWnd, this);
	m_vecChildElement.push_back(m_pTimeCode);

	m_pScrollHor = new UIScrollBar;
	m_pScrollHor->Create(0, 0, 10, 10, 10, 10, UIElementMsg_TimeLineHorScroll, true, m_hParentWnd, this);
	m_vecChildElement.push_back(m_pScrollHor);
}

void UITimeLine::ChildHandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
}

void UITimeLine::EnterLeaveChild(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
}

void UITimeLine::RearrangeChildPos()
{
	int nTimeCodeWidth = UIControl_GetIntValue(ThemeInt_TimeLineTimeCodeWidth);
	int nTimeCodeHeight = UIControl_GetIntValue(ThemeInt_TimeLineTimeCodeHeight);
	if (m_pTimeCode)
	{
		m_pTimeCode->MoveTo(0, 0, nTimeCodeWidth, nTimeCodeHeight);
	}
	if (m_pScrollHor)
	{
		m_pScrollHor->MoveTo(nTimeCodeWidth, 0, m_nWidth - nTimeCodeWidth, nTimeCodeHeight / 2);
		m_pScrollHor->ResetScroll(m_nWidth * 4, m_nWidth);
	}
}

bool UITimeLine::Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd)
{
	auto InsertOneDrawer = [&](int nStateValue, int nImgValue)
	{
		UIImageDrawer * pImgDrawer = new UIImageDrawer(nImgValue);
		m_mapDrawer[nStateValue].push_back(pImgDrawer);

		UITextDrawer * pTextDrawer = new UITextDrawer(ThemeFont_SkinWndTitleText);
		pTextDrawer->SetColor(RGB(255, 255, 255));
		m_mapDrawer[nStateValue].push_back(pTextDrawer);
		m_strText = _T(" ±º‰œﬂ");
	};
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, 0, hParentWnd);
		InitializeObject();

		InsertOneDrawer(0, ThemeImage_ElementArea);
	}
	return true;
}

EmUIElementMsgRet UITimeLine::HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
	return UIElementMsgSuccBreak;
}
