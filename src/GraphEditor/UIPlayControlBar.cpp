#include "stdafx.h"
#include "UIPlayControlBar.h"


UIPlayControlBar::UIPlayControlBar()
{
}


UIPlayControlBar::~UIPlayControlBar()
{
}

void UIPlayControlBar::InitializeObject()
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
		UIButton * pCheck = new UIButton;
		int nXPos = (m_nWidth - 5 * (nImgSize + 1)) / 2 + (int)m_vecChildElement.size() * (nImgSize + 1);
		int nYPos = (m_nHeight - nImgSize) / 2;
		pCheck->Create(nXPos, nYPos, nImgSize, nImgSize, emMsg, m_hParentWnd, nBKImgN, nBKImgF, nBKImgD, nBKImgX, this, strText, ThemeFont_SkinWndTitleText, nTextColor);
		m_vecChildElement.push_back(pCheck);
	};

	AddOneObjectBtn(UIElementMsg_PlayCtrlFirstFrame, ThemeImage_ObjectBtnBKImg_N, ThemeImage_ObjectBtnBKImg_F, ThemeImage_ObjectBtnBKImg_D, ThemeImage_ObjectBtnBKImg_X, _T("首帧"), RGB(255, 255, 255));
	AddOneObjectBtn(UIElementMsg_PlayCtrlPreFrame, ThemeImage_ObjectBtnBKImg_N, ThemeImage_ObjectBtnBKImg_F, ThemeImage_ObjectBtnBKImg_D, ThemeImage_ObjectBtnBKImg_X, _T("前帧"), RGB(255, 255, 255));
	AddOneObjectBtn(UIElementMsg_PlayCtrlPlayPause, ThemeImage_ObjectBtnBKImg_N, ThemeImage_ObjectBtnBKImg_F, ThemeImage_ObjectBtnBKImg_D, ThemeImage_ObjectBtnBKImg_X, _T("播放"), RGB(255, 255, 255));
	AddOneObjectBtn(UIElementMsg_PlayCtrlNextFrame, ThemeImage_ObjectBtnBKImg_N, ThemeImage_ObjectBtnBKImg_F, ThemeImage_ObjectBtnBKImg_D, ThemeImage_ObjectBtnBKImg_X, _T("后帧"), RGB(255, 255, 255));
	AddOneObjectBtn(UIElementMsg_PlayCtrlLastFrame, ThemeImage_ObjectBtnBKImg_N, ThemeImage_ObjectBtnBKImg_F, ThemeImage_ObjectBtnBKImg_D, ThemeImage_ObjectBtnBKImg_X, _T("尾帧"), RGB(255, 255, 255));
}

void UIPlayControlBar::RearrangeChildPos()
{
	for (size_t i = 0; i < m_vecChildElement.size(); i++)
	{
		UIElement *& m_pChild = m_vecChildElement[i];
		if (m_pChild)
		{
			int nXPos = (m_nWidth - 5 * (m_pChild->GetWidth() + 1)) / 2 + (int)i * (m_pChild->GetWidth() + 1);
			int nYPos = (m_nHeight - m_pChild->GetWidth()) / 2;
			m_pChild->MoveTo(nXPos, nYPos);
		}
	}
}
