#pragma once
#include <map>
#include <vector>
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
	public:
		bool Serialize(CArchive & archive);
		BaseGraph_DataBase::ObjectType GetObjectType();

		virtual std::map<ParamValueDef, Value> GetProperty();
		void SetProperty(std::map<ParamValueDef, Value> mapParam);
		virtual Value GetValue(ParamValueDef emValueDef);
		virtual void SetValue(ParamValueDef emValueDef, Value stuValue);

		CString GetItemID();
		ObjectType GetItemType();

		point3 GetItemPos();
		void SetItemPos(point3 ptValue);
		point3 GetItemRotate();
		void SetItemRotate(point3 ptValue);
		point3 GetItemScale();
		void SetItemScale(point3 ptValue);
		point3 GetItemCenter();
		void SetItemCenter(point3 ptValue);
	};
};
