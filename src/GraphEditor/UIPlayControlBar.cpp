#include "stdafx.h"
#include "UIPlayControlBar.h"
#include "UITLRuler.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
UIPlayControlBar::UIPlayControlBar()
	: m_pThreadPlay(nullptr)
	, m_evtPlayQuit(FALSE, TRUE, NULL, NULL)
	, m_evtLoop(FALSE, TRUE, NULL, NULL)
	, m_pBtnPlay(nullptr)
	, m_pBtnLoop(nullptr)
{
}


UIPlayControlBar::~UIPlayControlBar()
{
	StopPlay();
	m_pBtnPlay = nullptr;
	m_pBtnPlay = nullptr;
}

UINT UIPlayControlBar::PlayThread(LPVOID pParam)
{
	UIPlayControlBar * pThis = (UIPlayControlBar *)pParam;
	if (pThis)
	{
		pThis->DoPlay();
	}
	return 0;
}

void UIPlayControlBar::DoPlay()
{
	int nFrameGap = 40;//40ms一帧
	while (true)
	{
		if (WaitForSingleObject(m_evtPlayQuit, nFrameGap) == WAIT_OBJECT_0)
		{
			break;
		}
		//每40ms扔个消息出去
		UIControl_SendMsgTo(UIElementMsg_MainViewWnd, WM_MSG_PlayThreadTimeup, 0, 0);
	}
}

void UIPlayControlBar::StartPlay()
{
	StopPlay();
	assert(m_pThreadPlay == nullptr);
	if (m_pThreadPlay == nullptr)
	{
		m_evtPlayQuit.ResetEvent();
		m_pThreadPlay = ::AfxBeginThread(UIPlayControlBar::PlayThread, this);
	}
}

void UIPlayControlBar::StopPlay()
{
	if (m_pThreadPlay)
	{
		m_evtPlayQuit.SetEvent();
		WaitForSingleObject(m_pThreadPlay->m_hThread, INFINITE);
		m_pThreadPlay = nullptr;
	}
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
		UIButton * pBtn = new UIButton;
		pBtn->Create(0, 0, nImgWidth, nImgHeight, emMsg, m_hParentWnd, nBKImgN, nBKImgF, nBKImgD, nBKImgX, this);
		return pBtn;
	};

	UIButton * pBtn = AddOneObjectBtn(UIElementMsg_PlayCtrlFirstFrame, ThemeImage_BtnFirstFrame_N, ThemeImage_BtnFirstFrame_F, ThemeImage_BtnFirstFrame_D, ThemeImage_BtnFirstFrame_X);
	m_vecChildElement.push_back(pBtn);
	pBtn = AddOneObjectBtn(UIElementMsg_PlayCtrlPreFrame, ThemeImage_BtnPreFrame_N, ThemeImage_BtnPreFrame_F, ThemeImage_BtnPreFrame_D, ThemeImage_BtnPreFrame_X);
	m_vecChildElement.push_back(pBtn);
	m_pBtnPlay = AddOneObjectBtn(UIElementMsg_PlayCtrlPlayPause, ThemeImage_BtnPlay_N, ThemeImage_BtnPlay_F, ThemeImage_BtnPlay_D, ThemeImage_BtnPlay_X);
	m_vecChildElement.push_back(m_pBtnPlay);
	pBtn = AddOneObjectBtn(UIElementMsg_PlayCtrlNextFrame, ThemeImage_BtnNextFrame_N, ThemeImage_BtnNextFrame_F, ThemeImage_BtnNextFrame_D, ThemeImage_BtnNextFrame_X);
	m_vecChildElement.push_back(pBtn);
	pBtn = AddOneObjectBtn(UIElementMsg_PlayCtrlLastFrame, ThemeImage_BtnLastFrame_N, ThemeImage_BtnLastFrame_F, ThemeImage_BtnLastFrame_D, ThemeImage_BtnLastFrame_X);
	m_vecChildElement.push_back(pBtn);
	m_pBtnLoop = AddOneObjectBtn(UIElementMsg_PlayCtrlLoopPlay, ThemeImage_BtnLoopPlay_N, ThemeImage_BtnLoopPlay_F, ThemeImage_BtnLoopPlay_D, ThemeImage_BtnLoopPlay_X);
	m_vecChildElement.push_back(m_pBtnLoop);
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

bool UIPlayControlBar::IsLoop()
{
	if (WaitForSingleObject(m_evtLoop, 0) == WAIT_OBJECT_0)
	{
		return true;
	}
	return false;
}

void UIPlayControlBar::SetLoop(bool bLoop)
{
	if (bLoop)
	{
		m_evtLoop.SetEvent();
	}
	else
	{
		m_evtLoop.ResetEvent();
	}
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
	StartPlay();
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
	if (IsLoop())
	{
		SetLoop(false);
	}
	else
	{
		SetLoop(true);
	}
}

bool UIPlayControlBar::IsPlaying()
{
	if (WaitForSingleObject(m_evtPlayQuit, 0) == WAIT_OBJECT_0)
	{
		return false;
	}
	return true;
}
