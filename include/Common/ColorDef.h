#pragma once
#include "Common\DataDef.h"
// 颜色种类

enum BaseGraphCOLOR_TYPE
{
	BaseGraphCOLOR_HOMO = 0,				// 单色
	BaseGraphCOLOR_GRAD_LINEAR,			// 线性渐变色
	BaseGraphCOLOR_GRAD_CENTER_ROUND,		// 圆形中心渐变色
	BaseGraphCOLOR_GRAD_CENTER_POLYGON,	// 凸多边形中心渐变色
	BaseGraphCOLOR_GRAD_OUTLINE_CIRCLE,	// 圆周轮廓渐变色
	BaseGraphCOLOR_GRAD_OUTLINE_POLYGON,	// 多边形轮廓渐变色
	BaseGraphCOLOR_GRAD_PT_CONTOUR,		// 多点等高渐变色
	BaseGraphCOLOR_GRAD_PT_BLEND,			// 多点混成渐变色
	BaseGraphCOLOR_TYPE_COUNT,				// 种类数量
	BaseGraphCOLOR_GRAD_TEMPLET,			// 模板渐变色
	BaseGraphCOLOR_TEX_IMG,				// 图片纹理
	BaseGraphCOLOR_NONE,					//没有颜色
};
//player status
enum BaseGraphPLAYERSTATUS
{
	PRERUN = 0,			//being preparation
	PRERUNFINISH,		//preparation finished
	PLAY,				//Playing
	PAUSE,				//pause
	STOP,				//stop
	CURCLIPFINISH,		//when current clip finished running(in project play and loop play) is looping, GETFRAME(dwOutData)shows loop time
	HANDPAUSE,			//Manually pause, GETFRAME(dwOutData)shows HPTTYPE, return HANDPAUSE_NOSENDMSG to show user handwnd
	DYMSTOP,				//DYM play finished
	CURCLIPRENDERFINISH,	//Current Clip render finish	
	LOOPONETIMEFINISH,	//Loop One Time finish	
	PRELOOPONETIMEFINISH,
	SEGSTOP,
	ISSEGEND,
	ISPRESEGEND,
	DLGINPAUSE,
	DLGOUTPAUSE,
	DLGSTOP,
	TURNDLGPLAY,
	TIMEFREEZING,
	IDLE,
	STORYSTARTTIME,
	PLAYSTARTTIME,		//设置时间
	UNKNOWN,				//Unknown status	
							//////////////////////////////////////////////////////////////////////////
							USERSTATUS_START,		//Beginning for user self define status
							SYNCHRONISE,			//Synchronise
							FORCESTOP			//Force stop
};

//Split callback parameter
#define GETSTATUS(dwOutData)	((int)((dwOutData & 0xff000000) >> 24))  //obtain player running status
#define GETFRAME(dwOutData)		((DWORD)(dwOutData & 0x00ffffff))        //obtain player current frame number

// 重复渲染方式
enum BaseGraphCOLOR_STYLE
{
	BaseGraphCOLOR_STYLE_BaseGraphMMETRY = 0,		// 对称排列
	BaseGraphCOLOR_STYLE_SEQUENCE,			// 顺序排列

	BaseGraphCOLOR_STYLE_COUNT,
};
// 颜色容器支持颜色的定义
enum BaseGraphCOLOR_SUPPORT
{
	SUPPROT_NONE = 0x00000000,											// 不支持任何

	SUPPORT_HOMO = (1 << BaseGraphCOLOR_HOMO),									// 支持单色
	SUPPORT_GRAD_LINEAR = (1 << BaseGraphCOLOR_GRAD_LINEAR),					// 支持线性渐变色
	SUPPORT_GRAD_CENTER_ROUND = (1 << BaseGraphCOLOR_GRAD_CENTER_ROUND),		// 支持圆形中心渐变色
	SUPPORT_GRAD_CENTER_POLYGON = (1 << BaseGraphCOLOR_GRAD_CENTER_POLYGON),	// 支持凸多边形中心渐变色
	SUPPORT_GRAD_OUTLINE_CIRCLE = (1 << BaseGraphCOLOR_GRAD_OUTLINE_CIRCLE),	// 支持圆周轮廓渐变色
	SUPPORT_GRAD_OUTLINE_POLYGON = (1 << BaseGraphCOLOR_GRAD_OUTLINE_POLYGON),	// 支持多边形轮廓渐变色
	SUPPORT_GRAD_PT_CONTOUR = (1 << BaseGraphCOLOR_GRAD_PT_CONTOUR),			// 支持多点等高渐变色
	SUPPORT_GRAD_PT_BLEND = (1 << BaseGraphCOLOR_GRAD_PT_BLEND),				// 支持多点混成渐变色
	SUPPORT_GRAD_TEMPLET = (1 << BaseGraphCOLOR_GRAD_TEMPLET),					// 支持模板渐变色
	SUPPORT_TEX_IMG = (1 << BaseGraphCOLOR_TEX_IMG),							// 支持图片纹理

