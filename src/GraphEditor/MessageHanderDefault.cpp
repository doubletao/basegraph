#include "stdafx.h"
#include "MessageHanderDefault.h"
#include "GraphEditor.h"

MessageHanderDefault::MessageHanderDefault()
	: m_emHandleState(HandleState_NULL)
	, m_strHitObjID(_T(""))
	, m_emHitCtrlPt(CtrlPoint_NULL)
{
	m_ptBegin.x = 0.0f;
	m_ptBegin.y = 0.0f;
}

MessageHanderDefault::~MessageHanderDefault()
{
}

void MessageHanderDefault::DrawSelectBox(C3DSceneViewWnd * pWnd, pointf ptBegin, pointf ptEnd)
{
	if (pWnd)
	{
		int nWidth = DataBase_GetSceneWidth();
		int nHeight = DataBase_GetSceneHeight();
		SceneToBuffer(&ptBegin, &ptBegin, nWidth / 2, nHeight / 2, 1.0f);
		SceneToBuffer(&ptEnd, &ptEnd, nWidth / 2, nHeight / 2, 1.0f);
		int nEditBuffer = pWnd->GetViewBufInfo().GetEditMemBuffer();
		BufferDrawerSettingParam stuDrawerParam = CGraphEditorApp::GetBaseGraphFrameWnd()->GetBufferDrawerSettingParam();
		//准备好DC
		CSYBufferDC bufferDC;
		//清空画布
		DataRender_Clear2DBuf(nEditBuffer, 0);
		// 先画出编辑控制线，后画选中。
		StuSimpleBuffer stuSimBuf;
		DataRender_Get2DBuf(nEditBuffer, stuSimBuf);
		bufferDC.AttachBuffer((DWORD *)stuSimBuf.pBuffer, stuSimBuf.nWidth, stuSimBuf.nHeight);
		bufferDC.SetZoomRef(pWnd->GetViewInfo().m_fScale);
		CPoint ptOffset(pWnd->GetViewInfo().m_nSceneOffsetX, pWnd->GetViewInfo().m_nSceneOffsetY);
		bufferDC.SetOffsetRef(ptOffset.x, ptOffset.y);
		bufferDC.SetPenColor(stuDrawerParam.nColorBoxSelect);

		bufferDC.DrawRect(ptBegin, ptEnd);
		UIControl_SendMsgTo(UIElementMsg_MainViewWnd, WM_MSG_SerRenderTask, 0, 0);//通知刷新
	}
}

void MessageHanderDefault::SelectObjectByBox(pointf ptBegin, pointf ptEnd, bool bPreSelect)
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

EmMessageHandlerResult MessageHanderDefault::OnMouseMove(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point)
{
	if (m_emHandleState == HandleState_ClickSelect)
	{
		//单击后move则变成box
		m_emHandleState = HandleState_BoxSelect;
	}
	else if (m_emHandleState == HandleState_BoxSelect)
	{
		DrawSelectBox(pWnd, m_ptBegin, point);
		SelectObjectByBox(m_ptBegin, point, true);
	}
	return Msg_Handle_Continue;
}

EmMessageHandlerResult MessageHanderDefault::OnLButtonDown(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point)
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
			//点中了物件，则进入单击选中状态
			m_emHandleState = HandleState_ClickSelect;
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

EmMessageHandlerResult MessageHanderDefault::OnLButtonUp(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point)
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
