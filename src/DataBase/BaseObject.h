#pragma once
#include <map>
#include <vector>
#include "DataBase\DataTypeDefDetail.h"
#include "DataBase\DataTypeDef.h"
using BaseGraph_Common::Value;

namespace BaseGraph_DataBase
{
	class BaseObject
	{
	protected:
		std::map<ParamValueDef, Value> m_mapParam;
		CString m_strParentID;
		std::vector<CString> m_vecChildID;
		EmObjectChgLevel m_emChgLevel;//不进序列化
	public:
		BaseObject();
		virtual ~BaseObject() = 0;
		virtual void InitialObjectParam();
	protected:
		void AddDefaultParam(ParamValueDef emParamValueDef);
	public:
		bool Serialize(CArchive & archive);
		BaseGraph_DataBase::ObjectType GetObjectType();

		virtual std::map<ParamValueDef, Value> GetProperty();
		void SetProperty(std::map<ParamValueDef, Value> mapParam);
		virtual Value GetValue(ParamValueDef emValueDef);
		virtual void SetValue(ParamValueDef emValueDef, Value stuValue);

		CString GetItemID();
		ObjectType GetItemType();

		point3f GetItemPos();
		void SetItemPos(point3f ptValue);
		point3f GetItemRotate();
		void SetItemRotate(point3f ptValue);
		//弧度版的set和get角度
		point3f GetItemRotateRadian();
		void SetItemRotateRadian(point3f ptValue);
		point3f GetItemScale();
		void SetItemScale(point3f ptValue);
		point3f GetItemCenter();
		void SetItemCenter(point3f ptValue);

		BaseGraph_DataBase::ITEM_FORMTYPE Get3DType();

		CString GetParentID();
		std::vector<CString> GetChildID();
		void SetParentID(CString strParentID);
		void AddChildObject(CString strChildID);
		bool IsChild(CString strChildID);

		void SetChgLevel(EmObjectChgLevel emChgLevel);//set是赋值
		void AddChgLevel(EmObjectChgLevel emChgLevel);//add是叠加
		EmObjectChgLevel GetChgLevel();
	};
};