	SUPPORT_ALL_COLOR = 0x0000ffff,										// 支持所有颜色
};
// 颜色特殊常量定义
enum BaseGraphCOLOR_CONSTANT
{
	BaseGraphCOLOR_GRAD_PEG_MAX = 8,			// 渐变色的最多节点数
	BaseGraphCOLOR_GRAD_PEG_MIN = 2,			// 渐变色的最少节点数

	BaseGraphCOLOR_GRAD_VERTEX_MIN = 3,		// 渐变色的最少顶点数
	BaseGraphCOLOR_GRAD_VERTEX_MAX = 8,		// 渐变色的最多顶点数

	BaseGraphCOLOR_GRAD_REPEAT_MAX = 32,		// 渐变色重复的最大数
};
// 图片纹理的填充与排列方式定义
enum BaseGraphCOLOR_IMG_TYPE {
	BaseGraphCOLOR_IMG_ZOOM = 0,		// 缩放
	BaseGraphCOLOR_IMG_COPY,			// 拷贝
	BaseGraphCOLOR_IMG_TILES,			// 平铺

	BaseGraphCOLOR_IMG_TYPE_COUNT,
};
enum BaseGraphCOLOR_IMG_ALIGN_ZOOM {	// 缩放的对齐方式
	BaseGraphCOLOR_ALIGN_ZOOM_FULL = 0,	// 满屏填充
	BaseGraphCOLOR_ALIGN_ZOOM_WIDTH,		// 按宽度比例
	BaseGraphCOLOR_ALIGN_ZOOM_HEIGHT,		// 按高度比例

	BaseGraphCOLOR_ALIGN_ZOOM_COUNT,
};
enum BaseGraphCOLOR_IMG_ALIGN_COPY {	// 拷贝的对齐方式
	BaseGraphCOLOR_ALIGN_COPY_CENTER = 0,		// 居中
	BaseGraphCOLOR_ALIGN_COPY_LEFTTOP,			// 左上角
	BaseGraphCOLOR_ALIGN_COPY_LEFTBOTTOM,		// 左下角
	BaseGraphCOLOR_ALIGN_COPY_RIGHTTOP,		// 右上角
	BaseGraphCOLOR_ALIGN_COPY_RIGHTBOTTOM,		// 右下角

	BaseGraphCOLOR_ALIGN_COPY_COUNT,
};

#define BaseGraphCOLOR_POINT_VALUE_MIN		-10.f	// 点的最小位置（允许点落在一个范围）
#define BaseGraphCOLOR_POINT_VALUE_MAX		10.f	// 点的最大位置

// 渐变动画类型定义
enum GRAD_ANI_STYLE
{
	GRAD_ANI_NONE = 0x00000000,		// 静态

	GRAD_ANI_COLOR = 0x00000001,		// 节点颜色
	GRAD_ANI_LINEAR_RAD = 0x00000002,		// 线性渐变角度
	GRAD_ANI_REPEAT = 0x00000004,		// 重复次数

	GRAD_ANI_CENTER_X = 0x00000010,		// 中心点X
	GRAD_ANI_CENTER_Y = 0x00000020,		// 中心点Y

