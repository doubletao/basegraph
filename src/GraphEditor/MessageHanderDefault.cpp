#include "stdafx.h"
#include "MessageHanderDefault.h"


MessageHanderDefault::MessageHanderDefault()
{
}


MessageHanderDefault::~MessageHanderDefault()
{
}

EmMessageHandlerResult MessageHanderDefault::OnMouseMove(IN CWnd * pWnd, UINT nFlags, CPoint point)
{
	return Msg_Handle_Continue;
}

EmMessageHandlerResult MessageHanderDefault::OnLButtonDown(IN CWnd * pWnd, UINT nFlags, CPoint point)
{
	return Msg_Handle_Continue;
}

EmMessageHandlerResult MessageHanderDefault::OnLButtonUp(IN CWnd * pWnd, UINT nFlags, CPoint point)
{
	return Msg_Handle_Continue;
}
