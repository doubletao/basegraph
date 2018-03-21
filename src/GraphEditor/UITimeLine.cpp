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
	UITLRuler::Instance().SetScrollBar(nullptr, nullptr);

	UIElement::Release();
	m_pTimeCode = nullptr;
	m_pScrollHor = nullptr;
}

void UITimeLine::Initialize(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd)
{
	UIElement::Initialize(nX, nY, nWidth, nHeight, UIElementMsg_TimeLine, hParentWnd);
	m_setMsgFilter.insert(WM_LBUTTONDOWN);
	m_setMsgFilter.insert(WM_LBUTTONUP);
	m_setMsgFilter.insert(WM_MOUSEMOVE);
}

void UITimeLine::InitializeObject()
{
	m_pObjectArea = new UITLObjectArea;
	m_pObjectArea->Create(0, 0, 10, 10, UIElementMsg_TimeLineObjectArea, m_hParentWnd, this);
	m_vecChildElement.push_back(m_pObjectArea);

	m_pTimeCode = new UITLTimeCode;
	m_pTimeCode->Create(0, 0, 100, 100, m_hParentWnd, this);
	m_vecChildElement.push_back(m_pTimeCode);

	m_pScrollHor = new UIScrollBar;
	m_pScrollHor->Create(0, 0, 10, 10, 10, 10, UIElementMsg_TimeLineHorScroll, true, m_hParentWnd, this);
	m_vecChildElement.push_back(m_pScrollHor);

	m_pScrollVer = new UIScrollBar;
	m_pScrollVer->Create(0, 0, 10, 10, 10, 10, UIElementMsg_TimeLineVerScroll, false, m_hParentWnd, this);
	m_vecChildElement.push_back(m_pScrollVer);

	//标尺在这里初始化，但不push到子物件集合
	UITLRuler::Instance().Create(0, 0, 10, 10, UIElementMsg_TimeLineRuler, m_hParentWnd, this);
	UITLRuler::Instance().SetScrollBar(m_pScrollHor, m_pScrollVer);
}

void UITimeLine::OnFrameChanged(int nFrame)
{
	SetFrame(nFrame);
}

void UITimeLine::RearrangeChildPos()
{
	int nTimeCodeWidth = UIControl_GetIntValue(ThemeInt_TimeLineTimeCodeWidth);
	int nTimeCodeHeight = UIControl_GetIntValue(ThemeInt_TimeLineTimeCodeHeight);
	int nScrollHeight = UIControl_GetIntValue(ThemeInt_TimeLineTimeScrollHeight);
	int nGap = UIControl_GetIntValue(ThemeInt_UIModualGap);
	if (m_pTimeCode)
	{
		m_pTimeCode->MoveTo(0, 0, nTimeCodeWidth, nTimeCodeHeight);
	}
	if (m_pScrollHor)
	{
		m_pScrollHor->MoveTo(nTimeCodeWidth + nGap, 0, m_nWidth - nTimeCodeWidth - nGap * 2, nScrollHeight);
	}
	if (m_pScrollVer)
	{
		m_pScrollVer->MoveTo(m_nWidth - nScrollHeight - nGap, nTimeCodeHeight + nGap, nScrollHeight, m_nHeight - nTimeCodeHeight - nGap * 2);
	}
	if (m_pObjectArea)
	{
		m_pObjectArea->MoveTo(0, nTimeCodeHeight + nGap, m_nWidth - nScrollHeight - nGap, m_nHeight - nTimeCodeHeight - nGap * 2);
		m_pObjectArea->SetScrollInfo();
	}

	UITLRuler::Instance().MoveTo(nTimeCodeWidth + 1, nScrollHeight, m_nWidth - nTimeCodeWidth - nGap * 2, nTimeCodeHeight - nScrollHeight);
}

bool UITimeLine::OnChildMsg(EmUIElementMsg emMsg, WPARAM wParam, LPARAM lParam)
{
	if (EmUIElementMsg_OnHScroll == emMsg)
	{
		if (false)
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
		if (false)
		{
			CString strDebug;
			strDebug.Format(_T("m_nID=%d, m_nPos=%d\n"), wParam, lParam);
			OutputDebugString(strDebug);
		}
		if (m_pObjectArea)
		{
			m_pObjectArea->SetScrollYPos((int)lParam);
		}
		return true;
	}
	return false;
}

void UITimeLine::OnDispatchMsg(int nMsgID, WPARAM wParam, LPARAM lParam, int nIDFrom)
{
	switch (nMsgID)
	{
	case WM_MSG_FrameChanged:
		if (nIDFrom != GetMsgDispatchID())
		{
			OnFrameChanged((int)wParam);
		}
		break;
	default:
		break;
	}
}

void UITimeLine::AdjustScrollZoom()
{
	if (m_pObjectArea)
	{
		m_pObjectArea->AdjustScrollZoom(1.0f);
		m_pObjectArea->SetScrollInfo();
	}
}

void UITimeLine::AdjustScrollZoom(int nDeta)
{
	if (m_pObjectArea)
	{
		float fCurPercent = m_pObjectArea->GetCurrentPercent();
		fCurPercent += nDeta * 1.0f / 100;
		m_pObjectArea->AdjustScrollZoom(fCurPercent);
		m_pObjectArea->SetScrollInfo();
	}
}

void UITimeLine::SetFrame(int nFrame, float fFPS)
{
	if (m_pTimeCode)
	{
		m_pTimeCode->SetFrame(nFrame, fFPS);
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
		m_strText = _T("时间线");
	}
	return true;
}

void UITimeLine::DrawElement(CDC * pDC)
{
	//时间线的绘制自己来，因为物件区绘制后需要更新物件区高度后再绘制垂直滚动条，所以需要保证物件区绘制在垂直滚动条前边
	if (m_pTimeCode)
	{
		m_pTimeCode->DrawElement(pDC);
	}
	if (m_pScrollHor)
	{
		m_pScrollHor->DrawElement(pDC);
	}
	if (m_pObjectArea)
	{
		m_pObjectArea->DrawElement(pDC);
		if (m_pScrollVer)
		{
			if (false)
			{
				CString strDebug;
				strDebug.Format(_T("m_pObjectArea->GetVScrollHeight()=%d\n"), m_pObjectArea->GetVScrollHeight());
				OutputDebugString(strDebug);
			}
			m_pScrollVer->ResetScroll(m_pObjectArea->GetVScrollHeight(), m_pObjectArea->GetHeight(), -1, false);
			m_pScrollVer->DrawElement(pDC);
		}
		m_pObjectArea->SetScrollInfo(false);
	}

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
