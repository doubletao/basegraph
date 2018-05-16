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
	return EmMessageHandlerResult();
}

EmMessageHandlerResult MessageHanderDefault::OnLButtonDown(IN CWnd * pWnd, UINT nFlags, CPoint point)
{
	return EmMessageHandlerResult();
}

EmMessageHandlerResult MessageHanderDefault::OnLButtonUp(IN CWnd * pWnd, UINT nFlags, CPoint point)
{
	return EmMessageHandlerResult();
}
