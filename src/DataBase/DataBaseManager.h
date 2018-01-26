#pragma once
#include "DataTypeDef.h"
#include "DataDef.h"
#include "BaseObject.h"
using BaseGraph_DataBase::BaseObject;
using BaseGraph_DataBase::ObjectType;
using BaseGraph_DataBase::ParamValueDef;
using BaseGraph_Common::Value;
using std::vector;
using std::map;
#define IN
#define OUT

class DataBaseManager
{
private:
	DataBaseManager();
	~DataBaseManager();
private:
	vector<BaseObject> m_vecObject;
public:
	static DataBaseManager & Instance();
	//取得列表
	bool GetAllItem(OUT vector<BaseObject> & vecObject);
	bool GetAllItemExpandSub(OUT vector<BaseObject>& vecObject);
	bool GetItem(IN int nID, OUT BaseObject & stuObject);
	//增删
	bool CreateItem(IN ObjectType emType, IN const map<BaseGraph_DataBase::ParamValueDef, Value> & mapParam, OUT int & nID);
	bool AddSubItemTo(IN int nParentID, IN int nSubID);
	bool DeleteItem(IN int nID);
	//层次调整
	bool MoveItemToTop(IN int nID);
	bool MoveItemToBottom(IN int nID);
	bool MoveItemBefore(IN int nSrcID, IN int nDesID);
	bool MoveItemAfter(IN int nSrcID, IN int nDesID);
	//属性获取
	bool GetItemProperty(IN int nID, IN ParamValueDef emParam, OUT Value & stuParam);
	bool GetItemProperty(IN int nID, OUT map<ParamValueDef, Value> & mapParam);
	//属性设置
	bool SetItemProperty(IN int nID, IN ParamValueDef emParam, IN const Value & stuParam);
	bool SetItemProperty(IN int nID, IN const map<ParamValueDef, Value> & mapParam);
	//序列化
	bool SaveToFile(CString strFile);
	bool LoadFromFile(CString strFile);
};

