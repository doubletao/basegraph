#include "stdafx.h"
#include "MessageHandlerTitlewordTool.h"
#include "GraphEditor.h"

MessageHanderTitleword::MessageHanderTitleword()
	: m_bLButtonDown(false)
{
	m_ptBegin.x = 0.0f;
	m_ptBegin.y = 0.0f;
}

MessageHanderTitleword::~MessageHanderTitleword()
{
}

void MessageHanderTitleword::OnSetFocus(ISceneMessageHandler * pLastHandler)
{
	ISceneMessageHandler::OnSetFocus(pLastHandler);
	//拿到焦点后干掉选中项
	DataRender_ClearAllSelect();
	UIControl_SendMsgTo(UIElementMsg_MainViewWnd, WM_MSG_SerRenderTask, 0, 0);//通知刷新
}

void MessageHanderTitleword::DrawObjCreateBox(C3DSceneViewWnd * pWnd, pointf ptBegin, pointf ptEnd)
{
	EditorSettingParam stuDrawerParam = CGraphEditorApp::GetBaseGraphFrameWnd()->GetEditorSettingParam();
	DrawBoxToEditBuf(pWnd, ptBegin, ptEnd, PS_SOLID, 1, stuDrawerParam.nColorObjCreateLine);
}

EmMessageHandlerResult MessageHanderTitleword::OnLButtonDown(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point)
{
	if (pWnd)
	{
		m_ptBegin = point;
		pWnd->SetCapture();
		m_bLButtonDown = true;
	}
	return Msg_Handle_Break;
}

EmMessageHandlerResult MessageHanderTitleword::OnMouseMove(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point)
{
	if (m_bLButtonDown)
	{
		EditorSettingParam stuDrawerParam = CGraphEditorApp::GetBaseGraphFrameWnd()->GetEditorSettingParam();
		if ((fabs(point.x - m_ptBegin.x) > stuDrawerParam.nObjectMinSize)
			&& (fabs(point.y - m_ptBegin.y) > stuDrawerParam.nObjectMinSize))
		{
			DrawObjCreateBox(pWnd, m_ptBegin, point);
		}
	}
	return Msg_Handle_Break;
}

EmMessageHandlerResult MessageHanderTitleword::OnLButtonUp(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point)
{
	if (pWnd)
	{
		ReleaseCapture();
		m_ptBegin.x = 0.0f;
		m_ptBegin.y = 0.0f;
		m_bLButtonDown = false;
		ClearEditBuf(pWnd);
	}
	return Msg_Handle_Exit;
}

