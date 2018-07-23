#pragma once
#include "ObjectCalculator.h"

class Calculator2DLayer
{
public:
	static const CString strCalculator2DLayer;
	Calculator2DLayer();
	~Calculator2DLayer();
	virtual BaseGraph_DataBase::ITEM_FORMTYPE Get3DType()const;
};
