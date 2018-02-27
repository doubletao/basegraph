// Header FileName:	Common\PlaneDef.h
// CPP FileName:	SYPlane.cpp
#include "stdafx.h" 
#include "Common\PlaneDef.h"
#include "MathOpASM64.h"
#include <assert.h>

#ifdef _DEBUG
#define new SY_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
///////////////////////////////////////////////////////////////////////////////
// 描述：	设置点值
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYPoint_SetPoint(LPSYPOINTL pP,			// 点指针
							   LONG x,					// x值
							   LONG y)					// y值
{
	assert(pP != NULL);

	pP->x = x;
	pP->y = y;
}

void __cdecl SYPoint2f_SetPoint(LPSYPOINTF pP,			// 点指针
							   float x,					// x值
							   float y)					// y值
{
	assert(pP != NULL);

	pP->x = x;
	pP->y = y;
}

void __cdecl SYPoint2lf_SetPoint(LPSYPOINTD pP,			// 点指针
							   double x,				// x值
							   double y)				// y值
{
	assert(pP != NULL);

	pP->x = x;
	pP->y = y;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	按给定值偏移
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYPoint_Offset(LPSYPOINTL pP,			// 点指针
							 LONG xOffset,			// x偏移量
							 LONG yOffset)			// y偏移量
{
	assert(pP != NULL);

	pP->x += xOffset;
	pP->y += yOffset;
}

void __cdecl SYPoint2f_Offset(LPSYPOINTF pP,			// 点指针
							 float xOffset,			// x偏移量
							 float yOffset)			// y偏移量
{
	assert(pP != NULL);

	pP->x += xOffset;
	pP->y += yOffset;
}

void __cdecl SYPoint2lf_Offset(LPSYPOINTD pP,			// 点指针
							 double xOffset,		// x偏移量
							 double yOffset)		// y偏移量
{
	assert(pP != NULL);

	pP->x += xOffset;
	pP->y += yOffset;
}


///////////////////////////// End of SYPoint //////////////////////////////////


///////////////////////////////// SYSize //////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置size值
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYSize_SetSize(LPSYSIZE pS,			// size指针
							 LONG cx,				// cx值
							 LONG cy)				// cy值
{
	assert(pS != NULL);

	pS->cx = cx;
	pS->cy = cy;
}

void __cdecl SYSizef_SetSize(LPSYSIZEF pS,			// size指针
							 float cx,				// cx值
							 float cy)				// cy值
{
	assert(pS != NULL);

	pS->cx = cx;
	pS->cy = cy;
}

void __cdecl SYSizelf_SetSize(LPSYSIZELF pS,			// size指针
							 double cx,				// cx值
							 double cy)				// cy值
{
	assert(pS != NULL);

	pS->cx = cx;
	pS->cy = cy;
}

///////////////////////////// End of SYSize ///////////////////////////////////


///////////////////////////////// SYRect //////////////////////////////////////

// 整型实现

///////////////////////////////////////////////////////////////////////////////
// 描述：	通过点和半径设置矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRect_SetRectBPR(LPSYRECT pR,			// 矩形指针
								SYPoint point,			// 中心点
								int nRadius)			// 半径
{
	assert(pR != NULL);

	pR->left = point.x - nRadius;
	pR->top = point.y - nRadius;
	pR->right = point.x + nRadius;
	pR->bottom = point.y + nRadius;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRect_SetRect(LPSYRECT pR,			// 矩形指针
							 LONG l,				// 左边位置
							 LONG t,				// 上边位置
							 LONG r,				// 右边位置
							 LONG b)				// 下边位置
{
	assert(pR != NULL);

	pR->left = l;
	pR->top = t;
	pR->right = r;
	pR->bottom = b;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	清空矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRect_SetRectEmpty(LPSYRECT pR)		// 矩形指针
{
	assert(pR != NULL);

	pR->right = pR->left;
	pR->bottom = pR->top;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形的宽
//
// 返回值:	LONG
///////////////////////////////////////////////////////////////////////////////
LONG __cdecl SYRect_Width(LPSYRECT pR)				// 矩形指针
{
	assert(pR != NULL);

	return (pR->right - pR->left);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形的高
//
// 返回值:	LONG
///////////////////////////////////////////////////////////////////////////////
LONG __cdecl SYRect_Height(LPSYRECT pR)				// 矩形指针
{
	assert(pR != NULL);

	return (pR->bottom - pR->top);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断矩形是否为空（left = top = right = bottom = 0）
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYRect_IsRectNull(LPSYRECT pR)			// 矩形指针
{
	assert(pR != NULL);

	return ((pR->left == 0) && (pR->top == 0)
			&& (pR->right == 0) && (pR->bottom == 0));
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断矩形范围是否为空（width <= 0 || heigth <= 0）（区别于IsRectNull）
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYRect_IsRectEmpty(LPSYRECT pR)			// 矩形指针
{
	assert(pR != NULL);

	return ((pR->left >= pR->right) || (pR->top >= pR->bottom));
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断点是否在矩形内
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYRect_PtInRect(LPSYRECT pR,				// 矩形指针
							  SYPoint point)			// 点
{
	assert(pR != NULL);

	return ((point.x >= pR->left) && (point.x < pR->right)
			&& (point.y >= pR->top) && (point.y < pR->bottom));
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩形相等判断
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYRect_EqualRect(LPSYRECT pR1,			// 矩形
							   LPSYRECT pR2)			// 矩形
{
	assert(pR1 != NULL && pR2 != NULL);

	return ((pR1->left == pR2->left) && (pR1->right == pR2->right)
			&& (pR1->top == pR2->top) && (pR1->bottom == pR2->bottom));
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	拷贝矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRect_CopyRect(LPSYRECT pDesR,			// 目标矩形指针
							  LPSYRECT pSrcR)			// 源矩形指针
{
	assert(pDesR != NULL && pSrcR != NULL);

	pDesR->left = pSrcR->left;
	pDesR->right = pSrcR->right;
	pDesR->top = pSrcR->top;
	pDesR->bottom = pSrcR->bottom;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	扩张矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRect_InflateRect1(LPSYRECT pR,			// 矩形指针
								  int x,				// 左、右外扩偏移量
								  int y)				// 上、下外扩偏移量
{
	assert(pR != NULL);

	pR->left -= x;
	pR->right += x;
	pR->top -= y;
	pR->bottom += y;
}

void __cdecl SYRect_InflateRect2(LPSYRECT pR,			// 矩形指针
								  SYSizeL size)			// 外扩偏移量
{
	assert(pR != NULL);

	pR->left -= size.cx;
	pR->right += size.cx;
	pR->top -= size.cy;
	pR->bottom += size.cy;
}
								 
void __cdecl SYRect_InflateRect3(LPSYRECT pR,			// 矩形指针
								  int l,				// 左外扩偏移量
								  int t,				// 上外扩偏移量
								  int r,				// 右外扩偏移量
								  int b)				// 下外扩偏移量
{
	assert(pR != NULL);

	pR->left -= l;
	pR->right += r;
	pR->top -= t;
	pR->bottom += b;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	压缩矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRect_DeflateRect1(LPSYRECT pR,			// 矩形指针
								  int x,				// 左、右内缩偏移量
								  int y)				// 上、下内缩偏移量
{
	assert(pR != NULL);

	pR->left += x;
	pR->right -= x;
	pR->top += y;
	pR->bottom -= y;
}

void __cdecl SYRect_DeflateRect2(LPSYRECT pR,			// 矩形指针
								  SYSizeL size)			// 内缩偏移量
{
	assert(pR != NULL);

	pR->left += size.cx;
	pR->right -= size.cx;
	pR->top += size.cy;
	pR->bottom -= size.cy;
}

void __cdecl SYRect_DeflateRect3(LPSYRECT pR,			// 矩形指针
								  int l,				// 左内缩偏移量
								  int t,				// 上内缩偏移量
								  int r,				// 右内缩偏移量
								  int b)				// 下内缩偏移量
{
	assert(pR != NULL);

	pR->left += l;
	pR->right -= r;
	pR->top += t;
	pR->bottom -= b;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	规格化矩形（left < right && top < bottom）
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRect_NormalizeRect(LPSYRECT pR)			// 矩形指针
{
	assert(pR != NULL);

	if (pR->left > pR->right)
	{
		pR->left += pR->right;
		pR->right = pR->left - pR->right;
		pR->left -= pR->right;
	}
	if (pR->top > pR->bottom)
	{
		pR->top += pR->bottom;
		pR->bottom = pR->top - pR->right;
		pR->top -= pR->bottom;
	}
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	按点归一化矩形
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRect_NormalizeDRect(LPSYRECTF pDesR,		// 目标矩形
									LPSYRECT pSrcR,		// 矩形指针
								    SYPoint point,			// 归一化中心点
								    int nWidth,				// 归一化宽
								    int nHeight)			// 归一化高
{
	assert((pDesR != NULL) && (pSrcR != NULL));
	assert((nWidth != 0) && (nHeight != 0));

	pDesR->left = (float)(point.x - pSrcR->left) / nWidth;
	pDesR->right = (float)(pSrcR->right - point.x) / nWidth;
	pDesR->top = (float)(point.y - pSrcR->top) / nHeight;
	pDesR->bottom = (float)(pSrcR->bottom - point.y) / nHeight;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	还原已归一化的矩形
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRect_RevertDRect(LPSYRECT pDesR,			// 目标矩形指针
								 LPSYRECTF pSrcR,			// 源矩形
								 SYPoint point,			// 还原中心点
								 int nWidth,				// 还原宽
								 int nHeight)				// 还原高
{
	assert((pDesR != NULL) && (pSrcR != NULL));

	pDesR->left = point.x - (LONG)(pSrcR->left * nWidth + 0.5);
	pDesR->right = point.x + (LONG)(pSrcR->right * nWidth + 0.5);
	pDesR->top = point.y - (LONG)(pSrcR->top * nHeight + 0.5);
	pDesR->bottom = point.y + (LONG)(pSrcR->bottom * nHeight + 0.5);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	在x和y方向上按给定值偏移
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRect_OffsetRect1(LPSYRECT pR,			// 矩形指针
								 int x,					// 水平偏移量
								 int y)					// 垂直偏移量
{
	assert(pR != NULL);

	pR->left += x;
	pR->right += x;
	pR->top += y;
	pR->bottom += y;
}

void __cdecl SYRect_OffsetRect2(LPSYRECT pR,			// 矩形指针
								 SYPoint point)		// 偏移点
{
	assert(pR != NULL);

	pR->left += point.x;
	pR->right += point.x;
	pR->top += point.y;
	pR->bottom += point.y;
}

void __cdecl SYRect_OffsetRect3(LPSYRECT pR,			// 矩形指针
								 SYSizeL size)			// 偏移大小
{
	assert(pR != NULL);

	pR->left += size.cx;
	pR->right += size.cx;
	pR->top += size.cy;
	pR->bottom += size.cy;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形范围的大小
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRect_Size(LPSYSIZE pDesSz,			// 目标Size
						  LPSYRECT pSrcR)			// 源矩形指针

{
	assert((pDesSz != NULL) && (pSrcR != NULL));

	pDesSz->cx = pSrcR->right - pSrcR->left;
	pDesSz->cy = pSrcR->bottom - pSrcR->top;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形左上点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRect_TopLeft(LPSYPOINTL pDesP,			// 目标点
							 LPSYRECT pSrcR)			// 源矩形指针
{
	assert((pDesP != NULL) && (pSrcR != NULL));

	pDesP->x = pSrcR->left;
	pDesP->y = pSrcR->top;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形右下点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRect_BottomRight(LPSYPOINTL pDesP,			// 目标点
								 LPSYRECT pSrcR)			// 源矩形指针
{
	assert((pDesP != NULL) && (pSrcR != NULL));

	pDesP->x = pSrcR->right;
	pDesP->y = pSrcR->bottom;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形中心点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRect_CenterPoint(LPSYPOINTL pDesP,			// 目标点
								 LPSYRECT pSrcR)			// 源矩形指针
{
	assert((pDesP != NULL) && (pSrcR != NULL));

	pDesP->x = (pSrcR->left + pSrcR->right) >> 1;
	pDesP->y = (pSrcR->top + pSrcR->bottom) >> 1;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形拖动点的爪子，从左上角开始顺时针给出
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRect_DragPoint(LPSYRECT* ppDesR,			// 目标矩形数组
							   LPSYRECT pSrcR,				// 源矩形
							   int nStyle,					// 爪子的风格
							   int nRadius)					// 方格半径
{
	assert((ppDesR != NULL) && (pSrcR != NULL));

	SYPoint point;

	// 先按矩形在线上的风格获取爪子，然后再根据风格来偏移矩阵
	// 左上点
	SYPoint_SetPoint(&point, pSrcR->left, pSrcR->top);
	SYRect_SetRectBPR(ppDesR[SYRECT_DRAG_LTP], point, nRadius);
	// 上中点
	SYPoint_SetPoint(&point, (pSrcR->left + pSrcR->right) >> 1, pSrcR->top);
	SYRect_SetRectBPR(ppDesR[SYRECT_DRAG_TMP], point, nRadius);
	// 右上点
	SYPoint_SetPoint(&point, pSrcR->right, pSrcR->top);
	SYRect_SetRectBPR(ppDesR[SYRECT_DRAG_RTP], point, nRadius);
	// 右中点
	SYPoint_SetPoint(&point, pSrcR->right, (pSrcR->top + pSrcR->bottom) >> 1);
	SYRect_SetRectBPR(ppDesR[SYRECT_DRAG_RMP], point, nRadius);
	// 右下点
	SYPoint_SetPoint(&point, pSrcR->right, pSrcR->bottom);
	SYRect_SetRectBPR(ppDesR[SYRECT_DRAG_RBP], point, nRadius);
	// 下中点
	SYPoint_SetPoint(&point, (pSrcR->left + pSrcR->right) >> 1, pSrcR->bottom);
	SYRect_SetRectBPR(ppDesR[SYRECT_DRAG_BMP], point, nRadius);
	// 左下点
	SYPoint_SetPoint(&point, pSrcR->left, pSrcR->bottom);
	SYRect_SetRectBPR(ppDesR[SYRECT_DRAG_LBP], point, nRadius);
	// 左中点
	SYPoint_SetPoint(&point, pSrcR->left, (pSrcR->top + pSrcR->bottom) >> 1);
	SYRect_SetRectBPR(ppDesR[SYRECT_DRAG_LMP], point, nRadius);
	switch (nStyle)
	{
	case SYRECT_DRAG_INLINE:
		SYRect_OffsetRect1(ppDesR[SYRECT_DRAG_LTP], nRadius, nRadius);
		SYRect_OffsetRect1(ppDesR[SYRECT_DRAG_TMP], 0, nRadius);
		SYRect_OffsetRect1(ppDesR[SYRECT_DRAG_RTP], -nRadius, nRadius);
		SYRect_OffsetRect1(ppDesR[SYRECT_DRAG_RMP], -nRadius, 0);
		SYRect_OffsetRect1(ppDesR[SYRECT_DRAG_RBP], -nRadius, -nRadius);
		SYRect_OffsetRect1(ppDesR[SYRECT_DRAG_BMP], 0, -nRadius);
		SYRect_OffsetRect1(ppDesR[SYRECT_DRAG_LBP], nRadius, -nRadius);
		SYRect_OffsetRect1(ppDesR[SYRECT_DRAG_LMP], -nRadius, 0);
		break;
	default:
		break;
	}
}
void __cdecl SYRectf_Scale(rectf* rt,float fscale)
{
	rectf_scale_x64(rt,fscale);
}
///////////////////////////////////////////////////////////////////////////////
// 描述：	求两个矩形的交集
//
// 返回值:	TRUE			交集不为空
//			FALSE			交集为空
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYRect_IntersectRect(LPSYRECT pDesR,			// 交集结果
								   LPSYRECT pSrcR1,		// 源矩形
								   LPSYRECT pSrcR2)		// 源矩形
{
	assert((pDesR != NULL) && (pSrcR1 != NULL) && (pSrcR2 != NULL));
		
	pDesR->left = (pSrcR1->left > pSrcR2->left) ? pSrcR1->left : pSrcR2->left;
	pDesR->top = (pSrcR1->top > pSrcR2->top) ? pSrcR1->top : pSrcR2->top;
	pDesR->right = (pSrcR1->right > pSrcR2->right) ? pSrcR2->right : pSrcR1->right;
	pDesR->bottom = (pSrcR1->bottom < pSrcR2->bottom) ? pSrcR1->bottom : pSrcR2->bottom;

	return !SYRect_IsRectEmpty(pDesR);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	求两个矩形的并集
//
// 返回值:	TRUE			并集不为空
//			FALSE			并集为空
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYRect_UnionRect(LPSYRECT pDesR,			// 并集结果
							   LPSYRECT pSrcR1,		// 源矩形
							   LPSYRECT pSrcR2)		// 源矩形
{
	assert((pDesR != NULL) && (pSrcR1 != NULL) && (pSrcR2 != NULL));
			
	pDesR->left = (pSrcR1->left < pSrcR2->left) ? pSrcR1->left : pSrcR2->left;
	pDesR->top = (pSrcR1->top < pSrcR2->top) ? pSrcR1->top : pSrcR2->top;
	pDesR->right = (pSrcR1->right < pSrcR2->right) ? pSrcR2->right : pSrcR1->right;
	pDesR->bottom = (pSrcR1->bottom > pSrcR2->bottom) ? pSrcR1->bottom : pSrcR2->bottom;

	return !SYRect_IsRectEmpty(pDesR);
}


// 浮点型实现

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectf_SetRect(LPSYRECTF pR,			// 矩形指针
							 float l,				// 左边位置
							 float t,				// 上边位置
							 float r,				// 右边位置
							 float b)				// 下边位置
{
	assert(pR != NULL);

	pR->left = l;
	pR->top = t;
	pR->right = r;
	pR->bottom = b;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	清空矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectf_SetRectEmpty(LPSYRECTF pR)		// 矩形指针
{
	assert(pR != NULL);

	pR->right = pR->left = 0;
	pR->bottom = pR->top = 0;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形的宽
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
float __cdecl SYRectf_Width(LPSYRECTF pR)				// 矩形指针
{
	assert(pR != NULL);

	return (pR->right - pR->left);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形的高
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
float __cdecl SYRectf_Height(LPSYRECTF pR)				// 矩形指针
{
	assert(pR != NULL);

	return (pR->bottom - pR->top);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断矩形是否为空（left = top = right = bottom = 0）
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYRectf_IsRectNull(LPSYRECTF pR)			// 矩形指针
{
	assert(pR != NULL);
	return ((fabs(pR->left)<=ABS_FLOAT_0) && (fabs(pR->top)<=ABS_FLOAT_0)
			&& (fabs(pR->right)<=ABS_FLOAT_0) && (fabs(pR->bottom)<=ABS_FLOAT_0));
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断矩形范围是否为空（width <= 0 || heigth <= 0）（区别于IsRectNull）
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYRectf_IsRectEmpty(LPSYRECTF pR)			// 矩形指针
{
	assert(pR != NULL);

	return ((pR->left >= pR->right) || (pR->top >= pR->bottom));
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断点是否在矩形内
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYRectf_PtInRect(LPSYRECTF pR,				// 矩形指针
							  SYPoint2f point)			// 点
{
	assert(pR != NULL);

	return ((point.x >= pR->left) && (point.x < pR->right)
			&& (point.y >= pR->top) && (point.y < pR->bottom));
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩形相等判断
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYRectf_EqualRect(LPSYRECTF pR1,			// 矩形
							   LPSYRECTF pR2)			// 矩形
{
	assert((pR1 != NULL) && (pR2 != NULL));

	return ((pR1->left == pR2->left) && (pR1->right == pR2->right)
			&& (pR1->top == pR2->top) && (pR1->bottom == pR2->bottom));
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	拷贝矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectf_CopyRect(LPSYRECTF pDesR,			// 目标矩形指针
							  LPSYRECTF pSrcR)			// 源矩形指针
{
	assert((pDesR != NULL) && (pSrcR != NULL));

	pDesR->left = pSrcR->left;
	pDesR->right = pSrcR->right;
	pDesR->top = pSrcR->top;
	pDesR->bottom = pSrcR->bottom;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	扩张矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectf_InflateRect1(LPSYRECTF pR,			// 矩形指针
								  float x,				// 左、右外扩偏移量
								  float y)				// 上、下外扩偏移量
{
	assert(pR != NULL);

	pR->left -= x;
	pR->right += x;
	pR->top -= y;
	pR->bottom += y;
}

void __cdecl SYRectf_InflateRect2(LPSYRECTF pR,			// 矩形指针
								  SYSizef size)			// 外扩偏移量
{
	assert(pR != NULL);

	pR->left -= size.cx;
	pR->right += size.cx;
	pR->top -= size.cy;
	pR->bottom += size.cy;
}
								 
void __cdecl SYRectf_InflateRect3(LPSYRECTF pR,			// 矩形指针
								  float l,				// 左外扩偏移量
								  float t,				// 上外扩偏移量
								  float r,				// 右外扩偏移量
								  float b)				// 下外扩偏移量
{
	assert(pR != NULL);

	pR->left -= l;
	pR->right += r;
	pR->top -= t;
	pR->bottom += b;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	压缩矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectf_DeflateRect1(LPSYRECTF pR,			// 矩形指针
								  float x,				// 左、右内缩偏移量
								  float y)				// 上、下内缩偏移量
{
	assert(pR != NULL);

	pR->left += x;
	pR->right -= x;
	pR->top += y;
	pR->bottom -= y;
}

void __cdecl SYRectf_DeflateRect2(LPSYRECTF pR,			// 矩形指针
								  SYSizef size)			// 内缩偏移量
{
	assert(pR != NULL);

	pR->left += size.cx;
	pR->right -= size.cx;
	pR->top += size.cy;
	pR->bottom -= size.cy;
}

void __cdecl SYRectf_DeflateRect3(LPSYRECTF pR,			// 矩形指针
								  float l,				// 左内缩偏移量
								  float t,				// 上内缩偏移量
								  float r,				// 右内缩偏移量
								  float b)				// 下内缩偏移量
{
	assert(pR != NULL);

	pR->left += l;
	pR->right -= r;
	pR->top += t;
	pR->bottom -= b;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	规格化矩形（left < right && top < bottom）
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectf_NormalizeRect(LPSYRECTF pR)			// 矩形指针
{
	assert(pR != NULL);

	float fTemp;
	if (pR->left > pR->right)
	{
		fTemp = pR->left;
		pR->left = pR->right;
		pR->right = fTemp;
	}
	if (pR->top > pR->bottom)
	{
		fTemp = pR->top;
		pR->top = pR->bottom;
		pR->bottom = fTemp;
	}
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	按点归一化矩形
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectf_NormalizeDRect(LPSYRECTF pDesR,		// 目标矩形
									LPSYRECTF pSrcR,		// 矩形指针
								    SYPoint2f point,			// 归一化中心点
								    float fWidth,			// 归一化宽
									float fHeight)			// 归一化高
{
	assert((pDesR != NULL) && (pSrcR != NULL));
	assert((fWidth != 0) && (fHeight != 0));

	pDesR->left = (point.x - pSrcR->left) / fWidth;
	pDesR->right = (pSrcR->right - point.x) / fWidth;
	pDesR->top = (point.y - pSrcR->top) / fHeight;
	pDesR->bottom = (pSrcR->bottom - point.y) / fHeight;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	还原已归一化的矩形
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectf_RevertDRect(LPSYRECTF pDesR,			// 目标矩形指针
								 LPSYRECTF pSrcR,			// 源矩形
								 SYPoint2f point,			// 还原中心点
								 float fWidth,				// 还原宽
								 float fHeight)				// 还原高
{
	assert((pDesR != NULL) && (pSrcR != NULL));

	pDesR->left = point.x - (float)(pSrcR->left * fWidth + 0.5);
	pDesR->right = point.x + (float)(pSrcR->right * fWidth + 0.5);
	pDesR->top = point.y - (float)(pSrcR->top * fHeight + 0.5);
	pDesR->bottom = point.y + (float)(pSrcR->bottom * fHeight + 0.5);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	在x和y方向上按给定值偏移
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectf_OffsetRect1(LPSYRECTF pR,			// 矩形指针
								 float x,				// 水平偏移量
								 float y)				// 垂直偏移量
{
	assert(pR != NULL);

	pR->left += x;
	pR->right += x;
	pR->top += y;
	pR->bottom += y;
}

void __cdecl SYRectf_OffsetRect2(LPSYRECTF pR,			// 矩形指针
								 SYPoint2f point)		// 偏移点
{
	assert(pR != NULL);

	pR->left += point.x;
	pR->right += point.x;
	pR->top += point.y;
	pR->bottom += point.y;
}

void __cdecl SYRectf_OffsetRect3(LPSYRECTF pR,			// 矩形指针
								 SYSizef size)			// 偏移大小
{
	assert(pR != NULL);

	pR->left += size.cx;
	pR->right += size.cx;
	pR->top += size.cy;
	pR->bottom += size.cy;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形范围的大小
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectf_Size(LPSYSIZEF pDesSz,			// 目标Size
						  LPSYRECTF pSrcR)			// 源矩形指针

{
	assert((pDesSz != NULL) && (pSrcR != NULL));

	pDesSz->cx = pSrcR->right - pSrcR->left;
	pDesSz->cy = pSrcR->bottom - pSrcR->top;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形左上点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectf_TopLeft(LPSYPOINTF pDesP,			// 目标点
							 LPSYRECTF pSrcR)			// 源矩形指针
{
	assert((pDesP != NULL) && (pSrcR != NULL));

	pDesP->x = pSrcR->left;
	pDesP->y = pSrcR->top;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形右下点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectf_BottomRight(LPSYPOINTF pDesP,			// 目标点
								 LPSYRECTF pSrcR)			// 源矩形指针
{
	assert((pDesP != NULL) && (pSrcR != NULL));

	pDesP->x = pSrcR->right;
	pDesP->y = pSrcR->bottom;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形中心点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectf_CenterPoint(LPSYPOINTF pDesP,			// 目标点
								 LPSYRECTF pSrcR)			// 源矩形指针
{
	assert((pDesP != NULL) && (pSrcR != NULL));

	pDesP->x = (pSrcR->left + pSrcR->right) / 2;
	pDesP->y = (pSrcR->top + pSrcR->bottom) / 2;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形拖动点的爪子，从左上角开始顺时针给出
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectf_DragPoint(LPSYRECT* ppDesR,			// 目标矩形数组
							   LPSYRECTF pSrcR,				// 源矩形
							   int nStyle,					// 爪子的风格
							   int nRadius)					// 方格半径
{
	assert((ppDesR != NULL) && (pSrcR != NULL));

	SYPoint point;

	// 先按矩形在线上的风格获取爪子，然后再根据风格来偏移矩阵
	// 左上点
	SYPoint_SetPoint(&point, (LONG)pSrcR->left, (LONG)pSrcR->top);
	SYRect_SetRectBPR(ppDesR[SYRECT_DRAG_LTP], point, nRadius);
	// 上中点
	SYPoint_SetPoint(&point, (LONG)((pSrcR->left + pSrcR->right) / 2), (LONG)pSrcR->top);
	SYRect_SetRectBPR(ppDesR[SYRECT_DRAG_TMP], point, nRadius);
	// 右上点
	SYPoint_SetPoint(&point, (LONG)pSrcR->right, (LONG)pSrcR->top);
	SYRect_SetRectBPR(ppDesR[SYRECT_DRAG_RTP], point, nRadius);
	// 右中点
	SYPoint_SetPoint(&point, (LONG)pSrcR->right, (LONG)((pSrcR->top + pSrcR->bottom) / 2));
	SYRect_SetRectBPR(ppDesR[SYRECT_DRAG_RMP], point, nRadius);
	// 右下点
	SYPoint_SetPoint(&point, (LONG)pSrcR->right, (LONG)pSrcR->bottom);
	SYRect_SetRectBPR(ppDesR[SYRECT_DRAG_RBP], point, nRadius);
	// 下中点
	SYPoint_SetPoint(&point, (LONG)((pSrcR->left + pSrcR->right) / 2), (LONG)pSrcR->bottom);
	SYRect_SetRectBPR(ppDesR[SYRECT_DRAG_BMP], point, nRadius);
	// 左下点
	SYPoint_SetPoint(&point, (LONG)pSrcR->left, (LONG)pSrcR->bottom);
	SYRect_SetRectBPR(ppDesR[SYRECT_DRAG_LBP], point, nRadius);
	// 左中点
	SYPoint_SetPoint(&point, (LONG)pSrcR->left, (LONG)((pSrcR->top + pSrcR->bottom) / 2));
	SYRect_SetRectBPR(ppDesR[SYRECT_DRAG_LMP], point, nRadius);
	switch (nStyle)
	{
	case SYRECT_DRAG_INLINE:
		SYRect_OffsetRect1(ppDesR[SYRECT_DRAG_LTP], nRadius, nRadius);
		SYRect_OffsetRect1(ppDesR[SYRECT_DRAG_TMP], 0, nRadius);
		SYRect_OffsetRect1(ppDesR[SYRECT_DRAG_RTP], -nRadius, nRadius);
		SYRect_OffsetRect1(ppDesR[SYRECT_DRAG_RMP], -nRadius, 0);
		SYRect_OffsetRect1(ppDesR[SYRECT_DRAG_RBP], -nRadius, -nRadius);
		SYRect_OffsetRect1(ppDesR[SYRECT_DRAG_BMP], 0, -nRadius);
		SYRect_OffsetRect1(ppDesR[SYRECT_DRAG_LBP], nRadius, -nRadius);
		SYRect_OffsetRect1(ppDesR[SYRECT_DRAG_LMP], -nRadius, 0);
		break;
	default:
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	求两个矩形的交集
//
// 返回值:	TRUE			交集不为空
//			FALSE			交集为空
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYRectf_IntersectRect(LPSYRECTF pDesR,			// 交集结果
								   LPSYRECTF pSrcR1,		// 源矩形
								   LPSYRECTF pSrcR2)		// 源矩形
{
	assert((pDesR != NULL) && (pSrcR1 != NULL) && (pSrcR2 != NULL));
		
	pDesR->left = (pSrcR1->left > pSrcR2->left) ? pSrcR1->left : pSrcR2->left;
	pDesR->top = (pSrcR1->top > pSrcR2->top) ? pSrcR1->top : pSrcR2->top;
	pDesR->right = (pSrcR1->right > pSrcR2->right) ? pSrcR2->right : pSrcR1->right;
	pDesR->bottom = (pSrcR1->bottom < pSrcR2->bottom) ? pSrcR1->bottom : pSrcR2->bottom;

	return SYRectf_IsRectEmpty(pDesR);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	求两个矩形的并集
//
// 返回值:	TRUE			并集不为空
//			FALSE			并集为空
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYRectf_UnionRect(LPSYRECTF pDesR,			// 并集结果
							   LPSYRECTF pSrcR1,		// 源矩形
							   LPSYRECTF pSrcR2)		// 源矩形
{
	assert((pDesR != NULL) && (pSrcR1 != NULL) && (pSrcR2 != NULL));
			
	pDesR->left = (pSrcR1->left < pSrcR2->left) ? pSrcR1->left : pSrcR2->left;
	pDesR->top = (pSrcR1->top < pSrcR2->top) ? pSrcR1->top : pSrcR2->top;
	pDesR->right = (pSrcR1->right < pSrcR2->right) ? pSrcR2->right : pSrcR1->right;
	pDesR->bottom = (pSrcR1->bottom > pSrcR2->bottom) ? pSrcR1->bottom : pSrcR2->bottom;

	return SYRectf_IsRectEmpty(pDesR);
}


// 双精度型实现

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectlf_SetRect(LPSYRECTLF pR,			// 矩形指针
							 double l,				// 左边位置
							 double t,				// 上边位置
							 double r,				// 右边位置
							 double b)				// 下边位置
{
	assert(pR != NULL);

	pR->left = l;
	pR->top = t;
	pR->right = r;
	pR->bottom = b;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	清空矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectlf_SetRectEmpty(LPSYRECTLF pR)		// 矩形指针
{
	assert(pR != NULL);

	pR->right = pR->left;
	pR->bottom = pR->top;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形的宽
//
// 返回值:	double
///////////////////////////////////////////////////////////////////////////////
double __cdecl SYRectlf_Width(LPSYRECTLF pR)				// 矩形指针
{
	assert(pR != NULL);

	return (pR->right - pR->left);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形的高
//
// 返回值:	double
///////////////////////////////////////////////////////////////////////////////
double __cdecl SYRectlf_Height(LPSYRECTLF pR)				// 矩形指针
{
	assert(pR != NULL);

	return (pR->bottom - pR->top);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断矩形是否为空（left = top = right = bottom = 0）
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYRectlf_IsRectNull(LPSYRECTLF pR)			// 矩形指针
{
	assert(pR != NULL);

	return ((pR->left == 0) && (pR->top == 0)
			&& (pR->right == 0) && (pR->bottom == 0));
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断矩形范围是否为空（width <= 0 || heigth <= 0）（区别于IsRectNull）
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYRectlf_IsRectEmpty(LPSYRECTLF pR)			// 矩形指针
{
	assert(pR != NULL);

	return ((pR->left >= pR->right) || (pR->top >= pR->bottom));
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	判断点是否在矩形内
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYRectlf_PtInRect(LPSYRECTLF pR,				// 矩形指针
							  SYPoint2lf point)			// 点
{
	assert(pR != NULL);

	return ((point.x >= pR->left) && (point.x < pR->right)
			&& (point.y >= pR->top) && (point.y < pR->bottom));
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩形相等判断
//
// 返回值:	TRUE		是
//			FALSE		否
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYRectlf_EqualRect(LPSYRECTLF pR1,			// 矩形
							   LPSYRECTLF pR2)			// 矩形
{
	assert((pR1 != NULL) && (pR2 != NULL));

	return ((pR1->left == pR2->left) && (pR1->right == pR2->right)
			&& (pR1->top == pR2->top) && (pR1->bottom == pR2->bottom));
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	拷贝矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectlf_CopyRect(LPSYRECTLF pDesR,			// 目标矩形指针
							  LPSYRECTLF pSrcR)			// 源矩形指针
{
	assert((pDesR != NULL) && (pSrcR != NULL));

	pDesR->left = pSrcR->left;
	pDesR->right = pSrcR->right;
	pDesR->top = pSrcR->top;
	pDesR->bottom = pSrcR->bottom;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	扩张矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectlf_InflateRect1(LPSYRECTLF pR,			// 矩形指针
								  double x,				// 左、右外扩偏移量
								  double y)				// 上、下外扩偏移量
{
	assert(pR != NULL);

	pR->left -= x;
	pR->right += x;
	pR->top -= y;
	pR->bottom += y;
}

void __cdecl SYRectlf_InflateRect2(LPSYRECTLF pR,			// 矩形指针
								  SYSizelf size)			// 外扩偏移量
{
	assert(pR != NULL);

	pR->left -= size.cx;
	pR->right += size.cx;
	pR->top -= size.cy;
	pR->bottom += size.cy;
}
								 
void __cdecl SYRectlf_InflateRect3(LPSYRECTLF pR,			// 矩形指针
								  double l,				// 左外扩偏移量
								  double t,				// 上外扩偏移量
								  double r,				// 右外扩偏移量
								  double b)				// 下外扩偏移量
{
	assert(pR != NULL);

	pR->left -= l;
	pR->right += r;
	pR->top -= t;
	pR->bottom += b;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	压缩矩形范围
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectlf_DeflateRect1(LPSYRECTLF pR,			// 矩形指针
								  double x,				// 左、右内缩偏移量
								  double y)				// 上、下内缩偏移量
{
	assert(pR != NULL);

	pR->left += x;
	pR->right -= x;
	pR->top += y;
	pR->bottom -= y;
}

void __cdecl SYRectlf_DeflateRect2(LPSYRECTLF pR,			// 矩形指针
								  SYSizelf size)			// 内缩偏移量
{
	assert(pR != NULL);

	pR->left += size.cx;
	pR->right -= size.cx;
	pR->top += size.cy;
	pR->bottom -= size.cy;
}

void __cdecl SYRectlf_DeflateRect3(LPSYRECTLF pR,			// 矩形指针
								  double l,				// 左内缩偏移量
								  double t,				// 上内缩偏移量
								  double r,				// 右内缩偏移量
								  double b)				// 下内缩偏移量
{
	assert(pR != NULL);

	pR->left += l;
	pR->right -= r;
	pR->top += t;
	pR->bottom -= b;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	规格化矩形（left < right && top < bottom）
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectlf_NormalizeRect(LPSYRECTLF pR)			// 矩形指针
{
	assert(pR != NULL);

	if (pR->left > pR->right)
	{
		pR->left += pR->right;
		pR->right = pR->left - pR->right;
		pR->left -= pR->right;
	}
	if (pR->top > pR->bottom)
	{
		pR->top += pR->bottom;
		pR->bottom = pR->top - pR->right;
		pR->top -= pR->bottom;
	}
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	按点归一化矩形
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectlf_NormalizeDRect(LPSYRECTF pDesR,		// 目标矩形
									LPSYRECTLF pSrcR,		// 矩形指针
								    SYPoint2lf point,			// 归一化中心点
								    double dWidth,			// 归一化宽
								    double dHeight)			// 归一化高
{
	assert((pDesR != NULL) && (pSrcR != NULL));
	assert((dWidth != 0) && (dHeight != 0));

	pDesR->left = (float)((point.x - pSrcR->left) / dWidth);
	pDesR->right = (float)((pSrcR->right - point.x) / dWidth);
	pDesR->top = (float)((point.y - pSrcR->top) / dHeight);
	pDesR->bottom = (float)((pSrcR->bottom - point.y) / dHeight);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	还原已归一化的矩形
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectlf_RevertDRect(LPSYRECTLF pDesR,			// 目标矩形指针
								 LPSYRECTF pSrcR,			// 源矩形
								 SYPoint2lf point,			// 还原中心点
								 double dWidth,				// 还原宽
								 double dHeight)				// 还原高
{
	assert((pDesR != NULL) && (pSrcR != NULL));

	pDesR->left = point.x - pSrcR->left * dWidth;
	pDesR->right = point.x + pSrcR->right * dWidth;
	pDesR->top = point.y - pSrcR->top * dHeight;
	pDesR->bottom = point.y + pSrcR->bottom * dHeight;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	在x和y方向上按给定值偏移
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectlf_OffsetRect1(LPSYRECTLF pR,			// 矩形指针
								 double x,				// 水平偏移量
								 double y)				// 垂直偏移量
{
	assert(pR != NULL);

	pR->left += x;
	pR->right += x;
	pR->top += y;
	pR->bottom += y;
}

void __cdecl SYRectlf_OffsetRect2(LPSYRECTLF pR,			// 矩形指针
								 SYPoint2lf point)		// 偏移点
{
	assert(pR != NULL);

	pR->left += point.x;
	pR->right += point.x;
	pR->top += point.y;
	pR->bottom += point.y;
}

void __cdecl SYRectlf_OffsetRect3(LPSYRECTLF pR,			// 矩形指针
								 SYSizelf size)			// 偏移大小
{
	assert(pR != NULL);

	pR->left += size.cx;
	pR->right += size.cx;
	pR->top += size.cy;
	pR->bottom += size.cy;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形范围的大小
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectlf_Size(LPSYSIZELF pDesSz,			// 目标Size
						  LPSYRECTLF pSrcR)			// 源矩形指针

{
	assert((pDesSz != NULL) && (pSrcR != NULL));

	pDesSz->cx = pSrcR->right - pSrcR->left;
	pDesSz->cy = pSrcR->bottom - pSrcR->top;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形左上点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectlf_TopLeft(LPSYPOINTD pDesP,			// 目标点
							 LPSYRECTLF pSrcR)			// 源矩形指针
{
	assert((pDesP != NULL) && (pSrcR != NULL));

	pDesP->x = pSrcR->left;
	pDesP->y = pSrcR->top;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形右下点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectlf_BottomRight(LPSYPOINTD pDesP,			// 目标点
								 LPSYRECTLF pSrcR)			// 源矩形指针
{
	assert((pDesP != NULL) && (pSrcR != NULL));

	pDesP->x = pSrcR->right;
	pDesP->y = pSrcR->bottom;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形中心点
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectlf_CenterPoint(LPSYPOINTD pDesP,			// 目标点
								 LPSYRECTLF pSrcR)			// 源矩形指针
{
	assert((pDesP != NULL) && (pSrcR != NULL));

	pDesP->x = (pSrcR->left + pSrcR->right) / 2;
	pDesP->y = (pSrcR->top + pSrcR->bottom) / 2;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩形拖动点的爪子，从左上角开始顺时针给出
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYRectlf_DragPoint(LPSYRECT* ppDesR,			// 目标矩形数组
							   LPSYRECTLF pSrcR,				// 源矩形
							   int nStyle,					// 爪子的风格
							   int nRadius)					// 方格半径
{
	assert((ppDesR != NULL) && (pSrcR != NULL));

	SYPoint point;

	// 先按矩形在线上的风格获取爪子，然后再根据风格来偏移矩阵
	// 左上点
	SYPoint_SetPoint(&point, (LONG)pSrcR->left, (LONG)pSrcR->top);
	SYRect_SetRectBPR(ppDesR[SYRECT_DRAG_LTP], point, nRadius);
	// 上中点
	SYPoint_SetPoint(&point, (LONG)((pSrcR->left + pSrcR->right) / 2), (LONG)pSrcR->top);
	SYRect_SetRectBPR(ppDesR[SYRECT_DRAG_TMP], point, nRadius);
	// 右上点
	SYPoint_SetPoint(&point, (LONG)pSrcR->right, (LONG)pSrcR->top);
	SYRect_SetRectBPR(ppDesR[SYRECT_DRAG_RTP], point, nRadius);
	// 右中点
	SYPoint_SetPoint(&point, (LONG)pSrcR->right, (LONG)((pSrcR->top + pSrcR->bottom) / 2));
	SYRect_SetRectBPR(ppDesR[SYRECT_DRAG_RMP], point, nRadius);
	// 右下点
	SYPoint_SetPoint(&point, (LONG)pSrcR->right, (LONG)pSrcR->bottom);
	SYRect_SetRectBPR(ppDesR[SYRECT_DRAG_RBP], point, nRadius);
	// 下中点
	SYPoint_SetPoint(&point, (LONG)((pSrcR->left + pSrcR->right) / 2), (LONG)pSrcR->bottom);
	SYRect_SetRectBPR(ppDesR[SYRECT_DRAG_BMP], point, nRadius);
	// 左下点
	SYPoint_SetPoint(&point, (LONG)pSrcR->left, (LONG)pSrcR->bottom);
	SYRect_SetRectBPR(ppDesR[SYRECT_DRAG_LBP], point, nRadius);
	// 左中点
	SYPoint_SetPoint(&point, (LONG)pSrcR->left, (LONG)((pSrcR->top + pSrcR->bottom) / 2));
	SYRect_SetRectBPR(ppDesR[SYRECT_DRAG_LMP], point, nRadius);
	switch (nStyle)
	{
	case SYRECT_DRAG_INLINE:
		SYRect_OffsetRect1(ppDesR[SYRECT_DRAG_LTP], nRadius, nRadius);
		SYRect_OffsetRect1(ppDesR[SYRECT_DRAG_TMP], 0, nRadius);
		SYRect_OffsetRect1(ppDesR[SYRECT_DRAG_RTP], -nRadius, nRadius);
		SYRect_OffsetRect1(ppDesR[SYRECT_DRAG_RMP], -nRadius, 0);
		SYRect_OffsetRect1(ppDesR[SYRECT_DRAG_RBP], -nRadius, -nRadius);
		SYRect_OffsetRect1(ppDesR[SYRECT_DRAG_BMP], 0, -nRadius);
		SYRect_OffsetRect1(ppDesR[SYRECT_DRAG_LBP], nRadius, -nRadius);
		SYRect_OffsetRect1(ppDesR[SYRECT_DRAG_LMP], -nRadius, 0);
		break;
	default:
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	求两个矩形的交集
//
// 返回值:	TRUE			交集不为空
//			FALSE			交集为空
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYRectlf_IntersectRect(LPSYRECTLF pDesR,			// 交集结果
								   LPSYRECTLF pSrcR1,		// 源矩形
								   LPSYRECTLF pSrcR2)		// 源矩形
{
	assert((pDesR != NULL) && (pSrcR1 != NULL) && (pSrcR2 != NULL));
	
	pDesR->left = (pSrcR1->left > pSrcR2->left) ? pSrcR1->left : pSrcR2->left;
	pDesR->top = (pSrcR1->top > pSrcR2->top) ? pSrcR1->top : pSrcR2->top;
	pDesR->right = (pSrcR1->right > pSrcR2->right) ? pSrcR2->right : pSrcR1->right;
	pDesR->bottom = (pSrcR1->bottom < pSrcR2->bottom) ? pSrcR1->bottom : pSrcR2->bottom;

	return !SYRectlf_IsRectEmpty(pDesR);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	求两个矩形的并集
//
// 返回值:	TRUE			并集不为空
//			FALSE			并集为空
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYRectlf_UnionRect(LPSYRECTLF pDesR,			// 并集结果
							   LPSYRECTLF pSrcR1,		// 源矩形
							   LPSYRECTLF pSrcR2)		// 源矩形
{
	assert((pDesR != NULL) && (pSrcR1 != NULL) && (pSrcR2 != NULL));
			
	pDesR->left = (pSrcR1->left < pSrcR2->left) ? pSrcR1->left : pSrcR2->left;
	pDesR->top = (pSrcR1->top < pSrcR2->top) ? pSrcR1->top : pSrcR2->top;
	pDesR->right = (pSrcR1->right < pSrcR2->right) ? pSrcR2->right : pSrcR1->right;
	pDesR->bottom = (pSrcR1->bottom > pSrcR2->bottom) ? pSrcR1->bottom : pSrcR2->bottom;

	return SYRectlf_IsRectEmpty(pDesR);
}

#include <emmintrin.h>
void __cdecl SYRectf_UnionPoint( rectf* pDesR, /* 并集结果 */ rectf* pSrcR1, /* 源矩形 */ float fX,float fY )
{
	__m128 m1 = _mm_loadu_ps((float*)pDesR);
	__m128 m2 = m1;
	__m128 pt = _mm_set_ps(fY,fX,fY,fX);
	m1 = _mm_max_ps(m1,pt);
	m2 = _mm_min_ps(m2,pt);
	m1 = _mm_shuffle_ps(m2,m1,_MM_SHUFFLE(3,2,1,0));
	_mm_storeu_ps((float*)pDesR,m1);
}

void __cdecl SYRectf_To_Rect( RECT* pDst,const rectf* pSrc )
{
	__m128 m1 = _mm_loadu_ps((float*)pSrc);
	__m128 round = _mm_set_ps1(0.5f);
	m1 = _mm_add_ps(m1,round);
	__m128i dst = _mm_cvtps_epi32(m1);
	_mm_storeu_si128((__m128i*)pDst,dst);
}

///////////////////////////// End of SYRect ///////////////////////////////////
//------------------------------- End of File ---------------------------------
