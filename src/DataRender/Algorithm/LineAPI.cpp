/************************************************************************/
//    文件：   SYLineAPI                                                   
//    描述：   线算法，包含直线、线段、射线等
//			   直线标识：Line
//			   线段标识：Segment
//			   射线标识：Ray                                    
/************************************************************************/
#include "stdafx.h"
#include "DataRender\LineAPI.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// 求两点之间距离 
float PointToPointDis(IN point2f p1, IN point2f p2)
{
	float fRet;
	fRet = (float)sqrtf( (double)( (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y) ) );
	return fRet;
}

// 求点pt到两点所在直线的距离
double PointToLineDis(IN point2f pt, IN point2f ptLS, IN point2f ptLE) 
{ 
	double l = sqrt(double((ptLE.x - ptLS.x) * (ptLE.x - ptLS.x) + (ptLE.y - ptLS.y) * (ptLE.y - ptLS.y)));
	if (l < FLOAT_0) 
	{
		return sqrt(double((pt.x - ptLS.x) * (pt.x - ptLS.x) + (pt.y - ptLS.y) * (pt.y - ptLS.y)));
	}
	double dMultiply = double((pt.x - ptLS.x) * (ptLE.y - ptLS.y)) - double((ptLE.x - ptLS.x) * (pt.y - ptLS.y));
	return abs(dMultiply) / l;
} 

// 判断点pt是否位于线段上    
BOOL IsPointOnSegment(IN point2f pt, IN point2f ptLS, IN point2f ptLE)
{
	// 左闭右开
	float fDis = PointToLineDis(pt, ptLS, ptLE);
	BOOL bRtn = fDis < (1e-4) 
		&& pt.x >= min(ptLS.x, ptLE.x) && pt.y >= min(ptLS.y, ptLE.y) 
		&& pt.x <= max(ptLS.x, ptLE.x) && pt.y <= max(ptLS.y, ptLE.y); 
	return bRtn;
} 

// 判断两直线是否相交
BOOL IsLineIntersection(point2f ptL1S,point2f ptL1E,point2f ptL2S,point2f ptL2E,point2f* pPtOut)
{
	point2f v1,v2;//分别是两个直线对应向量
	v1.x = ptL1E.x - ptL1S.x;
	v1.y = ptL1E.y - ptL1S.y;
	v2.x = ptL2E.x - ptL2S.x;
	v2.y = ptL2E.y - ptL2S.y;

	float fDenominator = v1.x * v2.y - v1.y * v2.x;
	if ( abs(fDenominator) < FLOAT_0 ) 
	{
		// 平行
		return FALSE;
	}

	pPtOut->x = (double)((v2.x * (ptL1S.y * ptL1E.x - ptL1E.y * ptL1S.x) - v1.x * (ptL2S.y * ptL2E.x - ptL2E.y * ptL2S.x))) / fDenominator;
	if( abs(v1.x) < FLOAT_0)
	{
		pPtOut->y = ptL2E.y - (double)((ptL2E.x - pPtOut->x) * v2.y) / v2.x;
	}
	else
	{
		pPtOut->y = (double)(v1.y * pPtOut->x + (ptL1S.y * ptL1E.x - ptL1E.y * ptL1S.x)) / v1.x;	
	}
	return TRUE;
}

