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
		Titleword_UnderLine_Bool,
		Titleword_LineWrap_Bool,
		Titleword_FontWidth_Int,
		Titleword_FontHeight_Int,
		Titleword_FontDistance_Int,
		Titleword_Horizontal_Bool,
		TitlewordParam_End,

		ScrollParam = 2000,
		ScrollParam_End,

		ImageParam = 3000,
		Image_Path_Str,
		Image_SrcWidth_Int,
		Image_SrcHeight_Int,
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
		int nPrecision;
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

	BASEGRAPH_DATABASE_API ValueDescriptor GetValueDescriptor(ParamValueDef emParamValueDef);

	BASEGRAPH_DATABASE_API bool GetItemPropertyIDRange(IN ObjectType emType, OUT ParamValueDef & emParamBegin, OUT ParamValueDef & emParamEmd);
};
