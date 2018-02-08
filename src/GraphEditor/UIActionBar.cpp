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
	int nImgSize = 0;
	auto AddOneObjectBtn = [&](bool bButton, UIElementMsgDef emMsg, int nBKImgN, int nBKImgF, int nBKImgD, int nBKImgX, LPCTSTR strText, COLORREF nTextColor)
	{
		if (0 == nImgSize)
		{
			CImage * pImg = UIControl_GetImage(nBKImgN);
			if (pImg)
			{
				nImgSize = pImg->GetWidth();
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
		int nXPos = (m_nWidth - nImgSize) / 2; 
		int nYPos = (int)m_vecChildElement.size() * (nImgSize + 1);
		pButton->Create(nXPos, nYPos, nImgSize, nImgSize, emMsg, m_hParentWnd, nBKImgN, nBKImgF, nBKImgD, nBKImgX, this, strText, ThemeFont_SkinWndTitleText, nTextColor);
		m_vecChildElement.push_back(pButton);
	};

	AddOneObjectBtn(true, UIElementMsg_NewFile, ThemeImage_ObjectBtnBKImg_N, ThemeImage_ObjectBtnBKImg_F, ThemeImage_ObjectBtnBKImg_D, ThemeImage_ObjectBtnBKImg_X, _T("新建"), RGB(255, 255, 255));
	AddOneObjectBtn(true, UIElementMsg_SaveFile, ThemeImage_ObjectBtnBKImg_N, ThemeImage_ObjectBtnBKImg_F, ThemeImage_ObjectBtnBKImg_D, ThemeImage_ObjectBtnBKImg_X, _T("保存"), RGB(255, 255, 255));
	AddOneObjectBtn(true, UIElementMsg_SaveAs, ThemeImage_ObjectBtnBKImg_N, ThemeImage_ObjectBtnBKImg_F, ThemeImage_ObjectBtnBKImg_D, ThemeImage_ObjectBtnBKImg_X, _T("另存"), RGB(255, 255, 255));
	AddOneObjectBtn(false, UIElementMsg_DefaultAction, ThemeImage_ObjectBtnBKImg_N, ThemeImage_ObjectBtnBKImg_F, ThemeImage_ObjectBtnBKImg_D, ThemeImage_ObjectBtnBKImg_X, _T("默认"), RGB(255, 255, 255));
	AddOneObjectBtn(false, UIElementMsg_MoveAction, ThemeImage_ObjectBtnBKImg_N, ThemeImage_ObjectBtnBKImg_F, ThemeImage_ObjectBtnBKImg_D, ThemeImage_ObjectBtnBKImg_X, _T("移动"), RGB(255, 255, 255));
	AddOneObjectBtn(false, UIElementMsg_ZoomAction, ThemeImage_ObjectBtnBKImg_N, ThemeImage_ObjectBtnBKImg_F, ThemeImage_ObjectBtnBKImg_D, ThemeImage_ObjectBtnBKImg_X, _T("缩放"), RGB(255, 255, 255));
	AddOneObjectBtn(false, UIElementMsg_RotateAction, ThemeImage_ObjectBtnBKImg_N, ThemeImage_ObjectBtnBKImg_F, ThemeImage_ObjectBtnBKImg_D, ThemeImage_ObjectBtnBKImg_X, _T("旋转"), RGB(255, 255, 255));
	AddOneObjectBtn(true, UIElementMsg_SystemSetting, ThemeImage_ObjectBtnBKImg_N, ThemeImage_ObjectBtnBKImg_F, ThemeImage_ObjectBtnBKImg_D, ThemeImage_ObjectBtnBKImg_X, _T("设置"), RGB(255, 255, 255));
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
	OutputDebugString(_T("UpdateStatus\n"));
	UnCkeckAll(GraphEditorStatusManager::Instance().GetCurrentActionTool());
}
