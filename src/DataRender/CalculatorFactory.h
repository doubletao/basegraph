#pragma once
#include "ObjectCalculator.h"
using BaseGraph_DataBase::ObjectType;

class CalculatorFactory
{
private:
	CalculatorFactory();
	virtual ~CalculatorFactory() = 0;
public:
	static ObjectCalculator * CreateCalculator(ObjectType emObjectType, CString strObjID);
};

