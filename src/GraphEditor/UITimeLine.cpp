#include "stdafx.h"
#include "UITimeLine.h"
#include "GraphEditorCommonDef.h"

UITimeLine::UITimeLine()
	: m_pTimeCode(nullptr)
	, m_pScrollHor(nullptr)
	, m_pObjectArea(nullptr)
	, m_pScrollVer(nullptr)
{
}


UITimeLine::~UITimeLine()
{
}

void UITimeLine::Release()
{
	UIElement::Release();
	m_pTimeCode = nullptr;
	m_pScrollHor = nullptr;
}

void UITimeLine::Initialize(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd)
{
	UIElement::Initialize(nX, nY, nWidth, nHeight, 0, hParentWnd);
	m_setMsgFilter.insert(WM_LBUTTONDOWN);
	m_setMsgFilter.insert(WM_LBUTTONUP);
	m_setMsgFilter.insert(WM_MOUSEMOVE);
}

void UITimeLine::InitializeObject()
{
	m_pTimeCode = new UITLTimeCode;
	m_pTimeCode->Create(0, 0, 100, 100, m_hParentWnd, this);
	m_vecChildElement.push_back(m_pTimeCode);

	m_pScrollHor = new UIScrollBar;
	m_pScrollHor->Create(0, 0, 10, 10, 10, 10, UIElementMsg_TimeLineHorScroll, true, m_hParentWnd, this);
	m_vecChildElement.push_back(m_pScrollHor);

	m_pScrollVer = new UIScrollBar;
	m_pScrollVer->Create(0, 0, 10, 10, 10, 10, UIElementMsg_TimeLineVerScroll, false, m_hParentWnd, this);
	m_vecChildElement.push_back(m_pScrollVer);

	m_pObjectArea = new UITLObjectArea;
	m_pObjectArea->Create(0, 0, 10, 10, UIElementMsg_TimeLineObjectArea, m_hParentWnd, this);
	m_pObjectArea->SetScrollBar(m_pScrollHor);
	m_vecChildElement.push_back(m_pObjectArea);
}

void UITimeLine::RearrangeChildPos()
{
	int nTimeCodeWidth = UIControl_GetIntValue(ThemeInt_TimeLineTimeCodeWidth);
	int nTimeCodeHeight = UIControl_GetIntValue(ThemeInt_TimeLineTimeCodeHeight);
	if (m_pTimeCode)
	{
		m_pTimeCode->MoveTo(0, 0, nTimeCodeWidth, nTimeCodeHeight);
	}
	float fXRatio = 1.0f;
	float fYRatio = 1.0f;
	if (m_pScrollHor)
	{
		m_pScrollHor->MoveTo(nTimeCodeWidth, 0, m_nWidth - nTimeCodeWidth, nTimeCodeHeight / 2);
		m_pScrollHor->ResetScroll(1000, 250);
		fXRatio = m_pScrollHor->GetPagePercent();
	}
	if (m_pScrollVer)
	{
		m_pScrollVer->MoveTo(m_nWidth - nTimeCodeHeight / 2, nTimeCodeHeight, nTimeCodeHeight / 2, m_nHeight - nTimeCodeHeight);
		fYRatio = m_pScrollHor->GetPagePercent();
	}
	if (m_pObjectArea)
	{
		m_pObjectArea->MoveTo(0, nTimeCodeHeight, m_nWidth - nTimeCodeHeight / 2, m_nHeight - nTimeCodeHeight);
		int nLogicWidth = int(m_pObjectArea->GetWidth() / fXRatio);
		int nLogicHeight = int(m_pObjectArea->GetHeight() / fYRatio);
		m_pObjectArea->SetScrollInfo(nLogicWidth, nLogicHeight);
	}
}

bool UITimeLine::OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam)
{
	if (EmUIElementMsg_OnHScroll == emMsg)
	{
		if (true)
		{
			CString strDebug;
			strDebug.Format(_T("m_nID=%d, m_nPos=%d\n"), wParam, lParam);
			OutputDebugString(strDebug);
		}
		if (m_pObjectArea)
		{
			m_pObjectArea->SetScrollXPos((int)lParam);
		}
		return true;
	}
	else if (EmUIElementMsg_OnVScroll == emMsg)
	{
		if (true)
		{
			CString strDebug;
			strDebug.Format(_T("m_nID=%d, m_nPos=%d\n"), wParam, lParam);
			OutputDebugString(strDebug);
		}
		return true;
	}
	return false;
}

void UITimeLine::AdjustScrollZoom()
{
	if (m_pScrollHor)
	{
		if (m_pObjectArea)
		{
			m_pObjectArea->AdjustScrollZoom(1.0f);
			int nLogicWidth = int(m_pObjectArea->GetWidth() / m_pScrollHor->GetPagePercent());
			int nLogicHeight = int(m_pObjectArea->GetHeight() / m_pScrollHor->GetPagePercent());
			m_pObjectArea->SetScrollInfo(nLogicWidth, nLogicHeight);
		}
	}
}

bool UITimeLine::Create(int nX, int nY, int nWidth, int nHeight, HWND hParentWnd)
{
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, 0, hParentWnd);
		InitializeObject();

		UIImageDrawer * pImgDrawer = new UIImageDrawer(ThemeImage_ElementArea);
		m_mapDrawer[0].push_back(pImgDrawer);

		UITextDrawer * pTextDrawer = new UITextDrawer(ThemeFont_SkinWndTitleText);
		pTextDrawer->SetColor(RGB(255, 255, 255));
		m_mapDrawer[0].push_back(pTextDrawer);
		m_strText = _T("Ê±¼äÏß");
	}
	return true;
}

EmUIElementMsgRet UITimeLine::HandleMsg(UINT nMsg, LPARAM lParam, WPARAM wParam)
{
	if (m_setMsgFilter.count(nMsg) > 0)
	{
		switch (nMsg)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			ChildHandleMsg(nMsg, lParam, wParam);
			break;
		default:
			break;
		}
		return UIElementMsgSuccBreak;
	}
	return UIElementMsgSuccContinue;
}
