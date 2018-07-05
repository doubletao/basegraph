#include "stdafx.h"
#include "DataResourceManager.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
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