	GRAD_ANI_VERTEX_0_X = 0x00010000,		// 顶点X0
	GRAD_ANI_VERTEX_0_Y = 0x00020000,		// 顶点Y0
	GRAD_ANI_VERTEX_1_X = 0x00040000,		// 顶点X1
	GRAD_ANI_VERTEX_1_Y = 0x00080000,		// 顶点Y1
	GRAD_ANI_VERTEX_2_X = 0x00100000,		// 顶点X2
	GRAD_ANI_VERTEX_2_Y = 0x00200000,		// 顶点Y2
	GRAD_ANI_VERTEX_3_X = 0x00400000,		// 顶点X3
	GRAD_ANI_VERTEX_3_Y = 0x00800000,		// 顶点Y3
	GRAD_ANI_VERTEX_4_X = 0x01000000,		// 顶点X4
	GRAD_ANI_VERTEX_4_Y = 0x02000000,		// 顶点Y4
	GRAD_ANI_VERTEX_5_X = 0x04000000,		// 顶点X5
	GRAD_ANI_VERTEX_5_Y = 0x08000000,		// 顶点Y5
	GRAD_ANI_VERTEX_6_X = 0x10000000,		// 顶点X6
	GRAD_ANI_VERTEX_6_Y = 0x20000000,		// 顶点Y6
	GRAD_ANI_VERTEX_7_X = 0x40000000,		// 顶点X7
	GRAD_ANI_VERTEX_7_Y = 0x80000000,		// 顶点Y7
};
// 渐变动画参数索引定义
enum GRAD_ANI_PARAM
{
	GRAD_ANI_PARAM_COLOR = 0,	// 节点颜色
	GRAD_ANI_PARAM_REPEAT,		// 重复次数

	GRAD_ANI_PARAM_LINEAR_RAD,	// 线性渐变角度
								// 中心点
								GRAD_ANI_PARAM_CENTER_X = GRAD_ANI_PARAM_LINEAR_RAD,
								GRAD_ANI_PARAM_CENTER_Y,

								GRAD_ANI_PARAM_V0_X,		// 顶点参数索引
								GRAD_ANI_PARAM_V0_Y,
								GRAD_ANI_PARAM_V1_X,
								GRAD_ANI_PARAM_V1_Y,
								GRAD_ANI_PARAM_V2_X,
								GRAD_ANI_PARAM_V2_Y,
								GRAD_ANI_PARAM_V3_X,
								GRAD_ANI_PARAM_V3_Y,
								GRAD_ANI_PARAM_V4_X,
								GRAD_ANI_PARAM_V4_Y,
								GRAD_ANI_PARAM_V5_X,
								GRAD_ANI_PARAM_V5_Y,
								GRAD_ANI_PARAM_V6_X,
								GRAD_ANI_PARAM_V6_Y,
								GRAD_ANI_PARAM_V7_X,
								GRAD_ANI_PARAM_V7_Y,

								GRAD_ANI_PARAM_COUNT,		// 参数个数
};

//////////////////////////////////////////////////////////////////////////
// 获取单色信息中的分量值

//#define COLORABGR_A(pcr)  *((BYTE*)pcr + 4)
#define GetRGBA_A(cr)		((BYTE)(((DWORD)(cr)) >> 24))
#define GetAValue(cr)		((BYTE)(((DWORD)(cr)) >> 24))
#define GetBGRA_R(cr)		((BYTE)((((DWORD)(cr)) >> 16) & 0x000000ff))
#define GetBGRA_G(cr)		((BYTE)((((DWORD)(cr)) >> 8) & 0x000000ff))
#define GetBGRA_B(cr)		((BYTE)(((DWORD)(cr)) & 0x000000ff))

#define GetRGBA_R(cr)		((BYTE)(((DWORD)(cr)) & 0x000000ff))
#define GetRGBA_G(cr)		((BYTE)((((DWORD)(cr)) >> 8) & 0x000000ff))
#define GetRGBA_B(cr)		((BYTE)((((DWORD)(cr)) >> 16) & 0x000000ff))
// 组色
#define RGBA(r, g, b, a)  	((DWORD)(r) | (((DWORD)(g)) << 8) | (((DWORD)(b)) << 16) | (((DWORD)(a)) << 24))
#define BGRA(b, g, r, a)  	((DWORD)(b) | (((DWORD)(g)) << 8) | (((DWORD)(r)) << 16) | (((DWORD)(a)) << 24))
// 求解亮度
#define GetLum(r, g, b)		(0.299f * (r) + 0.587f * (g) + 0.114f * (b))
// 从带 Alpha 单色中取出系统显示需要的单色 COLORREF
#define GetRGBA_Cr(rgba)	((COLORREF)((rgba) & 0x00ffffff))
#define GetBGRA_Cr(bgra)	RGB(GetBGRA_R((bgra)), GetBGRA_G((bgra)), GetBGRA_B((bgra)))

