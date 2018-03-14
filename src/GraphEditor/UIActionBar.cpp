#include "stdafx.h"
#include "UIActionBar.h"
#include "GraphEditorStatusManager.h"

UIActionBar::UIActionBar()
{
}


UIActionBar::~UIActionBar()
{
}

void UIActionBar::InitializeObject()
{
	int nImgWidth = 0;
	int nImgHeight = 0;
	auto AddOneObjectBtn = [&](bool bButton, UIElementMsgDef emMsg, int nBKImgN, int nBKImgF, int nBKImgD, int nBKImgX)
	{
		if (0 == nImgWidth
			|| 0 == nImgHeight)
		{
			CImage * pImg = UIControl_GetImage(nBKImgN);
			if (pImg)
			{
				nImgWidth = pImg->GetWidth();
				nImgHeight = pImg->GetHeight();
			}
		}
		UIButton * pButton = nullptr;
		if (bButton)
		{
			pButton = new UIButton;
		}
		else
		{
			pButton = new UICheckBtn;
		}
		int nXPos = (m_nWidth - nImgWidth) / 2; 
		int nYPos = (int)m_vecChildElement.size() * (nImgHeight + 1);
		pButton->Create(nXPos, nYPos, nImgWidth, nImgHeight, emMsg, m_hParentWnd, nBKImgN, nBKImgF, nBKImgD, nBKImgX, this);
		m_vecChildElement.push_back(pButton);
	};

	AddOneObjectBtn(true, UIElementMsg_NewFile, ThemeImage_BtnNewFile_N, ThemeImage_BtnNewFile_F, ThemeImage_BtnNewFile_D, ThemeImage_BtnNewFile_X);
	AddOneObjectBtn(true, UIElementMsg_SaveFile, ThemeImage_BtnSave_N, ThemeImage_BtnSave_F, ThemeImage_BtnSave_D, ThemeImage_BtnSave_X);
	AddOneObjectBtn(true, UIElementMsg_SaveAs, ThemeImage_BtnSaveAs_N, ThemeImage_BtnSaveAs_F, ThemeImage_BtnSaveAs_D, ThemeImage_BtnSaveAs_X);
	AddOneObjectBtn(false, UIElementMsg_DefaultAction, ThemeImage_BtnDefaultAction_N, ThemeImage_BtnDefaultAction_F, ThemeImage_BtnDefaultAction_D, ThemeImage_BtnDefaultAction_X);
	AddOneObjectBtn(false, UIElementMsg_ZoomAction, ThemeImage_BtnScaleAction_N, ThemeImage_BtnScaleAction_F, ThemeImage_BtnScaleAction_D, ThemeImage_BtnScaleAction_X);
	AddOneObjectBtn(false, UIElementMsg_RotateAction, ThemeImage_BtnRotateAction_N, ThemeImage_BtnRotateAction_F, ThemeImage_BtnRotateAction_D, ThemeImage_BtnRotateAction_X);
	AddOneObjectBtn(true, UIElementMsg_SystemSetting, ThemeImage_BtnSysSetting_N, ThemeImage_BtnSysSetting_F, ThemeImage_BtnSysSetting_D, ThemeImage_BtnSysSetting_X);
}

void UIActionBar::RearrangeChildPos()
{
	for (size_t i = 0; i < m_vecChildElement.size(); i++)
	{
		UIElement *& m_pChild = m_vecChildElement[i];
		if (m_pChild)
		{
			int nXPos = (m_nWidth - m_pChild->GetWidth()) / 2;
			int nYPos = (int)i * (m_pChild->GetWidth() + 1);
			m_pChild->MoveTo(nXPos, nYPos);
		}
	}
}

void UIActionBar::UpdateStatus()
{
	//OutputDebugString(_T("UpdateStatus\n"));
	UnCkeckAll(GraphEditorStatusManager::Instance().GetCurrentActionTool());
}
