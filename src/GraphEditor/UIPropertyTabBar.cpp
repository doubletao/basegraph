#include "stdafx.h"
#include "UIPropertyTabBar.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
UIPropertyTabBar::UIPropertyTabBar()
{
}


UIPropertyTabBar::~UIPropertyTabBar()
{
}

bool UIPropertyTabBar::Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd, UIElement * pParent)
{
	auto InsertOneDrawer = [&](int nStateValue, int nImgValue)
	{
		UIImageDrawer * pImgDrawer = new UIImageDrawer(nImgValue);
		m_mapDrawer[nStateValue].push_back(pImgDrawer);
		m_strText = _T("ÊôÐÔTabÀ¸");
	};
	bool bRet = UICheckBtnBar::Create(nX, nY, nWidth, nHeight, hParentWnd);
	InsertOneDrawer(0, ThemeImage_SkinWndNcBkgrd);
	m_pParentElement = pParent;
	return bRet;
}

void UIPropertyTabBar::InitializeObject()
{
	int nImgWidth = 0;
	int nImgHeight = 0;
	auto AddOneObjectBtn = [&](UIElementMsgDef emMsg, int nBKImgN, int nBKImgF, int nBKImgD, int nBKImgX)
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
		UICheckBtn * pCheck = new UICheckBtn;
		int nXPos = (m_nWidth - nImgWidth) / 2;
		int nYPos = (int)m_vecChildElement.size() * (nImgHeight + 1);
		pCheck->Create(nXPos, nYPos, nImgWidth, nImgHeight, emMsg, m_hParentWnd, nBKImgN, nBKImgF, nBKImgD, nBKImgX, this);
		m_vecChildElement.push_back(pCheck);
	};

	int nSplitWidth = UIControl_GetIntValue(ThemeInt_PropretyViewTabSplitWidth);
	int nSplitHeight = UIControl_GetIntValue(ThemeInt_PropretyViewTabSplitHeight);

	AddOneObjectBtn(UIElementMsg_PropertyTabColor, ThemeImage_TabBtnColor_N, ThemeImage_TabBtnColor_F, ThemeImage_TabBtnColor_D, ThemeImage_TabBtnColor_X);
	AddSplitLine(nSplitWidth, nSplitHeight, ThemeColor_ButtonBarSplitLine);
	AddOneObjectBtn(UIElementMsg_PropertyTabObject, ThemeImage_TabBtnObject_N, ThemeImage_TabBtnObject_F, ThemeImage_TabBtnObject_D, ThemeImage_TabBtnObject_X);
	AddSplitLine(nSplitWidth, nSplitHeight, ThemeColor_ButtonBarSplitLine);
	AddOneObjectBtn(UIElementMsg_PropertyTabSpace, ThemeImage_TabBtnSpace_N, ThemeImage_TabBtnSpace_F, ThemeImage_TabBtnSpace_D, ThemeImage_TabBtnSpace_X);
	AddSplitLine(nSplitWidth, nSplitHeight, ThemeColor_ButtonBarSplitLine);
	AddOneObjectBtn(UIElementMsg_PropertyTabFx, ThemeImage_TabBtnEffect_N, ThemeImage_TabBtnEffect_F, ThemeImage_TabBtnEffect_D, ThemeImage_TabBtnEffect_X);
	AddSplitLine(nSplitWidth, nSplitHeight, ThemeColor_ButtonBarSplitLine);
	AddOneObjectBtn(UIElementMsg_PropertyTabText, ThemeImage_TabBtnText_N, ThemeImage_TabBtnText_F, ThemeImage_TabBtnText_D, ThemeImage_TabBtnText_X);
}

void UIPropertyTabBar::RearrangeChildPos()
{
	int nButtonGap = UIControl_GetIntValue(ThemeInt_PropretyViewTabGap);
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
