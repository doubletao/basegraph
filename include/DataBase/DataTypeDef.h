#pragma once
#include <vector>
#include <map>
using BaseGraph_Common::Value;

namespace BaseGraph_DataBase
{
	enum ITEM_FORMTYPE
	{
		ITEM_FORMTYPE_2D = 0,
		ITEM_FORMTYPE_2DIN3D,
		ITEM_FORMTYPE_3D,
		ITEM_FORMTYPE_3DIN2D,
		ITEM_FORMTYPE_LIGHT,
		ITEM_FORMTYPE_CAMERA,
		ITEM_FORMTYPE_2DRENDERBY3D,//以三维的方式渲染2D保持纵深效果出来
		ITEM_FORMTYPE_UNKNOW,
	};

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

	enum GRAPH_ART_PROC_TYPE
	{
		ARTNULL,             //无
		CONCAVO,			//凹
		CONVEX,				//凸
		BRIGHT_EDGE,		//亮边
		BEVAL_3D 			//立体边
	};

	struct metallight : public BaseGraph_Common::ValueAbleInterface
	{
		BOOL  enable;
		point3f pos;
		float strength[3];
		BASEGRAPH_DATABASE_API metallight();
		BASEGRAPH_DATABASE_API void Reset();
		BASEGRAPH_DATABASE_API virtual int GetSerializeSize() const;
		BASEGRAPH_DATABASE_API virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		BASEGRAPH_DATABASE_API virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct metalinfo : public BaseGraph_Common::ValueAbleInterface
	{
		DWORD		 highlight;
		BOOL         barc;
		float		 specular;
		DWORD		 width;//0-100
		DWORD		 height;//0-100
		metallight	 light[3];
		vector3f	 eyepos;
		BASEGRAPH_DATABASE_API metalinfo();
		BASEGRAPH_DATABASE_API void Reset();
		BASEGRAPH_DATABASE_API virtual int GetSerializeSize() const;
		BASEGRAPH_DATABASE_API virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		BASEGRAPH_DATABASE_API virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct artinfo : public BaseGraph_Common::ValueAbleInterface
	{
		GRAPH_ART_PROC_TYPE		type;		//效果类型
		DWORD					cv_height;   //凹凸高度       
		float 					cu_angle;	//凹凸角度
		int 					soft;		//虚边宽
		DWORD					color;		//凹凸单色,不透明
		metalinfo				metal;		//立体边
		BASEGRAPH_DATABASE_API artinfo();
		BASEGRAPH_DATABASE_API void Reset();
		BASEGRAPH_DATABASE_API virtual int GetSerializeSize() const;
		BASEGRAPH_DATABASE_API virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		BASEGRAPH_DATABASE_API virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct faceinfo : public BaseGraph_Common::ValueAbleInterface
	{
		int		disable;
		int		soft;
		int		effect_type;
		int		effect_width;
		DWORD	avcolor;
		artinfo art;
		sycolor	color;
		BASEGRAPH_DATABASE_API faceinfo();
		BASEGRAPH_DATABASE_API void Reset();
		BASEGRAPH_DATABASE_API virtual int GetSerializeSize() const;
		BASEGRAPH_DATABASE_API virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		BASEGRAPH_DATABASE_API virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct borderinfo : public BaseGraph_Common::ValueAbleInterface
	{
		int		disable;
		int		soft;
		int		linestyle;
		float	width;
		float   interval;//点线的间距
		DWORD	effect_type;
		int		effect_width;
		DWORD	avcolor;
		artinfo	art;
		sycolor	color;
		BASEGRAPH_DATABASE_API borderinfo();
		BASEGRAPH_DATABASE_API void Reset();
		BASEGRAPH_DATABASE_API virtual int GetSerializeSize() const;
		BASEGRAPH_DATABASE_API virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		BASEGRAPH_DATABASE_API virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct shadowinfo : public BaseGraph_Common::ValueAbleInterface
	{
		///注意溢出
		int		disable;
		int 	soft;
		int		offsetx;
		int		offsety;
		int	    effect_type;

		float	effect_fxtitle;
		float	effect_fyscale;
		float	effect_fadeout;
		DWORD	effect_style;
		DWORD	avcolor;
		artinfo  art;
		sycolor	color;
		BASEGRAPH_DATABASE_API shadowinfo();
		BASEGRAPH_DATABASE_API void Reset();
		BASEGRAPH_DATABASE_API virtual int GetSerializeSize() const;
		BASEGRAPH_DATABASE_API virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		BASEGRAPH_DATABASE_API virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct cubeinfo : public BaseGraph_Common::ValueAbleInterface
	{
		int		disable;
		float	width;
		float	angle;
		int		effect_type;
		int		  effect_width;
		unsigned long	avcolor;
		artinfo	art;
		sycolor	color;
		BASEGRAPH_DATABASE_API cubeinfo();
		BASEGRAPH_DATABASE_API void Reset();
		BASEGRAPH_DATABASE_API virtual int GetSerializeSize() const;
		BASEGRAPH_DATABASE_API virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		BASEGRAPH_DATABASE_API virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct lightinfo : public BaseGraph_Common::ValueAbleInterface
	{
		DWORD ambience;	//环镜单色,不透明
		DWORD ambience_degree;  //明亮程度1-100
		DWORD reflectcolor;		//反射单色,不透明
		DWORD dwReverse1;
		DWORD dwReverse2;
		TCHAR szPath[512];
		BASEGRAPH_DATABASE_API lightinfo();
		BASEGRAPH_DATABASE_API void Reset();
		BASEGRAPH_DATABASE_API virtual int GetSerializeSize() const;
		BASEGRAPH_DATABASE_API virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		BASEGRAPH_DATABASE_API virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct channelinfo : public BaseGraph_Common::ValueAbleInterface
	{
		TCHAR szPath[512];
		int   weight;
		int	  channelid;
		DWORD dwReverse1;
		DWORD dwReverse2;
		BASEGRAPH_DATABASE_API channelinfo();
		BASEGRAPH_DATABASE_API void Reset();
		BASEGRAPH_DATABASE_API virtual int GetSerializeSize() const;
		BASEGRAPH_DATABASE_API virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		BASEGRAPH_DATABASE_API virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct materialinfo : public BaseGraph_Common::ValueAbleInterface
	{
		TCHAR szPath[512];
		DWORD dwReverse1;
		DWORD dwReverse2;
		BASEGRAPH_DATABASE_API materialinfo();
		BASEGRAPH_DATABASE_API void Reset();
		BASEGRAPH_DATABASE_API virtual int GetSerializeSize() const;
		BASEGRAPH_DATABASE_API virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		BASEGRAPH_DATABASE_API virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	struct dynamictexture : public BaseGraph_Common::ValueAbleInterface
	{
		TCHAR szPath[512];
		BOOL  disable;
		float speed;
		float offsetx;
		float offsety;
		float vRunDirx;		// 以此表示方向角度，Y暂不用
		float vRunDiry;
		float scalex;
		float scaley;
		float centerx;
		float centery;
		float rotate;
		float alpha;
		BOOL  bRepeat;
		BOOL  bUseAlpha;
		BASEGRAPH_DATABASE_API dynamictexture();
		BASEGRAPH_DATABASE_API void Reset();
		BASEGRAPH_DATABASE_API virtual int GetSerializeSize() const;
		BASEGRAPH_DATABASE_API virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
		BASEGRAPH_DATABASE_API virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	};

	BASEGRAPH_DATABASE_API ValueDescriptor GetValueDescriptor(ParamValueDef emParamValueDef);

	BASEGRAPH_DATABASE_API bool GetItemPropertyIDRange(IN ObjectType emType, OUT ParamValueDef & emParamBegin, OUT ParamValueDef & emParamEmd);
};
