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
		UIButton * pCheck = new UIButton;
		int nXPos = (m_nWidth - 5 * (nImgWidth + 1)) / 2 + (int)m_vecChildElement.size() * (nImgWidth + 1);
		int nYPos = (m_nHeight - nImgHeight) / 2;
		pCheck->Create(nXPos, nYPos, nImgWidth, nImgHeight, emMsg, m_hParentWnd, nBKImgN, nBKImgF, nBKImgD, nBKImgX, this);
		m_vecChildElement.push_back(pCheck);
	};

	AddOneObjectBtn(UIElementMsg_PlayCtrlFirstFrame, ThemeImage_BtnFirstFrame_N, ThemeImage_BtnFirstFrame_F, ThemeImage_BtnFirstFrame_D, ThemeImage_BtnFirstFrame_X);
	AddOneObjectBtn(UIElementMsg_PlayCtrlPreFrame, ThemeImage_BtnPreFrame_N, ThemeImage_BtnPreFrame_F, ThemeImage_BtnPreFrame_D, ThemeImage_BtnPreFrame_X);
	AddOneObjectBtn(UIElementMsg_PlayCtrlPlayPause, ThemeImage_BtnPlay_N, ThemeImage_BtnPlay_F, ThemeImage_BtnPlay_D, ThemeImage_BtnPlay_X);
	AddOneObjectBtn(UIElementMsg_PlayCtrlNextFrame, ThemeImage_BtnNextFrame_N, ThemeImage_BtnNextFrame_F, ThemeImage_BtnNextFrame_D, ThemeImage_BtnNextFrame_X);
	AddOneObjectBtn(UIElementMsg_PlayCtrlLastFrame, ThemeImage_BtnLastFrame_N, ThemeImage_BtnLastFrame_F, ThemeImage_BtnLastFrame_D, ThemeImage_BtnLastFrame_X);
}

void UIPlayControlBar::RearrangeChildPos()
{
	for (size_t i = 0; i < m_vecChildElement.size(); i++)
	{
		UIElement *& m_pChild = m_vecChildElement[i];
		if (m_pChild)
		{
			int nXPos = (m_nWidth - 5 * (m_pChild->GetWidth() + 1)) / 2 + (int)i * (m_pChild->GetWidth() + 1);
			int nYPos = (m_nHeight - m_pChild->GetHeight()) / 2;
			m_pChild->MoveTo(nXPos, nYPos);
		}
	}
}
