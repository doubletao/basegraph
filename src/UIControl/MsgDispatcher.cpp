#include "stdafx.h"
#include "UIControl\MsgDispatcher.h"


MsgDispatcher::MsgDispatcher()
{
}


MsgDispatcher::~MsgDispatcher()
{
}

MsgDispatcher & MsgDispatcher::Instance()
{
	static MsgDispatcher inst;
	return inst;
}

void MsgDispatcher::RegistMsgDispatcher(IMsgDispatchObject * pMsgObj)
{
	m_mapDispatchObject.insert(std::make_pair(pMsgObj->GetMsgDispatchID(), pMsgObj));
}

void MsgDispatcher::UnRegistDispatchObj(IMsgDispatchObject * pMsgObj)
{
	if (m_mapDispatchObject.count(pMsgObj->GetMsgDispatchID()))
	{
		m_mapDispatchObject.erase(pMsgObj->GetMsgDispatchID());
	}
}

void MsgDispatcher::BroadcastMsg(int nMsgID, WPARAM wParam, LPARAM lParam, int nIDFrom)
{
	for (std::map<int, IMsgDispatchObject *>::iterator it = m_mapDispatchObject.begin(); it != m_mapDispatchObject.end(); it++)
	{
		if (it->second)
		{
			it->second->OnDispatchMsg(nMsgID, wParam, lParam, nIDFrom);
		}
	}
}

void MsgDispatcher::BroadcastMsg(int nMsgID, WPARAM wParam, LPARAM lParam, IMsgDispatchObject * pMsgObj)
{
	for (std::map<int, IMsgDispatchObject *>::iterator it = m_mapDispatchObject.begin(); it != m_mapDispatchObject.end(); it++)
	{
		if (it->second)
		{
			if (pMsgObj)
			{
				it->second->OnDispatchMsg(nMsgID, wParam, lParam, pMsgObj->GetMsgDispatchID());
			}
			else
			{
				it->second->OnDispatchMsg(nMsgID, wParam, lParam, 0);
			}
		}
	}
}

void MsgDispatcher::SendMsgTo(int nIDTo, int nMsgID, WPARAM wParam, LPARAM lParam, int nIDFrom)
{
	if (m_mapDispatchObject.count(nIDTo) > 0)
	{
		IMsgDispatchObject * pObj = m_mapDispatchObject[nIDTo];
		if (pObj)
		{
			pObj->OnDispatchMsg(nMsgID, wParam, lParam, nIDFrom);
		}
	}
}

IMsgDispatchObject * MsgDispatcher::GetMsgDispatcher(int nDispatcherID)
{
	if (m_mapDispatchObject.count(nDispatcherID) > 0)
	{
		return m_mapDispatchObject[nDispatcherID];
	}
	return nullptr;
}

///////////////////////IMsgDispatchObject//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void IMsgDispatchObject::OnDispatchMsg(int nMsgID, WPARAM wParam, LPARAM lParam, int nIDFrom)
{
}

void IMsgDispatchObject::UnRegistDispatchObj()
{
	MsgDispatcher::Instance().UnRegistDispatchObj(this);
}
