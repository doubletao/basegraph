#include "stdafx.h"
#include "ImageCalculator.h"

ImageCalculator::ImageCalculator()
{
}


ImageCalculator::~ImageCalculator()
{
}

bool ImageCalculator::UpdateRenderBuffer()
{
	if (NeedRenderBuffer())
	{
	}
	return true;
}

int ImageCalculator::GetWidth()
{
	int nRet = 0;
	BaseGraph_Common::Value stuValue;
	if (DataBase_GetItemProperty(m_nObjID, BaseGraph_DataBase::Image_DispWidth, stuValue))
	{
		nRet = stuValue.ToInt();
	}
	return nRet;
}

int ImageCalculator::GetHeight()
{
	int nRet = 0;
	BaseGraph_Common::Value stuValue;
	if (DataBase_GetItemProperty(m_nObjID, BaseGraph_DataBase::Image_DispHeight, stuValue))
	{
		nRet = stuValue.ToInt();
	}
	return nRet;
}
