#pragma once
#include "ISceneMessageHandler.h"

class MessageHanderDefault :
	public ISceneMessageHandler
{
public:
	MessageHanderDefault();
	virtual ~MessageHanderDefault();
public:
	virtual EmMessageHandlerResult OnMouseMove(IN CWnd * pWnd, UINT nFlags, CPoint point);
	virtual EmMessageHandlerResult OnLButtonDown(IN CWnd * pWnd, UINT nFlags, CPoint point);
	virtual EmMessageHandlerResult OnLButtonUp(IN CWnd * pWnd, UINT nFlags, CPoint point);
};

