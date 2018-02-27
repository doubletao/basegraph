// Header FileName:	Common\PlaneDef.h
// CPP FileName:	SYPlane.cpp

#ifndef __SY_PLANE_INC__
#define __SY_PLANE_INC__

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
void __cdecl SYPoint_SetPoint(LPSYPOINTL pP,			// 点指针
							   LONG x,					// x值
							   LONG y);					// y值

void __cdecl SYPoint2f_SetPoint(LPSYPOINTF pP,			// 点指针
							   float x,					// x值
							   float y);				// y值

void __cdecl SYPoint2lf_SetPoint(LPSYPOINTD pP,			// 点指针
							   double x,				// x值
							   double y);				// y值

///////////////////////////////////////////////////////////////////////////////
// 描述：	按给定值偏移
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYPoint_Offset(LPSYPOINTL pP,			// 点指针
							 LONG xOffset,			// x偏移量
							 LONG yOffset);			// y偏移量

void __cdecl SYPoint2f_Offset(LPSYPOINTF pP,			// 点指针
							 float xOffset,			// x偏移量
							 float yOffset);		// y偏移量

void __cdecl SYPoint2lf_Offset(LPSYPOINTD pP,			// 点指针
							 double xOffset,		// x偏移量
							 double yOffset);		// y偏移量




///////////////////////////// End of SYPoint //////////////////////////////////


///////////////////////////////// SYSize //////////////////////////////////////

typedef struct tag_SYSizeL{		// 整型
	LONG	cx;
	LONG	cy;
}SYSizeL, *LPSYSIZE;

typedef struct tag_SYSizef{		// 浮点型
	float	cx;
	float	cy;
}SYSizef, *LPSYSIZEF;

typedef struct tag_SYSizelf{		// 双精度型
	double	cx;
	double	cy;
}SYSizelf, *LPSYSIZELF;

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置size值
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYSize_SetSize(LPSYSIZE pS,			// size指针
							 LONG cx,				// cx值
							 LONG cy);				// cy值

void __cdecl SYSizef_SetSize(LPSYSIZEF pS,			// size指针
							 float cx,					// cx值
							 float cy);				// cy值

void __cdecl SYSizelf_SetSize(LPSYSIZELF pS,			// size指针
							 double cx,				// cx值
							 double cy);			// cy值

///////////////////////////// End of SYSize ///////////////////////////////////


///////////////////////////////// SYRect //////////////////////////////////////

#define SYRECT_DRAG_COUNT	8		// 拖动点的数量

#define SYRECT_DRAG_LTP		0		// 左上点
#define SYRECT_DRAG_TMP		1		// 上中点
#define SYRECT_DRAG_RTP		2		// 右上点
#define SYRECT_DRAG_RMP		3		// 右中点
#define SYRECT_DRAG_RBP		4		// 右下点
#define SYRECT_DRAG_BMP		5		// 下中点
#define SYRECT_DRAG_LBP		6		// 左下点
#define SYRECT_DRAG_LMP		7		// 左中点

#define SYRECT_DRAG_ONLINE	1		// 拖动点的风格：方格在线上
#define SYRECT_DRAG_INLINE	2		// 拖动点的风格：方格在线内


typedef struct tag_SYRectL{		// 整型
	LONG	left;
	LONG	top;
	LONG	right;
	LONG	bottom;
}SYRect, *LPSYRECT;

typedef struct tag_SYRectF{		// 浮点型
	float	left;
	float	top;
	float	right;
	float	bottom;
}rectf, SYRectF,*LPSYRECTF;

typedef struct tag_SYRectlf{		// 双精度型
	double	left;
	double	top;
	double	right;
	double	bottom;
}SYRectlf,rectlf, *LPSYRECTLF;

// 整型实现

///////////////////////////////////////////////////////////////////////////////
// 描述：	通过点和半径设置矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRect_SetRectBPR(LPSYRECT pR,			// 矩形指针
								POINT point,			// 中心点
								int nRadius);			// 半径

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRect_SetRect(LPSYRECT pR,			// 矩形指针
							 LONG l,				// 左边位置
							 LONG t,				// 上边位置
							 LONG r,				// 右边位置
							 LONG b);				// 下边位置

