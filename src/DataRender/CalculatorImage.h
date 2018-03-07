#pragma once
#include "ObjectCalculator.h"

class CalculatorImage :
	public ObjectCalculator
{
private:
	int m_nImageBufferID;
public:
	CalculatorImage(CString strObjID);
	virtual ~CalculatorImage();
protected:
	bool UpdateImage(CString &strImgPath);
	bool StretchImage();
	bool PrepareObjBuffer();
public:
	virtual bool UpdateRenderBuffer();
	virtual int GetWidth();
	virtual int GetHeight();
};

