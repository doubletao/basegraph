#include "stdafx.h"
#include "DataRender\Arc2BezierAPI.h"
#include <math.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
int SolveSqure(double a, double b, double c, double *xx)
{
	//形如a*xx^2 +b*xx +c=0(a != 0)的一元二次方程的实数解。由*xx返回。
	// 注意: 默认 a != 0
	double delta = b*b - 4*a*c;
	if (delta < 0) 
	{
		return 0;
	}

	double sqrtDelta = sqrt(delta);
	if (sqrtDelta < ABS_FLOAT_0) 
	{
		xx[0] = -b / a * 0.5;
		return 1;
	}

	xx[0] = (-b + sqrtDelta ) / a * 0.5;
	xx[1] = (-b - sqrtDelta ) / a * 0.5;
	return 2;	
} // END OF SolveSqure
////////////////////////////////////////////////////////////////////////// begin
// 计算三次Bezier曲线的最小外包矩形
////////////////////////////////////////////////////////////////////////// end
BASEGRAPH_DATARENDER_API void CalBezier3Bound(IN const point2f *pt, // 四个控制顶点
	OUT rectf &rt // 外包矩形
	)
{
	// 分如下三步:
	// 1. 首先用起讫点给外包赋初值
	// 2. 其次, 求 y'(t) = 0 的点, 重新确定矩形外包
	// 3. 再次, 求 x'(t) = 0 的点, 重新确定矩形外包

	//ASSERT(pt != 0);
	if(pt == 0)
	{
		rt.bottom = rt.left = rt.right = rt.top = 0;
		return;
	}

	int i; 
	//-----------------------------------------------------------------
	// 1. 首先用起讫点给外包赋初值
	rt.left = rt.right = pt[0].x;
	rt.top = rt.bottom = pt[0].y;
	if (pt[3].x > rt.right) 
		rt.right = pt[3].x;
	if (pt[3].x < rt.left) 
		rt.left = pt[3].x;
	if (pt[3].y > rt.bottom) 
		rt.bottom = pt[3].y;
	if (pt[3].y < rt.top) 
		rt.top = pt[3].y;

	//-----------------------------------------------------------------
	// 2. 其次, 求 y'(t) = 0 的点, 重新确定矩形外包
	double dParam[2]; // 导数为0的参数值
	int nSolveNum = 0; //解的个数
	double a, b, c; // a, b, c分别为Bezier曲线参数t的三次,二次,一次的系数. 
	a = -pt[0].y + 3.0 * pt[1].y - 3.0 * pt[2].y + pt[3].y;
	b = 2.0 * pt[0].y - 4.0 * pt[1].y +  2.0 * pt[2].y;
	c = - pt[0].y + pt[1].y;

	// 退化为小于2次
	if ( abs(a) <= ABS_FLOAT_0 && abs(b) <= ABS_FLOAT_0 ) 
	{
		//nSolveNum = 0; 
	}
	// 退化为2次, 
	else if ( abs(a) <= ABS_FLOAT_0 && abs(b) > ABS_FLOAT_0 ) 
	{
		dParam[0] = - c / b;
		nSolveNum = 1; 
	}
	// 为3次, 
	else 
	{
		nSolveNum = SolveSqure(a, b, c, dParam);
		// 还要判断哪些是有效根: 落入区间[0, 1]才算有效
	}

	float fTemp;
	for (i = 0; i < nSolveNum; i++) 
	{
		if (dParam[i] > 1.0 || dParam[i] < 0.0) 
		{
			continue;
		}
		fTemp = pt[0].y * (1 - dParam[i]) * (1 - dParam[i]) * (1 - dParam[i]) + 3.f * pt[1].y * dParam[i] * (1 - dParam[i]) * (1 - dParam[i]) + 3.f * pt[2].y * dParam[i] * dParam[i] * (1 - dParam[i]) + pt[3].y * dParam[i] * dParam[i] * dParam[i];
		if ( fTemp > rt.bottom ) 
			rt.bottom = fTemp;
		if ( fTemp < rt.top ) 
			rt.top = fTemp;
	}

	//-----------------------------------------------------------------
	// 3. 再次, 求 x'(t) = 0 的点, 重新确定矩形外包
	a = -pt[0].x + 3.0 * pt[1].x - 3.0 * pt[2].x + pt[3].x;
	b = 2.0 * pt[0].x - 4.0 * pt[1].x + 2.0 * pt[2].x;
	c = -pt[0].x + pt[1].x;

	// 退化为小于2次
	if ( abs(a) <= ABS_FLOAT_0 && abs(b) <= ABS_FLOAT_0 ) 
	{
		nSolveNum = 0; 
	}
	// 退化为2次, 
	else if ( abs(a) <= ABS_FLOAT_0 && abs(b) > ABS_FLOAT_0 ) 
	{
		dParam[0] = - c / b;
		nSolveNum = 1; 
	}
	// 为3次, 
	else 
	{
		nSolveNum = SolveSqure(a, b, c, dParam);
		// 还要判断哪些是有效根: 落入区间[0, 1]才算有效
	}

	for (i = 0; i < nSolveNum; i++) 
	{
		if (dParam[i] > 1.0 || dParam[i] < 0.0) 
		{
			continue;
		}
		fTemp = pt[0].x * (1 - dParam[i]) * (1 - dParam[i]) * (1 - dParam[i]) + 3.f * pt[1].x * dParam[i] * (1 - dParam[i]) * (1 - dParam[i]) + 3.f * pt[2].x * dParam[i] * dParam[i] * (1 - dParam[i]) + pt[3].x * dParam[i] * dParam[i] * dParam[i];
		if ( fTemp > rt.right ) 
			rt.right = fTemp;
		if ( fTemp < rt.left ) 
			rt.left = fTemp;
	}
} // END OF CalBezier3Bound

