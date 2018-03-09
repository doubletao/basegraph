#pragma once
#include "ObjectCalculator.h"
#include "Typeset\TextTypeset.h"

class CalculatorTitleword :
	public ObjectCalculator
{
public:
	CalculatorTitleword(CString strObjID);
	virtual ~CalculatorTitleword();
protected:
	CSYTypeset m_TypeSet;
	SYText m_text;
public:
	virtual bool UpdateRenderBuffer();
};
