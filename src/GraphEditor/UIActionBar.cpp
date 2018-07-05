#include "stdafx.h"
#include "UIActionBar.h"
#include "GraphEditorStatusManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
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
		pButton->Create(0, 0, nImgWidth, nImgHeight, emMsg, m_hParentWnd, nBKImgN, nBKImgF, nBKImgD, nBKImgX, this);
		m_vecChildElement.push_back(pButton);
	};

	int nSplitWidth = UIControl_GetIntValue(ThemeInt_ActionBarSplitWidth);
	int nSplitHeight = UIControl_GetIntValue(ThemeInt_ActionBarSplitHeight);

	AddOneObjectBtn(true, UIElementMsg_NewFile, ThemeImage_BtnNewFile_N, ThemeImage_BtnNewFile_F, ThemeImage_BtnNewFile_D, ThemeImage_BtnNewFile_X);
	AddOneObjectBtn(true, UIElementMsg_SaveFile, ThemeImage_BtnSave_N, ThemeImage_BtnSave_F, ThemeImage_BtnSave_D, ThemeImage_BtnSave_X);
	AddOneObjectBtn(true, UIElementMsg_SaveAs, ThemeImage_BtnSaveAs_N, ThemeImage_BtnSaveAs_F, ThemeImage_BtnSaveAs_D, ThemeImage_BtnSaveAs_X);
	AddSplitLine(nSplitWidth, nSplitHeight, ThemeColor_ButtonBarSplitLine);
	AddOneObjectBtn(false, UIElementMsg_DefaultAction, ThemeImage_BtnDefaultAction_N, ThemeImage_BtnDefaultAction_F, ThemeImage_BtnDefaultAction_D, ThemeImage_BtnDefaultAction_X);
	AddOneObjectBtn(false, UIElementMsg_ZoomAction, ThemeImage_BtnScaleAction_N, ThemeImage_BtnScaleAction_F, ThemeImage_BtnScaleAction_D, ThemeImage_BtnScaleAction_X);
	AddOneObjectBtn(false, UIElementMsg_RotateAction, ThemeImage_BtnRotateAction_N, ThemeImage_BtnRotateAction_F, ThemeImage_BtnRotateAction_D, ThemeImage_BtnRotateAction_X);
	AddSplitLine(nSplitWidth, nSplitHeight, ThemeColor_ButtonBarSplitLine);
	AddOneObjectBtn(true, UIElementMsg_SystemSetting, ThemeImage_BtnSysSetting_N, ThemeImage_BtnSysSetting_F, ThemeImage_BtnSysSetting_D, ThemeImage_BtnSysSetting_X);
}

void UIActionBar::RearrangeChildPos()
{
	int nButtonGap = UIControl_GetIntValue(ThemeInt_ActionBarButtonGap);
	int nYPos = nButtonGap;
	for (size_t i = 0; i < m_vecChildElement.size(); i++)
	{
		UIElement *& m_pChild = m_vecChildElement[i];
		if (m_pChild)
		{
			int nXPos = (m_nWidth - m_pChild->GetWidth()) / 2;
			m_pChild->MoveTo(nXPos, nYPos);
			nYPos += m_pChild->GetHeight() + nButtonGap;
		}
	}
}

void UIActionBar::UpdateStatus()
{
	//OutputDebugString(_T("UpdateStatus\n"));
	CkeckOne(GraphEditorStatusManager::Instance().GetCurrentUIElementTool());
}
