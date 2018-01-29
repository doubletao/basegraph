#include "stdafx.h"
#include "ObjectImage.h"
using namespace BaseGraph_DataBase;

ObjectImage::ObjectImage()
{
	m_mapParam[ObjType].SetValue(ObjectType_Image, BaseGraph_Common::ValueType_ENUM);
}

ObjectImage::~ObjectImage()
{
}

void ObjectImage::InitialObjectParam()
{
	//先初始化基础的
	BaseObject::InitialObjectParam();
	auto AddDefaultParam = [this](ParamValueDef emParamValueDef)
	{
		if (m_mapParam.count(emParamValueDef) < 1)
		{
			m_mapParam[emParamValueDef] = GetValueDescriptor(emParamValueDef).stuDefault;
		}
	};

	AddDefaultParam(Image_Path);
	AddDefaultParam(Image_SrcWidth);
	AddDefaultParam(Image_SrcHeight);
	AddDefaultParam(Image_DispWidth);
	AddDefaultParam(Image_DispHeight);
}
