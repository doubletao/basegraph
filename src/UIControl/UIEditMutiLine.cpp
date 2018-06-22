#include "stdafx.h"
#include "UIControl\UIEditMutiLine.h"
#include "UIControl\ThemeResoueceID.h"
#include "ThemeManager.h"
#include <assert.h>

UIEditMutiLine::UIEditMutiLine()
	: m_pUIEdit(nullptr)
	, m_pUIStatic(nullptr)
{
}

UIEditMutiLine::~UIEditMutiLine()
{
}

bool UIEditMutiLine::Create(CString strTitle, int nX, int nY, int nWidth, int nHeight, int nID, int nEditID, DWORD dwStyleEs, HWND hParentWnd, UIElement * pParent)
{
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParent);
		m_pUIEdit = new UIEdit;
		m_pUIEdit->Create(nX, nY, nWidth, nHeight, nID, nEditID, dwStyleEs | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | ES_WANTRETURN, hParentWnd, this);
		m_vecChildElement.push_back(m_pUIEdit);

		m_pUIStatic = new UIStatic;
		m_pUIStatic->Create(nX, nY, 10, 10, 0, nullptr, this, strTitle, ThemeFont_CustomEditCtr, ThemeManager::Instance().GetColor(ThemeColor_CustomStatic));
		m_vecChildElement.push_back(m_pUIStatic);
		return true;
	}
	return false;
}

bool UIEditMutiLine::OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam)
{
	if (emMsg == EmUIElementMsg_OnEditEnded)
	{
		UpdateUIToValue();
		return true;
	}
	return UICombinedCtrl::OnChildMsg(emMsg, wParam, lParam);
}

void UIEditMutiLine::UpdateValueToUI()
{
	if (m_pUIEdit)
	{
		CString strText = m_value.ToCString();
		m_pUIEdit->SetText(strText);
	}
}

void UIEditMutiLine::UpdateUIToValue()
{
	if (m_pUIEdit)
	{
		CString strValue = m_pUIEdit->GetText();
		m_value = strValue;
		NotifyValueChg();
	}
}

void UIEditMutiLine::RearrangeChildPos()
{
	//多行文本的排列是纵向的
	if (m_pUIEdit && m_pUIStatic)
	{
		int nStaticHeight = ThemeManager::Instance().GetIntValue(ThemeInt_PropertyViewStaticTextHeight);
		if (m_nHeight < nStaticHeight)
		{
			m_nHeight = nStaticHeight + 10;
		}
		m_pUIStatic->MoveTo(0, 0, m_nWidth, nStaticHeight);
		m_pUIEdit->MoveTo(0, nStaticHeight, m_nWidth, m_nHeight - nStaticHeight);
	}
}