////////////////////////////////////////////////////////////////////////// begin
// 用三次Bezier逼近圆弧时的分类
// 1，假设是完整圆，则先分成3份
// 2，否则，2.1 若大于240度，则划分为3份
//                      2.2  否则，若大于120度，则划分为2份
//                      2.3  否则，直接逼近
////////////////////////////////////////////////////////////////////////// end
// 用三次Bezier逼近圆弧:圆弧要等分成多少段
BASEGRAPH_DATARENDER_API int Arc2Bezier_SplitNum(IN const point2f ptfCenter, // 圆心
						IN const float fRadius, // 半径
						IN const BOOL bCircle, // 是否整圆
						IN const float fAngleStartIN, // 起始角度 
						IN const float fAngleEndIN, // 终止角度 默认终止角度大于起始角度,且都在0-2*PI之间
						IN const float fThreshold, // 误差阈值
						IN OUT int & nArcs // 圆弧等分段数
						)
{
	//nArcs = 1;
	float fStartAngle = fAngleStartIN;
	float fEndAngle = fAngleEndIN;
	// 完整圆
	if (bCircle) 
	{
		nArcs = 3;
		fStartAngle = 0.f;
		fEndAngle = PAI * 0.6666667f;
	}
	else
	{
		if (fAngleEndIN - fAngleStartIN > PAI * 4 / 3)  // 大于240度
		{
			nArcs = 3;
			fEndAngle = fStartAngle + (fEndAngle - fStartAngle) /3;
		}
		else if (fAngleEndIN - fAngleStartIN > PAI * 2 / 3) // 大于120度
		{
			nArcs = 2;
			fEndAngle = fStartAngle + (fEndAngle - fStartAngle) /2;
		}
		else
		{
			nArcs = 1;
		}
	}

	// 对片段进行逼近
	DividedArc2Bezier(ptfCenter, fRadius, fStartAngle, fEndAngle, fThreshold, nArcs);
	return nArcs;

} // END OF Arc2Bezier_SplitNum


// 用三次Bezier逼近圆弧: 得到控制顶点数组
BASEGRAPH_DATARENDER_API void Arc2Bezier_GetCtrlPts(IN const point2f ptfCenter, // 圆心
						   IN const float fRadius, // 半径
						   IN const float fAngleStart, // 起始角度 
						   IN const float fAngleEnd, // 终止角度 默认终止角度大于起始角度,且都在0-2*PI之间
						   IN const int nArcs, // 圆弧等分段数
						   OUT point2f* ptfCtrlPts // 逼近结果 控制顶点数组 数组大小：nArcs * 3 + 1 
						   )
{
	int i = 0;
	int j = 0;
	int k = 0;
	point2f ptfCtrl[4];
	float fAngleGap = (fAngleEnd-fAngleStart)/nArcs;
	float fStart = fAngleStart, fEnd = fAngleStart + fAngleGap;
	Get4CtrlPts(ptfCenter, fRadius, fStart, fEnd, ptfCtrl);
	for (i=0; i<4; i++)
	{
		ptfCtrlPts[k].x = ptfCtrl[i].x;
		ptfCtrlPts[k].y = ptfCtrl[i].y;
		k++;
	} // end of for

	// 可以在此处全部添加
	for (i=1; i<nArcs; i++)
	{
		fStart = fEnd;
		fEnd += fAngleGap;
		Get4CtrlPts(ptfCenter, fRadius, fStart, fEnd, ptfCtrl);
		for (j=1; j<4; j++)
		{
			ptfCtrlPts[k].x = ptfCtrl[j].x;
			ptfCtrlPts[k].y = ptfCtrl[j].y;
			k++;
		} // end of inner-for
	} // end of outer-for

} // END OF Arc2Bezier_GetCtrlPts

//----------------------------------
// 用三次Bezier逼近圆
//----------------------------------
BASEGRAPH_DATARENDER_API void DividCircle2Bezier(
    IN const point2f ptCenter, //圆心
    IN const float fRadius_a, // 半径
    IN const float fRadius_b, // 半径
    OUT SYArray &aBezier // 逼近结果 4个point2f型顶点 
    )
{
    DividQuarterArc2Bezier(ptCenter, fRadius_a, fRadius_b, ARCLOC_RIGHTTOP, aBezier);  //第一象限
	point2f* pPt = (point2f*)SYAR_GetAt(&aBezier, (int)SYAR_GetSize(&aBezier) - 1);
    SYAR_RemoveAt(&aBezier, (int)SYAR_GetSize(&aBezier) - 1 , 1);                    //移除最后一个点
	delete pPt;    
	DividQuarterArc2Bezier(ptCenter, fRadius_a, fRadius_b, ARCLOC_LEFTTOP, aBezier);   //第二象限
	pPt = (point2f*)SYAR_GetAt(&aBezier, (int)SYAR_GetSize(&aBezier) - 1);
    SYAR_RemoveAt(&aBezier, (int)SYAR_GetSize(&aBezier) - 1, 1);                    //移除最后一个点
	delete pPt;
    DividQuarterArc2Bezier(ptCenter, fRadius_a, fRadius_b, ARCLOC_LEFTBOTTOM, aBezier);   //第三象限
	pPt = (point2f*)SYAR_GetAt(&aBezier, (int)SYAR_GetSize(&aBezier) - 1);
    SYAR_RemoveAt(&aBezier, (int)SYAR_GetSize(&aBezier) - 1, 1);                    //移除最后一个点
	delete pPt;
    DividQuarterArc2Bezier(ptCenter, fRadius_a, fRadius_b, ARCLOC_RIGHTBOTTOM, aBezier);   //第四象限
}

//----------------------------------
// 得到起点所在象限
//----------------------------------
void GetStartQuadrant( IN const float fStartX,   // 起点坐标
					   IN const float fStartY, 
					   IN const point2f ptCenter, // 圆心
					   OUT int &nStartQuadrant)  // 起点所在象限
{
	if ( fStartX > ptCenter.x && fabs(fStartY - ptCenter.y) < ABS_FLOAT_0 )
	{
		// 起点在第一象限
		nStartQuadrant = 1;
	}
	else if ( fStartX < ptCenter.x && fabs(fStartY - ptCenter.y) < ABS_FLOAT_0 )
	{
		// 起点在第三象限
		nStartQuadrant = 3;
	}
	else if ( fStartY < ptCenter.y && fabs(fStartX - ptCenter.x) < ABS_FLOAT_0 )
	{
		// 起点在第二象限
		nStartQuadrant = 2;
	}
	else if ( fStartY > ptCenter.y && fabs(fStartX - ptCenter.x) < ABS_FLOAT_0 )
	{
		// 起点在第四象限
		nStartQuadrant = 4;
	}
	else if ( fStartX > ptCenter.x && fStartY < ptCenter.y )
	{
		// 起点在第一象限
		nStartQuadrant = 1;
	}
	else if ( fStartX < ptCenter.x && fStartY < ptCenter.y )
	{
		// 起点在第二象限
		nStartQuadrant = 2;
	}
	else if ( fStartX < ptCenter.x && fStartY > ptCenter.y )
	{
		// 起点在第三象限
		nStartQuadrant = 3;
	}
	else if ( fStartX > ptCenter.x && fStartY > ptCenter.y )
	{
		// 起点在第四象限
		nStartQuadrant = 4;
	}
}