// 判断两线段是否相交
BOOL IsSegmentIntersection(point2f ptL1S,point2f ptL1E,point2f ptL2S,point2f ptL2E,point2f* pPtOut)
{
	// ptL1S, 是内环上的点. 肯定不能与外环上的点重合
	// ptL1E, 是外环上的备选点
	// 如果外环上的点, 不与ptL1E重合, 那么该点如果落在线段ptL1S,ptL1E上, 则认为相交, 备选点ptL1E不合格.
	if ( ptL2S.x != ptL1E.x || ptL2S.y != ptL1E.y ) 
	{
		if ( IsPointOnSegment(ptL2S, ptL1S, ptL1E) ) 
		{
			pPtOut->x = ptL2S.x;
			pPtOut->y = ptL2S.y;
			return TRUE; // 认为相交 , 不合格
		}
	}

	if ( ptL2E.x != ptL1E.x || ptL2E.y != ptL1E.y ) 
	{
		if ( IsPointOnSegment(ptL2E, ptL1S, ptL1E) ) 
		{
			pPtOut->x = ptL2E.x;
			pPtOut->y = ptL2E.y;
			return TRUE; // 认为相交 , 不合格
		}
	}

	if ( POINTF_EQUAL(ptL2S,ptL1E) || POINTF_EQUAL(ptL2E,ptL1E)) 
	{
		return FALSE;
	}
	point2f v1,v2;//分别是两个线段对应向量
	v1.x = ptL1E.x - ptL1S.x;
	v1.y = ptL1E.y - ptL1S.y;
	v2.x = ptL2E.x - ptL2S.x;
	v2.y = ptL2E.y - ptL2S.y;

	double fDenominator = v1.x * v2.y - v1.y * v2.x;
	if ( abs(fDenominator) < FLOAT_0 ) 
	{
		return FALSE;
	}

	double fScale = v1.y * (ptL2S.x - ptL1S.x) - v1.x * (ptL2S.y - ptL1S.y);
	if(fDenominator > 0) 
	{
		if (fScale >= 0 && fScale <= fDenominator/* - 0.00001*/) //交点在线段2上
		{
			fScale = v2.y *(ptL2S.x - ptL1S.x) - v2.x *(ptL2S.y - ptL1S.y);
			if(fScale >= 0 && fScale < fDenominator - FLOAT_0) //交点在线段１上
			{
				pPtOut->x = (double)(v2.x * (ptL1S.y * ptL1E.x - ptL1E.y * ptL1S.x) - v1.x * (ptL2S.y * ptL2E.x - ptL2E.y * ptL2S.x)) / fDenominator;
				if( abs(v1.x) < abs(v2.x))
				{
					pPtOut->y = ptL2E.y - (double)((ptL2E.x - pPtOut->x) * v2.y) / v2.x;
				}
				else
				{
					pPtOut->y = (double)(v1.y * pPtOut->x + (ptL1S.y * ptL1E.x - ptL1E.y * ptL1S.x)) / v1.x;	
				}
				return TRUE;
			}
		}
	}
	else
	{
		if (fScale <= 0 && fScale >= fDenominator/* - 0.00001*/) //交点在线段2上
		{
			fScale = v2.y * (ptL2S.x - ptL1S.x) - v2.x * (ptL2S.y - ptL1S.y);
			if(fScale <= 0 && fScale > fDenominator + FLOAT_0) //交点在线段１上
			{
				pPtOut->x = (double)(v2.x * (ptL1S.y * ptL1E.x - ptL1E.y * ptL1S.x) - v1.x * (ptL2S.y * ptL2E.x - ptL2E.y * ptL2S.x)) / fDenominator;
				if( abs(v1.x) < abs(v2.x))
				{
					pPtOut->y = ptL2E.y - (double)((ptL2E.x - pPtOut->x) * v2.y) / v2.x;
				}
				else
				{
					pPtOut->y = (double)(v1.y * pPtOut->x + (ptL1S.y * ptL1E.x - ptL1E.y * ptL1S.x)) / v1.x;	
				}
				return TRUE;
			}
		}
	}

	return FALSE;

}

