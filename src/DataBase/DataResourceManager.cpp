#include "stdafx.h"
#include "DataResourceManager.h"


DataResourceManager::DataResourceManager()
{
}


DataResourceManager::~DataResourceManager()
{
}

DataResourceManager & DataResourceManager::Instance()
{
	static DataResourceManager inst;
	return inst;
}

CString DataResourceManager::AddResource(CString strSrc)
{
	CString strID = CommonFunction::CreateGUID();
	m_mapResource[strID] = strSrc;
	return strID;
}

bool DataResourceManager::DelResource(CString strID)
{
	if (m_mapResource.count(strID) > 0)
	{
		m_mapResource.erase(strID);
	}
	return true;
}

CString DataResourceManager::GetResource(CString strID)
{
	if (m_mapResource.count(strID) > 0)
	{
		return m_mapResource[strID];
	}
	return CString();
}
