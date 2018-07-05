#include "StdAfx.h"
#include "DataRender\TriangulateApp.h"
#include "DataRender\OffsetCurve.h"
#include "DataRender\DataRenderUtityFunc.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
typedef struct tag_EnvelopeNode
{
	int nCellIndex;
	float fXMin;
	float fYMin;
	float fXMax;
	float fYMax;
	tag_EnvelopeNode * nextNode;
}EnvelopeNode; 

typedef CSYArray<EnvelopeNode*> CSYEnvelopeArray;

template<typename t1>
void floatcross(t1& vresult,t1 pt1,t1 pt2)
{
	vresult.x = 0;
	vresult.y = pt1.x * pt2.y - pt1.y * pt2.x;
}
template<typename t1> 
int vz_ptin_triangle(t1* pt1,t1* pt2,t1* pt3,t1 *ptin)
{
	t1 v1 = *ptin - *pt1;
	t1 v2 = *ptin - *pt2;
	t1 v3 = *ptin - *pt3;
	if((fabs(v1.x) < ABS_FLOAT_0 && fabs(v1.y) < ABS_FLOAT_0)
		||(fabs(v2.x) < ABS_FLOAT_0 && fabs(v2.y) < ABS_FLOAT_0)
		||(fabs(v3.x) < ABS_FLOAT_0 && fabs(v3.y) < ABS_FLOAT_0))
		return FALSE;
	t1 vr1,vr2,vr3;
	floatcross(vr1,v1,v2);
	floatcross(vr2,v2,v3);
	floatcross(vr3,v3,v1);
	if(vr1.y * vr2.y > 0 && vr2.y * vr3.y > 0 && vr3.y * vr1.y > 0)
	{
		return TRUE;
	}
	return FALSE;
}
template<typename t1> 
int vz_pointdirect(t1* ptPrv,t1* ptCur,t1* ptNext)
{
	t1 v1,v2;
	v1 = *ptCur - *ptPrv;
	v2 = *ptNext - *ptCur;
	t1 vR;
	floatcross(vR,v1,v2);
	if(vR.y > 0)
		return 1;
	else
		return -1;
}
SYTriangulateApp::SYTriangulateApp(void)
{
}

SYTriangulateApp::~SYTriangulateApp(void)
{
}


//------------------------------------------------------------------
//函数 NearLine 说明
//功能：三次Bezier曲线控制顶点接近一条线段
//输入：
//输出：
//------------------------------------------------------------------
BOOL NearLine(IN pointf* ptf ) 
{
	float FDIST = 0.1f;
	float d0 = (float)sqrt( (double)( (ptf[3].x - ptf[0].x)*(ptf[3].x - ptf[0].x) + (ptf[3].y - ptf[0].y)*(ptf[3].y - ptf[0].y) ) );
	float t1 = (ptf[3].x - ptf[0].x)*(ptf[1].x - ptf[0].x) + (ptf[3].y - ptf[0].y)*(ptf[1].y - ptf[0].y);
	if (d0 < 0.0001f) 
	{
		float d1 = (float)sqrt( (double)( (ptf[2].x - ptf[1].x)*(ptf[2].x - ptf[1].x) + (ptf[2].y - ptf[1].y)*(ptf[2].y - ptf[1].y) ) );
		if (d1 < 0.0001f) 
		{
			return TRUE;
		}
		return FALSE;
	}
	t1 /= d0*d0;
	float t2 = (ptf[3].x - ptf[0].x)*(ptf[2].x - ptf[0].x) + (ptf[3].y - ptf[0].y)*(ptf[2].y - ptf[0].y);
	t2 /= d0*d0;
	if ( t1>-ABS_FLOAT_0 && t1<1+ABS_FLOAT_0 && t2>-ABS_FLOAT_0 && t2<1+ABS_FLOAT_0 ) 
	{
	}
	else
	{
		return FALSE;
	}

	//小于一定值
	pointf ptfA1,ptfA2;
	ptfA1.x = ptf[0].x + t1 * (ptf[3].x - ptf[0].x);
	ptfA1.y = ptf[0].y + t1 * (ptf[3].y - ptf[0].y);

	ptfA2.x = ptf[0].x + t2 * (ptf[3].x - ptf[0].x);
	ptfA2.y = ptf[0].y + t2 * (ptf[3].y - ptf[0].y);

	float d3, d4;
	d3 = (float)sqrt( (double)( (ptf[1].x - ptfA1.x)*(ptf[1].x - ptfA1.x) + (ptf[1].y - ptfA1.y)*(ptf[1].y - ptfA1.y) ) );
	d4 = (float)sqrt( (double)( (ptf[2].x - ptfA2.x)*(ptf[2].x - ptfA2.x) + (ptf[2].y - ptfA2.y)*(ptf[2].y - ptfA2.y) ) );

	if ( d3<= FDIST && d4<=FDIST ) 
	{
		return TRUE;
	}
	return FALSE;
} // end of NearLine

//------------------------------------------------------------------
//函数 AddCurve2PtfPoly 说明
//功能：将三次Bezier曲线段转化并加入多边形数组
//输入：
//输出：
//------------------------------------------------------------------
void AddCurve2PtfPoly(OUT CSYPoint2fPtrArray * pptrPoly, IN pointf * ctrlPoints)
{
	// 接近直线，则直接加入
	// 否则分成两段，递归调用
	pointf *ptfNew;
	if ( NearLine(ctrlPoints) )
	{
		ptfNew = new pointf;
		ptfNew->x = ctrlPoints[3].x;
		ptfNew->y = ctrlPoints[3].y;
		pptrPoly->Add(ptfNew);
	}
	else
	{
		pointf ptfFormer[4];
		pointf ptfLatter[4];

		ptfFormer[0].x = ctrlPoints[0].x;
		ptfFormer[0].y = ctrlPoints[0].y;

		ptfLatter[3].x = ctrlPoints[3].x;
		ptfLatter[3].y = ctrlPoints[3].y;

		ptfFormer[1].x = (ctrlPoints[0].x + ctrlPoints[1].x)/2;
		ptfFormer[1].y = (ctrlPoints[0].y + ctrlPoints[1].y)/2;

		ptfLatter[2].x = (ctrlPoints[2].x + ctrlPoints[3].x)/2;
		ptfLatter[2].y = (ctrlPoints[2].y + ctrlPoints[3].y)/2;

		ptfFormer[3].x = (ctrlPoints[1].x + ctrlPoints[2].x)/2;
		ptfFormer[3].y = (ctrlPoints[1].y + ctrlPoints[2].y)/2;

		ptfFormer[2].x = (ptfFormer[1].x + ptfFormer[3].x)/2;
		ptfFormer[2].y = (ptfFormer[1].y + ptfFormer[3].y)/2;

		ptfLatter[1].x = (ptfLatter[2].x + ptfFormer[3].x)/2;
		ptfLatter[1].y = (ptfLatter[2].y + ptfFormer[3].y)/2;

		ptfFormer[3].x = (ptfLatter[1].x + ptfFormer[2].x)/2;
		ptfFormer[3].y = (ptfLatter[1].y + ptfFormer[2].y)/2;

		ptfLatter[0].x = ptfFormer[3].x;
		ptfLatter[0].y = ptfFormer[3].y;

		AddCurve2PtfPoly(pptrPoly, ptfFormer ) ;
		AddCurve2PtfPoly(pptrPoly, ptfLatter ) ;
	}
} // END OF AddCurve2PtfPoly
//--------------------------------------------------------------------------------
//功能：判断两点是否在直线两侧
//--------------------------------------------------------------------------------
BOOL SYTriangulateApp::PtsOnDiffSide(IN pointf p1, IN pointf p2, IN pointf l1, IN pointf l2)
{
	// 只需判断线段pt1,pt2是否与直线相交(不考虑线段端点)
	if (l2.x == l1.x) 
	{
		if (   p1.x < l1.x && p2.x > l1.x
			|| p1.x > l1.x && p2.x < l1.x
			) 
		{
			return TRUE;
		}
	}

	if (l2.y == l1.y) 
	{
		if (   p1.y < l1.y && p2.y > l1.y
			|| p1.y > l1.y && p2.y < l1.y
			) 
		{
			return TRUE;
		}
	}

	float fm = (l2.y - l1.y)*(p2.x - p1.x) - (l2.x - l1.x) * (p2.y - p1.y);
	if ( abs(fm) < ABS_FLOAT_0 )  // 平行
	{
		return FALSE;
	}

	float t = (l2.x - l1.x) * (p1.y - l1.y) - (l2.y - l1.y) * (p1.x - l1.x);
	t /= fm;
	if ( t > 0 && t < 1) 
	{
		return TRUE;
	}

	return FALSE;
} // END OF PtsOnDiffSide

BOOL SYTriangulateApp::PtsOnDiffSide(IN point3f p1, IN point3f p2, IN point3f l1, IN point3f l2)
{
	// 只需判断线段pt1,pt2是否与直线相交(不考虑线段端点)
	if (l2.x == l1.x) 
	{
		if (   p1.x < l1.x && p2.x > l1.x
			|| p1.x > l1.x && p2.x < l1.x
			) 
		{
			return TRUE;
		}
	}

	if (l2.y == l1.y) 
	{
		if (   p1.y < l1.y && p2.y > l1.y
			|| p1.y > l1.y && p2.y < l1.y
			) 
		{
			return TRUE;
		}
	}

	float fm = (l2.y - l1.y)*(p2.x - p1.x) - (l2.x - l1.x) * (p2.y - p1.y);
	if ( abs(fm) < ABS_FLOAT_0 )  // 平行
	{
		return FALSE;
	}

	float t = (l2.x - l1.x) * (p1.y - l1.y) - (l2.y - l1.y) * (p1.x - l1.x);
	t /= fm;
	if ( t > 0 && t < 1) 
	{
		return TRUE;
	}

	return FALSE;
} // END OF PtsOnDiffSide

