#pragma once
#include <vector>
#include <map>
using BaseGraph_Common::Value;

namespace BaseGraph_DataBase
{
	enum ObjectType
	{
		ObjectType_Err = -1,
		ObjectType_Title,
		ObjectType_Scroll,
		ObjectType_Image,
	};

	//注意，要确保版本兼容性，此枚举可以增加但不要删改
	enum ParamValueDef
	{
		ObjParam = 0,
		ObjID,
		ObjType,
		ObjName,
		ObjParam_End,

		SpaceParam = 100,
		Space_X,
		SpaceParam_End,

		ColorParam = 300,
		ColorParam_End,

		TitleWordParam = 1000,
		TitleWordParam_End,

		ScrollParam = 2000,
		ScrollParam_End,

		ImageParam = 3000,
		ImageParam_End,
	};

	struct ValueDescriptor
	{
		CString strDispName;//属性名
		int nDispIndex;//属性界面上的显示顺序
		std::vector<std::map<int, CString>> vecValueLimit;//值限定，可为空，对于数值类型，意义为最大值最小值，对于枚举类型，意义为可选项以及显示名称，对于字符串类型，意义为最大长度
		Value stuDefault;//默认值
	};

	inline ValueDescriptor GetValueDescriptor(ParamValueDef emParamValueDef)
	{
		ValueDescriptor valueDesc;
		switch (emParamValueDef)
		{
		case BaseGraph_DataBase::ObjID:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("ID");
			valueDesc.stuDefault.SetValue(-1);
			break;
		case BaseGraph_DataBase::ObjType:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("物件类型");
			valueDesc.stuDefault.SetValue(ObjectType_Err, BaseGraph_Common::ValueType_ENUM);
			break;
		case BaseGraph_DataBase::ObjName:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("物件名称");
			valueDesc.stuDefault.SetValue(_T("未知物件"));
			break;
		default:
			break;
		}
		return valueDesc;
	}
};