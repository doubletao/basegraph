#include "stdafx.h"
#include "UIControl\UIEditNumSpin.h"
#include "UIControl\ThemeResoueceID.h"
#include "ThemeManager.h"

UIEditNumSpin::UIEditNumSpin()
	: m_pBtnUpSpin(nullptr)
	, m_pBtnDownSpin(nullptr)
	, m_fStep(1.0f)
{
}

UIEditNumSpin::~UIEditNumSpin()
{
}

bool UIEditNumSpin::Create(int nWidth, int nMin, int nMax, int nPrecision, float fStep, int nID, HWND hParentWnd, UIElement * pParent)
{
	bool bRet = false;
	if (!m_bCreated)
	{
		CImage * pImg = ThemeManager::Instance().GetImage(ThemeImage_BtnSpinUp_N);
		if (pImg)
		{
			int nSpinWidth = pImg->GetWidth() + 1;
			int nSpinHeight = pImg->GetHeight();
			m_nHeight = nSpinHeight * 2;

			UIEditNum::Create(0, 0, nWidth, m_nHeight, nID, 0, hParentWnd, pParent);
			m_fStep = fStep;
			m_pBtnUpSpin = new UIButton;
			m_pBtnUpSpin->Create(0, 0, 10, 10, 0, nullptr, ThemeImage_BtnSpinUp_N, ThemeImage_BtnSpinUp_F, ThemeImage_BtnSpinUp_D, ThemeImage_BtnSpinUp_X, this);
			m_vecChildElement.push_back(m_pBtnUpSpin);
			m_pBtnDownSpin = new UIButton;
			m_pBtnDownSpin->Create(0, 0, 10, 10, 0, nullptr, ThemeImage_BtnSpinDown_N, ThemeImage_BtnSpinDown_F, ThemeImage_BtnSpinDown_D, ThemeImage_BtnSpinDown_X, this);
			m_vecChildElement.push_back(m_pBtnDownSpin);
		}
	}
	return bRet;
}

bool UIEditNumSpin::OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam)
{
	if (emMsg == EmUIElementMsg_BtnClick)
	{
		UIButton * pButton = (UIButton *)lParam;
		if (m_pBtnUpSpin = pButton)
		{
			float fValue = GetValue().ToFloat();
			SetValue(fValue + m_fStep);
		}
		else if (m_pBtnDownSpin = pButton)
		{
			float fValue = GetValue().ToFloat();
			SetValue(fValue - m_fStep);
		}
	}
	return true;
}

void UIEditNumSpin::RearrangeChildPos()
{
	if (m_pBtnUpSpin && m_pBtnDownSpin)
	{
		CImage * pImg = ThemeManager::Instance().GetImage(ThemeImage_BtnSpinUp_N);
		if (pImg)
		{
			int nSpinWidth = pImg->GetWidth() + 1;
			int nSpinHeight = pImg->GetHeight();
			m_nHeight = nSpinHeight * 2;
			m_pUIEdit->MoveTo(0, 0, m_nWidth - nSpinWidth, m_nHeight);
			m_pBtnUpSpin->MoveTo(m_nWidth - nSpinWidth, 0, nSpinWidth, nSpinHeight);
			m_pBtnDownSpin->MoveTo(m_nWidth - nSpinWidth, nSpinHeight, nSpinWidth, nSpinHeight);
		}
	}
}