//////////////////////////////////////////////////////////////////////////
// 颜色结构定义，采用联合的方式做到对所有种类颜色的兼容
typedef   struct tag_common_color : public BaseGraph_Common::ValueAbleInterface
{
	unsigned long type;				// 颜色种类
									// 按分类定义
	struct
	{
		unsigned long color;		// 单色颜色值（0xARGB）
		struct
		{
			TCHAR	name[512];	// 图片纹理名称
			unsigned long	fill;// 填充方式
			unsigned long	align;// 对齐方式
		}imgtex;

		struct
		{					// 渐变色
							// 公共属性
			unsigned long	color[8];	// 渐变节点颜色
			int				ncount;		// 限制上限为 8
			float			pos[8];		// 节点百分比位置
			float			mid[7];		// 中间点百分比（最多使用 n - 1 个中间点）
										// 按内部子类定义
			struct {
				struct 
				{		// 线性渐变
					float			frad;		// 角度
					unsigned long	repeat;		// 重复次数
					unsigned long	style;		// 方式：对称、顺序
				}linear;
				struct 
				{		// 中心渐变
					float			centerx;	// 中心坐标
					float			centery;
					unsigned long	repeat;		// 重复次数
					unsigned long	style;		// 方式：对称、顺序
												// 多边形渐变的顶点位置（必须为围绕中心点的凸多边形）
												// 顶点与节点不绑定
					int				nvertex;	// 顶点数量
					float			xpt[8];		// 目前只支持 8 个点
					float			ypt[8];
				}center;
				struct 
				{		// 轮廓渐变
					float			centerx;	// 中心坐标
					float			centery;
					// 圆周渐变的参数
					unsigned long	repeat;		// 重复次数
					unsigned long	style;		// 方式：对称、顺序
												// 多边形渐变的顶点位置（必须为围绕中心点的凸多边形）
												// 顶点与节点绑定
					float			xpt[8];		// 目前只支持 8 个点
					float			ypt[8];
				}outline;
				struct 
				{		// 多点渐变，与节点绑定
					float			xpt[8];		// 顶点位置
					float			ypt[8];
				}multipt;
				struct 
				{		// 模板渐变
					TCHAR	name[32];	// 固定位置下的模板名称（8 位位图文件）
				}templet;
			};
			// 渐变动画参数
			unsigned long	animation;		// 动画类型
			float			aniparam[GRAD_ANI_PARAM_COUNT];	// 参数
		}gradual;
	};
	short				theme;			//0表示不使用主题色，从1开始编号
	short				subtheme;		//饱和度

	BOOL operator == (const  tag_common_color & other)
	{
		if (type != other.type)
		{
			return FALSE;
		}
		if (type == other.type)
		{
			if (type == BaseGraphCOLOR_NONE)
			{
				return TRUE;
			}
			else if (type == BaseGraphCOLOR_HOMO)
			{
				if (color != other.color)
				{
					return FALSE;
				}
			}
			else if (type == BaseGraphCOLOR_TEX_IMG)
			{
				if (memcmp(&imgtex, &other.imgtex, sizeof(imgtex)) != 0)
				{
					return FALSE;
				}
			}
			else
			{
				if (memcmp(&gradual, &other.gradual, sizeof(gradual)) != 0)
				{
					return FALSE;
				}
			}
		}

		return TRUE;
	}
	BASEGRAPH_COMMON_API tag_common_color();
	BASEGRAPH_COMMON_API void Reset();
	BASEGRAPH_COMMON_API virtual int GetSerializeSize() const;
	BASEGRAPH_COMMON_API virtual bool GetSerializeBuf(byte * pBuf, int nBufSize) const;
	BASEGRAPH_COMMON_API virtual bool RecoverFromBuf(byte * pBuf, int nBufSize);
	BASEGRAPH_COMMON_API virtual CString GetValueStr();
	BASEGRAPH_COMMON_API virtual bool RecoverFromCString(CString strType, CString strValue);
}common_color;

typedef common_color BaseGraphColor, *LPCommonCOLOR;

// BaseGraphRGBA 联合定义
union BaseGraphRGBA
{
	struct
	{
		unsigned char byR;
		unsigned char byG;
		unsigned char byB;
		unsigned char byA;
	} m_rgbaValue;

	unsigned long m_dwColor;
};

// BGRA 联合定义
union BGRA
{
	struct
	{
		unsigned char byB;
		unsigned char byG;
		unsigned char byR;
		unsigned char byA;
	} m_bgraValue;

	unsigned long m_dwColor;
};
//////////////////////////////// TYUV 颜色结构 /////////////////////////////////
union COLORTYUV
{
	struct {
		BYTE	V;
		BYTE	U;
		BYTE	Y;
		BYTE	T;
	};

	DWORD	TYUV;
};

typedef	COLORTYUV	*LPCOLORTYUV;