//--------------------------------------------------------------------------------
//功能：打断并加入内环上的顶点
//--------------------------------------------------------------------------------
void SYTriangulateApp::BreakAndAddCurCell(CSYPoint3fPtrArray *pptrTri, 
						CPtNodeArray &ptPolyArray, //ptfArray中的顶点加入到ptPolyArray
						SYArray &ptfArray, 
						int nMinXIndex, // ptfArray中最小x值点的序号
						float fMinX, // ptfArray中最小x值
						CSYPoint2fPtrArray &ptfArrAddedMinPts)
{
	int i, j;
	point3f* lpPtf3D = NULL;
	PtNode* pPtNode = NULL;
	PtNode* pPreNode = NULL;
	PtNode* pPostNode = NULL;
	BOOL bChoosePre = TRUE;
	pointf* pptf = NULL;
	pointf* pptfTemp = NULL;
	pointf startPt, endPt;
	pointf searchPt;
	
	int nInSize = (int)SYAR_GetSize(&ptfArray);
	pptf = (pointf * )SYAR_GetAt(&ptfArray,nMinXIndex);
	startPt = *pptf;
	// 从外环上找到一点P, 使得Px<Lx(即fMinX), 并且PL不与外环相交
	int nSize = (int)ptPolyArray.GetSize();
	float fCurSearchX = fMinX;
	float fTemp = -65535.f;
	int nIndex = 0;
	int *pbMay = new int[nSize];
	//memset( pbMay, 1, sizeof(int) * nSize );
	for ( i = 0; i < nSize; i++) 
	{
		pbMay[i] = 1;
	}

	int nAddedMin = (int)ptfArrAddedMinPts.GetSize();
	for ( i = 0; i < nSize; i++) 
	{
		pPtNode = (PtNode*)ptPolyArray.GetAt(i);
		if ( pPtNode->pt.x <= fMinX)
		{
			for ( int jjj = 0; jjj < nAddedMin; jjj++)
			{
				pptfTemp = (pointf*)ptfArrAddedMinPts.GetAt(jjj);
				if (pPtNode->pt == *pptfTemp)
				{
					pbMay[i] = 0;
					break;
				}
			}
		}
	}

	for ( i = nSize - 1; i >= 0; i-- ) 
	{
		////////////////////////////////////////////////////////////////////////// begin
		// old code
		//pPtNode = (PtNode*)ptPolyArray.GetAt(i);
		//if (pPtNode->pt.x < fMinX) 
		
		// new code
		// 找到其中最大的来测
		nIndex = -1;
		fTemp = -65535.f;
		for ( j = 0; j < nSize; j++) 
		{
			pPtNode = (PtNode*)ptPolyArray.GetAt(j);
			if (pPtNode->pt.x <= fCurSearchX && pPtNode->pt.x < fMinX && pbMay[j] == 1 && pPtNode->pt.x >= fTemp)
			{
				fTemp = pPtNode->pt.x;
				nIndex = j;
			}
		}
		if (nIndex != -1) 
		////////////////////////////////////////////////////////////////////////// end
		{
			fCurSearchX = fTemp;
			pbMay[nIndex] = 0;
			pPtNode = (PtNode*)ptPolyArray.GetAt(nIndex);
			float2Fromfloat3(&endPt,&pPtNode->pt);
			//endPt = pPtNode->pt;
			// 查看是否PL不与外环相交
			if ( !LineIntersection2(startPt, endPt, ptPolyArray) ) 
			{
				//找到另外一个点
				// P两相邻点R, Q
				// 若LPR不共线, 则从线段PR上选择一点: 先看R.如果R位于L左侧, 且LR不与外环相交, 则选R. 否则二分查找
				if (nIndex == 0) 
				{
					pPreNode = (PtNode*)ptPolyArray.GetAt( nSize - 1);
				}
				else
				{
					pPreNode = (PtNode*)ptPolyArray.GetAt(nIndex-1);
				}
				pPostNode = (PtNode*)ptPolyArray.GetAt( (nIndex+1) % nSize );

				BOOL bDiffSide = PtsOnDiffSide( *(point2f*)&pPreNode->pt, *(point2f*)&pPostNode->pt, startPt, endPt);

				if (bDiffSide) // 异侧
				{
					if ( PToLDist((point2f*)&pPreNode->pt, &startPt, &endPt) > ABS_FLOAT_0 ) // 不共线
					{
						bChoosePre = TRUE;
					}
					else
					{
						bChoosePre = FALSE;
					}
				}
				else // 非异侧
				{
					if ( PToLDist(&startPt, (point2f*)&pPreNode->pt, &endPt) > ABS_FLOAT_0 ) // 不共线
					{
						bDiffSide = PtsOnDiffSide( startPt, *(point2f*)&pPostNode->pt, *(point2f*)&pPreNode->pt, endPt);
						if (bDiffSide) // 选择pPreNode->pt
						{
							bChoosePre = TRUE;
						}
						else // 选择pPostNode->pt
						{
							bChoosePre = FALSE;
						}
					}
					else
					{
						bDiffSide = PtsOnDiffSide( startPt, *(point2f*)&pPreNode->pt, *(point2f*)&pPostNode->pt, endPt);
						if (bDiffSide) // 选择pPostNode->pt
						{
							bChoosePre = FALSE;
						}
						else // 选择pPreNode->pt
						{
							bChoosePre = TRUE;
						}
					}
				}

				if (bChoosePre) 
				{
					if ( pPreNode->pt.x < fMinX && !LineIntersection2(startPt, *(point2f*)&pPreNode->pt, ptPolyArray) ) 
					{
						//选择此点. 即nIndex-1, nIndex, L构成三角形
						// 1. 加入三角形 L, nIndex-1, nIndex
						lpPtf3D = new point3f;// L
						lpPtf3D->x = startPt.x;
						lpPtf3D->y = 0;
						lpPtf3D->z = startPt.y;
						pptrTri->Add(lpPtf3D);

						lpPtf3D = new point3f; // nIndex-1
						lpPtf3D->x = pPreNode->pt.x;
						lpPtf3D->y = 0;
						lpPtf3D->z = pPreNode->pt.y;
						pptrTri->Add(lpPtf3D);

						lpPtf3D = new point3f; // nIndex
						lpPtf3D->x = endPt.x;
						lpPtf3D->y = 0;
						lpPtf3D->z = endPt.y;
						pptrTri->Add(lpPtf3D);
						// 2. 在nIndex处插入内环
						pptf = (pointf * )SYAR_GetAt(&ptfArray,nMinXIndex );
						pPtNode = new PtNode;
						pPtNode->pt.x = pptf->x;
						pPtNode->pt.y = pptf->y;
						pPtNode->bStart = FALSE;
						if (nIndex == 0) 
						{
							ptPolyArray.InsertAt(nSize, pPtNode);
						}
						else
						{
							ptPolyArray.InsertAt(nIndex, pPtNode);
						}
						//for ( j = 0; j < nInSize; j++) 
						for ( j = nInSize - 1; j >= 0; j--) 
						{
							pptf = (pointf * )SYAR_GetAt(&ptfArray,(j+nMinXIndex)%nInSize );
							pPtNode = new PtNode;
							pPtNode->pt.x = pptf->x;
							pPtNode->pt.y = pptf->y;
							pPtNode->bStart = FALSE;
							if (nIndex == 0) 
							{
								ptPolyArray.InsertAt(nSize, pPtNode);
							}
							else
							{
								ptPolyArray.InsertAt(nIndex, pPtNode);
							}
						}
					}
					else //二分查找
					{
						// 找到线段内某点A
						searchPt.x = ( pPreNode->pt.x + endPt.x) * 0.5f;
						searchPt.y = ( pPreNode->pt.y + endPt.y) * 0.5f;
						while ( ! (searchPt.x < fMinX && !LineIntersection2(startPt, searchPt, ptPolyArray)) ) 
						{
							searchPt.x = ( searchPt.x + endPt.x) * 0.5f;
							searchPt.y = ( searchPt.y + endPt.y) * 0.5f;
						}
						// 加入三角形L, A, nIndex
						lpPtf3D = new point3f;// L
						lpPtf3D->x = startPt.x;
						lpPtf3D->y = 0;
						lpPtf3D->z = startPt.y;
						pptrTri->Add(lpPtf3D);

						lpPtf3D = new point3f; // A
						lpPtf3D->x = searchPt.x;
						lpPtf3D->y = 0;
						lpPtf3D->z = searchPt.y;
						pptrTri->Add(lpPtf3D);

						lpPtf3D = new point3f; // nIndex
						lpPtf3D->x = endPt.x;
						lpPtf3D->y = 0;
						lpPtf3D->z = endPt.y;
						pptrTri->Add(lpPtf3D);
						// 在nIndex处插入A点,再插入内环
						pptf = (pointf * )SYAR_GetAt(&ptfArray,nMinXIndex );
						pPtNode = new PtNode;
						pPtNode->pt.x = pptf->x;
						pPtNode->pt.y = pptf->y;
						pPtNode->bStart = FALSE;
						if (nIndex == 0) 
						{
							ptPolyArray.InsertAt(nSize, pPtNode);
						}
						else
						{
							ptPolyArray.InsertAt(nIndex, pPtNode);
						}
						
						//for ( j = 0; j < nInSize; j++) 
						for ( j = nInSize - 1; j >= 0; j--) 
						{
							pptf = (pointf * )SYAR_GetAt(&ptfArray,(j+nMinXIndex)%nInSize );
							pPtNode = new PtNode;
							pPtNode->pt.x = pptf->x;
							pPtNode->pt.y = pptf->y;
							pPtNode->bStart = FALSE;
							if (nIndex == 0) 
							{
								ptPolyArray.InsertAt(nSize, pPtNode);
							}
							else
							{
								ptPolyArray.InsertAt(nIndex, pPtNode);
							}
						}
						// A
						pPtNode = new PtNode;
						pPtNode->pt.x = searchPt.x;
						pPtNode->pt.y = searchPt.y;
						pPtNode->bStart = FALSE;
						if (nIndex == 0) 
						{
							ptPolyArray.InsertAt(nSize, pPtNode);
						}
						else
						{
							ptPolyArray.InsertAt(nIndex, pPtNode);
						}
					}
				}
				else
				{
					if ( PToLDist((point2f*)&pPostNode->pt, &startPt, &endPt) > ABS_FLOAT_0 ) // 不共线
					{
						if ( pPostNode->pt.x < fMinX && !LineIntersection2(startPt, *(point2f*)&pPostNode->pt, ptPolyArray) ) 
						{
							//选择此点. 即nIndex, nIndex+1, L构成三角形
							// 1. 加入三角形 L, nIndex, nIndex+1
							lpPtf3D = new point3f;// L
							lpPtf3D->x = startPt.x;
							lpPtf3D->y = 0;
							lpPtf3D->z = startPt.y;
							pptrTri->Add(lpPtf3D);

							lpPtf3D = new point3f; // nIndex
							lpPtf3D->x = endPt.x;
							lpPtf3D->y = 0;
							lpPtf3D->z = endPt.y;
							pptrTri->Add(lpPtf3D);

							lpPtf3D = new point3f; // nIndex+1
							lpPtf3D->x = pPostNode->pt.x;
							lpPtf3D->y = 0;
							lpPtf3D->z = pPostNode->pt.y;
							pptrTri->Add(lpPtf3D);
							// 2. 在nIndex+1处插入内环
							pptf = (pointf * )SYAR_GetAt(&ptfArray,nMinXIndex );
							pPtNode = new PtNode;
							pPtNode->pt.x = pptf->x;
							pPtNode->pt.y = pptf->y;
							pPtNode->bStart = FALSE;
							
							if ((nIndex+1)%nSize == 0) 
							{
								ptPolyArray.InsertAt( nIndex+1, pPtNode);
							}
							else
							{
								ptPolyArray.InsertAt((nIndex+1)%nSize, pPtNode);
							}
							
							//for ( j = 0; j < nInSize; j++) 
							for ( j = nInSize - 1; j >= 0; j--) 
							{
								pptf = (pointf * )SYAR_GetAt(&ptfArray,(j+nMinXIndex)%nInSize );
								pPtNode = new PtNode;
								pPtNode->pt.x = pptf->x;
								pPtNode->pt.y = pptf->y;
								pPtNode->bStart = FALSE;
								if ((nIndex+1)%nSize == 0) 
								{
									ptPolyArray.InsertAt( nIndex+1, pPtNode);
								}
								else
								{
									ptPolyArray.InsertAt((nIndex+1)%nSize, pPtNode);
								}
							}
						}
						else //二分查找
						{
							// 找到线段内某点A
							searchPt.x = ( pPostNode->pt.x + endPt.x) * 0.5f;
							searchPt.y = ( pPostNode->pt.y + endPt.y) * 0.5f;
							while ( ! (searchPt.x < fMinX && !LineIntersection2(startPt, searchPt, ptPolyArray)) ) 
							{
								searchPt.x = ( searchPt.x + endPt.x) * 0.5f;
								searchPt.y = ( searchPt.y + endPt.y) * 0.5f;
							}
							// 加入三角形L, nIndex, A
							lpPtf3D = new point3f;// L
							lpPtf3D->x = startPt.x;
							lpPtf3D->y = 0;
							lpPtf3D->z = startPt.y;
							pptrTri->Add(lpPtf3D);

							lpPtf3D = new point3f; // nIndex
							lpPtf3D->x = endPt.x;
							lpPtf3D->y = 0;
							lpPtf3D->z = endPt.y;
							pptrTri->Add(lpPtf3D);

							lpPtf3D = new point3f; // A
							lpPtf3D->x = searchPt.x;
							lpPtf3D->y = 0;
							lpPtf3D->z = searchPt.y;
							pptrTri->Add(lpPtf3D);
							// 在nIndex+1处插入A点,再插入内环
							//A
							pPtNode = new PtNode;
							pPtNode->pt.x = searchPt.x;
							pPtNode->pt.y = searchPt.y;
							pPtNode->bStart = FALSE;
							if ((nIndex+1)%nSize == 0) 
							{
								ptPolyArray.InsertAt( nIndex+1, pPtNode);
							}
							else
							{
								ptPolyArray.InsertAt((nIndex+1)%nSize, pPtNode);
							}

							pptf = (pointf * )SYAR_GetAt(&ptfArray,nMinXIndex );
							pPtNode = new PtNode;
							pPtNode->pt.x = pptf->x;
							pPtNode->pt.y = pptf->y;
							pPtNode->bStart = FALSE;
							if ((nIndex+1)%nSize == 0) 
							{
								ptPolyArray.InsertAt( nIndex+1, pPtNode);
							}
							else
							{
								ptPolyArray.InsertAt((nIndex+1)%nSize, pPtNode);
							}
							//for ( j = 0; j < nInSize; j++) 
							for ( j = nInSize - 1; j >= 0; j--) 
							{
								pptf = (pointf * )SYAR_GetAt(&ptfArray,(j+nMinXIndex)%nInSize );
								pPtNode = new PtNode;
								pPtNode->pt.x = pptf->x;
								pPtNode->pt.y = pptf->y;
								pPtNode->bStart = FALSE;
								if ((nIndex+1)%nSize == 0) 
								{
									ptPolyArray.InsertAt( nIndex+1, pPtNode);
								}
								else
								{
									ptPolyArray.InsertAt((nIndex+1)%nSize, pPtNode);
								}
							}
						}
					}
					else
					{
						delete [] pbMay;
						pbMay = NULL;
						return;
					}
				}
				break;
			}
		}
	}
	pptf = (pointf * )SYAR_GetAt(&ptfArray,nMinXIndex);
	pointf * pptTempNew = new pointf;
	*pptTempNew = *pptf;
	ptfArrAddedMinPts.Add(pptTempNew);
	delete [] pbMay;
	pbMay = NULL;	
} // END OF BreakAndAddCurCell

//--------------------------------------------------------------------------------
//功能：平面非自交多边形Triangulation
//输入：
//输出：
//--------------------------------------------------------------------------------
//void SYTriangulateApp::SYTriangulateLine(IN vectorgraph* pGraph,IN CSYEnvelopeArray* pLineEnvelopeAr,OUT CSYPoint3fPtrArray *pptrTri,OUT CSYPoint3fPtrArray *pptrUV,IN BOOL bAntiClockwise)
//{
//	vectorcell* pCell = NULL;
//	int nEnvelopeNum = (int)pLineEnvelopeAr->GetSize();
//	for(int i=0; i<nEnvelopeNum; i++)
//	{
//		EnvelopeNode *pEnvelopeNode = (EnvelopeNode *)pLineEnvelopeAr->GetAt(i);
//		pCell = &pGraph->cell[pEnvelopeNode->nCellIndex];
//	}
//}
void SYTriangulateApp::SYTriangulateLine(IN vectorgraph* pGraph, OUT OUT CSYPoint3fPtrArray *pptrTri, OUT CSYPoint3fPtrArray *pptrUV, IN BOOL bAntiClockwise)
{
	if (pGraph == NULL || pGraph->ncell < 1) 
	{
		return;
	}

	int ndirect = bAntiClockwise == 1? 1:-1;
	int i;//, j;
	vectorcell* pCell = NULL;
	PtNode* pPtNode = NULL;
	SYArray ptfArray; // 将一个cell转化为多边形顶点
	SYArray lenArray; // 将一个cell转化为多边形顶点
	SYAR_Init(&ptfArray);
	SYAR_Init(&lenArray);

	CSYEnvelopeArray lineelopeArr;
	EnvelopeNode * pEnvelopeNode = NULL;
	int nCellNum = pGraph->ncell;
	//先把外环加入队列中,把线单独列出
	for(i = 0; i < nCellNum; i++)
	{
		int nPtSize = SYAR_GetSize(&ptfArray);
		for(int j = 0; j< nPtSize;j ++)
		{
			point2f* pt = (point2f*)SYAR_GetAt(&ptfArray,j);
			delete pt;
		}
		SYAR_RemoveAll(&ptfArray);
		
		if(pGraph->cell[i].type == uncloss_line || pGraph->cell[i].type == close_line)
		{
			cell_extend_tri(
				&ptfArray,
				&lenArray,
				&pGraph->cell[i],
				1,
				pGraph->cell[i].width*0.5f);

			int nsize =  (int)SYAR_GetSize(&lenArray);
			void* pdata = SYAR_GetAt(&lenArray,nsize -1);
			float falllen = *(float*)(void**)&pdata;
			float falllend = 1/falllen;
			int nSize = (int)SYAR_GetSize(&ptfArray);
			int nLoopCount = nSize - 3;
			if(pGraph->cell[i].type == close_line)
				nLoopCount = nSize - 1;
		
			for(int k = 0;k < nLoopCount;k += 2)
			{
				int nIndex = k%nSize;
				int nIndex_1 = (k + 1)%nSize;
				int nIndex_2 = (k + 2)%nSize;
				int nIndex_3 = (k + 3)%nSize;
				point2f* pt1 = (point2f*)SYAR_GetAt(&ptfArray,nIndex);
				point2f* pt2 = (point2f*)SYAR_GetAt(&ptfArray,nIndex_1);
				point2f* pt3 = (point2f*)SYAR_GetAt(&ptfArray,nIndex_2);
				point2f* pt4 = (point2f*)SYAR_GetAt(&ptfArray,nIndex_3);

				void* pdata = SYAR_GetAt(&lenArray,k>>1);
				float flen1 = *(float*)(void**)&pdata;
				pdata = SYAR_GetAt(&lenArray,(k>>1) + 1);
				float flen2 = *(float*)(void**)&pdata;

				point3f* pnewf1 = new point3f;
				point3f* pnewf2 = new point3f;
				point3f* pnewf3 = new point3f;
				point3f* pnewf4 = new point3f;
				point3f* pnewf5 = new point3f;
				point3f* pnewf6 = new point3f;

				point3f* pnewuv1 = new point3f;
				point3f* pnewuv2 = new point3f;
				point3f* pnewuv3 = new point3f;
				point3f* pnewuv4 = new point3f;
				point3f* pnewuv5 = new point3f;
				point3f* pnewuv6 = new point3f;


				float3fFromfloat2f(pnewf1,pt1,Plane_XZ);
				float3fFromfloat2f(pnewf2,pt2,Plane_XZ);
				float3fFromfloat2f(pnewf3,pt3,Plane_XZ);
				pnewuv1->x = flen1 * falllend;
				pnewuv1->y = 0;
				pnewuv1->z = 0;
				pnewuv2->x = pnewuv1->x;
				pnewuv2->y = 1.0f;
				pnewuv2->z = 0;
				pnewuv3->x = flen2 * falllend;
				pnewuv3->y = 0.0f;
				pnewuv3->z = 0;
				pptrTri->Add(pnewf1);
				pptrTri->Add(pnewf2);
				pptrTri->Add(pnewf3);
				pptrUV->Add(pnewuv1);
				pptrUV->Add(pnewuv2);
				pptrUV->Add(pnewuv3);

				float3fFromfloat2f(pnewf4,pt2,Plane_XZ);
				float3fFromfloat2f(pnewf5,pt4,Plane_XZ);
				float3fFromfloat2f(pnewf6,pt3,Plane_XZ);

				pnewuv4->x = flen1 * falllend;
				pnewuv4->y = 1.0f;
				pnewuv4->z = 0;
				pnewuv5->x = flen2 * falllend;
				pnewuv5->y = 1.0f;
				pnewuv5->z = 0;
				pnewuv6->x = pnewuv5->x;
				pnewuv6->y = 0.0f;
				pnewuv6->z = 0;

				pptrTri->Add(pnewf4);
				pptrTri->Add(pnewf5);
				pptrTri->Add(pnewf6);
				pptrUV->Add(pnewuv4);
				pptrUV->Add(pnewuv5);
				pptrUV->Add(pnewuv6);
			}
		}
	} // end of for

	//删除顶点
	{
		int nPtSize = SYAR_GetSize(&ptfArray);
		for (int i = 0; i < nPtSize; i++)
		{
			point2f * pt = (point2f*)SYAR_GetAt(&ptfArray, i);
			SAFE_DELETE(pt);
		}
		SYAR_RemoveAll(&ptfArray);
	}
	{
		int nPtSize = SYAR_GetSize(&lenArray);
		for (int i = 0; i < nPtSize; i++)
		{
			point2f * pt = (point2f*)SYAR_GetAt(&lenArray, i);
			//SAFE_DELETE(pt);
		}
		SYAR_RemoveAll(&lenArray);
	}
}
//顶点的回调函数
struct PolyDivInfo
{
	SYArray m_IndexAr;
	SYArray m_PolyVectorAr;
};
#include "gl/gl.h"
#include "gl/glu.h"
#pragma comment(lib,"glu32.lib")

