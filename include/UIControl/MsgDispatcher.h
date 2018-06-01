#pragma once
#include <map>
/*
 * 一个消息广播及分发的类，为了方便消息传递使用
*/

class BASEGRAPH_UICONTROL_API IMsgDispatchObject
{
public:
	virtual int GetMsgDispatchID() = 0;
	virtual void OnDispatchMsg(int nMsgID, WPARAM wParam, LPARAM lParam, int nIDFrom);
	virtual void UnRegistDispatchObj();//析构前应调用注销，防止野指针
};

class BASEGRAPH_UICONTROL_API MsgDispatcher
{
private:
	MsgDispatcher();
	~MsgDispatcher();
	std::map<int, IMsgDispatchObject *> m_mapDispatchObject;
public:
	static MsgDispatcher & Instance();
	void RegistMsgDispatcher(IMsgDispatchObject * pMsgObj);
	void UnRegistDispatchObj(IMsgDispatchObject * pMsgObj);
	void BroadcastMsg(int nMsgID, WPARAM wParam, LPARAM lParam, int nIDFrom = 0);
	void BroadcastMsg(int nMsgID, WPARAM wParam, LPARAM lParam, IMsgDispatchObject * pMsgObj = nullptr);
	void SendMsgTo(int nIDTo, int nMsgID, WPARAM wParam, LPARAM lParam, int nIDFrom = 0);
};
