#include "stdafx.h"
#include "UIControl\UIEditNumSlider.h"
#include "UIControl\ThemeResoueceID.h"
#include "ThemeManager.h"

UIEditNumSlider::UIEditNumSlider()
	: m_pSliderCtrl(nullptr)
{
}

UIEditNumSlider::~UIEditNumSlider()
{
}

bool UIEditNumSlider::Create(CString strTitle, int nTitleWidth, DWORD dwStyleEs, int nWidth, int nMin, int nMax, int nPrecision, float fStep, int nID, HWND hParentWnd, UIElement * pParent)
{
	if (!m_bCreated)
	{
		//步长至少是1%
		if (fStep < 0.01)
		{
			fStep = 0.01;
		}
		UIEditNumSpin::Create(strTitle, nTitleWidth, dwStyleEs, nWidth, nMin, nMax, nPrecision, fStep, nID, hParentWnd, pParent);
		m_pSliderCtrl = new UISliderCtr;
		m_pSliderCtrl->Create(0, 0, 10, m_nHeight, nMin / fStep, nMax / fStep, 0, true, hParentWnd, this);
		m_vecChildElement.push_back(m_pSliderCtrl);
	}
	return false;
}

void UIEditNumSlider::RearrangeChildPos()
{
	UIEditNumSpin::RearrangeChildPos();
	if (m_pSliderCtrl
		&& m_pBtnDownSpin
		&& m_pUIStatic)
	{
		int nSpinWidth = m_pBtnDownSpin->GetWidth();
		int nTitleWidth = m_pUIStatic->GetWidth();
		int nEditWidth = ThemeManager::Instance().GetIntValue(ThemeInt_PropertyViewCtrlNumEditWidth);
		m_pSliderCtrl->MoveTo(nTitleWidth, 0, m_nWidth - nEditWidth - nTitleWidth - nSpinWidth, m_nHeight);
	}
}

bool UIEditNumSlider::OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam)
{
	if (emMsg == EmUIElementMsg_OnSliderChanging
		|| emMsg == EmUIElementMsg_OnSliderChanged)
	{
		if (m_pSliderCtrl)
		{
			float fValue = (float)m_pSliderCtrl->GetPos();
			SetValue(fValue * m_fStep);
			NotifyValueChg();
		}
		return true;
	}
	else if (emMsg == EmUIElementMsg_BtnClick)
	{
		UIButton * pButton = (UIButton *)lParam;
		if (m_pBtnUpSpin == pButton)
		{
			float fValue = GetValue().ToFloat();
			SetValue(fValue + m_fStep);
			NotifyValueChg();
		}
		else if (m_pBtnDownSpin == pButton)
		{
			float fValue = GetValue().ToFloat();
			SetValue(fValue - m_fStep);
			NotifyValueChg();
		}
		UpdateValueToUI();
		return true;
	}

	else if (emMsg == EmUIElementMsg_OnEditEnded)
	{
		UpdateUIToValue();
		UpdateValueToUI();
		NotifyValueChg();
		return true;
	}
	return UIEditNumSpin::OnChildMsg(emMsg, wParam, lParam);
}

void UIEditNumSlider::UpdateValueToUI()
{
	//对于数字edit来说，全部使用float类型，通过精度来控制是否显示小数点
	float fValue = m_value.ToFloat();
	CString strValue = _T("0");
	if (m_nPrecision < 1)
	{
		int nValue = (int)fValue;
		strValue = CommonFunction::ConvertNumToCString(nValue);
	}
	else
	{
		strValue = CommonFunction::ConvertNumToCString(fValue, m_nPrecision);
	}
	if (m_pUIEdit)
	{
		m_pUIEdit->SetText(strValue);
	}
	if (m_pSliderCtrl)
	{
		m_pSliderCtrl->SetPos((int)(fValue / m_fStep), true, false, true);
	}
}

void UIEditNumSlider::UpdateUIToValue()
{
	if (m_pUIEdit)
	{
		CString strValue = m_pUIEdit->GetText();
		float fValue = CommonFunction::ConvertCStringToFloat(strValue);
		m_value = fValue;
	}
}

