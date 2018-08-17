#pragma once

//字体质量定义
enum PI_Quality
{
	PI_Quality_LOW,
	PI_Quality_MIDDLE,
	PI_Quality_HIGH,
};

BASEGRAPH_DATARENDER_API LPCTSTR	GetDefaultFontFaceName();
BASEGRAPH_DATARENDER_API void		SetDefaultFontFaceName(LPCTSTR szFaceName);
BASEGRAPH_DATARENDER_API LPCTSTR	GetDefaultChnFontFaceName();
BASEGRAPH_DATARENDER_API void		SetDefaultChnFontFaceName(LPCTSTR szFaceName);
BASEGRAPH_DATARENDER_API LPCTSTR	GetPredefineSample();
BASEGRAPH_DATARENDER_API void		SetPredefineSample(LPCTSTR szSample);

//需要调用的外部接口，由调用者根据自己的实际情况来实现
typedef BOOL(*BaseGraphTypesetInterface_GetPropFromPropPage)(LPVOID pFont, LPVOID pParagraph, LPVOID pEffect);

#define BaseGraphVGROT_NODEF		0		//未定义（使用默认设置）
#define BaseGraphVGROT_ROT			1		//旋转
#define BaseGraphVGROT_UNROT		2		//不旋转
#define CHARSCRIPT_BAS	0		//（默认）基线对齐
#define CHARSCRIPT_SUP	1		//上标（Superscript）
#define CHARSCRIPT_SUB	2		//下标（Subscript）
#define CHARSCRIPT_TOP	3		//顶对齐
#define CHARSCRIPT_BOT	4		//底对齐
#define CHARSCRIPT_OFF	5		//自定义脚注偏移
#define RENDERUNITIDLENGTH	64
#define BaseGraphTEXT_SUBFACE_CNT	3		//3D文字子面个数
#define BaseGraphTEXT_DIMENSION_MIN			60		//文字排版矩形最小值
#define BaseGraphTEXT_TEMPFLAG_EMPTYTEXT		0x01		//空标题字
#define BaseGraphTEXT_TEMPFLAG_WHOLERENDER		0x02		//整体渲染
//判断是否是标准格式回车换行符“\r\n”
#define BaseGraphISCRLF(szText, ni, nLen)		((ni) < (nLen) && szText[(ni)] == _T('\r') && ((ni)+1 < (nLen) && szText[(ni)+1] == _T('\n')))
#define BaseGraphISTAB(szText, ni, nLen)		((ni) < (nLen) && szText[(ni)] == _T('\t'))

//字体属性设置掩膜
#define	 BaseGraphTEXTFONTMASK_ALL				0xffffffff
#define BaseGraphTEXTFONTMASK_HEIGHT			0x00000001
#define BaseGraphTEXTFONTMASK_WIDTH			0x00000002
#define BaseGraphTEXTFONTMASK_FONTSIZE			0x00000003		//HEIGHT与WIDTH复合
#define BaseGraphTEXTFONTMASK_ESCAPEMENT		0x00000004
#define BaseGraphTEXTFONTMASK_ORIENTATION		0x00000008
#define BaseGraphTEXTFONTMASK_BOLD				0x00000010
#define BaseGraphTEXTFONTMASK_ITALIC			0x00000020
#define BaseGraphTEXTFONTMASK_UNDERLINE		0x00000040
#define BaseGraphTEXTFONTMASK_STRIKEOUT		0x00000080
#define BaseGraphTEXTFONTMASK_CHARSET			0x00000100
#define BaseGraphTEXTFONTMASK_OUTPRECISION		0x00000200
#define BaseGraphTEXTFONTMASK_CLIPPRECISION	0x00000400
#define BaseGraphTEXTFONTMASK_QUALITY			0x00000800
#define BaseGraphTEXTFONTMASK_PITCHANDFAMILY	0x00001000
#define BaseGraphTEXTFONTMASK_FACENAME			0x00002000
#define BaseGraphTEXTFONTMASK_FACENAMEENG		0x00004000
#define BaseGraphTEXTFONTMASK_DISTANCE			0x00008000
#define BaseGraphTEXTFONTMASK_VGROTATE			0x00010000
#define BaseGraphTEXTFONTMASK_SCRIPT			0x00020000
#define BaseGraphTEXTFONTMASK_VEMBEDING		0x00040000
#define BaseGraphTEXTFONTMASK_HEMBEDING		0x00080000
//下划线（作为字体子属性）
#define BaseGraphTEXTFONTMASK_UNDERLINE_ALL		0x0f000000
#define BaseGraphTEXTFONTMASK_UNDERLINE_TYPE		0x01000000
#define BaseGraphTEXTFONTMASK_UNDERLINE_DISTANCE	0x02000000
#define BaseGraphTEXTFONTMASK_UNDERLINE_HEIGHT		0x04000000
#define BaseGraphTEXTFONTMASK_UNDERLINE_2DCOLOR	0x08000000