void __stdcall begincallback(GLenum type,PolyDivInfo* caller)
{
	SYAR_Init(&caller->m_IndexAr);
}

void __stdcall edgeflagcallback(GLboolean flag,
										  PolyDivInfo* caller)
{
	int a = 0;
}

void __stdcall vectorcallback(unsigned int vertexIndex,
										PolyDivInfo* caller)
{
	SYAR_Add(&caller->m_IndexAr,(void*)vertexIndex);
}


void __stdcall endcallback(PolyDivInfo* caller)
{
}
void __stdcall combinecallback(GLdouble coords[3],unsigned int vertexData[4],GLfloat weight[4],unsigned int* outData,PolyDivInfo* caller)
{
	// create a new vertex with the given coordinates
	point3lf * pvertex = new point3lf;
	pvertex->x = coords[0];
	pvertex->y = coords[1];
	pvertex->z = coords[2];
	SYAR_Add(&caller->m_PolyVectorAr,pvertex);
	//FontVertex newVertex;
	//newVertex.x = coords[0];
	//newVertex.y = coords[1];
	//newVertex.z = coords[2];

	//const FontVertex& vert0 = caller->m_Vertices[vertexData[0]];
	//const FontVertex& vert1 = caller->m_Vertices[vertexData[1]];
	//const FontVertex& vert2 = caller->m_Vertices[vertexData[2]];
	//const FontVertex& vert3 = caller->m_Vertices[vertexData[3]];

	//newVertex.u =
	//	vert0.u * weight[0] +
	//	vert1.u * weight[1] +
	//	vert2.u * weight[2] +
	//	vert3.u * weight[3];
	//newVertex.v =
	//	vert0.v * weight[0] +
	//	vert1.v * weight[1] +
	//	vert2.v * weight[2] +
	//	vert3.v * weight[3];

	//// add the vertex to the calling object's vertex vector
	//caller->m_Vertices.push_back(newVertex);

	//// pass back the index of the new vertex; it will be passed
	//// as the vertexIndex parameter to VertexCB in turn
	*outData =  (int)SYAR_GetSize(&caller->m_PolyVectorAr) - 1;
	//*outData = (unsigned int)caller->m_Vertices.size() - 1;
}