// 判断射线与线段是否相交
BOOL IsRaySegmentIntersection(IN point2f ptRay0,IN point2f ptRay1,IN point2f pt0,IN point2f pt1,OUT point2f* pPtOut)
{
	if( !IsLineIntersection(ptRay0,ptRay1,pt0,pt1,pPtOut))
		return FALSE;

	if( pPtOut->x < pt0.x && pPtOut->x < pt1.x)
		return FALSE;

	if( pPtOut->x > pt0.x && pPtOut->x > pt1.x)
		return FALSE;

	if( pPtOut->y < pt0.y && pPtOut->y < pt1.y)
		return FALSE;

	if( pPtOut->y > pt0.y && pPtOut->y > pt1.y)
		return FALSE;

	BOOL bOnSegment = IsPointOnSegment(*pPtOut,ptRay0,ptRay1);
	if( !bOnSegment )
	{
		float fDis0 = PointToPointDis(ptRay0,*pPtOut);
		float fDis1 = PointToPointDis(ptRay1,*pPtOut);
		if( fDis0 < fDis1)
			return FALSE;
	}
	return TRUE;
}

// 判断线段是否与区域（线段组）相交
int IsSegmentRegionIntersection(IN point2f pt0,IN point2f pt1,IN BaseARray* pArSeg,OUT BaseARray* pArOut)
{
	int nCount = 0;
	int nSize = (int)BaseAR_GetSize(pArSeg);
	for(int i = 0; i < nSize - 1; i++)
	{
		point2lf* pPt0 = (point2lf*)BaseAR_GetAt(pArSeg,i);
		point2lf* pPt1 = (point2lf*)BaseAR_GetAt(pArSeg,i + 1);
		point2f ptRgn0;
		point2f ptRgn1;
		ptRgn0.x = (float)pPt0->x;
		ptRgn0.y = (float)pPt0->y;
		ptRgn1.x = (float)pPt1->x;
		ptRgn1.y = (float)pPt1->y;

		point2f ptTmp;
		if(IsSegmentIntersection(pt0, pt1, ptRgn0, ptRgn1,&ptTmp))
		{
			point2f* ptInter = new point2f;
			ptInter->x = ptTmp.x;
			ptInter->y = ptTmp.y;
			if( nCount <= 0)
			{
				BaseAR_Add(pArOut,ptInter);
			}
			else
			{
				BOOL bAdd = FALSE;
				float fDis = PointToPointDis(*ptInter,pt0);
				for( int j = 0; j < nCount; j++)
				{
					point2f* ptTmp = (point2f*)BaseAR_GetAt(pArOut,j);
					float fTmp = PointToPointDis(*ptTmp,pt0);
					if( fDis < fTmp)
					{
						BaseAR_InsertAt(pArOut,j,ptInter,1);
						bAdd = TRUE;
						break;
					}
				}
				if( !bAdd)
					BaseAR_Add(pArOut,ptInter);
			}

			nCount++;
		}
	}

	return nCount;
}

