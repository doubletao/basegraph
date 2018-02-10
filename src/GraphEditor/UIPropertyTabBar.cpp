#include "stdafx.h"
#include "UIPropertyTabBar.h"


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
		m_strText = _T("属性Tab栏");
	};
	bool bRet = UICheckBtnBar::Create(nX, nY, nWidth, nHeight, hParentWnd);
	InsertOneDrawer(0, ThemeImage_SkinWndNcBkgrd);
	m_pParentElement = pParent;
	return bRet;
}

void UIPropertyTabBar::InitializeObject()
{
	int nImgSize = 0;
	auto AddOneObjectBtn = [&](UIElementMsgDef emMsg, int nBKImgN, int nBKImgF, int nBKImgD, int nBKImgX, LPCTSTR strText, COLORREF nTextColor)
	{
		if (0 == nImgSize)
		{
			CImage * pImg = UIControl_GetImage(nBKImgN);
			if (pImg)
			{
				nImgSize = pImg->GetWidth();
			}
		}
		UICheckBtn * pCheck = new UICheckBtn;
		int nXPos = (m_nWidth - nImgSize) / 2;
		int nYPos = (int)m_vecChildElement.size() * (nImgSize + 1);
		pCheck->Create(nXPos, nYPos, nImgSize, nImgSize, emMsg, m_hParentWnd, nBKImgN, nBKImgF, nBKImgD, nBKImgX, this, strText, ThemeFont_SkinWndTitleText, nTextColor);
		m_vecChildElement.push_back(pCheck);
	};

	AddOneObjectBtn(UIElementMsg_PropertyTabColor, ThemeImage_ObjectBtnBKImg_N, ThemeImage_ObjectBtnBKImg_F, ThemeImage_ObjectBtnBKImg_D, ThemeImage_ObjectBtnBKImg_X, _T("颜色"), RGB(255, 255, 255));
	AddOneObjectBtn(UIElementMsg_PropertyTabObject, ThemeImage_ObjectBtnBKImg_N, ThemeImage_ObjectBtnBKImg_F, ThemeImage_ObjectBtnBKImg_D, ThemeImage_ObjectBtnBKImg_X, _T("物件"), RGB(255, 255, 255));
	AddOneObjectBtn(UIElementMsg_PropertyTabSpace, ThemeImage_ObjectBtnBKImg_N, ThemeImage_ObjectBtnBKImg_F, ThemeImage_ObjectBtnBKImg_D, ThemeImage_ObjectBtnBKImg_X, _T("空间"), RGB(255, 255, 255));
	AddOneObjectBtn(UIElementMsg_PropertyTabFx, ThemeImage_ObjectBtnBKImg_N, ThemeImage_ObjectBtnBKImg_F, ThemeImage_ObjectBtnBKImg_D, ThemeImage_ObjectBtnBKImg_X, _T("特技"), RGB(255, 255, 255));
	AddOneObjectBtn(UIElementMsg_PropertyTabText, ThemeImage_ObjectBtnBKImg_N, ThemeImage_ObjectBtnBKImg_F, ThemeImage_ObjectBtnBKImg_D, ThemeImage_ObjectBtnBKImg_X, _T("文本"), RGB(255, 255, 255));
}

void UIPropertyTabBar::RearrangeChildPos()
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
