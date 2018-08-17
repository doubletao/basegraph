// Header FileName:	Common\PlaneDef.h
// CPP FileName:	SYPlane.cpp

#ifndef __Common_PLANE_INC__
#define __Common_PLANE_INC__

#include "Common\VectorDef.h"

/* C++版本编译条件
#ifndef __cplusplus
#define __cplusplus
#endif */
 
///////////////////////////////////////////////////////////////////////////////
//updated point set bound 
#define UPDATE_BOUNDF(RtBound,fpt)\
{\
	if(fpt.x < RtBound.left)\
	RtBound.left = fpt.x;\
	if(fpt.x > RtBound.right)\
	RtBound.right = fpt.x;\
	if(fpt.y < RtBound.top)\
	RtBound.top = fpt.y;\
	if(fpt.y > RtBound.bottom)\
	RtBound.bottom = fpt.y;\
}
//updated point set Y coodinate limit 
#define UPDATE_Y_RANGEF(fYmax,fYmin,fpt)\
{\
	if(fpt.y < fYmin)\
	fYmin = fpt.y;\
	if(fpt.y > fYmax)\
	fYmax = fpt.y;\
}
///////////////////////////////////////////////////////////////////////////////


//////////////////////////////// SYPoint //////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置点值
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYPoint_SetPoint(LPCommonPOINTL pP,			// 点指针
							   LONG x,					// x值
							   LONG y);					// y值

void __cdecl SYPoint2f_SetPoint(LPCommonPOINTF pP,			// 点指针
							   float x,					// x值
							   float y);				// y值

void __cdecl SYPoint2lf_SetPoint(LPCommonPOINTD pP,			// 点指针
							   double x,				// x值
							   double y);				// y值

///////////////////////////////////////////////////////////////////////////////
// 描述：	按给定值偏移
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYPoint_Offset(LPCommonPOINTL pP,			// 点指针
							 LONG xOffset,			// x偏移量
							 LONG yOffset);			// y偏移量

void __cdecl SYPoint2f_Offset(LPCommonPOINTF pP,			// 点指针
							 float xOffset,			// x偏移量
							 float yOffset);		// y偏移量

void __cdecl SYPoint2lf_Offset(LPCommonPOINTD pP,			// 点指针
							 double xOffset,		// x偏移量
							 double yOffset);		// y偏移量




///////////////////////////// End of SYPoint //////////////////////////////////


///////////////////////////////// SYSize //////////////////////////////////////

typedef struct tag_SYSizeL{		// 整型
	LONG	cx;
	LONG	cy;
}SYSizeL, *LPCommonSIZE;

typedef struct tag_SYSizef{		// 浮点型
	float	cx;
	float	cy;
}SYSizef, *LPCommonSIZEF;

typedef struct tag_SYSizelf{		// 双精度型
	double	cx;
	double	cy;
}SYSizelf, *LPCommonSIZELF;

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置size值
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYSize_SetSize(LPCommonSIZE pS,			// size指针
							 LONG cx,				// cx值
							 LONG cy);				// cy值

void __cdecl SYSizef_SetSize(LPCommonSIZEF pS,			// size指针
							 float cx,					// cx值
							 float cy);				// cy值

void __cdecl SYSizelf_SetSize(LPCommonSIZELF pS,			// size指针
							 double cx,				// cx值
							 double cy);			// cy值

///////////////////////////// End of SYSize ///////////////////////////////////


///////////////////////////////// BaseRect //////////////////////////////////////

#define BaseRect_DRAG_COUNT	8		// 拖动点的数量

#define BaseRect_DRAG_LTP		0		// 左上点
#define BaseRect_DRAG_TMP		1		// 上中点
#define BaseRect_DRAG_RTP		2		// 右上点
#define BaseRect_DRAG_RMP		3		// 右中点
#define BaseRect_DRAG_RBP		4		// 右下点
#define BaseRect_DRAG_BMP		5		// 下中点
#define BaseRect_DRAG_LBP		6		// 左下点
#define BaseRect_DRAG_LMP		7		// 左中点

#define BaseRect_DRAG_ONLINE	1		// 拖动点的风格：方格在线上
#define BaseRect_DRAG_INLINE	2		// 拖动点的风格：方格在线内


typedef struct tag_BaseRectL{		// 整型
	LONG	left;
	LONG	top;
	LONG	right;
	LONG	bottom;
}BaseRect, *LPBaseRect;

typedef struct tag_BaseRectF{		// 浮点型
	float	left;
	float	top;
	float	right;
	float	bottom;
}rectf, BaseRectF,*LPBaseRectF;

typedef struct tag_BaseRectlf{		// 双精度型
	double	left;
	double	top;
	double	right;
	double	bottom;
}BaseRectlf,rectlf, *LPBaseRectLF;

// 整型实现

///////////////////////////////////////////////////////////////////////////////
// 描述：	通过点和半径设置矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRect_SetRectBPR(LPBaseRect pR,			// 矩形指针
								POINT point,			// 中心点
								int nRadius);			// 半径

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRect_SetRect(LPBaseRect pR,			// 矩形指针
							 LONG l,				// 左边位置
							 LONG t,				// 上边位置
							 LONG r,				// 右边位置
							 LONG b);				// 下边位置

///////////////////////////////////////////////////////////////////////////////
// 描述：	清空矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRect_SetRectEmpty(LPBaseRect pR);		// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形的宽
//
// 返回值:	LONG
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API LONG __cdecl BaseRect_Width(LPBaseRect pR);				// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形的高
//
// 返回值:	LONG
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API LONG __cdecl BaseRect_Height(LPBaseRect pR);				// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断矩形是否为空（left = top = right = bottom = 0）
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BaseRect_IsRectNull(LPBaseRect pR);			// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断矩形范围是否为空（width <= 0 || heigth <= 0）（区别于IsRectNull）
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BaseRect_IsRectEmpty(LPBaseRect pR);			// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断点是否在矩形内
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BaseRect_PtInRect(LPBaseRect pR,				// 矩形指针
							  SYPoint point);			// 点

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩形相等判断
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BaseRect_EqualRect(LPBaseRect pR1,				// 矩形
							   LPBaseRect pR2);			// 矩形

