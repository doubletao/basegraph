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
		ObjID,
		ObjType,
		ObjName,
		ObjActionBegin,//使用场作为单位
		ObjActionEnd,//使用场作为单位
		ObjParam_End,

		SpaceParam = 100,
		SpacePosition_X,
		SpacePosition_Y,
		SpacePosition_Z,
		SpaceRotate_X,
		SpaceRotate_Y,
		SpaceRotate_Z,
		SpaceScale_X,
		SpaceScale_Y,
		SpaceScale_Z,
		SpaceCenter_X,
		SpaceCenter_Y,
		SpaceCenter_Z,
		SpaceParam_End,

		ColorParam = 300,
		ColorFace_Enable,
		ColorFace_Soft,
		ColorFace_EffectType,
		ColorFace_EffectWidth,
		ColorFace_AVColor,
		ColorFace_Art,
		ColorFace_Color,
		ColorBorder_Enable = 350,
		ColorBorder_Soft,
		ColorBorder_LineStyle,
		ColorBorder_Width,
		ColorBorder_Interval,
		ColorBorder_EffectType,
		ColorBorder_EffectWidth,
		ColorBorder_AVColor,
		ColorBorder_Art,
		ColorBorder_Color,
		ColorParam_End,

		TitleWordParam = 1000,
		TitleWordParam_End,

		ScrollParam = 2000,
		ScrollParam_End,

		ImageParam = 3000,
		Image_Path,
		Image_SrcWidth,
		Image_SrcHeight,
		Image_DispWidth,
		Image_DispHeight,
		ImageParam_End,
	};

	struct ValueDescriptor
	{
		CString strDispName;//属性名
		int nDispIndex;//属性界面上的显示顺序
		Value stuDefault;//默认值
		std::vector<std::pair<int, CString>> vecValueLimit;//值限定，可为空，对于数值类型，意义为最大值最小值，对于枚举类型，意义为可选项以及显示名称，对于字符串类型，意义为最大长度
	};

	struct ValueArt : public BaseGraph_Common::ValueAbleInterface
	{
		virtual int GetSerializeSize() const;
		virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct ValueColor : public BaseGraph_Common::ValueAbleInterface
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
		case BaseGraph_DataBase::ObjID:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("ID");
			valueDesc.stuDefault.SetValue(_T("unknown"));
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
		case ObjActionBegin:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("起始场");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(INT32_MAX, _T("最大值")));
			break;
		case ObjActionEnd:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("结束场");
			valueDesc.stuDefault.SetValue(10 * 25 * 2, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(INT32_MAX, _T("最大值")));
			break;
		case SpacePosition_X:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("X");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(-3000, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(3000, _T("最大值")));
			break;
		case SpacePosition_Y:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("Y");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(-3000, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(3000, _T("最大值")));
			break;
		case SpacePosition_Z:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("Z");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(-3000, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(3000, _T("最大值")));
			break;
		case SpaceRotate_X:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("X");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(-3600, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(3600, _T("最大值")));
			break;
		case SpaceRotate_Y:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("Y");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(-3600, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(3600, _T("最大值")));
			break;
		case SpaceRotate_Z:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("Z");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(-3600, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(3600, _T("最大值")));
			break;
		case SpaceScale_X:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("X");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(100, _T("最大值")));
			break;
		case SpaceScale_Y:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("Y");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(100, _T("最大值")));
			break;
		case SpaceScale_Z:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("Z");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(100, _T("最大值")));
			break;
		case SpaceCenter_X:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("X");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(-3000, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(3000, _T("最大值")));
			break;
		case SpaceCenter_Y:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("Y");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(-3000, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(3000, _T("最大值")));
			break;
		case SpaceCenter_Z:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("Z");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(-3000, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(3000, _T("最大值")));
			break;
		case ColorFace_Enable:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("面色");
			valueDesc.stuDefault.SetValue(TRUE, BaseGraph_Common::ValueType_BOOL);
			break;
		case ColorFace_Soft:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("不确定？");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(100, _T("最大值")));
			break;
		case ColorFace_EffectType:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("艺术效果类型");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_ENUM);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("无")));
			valueDesc.vecValueLimit.push_back(std::make_pair(1, _T("凹")));
			valueDesc.vecValueLimit.push_back(std::make_pair(2, _T("凸")));
			valueDesc.vecValueLimit.push_back(std::make_pair(3, _T("亮边")));
			valueDesc.vecValueLimit.push_back(std::make_pair(4, _T("立体边")));
			break;
		case ColorFace_EffectWidth:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("不确定？");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(100, _T("最大值")));
			break;
		case ColorFace_AVColor:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("不确定？");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(0xFFFFFFFF, _T("最大值")));
			break;
		case ColorFace_Art:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("艺术效果参数");
			valueDesc.stuDefault.SetValue(ValueArt());
			break;
		case ColorFace_Color:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("颜色参数");
			valueDesc.stuDefault.SetValue(ValueColor());
			break;
		case ColorBorder_Enable:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("周边");
			valueDesc.stuDefault.SetValue(FALSE, BaseGraph_Common::ValueType_BOOL);
			break;
		case ColorBorder_Soft:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("不确定？");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(100, _T("最大值")));
			break;
		case ColorBorder_LineStyle:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("周边线型");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_ENUM);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("实线")));
			valueDesc.vecValueLimit.push_back(std::make_pair(1, _T("划线")));
			valueDesc.vecValueLimit.push_back(std::make_pair(2, _T("点线")));
			valueDesc.vecValueLimit.push_back(std::make_pair(3, _T("左划线")));
			valueDesc.vecValueLimit.push_back(std::make_pair(4, _T("左点线")));
			break;
		case ColorBorder_Width:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("周边宽度");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(100, _T("最大值")));
			break;
		case ColorBorder_Interval:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("周边间隙");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(100, _T("最大值")));
			break;
		case ColorBorder_EffectType:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("艺术效果类型");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_ENUM);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("无")));
			valueDesc.vecValueLimit.push_back(std::make_pair(1, _T("凹")));
			valueDesc.vecValueLimit.push_back(std::make_pair(2, _T("凸")));
			valueDesc.vecValueLimit.push_back(std::make_pair(3, _T("亮边")));
			valueDesc.vecValueLimit.push_back(std::make_pair(4, _T("立体边")));
			break;
		case ColorBorder_EffectWidth:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("不确定？");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(100, _T("最大值")));
			break;
		case ColorBorder_AVColor:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("不确定？");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(0xFFFFFFFF, _T("最大值")));
			break;
		case ColorBorder_Art:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("艺术效果参数");
			valueDesc.stuDefault.SetValue(ValueArt());
			break;
		case ColorBorder_Color:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("颜色参数");
			valueDesc.stuDefault.SetValue(ValueColor());
			break;
		case Image_Path:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("图片路径");
			valueDesc.stuDefault.SetValue(_T(""));
			break;
		case Image_SrcWidth:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("图源宽");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(8192, _T("最大值")));
			break;
		case Image_SrcHeight:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("图源高");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(8192, _T("最大值")));
			break;
		case Image_DispWidth:
			valueDesc.nDispIndex = 0;
			valueDesc.strDispName = _T("展示宽");
			valueDesc.stuDefault.SetValue(0, BaseGraph_Common::ValueType_INT);
			valueDesc.vecValueLimit.push_back(std::make_pair(0, _T("最小值")));
			valueDesc.vecValueLimit.push_back(std::make_pair(8192, _T("最大值")));
			break;
		case Image_DispHeight:
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

	inline int ValueArt::GetSerializeSize() const
	{
		return 0;
	}
	inline bool ValueArt::GetSerializeBuf(byte * pBuf, int nBufSize) const
	{
		return false;
	}
	inline bool ValueArt::RecoverFromBuf(byte * pBuf, int nBufSize)
	{
		return false;
	}

	inline int ValueColor::GetSerializeSize() const
	{
		return 0;
	}
	inline bool ValueColor::GetSerializeBuf(byte * pBuf, int nBufSize) const
	{
		return false;
	}
	inline bool ValueColor::RecoverFromBuf(byte * pBuf, int nBufSize)
	{
		return false;
	}
};