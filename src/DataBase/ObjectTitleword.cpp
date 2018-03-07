#include "stdafx.h"
#include "ObjectTitleword.h"
using namespace BaseGraph_DataBase;

ObjectTitleword::ObjectTitleword()
{
	m_mapParam[ObjType_Enum].SetValue(ObjectType_Title, BaseGraph_Common::ValueType_ENUM);
}


ObjectTitleword::~ObjectTitleword()
{
}

void ObjectTitleword::InitialObjectParam()
{
}
