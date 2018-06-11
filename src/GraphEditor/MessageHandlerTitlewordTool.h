#pragma once
#include "ISceneMessageHandler.h"

//默认工具担当点选，框选，移动，控制点缩放，控制点旋转等一系列功能
class MessageHanderTitleword :
	public ISceneMessageHandler
{
protected:
	pointf m_ptBegin;
	bool m_bLButtonDown;
public:
	MessageHanderTitleword();
	virtual ~MessageHanderTitleword();
protected:
	virtual void OnSetFocus(ISceneMessageHandler * pLastHandler = nullptr);
	void DrawObjCreateBox(C3DSceneViewWnd * pWnd, pointf ptBegin, pointf ptEnd);
public:
	virtual EmMessageHandlerResult OnLButtonDown(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point);
	virtual EmMessageHandlerResult OnMouseMove(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point);
	virtual EmMessageHandlerResult OnLButtonUp(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point);
};
