#include "stdafx.h"
#include "UIControl\UISliderCtr.h"
#include "UIControl\ThemeResoueceID.h"
#include "UIControl\UIControlDef.h"
#include "ThemeManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
UISliderCtr::UISliderCtr()
	: m_bHorScroll(true)
	, m_nMin(0)
	, m_nMax(0)
	, m_nLogicPos(50)
	, m_bMoving(false)
	, m_pSliderBtn(nullptr)
{
}

UISliderCtr::~UISliderCtr()
{
}

void UISliderCtr::RearrangeChildPos()
{
	Recalculate(true);
}

void UISliderCtr::Recalculate(bool bRefresh)
{
	int nRealPos = LogicToReal(m_nLogicPos);
	if (m_pSliderBtn)
	{
		if (m_bHorScroll)
		{
			m_pSliderBtn->MoveTo(nRealPos, 0, m_nHeight, m_nHeight, bRefresh);
		}
		else
		{
			m_pSliderBtn->MoveTo(0, nRealPos, m_nWidth, m_nWidth, bRefresh);
		}
	}
}

void UISliderCtr::Refresh()
{
	Recalculate(false);
	UIElement::Refresh();
}

int UISliderCtr::LogicToReal(int nLogicPos)
{
	float fLogicLength = float(m_nMax - m_nMin);
	float fLogicPos = float(nLogicPos);
	float fRealPos = 0.0f;
	if (fLogicPos < m_nMin)
	{
		fLogicPos = (float)m_nMin;
	}
	else if (fLogicPos > m_nMax)
	{
		fLogicPos = (float)m_nMax;
	}
	float fRealLength = 0.0f;
	if (m_bHorScroll)
	{
		fRealLength = (float)(m_nWidth - m_nHeight);
	}
	else
	{
		fRealLength = (float)(m_nHeight - m_nWidth);
	}
	if (fLogicLength > 0
		&& fRealLength > 0)
	{
		fRealPos = fLogicPos / fLogicLength * fRealLength;
		float fZeroOffset = (0 - m_nMin) * 1.0f / (m_nMax - m_nMin) * fRealLength;
		fRealPos += fZeroOffset;
	}
	return (int)fRealPos;
}

int UISliderCtr::RealToLogic(int nRealPos)
{
	float fLogicLength = float(m_nMax - m_nMin);
	float fLogicPos = (float)m_nMin;
	float fRealLength = 0.0f;
	if (m_bHorScroll)
	{
		fRealLength = (float)(m_nWidth - m_nHeight);
	}
	else
	{
		fRealLength = (float)(m_nHeight - m_nWidth);
	}
	float fRealPos = float(nRealPos);
	if (fRealPos < 0)
	{
		fRealPos = 0;
	}
	else if (fRealPos > fRealLength)
	{
		fRealPos = fRealLength;
	}
	if (fLogicLength > 0
		&& fRealLength > 0)
	{
		fLogicPos = fRealPos * fLogicLength / fRealLength;
		float fZeroOffset = (0 - m_nMin) * 1.0f / (m_nMax - m_nMin) * fLogicLength;
		fLogicPos -= fZeroOffset;
	}
	return (int)fLogicPos;
}

