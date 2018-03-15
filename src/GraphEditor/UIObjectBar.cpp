#include "stdafx.h"
#include "UIObjectBar.h"
#include "GraphEditorStatusManager.h"

UIObjectBar::UIObjectBar()
{
}


UIObjectBar::~UIObjectBar()
{
}

void UIObjectBar::InitializeObject()
{
	int nImgWidth = 0;
	int nImgHeight = 0;
	auto AddOneObjectBtn = [&](UIElementMsgDef emMsg, int nBKImgN, int nBKImgF, int nBKImgD, int nBKImgX)
	{
		if (0 == nImgWidth
			|| 0== nImgHeight)
		{
			CImage * pImg = UIControl_GetImage(nBKImgN);
			if (pImg)
			{
				nImgWidth = pImg->GetWidth();
				nImgHeight = pImg->GetHeight();
			}
		}
		UICheckBtn * pCheck = new UICheckBtn;
		int nXPos = (int)m_vecChildElement.size() * (nImgWidth + 1);
		int nYPos = (m_nHeight - nImgHeight) / 2;
		pCheck->Create(nXPos, nYPos, nImgWidth, nImgHeight, emMsg, m_hParentWnd, nBKImgN, nBKImgF, nBKImgD, nBKImgX, this);
		m_vecChildElement.push_back(pCheck);
	};

	AddOneObjectBtn(UIElementMsg_CreateTitle, ThemeImage_BtnObjTitleWord_N, ThemeImage_BtnObjTitleWord_F, ThemeImage_BtnObjTitleWord_D, ThemeImage_BtnObjTitleWord_X);
	AddOneObjectBtn(UIElementMsg_CreateScroll, ThemeImage_BtnObjScroll_N, ThemeImage_BtnObjScroll_F, ThemeImage_BtnObjScroll_D, ThemeImage_BtnObjScroll_X);
	AddOneObjectBtn(UIElementMsg_CreateImage, ThemeImage_BtnObjImage_N, ThemeImage_BtnObjImage_F, ThemeImage_BtnObjImage_D, ThemeImage_BtnObjImage_X);

	m_nWidth = (int)m_vecChildElement.size() * (nImgWidth + 1);
}

void UIObjectBar::RearrangeChildPos()
{
	for (size_t i = 0; i < m_vecChildElement.size(); i++)
	{
		UIElement *& m_pChild = m_vecChildElement[i];
		if (m_pChild)
		{
			int nXPos = (int)i * (m_pChild->GetWidth() + 1); 
			int nYPos = (m_nHeight - m_pChild->GetHeight()) / 2;
			m_pChild->MoveTo(nXPos, nYPos);
		}
	}
}

void UIObjectBar::UpdateStatus()
{
	UnCkeckAll(GraphEditorStatusManager::Instance().GetCurrentObjectTool());
}
