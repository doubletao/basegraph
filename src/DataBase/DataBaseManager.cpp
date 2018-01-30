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

bool DataBaseManager::GetAllItemID(OUT std::vector<int>& vecID)
{
	vecID.clear();
	for (size_t i = 0; i < m_vecObject.size(); i++)
	{
		if (m_vecObject[i])
		{
			vecID.push_back(m_vecObject[i]->GetItemID());
		}
	}
	return true;
}

bool DataBaseManager::GetItemType(IN int nID, OUT BaseGraph_DataBase::ObjectType & emType)
{
	BaseObject * pObj = nullptr;
	if (GetItem(nID, pObj)
		&& pObj)
	{
		BaseGraph_Common::Value stuParam = pObj->GetValue(BaseGraph_DataBase::ObjType);
		emType = (BaseGraph_DataBase::ObjectType)stuParam.ToInt();
		return true;
	}
	return false;
}

bool DataBaseManager::GetAllItem(OUT vector<BaseObject *>& vecObject)
{
	return false;
}

bool DataBaseManager::GetItem(IN int nID, OUT BaseObject * & pObject)
{
	for (size_t i = 0; i < m_vecObject.size(); i++)
	{
		if (m_vecObject[i] && m_vecObject[i]->GetItemID() == nID)
		{
			pObject = m_vecObject[i];
			return true;
		}
	}
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
	BaseObject * pObj = nullptr;
	if (GetItem(nID, pObj)
		&& pObj)
	{
		stuParam = pObj->GetValue(emParam);
		return true;
	}
	return false;
}

bool DataBaseManager::GetItemProperty(IN int nID, OUT map<ParamValueDef, Value>& mapParam)
{
	BaseObject * pObj = nullptr;
	if (GetItem(nID, pObj)
		&& pObj)
	{
		mapParam = pObj->GetProperty();
		return true;
	}
	return false;
}

bool DataBaseManager::SetItemProperty(IN int nID, IN ParamValueDef emParam, IN const Value & stuParam)
{
	BaseObject * pObj = nullptr;
	if (GetItem(nID, pObj)
		&& pObj)
	{
		pObj->SetValue(emParam, stuParam);
		return true;
	}
	return false;
}

bool DataBaseManager::SetItemProperty(IN int nID, IN const map<ParamValueDef, Value>& mapParam)
{
	BaseObject * pObj = nullptr;
	if (GetItem(nID, pObj)
		&& pObj)
	{
		for (map<ParamValueDef, Value>::const_iterator it = mapParam.begin(); it != mapParam.end(); it++)
		{
			pObj->SetValue(it->first, it->second);
		}
		return true;
	}
	return false;
}

bool DataBaseManager::SaveToFile(CString strFile)
{
	CFile cf;
	CFileException e;
	BOOL bOpenSuc = FALSE;
	bOpenSuc = cf.Open(strFile, CFile::modeCreate | CFile::modeWrite, &e);

	if (bOpenSuc)
	{
		CArchive ar(&cf, CArchive::store);
		//先序列化物件
		bool bRet = SerializeObjects(ar, m_vecObject);
		//序列化其他
		ar.Close();
		cf.Close();
		return bRet;
	}
	return false;
}

bool DataBaseManager::LoadFromFile(CString strFile)
{
	CFile cf;
	CFileException e;
	BOOL bOpenSuc = FALSE;
	if (_taccess(strFile, 0) != -1)
	{
		bOpenSuc = cf.Open(strFile, CFile::modeRead, &e);
	}

	if (bOpenSuc)
	{
		CArchive ar(&cf, CArchive::load);
		bool bRet = SerializeObjects(ar, m_vecObject);
		ar.Close();
		cf.Close();
		return bRet;
	}
	return false;
}

bool DataBaseManager::SerializeObjects(CArchive & archive, vector<BaseObject*>& vecObject)
{
	if (archive.IsStoring())
	{
	}
	else
	{
	}
	return false;
}

