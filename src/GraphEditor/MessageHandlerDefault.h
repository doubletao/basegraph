#pragma once
#include "ISceneMessageHandler.h"

//默认工具担当点选，框选，移动，控制点缩放，控制点旋转等一系列功能
class MessageHandlerDefault :
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
	pointf m_ptBegin;
	CString m_strHitObjID;
	EmCtrlPointIndex m_emHitCtrlPt;
public:
	MessageHandlerDefault();
	virtual ~MessageHandlerDefault();
protected:
	void DrawSelectBox(C3DSceneViewWnd * pWnd, pointf ptBegin, pointf ptEnd);
	void SelectObjectByBox(pointf ptBegin, pointf ptEnd, bool bPreSelect);//bPreSelect指示是否是预选中，预选中仅将选中变动消息通知回显窗，真正的选中才会将消息广播出去
public:
	virtual EmMessageHandlerResult OnMouseMove(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point);
	virtual EmMessageHandlerResult OnLButtonDown(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point);
	virtual EmMessageHandlerResult OnLButtonUp(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point);
	virtual EmMessageHandlerResult OnKeyDown(IN C3DSceneViewWnd * pWnd, UINT nChar, UINT nRepCnt, UINT nFlags);
};
