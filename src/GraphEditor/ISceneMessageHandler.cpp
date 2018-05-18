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