//颜色属性设置掩膜
#define BaseGraphTEXTCOLORMASK_ALL				0xffffffff
#define BaseGraphTEXTCOLORMASK_FACE			0x00000001
#define BaseGraphTEXTCOLORMASK_BORDER			0x00000002
#define BaseGraphTEXTCOLORMASK_SHADOW			0x00000004
#define BaseGraphTEXTCOLORMASK_CUBE			0x00000008
#define BaseGraphTEXTCOLORMASK_BORDERCUBE		0x00000010
#define BaseGraphTEXTCOLORMASK_BORDERSHADOW	0x00000020
#define BaseGraphTEXTCOLORMASK_FACELIGHT		0x00000040
#define BaseGraphTEXTCOLORMASK_CUBELIGHT		0x00000080
#define BaseGraphTEXTCOLORMASK_CHANNEL			0x00000100
#define BaseGraphTEXTCOLORMASK_MATERIAL		0x00000200
#define BaseGraphTEXTCOLORMASK_APPENDMATERIAL	0x00000400
#define BaseGraphTEXTCOLORMASK_RENDERLINE		0x00000800
#define BaseGraphTEXTCOLORMASK_WIDTH			0x00001000
#define BaseGraphTEXTCOLORMASK_INDEX			0x00002000

#define BaseGraphTEXTCOLORMASK_FACELIGHTCHANNEL	0x00010000
#define BaseGraphTEXTCOLORMASK_CUBELIGHTCHANNEL	0x00020000
#define BaseGraphTEXTCOLORMASK_ACTIVEATTRIBLE		0x00040000
#define BaseGraphTEXTCOLORMASK_FACEADORNCHANNEL	0x00080000
#define BaseGraphTEXTCOLORMASK_CUBEADORNCHANNEL	0x00100000
#define BaseGraphTEXTCOLORMASK_MASKADORNCHANNEL	0x00200000
#define BaseGraphTEXTCOLORMASK_PATTANADORNCHANNEL	0x00400000
#define BaseGraphTEXTCOLORMASK_APPENDADORNCHANNEL	0x00800000

#define BaseGraphTEXT_UNIT_AUTO				0000
#define BaseGraphTEXT_UNIT_2D					0001
#define BaseGraphTEXT_UNIT_3D					0002
#define BaseGraphTEXT_UNIT_2D3D				0003

//效果
typedef struct tag_staticeffect
{
	BaseGraph_DataBase::faceinfo			face;
	BaseGraph_DataBase::borderinfo	        border;
	BaseGraph_DataBase::shadowinfo	        shadow;
	BaseGraph_DataBase::cubeinfo			cube;
	BaseGraph_DataBase::cubeinfo			bordercube;
	BaseGraph_DataBase::shadowinfo			bordershadow;

	BaseGraph_DataBase::lightinfo			facelight;
	BaseGraph_DataBase::lightinfo			cubelight;

	BaseGraph_DataBase::channelinfo			channel;
	BaseGraph_DataBase::materialinfo		material;
	BaseGraph_DataBase::materialinfo		appendmaterial;
	BaseGraph_DataBase::dynamictexture		dyntex;
	unsigned long		renderline;
	float		        fwidth;
	int			        index;
	tag_staticeffect()
	{
		Reset();
	}
	void Reset()
	{
		face.Reset();
		border.Reset();
		shadow.Reset();
		cube.Reset();
		bordercube.Reset();
		bordershadow.Reset();
		facelight.Reset();
		cubelight.Reset();
		channel.Reset();
		material.Reset();
		appendmaterial.Reset();
		dyntex.Reset();
		renderline = 0;
		fwidth = 0;
		index = 0;
	}
}static_effect;
typedef static_effect BaseGraphStaticEffect, *LPCommonStaticEffect;
typedef const LPCommonStaticEffect LPCBaseStaticEffect;

