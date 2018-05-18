#pragma once
#include "GraphEditorCommonDef.h"
#include "3DSceneViewWnd.h"

// 消息处理返回值定义
enum EmMessageHandlerResult
{
	Msg_Handle_NULL			= 0x00000000,
	Msg_Handle_Success		= 0x00000001,
	Msg_Handle_Failed		= 0x00000002,
	Msg_Handle_Continue		= 0x00000004,
	Msg_Handle_Break		= 0x00000008,
	Msg_Handle_Exit			= 0x00000010,
	Msg_Handle_Success_Continue = Msg_Handle_Success | Msg_Handle_Continue,
	Msg_Handle_Failed_Continue = Msg_Handle_Failed | Msg_Handle_Continue,
	Msg_Handle_Success_Break = Msg_Handle_Success | Msg_Handle_Break,
	Msg_Handle_Failed_Break = Msg_Handle_Failed | Msg_Handle_Break,
};

class ISceneMessageHandler
{
protected:
	CString m_strObjID;
public:
	virtual ~ISceneMessageHandler() = 0;
public:
	//基本属性
	virtual void SetObjectID(CString strObjID);
	virtual CString GetObjectID();
	//焦点
	virtual void OnSetFocus(ISceneMessageHandler * pLastHandler = nullptr);
	virtual void OnKillFocus(ISceneMessageHandler * pNextHandler = nullptr);
	//鼠标
	virtual EmMessageHandlerResult OnMouseMove(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point);
	virtual EmMessageHandlerResult OnMouseWheel(IN C3DSceneViewWnd * pWnd, UINT nFlags, short zDelta, pointf pt);
	virtual EmMessageHandlerResult OnLButtonDown(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point);
	virtual EmMessageHandlerResult OnLButtonUp(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point);
	virtual EmMessageHandlerResult OnLButtonDblClk(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point);
	virtual EmMessageHandlerResult OnRButtonDown(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point);
	virtual EmMessageHandlerResult OnRButtonUp(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point);
	virtual EmMessageHandlerResult OnRButtonDblClk(IN C3DSceneViewWnd * pWnd, UINT nFlags, pointf point);
	//键盘
	virtual EmMessageHandlerResult OnKeyDown(IN C3DSceneViewWnd * pWnd, UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual EmMessageHandlerResult OnKeyUp(IN C3DSceneViewWnd * pWnd, UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual EmMessageHandlerResult OnChar(IN C3DSceneViewWnd * pWnd, UINT nChar, UINT nRepCnt, UINT nFlags);
};
