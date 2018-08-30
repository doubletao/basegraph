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
		CString m_nParentID;
		std::vector<CString> m_vecChildID;
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
	};
};