///////////////////////////////////////////////////////////////////////////////
// 描述：	拷贝矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRect_CopyRect(LPBaseRect pDesR,			// 目标矩形指针
							  LPBaseRect srcR);			// 源矩形

///////////////////////////////////////////////////////////////////////////////
// 描述：	扩张矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRect_InflateRect1(LPBaseRect pR,			// 矩形指针
								  int x,				// 左、右外扩偏移量
								  int y);				// 上、下外扩偏移量

BASEGRAPH_COMMON_API void __cdecl BaseRect_InflateRect2(LPBaseRect pR,			// 矩形指针
								  SYSizeL size);		// 外扩偏移量
								 
BASEGRAPH_COMMON_API void __cdecl BaseRect_InflateRect3(LPBaseRect pR,			// 矩形指针
								  int l,				// 左外扩偏移量
								  int t,				// 上外扩偏移量
								  int r,				// 右外扩偏移量
								  int b);				// 下外扩偏移量

///////////////////////////////////////////////////////////////////////////////
// 描述：	压缩矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRect_DeflateRect1(LPBaseRect pR,			// 矩形指针
								  int x,				// 左、右内缩偏移量
								  int y);				// 上、下内缩偏移量

BASEGRAPH_COMMON_API void __cdecl BaseRect_DeflateRect2(LPBaseRect pR,			// 矩形指针
								  SYSizeL size);		// 内缩偏移量

BASEGRAPH_COMMON_API void __cdecl BaseRect_DeflateRect3(LPBaseRect pR,			// 矩形指针
								  int l,				// 左内缩偏移量
								  int t,				// 上内缩偏移量
								  int r,				// 右内缩偏移量
								  int b);				// 下内缩偏移量

///////////////////////////////////////////////////////////////////////////////
// 描述：	规格化矩形（left < right && top < bottom）
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRect_NormalizeRect(LPBaseRect pR);			// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	归一化矩形
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRect_NormalizeDRect(rectf* pDesR,		// 目标矩形
									LPBaseRect pSrcR,		// 矩形指针
								    SYPoint point,			// 归一化中心点
								    int nWidth,				// 归一化宽
								    int nHeight);			// 归一化高

///////////////////////////////////////////////////////////////////////////////
// 描述：	还原已归一化的矩形
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRect_RevertDRect(LPBaseRect pDesR,			// 目标矩形指针
								 rectf* pSrcR,			// 源矩形
								 SYPoint point,			// 还原中心点
								 int nWidth,				// 还原宽
								 int nHeight);				// 还原高

///////////////////////////////////////////////////////////////////////////////
// 描述：	在x和y方向上按给定值偏移
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRect_OffsetRect1(LPBaseRect pR,			// 矩形指针
								 int x,					// 水平偏移量
								 int y);				// 垂直偏移量

BASEGRAPH_COMMON_API void __cdecl BaseRect_OffsetRect2(LPBaseRect pR,			// 矩形指针
								 SYPoint point);		// 偏移点