enum Common_BEVEL_TYPE
{
	Common_BEVEL_NONE,
	Common_BEVEL_PLANE,
	Common_BEVEL_RAKE_ANGLE,
	Common_BEVEL_CONVEX,	//凸面
	Common_BEVEL_CONCAVE,//凹面
};

enum CommonGRAPH_TYPE
{
	NORMAL_GRAPH,    //正常
	EMBOSS_GRAPH,	//浮雕
	SELF_BACK,		//自绘背景
	FIRE_GRAPH,     //火焰
};

enum Common_POST_PROCESS_TYPE
{
	Common_POST_DEFAULT_PROCESS,
	Common_POST_NORMAL_PROCESS,
	Common_POST_POSITION_PROCESS,
	Common_POST_DEPTH_PROCESS,
	Common_POST_TRANSPARENT_PROCESS,
	Common_POST_PROCESS_COUNT,
};

//特殊效果
typedef struct tag_common_specialeffect
{
	int				m_nVersion;     //版本号
	CommonGRAPH_TYPE	m_GraphType;	//图形类型
	struct
	{
		//浮雕字
		struct
		{
			BaseGraphStaticEffect	m_BKPublicAttr; //背景属性 
			rectf			m_BKRect;       //背景位置
			BOOL			m_bUnitary;		//矩形是否归一化
			DWORD			m_nAlignMode;   //若不归一化，矩形坐标原点在字的哪个点。
		};
		//火焰字
		struct
		{
			BaseGraphColor		m_FireColor;      //火焰色
			DWORD		m_nFireType;      //火焰形状
			DWORD     	m_nAngle;		  //火焰偏移方向
			DWORD		m_nDegree;        //火焰幅度
		};
	};
}common_special_effect;
typedef  common_special_effect SYSpecialEffect, *LPCommonSpecialEffect;
typedef  const LPCommonSpecialEffect *LPCBaseSpecialEffect;
//拉伸数据结构
typedef struct sy_stretch_param
{
	BOOL				bEnable;		//拉伸标识
	float				fThick;			//拉伸厚度
	Common_BEVEL_TYPE		eBevelType;		//导角类型
	float				fBevelThick;	//导角厚度
	float				fBevelLen;		//导角长度
	float				fBevelPrecision;//导角精度
	sy_stretch_param()
	{
		bEnable = FALSE;
		fThick = 0;
		eBevelType = Common_BEVEL_NONE;
		fBevelThick = 0;
		fBevelLen = 0;
		fBevelPrecision = 0;
	}
}BaseGraphStretchParam, *LPCommonStretchParam;


//扩充的字体数据结构
struct sy_font_def
{
	//仿照LOGFONT
	float		lfHeight;
	float		lfWidth;
	LONG		lfEscapement;				//旋转
	LONG		lfOrientation;
	LONG		lfWeight;
	BYTE		lfItalic;
	BYTE		lfUnderline;
	BYTE		lfStrikeOut;
	BYTE		lfCharSet;
	BYTE		lfOutPrecision;
	BYTE		lfClipPrecision;
	BYTE		lfQuality;
	BYTE		lfPitchAndFamily;
	TCHAR		lfFaceName[LF_FACESIZE];
	//扩展的
	DWORD		dwVersion;					//版本号
	float		lfDistance;					//字间距
	BYTE		bBold;						//是否加粗（lfWeight会重新调整）

	LPVOID		lpParam;
	TCHAR		lfFaceNameEng[LF_FACESIZE];	//英文字体

	BYTE		lfVEmbeding;				//（横排版中）嵌入竖排版
	BYTE		lfHEmbeding;				//（竖排版中）嵌入横排版
	BYTE		lfVEmbedingAlign;			//（横排版中）嵌入竖排版的对齐方式
	BYTE		lfHEmbedingAlign;			//（竖排版中）嵌入横排版的对齐方式

	BYTE		lfVGRotate;					//竖排旋转字形
	BYTE		lfScript;					//脚注
	float		lfScriptOffset;				//自定义脚注偏移
	sy_font_def() { Reset(); }
	BASEGRAPH_DATARENDER_API void Reset();
};

typedef sy_font_def BaseGraphFont, *LPCommonFont;
typedef const LPCommonFont LPCBaseFont;

