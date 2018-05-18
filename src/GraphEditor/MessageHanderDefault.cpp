#include "stdafx.h"
#include "MessageHanderDefault.h"

MessageHanderDefault::MessageHanderDefault()
	: m_emHandleState(HandleState_NULL)
	, m_ptBegin(0)
	, m_strHitObjID(_T(""))
	, m_emHitCtrlPt(CtrlPoint_NULL)
{
}


MessageHanderDefault::~MessageHanderDefault()
{
}

EmMessageHandlerResult MessageHanderDefault::OnMouseMove(IN CWnd * pWnd, UINT nFlags, pointf point)
{
	if (m_emHandleState == HandleState_ClickSelect)
	{
		//单击后move则变成box
		//m_emHandleState = HandleState_BoxSelect;
	}
	return Msg_Handle_Continue;
}

EmMessageHandlerResult MessageHanderDefault::OnLButtonDown(IN CWnd * pWnd, UINT nFlags, pointf point)
{
	if (pWnd)
	{
		EmHittestRetType emHitRet = DataRender_HitTest(point, m_strHitObjID, m_emHitCtrlPt);
		if (emHitRet == Hittest_CtrlPoint)
		{
			//判定选中点是什么，根据选中点来确定当前动作
			m_strHitObjID.Empty();
			m_emHitCtrlPt = CtrlPoint_NULL;
		}
		else if (emHitRet == Hittest_Object)
		{
			//点中了物件，则进入单击选中状态
			m_emHandleState = HandleState_ClickSelect;
			pWnd->SetCapture();
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

EmMessageHandlerResult MessageHanderDefault::OnLButtonUp(IN CWnd * pWnd, UINT nFlags, pointf point)
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
		//up之后全清掉
		ReleaseCapture();
		m_emHandleState = HandleState_NULL;
		m_strHitObjID.Empty();
		m_emHitCtrlPt = CtrlPoint_NULL;
	}
	return Msg_Handle_Continue;
}
