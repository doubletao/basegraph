#include "stdafx.h"
#include "ObjectFactory.h"
#include "ObjectTitleword.h"
#include "ObjectScroll.h"
#include "ObjectImage.h"

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