//----------------------------------
// 得到终点所在象限
//----------------------------------
void GetEndQuadrant( IN const float fEndX,     // 终点坐标
					 IN const float fEndY, 
					 IN const point2f ptCenter, // 圆心
					 OUT int &nEndQuadrant)    // 终点所在象限
{
	if ( fEndX > ptCenter.x && fabs(fEndY - ptCenter.y) < ABS_FLOAT_0 )
	{
		// 终点在第四象限
		nEndQuadrant = 4;
	}
	else if ( fEndX < ptCenter.x && fabs(fEndY - ptCenter.y) < ABS_FLOAT_0 )
	{
		// 终点在第二象限
		nEndQuadrant = 2;
	}
	else if ( fEndY < ptCenter.y && fabs(fEndX - ptCenter.x) < ABS_FLOAT_0 )
	{
		// 终点在第一象限
		nEndQuadrant = 1;
	}
	else if ( fEndY > ptCenter.y && fabs(fEndX - ptCenter.x) < ABS_FLOAT_0 )
	{
		// 终点在第三象限
		nEndQuadrant = 3;
	}
	else if ( fEndX > ptCenter.x && fEndY > ptCenter.y )
	{
		// 终点在第四象限
		nEndQuadrant = 4;
	}
	else if ( fEndX < ptCenter.x && fEndY > ptCenter.y )
	{
		// 终点在第三象限
		nEndQuadrant = 3;
	}
	else if ( fEndX < ptCenter.x && fEndY < ptCenter.y )
	{
		// 终点在第二象限
		nEndQuadrant = 2;
	}
	else if ( fEndX > ptCenter.x && fEndY < ptCenter.y )
	{
		// 终点在第一象限
		nEndQuadrant = 1;
	}
}

//-------------------------------------------------------------------
//判断点是否落在四个点的外包矩形内
//-------------------------------------------------------------------
BOOL SYPointInCurveRect( IN point2f point,IN point2f pt1,IN point2f pt2, IN point2f pt3,IN point2f pt4)
{
	BOOL bx1 = point.x <= pt1.x || point.x <= pt2.x || point.x <= pt3.x || point.x <= pt4.x;
	BOOL bx2 = point.x >= pt1.x || point.x >= pt2.x || point.x >= pt3.x || point.x >= pt4.x;
	BOOL by1 = point.y <= pt1.y || point.y <= pt2.y || point.y <= pt3.y || point.y <= pt4.y;
	BOOL by2 = point.y >= pt1.y || point.y >= pt2.y || point.y >= pt3.y || point.y >= pt4.y;
	if (bx1 && bx2 && by1 && by2)
	{
		return TRUE;
	}
	return FALSE;
}

//-------------------------------------------------------------------
// 由点得到对应在贝塞尔曲线的参数t（[0，1]）
//-------------------------------------------------------------------
float SYGetBezierTByPoint(IN point2f point,IN point2f P1,IN point2f P2,IN point2f P3,IN point2f P4)
{
	point2f P12,P23,P34,P1223,P2334,P0;
	P12.x = (P1.x + P2.x) * 0.5f;		// P12 = (P1 + P2) / 2
	P12.y = (P1.y + P2.y) * 0.5f;
	P23.x = (P2.x + P3.x) * 0.5f;		// P23 = (P2 + P3) / 2
	P23.y = (P2.y + P3.y) * 0.5f;
	P34.x = (P3.x + P4.x) * 0.5f;		// P34 = (P3 + P4) / 2
	P34.y = (P3.y + P4.y) * 0.5f;
	P1223.x = (P12.x + P23.x) * 0.5f;		// P1223 = (P12 + P23) / 2
	P1223.y = (P12.y + P23.y) * 0.5f;
	P2334.x = (P23.x + P34.x) * 0.5f;		// P2334 = (P23 + P34) / 2
	P2334.y = (P23.y + P34.y) * 0.5f;
	P0.x = (P1223.x + P2334.x) * 0.5f;		// P0 = (P1223 + P2334) / 2
	P0.y = (P1223.y + P2334.y) * 0.5f;

	if( sqrt((P0.x - point.x) * (P0.x - point.x) + (P0.y - point.y) * (P0.y - point.y)) < 3.0 )
	{
		return 0.5f;
	}
	else 
	{
		if( SYPointInCurveRect(point,P1,P12,P1223,P0) )    //是否落在矩形包内
		{
			return 0.5f * SYGetBezierTByPoint(point,P1,P12,P1223,P0);
		}
		else if( SYPointInCurveRect(point,P0,P2334,P34,P4) )
		{
			return 1.0f - 0.5f * (1.0f - SYGetBezierTByPoint(point,P0,P2334,P34,P4));
		}
	}
	return 1.0f;
}

//-------------------------------------------------------------------
//DivBezier 
//功能：将一条三次贝塞尔曲线在参数t（[0，1]）处划分成两条贝塞尔曲线
//输入：POINTF3D pt[4]   原四个控制顶点
//      float  t		 原曲线参数t
//输出：POINTF3D pt1[4], 划分后第一段三次贝塞尔曲线的四个控制顶点
//      POINTF3D pt2[4], 划分后第二段三次贝塞尔曲线的四个控制顶点
//-------------------------------------------------------------------
void SYDivBezier(IN point2f pt[4], OUT point2f pt1[4],OUT point2f pt2[4],IN float t)
{
	assert( t >= 0.0f && t <= 1.0f ); 
	float P01x = (1.0f - t) * pt[0].x + t * pt[1].x;
	float P01y = (1.0f - t) * pt[0].y + t * pt[1].y;

	float P11x = (1.0f - t) * pt[1].x + t * pt[2].x;
	float P11y = (1.0f - t) * pt[1].y + t * pt[2].y;

	float P21x = (1.0f - t) * pt[2].x + t * pt[3].x;
	float P21y = (1.0f - t) * pt[2].y + t * pt[3].y;

	float P02x = (1.0f - t) * P01x + t * P11x;
	float P02y = (1.0f - t) * P01y + t * P11y;

	float P12x = (1.0f - t) * P11x + t * P21x;
	float P12y = (1.0f - t) * P11y + t * P21y;

	float P03x = (1.0f - t) * P02x + t * P12x;
	float P03y = (1.0f - t) * P02y + t * P12y;

	pt1[0] = pt[0];
	pt1[1].x = P01x;
	pt1[1].y = P01y;
	pt1[2].x = P02x;
	pt1[2].y = P02y;
	pt1[3].x = P03x;
	pt1[3].y = P03y;

	pt2[0]   = pt1[3];
	pt2[1].x = P12x;
	pt2[1].y = P12y;
	pt2[2].x = P21x;
	pt2[2].y = P21y;
	pt2[3]   = pt[3];
}

