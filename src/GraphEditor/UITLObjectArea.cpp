#include "stdafx.h"
#include "UITLObjectArea.h"
#include "UITLRuler.h"
#include <assert.h>
#include "GraphEditorCommonDef.h"

UITLObjectArea::UITLObjectArea()
	: m_pLeftArea(nullptr)
	, m_pRightArea(nullptr)
	, m_nMaxHeight(0)
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
	pObjectItem->m_strObjID = strID;
	if (DataBase_HasSubItem(strID))
	{
		pObjectItem->m_bHasSub = true;
		pObjectItem->m_bExpandSub = true;
		//轨道暂时不实现
	}
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

void UITLObjectArea::RearrangeChildPos()
{
	if (UITLRuler::Instance().GetHScrollBar())
	{
		AdjustScrollZoom(UITLRuler::Instance().GetHScrollBar()->GetPagePercent());
	}
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
	//标尺的绘制逻辑放在这里调用
	UITLRuler::Instance().DrawElement(pDC);
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

bool UITLObjectArea::SetScrollInfo(bool bRefresh)
{
	float fXRatio = 1.0f;
	float fYRatio = 1.0f;

	if (UITLRuler::Instance().GetHScrollBar())
	{
		fXRatio = UITLRuler::Instance().GetHScrollBar()->GetPagePercent();
	}
	if (UITLRuler::Instance().GetVScrollBar())
	{
		fYRatio = UITLRuler::Instance().GetVScrollBar()->GetPagePercent();
	}

	int nLogicHeight = int(m_nHeight / fYRatio);

	int nItemTreeWidth = UIControl_GetIntValue(ThemeInt_TimeLineItemTreeWidth);
	if (m_pLeftArea)
	{
		m_pLeftArea->SetScrollInfo(nItemTreeWidth, nLogicHeight, bRefresh);
	}
	if (m_pRightArea)
	{
		int nLogicWidth = int(m_pRightArea->GetWidth() / fXRatio);
		m_pRightArea->SetScrollInfo(nLogicWidth, nLogicHeight, bRefresh);
		nLogicWidth = int(UITLRuler::Instance().GetWidth() / fXRatio);
		UITLRuler::Instance().SetScrollInfo(nLogicWidth, nLogicHeight, bRefresh);
	}
	return true;
}

bool UITLObjectArea::SetScrollXPos(int nXPos, bool bRefresh)
{
	if (m_pLeftArea)
	{
		m_pLeftArea->SetScrollXPos(nXPos, bRefresh);
	}
	if (m_pRightArea)
	{
		m_pRightArea->SetScrollXPos(nXPos, bRefresh);
	}
	//尺子的滚动也跟着物件区一起走
	UITLRuler::Instance().SetScrollXPos(nXPos, bRefresh);
	return true;
}

bool UITLObjectArea::SetScrollYPos(int nYPos, bool bRefresh)
{
	if (m_pLeftArea)
	{
		m_pLeftArea->SetScrollYPos(nYPos, bRefresh);
	}
	if (m_pRightArea)
	{
		m_pRightArea->SetScrollYPos(nYPos, bRefresh);
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

int UITLObjectArea::SetMaxHeight(int nHeight/* = 0*/, bool bReset/* = false*/)
{
	if (bReset)
	{
		m_nMaxHeight = nHeight;
	}
	else
	{
		if (nHeight > m_nMaxHeight)
		{
			m_nMaxHeight = nHeight;
		}
	}
	return m_nMaxHeight;
}

void UITLObjectArea::AdjustScrollZoom(float fScrollPercent)
{
	if (fScrollPercent > 1.0f)
	{
		fScrollPercent = 1.0f;
	}
	else if (fScrollPercent < 0.01f)
	{
		fScrollPercent = 0.01f;
	}
	
	int nItemTreeWidth = UIControl_GetIntValue(ThemeInt_TimeLineItemTreeWidth);
	int nRightWidth = m_nWidth - nItemTreeWidth;
	int nFrameLength = UITLRuler::Instance().GetDispFrameLength();
	int nAdjustPreservePercent = UIControl_GetIntValue(ThemeInt_TimeLineAdjustPreservePercent);

	nFrameLength = int(nFrameLength * (1 + 10 / 100.0));
	fScrollPercent = UITLRuler::Instance().RecalPixelPerFrame(nRightWidth, nFrameLength, fScrollPercent);
	if (m_pRightArea && UITLRuler::Instance().GetHScrollBar())
	{
		UITLRuler::Instance().GetHScrollBar()->ResetScroll(int(m_pRightArea->GetWidth() / fScrollPercent), m_pRightArea->GetWidth());
	}
}

float UITLObjectArea::GetCurrentPercent()
{
	if (UITLRuler::Instance().GetHScrollBar())
	{
		return UITLRuler::Instance().GetHScrollBar()->GetPagePercent();
	}
	return 1.0f;
}
