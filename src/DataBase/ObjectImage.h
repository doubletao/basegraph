#pragma once
#include "BaseObject.h"
using BaseGraph_DataBase::BaseObject;

class ObjectImage :
	public BaseObject
{
public:
	ObjectImage();
	virtual ~ObjectImage();
public:
	virtual void InitialObjectParam();
};