//横向对齐方式
typedef enum e_text_align_horizontal
{
	TEXT_ALIGN_H_LEFT = 0,	//左对齐
	TEXT_ALIGN_H_CENTER,	//中对齐
	TEXT_ALIGN_H_RIGHT,		//右对齐
	TEXT_ALIGN_H_ENDS,		//两端对齐
}E_TextAlignHorizontal;
//纵向对齐方式
typedef enum e_text_align_vertical
{
	TEXT_ALIGN_V_TOP = 0,		//顶对齐
	TEXT_ALIGN_V_CENTER,		//中对齐
	TEXT_ALIGN_V_BOTTOM,		//底对齐
	TEXT_ALIGN_V_ENDS,			//两端对齐
	TEXT_ALIGN_V_BOTTOMBASE,	//基线底对齐
}E_TextAlignVeritical;
#define ARTPATHTYPECNT	2
//艺术与路径曲线样式
enum  eBaseARtTypesetPath
{
	BaseGraphPATH_NULL,
	BaseGraphPATH_ARC,				//弧
	BaseGraphPATH_BEZIER,			//任意BEZIER
	BaseGraphPATH_ARTPATH_START,	//艺术字样式开始
	BaseGraphPATH_FLAT,			//直线
	BaseGraphPATH_UPBEZIER,		//上BEZIER
	BaseGraphPATH_DOWNBEZIER,		//下BEZIER
	BaseGraphPATH_BaseGraphMMETRYBEZIER,	//对称BEZIER
	BaseGraphPATH_SAMEBEZIER,		//上下一样BEZIER
	BaseGraphPATH_ARTPATH_END,		//艺术字样式结束
};

//文字属性结构体
typedef struct sy_text_prop_def
{
	DWORD		dwVersion;			//版本号
	BaseARray		arFont;				//字体表
	BaseARray		ar2DColor;			//2D颜色表
	BaseARray		ar3DColor;			//3D颜色表
	BaseARray		arUnderLine;		//下划线表
	BaseARray		arIndex;			//属性检索表
	sy_text_prop_def() { Reset(); }
	BASEGRAPH_DATARENDER_API void Reset();
}BaseGraphTextProp, *LPCommonTextProp;
typedef const BaseGraphTextProp* LPCBaseTextProp;

//下划线类型
typedef enum e_text_under_line
{
	UNDERLINE_NULL = -1,	//无下划线
	UNDERLINE_SOLID = 0,	//实线
	UNDERLINE_DASH,			//-------
	UNDERLINE_DOT,			//.......
	UNDERLINE_DASHDOT,		//_._._._
	UNDERLINE_DASHDOTDOT,	//_.._.._
}E_TextUnderLine;

//艺术排版数据结构
typedef struct sy_art_typeset_def
{
	DWORD				dwVersion;			//版本号
	eBaseARtTypesetPath	ePath;				//排版路径曲线类型
	BOOL				bRotate;			//是否旋转
	float				fPitch;				//字间距0-0.5,曲线长度的百分比。
	BaseARray				arHandlePointUp;		//关键点位置数组
	BaseARray				arHandlePointDown;		//关键点位置数组
	LPVOID				pTitleCurve;			// 曲线文字，专用
	sy_art_typeset_def() { Reset(); }
	BASEGRAPH_DATARENDER_API void Reset();
}BaseARtTypeset, *LPBaseARtTypeset;
typedef const BaseARtTypeset* LPCBaseARtTypeset;

