#pragma once
#include <map>
#include <vector>
#include "DataTypeDef.h"
using BaseGraph_Common::Value;

namespace BaseGraph_DataBase
{
	class BASEGRAPH_DATABASE_API BaseObject
	{
	protected:
		std::map<ParamValueDef, Value> m_mapParam;
		BaseObject * m_pParent;
		std::vector<BaseObject *> m_vecChildObj;
	public:
		BaseObject();
		virtual ~BaseObject() = 0;
		virtual void InitialObjectParam();
	public:
		bool Serilize(CArchive & archive);
		BaseGraph_DataBase::ObjectType GetObjectType();
	};
};