void __stdcall errorcallback(GLenum errno, PolyDivInfo* caller)
{
	int a = 0;
}
typedef void (__stdcall *GluTessCallbackType)();
void DivGraphics(vectorgraph* pGraph,PolyDivInfo* infov)
{
	SYArray ptfArray; // 将一个cell转化为多边形顶点
	SYAR_Init(&ptfArray);
	
	GLUtesselator * tessobj;
	tessobj = gluNewTess();
	//注册回调函数

	gluTessCallback(tessobj, GLU_TESS_BEGIN_DATA, (GluTessCallbackType)begincallback);
	gluTessCallback(tessobj, GLU_TESS_EDGE_FLAG_DATA, (GluTessCallbackType)edgeflagcallback);
	gluTessCallback(tessobj, GLU_TESS_VERTEX_DATA, (GluTessCallbackType)vectorcallback);
	gluTessCallback(tessobj, GLU_TESS_END_DATA, (GluTessCallbackType)endcallback);
	gluTessCallback(tessobj, GLU_TESS_COMBINE_DATA, (GluTessCallbackType)combinecallback);
	gluTessCallback(tessobj, GLU_TESS_ERROR_DATA, (GluTessCallbackType)errorcallback);

	memset(infov,0,sizeof(PolyDivInfo));
	SYAR_Init(&infov->m_IndexAr);
	SYAR_Init(&infov->m_PolyVectorAr);

	//设置环绕规则
	gluTessProperty(tessobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
	gluTessNormal(tessobj,0.0,0.0,1.0f);
	//double dvector[3];
	gluTessBeginPolygon(tessobj, infov);
	int ncell = pGraph->ncell;
	for(int i = 0;i < ncell;i ++)
	{
		vectorcell* pcell = &pGraph->cell[i];
		if(pcell->type == uncloss_line ||pcell->type == close_line)
			continue;

		SYOC_Cell2PtlfPoly(pcell, &ptfArray,NULL,NULL);
		//SYCommon_CallDebug(_T("Cell2PtfPoly"),pcell,&ptfArray,NULL,NULL);
		//SYOC_DelOverlap(&ptfArray,NULL);
		//SYOC_DelColine(&ptfArray,NULL);

		int nPoint = SYAR_GetSize(&ptfArray);
		int nstartindex = SYAR_GetSize(&infov->m_PolyVectorAr);
		SYAR_Append(&infov->m_PolyVectorAr,&ptfArray);

		point3lf* prv = NULL;
		gluTessBeginContour(tessobj);
		for(int k = 0;k< nPoint;k ++)
		{
			point3lf* p = (point3lf*)SYAR_GetAt(&ptfArray,k);
			p->z = 0;
			gluTessVertex(tessobj,(GLdouble*)p,(void*)(k + nstartindex));

			//if(prv != NULL)
			//{
			//	SYCommon_CallDebug(_T("LineTo"),prv,p,&ptfArray,NULL);
			//}
			//prv = p;
			
		}
		SYAR_RemoveAll(&ptfArray);
		gluTessEndContour(tessobj);
	}
	gluTessEndPolygon(tessobj);

}

void SYTriangulateApp::SYTriangulateGL(IN vectorgraph* pGraph, OUT CSYPoint3fPtrArray *pptrTri,OUT CSYPoint3fPtrArray *pptrUV, IN BOOL bAntiClockwise)
{
	PolyDivInfo	infov;
	bAntiClockwise = FALSE;
	DivGraphics(pGraph,&infov);
	int nSize = SYAR_GetSize(&infov.m_IndexAr);
	for(int i = 0;i < nSize;i += 3)
	{
		int nI1 = (int)SYAR_GetAt(&infov.m_IndexAr,i);
		int nI2 = (int)SYAR_GetAt(&infov.m_IndexAr,i + 1);
		int nI3 = (int)SYAR_GetAt(&infov.m_IndexAr,i + 2);

		point3lf* p1 = (point3lf*)SYAR_GetAt(&infov.m_PolyVectorAr,nI1);
		point3lf* p2 = (point3lf*)SYAR_GetAt(&infov.m_PolyVectorAr,nI2);
		point3lf* p3 = (point3lf*)SYAR_GetAt(&infov.m_PolyVectorAr,nI3);

		point3f* pnew1 = new point3f;
		pnew1->x = p1->x;
		pnew1->y = 0;
		pnew1->z = p1->y;
		point3f* pnew2 = new point3f;
		pnew2->x = p2->x;
		pnew2->y = 0;
		pnew2->z = p2->y;
		point3f* pnew3 = new point3f;
		pnew3->x = p3->x;
		pnew3->y = 0;
		pnew3->z = p3->y;
		if(bAntiClockwise)
		{
			pptrTri->Add(pnew1);
			pptrTri->Add(pnew2);
			pptrTri->Add(pnew3);
		}
		else
		{
			pptrTri->Add(pnew1);
			pptrTri->Add(pnew3);
			pptrTri->Add(pnew2);
		}
	}
	nSize = SYAR_GetSize(&infov.m_IndexAr);
	SYAR_RemoveAll(&infov.m_IndexAr);
	nSize = SYAR_GetSize(&infov.m_PolyVectorAr);
	for(int i = 0;i < nSize;i ++)
	{
		point3lf* p1 = (point3lf*)SYAR_GetAt(&infov.m_PolyVectorAr,i);
		delete p1;
	}
	SYAR_RemoveAll(&infov.m_PolyVectorAr);
}
void SYTriangulateApp::SYTriangulate(IN vectorgraph* pGraph, OUT CSYPoint3fPtrArray *pptrTri,OUT CSYPoint3fPtrArray *pptrUV, IN BOOL bAntiClockwise)
{
	if (pGraph == NULL || pGraph->ncell < 1) 
	{
		return;
	}
	SYTriangulateLine(pGraph, pptrTri, pptrUV, bAntiClockwise);

	int i, j;
	vectorcell* pCell = NULL;
	PtNode* pPtNode = NULL;
	SYArray ptfArray; // 将一个cell转化为多边形顶点
	SYAR_Init(&ptfArray);
	CPtNodeArray ptPolyArray; // 顶点数组. 将一个处理单元内各Cell转化出来的顶点,全部放在内
	CLineNodeArray lineArray; // 边数组
	CTriangleArray TriangleArray; // 三角形数组
	int nCellNum = pGraph->ncell; // cell数目
	BOOL *bOutLoop = new BOOL[nCellNum];
	InitOutLoopArr(bOutLoop, pGraph, bAntiClockwise); // 调用IsAntiClockwise(pCell);

	CSYEnvelopeArray envelopeArr;
	EnvelopeNode * pEnvelopeNode = NULL;
	//先把外环加入队列中,把线单独列出
	for(i = 0; i < nCellNum; i++)
	{
		if (bOutLoop[i] && pGraph->cell[i].type != uncloss_line && pGraph->cell[i].type != close_line) 
		{
			pEnvelopeNode = new EnvelopeNode;
			pEnvelopeNode->nCellIndex = i;
			pEnvelopeNode->fXMin = min(pGraph->cell[i].bound.left,pGraph->cell[i].bound.right);
			pEnvelopeNode->fXMax = max(pGraph->cell[i].bound.left,pGraph->cell[i].bound.right);
			pEnvelopeNode->fYMin = min(pGraph->cell[i].bound.top,pGraph->cell[i].bound.bottom);
			pEnvelopeNode->fYMax = max(pGraph->cell[i].bound.top,pGraph->cell[i].bound.bottom);
			pEnvelopeNode->nextNode = NULL;
			envelopeArr.Add(pEnvelopeNode);
		} // end of if
	} // end of for


	EnvelopeNode * pEnvelopeNodeNew = NULL;
	int nEnvelopeNum = (int)envelopeArr.GetSize();

	//逐步填入内环
	for(i = 0; i < nCellNum; i++)
	{
		if (!bOutLoop[i] && pGraph->cell[i].type != uncloss_line && pGraph->cell[i].type != close_line) 
		{
			pEnvelopeNodeNew = new EnvelopeNode;
			pEnvelopeNodeNew->nCellIndex = i;
			pEnvelopeNodeNew->fXMin = min(pGraph->cell[i].bound.left,pGraph->cell[i].bound.right);
			pEnvelopeNodeNew->fXMax = max(pGraph->cell[i].bound.left,pGraph->cell[i].bound.right);
			pEnvelopeNodeNew->fYMin = min(pGraph->cell[i].bound.top,pGraph->cell[i].bound.bottom);
			pEnvelopeNodeNew->fYMax = max(pGraph->cell[i].bound.top,pGraph->cell[i].bound.bottom);

			pEnvelopeNodeNew->nextNode = NULL; // 先指向自己
			for(j=0; j<nEnvelopeNum; j++)
			{
				pEnvelopeNode = (EnvelopeNode *)envelopeArr.GetAt(j);
				if(   pEnvelopeNodeNew->fXMin > pEnvelopeNode->fXMin && pEnvelopeNodeNew->fYMin > pEnvelopeNode->fYMin
					&& pEnvelopeNodeNew->fXMax < pEnvelopeNode->fXMax && pEnvelopeNodeNew->fYMax < pEnvelopeNode->fYMax
					)//pEnvelopeNode包含pEnvelopeNodeNew
				{
					if( pEnvelopeNodeNew->nextNode == NULL || (pEnvelopeNode->fXMax - pEnvelopeNode->fXMin) < (pEnvelopeNodeNew->nextNode->fXMax - pEnvelopeNodeNew->nextNode->fXMin) )
					{
						pEnvelopeNodeNew->nextNode = pEnvelopeNode;
					}
				}
			}
			
			//WLM_MODIFY TEMP
			pEnvelopeNode = pEnvelopeNodeNew->nextNode;
			EnvelopeNode * pEnvelopeNodePre = pEnvelopeNode;
			while (pEnvelopeNode && pEnvelopeNodeNew->fXMin > pEnvelopeNode->fXMin) 
			{
				pEnvelopeNodePre = pEnvelopeNode;
				pEnvelopeNode = pEnvelopeNode->nextNode;
			}
			if(pEnvelopeNodePre)
			{
				pEnvelopeNodeNew->nextNode = pEnvelopeNodePre->nextNode;
				pEnvelopeNodePre->nextNode = pEnvelopeNodeNew;
			}
			
		} // end of if 
	} // end of for

	// 先去掉空的cell
	for(i=0; i<nEnvelopeNum; i++)
	{
		pEnvelopeNode = (EnvelopeNode *)envelopeArr.GetAt(i);
		if(!pEnvelopeNode)
		{
			envelopeArr.RemoveAt(i);
			nEnvelopeNum--;
			i--;
			continue;
		}
		pCell = &pGraph->cell[pEnvelopeNode->nCellIndex];
		if(pCell->npoint < 2)
		{
			delete pEnvelopeNode;
			pEnvelopeNode = NULL;
			envelopeArr.RemoveAt(i);
			nEnvelopeNum--;
			i--;
			continue;
		}
		while (pEnvelopeNode->nextNode) 
		{
			pCell = &pGraph->cell[pEnvelopeNode->nextNode->nCellIndex];
			if(pCell->npoint < 2)
			{
				EnvelopeNode * pEnvelopeNodeTemp = pEnvelopeNode->nextNode;
				pEnvelopeNode->nextNode = pEnvelopeNodeTemp->nextNode;
				pEnvelopeNodeTemp->nextNode = NULL;
				delete pEnvelopeNodeTemp;
				pEnvelopeNodeTemp = NULL;
				continue;
			}
			pEnvelopeNode = pEnvelopeNode->nextNode;
		}
	}


	int ndirect = bAntiClockwise == 1? 1:-1;
	int calccount = 1;
	vectorcell cellline[2];
	memset(&cellline,0,sizeof(vectorcell) * 2);



	for(i=0; i<nEnvelopeNum; i++)
	{
		pEnvelopeNode = (EnvelopeNode *)envelopeArr.GetAt(i);
		BOOL bOut = TRUE;
		CSYPoint2fPtrArray ptfArrAddedMinPts; 
		while (pEnvelopeNode) 
		{
			pCell = &pGraph->cell[pEnvelopeNode->nCellIndex];
			if(pCell->npoint == 0)
			{
				pEnvelopeNode = pEnvelopeNode->nextNode;
				continue;
			}
			SYOC_Cell2PtfPoly(pCell, &ptfArray,NULL,NULL);
			SYOC_DelOverlap(&ptfArray,NULL);
			SYOC_DelColine(&ptfArray,NULL);

			int nDesSize = (int)ptPolyArray.GetSize();
			pointf ptOld;
			ptOld.x = 999999.0f;
			ptOld.y = 999999.0f;

			int nSize = (int)SYAR_GetSize(&ptfArray);
			if (nSize < 3) 
			{
				// 释放
				for (j = 0; j<nSize; j++)
				{
					pointf* pptf = (pointf*)SYAR_GetAt(&ptfArray,j);
					delete pptf;
					pptf = NULL;
				}
				SYAR_RemoveAll(&ptfArray);
				pEnvelopeNode = pEnvelopeNode->nextNode;
				continue;
			}
			pointf* pptf0 = (pointf * )SYAR_GetAt(&ptfArray,0);
			pointf* pptf1 = (pointf * )SYAR_GetAt(&ptfArray,nSize - 1);
			if (fabs(pptf0->x - pptf1->x) < ABS_FLOAT_0 && fabs(pptf0->y - pptf1->y) < ABS_FLOAT_0)
			{
				SYAR_RemoveAt(&ptfArray,nSize - 1,1);
				delete pptf1;
				pptf1 = NULL;
			}

			nSize = (int)SYAR_GetSize(&ptfArray);
			if (bOut) 
			{
				for(j = 0 ;j< nSize;j ++)
				{
					point3f* pptf = (point3f * )SYAR_GetAt(&ptfArray,j);
					if(fabs(pptf->x - ptOld.x) < ABS_FLOAT_0 && fabs(pptf->y - ptOld.y) < ABS_FLOAT_0)
						continue;
					pPtNode = new PtNode;
					pPtNode->pt.x = pptf->x;
					pPtNode->pt.y = pptf->y;
					pPtNode->pt.z = pptf->z;
					pPtNode->bStart = FALSE;
					ptPolyArray.Add(pPtNode);
					ptOld = *(point2f*)pptf;
				}


				if(ptPolyArray.GetSize() > nDesSize)
				{
					ptPolyArray[nDesSize]->bStart = TRUE;
				}
				bOut = FALSE;
			}
			else
			{
				// 修改此处. 使得加入时, 便去除掉内环.
				// 选择此内环上最左一点(x最小的)L
				float fMinX = 65535.f;
				int nMinXIndex = 0;
				for(j = 0 ;j< nSize;j ++)
				{
					pointf* pptf = (pointf * )SYAR_GetAt(&ptfArray,j);
					if(fabs(pptf->x - ptOld.x) < ABS_FLOAT_0 && fabs(pptf->y - ptOld.y) < ABS_FLOAT_0)
						continue;
					if (pptf->x < fMinX) 
					{
						fMinX = pptf->x;
						nMinXIndex = j;
					}
					ptOld = *pptf;
				}
				// 从外环上找到一点P, 使得Px<Lx(即fMinX), 并且PL不与外环相交
				BreakAndAddCurCell(pptrTri, ptPolyArray, ptfArray, nMinXIndex, fMinX, ptfArrAddedMinPts);
				//bOnce = FALSE;
			}
			
			// 释放
			nSize = (int)SYAR_GetSize(&ptfArray);
			for (j = 0; j<nSize; j++)
			{
				pointf* pptf = (pointf*)SYAR_GetAt(&ptfArray,j);
				delete pptf;
				pptf = NULL;
			}
			SYAR_RemoveAll(&ptfArray);
			if(pEnvelopeNode)
				pEnvelopeNode = pEnvelopeNode->nextNode;
		} // end of while
		for(int jjj = 0; jjj < ptfArrAddedMinPts.GetSize(); jjj ++)
		{
			pointf *pptfTemp = (pointf*)ptfArrAddedMinPts.GetAt(jjj);
			delete pptfTemp;
			pptfTemp = NULL;
		}
		ptfArrAddedMinPts.RemoveAll();
		
		// 依次去掉各内环. 找到最左点, 然后找到外环上在此点左侧的一个点, 使得二者之间的线段不与外环任何线段相交.
		InitLines( &ptPolyArray, &lineArray);
		//InitLines2( &ptPolyArray, &lineArray, &TriangleArray, bAntiClockwise);
		//Triangulate2( &ptPolyArray, &lineArray, &TriangleArray, bAntiClockwise);
		Triangulate3WithPoly( &ptPolyArray, &TriangleArray, bAntiClockwise);
		//Triangulate3( &ptPolyArray, &lineArray, &TriangleArray, bAntiClockwise);
		//SYProgress_SetValue((1.0f / nEnvelopeNum) * (i + 1));

		point3f* lpPtf3D = NULL;
		int nTriNum = (int)TriangleArray.GetSize();
		for (j=0; j<nTriNum; j++)
		{
			Triangle* pTriangle = (Triangle*)TriangleArray.GetAt(j);
			pPtNode = (PtNode*)ptPolyArray.GetAt(pTriangle->nPt1); // 一个点
			lpPtf3D = new point3f;
			lpPtf3D->x = pPtNode->pt.x;
			lpPtf3D->y = 0;
			lpPtf3D->z = pPtNode->pt.y;
			pptrTri->Add(lpPtf3D);

			pPtNode = (PtNode*)ptPolyArray.GetAt(pTriangle->nPt2); // 一个点
			lpPtf3D = new point3f;
			lpPtf3D->x = pPtNode->pt.x;
			lpPtf3D->y = 0;
			lpPtf3D->z = pPtNode->pt.y;
			pptrTri->Add(lpPtf3D);

			pPtNode = (PtNode*)ptPolyArray.GetAt(pTriangle->nPt3); // 一个点
			lpPtf3D = new point3f;
			lpPtf3D->x = pPtNode->pt.x;
			lpPtf3D->y = 0;
			lpPtf3D->z = pPtNode->pt.y;
			pptrTri->Add(lpPtf3D);
		}
		// 释放TriangleArray
		for (j=0; j<nTriNum; j++)
		{
			Triangle* pTriangle = (Triangle*)TriangleArray.GetAt(j);
			delete pTriangle;
			pTriangle = NULL;
		}
		TriangleArray.RemoveAll();

		//释放ptPolyArray, lineArray
		int nSize = (int)ptPolyArray.GetSize();
		for (j=0; j<nSize; j++)
		{
			pPtNode = (PtNode*)ptPolyArray.GetAt(j);
			delete pPtNode;
			pPtNode = NULL;
		}
		ptPolyArray.RemoveAll();

		nSize = (int)lineArray.GetSize();
		for (j=0; j<nSize; j++)
		{
			LineNode* pLineNode = (LineNode*)lineArray.GetAt(j);
			delete pLineNode;
			pLineNode = NULL;
		}
		lineArray.RemoveAll();
	}// end of for

	// 释放
	for(i = (int)envelopeArr.GetSize()-1; i>-1; i--)
	{
		pEnvelopeNode = (EnvelopeNode *)envelopeArr.GetAt(i);
		EnvelopeNode * pNextNode = pEnvelopeNode->nextNode;
		while (pEnvelopeNode)
		{
			pNextNode = pEnvelopeNode->nextNode;
			delete pEnvelopeNode;
			pEnvelopeNode = pNextNode;
		}
		pEnvelopeNode = NULL;
	} // end of for
	envelopeArr.RemoveAll();

	delete []bOutLoop;
	bOutLoop = NULL;

}// END OF SYTriangulate

//--------------------------------------------------------------------------------
//功能：平面非自交多边形Triangulation
//输入：
//输出：
//--------------------------------------------------------------------------------
void SYTriangulateApp::SYTriangulate(IN vectorgraph* pGraph, pointf* pt, int nPtSize, int* nUnitStart, int nUnit, int nCurUnit, int* nCellStart, int nCell, OUT CSYPoint3fPtrArray *pptrTri, IN BOOL bAntiClockwise)
{
	if (pGraph == NULL || pGraph->ncell < 1) 
	{
		return;
	}

	int i, j;
	vectorcell* pCell = NULL;
	PtNode* pPtNode = NULL;
	SYArray ptfArray; // 将一个cell转化为多边形顶点
	CPtNodeArray ptPolyArray; // 顶点数组. 将一个处理单元内各Cell转化出来的顶点,全部放在内
	CLineNodeArray lineArray; // 边数组
	CTriangleArray TriangleArray; // 三角形数组
	int nCellNum = pGraph->ncell; // cell数目
	BOOL *bOutLoop = new BOOL[nCellNum];
	InitOutLoopArr(bOutLoop, pGraph, bAntiClockwise); // 调用IsAntiClockwise(pCell);

	CSYEnvelopeArray envelopeArr;
	EnvelopeNode * pEnvelopeNode = NULL;
	for(i = 0; i < nCellNum; i++)
	{
		if (bOutLoop[i]) 
		{
			pEnvelopeNode = new EnvelopeNode;
			pEnvelopeNode->nCellIndex = i;
			pEnvelopeNode->fXMin = pGraph->cell[i].bound.left;
			pEnvelopeNode->fXMax = pGraph->cell[i].bound.right;
			pEnvelopeNode->fYMin = pGraph->cell[i].bound.top;
			pEnvelopeNode->fYMax = pGraph->cell[i].bound.bottom;
			pEnvelopeNode->nextNode = NULL;
			envelopeArr.Add(pEnvelopeNode);
		} // end of if
	} // end of for

	EnvelopeNode * pEnvelopeNodeNew = NULL;
	int nEnvelopeNum = (int)envelopeArr.GetSize();
	for(i = 0; i < nCellNum; i++)
	{
		if (!bOutLoop[i]) 
		{
			pEnvelopeNodeNew = new EnvelopeNode;
			pEnvelopeNodeNew->nCellIndex = i;
			pEnvelopeNodeNew->fXMin = pGraph->cell[i].bound.left;
			pEnvelopeNodeNew->fXMax = pGraph->cell[i].bound.right;
			pEnvelopeNodeNew->fYMin = pGraph->cell[i].bound.top;
			pEnvelopeNodeNew->fYMax = pGraph->cell[i].bound.bottom;
			pEnvelopeNodeNew->nextNode = NULL; // 先指向自己
			for(j=0; j<nEnvelopeNum; j++)
			{
				pEnvelopeNode = (EnvelopeNode *)envelopeArr.GetAt(j);
				if(   pEnvelopeNodeNew->fXMin > pEnvelopeNode->fXMin && pEnvelopeNodeNew->fYMin > pEnvelopeNode->fYMin
					&& pEnvelopeNodeNew->fXMax < pEnvelopeNode->fXMax && pEnvelopeNodeNew->fYMax < pEnvelopeNode->fYMax
					)//pEnvelopeNode包含pEnvelopeNodeNew
				{
					if( pEnvelopeNodeNew->nextNode == NULL || (pEnvelopeNode->fXMax - pEnvelopeNode->fXMin) < (pEnvelopeNodeNew->nextNode->fXMax - pEnvelopeNodeNew->nextNode->fXMin) )
					{
						pEnvelopeNodeNew->nextNode = pEnvelopeNode;
					}
				}
			}
			
			//WLM_MODIFY TEMP
			pEnvelopeNode = pEnvelopeNodeNew->nextNode;
			EnvelopeNode * pEnvelopeNodePre = pEnvelopeNode;
			while (pEnvelopeNode && pEnvelopeNodeNew->fXMin > pEnvelopeNode->fXMin) 
			{
				pEnvelopeNodePre = pEnvelopeNode;
				pEnvelopeNode = pEnvelopeNode->nextNode;
			}

			if(pEnvelopeNodePre)
			{
				pEnvelopeNodeNew->nextNode = pEnvelopeNodePre->nextNode;
				pEnvelopeNodePre->nextNode = pEnvelopeNodeNew;
			}
			
		} // end of if 
	} // end of for

	// 先去掉空的cell
	for(i=0; i<nEnvelopeNum; i++)
	{
		pEnvelopeNode = (EnvelopeNode *)envelopeArr.GetAt(i);
		if (pEnvelopeNode) 
		{
			pCell = &pGraph->cell[pEnvelopeNode->nCellIndex];
			if(pCell->npoint < 2)
			{
				delete pEnvelopeNode;
				pEnvelopeNode = NULL;
				envelopeArr.RemoveAt(i);
				nEnvelopeNum--;
				i--;
				continue;
			}
		}
		else
		{
			delete pEnvelopeNode;
			pEnvelopeNode = NULL;
			envelopeArr.RemoveAt(i);
			nEnvelopeNum--;
			i--;
			continue;
		}
		while (pEnvelopeNode->nextNode) 
		{
			pCell = &pGraph->cell[pEnvelopeNode->nextNode->nCellIndex];
			if(pCell->npoint < 2)
			{
				EnvelopeNode * pEnvelopeNodeTemp = pEnvelopeNode->nextNode;
				pEnvelopeNode->nextNode = pEnvelopeNodeTemp->nextNode;
				pEnvelopeNodeTemp->nextNode = NULL;
				delete pEnvelopeNodeTemp;
				pEnvelopeNodeTemp = NULL;
				continue;
			}
			pEnvelopeNode = pEnvelopeNode->nextNode;
		}
	}
	int nCellStartIndex = 0;
	while (nUnitStart[nCurUnit] != nCellStart[nCellStartIndex])
	{		
		nCellStartIndex++;
	}
	for(i=0; i<nEnvelopeNum; i++)
	{
		pEnvelopeNode = (EnvelopeNode *)envelopeArr.GetAt(i);
		BOOL bOut = TRUE;
		CSYPoint2fPtrArray ptfArrAddedMinPts; 
		while (pEnvelopeNode) 
		{
			//////////////////////////////////////////////////////////////////////// begin
			// new 
			int nStart = nCellStart[nCellStartIndex + pEnvelopeNode->nCellIndex];
			int nEnd;
			if (nCellStartIndex + 1 == nCell)
			{
				nEnd = nPtSize;
			}
			else
			{
				nEnd = nUnitStart[nCellStartIndex + 1];
			}

			if(nEnd - nStart < 2)
			{
				pEnvelopeNode = pEnvelopeNode->nextNode;
				continue;
			}

			for (int kkk = nStart; kkk < nEnd; kkk++)
			{
				pointf *pptf = new pointf;
				*pptf = pt[kkk];
				SYAR_Add(&ptfArray,pptf);
			}

			int nDesSize = (int)ptPolyArray.GetSize();
			pointf ptOld;
			ptOld.x = 999999.0f;
			ptOld.y = 999999.0f;
			int nSize = (int)SYAR_GetSize(&ptfArray);
			j = nSize - 1;
			for(j = nSize - 1; j>= 0; j --)
			while (j) 
			{
				pointf* pptf = (pointf*)SYAR_GetAt(&ptfArray,j);
				for (int k = j - 1; k>=0; k--)
				{
					pointf* pptfk = (pointf*)SYAR_GetAt(&ptfArray,k);
					if(fabs(pptf->x - pptfk->x) < ABS_FLOAT_0 && fabs(pptf->y - pptfk->y) < ABS_FLOAT_0)
					{
						SYAR_RemoveAt(&ptfArray,k,1);
						delete pptfk;
						pptfk = NULL;					
					}
					else
					{
						j = k;
						break;
					}
				} // end of for
			} // end of while

			SYOC_DelColine(&ptfArray,NULL);

			nSize = (int)SYAR_GetSize(&ptfArray);
			if (nSize < 3) 
			{
				for (j = 0; j<nSize; j++)
				{
					pointf* pptf = (pointf*)SYAR_GetAt(&ptfArray,j);
					delete pptf;
					pptf = NULL;
				}
				SYAR_RemoveAll(&ptfArray);
				pEnvelopeNode = pEnvelopeNode->nextNode;
				continue;
			}
			pointf* pptf0 = (pointf * )SYAR_GetAt(&ptfArray,0);
			pointf* pptf1 = (pointf * )SYAR_GetAt(&ptfArray,nSize - 1);
			if (fabs(pptf0->x - pptf1->x) < ABS_FLOAT_0 && fabs(pptf0->y - pptf1->y) < ABS_FLOAT_0)
			{
				SYAR_RemoveAt(&ptfArray,nSize - 1,1);
				delete pptf1;
				pptf1 = NULL;
			}

			nSize = (int)SYAR_GetSize(&ptfArray);
			if (bOut) 
			{
				for(j = 0 ;j< nSize;j ++)
				{
					pointf* pptf = (pointf * )SYAR_GetAt(&ptfArray,j);
					if(fabs(pptf->x - ptOld.x) < ABS_FLOAT_0 && fabs(pptf->y - ptOld.y) < ABS_FLOAT_0)
						continue;
					pPtNode = new PtNode;
					pPtNode->pt.x = pptf->x;
					pPtNode->pt.y = pptf->y;
					pPtNode->bStart = FALSE;
					ptPolyArray.Add(pPtNode);
					ptOld = *pptf;
				}


				if(ptPolyArray.GetSize() > nDesSize)
				{
					ptPolyArray[nDesSize]->bStart = TRUE;
				}
				bOut = FALSE;
			}
			else
			{
				// 修改此处. 使得加入时, 便去除掉内环.
				// 选择此内环上最左一点(x最小的)L
				//static bOnce = TRUE;
				//if (bOnce == FALSE) 
				//{
				//	// 释放
				//	nSize = (int)ptfArray.GetSize();
				//	for (j = 0; j<nSize; j++)
				//	{
				//		pointf* pptf = ptfArray.GetAt(j);
				//		delete pptf;
				//		pptf = NULL;
				//	}
				//	ptfArray.RemoveAll();
				//	break;
				//}
				float fMinX = 65535.f;
				int nMinXIndex = 0;
				for(j = 0 ;j< nSize;j ++)
				{
					pointf* pptf = (pointf * )SYAR_GetAt(&ptfArray,j);
					if(fabs(pptf->x - ptOld.x) < ABS_FLOAT_0 && fabs(pptf->y - ptOld.y) < ABS_FLOAT_0)
						continue;
					if (pptf->x < fMinX) 
					{
						fMinX = pptf->x;
						nMinXIndex = j;
					}
					ptOld = *pptf;
				}
				// 从外环上找到一点P, 使得Px<Lx(即fMinX), 并且PL不与外环相交
				BreakAndAddCurCell(pptrTri, ptPolyArray, ptfArray, nMinXIndex, fMinX, ptfArrAddedMinPts);
				//bOnce = FALSE;
			}
			
			// 释放
			nSize = (int)SYAR_GetSize(&ptfArray);
			for (j = 0; j<nSize; j++)
			{
				pointf* pptf = (pointf*)SYAR_GetAt(&ptfArray,j);
				delete pptf;
				pptf = NULL;
			}
			SYAR_RemoveAll(&ptfArray);
			pEnvelopeNode = pEnvelopeNode->nextNode;
		} // end of while

		for(int jjj = 0; jjj < ptfArrAddedMinPts.GetSize(); jjj ++)
		{
			pointf *pptfTemp = (pointf*)ptfArrAddedMinPts.GetAt(jjj);
			delete pptfTemp;
			pptfTemp = NULL;
		}
		ptfArrAddedMinPts.RemoveAll();
		
		// 依次去掉各内环. 找到最左点, 然后找到外环上在此点左侧的一个点, 使得二者之间的线段不与外环任何线段相交.
		InitLines( &ptPolyArray, &lineArray);
		//InitLines2( &ptPolyArray, &lineArray, &TriangleArray, bAntiClockwise);
		//Triangulate2( &ptPolyArray, &lineArray, &TriangleArray, bAntiClockwise);
		Triangulate3( &ptPolyArray, &lineArray, &TriangleArray, bAntiClockwise);
		//SYProgress_SetValue((1.0f / nEnvelopeNum) * (i + 1));

		point3f* lpPtf3D = NULL;
		int nTriNum = (int)TriangleArray.GetSize();
		for (j=0; j<nTriNum; j++)
		{
			Triangle* pTriangle = (Triangle*)TriangleArray.GetAt(j);
			pPtNode = (PtNode*)ptPolyArray.GetAt(pTriangle->nPt1); // 一个点
			lpPtf3D = new point3f;
			lpPtf3D->x = pPtNode->pt.x;
			lpPtf3D->y = 0;
			lpPtf3D->z = pPtNode->pt.y;
			pptrTri->Add(lpPtf3D);

			pPtNode = (PtNode*)ptPolyArray.GetAt(pTriangle->nPt2); // 一个点
			lpPtf3D = new point3f;
			lpPtf3D->x = pPtNode->pt.x;
			lpPtf3D->y = 0;
			lpPtf3D->z = pPtNode->pt.y;
			pptrTri->Add(lpPtf3D);

			pPtNode = (PtNode*)ptPolyArray.GetAt(pTriangle->nPt3); // 一个点
			lpPtf3D = new point3f;
			lpPtf3D->x = pPtNode->pt.x;
			lpPtf3D->y = 0;
			lpPtf3D->z = pPtNode->pt.y;
			pptrTri->Add(lpPtf3D);
		}
		// 释放TriangleArray
		for (j=0; j<nTriNum; j++)
		{
			Triangle* pTriangle = (Triangle*)TriangleArray.GetAt(j);
			delete pTriangle;
			pTriangle = NULL;
		}
		TriangleArray.RemoveAll();

		//释放ptPolyArray, lineArray
		int nSize = (int)ptPolyArray.GetSize();
		for (j=0; j<nSize; j++)
		{
			pPtNode = (PtNode*)ptPolyArray.GetAt(j);
			delete pPtNode;
			pPtNode = NULL;
		}
		ptPolyArray.RemoveAll();

		nSize = (int)lineArray.GetSize();
		for (j=0; j<nSize; j++)
		{
			LineNode* pLineNode = (LineNode*)lineArray.GetAt(j);
			delete pLineNode;
			pLineNode = NULL;
		}
		lineArray.RemoveAll();
	} // end of for

	// 释放
	for(i = (int)envelopeArr.GetSize()-1; i>-1; i--)
	{
		pEnvelopeNode = (EnvelopeNode *)envelopeArr.GetAt(i);
		pEnvelopeNode->nextNode = NULL;
		delete pEnvelopeNode;
		pEnvelopeNode = NULL;
	} // end of for
	envelopeArr.RemoveAll();

	delete []bOutLoop;
	bOutLoop = NULL;

}// END OF SYTriangulate

//--------------------------------------------------------------------------------
//功能：平面非自交多边形Triangulation
//输入：多边形顶点数组
//输出：三角形数组（每个元素含三个顶点）
//说明: 没有洞
//--------------------------------------------------------------------------------
void SYTriangulateApp::Triangulate2(CPtNodeArray* pptPolyArray, CLineNodeArray* pLineArray, CTriangleArray* pTriangleArray, BOOL bAntiClockWise)
{
	///算法开始
	int i = 0;
	Triangle *pTriangle = NULL;
	
	int nLineCount = (int)pLineArray->GetSize(); 

	LineNode *pLineNode = NULL;
	PtNode *pPtNode = NULL;

	pointf pt01,pt02,ptk2;
	int k = 0;
	int nTheOne = 0;
	int nPt1=0, nPt2=0;
	if(nLineCount <3) 
		return ; //输入非法 

	while(nLineCount >= 3)
	{
		pLineNode = (LineNode*)pLineArray->GetAt(0);
		nPt1 = pLineNode->nStartPt;
		pPtNode = (PtNode*)pptPolyArray->GetAt(nPt1);
		pt01.x = pPtNode->pt.x;
		pt01.y = pPtNode->pt.y;
		nPt2 = pLineNode->nEndPt;
		pPtNode = (PtNode*)pptPolyArray->GetAt(nPt2);
		pt02.x = pPtNode->pt.x;
		pt02.y = pPtNode->pt.y;
		for ( k =1; k < nLineCount-1; k++ )//inner for-loop
		{
			pLineNode = (LineNode*)pLineArray->GetAt(k);//
			if(pLineNode->nEndPt == nPt1 || pLineNode->nEndPt == nPt2)
			{
				continue;
			}
			pPtNode = (PtNode*)pptPolyArray->GetAt(pLineNode->nEndPt);
			ptk2.x = pPtNode->pt.x;
			ptk2.y = pPtNode->pt.y;
			if( !PtOnLeft(pt01, pt02, ptk2, bAntiClockWise) )
			{
				continue;
			}

			if( LineIntersection(ptk2, pt01, pLineArray, pptPolyArray) 
				|| LineIntersection(pt02, ptk2, pLineArray, pptPolyArray) 
				)
			{
				continue;
			}

			// 点位于三角形pLineNode->nEndPt, nPt1, nPt2外
			BOOL bPtInTri = FALSE;
			int nLineSize = (int)pLineArray->GetSize();
			for ( int kk = 0; kk < nLineSize; kk++)
			{
				LineNode* pLineNodeTemp = (LineNode*)pLineArray->GetAt(kk);
				PtNode* pEPtNode = (PtNode*)pptPolyArray->GetAt(pLineNodeTemp->nEndPt);
				if (pEPtNode->pt == pt01 || pEPtNode->pt == pt02 || pEPtNode->pt == ptk2) 
				{
					continue;
				}
				if ( PtOnLeft(pt01, pt02, *(pointf*)&pEPtNode->pt, bAntiClockWise)
					&& PtOnLeft(pt02, ptk2, *(pointf*)&pEPtNode->pt, bAntiClockWise)
					&& PtOnLeft(ptk2, pt01, *(pointf*)&pEPtNode->pt, bAntiClockWise)
					) 
				{
					bPtInTri = TRUE;
					break;
				}
			}
			if (bPtInTri) 
			{
				continue;
			}

			// found
			nTheOne = pLineNode->nEndPt;

			if(nPt1 != nPt2 && nPt2 != nTheOne && nPt1 != nTheOne)
			{
				pTriangle = new Triangle;
				pTriangle->nPt1 = nPt1;
				pTriangle->nPt2 = nPt2;
				pTriangle->nPt3 = nTheOne;
				pTriangleArray->Add(pTriangle);

				int m = -1;
				int n = -1;
				int nOldLine = CountLineInArray(nPt1, nPt2, nTheOne, pLineArray, m, n);

				if(nOldLine > 2)
				{
					int nTest = 1;
					return;
				}

				if(nOldLine == 0)
				{
					// 要分成两个回路继续
					CLineNodeArray LineArray2;
					
					pLineNode = new LineNode;
					pLineNode->nStartPt = nPt1;
					pLineNode->nEndPt = nTheOne;
					LineArray2.Add(pLineNode);

					for( int nIndex = k+1; nIndex < nLineCount; nIndex++) 
					{
						pLineNode = (LineNode*)pLineArray->GetAt(nIndex);
						LineArray2.Add(pLineNode);
					}

					for( int nIndex = nLineCount - 1;nIndex > k ; nIndex--) 
					{
						pLineArray->RemoveAt(nIndex);
					}
					pLineNode = new LineNode;
					pLineNode->nStartPt = nTheOne;
					pLineNode->nEndPt = nPt2;
					pLineArray->Add(pLineNode);

					Triangulate2(pptPolyArray, &LineArray2, pTriangleArray, bAntiClockWise);
					//Triangulate2(pptPolyArray, pLineArray, pTriangleArray, bAntiClockWise);
					//return;
				}
				else if(nOldLine == 1)
				{
					int nWhichLine = 0;
					pLineNode = (LineNode*)pLineArray->GetAt(m);
					if(pLineNode->nStartPt == nTheOne)
					{
						nWhichLine = 2;
					}
					delete pLineNode;
					pLineNode = NULL;
					pLineArray->RemoveAt(m);

					pLineNode = new LineNode;
					if(nWhichLine == 2)
					{
						pLineNode->nStartPt = nTheOne;
						pLineNode->nEndPt = nPt2;
					}
					else
					{
						pLineNode->nStartPt = nPt1;
						pLineNode->nEndPt = nTheOne;
					}			
					pLineArray->Add(pLineNode);

					// 切掉一个角, 继续本回路的剖分
				}
				else if(nOldLine == 2)
				{
					pLineNode = (LineNode*)pLineArray->GetAt(m);
					delete pLineNode;
					pLineNode = NULL;

					pLineNode = (LineNode*)pLineArray->GetAt(n);
					delete pLineNode;
					pLineNode = NULL;

					if( m > n ) 
					{
						pLineArray->RemoveAt(m);
						pLineArray->RemoveAt(n);
					}
					else
					{
						pLineArray->RemoveAt(n);
						pLineArray->RemoveAt(m);
					}

					nLineCount -= 3;

					pLineNode = (LineNode*)pLineArray->GetAt(0);
					delete pLineNode;
					pLineNode = NULL;
					pLineArray->RemoveAt(0);
					return; // 本回路已经弄完
				}
				pLineNode = (LineNode*)pLineArray->GetAt(0);
				delete pLineNode;
				pLineNode = NULL;
				pLineArray->RemoveAt(0);
			}
			else 
			{
				int nTest = 1;
				return;
			}

			break;

		}//end of inner for-loop

		nLineCount = (int)pLineArray->GetSize(); 
		if (nLineCount == 3) 
		{
			pTriangle = new Triangle;
			pLineNode = (LineNode*)pLineArray->GetAt(0);
			pTriangle->nPt1 = pLineNode->nStartPt;
			pTriangle->nPt2 = pLineNode->nEndPt;
			pLineNode = (LineNode*)pLineArray->GetAt(1);
			pTriangle->nPt3 = pLineNode->nEndPt;
			pTriangleArray->Add(pTriangle);

			for (int k = 0; k < 3; k++) 
			{
				pLineNode = (LineNode*)pLineArray->GetAt(k);
				delete pLineNode;
				pLineNode = NULL;
			}
			pLineArray->RemoveAll();
			return;
		}

	}//end of outer while-loop
	pLineNode = (LineNode*)pLineArray->GetAt(0);
	nPt1 = pLineNode->nStartPt;
	nPt2 = pLineNode->nEndPt;
	pLineNode = (LineNode*)pLineArray->GetAt(1);
	if(pLineNode->nStartPt != nPt1 && pLineNode->nStartPt != nPt2)
	{
		nTheOne = pLineNode->nStartPt;
	}
	else if(pLineNode->nEndPt != nPt1 && pLineNode->nEndPt != nPt2)
	{
		nTheOne = pLineNode->nEndPt;
	}
	if(nPt1 != nPt2 && nPt2 != nTheOne && nPt1 != nTheOne)
	{
		pTriangle = new Triangle;
		pTriangle->nPt1 = nPt1;
		pTriangle->nPt2 = nPt2;
		pTriangle->nPt3 = nTheOne;
		pTriangleArray->Add(pTriangle);
	}
	///算法结束
}//end of Triangulate2

//--------------------------------------------------------------------------------
//功能：采用割角法对平面非自交无洞多边形Triangulation
//输入：多边形顶点数组
//输出：三角形数组（每个元素含三个顶点）
//说明: 没有洞
//--------------------------------------------------------------------------------

void SYTriangulateApp::Triangulate3WithPoly(CPtNodeArray* pptPolyArray,CTriangleArray* pTriangleArray, BOOL bAntiClockWise)
{
	Triangle *pTriangle = NULL;
	//从小到大排序
	int nPolySize = pptPolyArray->GetSize();
	int* nPtIndex = new int[nPolySize];
	for(int i = 0;i < nPolySize; nPtIndex[i] = i,i++);
	pointf pt1,pt2,pt3,pt4;
	int nptcount = nPolySize;
	int nstart = 0;
	do 
	{
		int midpt = nPtIndex[nstart];
		int ntempI = (nstart - 1 + nPolySize)%nPolySize;
		while(nPtIndex[ntempI] == -1) 
			ntempI = (ntempI - 1 + nPolySize)%nPolySize;
		int prvpt = nPtIndex[ntempI];

		ntempI = (nstart + 1)%nPolySize;
		while(nPtIndex[ntempI] == -1) 
			ntempI = (ntempI + 1)%nPolySize;

		int nexpt = nPtIndex[ntempI];//(midpt + 1)%nPolySize;

		float2Fromfloat3(&pt1,&(*pptPolyArray)[prvpt]->pt);
		float2Fromfloat3(&pt2,&(*pptPolyArray)[midpt]->pt);
		float2Fromfloat3(&pt3,&(*pptPolyArray)[nexpt]->pt);

		if ( abs(pt3.x - pt1.x) < ABS_FLOAT_0 && abs(pt3.y - pt1.y) < ABS_FLOAT_0 ) 
		{
			nPtIndex[nstart] = -1;
			nptcount --;
			while(nPtIndex[(++nstart)%nPolySize] == -1);
			nstart %= nPolySize;
			continue;
		}

		int direct = vz_pointdirect(&pt1,&pt2,&pt3);
		if((bAntiClockWise && direct > 0) || (!bAntiClockWise && direct < 0)) 
		{
			while(nPtIndex[(++nstart)%nPolySize] == -1);
			nstart %= nPolySize;
			continue;
		}
		BOOL bPtInTri = FALSE;
		for (int j = 0; j < nPolySize; j++)
		{
			if(nPtIndex[j] == -1)
				continue;
			if(nPtIndex[j] == midpt || nPtIndex[j] == prvpt ||nPtIndex[j] == nexpt)
				continue;
			float2Fromfloat3(&pt4,&(*pptPolyArray)[j]->pt);
			if(vz_ptin_triangle(&pt1,&pt2,&pt3,&pt4))
			{
				bPtInTri = TRUE;
				break;
			}
		}
		if(bPtInTri)
		{
			while(nPtIndex[(++nstart)%nPolySize] == -1);
			nstart %= nPolySize;
			continue;
		}

		pTriangle = new Triangle;
		if(bAntiClockWise)
		{
			pTriangle->nPt1 = prvpt;
			pTriangle->nPt2 = midpt;
			pTriangle->nPt3 = nexpt;
		}
		else
		{
			pTriangle->nPt1 = prvpt;
			pTriangle->nPt2 = nexpt;
			pTriangle->nPt3 = midpt;
		}	
		pTriangleArray->Add(pTriangle);

		nptcount --;
		nPtIndex[nstart] = -1;
		while(nPtIndex[(++nstart)%nPolySize] == -1);
		nstart %= nPolySize;
	} while (nptcount >= 3);
}
void SYTriangulateApp::Triangulate3(CPtNodeArray* pptPolyArray, CLineNodeArray* pLineArray, CTriangleArray* pTriangleArray, BOOL bAntiClockWise)
{
	// 顺序找到三个顺序能形成三角形的顶点, 割掉此角. 继续. 一直到只剩下3个顶点.
	///算法开始
SYTri_START:
	int nLineCount = (int)pLineArray->GetSize(); 
	if ( nLineCount < 3 ) //输入非法 
		return;

	int i = 0, j = 0;
	Triangle *pTriangle = NULL;
	LineNode *pLineNode = NULL;
	LineNode *pLineNodeNext = NULL;
	if (nLineCount < 3)
	{
		for (int k = 0; k < nLineCount; k++) 
		{
			pLineNode = (LineNode*)pLineArray->GetAt(k);
			delete pLineNode;
			pLineNode = NULL;
		}
		pLineArray->RemoveAll();
		return;
	}
	if (nLineCount == 3) // 只有三个点, 直接加入
	{
		pTriangle = new Triangle;
		pLineNode = (LineNode*)pLineArray->GetAt(0);
		pLineNodeNext = (LineNode*)pLineArray->GetAt(1);

		if(bAntiClockWise)
		{
			pTriangle->nPt1 = pLineNode->nStartPt;
			pTriangle->nPt2 = pLineNode->nEndPt;
			pTriangle->nPt3 = pLineNodeNext->nEndPt;
		}
		else
		{
			pTriangle->nPt1 = pLineNode->nStartPt;
			pTriangle->nPt2 = pLineNodeNext->nEndPt;
			pTriangle->nPt3 = pLineNode->nEndPt;
		}
		
		pTriangleArray->Add(pTriangle);

		for (int k = 0; k < 3; k++) 
		{
			pLineNode = (LineNode*)pLineArray->GetAt(k);
			delete pLineNode;
			pLineNode = NULL;
		}
		pLineArray->RemoveAll();
		return;
	}

	PtNode *pPtNode = NULL;

	pointf pt1,pt2,pt3;
	int nPt1=0, nPt2=0, nPt3=0;

	int nChoose = -1;
	float fS2Max = 0.f;
	float fS2 = 0.f;
	int nSPt1, nSPt2, nSPt3;
	LineNode *pSLineNode = NULL;
	LineNode *pSLineNodeNext = NULL;
	//float fy[3];	
	for ( i = 0; i < nLineCount; i++ ) 
	{
		// 取出三个点pt1,pt2,pt3
		pLineNode = (LineNode*)pLineArray->GetAt(i);
		nPt1 = pLineNode->nStartPt;
		pPtNode = (PtNode*)pptPolyArray->GetAt(nPt1);
		//fy[0] = pPtNode->uv.y;
		pt1.x = pPtNode->pt.x;
		pt1.y = pPtNode->pt.y;
		nPt2 = pLineNode->nEndPt;
		pPtNode = (PtNode*)pptPolyArray->GetAt(nPt2);
		pt2.x = pPtNode->pt.x;
		pt2.y = pPtNode->pt.y;
		//fy[1] = pPtNode->uv.y;

		pLineNodeNext = (LineNode*)pLineArray->GetAt( (i+1) % nLineCount );
		nPt3 = pLineNodeNext->nEndPt;
		pPtNode = (PtNode*)pptPolyArray->GetAt(nPt3);
		pt3.x = pPtNode->pt.x;
		pt3.y = pPtNode->pt.y;
		//fy[2] = pPtNode->uv.y;
		/*if(fy[0] == fy[1] && fy[1] == fy[2])
			continue;*/

		// 如果pt1,pt2,pt3能够组成合法的三角形, 割掉此角. 否则continue
		// 如果一直到i == nLineCount - 1还没有找到, 说明输入不合法, 返回.

		// 如果三角形退化
		if ( abs(pt3.x - pt1.x) < ABS_FLOAT_0 && abs(pt3.y - pt1.y) < ABS_FLOAT_0 ) 
		{
			// 去掉pLineNode(nPt1,nPt2), 去掉pLineNodeNext(nPt2,nPt3), 并将pLineNodeNextNext(nPt3,nPt4)换成(nPt1,nPt4)
			// 不添加三角形
			delete pLineNodeNext;
			pLineNodeNext = NULL;
			pLineArray->RemoveAt((i+1) % nLineCount);

			pLineNodeNext = (LineNode*)pLineArray->GetAt( (i+1) % nLineCount );
			pLineNodeNext->nStartPt = nPt1;

			delete pLineNode;
			pLineNode = NULL;
			pLineArray->RemoveAt(i);

			nLineCount -= 2;

			if (nLineCount < 3)
			{
				for (int k = 0; k < nLineCount; k++) 
				{
					pLineNode = (LineNode*)pLineArray->GetAt(k);
					delete pLineNode;
					pLineNode = NULL;
				}
				pLineArray->RemoveAll();
				return;
			}

			continue;
		}
		
		// 什么样算是合法? 第一, pt3应该位于射线pt1,pt2右侧. 第二, 没有其他顶点落入此三角形内部
		// 第一, pt3应该位于射线pt1,pt2右侧. 
		if( !PtOnLeft(pt1, pt2, pt3, bAntiClockWise) )
		{	
			continue;
		}

		if( LineIntersection(pt3, pt1, pLineArray, pptPolyArray) 
			|| LineIntersection(pt2, pt3, pLineArray, pptPolyArray) 
			)
		{
			continue;
		}

		// 第二, 没有其他顶点落入此三角形内部
		BOOL bPtInTri = FALSE;
		for ( j = 0; j < nLineCount; j++)
		{
			LineNode* pLineNodeTemp = (LineNode*)pLineArray->GetAt(j);
			PtNode* pEPtNode = (PtNode*)pptPolyArray->GetAt(pLineNodeTemp->nEndPt);
			if (pEPtNode->pt == pt1 || pEPtNode->pt == pt2 || pEPtNode->pt == pt3) 
			{
				continue;
			}
			//vz_ptin_triangle(pt1,pt2,pt3,*(pointf*)&pEPtNode->pt);
			if ( PtOnLeft2(pt1, pt2, *(pointf*)&pEPtNode->pt, bAntiClockWise)
				&& PtOnLeft2(pt2, pt3, *(pointf*)&pEPtNode->pt, bAntiClockWise)
				&& PtOnLeft2(pt3, pt1, *(pointf*)&pEPtNode->pt, bAntiClockWise)
				) 
			{
				bPtInTri = TRUE;
				break;
			}
		}
		if (bPtInTri) 
		{
			continue;
		}

		//a = sqrtf( (pt2.x - pt1.x) * (pt2.x - pt1.x) + (pt2.y - pt1.y) * (pt2.y - pt1.y) );
		//b = sqrtf( (pt3.x - pt2.x) * (pt3.x - pt2.x) + (pt3.y - pt2.y) * (pt3.y - pt2.y) );
		//c = sqrtf( (pt3.x - pt1.x) * (pt3.x - pt1.x) + (pt3.y - pt1.y) * (pt3.y - pt1.y) );
		//p = (a + b + c) * 0.5f;
		//fS2 = p * (p-a) * (p-b) * (p-c);
		//if (fS2 > fS2Max) 
		{
			fS2Max = fS2;
			nChoose = i;
			nSPt1 = nPt1;
			nSPt2 = nPt2;
			nSPt3 = nPt3;
			pSLineNode = pLineNode;
			pSLineNodeNext = pLineNodeNext;
			break;
		}
	}

	if (nChoose != -1) 
	{
		// 运行至此, 说明pt1,pt2,pt3能够组成合法的三角形, 割掉此角.
		pTriangle = new Triangle;
		if(bAntiClockWise)
		{
			pTriangle->nPt1 = nSPt1;
			pTriangle->nPt2 = nSPt2;
			pTriangle->nPt3 = nSPt3;
		}
		else
		{
			pTriangle->nPt1 = nSPt1;
			pTriangle->nPt2 = nSPt3;
			pTriangle->nPt3 = nSPt2;
		}		
		pTriangleArray->Add(pTriangle);

		// 割角: 去掉两条边, 添加一条新边
		pSLineNode->nEndPt = nSPt3;

		delete pSLineNodeNext;
		pSLineNodeNext = NULL;
		pLineArray->RemoveAt((nChoose+1) % nLineCount);

		//Triangulate3(pptPolyArray, pLineArray, pTriangleArray, bAntiClockWise);
		goto SYTri_START;
	}
	else
	{
		int nTest = 1;
	}
	
} // END OF Triangulate3

//--------------------------------------------------------------------------------
//功能：平面非自交多边形Triangulation
//输入：多边形顶点数组
//输出：三角形数组（每个元素含三个顶点）
//--------------------------------------------------------------------------------
void SYTriangulateApp::Triangulate(CPtNodeArray* pptPolyArray, CLineNodeArray* pLineArray, CTriangleArray* pTriangleArray, BOOL bAntiClockWise)
{
	///算法开始
	//若pTriangleArray 非空,则清空
	int i = 0;
	int nTriangleCount = 0;
	Triangle *pTriangle = NULL;
	nTriangleCount = (int)pTriangleArray->GetSize();
	if(nTriangleCount >0 ) 
	{
		for(i=0; i<nTriangleCount; i++)
		{
			pTriangle = (Triangle*)pTriangleArray->GetAt(i);
			delete pTriangle;
			pTriangle = NULL;
		}
		pTriangleArray->RemoveAll();
	}

	int nUndoneLineNum = (int)pLineArray->GetSize(); // nUndoneLineNum即n

	int nTestVertexNum = 0; // nTestVertexNum即k
	int nCandidateCount = 0;
	//	CandidateNode *pCandiNode = NULL;
	LineNode *pLineNode = NULL;
	PtNode *pPtNode = NULL;

	pointf pt01,pt02,ptk2;
	int k = 0;
	int nTheOne = 0;
	int nPt1=0,nPt2=0;
	float fCosMin = 1.f;
	float fCosTemp;
	if(nUndoneLineNum <3) return ; //输入非法 
	while(nUndoneLineNum != 3)
	{
		////若CandidateVertices 非空,则清空
		//nCandidateCount = CandidateVertices.GetSize();
		//if(nCandidateCount >0 ) 
		//{
		//	for(i=0; i<nCandidateCount; i++)
		//	{
		//		pCandiNode = (CandidateNode*)CandidateVertices.GetAt(i);
		//		delete pCandiNode;
		//		pCandiNode = NULL;
		//	}
		//	CandidateVertices.RemoveAll();
		//}

		k = 0;//因为k从0开始排序，所以相当于第1步中的k=1;
		nTestVertexNum = 1; 
		pLineNode = (LineNode*)pLineArray->GetAt(0);
		nPt1 = pLineNode->nStartPt;
		pPtNode = (PtNode*)pptPolyArray->GetAt(nPt1);
		pt01.x = pPtNode->pt.x;
		pt01.y = pPtNode->pt.y;
		nPt2 = pLineNode->nEndPt;
		pPtNode = (PtNode*)pptPolyArray->GetAt(nPt2);
		pt02.x = pPtNode->pt.x;
		pt02.y = pPtNode->pt.y;
		fCosMin = 2;
		while(k != nUndoneLineNum-1)//inner while-loop
		{
			if( nUndoneLineNum != (int)pLineArray->GetSize() )
			{
				//AfxMessageBox(_T("线段数目出错！"));
			}
			k++;
			pLineNode = (LineNode*)pLineArray->GetAt(k);//
			if(pLineNode->nEndPt == nPt1 || pLineNode->nEndPt == nPt2)
			{
				continue;
			}
			pPtNode = (PtNode*)pptPolyArray->GetAt(pLineNode->nEndPt);
			ptk2.x = pPtNode->pt.x;
			ptk2.y = pPtNode->pt.y;
			if( !PtOnLeft(pt01, pt02, ptk2, bAntiClockWise) )
			{
				continue;
			}

			fCosTemp = cosine(ptk2, pt01, ptk2, pt02);
			if(fCosTemp > fCosMin) 
			{
				continue;
			}
			if( LineIntersection(ptk2, pt01, pLineArray, pptPolyArray) 
				|| LineIntersection(pt02, ptk2, pLineArray, pptPolyArray) 
				)
			{
				continue;
			}
			fCosMin = fCosTemp;
			nTheOne = pLineNode->nEndPt;
			//break;

		}//end of inner while-loop

		if(nPt1 != nPt2 && nPt2 != nTheOne && nPt1 != nTheOne)
		{
			pTriangle = new Triangle;
			pTriangle->nPt1 = nPt1;
			pTriangle->nPt2 = nPt2;
			pTriangle->nPt3 = nTheOne;
			pTriangleArray->Add(pTriangle);

			int m = -1;
			int n = -1;
			int nOldLine = CountLineInArray(nPt1, nPt2, nTheOne, pLineArray, m, n);

			if(nOldLine > 2)
			{
				int nTest = 1;
			}

			if(nOldLine == 0)
			{
				pLineNode = new LineNode;
				pLineNode->nStartPt = nTheOne;
				pLineNode->nEndPt = nPt2;
				pLineArray->Add(pLineNode);

				pLineNode = new LineNode;
				pLineNode->nStartPt = nPt1;
				pLineNode->nEndPt = nTheOne;
				pLineArray->Add(pLineNode);

				nUndoneLineNum++;
			}		

			if(nOldLine == 1)
			{
				int nWhichLine = 0;
				pLineNode = (LineNode*)pLineArray->GetAt(m);
				if(pLineNode->nStartPt == nTheOne)
				{
					nWhichLine = 2;
				}
				delete pLineNode;
				pLineNode = NULL;
				pLineArray->RemoveAt(m);

				pLineNode = new LineNode;
				if(nWhichLine == 2)
				{
					pLineNode->nStartPt = nTheOne;
					pLineNode->nEndPt = nPt2;
				}
				else
				{
					pLineNode->nStartPt = nPt1;
					pLineNode->nEndPt = nTheOne;
				}			
				pLineArray->Add(pLineNode);

				nUndoneLineNum--;
			}

			if(nOldLine == 2)
			{
				pLineNode = (LineNode*)pLineArray->GetAt(m);
				delete pLineNode;
				pLineNode = NULL;

				pLineNode = (LineNode*)pLineArray->GetAt(n);
				delete pLineNode;
				pLineNode = NULL;

				if( m > n ) 
				{
					pLineArray->RemoveAt(m);
					pLineArray->RemoveAt(n);
				}
				else
				{
					pLineArray->RemoveAt(n);
					pLineArray->RemoveAt(m);
				}

				nUndoneLineNum -= 3;
			}
			pLineNode = (LineNode*)pLineArray->GetAt(0);
			delete pLineNode;
			pLineNode = NULL;
			pLineArray->RemoveAt(0);
		}
		else 
		{
			int nTest = 1;
		}
	}//end of outer while-loop
	pLineNode = (LineNode*)pLineArray->GetAt(0);
	nPt1 = pLineNode->nStartPt;
	nPt2 = pLineNode->nEndPt;
	pLineNode = (LineNode*)pLineArray->GetAt(1);
	if(pLineNode->nStartPt != nPt1 && pLineNode->nStartPt != nPt2)
	{
		nTheOne = pLineNode->nStartPt;
	}
	else if(pLineNode->nEndPt != nPt1 && pLineNode->nEndPt != nPt2)
	{
		nTheOne = pLineNode->nEndPt;
	}
	if(nPt1 != nPt2 && nPt2 != nTheOne && nPt1 != nTheOne)
	{
		pTriangle = new Triangle;
		pTriangle->nPt1 = nPt1;
		pTriangle->nPt2 = nPt2;
		pTriangle->nPt3 = nTheOne;
		pTriangleArray->Add(pTriangle);
	}
	///算法结束
}//end of Triangulate
//////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------
//功能:
//     返回两个矢量a和b的夹角的余弦(-1 --- 1),
//     注意：如果想从余弦求夹角的话，注意反
//     余弦函数的定义域是从 0到pi 
//     两个向量a,b 的点积: ab = |a||b|cosA,其中|a|为向量a的模,A为两向量夹角
//输入:向量a起讫点: s1,e1
//     向量b起讫点: s2,e2
//输出:返回值为两者夹角余弦值. 如果a或者b退化为一点,则返回 -1 (180度对应的余弦值)
//--------------------------------------------------------------------------------
float SYTriangulateApp::cosine(pointf s1, pointf e1, pointf s2, pointf e2) 
{
	float dist1 = ( (e1.x - s1.x)*(e1.x - s1.x) + (e1.y - s1.y)*(e1.y - s1.y) );
	if( dist1 < 0.00001 ) 
	{
		//AfxMessageBox("cosine(): dist1==0");
		return -1;
	}
	float dist2 = ( (e2.x - s2.x)*(e2.x - s2.x) + (e2.y - s2.y)*(e2.y - s2.y) );
	if( dist2 < 0.00001 ) 
	{
		//AfxMessageBox("cosine(): dist2==0");
		return -1;
	}
	return ( ( (e1.x-s1.x)*(e2.x-s2.x) + (e1.y-s1.y)*(e2.y-s2.y) ) 
		/ sqrt(dist1*dist2)  
		); 
}//end of cosine


//--------------------------------------------------------------------------------
//功能:
//     测试pt2 是否在pt0pt1 右侧
//     fPositive = (pt1.x - pt0.x)*(pt2.y - pt0.y) - (pt1.y - pt0.y)*(pt2.x - pt0.x);
//输入:向量起讫点: pt0,pt1
//     被测点: pt2
//输出:返回值为TRUE: 在左侧。否则FALSE
//--------------------------------------------------------------------------------
BOOL SYTriangulateApp::PtOnLeft(IN pointf pt0, IN pointf pt1, IN pointf pt2, IN BOOL bAntiClockWise) 
{
	if(bAntiClockWise) 
	{
		if( ((pt1.x - pt0.x)*(pt2.y - pt0.y) - (pt1.y - pt0.y)*(pt2.x - pt0.x)) < 0 )//左侧
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		if( ((pt1.x - pt0.x)*(pt2.y - pt0.y) - (pt1.y - pt0.y)*(pt2.x - pt0.x)) > 0 )//左侧
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
}//end of PtOnLeft

//--------------------------------------------------------------------------------
//功能:
//     测试pt2 是否在pt0pt1 右侧
//     fPositive = (pt1.x - pt0.x)*(pt2.y - pt0.y) - (pt1.y - pt0.y)*(pt2.x - pt0.x);
//输入:向量起讫点: pt0,pt1
//     被测点: pt2
//输出:返回值为TRUE: 在左侧。否则FALSE
//--------------------------------------------------------------------------------
BOOL SYTriangulateApp::PtOnLeft2(IN pointf pt0, IN pointf pt1, IN pointf pt2, IN BOOL bAntiClockWise) 
{
	if(bAntiClockWise) 
	{
		if( ((pt1.x - pt0.x)*(pt2.y - pt0.y) - (pt1.y - pt0.y)*(pt2.x - pt0.x)) <= 0 )//左侧
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		if( ((pt1.x - pt0.x)*(pt2.y - pt0.y) - (pt1.y - pt0.y)*(pt2.x - pt0.x)) >= 0 )//左侧
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
}//end of PtOnLeft2

//--------------------------------------------------------------------------------
//功能:
//     多边形顶点数组
//输入:ptPolyArray : 多边形顶点数组
//输出:返回值为两者夹角余弦值. 如果a或者b退化为一点,则返回 -1 (180度对应的余弦值)
//--------------------------------------------------------------------------------
BOOL SYTriangulateApp::InitLines( IN CPtNodeArray* pptPolyArray, OUT CLineNodeArray* pLineArray ) 
{
	//参数检查
	int nPtCount = (int)pptPolyArray->GetSize();
	if(nPtCount<3) 
	{
		return FALSE;
	}

	int i = 0;
	int nLineCount = 0;
	LineNode *pLineNode = NULL;
	PtNode *pPtNode = NULL;
	//若lineArray 非空,则清空
	nLineCount = (int)pLineArray->GetSize();
	for(i=0; i<nLineCount; i++)
	{
		pLineNode = (LineNode*)pLineArray->GetAt(i);
		delete pLineNode;
		pLineNode = NULL;
	}
	pLineArray->RemoveAll();

	int nLastStartNode = 0;
	for(i=0; i<nPtCount; i++)
	{
		pLineNode = new LineNode;

		pPtNode = (PtNode*)pptPolyArray->GetAt((i+1)%nPtCount);
		
		if( pPtNode->bStart ==FALSE  && i<nPtCount-1 ) 
		{
			pLineNode->nStartPt = i;
			pLineNode->nEndPt = i+1;
			pLineArray->Add(pLineNode);
		}
		else
		{
			if(i != nLastStartNode)
			{
				pLineNode->nStartPt = i;
				pLineNode->nEndPt = nLastStartNode;
				pLineArray->Add(pLineNode);
			}

			nLastStartNode = i+1;
		}


	}//end of for
	return TRUE;
}//end of InitLines

//------------------------------------------------------------------
//功能：判断两线段是否相交
//输入：两线段的起讫点
//输出：
//返回值：若相交，则返回TRUE;否则，返回FALSE
//------------------------------------------------------------------
BOOL SYTriangulateApp::SingleLineIntersection(IN pointf startPt1, IN pointf endPt1, IN pointf startPt2, IN pointf endPt2)
{
	//if (startPt1 == startPt2 || startPt1 == endPt2 || endPt1 == startPt2 || endPt1 == endPt2) 
	//if(( (startPt1.x - startPt2.x)*(startPt1.x - startPt2.x) + (startPt1.y - startPt2.y)*(startPt1.y - startPt2.y) )< 0.00001
	//   || ( (startPt1.x - endPt2.x)*(startPt1.x - endPt2.x) + (startPt1.y - endPt2.y)*(startPt1.y - endPt2.y) )< 0.00001
	//   || ( (endPt1.x - startPt2.x)*(endPt1.x - startPt2.x) + (endPt1.y - startPt2.y)*(endPt1.y - startPt2.y) )< 0.00001
	//   || ( (endPt1.x - endPt2.x)*(endPt1.x - endPt2.x) + (endPt1.y - endPt2.y)*(endPt1.y - endPt2.y) )< 0.00001
	//  ) 
	if(    ( abs(startPt1.x - startPt2.x) < 0.0001 && abs(startPt1.y - startPt2.y) < 0.0001 )
		|| ( abs(startPt1.x - endPt2.x)   < 0.0001 && abs(startPt1.y - endPt2.y)   < 0.0001 )
		|| ( abs(endPt1.x - startPt2.x)   < 0.0001 && abs(endPt1.y - startPt2.y)   < 0.0001 )
		|| ( abs(endPt1.x - endPt2.x)     < 0.0001 && abs(endPt1.y - endPt2.y)     < 0.0001 )
		) 
	{
		return FALSE;
	}
	pointf v1,v2;//分别是两个线段对应向量
	v1.x = endPt1.x - startPt1.x;
	v1.y = endPt1.y - startPt1.y;
	v2.x = endPt2.x - startPt2.x;
	v2.y = endPt2.y - startPt2.y;

	float fDenominator = v1.x * v2.y - v1.y * v2.x;
	if ( abs(fDenominator)< 0.000001f ) 
	{
		return FALSE;
	}

	float fScale = v1.y *(startPt2.x - startPt1.x) - v1.x *(startPt2.y - startPt1.y);
	if(fDenominator>0) 
	{
		if (fScale > 0 && fScale< fDenominator/* - 0.00001*/) //交点在线段2上
		{
			fScale = v2.y *(startPt2.x - startPt1.x) - v2.x *(startPt2.y - startPt1.y);
			if(fScale > 0 && fScale< fDenominator/* - 0.00001*/) //交点在线段１上
			{
				return TRUE;
			}
		}
	}
	else
	{
		if (fScale < 0 && fScale> fDenominator/* - 0.00001*/) //交点在线段2上
		{
			fScale = v2.y *(startPt2.x - startPt1.x) - v2.x *(startPt2.y - startPt1.y);
			if(fScale < 0 && fScale> fDenominator/* - 0.00001*/) //交点在线段１上
			{
				return TRUE;
			}
		}
	}

	return FALSE;

}//End of SingleLineIntersection

//------------------------------------------------------------------
//功能：判断线段是否与线段组相交
//输入：
//输出：
//返回值：若相交，则返回TRUE;否则，返回FALSE
//------------------------------------------------------------------
BOOL SYTriangulateApp::LineIntersection(IN pointf startPt1, IN pointf endPt1, IN CLineNodeArray* pLineArray,IN CPtNodeArray* pptPolyArray )
{
	int nLineCount = (int)pLineArray->GetSize();
	int i = 0;
	LineNode *pLineNode = NULL;
	PtNode *pSPtNode = NULL;
	PtNode *pEPtNode = NULL;
	pointf ptS,ptE;
	for(i=0; i<nLineCount; i++)
	{
		pLineNode = (LineNode*)pLineArray->GetAt(i);
		pSPtNode = (PtNode*)pptPolyArray->GetAt(pLineNode->nStartPt);
		pEPtNode = (PtNode*)pptPolyArray->GetAt(pLineNode->nEndPt);
		ptS.x = pSPtNode->pt.x;
		ptS.y = pSPtNode->pt.y;
		ptE.x = pEPtNode->pt.x;
		ptE.y = pEPtNode->pt.y;
		if(SingleLineIntersection(startPt1, endPt1, ptS, ptE))
		{
			return TRUE;
		}
	}
	return FALSE;

}//End of LineIntersection

//////////////////////////////////////////////////////////
// 求点p到线段l所在直线的距离 
//////////////////////////////////////////////////////////
double SYTriangulateApp::PToLDist(pointf* p, pointf* pLS, pointf* pLE) 
{ 
	double l = sqrt(double( (pLE->x - pLS->x)*(pLE->x - pLS->x) + (pLE->y - pLS->y)*(pLE->y - pLS->y) ));
	if (l<0.0000001) 
	{
		return sqrt(double( (p->x - pLS->x)*(p->x - pLS->x) + (p->y - pLS->y)*(p->y - pLS->y) ));
	}
	double dMultiply = (p->x - pLS->x)*(pLE->y - pLS->y) - (pLE->x - pLS->x)*(p->y - pLS->y);
	return abs(dMultiply) / l;
} // End of PToLDist

//------------------------------------------------------------------
//功能：判断点pt是否位于线段上
//------------------------------------------------------------------
BOOL SYTriangulateApp::PtOnLineSeg(IN pointf pt, IN pointf line1, IN pointf line2 )
{
	BOOL bRtn = PToLDist(&pt, &line1, &line2) < ABS_FLOAT_0 
		&& pt.x >= min(line1.x, line2.x) && pt.y >= min(line1.y, line2.y) 
		&& pt.x <= max(line1.x, line2.x) && pt.y <= max(line1.y, line2.y)
		; // pt位于线段line1, line2上, 则认为相交
	return bRtn;
} // END OF PtOnLineSeg

//------------------------------------------------------------------
//功能：判断两线段是否相交
//输入：两线段的起讫点
//输出：
//返回值：若相交，则返回TRUE;否则，返回FALSE
//------------------------------------------------------------------
BOOL SYTriangulateApp::SingleLineIntersection2(IN pointf startPt1, IN pointf endPt1, IN pointf startPt2, IN pointf endPt2)
{
	// startPt1, 是内环上的点. 肯定不能与外环上的点重合
	// endPt1, 是外环上的备选点
	// 如果外环上的点, 不与endPt1重合, 那么该点如果落在线段startPt1,endPt1上, 则认为相交, 备选点endPt1不合格.
	if ( startPt2.x != endPt1.x || startPt2.y != endPt1.y ) 
	{
		if ( PtOnLineSeg(startPt2, startPt1, endPt1) ) 
		{
			return TRUE; // 认为相交 , 不合格
		}
	}

	if ( endPt2.x != endPt1.x || endPt2.y != endPt1.y ) 
	{
		if ( PtOnLineSeg(endPt2, startPt1, endPt1) ) 
		{
			return TRUE; // 认为相交 , 不合格
		}
	}

	if ( startPt2 == endPt1 || endPt2 == endPt1 ) 
	{
		return FALSE;
	}
	pointf v1,v2;//分别是两个线段对应向量
	v1.x = endPt1.x - startPt1.x;
	v1.y = endPt1.y - startPt1.y;
	v2.x = endPt2.x - startPt2.x;
	v2.y = endPt2.y - startPt2.y;

	float fDenominator = v1.x * v2.y - v1.y * v2.x;
	if ( abs(fDenominator)< 0.000001f ) 
	{
		return FALSE;
	}

	float fScale = v1.y *(startPt2.x - startPt1.x) - v1.x *(startPt2.y - startPt1.y);
	if(fDenominator>0) 
	{
		if (fScale >= 0 && fScale<= fDenominator/* - 0.00001*/) //交点在线段2上
		{
			fScale = v2.y *(startPt2.x - startPt1.x) - v2.x *(startPt2.y - startPt1.y);
			if(fScale >= 0 && fScale< fDenominator - ABS_FLOAT_0) //交点在线段１上
			{
				return TRUE;
			}
		}
	}
	else
	{
		if (fScale <= 0 && fScale>= fDenominator/* - 0.00001*/) //交点在线段2上
		{
			fScale = v2.y *(startPt2.x - startPt1.x) - v2.x *(startPt2.y - startPt1.y);
			if(fScale <= 0 && fScale> fDenominator + ABS_FLOAT_0) //交点在线段１上
			{
				return TRUE;
			}
		}
	}

	return FALSE;

}//End of SingleLineIntersection2

//------------------------------------------------------------------
//功能：判断线段是否与线段组相交
//输入：
//输出：
//返回值：若相交，则返回TRUE;否则，返回FALSE
//------------------------------------------------------------------
BOOL SYTriangulateApp::LineIntersection2(IN pointf startPt1, IN pointf endPt1, IN CPtNodeArray &ptPolyArray)
{
	int i = 0;
	PtNode* pPt0 = NULL; 
	PtNode* pPt1 = NULL; 
	pointf ptS,ptE;
	int nSize = (int)ptPolyArray.GetSize();
	for(i=0; i<nSize; i++)
	{
		pPt0 = (PtNode*)ptPolyArray.GetAt(i);
		pPt1 = (PtNode*)ptPolyArray.GetAt((i+1)%nSize);

		ptS.x = pPt0->pt.x;
		ptS.y = pPt0->pt.y;
		ptE.x = pPt1->pt.x;
		ptE.y = pPt1->pt.y;
		if(SingleLineIntersection2(startPt1, endPt1, ptS, ptE))
		{
			return TRUE;
		}
	}
	return FALSE;

}//End of LineIntersection2

//------------------------------------------------------------------
//功能：判断线段nTheOnenPt1,nPt2nTheOne有几条线段是数组lineArray中已有的
//输入：
//输出：
//返回值：有几条新的线段(弦)
//同时用m,n返回已有线段的序号
//------------------------------------------------------------------
int SYTriangulateApp::CountLineInArray(IN int nPt1, IN int nPt2, IN int nTheOne, CLineNodeArray* pLineArray, OUT int& m, OUT int& n)
{
	int nRet = 0;//返回值
	int i = 0;
	int nLineCount = 0;
	LineNode *pLineNode = NULL;
	nLineCount = (int)pLineArray->GetSize();
	int M = -1;
	int N = -1;
	for(i=0; i<nLineCount; i++)
	{
		pLineNode = (LineNode*)pLineArray->GetAt(i);
		if(pLineNode->nStartPt == nTheOne && pLineNode->nEndPt == nPt1)
		{
			M = i;
		}
		if(pLineNode->nStartPt == nPt2 && pLineNode->nEndPt == nTheOne)
		{
			N = i;
		}
	}

	if (M != -1 && N != -1) 
	{
		m = M;
		n = N;
		nRet = 2;
	}
	else if (M != -1 && N == -1)
	{
		m = M;
		nRet = 1;
	}
	else if (M == -1 && N != -1)
	{
		m = N;
		nRet = 1;
	}
	//else if (M == -1 && N == -1)
	//{
	//	nRet = 0;
	//}
	return nRet;
}//end of SearchNewLine

//--------------------------------------------------------------------------------------
// 给数组bOutLoop赋值
//--------------------------------------------------------------------------------------
void SYTriangulateApp::InitOutLoopArr(OUT BOOL *bOutLoop, IN const vectorgraph* pGraph, IN const BOOL bAntiClockwise)
{
	for(int i=0; i<pGraph->ncell; i++)
	{
		if ( IsAntiClockwise(&pGraph->cell[i]) != bAntiClockwise ) 
			bOutLoop[i] = TRUE;
		else
			bOutLoop[i] = FALSE;
	} // end of for
} // END OF InitOutLoopArr

//------------------------------------------------------------------
//功能：得到一个点列最小y值点对应的序号
//输入：
//输出：
//返回值：
//------------------------------------------------------------------
int SYTriangulateApp::GetMinYPt(IN vectorcell* pCurveCell )//得到最小y值点 //多边形顶点数组　顶点数据类型：ShiftNode*
{
	int nPtCount = pCurveCell->npoint;
	if(nPtCount<=0) 
	{
		return -1;
	}

	int nRetNum = 0; //返回值
	float fMinY = pCurveCell->point[0].y;

	for(int i=1; i<nPtCount; i++)
	{
		if(pCurveCell->point[i].y < fMinY)
		{
			fMinY = pCurveCell->point[i].y;
			nRetNum = i;
		}
        else if(pCurveCell->point[i].y == fMinY)
		{
			if (pCurveCell->point[i].x <= pCurveCell->point[nRetNum].x) 
			{
				nRetNum = i;
			}
		}
	}
	return nRetNum;
}//end of GetMinYPt

//------------------------------------------------------------------
//功能：判断左侧是否为外侧
//输入：
//输出：
//返回值：
//------------------------------------------------------------------
BOOL SYTriangulateApp::OutIsLeft(IN pointf pt0,IN pointf pt1,IN pointf pt2)//判断左侧是否外侧
{
	if ( (pt1.x - pt0.x)*(pt2.y - pt0.y) - (pt1.y - pt0.y)*(pt2.x - pt0.x) > 0 )  
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}//end of OutIsLeft

BOOL SYTriangulateApp::IsAntiClockwise(IN vectorcell* pCurveCell )
{
	if(pCurveCell->npoint <= 0)
		return FALSE;
	/////////////////////////////////////////////////
	int nNum = GetMinYPt(pCurveCell);
	

	BOOL bAntiClockwise ;
	pointf pt1,pt2,pt3;
	pointf *pPt = NULL;
	int nPtCount = pCurveCell->npoint;
	if(nNum == 0)
	{
		pt1 = pCurveCell->point[nPtCount-1]; 
		pt2 = pCurveCell->point[0]; 
		pt3 = pCurveCell->point[1]; 
		if ( abs(pt1.x - pt2.x) < ABS_FLOAT_0 && abs(pt1.y - pt2.y) < ABS_FLOAT_0 ) 
		{
			for(int i = nPtCount -2; i > 1; i--)
			{
				if ( abs(pCurveCell->point[i].x - pt2.x) > ABS_FLOAT_0 || abs(pCurveCell->point[i].y - pt2.y) > ABS_FLOAT_0 ) 
				{
					pt1 = pCurveCell->point[i];
					break;
				}
			} // end of for
		} // end of if
		if ( abs(pt3.x - pt2.x) < ABS_FLOAT_0 && abs(pt3.y - pt2.y) < ABS_FLOAT_0 ) 
		{
			for(int i = 2; i < nPtCount-1; i++)
			{
				if ( abs(pCurveCell->point[i].x - pt2.x) > ABS_FLOAT_0 && abs(pCurveCell->point[i].y - pt2.y) > ABS_FLOAT_0 ) 
				{
					pt3 = pCurveCell->point[i];
					break;
				}
			} // end of for
		} // end of if
	} // end of if(nNum == 0)
	else if(nNum == nPtCount - 1)
	{
		pt1 = pCurveCell->point[nNum-1]; 
		pt2 = pCurveCell->point[nNum]; 
		pt3 = pCurveCell->point[0];
		if ( abs(pt1.x - pt2.x) < ABS_FLOAT_0 && abs(pt1.y - pt2.y) < ABS_FLOAT_0 ) 
		{
			for(int i = nPtCount -3; i > 0; i--)
			{
				if ( abs(pCurveCell->point[i].x - pt2.x) > ABS_FLOAT_0 || abs(pCurveCell->point[i].y - pt2.y) > ABS_FLOAT_0 ) 
				{
					pt1 = pCurveCell->point[i];
					break;
				}
			} // end of for
		} // end of if
		if ( abs(pt3.x - pt2.x) < ABS_FLOAT_0 && abs(pt3.y - pt2.y) < ABS_FLOAT_0 ) 
		{
			for(int i = 1; i < nPtCount-2; i++)
			{
				if ( abs(pCurveCell->point[i].x - pt2.x) > ABS_FLOAT_0 && abs(pCurveCell->point[i].y - pt2.y) > ABS_FLOAT_0 ) 
				{
					pt3 = pCurveCell->point[i];
					break;
				}
			} // end of for
		} // end of if
	}
	else if(nNum > 0)
	{
		pt1 = pCurveCell->point[nNum-1];
		pt2 = pCurveCell->point[nNum];
		pt3 = pCurveCell->point[nNum+1];
		if ( abs(pt1.x - pt2.x) < ABS_FLOAT_0 && abs(pt1.y - pt2.y) < ABS_FLOAT_0 ) 
		{
			for(int i = nNum-2 + nPtCount; i > nNum+1; i--)
			{
				int j = i%nPtCount;
				if ( abs(pCurveCell->point[j].x - pt2.x) > ABS_FLOAT_0 || abs(pCurveCell->point[j].y - pt2.y) > ABS_FLOAT_0 ) 
				{
					pt1 = pCurveCell->point[j];
					break;
				}
			} // end of for
		} // end of if
		if ( abs(pt3.x - pt2.x) < ABS_FLOAT_0 && abs(pt3.y - pt2.y) < ABS_FLOAT_0 ) 
		{
			for(int i = nNum+2; i < nNum-1 + nPtCount; i++)
			{
				int j = i%nPtCount;
				if ( abs(pCurveCell->point[j].x - pt2.x) > ABS_FLOAT_0 && abs(pCurveCell->point[j].y - pt2.y) > ABS_FLOAT_0 ) 
				{
					pt3 = pCurveCell->point[j];
					break;
				}
			} // end of for
		} // end of if
	}

	bAntiClockwise = OutIsLeft(pt1,pt2,pt3);
	return bAntiClockwise;
} // END OF IsAntiClockwise

