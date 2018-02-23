#include "stdafx.h"
#include "UIControl\UIScrollBar.h"
#include "UIControl\ThemeResoueceID.h"
#include "UIControl\UIControlDef.h"


UIScrollBar::UIScrollBar()
	: m_pBtnLeftTop(nullptr)
	, m_pBtnRightBottom(nullptr)
	, m_pBtnThumb(nullptr)
	, m_bHorScroll(false)
	, m_nWholeLength(0)
	, m_nPageLength(0)
	, m_nPos(0)
	, m_nStep(50)
	, m_bMoving(false)
	, m_ptBegin(0)
	, m_ptEnd(0)
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
		m_pBtnThumb->MoveTo(GetThumbRect(), bRefresh);
	}
	else
	{
		float fRatio = m_nWholeLength * 1.0f / (m_nHeight - m_nWidth * 2);
		m_pBtnLeftTop->MoveTo(0, 0, m_nWidth, m_nWidth, bRefresh);
		m_pBtnRightBottom->MoveTo(0, m_nHeight - m_nWidth, m_nWidth, m_nWidth, bRefresh);
		m_pBtnThumb->MoveTo(GetThumbRect(), bRefresh);
	}
}

void UIScrollBar::MoveStep(bool bFore)
{
	if (!bFore)
	{
		int nPos = m_nPos - m_nStep;
		SetPos(nPos);
	}
	else
	{
		int nPos = m_nPos + m_nStep;
		SetPos(nPos);
	}
}

void UIScrollBar::MovePage(bool bFore)
{
	if (!bFore)
	{
		int nPos = m_nPos - m_nPageLength;
		SetPos(nPos);
	}
	else
	{
		int nPos = m_nPos + m_nPageLength;
		SetPos(nPos);
	}
}

bool UIScrollBar::ThumbAreaMsgHandler(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
	int nRealX = GetRealX();
	int nRealY = GetRealY();
	CRect rtThumbArea = GetThumbAreaRect();
	rtThumbArea.OffsetRect(nRealX, nRealY);
	CRect rtThumb = GetThumbRect();
	rtThumb.OffsetRect(nRealX, nRealY);
	CPoint ptMsg(LOWORD(lParam), HIWORD(lParam));
	switch (nMsg)
	{
	case WM_LBUTTONDOWN:
		if (rtThumbArea.PtInRect(ptMsg))
		{
			if (rtThumb.PtInRect(ptMsg))
			{
				if (m_pBtnThumb)
				{
					m_pBtnThumb->SetState(UIButton::UIElement_BtnDown, true);
				}
				m_ptBegin = ptMsg;
				m_bMoving = true;
				return true;
			}
			m_bMoving = false;
			return true;
		}
		break;
	case WM_LBUTTONUP:
		if (m_bMoving && m_pBtnThumb)
		{
			m_pBtnThumb->SetState(UIButton::UIElement_BtnFloat, true);
			m_ptBegin = 0;
			m_bMoving = false;
			return true;
		}
		break;
	case WM_MOUSEMOVE:
		if (m_bMoving)
		{
			int nOffset = 0;
			if (m_bHorScroll)
			{
				float fRatio = (m_nWidth - m_nHeight * 2) * 1.0f / m_nWholeLength;
				nOffset = ptMsg.x - m_ptBegin.x;
				int nPos = m_nPos + int(nOffset / fRatio);
				SetPos(nPos);
			}
			else
			{
				float fRatio = m_nWholeLength * 1.0f / (m_nHeight - m_nWidth * 2);
				nOffset = ptMsg.y - m_ptBegin.y;
				int nPos = m_nPos + int(nOffset / fRatio);
				SetPos(nPos);
			}
			m_ptBegin = ptMsg;
			Refresh();
			return true;
		}
		break;
	default:
		break;
	}
	return false;
}

CRect UIScrollBar::GetThumbAreaRect()
{
	CRect rtRet = { 0 };
	if (m_bHorScroll)
	{
		rtRet.SetRect(m_nHeight, 0, m_nWidth - m_nHeight, m_nHeight);
	}
	else
	{
		rtRet.SetRect(0, m_nWidth, m_nWidth, m_nHeight - m_nWidth);
	}
	return rtRet;
}

