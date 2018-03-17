#include "stdafx.h"
#include "UIControl\UISliderCtr.h"
#include "UIControl\ThemeResoueceID.h"
#include "UIControl\UIControlDef.h"
#include "ThemeManager.h"

UISliderCtr::UISliderCtr()
	: m_bHorScroll(true)
	, m_nMin(0)
	, m_nMax(0)
	, m_nLogicPos(0)
	, m_bMoving(false)
	, m_ptBegin(0)
	, m_ptEnd(0)
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
		fLogicPos = 0.0f;
	}
	else if (fLogicPos > fRealLength)
	{
		fLogicPos = fRealLength;
	}
	if (fLogicLength > 0
		&& fRealLength > 0)
	{
		fLogicPos = fRealPos * fLogicLength / fRealLength;
	}
	return (int)fLogicPos;
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

EmUIElementMsgRet UISliderCtr::HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
	return EmUIElementMsgRet();
}

void UISliderCtr::SetPos(int nPos, bool bRefresh, bool bChanging)
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
}
