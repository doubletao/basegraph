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
	auto AddDefaultParam = [this](ParamValueDef emParamValueDef)
	{
		if (m_mapParam.count(emParamValueDef) < 1)
		{
			m_mapParam[emParamValueDef] = GetValueDescriptor(emParamValueDef).stuDefault;
		}
	};

	AddDefaultParam(Titleword_Text_Str);
	AddDefaultParam(Titleword_FontName_Str);
	AddDefaultParam(Titleword_LineWrap_Bool);
	AddDefaultParam(Titleword_FontDistance_Int);
	AddDefaultParam(Titleword_Horizontal_Bool);
	AddDefaultParam(Titleword_FontWidth_Int);
	AddDefaultParam(Titleword_FontHeight_Int);
	AddDefaultParam(Titleword_FontBold_Bool);
	AddDefaultParam(Titleword_FontItalic_Bool);
}
