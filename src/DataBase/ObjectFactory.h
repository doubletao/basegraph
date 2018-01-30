#pragma once
#include "BaseObject.h"
using BaseGraph_DataBase::BaseObject;
using BaseGraph_DataBase::ObjectType;

class ObjectFactory
{
private:
	ObjectFactory();
	virtual ~ObjectFactory() = 0;
public:
	static BaseObject * CreateObject(ObjectType emObjectType);
	static Value GetValue(BaseObject * pObj, BaseGraph_DataBase::ParamValueDef emValueDef);
	static void SetValue(BaseObject * pObj, BaseGraph_DataBase::ParamValueDef emValueDef, Value stuValue);
};

