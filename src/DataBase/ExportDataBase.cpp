#include "stdafx.h"
#include "ExportDataBase.h"

bool BASEGRAPH_DATABASE_API DataBase_CreateItem(IN BaseGraph_DataBase::ObjectType emType, IN const std::map<BaseGraph_DataBase::ParamValueDef, Value>& mapParam, OUT int & nID)
{
	return false;
}

bool BASEGRAPH_DATABASE_API DataBase_AddSubItemTo(IN int nParentID, IN int nSubID)
{
	return false;
}

bool BASEGRAPH_DATABASE_API DataBase_DeleteItem(IN int nID)
{
	return false;
}

bool BASEGRAPH_DATABASE_API DataBase_MoveItemToTop(IN int nID)
{
	return false;
}

bool BASEGRAPH_DATABASE_API DataBase_MoveItemToBottom(IN int nID)
{
	return false;
}

bool BASEGRAPH_DATABASE_API DataBase_MoveItemBefore(IN int nSrcID, IN int nDesID)
{
	return false;
}

bool BASEGRAPH_DATABASE_API DataBase_MoveItemAfter(IN int nSrcID, IN int nDesID)
{
	return false;
}

bool BASEGRAPH_DATABASE_API DataBase_GetItemProperty(IN int nID, IN BaseGraph_DataBase::ParamValueDef emParam, OUT Value & stuParam)
{
	return false;
}

bool BASEGRAPH_DATABASE_API DataBase_GetItemProperty(IN int nID, OUT std::map<BaseGraph_DataBase::ParamValueDef, Value>& mapParam)
{
	return false;
}

bool BASEGRAPH_DATABASE_API DataBase_SetItemProperty(IN int nID, IN BaseGraph_DataBase::ParamValueDef emParam, IN const Value & stuParam)
{
	return false;
}

bool BASEGRAPH_DATABASE_API DataBase_SetItemProperty(IN int nID, IN const std::map<BaseGraph_DataBase::ParamValueDef, Value>& mapParam)
{
	return false;
}

bool BASEGRAPH_DATABASE_API DataBase_SaveToFile(CString strFile)
{
	return false;
}

bool BASEGRAPH_DATABASE_API DataBase_LoadFromFile(CString strFile)
{
	return false;
}
