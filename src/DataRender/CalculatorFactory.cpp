#include "stdafx.h"
#include "CalculatorFactory.h"
#include "CalculatorImage.h"
#include "CalculatorTitleWord.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
CalculatorFactory::CalculatorFactory()
{
}

CalculatorFactory::~CalculatorFactory()
{
}

ObjectCalculator * CalculatorFactory::CreateCalculator(ObjectType emObjectType, CString strObjID)
{
	ObjectCalculator * pCalc = nullptr;
	switch (emObjectType)
	{
	case BaseGraph_DataBase::ObjectType_Err:
		break;
	case BaseGraph_DataBase::ObjectType_Image:
		pCalc = new CalculatorImage(strObjID);
		break;
	case BaseGraph_DataBase::ObjectType_Titleword:
		pCalc = new CalculatorTitleword(strObjID);
		break;
	default:
		pCalc = new ObjectCalculator(strObjID);
		break;
	}
	return pCalc;
}
