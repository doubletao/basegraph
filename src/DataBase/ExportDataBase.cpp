#include "stdafx.h"
#include "ExportDataBase.h"
#include "DataBaseManager.h"

bool BASEGRAPH_DATABASE_API DataBase_GetAllItemID(OUT std::vector<int>& vecID)
{
	return DataBaseManager::Instance().GetAllItemID(vecID);
}

bool BASEGRAPH_DATABASE_API DataBase_GetItemType(IN int nID, OUT BaseGraph_DataBase::ObjectType & emType)
{
	return DataBaseManager::Instance().GetItemType(nID, emType);
}

bool BASEGRAPH_DATABASE_API DataBase_CreateItem(IN BaseGraph_DataBase::ObjectType emType, IN const std::map<BaseGraph_DataBase::ParamValueDef, Value>& mapParam, OUT int & nID)
{
	return DataBaseManager::Instance().CreateItem(emType, mapParam, nID);
}

bool BASEGRAPH_DATABASE_API DataBase_AddSubItemTo(IN int nParentID, IN int nSubID)
{
	return DataBaseManager::Instance().AddSubItemTo(nParentID, nSubID);
}

bool BASEGRAPH_DATABASE_API DataBase_DeleteItem(IN int nID)
{
	return DataBaseManager::Instance().DeleteItem(nID);
}

bool BASEGRAPH_DATABASE_API DataBase_MoveItemToTop(IN int nID)
{
	return DataBaseManager::Instance().MoveItemToTop(nID);
}

bool BASEGRAPH_DATABASE_API DataBase_MoveItemToBottom(IN int nID)
{
	return DataBaseManager::Instance().MoveItemToBottom(nID);
}

bool BASEGRAPH_DATABASE_API DataBase_MoveItemBefore(IN int nSrcID, IN int nDesID)
{
	return DataBaseManager::Instance().MoveItemBefore(nSrcID, nDesID);
}

bool BASEGRAPH_DATABASE_API DataBase_MoveItemAfter(IN int nSrcID, IN int nDesID)
{
	return DataBaseManager::Instance().MoveItemAfter(nSrcID, nDesID);
}

bool BASEGRAPH_DATABASE_API DataBase_GetItemProperty(IN int nID, IN BaseGraph_DataBase::ParamValueDef emParam, OUT Value & stuParam)
{
	return DataBaseManager::Instance().GetItemProperty(nID, emParam, stuParam);
}

//bool BASEGRAPH_DATABASE_API DataBase_GetItemProperty(IN int nID, OUT std::map<BaseGraph_DataBase::ParamValueDef, Value>& mapParam)
//{
//	return DataBaseManager::Instance().GetItemProperty(nID, mapParam);
//}

bool BASEGRAPH_DATABASE_API DataBase_SetItemProperty(IN int nID, IN BaseGraph_DataBase::ParamValueDef emParam, IN const Value & stuParam)
{
	return DataBaseManager::Instance().SetItemProperty(nID, emParam, stuParam);
}

bool BASEGRAPH_DATABASE_API DataBase_SetItemProperty(IN int nID, IN const std::map<BaseGraph_DataBase::ParamValueDef, Value>& mapParam)
{
	return DataBaseManager::Instance().SetItemProperty(nID, mapParam);
}

bool BASEGRAPH_DATABASE_API DataBase_SaveToFile(IN CString strFile)
{
	return DataBaseManager::Instance().SaveToFile(strFile);
}

bool BASEGRAPH_DATABASE_API DataBase_LoadFromFile(IN CString strFile)
{
	return DataBaseManager::Instance().LoadFromFile(strFile);
}
