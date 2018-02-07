#include "stdafx.h"
#include "UIObjectBar.h"


UIObjectBar::UIObjectBar()
{
}


UIObjectBar::~UIObjectBar()
{
}

void UIObjectBar::Initialize(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd)
{
	UIElement::Initialize(nX, nY, nWidth, nHeight, 0, hParentWnd);
	m_setMsgFilter.insert(WM_LBUTTONDOWN);
	m_setMsgFilter.insert(WM_LBUTTONUP);
	m_setMsgFilter.insert(WM_MOUSEMOVE);
}

void UIObjectBar::InitializeObject()
{
	int nImgSize = 0;
	auto AddOneObjectBtn = [&](UIElementMsgDef emMsg, int nBKImgN, int nBKImgF, int nBKImgD, int nBKImgX, LPCTSTR strText, COLORREF nTextColor)
	{
		if (0 == nImgSize)
		{
			CImage * pImg = UIControl_GetImage(nBKImgN);
			if (pImg)
			{
				nImgSize = pImg->GetWidth();
			}
		}
		UIButton * pButton = new UIButton;
		int nXPos = (int)m_vecChildElement.size() * (nImgSize + 1);
		int nYPos = (m_nHeight - nImgSize) / 2;
		pButton->Create(nXPos, nYPos, nImgSize, nImgSize, emMsg, m_hParentWnd, nBKImgN, nBKImgF, nBKImgD, nBKImgX, strText, ThemeFont_SkinWndTitleText, nTextColor);
		m_vecChildElement.push_back(pButton);
	};

	AddOneObjectBtn(UIElementMsg_CreateTitle, ThemeImage_ObjectBtnBKImg_N, ThemeImage_ObjectBtnBKImg_F, ThemeImage_ObjectBtnBKImg_D, ThemeImage_ObjectBtnBKImg_X, _T("ÎÄ×Ö"), RGB(255, 255, 255));
	AddOneObjectBtn(UIElementMsg_CreateScroll, ThemeImage_ObjectBtnBKImg_N, ThemeImage_ObjectBtnBKImg_F, ThemeImage_ObjectBtnBKImg_D, ThemeImage_ObjectBtnBKImg_X, _T("¹öÆÁ"), RGB(255, 255, 255));
	AddOneObjectBtn(UIElementMsg_CreateImage, ThemeImage_ObjectBtnBKImg_N, ThemeImage_ObjectBtnBKImg_F, ThemeImage_ObjectBtnBKImg_D, ThemeImage_ObjectBtnBKImg_X, _T("Í¼Æ¬"), RGB(255, 255, 255));

	m_nWidth = (int)m_vecChildElement.size() * (nImgSize + 1);
}

void UIObjectBar::ChildHandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
	for (size_t i = 0; i < m_vecChildElement.size(); i++)
	{
		UIElement *& pChild = m_vecChildElement[i];
		if (pChild && pChild->HitTest(MapPointIn(lParam)))
		{
			pChild->HandleMsg(nMsg, lParam, wParam);
		}
	}
}

void UIObjectBar::EnterLeaveChild(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
	for (size_t i = 0; i < m_vecChildElement.size(); i++)
	{
		UIElement *& pChild = m_vecChildElement[i];
		if (pChild && pChild->HitTest(MapPointIn(lParam)))
		{
			pChild->OnMouseHover();
		}
		else
		{
			pChild->OnMouseLeave();
		}
	}
}

LPARAM UIObjectBar::MapPointIn(LPARAM lParam)
{
	LPARAM lRet = 0;
	int nX = LOWORD(lParam);
	int nY = HIWORD(lParam);
	lRet = MAKELPARAM(nX - m_nX, nY - m_nY);
	return lRet;
}

bool UIObjectBar::Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd)
{
	Initialize(nX, nY, nWidth, nHeight, 0, hParentWnd);
	InitializeObject();
	return true;
}

EmUIElementMsgRet UIObjectBar::HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
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
