#pragma once
#include "ObjectCalculator.h"

class CalculatorTitleWord :
	public ObjectCalculator
{
public:
	CalculatorTitleWord(CString strObjID);
	virtual ~CalculatorTitleWord();
protected:
public:
	virtual bool UpdateRenderBuffer();
	virtual int GetWidth();
	virtual int GetHeight();
};
