#pragma once
#include <stack>
#include <map>
#include "ISceneMessageHandler.h"

enum EmMessageHandlerType
{
	MessageHandlerType_NULL,
	//action
	MessageHandlerType_Default,
	MessageHandlerType_Scale,
	MessageHandlerType_Rotate,
	//ObjTool
	MessageHandlerType_CreateTitle,
	MessageHandlerType_CreateScroll,
	MessageHandlerType_CreateImage,
	//ObjEdit
	MessageHandlerType_EditTitle,
	MessageHandlerType_EditScroll,
	MessageHandlerType_EditImage,
};

class MessageHandlerManager
{
private:
	MessageHandlerManager();
	~MessageHandlerManager();
protected:
	std::stack<EmMessageHandlerType> m_stkHandler;//当前handler队列
	std::map<EmMessageHandlerType, ISceneMessageHandler *> m_mapHandlerStorage;//handler存储，存放已经创建出来的handler，等着最终一起释放
	ISceneMessageHandler * GetHandler(EmMessageHandlerType emHandlerType);
	ISceneMessageHandler * GetTopHanderPointer();//获取当前handler指针
public:
	static MessageHandlerManager & Instance();
	void PushHandler(EmMessageHandlerType emHandlerType);//入栈一个handler
	EmMessageHandlerType PopHandler();//出栈一个handler，返回弹出的handler
	void ChangeHandler(EmMessageHandlerType emHandlerType);//修改一个handler（先出后入）
	void ClearHandlerTo(EmMessageHandlerType emHandlerType);//清空handler栈到指定handler
	EmMessageHandlerType GetTopHandlerType();//获取当前handler类型
public:
	//鼠标
	virtual EmMessageHandlerResult OnMouseMove(IN CWnd * pWnd, UINT nFlags, CPoint point);
	virtual EmMessageHandlerResult OnMouseWheel(IN CWnd * pWnd, UINT nFlags, short zDelta, CPoint pt);
	virtual EmMessageHandlerResult OnLButtonDown(IN CWnd * pWnd, UINT nFlags, CPoint point);
	virtual EmMessageHandlerResult OnLButtonUp(IN CWnd * pWnd, UINT nFlags, CPoint point);
	virtual EmMessageHandlerResult OnLButtonDblClk(IN CWnd * pWnd, UINT nFlags, CPoint point);
	virtual EmMessageHandlerResult OnRButtonDown(IN CWnd * pWnd, UINT nFlags, CPoint point);
	virtual EmMessageHandlerResult OnRButtonUp(IN CWnd * pWnd, UINT nFlags, CPoint point);
	virtual EmMessageHandlerResult OnRButtonDblClk(IN CWnd * pWnd, UINT nFlags, CPoint point);
	//键盘
	virtual EmMessageHandlerResult OnKeyDown(IN CWnd * pWnd, UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual EmMessageHandlerResult OnKeyUp(IN CWnd * pWnd, UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual EmMessageHandlerResult OnChar(IN CWnd * pWnd, UINT nChar, UINT nRepCnt, UINT nFlags);
};

