#include "stdafx.h"
#include "ObjectTitleword.h"
using namespace BaseGraph_DataBase;

ObjectTitleword::ObjectTitleword()
{
	m_mapParam[ObjType].SetValue(ObjectType_Title, BaseGraph_Common::ValueType_ENUM);
}


ObjectTitleword::~ObjectTitleword()
{
}
