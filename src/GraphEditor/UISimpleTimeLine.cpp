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
	UIElement::Initialize(nX, nY, nWidth, nHeight, 0, hParentWnd);
	m_setMsgFilter.insert(WM_LBUTTONDOWN);
	m_setMsgFilter.insert(WM_LBUTTONUP);
	m_setMsgFilter.insert(WM_MOUSEMOVE);
}

void UISimpleTimeLine::InitializeObject()
{
	m_pSimpTimeLine = new UISliderCtr;
	m_pSimpTimeLine->Create(0, 0, 100, 100, 0, 100, UIElementMsg_SimpleTimeLine, true, m_hParentWnd, this);
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
		Initialize(nX, nY, nWidth, nHeight, 0, hParentWnd);
		InitializeObject();

		InsertOneDrawer(0, ThemeImage_ElementArea);
	}
	return true;
}

EmUIElementMsgRet UISimpleTimeLine::HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
	return UIElementMsgSuccBreak;
}
