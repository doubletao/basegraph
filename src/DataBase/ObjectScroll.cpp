#include "stdafx.h"
#include "ObjectScroll.h"
using namespace BaseGraph_DataBase;

ObjectScroll::ObjectScroll()
{
	m_mapParam[Obj_Type_Enum].SetValue(ObjectType_Scroll, BaseGraph_Common::ValueType_ENUM);
}


ObjectScroll::~ObjectScroll()
{
}

void ObjectScroll::InitialObjectParam()
{
}
