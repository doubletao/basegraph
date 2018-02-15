#include "stdafx.h"
#include "UIControl\UIScrollBar.h"
#include "UIControl\ThemeResoueceID.h"


UIScrollBar::UIScrollBar()
	: m_pBtnLeftTop(nullptr)
	, m_pBtnRightBottom(nullptr)
	, m_pBtnThumb(nullptr)
	, m_bHorScroll(false)
	, m_nWholeLength(0)
	, m_nPageLength(0)
	, m_nPos(0)
{
}


UIScrollBar::~UIScrollBar()
{
}

void UIScrollBar::Initialize(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UIElement * pParent)
{
	UIElement::Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParent);
	m_setMsgFilter.insert(WM_LBUTTONDOWN);
	m_setMsgFilter.insert(WM_LBUTTONUP);
	m_setMsgFilter.insert(WM_MOUSEMOVE);
}

bool UIScrollBar::Create(int nX, int nY, int nWidth, int nHeight, int nWholeLength, int nPageLength, int nID, bool bHorizon, HWND hParentWnd, UIElement * pParent)
{
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParent);

		//创建按钮及thumb
		m_pBtnLeftTop = new UIButton;
		m_pBtnLeftTop->Create(0, 0, 10, 10, EmLeftTopButton, m_hParentWnd, ThemeImage_ScrollLeftTopBtn_N, ThemeImage_ScrollLeftTopBtn_F, ThemeImage_ScrollLeftTopBtn_D, ThemeImage_ScrollLeftTopBtn_X);

		m_pBtnRightBottom = new UIButton;
		m_pBtnLeftTop->Create(0, 0, 10, 10, EmRightBottomButton, m_hParentWnd, ThemeImage_ScrollRightBottomBtn_N, ThemeImage_ScrollRightBottomBtn_F, ThemeImage_ScrollRightBottomBtn_D, ThemeImage_ScrollRightBottomBtn_X);

		m_pBtnThumb = new UIButton;
		m_pBtnLeftTop->Create(0, 0, 10, 10, EmThumbButton, m_hParentWnd, ThemeImage_ScrollThumbBtn_N, ThemeImage_ScrollThumbBtn_F, ThemeImage_ScrollThumbBtn_D, ThemeImage_ScrollThumbBtn_X);

		return true;
	}
	return false;
}

EmUIElementMsgRet UIScrollBar::HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
	return EmUIElementMsgRet();
}

bool UIScrollBar::OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam)
{
	return false;
}
