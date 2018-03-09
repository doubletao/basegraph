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
	case BaseGraph_DataBase::ObjectType_Titleword:
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

std::map<BaseGraph_DataBase::ParamValueDef, Value> ObjectFactory::GetDefaultParam(BaseGraph_DataBase::ObjectType emType)
{
	std::map<BaseGraph_DataBase::ParamValueDef, Value> mapParam;
	BaseObject * pObj = CreateObject(emType);
	if (pObj)
	{
		pObj->InitialObjectParam();
		mapParam = pObj->GetProperty();
		delete pObj;
		pObj = nullptr;
	}
	return mapParam;
}