//文本排版原始数据结构
typedef struct sy_text_def
{
	/**********存档数据**********/
	DWORD				dwVersion;			//版本号
	LPTSTR				szText;				//文本数据
	rectf				rtBorder;			//排版参考边框
	rectf				rtTypesetReal;		//线性排版后实际占据的矩形（排版后更新）。
	rectf				rtFixedFrame;		//固定字框矩形（暂时未启用）

	BOOL				bUseBorderEditMode;	//是否使用参考边框编辑模式
	BOOL				bOneClusterOneUnit;	//一个图形簇作为一个处理单元（同时禁止簇中换行）
	BOOL				bHorizontal;		//是否横排
	BOOL				bLeftInput;			//是否从左向右阅读顺序（文档序）
	BOOL				bVLeftTypeset;		//竖排版时是否从左向右排版
	BOOL				bLineWrap;			//是否自动折行
	BOOL				bFixedFrame;		//是否固定字框
	E_TextAlignVeritical	eAlignVertical;	//纵向对齐方式

	BOOL				bPageMode;
	int					nCurPageIndex;

	//小字特殊处理
	int					nSmallFontFactor;	//小字加亮(加粗)系数,0-10,表示n/16像素
	PI_Quality			nSmallFontQuality;	//小字抗闪质量
	BOOL				bSmallFontCalcBorder;//小字清晰

	BaseARray				arParagraph;			//段落数组
	BaseGraphTextProp			stuTextProp;			//属性数据集合

	point2f				vGraphScale;			//图形放缩

	int					nLinesLimit;			//排版总行数极限（<0时无效）
	BOOL				bHideBorderCrosserC;	//隐藏列方向越出边框者
	BOOL				bHideBorderCrosserL;	//隐藏行方向越出边框者

												//艺术属性
	sy_art_typeset_def*	pArtWord;			//非空时，才为艺术字。
	sy_art_typeset_def*	pTypesetPath;		//非空时，才有排版路径。
	BOOL				bCurveFlow;			// 文字是否流动
	double				dEndTime;			// 从流入到结束的时间（总时长的比例）
	double				dCurTime;			// 当前位置的时间（总时长比例）

											/**********临时数据**********/
	LPCommonStretchParam	pStretch;			//3D拉伸属性
	LPCommonSpecialEffect	pSpecialEffect;		//特殊效果（引用）

	BaseARray				ar2DUnit;			//2d unit数组，Unit序列：文字的，下划线的，其它的。
	BaseARray				ar2DUnitTemp;		//临时数组，公告板专用
	BOOL				bStartEdit;			//临时变量，双机进入编辑，公告板专用
	BaseARray				ar3DUnit;			//3d unit数组
	int					nTextUnitCnt;		//文字Unit个数（界定文字与下划线Unit在数组中的范围）
	int					nUnderlineUnitCnt;	//下划线Unit个数
	int					nSpecialUnitCnt;	//特殊效果Unit个数
	WORD				wTempFlag;			//临时标志位

	LPVOID				pTypesetPrivate;	//排版器的私有数据
	LPVOID				hHandle;			//操纵句柄（存本结构指针）
	LPVOID				lpParam;			//预留数据
	sy_text_def() { Reset(); }
	BASEGRAPH_DATARENDER_API void Reset();
}BaseGraphText, *LPCommonText;
typedef const BaseGraphText* LPCBaseText;

//项目符号的原始数据定义
typedef struct sy_textsymbol_def
{
	/**********存档数据**********/
	DWORD				dwVersion;			//版本号
	static_effect		seFillColor;		//填充色
	rectf				rtSize;				//原始大小
	float				fScale;				//缩放比例(0~1)
											/**********临时数据**********/
	LPVOID				hHandle;			//操纵句柄（存本结构指针）
	LPVOID				lpParam;			//预留数据
	sy_textsymbol_def() { Reset(); }
	BASEGRAPH_DATARENDER_API void Reset();
}BaseGraphTextSymbol, *LPCommonTextSymbol;

//段落的原始数据定义
typedef struct sy_textparagraph_def
{
	/**********存档数据**********/
	DWORD					dwVersion;		//版本号
	int						nBeginIndex;	//段起始字符索引
	E_TextAlignHorizontal	eAlignHorizontal;//横向对齐方式	
	float					fLineDistance;	//行间距
	float					fBeforeDistance;//段前间距
	float					fAfterDistance;	//段后间距
	int						nLevel;			//级别
	float					fIndent;		//缩进量
	LPCommonTextSymbol			lpSymbol;		//项目符号

											/**********临时数据**********/
	LPVOID					hHandle;		//操作句柄（存本结构指针）
	LPVOID					lpParam;		//预留数据
	sy_textparagraph_def() { Reset(); }
	BASEGRAPH_DATARENDER_API void Reset();
}BaseGraphTextParagraph, *LPCommonTextParagraph;
typedef struct  tag_BaseGraphTypesetPrivate
{
	BaseARray				m_arParagraph;		//段落
	LPVOID				lpParam;			//需要做同步的Env
	rectf				rtTypesetBorder;	//排版实际使用的矩形
}BaseGraphTypesetPrivate, *LPCommonTypesetPrivate;