BASEGRAPH_COMMON_API void __cdecl BaseRect_OffsetRect3(LPBaseRect pR,			// 矩形指针
								 SYSizeL size);			// 偏移大小

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形范围的大小
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRect_Size(LPCommonSIZE pDesSz,			// 目标Size
						  LPBaseRect pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形左上点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRect_TopLeft(LPCommonPOINTL pDesP,			// 目标点
							 LPBaseRect pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形右下点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRect_BottomRight(LPCommonPOINTL pDesP,			// 目标点
								 LPBaseRect pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形中心点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRect_CenterPoint(LPCommonPOINTL pDesP,			// 目标点
								 LPBaseRect pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形拖动点的爪子，从左上角开始顺时针给出
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRect_DragPoint(LPBaseRect* ppDesR,			// 目标矩形数组
							   LPBaseRect pSrcR,				// 源矩形
							   int nStyle,					// 爪子的风格
							   int nRadius);				// 方格半径

///////////////////////////////////////////////////////////////////////////////
// 描述：	求两个矩形的交集
//
// 返回值:	TRUE			交集不为空
//			FALSE			交集为空
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BaseRect_IntersectRect(LPBaseRect pDesR,			// 交集结果
								   LPBaseRect pSrcR1,		// 源矩形
								   LPBaseRect pSrcR2);		// 源矩形

///////////////////////////////////////////////////////////////////////////////
// 描述：	求两个矩形的并集
//
// 返回值:	TRUE			并集不为空
//			FALSE			并集为空
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BaseRect_UnionRect(LPBaseRect pDesR,			// 并集结果
							   LPBaseRect pSrcR1,		// 源矩形
							   LPBaseRect pSrcR2);		// 源矩形


// 浮点型实现
BASEGRAPH_COMMON_API void __cdecl BaseRectf_Scale(rectf* rt,float fscale);
///////////////////////////////////////////////////////////////////////////////
// 描述：	设置矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectf_SetRect(rectf* pR,			// 矩形指针
							 float l,				// 左边位置
							 float t,				// 上边位置
							 float r,				// 右边位置
							 float b);				// 下边位置

///////////////////////////////////////////////////////////////////////////////
// 描述：	清空矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectf_SetRectEmpty(rectf* pR);		// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形的宽
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API float __cdecl BaseRectf_Width(rectf* pR);				// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形的高
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API float __cdecl BaseRectf_Height(rectf* pR);				// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断矩形是否为空（left = top = right = bottom = 0）
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BaseRectf_IsRectNull(rectf* pR);			// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断矩形范围是否为空（width <= 0 || heigth <= 0）（区别于IsRectNull）
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BaseRectf_IsRectEmpty(rectf* pR);			// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断点是否在矩形内
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BaseRectf_PtInRect(rectf* pR,				// 矩形指针
							  SYPoint2f point);			// 点

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩形相等判断
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BaseRectf_EqualRect(rectf* pR1,				// 矩形
							   rectf* pR2);				// 矩形

///////////////////////////////////////////////////////////////////////////////
// 描述：	拷贝矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API  void __cdecl BaseRectf_CopyRect(rectf* pDesR,			// 目标矩形指针
							  rectf* pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	扩张矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectf_InflateRect1(rectf* pR,			// 矩形指针
								  float x,				// 左、右外扩偏移量
								  float y);				// 上、下外扩偏移量

BASEGRAPH_COMMON_API void __cdecl BaseRectf_InflateRect2(rectf* pR,			// 矩形指针
								  SYSizef size);		// 外扩偏移量
								 
BASEGRAPH_COMMON_API void __cdecl BaseRectf_InflateRect3(rectf* pR,			// 矩形指针
								  float l,				// 左外扩偏移量
								  float t,				// 上外扩偏移量
								  float r,				// 右外扩偏移量
								  float b);				// 下外扩偏移量

///////////////////////////////////////////////////////////////////////////////
// 描述：	压缩矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectf_DeflateRect1(rectf* pR,			// 矩形指针
								  float x,				// 左、右内缩偏移量
								  float y);				// 上、下内缩偏移量

BASEGRAPH_COMMON_API void __cdecl BaseRectf_DeflateRect2(rectf* pR,			// 矩形指针
								  SYSizef size);		// 内缩偏移量

BASEGRAPH_COMMON_API void __cdecl BaseRectf_DeflateRect3(rectf* pR,			// 矩形指针
								  float l,				// 左内缩偏移量
								  float t,				// 上内缩偏移量
								  float r,				// 右内缩偏移量
								  float b);				// 下内缩偏移量

///////////////////////////////////////////////////////////////////////////////
// 描述：	规格化矩形（left < right && top < bottom）
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectf_NormalizeRect(rectf* pR);			// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	归一化矩形
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectf_NormalizeDRect(rectf* pDesR,		// 目标矩形
									rectf* pSrcR,		// 矩形指针
								    SYPoint2f point,			// 归一化中心点
								    float fWidth,			// 归一化宽
								    float fHeight);			// 归一化高

///////////////////////////////////////////////////////////////////////////////
// 描述：	还原已归一化的矩形
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectf_RevertDRect(rectf* pDesR,			// 目标矩形指针
								 rectf* pSrcR,			// 源矩形
								 SYPoint2f point,			// 还原中心点
								 float fWidth,				// 还原宽
								 float fHeight);			// 还原高

///////////////////////////////////////////////////////////////////////////////
// 描述：	在x和y方向上按给定值偏移
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectf_OffsetRect1(rectf* pR,			// 矩形指针
								 float x,				// 水平偏移量
								 float y);				// 垂直偏移量

BASEGRAPH_COMMON_API void __cdecl BaseRectf_OffsetRect2(rectf* pR,			// 矩形指针
								 SYPoint2f point);		// 偏移点

BASEGRAPH_COMMON_API void __cdecl BaseRectf_OffsetRect3(rectf* pR,			// 矩形指针
								 SYSizef size);			// 偏移大小

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形范围的大小
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectf_Size(LPCommonSIZEF pDesSz,			// 目标Size
						  rectf* pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形左上点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectf_TopLeft(LPCommonPOINTF pDesP,			// 目标点
							 rectf* pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形右下点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectf_BottomRight(LPCommonPOINTF pDesP,			// 目标点
								 rectf* pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形中心点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectf_CenterPoint(LPCommonPOINTF pDesP,			// 目标点
								 rectf* pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形拖动点的爪子，从左上角开始顺时针给出
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectf_DragPoint(LPBaseRect* ppDesP,			// 目标矩形数组
							   rectf* pSrcR,				// 源矩形
							   int nStyle,					// 爪子的风格
							   int nRadius);				// 方格半径

///////////////////////////////////////////////////////////////////////////////
// 描述：	求两个矩形的交集
//
// 返回值:	TRUE			交集不为空
//			FALSE			交集为空
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BaseRectf_IntersectRect(rectf* pDesR,			// 交集结果
								   rectf* pSrcR1,		// 源矩形
								   rectf* pSrcR2);		// 源矩形

///////////////////////////////////////////////////////////////////////////////
// 描述：	求两个矩形的并集
//
// 返回值:	TRUE			并集不为空
//			FALSE			并集为空
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BaseRectf_UnionRect(rectf* pDesR,			// 并集结果
							   rectf* pSrcR1,		// 源矩形
							   rectf* pSrcR2);		// 源矩形

BASEGRAPH_COMMON_API void __cdecl BaseRectf_UnionPoint(rectf* pDesR,			// 并集结果
							   rectf* pSrcR1,		// 源矩形
							   float fX,float fY);		// 源矩形

BASEGRAPH_COMMON_API void __cdecl BaseRectf_To_Rect(RECT* pDst,const rectf* pSrc);

// 双精度型实现

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectlf_SetRect(LPBaseRectLF pR,			// 矩形指针
							 double l,				// 左边位置
							 double t,				// 上边位置
							 double r,				// 右边位置
							 double b);				// 下边位置

///////////////////////////////////////////////////////////////////////////////
// 描述：	清空矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectlf_SetRectEmpty(LPBaseRectLF pR);		// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形的宽
//
// 返回值:	double
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API double __cdecl BaseRectlf_Width(LPBaseRectLF pR);				// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形的高
//
// 返回值:	double
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API double __cdecl BaseRectlf_Height(LPBaseRectLF pR);				// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断矩形是否为空（left = top = right = bottom = 0）
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BaseRectlf_IsRectNull(LPBaseRectLF pR);			// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断矩形范围是否为空（width <= 0 || heigth <= 0）（区别于IsRectNull）
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BaseRectlf_IsRectEmpty(LPBaseRectLF pR);			// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断点是否在矩形内
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BaseRectlf_PtInRect(LPBaseRectLF pR,				// 矩形指针
							  SYPoint2lf point);			// 点

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩形相等判断
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BaseRectlf_EqualRect(LPBaseRectLF pR1,				// 矩形
							   LPBaseRectLF pR2);				// 矩形

///////////////////////////////////////////////////////////////////////////////
// 描述：	拷贝矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectlf_CopyRect(LPBaseRectLF pDesR,			// 目标矩形指针
							  BaseRectlf srcR);				// 源矩形

///////////////////////////////////////////////////////////////////////////////
// 描述：	扩张矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectlf_InflateRect1(LPBaseRectLF pR,			// 矩形指针
								  double x,				// 左、右外扩偏移量
								  double y);			// 上、下外扩偏移量

BASEGRAPH_COMMON_API void __cdecl BaseRectlf_InflateRect2(LPBaseRectLF pR,			// 矩形指针
								  SYSizelf size);		// 外扩偏移量
								 
BASEGRAPH_COMMON_API void __cdecl BaseRectlf_InflateRect3(LPBaseRectLF pR,			// 矩形指针
								  double l,				// 左外扩偏移量
								  double t,				// 上外扩偏移量
								  double r,				// 右外扩偏移量
								  double b);			// 下外扩偏移量

///////////////////////////////////////////////////////////////////////////////
// 描述：	压缩矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectlf_DeflateRect1(LPBaseRectLF pR,			// 矩形指针
								  double x,				// 左、右内缩偏移量
								  double y);			// 上、下内缩偏移量

BASEGRAPH_COMMON_API void __cdecl BaseRectlf_DeflateRect2(LPBaseRectLF pR,			// 矩形指针
								  SYSizelf size);		// 内缩偏移量

BASEGRAPH_COMMON_API void __cdecl BaseRectlf_DeflateRect3(LPBaseRectLF pR,			// 矩形指针
								  double l,				// 左内缩偏移量
								  double t,				// 上内缩偏移量
								  double r,				// 右内缩偏移量
								  double b);			// 下内缩偏移量

///////////////////////////////////////////////////////////////////////////////
// 描述：	规格化矩形（left < right && top < bottom）
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectlf_NormalizeRect(LPBaseRectLF pR);			// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	归一化矩形
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectlf_NormalizeDRect(rectf* pDesR,		// 目标矩形
									LPBaseRectLF pSrcR,		// 矩形指针
								    SYPoint2lf point,			// 归一化中心点
								    double dWidth,			// 归一化宽
								    double dHeight);		// 归一化高

///////////////////////////////////////////////////////////////////////////////
// 描述：	还原已归一化的矩形
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectlf_RevertDRect(LPBaseRectLF pDesR,			// 目标矩形指针
								 rectf* pSrcR,			// 源矩形
								 SYPoint2lf point,			// 还原中心点
								 double dWidth,				// 还原宽
								 double dHeight);			// 还原高

///////////////////////////////////////////////////////////////////////////////
// 描述：	在x和y方向上按给定值偏移
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectlf_OffsetRect1(LPBaseRectLF pR,			// 矩形指针
								 double x,				// 水平偏移量
								 double y);				// 垂直偏移量

BASEGRAPH_COMMON_API void __cdecl BaseRectlf_OffsetRect2(LPBaseRectLF pR,			// 矩形指针
								 SYPoint2lf point);		// 偏移点

BASEGRAPH_COMMON_API void __cdecl BaseRectlf_OffsetRect3(LPBaseRectLF pR,			// 矩形指针
								 SYSizelf size);			// 偏移大小

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形范围的大小
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectlf_Size(LPCommonSIZELF pDesSz,			// 目标Size
						  LPBaseRectLF pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形左上点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectlf_TopLeft(LPCommonPOINTD pDesP,			// 目标点
							 LPBaseRectLF pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形右下点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectlf_BottomRight(LPCommonPOINTD pDesP,			// 目标点
								 LPBaseRectLF pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形中心点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectlf_CenterPoint(LPCommonPOINTD pDesP,			// 目标点
								 LPBaseRectLF pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形拖动点的爪子，从左上角开始顺时针给出
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseRectlf_DragPoint(LPBaseRect* ppDesP,			// 目标矩形数组
							   LPBaseRectLF pSrcR,				// 源矩形
							   int nStyle,					// 爪子的风格
							   int nRadius);				// 方格半径

///////////////////////////////////////////////////////////////////////////////
// 描述：	求两个矩形的交集
//
// 返回值:	TRUE			交集不为空
//			FALSE			交集为空
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BaseRectlf_IntersectRect(LPBaseRectLF pDesR,			// 交集结果
								   LPBaseRectLF pSrcR1,		// 源矩形
								   LPBaseRectLF pSrcR2);		// 源矩形

///////////////////////////////////////////////////////////////////////////////
// 描述：	求两个矩形的并集
//
// 返回值:	TRUE			并集不为空
//			FALSE			并集为空
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BaseRectlf_UnionRect(LPBaseRectLF pDesR,			// 并集结果
							   LPBaseRectLF pSrcR1,		// 源矩形
							   LPBaseRectLF pSrcR2);		// 源矩形

///////////////////////////// End of BaseRect ///////////////////////////////////

void __cdecl rectf_scale(rectf* prect,float fscale);
//////////////////////////////////////////////////////////////////////////
// C++ 代码

#ifdef __cplusplus

//////////////////////////////////////////////////////////////////////////
// 结构操作符重载

//////////////////////////////////////////////////////////////////////////
// Point

// 整型实现

inline BOOL operator == (SYPoint& p1, SYPoint& p2)
{
	return (p1.x == p2.x && p1.y == p2.y);
}

inline BOOL operator != (SYPoint& p1, SYPoint& p2)
{
	return (p1.x != p2.x || p1.y != p2.y);
}

inline SYPoint operator + (SYPoint& p1, SYPoint& p2)
{
	SYPoint p3;

	p3.x = p1.x + p2.x;
	p3.y = p1.y + p2.y;

	return p3;
}

inline SYPoint& operator += (SYPoint& p1, SYPoint& p2)
{
	p1.x += p2.x;
	p1.y += p2.y;

	return p1;
}

inline SYPoint operator + (SYPoint& p1, LONG k)
{
	SYPoint p3;

	p3.x = p1.x + k;
	p3.y = p1.y + k;

	return p3;
}

inline SYPoint& operator += (SYPoint& p1, LONG k)
{
	p1.x += k;
	p1.y += k;

	return p1;
}

inline SYPoint operator + (SYPoint& p1, SYSizeL& s1)
{
	SYPoint p3;

	p3.x = p1.x + s1.cx;
	p3.y = p1.y + s1.cy;

	return p3;
}

inline SYPoint& operator += (SYPoint& p1, SYSizeL& s1)
{
	p1.x += s1.cx;
	p1.y += s1.cy;

	return p1;
}

inline SYPoint operator - (SYPoint& p1, SYPoint& p2)
{
	SYPoint p3;

	p3.x = p1.x - p2.x;
	p3.y = p1.y - p2.y;

	return p3;
}

inline SYPoint& operator -= (SYPoint& p1, SYPoint& p2)
{
	p1.x -= p2.x;
	p1.y -= p2.y;

	return p1;
}

inline SYPoint operator - (SYPoint& p1, LONG k)
{
	SYPoint p3;

	p3.x = p1.x - k;
	p3.y = p1.y - k;

	return p3;
}

inline SYPoint& operator -= (SYPoint& p1, LONG k)
{
	p1.x -= k;
	p1.y -= k;

	return p1;
}

inline SYPoint operator - (SYPoint& p1, SYSizeL& s1)
{
	SYPoint p3;

	p3.x = p1.x - s1.cx;
	p3.y = p1.y - s1.cy;

	return p3;
}

inline SYPoint& operator -= (SYPoint& p1, SYSizeL& s1)
{
	p1.x -= s1.cx;
	p1.y -= s1.cy;

	return p1;
}

inline SYPoint operator * (SYPoint& p1, LONG k)
{
	SYPoint p3;

	p3.x = p1.x * k;
	p3.y = p1.y * k;

	return p3;
}

inline SYPoint& operator *= (SYPoint& p1, LONG k)
{
	p1.x *= k;
	p1.y *= k;

	return p1;
}

inline SYPoint operator / (SYPoint& p1, LONG k)
{
	SYPoint p3;
	double kTmp = 1.0 / (double)k;

	p3.x = (LONG)(p1.x * kTmp);
	p3.y = (LONG)(p1.y * kTmp);

	return p3;
}

inline SYPoint& operator /= (SYPoint& p1, LONG k)
{
	double kTmp = 1.0 / (double)k;

	p1.x = (LONG)(p1.x * kTmp);
	p1.y = (LONG)(p1.y * kTmp);

	return p1;
}

// 单精度实现

inline BOOL operator == (SYPoint2f& p1, SYPoint2f& p2)
{
	return (p1.x == p2.x && p1.y == p2.y);
}

inline BOOL operator != (SYPoint2f& p1, SYPoint2f& p2)
{
	return (p1.x != p2.x || p1.y != p2.y);
}

inline SYPoint2f operator + (SYPoint2f& p1, SYPoint2f& p2)
{
	SYPoint2f p3;

	p3.x = p1.x + p2.x;
	p3.y = p1.y + p2.y;

	return p3;
}

inline SYPoint2f& operator += (SYPoint2f& p1, SYPoint2f& p2)
{
	p1.x += p2.x;
	p1.y += p2.y;

	return p1;
}

inline SYPoint2f operator + (SYPoint2f& p1, float k)
{
	SYPoint2f p3;

	p3.x = p1.x + k;
	p3.y = p1.y + k;

	return p3;
}

inline SYPoint2f& operator += (SYPoint2f& p1, float k)
{
	p1.x += k;
	p1.y += k;

	return p1;
}

inline SYPoint2f operator + (SYPoint2f& p1, SYSizef& s1)
{
	SYPoint2f p3;

	p3.x = p1.x + s1.cx;
	p3.y = p1.y + s1.cy;

	return p3;
}

inline SYPoint2f& operator += (SYPoint2f& p1, SYSizef& s1)
{
	p1.x += s1.cx;
	p1.y += s1.cy;

	return p1;
}

inline SYPoint2f operator - (SYPoint2f& p1, SYPoint2f& p2)
{
	SYPoint2f p3;

	p3.x = p1.x - p2.x;
	p3.y = p1.y - p2.y;

	return p3;
}

inline SYPoint2f& operator -= (SYPoint2f& p1, SYPoint2f& p2)
{
	p1.x -= p2.x;
	p1.y -= p2.y;

	return p1;
}

inline SYPoint2f operator - (SYPoint2f& p1, float k)
{
	SYPoint2f p3;

	p3.x = p1.x - k;
	p3.y = p1.y - k;

	return p3;
}

inline SYPoint2f& operator -= (SYPoint2f& p1, float k)
{
	p1.x -= k;
	p1.y -= k;

	return p1;
}

inline SYPoint2f operator - (SYPoint2f& p1, SYSizef& s1)
{
	SYPoint2f p3;

	p3.x = p1.x - s1.cx;
	p3.y = p1.y - s1.cy;

	return p3;
}

inline SYPoint2f& operator -= (SYPoint2f& p1, SYSizef& s1)
{
	p1.x -= s1.cx;
	p1.y -= s1.cy;

	return p1;
}

inline SYPoint2f operator * (SYPoint2f& p1, float k)
{
	SYPoint2f p3;

	p3.x = p1.x * k;
	p3.y = p1.y * k;

	return p3;
}

inline SYPoint2f operator / (SYPoint2f& p1, float k)
{
	SYPoint2f p3;
	float kTmp = 1.f / k;

	p3.x = p1.x * kTmp;
	p3.y = p1.y * kTmp;

	return p3;
}

// 双精度实现

inline BOOL operator == (SYPoint2lf& p1, SYPoint2lf& p2)
{
	return (p1.x == p2.x && p1.y == p2.y);
}

inline BOOL operator != (SYPoint2lf& p1, SYPoint2lf& p2)
{
	return (p1.x != p2.x || p1.y != p2.y);
}

inline SYPoint2lf operator + (SYPoint2lf& p1, SYPoint2lf& p2)
{
	SYPoint2lf p3;

	p3.x = p1.x + p2.x;
	p3.y = p1.y + p2.y;

	return p3;
}

inline SYPoint2lf& operator += (SYPoint2lf& p1, SYPoint2lf& p2)
{
	p1.x += p2.x;
	p1.y += p2.y;

	return p1;
}

inline SYPoint2lf operator + (SYPoint2lf& p1, double k)
{
	SYPoint2lf p3;

	p3.x = p1.x + k;
	p3.y = p1.y + k;

	return p3;
}

inline SYPoint2lf& operator += (SYPoint2lf& p1, double k)
{
	p1.x += k;
	p1.y += k;

	return p1;
}

inline SYPoint2lf operator + (SYPoint2lf& p1, SYSizelf& s1)
{
	SYPoint2lf p3;

	p3.x = p1.x + s1.cx;
	p3.y = p1.y + s1.cy;

	return p3;
}

inline SYPoint2lf& operator += (SYPoint2lf& p1, SYSizelf& s1)
{
	p1.x += s1.cx;
	p1.y += s1.cy;

	return p1;
}

inline SYPoint2lf operator - (SYPoint2lf& p1, SYPoint2lf& p2)
{
	SYPoint2lf p3;

	p3.x = p1.x - p2.x;
	p3.y = p1.y - p2.y;

	return p3;
}

inline SYPoint2lf& operator -= (SYPoint2lf& p1, SYPoint2lf& p2)
{
	p1.x -= p2.x;
	p1.y -= p2.y;

	return p1;
}

inline SYPoint2lf operator - (SYPoint2lf& p1, double k)
{
	SYPoint2lf p3;

	p3.x = p1.x - k;
	p3.y = p1.y - k;

	return p3;
}

inline SYPoint2lf& operator -= (SYPoint2lf& p1, double k)
{
	p1.x -= k;
	p1.y -= k;

	return p1;
}

inline SYPoint2lf operator - (SYPoint2lf& p1, SYSizelf& s1)
{
	SYPoint2lf p3;

	p3.x = p1.x - s1.cx;
	p3.y = p1.y - s1.cy;

	return p3;
}

inline SYPoint2lf& operator -= (SYPoint2lf& p1, SYSizelf& s1)
{
	p1.x -= s1.cx;
	p1.y -= s1.cy;

	return p1;
}

inline SYPoint2lf operator * (SYPoint2lf& p1, double k)
{
	SYPoint2lf p3;

	p3.x = p1.x * k;
	p3.y = p1.y * k;

	return p3;
}

inline SYPoint2lf& operator *= (SYPoint2lf& p1, double k)
{
	p1.x *= k;
	p1.y *= k;

	return p1;
}

inline SYPoint2lf operator / (SYPoint2lf& p1, double k)
{
	SYPoint2lf p3;
	double kTmp = 1.0 / k;

	p3.x = p1.x * kTmp;
	p3.y = p1.y * kTmp;

	return p3;
}

inline SYPoint2lf& operator /= (SYPoint2lf& p1, double k)
{
	double kTmp = 1.0 / k;

	p1.x *= kTmp;
	p1.y *= kTmp;

	return p1;
}

//////////////////////////////////////////////////////////////////////////
// Size

//////////////////////////////////////////////////////////////////////////
// Rect

// 整型实现

inline BaseRect operator & (BaseRect& r1, BaseRect& r2)
{
	BaseRect r3;

	BaseRect_IntersectRect(&r3, &r1, &r2);

	return r3;
}

inline BaseRect& operator &= (BaseRect& r1, BaseRect& r2)
{
	BaseRect r3;

	BaseRect_IntersectRect(&r3, &r1, &r2);
	BaseRect_SetRect(&r1, r3.left, r3.top, r3.right, r3.bottom);

	return r1;
}

inline BaseRect operator | (BaseRect& r1, BaseRect& r2)
{
	BaseRect r3;

	BaseRect_UnionRect(&r3, &r1, &r2);

	return r3;
}

inline BaseRect& operator |= (BaseRect& r1, BaseRect& r2)
{
	BaseRect r3;

	BaseRect_UnionRect(&r3, &r1, &r2);
	BaseRect_SetRect(&r1, r3.left, r3.top, r3.right, r3.bottom);

	return r1;
}

// 单精度型实现

inline rectf operator & (rectf& r1, rectf& r2)
{
	rectf r3;

	BaseRectf_IntersectRect(&r3, &r1, &r2);

	return r3;
}

inline rectf& operator &= (rectf& r1, rectf& r2)
{
	rectf r3;

	BaseRectf_IntersectRect(&r3, &r1, &r2);
	BaseRectf_SetRect(&r1, r3.left, r3.top, r3.right, r3.bottom);

	return r1;
}

inline rectf operator | (rectf& r1, rectf& r2)
{
	rectf r3;

	BaseRectf_UnionRect(&r3, &r1, &r2);

	return r3;
}

inline rectf& operator |= (rectf& r1, rectf& r2)
{
	rectf r3;

	BaseRectf_UnionRect(&r3, &r1, &r2);
	BaseRectf_SetRect(&r1, r3.left, r3.top, r3.right, r3.bottom);

	return r1;
}

// 双精度型实现

inline BaseRectlf operator & (BaseRectlf& r1, BaseRectlf& r2)
{
	BaseRectlf r3;

	BaseRectlf_IntersectRect(&r3, &r1, &r2);

	return r3;
}

inline BaseRectlf& operator &= (BaseRectlf& r1, BaseRectlf& r2)
{
	BaseRectlf r3;

	BaseRectlf_IntersectRect(&r3, &r1, &r2);
	BaseRectlf_SetRect(&r1, r3.left, r3.top, r3.right, r3.bottom);

	return r1;
}

inline BaseRectlf operator | (BaseRectlf& r1, BaseRectlf& r2)
{
	BaseRectlf r3;

	BaseRectlf_UnionRect(&r3, &r1, &r2);

	return r3;
}

inline BaseRectlf& operator |= (BaseRectlf& r1, BaseRectlf& r2)
{
	BaseRectlf r3;

	BaseRectlf_UnionRect(&r3, &r1, &r2);
	BaseRectlf_SetRect(&r1, r3.left, r3.top, r3.right, r3.bottom);

	return r1;
}


///////////////////////////////// 类封装 //////////////////////////////////////

// 声明用到的类
template <class TYPE, class ARG_TYPE> class CBasePoint;
template <class TYPE, class ARG_TYPE> class CBaseSize;
template <class TYPE, class ARG_TYPE> class CBaseRect;

//////////////////////////////// 类的定义 /////////////////////////////////////

/////////////////////////////// CBasePoint //////////////////////////////////////

template <class TYPE, class ARG_TYPE = const TYPE&>
class CBasePoint
{
public:
	TYPE	x;
	TYPE	y;

public:
	// 空值创建
	CBasePoint();
	// 初始化创建
	CBasePoint(ARG_TYPE X, ARG_TYPE Y);
	// 拷贝创建
	CBasePoint(const CBasePoint& point);
	// 取 CBaseSize 值创建
	CBasePoint(const CBaseSize<TYPE, ARG_TYPE>& size);
	CBasePoint(CBaseSize<TYPE, ARG_TYPE> size);
	// 取结构值创建
	CBasePoint(const SYPoint& pointL);
	CBasePoint(const SYPoint2f& pointF);
	CBasePoint(const SYPoint2lf& pointD);

	// 按指定偏移量移动
	void Offset(ARG_TYPE xOffset, ARG_TYPE yOffset);
	void Offset(const CBasePoint& point);
	void Offset(CBasePoint point);
	void Offset(const CBaseSize<TYPE, ARG_TYPE>& size);
	void Offset(CBaseSize<TYPE, ARG_TYPE> size);
	// 设值
	void SetPoint(ARG_TYPE X, ARG_TYPE Y);
	// 拷贝点值
	void CopyPoint(const CBasePoint& point);
	void CopyPoint(CBasePoint point);

	CBasePoint& operator=(const CBasePoint<TYPE, ARG_TYPE>& point);
	CBasePoint& operator=(const SYPoint& pointL);
	CBasePoint& operator=(SYPoint pointL);
	CBasePoint& operator=(const SYPoint2f& pointL);
	CBasePoint& operator=(SYPoint2f pointL);
	CBasePoint& operator=(const SYPoint2lf& pointL);
	CBasePoint& operator=(SYPoint2lf pointL);

	BOOL operator==(const CBasePoint& point);
	BOOL operator!=(const CBasePoint& point);
	BOOL operator==(CBasePoint point);
	BOOL operator!=(CBasePoint point);

	CBasePoint& operator+=(const CBaseSize<TYPE, ARG_TYPE>& size);
	CBasePoint& operator-=(const CBaseSize<TYPE, ARG_TYPE>& size);
	CBasePoint& operator+=(const CBasePoint& point);
	CBasePoint& operator-=(const CBasePoint& point);
	CBasePoint& operator+=(CBaseSize<TYPE, ARG_TYPE> size);
	CBasePoint& operator-=(CBaseSize<TYPE, ARG_TYPE> size);
	CBasePoint& operator+=(CBasePoint point);
	CBasePoint& operator-=(CBasePoint point);
	CBasePoint& operator+=(const SYPoint& pointL);
	CBasePoint& operator-=(const SYPoint& pointL);
	CBasePoint& operator+=(const SYPoint2f& pointF);
	CBasePoint& operator-=(const SYPoint2f& pointF);
	CBasePoint& operator+=(const SYPoint2lf& pointD);
	CBasePoint& operator-=(const SYPoint2lf& pointD);
	CBasePoint& operator+=(SYPoint pointL);
	CBasePoint& operator-=(SYPoint pointL);
	CBasePoint& operator+=(SYPoint2f pointF);
	CBasePoint& operator-=(SYPoint2f pointF);
	CBasePoint& operator+=(SYPoint2lf pointD);
	CBasePoint& operator-=(SYPoint2lf pointD);

// Operators returning CBasePoint values
	CBasePoint operator+(const CBaseSize<TYPE, ARG_TYPE>& size);
	CBasePoint operator-(const CBaseSize<TYPE, ARG_TYPE>& size);
	CBasePoint operator+(CBaseSize<TYPE, ARG_TYPE> size);
	CBasePoint operator-(CBaseSize<TYPE, ARG_TYPE> size);
	CBasePoint operator-();
	CBasePoint operator+(const CBasePoint& point);
	CBasePoint operator+(CBasePoint point);
	CBasePoint operator+(const SYPoint& pointL);
	CBasePoint operator+(const SYPoint2f& pointF);
	CBasePoint operator+(const SYPoint2lf& pointD);
	CBasePoint operator+(SYPoint pointL);
	CBasePoint operator+(SYPoint2f pointF);
	CBasePoint operator+(SYPoint2lf pointD);

// Operators returning CBaseSize values
	CBaseSize<TYPE, ARG_TYPE> operator-(const CBasePoint& point);
	CBaseSize<TYPE, ARG_TYPE> operator-(CBasePoint point);
	CBaseSize<TYPE, ARG_TYPE> operator-(const SYPoint& pointL);
	CBaseSize<TYPE, ARG_TYPE> operator-(const SYPoint2f& pointF);
	CBaseSize<TYPE, ARG_TYPE> operator-(const SYPoint2lf& pointD);
	CBaseSize<TYPE, ARG_TYPE> operator-(SYPoint pointL);
	CBaseSize<TYPE, ARG_TYPE> operator-(SYPoint2f pointF);
	CBaseSize<TYPE, ARG_TYPE> operator-(SYPoint2lf pointD);
};

//////////////////////////// End of CBasePoint //////////////////////////////////


//////////////////////////////// CBaseSize //////////////////////////////////////

template <class TYPE, class ARG_TYPE = const TYPE&>
class CBaseSize
{
public:
	TYPE	cx;
	TYPE	cy;

public:
	// 空值创建
	CBaseSize();
	// 初始化创建
	CBaseSize(ARG_TYPE CX, ARG_TYPE CY);
	// 拷贝创建
	CBaseSize(const CBaseSize& size);
	// 从点创建
	CBaseSize(const CBasePoint<TYPE, ARG_TYPE>& point);
	CBaseSize(CBasePoint<TYPE, ARG_TYPE> point);
	// 从结构创建
	CBaseSize(const SYSizeL& sizeL);
	CBaseSize(const SYSizef& sizeF);
	CBaseSize(const SYSizelf& sizeD);

	// 设值
	void SetSize(ARG_TYPE CX, ARG_TYPE CY);
	// 拷贝值
	void CopySize(const CBaseSize& size);
	void CopySize(CBaseSize size);

	CBaseSize& operator=(const CBaseSize<TYPE, ARG_TYPE>& size);
	CBaseSize& operator=(const SYSizeL& sizeL);
	CBaseSize& operator=(SYSizeL sizeL);
	CBaseSize& operator=(const SYSizef& sizeF);
	CBaseSize& operator=(SYSizef sizeF);
	CBaseSize& operator=(const SYSizelf& sizeD);
	CBaseSize& operator=(SYSizelf sizeD);

	BOOL operator==(const CBaseSize& size);
	BOOL operator!=(const CBaseSize& size);
	BOOL operator==(CBaseSize size);
	BOOL operator!=(CBaseSize size);

	CBaseSize& operator+=(const CBaseSize& size);
	CBaseSize& operator-=(const CBaseSize& size);
	CBaseSize& operator+=(CBaseSize size);
	CBaseSize& operator-=(CBaseSize size);
	CBaseSize& operator+=(const SYSizeL& sizeL);
	CBaseSize& operator-=(const SYSizeL& sizeL);
	CBaseSize& operator+=(const SYSizef& sizeF);
	CBaseSize& operator-=(const SYSizef& sizeF);
	CBaseSize& operator+=(const SYSizelf& sizeD);
	CBaseSize& operator-=(const SYSizelf& sizeD);
	CBaseSize& operator+=(SYSizeL sizeL);
	CBaseSize& operator-=(SYSizeL sizeL);
	CBaseSize& operator+=(SYSizef sizeF);
	CBaseSize& operator-=(SYSizef sizeF);
	CBaseSize& operator+=(SYSizelf sizeD);
	CBaseSize& operator-=(SYSizelf sizeD);

// Operators returning CBaseSize values
	CBaseSize operator+(const CBaseSize& size);
	CBaseSize operator-(const CBaseSize& size);
	CBaseSize operator+(CBaseSize size);
	CBaseSize operator-(CBaseSize size);
	CBaseSize operator-();
	CBaseSize operator+(const SYSizeL& sizeL);
	CBaseSize operator-(const SYSizeL& sizeL);
	CBaseSize operator+(const SYSizef& sizeF);
	CBaseSize operator-(const SYSizef& sizeF);
	CBaseSize operator+(const SYSizelf& sizeD);
	CBaseSize operator-(const SYSizelf& sizeD);
	CBaseSize operator+(SYSizeL sizeL);
	CBaseSize operator-(SYSizeL sizeL);
	CBaseSize operator+(SYSizef sizeF);
	CBaseSize operator-(SYSizef sizeF);
	CBaseSize operator+(SYSizelf sizeD);
	CBaseSize operator-(SYSizelf sizeD);

// Operators returning CBasePoint values
	CBasePoint<TYPE, ARG_TYPE> operator+(const CBasePoint<TYPE, ARG_TYPE>& point);
	CBasePoint<TYPE, ARG_TYPE> operator-(const CBasePoint<TYPE, ARG_TYPE>& point);
	CBasePoint<TYPE, ARG_TYPE> operator+(CBasePoint<TYPE, ARG_TYPE> point);
	CBasePoint<TYPE, ARG_TYPE> operator-(CBasePoint<TYPE, ARG_TYPE> point);
};

///////////////////////////// End of CBaseSize //////////////////////////////////

#endif

#endif		// #ifndef __Common_PLANE_INC__
//------------------------------- End of File ---------------------------------