///////////////////////////////////////////////////////////////////////////////
// 描述：	清空矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRect_SetRectEmpty(LPSYRECT pR);		// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形的宽
//
// 返回值:	LONG
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API LONG __cdecl SYRect_Width(LPSYRECT pR);				// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形的高
//
// 返回值:	LONG
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API LONG __cdecl SYRect_Height(LPSYRECT pR);				// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断矩形是否为空（left = top = right = bottom = 0）
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl SYRect_IsRectNull(LPSYRECT pR);			// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断矩形范围是否为空（width <= 0 || heigth <= 0）（区别于IsRectNull）
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl SYRect_IsRectEmpty(LPSYRECT pR);			// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断点是否在矩形内
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl SYRect_PtInRect(LPSYRECT pR,				// 矩形指针
							  SYPoint point);			// 点

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩形相等判断
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl SYRect_EqualRect(LPSYRECT pR1,				// 矩形
							   LPSYRECT pR2);			// 矩形

///////////////////////////////////////////////////////////////////////////////
// 描述：	拷贝矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRect_CopyRect(LPSYRECT pDesR,			// 目标矩形指针
							  LPSYRECT srcR);			// 源矩形

///////////////////////////////////////////////////////////////////////////////
// 描述：	扩张矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRect_InflateRect1(LPSYRECT pR,			// 矩形指针
								  int x,				// 左、右外扩偏移量
								  int y);				// 上、下外扩偏移量

BASEGRAPH_COMMON_API void __cdecl SYRect_InflateRect2(LPSYRECT pR,			// 矩形指针
								  SYSizeL size);		// 外扩偏移量
								 
BASEGRAPH_COMMON_API void __cdecl SYRect_InflateRect3(LPSYRECT pR,			// 矩形指针
								  int l,				// 左外扩偏移量
								  int t,				// 上外扩偏移量
								  int r,				// 右外扩偏移量
								  int b);				// 下外扩偏移量

///////////////////////////////////////////////////////////////////////////////
// 描述：	压缩矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRect_DeflateRect1(LPSYRECT pR,			// 矩形指针
								  int x,				// 左、右内缩偏移量
								  int y);				// 上、下内缩偏移量

BASEGRAPH_COMMON_API void __cdecl SYRect_DeflateRect2(LPSYRECT pR,			// 矩形指针
								  SYSizeL size);		// 内缩偏移量

BASEGRAPH_COMMON_API void __cdecl SYRect_DeflateRect3(LPSYRECT pR,			// 矩形指针
								  int l,				// 左内缩偏移量
								  int t,				// 上内缩偏移量
								  int r,				// 右内缩偏移量
								  int b);				// 下内缩偏移量

///////////////////////////////////////////////////////////////////////////////
// 描述：	规格化矩形（left < right && top < bottom）
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRect_NormalizeRect(LPSYRECT pR);			// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	归一化矩形
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRect_NormalizeDRect(rectf* pDesR,		// 目标矩形
									LPSYRECT pSrcR,		// 矩形指针
								    SYPoint point,			// 归一化中心点
								    int nWidth,				// 归一化宽
								    int nHeight);			// 归一化高

///////////////////////////////////////////////////////////////////////////////
// 描述：	还原已归一化的矩形
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRect_RevertDRect(LPSYRECT pDesR,			// 目标矩形指针
								 rectf* pSrcR,			// 源矩形
								 SYPoint point,			// 还原中心点
								 int nWidth,				// 还原宽
								 int nHeight);				// 还原高

///////////////////////////////////////////////////////////////////////////////
// 描述：	在x和y方向上按给定值偏移
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRect_OffsetRect1(LPSYRECT pR,			// 矩形指针
								 int x,					// 水平偏移量
								 int y);				// 垂直偏移量

BASEGRAPH_COMMON_API void __cdecl SYRect_OffsetRect2(LPSYRECT pR,			// 矩形指针
								 SYPoint point);		// 偏移点

