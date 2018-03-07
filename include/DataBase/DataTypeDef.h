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

	//注意，要确保版本向前兼容，此枚举定义可以增加但不要删改
	enum ParamValueDef
	{
		ObjParam = 0,
		ObjID_Str,
		ObjType_Enum,
		ObjName_Str,
		ObjActionBegin_Int,//使用场作为单位
		ObjActionEnd_Int,//使用场作为单位
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

		TitleWordParam = 1000,
		TitleWordParam_End,

		ScrollParam = 2000,
		ScrollParam_End,

		ImageParam = 3000,
		Image_Path_Str,
		Image_SrcWidth_Int,
		Image_SrcHeight_Int,
		Image_DispWidth_Int,
		Image_DispHeight_Int,
		ImageParam_End,
	};

	struct ValueDescriptor
	{
		CString strDispName;//属性名
		int nDispIndex;//属性界面上的显示顺序
		Value stuDefault;//默认值
		std::vector<std::pair<int, CString>> vecValueLimit;//值限定，可为空，对于数值类型，意义为最大值最小值，对于枚举类型，意义为可选项以及显示名称，对于字符串类型，意义为最大长度
	};

	struct artinfo : public BaseGraph_Common::ValueAbleInterface
	{
		virtual int GetSerializeSize() const;
		virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct faceinfo : public BaseGraph_Common::ValueAbleInterface
	{
		virtual int GetSerializeSize() const;
		virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct borderinfo : public BaseGraph_Common::ValueAbleInterface
	{
		virtual int GetSerializeSize() const;
		virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct shadowinfo : public BaseGraph_Common::ValueAbleInterface
	{
		virtual int GetSerializeSize() const;
		virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct cubeinfo : public BaseGraph_Common::ValueAbleInterface
	{
		virtual int GetSerializeSize() const;
		virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct lightinfo : public BaseGraph_Common::ValueAbleInterface
	{
		virtual int GetSerializeSize() const;
		virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct channelinfo : public BaseGraph_Common::ValueAbleInterface
	{
		virtual int GetSerializeSize() const;
		virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct materialinfo : public BaseGraph_Common::ValueAbleInterface
	{
		virtual int GetSerializeSize() const;
		virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct dynamictexture : public BaseGraph_Common::ValueAbleInterface
	{
		virtual int GetSerializeSize() const;
		virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	inline ValueDescriptor GetValueDescriptor(ParamValueDef emParamValueDef)
	{
		ValueDescriptor valueDesc;
		switch (emParamValueDef)
		{
		case BaseGraph_DataBase::ObjID_Str:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("ID");
			valueDesc.stuDefault.SetValue(_T("unknown"));
			break;
		case BaseGraph_DataBase::ObjType_Enum:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("物件类型");
			valueDesc.stuDefault.SetValue(ObjectType_Err, BaseGraph_Common::ValueType_ENUM);
			break;
		case BaseGraph_DataBase::ObjName_Str:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("物件名称");
			valueDesc.stuDefault.SetValue(_T("未知物件"));
			break;
		case ObjActionBegin_Int:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("起始场");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(INT32_MAX, _T("最大值")));
			break;
		case ObjActionEnd_Int:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("结束场");
			valueDesc.stuDefault.SetValue(10 * 25 * 2, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(INT32_MAX, _T("最大值")));
			break;
		case SpacePosition_X_Float:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("X");
			valueDesc.stuDefault.SetValue(10.0f);
			valueDesc.vecValueLimit.push_back(std::make_pair(-3000, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(3000, _T("最大值")));
			break;
		case SpacePosition_Y_Float:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("Y");
			valueDesc.stuDefault.SetValue(20.0f);
			valueDesc.vecValueLimit.push_back(std::make_pair(-3000, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(3000, _T("最大值")));
			break;
		case SpacePosition_Z_Float:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("Z");
			valueDesc.stuDefault.SetValue(30.0f);
			valueDesc.vecValueLimit.push_back(std::make_pair(-3000, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(3000, _T("最大值")));
			break;
		case SpaceRotate_X_Float:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("X");
			valueDesc.stuDefault.SetValue(0.0f);
			valueDesc.vecValueLimit.push_back(std::make_pair(-3600, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(3600, _T("最大值")));
			break;
		case SpaceRotate_Y_Float:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("Y");
			valueDesc.stuDefault.SetValue(0.0f);
			valueDesc.vecValueLimit.push_back(std::make_pair(-3600, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(3600, _T("最大值")));
			break;
		case SpaceRotate_Z_Float:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("Z");
			valueDesc.stuDefault.SetValue(0.0f);
			valueDesc.vecValueLimit.push_back(std::make_pair(-3600, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(3600, _T("最大值")));
			break;
		case SpaceScale_X_Float:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("X");
			valueDesc.stuDefault.SetValue(1.0f);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(1, _T("最大值")));
			break;
		case SpaceScale_Y_Float:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("Y");
			valueDesc.stuDefault.SetValue(1.0f);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(1, _T("最大值")));
			break;
		case SpaceScale_Z_Float:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("Z");
			valueDesc.stuDefault.SetValue(1.0f);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(1, _T("最大值")));
			break;
		case SpaceCenter_X_Float:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("X");
			valueDesc.stuDefault.SetValue(0.0f);
			valueDesc.vecValueLimit.push_back(std::make_pair(-3000, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(3000, _T("最大值")));
			break;
		case SpaceCenter_Y_Float:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("Y");
			valueDesc.stuDefault.SetValue(0.0f);
			valueDesc.vecValueLimit.push_back(std::make_pair(-3000, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(3000, _T("最大值")));
			break;
		case SpaceCenter_Z_Float:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("Z");
			valueDesc.stuDefault.SetValue(0.0f);
			valueDesc.vecValueLimit.push_back(std::make_pair(-3000, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(3000, _T("最大值")));
			break;
		case ColorParam_Face_Stu:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("面");
			valueDesc.stuDefault.SetValue(faceinfo());
			break;
		case ColorParam_Border_Stu:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("周边");
			valueDesc.stuDefault.SetValue(borderinfo());
			break;
		case ColorParam_Shadow_Stu:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("阴影");
			valueDesc.stuDefault.SetValue(shadowinfo());
			break;
		case ColorParam_Cube_Stu:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("立体边");
			valueDesc.stuDefault.SetValue(cubeinfo());
			break;
		case ColorParam_Bordercube_Stu:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("周边立体边");
			valueDesc.stuDefault.SetValue(cubeinfo());
			break;
		case ColorParam_Bordershadow_Stu:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("周边阴影");
			valueDesc.stuDefault.SetValue(shadowinfo());
			break;
		case ColorParam_Facelight_Stu:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("面光照效果");
			valueDesc.stuDefault.SetValue(lightinfo());
			break;
		case ColorParam_Cubelight_Stu:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("立体边光照");
			valueDesc.stuDefault.SetValue(lightinfo());
			break;
		case ColorParam_Channel_Stu:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("通道");
			valueDesc.stuDefault.SetValue(channelinfo());
			break;
		case ColorParam_Material_Stu:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("材质");
			valueDesc.stuDefault.SetValue(materialinfo());
			break;
		case ColorParam_Appendmaterial_Stu:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("材质扩展");
			valueDesc.stuDefault.SetValue(materialinfo());
			break;
		case ColorParam_Dyntex_Stu:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("动态纹理");
			valueDesc.stuDefault.SetValue(dynamictexture());
			break;
		case ColorParam_Renderline_Int:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("不确定");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			break;
		case ColorParam_Width_Float:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("不确定");
			valueDesc.stuDefault.SetValue(0.0f);
			break;
		case ColorParam_Index_Int:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("不确定");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			break;
		case Image_Path_Str:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("图片路径");
			valueDesc.stuDefault.SetValue(_T(""));
			break;
		case Image_SrcWidth_Int:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("图源宽");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(8192, _T("最大值")));
			break;
		case Image_SrcHeight_Int:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("图源高");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(8192, _T("最大值")));
			break;
		case Image_DispWidth_Int:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("展示宽");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(8192, _T("最大值")));
			break;
		case Image_DispHeight_Int:
			valueDesc.nDispIndex = 0;
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
};