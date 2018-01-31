#pragma once
#include <map>
#include "ObjectCalculator.h"

class ObjectCalculatorManager
{
private:
	std::map<int, ObjectCalculator * > m_mapObjCalculator;//物件ID和物件计算器的键值对
private:
	ObjectCalculatorManager();
	~ObjectCalculatorManager();
public:
	static ObjectCalculatorManager & Instance();
};

