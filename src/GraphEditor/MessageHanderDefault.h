#pragma once
#include "ISceneMessageHandler.h"

//默认工具担当点选，框选，移动，控制点缩放，控制点旋转等一系列功能
class MessageHanderDefault :
	public ISceneMessageHandler
{
protected:
	enum EmCurrentHandleState
	{
		HandleState_NULL,//空白
		HandleState_ClickSelect,//单击
		HandleState_BoxSelect,//框选
		HandleState_Move,//移动
		HandleState_Stretch,//控制点缩放
		HandleState_Rotate,//控制点旋转
	};
	EmCurrentHandleState m_emHandleState;
	CPoint m_ptBegin;
	CString m_strHitObjID;
	EmCtrlPointIndex m_emHitCtrlPt;
public:
	MessageHanderDefault();
	virtual ~MessageHanderDefault();
public:
	virtual EmMessageHandlerResult OnMouseMove(IN CWnd * pWnd, UINT nFlags, pointf point);
	virtual EmMessageHandlerResult OnLButtonDown(IN CWnd * pWnd, UINT nFlags, pointf point);
	virtual EmMessageHandlerResult OnLButtonUp(IN CWnd * pWnd, UINT nFlags, pointf point);
};
