#include "stdafx.h"
#include "ISceneMessageHandler.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
ISceneMessageHandler::~ISceneMessageHandler()
{
}

void ISceneMessageHandler::ClearEditBuf(C3DSceneViewWnd * pWnd)
{
	int nEditBuffer = pWnd->GetViewBufInfo().GetEditMemBuffer();
	//清空画布
	DataRender_Clear2DBuf(nEditBuffer, 0);
	UIControl_SendMsgTo(UIElementMsg_MainViewWnd, WM_MSG_SerRenderTask, 0, 0);//通知刷新
}

void ISceneMessageHandler::DrawBoxToEditBuf(C3DSceneViewWnd * pWnd, pointf ptBegin, pointf ptEnd, int nPenStyle, int nPenWidth, int nPenColor)
{
	if (pWnd)
	{
		int nWidth = DataBase_GetSceneWidth();
		int nHeight = DataBase_GetSceneHeight();
		SceneToBuffer(&ptBegin, &ptBegin, nWidth / 2, nHeight / 2, 1.0f);
		SceneToBuffer(&ptEnd, &ptEnd, nWidth / 2, nHeight / 2, 1.0f);
		int nEditBuffer = pWnd->GetViewBufInfo().GetEditMemBuffer();
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
		bufferDC.SetPenStyle(nPenStyle);
		bufferDC.SetPenWidth(nPenWidth);
		bufferDC.SetPenColor(nPenColor);

		bufferDC.DrawRect(ptBegin, ptEnd);
		UIControl_SendMsgTo(UIElementMsg_MainViewWnd, WM_MSG_SerRenderTask, 0, 0);//通知刷新
	}
}

void ISceneMessageHandler::SetObjectID(CString strObjID)
{
	m_strObjID = strObjID;
}

CString ISceneMessageHandler::GetObjectID()
{
	return m_strObjID;
}

void ISceneMessageHandler::OnSetFocus(ISceneMessageHandler * pLastHandler)
{
}

void ISceneMessageHandler::OnKillFocus(ISceneMessageHandler * pNextHandler)
{
}

EmMessageHandlerResult ISceneMessageHandler::OnMouseMove(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point)
{
	return Msg_Handle_Continue;
}

EmMessageHandlerResult ISceneMessageHandler::OnMouseWheel(IN C3DSceneViewWnd * pWnd, UINT nFlags, short zDelta, pointf pt)
{
	return Msg_Handle_Continue;
}

EmMessageHandlerResult ISceneMessageHandler::OnLButtonDown(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point)
{
	return Msg_Handle_Continue;
}

EmMessageHandlerResult ISceneMessageHandler::OnLButtonUp(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point)
{
	return Msg_Handle_Continue;
}

EmMessageHandlerResult ISceneMessageHandler::OnLButtonDblClk(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point)
{
	return Msg_Handle_Continue;
}

EmMessageHandlerResult ISceneMessageHandler::OnRButtonDown(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point)
{
	return Msg_Handle_Continue;
}

EmMessageHandlerResult ISceneMessageHandler::OnRButtonUp(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point)
{
	return Msg_Handle_Continue;
}

EmMessageHandlerResult ISceneMessageHandler::OnRButtonDblClk(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point)
{
	return Msg_Handle_Continue;
}

EmMessageHandlerResult ISceneMessageHandler::OnKeyDown(IN C3DSceneViewWnd * pWnd, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return Msg_Handle_Continue;
}

EmMessageHandlerResult ISceneMessageHandler::OnKeyUp(IN C3DSceneViewWnd * pWnd, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return Msg_Handle_Continue;
}

EmMessageHandlerResult ISceneMessageHandler::OnChar(IN C3DSceneViewWnd * pWnd, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return Msg_Handle_Continue;
}
