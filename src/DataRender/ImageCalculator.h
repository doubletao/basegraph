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
public:
	virtual bool UpdateRenderBuffer();
	bool UpdateImage(CString &strImgPath);
	bool StretchImage();
	virtual int GetWidth();
	virtual int GetHeight();
};

