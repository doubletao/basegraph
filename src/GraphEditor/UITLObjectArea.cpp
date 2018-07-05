#include "stdafx.h"
#include "UITLObjectArea.h"
#include "UITLRuler.h"
#include <assert.h>
#include "GraphEditorCommonDef.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
UITLObjectArea::UITLObjectArea()
	: m_pLeftArea(nullptr)
	, m_pRightArea(nullptr)
	, m_nVScrollHeight(0)
{
	m_nItemBarEndAreaWidth = UIControl_GetIntValue(ThemeInt_TimeLineItemBarEndAreaWidth);
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

void UITLObjectArea::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	if (!HitTest(lParam))
	{
		return;
	}
	if (false)
	{
		CString strDebug;
		strDebug.Format(_T("OnLButtonDown\n"));
		OutputDebugString(strDebug);
	}
	//如果点中了某个物件，则清掉其他选中物件并将该物件置为选中状态，如果没有点中物件，则清掉所有物件的选中状态
	CString strHitID;
	DataRender_ClearAllSelect();
	if (ObjectArea_HitItem == ObjectHittest(lParam, &strHitID)
		|| ObjectArea_HitItemBarMiddle == ObjectHittest(lParam, &strHitID))
	{
		DataRender_SetItemSelected(strHitID, true);
	}
	else if (ObjectArea_HitItemBarLeftEnd == ObjectHittest(lParam, &strHitID))
	{
		//点中左边
		DataRender_SetItemSelected(strHitID, true);
	}
	else if (ObjectArea_HitItemBarRightEnd == ObjectHittest(lParam, &strHitID))
	{
		//点中右边
		DataRender_SetItemSelected(strHitID, true);
	}
	//广播通知选中项变更
	UIControl_BroadcastMsg(WM_MSG_OnSelectChanged, 0, 0, this);
	Refresh();
}

void UITLObjectArea::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
}

void UITLObjectArea::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
}

void UITLObjectArea::OnDispatchMsg(int nMsgID, WPARAM wParam, LPARAM lParam, int nIDFrom)
{
	switch (nMsgID)
	{
	case WM_MSG_OnSelectChanged:
		if (nIDFrom != GetMsgDispatchID())
		{
			Refresh();
		}
		break;
	default:
		break;
	}
}

ObjectAreaHittestType UITLObjectArea::ObjectHittest(CPoint ptHit, CString * pStrID)
{
	ObjectAreaHittestType emHitRet = ObjectArea_HitNull;
	for (std::map<CString, UITLObjectItem *>::iterator it = m_mapObjDrawerInfo.begin(); it != m_mapObjDrawerInfo.end(); it++)
	{
		UITLObjectItem * pObjDrawer = it->second;
		if (pObjDrawer)
		{
			if (pObjDrawer->rtLeft.PtInRect(ptHit))
			{
				//如果左侧区域被点中，则是物件被点中
				emHitRet = ObjectArea_HitItem;
				if (pStrID)
				{
					*pStrID = pObjDrawer->m_strObjID;
				}
				break;
			}
			//如果右侧区域被点中，则分情况处理，判定是左端点，右端点还是中部
			else if (pObjDrawer->rtRight.PtInRect(ptHit))
			{
				if (ptHit.x > pObjDrawer->rtActionBar.right - m_nItemBarEndAreaWidth && ptHit.x < pObjDrawer->rtActionBar.right)
				{
					emHitRet = ObjectArea_HitItemBarRightEnd;
				}
				else if (ptHit.x > pObjDrawer->rtActionBar.left && ptHit.x < pObjDrawer->rtActionBar.left + m_nItemBarEndAreaWidth)
				{
					emHitRet = ObjectArea_HitItemBarLeftEnd;
				}
				else if (pObjDrawer->rtActionBar.PtInRect(ptHit))
				{
					emHitRet = ObjectArea_HitItemBarMiddle;
				}
				else
				{
					emHitRet = ObjectArea_HitItem;
				}
				if (pStrID)
				{
					*pStrID = pObjDrawer->m_strObjID;
				}
				break;
			}
		}
	}
	return emHitRet;
}

ObjectAreaHittestType UITLObjectArea::ObjectHittest(int x, int y, CString * strID)
{
	return ObjectHittest(CPoint(x, y), strID);
}

