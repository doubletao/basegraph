#include "stdafx.h"
#include "DataBaseManager.h"
#include "ObjectFactory.h"
#include "IntIDGenerator.h"
#include <assert.h>

DataBaseManager::DataBaseManager()
	: m_nSceneWidth(1920)
	, m_nSceneHeight(1080)
{
	InitializeCriticalSection(&m_lockData);
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
	DeleteCriticalSection(&m_lockData);
}

DataBaseManager & DataBaseManager::Instance()
{
	static DataBaseManager inst;
	return inst;
}

//#define DEBUG_LOCK
void DataBaseManager::Lock()
{
#ifdef DEBUG_LOCK
	if (true)
	{
		CString strDebug;
		strDebug.Format(_T("DataBaseManager::Lock() ask\n"));
		OutputDebugString(strDebug);
	}
#endif // DEBUG
	EnterCriticalSection(&m_lockData);
#ifdef DEBUG_LOCK
	if (true)
	{
		CString strDebug;
		strDebug.Format(_T("DataBaseManager::Lock() locked\n"));
		OutputDebugString(strDebug);
	}
#endif // DEBUG
}

void DataBaseManager::Unlock()
{
	LeaveCriticalSection(&m_lockData);
#ifdef DEBUG_LOCK
	if (true)
	{
		OutputDebugString(_T("DataBaseManager::Unlock()\n"));
	}
#endif // DEBUG
}

bool DataBaseManager::CheckItemID(CString strID)
{
	for (size_t i = 0; i < m_vecObject.size(); i++)
	{
		if (m_vecObject[i] && m_vecObject[i]->GetItemID() == strID)
		{
			return false;
		}
	}
	return true;
}

bool DataBaseManager::GetAllItemID(OUT std::vector<CString>& vecID)
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

bool DataBaseManager::GetAllItemIDExceptSubItem(OUT std::vector<CString>& vecID)
{
	vecID.clear();
	for (size_t i = 0; i < m_vecObject.size(); i++)
	{
		if (m_vecObject[i]
			&& !HasSubItem(m_vecObject[i]->GetItemID()))
		{
			vecID.push_back(m_vecObject[i]->GetItemID());
		}
	}
	return true;
}

bool DataBaseManager::GetItemType(IN CString strID, OUT BaseGraph_DataBase::ObjectType & emType)
{
	BaseObject * pObj = nullptr;
	if (GetItem(strID, pObj)
		&& pObj)
	{
		emType = pObj->GetItemType();
		return true;
	}
	return false;
}

bool DataBaseManager::GetAllItem(OUT vector<BaseObject *>& vecObject)
{
	return false;
}

bool DataBaseManager::GetItem(IN CString strID, OUT BaseObject * & pObject)
{
	for (size_t i = 0; i < m_vecObject.size(); i++)
	{
		if (m_vecObject[i] && m_vecObject[i]->GetItemID() == strID)
		{
			pObject = m_vecObject[i];
			return true;
		}
	}
	return false;
}

bool DataBaseManager::CreateItem(IN ObjectType emType, IN const map<BaseGraph_DataBase::ParamValueDef, Value>& mapParam, OUT CString & strID)
{
	//物件厂造一个指定物件并赋值
	BaseObject * pObj = ObjectFactory::CreateObject(emType);
	if (pObj)
	{
		pObj->SetProperty(mapParam);
		strID = pObj->GetItemID();
		if (AddItem(pObj))
		{
			return true;
		}
		delete pObj;
		pObj = nullptr;
		return false;
	}
	return false;
}

bool DataBaseManager::AddItem(BaseObject * pObj)
{
	CString strID = pObj->GetItemID();
	if (CheckItemID(strID))
	{
		m_vecObject.push_back(pObj);
		return true;
	}
	else
	{
		return false;
	}
}

bool DataBaseManager::AddSubItemTo(IN CString strParentID, IN CString strSubID)
{
	return false;
}

bool DataBaseManager::DeleteItem(IN CString strID)
{
	return false;
}

bool DataBaseManager::MoveItemToTop(IN CString strID)
{
	return false;
}

bool DataBaseManager::MoveItemToBottom(IN CString strID)
{
	return false;
}

