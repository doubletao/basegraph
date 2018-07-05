#include "stdafx.h"
#include "ExportDataBase.h"
#include "DataBaseManager.h"
#include "ObjectFactory.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
bool BASEGRAPH_DATABASE_API DataBase_GetAllItemID(OUT std::vector<CString>& vecID)
{
	return DataBaseManager::Instance().GetAllItemID(vecID);
}

bool BASEGRAPH_DATABASE_API DataBase_GetAllItemIDExceptSubItem(OUT std::vector<CString>& vecID)
{
	return DataBaseManager::Instance().GetAllItemIDExceptSubItem(vecID);
}

bool BASEGRAPH_DATABASE_API DataBase_GetItemType(IN CString strID, OUT BaseGraph_DataBase::ObjectType & emType)
{
	return DataBaseManager::Instance().GetItemType(strID, emType);
}

BASEGRAPH_DATABASE_API BaseGraph_DataBase::ObjectType DataBase_GetItemType(IN CString strID)
{
	BaseGraph_DataBase::ObjectType emType = BaseGraph_DataBase::ObjectType_Err;
	DataBaseManager::Instance().GetItemType(strID, emType);
	return emType;
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

float3 BASEGRAPH_DATABASE_API DataBase_GetItemPos(IN CString strID)
{
	return DataBaseManager::Instance().GetItemPos(strID);
}

float3 BASEGRAPH_DATABASE_API DataBase_GetItemCenter(IN CString strID)
{
	return DataBaseManager::Instance().GetItemCenter(strID);
}

float3 BASEGRAPH_DATABASE_API DataBase_GetItemAngle(IN CString strID)
{
	return DataBaseManager::Instance().GetItemAngle(strID);
}

float3 BASEGRAPH_DATABASE_API DataBase_GetItemAngleRadian(IN CString strID)
{
	return DataBaseManager::Instance().GetItemAngleRadian(strID);
}

float3 BASEGRAPH_DATABASE_API DataBase_GetItemScale(IN CString strID)
{
	return DataBaseManager::Instance().GetItemScale(strID);
}

bool BASEGRAPH_DATABASE_API DataBase_GetItemTypePropertyIDRange(IN BaseGraph_DataBase::ObjectType emType, OUT BaseGraph_DataBase::ParamValueDef & emParamBegin, OUT BaseGraph_DataBase::ParamValueDef & emParamEmd)
{
	return GetItemPropertyIDRange(emType, emParamBegin, emParamEmd);
}

bool BASEGRAPH_DATABASE_API DataBase_GetItemPropertyIDRange(IN CString strID, OUT BaseGraph_DataBase::ParamValueDef & emParamBegin, OUT BaseGraph_DataBase::ParamValueDef & emParamEmd)
{
	BaseGraph_DataBase::ObjectType emType;
	if (DataBaseManager::Instance().GetItemType(strID, emType))
	{
		return GetItemPropertyIDRange(emType, emParamBegin, emParamEmd);
	}
	return false;
}

BaseGraph_DataBase::ITEM_FORMTYPE BASEGRAPH_DATABASE_API DataBase_Get3DType(IN CString strID)
{
	return DataBaseManager::Instance().Get3DType(strID);
}

CString BASEGRAPH_DATABASE_API DataBase_GetNextItemName(IN BaseGraph_DataBase::ObjectType emItemType)
{
	return DataBaseManager::Instance().GetNextItemName(emItemType);
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

bool BASEGRAPH_DATABASE_API DataBase_HasSubItem(IN CString strParentID)
{
	return DataBaseManager::Instance().HasSubItem(strParentID);
}

bool BASEGRAPH_DATABASE_API DataBase_GetSubItem(IN CString strParentID, OUT std::vector<CString>& vecSubID)
{
	return DataBaseManager::Instance().GetSubItem(strParentID, vecSubID);
}

bool BASEGRAPH_DATABASE_API DataBase_HasParentItem(IN CString strSubID)
{
	return DataBaseManager::Instance().HasParentItem(strSubID);
}

bool BASEGRAPH_DATABASE_API DataBase_GetParentItem(IN CString strSubID, OUT CString & strParentID)
{
	return DataBaseManager::Instance().GetParentItem(strSubID, strParentID);
}

int BASEGRAPH_DATABASE_API DataBase_GetSceneWidth()
{
	return DataBaseManager::Instance().GetSceneWidth();
}

int BASEGRAPH_DATABASE_API DataBase_GetSceneHeight()
{
	return DataBaseManager::Instance().GetSceneHeight();
}

bool BASEGRAPH_DATABASE_API DataBase_SaveToFile(IN CString strFile)
{
	return DataBaseManager::Instance().SaveToFile(strFile);
}

bool BASEGRAPH_DATABASE_API DataBase_LoadFromFile(IN CString strFile)
{
	return DataBaseManager::Instance().LoadFromFile(strFile);
}

std::map<BaseGraph_DataBase::ParamValueDef, Value> BASEGRAPH_DATABASE_API DataBase_GetDefaultParam(IN BaseGraph_DataBase::ObjectType emType)
{
	return ObjectFactory::GetDefaultParam(emType);
}

bool BASEGRAPH_DATABASE_API DataBase_LockData()
{
	DataBaseManager::Instance().Lock();
	return true;
}

bool BASEGRAPH_DATABASE_API DataBase_UnlockData()
{
	DataBaseManager::Instance().Unlock();
	return false;
}