//----------------------------------
// 用三次Bezier逼近椭圆弧
//----------------------------------
BASEGRAPH_DATARENDER_API void DividArc2Bezier(
    IN const point2f ptCenter,       //圆心
    IN const float fRadius_a,       // 沿x轴半径
    IN const float fRadius_b,       // 沿y轴半径
	IN const float fStartRadIn,		// 起始弧度
	IN const float fEndRad,			// 终止弧度
    OUT SYArray &aBezier			// 逼近结果 4个point2f型顶点 
    )
{
	int nSize = (int)SYAR_GetSize(&aBezier);
	for (int i = 0; i < nSize; i++)
	{
		point2f * pptf = (point2f *)SYAR_GetAt( &aBezier, i);
		delete pptf;
	}
	SYAR_RemoveAll(&aBezier);

	// 一个问题: 如果fStartRad == fEndRad, 怎么处理?
	float fStartRad = fStartRadIn;
	while(fStartRad > fEndRad)
    {
        fStartRad = (float)(fStartRad - 2 * PAI);
    }

	// 求出起点, 看在哪个象限. 
	// 求出终点, 看在哪个象限.
	// 求出所位于象限, 按照1/4圆弧求出所有的圆弧
	// 求交. 得到结果

	// 求出起点
	point2f ptStart;
	GetEllipsePtPos(fStartRad, fRadius_a, fRadius_b, &ptStart);
	ptStart.y = -ptStart.y;
	ptStart.x += ptCenter.x;
	ptStart.y += ptCenter.y;
	float fStartX =  ptStart.x;
	float fStartY = ptStart.y;
	// 求出终点
	point2f ptEnd;
	GetEllipsePtPos(fEndRad, fRadius_a, fRadius_b, &ptEnd);
	ptEnd.y = -ptEnd.y;
	ptEnd.x += ptCenter.x;
	ptEnd.y += ptCenter.y;
	float fEndX =  ptEnd.x;
	float fEndY = ptEnd.y;

	// 看起讫点在哪个象限
	int nStartQuadrant = 1;
	int nEndQuadrant = 1;

	// 看起点在哪个象限
	GetStartQuadrant(fStartX, fStartY, ptCenter, nStartQuadrant);

	// 看终点在哪个象限
	GetEndQuadrant(fEndX, fEndY, ptCenter, nEndQuadrant);
	
	// 根据起讫点所在象限, 得到所有覆盖象限内的曲线.
	BOOL bInitialized = FALSE;
	// 如下if内, 只处理原始只有一段Bezier的情况
	if ( nStartQuadrant == nEndQuadrant )
	{
		// 起讫点在同一个象限内
		switch(nStartQuadrant)
		{
		case 1:
			{
				if ( fEndX < fStartX )
				{
					// 第一象限的1/4椭圆弧作为初始
					DividQuarterArc2Bezier(ptCenter, fRadius_a, fRadius_b, ARCLOC_RIGHTTOP, aBezier);  //第一象限
                    bInitialized = TRUE;
				}
			}
			break;
		case 2:
			{
				if ( fEndX < fStartX )
				{
					// 第二象限的1/4椭圆弧作为初始
					DividQuarterArc2Bezier(ptCenter, fRadius_a, fRadius_b, ARCLOC_LEFTTOP, aBezier);   //第二象限
                    bInitialized = TRUE;
				}
			}
			break;
		case 3:
			{
				if ( fEndX > fStartX )
				{
					// 第三象限的1/4椭圆弧作为初始
					DividQuarterArc2Bezier(ptCenter, fRadius_a, fRadius_b, ARCLOC_LEFTBOTTOM, aBezier);   //第三象限
                    bInitialized = TRUE;
				}
			}
			break;
		case 4:
			{
				if ( fEndX > fStartX )
				{
					// 第四象限的1/4椭圆弧作为初始
					DividQuarterArc2Bezier(ptCenter, fRadius_a, fRadius_b, ARCLOC_RIGHTBOTTOM, aBezier);   //第四象限
                    bInitialized = TRUE;
				}
			}
			break;
		}
	}
	
    if (!bInitialized)
	{
		// 初始为多段半椭圆弧
		if (nEndQuadrant <= nStartQuadrant)
		{
			nEndQuadrant += 4;
		}

	    for ( int i = nStartQuadrant; i < nEndQuadrant; i++)
		{
            if (i % 4 == 0)
            {
                DividQuarterArc2Bezier(ptCenter, fRadius_a, fRadius_b, (ARCLOC)4, aBezier); 
            }
            else
            {
                DividQuarterArc2Bezier(ptCenter, fRadius_a, fRadius_b, (ARCLOC) (i % 4), aBezier); 
            }
			point2f* pPt = (point2f*)SYAR_GetAt(&aBezier, (int)SYAR_GetSize(&aBezier) - 1);
			SYAR_RemoveAt(&aBezier, (int)SYAR_GetSize(&aBezier) - 1 , 1);
			//内存泄漏
			if (pPt)
				SAFE_DELETE(pPt);
		}
        if (nEndQuadrant % 4 == 0)
        {
            DividQuarterArc2Bezier(ptCenter, fRadius_a, fRadius_b, (ARCLOC)4, aBezier); 
        }
        else
        {
            DividQuarterArc2Bezier(ptCenter, fRadius_a, fRadius_b, (ARCLOC) (nEndQuadrant % 4), aBezier); 
        }
	}

	point2f ptfBezier[4];
	point2f *pptf = NULL;
	// 用起点剪裁第一个半圆弧
	// 取出前面四个点
	for (int i = 0; i < 4; i++)
	{
		pptf = (point2f *)SYAR_GetAt(&aBezier, i);
		ptfBezier[i] = *pptf;
	}

	float fDivU = SYGetBezierTByPoint( ptStart,ptfBezier[0], ptfBezier[1], ptfBezier[2], ptfBezier[3]);
	
	point2f ptNew1[4];
	point2f ptNew2[4];
	SYDivBezier( ptfBezier, ptNew1, ptNew2, fDivU);
	
	for (int i = 0; i < 3; i++)
	{
		pptf = (point2f *)SYAR_GetAt(&aBezier, i);
		*pptf = ptNew2[i];
	}

	// 用终点剪裁最后一个半圆弧
	// 取出最后四个点
	nSize = (int)SYAR_GetSize(&aBezier);
	for (int i = 0; i < 4; i++)
	{
		pptf = (point2f *)SYAR_GetAt(&aBezier, nSize - 4 + i);
		ptfBezier[i] = *pptf;
	}
	
	fDivU = SYGetBezierTByPoint( ptEnd,ptfBezier[0], ptfBezier[1], ptfBezier[2], ptfBezier[3]);
	
	SYDivBezier( ptfBezier, ptNew1, ptNew2, fDivU);
	
	for (int i = 1; i < 4; i++)
	{
		pptf = (point2f *)SYAR_GetAt(&aBezier, nSize - 4 + i);
		*pptf = ptNew1[i];
	}
}

