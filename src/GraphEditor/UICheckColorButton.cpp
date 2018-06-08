#include "stdafx.h"
#include "UICheckColorButton.h"

UICheckColorButton::UICheckColorButton()
	: m_pCheckButton(nullptr)
	, m_pColorButton(nullptr)
{
}

UICheckColorButton::~UICheckColorButton()
{
}

BaseGraph_Common::Value UICheckColorButton::GetValue()
{
#ifdef _DEBUG
	BaseGraph_DataBase::faceinfo info;
	if (m_value.ToStruct(info))
	{
		int i = 0;
		i++;
	}
#endif
	return m_value;
}

void UICheckColorButton::SetValue(BaseGraph_Common::Value value, bool bRefresh)
{
	m_value = value;
	if (bRefresh)
	{
		UpdateValueToUI();
	}
}

void UICheckColorButton::UpdateValueToUI()
{
	BaseGraph_DataBase::faceinfo info;
	if (m_value.ToStruct(info))
	{
		bool bCheck = !info.disable;
		if (m_pCheckButton)
		{
			m_pCheckButton->SetCheck(bCheck);
		}
		sycolor color = info.color;
		if (m_pColorButton)
		{
			m_pColorButton->SetColor(color);
		}
	}
}

void UICheckColorButton::UpdateUIToValue()
{
	BaseGraph_DataBase::faceinfo info;
	if (m_value.ToStruct(info))
	{
		if (m_pCheckButton)
		{
			bool bCheck = m_pCheckButton->GetCheck();
			info.disable = !bCheck;
		}
		if (m_pColorButton)
		{       
			sycolor color = m_pColorButton->GetColor();
			info.color = color;
		}
		m_value = info;
	}
#ifdef _DEBUG
	BaseGraph_DataBase::faceinfo inf2;
	if (m_value.ToStruct(inf2))
	{
		int i = 0;
		i++;
	}
#endif
}

bool UICheckColorButton::Create(int nID, HWND hParentWnd, UIElement * pParent)
{
	if (!m_bCreated)
	{
		int nColorBtnHeight = UIControl_GetIntValue(ThemeInt_PropertyViewCtrlColorBtnHeight);
		Initialize(0, 0, 100, nColorBtnHeight, nID, hParentWnd, pParent);

		m_pCheckButton = new UICheckBtn;
		m_pCheckButton->Create(0, 0, 100, 100, 0, hParentWnd,
			ThemeImage_CheckBtn_Bk_N, ThemeImage_CheckBtn_Bk_F, ThemeImage_CheckBtn_Bk_D, ThemeImage_CheckBtn_Bk_X,
			ThemeImage_CheckBtn_Fore_N, ThemeImage_CheckBtn_Fore_F, ThemeImage_CheckBtn_Fore_D, ThemeImage_CheckBtn_Fore_X, this);
		m_vecChildElement.push_back(m_pCheckButton);

		m_pColorButton = new UIColorButton;
		m_pColorButton->Create(0, 0, 100, 100, 0, hParentWnd, this);
		m_vecChildElement.push_back(m_pColorButton);

		m_setMsgFilter.insert(WM_LBUTTONDOWN);
		m_setMsgFilter.insert(WM_LBUTTONUP);
		m_setMsgFilter.insert(WM_MOUSEMOVE);
		return true;
	}
	return false;
}

EmUIElementMsgRet UICheckColorButton::HandleMsg(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_setMsgFilter.count(nMsg) > 0)
	{
		switch (nMsg)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			ChildHandleMsg(nMsg, wParam, lParam);
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

bool UICheckColorButton::OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam)
{
	if (emMsg == EmUIElementMsg_BtnClick)
	{
		UIButton * pButton = (UIButton *)lParam;
		if (m_pCheckButton == pButton)
		{
			UpdateUIToValue();
			NotifyValueChg();
		}
		return true;
	}
	else if (emMsg == EmUIElementMsg_OnColorChanged)
	{
		UpdateUIToValue();
		NotifyValueChg();
		return true;
	}
	return UIElement::OnChildMsg(emMsg, wParam, lParam);
}

void UICheckColorButton::RearrangeChildPos()
{
	if (m_pCheckButton && m_pColorButton)
	{
		CImage * pImg = UIControl_GetImage(ThemeImage_CheckBtn_Bk_N);
		if (pImg)
		{
			int nCheckWidth = pImg->GetWidth();
			int nCheckHeight = pImg->GetHeight();
			int nColorBtnWidth = UIControl_GetIntValue(ThemeInt_PropertyViewCtrlColorBtnWidth);
			int nColorBtnHeight = UIControl_GetIntValue(ThemeInt_PropertyViewCtrlColorBtnHeight);
			m_pCheckButton->MoveTo(0, (nColorBtnHeight - nCheckHeight) / 2, nCheckWidth, nCheckHeight);
			m_pColorButton->MoveTo((m_nWidth - nCheckWidth - nColorBtnWidth) / 2, 0, nColorBtnWidth, nColorBtnHeight);
		}
	}
}
