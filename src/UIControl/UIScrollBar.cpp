#include "stdafx.h"
#include "UIControl\UIScrollBar.h"
#include "UIControl\ThemeResoueceID.h"
#include "UIControl\UIControlDef.h"
#include "ThemeManager.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
UIScrollBar::UIScrollBar()
	: m_pBtnThumb(nullptr)
	, m_bHorScroll(false)
	, m_nWholeLength(0)
	, m_nPageLength(0)
	, m_nLogicPos(0)
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
		m_pBtnThumb->MoveTo(GetThumbRect(), bRefresh);
	}
	else
	{
		m_pBtnThumb->MoveTo(GetThumbRect(), bRefresh);
	}
}

void UIScrollBar::MoveStep(bool bFore)
{
	if (!bFore)
	{
		int nPos = m_nLogicPos - m_nStep;
		SetPos(nPos);
	}
	else
	{
		int nPos = m_nLogicPos + m_nStep;
		SetPos(nPos);
	}
}

void UIScrollBar::MovePage(bool bFore)
{
	if (!bFore)
	{
		int nPos = m_nLogicPos - m_nPageLength;
		SetPos(nPos);
	}
	else
	{
		int nPos = m_nLogicPos + m_nPageLength;
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
	CPoint ptMsg(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	switch (nMsg)
	{
	case WM_LBUTTONDOWN:
		if (rtThumbArea.PtInRect(ptMsg))
		{
			if (rtThumb.PtInRect(ptMsg))
			{
				SetElementCapture();
				if (m_pBtnThumb)
				{
					m_pBtnThumb->SetState(UIButton::UIElement_BtnDown, true);
				}
				m_ptBegin = ptMsg;
				m_bMoving = true;
				return true;
			}
			if (m_bHorScroll)
			{
				if (ptMsg.x < rtThumb.left)
				{
					MovePage(false);
				}
				else if (ptMsg.x > rtThumb.right)
				{
					MovePage(true);
				}
			}
			else
			{
				if (ptMsg.y < rtThumb.top)
				{
					MovePage(false);
				}
				else if (ptMsg.y > rtThumb.bottom)
				{
					MovePage(true);
				}
			}
			
			m_bMoving = false;
			return true;
		}
		break;
	case WM_LBUTTONUP:
		if (m_bMoving && m_pBtnThumb)
		{
			ReleaseElementCapture();
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
				int nPos = m_nLogicPos + int(nOffset / fRatio);
				SetPos(nPos);
			}
			else
			{
				float fRatio = (m_nHeight - m_nWidth * 2) * 1.0f / m_nWholeLength;
				nOffset = ptMsg.y - m_ptBegin.y;
				int nPos = m_nLogicPos + int(nOffset / fRatio);
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
		rtRet.SetRect(0, 0, m_nWidth, m_nHeight);
	}
	else
	{
		rtRet.SetRect(0, 0, m_nWidth, m_nHeight);
	}
	return rtRet;
}

CRect UIScrollBar::GetThumbRect()
{
	CRect rtRet = {0};
	if (m_bHorScroll)
	{
		float fRatio = m_nWidth * 1.0f / m_nWholeLength;
		rtRet.SetRect((int)(m_nLogicPos * fRatio), 0, (int)((m_nLogicPos + m_nPageLength) * fRatio), m_nHeight);
	}
	else
	{
		float fRatio = m_nHeight * 1.0f / m_nWholeLength;
		rtRet.SetRect(0, (int)(m_nLogicPos * fRatio), m_nWidth, (int)((m_nLogicPos + m_nPageLength) * fRatio));
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

		//创建thumb
		m_pBtnThumb = new UIScrollThumbButton;
		m_pBtnThumb->Create(0, 0, 10, 10, EmThumbButton, bHorizon, hParentWnd, this);

		m_vecChildElement.push_back(m_pBtnThumb);
		return true;
	}
	return false;
}

EmUIElementMsgRet UIScrollBar::HandleMsg(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	bool bContinue = true;
	auto BtnHandleMsg = [&](UIElement * pElement)->bool
	{
		if (pElement)
		{
			if (UIElementMsgSuccBreak == pElement->HandleMsg(nMsg, wParam, lParam))
			{
				bContinue = false;
			}
			return true;
		}
		return false;
	};
	auto BtnEnterLeaveMsg = [&](UIElement * pElement)
	{
		if (pElement)
		{
			if (pElement->HitTest(lParam))
			{
				pElement->OnMouseHover();
			}
			else
			{
				pElement->OnMouseLeave();
			}
		}
	};
	if (m_setMsgFilter.count(nMsg) > 0)
	{
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
	else
	{
		SetPos(m_nLogicPos, false);
	}
	Recalculate(bRefresh);
	if (bRefresh)
	{
		Refresh();
	}
}

void UIScrollBar::SetPos(int nPos, bool bRefresh/* = true*/)
{
	bool bMsg = false;
	if (nPos > m_nWholeLength - m_nPageLength)
	{
		nPos = m_nWholeLength - m_nPageLength;
	}
	if (nPos < 0)
	{
		nPos = 0;
	}
	if (m_nLogicPos != nPos)
	{
		bMsg = true;
	}
	m_nLogicPos = nPos;
	if (bRefresh)
	{
		Refresh();
	}
	if (bMsg)
	{
		if (m_bHorScroll)
		{
			EmitMsg(EmUIElementMsg_OnHScroll, m_nID, m_nLogicPos);
		}
		else
		{
			EmitMsg(EmUIElementMsg_OnVScroll, m_nID, m_nLogicPos);
		}
	}
}

int UIScrollBar::GetPos()
{
	return m_nLogicPos;
}

void UIScrollBar::SetWholeLength(int nWholeLength, bool bRefresh/* = true*/)
{
	if (nWholeLength < 10)
	{
		nWholeLength = 10;
	}
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
	return 1.0f;
}

void UIScrollBar::DrawElement(CDC * pDC)
{
	if (m_bVisible)
	{
		COLORREF clScrollBkColor = ThemeManager::Instance().GetColor(ThemeColor_ScrollBkColor);
		int nX = GetRealX();
		int nY = GetRealY();
		pDC->FillSolidRect(nX, nY, m_nWidth, m_nHeight, clScrollBkColor);

		if (m_pBtnThumb)
		{
			m_pBtnThumb->DrawElement(pDC);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UIScrollThumbButton::UIScrollThumbButton()
	: m_bHorScroll(false)
{
}

UIScrollThumbButton::~UIScrollThumbButton()
{
	m_bHorScroll = false;
}

bool UIScrollThumbButton::Create(int nX, int nY, int nWidth, int nHeight, int nID, bool bHorScroll, HWND hParentWnd, UIElement * pParent)
{
	if (!m_bCreated)
	{
		m_bHorScroll = bHorScroll;
		Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParent);
		return true;
	}
	return false;
}

void UIScrollThumbButton::DrawElement(CDC * pDC)
{
	if (m_bVisible)
	{
		COLORREF clThumb_N = ThemeManager::Instance().GetColor(ThemeColor_ScrollThumbColor_N);
		COLORREF clThumb_F = ThemeManager::Instance().GetColor(ThemeColor_ScrollThumbColor_F);
		COLORREF clThumb_D = ThemeManager::Instance().GetColor(ThemeColor_ScrollThumbColor_D);
		COLORREF clThumb_X = ThemeManager::Instance().GetColor(ThemeColor_ScrollThumbColor_X);
		//根据状态变换颜色
		COLORREF clThumb = clThumb_N;
		switch (m_nState)
		{
		case UIElement_BtnNormal:
			clThumb = clThumb_N;
			break;
		case UIElement_BtnFloat:
			clThumb = clThumb_F;
			break;
		case UIElement_BtnDown:
			clThumb = clThumb_D;
			break;
		case UIElement_BtnXable:
			clThumb = clThumb_X;
			break;
		default:
			break;
		}

		//为了解决锯齿问题，两头的圆拿gdi+画
		Gdiplus::Graphics graphics(pDC->GetSafeHdc());
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		Gdiplus::SolidBrush brush(Gdiplus::Color(255, GetRValue(clThumb), GetGValue(clThumb), GetBValue(clThumb)));

		//绘制方式是两个圆一个矩形
		int nX = GetRealX();
		int nY = GetRealY();
		HBRUSH hBrush = CreateSolidBrush(clThumb);
		HPEN hPen = CreatePen(PS_NULL, 0, 0);
		HGDIOBJ hOldBrush = pDC->SelectObject(hBrush);
		HGDIOBJ hOldPen = pDC->SelectObject(hPen);
		//圆的直径根据横纵不同
		if (m_bHorScroll)
		{
			int nR = m_nHeight;
			graphics.FillEllipse(&brush, nX * 1.0f, nY - 0.5f, nR * 1.0f, nR * 1.0f);
			graphics.FillEllipse(&brush, nX + m_nWidth - nR * 1.0f, nY - 0.5f, nR * 1.0f, nR * 1.0f);
			pDC->FillSolidRect(nX + nR / 2, nY, m_nWidth - nR, m_nHeight, clThumb);
		}
		else
		{
			int nR = m_nWidth;
			graphics.FillEllipse(&brush, nX - 0.5f, nY * 1.0f, nR * 1.0f, nR * 1.0f);
			graphics.FillEllipse(&brush, nX - 0.5f, nY + m_nHeight - nR * 1.0f, nR * 1.0f, nR * 1.0f);
			pDC->FillSolidRect(nX, nY + nR / 2, m_nWidth, m_nHeight - nR, clThumb);
		}
		pDC->SelectObject(hOldPen);
		pDC->SelectObject(hOldBrush);
		DeleteObject(hBrush);
		DeleteObject(hPen);
	}
}
