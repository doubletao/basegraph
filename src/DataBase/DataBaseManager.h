#pragma once
#include "BaseObject.h"
using BaseGraph_DataBase::BaseObject;
using BaseGraph_DataBase::ObjectType;
using BaseGraph_DataBase::ParamValueDef;
using BaseGraph_Common::Value;
using std::vector;
using std::map;

class DataBaseManager
{
private:
	DataBaseManager();
	~DataBaseManager();
private:
	vector<BaseObject *> m_vecObject;
	//锁
	CRITICAL_SECTION m_lockData;
public:
	static DataBaseManager & Instance();
	//锁
	void Lock();
	void Unlock();
	//重复检测
	bool CheckItemID(CString strID);
	//取得列表
	bool GetAllItemID(OUT std::vector<CString>& vecID);
	bool GetAllItemIDExceptSubItem(OUT std::vector<CString>& vecID);
	bool GetItemType(IN CString strID, OUT BaseGraph_DataBase::ObjectType & emType);
	bool GetAllItem(OUT vector<BaseObject *> & vecObject);
	bool GetItem(IN CString strID, OUT BaseObject * & pObject);
	//增删
	bool CreateItem(IN ObjectType emType, IN const map<BaseGraph_DataBase::ParamValueDef, Value> & mapParam, OUT CString & strID);
	bool AddItem(BaseObject * pObj);
	bool AddSubItemTo(IN CString strParentID, IN CString strSubID);
	bool DeleteItem(IN CString strID);
	//层次调整
	bool MoveItemToTop(IN CString strID);
	bool MoveItemToBottom(IN CString strID);
	bool MoveItemBefore(IN CString strSrcID, IN CString strDesID);
	bool MoveItemAfter(IN CString strSrcID, IN CString strDesID);
	//父子物件
	bool HasSubItem(IN CString strParentID);
	bool GetSubItem(IN CString strParentID, OUT std::vector<CString> & vecSubID);
	bool HasParentItem(IN CString strSubID);
	bool GetParentItem(IN CString strSubID, OUT CString & strParentID);
	//属性获取
	bool GetItemProperty(IN CString strID, IN ParamValueDef emParam, OUT Value & stuParam);
	bool GetItemProperty(IN CString strID, OUT map<ParamValueDef, Value> & mapParam);

	float3 GetItemPos(IN CString strID);
	float3 GetItemCenter(IN CString strID);
	float3 GetItemAngle(IN CString strID);
	float3 GetItemScale(IN CString strID);
	//属性设置
	bool SetItemProperty(IN CString strID, IN ParamValueDef emParam, IN const Value & stuParam);
	bool SetItemProperty(IN CString strID, IN const map<ParamValueDef, Value> & mapParam);
	//统计
	bool GetItemLength(OUT int & nLength);
	bool GetItemLength(IN const std::vector<CString> & vecID, OUT int & nLength);
	//序列化
	bool SaveToFile(CString strFile);
	bool LoadFromFile(CString strFile);
	bool SerializeObjects(CArchive & archive, vector<BaseObject *> & vecObject);//特殊的序列化接口，仅处理物件，不管其他(注意，load出来的物件需要自己释放)
};