ObjectAreaHittestType UITLObjectArea::ObjectHittest(LPARAM lParam, CString * strID)
{
	return ObjectHittest(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), strID);
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
	std::map<CString, UITLObjectItem *>::iterator it = m_mapObjDrawerInfo.find(strID);
	if (it != m_mapObjDrawerInfo.end())
	{
		UITLObjectItem *& pObjEle = it->second;
		if (pObjEle)
		{
			delete pObjEle;
			pObjEle = nullptr;
		}
		m_mapObjDrawerInfo.erase(it);
		return true;
	}
	return false;
}

void UITLObjectArea::RearrangeChildPos()
{
	if (UITLRuler::Instance().GetHScrollBar())
	{
		AdjustScrollZoom(UITLRuler::Instance().GetHScrollBar()->GetPagePercent());
	}
	int nItemTreeWidth = UIControl_GetIntValue(ThemeInt_TimeLineItemTreeWidth);
	int nGap = UIControl_GetIntValue(ThemeInt_UIModualGap);
	if (m_pLeftArea)
	{
		m_pLeftArea->MoveTo(0, 0, nItemTreeWidth, m_nHeight);
	}
	if (m_pRightArea)
	{
		m_pRightArea->MoveTo(nItemTreeWidth + nGap, 0, m_nWidth - nItemTreeWidth - nGap, m_nHeight);
	}
}

void UITLObjectArea::DrawElement(CDC * pDC)
{
	if (m_bVisible)
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
}

EmUIElementMsgRet UITLObjectArea::HandleMsg(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_setMsgFilter.count(nMsg) > 0)
	{
		switch (nMsg)
		{
		case WM_LBUTTONDOWN:
			OnLButtonDown(wParam, lParam);
			break;
		case WM_LBUTTONUP:
			OnLButtonUp(wParam, lParam);
			break;
		case WM_MOUSEMOVE:
			OnMouseMove(wParam, lParam);
			break;
		default:
			break;
		}
		return UIElementMsgSuccBreak;
	}
	return UIElementMsgSuccContinue;
}

bool UITLObjectArea::Create(int nX, int nY, int nWidth, int nHeight, int nID, HWND hParentWnd, UIElement * pParentElement)
{
	if (!m_bCreated)
	{
		Initialize(nX, nY, nWidth, nHeight, nID, hParentWnd, pParentElement);
		m_setMsgFilter.insert(WM_LBUTTONDOWN);
		m_setMsgFilter.insert(WM_LBUTTONUP);
		m_setMsgFilter.insert(WM_MOUSEMOVE);

		m_pLeftArea = new UITLObjectLeftArea;
		m_pLeftArea->Create(0, 0, 10, 10, UIElementMsg_TimeLineObjectLeftArea, hParentWnd, this);

		m_pRightArea = new UITLObjectRightArea;
		m_pRightArea->Create(0, 0, 10, 10, UIElementMsg_TimeLineObjectRightArea, hParentWnd, this);

		//注册消息中心
		UIControl_RegistMsgDispatcher(this);

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
		UITLRuler::Instance().SetScrollInfo(nLogicWidth, UITLRuler::Instance().GetHeight(), bRefresh);
	}
	if (bRefresh)
	{
		//通知时间线刷新一下pointer的位置
		UIControl_SendMsgTo(UIElementMsg_TimeLine, WM_MSG_TimeLinePointerChanged, 0, 0, 0);
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
	//通知时间线刷新一下pointer的位置
	UIControl_SendMsgTo(UIElementMsg_TimeLine, WM_MSG_TimeLinePointerChanged, 0, 0, 0);
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

void UITLObjectArea::SetVScrollHeight(int nHeight/* = 0*/, bool bReset/* = false*/)
{
	if (bReset)
	{
		m_nVScrollHeight = nHeight;
	}
	else
	{
		if (nHeight > m_nVScrollHeight)
		{
			m_nVScrollHeight = nHeight;
		}
	}
}

int UITLObjectArea::GetVScrollHeight()
{
	if (m_nVScrollHeight < m_nHeight)
	{
		m_nVScrollHeight = m_nHeight;
	}
	return m_nVScrollHeight;
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
