#include "stdafx.h"
#include "UIControl\UIFlexibleScrollBar.h"
#include "UIControl\ThemeResoueceID.h"
#include "UIControl\UIControlDef.h"
#include "ThemeManager.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
UIFlexibleScrollBar::UIFlexibleScrollBar()
{
}


UIFlexibleScrollBar::~UIFlexibleScrollBar()
{
}

void UIFlexibleScrollBar::Initialize(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UIElement * pParent)
{
	UIElement::Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParent);
	m_setMsgFilter.insert(WM_LBUTTONDBLCLK);
	m_setMsgFilter.insert(WM_LBUTTONDOWN);
	m_setMsgFilter.insert(WM_LBUTTONUP);
	m_setMsgFilter.insert(WM_MOUSEMOVE);
}

bool UIFlexibleScrollBar::ThumbAreaMsgHandler(UINT nMsg, LPARAM lParam, WPARAM wParam)
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

bool UIFlexibleScrollBar::Create(int nX, int nY, int nWidth, int nHeight, int nWholeLength, int nPageLength, int nID, bool bHorizon, HWND hParentWnd, UIElement * pParent)
{
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParent);

		m_nWholeLength = nWholeLength;
		m_nPageLength = nPageLength;
		m_bHorScroll = bHorizon;

		//创建thumb
		m_pBtnThumb = new UIFlexibleScrollThumbButton;
		m_pBtnThumb->Create(0, 0, 10, 10, EmThumbButton, bHorizon, hParentWnd, this);

		m_vecChildElement.push_back(m_pBtnThumb);
		return true;
	}
	return false;
}

bool UIFlexibleScrollBar::OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam)
{
	switch (emMsg)
	{
	default:
		break;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UIFlexibleScrollThumbButton::UIFlexibleScrollThumbButton()
{
}

UIFlexibleScrollThumbButton::~UIFlexibleScrollThumbButton()
{
	m_bHorScroll = false;
}

bool UIFlexibleScrollThumbButton::Create(int nX, int nY, int nWidth, int nHeight, int nID, bool bHorScroll, HWND hParentWnd, UIElement * pParent)
{
	if (!m_bCreated)
	{
		m_bHorScroll = bHorScroll;
		Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParent);
		return true;
	}
	return false;
}

void UIFlexibleScrollThumbButton::DrawElement(CDC * pDC)
{
	if (m_bVisible)
	{
		COLORREF clThumb_N = ThemeManager::Instance().GetColor(ThemeColor_ScrollThumbColor_N);
		COLORREF clThumb_F = ThemeManager::Instance().GetColor(ThemeColor_ScrollThumbColor_F);
		COLORREF clThumb_D = ThemeManager::Instance().GetColor(ThemeColor_ScrollThumbColor_D);
		COLORREF clThumb_X = ThemeManager::Instance().GetColor(ThemeColor_ScrollThumbColor_X);
		COLORREF clThumbEndArea = ThemeManager::Instance().GetColor(ThemeColor_FlexibleScrollThumbColor);//两端半圆的颜色
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
		Gdiplus::SolidBrush brush(Gdiplus::Color(255, GetRValue(clThumbEndArea), GetGValue(clThumbEndArea), GetBValue(clThumbEndArea)));

		//绘制方式是两个圆一个矩形
		int nX = GetRealX();
		int nY = GetRealY();
		HBRUSH hBrush = CreateSolidBrush(clThumbEndArea);
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