BASEGRAPH_COMMON_API void __cdecl SYRect_OffsetRect3(LPSYRECT pR,			// 矩形指针
								 SYSizeL size);			// 偏移大小

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形范围的大小
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRect_Size(LPSYSIZE pDesSz,			// 目标Size
						  LPSYRECT pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形左上点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRect_TopLeft(LPSYPOINTL pDesP,			// 目标点
							 LPSYRECT pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形右下点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRect_BottomRight(LPSYPOINTL pDesP,			// 目标点
								 LPSYRECT pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形中心点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRect_CenterPoint(LPSYPOINTL pDesP,			// 目标点
								 LPSYRECT pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形拖动点的爪子，从左上角开始顺时针给出
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRect_DragPoint(LPSYRECT* ppDesR,			// 目标矩形数组
							   LPSYRECT pSrcR,				// 源矩形
							   int nStyle,					// 爪子的风格
							   int nRadius);				// 方格半径

///////////////////////////////////////////////////////////////////////////////
// 描述：	求两个矩形的交集
//
// 返回值:	TRUE			交集不为空
//			FALSE			交集为空
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl SYRect_IntersectRect(LPSYRECT pDesR,			// 交集结果
								   LPSYRECT pSrcR1,		// 源矩形
								   LPSYRECT pSrcR2);		// 源矩形

///////////////////////////////////////////////////////////////////////////////
// 描述：	求两个矩形的并集
//
// 返回值:	TRUE			并集不为空
//			FALSE			并集为空
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl SYRect_UnionRect(LPSYRECT pDesR,			// 并集结果
							   LPSYRECT pSrcR1,		// 源矩形
							   LPSYRECT pSrcR2);		// 源矩形


// 浮点型实现
BASEGRAPH_COMMON_API void __cdecl SYRectf_Scale(rectf* rt,float fscale);
///////////////////////////////////////////////////////////////////////////////
// 描述：	设置矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectf_SetRect(rectf* pR,			// 矩形指针
							 float l,				// 左边位置
							 float t,				// 上边位置
							 float r,				// 右边位置
							 float b);				// 下边位置

///////////////////////////////////////////////////////////////////////////////
// 描述：	清空矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectf_SetRectEmpty(rectf* pR);		// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形的宽
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API float __cdecl SYRectf_Width(rectf* pR);				// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形的高
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API float __cdecl SYRectf_Height(rectf* pR);				// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断矩形是否为空（left = top = right = bottom = 0）
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl SYRectf_IsRectNull(rectf* pR);			// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断矩形范围是否为空（width <= 0 || heigth <= 0）（区别于IsRectNull）
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl SYRectf_IsRectEmpty(rectf* pR);			// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断点是否在矩形内
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl SYRectf_PtInRect(rectf* pR,				// 矩形指针
							  SYPoint2f point);			// 点

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩形相等判断
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl SYRectf_EqualRect(rectf* pR1,				// 矩形
							   rectf* pR2);				// 矩形

///////////////////////////////////////////////////////////////////////////////
// 描述：	拷贝矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API  void __cdecl SYRectf_CopyRect(rectf* pDesR,			// 目标矩形指针
							  rectf* pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	扩张矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectf_InflateRect1(rectf* pR,			// 矩形指针
								  float x,				// 左、右外扩偏移量
								  float y);				// 上、下外扩偏移量

BASEGRAPH_COMMON_API void __cdecl SYRectf_InflateRect2(rectf* pR,			// 矩形指针
								  SYSizef size);		// 外扩偏移量
								 
BASEGRAPH_COMMON_API void __cdecl SYRectf_InflateRect3(rectf* pR,			// 矩形指针
								  float l,				// 左外扩偏移量
								  float t,				// 上外扩偏移量
								  float r,				// 右外扩偏移量
								  float b);				// 下外扩偏移量

///////////////////////////////////////////////////////////////////////////////
// 描述：	压缩矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectf_DeflateRect1(rectf* pR,			// 矩形指针
								  float x,				// 左、右内缩偏移量
								  float y);				// 上、下内缩偏移量

BASEGRAPH_COMMON_API void __cdecl SYRectf_DeflateRect2(rectf* pR,			// 矩形指针
								  SYSizef size);		// 内缩偏移量

BASEGRAPH_COMMON_API void __cdecl SYRectf_DeflateRect3(rectf* pR,			// 矩形指针
								  float l,				// 左内缩偏移量
								  float t,				// 上内缩偏移量
								  float r,				// 右内缩偏移量
								  float b);				// 下内缩偏移量

///////////////////////////////////////////////////////////////////////////////
// 描述：	规格化矩形（left < right && top < bottom）
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectf_NormalizeRect(rectf* pR);			// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	归一化矩形
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectf_NormalizeDRect(rectf* pDesR,		// 目标矩形
									rectf* pSrcR,		// 矩形指针
								    SYPoint2f point,			// 归一化中心点
								    float fWidth,			// 归一化宽
								    float fHeight);			// 归一化高

///////////////////////////////////////////////////////////////////////////////
// 描述：	还原已归一化的矩形
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectf_RevertDRect(rectf* pDesR,			// 目标矩形指针
								 rectf* pSrcR,			// 源矩形
								 SYPoint2f point,			// 还原中心点
								 float fWidth,				// 还原宽
								 float fHeight);			// 还原高

///////////////////////////////////////////////////////////////////////////////
// 描述：	在x和y方向上按给定值偏移
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectf_OffsetRect1(rectf* pR,			// 矩形指针
								 float x,				// 水平偏移量
								 float y);				// 垂直偏移量

BASEGRAPH_COMMON_API void __cdecl SYRectf_OffsetRect2(rectf* pR,			// 矩形指针
								 SYPoint2f point);		// 偏移点

BASEGRAPH_COMMON_API void __cdecl SYRectf_OffsetRect3(rectf* pR,			// 矩形指针
								 SYSizef size);			// 偏移大小

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形范围的大小
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectf_Size(LPSYSIZEF pDesSz,			// 目标Size
						  rectf* pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形左上点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectf_TopLeft(LPSYPOINTF pDesP,			// 目标点
							 rectf* pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形右下点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectf_BottomRight(LPSYPOINTF pDesP,			// 目标点
								 rectf* pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形中心点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectf_CenterPoint(LPSYPOINTF pDesP,			// 目标点
								 rectf* pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形拖动点的爪子，从左上角开始顺时针给出
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectf_DragPoint(LPSYRECT* ppDesP,			// 目标矩形数组
							   rectf* pSrcR,				// 源矩形
							   int nStyle,					// 爪子的风格
							   int nRadius);				// 方格半径

///////////////////////////////////////////////////////////////////////////////
// 描述：	求两个矩形的交集
//
// 返回值:	TRUE			交集不为空
//			FALSE			交集为空
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl SYRectf_IntersectRect(rectf* pDesR,			// 交集结果
								   rectf* pSrcR1,		// 源矩形
								   rectf* pSrcR2);		// 源矩形

///////////////////////////////////////////////////////////////////////////////
// 描述：	求两个矩形的并集
//
// 返回值:	TRUE			并集不为空
//			FALSE			并集为空
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl SYRectf_UnionRect(rectf* pDesR,			// 并集结果
							   rectf* pSrcR1,		// 源矩形
							   rectf* pSrcR2);		// 源矩形

BASEGRAPH_COMMON_API void __cdecl SYRectf_UnionPoint(rectf* pDesR,			// 并集结果
							   rectf* pSrcR1,		// 源矩形
							   float fX,float fY);		// 源矩形

BASEGRAPH_COMMON_API void __cdecl SYRectf_To_Rect(RECT* pDst,const rectf* pSrc);

// 双精度型实现

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectlf_SetRect(LPSYRECTLF pR,			// 矩形指针
							 double l,				// 左边位置
							 double t,				// 上边位置
							 double r,				// 右边位置
							 double b);				// 下边位置

///////////////////////////////////////////////////////////////////////////////
// 描述：	清空矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectlf_SetRectEmpty(LPSYRECTLF pR);		// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形的宽
//
// 返回值:	double
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API double __cdecl SYRectlf_Width(LPSYRECTLF pR);				// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形的高
//
// 返回值:	double
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API double __cdecl SYRectlf_Height(LPSYRECTLF pR);				// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断矩形是否为空（left = top = right = bottom = 0）
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl SYRectlf_IsRectNull(LPSYRECTLF pR);			// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断矩形范围是否为空（width <= 0 || heigth <= 0）（区别于IsRectNull）
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl SYRectlf_IsRectEmpty(LPSYRECTLF pR);			// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断点是否在矩形内
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl SYRectlf_PtInRect(LPSYRECTLF pR,				// 矩形指针
							  SYPoint2lf point);			// 点

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩形相等判断
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl SYRectlf_EqualRect(LPSYRECTLF pR1,				// 矩形
							   LPSYRECTLF pR2);				// 矩形

///////////////////////////////////////////////////////////////////////////////
// 描述：	拷贝矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectlf_CopyRect(LPSYRECTLF pDesR,			// 目标矩形指针
							  SYRectlf srcR);				// 源矩形

///////////////////////////////////////////////////////////////////////////////
// 描述：	扩张矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectlf_InflateRect1(LPSYRECTLF pR,			// 矩形指针
								  double x,				// 左、右外扩偏移量
								  double y);			// 上、下外扩偏移量

BASEGRAPH_COMMON_API void __cdecl SYRectlf_InflateRect2(LPSYRECTLF pR,			// 矩形指针
								  SYSizelf size);		// 外扩偏移量
								 
BASEGRAPH_COMMON_API void __cdecl SYRectlf_InflateRect3(LPSYRECTLF pR,			// 矩形指针
								  double l,				// 左外扩偏移量
								  double t,				// 上外扩偏移量
								  double r,				// 右外扩偏移量
								  double b);			// 下外扩偏移量

///////////////////////////////////////////////////////////////////////////////
// 描述：	压缩矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectlf_DeflateRect1(LPSYRECTLF pR,			// 矩形指针
								  double x,				// 左、右内缩偏移量
								  double y);			// 上、下内缩偏移量

BASEGRAPH_COMMON_API void __cdecl SYRectlf_DeflateRect2(LPSYRECTLF pR,			// 矩形指针
								  SYSizelf size);		// 内缩偏移量

BASEGRAPH_COMMON_API void __cdecl SYRectlf_DeflateRect3(LPSYRECTLF pR,			// 矩形指针
								  double l,				// 左内缩偏移量
								  double t,				// 上内缩偏移量
								  double r,				// 右内缩偏移量
								  double b);			// 下内缩偏移量

///////////////////////////////////////////////////////////////////////////////
// 描述：	规格化矩形（left < right && top < bottom）
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectlf_NormalizeRect(LPSYRECTLF pR);			// 矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	归一化矩形
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectlf_NormalizeDRect(rectf* pDesR,		// 目标矩形
									LPSYRECTLF pSrcR,		// 矩形指针
								    SYPoint2lf point,			// 归一化中心点
								    double dWidth,			// 归一化宽
								    double dHeight);		// 归一化高

///////////////////////////////////////////////////////////////////////////////
// 描述：	还原已归一化的矩形
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectlf_RevertDRect(LPSYRECTLF pDesR,			// 目标矩形指针
								 rectf* pSrcR,			// 源矩形
								 SYPoint2lf point,			// 还原中心点
								 double dWidth,				// 还原宽
								 double dHeight);			// 还原高

///////////////////////////////////////////////////////////////////////////////
// 描述：	在x和y方向上按给定值偏移
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectlf_OffsetRect1(LPSYRECTLF pR,			// 矩形指针
								 double x,				// 水平偏移量
								 double y);				// 垂直偏移量

BASEGRAPH_COMMON_API void __cdecl SYRectlf_OffsetRect2(LPSYRECTLF pR,			// 矩形指针
								 SYPoint2lf point);		// 偏移点

BASEGRAPH_COMMON_API void __cdecl SYRectlf_OffsetRect3(LPSYRECTLF pR,			// 矩形指针
								 SYSizelf size);			// 偏移大小

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形范围的大小
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectlf_Size(LPSYSIZELF pDesSz,			// 目标Size
						  LPSYRECTLF pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形左上点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectlf_TopLeft(LPSYPOINTD pDesP,			// 目标点
							 LPSYRECTLF pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形右下点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectlf_BottomRight(LPSYPOINTD pDesP,			// 目标点
								 LPSYRECTLF pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形中心点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectlf_CenterPoint(LPSYPOINTD pDesP,			// 目标点
								 LPSYRECTLF pSrcR);			// 源矩形指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形拖动点的爪子，从左上角开始顺时针给出
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl SYRectlf_DragPoint(LPSYRECT* ppDesP,			// 目标矩形数组
							   LPSYRECTLF pSrcR,				// 源矩形
							   int nStyle,					// 爪子的风格
							   int nRadius);				// 方格半径

///////////////////////////////////////////////////////////////////////////////
// 描述：	求两个矩形的交集
//
// 返回值:	TRUE			交集不为空
//			FALSE			交集为空
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl SYRectlf_IntersectRect(LPSYRECTLF pDesR,			// 交集结果
								   LPSYRECTLF pSrcR1,		// 源矩形
								   LPSYRECTLF pSrcR2);		// 源矩形

///////////////////////////////////////////////////////////////////////////////
// 描述：	求两个矩形的并集
//
// 返回值:	TRUE			并集不为空
//			FALSE			并集为空
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl SYRectlf_UnionRect(LPSYRECTLF pDesR,			// 并集结果
							   LPSYRECTLF pSrcR1,		// 源矩形
							   LPSYRECTLF pSrcR2);		// 源矩形

///////////////////////////// End of SYRect ///////////////////////////////////

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

inline SYRect operator & (SYRect& r1, SYRect& r2)
{
	SYRect r3;

	SYRect_IntersectRect(&r3, &r1, &r2);

	return r3;
}

inline SYRect& operator &= (SYRect& r1, SYRect& r2)
{
	SYRect r3;

	SYRect_IntersectRect(&r3, &r1, &r2);
	SYRect_SetRect(&r1, r3.left, r3.top, r3.right, r3.bottom);

	return r1;
}

inline SYRect operator | (SYRect& r1, SYRect& r2)
{
	SYRect r3;

	SYRect_UnionRect(&r3, &r1, &r2);

	return r3;
}

inline SYRect& operator |= (SYRect& r1, SYRect& r2)
{
	SYRect r3;

	SYRect_UnionRect(&r3, &r1, &r2);
	SYRect_SetRect(&r1, r3.left, r3.top, r3.right, r3.bottom);

	return r1;
}

// 单精度型实现

inline rectf operator & (rectf& r1, rectf& r2)
{
	rectf r3;

	SYRectf_IntersectRect(&r3, &r1, &r2);

	return r3;
}

inline rectf& operator &= (rectf& r1, rectf& r2)
{
	rectf r3;

	SYRectf_IntersectRect(&r3, &r1, &r2);
	SYRectf_SetRect(&r1, r3.left, r3.top, r3.right, r3.bottom);

	return r1;
}

inline rectf operator | (rectf& r1, rectf& r2)
{
	rectf r3;

	SYRectf_UnionRect(&r3, &r1, &r2);

	return r3;
}

inline rectf& operator |= (rectf& r1, rectf& r2)
{
	rectf r3;

	SYRectf_UnionRect(&r3, &r1, &r2);
	SYRectf_SetRect(&r1, r3.left, r3.top, r3.right, r3.bottom);

	return r1;
}

// 双精度型实现

inline SYRectlf operator & (SYRectlf& r1, SYRectlf& r2)
{
	SYRectlf r3;

	SYRectlf_IntersectRect(&r3, &r1, &r2);

	return r3;
}

inline SYRectlf& operator &= (SYRectlf& r1, SYRectlf& r2)
{
	SYRectlf r3;

	SYRectlf_IntersectRect(&r3, &r1, &r2);
	SYRectlf_SetRect(&r1, r3.left, r3.top, r3.right, r3.bottom);

	return r1;
}

inline SYRectlf operator | (SYRectlf& r1, SYRectlf& r2)
{
	SYRectlf r3;

	SYRectlf_UnionRect(&r3, &r1, &r2);

	return r3;
}

inline SYRectlf& operator |= (SYRectlf& r1, SYRectlf& r2)
{
	SYRectlf r3;

	SYRectlf_UnionRect(&r3, &r1, &r2);
	SYRectlf_SetRect(&r1, r3.left, r3.top, r3.right, r3.bottom);

	return r1;
}


///////////////////////////////// 类封装 //////////////////////////////////////

// 声明用到的类
template <class TYPE, class ARG_TYPE> class CSYPoint;
template <class TYPE, class ARG_TYPE> class CSYSize;
template <class TYPE, class ARG_TYPE> class CSYRect;

//////////////////////////////// 类的定义 /////////////////////////////////////

/////////////////////////////// CSYPoint //////////////////////////////////////

template <class TYPE, class ARG_TYPE = const TYPE&>
class CSYPoint
{
public:
	TYPE	x;
	TYPE	y;

public:
	// 空值创建
	CSYPoint();
	// 初始化创建
	CSYPoint(ARG_TYPE X, ARG_TYPE Y);
	// 拷贝创建
	CSYPoint(const CSYPoint& point);
	// 取 CSYSize 值创建
	CSYPoint(const CSYSize<TYPE, ARG_TYPE>& size);
	CSYPoint(CSYSize<TYPE, ARG_TYPE> size);
	// 取结构值创建
	CSYPoint(const SYPoint& pointL);
	CSYPoint(const SYPoint2f& pointF);
	CSYPoint(const SYPoint2lf& pointD);

	// 按指定偏移量移动
	void Offset(ARG_TYPE xOffset, ARG_TYPE yOffset);
	void Offset(const CSYPoint& point);
	void Offset(CSYPoint point);
	void Offset(const CSYSize<TYPE, ARG_TYPE>& size);
	void Offset(CSYSize<TYPE, ARG_TYPE> size);
	// 设值
	void SetPoint(ARG_TYPE X, ARG_TYPE Y);
	// 拷贝点值
	void CopyPoint(const CSYPoint& point);
	void CopyPoint(CSYPoint point);

	CSYPoint& operator=(const CSYPoint<TYPE, ARG_TYPE>& point);
	CSYPoint& operator=(const SYPoint& pointL);
	CSYPoint& operator=(SYPoint pointL);
	CSYPoint& operator=(const SYPoint2f& pointL);
	CSYPoint& operator=(SYPoint2f pointL);
	CSYPoint& operator=(const SYPoint2lf& pointL);
	CSYPoint& operator=(SYPoint2lf pointL);

	BOOL operator==(const CSYPoint& point);
	BOOL operator!=(const CSYPoint& point);
	BOOL operator==(CSYPoint point);
	BOOL operator!=(CSYPoint point);

	CSYPoint& operator+=(const CSYSize<TYPE, ARG_TYPE>& size);
	CSYPoint& operator-=(const CSYSize<TYPE, ARG_TYPE>& size);
	CSYPoint& operator+=(const CSYPoint& point);
	CSYPoint& operator-=(const CSYPoint& point);
	CSYPoint& operator+=(CSYSize<TYPE, ARG_TYPE> size);
	CSYPoint& operator-=(CSYSize<TYPE, ARG_TYPE> size);
	CSYPoint& operator+=(CSYPoint point);
	CSYPoint& operator-=(CSYPoint point);
	CSYPoint& operator+=(const SYPoint& pointL);
	CSYPoint& operator-=(const SYPoint& pointL);
	CSYPoint& operator+=(const SYPoint2f& pointF);
	CSYPoint& operator-=(const SYPoint2f& pointF);
	CSYPoint& operator+=(const SYPoint2lf& pointD);
	CSYPoint& operator-=(const SYPoint2lf& pointD);
	CSYPoint& operator+=(SYPoint pointL);
	CSYPoint& operator-=(SYPoint pointL);
	CSYPoint& operator+=(SYPoint2f pointF);
	CSYPoint& operator-=(SYPoint2f pointF);
	CSYPoint& operator+=(SYPoint2lf pointD);
	CSYPoint& operator-=(SYPoint2lf pointD);

// Operators returning CSYPoint values
	CSYPoint operator+(const CSYSize<TYPE, ARG_TYPE>& size);
	CSYPoint operator-(const CSYSize<TYPE, ARG_TYPE>& size);
	CSYPoint operator+(CSYSize<TYPE, ARG_TYPE> size);
	CSYPoint operator-(CSYSize<TYPE, ARG_TYPE> size);
	CSYPoint operator-();
	CSYPoint operator+(const CSYPoint& point);
	CSYPoint operator+(CSYPoint point);
	CSYPoint operator+(const SYPoint& pointL);
	CSYPoint operator+(const SYPoint2f& pointF);
	CSYPoint operator+(const SYPoint2lf& pointD);
	CSYPoint operator+(SYPoint pointL);
	CSYPoint operator+(SYPoint2f pointF);
	CSYPoint operator+(SYPoint2lf pointD);

// Operators returning CSYSize values
	CSYSize<TYPE, ARG_TYPE> operator-(const CSYPoint& point);
	CSYSize<TYPE, ARG_TYPE> operator-(CSYPoint point);
	CSYSize<TYPE, ARG_TYPE> operator-(const SYPoint& pointL);
	CSYSize<TYPE, ARG_TYPE> operator-(const SYPoint2f& pointF);
	CSYSize<TYPE, ARG_TYPE> operator-(const SYPoint2lf& pointD);
	CSYSize<TYPE, ARG_TYPE> operator-(SYPoint pointL);
	CSYSize<TYPE, ARG_TYPE> operator-(SYPoint2f pointF);
	CSYSize<TYPE, ARG_TYPE> operator-(SYPoint2lf pointD);
};

//////////////////////////// End of CSYPoint //////////////////////////////////


//////////////////////////////// CSYSize //////////////////////////////////////

template <class TYPE, class ARG_TYPE = const TYPE&>
class CSYSize
{
public:
	TYPE	cx;
	TYPE	cy;

public:
	// 空值创建
	CSYSize();
	// 初始化创建
	CSYSize(ARG_TYPE CX, ARG_TYPE CY);
	// 拷贝创建
	CSYSize(const CSYSize& size);
	// 从点创建
	CSYSize(const CSYPoint<TYPE, ARG_TYPE>& point);
	CSYSize(CSYPoint<TYPE, ARG_TYPE> point);
	// 从结构创建
	CSYSize(const SYSizeL& sizeL);
	CSYSize(const SYSizef& sizeF);
	CSYSize(const SYSizelf& sizeD);

	// 设值
	void SetSize(ARG_TYPE CX, ARG_TYPE CY);
	// 拷贝值
	void CopySize(const CSYSize& size);
	void CopySize(CSYSize size);

	CSYSize& operator=(const CSYSize<TYPE, ARG_TYPE>& size);
	CSYSize& operator=(const SYSizeL& sizeL);
	CSYSize& operator=(SYSizeL sizeL);
	CSYSize& operator=(const SYSizef& sizeF);
	CSYSize& operator=(SYSizef sizeF);
	CSYSize& operator=(const SYSizelf& sizeD);
	CSYSize& operator=(SYSizelf sizeD);

	BOOL operator==(const CSYSize& size);
	BOOL operator!=(const CSYSize& size);
	BOOL operator==(CSYSize size);
	BOOL operator!=(CSYSize size);

	CSYSize& operator+=(const CSYSize& size);
	CSYSize& operator-=(const CSYSize& size);
	CSYSize& operator+=(CSYSize size);
	CSYSize& operator-=(CSYSize size);
	CSYSize& operator+=(const SYSizeL& sizeL);
	CSYSize& operator-=(const SYSizeL& sizeL);
	CSYSize& operator+=(const SYSizef& sizeF);
	CSYSize& operator-=(const SYSizef& sizeF);
	CSYSize& operator+=(const SYSizelf& sizeD);
	CSYSize& operator-=(const SYSizelf& sizeD);
	CSYSize& operator+=(SYSizeL sizeL);
	CSYSize& operator-=(SYSizeL sizeL);
	CSYSize& operator+=(SYSizef sizeF);
	CSYSize& operator-=(SYSizef sizeF);
	CSYSize& operator+=(SYSizelf sizeD);
	CSYSize& operator-=(SYSizelf sizeD);

// Operators returning CSYSize values
	CSYSize operator+(const CSYSize& size);
	CSYSize operator-(const CSYSize& size);
	CSYSize operator+(CSYSize size);
	CSYSize operator-(CSYSize size);
	CSYSize operator-();
	CSYSize operator+(const SYSizeL& sizeL);
	CSYSize operator-(const SYSizeL& sizeL);
	CSYSize operator+(const SYSizef& sizeF);
	CSYSize operator-(const SYSizef& sizeF);
	CSYSize operator+(const SYSizelf& sizeD);
	CSYSize operator-(const SYSizelf& sizeD);
	CSYSize operator+(SYSizeL sizeL);
	CSYSize operator-(SYSizeL sizeL);
	CSYSize operator+(SYSizef sizeF);
	CSYSize operator-(SYSizef sizeF);
	CSYSize operator+(SYSizelf sizeD);
	CSYSize operator-(SYSizelf sizeD);

// Operators returning CSYPoint values
	CSYPoint<TYPE, ARG_TYPE> operator+(const CSYPoint<TYPE, ARG_TYPE>& point);
	CSYPoint<TYPE, ARG_TYPE> operator-(const CSYPoint<TYPE, ARG_TYPE>& point);
	CSYPoint<TYPE, ARG_TYPE> operator+(CSYPoint<TYPE, ARG_TYPE> point);
	CSYPoint<TYPE, ARG_TYPE> operator-(CSYPoint<TYPE, ARG_TYPE> point);
};

///////////////////////////// End of CSYSize //////////////////////////////////

#endif

#endif		// #ifndef __SY_PLANE_INC__
//------------------------------- End of File ---------------------------------