#include "stdafx.h"
#include "ObjectCalculatorManager.h"


ObjectCalculatorManager::ObjectCalculatorManager()
{
}


ObjectCalculatorManager::~ObjectCalculatorManager()
{
}

ObjectCalculatorManager & ObjectCalculatorManager::Instance()
{
	ObjectCalculatorManager inst;
	return inst;
}