CRect UIScrollBar::GetThumbRect()
{
	CRect rtRet = {0};
	if (m_bHorScroll)
	{
		float fRatio = (m_nWidth - m_nHeight * 2) * 1.0f / m_nWholeLength;
		rtRet.SetRect((int)(m_nHeight + m_nPos * fRatio), 0, (int)(m_nHeight + (m_nPos + m_nPageLength) * fRatio), m_nHeight);
	}
	else
	{
		float fRatio = (m_nHeight - m_nWidth * 2) * 1.0f / m_nWholeLength;
		rtRet.SetRect(0, (int)(m_nWidth + m_nPos * fRatio), m_nWidth, (int)(m_nWidth + (m_nPos + m_nPageLength) * fRatio));
	}
	return rtRet;
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
	bool bContinue = true;
	auto BtnHandleMsg = [&](UIElement * pElement)->bool
	{
		if (pElement)
		{
			if (UIElementMsgSuccBreak == pElement->HandleMsg(nMsg, lParam, wParam))
			{
				bContinue = false;
			}
			return true;
		}
		return false;
	};
	auto BtnEnterLeaveMsg = [&](UIElement * pElement)
	{
		if (pElement && pElement->HitTest(lParam))
		{
			pElement->OnMouseHover();
		}
		else
		{
			pElement->OnMouseLeave();
		}
	};
	if (m_setMsgFilter.count(nMsg) > 0)
	{
		switch (nMsg)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			BtnHandleMsg(m_pBtnLeftTop);
			if (bContinue)
			{
				BtnHandleMsg(m_pBtnRightBottom);
			}
			break;
		case WM_MOUSEMOVE:
			BtnEnterLeaveMsg(m_pBtnLeftTop);
			BtnEnterLeaveMsg(m_pBtnRightBottom);
			if (!m_bMoving)
			{
				BtnEnterLeaveMsg(m_pBtnThumb);
			}
			break;
		default:
			break;
		}
		if (bContinue)
		{
			ThumbAreaMsgHandler(nMsg, lParam, wParam);
		}
		return UIElementMsgSuccBreak;
	}
	return UIElementMsgSuccContinue;
}

bool UIScrollBar::OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam)
{
	switch (emMsg)
	{
	case EmUIElementMsg_BtnClick:
		if (EmLeftTopButton == wParam)
		{
			MoveStep(false);
		}
		else if (EmRightBottomButton == wParam)
		{
			MoveStep(true);
		}
		break;
	default:
		break;
	}
	return true;
}

void UIScrollBar::ResetScroll(int nWholeLength, int nPageLength, int nPos/* = -1*/, bool bRefresh/* = true*/)
{
	SetWholeLength(nWholeLength, false);
	SetPageLength(nPageLength, false);
	if (nPos > -1)
	{
		SetPos(nPos, false);
	}
	if (bRefresh)
	{
		Refresh();
	}
}

void UIScrollBar::SetPos(int nPos, bool bRefresh/* = true*/)
{
	bool bMsg = false;
	if (m_nPos != nPos)
	{
		bMsg = true;
	}
	m_nPos = nPos;
	if (m_nPos < 0)
	{
		m_nPos = 0;
	}
	if (m_nPos > m_nWholeLength - m_nPageLength)
	{
		m_nPos = m_nWholeLength - m_nPageLength;
	}
	if (bRefresh)
	{
		Refresh();
	}
	if (bMsg)
	{
		if (m_bHorScroll)
		{
			EmitMsg(EmUIElementMsg_OnHScroll, m_nID, m_nPos);
		}
		else
		{
			EmitMsg(EmUIElementMsg_OnVScroll, m_nID, m_nPos);
		}
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

float UIScrollBar::GetPagePercent()
{
	if (m_nWholeLength > 0)
	{
		return m_nPageLength * 1.0f / m_nWholeLength;
	}
	return 0.0f;
}
