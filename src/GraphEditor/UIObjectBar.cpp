#include "stdafx.h"
#include "UIObjectBar.h"


UIObjectBar::UIObjectBar()
{
}


UIObjectBar::~UIObjectBar()
{
}

void UIObjectBar::InitializeObject()
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
		int nXPos = (int)m_vecChildElement.size() * (nImgSize + 1);
		int nYPos = (m_nHeight - nImgSize) / 2;
		pCheck->Create(nXPos, nYPos, nImgSize, nImgSize, emMsg, m_hParentWnd, nBKImgN, nBKImgF, nBKImgD, nBKImgX, this, strText, ThemeFont_SkinWndTitleText, nTextColor);
		m_vecChildElement.push_back(pCheck);
	};

	AddOneObjectBtn(UIElementMsg_CreateTitle, ThemeImage_ObjectBtnBKImg_N, ThemeImage_ObjectBtnBKImg_F, ThemeImage_ObjectBtnBKImg_D, ThemeImage_ObjectBtnBKImg_X, _T("ÎÄ×Ö"), RGB(255, 255, 255));
	AddOneObjectBtn(UIElementMsg_CreateScroll, ThemeImage_ObjectBtnBKImg_N, ThemeImage_ObjectBtnBKImg_F, ThemeImage_ObjectBtnBKImg_D, ThemeImage_ObjectBtnBKImg_X, _T("¹öÆÁ"), RGB(255, 255, 255));
	AddOneObjectBtn(UIElementMsg_CreateImage, ThemeImage_ObjectBtnBKImg_N, ThemeImage_ObjectBtnBKImg_F, ThemeImage_ObjectBtnBKImg_D, ThemeImage_ObjectBtnBKImg_X, _T("Í¼Æ¬"), RGB(255, 255, 255));

	m_nWidth = (int)m_vecChildElement.size() * (nImgSize + 1);
}
