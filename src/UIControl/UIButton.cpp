#include "stdafx.h"
#include "UIControl\UIButton.h"
#include "UIControl\UIImageDrawer.h"
#include "UIControl\UITextDrawer.h"
#include "UIControl\UIRectDrawer.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
UIButton::UIButton()
{
}


UIButton::~UIButton()
{
}

void UIButton::Initialize(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UIElement * pParent/* = nullptr*/)
{
	UIElement::Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParent);
	RegistWndMsg(WM_LBUTTONDOWN);
	RegistWndMsg(WM_LBUTTONUP);
}

void UIButton::OnButtonClickDown()
{
	//ÉèÖÃ×´Ì¬
	SetState(UIElement_BtnDown, true);
}

void UIButton::OnButtonClickUp()
{
	//ÏìÓ¦°´Å¥
	EmitMsg(EmUIElementMsg_BtnClick, m_nID, (LPARAM)this);
	//ÉèÖÃ×´Ì¬
	SetState(UIElement_BtnFloat, true);
}

void UIButton::SetState(int nState, bool bRefresh)
{
	if (m_nState == nState)
	{
		bRefresh = false;
	}
	m_nState = nState;
	if (bRefresh)
	{
		Refresh();
	}
}

bool UIButton::Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, int nBKImgN, int nBKImgF, int nBKImgD, int nBKImgX, UIElement * pParent/* = nullptr*/, LPCTSTR strText/* = nullptr*/, int nFont/* = 0*/, COLORREF nFontColor/* = 0*/)
{
	auto InsertOneDrawer = [&](int nStateValue, int nImgValue)
	{
		UIImageDrawer * pImgDrawer = new UIImageDrawer(nImgValue);
		m_mapDrawer[nStateValue].push_back(pImgDrawer);
		if (strText)
		{
			UITextDrawer * pTextDrawer = new UITextDrawer(nFont);
			pTextDrawer->SetColor(nFontColor);
			m_mapDrawer[nStateValue].push_back(pTextDrawer);
		}
	};
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParent);
		m_strText = strText;

		InsertOneDrawer(UIElement_BtnNormal, nBKImgN);
		InsertOneDrawer(UIElement_BtnFloat, nBKImgF);
		InsertOneDrawer(UIElement_BtnDown, nBKImgD);
		InsertOneDrawer(UIElement_BtnXable, nBKImgX);

		return true;
	}
	return false;
}

bool UIButton::Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, int nBKImgN, int nBKImgF, int nBKImgD, int nBKImgX, int nForeImgN, int nForeImgF, int nForeImgD, int nForeImgX, UIElement * pParent/* = nullptr*/, LPCTSTR strText/* = nullptr*/, int nFont/* = 0*/, COLORREF nFontColor/* = 0*/)
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
		Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParent);
		m_strText = strText;

		InsertOneDrawer(UIElement_BtnNormal, nBKImgN, nForeImgN);
		InsertOneDrawer(UIElement_BtnFloat, nBKImgF, nForeImgF);
		InsertOneDrawer(UIElement_BtnDown, nBKImgD, nForeImgD);
		InsertOneDrawer(UIElement_BtnXable, nBKImgX, nForeImgX);

		return true;
	}
	return false;
}

bool UIButton::Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, COLORREF clN, COLORREF clF, COLORREF clD, COLORREF clX, UIElement * pParent, LPCTSTR strText, int nFont, COLORREF nFontColor)
{
	auto InsertOneDrawer = [&](int nStateValue, COLORREF clValue)
	{
		UIRectDrawer * pRectDrawer = new UIRectDrawer(clValue);
		m_mapDrawer[nStateValue].push_back(pRectDrawer);
		if (strText)
		{
			UITextDrawer * pTextDrawer = new UITextDrawer(nFont);
			m_mapDrawer[nStateValue].push_back(pTextDrawer);
		}
	};
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParent);
		m_strText = strText;

		InsertOneDrawer(UIElement_BtnNormal, clN);
		InsertOneDrawer(UIElement_BtnFloat, clF);
		InsertOneDrawer(UIElement_BtnDown, clD);
		InsertOneDrawer(UIElement_BtnXable, clX);

		return true;
	}
	return false;
}

EmUIElementMsgRet UIButton::HandleMsg(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_bEnable && m_setMsgFilter.count(nMsg) > 0)
	{
		switch (nMsg)
		{
		case WM_LBUTTONDOWN:
			if (HitTest(lParam))
			{
				OnButtonClickDown();
				return UIElementMsgSuccBreak;
			}
			break;
		case WM_LBUTTONUP:
			if (HitTest(lParam))
			{
				OnButtonClickUp();
				return UIElementMsgSuccBreak;
			}
			break;
		default:
			break;
		}
	}
	return UIElementMsgSuccContinue;
}

void UIButton::OnMouseHover()
{
	SetState(UIElement_BtnFloat, true);
}

void UIButton::OnMouseLeave()
{
	SetState(UIElement_BtnNormal, true);
}

void UIButton::SetCheck(bool bCheck)
{
}

bool UIButton::GetCheck()
{
	return false;
}
