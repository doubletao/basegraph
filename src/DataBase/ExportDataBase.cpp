#include "stdafx.h"
#include "ExportDataBase.h"
#include "DataBaseManager.h"
#include "ObjectFactory.h"

bool BASEGRAPH_DATABASE_API DataBase_GetAllItemID(OUT std::vector<CString>& vecID)
{
	return DataBaseManager::Instance().GetAllItemID(vecID);
}

bool BASEGRAPH_DATABASE_API DataBase_GetItemType(IN CString strID, OUT BaseGraph_DataBase::ObjectType & emType)
{
	return DataBaseManager::Instance().GetItemType(strID, emType);
}

bool BASEGRAPH_DATABASE_API DataBase_CreateItem(IN BaseGraph_DataBase::ObjectType emType, IN const std::map<BaseGraph_DataBase::ParamValueDef, Value>& mapParam, OUT CString & strID)
{
	return DataBaseManager::Instance().CreateItem(emType, mapParam, strID);
}

bool BASEGRAPH_DATABASE_API DataBase_AddSubItemTo(IN CString strParentID, IN CString strSubID)
{
	return DataBaseManager::Instance().AddSubItemTo(strParentID, strSubID);
}

bool BASEGRAPH_DATABASE_API DataBase_DeleteItem(IN CString strID)
{
	return DataBaseManager::Instance().DeleteItem(strID);
}

bool BASEGRAPH_DATABASE_API DataBase_MoveItemToTop(IN CString strID)
{
	return DataBaseManager::Instance().MoveItemToTop(strID);
}

bool BASEGRAPH_DATABASE_API DataBase_MoveItemToBottom(IN CString strID)
{
	return DataBaseManager::Instance().MoveItemToBottom(strID);
}

bool BASEGRAPH_DATABASE_API DataBase_MoveItemBefore(IN CString strSrcID, IN CString strDesID)
{
	return DataBaseManager::Instance().MoveItemBefore(strSrcID, strDesID);
}

bool BASEGRAPH_DATABASE_API DataBase_MoveItemAfter(IN CString strSrcID, IN CString strDesID)
{
	return DataBaseManager::Instance().MoveItemAfter(strSrcID, strDesID);
}

bool BASEGRAPH_DATABASE_API DataBase_GetItemProperty(IN CString strID, IN BaseGraph_DataBase::ParamValueDef emParam, OUT Value & stuParam)
{
	return DataBaseManager::Instance().GetItemProperty(strID, emParam, stuParam);
}

bool BASEGRAPH_DATABASE_API DataBase_GetItemProperty(IN CString strID, OUT std::map<BaseGraph_DataBase::ParamValueDef, Value>& mapParam)
{
	return DataBaseManager::Instance().GetItemProperty(strID, mapParam);
}

bool BASEGRAPH_DATABASE_API DataBase_SetItemProperty(IN CString strID, IN BaseGraph_DataBase::ParamValueDef emParam, IN const Value & stuParam)
{
	return DataBaseManager::Instance().SetItemProperty(strID, emParam, stuParam);
}

bool BASEGRAPH_DATABASE_API DataBase_SetItemProperty(IN CString strID, IN const std::map<BaseGraph_DataBase::ParamValueDef, Value>& mapParam)
{
	return DataBaseManager::Instance().SetItemProperty(strID, mapParam);
}

bool BASEGRAPH_DATABASE_API DataBase_GetItemLength(OUT int & nLength)
{
	return DataBaseManager::Instance().GetItemLength(nLength);
}

bool BASEGRAPH_DATABASE_API DataBase_GetItemLength(IN const std::vector<CString>& vecID, OUT int & nLength)
{
	return DataBaseManager::Instance().GetItemLength(vecID, nLength);
}

bool BASEGRAPH_DATABASE_API DataBase_SaveToFile(IN CString strFile)
{
	return DataBaseManager::Instance().SaveToFile(strFile);
}

bool BASEGRAPH_DATABASE_API DataBase_LoadFromFile(IN CString strFile)
{
	return DataBaseManager::Instance().LoadFromFile(strFile);
}

std::map<BaseGraph_DataBase::ParamValueDef, Value>BASEGRAPH_DATABASE_API DataBase_GetDefaultParam(IN BaseGraph_DataBase::ObjectType emType)
{
	std::map<BaseGraph_DataBase::ParamValueDef, Value> mapParam;
	return ObjectFactory::GetDefaultParam(emType);
}
