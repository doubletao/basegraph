#include "stdafx.h"
#include "UIPlayControlBar.h"
#include "UITLRuler.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
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
		pCheck->Create(0, 0, nImgWidth, nImgHeight, emMsg, m_hParentWnd, nBKImgN, nBKImgF, nBKImgD, nBKImgX, this);
		m_vecChildElement.push_back(pCheck);
	};

	AddOneObjectBtn(UIElementMsg_PlayCtrlFirstFrame, ThemeImage_BtnFirstFrame_N, ThemeImage_BtnFirstFrame_F, ThemeImage_BtnFirstFrame_D, ThemeImage_BtnFirstFrame_X);
	AddOneObjectBtn(UIElementMsg_PlayCtrlPreFrame, ThemeImage_BtnPreFrame_N, ThemeImage_BtnPreFrame_F, ThemeImage_BtnPreFrame_D, ThemeImage_BtnPreFrame_X);
	AddOneObjectBtn(UIElementMsg_PlayCtrlPlayPause, ThemeImage_BtnPlay_N, ThemeImage_BtnPlay_F, ThemeImage_BtnPlay_D, ThemeImage_BtnPlay_X);
	AddOneObjectBtn(UIElementMsg_PlayCtrlNextFrame, ThemeImage_BtnNextFrame_N, ThemeImage_BtnNextFrame_F, ThemeImage_BtnNextFrame_D, ThemeImage_BtnNextFrame_X);
	AddOneObjectBtn(UIElementMsg_PlayCtrlLastFrame, ThemeImage_BtnLastFrame_N, ThemeImage_BtnLastFrame_F, ThemeImage_BtnLastFrame_D, ThemeImage_BtnLastFrame_X);
	AddOneObjectBtn(UIElementMsg_PlayCtrlLoopPlay, ThemeImage_BtnLoopPlay_N, ThemeImage_BtnLoopPlay_F, ThemeImage_BtnLoopPlay_D, ThemeImage_BtnLoopPlay_X);
}

void UIPlayControlBar::RearrangeChildPos()
{
	int nCount = (int)m_vecChildElement.size();
	for (int i = 0; i < nCount; i++)
	{
		UIElement *& m_pChild = m_vecChildElement[i];
		if (m_pChild)
		{
			int nXPos = (m_nWidth - nCount * (m_pChild->GetWidth() + 1)) / 2 + (int)i * (m_pChild->GetWidth() + 1);
			int nYPos = (m_nHeight - m_pChild->GetHeight()) / 2;
			m_pChild->MoveTo(nXPos, nYPos);
		}
	}
}

bool UIPlayControlBar::OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam)
{
	switch (emMsg)
	{
	case EmUIElementMsg_BtnClick:
		switch (wParam)
		{
		case UIElementMsg_PlayCtrlFirstFrame:
			OnBtnClickedFirstFrame();
			break;
		case UIElementMsg_PlayCtrlPreFrame:
			OnBtnClickedPreFrame();
			break;
		case UIElementMsg_PlayCtrlPlayPause:
			OnBtnClickedPlayPause();
			break;
		case UIElementMsg_PlayCtrlNextFrame:
			OnBtnClickedNextFrame();
			break;
		case UIElementMsg_PlayCtrlLastFrame:
			OnBtnClickedLastFrame();
			break;
		case UIElementMsg_PlayCtrlLoopPlay:
			OnBtnClickedLoopPlay();
			break;
		default:
			break;
		}
		return true;
	default:
		return true;
	}
	return false;
}

void UIPlayControlBar::OnBtnClickedFirstFrame()
{
	int nFrame = 0;
	DataRender_SetFrame(nFrame);
	UIControl_BroadcastMsg(WM_MSG_FrameChanged, nFrame, m_nID, this);
}

void UIPlayControlBar::OnBtnClickedPreFrame()
{
	int nFrame = DataRender_GetCurFrame() / 2;
	nFrame -= 1;
	if (nFrame < 0)
	{
		nFrame = 0;
	}
	DataRender_SetFrame(nFrame);
	UIControl_BroadcastMsg(WM_MSG_FrameChanged, nFrame, m_nID, this);
}

void UIPlayControlBar::OnBtnClickedPlayPause()
{
}

void UIPlayControlBar::OnBtnClickedNextFrame()
{
	int nFrame = DataRender_GetCurFrame() / 2;
	nFrame += 1;
	if (nFrame < 0)
	{
		nFrame = 0;
	}
	DataRender_SetFrame(nFrame);
	UIControl_BroadcastMsg(WM_MSG_FrameChanged, nFrame, m_nID, this);
}

void UIPlayControlBar::OnBtnClickedLastFrame()
{
	int nFrame = UITLRuler::Instance().GetCurLogicWholeLength();
	DataRender_SetFrame(nFrame);
	UIControl_BroadcastMsg(WM_MSG_FrameChanged, nFrame, m_nID, this);//通知frame变更
}

void UIPlayControlBar::OnBtnClickedLoopPlay()
{
}
