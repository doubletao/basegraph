#pragma once
#include <vector>
#include <map>
using BaseGraph_Common::Value;

namespace BaseGraph_DataBase
{
	enum ObjectType
	{
		ObjectType_Err = -1,
		ObjectType_2DLayer,
		ObjectType_Titleword,
		ObjectType_Scroll,
		ObjectType_Image,
	};

	//注意，要确保版本向前兼容，此枚举定义可以增加但不要删改
	enum ParamValueDef
	{
		ObjParam = 0,
		Obj_ID_Str,
		Obj_Type_Enum,
		Obj_Name_Str,
		Obj_Width_Int,
		Obj_Height_Int,
		Obj_ActionBegin_Int,//使用场作为单位
		Obj_ActionEnd_Int,//使用场作为单位
		Obj_3DType,
		ObjParam_End,

		SpaceParam = 100,
		SpacePosition_X_Float,
		SpacePosition_Y_Float,
		SpacePosition_Z_Float,
		SpaceRotate_X_Float,
		SpaceRotate_Y_Float,
		SpaceRotate_Z_Float,
		SpaceScale_X_Float,
		SpaceScale_Y_Float,
		SpaceScale_Z_Float,
		SpaceCenter_X_Float,
		SpaceCenter_Y_Float,
		SpaceCenter_Z_Float,
		SpaceParam_End,

		ColorParam = 300,
		ColorParam_Face_Stu,
		ColorParam_Border_Stu,
		ColorParam_Shadow_Stu,
		ColorParam_Cube_Stu,
		ColorParam_Bordercube_Stu,
		ColorParam_Bordershadow_Stu,
		ColorParam_Facelight_Stu,
		ColorParam_Cubelight_Stu,
		ColorParam_Channel_Stu,
		ColorParam_Material_Stu,
		ColorParam_Appendmaterial_Stu,
		ColorParam_Dyntex_Stu,
		ColorParam_Renderline_Int,
		ColorParam_Width_Float,
		ColorParam_Index_Int,
		ColorParam_End,

		TitlewordParam = 1000,
		Titleword_Text_Str,
		Titleword_FontName_Str,
		Titleword_LineWrap_Bool,
		Titleword_FontWidth_Int,
		Titleword_FontHeight_Int,
		Titleword_FontDistance_Int,
		Titleword_Horizontal_Bool,
		Titleword_FontBold_Bool,
		Titleword_FontItalic_Bool,
		TitlewordParam_End,

		ScrollParam = 2000,
		ScrollParam_End,

		ImageParam = 3000,
		Image_Path_Str,
		Image_SrcWidth_Int,
		Image_SrcHeight_Int,
		Image_UpDownChange_Bool,//上下反转
		Image_LeftRightChange_Bool,//左右反转
		Image_FlagChange_Bool,//旗飘变换
		ImageParam_End,
	};

	//所有界面上的控件形式都在这里通过enum来定义，确保属性窗可以通过enum来生成
	enum PropertyDispType
	{
		PropertyDisp_StrSingleEdit,//单行文本框
		PropertyDisp_StrMutiEdit,//多行文本框
		PropertyDisp_StrPathEdit,//路径文本框
		PropertyDisp_NumEdit,//一个数字框
		PropertyDisp_SpinNumEdit,//一个带spin的数字框
		PropertyDisp_SliderNumEdit,//一个带滑杆的数字框
		PropertyDisp_CheckBmp,//bool类型check图按钮
		PropertyDisp_RatioBmp,//多选一图按钮
		PropertyDisp_ColorCheckButton,//可关闭的颜色选择控件
		PropertyDisp_FontNameCombo,//字体选择控件
	};

