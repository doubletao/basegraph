#include "stdafx.h"
#include "CalculatorTitleWord.h"

CalculatorTitleWord::CalculatorTitleWord(CString strObjID)
	: ObjectCalculator(strObjID)
{
}

CalculatorTitleWord::~CalculatorTitleWord()
{
}

bool CalculatorTitleWord::UpdateRenderBuffer()
{
	return false;
}

int CalculatorTitleWord::GetWidth()
{
	return 0;
}

int CalculatorTitleWord::GetHeight()
{
	return 0;
}
