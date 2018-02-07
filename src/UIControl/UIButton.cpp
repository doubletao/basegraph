#include "stdafx.h"
#include "UIControl\UIButton.h"
#include "UIControl\UIImageDrawer.h"
#include "UIControl\UITextDrawer.h"

UIButton::UIButton()
	: m_strText(_T(""))
{
}


UIButton::~UIButton()
{
}

void UIButton::Initialize(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd)
{
	UIElement::Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd);
	m_setMsgFilter.insert(WM_LBUTTONDOWN);
	m_setMsgFilter.insert(WM_LBUTTONUP);
}

void UIButton::OnButtonClickDown()
{
	::SetCapture(m_hParentWnd);
	//ÉèÖÃ×´Ì¬
	SetState(UIElement_Down, true);
}

void UIButton::OnButtonClickUp()
{
	::ReleaseCapture();
	//ÉèÖÃ×´Ì¬
	SetState(UIElement_Float, true);
	//ÏìÓ¦°´Å¥
	::SendMessage(m_hParentWnd, EmUIElementMsg_BtnClick, m_nID, 0);
}

void UIButton::SetState(int nState, bool bRefresh)
{
	m_nState = nState;
	Refresh();
}

bool UIButton::Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, int nBKImgN, int nBKImgF, int nBKImgD, int nBKImgX, LPCTSTR strText, int nFont/* = 0*/)
{
	auto InsertOneDrawer = [&](int nStateValue, int nImgValue)
	{
		UIImageDrawer * pImgDrawer = new UIImageDrawer(nImgValue);
		m_mapDrawer[nStateValue].push_back(pImgDrawer);
		if (strText)
		{
			UITextDrawer * pTextDrawer = new UITextDrawer(nFont);
			m_mapDrawer[nStateValue].push_back(pTextDrawer);
		}
	};
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd);
		m_strText = strText;

		InsertOneDrawer(UIElement_Normal, nBKImgN);
		InsertOneDrawer(UIElement_Float, nBKImgF);
		InsertOneDrawer(UIElement_Down, nBKImgD);
		InsertOneDrawer(UIElement_Xable, nBKImgX);

		return true;
	}
	return false;
}

bool UIButton::Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, int nBKImgN, int nBKImgF, int nBKImgD, int nBKImgX, int nForeImgN, int nForeImgF, int nForeImgD, int nForeImgX, LPCTSTR strText, int nFont/* = 0*/)
{
	auto InsertOneDrawer = [&](int nStateValue, int nBKImgValue, int nForeImgValue)
	{
		UIImageDrawer * pImgDrawer = nullptr;
		pImgDrawer = new UIImageDrawer(nBKImgValue);
		m_mapDrawer[nStateValue].push_back(pImgDrawer);

		pImgDrawer = new UIImageDrawer(nForeImgValue);
		m_mapDrawer[nStateValue].push_back(pImgDrawer);
		if (strText)
		{
			UITextDrawer * pTextDrawer = new UITextDrawer(nFont);
			m_mapDrawer[nStateValue].push_back(pTextDrawer);
		}
	};
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd);
		m_strText = strText;

		InsertOneDrawer(UIElement_Normal, nBKImgN, nForeImgN);
		InsertOneDrawer(UIElement_Float, nBKImgF, nForeImgF);
		InsertOneDrawer(UIElement_Down, nBKImgD, nForeImgD);
		InsertOneDrawer(UIElement_Xable, nBKImgX, nForeImgX);

		return true;
	}
	return false;
}

EmUIElementMsgRet UIButton::HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
	if (m_setMsgFilter.count(nMsg) > 0)
	{
		switch (nMsg)
		{
		case WM_LBUTTONDOWN:
			OnButtonClickDown();
			break;
		case WM_LBUTTONUP:
			OnButtonClickUp();
			break;
		default:
			break;
		}
		return UIElementMsgSuccBreak;
	}
	return UIElementMsgSuccContinue;
}

void UIButton::OnMouseHover()
{
	if (UIElement_Normal == m_nState)
	{
		SetState(UIElement_Float, true);
	}
}

void UIButton::OnMouseLeave()
{
	if (UIElement_Float == m_nState)
	{
		SetState(UIElement_Normal, true);
	}
}