bool UISliderCtr::SliderMsgHandle(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
	int nRealX = GetRealX();
	int nRealY = GetRealY();
	CRect rtCtrl = GetRect();
	CRect rtBtn = 0;
	if (m_pSliderBtn)
	{
		rtBtn = m_pSliderBtn->GetRect();
	}
	auto MoveButton = [&](const CPoint & ptMsg)
	{
		if (m_bHorScroll)
		{
			int nReal = ptMsg.x - GetRealX() - m_nHeight / 2;
			int nLogicPos = RealToLogic(nReal);
			SetPos(nLogicPos, true, true, false);
		}
		else
		{
			int nReal = ptMsg.y - GetRealY() - m_nWidth / 2;
			int nLogicPos = RealToLogic(nReal);
			SetPos(nLogicPos, true, true, false);
		}
	};
	CPoint ptMsg(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	switch (nMsg)
	{
	case WM_LBUTTONDOWN:
		if (rtCtrl.PtInRect(ptMsg))
		{
			SetElementCapture();
			//控件范围内的单击都会导致滑块的按下效果
			if (m_pSliderBtn)
			{
				m_pSliderBtn->SetState(UIButton::UIElement_BtnDown, true);
			}
			m_bMoving = true;
			if (!rtBtn.PtInRect(ptMsg))
			{
				//如果按在了滑块内部，则滑块本身是不移动的，否则将滑块滑动到鼠标点下去的位置
				MoveButton(ptMsg);
			}
			return true;
		}
		break;
	case WM_LBUTTONUP:
		if (m_bMoving && m_pSliderBtn)
		{
			ReleaseElementCapture();
			m_pSliderBtn->SetState(UIButton::UIElement_BtnFloat, true);
			SetPos(m_nLogicPos, true, false, false);
			m_bMoving = false;
			return true;
		}
		break;
	case WM_MOUSEMOVE:
		if (m_bMoving)
		{
			MoveButton(ptMsg);
			Refresh();
			return true;
		}
		break;
	default:
		break;
	}
	return false;
}

bool UISliderCtr::Create(int nX, int nY, int nWidth, int nHeight, int nMin, int nMax, int nID, bool bHorizon, HWND hParentWnd, UIElement * pParent)
{
	if (!m_bCreated)
	{
		UIElement::Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParent);
		m_setMsgFilter.insert(WM_LBUTTONDOWN);
		m_setMsgFilter.insert(WM_LBUTTONUP);
		m_setMsgFilter.insert(WM_MOUSEMOVE);

		m_nMin = nMin;
		m_nMax = nMax;
		m_bHorScroll = bHorizon;

		m_pSliderBtn = new UISliderButton;
		m_pSliderBtn->Create(0, 0, 10, 10, EMSliderBtn, hParentWnd, this);
		m_vecChildElement.push_back(m_pSliderBtn);

		return true;
	}
	return false;
}

EmUIElementMsgRet UISliderCtr::HandleMsg(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_setMsgFilter.count(nMsg) > 0)
	{
		if (!SliderMsgHandle(nMsg, lParam, wParam))
		{
			if (nMsg == WM_MOUSEMOVE)
			{
				if (m_pSliderBtn)
				{
					if (HitTest(lParam))
					{
						m_pSliderBtn->OnMouseHover();
					}
					else
					{
						m_pSliderBtn->OnMouseLeave();
					}
				}
			}
		}
	}
	return UIElementMsgSuccContinue;
}

void UISliderCtr::SetPos(int nPos, bool bRefresh, bool bChanging, bool bForbidMsg)
{
	if (nPos < m_nMin)
	{
		nPos = m_nMin;
	}
	else if (nPos > m_nMax)
	{
		nPos = m_nMax;
	}
	bool bMsg = m_nLogicPos != nPos;
	m_nLogicPos = nPos;
	if (bRefresh)
	{
		Refresh();
	}
	if (!bForbidMsg)
	{
		if (bChanging)
		{
			if (bMsg)
			{
				EmitMsg(EmUIElementMsg_OnSliderChanging, m_nID, m_nLogicPos);
			}
		}
		else
		{
			EmitMsg(EmUIElementMsg_OnSliderChanged, m_nID, m_nLogicPos);
		}
	}
	//TRACE(_T("UISliderCtr::SetPos %d\n"), m_nLogicPos);
}

int UISliderCtr::GetPos()
{
	return m_nLogicPos;
}

void UISliderCtr::SetLimit(int nMin, int nMax, bool bRefresh)
{
	m_nMin = nMin;
	m_nMax = nMax;
	if (bRefresh)
	{
		Refresh();
	}
}

int UISliderCtr::GetMax()
{
	return m_nMax;
}

int UISliderCtr::GetMin()
{
	return m_nMin;
}