	struct ValueDescriptor
	{
		CString strDispName;//属性名
		int nDispIndex;//属性界面上的显示顺序
		PropertyDispType emDispType;//属性界面的控件形式
		float fStep;//步长（仅对SpinEdit类型有效）
		int nPrecision;//精度（小数点后位数，仅对SpinEdit类型有效）
		Value stuDefault;//默认值;对于editpath类型，为filter
		std::vector<std::pair<int, CString>> vecValueLimit;//值限定，可为空，对于数值类型，意义为最大值最小值，对于枚举类型，意义为可选项以及显示名称，对于字符串类型，意义为最大长度
		ValueDescriptor()
			: strDispName(_T(""))
			, nDispIndex(0)
			, emDispType(PropertyDisp_StrSingleEdit)
			, fStep(1.0f)
			, nPrecision(0)
		{
		}
	};
	extern ValueDescriptor GetValueDescriptor(ParamValueDef emParamValueDef);
	extern bool GetItemPropertyIDRange(IN ObjectType emType, OUT ParamValueDef & emParamBegin, OUT ParamValueDef & emParamEmd);

	BASEGRAPH_DATABASE_API CString GetDateTypeValueStr(BaseGraph_DataBase::ParamValueDef emParamDef, const BaseGraph_Common::Value & value);
	BASEGRAPH_DATABASE_API bool RestoreDataTypeValueFromXML(BaseGraph_DataBase::ParamValueDef emParamDef, CString strValueType, CString strXML, BaseGraph_Common::Value & value);
};

