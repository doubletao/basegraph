#include "stdafx.h"
#include "UIControl\UICheckBtn.h"


UICheckBtn::UICheckBtn()
	: m_bCheck(false)
{
}


UICheckBtn::~UICheckBtn()
{
}

void UICheckBtn::SetCheck(bool bCheck)
{
	m_bCheck = bCheck;
	if (m_bCheck)
	{
		SetState(UIElement_Down, true);
	}
	else
	{
		SetState(UIElement_Normal, true);
	}
}

bool UICheckBtn::GetCheck()
{
	return m_bCheck;
}

void UICheckBtn::OnButtonClickDown()
{
	if (m_bCheck)
	{
		//按下状态不响应down消息
	}
	else
	{
		SetState(UIElement_Down, true);
	}
}

void UICheckBtn::OnButtonClickUp()
{
	if (!m_bCheck)
	{
		m_bCheck = true;
	}
	if (m_bCheck && m_nState == UIElement_Down)
	{
		//响应按钮
		if (m_hParentWnd && ::IsWindow(m_hParentWnd))
		{
			::SendMessage(m_hParentWnd, EmUIElementMsg_BtnClick, m_nID, 0);
		}
		SetState(UIElement_Down, true);
	}
	else
	{
		SetState(UIElement_Float, true);
	}
}

void UICheckBtn::OnMouseHover()
{
	if (m_bCheck || GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		SetState(UIElement_Down, true);
	}
	else if (m_nState != UIElement_Down)
	{
		SetState(UIElement_Float, true);
	}
}

void UICheckBtn::OnMouseLeave()
{
	if (m_bCheck)
	{
		SetState(UIElement_Down, true);
	}
	else
	{
		SetState(UIElement_Normal, true);
	}
}