#define COLORTYUV_T(color)	(((LPCOLORTYUV)(&color))->T)
#define COLORTYUV_Y(color)	(((LPCOLORTYUV)(&color))->Y)
#define COLORTYUV_U(color)	(((LPCOLORTYUV)(&color))->U)
#define COLORTYUV_V(color)	(((LPCOLORTYUV)(&color))->V)

//#define TYUV(t, y, u, v)	(DWORD)(((t<<24)&0xff000000)|((y<<16)&0xff0000)|((u<<8)&0xff00)|((v)&0xff))
#define TYUV(t, y, u, v)	((DWORD)((((DWORD)(BYTE)(t))<<24)|(((DWORD)(BYTE)(y))<<16)|((WORD)((BYTE)(u))<<8)|((BYTE)(v))))

#define TYUVColor(t, y, u, v)	\
	TYUV(t, y, u, v)

#define ColorTYUV(Color, pT, pY, pU, pV)	\
	*pT = COLORTYUV_T(Color), \
	*pY = COLORTYUV_Y(Color), \
	*pU = COLORTYUV_U(Color), \
	*pV = COLORTYUV_V(Color)

#define DWGetUValue(tyuv)   ((BYTE)(tyuv))
#define DWGetVValue(tyuv)   ((BYTE)((tyuv)>>8))
#define DWGetYValue(tyuv)   ((BYTE)((tyuv)>>16))
#define DWGetTValue(tyuv)   ((BYTE)((tyuv)>>24))

///////////////////////////// ARGB 颜色结构(RGBQUAD) ///////////////////////////
union COLORARGB
{
	struct {
		BYTE	B;
		BYTE	G;
		BYTE	R;
		BYTE	A;
	};

	DWORD	ARGB;
};

typedef	COLORARGB	*LPCOLORARGB;

#define COLORARGB_A(color)	(((LPCOLORARGB)(&color))->A)
#define COLORARGB_R(color)	(((LPCOLORARGB)(&color))->R)
#define COLORARGB_G(color)	(((LPCOLORARGB)(&color))->G)
#define COLORARGB_B(color)	(((LPCOLORARGB)(&color))->B)

#define ARGBColor(a, r, g, b)	\
	BGRA(b, g, r, a)

#define ColorARGB(Color, pA, pR, pG, pB)	\
	*pA = COLORARGB_A(Color), \
	*pR = COLORARGB_R(Color), \
	*pG = COLORARGB_G(Color), \
	*pB = COLORARGB_B(Color)

#define	ARGB_WEIGHT(color)	(0.299*COLORARGB_R(color)+0.587*COLORARGB_G(color)+0.114*COLORARGB_B(color))

#define DWGetBValue(argb)   ((BYTE)(argb))
#define DWGetGValue(argb)   ((BYTE)((argb)>>8))
#define DWGetRValue(argb)   ((BYTE)((argb)>>16))
#define DWGetAValue(argb)   ((BYTE)((argb)>>24))

////////////////////////////////////////////////////////////////////////////////
////////////////////////////// ABGR 颜色结构(COLORREF) /////////////////////////

union COLORABGR
{
	struct {
		BYTE	R;
		BYTE	G;
		BYTE	B;
		BYTE	A;
	};

	DWORD	ABGR;
};

typedef	COLORABGR	*LPCOLORABGR;

#ifndef COLORABGR_A
#define COLORABGR_A(color)	(((LPCOLORABGR)(&color))->A)
#endif
#define COLORABGR_B(color)	(((LPCOLORABGR)(&color))->B)
#define COLORABGR_G(color)	(((LPCOLORABGR)(&color))->G)
#define COLORABGR_R(color)	(((LPCOLORABGR)(&color))->R)

//#define ABGR(a, b, g, r)	(DWORD)(((a<<24)&0xff000000)|((b<<16)&0xff0000)|((g<<8)&0xff00)|((r)&0xff))
#define ABGR(a, b, g, r)	((DWORD)((((DWORD)(BYTE)(a))<<24)|(((DWORD)(BYTE)(b))<<16)|((WORD)((BYTE)(g))<<8)|((BYTE)(r))))
#define ABGRColor(a, b, g, r)	\
	ABGR(a, b, g, r)

#define ColorABGR(Color, pA, pB, pG, pR)	\
	*pA = COLORABGR_A(Color), \
	*pB = COLORABGR_B(Color), \
	*pG = COLORABGR_G(Color), \
	*pR = COLORABGR_R(Color)

