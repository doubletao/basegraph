#include "stdafx.h"
#include "UIControl\UIEditNum.h"
#include <assert.h>

UIEditNum::UIEditNum()
	: m_nPrecision(0)
	, m_nMin(INT_MIN)
	, m_nMax(INT_MAX)
	, m_pUIEdit(nullptr)
{
}

UIEditNum::~UIEditNum()
{
}

bool UIEditNum::Create(int nX, int nY, int nWidth, int nHeight, int nID, DWORD dwStyleEs, HWND hParentWnd, UIElement * pParent)
{
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParent);
		m_pUIEdit = new UIEdit;
		m_pUIEdit->Create(nX, nY, nWidth, nHeight, nID, dwStyleEs, hParentWnd, this);
		m_vecChildElement.push_back(m_pUIEdit);
		return true;
	}
	return false;
}

void UIEditNum::SetPrecision(int nPrecision)
{
	m_nPrecision = nPrecision;
}

void UIEditNum::SetLimit(int nMin, int nMax)
{
	m_nMin = nMin;
	m_nMax = nMax;
}

bool UIEditNum::OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam)
{
	if (emMsg == EmUIElementMsg_OnEditEnded)
	{
		UpdateUIToValue();
		return true;
	}
	return UICombinedCtrl::OnChildMsg(emMsg, wParam, lParam);
}

void UIEditNum::UpdateValueToUI()
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
}

void UIEditNum::UpdateUIToValue()
{
	if (m_pUIEdit)
	{
		CString strValue = m_pUIEdit->GetText();
		float fValue = CommonFunction::ConvertCStringToFloat(strValue);
		m_value = fValue;
	}
}

