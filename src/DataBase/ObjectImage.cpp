#include "stdafx.h"
#include "ObjectImage.h"
#include "DataResourceManager.h"
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

CString ObjectImage::GetImagePath()
{
	return GetValue(Image_Path).ToCString();
}

void ObjectImage::SetImagePath(CString strPath)
{
	DataResourceManager::Instance().DelResource(GetValue(Image_Path).ToCString());
	m_mapParam[Image_Path].SetValue(DataResourceManager::Instance().AddResource(strPath));
}

int ObjectImage::GetSrcWidth()
{
	return GetValue(Image_SrcWidth).ToInt();
}

void ObjectImage::SetSrcWidth(int nValue)
{
	m_mapParam[Image_SrcWidth].SetValue(nValue);
}

int ObjectImage::GetSrcHeight()
{
	return GetValue(Image_SrcHeight).ToInt();
}

void ObjectImage::SetSrcHeight(int nValue)
{
	m_mapParam[Image_SrcHeight].SetValue(nValue);
}

int ObjectImage::GetDispWidth()
{
	return GetValue(Image_DispWidth).ToInt();
}

void ObjectImage::SetDispWidth(int nValue)
{
	m_mapParam[Image_DispWidth].SetValue(nValue);
}

int ObjectImage::GetDispHeight()
{
	return GetValue(Image_DispHeight).ToInt();
}

void ObjectImage::SetDispHeight(int nValue)
{
	m_mapParam[Image_DispHeight].SetValue(nValue);
}
