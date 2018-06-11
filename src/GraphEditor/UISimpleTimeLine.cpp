#include "stdafx.h"
#include "UISimpleTimeLine.h"
#include "GraphEditorCommonDef.h"

UISimpleTimeLine::UISimpleTimeLine()
	: m_pSimpTimeLine(nullptr)
{
}


UISimpleTimeLine::~UISimpleTimeLine()
{
}

void UISimpleTimeLine::Initialize(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd)
{
	UIElement::Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd);
	m_setMsgFilter.insert(WM_LBUTTONDOWN);
	m_setMsgFilter.insert(WM_LBUTTONUP);
	m_setMsgFilter.insert(WM_MOUSEMOVE);
	//注册消息中心
	UIControl_RegistMsgDispatcher(this);
}

void UISimpleTimeLine::InitializeObject()
{
	m_pSimpTimeLine = new UISliderCtr;
	m_pSimpTimeLine->Create(0, 0, 100, 100, 0, 100, UIElementMsg_SimpleTimeLineSlider, true, m_hParentWnd, this);
	m_vecChildElement.push_back(m_pSimpTimeLine);
}

void UISimpleTimeLine::RearrangeChildPos()
{
	int nHorGap = UIControl_GetIntValue(ThemeInt_SimpleTimeLineHorGap);
	int nSimpTLHeight = UIControl_GetIntValue(ThemeInt_SimpleTimeLineHeight);
	if (m_pSimpTimeLine)
	{
		m_pSimpTimeLine->MoveTo(nHorGap, (m_nHeight - nSimpTLHeight) / 2, m_nWidth - nHorGap * 2, nSimpTLHeight);
	}
}

void UISimpleTimeLine::OnDispatchMsg(int nMsgID, WPARAM wParam, LPARAM lParam, int nIDFrom)
{
	switch (nMsgID)
	{
	case WM_MSG_FrameChanged:
		if (nIDFrom != GetMsgDispatchID())
		{
			OnFrameChanged((int)wParam);
		}
		break;
	case WM_MSG_WholeFrameLengthChanged:
		if (nIDFrom != GetMsgDispatchID())
		{
			SetWholeFrame((int)wParam);
		}
	default:
		break;
	}
}

void UISimpleTimeLine::OnFrameChanged(int nFrame)
{
	SetFrame(nFrame);
}

bool UISimpleTimeLine::Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd)
{
	auto InsertOneDrawer = [&](int nStateValue, int nImgValue)
	{
		UIImageDrawer * pImgDrawer = new UIImageDrawer(nImgValue);
		m_mapDrawer[nStateValue].push_back(pImgDrawer);

		UITextDrawer * pTextDrawer = new UITextDrawer(ThemeFont_SkinWndTitleText);
		pTextDrawer->SetColor(RGB(255, 255, 255));
		m_mapDrawer[nStateValue].push_back(pTextDrawer);
		m_strText = _T("简单时间线");
	};
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, UIElementMsg_SimpleTimeLine, hParentWnd);
		InitializeObject();

		InsertOneDrawer(0, ThemeImage_ElementArea);

		SetFrame(0);
	}
	return true;
}

EmUIElementMsgRet UISimpleTimeLine::HandleMsg(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_setMsgFilter.count(nMsg) > 0)
	{
		switch (nMsg)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			ChildHandleMsg(nMsg, wParam, lParam);
			break;
		default:
			break;
		}
		return UIElementMsgSuccBreak;
	}
	return UIElementMsgSuccContinue;
}

bool UISimpleTimeLine::OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam)
{
	if (emMsg == EmUIElementMsg_OnSliderChanging)
	{
		DataRender_SetStaticMode(false);
		SetFrame((int)lParam);
	}
	else if (emMsg == EmUIElementMsg_OnSliderChanged)
	{
		DataRender_SetStaticMode(true);
		SetFrame((int)lParam);
	}
	return UIElement::OnChildMsg(emMsg, wParam, lParam);
}

void UISimpleTimeLine::SetWholeFrame(int nFrame)
{
	if (m_pSimpTimeLine)
	{
		m_pSimpTimeLine->SetLimit(0, nFrame);
	}
}

void UISimpleTimeLine::SetFrame(int nFrame)
{
	if (m_pSimpTimeLine)
	{
		m_pSimpTimeLine->SetPos(nFrame, true, true, true);
	}
	DataRender_SetFrame(nFrame * 2);
	UIControl_BroadcastMsg(WM_MSG_FrameChanged, nFrame, m_nID, this);
}
