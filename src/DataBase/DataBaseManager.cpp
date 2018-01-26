#include "stdafx.h"
#include "DataBaseManager.h"


DataBaseManager::DataBaseManager()
{
}


DataBaseManager::~DataBaseManager()
{
	for (size_t i = 0; i < m_vecObject.size(); i++)
	{
		BaseObject *& pObj = m_vecObject[i];
		delete pObj;
		pObj = nullptr;
	}
	m_vecObject.clear();
}

DataBaseManager & DataBaseManager::Instance()
{
	static DataBaseManager inst;
	return inst;
}

bool DataBaseManager::GetAllItem(OUT vector<BaseObject *>& vecObject)
{
	return false;
}

bool DataBaseManager::GetAllItemExpandSub(OUT vector<BaseObject *>& vecObject)
{
	return false;
}

bool DataBaseManager::GetItem(IN int nID, OUT BaseObject * & stuObject)
{
	return false;
}

bool DataBaseManager::CreateItem(IN ObjectType emType, IN const map<BaseGraph_DataBase::ParamValueDef, Value>& mapParam, OUT int & nID)
{
	return false;
}

bool DataBaseManager::AddSubItemTo(IN int nParentID, IN int nSubID)
{
	return false;
}

bool DataBaseManager::DeleteItem(IN int nID)
{
	return false;
}

bool DataBaseManager::MoveItemToTop(IN int nID)
{
	return false;
}

bool DataBaseManager::MoveItemToBottom(IN int nID)
{
	return false;
}

bool DataBaseManager::MoveItemBefore(IN int nSrcID, IN int nDesID)
{
	return false;
}

bool DataBaseManager::MoveItemAfter(IN int nSrcID, IN int nDesID)
{
	return false;
}

bool DataBaseManager::GetItemProperty(IN int nID, IN ParamValueDef emParam, OUT Value & stuParam)
{
	return false;
}

bool DataBaseManager::GetItemProperty(IN int nID, OUT map<ParamValueDef, Value>& mapParam)
{
	return false;
}

bool DataBaseManager::SetItemProperty(IN int nID, IN ParamValueDef emParam, IN const Value & stuParam)
{
	return false;
}

bool DataBaseManager::SetItemProperty(IN int nID, IN const map<ParamValueDef, Value>& mapParam)
{
	return false;
}

bool DataBaseManager::SaveToFile(CString strFile)
{
	return false;
}

bool DataBaseManager::LoadFromFile(CString strFile)
{
	return false;
}