inline BaseGraph_DataBase::ValueDescriptor BaseGraph_DataBase::GetValueDescriptor(ParamValueDef emParamValueDef)
{
	ValueDescriptor valueDesc;
	switch (emParamValueDef)
	{
	case BaseGraph_DataBase::Obj_ID_Str:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("ID");
		valueDesc.stuDefault.SetValue(_T("unknown"));
		break;
	case BaseGraph_DataBase::Obj_Type_Enum:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("物件类型");
		valueDesc.stuDefault.SetValue(ObjectType_Err, BaseGraph_Common::ValueType_ENUM);
		break;
	case BaseGraph_DataBase::Obj_Name_Str:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("物件名称");
		valueDesc.stuDefault.SetValue(_T("未知物件"));
		break;
	case Obj_ActionBegin_Int:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 1.0f;
		valueDesc.strDispName = _T("起始场");
		valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
		valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
		valueDesc.vecValueLimit.push_back(std::make_pair(INT32_MAX, _T("最大值")));
		break;
	case Obj_ActionEnd_Int:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 1.0f;
		valueDesc.strDispName = _T("结束场");
		valueDesc.stuDefault.SetValue(10 * 25 * 2, BaseGraph_Common::ValueType_INT);
		valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
		valueDesc.vecValueLimit.push_back(std::make_pair(INT32_MAX, _T("最大值")));
		break;
	case Obj_3DType:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("3D类型");
		valueDesc.stuDefault.SetValue(ITEM_FORMTYPE_2D, BaseGraph_Common::ValueType_ENUM);
		break;
	case SpacePosition_X_Float:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 1.0f;
		valueDesc.strDispName = _T("位置_X");
		valueDesc.stuDefault.SetValue(0.0f);
		valueDesc.emDispType = PropertyDisp_SpinNumEdit;
		valueDesc.vecValueLimit.push_back(std::make_pair(-3000, _T("最小值")));
		valueDesc.vecValueLimit.push_back(std::make_pair(3000, _T("最大值")));
		break;
	case SpacePosition_Y_Float:
		valueDesc.nDispIndex = 1;
		valueDesc.fStep = 1.0f;
		valueDesc.strDispName = _T("位置_Y");
		valueDesc.stuDefault.SetValue(0.0f);
		valueDesc.emDispType = PropertyDisp_SpinNumEdit;
		valueDesc.vecValueLimit.push_back(std::make_pair(-3000, _T("最小值")));
		valueDesc.vecValueLimit.push_back(std::make_pair(3000, _T("最大值")));
		break;
	case SpacePosition_Z_Float:
		valueDesc.nDispIndex = 2;
		valueDesc.fStep = 1.0f;
		valueDesc.strDispName = _T("位置_Z");
		valueDesc.stuDefault.SetValue(0.0f);
		valueDesc.emDispType = PropertyDisp_SpinNumEdit;
		valueDesc.vecValueLimit.push_back(std::make_pair(-3000, _T("最小值")));
		valueDesc.vecValueLimit.push_back(std::make_pair(3000, _T("最大值")));
		break;
	case SpaceRotate_X_Float:
		valueDesc.nDispIndex = 3;
		valueDesc.fStep = 1.0f;
		valueDesc.strDispName = _T("旋转_X");
		valueDesc.stuDefault.SetValue(0.0f);
		valueDesc.emDispType = PropertyDisp_SliderNumEdit;
		valueDesc.vecValueLimit.push_back(std::make_pair(-3600, _T("最小值")));
		valueDesc.vecValueLimit.push_back(std::make_pair(3600, _T("最大值")));
		break;
	case SpaceRotate_Y_Float:
		valueDesc.nDispIndex = 4;
		valueDesc.fStep = 1.0f;
		valueDesc.strDispName = _T("旋转_Y");
		valueDesc.stuDefault.SetValue(0.0f);
		valueDesc.emDispType = PropertyDisp_SliderNumEdit;
		valueDesc.vecValueLimit.push_back(std::make_pair(-3600, _T("最小值")));
		valueDesc.vecValueLimit.push_back(std::make_pair(3600, _T("最大值")));
		break;
	case SpaceRotate_Z_Float:
		valueDesc.nDispIndex = 5;
		valueDesc.fStep = 1.0f;
		valueDesc.strDispName = _T("旋转_Z");
		valueDesc.emDispType = PropertyDisp_SliderNumEdit;
		valueDesc.stuDefault.SetValue(0.0f);
		valueDesc.vecValueLimit.push_back(std::make_pair(-3600, _T("最小值")));
		valueDesc.vecValueLimit.push_back(std::make_pair(3600, _T("最大值")));
		break;
	case SpaceScale_X_Float:
		valueDesc.nDispIndex = 6;
		valueDesc.fStep = 0.1f;
		valueDesc.nPrecision = 1;
		valueDesc.strDispName = _T("缩放_X");
		valueDesc.stuDefault.SetValue(1.0f);
		valueDesc.emDispType = PropertyDisp_SliderNumEdit;
		valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
		valueDesc.vecValueLimit.push_back(std::make_pair(10, _T("最大值")));
		break;
	case SpaceScale_Y_Float:
		valueDesc.nDispIndex = 7;
		valueDesc.fStep = 0.1f;
		valueDesc.nPrecision = 1;
		valueDesc.strDispName = _T("缩放_Y");
		valueDesc.stuDefault.SetValue(1.0f);
		valueDesc.emDispType = PropertyDisp_SliderNumEdit;
		valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
		valueDesc.vecValueLimit.push_back(std::make_pair(10, _T("最大值")));
		break;
	case SpaceScale_Z_Float:
		valueDesc.nDispIndex = 8;
		valueDesc.fStep = 0.1f;
		valueDesc.nPrecision = 1;
		valueDesc.strDispName = _T("缩放_Z");
		valueDesc.stuDefault.SetValue(1.0f);
		valueDesc.emDispType = PropertyDisp_SliderNumEdit;
		valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
		valueDesc.vecValueLimit.push_back(std::make_pair(10, _T("最大值")));
		break;
	case SpaceCenter_X_Float:
		valueDesc.nDispIndex = 9;
		valueDesc.fStep = 1.0f;
		valueDesc.strDispName = _T("中心_X");
		valueDesc.stuDefault.SetValue(0.0f);
		valueDesc.emDispType = PropertyDisp_SpinNumEdit;
		valueDesc.vecValueLimit.push_back(std::make_pair(-3000, _T("最小值")));
		valueDesc.vecValueLimit.push_back(std::make_pair(3000, _T("最大值")));
		break;
	case SpaceCenter_Y_Float:
		valueDesc.nDispIndex = 10;
		valueDesc.fStep = 1.0f;
		valueDesc.strDispName = _T("中心_Y");
		valueDesc.stuDefault.SetValue(0.0f);
		valueDesc.emDispType = PropertyDisp_SpinNumEdit;
		valueDesc.vecValueLimit.push_back(std::make_pair(-3000, _T("最小值")));
		valueDesc.vecValueLimit.push_back(std::make_pair(3000, _T("最大值")));
		break;
	case SpaceCenter_Z_Float:
		valueDesc.nDispIndex = 11;
		valueDesc.fStep = 1.0f;
		valueDesc.strDispName = _T("中心_Z");
		valueDesc.stuDefault.SetValue(0.0f);
		valueDesc.emDispType = PropertyDisp_SpinNumEdit;
		valueDesc.vecValueLimit.push_back(std::make_pair(-3000, _T("最小值")));
		valueDesc.vecValueLimit.push_back(std::make_pair(3000, _T("最大值")));
		break;
	case ColorParam_Face_Stu:
	{
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("面");
		valueDesc.emDispType = PropertyDisp_ColorCheckButton;
		faceinfo faceInfo;
		faceInfo.disable = 0;
		faceInfo.color.type = BaseGraphCOLOR_HOMO;
		faceInfo.color.color = ABGR(255, 255, 0, 0);
		valueDesc.stuDefault.SetValue(faceInfo);
	}
	break;
	case ColorParam_Border_Stu:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("周边");
		valueDesc.stuDefault.SetValue(borderinfo());
		break;
	case ColorParam_Shadow_Stu:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("阴影");
		valueDesc.stuDefault.SetValue(shadowinfo());
		break;
	case ColorParam_Cube_Stu:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("立体边");
		valueDesc.stuDefault.SetValue(cubeinfo());
		break;
	case ColorParam_Bordercube_Stu:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("周边立体边");
		valueDesc.stuDefault.SetValue(cubeinfo());
		break;
	case ColorParam_Bordershadow_Stu:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("周边阴影");
		valueDesc.stuDefault.SetValue(shadowinfo());
		break;
	case ColorParam_Facelight_Stu:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("面光照效果");
		valueDesc.stuDefault.SetValue(lightinfo());
		break;
	case ColorParam_Cubelight_Stu:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("立体边光照");
		valueDesc.stuDefault.SetValue(lightinfo());
		break;
	case ColorParam_Channel_Stu:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("通道");
		valueDesc.stuDefault.SetValue(channelinfo());
		break;
	case ColorParam_Material_Stu:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("材质");
		valueDesc.stuDefault.SetValue(materialinfo());
		break;
	case ColorParam_Appendmaterial_Stu:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("材质扩展");
		valueDesc.stuDefault.SetValue(materialinfo());
		break;
	case ColorParam_Dyntex_Stu:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("动态纹理");
		valueDesc.stuDefault.SetValue(dynamictexture());
		break;
	case ColorParam_Renderline_Int:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 1.0f;
		valueDesc.strDispName = _T("不确定");
		valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
		break;
	case ColorParam_Width_Float:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 1.0f;
		valueDesc.strDispName = _T("不确定");
		valueDesc.stuDefault.SetValue(0.0f);
		break;
	case ColorParam_Index_Int:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 1.0f;
		valueDesc.strDispName = _T("不确定");
		valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
		break;
	case Titleword_Text_Str:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("文本内容");
		valueDesc.emDispType = PropertyDisp_StrMutiEdit;
		valueDesc.stuDefault.SetValue(_T(""));
		break;
	case Titleword_FontName_Str:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("字体名称");
		valueDesc.emDispType = PropertyDisp_FontNameCombo;
		valueDesc.stuDefault.SetValue(_T("宋体"));
		break;
	case Titleword_LineWrap_Bool:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("自动折行");
		valueDesc.stuDefault.SetValue(FALSE, BaseGraph_Common::ValueType_BOOL);
		break;
	case Titleword_FontWidth_Int:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 1.0f;
		valueDesc.strDispName = _T("字宽");
		valueDesc.stuDefault.SetValue(40);
		valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
		valueDesc.vecValueLimit.push_back(std::make_pair(1024, _T("最大值")));
		break;
	case Titleword_FontHeight_Int:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 1.0f;
		valueDesc.strDispName = _T("字高");
		valueDesc.stuDefault.SetValue(40);
		valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
		valueDesc.vecValueLimit.push_back(std::make_pair(1024, _T("最大值")));
		break;
	case Titleword_FontDistance_Int:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 1.0f;
		valueDesc.strDispName = _T("字间距");
		valueDesc.stuDefault.SetValue(0);
		break;
	case Titleword_FontBold_Bool:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("加粗");
		valueDesc.stuDefault.SetValue(FALSE, BaseGraph_Common::ValueType_BOOL);
		break;
	case Titleword_FontItalic_Bool:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("斜体");
		valueDesc.stuDefault.SetValue(FALSE, BaseGraph_Common::ValueType_BOOL);
		break;
	case Titleword_Horizontal_Bool:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("水平");
		valueDesc.stuDefault.SetValue(TRUE, BaseGraph_Common::ValueType_BOOL);
		break;
	case Image_Path_Str:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("图片路径");
		valueDesc.emDispType = PropertyDisp_StrPathEdit;
		valueDesc.stuDefault.SetValue(_T(""));
		break;
	case Image_SrcWidth_Int:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 1.0f;
		valueDesc.strDispName = _T("图源宽");
		valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
		valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
		valueDesc.vecValueLimit.push_back(std::make_pair(8192, _T("最大值")));
		break;
	case Image_SrcHeight_Int:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 1.0f;
		valueDesc.strDispName = _T("图源高");
		valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
		valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
		valueDesc.vecValueLimit.push_back(std::make_pair(8192, _T("最大值")));
		break;
	case Image_UpDownChange_Bool:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("垂直反转");
		valueDesc.stuDefault.SetValue(FALSE, BaseGraph_Common::ValueType_BOOL);
		break;
	case Image_LeftRightChange_Bool:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("水平反转");
		valueDesc.stuDefault.SetValue(FALSE, BaseGraph_Common::ValueType_BOOL);
		break;
	case Image_FlagChange_Bool:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 0.0f;
		valueDesc.strDispName = _T("旗飘变换");
		valueDesc.stuDefault.SetValue(FALSE, BaseGraph_Common::ValueType_BOOL);
		break;
	case Obj_Width_Int:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 1.0f;
		valueDesc.strDispName = _T("展示宽");
		valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
		valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
		valueDesc.vecValueLimit.push_back(std::make_pair(8192, _T("最大值")));
		break;
	case Obj_Height_Int:
		valueDesc.nDispIndex = 0;
		valueDesc.fStep = 1.0f;
		valueDesc.strDispName = _T("展示高");
		valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
		valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
		valueDesc.vecValueLimit.push_back(std::make_pair(8192, _T("最大值")));
		break;
	default:
		break;
	}
	return valueDesc;
}

inline bool BaseGraph_DataBase::GetItemPropertyIDRange(IN ObjectType emType, OUT ParamValueDef & emParamBegin, OUT ParamValueDef & emParamEmd)
{

	switch (emType)
	{
	case ObjectType_Err:
		return false;
	case ObjectType_2DLayer:
		return false;
	case ObjectType_Titleword:
		emParamBegin = TitlewordParam;
		emParamEmd = TitlewordParam_End;
		return true;
	case ObjectType_Scroll:
		emParamBegin = ScrollParam;
		emParamEmd = ScrollParam_End;
		return true;
	case ObjectType_Image:
		emParamBegin = ImageParam;
		emParamEmd = ImageParam_End;
		return true;
	default:
		return false;
	}
}
