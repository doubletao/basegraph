#include "stdafx.h"
#include "ObjectScroll.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
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
