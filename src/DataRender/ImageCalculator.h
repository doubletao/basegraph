#pragma once
#include "ObjectCalculator.h"
class ImageCalculator :
	public ObjectCalculator
{
public:
	ImageCalculator();
	virtual ~ImageCalculator();
public:
	virtual bool UpdateRenderBuffer();
	virtual int GetWidth();
	virtual int GetHeight();
};

