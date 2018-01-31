#pragma once
#include "ObjectCalculator.h"
class ImageCalculator :
	public ObjectCalculator
{
private:
	int m_nImageBufferID;
public:
	ImageCalculator();
	virtual ~ImageCalculator();
protected:
	bool UpdateImage(CString &strImgPath);
	bool StretchImage();
	bool PrepareObjBuffer();
public:
	virtual bool UpdateRenderBuffer();
	virtual int GetWidth();
	virtual int GetHeight();
};

