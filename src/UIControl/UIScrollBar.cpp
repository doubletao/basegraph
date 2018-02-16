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

void UIScrollBar::RearrangeChildPos()
{
	Recalculate(true);
}

void UIScrollBar::Refresh()
{
	Recalculate(false);
	UIElement::Refresh();
}

void UIScrollBar::Recalculate(bool bRefresh/* = true*/)
{
	//两个按钮一个thumb
	if (m_bHorScroll)
	{
		float fRatio = (m_nWidth - m_nHeight * 2) * 1.0f / m_nWholeLength;
		m_pBtnLeftTop->MoveTo(0, 0, m_nHeight, m_nHeight, bRefresh);
		m_pBtnRightBottom->MoveTo(m_nWidth - m_nHeight, 0, m_nHeight, m_nHeight, bRefresh);
		m_pBtnThumb->MoveTo((int)(m_nHeight + m_nPos * fRatio), 0, int(m_nPageLength * fRatio), m_nHeight, bRefresh);
	}
	else
	{
		float fRatio = m_nWholeLength * 1.0f / (m_nHeight - m_nWidth * 2);
		m_pBtnLeftTop->MoveTo(0, 0, m_nWidth, m_nWidth, bRefresh);
		m_pBtnRightBottom->MoveTo(0, m_nHeight - m_nWidth, m_nHeight, m_nHeight, bRefresh);
		m_pBtnThumb->MoveTo(0, (int)(m_nWidth + m_nPos * fRatio), m_nHeight, (int)(m_nPageLength * fRatio), bRefresh);
	}
}

bool UIScrollBar::Create(int nX, int nY, int nWidth, int nHeight, int nWholeLength, int nPageLength, int nID, bool bHorizon, HWND hParentWnd, UIElement * pParent)
{
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParent);

		m_nWholeLength = nWholeLength;
		m_nPageLength = nPageLength;
		m_bHorScroll = bHorizon;

		//创建按钮及thumb
		m_pBtnLeftTop = new UIButton;
		m_pBtnLeftTop->Create(0, 0, 10, 10, EmLeftTopButton, m_hParentWnd, ThemeImage_ScrollLeftTopBtn_N, ThemeImage_ScrollLeftTopBtn_F, ThemeImage_ScrollLeftTopBtn_D, ThemeImage_ScrollLeftTopBtn_X, this);

		m_pBtnRightBottom = new UIButton;
		m_pBtnRightBottom->Create(0, 0, 10, 10, EmRightBottomButton, m_hParentWnd, ThemeImage_ScrollRightBottomBtn_N, ThemeImage_ScrollRightBottomBtn_F, ThemeImage_ScrollRightBottomBtn_D, ThemeImage_ScrollRightBottomBtn_X, this);

		m_pBtnThumb = new UIButton;
		m_pBtnThumb->Create(0, 0, 10, 10, EmThumbButton, m_hParentWnd, ThemeImage_ScrollThumbBtn_N, ThemeImage_ScrollThumbBtn_F, ThemeImage_ScrollThumbBtn_D, ThemeImage_ScrollThumbBtn_X, this);

		m_vecChildElement.push_back(m_pBtnLeftTop);
		m_vecChildElement.push_back(m_pBtnRightBottom);
		m_vecChildElement.push_back(m_pBtnThumb);
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

void UIScrollBar::ResetScroll(int nWholeLength, int nPageLength, int nPos/* = 0*/, bool bRefresh/* = true*/)
{
	SetWholeLength(nWholeLength, false);
	SetPageLength(nPageLength, false);
	SetPos(nPos, false);
	if (bRefresh)
	{
		Refresh();
	}
}

void UIScrollBar::SetPos(int nPos, bool bRefresh/* = true*/)
{
	m_nPos = nPos;
	if (bRefresh)
	{
		Refresh();
	}
}

int UIScrollBar::GetPos()
{
	return m_nPos;
}

void UIScrollBar::SetWholeLength(int nWholeLength, bool bRefresh/* = true*/)
{
	m_nWholeLength = nWholeLength;
	if (bRefresh)
	{
		Refresh();
	}
}

int UIScrollBar::GetWholeLength()
{
	return m_nWholeLength;
}

void UIScrollBar::SetPageLength(int nPageLength, bool bRefresh/* = true*/)
{
	m_nPageLength = nPageLength;
	if (bRefresh)
	{
		Refresh();
	}
}

int UIScrollBar::GetPageLength()
{
	return m_nPageLength;
}
