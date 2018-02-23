#include "stdafx.h"
#include "UITLObjectArea.h"
#include "UITLRuler.h"
#include <assert.h>
#include "GraphEditorCommonDef.h"

UITLObjectArea::UITLObjectArea()
	: m_pLeftArea(nullptr)
	, m_pRightArea(nullptr)
	, m_pScrollHor(nullptr)
{
}


UITLObjectArea::~UITLObjectArea()
{
	for (std::map<CString, UITLObjectItem *>::iterator it = m_mapObjDrawerInfo.begin(); it != m_mapObjDrawerInfo.end(); it++)
	{
		UITLObjectItem *& pObjEle = it->second;
		if (pObjEle)
		{
			delete pObjEle;
			pObjEle = nullptr;
		}
	}
	m_mapObjDrawerInfo.clear();
	if (m_pLeftArea)
	{
		delete m_pLeftArea;
		m_pLeftArea = nullptr;
	}
	if (m_pRightArea)
	{
		delete m_pRightArea;
		m_pRightArea = nullptr;
	}
}

UITLObjectItem * UITLObjectArea::CreateObjectItem(CString strID)
{
	UITLObjectItem * pObjectItem = new UITLObjectItem;
	pObjectItem->SetObjID(strID);
	pObjectItem->SetExpand(false);
	m_mapObjDrawerInfo.insert(std::make_pair(strID, pObjectItem));
	return pObjectItem;
}

std::map<CString, UITLObjectItem*>& UITLObjectArea::GetObjDrawerInfo()
{
	return m_mapObjDrawerInfo;
}

UITLObjectItem * UITLObjectArea::GetObjDrawerInfo(CString strID)
{
	if (m_mapObjDrawerInfo.count(strID) > 0)
	{
		return m_mapObjDrawerInfo[strID];
	}
	else
	{
		return CreateObjectItem(strID);
	}
}

bool UITLObjectArea::DeleteObjDrawerInfo(CString strID)
{
	return false;
}

int UITLObjectArea::GetTotalLength()
{
	int nFrameLength = 0;
	if (DataBase_GetItemLength(nFrameLength))
	{
		nFrameLength = nFrameLength / 2;
	}
	else
	{
		nFrameLength = 5;
	}
	return nFrameLength;
}

void UITLObjectArea::RearrangeChildPos()
{
	int nItemTreeWidth = UIControl_GetIntValue(ThemeInt_TimeLineItemTreeWidth);
	if (m_pLeftArea)
	{
		m_pLeftArea->MoveTo(0, 0, nItemTreeWidth, m_nHeight);
	}
	if (m_pRightArea)
	{
		m_pRightArea->MoveTo(nItemTreeWidth, 0, m_nWidth - nItemTreeWidth, m_nHeight);
	}
}

void UITLObjectArea::DrawElement(CDC * pDC)
{
	if (m_pLeftArea)
	{
		m_pLeftArea->DrawElement(pDC);
	}
	if (m_pRightArea)
	{
		m_pRightArea->DrawElement(pDC);
	}
}

bool UITLObjectArea::Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UIElement * pParentElement)
{
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParentElement);

		m_pLeftArea = new UITLObjectLeftArea;
		m_pLeftArea->Create(0, 0, 10, 10, UIElementMsg_TimeLineObjectLeftArea, hParentWnd, this);

		m_pRightArea = new UITLObjectRightArea;
		m_pRightArea->Create(0, 0, 10, 10, UIElementMsg_TimeLineObjectRightArea, hParentWnd, this);

		return true;
	}

	return false;
}

bool UITLObjectArea::SetScrollInfo(int nLogicWidth, int nLogicHeight, bool bRefresh)
{
	int nItemTreeWidth = UIControl_GetIntValue(ThemeInt_TimeLineItemTreeWidth);
	if (m_pLeftArea)
	{
		m_pLeftArea->SetScrollInfo(nItemTreeWidth, nLogicHeight, bRefresh);
	}
	if (m_pRightArea)
	{
		m_pRightArea->SetScrollInfo(nLogicWidth - nItemTreeWidth, nLogicHeight, bRefresh);
	}
	return true;
}

bool UITLObjectArea::SetScrollXPos(int nXPos, bool bRefresh)
{
	if (m_pRightArea)
	{
		m_pRightArea->SetScrollXPos(nXPos, bRefresh);
	}
	return true;
}

bool UITLObjectArea::SetScrollYPos(int nYPos, bool bRefresh)
{
	if (m_pLeftArea)
	{
		m_pLeftArea->SetScrollXPos(nYPos, bRefresh);
	}
	if (m_pRightArea)
	{
		m_pRightArea->SetScrollXPos(nYPos, bRefresh);
	}
	return true;
}

int UITLObjectArea::GetScrollXPos()
{
	if (m_pRightArea)
	{
		return m_pRightArea->GetScrollXPos();
	}
	return 0;
}

int UITLObjectArea::GetScrollYPos()
{
	if (m_pRightArea)
	{
		return m_pRightArea->GetScrollYPos();
	}
	return 0;
}

void UITLObjectArea::SetScrollBar(UIScrollBar * pScrollHor)
{
	m_pScrollHor = pScrollHor;
}

void UITLObjectArea::AdjustScrollZoom()
{
	int nItemTreeWidth = UIControl_GetIntValue(ThemeInt_TimeLineItemTreeWidth);
	int nRightWidth = m_nWidth - nItemTreeWidth;
	int nFrameLength = GetTotalLength();
	int nAdjustPreservePercent = UIControl_GetIntValue(ThemeInt_TimeLineAdjustPreservePercent);

	nFrameLength = int(nFrameLength * (1 + 10 / 100.0));
	UITLRuler::Instance().RecalPixelPerFrame(nRightWidth, nFrameLength, 1.0f);
}
