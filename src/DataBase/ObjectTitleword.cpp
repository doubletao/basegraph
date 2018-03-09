#include "stdafx.h"
#include "ObjectTitleword.h"
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
	auto AddDefaultParam = [this](ParamValueDef emParamValueDef)
	{
		if (m_mapParam.count(emParamValueDef) < 1)
		{
			m_mapParam[emParamValueDef] = GetValueDescriptor(emParamValueDef).stuDefault;
		}
	};

	AddDefaultParam(Titleword_Text_Str);
	AddDefaultParam(Titleword_FontName_Str);
	AddDefaultParam(Titleword_UnderLine_Bool);
	AddDefaultParam(Titleword_FontDistance_Int);
	AddDefaultParam(Titleword_Horizontal_Bool);
}
