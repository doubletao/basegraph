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

#include "DataBase\DataTypeDefDetail.h"
#include "DataBase\DataTypeDef.h"

//物件管理
bool BASEGRAPH_DATABASE_API DataBase_GetAllItemID(OUT std::vector<CString> & vecID);
bool BASEGRAPH_DATABASE_API DataBase_GetAllItemIDExceptSubItem(OUT std::vector<CString>& vecID);
bool BASEGRAPH_DATABASE_API DataBase_GetItemType(IN CString strID, OUT BaseGraph_DataBase::ObjectType & emType);
BASEGRAPH_DATABASE_API BaseGraph_DataBase::ObjectType DataBase_GetItemType(IN CString strID);
//增删
bool BASEGRAPH_DATABASE_API DataBase_CreateItem(IN BaseGraph_DataBase::ObjectType emType, IN const std::map<BaseGraph_DataBase::ParamValueDef, Value> & mapParam, OUT CString & strID);
bool BASEGRAPH_DATABASE_API DataBase_AddSubItemTo(IN CString strParentID, IN CString strSubID);
bool BASEGRAPH_DATABASE_API DataBase_DeleteItem(IN CString strID);
//层次调整
bool BASEGRAPH_DATABASE_API DataBase_MoveItemToTop(IN CString strID);
bool BASEGRAPH_DATABASE_API DataBase_MoveItemToBottom(IN CString strID);
bool BASEGRAPH_DATABASE_API DataBase_MoveItemBefore(IN CString strSrcID, IN CString strDesID);
bool BASEGRAPH_DATABASE_API DataBase_MoveItemAfter(IN CString strSrcID, IN CString strDesID);
//属性获取
bool BASEGRAPH_DATABASE_API DataBase_GetItemProperty(IN CString strID, IN BaseGraph_DataBase::ParamValueDef emParam, OUT Value & stuParam);
bool BASEGRAPH_DATABASE_API DataBase_GetItemProperty(IN CString strID, OUT std::map<BaseGraph_DataBase::ParamValueDef, Value> & mapParam);
//属性设置
bool BASEGRAPH_DATABASE_API DataBase_SetItemProperty(IN CString strID, IN BaseGraph_DataBase::ParamValueDef emParam, IN const Value & stuParam);
bool BASEGRAPH_DATABASE_API DataBase_SetItemProperty(IN CString strID, IN const std::map<BaseGraph_DataBase::ParamValueDef, Value> & mapParam);

float3 BASEGRAPH_DATABASE_API DataBase_GetItemPos(IN CString strID);
float3 BASEGRAPH_DATABASE_API DataBase_GetItemCenter(IN CString strID);
float3 BASEGRAPH_DATABASE_API DataBase_GetItemAngle(IN CString strID);
float3 BASEGRAPH_DATABASE_API DataBase_GetItemAngleRadian(IN CString strID);//取得旋转弧度
float3 BASEGRAPH_DATABASE_API DataBase_GetItemScale(IN CString strID);

//获取物件属性ID边界
bool BASEGRAPH_DATABASE_API DataBase_GetItemTypePropertyIDRange(IN BaseGraph_DataBase::ObjectType emType, OUT BaseGraph_DataBase::ParamValueDef & emParamBegin, OUT BaseGraph_DataBase::ParamValueDef & emParamEmd);
bool BASEGRAPH_DATABASE_API DataBase_GetItemPropertyIDRange(IN CString strID, OUT BaseGraph_DataBase::ParamValueDef & emParamBegin, OUT BaseGraph_DataBase::ParamValueDef & emParamEmd);

BaseGraph_DataBase::ITEM_FORMTYPE BASEGRAPH_DATABASE_API DataBase_Get3DType(IN CString strID);

//名字
CString BASEGRAPH_DATABASE_API DataBase_GetNextItemName(IN BaseGraph_DataBase::ObjectType emItemType);

//ChangLevel
void BASEGRAPH_DATABASE_API DataBase_SetChgLevel(CString strObjID, EmObjectChgLevel emChgLevel);//set是赋值
void BASEGRAPH_DATABASE_API DataBase_AddChgLevel(CString strObjID, EmObjectChgLevel emChgLevel);//add是叠加
void BASEGRAPH_DATABASE_API DataBase_AddChgLevel(CString strObjID, BaseGraph_DataBase::ParamValueDef emParam);//通过变更的属性设置changlevel
EmObjectChgLevel BASEGRAPH_DATABASE_API DataBase_GetChgLevel(CString strObjID);
//根据属性来获取属性对应的changlevel
EmObjectChgLevel BASEGRAPH_DATABASE_API DataBase_GetPropertyChanegLevel(IN BaseGraph_DataBase::ParamValueDef emParam);

//物件长度（注意，这里的单位是场(按照一帧两场计数)）
bool BASEGRAPH_DATABASE_API DataBase_GetItemLength(OUT int & nLength);
bool BASEGRAPH_DATABASE_API DataBase_GetItemLength(IN const std::vector<CString> & vecID, OUT int & nLength);
//父子物件
bool BASEGRAPH_DATABASE_API DataBase_HasSubItem(IN CString strParentID);
bool BASEGRAPH_DATABASE_API DataBase_GetSubItem(IN CString strParentID, OUT std::vector<CString> & vecSubID);
bool BASEGRAPH_DATABASE_API DataBase_HasParentItem(IN CString strSubID);
bool BASEGRAPH_DATABASE_API DataBase_GetParentItem(IN CString strSubID, OUT CString & strParentID);
//场景size
int BASEGRAPH_DATABASE_API DataBase_GetSceneWidth();
int BASEGRAPH_DATABASE_API DataBase_GetSceneHeight();
//序列化
bool BASEGRAPH_DATABASE_API DataBase_SaveToFile(IN CString strFile, IN bool bResIncluded);//bResIncluded指示是否需要将资源序列化进去
bool BASEGRAPH_DATABASE_API DataBase_LoadFromFile(IN CString strFile, IN bool bResIncluded);
//XML
bool BASEGRAPH_DATABASE_API DataBase_SaveObjToXMLStr(IN const std::vector<CString> & vecObjID, OUT CString & strXML);
bool BASEGRAPH_DATABASE_API DataBase_LoadObjFromXMLStr(IN const CString & strXML, OUT std::vector<CString> & vecObjID);
bool BASEGRAPH_DATABASE_API DataBase_SaveObjToXMLFile(IN const std::vector<CString> & vecObjID, IN CString strFilePathName);
bool BASEGRAPH_DATABASE_API DataBase_LoadObjFromXMLFile(IN CString strFilePathName, OUT std::vector<CString> & vecObjID);
//关键帧
//...
//工具/其他
std::map<BaseGraph_DataBase::ParamValueDef, Value> BASEGRAPH_DATABASE_API DataBase_GetDefaultParam(IN BaseGraph_DataBase::ObjectType emType);
//锁
bool BASEGRAPH_DATABASE_API DataBase_LockData();
bool BASEGRAPH_DATABASE_API DataBase_UnlockData();

//一些全局配置值
struct StuDataBaseGlobalConfigValue
{
	int m_nMinActionLenth;
	StuDataBaseGlobalConfigValue() { Reset(); }
	void Reset() { m_nMinActionLenth = 0; }
};
StuDataBaseGlobalConfigValue BASEGRAPH_DATABASE_API DataBase_GetGlobalConfig();