// 判断射线是否与区域（线段组）相交
BOOL IsRayRegionIntersection(IN point2f ptRay0,IN point2f ptRay1,IN BaseARray* pArSeg,OUT point2f* pPtOut)
{
	int nSize = (int)BaseAR_GetSize(pArSeg);
	if (nSize < 3) 
	{
		return FALSE;
	}
	int nInterPts = 0;
	float fDis = PointToPointDis(ptRay0,ptRay1);
	// 分别取出每条线段的两个端点，判别是否和扫描线相交 如果相交，交点计数加1，或者内外
	point2lf* pptf1 = NULL;
	point2lf* pptf2 = NULL;
	point2f pt1;
	point2f pt2;
	point2f ptTmp;
	for (int i = 0; i < nSize - 1; i++)
	{
		pptf1 = (point2lf*)BaseAR_GetAt(pArSeg,i);
		pptf2 = (point2lf*)BaseAR_GetAt(pArSeg,i+1);	
		pt1.x = (float)pptf1->x;
		pt1.y = (float)pptf1->y;
		pt2.x = (float)pptf2->x;
		pt2.y = (float)pptf2->y;
		if ( IsRaySegmentIntersection(ptRay0, ptRay1,pt1,pt2,&ptTmp)) 
		{
			float dis1 = PointToPointDis(ptTmp,ptRay0);
			if( dis1 <= fDis)
				continue;

			if( nInterPts == 0)
			{
				pPtOut->x = ptTmp.x;
				pPtOut->y = ptTmp.y;
			}
			else
			{
				float dis2 = PointToPointDis(*pPtOut,ptRay0);
				if( dis1 < dis2)
				{
					pPtOut->x = ptTmp.x;
					pPtOut->y = ptTmp.y;
				}
			}
			nInterPts++;
		}
	}

	pptf1 = (point2lf*)BaseAR_GetAt(pArSeg,nSize - 1);
	pptf2 = (point2lf*)BaseAR_GetAt(pArSeg,0);
	pt1.x = pptf1->x;
	pt1.y = pptf1->y;
	pt2.x = pptf2->x;
	pt2.y = pptf2->y;
	if ( IsRaySegmentIntersection(ptRay0, ptRay1,pt1,pt2,&ptTmp)) 
	{
		float dis1 = PointToPointDis(ptTmp,ptRay0);
		if( dis1 > fDis)
		{
			if( nInterPts == 0)
			{
				pPtOut->x = ptTmp.x;
				pPtOut->y = ptTmp.y;
			}
			else
			{
				float dis2 = PointToPointDis(*pPtOut,ptRay0);
				if( dis1 < dis2)
				{
					pPtOut->x = ptTmp.x;
					pPtOut->y = ptTmp.y;
				}
			}
			nInterPts++;
		}
	}

	if (nInterPts > 0)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL BeamHit(IN float x, IN float y, IN point2f ptf1, IN point2f ptf2)
{
	float yy = y + 0.00001f;
	// 如果一个大于y，另外一个小于等于y，则认为相交
	// 否则不相交
	if (ptf1.y <= ptf2.y) 
	{
		if (ptf1.y <= yy && ptf2.y > yy) 
		{
			// 还要加上一点， 交点在被测点的右侧 
			// 如果两个点都在左侧，不相交 
			if (ptf1.x < x && ptf2.x < x) 
			{
				return FALSE;
			}
			// 如果两个点都在右侧，相交 
			if (ptf1.x >= x && ptf2.x >= x) 
			{
				return TRUE;
			}
			// 一左一右要求交点的横坐标，大于x才可以
			if (ptf1.x + (double)((y-ptf1.y)*(ptf2.x - ptf1.x))/(ptf2.y -ptf1.y) >= x ) 
			{
				return TRUE;
			}
		}
	}
	else
	{
		if (ptf2.y <= yy && ptf1.y > yy) 
		{
			// 还要加上一点， 交点在被测点的右侧 
			// 如果两个点都在左侧，不相交 
			if (ptf1.x < x && ptf2.x < x) 
			{
				return FALSE;
			}
			// 如果两个点都在右侧，相交 
			if (ptf1.x >= x && ptf2.x >= x) 
			{
				return TRUE;
			}
			// 一左一右要求交点的横坐标，大于x才可以
			if (ptf1.x + (double)((y-ptf1.y)*(ptf2.x - ptf1.x))/(ptf2.y -ptf1.y) >= x ) 
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

// 判断点pt是否位于区域（线段组）上
BOOL IsPointOnRegion(IN point2f pt,IN BaseARray* pArSeg)
{
	int nSize = (int)BaseAR_GetSize(pArSeg);
	point2lf* pptf1 = NULL;
	point2lf* pptf2 = NULL;
	point2f pt1;
	point2f pt2;
	for (int i = 0; i < nSize - 1; i++)
	{
		pptf1 = (point2lf*)BaseAR_GetAt(pArSeg,i);
		pptf2 = (point2lf*)BaseAR_GetAt(pArSeg,i+1);	
		pt1.x = pptf1->x;
		pt1.y = pptf1->y;
		pt2.x = pptf2->x;
		pt2.y = pptf2->y;
		if ( IsPointOnSegment(pt,pt1,pt2) ) 
		{
			return TRUE;
		}
	}

	pptf1 = (point2lf*)BaseAR_GetAt(pArSeg,nSize - 1);
	pptf2 = (point2lf*)BaseAR_GetAt(pArSeg,0);
	pt1.x = pptf1->x;
	pt1.y = pptf1->y;
	pt2.x = pptf2->x;
	pt2.y = pptf2->y;
	if ( IsPointOnSegment(pt,pt1,pt2) ) 
	{
		return TRUE;
	}

	return FALSE;
}

// 判断点pt是否位于区域（线段组）内
BOOL IsPointInRegionLF(IN point2f pt,IN BaseARray* pArSeg)
{
	int nSize = (int)BaseAR_GetSize(pArSeg);
	if (nSize < 3) 
	{
		return FALSE;
	}
	int nInterPts = 0;
	// 分别取出每条线段的两个端点，判别是否和扫描线相交 如果相交，交点计数加1，或者内外
	point2lf* pptf1 = NULL;
	point2lf* pptf2 = NULL;
	point2f pt1;
	point2f pt2;
	for (int i = 0; i < nSize - 1; i++)
	{
		pptf1 = (point2lf*)BaseAR_GetAt(pArSeg,i);
		pptf2 = (point2lf*)BaseAR_GetAt(pArSeg,i+1);	
		pt1.x = pptf1->x;
		pt1.y = pptf1->y;
		pt2.x = pptf2->x;
		pt2.y = pptf2->y;
		if ( BeamHit(pt.x, pt.y, pt1,pt2) ) 
		{
			nInterPts++;
		}
	}

	pptf1 = (point2lf*)BaseAR_GetAt(pArSeg,nSize - 1);
	pptf2 = (point2lf*)BaseAR_GetAt(pArSeg,0);
	pt1.x = pptf1->x;
	pt1.y = pptf1->y;
	pt2.x = pptf2->x;
	pt2.y = pptf2->y;
	if ( BeamHit(pt.x,pt.y, pt1,pt2) ) 
	{
		nInterPts++;
	}

	if (nInterPts % 2 == 0) // 偶数个 
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


// 判断点pt是否位于区域（线段组）内
BOOL IsPointInRegionF(IN point2f pt,IN BaseARray* pArSeg)
{
	int nSize = (int)BaseAR_GetSize(pArSeg);
	if (nSize < 3) 
	{
		return FALSE;
	}
	int nInterPts = 0;
	// 分别取出每条线段的两个端点，判别是否和扫描线相交 如果相交，交点计数加1，或者内外
	point2f* pptf1 = NULL;
	point2f* pptf2 = NULL;
	for (int i = 0; i < nSize - 1; i++)
	{
		pptf1 = (point2f*)BaseAR_GetAt(pArSeg,i);
		pptf2 = (point2f*)BaseAR_GetAt(pArSeg,i+1);	
		if ( BeamHit(pt.x, pt.y, *pptf1,*pptf2) ) 
		{
			nInterPts++;
		}
	}

	pptf1 = (point2f*)BaseAR_GetAt(pArSeg,nSize - 1);
	pptf2 = (point2f*)BaseAR_GetAt(pArSeg,0);
	if ( BeamHit(pt.x,pt.y, *pptf1,*pptf2) ) 
	{
		nInterPts++;
	}

	if (nInterPts % 2 == 0) // 偶数个 
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

// 给一条连续的线段和一个连续的封闭边界，输出线段在边界内的部分
void GetSegmentInRegion(IN BaseARray* pArLine,IN BaseARray* pArSeg,OUT BaseARray* pArOut)
{
	int nPointCount = BaseAR_GetSize(pArLine);
	if( nPointCount <= 1)
		return;

	BOOL bClose = FALSE;
	point2f* pStart = (point2f*)BaseAR_GetAt(pArLine,0);
	point2f* pEnd = (point2f*)BaseAR_GetAt(pArLine,nPointCount - 1);
	if( POINTF_EQUAL(*pStart,*pEnd) )
		bClose = TRUE;

	BOOL bCloseStartInRgn = FALSE;
	BOOL bHasSpelid = FALSE;
	BOOL bInRgn = FALSE;
	BaseARray* arCell = NULL;
	for (int i = 0; i < nPointCount; i++)
	{
		point2f* pt = (point2f*)BaseAR_GetAt(pArLine,i);

		// 第一个点
		if( i == 0 )
		{
			if( IsPointInRegionLF(*pt,pArSeg))		// 在区域内
			{
				bInRgn = TRUE;
				arCell = BaseAR_Create();
				BaseAR_Init(arCell);
				BaseAR_Add(pArOut,arCell);

				if( bClose)
				{
					bCloseStartInRgn = TRUE;
				}
				else
				{
					point2f* ptNext = (point2f*)BaseAR_GetAt(pArLine,i + 1);
					point2f* ptInter = new point2f;
					if( IsRayRegionIntersection(*ptNext,*pt,pArSeg,ptInter))
					{
						BaseAR_Add(arCell,ptInter);
					}
					else
					{
						delete ptInter;
					}
				}
				point2f* ptNew = new point2f;
				ptNew->x = pt->x;
				ptNew->y = pt->y;
				BaseAR_Add(arCell,ptNew);
			}
			else
			{
				bInRgn = FALSE;
			}
		}
		else
		{
			point2f* ptLast = (point2f*)BaseAR_GetAt(pArLine,i - 1);
			BaseARray arInter;
			BaseAR_Init(&arInter);
			int nIntCount = IsSegmentRegionIntersection(*ptLast,*pt,pArSeg,&arInter);
			if( nIntCount > 0)
				bHasSpelid = TRUE;
			for( int j = 0; j < nIntCount; j++)
			{
				point2f* ptInter = (point2f*)BaseAR_GetAt(&arInter,j);
				BOOL bNew = bInRgn ? (j % 2 == 1) : (j % 2 == 0);
				if( bNew )
				{
					arCell = BaseAR_Create();
					BaseAR_Init(arCell);
					BaseAR_Add(pArOut,arCell);
				}
				BaseAR_Add(arCell,ptInter);
			}
			BaseAR_RemoveAll(&arInter);

			if( IsPointInRegionLF(*pt,pArSeg))		// 在区域内
			{
				bInRgn = TRUE;
				point2f* ptNew = new point2f;
				ptNew->x = pt->x;
				ptNew->y = pt->y;
				BaseAR_Add(arCell,ptNew);

				if( i == nPointCount - 1 && !bClose)
				{
					point2f* ptLast = (point2f*)BaseAR_GetAt(pArLine,i - 1);
					point2f* ptInter = new point2f;
					if( IsRayRegionIntersection(*ptLast,*pt,pArSeg,ptInter))
					{
						BaseAR_Add(arCell,ptInter);
					}
					else
					{
						delete ptInter;
					}
				}
			}
			else
			{
				bInRgn = FALSE;
			}
		}
	}

	if( bClose && bCloseStartInRgn && bHasSpelid)
	{
		BaseARray* arTmp = (BaseARray*)BaseAR_GetAt(pArOut,0);
		int nOutCount = BaseAR_GetSize(pArOut);
		BaseARray* arLast = (BaseARray*)BaseAR_GetAt(pArOut,nOutCount - 1);
		for( int i = 0; i < BaseAR_GetSize(arTmp); i++)
		{
			point2f* ptTmp = (point2f*)BaseAR_GetAt(arTmp,i);
			BaseAR_Add(arLast,ptTmp);	
		}
		BaseAR_RemoveAll(arTmp);
		BaseAR_Release(arTmp);
		BaseAR_RemoveAt(pArOut,0,1);
	}
}
