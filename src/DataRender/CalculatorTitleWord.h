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
	rectf m_rtUnitBound;//根据unit算出来的静态bound
public:
	virtual bool UpdateRenderBuffer();
	virtual rectf CalUnitBound();
	virtual void GetRenderUnitBound(int nIndex, rectf& rtBoundf);
	virtual int GetWidth();
	virtual int GetHeight();
	virtual SYText * GetText();
	virtual SYArray * Get2DTextRenderUnitArray();
	virtual void DeleteRenderUnitView(LPCTSTR szUnitID);
};