//----------------------------------
// 用三次Bezier逼近1/4椭圆，点为逆时针
//----------------------------------
BASEGRAPH_DATARENDER_API void DividQuarterArc2Bezier(
                    IN  const point2f ptCenter,   //圆心
                    IN  const float  fRadius_a,   // 半径
                    IN  const float  fRadius_b,   // 半径
                    IN  ARCLOC       loc,        //圆弧位置
                    OUT SYArray&     arPt
					   )
{
    float fTemp = (sqrtf(2.f) - 1) * 4 / 3.f;
	float a = fTemp * fRadius_a;
    float b = fTemp * fRadius_b;
    point2f* pPt = NULL;
    switch(loc)
    {
    case ARCLOC_RIGHTTOP:              //第一象限
        pPt = new point2f;                   SYAR_Add(&arPt, pPt);
        pPt->x = fRadius_a + ptCenter.x;    pPt->y = ptCenter.y;
        
        pPt = new point2f;                   SYAR_Add(&arPt, pPt);
        pPt->x = fRadius_a + ptCenter.x;    pPt->y = - b + ptCenter.y;
        
        pPt = new point2f;                   SYAR_Add(&arPt, pPt);
        pPt->x = a + ptCenter.x;            pPt->y = - fRadius_b + ptCenter.y;
        
        pPt = new point2f;                   SYAR_Add(&arPt, pPt);
        pPt->x = ptCenter.x;                pPt->y = - fRadius_b + ptCenter.y;
        
        break;  
    case ARCLOC_LEFTTOP:               //第二象限
        pPt = new point2f;                   SYAR_Add(&arPt, pPt);
        pPt->x = ptCenter.x;                pPt->y = - fRadius_b + ptCenter.y;
        
        pPt = new point2f;                   SYAR_Add(&arPt, pPt);
        pPt->x = - a + ptCenter.x;          pPt->y = - fRadius_b + ptCenter.y;
        
        pPt = new point2f;                   SYAR_Add(&arPt, pPt);
        pPt->x = -fRadius_a + ptCenter.x;   pPt->y = -b + ptCenter.y;
        
        pPt = new point2f;                   SYAR_Add(&arPt, pPt);
        pPt->x = -fRadius_a  + ptCenter.x;  pPt->y = ptCenter.y;
        
        break;
    case ARCLOC_LEFTBOTTOM:            //第三象限
        pPt = new point2f;                   SYAR_Add(&arPt, pPt);
        pPt->x = -fRadius_a + ptCenter.x;   pPt->y = ptCenter.y;
        
        pPt = new point2f;                   SYAR_Add(&arPt, pPt);
        pPt->x = -fRadius_a + ptCenter.x;   pPt->y = b + ptCenter.y;

        pPt = new point2f;                   SYAR_Add(&arPt, pPt);
        pPt->x = - a + ptCenter.x;          pPt->y = fRadius_b + ptCenter.y;
        
        pPt = new point2f;                   SYAR_Add(&arPt, pPt);
        pPt->x = ptCenter.x;                pPt->y = fRadius_b + ptCenter.y;
        
        break;

    case ARCLOC_RIGHTBOTTOM:           //第四象限
        pPt = new point2f;                   SYAR_Add(&arPt, pPt);
        pPt->x = ptCenter.x;                pPt->y = fRadius_b + ptCenter.y;

        pPt = new point2f;                   SYAR_Add(&arPt, pPt);
        pPt->x = a + ptCenter.x;            pPt->y = fRadius_b + ptCenter.y;

        pPt = new point2f;                   SYAR_Add(&arPt, pPt);
        pPt->x = fRadius_a + ptCenter.x;    pPt->y = b + ptCenter.y;

        pPt = new point2f;                   SYAR_Add(&arPt, pPt);
        pPt->x = fRadius_a + ptCenter.x;    pPt->y = ptCenter.y;

        break;
    }
    
}


// 用三次Bezier逼近圆弧片段
BASEGRAPH_DATARENDER_API void DividedArc2Bezier(IN const point2f ptfCenter, // 圆心
					   IN const float fRadius, // 半径
					   IN const float fAngleStart, // 起始角度 
					   IN OUT float fAngleEnd, // 终止角度 默认终止角度大于起始角度,且都在0-2*PI之间
					   IN const float fThreshold, // 误差阈值
					   //IN OUT point2f * ptfCtrlPts, // 单段逼近结果 4个控制顶点
					   //OUT CPtrArray *  pArrBezier, // 逼近结果 控制顶点数组
					   IN OUT int & nArcs // 圆等分段数
					   )
{
	// 算法步骤：
	//1，	三分圆，或四分圆
	//2，	求逼近的Bezier曲线
	//	求最佳逼近Bezier曲线，或者用固定的方式求出Bezier曲线
	//3，	求最大逼近误差
	//4，	判断最大逼近误差是否满足误差要求

	// 分段进行逼近
	//2，	求逼近的Bezier曲线
	//	        求最佳逼近Bezier曲线，或者用固定的方式求出Bezier曲线
	//          暂时先用中点定中间两个控制顶点.
	point2f ptfCtrl[4];
	// 在这里,可以选择各种实现方式
	Get4CtrlPts(ptfCenter, fRadius, fAngleStart, fAngleEnd, ptfCtrl);

	//3，	求最大逼近误差
	float fErr = GetMaxDis(ptfCenter,  fRadius, ptfCtrl );

	//4，	判断最大逼近误差是否满足误差要求
	if (fErr <= fThreshold) 
	{
		return;
	}
	else
	{
		if (fErr > 2.f* fThreshold) 
		{
			nArcs *= 2;
			fAngleEnd = fAngleStart + (fAngleEnd - fAngleStart) /2;
		}
		else
		{
			fAngleEnd = fAngleStart + nArcs*(fAngleEnd - fAngleStart) /(nArcs+1);
			nArcs++;
		}

		DividedArc2Bezier(ptfCenter, fRadius, fAngleStart, fAngleEnd, fThreshold, nArcs);
	}
} // END OF DividedArc2Bezier

