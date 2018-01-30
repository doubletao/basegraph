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
		int m_nParentID;
		std::vector<int> m_vecChildID;
	public:
		BaseObject();
		virtual ~BaseObject() = 0;
		virtual void InitialObjectParam();
	public:
		bool Serialize(CArchive & archive);
		BaseGraph_DataBase::ObjectType GetObjectType();

		Value GetValue(ParamValueDef emValueDef);

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
