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
		SetState(UIElement_BtnDown, true);
	}
	else
	{
		SetState(UIElement_BtnNormal, true);
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
		SetState(UIElement_BtnDown, true);
	}
}

void UICheckBtn::OnButtonClickUp()
{
	if (!m_bCheck)
	{
		m_bCheck = true;
	}
	if (m_bCheck && m_nState == UIElement_BtnDown)
	{
		//响应按钮
		EmitMsg(EmUIElementMsg_BtnClick, m_nID, (LPARAM)this);
		SetState(UIElement_BtnDown, true);
	}
	else
	{
		SetState(UIElement_BtnFloat, true);
	}
}

void UICheckBtn::OnMouseHover()
{
	if (m_bCheck || GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		SetState(UIElement_BtnDown, true);
	}
	else if (m_nState != UIElement_BtnDown)
	{
		SetState(UIElement_BtnFloat, true);
	}
}

void UICheckBtn::OnMouseLeave()
{
	if (m_bCheck)
	{
		SetState(UIElement_BtnDown, true);
	}
	else
	{
		SetState(UIElement_BtnNormal, true);
	}
}