// 得到逼近一段圆弧的Bezier曲线
BASEGRAPH_DATARENDER_API void Get4CtrlPts(
				 IN const point2f ptfCenter, // 圆心
				 IN const float fRadius, // 半径
				 IN const float fAngleStart, // 起始角度
				 IN const float fAngleEnd, // 终止角度
				 OUT point2f * ptfCtrl // 输出: 四个控制顶点
				 )
{	
	//////////////////////////////////////////////////////////////////////////
	// 圆心O：ptfCenter
	// 弧端点： A 
	//						 B
	// A, B处切线的交点：Ｑ
	// OQ方向：A+B-2*O再归一化
	// OQ长度：|OQ| = fRadius / cos a
	// 其中a = PI / nArcs = (fAngleEnd - fAngleStart)/2
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 1,求A，B
	// 2,求Q
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	float a = (fAngleEnd - fAngleStart)*0.5f; //PI / nArcs;

	// 1,求A，B
	point2f A, B, Q;
	A.x = ptfCenter.x + fRadius * cos(fAngleStart);
	A.y = ptfCenter.y - fRadius * sin(fAngleStart);

	B.x = ptfCenter.x + fRadius * cos(fAngleEnd);
	B.y = ptfCenter.y - fRadius * sin(fAngleEnd);

	// 2,求Q
	// OQ 长度
	float fDisOQ = fRadius / cos(a);
	// OQ 方向
	point2f fV;
	fV.x = A.x + B.x - ptfCenter.x - ptfCenter.x;
	fV.y = A.y + B.y - ptfCenter.y - ptfCenter.y;
	//// 注意到Ａ点的坐标值，故可简化为
	//fV.x = fRadius + B.x  - ptfCenter.x;
	//fV.y = B.y - ptfCenter.y;
	float fDisTemp = sqrtf( fV.x*fV.x + fV.y*fV.y );
	Q.x = ptfCenter.x + fDisOQ * fV.x / fDisTemp;
	Q.y = ptfCenter.y + fDisOQ * fV.y / fDisTemp;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 求控制顶点
	// 说明：
	// ptfCtrl[0] 就是A
	// ptfCtrl[1] 就是(A+Q)/2
	// ptfCtrl[2] 就是(B+Q)/2
	// ptfCtrl[3] 就是B
	//////////////////////////////////////////////////////////////////////////
	ptfCtrl[0] = A;

	ptfCtrl[1].x = (A.x+Q.x)*0.5f;
	ptfCtrl[1].y = (A.y+Q.y)*0.5f;

	ptfCtrl[2].x = (B.x+Q.x)*0.5f;
	ptfCtrl[2].y = (B.y+Q.y)*0.5f;

	ptfCtrl[3] = B;

} // END OF Get4CtrlPts

// 计算最大误差
BASEGRAPH_DATARENDER_API float GetMaxDis(IN point2f ptfCenter,  // 圆心
				IN float fRadius, //半径
				IN point2f * ptfCtrl // Bezier控制顶点
				)
{
	float fMaxDis = 0.0f;
	// 求Bezier中点到圆心的距离　与圆半径之差
	point2f ptfMid;
	ptfMid.x = (ptfCtrl[0].x + 3*ptfCtrl[1].x + 3*ptfCtrl[2].x + ptfCtrl[3].x ) * 0.125f;
	ptfMid.y = (ptfCtrl[0].y + 3*ptfCtrl[1].y + 3*ptfCtrl[2].y + ptfCtrl[3].y ) * 0.125f;
	fMaxDis = abs( sqrtf( (ptfMid.x - ptfCenter.x) * (ptfMid.x - ptfCenter.x) + (ptfMid.y - ptfCenter.y)*(ptfMid.y - ptfCenter.y) )
		- fRadius);
	return fMaxDis;
} // END OF GetMaxDis


//创建椭圆矢量图
BASEGRAPH_DATARENDER_API BOOL CreateEllipse(float fStartRad,float fEndRad,float a,float b,CSYFloatArray* pArray)
{
	while(fStartRad > fEndRad)
	{
		fStartRad = fStartRad - 2 * PAIF;
	}
// 	float fStartX = cos(fStartRad)*b;
// 	float fStartY = -sin(fStartRad)*a;
// 	float fEndX = cos(fEndRad)*b;
// 	float fEndY = -sin(fEndRad)*a;
	point2f ptStart, ptEnd;
	GetEllipsePtPos(fStartRad, a, b, &ptStart);
	GetEllipsePtPos(fEndRad, a, b, &ptEnd);
	float fdis = (ptEnd.x-ptStart.x)*(ptEnd.x-ptStart.x) + (ptEnd.y-ptStart.y)*(ptEnd.y-ptStart.y);
	if(fdis < 16 && abs(fEndRad - fStartRad) < PAIF)
	{
		pArray->Add(fEndRad);
	}
	else
	{
		float fCenterRad = (fEndRad+fStartRad)/2;
		CreateEllipse(fStartRad,fCenterRad,a,b,pArray);
		CreateEllipse(fCenterRad,fEndRad,a,b,pArray);
	}
	return TRUE;
}

