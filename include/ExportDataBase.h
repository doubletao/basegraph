#pragma once
#ifdef BASEGRAPH_DATABASE_EXPORTS
#define BASEGRAPH_DATABASE_API __declspec(dllexport)
#else
#define BASEGRAPH_DATABASE_API __declspec(dllimport)
#ifdef BASEGRAPH_AUTO_LOAD
#ifdef _DEBUG
#	pragma comment(lib, "DataBaseD.lib")
#else
#	pragma comment(lib, "DataBase.lib")
#endif
#endif // BASEGRAPH_AUTO_LOAD
#endif // BASEGRAPH_COMMON_EXPORTS

#include "DataBase\DataTypeDef.h"

//物件管理
bool BASEGRAPH_DATABASE_API DataBase_GetAllItemID(OUT std::vector<int> & vecID);
bool BASEGRAPH_DATABASE_API DataBase_GetItemType(IN int nID, OUT BaseGraph_DataBase::ObjectType & emType);
//增删
bool BASEGRAPH_DATABASE_API DataBase_CreateItem(IN BaseGraph_DataBase::ObjectType emType, IN const std::map<BaseGraph_DataBase::ParamValueDef, Value> & mapParam, OUT int & nID);
bool BASEGRAPH_DATABASE_API DataBase_AddSubItemTo(IN int nParentID, IN int nSubID);
bool BASEGRAPH_DATABASE_API DataBase_DeleteItem(IN int nID);
//层次调整
bool BASEGRAPH_DATABASE_API DataBase_MoveItemToTop(IN int nID);
bool BASEGRAPH_DATABASE_API DataBase_MoveItemToBottom(IN int nID);
bool BASEGRAPH_DATABASE_API DataBase_MoveItemBefore(IN int nSrcID, IN int nDesID);
bool BASEGRAPH_DATABASE_API DataBase_MoveItemAfter(IN int nSrcID, IN int nDesID);
//属性获取
bool BASEGRAPH_DATABASE_API DataBase_GetItemProperty(IN int nID, IN BaseGraph_DataBase::ParamValueDef emParam, OUT Value & stuParam);
//bool BASEGRAPH_DATABASE_API DataBase_GetItemProperty(IN int nID, OUT std::map<BaseGraph_DataBase::ParamValueDef, Value> & mapParam);
//属性设置
bool BASEGRAPH_DATABASE_API DataBase_SetItemProperty(IN int nID, IN BaseGraph_DataBase::ParamValueDef emParam, IN const Value & stuParam);
bool BASEGRAPH_DATABASE_API DataBase_SetItemProperty(IN int nID, IN const std::map<BaseGraph_DataBase::ParamValueDef, Value> & mapParam);
//序列化
bool BASEGRAPH_DATABASE_API DataBase_SaveToFile(IN CString strFile);
bool BASEGRAPH_DATABASE_API DataBase_LoadFromFile(IN CString strFile);
