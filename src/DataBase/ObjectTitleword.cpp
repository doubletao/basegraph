#include "stdafx.h"
#include "ObjectTitleword.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
using namespace BaseGraph_DataBase;

ObjectTitleword::ObjectTitleword()
{
	m_mapParam[Obj_Type_Enum].SetValue(ObjectType_Titleword, BaseGraph_Common::ValueType_ENUM);
}


ObjectTitleword::~ObjectTitleword()
{
}

void ObjectTitleword::InitialObjectParam()
{
	//先初始化基础的
	BaseObject::InitialObjectParam();

	//颜色值
	for (int i = ColorParam + 1; i < ColorParam_End; i++)
	{
		AddDefaultParam((ParamValueDef)i);
	}

	for (int i = TitlewordParam + 1; i < TitlewordParam_End; i++)
	{
		AddDefaultParam((ParamValueDef)i);
	}
}
