#include "stdafx.h"
#include "UIControl\UICheckBtnBar.h"
#include "UIControl\UICheckBtn.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
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

void UICheckBtnBar::CkeckOne(int nCheckID)
{
	for (size_t i = 0; i < m_vecChildElement.size(); i++)
	{
		UIButton * pChild = (UIButton *)m_vecChildElement[i];
		if (pChild)
		{
			if (pChild->GetMsgID() != nCheckID)
			{
				pChild->SetCheck(false);
			}
			else
			{
				pChild->SetCheck(true);
			}
		}
	}
}
