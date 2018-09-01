#include "stdafx.h"
#include "ObjectImage.h"
#include "DataResourceManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
using namespace BaseGraph_DataBase;

ObjectImage::ObjectImage()
{
	SetValue(Obj_Type_Enum, Value(ObjectType_Image, BaseGraph_Common::ValueType_ENUM));
}

ObjectImage::~ObjectImage()
{
}

void ObjectImage::InitialObjectParam()
{
	//先初始化基础的
	BaseObject::InitialObjectParam();

	for (int i = ImageParam + 1; i < ImageParam_End; i++)
	{
		AddDefaultParam((ParamValueDef)i);
	}
}

std::map<BaseGraph_DataBase::ParamValueDef, Value> ObjectImage::GetProperty()
{
	std::map<BaseGraph_DataBase::ParamValueDef, Value> mapParam = m_mapParam;
	mapParam[BaseGraph_DataBase::Image_Path_Str] = GetImagePath();
	return mapParam;
}

Value ObjectImage::GetValue(BaseGraph_DataBase::ParamValueDef emValueDef)
{
	if (emValueDef == BaseGraph_DataBase::Image_Path_Str)
	{
		return GetImagePath();
	}
	return BaseObject::GetValue(emValueDef);
}

void ObjectImage::SetValue(BaseGraph_DataBase::ParamValueDef emValueDef, Value stuValue)
{
	if (emValueDef == BaseGraph_DataBase::Image_Path_Str)
	{
		return SetImagePath(stuValue.ToCString());
	}
	return BaseObject::SetValue(emValueDef, stuValue);
}

CString ObjectImage::GetImagePath()
{
	if (m_mapParam.count(Image_Path_Str) > 0)
	{
		CString strResGUID = m_mapParam[Image_Path_Str].ToCString();
		return DataResourceManager::Instance().GetResource(strResGUID);
	}
	return _T("");
}

void ObjectImage::SetImagePath(CString strPath)
{
	DataResourceManager::Instance().DelResource(GetImagePath());
	m_mapParam[Image_Path_Str] = DataResourceManager::Instance().AddResource(strPath);
}

int ObjectImage::GetSrcWidth()
{
	return GetValue(Image_SrcWidth_Int).ToInt();
}

void ObjectImage::SetSrcWidth(int nValue)
{
	SetValue(Image_SrcWidth_Int, nValue);
}

int ObjectImage::GetSrcHeight()
{
	return GetValue(Image_SrcHeight_Int).ToInt();
}

void ObjectImage::SetSrcHeight(int nValue)
{
	SetValue(Image_SrcHeight_Int, nValue);
}

int ObjectImage::GetDispWidth()
{
	return GetValue(Obj_Width_Int).ToInt();
}

void ObjectImage::SetDispWidth(int nValue)
{
	SetValue(Obj_Width_Int, nValue);
}

int ObjectImage::GetDispHeight()
{
	return GetValue(Obj_Height_Int).ToInt();
}

void ObjectImage::SetDispHeight(int nValue)
{
	SetValue(Obj_Height_Int, nValue);
}
