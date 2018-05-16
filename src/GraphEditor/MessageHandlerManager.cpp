#include "stdafx.h"
#include "MessageHandlerManager.h"
#include "MessageHandlerFactory.h"
#include "GraphEditorCommonDef.h"

MessageHandlerManager::MessageHandlerManager()
{
}


MessageHandlerManager::~MessageHandlerManager()
{
	for (auto it : m_mapHandlerStorage)
	{
		delete it.second;
		it.second = nullptr;
	}
	m_mapHandlerStorage.clear();
}

ISceneMessageHandler * MessageHandlerManager::GetHandler(EmMessageHandlerType emHandlerType)
{
	//先查找是否已经创建，如果没有，则创建
	if (m_mapHandlerStorage.count(emHandlerType) > 0 && m_mapHandlerStorage[emHandlerType])
	{
		return m_mapHandlerStorage[emHandlerType];
	}
	ISceneMessageHandler * pMsgHandler = MessageHandlerFactory::CreateMessageHandler(emHandlerType);
	m_mapHandlerStorage[emHandlerType] = pMsgHandler;
	return pMsgHandler;
}

MessageHandlerManager & MessageHandlerManager::Instance()
{
	static MessageHandlerManager inst;
	return inst;
}

void MessageHandlerManager::PushHandler(EmMessageHandlerType emHandlerType)
{
	ISceneMessageHandler * pHandlerNew = GetHandler(emHandlerType);
	//push的前提是新push的handler不为空
	if (pHandlerNew)
	{
		//拿到原来的栈顶指针
		ISceneMessageHandler * pHandlerOri = GetTopHanderPointer();
		//压栈
		m_stkHandler.push(emHandlerType);
		//对老栈顶执行kill
		if (pHandlerOri)
		{
			pHandlerOri->OnKillFocus(pHandlerNew);
		}
		//对新的栈顶执行set
		pHandlerNew->OnSetFocus(pHandlerOri);
	}
	//广播通知大家handler变更了
	UIControl_BroadcastMsg(WM_MSG_OnMessageHandlerChg, 0, 0, nullptr);
}

EmMessageHandlerType MessageHandlerManager::PopHandler()
{
	//拿到原来的栈顶类型和指针
	EmMessageHandlerType emHandlerOri = GetTopHandlerType();
	ISceneMessageHandler * pHandlerOri = GetTopHanderPointer();
	//出栈
	if (m_stkHandler.size() > 0)
	{
		m_stkHandler.pop();
	}
	//拿到新的栈顶
	ISceneMessageHandler * pHandlerNew = GetTopHanderPointer();
	//对老栈顶执行kill
	if (pHandlerOri)
	{
		pHandlerOri->OnKillFocus(pHandlerNew);
	}
	//对新的栈顶执行set
	if (pHandlerNew)
	{
		pHandlerNew->OnSetFocus(pHandlerOri);
	}

	//广播通知大家handler变更了
	UIControl_BroadcastMsg(WM_MSG_OnMessageHandlerChg, 0, 0, nullptr);
	//返回原来的栈顶类型
	return emHandlerOri;
}

void MessageHandlerManager::ChangeHandler(EmMessageHandlerType emHandlerType)
{
	//先出栈
	PopHandler();
	//再入栈
	PushHandler(emHandlerType);
}

void MessageHandlerManager::ClearHandlerTo(EmMessageHandlerType emHandlerType)
{
	//先清空至最后一个
	while (m_stkHandler.size() > 1)
	{
		PopHandler();
	}
	//如果最后一个就是指定类型，则返回
	if (GetTopHandlerType() == emHandlerType)
	{
		return;
	}
	//否则将最后一个干成指定类型
	ChangeHandler(emHandlerType);
}

EmMessageHandlerType MessageHandlerManager::GetTopHandlerType()
{
	EmMessageHandlerType emHandler = MessageHandlerType_NULL;
	if (m_stkHandler.size() > 0)
	{
		emHandler = m_stkHandler.top();
	}
	return emHandler;
}

ISceneMessageHandler * MessageHandlerManager::GetTopHanderPointer()
{
	EmMessageHandlerType emTopHandler = GetTopHandlerType();
	ISceneMessageHandler * pMsgHandler = GetHandler(emTopHandler);
	return pMsgHandler;
}

EmMessageHandlerResult MessageHandlerManager::OnMouseMove(IN CWnd * pWnd, UINT nFlags, CPoint point)
{
	EmMessageHandlerResult emRet = Msg_Handle_Continue;
	ISceneMessageHandler * pHandler = GetTopHanderPointer();
	if (pHandler)
	{
		emRet = pHandler->OnMouseMove(pWnd, nFlags, point);
		//如果需要退出，则退出
		if (emRet == Msg_Handle_Exit)
		{
			PopHandler();
		}
	}
	return emRet;
}

EmMessageHandlerResult MessageHandlerManager::OnMouseWheel(IN CWnd * pWnd, UINT nFlags, short zDelta, CPoint pt)
{
	EmMessageHandlerResult emRet = Msg_Handle_Continue;
	ISceneMessageHandler * pHandler = GetTopHanderPointer();
	if (pHandler)
	{
		emRet = pHandler->OnMouseWheel(pWnd, nFlags, zDelta, pt);
		//如果需要退出，则退出
		if (emRet == Msg_Handle_Exit)
		{
			PopHandler();
		}
	}
	return emRet;
}