void UISliderCtr::DrawElement(CDC * pDC)
{
	if (m_bVisible)
	{
		//条的绘制分两部分，左（上）边绘制亮色，右（下）边绘制暗色，两头画两个圆
		COLORREF clLightArea = ThemeManager::Instance().GetColor(ThemeColor_SliderLightArea);
		COLORREF clDarkArea = ThemeManager::Instance().GetColor(ThemeColor_SliderDarkArea);

		Gdiplus::Graphics graphics(pDC->GetSafeHdc());
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		Gdiplus::SolidBrush brushLight(Gdiplus::Color(255, GetRValue(clLightArea), GetGValue(clLightArea), GetBValue(clLightArea)));
		Gdiplus::SolidBrush brushDark(Gdiplus::Color(255, GetRValue(clDarkArea), GetGValue(clDarkArea), GetBValue(clDarkArea)));

		//绘制方式是两个圆两个矩形
		int nX = GetRealX();
		int nY = GetRealY();
		float fRealPos = (float)LogicToReal(m_nLogicPos);

		//圆的直径根据横纵不同
		if (m_bHorScroll)
		{
			int nR = m_nHeight;
			graphics.FillEllipse(&brushLight, nX * 1.0f, nY * 1.0f, nR * 1.0f, nR * 1.0f);
			graphics.FillEllipse(&brushDark, nX + m_nWidth - nR * 1.0f, nY * 1.0f, nR * 1.0f, nR * 1.0f);
			graphics.FillRectangle(&brushLight, nX * 1.0f + nR / 2, nY * 1.0f, fRealPos, m_nHeight * 1.0f);
			graphics.FillRectangle(&brushDark, nX * 1.0f + nR / 2 + fRealPos, nY * 1.0f, m_nWidth - fRealPos - nR, m_nHeight * 1.0f);
		}
		else
		{
			int nR = m_nWidth;
			graphics.FillEllipse(&brushLight, nX * 1.0f, nY * 1.0f, nR * 1.0f, nR * 1.0f);
			graphics.FillEllipse(&brushDark, nX * 1.0f, nY + m_nHeight - nR * 1.0f, nR * 1.0f, nR * 1.0f);
			graphics.FillRectangle(&brushLight, nX * 1.0f, nY * 1.0f + nR / 2, m_nWidth * 1.0f, fRealPos);
			graphics.FillRectangle(&brushDark, nX * 1.0f, nY * 1.0f + nR / 2 + fRealPos, m_nWidth * 1.0f, m_nHeight * 1.0f - fRealPos - nR);
		}

		//调button的绘制逻辑
		if (m_pSliderBtn)
		{
			m_pSliderBtn->DrawElement(pDC);
		}
	}
}

void UISliderCtr::SetVisible(bool bVisible, bool bRefresh)
{
	m_bVisible = bVisible;
	for (auto it : m_vecChildElement)
	{
		if (it)
		{
			it->SetVisible(bVisible, bRefresh);
		}
	}
	if (bRefresh)
	{
		Refresh();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UISliderButton::UISliderButton()
{
}

UISliderButton::~UISliderButton()
{
}

bool UISliderButton::Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UIElement * pParent)
{
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParent);
		return true;
	}
	return false;
}

void UISliderButton::DrawElement(CDC * pDC)
{
	if (m_bVisible)
	{
		COLORREF clBtn_N = ThemeManager::Instance().GetColor(ThemeColor_SliderButton_N);
		COLORREF clBtn_F = ThemeManager::Instance().GetColor(ThemeColor_SliderButton_F);
		COLORREF clBtn_D = ThemeManager::Instance().GetColor(ThemeColor_SliderButton_D);
		COLORREF clBtn_X = ThemeManager::Instance().GetColor(ThemeColor_SliderButton_X);
		COLORREF clBtn = clBtn_N;
		switch (m_nState)
		{
		case UIElement_BtnNormal:
			clBtn = clBtn_N;
			break;
		case UIElement_BtnFloat:
			clBtn = clBtn_F;
			break;
		case UIElement_BtnDown:
			clBtn = clBtn_D;
			break;
		case UIElement_BtnXable:
			clBtn = clBtn_X;
			break;
		default:
			break;
		}

		Gdiplus::Graphics graphics(pDC->GetSafeHdc());
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		Gdiplus::SolidBrush brush(Gdiplus::Color(255, GetRValue(clBtn), GetGValue(clBtn), GetBValue(clBtn)));
		graphics.FillEllipse(&brush, GetRealX(), GetRealY(), m_nWidth, m_nHeight);
	}
}
