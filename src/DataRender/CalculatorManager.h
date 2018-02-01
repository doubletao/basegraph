#pragma once
#include <map>
#include "ObjectCalculator.h"

class CalculatorManager
{
private:
	std::map<CString, ObjectCalculator * > m_mapObjCalculator;//物件ID和物件计算器的键值对
private:
	CalculatorManager();
	~CalculatorManager();
public:
	static CalculatorManager & Instance();

	ObjectCalculator * GetObjCalculator(CString strObjID);
	bool ReleaseCalculator(CString strObjID);
	bool ReleaseCalculator(ObjectCalculator * pCalc);

	bool UpdateRenderBuffer(CString strObjID);
	bool GetBuffer(IN CString strID, OUT byte *& pBuffer);
};