#define	COLORABGR_WEIGHT(color)	(0.299*COLORABGR_R(color)+0.587*COLORABGR_G(color)+0.114*COLORABGR_B(color))

/////////////////////////// COLORRGB 颜色结构(RGBTRIPLE) ///////////////////////

#include <pshpack1.h>
struct COLORRGB
{
	BYTE		B;
	BYTE		G;
	BYTE		R;
};
#include <poppack.h>

typedef	COLORRGB	*LPCOLORRGB;

#define COLORRGB_R(color)	(((LPCOLORRGB)(&color))->R)
#define COLORRGB_G(color)	(((LPCOLORRGB)(&color))->G)
#define COLORRGB_B(color)	(((LPCOLORRGB)(&color))->B)

#define RGB2(r, g, b)		((COLORRGB)((((DWORD)(BYTE)(r))<<16)|((WORD)((BYTE)(g))<<8)|((BYTE)(b))))

#define RGBColor(r, g, b)	\
	RGB2(r, g, b)

#define ColorRGB(Color, pR, pG, pB)	\
	*pR = COLORRGB_R(Color), \
	*pG = COLORRGB_G(Color), \
	*pB = COLORRGB_B(Color)

#define	COLORRGB_WEIGHT(color)	(0.299*COLORRGB_R(color)+0.587*COLORRGB_G(color)+0.114*COLORRGB_B(color))

////////////////////////////////// TYUV 色值 ///////////////////////////////////

#define YUV_BLACK			0x00108080L
#define YUV_WHITE			0x00EB8080L
#define YUV_LIGHTGRAY		0x00B58080L
#define YUV_DARKGRAY		0x00478080L

// YUV ==> RGB 转换矩阵
#define YUV11				0x129E4L
#define YUV12				0x129E4L
#define YUV13				0x129E4L
#define YUV21				0x1D5L
#define YUV22				0x62EBL
#define YUV23				0x201DFL
#define YUV31    			0x19589L
#define YUV32    			0xCF41L
#define YUV33    			0x12FL

// RGB ==> YUV 转换矩阵
#define RGB11    			0x41FFL
#define RGB12    			0x25FDL
#define RGB13    			0x70FFL
#define RGB21    			0x80FFL
#define RGB22    			0x4B02L
#define RGB23    			0x5F00L
#define RGB31    			0x1903L
#define RGB32    			0x70FFL
#define RGB33    			0x11FFL

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// RGB 色值 ///////////////////////////////////

#define RGB_BLACK			0x000000
#define RGB_WHITE			0xFFFFFF
#define RGB_GRAY			0x7F7F7F

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// ARGB/ABGR 色值 ////////////////////////////////

#define ALPHA_ZERO			0x00	// 全透
#define ALPHA_FULL			0xFF	// 不透

// 绝对透明程度(0 -=> 255)
#define DEGREE_TRANS_MAX	0xFF	// 不透
#define DEGREE_TRANS_MIN	0x00	// 全透

#define DEGREE_TRANS_NO		0xFF	// 不透
#define DEGREE_TRANS_ALL	0x00	// 全透

#define HALF_TRANS_GRAY		0x80808080	// 半透灰色

#define ALL_TRANS_COLOR		0x00000000	// 全透色

#define ARGB_BLACK			0xFF000000
#define ARGB_WHITE			0xFFFFFFFF
#define ARGB_GRAY			0xFF808080
#define ARGB_DARKBLACK		0xFE010101
#define ARGB_DARKWHITE		0xFEFEFEFE

#define ABGR_BLACK			0xFF000000
#define ABGR_WHITE			0xFFFFFFFF
#define ABGR_GRAY			0xFF808080
#define ABGR_DARKWHITE		0xFEFEFEFE

/////////////////////////////////// 色制转换 ///////////////////////////////////
//  ARGB <==> ABGR
inline void ARGBtoABGRSelf(DWORD& color)
{
	register BYTE	bTemp = COLORARGB_B(color);
	COLORABGR_R(color) = COLORARGB_R(color);
	COLORABGR_B(color) = bTemp;
}

inline void ABGRtoARGBSelf(DWORD& color)
{
	register BYTE	bTemp = COLORABGR_R(color);
	COLORARGB_B(color) = COLORABGR_B(color);
	COLORARGB_R(color) = bTemp;
}

inline DWORD ARGBtoABGR(DWORD color)
{
	ARGBtoABGRSelf(color);
	return	color;
}

inline DWORD ABGRtoARGB(DWORD color)
{
	ABGRtoARGBSelf(color);
	return	color;
}
