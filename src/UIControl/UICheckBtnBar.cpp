#include "stdafx.h"
#include "UIControl\UICheckBtnBar.h"
#include "UIControl\UICheckBtn.h"


UICheckBtnBar::UICheckBtnBar()
{
}


UICheckBtnBar::~UICheckBtnBar()
{
}

void UICheckBtnBar::UnCkeckAll(int nExceptID/* = 0*/)
{
	for (size_t i = 0; i < m_vecChildElement.size(); i++)
	{
		UIButton * pChild = (UIButton *)m_vecChildElement[i];
		if (pChild)
		{
			if (pChild->GetMsgID() != nExceptID)
			{
				pChild->SetCheck(false);
			}
		}
	}
}
