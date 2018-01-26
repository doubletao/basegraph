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
};