EmMessageHandlerResult MessageHandlerManager::OnLButtonDown(IN CWnd * pWnd, UINT nFlags, CPoint point)
{
	EmMessageHandlerResult emRet = Msg_Handle_Continue;
	ISceneMessageHandler * pHandler = GetTopHanderPointer();
	if (pHandler)
	{
		emRet = pHandler->OnLButtonDown(pWnd, nFlags, point);
		//如果需要退出，则退出
		if (emRet == Msg_Handle_Exit)
		{
			PopHandler();
		}
	}
	return emRet;
}

EmMessageHandlerResult MessageHandlerManager::OnLButtonUp(IN CWnd * pWnd, UINT nFlags, CPoint point)
{
	EmMessageHandlerResult emRet = Msg_Handle_Continue;
	ISceneMessageHandler * pHandler = GetTopHanderPointer();
	if (pHandler)
	{
		emRet = pHandler->OnLButtonUp(pWnd, nFlags, point);
		//如果需要退出，则退出
		if (emRet == Msg_Handle_Exit)
		{
			PopHandler();
		}
	}
	return emRet;
}

EmMessageHandlerResult MessageHandlerManager::OnLButtonDblClk(IN CWnd * pWnd, UINT nFlags, CPoint point)
{
	EmMessageHandlerResult emRet = Msg_Handle_Continue;
	ISceneMessageHandler * pHandler = GetTopHanderPointer();
	if (pHandler)
	{
		emRet = pHandler->OnLButtonDblClk(pWnd, nFlags, point);
		//如果需要退出，则退出
		if (emRet == Msg_Handle_Exit)
		{
			PopHandler();
		}
	}
	return emRet;
}

EmMessageHandlerResult MessageHandlerManager::OnRButtonDown(IN CWnd * pWnd, UINT nFlags, CPoint point)
{
	EmMessageHandlerResult emRet = Msg_Handle_Continue;
	ISceneMessageHandler * pHandler = GetTopHanderPointer();
	if (pHandler)
	{
		emRet = pHandler->OnRButtonDown(pWnd, nFlags, point);
		//如果需要退出，则退出
		if (emRet == Msg_Handle_Exit)
		{
			PopHandler();
		}
	}
	return emRet;
}

EmMessageHandlerResult MessageHandlerManager::OnRButtonUp(IN CWnd * pWnd, UINT nFlags, CPoint point)
{
	EmMessageHandlerResult emRet = Msg_Handle_Continue;
	ISceneMessageHandler * pHandler = GetTopHanderPointer();
	if (pHandler)
	{
		emRet = pHandler->OnRButtonUp(pWnd, nFlags, point);
		//如果需要退出，则退出
		if (emRet == Msg_Handle_Exit)
		{
			PopHandler();
		}
	}
	return emRet;
}

EmMessageHandlerResult MessageHandlerManager::OnRButtonDblClk(IN CWnd * pWnd, UINT nFlags, CPoint point)
{
	EmMessageHandlerResult emRet = Msg_Handle_Continue;
	ISceneMessageHandler * pHandler = GetTopHanderPointer();
	if (pHandler)
	{
		emRet = pHandler->OnRButtonDblClk(pWnd, nFlags, point);
		//如果需要退出，则退出
		if (emRet == Msg_Handle_Exit)
		{
			PopHandler();
		}
	}
	return emRet;
}

EmMessageHandlerResult MessageHandlerManager::OnKeyDown(IN CWnd * pWnd, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	EmMessageHandlerResult emRet = Msg_Handle_Continue;
	ISceneMessageHandler * pHandler = GetTopHanderPointer();
	if (pHandler)
	{
		emRet = pHandler->OnKeyDown(pWnd, nChar, nRepCnt, nFlags);
		//如果需要退出，则退出
		if (emRet == Msg_Handle_Exit)
		{
			PopHandler();
		}
	}
	return emRet;
}

EmMessageHandlerResult MessageHandlerManager::OnKeyUp(IN CWnd * pWnd, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	EmMessageHandlerResult emRet = Msg_Handle_Continue;
	ISceneMessageHandler * pHandler = GetTopHanderPointer();
	if (pHandler)
	{
		emRet = pHandler->OnKeyUp(pWnd, nChar, nRepCnt, nFlags);
		//如果需要退出，则退出
		if (emRet == Msg_Handle_Exit)
		{
			PopHandler();
		}
	}
	return emRet;
}

EmMessageHandlerResult MessageHandlerManager::OnChar(IN CWnd * pWnd, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	EmMessageHandlerResult emRet = Msg_Handle_Continue;
	ISceneMessageHandler * pHandler = GetTopHanderPointer();
	if (pHandler)
	{
		emRet = pHandler->OnChar(pWnd, nChar, nRepCnt, nFlags);
		//如果需要退出，则退出
		if (emRet == Msg_Handle_Exit)
		{
			PopHandler();
		}
	}
	return emRet;
}