//得到椭圆边界点坐标(相对于椭圆中心点)
BASEGRAPH_DATARENDER_API void GetEllipsePtPos(float fRad, float a, float b, point2f* ptPos)
{
	if (SY_IsZero_F(a) && SY_IsZero_F(b))
	{
		ptPos->x = 0.f;
		ptPos->y = 0.f;
		return;
	}

	while(fRad < 0)
	{
		fRad += 2*PAIF;
	}
	
	if (SY_IsEqual_F(fRad, 0.f) || SY_IsEqual_F(fRad, PAIF*2))
	{
		ptPos->x =  a;
		ptPos->y = 0;
	}
	else if (SY_IsEqual_F(fRad, PAIF*0.5))
	{
		ptPos->x =  0;
		ptPos->y = b;
	}
	else if (SY_IsEqual_F(fRad, PAIF))
	{
		ptPos->x =  -a;
		ptPos->y = 0;
	}
	else if (SY_IsEqual_F(fRad, PAIF*1.5))
	{
		ptPos->x =  0;
		ptPos->y = -b;
	}
	else if (fRad < PAIF*0.5)
	{
		ptPos->x =  sqrt(pow(a*b, 2) / (pow(tan(fRad)*a, 2) + pow(b, 2)));
		ptPos->y = tan(fRad)*ptPos->x;
	}
	else if (fRad <= PAIF)
	{
		float fAngle = PAIF - fRad;
		ptPos->x =  -sqrt(pow(a*b, 2) / (pow(tan(fAngle)*a, 2) + pow(b, 2)));
		ptPos->y = -tan(fAngle)*ptPos->x;
	}
	else if (fRad < PAIF*1.5)
	{
		ptPos->x =  -sqrt(pow(a*b, 2) / (pow(tan(fRad)*a, 2) + pow(b, 2)));
		ptPos->y = tan(fRad)*ptPos->x;
	}
	else if (fRad <= PAIF*2.0)
	{
		float fAngle = PAIF*2.0 - fRad;
		ptPos->x =  sqrt(pow(a*b, 2) / (pow(tan(fAngle)*a, 2) + pow(b, 2)));
		ptPos->y = -tan(fAngle)*ptPos->x;
	}
}

//根据边界点坐标(相对于椭圆中心点)得到等比缩放的椭圆长短轴以及边界点角度
BASEGRAPH_DATARENDER_API void		CalcRadAndAngle(float3 ptEdg,float& fRad_A, float& fRad_B, float &fAngle, float fMinA, float fMaxA, float fMinB, float fMaxB)
{
	if (SY_IsEqual_F(ptEdg.x, 0.f) && SY_IsEqual_F(ptEdg.y, 0.f))
	{
		fAngle = 0.f;
	}
	else if (SY_IsEqual_F(ptEdg.x, 0.f) && ptEdg.y > 0)
	{
		fAngle = 90.0f;
	}
	else if (SY_IsEqual_F(ptEdg.x, 0.f) && ptEdg.y < 0)
	{
		fAngle = 270.0f;
	}
	else
	{
		fAngle = (float)(atanf(ptEdg.y/ptEdg.x) * 180.0/PAI);
		if (ptEdg.x > 0 && ptEdg.y < 0)
		{
			fAngle += 360.0f;
		}
	}

	if (ptEdg.x < 0)
	{
		fAngle += 180.0f;
	}

	if (SY_IsZero_F(fRad_A))
		fRad_A = 1.0f;
	if (SY_IsZero_F(fRad_B))
		fRad_B = 1.0f;

	// 等比缩放
	if (fRad_A > 0 && fRad_B > 0)
	{
		float k = 0.f, fRad = 0.f, fRate = 0.f;
		if (SY_IsEqual_F(ptEdg.x, 0.f))
		{
			fRate = fabs(ptEdg.y) / fRad_B; 
		}
		else
		{
			k = ptEdg.y / ptEdg.x;
			fRad = sqrt((1+pow(k, 2)) * (pow(fRad_A*fRad_B, 2)) / (pow(fRad_B, 2)+pow(fRad_A*k, 2)));
			fRate = sqrt(pow(ptEdg.x, 2)+pow(ptEdg.y, 2)) / fRad;
		}
		fRad_A *= fRate;
		fRad_B *= fRate;

		fRad_A = min(fRad_A, fMaxA);
		fRad_A = max(fRad_A, fMinA);
		fRad_B = min(fRad_B, fMaxB);
		fRad_B = max(fRad_B, fMinB);
	}
}

//创建圆的矢量图
BASEGRAPH_DATARENDER_API BOOL  CreateCircle(float fStartRad,double fEndRad,double fRad,CSYFloatArray*	pArray)
{
	while(fStartRad > fEndRad)
	{
		fStartRad = fStartRad - 2 * PAI;
	}
	float fStartX = cos(fStartRad)*fRad;
	float fStartY = -sin(fStartRad)*fRad;
	float fEndX = cos(fEndRad)*fRad;
	float fEndY = -sin(fEndRad)*fRad;
	float fdis = (fEndX-fStartX)*(fEndX-fStartX) + (fEndY-fStartY)*(fEndY-fStartY);
	if(fdis < 16 && abs(fEndRad - fStartRad) < PAI)
	{
		pArray->Add(fEndRad);
	}
	else
	{
		float fCenterRad = (fEndRad+fStartRad)/2;
		if(*((DWORD*)&fCenterRad) ==0xffc00000 )
			int a = 0;
		CreateCircle(fStartRad,fCenterRad,fRad,pArray);
		CreateCircle(fCenterRad,fEndRad,fRad,pArray);
	}
	return TRUE;
}
BASEGRAPH_DATARENDER_API BOOL  ReleaseCircleArray(CSYFloatArray*	pArray)
{
	if(!pArray)
		return FALSE;
	pArray->RemoveAll();
	return TRUE;
}


//计算垂直线
void BASEGRAPH_DATARENDER_API line_vertical_normalizing(OUT point2f* ptout,IN point2f* ptinstart,point2f* ptinend,IN int ndirect)
{
	*ptout = ptinend[0] - ptinstart[0];
	if(ndirect==0)
		return;
	float ftemp = ptout->x;
	ptout->x = ndirect * ptout->y;
	ptout->y = (-1 *ndirect) * ftemp;
	if (sqrt(ptout->x * ptout->x + ptout->y * ptout->y) > ABS_FLOAT_0)
	{
		// 归一化
		float fdis = 1.0f/sqrt(ptout->x * ptout->x + ptout->y * ptout->y); // 默认两点不重合, 否则发生除0错误
		if(*(DWORD*)&ptout->x == 0xffc00000)
		{
			int a = 0;
		}
		*ptout *= fdis;
	}
	else
	{
		//对本函数本身来说，两点不该重合，否则计算无意义了
		//ASSERT(FALSE);
	}
}


