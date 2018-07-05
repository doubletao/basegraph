#include "stdafx.h"
#include "MessageHandlerDefault.h"
#include "GraphEditor.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
MessageHandlerDefault::MessageHandlerDefault()
	: m_emHandleState(HandleState_NULL)
	, m_strHitObjID(_T(""))
	, m_emHitCtrlPt(CtrlPoint_NULL)
{
	m_ptBegin.x = 0.0f;
	m_ptBegin.y = 0.0f;
}

MessageHandlerDefault::~MessageHandlerDefault()
{
}

void MessageHandlerDefault::DrawSelectBox(C3DSceneViewWnd * pWnd, pointf ptBegin, pointf ptEnd)
{
	EditorSettingParam stuDrawerParam = CGraphEditorApp::GetBaseGraphFrameWnd()->GetEditorSettingParam();
	DrawBoxToEditBuf(pWnd, ptBegin, ptEnd, PS_DASH, 1, stuDrawerParam.nColorBoxSelect);
}

void MessageHandlerDefault::SelectObjectByBox(pointf ptBegin, pointf ptEnd, bool bPreSelect)
{
	//分两种情况处理，如果是向左上拉框，则碰到就算，否则圈住才算
	rectf rtBox = { ptBegin.x, ptBegin.y, ptEnd.x, ptEnd.y };
	std::vector<CString> vecStrObjID;
	if (ptEnd.x < ptBegin.x && ptEnd.y > ptBegin.y)
	{
		DataRender_BoxHitTest(rtBox, vecStrObjID);
	}
	else
	{
		DataRender_BoxInterTest(rtBox, vecStrObjID);
	}
	DataRender_ClearAllSelect();
	for (auto it : vecStrObjID)
	{
		DataRender_SetItemSelected(it, true);
	}
	if (bPreSelect)
	{
		//通知回显窗选中项变更
		UIControl_SendMsgTo(UIElementMsg_MainViewWnd, WM_MSG_OnSelectChanged, 0, 0);
	}
	else
	{
		//广播通知选中项变更
		UIControl_BroadcastMsg(WM_MSG_OnSelectChanged, 0, 0, nullptr);
	}
}

EmMessageHandlerResult MessageHandlerDefault::OnMouseMove(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point)
{
	if (m_emHandleState == HandleState_ClickSelect)
	{
		EditorSettingParam stuDrawerParam = CGraphEditorApp::GetBaseGraphFrameWnd()->GetEditorSettingParam();
		//单击后move且相差大于最小物件size则变成box
		if ((fabs(point.x - m_ptBegin.x) > stuDrawerParam.nObjectMinSize)
			&& (fabs(point.y - m_ptBegin.y) > stuDrawerParam.nObjectMinSize))
		{
			m_emHandleState = HandleState_BoxSelect;
		}
	}
	else if (m_emHandleState == HandleState_BoxSelect)
	{
		DrawSelectBox(pWnd, m_ptBegin, point);
		SelectObjectByBox(m_ptBegin, point, true);
	}
	else if (m_emHandleState == HandleState_Move)
	{
		float fXOffset = point.x - m_ptBegin.x;
		float fYOffset = point.y - m_ptBegin.y;
		float fZOffset = 0.0f;
		DataRender_OffsetSelObject(fXOffset, fYOffset, fZOffset);
		UIControl_BroadcastMsg(WM_MSG_ObjectSpaceChanged, 0, 0, nullptr);//通知刷新
		m_ptBegin = point;
	}
	return Msg_Handle_Continue;
}

EmMessageHandlerResult MessageHandlerDefault::OnLButtonDown(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point)
{
	if (pWnd)
	{
		m_ptBegin = point;
		pWnd->SetCapture();
		EmHittestRetType emHitRet = DataRender_HitTest(point, m_strHitObjID, m_emHitCtrlPt);
		if (emHitRet == Hittest_CtrlPoint)
		{
			//判定选中点是什么，根据选中点来确定当前动作
			m_strHitObjID.Empty();
			m_emHitCtrlPt = CtrlPoint_NULL;
		}
		else if (emHitRet == Hittest_Object)
		{
			//点中了物件，则分情况处理，如果点中项之前已被选中，则进入移动逻辑，如果点中项之前未被选中，则进入单击逻辑
			bool bSeled = false;
			if (DataRender_IsItemSelected(m_strHitObjID, bSeled))
			{
				if (bSeled)
				{
					m_emHandleState = HandleState_Move;
				}
				else
				{
					m_emHandleState = HandleState_ClickSelect;
				}
			}
			else
			{
				//物件有问题，清空
				m_strHitObjID.Empty();
			}
			m_emHitCtrlPt = CtrlPoint_NULL;
		}
		else if (emHitRet == Hittest_NULL)
		{
			//没点中物件，也进入单击选中状态
			m_emHandleState = HandleState_ClickSelect;
			m_strHitObjID.Empty();
			m_emHitCtrlPt = CtrlPoint_NULL;
		}
		else
		{
			m_strHitObjID.Empty();
			m_emHitCtrlPt = CtrlPoint_NULL;
		}
	}
	return Msg_Handle_Continue;
}

EmMessageHandlerResult MessageHandlerDefault::OnLButtonUp(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point)
{
	if (pWnd)
	{
		if (m_emHandleState == HandleState_ClickSelect)
		{
			//弹起的时候，依然是单击选中状态，则选中当前块
			DataRender_ClearAllSelect();
			DataRender_SetItemSelected(m_strHitObjID, true);
			//广播通知选中项变更
			UIControl_BroadcastMsg(WM_MSG_OnSelectChanged, 0, 0, nullptr);
		}
		else if (m_emHandleState == HandleState_BoxSelect)
		{
			SelectObjectByBox(m_ptBegin, point, false);
		}
		//up之后全清掉
		ReleaseCapture(); 
		ClearEditBuf(pWnd);
		m_ptBegin.x = 0.0f;
		m_ptBegin.y = 0.0f;
		m_emHandleState = HandleState_NULL;
		m_strHitObjID.Empty();
		m_emHitCtrlPt = CtrlPoint_NULL;
	}
	return Msg_Handle_Continue;
}

EmMessageHandlerResult MessageHandlerDefault::OnKeyDown(IN C3DSceneViewWnd * pWnd, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_DELETE)
	{
		//删除物件
		DataRender_DeleteSelObj();
		UIControl_BroadcastMsg(WM_MSG_ObjectCreateDelete, 0, 0, nullptr);//通知物件变动
		return Msg_Handle_Break;
	}
	return Msg_Handle_Continue;
}
