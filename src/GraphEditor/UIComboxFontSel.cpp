#include "stdafx.h"
#include "UIComboxFontSel.h"

UIComboxFontSel::UIComboxFontSel()
	: m_pStatic(nullptr)
	, m_pComboBoxFont(nullptr)
{
}

UIComboxFontSel::~UIComboxFontSel()
{
}

BaseGraph_Common::Value UIComboxFontSel::GetValue()
{
	return m_value;
}

void UIComboxFontSel::SetValue(BaseGraph_Common::Value value, bool bRefresh)
{
	m_value = value;
	if (bRefresh)
	{
		UpdateValueToUI();
	}
}

void UIComboxFontSel::UpdateValueToUI()
{
	CString strFontName = m_value.ToCString();
	if (m_pComboBoxFont)
	{
		m_pComboBoxFont->SelectString(-1, strFontName);
	}
}

void UIComboxFontSel::UpdateUIToValue()
{
	if (m_pComboBoxFont)
	{
		CString strFontName;
		m_pComboBoxFont->GetWindowText(strFontName);
		m_value.SetValue(strFontName);
	}
}

bool UIComboxFontSel::Create(CString strTitle, int nTitleWidth, int nID, HWND hParentWnd, UIElement * pParent)
{
	if (!m_bCreated)
	{
		int nBtnHeight = UIControl_GetIntValue(ThemeInt_PropertyViewComboBoxBtnHeight);
		int nBtnWidth = UIControl_GetIntValue(ThemeInt_PropertyViewComboBoxBtnWidth);
		Initialize(0, 0, 100, nBtnHeight, nID, hParentWnd, pParent);

		m_pStatic = new UIStatic;
		m_pStatic->Create(0, 0, nTitleWidth, nBtnHeight, 0, nullptr, this, strTitle, ThemeFont_CustomEditCtr, UIControl_GetColor(ThemeColor_CustomStatic));
		m_vecChildElement.push_back(m_pStatic);

		m_pComboBoxFont = new UIComboBox;
		m_pComboBoxFont->Create(0, 0, nBtnWidth, nBtnHeight, 0, 0, hParentWnd, this);
		m_vecChildElement.push_back(m_pComboBoxFont);
		//m_pComboBoxFont = new CComboBox;
		//m_pComboBoxFont->Create(WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL, CRect(0, 0, nBtnWidth, nBtnHeight), CWnd::FromHandle(hParentWnd), 10423);
		std::list<CString> lstFontName = UIControl_GetChnFontList();
		for (auto it : lstFontName)
		{
			m_pComboBoxFont->AddString(it);
		}

		return true;
	}
	return false;
}

bool UIComboxFontSel::OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam)
{
	if (emMsg == EmUIElementMsg_OnComboBoxSelChged)
	{
		UpdateUIToValue();
		NotifyValueChg();
		return true;
	}
	return UIElement::OnChildMsg(emMsg, wParam, lParam);
}

void UIComboxFontSel::RearrangeChildPos()
{
	if (m_pStatic && m_pComboBoxFont)
	{
		int nTitleWidth = m_pStatic->GetWidth();
		int nBtnWidth = m_pComboBoxFont->GetWidth();
		int nBtnHeight = m_pComboBoxFont->GetHeight();
		m_pComboBoxFont->MoveTo(m_nWidth - nBtnWidth, 0, nBtnWidth, nBtnHeight);
		//CRect rtComboBox;
		//m_pComboBoxFont->GetWindowRect(rtComboBox);
		//int nBtnWidth = rtComboBox.Width();
		//int nBtnHeight = rtComboBox.Height();
		//m_pStatic->MoveTo(0, 0, nTitleWidth, m_nHeight);
		//int nX = GetRealX();
		//int nY = GetRealY();
		//m_pComboBoxFont->MoveWindow(nX + m_nWidth - nBtnWidth, nY + 0, nBtnWidth, nBtnHeight);
	}
}