bool DataBaseManager::MoveItemBefore(IN CString strSrcID, IN CString strDesID)
{
	return false;
}

bool DataBaseManager::MoveItemAfter(IN CString strSrcID, IN CString strDesID)
{
	return false;
}

bool DataBaseManager::HasSubItem(IN CString strParentID)
{
	return false;
}

bool DataBaseManager::GetSubItem(IN CString strParentID, OUT std::vector<CString>& vecSubID)
{
	return false;
}

bool DataBaseManager::HasParentItem(IN CString strSubID)
{
	return false;
}

bool DataBaseManager::GetParentItem(IN CString strSubID, OUT CString & strParentID)
{
	return false;
}

bool DataBaseManager::GetItemProperty(IN CString strID, IN ParamValueDef emParam, OUT Value & stuParam)
{
	BaseObject * pObj = nullptr;
	if (GetItem(strID, pObj)
		&& pObj)
	{
		stuParam = pObj->GetValue(emParam);
		return true;
	}
	return false;
}

bool DataBaseManager::GetItemProperty(IN CString strID, OUT map<ParamValueDef, Value>& mapParam)
{
	BaseObject * pObj = nullptr;
	if (GetItem(strID, pObj)
		&& pObj)
	{
		mapParam = pObj->GetProperty();
		return true;
	}
	return false;
}

float3 DataBaseManager::GetItemPos(IN CString strID)
{
	float3 f3Pos = {0};
	BaseObject * pObj = nullptr;
	if (GetItem(strID, pObj)
		&& pObj)
	{
		f3Pos = pObj->GetItemPos();
	}
	return f3Pos;
}

float3 DataBaseManager::GetItemCenter(IN CString strID)
{
	float3 f3Center = { 0 };
	BaseObject * pObj = nullptr;
	if (GetItem(strID, pObj)
		&& pObj)
	{
		f3Center = pObj->GetItemCenter();
	}
	return f3Center;
}

float3 DataBaseManager::GetItemAngle(IN CString strID)
{
	float3 f3Angle = { 0 };
	BaseObject * pObj = nullptr;
	if (GetItem(strID, pObj)
		&& pObj)
	{
		f3Angle = pObj->GetItemRotate();
	}
	return f3Angle;
}

float3 DataBaseManager::GetItemScale(IN CString strID)
{
	float3 f3Scale = { 0 };
	BaseObject * pObj = nullptr;
	if (GetItem(strID, pObj)
		&& pObj)
	{
		f3Scale = pObj->GetItemScale();
	}
	return f3Scale;
}

bool DataBaseManager::SetItemProperty(IN CString strID, IN ParamValueDef emParam, IN const Value & stuParam)
{
	BaseObject * pObj = nullptr;
	if (GetItem(strID, pObj)
		&& pObj)
	{
		pObj->SetValue(emParam, stuParam);
		return true;
	}
	return false;
}

bool DataBaseManager::SetItemProperty(IN CString strID, IN const map<ParamValueDef, Value>& mapParam)
{
	BaseObject * pObj = nullptr;
	if (GetItem(strID, pObj)
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

bool DataBaseManager::GetItemLength(OUT int & nLength)
{
	std::vector<CString> vecItemID;
	GetAllItemID(vecItemID);
	return GetItemLength(vecItemID, nLength);
}

bool DataBaseManager::GetItemLength(IN const std::vector<CString>& vecID, OUT int & nLength)
{
	bool bRet = false;
	nLength = 0;
	for (size_t i = 0; i < vecID.size(); i++)
	{
		CString strID = vecID[i];
		BaseGraph_Common::Value valueEndFrame;
		if (GetItemProperty(strID, BaseGraph_DataBase::Obj_ActionEnd_Int, valueEndFrame))
		{
			int nEndFrame = valueEndFrame.ToInt();
			if (nLength < nEndFrame)
			{
				nLength = nEndFrame;
			}
			bRet = true;
		}
	}
	return bRet;
}

int DataBaseManager::GetSceneWidth()
{
	return m_nSceneWidth;
}

int DataBaseManager::GetSceneHeight()
{
	return m_nSceneHeight;
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

