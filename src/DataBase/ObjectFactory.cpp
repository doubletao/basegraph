#include "stdafx.h"
#include "ObjectFactory.h"
#include "ObjectTitleword.h"
#include "ObjectScroll.h"
#include "ObjectImage.h"
#include <assert.h>

ObjectFactory::ObjectFactory()
{
}


ObjectFactory::~ObjectFactory()
{
}

BaseObject * ObjectFactory::CreateObject(ObjectType emObjectType)
{
	BaseObject * pObj = nullptr;
	switch (emObjectType)
	{
	case BaseGraph_DataBase::ObjectType_Title:
		pObj = new ObjectTitleword;
		break;
	case BaseGraph_DataBase::ObjectType_Scroll:
		pObj = new ObjectScroll;
		break;
	case BaseGraph_DataBase::ObjectType_Image:
		pObj = new ObjectImage;
		break;
	default:
		break;
	}
	return pObj;
}

Value ObjectFactory::GetValue(BaseObject * pObj, BaseGraph_DataBase::ParamValueDef emValueDef)
{
	assert(pObj);
	Value stuValue;
	if (pObj)
	{
		ObjectType emObjType = pObj->GetItemType();
		switch (emObjType)
		{
		case BaseGraph_DataBase::ObjectType_Title:
			break;
		case BaseGraph_DataBase::ObjectType_Scroll:
			break;
		case BaseGraph_DataBase::ObjectType_Image:
		{
			ObjectImage * pImage = (ObjectImage *)pObj;
			switch (emValueDef)
			{
			case BaseGraph_DataBase::Image_Path:
				stuValue = pImage->GetImagePath();
				break;
			default:
				break;
			}
		}
			break;
		default:
			break;
		}
	}
	return stuValue;
}

void ObjectFactory::SetValue(BaseObject * pObj, BaseGraph_DataBase::ParamValueDef emValueDef, Value stuValue)
{
	assert(pObj);
	if (pObj)
	{
		ObjectType emObjType = pObj->GetItemType();
		switch (emObjType)
		{
		case BaseGraph_DataBase::ObjectType_Title:
			break;
		case BaseGraph_DataBase::ObjectType_Scroll:
			break;
		case BaseGraph_DataBase::ObjectType_Image:
		{
			ObjectImage * pImage = (ObjectImage *)pObj;
			switch (emValueDef)
			{
			case BaseGraph_DataBase::Image_Path:
				pImage->SetImagePath(stuValue.ToCString());
				break;
			default:
				break;
			}
		}
		break;
		default:
			break;
		}
	}
}
