#include "stdafx.h"
#include "UIControl\UIEditNumSpin.h"
#include "UIControl\ThemeResoueceID.h"
#include "ThemeManager.h"

UIEditNumSpin::UIEditNumSpin()
	: m_pBtnUpSpin(nullptr)
	, m_pBtnDownSpin(nullptr)
	, m_pUIStatic(nullptr)
	, m_fStep(1.0f)
{
}

UIEditNumSpin::~UIEditNumSpin()
{
}

bool UIEditNumSpin::Create(CString strTitle, int nTitleWidth, DWORD dwStyleEs, int nWidth, int nMin, int nMax, int nPrecision, float fStep, int nID, HWND hParentWnd, UIElement * pParent)
{
	bool bRet = false;
	if (!m_bCreated)
	{
		SetLimit(nMin, nMax);
		m_fStep = fStep;
		m_nPrecision = nPrecision;
		CImage * pImg = ThemeManager::Instance().GetImage(ThemeImage_BtnSpinUp_N);
		if (pImg)
		{
			int nSpinWidth = pImg->GetWidth();
			int nSpinHeight = pImg->GetHeight();
			m_nHeight = nSpinHeight * 2;

			UIEditNum::Create(0, 0, nWidth, m_nHeight, nID, 0, dwStyleEs, hParentWnd, pParent);
			m_pBtnUpSpin = new UIButton;
			m_pBtnUpSpin->Create(0, 0, 10, 10, 0, nullptr, ThemeImage_BtnSpinUp_N, ThemeImage_BtnSpinUp_F, ThemeImage_BtnSpinUp_D, ThemeImage_BtnSpinUp_X, this);
			m_vecChildElement.push_back(m_pBtnUpSpin);
			m_pBtnDownSpin = new UIButton;
			m_pBtnDownSpin->Create(0, 0, 10, 10, 0, nullptr, ThemeImage_BtnSpinDown_N, ThemeImage_BtnSpinDown_F, ThemeImage_BtnSpinDown_D, ThemeImage_BtnSpinDown_X, this);
			m_vecChildElement.push_back(m_pBtnDownSpin);
			m_pUIStatic = new UIStatic;
			m_pUIStatic->Create(0, 0, nTitleWidth, 10, 0, nullptr, this, strTitle, ThemeFont_CustomEditCtr, ThemeManager::Instance().GetColor(ThemeColor_CustomStatic));
			m_vecChildElement.push_back(m_pUIStatic);
		}
		m_setMsgFilter.insert(WM_LBUTTONDOWN);
		m_setMsgFilter.insert(WM_LBUTTONUP);
		m_setMsgFilter.insert(WM_MOUSEMOVE);
		m_bCreated = true;
	}
	return bRet;
}

EmUIElementMsgRet UIEditNumSpin::HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
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

bool UIEditNumSpin::OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam)
{
	if (emMsg == EmUIElementMsg_BtnClick)
	{
		UIButton * pButton = (UIButton *)lParam;
		if (m_pBtnUpSpin == pButton)
		{
			float fValue = GetValue().ToFloat();
			SetValue(fValue + m_fStep);
		}
		else if (m_pBtnDownSpin == pButton)
		{
			float fValue = GetValue().ToFloat();
			SetValue(fValue - m_fStep);
		}
		return true;
	}
	return UIEditNum::OnChildMsg(emMsg, wParam, lParam);
}

void UIEditNumSpin::RearrangeChildPos()
{
	if (m_pBtnUpSpin && m_pBtnDownSpin && m_pUIStatic)
	{
		CImage * pImg = ThemeManager::Instance().GetImage(ThemeImage_BtnSpinUp_N);
		if (pImg)
		{
			int nSpinWidth = pImg->GetWidth();
			int nSpinHeight = pImg->GetHeight();
			int nTitleWidth = m_pUIStatic->GetWidth();
			int nEditWidth = ThemeManager::Instance().GetIntValue(ThemeInt_PropertyViewCtrlNumEditWidth);
			m_nHeight = nSpinHeight * 2;
			m_pUIStatic->MoveTo(0, 0, nTitleWidth, m_nHeight);
			m_pUIEdit->MoveTo(m_nWidth - nEditWidth - nSpinWidth, 0, nEditWidth, m_nHeight);
			m_pBtnUpSpin->MoveTo(m_nWidth - nSpinWidth, 0, nSpinWidth, nSpinHeight);
			m_pBtnDownSpin->MoveTo(m_nWidth - nSpinWidth, nSpinHeight, nSpinWidth, nSpinHeight);
		}
	}
}