//直线扩展
//OUT point2f* ptout,output line,the array has two element
//IN point2f* ptin,input line,the array has two element
//IN int ndirect,direct
int BASEGRAPH_DATARENDER_API line_extend(OUT point2f& ptoutstart,point2f& ptoutend,
							 IN point2f& ptinstart,point2f& ptinend,
							 IN int ndirect,IN float fwidth)
{
	//计算垂直单位矢量
	point2f ptvector;
	line_vertical_normalizing(&ptvector,&ptinstart,&ptinend,ndirect);
	ptvector *= fwidth;

	//偏移垂线
	ptoutstart = ptinstart + ptvector;
	ptoutend = ptinend + ptvector;
	return 1;
}

int	BASEGRAPH_DATARENDER_API arc_near_line(const point2f& ptcenter,const point2f& ptstart,const point2f& ptend,float frad)
{
	point2f ptv1 = ptstart - ptcenter;
	point2f ptv2 = ptend - ptcenter;
	float f = float2DotProduct(&ptv1,&ptv2);
	if(f/(frad*frad) > 0.90)
		return 1;
	else if(f/(frad*frad) < -0.95)
		return 2;
	return 0;

}

int BASEGRAPH_DATARENDER_API cell_makearc(SYArray* parray,const point2f& ptcenter,const point2f& ptstart,const point2f& ptend,float rad)
{
	///////////////////////////////////////////////////////// begin
	// 有Action播放时出错。 原因在于传入此处的ptcenter并非在ptstart,ptend的垂直平分线上
	//  just for test
	// 之后此处代码应删除
#ifdef DEBUG
	/*if(*((int*)&(ptend.x)) == 0xffc00000)
		int a = 0;
	if(fabs(ptstart.x) > 65536.0f)
		int a = 0;
	float r1 = (ptcenter.x - ptstart.x) * (ptcenter.x - ptstart.x) + (ptcenter.y - ptstart.y) * (ptcenter.y - ptstart.y);
	float r2 = (ptcenter.x - ptend.x) * (ptcenter.x - ptend.x) + (ptcenter.y - ptend.y) * (ptcenter.y - ptend.y);
	if ( fabs(r1 - r2) > ABS_FLOAT_0 || fabs(r1 - rad * rad) > ABS_FLOAT_0 )
	{
		int nErr = 0; 
	}*/
#endif
	/////////////////////////////////////////////////////////  end
	point2f ptv1 = ptstart - ptcenter;
	if(arc_near_line(ptcenter,ptstart,ptend,rad) == 1)
	{
		point2f* pptend = new point2f;
		*pptend = ptend;
		SYAR_Add(parray,pptend);
		return 1;
	}
	point2f ptmid = (ptstart + ptend)*0.5f;
	point2f pvector = ptmid - ptcenter;
	if(fabs(sqrt((double)(pvector.x * pvector.x + pvector.y * pvector.y))) < ABS_FLOAT_0)
	{
		point2f ptv = (ptend - ptstart) * 0.5f;
		float f = ptv.x;
		ptv.x = -ptv.y;
		ptv.y = f;
		ptmid = ptv + ptcenter;
	}
	else
	{
		if(fabs(sqrt((double)(pvector.x * pvector.x + pvector.y * pvector.y))) < ABS_FLOAT_0)
			int a = 0;

		//float fdisrate = (float)(1.0/sqrt((double)(pvector.x * pvector.x + pvector.y * pvector.y)));
		double drad = rad;
		double dmod = drad / sqrt((double)(pvector.x * pvector.x + pvector.y * pvector.y));
		pvector.x = (float)(pvector.x * dmod);
		pvector.y =  (float)(pvector.y * dmod);
		ptmid = pvector + ptcenter;
#ifdef DEBUG
		if((*(DWORD*)(&ptmid.x)) == 0xffc00000)
		{
			int a = 0;
		}
#endif
	}
	//2016-11-7，DJT，当start和mid或者end和mid相等时，不该再继续，否则可能无限递归
	if (abs(ptstart.x - ptmid.x) < 0.00001
		|| abs(ptstart.y - ptmid.y) < 0.00001
		|| abs(ptend.x - ptmid.x) < 0.00001
		|| abs(ptend.y - ptmid.y) < 0.00001)
	{
		return 1;
	}
	cell_makearc(parray,ptcenter,ptstart,ptmid,rad);
	cell_makearc(parray,ptcenter,ptmid,ptend,rad);
	return 1;
}

#define POINT_BEZIER_COUNT		4			// 一条贝塞尔曲线的表示点个数
BOOL	SY_Beizer3IsNearLine(pointf* pt,float fscale);
BOOL	SplitBezier3(pointf* pOPoint,pointf* pSplit1,pointf* pSplit2);
void _SYGetBezierLength(pointf* pPtOrigin, float& fBezierLen)
{
	BOOL bIsNearLine = SY_Beizer3IsNearLine(pPtOrigin,1.0f);

	if (bIsNearLine)
	{
		// 接近直线,微积分获取曲线长度
		float fPerLen = sqrt((pPtOrigin[0].x-pPtOrigin[3].x)*(pPtOrigin[0].x-pPtOrigin[3].x) + (pPtOrigin[0].y-pPtOrigin[3].y)*(pPtOrigin[0].y-pPtOrigin[3].y));
		fBezierLen += fPerLen;
	}
	else		// 二分法拆分曲线
	{
		pointf pPtFirst[POINT_BEZIER_COUNT], pPtSecond[POINT_BEZIER_COUNT];

		SplitBezier3(pPtOrigin, pPtFirst, pPtSecond);
		_SYGetBezierLength(pPtFirst, fBezierLen);
		_SYGetBezierLength(pPtSecond, fBezierLen);
	}
}

// 获取一段曲线长度
float SYGetBezierLength(const pointf &Pt1,const pointf &Pt2,const pointf &Pt3,const pointf &Pt4)
{
	float fBezierLen = 0;
	pointf pPtOrigin[POINT_BEZIER_COUNT] = {Pt1, Pt2, Pt3, Pt4};
	_SYGetBezierLength(pPtOrigin, fBezierLen);

	return fBezierLen;
}