//扩充的字体数据结构
struct sy_under_line_def
{
	DWORD			dwVersion;				//版本号
	E_TextUnderLine	eType;					//线类型
	float			fDistance;				//线相对于基线的偏移
	float			fHeight;				//线高
	static_effect	se2DColor;				//2D颜色
	sy_under_line_def() { Reset(); }
	BASEGRAPH_DATARENDER_API void Reset();
};
typedef sy_under_line_def BaseGraphUnderLine, *LPCommonUnderLine;
typedef const LPCommonUnderLine LPCBaseUnderLine;
//文字属性数据块类别定义
typedef enum e_text_property_type
{
	/**********字体属性**********/
	TEXT_PROPERTY_TYPE_FONT_START = 0,
	TEXT_PROPERTY_TYPE_FONT_HEIGHT,
	TEXT_PROPERTY_TYPE_FONT_WIDTH,
	TEXT_PROPERTY_TYPE_FONT_ESCAPEMENT,
	TEXT_PROPERTY_TYPE_FONT_ORIENTATION,
	TEXT_PROPERTY_TYPE_FONT_BOLD,
	TEXT_PROPERTY_TYPE_FONT_ITALIC,
	TEXT_PROPERTY_TYPE_FONT_CHARSET,
	TEXT_PROPERTY_TYPE_FONT_OUTPRECISION,
	TEXT_PROPERTY_TYPE_FONT_CLIPPRECISION,
	TEXT_PROPERTY_TYPE_FONT_QUALITY,
	TEXT_PROPERTY_TYPE_FONT_PITCHANDFAMILY,
	TEXT_PROPERTY_TYPE_FONT_FACENAME,
	TEXT_PROPERTY_TYPE_FONT_FACENAMEENG,
	TEXT_PROPERTY_TYPE_FONT_DISTANCE,
	TEXT_PROPERTY_TYPE_FONT_STRIKEOUT,
	TEXT_PROPERTY_TYPE_FONT_UNDERLINE,
	TEXT_PROPERTY_TYPE_FONT_UNDERLINE_TYPE,
	TEXT_PROPERTY_TYPE_FONT_UNDERLINE_POS,
	TEXT_PROPERTY_TYPE_FONT_UNDERLINE_WIDTH,
	TEXT_PROPERTY_TYPE_FONT_UNDERLINE_COLOR,
	TEXT_PROPERTY_TYPE_FONT_END,

	/**********颜色属性**********/
	TEXT_PROPERTY_TYPE_COLOR_START = 100,
	TEXT_PROPERTY_TYPE_COLOR_FACE,
	TEXT_PROPERTY_TYPE_COLOR_BORDER,
	TEXT_PROPERTY_TYPE_COLOR_SHADOW,
	TEXT_PROPERTY_TYPE_COLOR_CUBE,
	TEXT_PROPERTY_TYPE_COLOR_BORDERCUBE,
	TEXT_PROPERTY_TYPE_COLOR_BORDERSHADOW,
	TEXT_PROPERTY_TYPE_COLOR_FACELIGHT,
	TEXT_PROPERTY_TYPE_COLOR_CUBELIGHT,
	TEXT_PROPERTY_TYPE_COLOR_CHANNEL,
	TEXT_PROPERTY_TYPE_COLOR_MATERIAL,
	TEXT_PROPERTY_TYPE_COLOR_APPENDMATERIAL,
	TEXT_PROPERTY_TYPE_COLOR_RENDERLINE,
	TEXT_PROPERTY_TYPE_COLOR_WIDTH,
	TEXT_PROPERTY_TYPE_COLOR_INDEX,
	TEXT_PROPERTY_TYPE_COLOR_END,

}E_TextPropertyType;

//文字属性数据块总类别定义
typedef enum e_text_prop_type
{
	TEXT_PROP_TYPE_FONT = 0,
	TEXT_PROP_TYPE_2DCOLOR,
	TEXT_PROP_TYPE_3DCOLOR,
	TEXT_PROP_TYPE_UNDERLINE,
}E_TextPropType;
//文字属性索引
typedef struct sy_text_prop_index_def
{
	LPVOID					pFont;		//字体属性索引
	LPVOID					p2DColor;	//2D颜色属性索引
	LPVOID					p3DColor[BaseGraphTEXT_SUBFACE_CNT];	//3D颜色属性索引
	LPVOID					pUnderLine;	//下划线属性索引
}BaseGraphTextPropIndex, *LPCommonTextPropIndex;
typedef const BaseGraphTextPropIndex* LPCBaseTextPropIndex;
