#include "stdafx.h"
#include "ISceneMessageHandler.h"

ISceneMessageHandler::~ISceneMessageHandler()
{
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

EmMessageHandlerResult ISceneMessageHandler::OnMouseMove(IN CWnd * pWnd, UINT nFlags, CPoint point)
{
	return Msg_Handle_Continue;
}

EmMessageHandlerResult ISceneMessageHandler::OnMouseWheel(IN CWnd * pWnd, UINT nFlags, short zDelta, CPoint pt)
{
	return Msg_Handle_Continue;
}

EmMessageHandlerResult ISceneMessageHandler::OnLButtonDown(IN CWnd * pWnd, UINT nFlags, CPoint point)
{
	return Msg_Handle_Continue;
}

EmMessageHandlerResult ISceneMessageHandler::OnLButtonUp(IN CWnd * pWnd, UINT nFlags, CPoint point)
{
	return Msg_Handle_Continue;
}

EmMessageHandlerResult ISceneMessageHandler::OnLButtonDblClk(IN CWnd * pWnd, UINT nFlags, CPoint point)
{
	return Msg_Handle_Continue;
}

EmMessageHandlerResult ISceneMessageHandler::OnRButtonDown(IN CWnd * pWnd, UINT nFlags, CPoint point)
{
	return Msg_Handle_Continue;
}

EmMessageHandlerResult ISceneMessageHandler::OnRButtonUp(IN CWnd * pWnd, UINT nFlags, CPoint point)
{
	return Msg_Handle_Continue;
}

EmMessageHandlerResult ISceneMessageHandler::OnRButtonDblClk(IN CWnd * pWnd, UINT nFlags, CPoint point)
{
	return Msg_Handle_Continue;
}

EmMessageHandlerResult ISceneMessageHandler::OnKeyDown(IN CWnd * pWnd, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return Msg_Handle_Continue;
}

EmMessageHandlerResult ISceneMessageHandler::OnKeyUp(IN CWnd * pWnd, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return Msg_Handle_Continue;
}

EmMessageHandlerResult ISceneMessageHandler::OnChar(IN CWnd * pWnd, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return Msg_Handle_Continue;
}
