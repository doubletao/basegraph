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
}
