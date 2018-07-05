#include "stdafx.h"
#include "DataRender\OffsetCurve.h"
#include <math.h>
#include "tchar.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
static BOOL g_nNearLineClosed = FALSE;

typedef struct tag_OffsetNode
{
	point2f pt; // 顶点数据
	BOOL    bStart;       //环路起始点: TRUE
}OffsetNode;

typedef CSYArray<OffsetNode *> COffsetNodeArray;

//------------------------------------------------------------------
//功能：得到一个点列最小y值点对应的序号
//输入：
//输出：
//返回值：
//------------------------------------------------------------------
int _SYOC_LineCross(point2f* ptstart,point2f* ptend,point2f * pptShift ,int nShiftCount,point2f* ptout,int nstart);
int SYOC_GetMinYPt(IN vectorcell* pCurveCell )//得到最小y值点 //多边形顶点数组　顶点数据类型：ShiftNode*
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
		if(pCurveCell->point[i].y == fMinY)
		{
			if (pCurveCell->point[i].x <= pCurveCell->point[nRetNum].x) 
			{
				nRetNum = i;
			}
		}
	}
	return nRetNum;
}//end of SYOC_GetMinYPt

//------------------------------------------------------------------
//功能：判断左侧是否为外侧
//输入：
//输出：
//返回值：若pt2在向量pt0,pt1右侧, 则返回true
//注意! 默认前提是 ,三点不共线
//------------------------------------------------------------------
BASEGRAPH_DATARENDER_API BOOL SYOC_PtOnRight(IN point2f pt0,IN point2f pt1,IN point2f pt2)//判断左侧是否外侧
{
	if(fabs(pt2.y - pt0.y) < ABS_FLOAT_0 && fabs(pt1.y - pt0.y) < ABS_FLOAT_0 )
	{
		return FALSE;
	}
	if ( (pt1.x - pt0.x)*(pt2.y - pt0.y) - (pt1.y - pt0.y)*(pt2.x - pt0.x) > 0 )  
	{
		return TRUE;//左侧
	}
	else
	{
		return FALSE;//右侧
	}
}//end of SYOC_PtOnRight

BOOL SYOC_IsAntiClockwise(IN vectorcell* pCurveCell )
{
	/////////////////////////////////////////////////
	//得到最小ｙ值点。
	int nNum = SYOC_GetMinYPt(pCurveCell);

	//判断哪侧是外侧
	BOOL bAntiClockwise;
	point2f pt1,pt2,pt3;
	point2f *pPt = NULL;
	int nPtCount = pCurveCell->npoint;
	if(nNum == 0)
	{
		pt1 = pCurveCell->point[nPtCount-1]; 
		pt2 = pCurveCell->point[0]; 
		pt3 = pCurveCell->point[1]; 
	}
	else if(nNum == nPtCount - 1)
	{
		pt1 = pCurveCell->point[nNum-1]; 
		pt2 = pCurveCell->point[nNum]; 
		pt3 = pCurveCell->point[0];
	}
	else if(nNum > 0)
	{
		pt1 = pCurveCell->point[nNum-1];
		pt2 = pCurveCell->point[nNum];
		pt3 = pCurveCell->point[nNum+1];
	}

	bAntiClockwise = SYOC_PtOnRight(pt1,pt2,pt3);
	return bAntiClockwise;
} // END OF SYOC_IsAntiClockwise

//------------------------------------------------------------------
//函数 SYOC_NearLine 说明
//功能：三次Bezier曲线控制顶点接近一条线段
//输入：
//输出：
//------------------------------------------------------------------
BOOL SYOC_NearLine(IN point2f* ptf ) 
{
	if (g_nNearLineClosed)
	{
		//直接关闭
		return TRUE;
		//后边可以考虑按照一定的比例进行精细化，越精细速度越慢，暂时选择关闭
		int f30X  = (ptf[3].x - ptf[0].x);
		int f10X  = (ptf[1].x - ptf[0].x);
		int f20X  = (ptf[2].x - ptf[0].x);
		if( abs(f30X) < 10 ) //if(f30X == 0.0f)
		{
			if( abs(f10X) < 50 && abs(f20X) <= 50)
				return TRUE;
		}
		int t1 = f10X/f30X;
		int fdy = (ptf[3].y - ptf[0].y);
		int fy1 = ptf[0].y + t1*fdy;
		if(abs(ptf[1].y - fy1)<10)//Y相差
		{
			int t2 = f20X/f30X;
			int fy2 = ptf[0].y + t2*fdy;
			if(abs(ptf[2].y -fy2) < 10)//Y相差
			{
				return TRUE;
				//接近直线
			}
		}
		return FALSE;
	}
	// new code
	float f30X  = (ptf[3].x - ptf[0].x);
	float f10X  = (ptf[1].x - ptf[0].x);
	float f20X  = (ptf[2].x - ptf[0].x);
	if( fabs(f30X) < 0.01f ) //if(f30X == 0.0f)
	{
		if( fabs(f10X) < 0.5f && fabs(f20X) <= 0.5f)
			return TRUE;
	}
	float t1 = f10X/f30X;
	float fdy = (ptf[3].y - ptf[0].y);
	float fy1 = ptf[0].y + t1*fdy;
	if(fabs(ptf[1].y - fy1)<0.1f)//Y相差
	{
		float t2 = f20X/f30X;
		float fy2 = ptf[0].y + t2*fdy;
		if(fabs(ptf[2].y -fy2) < 0.1f)//Y相差
		{
			return TRUE;
			//接近直线
		}
	}
	return FALSE;
	//需要分割	
} // end of SYOC_NearLine


void AddCurve2PtfPoly(OUT SYArray * pptrPoly, OUT SYArray * pptrPolyNorm, IN pointf * ctrlPoints,float* falllength)
{
	// 接近直线，则直接加入
	// 否则分成两段，递归调用
	float flength = 0;
	if(falllength)
		flength = *falllength;
	point3f *ptfNew;
	if ( SYOC_NearLine(ctrlPoints) )
	{
		pointf ptNormal;
		for(int i = 2;i >= 0;i --)
		{
			ptNormal.x = ctrlPoints[i].y - ctrlPoints[3].y;
			ptNormal.y = ctrlPoints[3].x - ctrlPoints[i].x;

			if (abs(ptNormal.x) > ABS_FLOAT_0 || abs(ptNormal.y) > ABS_FLOAT_0)
			{
				ptfNew = new point3f;
				if(falllength)
				{
					*falllength += float2Mode(&(ctrlPoints[3] - ctrlPoints[0]));
					flength = *falllength;
				}
				float3fFromfloat2fEx(ptfNew,&ctrlPoints[3],flength,Plane_XY);
				//*ptfNew = ctrlPoints[3];
				
				SYAR_Add(pptrPoly,ptfNew);
				//pptrPoly->Add(ptfNew);

				if(pptrPolyNorm)
				{
					point2f* ptfNormal1 = new pointf;
					*ptfNormal1 = ptNormal;
					SYAR_Add(pptrPolyNorm,ptfNormal1);
					point2f* ptfNormal2 = new pointf;
					*ptfNormal2 = ptNormal;
					SYAR_Add(pptrPolyNorm,ptfNormal2);
				}
				
				break;
			}
		}
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

		AddCurve2PtfPoly(pptrPoly, pptrPolyNorm, ptfFormer, falllength) ;
		AddCurve2PtfPoly(pptrPoly, pptrPolyNorm, ptfLatter, falllength) ;
	}
} // END OF AddCurve2PtfPoly



void AddCurve2PtlfPoly(OUT SYArray * pptrPoly, OUT SYArray * pptrPolyNorm, IN pointf * ctrlPoints,float* falllength)
{
	// 接近直线，则直接加入
	// 否则分成两段，递归调用
	float flength = 0;
	if(falllength)
		flength = *falllength;
	point3lf *ptfNew;
	if ( SYOC_NearLine(ctrlPoints) )
	{
		pointf ptNormal;
		for(int i = 2;i >= 0;i --)
		{
			ptNormal.x = ctrlPoints[i].y - ctrlPoints[3].y;
			ptNormal.y = ctrlPoints[3].x - ctrlPoints[i].x;

			if (abs(ptNormal.x) > ABS_FLOAT_0 || abs(ptNormal.y) > ABS_FLOAT_0)
			{
				ptfNew = new point3lf;
				if(falllength)
				{
					*falllength += float2Mode(&(ctrlPoints[3] - ctrlPoints[0]));
					flength = *falllength;
				}
				v2tov3(ptfNew,&ctrlPoints[3],flength,Plane_XY);
				//*ptfNew = ctrlPoints[3];
				
				SYAR_Add(pptrPoly,ptfNew);
				//pptrPoly->Add(ptfNew);

				if(pptrPolyNorm)
				{
					point2f* ptfNormal1 = new pointf;
					*ptfNormal1 = ptNormal;
					SYAR_Add(pptrPolyNorm,ptfNormal1);
					point2f* ptfNormal2 = new pointf;
					*ptfNormal2 = ptNormal;
					SYAR_Add(pptrPolyNorm,ptfNormal2);
				}
				
				break;
			}
		}
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

		AddCurve2PtlfPoly(pptrPoly, pptrPolyNorm, ptfFormer, falllength) ;
		AddCurve2PtlfPoly(pptrPoly, pptrPolyNorm, ptfLatter, falllength) ;
	}
} // END OF AddCurve2PtfPoly
//------------------------------------------------------------------
//函数 SYOC_AddCurve2PtfPoly 说明
//功能：将三次Bezier曲线段转化并加入多边形数组
//输入：
//输出：
//------------------------------------------------------------------
void SYOC_AddCurve2PtfPoly(OUT CSYPoint2fPtrArray * pptrPoly, IN point2f * ctrlPoints)
{
	// 接近直线，则直接加入
	// 否则分成两段，递归调用
	point2f *ptfNew;
	if ( SYOC_NearLine(ctrlPoints) )
	{
		ptfNew = new point2f;
		ptfNew->x = ctrlPoints[3].x;
		ptfNew->y = ctrlPoints[3].y;
		pptrPoly->Add(ptfNew);
	}
	else
	{
		point2f ptfFormer[4];
		point2f ptfLatter[4];

		ptfFormer[0].x = ctrlPoints[0].x;
		ptfFormer[0].y = ctrlPoints[0].y;

		ptfLatter[3].x = ctrlPoints[3].x;
		ptfLatter[3].y = ctrlPoints[3].y;

		ptfFormer[1].x = (ctrlPoints[0].x + ctrlPoints[1].x) * 0.5f;
		ptfFormer[1].y = (ctrlPoints[0].y + ctrlPoints[1].y) * 0.5f;

		ptfLatter[2].x = (ctrlPoints[2].x + ctrlPoints[3].x) * 0.5f;
		ptfLatter[2].y = (ctrlPoints[2].y + ctrlPoints[3].y) * 0.5f;

		ptfFormer[3].x = (ctrlPoints[1].x + ctrlPoints[2].x) * 0.5f;
		ptfFormer[3].y = (ctrlPoints[1].y + ctrlPoints[2].y) * 0.5f;

		ptfFormer[2].x = (ptfFormer[1].x + ptfFormer[3].x) * 0.5f;
		ptfFormer[2].y = (ptfFormer[1].y + ptfFormer[3].y) * 0.5f;

		ptfLatter[1].x = (ptfLatter[2].x + ptfFormer[3].x) * 0.5f;
		ptfLatter[1].y = (ptfLatter[2].y + ptfFormer[3].y) * 0.5f;

		ptfFormer[3].x = (ptfLatter[1].x + ptfFormer[2].x) * 0.5f;
		ptfFormer[3].y = (ptfLatter[1].y + ptfFormer[2].y) * 0.5f;

		ptfLatter[0].x = ptfFormer[3].x;
		ptfLatter[0].y = ptfFormer[3].y;

		SYOC_AddCurve2PtfPoly(pptrPoly, ptfFormer ) ;
		SYOC_AddCurve2PtfPoly(pptrPoly, ptfLatter ) ;
	}
} // END OF SYOC_AddCurve2PtfPoly


//------------------------------------------------------------------
//函数 TPGT_Cell2PtfPoly 说明
//功能：将pCurveCell转化为多边形数组
//输入：
//输出：point3f,x,y为转换后x，y值，z为到开头长度。
//------------------------------------------------------------------
void SYOC_Cell2PtfPoly(IN vectorcell * pCurveCell, OUT SYArray * pptrPoly,OUT SYArray * pptrPolyNorm,int* pMidPtIndex)
{
	int i, j;
	point3f * pptf = NULL;
	pointf ctrlPoints[4];
	pointf ptNormal;
	int nSize = pCurveCell->npoint;
	if (nSize <= 0)
	{
		return;
	}
	int nPolyBack = -1;
	pptf = new point3f;
	float3fFromfloat2fEx(pptf,&pCurveCell->point[0],0,Plane_XY);
	//*pptf = pCurveCell->point[0];
	SYAR_Add(pptrPoly,pptf);
	float falllength = 0;


	for(i = 0; i<pCurveCell->npoint; i++)
	{
		if(pMidPtIndex && *pMidPtIndex == i)
		{
			nPolyBack = SYAR_GetSize(pptrPoly);
		}
		if (pCurveCell->pttype[i] == point_line)
		{
			ptNormal.x = pCurveCell->point[i].y - pCurveCell->point[(i + 1)%nSize].y;
			ptNormal.y = pCurveCell->point[(i + 1)%nSize].x - pCurveCell->point[i].x;
			if (abs(ptNormal.x) > ABS_FLOAT_0 || abs(ptNormal.y) > ABS_FLOAT_0)
			{
				pptf = new point3f;
				falllength += float2Mode(&(pCurveCell->point[(i + 1)%nSize] - pCurveCell->point[i]));
				float3fFromfloat2fEx(pptf,&pCurveCell->point[(i + 1)%nSize],falllength,Plane_XY);
				
				//*pptf = pCurveCell->point[(i + 1)%nSize];
				SYAR_Add(pptrPoly,pptf);

				if(pptrPolyNorm)
				{
					pointf* pptnormal1 = new pointf;
					pointf* pptnormal2 = new pointf;
					*pptnormal2 = *pptnormal1 = ptNormal;
					SYAR_Add(pptrPolyNorm,pptnormal1);
					SYAR_Add(pptrPolyNorm,pptnormal2);			
				}
			}
		}
		else if (pCurveCell->pttype[i] == point_conic_start)
		{
			ctrlPoints[0] = pCurveCell->point[i];
			int k = i+1; // 中间控制顶点index
			float fTemp = 1.f / 3.f;
			while (k < pCurveCell->npoint && pCurveCell->pttype[k] == point_conic) 
			{
				if (pCurveCell->pttype[(k+1)%nSize] != point_conic || k+1 >= pCurveCell->npoint - 1) 
				{
					ctrlPoints[3] = pCurveCell->point[(k+1)%nSize];
				}
				else
				{
					ctrlPoints[3] = (pCurveCell->point[(k+1)%nSize] + pCurveCell->point[k]) * 0.5f;
				}
				ctrlPoints[1] = (pCurveCell->point[k] * 2 + ctrlPoints[0]) * fTemp;
				ctrlPoints[2] = (pCurveCell->point[k] * 2 + ctrlPoints[3]) * fTemp;

				//TPGT_AddCurve2PtfPoly(OUT CSYPoint2fArray * pptrPoly, IN pointf * ctrlPoints);
				AddCurve2PtfPoly(pptrPoly, pptrPolyNorm, ctrlPoints,&falllength);
				//printf("k = %d,pCurveCell->npoint = %d ctrlPoints = 0x%08x\n",k,pCurveCell->npoint,(DWORD)ctrlPoints);
				k++;
				ctrlPoints[0] = ctrlPoints[3];
			} // end of while
			i = k - 1;
		} // end of conic
		else if (pCurveCell->pttype[i] == point_curve)
		{
			if ( i+3 > pCurveCell->npoint) 
			{
				break;
			}
			for (j=0; j<4; j++)
			{
				ctrlPoints[j] = pCurveCell->point[(i+j)%nSize];
			}
			pointf ptNormal;
			for(int k = 1;k < 4;k ++)
			{
				ptNormal.x = ctrlPoints[0].y - ctrlPoints[k].y;
				ptNormal.y = ctrlPoints[k].x - ctrlPoints[0].x;
				if(abs(ptNormal.x) > ABS_FLOAT_0 || abs(ptNormal.y) > ABS_FLOAT_0)
				{
					AddCurve2PtfPoly(pptrPoly, pptrPolyNorm, ctrlPoints,&falllength);
					break;
				}
			}
			i += 2;
		}
	}
	if(pMidPtIndex)
		*pMidPtIndex = nPolyBack;

	

	nSize = SYAR_GetSize(pptrPoly);
	if(nSize > 0)
	{
	
		point2f *p2 = (point2f*)SYAR_GetAt(pptrPoly,nSize - 1);
		if(p2)
		{
			delete p2;
			p2 = NULL;
			SYAR_RemoveAt(pptrPoly,nSize -1 ,1);
		}
	}
} // END OF Cell2PtfPoly 


void SYOC_Cell2PtlfPoly(IN vectorcell * pCurveCell, OUT SYArray * pptrPoly,OUT SYArray * pptrPolyNorm,int* pMidPtIndex)
{
	int i, j;
	point3lf * pptf = NULL;
	point2f ctrlPoints[4];
	point2f ptNormal;
	int nSize = pCurveCell->npoint;
	int nPolyBack = -1;
	pptf = new point3lf;
	v2tov3(pptf,&pCurveCell->point[0],0,Plane_XY);
	//float3fFromfloat2fEx(pptf,pCurveCell->point[0],0,Plane_XY);
	//*pptf = pCurveCell->point[0];
	SYAR_Add(pptrPoly,pptf);
	float falllength = 0;


	for(i = 0; i<pCurveCell->npoint; i++)
	{
		if(pMidPtIndex && *pMidPtIndex == i)
		{
			nPolyBack = SYAR_GetSize(pptrPoly);
		}
		if (pCurveCell->pttype[i] == point_line)
		{
			ptNormal.x = pCurveCell->point[i].y - pCurveCell->point[(i + 1)%nSize].y;
			ptNormal.y = pCurveCell->point[(i + 1)%nSize].x - pCurveCell->point[i].x;
			if (abs(ptNormal.x) > ABS_FLOAT_0 || abs(ptNormal.y) > ABS_FLOAT_0)
			{
				pptf = new point3lf;
				falllength += float2Mode(&(pCurveCell->point[(i + 1)%nSize] - pCurveCell->point[i]));
				v2tov3(pptf,&pCurveCell->point[(i + 1)%nSize],falllength,Plane_XY);
				
				//*pptf = pCurveCell->point[(i + 1)%nSize];
				SYAR_Add(pptrPoly,pptf);

				if(pptrPolyNorm)
				{
					pointf* pptnormal1 = new pointf;
					pointf* pptnormal2 = new pointf;
					*pptnormal2 = *pptnormal1 = ptNormal;
					SYAR_Add(pptrPolyNorm,pptnormal1);
					SYAR_Add(pptrPolyNorm,pptnormal2);
				}
			}
		}
		else if (pCurveCell->pttype[i] == point_conic_start)
		{
			ctrlPoints[0] = pCurveCell->point[i];
			int k = i+1; // 中间控制顶点index
			float fTemp = 1.f / 3.f;
			while (k < pCurveCell->npoint && pCurveCell->pttype[k] == point_conic) 
			{
				if (pCurveCell->pttype[(k+1)%nSize] != point_conic || k+1 >= pCurveCell->npoint - 1) 
				{
					ctrlPoints[3] = pCurveCell->point[(k+1)%nSize];
				}
				else
				{
					ctrlPoints[3] = (pCurveCell->point[(k+1)%nSize] + pCurveCell->point[k]) * 0.5f;
				}
				ctrlPoints[1] = (pCurveCell->point[k] * 2 + ctrlPoints[0]) * fTemp;
				ctrlPoints[2] = (pCurveCell->point[k] * 2 + ctrlPoints[3]) * fTemp;

				//TPGT_AddCurve2PtfPoly(OUT CSYPoint2fArray * pptrPoly, IN pointf * ctrlPoints);
				AddCurve2PtlfPoly(pptrPoly, pptrPolyNorm, ctrlPoints,&falllength);
				k++;
				ctrlPoints[0] = ctrlPoints[3];
			} // end of while
			i = k - 1;
		} // end of conic
		else if (pCurveCell->pttype[i] == point_curve)
		{
			if ( i+3 > pCurveCell->npoint) 
			{
				break;
			}
			for (j=0; j<4; j++)
			{
				ctrlPoints[j] = pCurveCell->point[(i+j)%nSize];
			}

			//if (i + 3 == pCurveCell->npoint - 1) 
			//{
			//	ctrlPoints[3] = pCurveCell->point[0];
			//}

			pointf ptNormal;
			for(int k = 1;k < 4;k ++)
			{
				ptNormal.x = ctrlPoints[0].y - ctrlPoints[k].y;
				ptNormal.y = ctrlPoints[k].x - ctrlPoints[0].x;
				if(abs(ptNormal.x) > ABS_FLOAT_0 || abs(ptNormal.y) > ABS_FLOAT_0)
				{
					AddCurve2PtlfPoly(pptrPoly, pptrPolyNorm, ctrlPoints,&falllength);
					break;
				}
			}
			i += 2;
		}
	}
	if(pMidPtIndex)
		*pMidPtIndex = nPolyBack;

	

	nSize = SYAR_GetSize(pptrPoly);
	if(nSize > 0)
	{
		point2f *p2 = (point2f*)SYAR_GetAt(pptrPoly,nSize - 1);
		if(p2)
		{
			delete p2;
			p2 = NULL;
			SYAR_RemoveAt(pptrPoly,nSize -1 ,1);
		}
	}
} // END OF Cell2PtfPoly 

void SYOC_DelColine(IN OUT SYArray *pptfArr,int* pMidPtIndex)
{
	int i;
	pointf *prePt = NULL;
	pointf *curPt = NULL;
	pointf *nextPt = NULL;
	float fRight = 0;
	int nSize = (int)SYAR_GetSize(pptfArr);
	for (i = nSize - 1; i>-1; i--)
	{
		if(i == 0) 
		{
			prePt = (pointf*)SYAR_GetAt(pptfArr,nSize-1);
			curPt = (pointf*)SYAR_GetAt(pptfArr,0);
			nextPt = (pointf*)SYAR_GetAt(pptfArr,1);
		}
		else if(i == nSize-1)
		{
			prePt = (pointf*)SYAR_GetAt(pptfArr,nSize-2);
			curPt = (pointf*)SYAR_GetAt(pptfArr,nSize-1);
			nextPt = (pointf*)SYAR_GetAt(pptfArr,0);
		}
		else
		{
			prePt = (pointf*)SYAR_GetAt(pptfArr,i-1);
			curPt = (pointf*)SYAR_GetAt(pptfArr,i);
			nextPt = (pointf*)SYAR_GetAt(pptfArr,i+1);
		}


		fRight = (curPt->x - prePt->x)*(nextPt->y - prePt->y) - (curPt->y - prePt->y)*(nextPt->x - prePt->x);
		if(fRight> -0.01f && fRight< 0.01f )
		{//连续三点共线
			
			SYAR_RemoveAt(pptfArr,i,1);
			if(pMidPtIndex && *pMidPtIndex > i)
				*pMidPtIndex--;
			delete curPt;
			curPt = NULL;
			nSize = (int)SYAR_GetSize(pptfArr);
			if (nSize < 3) 
			{
				return;
			}
		}
	}
} // END OF DelColine
//------------------------------------------------------------------
//函数 SYOC_AddCurve2PtfPoly 说明
//功能：将三次Bezier曲线段转化并加入多边形数组
//输入：
//输出：
//------------------------------------------------------------------
void SYOC_AddCurve2PtfPoly(OUT point2f * pptPoly, OUT int &nPolyCount, IN point2f * ctrlPoints)
{
	// 接近直线，则直接加入
	// 否则分成两段，递归调用
	if ( SYOC_NearLine(ctrlPoints) )
	{
		if (pptPoly)
		{
			pptPoly[nPolyCount] = ctrlPoints[3];
		}

		nPolyCount++;
	}
	else
	{
		point2f ptfFormer[4];
		point2f ptfLatter[4];

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

		SYOC_AddCurve2PtfPoly(pptPoly, nPolyCount, ptfFormer ) ;
		SYOC_AddCurve2PtfPoly(pptPoly, nPolyCount, ptfLatter ) ;
	}
} // END OF SYOC_AddCurve2PtfPoly

//------------------------------------------------------------------
//函数 SYOC_Cell2PtfPoly 说明
//功能：将pCurveCell转化为多边形数组
//输入：
//输出：
//------------------------------------------------------------------
void SYOC_Cell2PtfPoly(IN vectorcell * pCurveCell, OUT point2f * pptPoly, OUT int &nPolyCount)
{
	int i, j;
	point2f * pptf = NULL;
	point2f ctrlPoints[4];

	for(i = 0; i<pCurveCell->npoint; i++)
	{
		if (pCurveCell->pttype[i] == point_line)
		{
			if (pptPoly)
			{
				pptPoly[nPolyCount] = pCurveCell->point[i];
			}

			nPolyCount++;
		}
		else if (pCurveCell->pttype[i] == point_conic_start)
		{
			ctrlPoints[0].x = pCurveCell->point[i].x;
			ctrlPoints[0].y = pCurveCell->point[i].y;
			int k = i+1; // 中间控制顶点index
			while (pCurveCell->pttype[k] == point_conic) 
			{
				if (pCurveCell->pttype[k+1] != point_conic || k + 1 >= pCurveCell->npoint - 1) 
				{
					ctrlPoints[3].x = pCurveCell->point[k+1].x;
					ctrlPoints[3].y = pCurveCell->point[k+1].y;
				}
				else
				{
					ctrlPoints[3].x = (pCurveCell->point[k+1].x + pCurveCell->point[k].x) * 0.5f;
					ctrlPoints[3].y = (pCurveCell->point[k+1].y + pCurveCell->point[k].y) * 0.5f;
				}
				ctrlPoints[1].x = (2.0f/3)*pCurveCell->point[k].x + ctrlPoints[0].x/3.0f;
				ctrlPoints[1].y = (2.0f/3)*pCurveCell->point[k].y + ctrlPoints[0].y/3.0f;

				ctrlPoints[2].x = (2.0f/3)*pCurveCell->point[k].x + ctrlPoints[3].x/3.0f;
				ctrlPoints[2].y = (2.0f/3)*pCurveCell->point[k].y + ctrlPoints[3].y/3.0f;

				if (pptPoly)
				{
					pptPoly[nPolyCount] = ctrlPoints[0];
				}

				nPolyCount++;

				SYOC_AddCurve2PtfPoly(pptPoly, nPolyCount, ctrlPoints);

				k++;
				i = k;

				ctrlPoints[0] = ctrlPoints[3];

				if (k == pCurveCell->npoint - 1) 
				{
					if (pptPoly)
					{
						pptPoly[nPolyCount] = pCurveCell->point[k];
					}

					nPolyCount++;

					i++;
					break;
				}
				if (k > pCurveCell->npoint - 1) 
				{
					i++;
					break;
				}
			} // end of while
			i--; 
		} // end of conic
		else if (pCurveCell->pttype[i] == point_curve_start || pCurveCell->pttype[i] == point_curve)
		{
			if ( i+2 >= pCurveCell->npoint - 1 ) 
			{
				break;
			}
			for (j=0; j<4; j++)
			{
				ctrlPoints[j].x = pCurveCell->point[i+j].x;
				ctrlPoints[j].y = pCurveCell->point[i+j].y;
			}
			if (pptPoly)
			{
				pptPoly[nPolyCount] = ctrlPoints[0];
			}

			nPolyCount++;

			SYOC_AddCurve2PtfPoly(pptPoly, nPolyCount, ctrlPoints);

			i += 2;
		}
	}
} // END OF SYOC_Cell2PtfPoly

//------------------------------------------------------------------
//功能：去掉相邻重合点
//输入：
//输出：
//返回值：
//------------------------------------------------------------------
void SYOC_DelOverlap(IN OUT SYArray *pptrOutlineArray, // point2f*型
					 IN OUT int* pMidIndex)
{
	int nPtCount = (int)SYAR_GetSize(pptrOutlineArray);
	// 去掉相邻重合点
	int nDel = 0;
	for(int j = nPtCount - 1; j>= 0; j --)
	{
		nDel = 0;
		point2f* pptf = (point2f*)SYAR_GetAt(pptrOutlineArray,j);
		for (int k = j - 1; k>=0; k--)
		{
			point2f* pptfk = (point2f*)SYAR_GetAt(pptrOutlineArray,k);
			if(fabs(pptf->x - pptfk->x) < ABS_FLOAT_0 && fabs(pptf->y - pptfk->y) < ABS_FLOAT_0 )
			{
				if(pMidIndex && k < *pMidIndex)
					*pMidIndex--;
				SYAR_RemoveAt(pptrOutlineArray,k,1);
				delete pptfk;
				pptfk = NULL;		
				nDel++;
			}
			else
			{
				break;
			}
		}
		j -= nDel;
	}

} // END OF SYOC_DelOverlap

//------------------------------------------------------------------
//功能：去掉相邻重合点
//输入：
//输出：
//返回值：
//------------------------------------------------------------------
void SYOC_DelOverlap(IN OUT SYArray *pptrOutlineArray,  // point2f*型
					 IN OUT SYArray *pptrNormArray  // point2f*型
					 )
{
	int nPtCount = (int)SYAR_GetSize(pptrOutlineArray);
	// 去掉相邻重合点
	int nDel = 0;
	for(int j = nPtCount - 1; j>= 0; j --)
	{
		nDel = 0;
		point2f* pptf = (point2f*)SYAR_GetAt(pptrOutlineArray,j);
		for (int k = j - 1; k>=0; k--)
		{
			point2f* pptfk = (point2f*)SYAR_GetAt(pptrOutlineArray,k);
			if(fabs(pptf->x - pptfk->x) < ABS_FLOAT_0 && fabs(pptf->y - pptfk->y) < ABS_FLOAT_0 )
			{
				SYAR_RemoveAt(pptrOutlineArray,k,1);
				delete pptfk;
				nDel++;

				pptfk = (point2f*)SYAR_GetAt(pptrNormArray,2*k + 1);
				delete pptfk;
				SYAR_RemoveAt(pptrNormArray,2*k + 1,1);

				pptfk = (point2f*)SYAR_GetAt(pptrNormArray,2*k);
				delete pptfk;
				pptfk = NULL;
				SYAR_RemoveAt(pptrNormArray,2*k,1);
			}
			else
			{
				break;
			}
		}
		j -= nDel;
	}

} // END OF SYOC_DelOverlap

//------------------------------------------------------------------
//功能：去掉相邻共线冗余点
//输入：
//输出：
//返回值：
//------------------------------------------------------------------
void SYOC_DelColine(IN OUT CSYPoint2fPtrArray *pptfArr)
{
	int i;
	point2f *prePt = NULL;
	point2f *curPt = NULL;
	point2f *nextPt = NULL;
	float fRight = 0;
	int nSize = (int)pptfArr->GetSize();
	for (i = nSize - 1; i>-1; i--)
	{
		if (nSize <= 2) 
		{
			return;
		}

		if(i == 0) 
		{
			prePt = (point2f*)pptfArr->GetAt(nSize-1);
			curPt = (point2f*)pptfArr->GetAt(0);
			nextPt = (point2f*)pptfArr->GetAt(1);
		}
		else if(i == nSize-1)
		{
			prePt = (point2f*)pptfArr->GetAt(nSize-2);
			curPt = (point2f*)pptfArr->GetAt(nSize-1);
			nextPt = (point2f*)pptfArr->GetAt(0);
		}
		else
		{
			prePt = (point2f*)pptfArr->GetAt(i-1);
			curPt = (point2f*)pptfArr->GetAt(i);
			nextPt = (point2f*)pptfArr->GetAt(i+1);
		}


		fRight = (curPt->x - prePt->x)*(nextPt->y - prePt->y) - (curPt->y - prePt->y)*(nextPt->x - prePt->x);
		if(fRight> -ABS_FLOAT_0 && fRight< ABS_FLOAT_0 
			&& (curPt->x <= prePt->x && curPt->x >= nextPt->x || curPt->x >= prePt->x && curPt->x <= nextPt->x)
			&& (curPt->y <= prePt->y && curPt->y >= nextPt->y || curPt->y >= prePt->y && curPt->y <= nextPt->y))
		{//连续三点共线
			pptfArr->RemoveAt(i);
			delete curPt;
			curPt = NULL;
			nSize = (int)pptfArr->GetSize();
			if (nSize < 3) 
			{
				return;
			}
		}
	}
} // END OF SYOC_DelColine

//------------------------------------------------------------------
//功能：去掉相邻共线冗余点
//输入：
//输出：
//返回值：
//------------------------------------------------------------------
void SYOC_DelColine(IN OUT SYArray *pptfArr, IN OUT SYArray *pptfArrNorm)
{
	int i;
	point2f *prePt = NULL;
	point2f *curPt = NULL;
	point2f *nextPt = NULL;
	float fRight = 0;
	int nSize = (int)SYAR_GetSize(pptfArr);
	for (i = nSize - 1; i>-1; i--)
	{
		if (nSize <= 2) 
		{
			return;
		}

		if(i == 0) 
		{
			prePt = (point2f*)SYAR_GetAt(pptfArr,nSize-1);
			curPt = (point2f*)SYAR_GetAt(pptfArr,0);
			nextPt = (point2f*)SYAR_GetAt(pptfArr,1);
		}
		else if(i == nSize-1)
		{
			prePt = (point2f*)SYAR_GetAt(pptfArr,nSize-2);
			curPt = (point2f*)SYAR_GetAt(pptfArr,nSize-1);
			nextPt = (point2f*)SYAR_GetAt(pptfArr,0);
		}
		else
		{
			prePt = (point2f*)SYAR_GetAt(pptfArr,i-1);
			curPt = (point2f*)SYAR_GetAt(pptfArr,i);
			nextPt = (point2f*)SYAR_GetAt(pptfArr,i+1);
		}


		fRight = (curPt->x - prePt->x)*(nextPt->y - prePt->y) - (curPt->y - prePt->y)*(nextPt->x - prePt->x);
		if(fRight> -ABS_FLOAT_0 && fRight< ABS_FLOAT_0 
			&& (curPt->x <= prePt->x && curPt->x >= nextPt->x || curPt->x >= prePt->x && curPt->x <= nextPt->x)
			&& (curPt->y <= prePt->y && curPt->y >= nextPt->y || curPt->y >= prePt->y && curPt->y <= nextPt->y))
		{//连续三点共线
			SYAR_RemoveAt(pptfArr,i,1);
			delete curPt;

			curPt = (point2f*)SYAR_GetAt(pptfArrNorm,2*i + 1);
			delete curPt;
			SYAR_RemoveAt(pptfArrNorm,2*i + 1,1);

			curPt = (point2f*)SYAR_GetAt(pptfArrNorm,2*i);
			delete curPt;
			curPt = NULL;
			SYAR_RemoveAt(pptfArrNorm,2*i,1);

			nSize = (int)SYAR_GetSize(pptfArr);
			if (nSize < 3) 
			{
				return;
			}
		}
	}
} // END OF SYOC_DelColine

//------------------------------------------------------------------
//功能：去掉重合点
//输入：
//输出：
//返回值：
//------------------------------------------------------------------
void SYOC_DelOverlap(IN OUT point2f * pPoly, int &nPtCount)
{
	// 去掉相邻重合点
	int nDel = 0;
	int nDelAll = 0;
	int i, j, k;
	for(j = nPtCount - 1; j>= 0; j --)
	{
		nDel = 0;
		
		if (pPoly[j].x == 65535)
		{
			continue;
		}
		for (k = (j - 1 + nPtCount)%nPtCount; k>=0; k--)
		{
			if (pPoly[k].x == 65535)
			{
				continue;
			}

			if(fabs(pPoly[j].x - pPoly[k].x) < ABS_FLOAT_0 && fabs(pPoly[j].y - pPoly[k].y) < ABS_FLOAT_0 )
			{
				pPoly[k].x = 65535;
				pPoly[k].y = 65535;
				nDel++;
				nDelAll++;
			}
			else
			{
				break;
			}
		}
		//j -= nDel;
	}

	// 收缩数组
	if (nDelAll > 0)
	{
		j = 0;
		point2f * pPolyTemp = pPoly;
		for (i = 0; i < nPtCount; i++)
		{
			if (pPoly[i].x == 65535)
			{
				continue;
			}
			pPolyTemp[j++] = pPoly[i];
		}
		nPtCount -= nDelAll;
	}	
} // END OF SYOC_DelOverlap

//------------------------------------------------------------------
//功能：去掉相邻共线冗余点
//输入：
//输出：
//返回值：
//------------------------------------------------------------------
void SYOC_DelColine(IN OUT point2f * pPoly, int &nSize)
{
	int i;
	point2f prePt, curPt, nextPt;
	float fRight = 0;
	for (i = nSize - 1; i>-1; i--)
	{
		if (nSize <= 2) 
		{
			return;
		}

		if(i == 0) 
		{
			prePt = pPoly[nSize - 1];
			curPt = pPoly[0];
			nextPt = pPoly[1];
		}
		else if(i == nSize-1)
		{
			prePt = pPoly[nSize - 2];
			curPt = pPoly[nSize - 1];
			nextPt = pPoly[0];
		}
		else
		{
			prePt = pPoly[i-1];
			curPt = pPoly[i];
			nextPt = pPoly[i+1];
		}

		fRight = (curPt.x - prePt.x)*(nextPt.y - prePt.y) - (curPt.y - prePt.y)*(nextPt.x - prePt.x);
		if(fRight> -ABS_FLOAT_0 && fRight< ABS_FLOAT_0 
			&& (curPt.x <= prePt.x && curPt.x >= nextPt.x || curPt.x >= prePt.x && curPt.x <= nextPt.x)
			&& (curPt.y <= prePt.y && curPt.y >= nextPt.y || curPt.y >= prePt.y && curPt.y <= nextPt.y))
		{//连续三点共线
			for (int j = i; j < nSize-1; j++)
			{
				pPoly[j] = pPoly[j+1];
			}
			nSize--;
			if (nSize < 3) 
			{
				return;
			}
		}
	}
} // END OF SYOC_DelColine

//------------------------------------------------------------------
//功能：去掉冗余点
//输入：
//输出：
//返回值：
//------------------------------------------------------------------
void SYOC_DelRedundance(IN OUT point2f * pPoly, int &nPtCount)
{
	SYOC_DelOverlap(pPoly, nPtCount);
	SYOC_DelColine(pPoly, nPtCount);
}

//------------------------------------------------------------------
//功能：向量归一化
//输入：
//输出：
//返回值：
//------------------------------------------------------------------
void SYOC_Identify(point2f &v)
{
	float d = sqrt(v.x * v.x + v.y * v.y);
	//ASSERT(d > ABS_FLOAT_0);
	if (d <= ABS_FLOAT_0)
	{
		return;
	}

	v.x /= d;
	v.y /= d;
} // END OF SYOC_Identify

//------------------------------------------------------------------
//功能：判断两个向量归一化后是否相等
//输入：
//输出：
//返回值：
//------------------------------------------------------------------
BOOL SYOC_SameDir(const point2f *p)
{
	point2f v0, v1;
	v0.x = p[1].x - p[0].x;
	v0.y = p[1].y - p[0].y;
	v1.x = p[3].x - p[2].x;
	v1.y = p[3].y - p[2].y;

	// 归一化向量
	SYOC_Identify(v0);
	SYOC_Identify(v1);
	if ( abs(v0.x - v1.x) < ABS_FLOAT_0 && abs(v0.y - v1.y) < ABS_FLOAT_0 ) 
	{
		return TRUE;
	}
	return FALSE;
} // END OF SYOC_SameDir

//------------------------------------------------------------------
//功能：偏移线段p[1], p[2]
//输入：
//输出：
//返回值：
//------------------------------------------------------------------
void SYOC_CalShiftPt(IN const point2f *p, // 0, 1, 2, 3
					 IN const float fDis, 
					 IN const int ndirect, 
					 OUT point2f &Q1, 
					 OUT point2f &Q2 )
{
	//给定一个端点的方向, 求点. 首先, 求对角线的单位向量. 然后求单位向量对应的点, 到线的距离. 然后求出所求的点是多少倍的单位向量.
	//先求单位向量到线段的投影, 然后用勾股定理求出距离
	vector2f v0, v1, v2;
	v0 = p[1] - p[0];
	v1 = p[2] - p[1];
	v2 = p[3] - p[2];

	float2Normalize(&v0,&v0);
	float2Normalize(&v1,&v1);
	float2Normalize(&v2,&v2);

	v0 = v1 - v0;
	v2 = v2 - v1;
	float2Normalize(&v0,&v0);
	float2Normalize(&v2,&v2);

	// 归一化后的点积, 就是cos, 然后可以算出sin. 然后fDis/sin即向量长度. 再乘以角平分线的单位向量, 就可以得到向量. 然后再加上端点就得到
	float fCos = float2DotProduct(&v0,&v1);
	float fSin = sqrt(1 - fCos * fCos);

	if (fSin < ABS_FLOAT_0) 
	{
		Q1.x = - fDis * v1.y + p[1].x;
		Q1.y = fDis * v1.x + p[1].y;
	}
	else
	{
		float fL = fDis / fSin;
		Q1 = p[1] + v0 * fL ;
	}
	
	fCos = float2DotProduct(&v1,&v2);
	fSin = sqrt(1 - fCos * fCos);
	if (fSin < ABS_FLOAT_0) 
	{
		Q2.x = - fDis * v1.y + p[2].x;
		Q2.y = fDis * v1.x + p[2].y;
	}
	else
	{
		float fL = fDis / fSin;
		Q2 = p[2] + v2 * fL;
	}
	
	// 如果 Q1, Q2在p[1]p[2]左侧, 则调整到右侧
	BOOL bTemp = FALSE;
	if (ndirect == -1) 
	{
		bTemp = TRUE;
	}
	if ( !(bTemp^SYOC_PtOnRight(p[1],p[2],Q1)) ) 
	{
		Q1 = p[1] * 2 -Q1;
		//Q1.x = p[1].x + p[1].x - Q1.x;
		//Q1.y = p[1].y + p[1].y - Q1.y;
	}

	if ( !(bTemp^SYOC_PtOnRight(p[1],p[2],Q2)) ) 
	{
		Q2 = p[2]* 2 - Q2;
		//Q2.x = p[2].x + p[2].x - Q2.x;
		//Q2.y = p[2].y + p[2].y - Q2.y;
	}
} // END OF SYOC_CalShiftPt

//------------------------------------------------------------------
//功能：偏移线段p[1], p[2], 两对角线不平行
//输入：
//输出：
//返回值：
//------------------------------------------------------------------
void SYOC_CalShiftPt2(IN const point2f *p, // 0, 1, 2, 3
					 IN const float fDis, 
					 IN const int ndirect,
					 OUT point2f &Q1, 
					 OUT point2f &Q2 )
{
	//给定一个端点的方向, 求点. 首先, 求对角线的单位向量. 然后求单位向量对应的点, 到线的距离. 然后求出所求的点是多少倍的单位向量.
	//先求单位向量到线段的投影, 然后用勾股定理求出距离
	point2f v0, v1, v2;
	v0.x = p[1].x - p[0].x;
	v0.y = p[1].y - p[0].y;
	v1.x = p[2].x - p[1].x;
	v1.y = p[2].y - p[1].y;
	v2.x = p[3].x - p[2].x;
	v2.y = p[3].y - p[2].y;

	// 归一化向量
	SYOC_Identify(v0);
	SYOC_Identify(v1);
	SYOC_Identify(v2);

	// 转化成对角线方向, 注意! 方向可能是反向
	v0 = v1 - v0;
	v2 = v2 - v1;

	SYOC_Identify(v0);
	SYOC_Identify(v2);

	// 归一化后的点积, 就是cos, 然后可以算出sin. 然后fDis/sin即向量长度. 再乘以角平分线的单位向量, 就可以得到向量. 然后再加上端点就得到
	float fCos = v2.x * v1.x + v2.y * v1.y;
	float fSin = sqrt(1 - fCos * fCos);
	float fL = fDis / fSin;

	Q2.x = fL * v2.x + p[2].x;
	Q2.y = fL * v2.y + p[2].y;

	fCos = v0.x * v1.x + v0.y * v1.y;
	fSin = sqrt(1 - fCos * fCos);
	fL = fDis / fSin;

	Q1.x = fL * v0.x + p[1].x;
	Q1.y = fL * v0.y + p[1].y;

	// 如果 Q1, Q2在p[1]p[2]左侧, 则调整到右侧
	BOOL bTemp = TRUE;
	if (ndirect == -1) 
	{
		bTemp = FALSE;
	}
	if ( !(bTemp^SYOC_PtOnRight(p[1],p[2],Q1)) ) 
	{
		Q1.x = p[1].x + p[1].x - Q1.x;
		Q1.y = p[1].y + p[1].y - Q1.y;
	}

	if ( !(bTemp^SYOC_PtOnRight(p[1],p[2],Q2)) ) 
	{
		Q2.x = p[2].x + p[2].x - Q2.x;
		Q2.y = p[2].y + p[2].y - Q2.y;
	}

	// 求MCC
	// 因为对角线不平行, 所以(v2.y * v0.x - v2.x * v0.y)不为0, 故可以求得交点; 
	float a = (v2.y * p[2].x - v2.x * p[2].y + v2.x * p[1].y - v2.y * p[1].x) / (v2.y * v0.x - v2.x * v0.y);	
	if (abs(fL) > abs(a) ) 
	{
		point2f MCC;
		MCC.x = p[1].x + v0.x * a;
		MCC.y = p[1].y + v0.y * a;
		if ( !(bTemp^SYOC_PtOnRight(p[1],p[2],MCC)) ) 
		{
			return;
		}
		Q1 = Q2 = MCC;
	}

} // END OF SYOC_CalShiftPt2

int	SYOC_arc_near_line(point2f ptcenter,point2f ptstart,point2f ptend,float frad)
{
	point2f ptv1 = ptstart - ptcenter;
	point2f ptv2 = ptend - ptcenter;

	point2f ptv = (ptv1 + ptv2)/2;
	float mode = float2Mode(&ptv);
	mode = frad - mode;
	//float mode = abs(frad * frad - (ptv.x * ptv.x + ptv.y * ptv.y));
	if(abs(mode) < 0.3f)
		return 1;
	return 0;
	//
	//float mode = float2Mode(&ptv);
	//float f = float2DotProduct(&ptv1,&ptv2);
	//if(f/(frad*frad) > 0.90)
	//	return 1;
	//return 0;

} // END OF SYOC_arc_near_line

int SYOC_makearc(COffsetNodeArray* parray,point2f ptcenter,point2f ptstart,point2f ptend,float rad)
{
	point2f ptv1 = ptstart - ptcenter;
	if(SYOC_arc_near_line(ptcenter,ptstart,ptend,rad))
	{
		OffsetNode *pNode = new OffsetNode;
		pNode->pt = ptend;
		pNode->bStart = FALSE;
		parray->Add(pNode);
		return 1;
	}

	point2f ptmid = (ptstart + ptend)*0.5f;
	point2f pvector = ptmid - ptcenter;
	if(pvector.x == 0.0f &&pvector.y == 0.0f)
	{
		point2f ptv = (ptend - ptstart) * 0.5f;
		float f = ptv.x;
		ptv.x = -ptv.y;
		ptv.y = f;
		ptmid = ptv + ptcenter;
	}
	else
	{
		float fdisrate = 1.0f/sqrt(pvector.x * pvector.x + pvector.y * pvector.y);
		pvector = pvector * fdisrate;
		pvector *= rad;
		ptmid = pvector + ptcenter;
		if((*(DWORD*)(&ptmid.x)) == 0xffc00000)
		{
			int a = 0;
		}
	}
	SYOC_makearc(parray,ptcenter,ptstart,ptmid,rad);
	SYOC_makearc(parray,ptcenter,ptmid,ptend,rad);
	return 1;
} // END OF SYOC_makearc

//------------------------------------------------------------------
//功能：偏移得到偏移曲线
//输入：
//输出：
//返回值：
//------------------------------------------------------------------
void SYOC_ShiftData(IN SYArray *pptSrcArr, 
				   OUT COffsetNodeArray *pOffsetArray, // OffsetNode*型
				   IN float fDis,
				   IN int ndirect
				  )
{
	// 对于每一段
	// 1. 先求AngleBisector i, i+1.
	//    1.1 若二者平行, 直接求Qi, Qi+1. 继续下一段
	//    1.2 否则, 求出当前线段的MCC
	// 2. 判断MCC在PiPi+1左侧还是右侧
	// 3. 求PiPi+1对应点QiQi+1
	// 4. 判断PiPi+1对应点QiQi+1是否超过MCC (即fDis > d, d 为MCC到直线PiPi+1的距离)
	//    4.1 如超过, 则令对应点为MCC. 
	//    4.2 否则, 求出对应点

	float fRight = 0;
	//float fV1,fV2;//Pi+1Pi 及 Pi+1Pi+2 长度
	int nPtCount = (int)SYAR_GetSize(pptSrcArr);
	if(nPtCount<2) 
	{
		return;
	}
	point2f *prePt = NULL, *curPt = NULL, *nextPt = NULL;
	point2f p[4]; // p[1], p[2]为当前线段的两个端点
	point2f Q1, Q2; //p[1], p[2]偏移后得到的点
	OffsetNode * pNode = NULL;
	point2f ptstart, ptend;
	float fD = 0.f;
	prePt = (point2f*)SYAR_GetAt(pptSrcArr,nPtCount-1);
	p[0] = *prePt;
	prePt = (point2f*)SYAR_GetAt(pptSrcArr,0);
	p[1] = *prePt;
	prePt = (point2f*)SYAR_GetAt(pptSrcArr,1);
	p[2] = *prePt;
	
	// 先加起点
	if ( abs( p[1].x - p[0].x ) + abs (p[1].y - p[0].y) > ABS_FLOAT_0)
	{
		// 非闭合
		fD = (p[2].x - p[1].x) * (p[2].x - p[1].x) + (p[2].y - p[1].y) * (p[2].y - p[1].y);
		fD = 1.f / sqrtf(fD);
		ptend.x = p[1].x - ndirect * fDis * (p[2].y - p[1].y) * fD;
		ptend.y = p[1].y + ndirect * fDis * (p[2].x - p[1].x) * fD;
		
		ptstart.x = p[1].x + fDis * (p[1].x - p[2].x) * fD;
		ptstart.y = p[1].y + fDis * (p[1].y - p[2].y) * fD;
		SYOC_makearc(pOffsetArray, p[1], ptstart, ptend, fDis);
	}
	else
	{
		// 闭合
		prePt = (point2f*)SYAR_GetAt(pptSrcArr,nPtCount-2);
		p[0] = *prePt;
		
		if (   ( !SYOC_PtOnRight(p[0],p[1],p[2]) && (ndirect == -1) )
			|| ( SYOC_PtOnRight(p[0],p[1],p[2]) && (ndirect == 1) )
			)
		{
			// 如果是角外, 生成圆弧. 最后一个点不再添加任何东西.(因为闭合情况下, 最后一个点, 就是第一个点)
			// ptstart 是p[0], p[1]在p[1]处的偏移点
			// ptend   是p[1], p[2]在p[1]处的偏移点
			fD = (p[1].x - p[0].x) * (p[1].x - p[0].x) + (p[1].y - p[0].y) * (p[1].y - p[0].y);
			fD = 1.f / sqrtf(fD);
			pNode = new OffsetNode;
			pNode->pt.x = p[1].x - ndirect * fDis * (p[1].y - p[0].y) * fD;
			pNode->pt.y = p[1].y + ndirect * fDis * (p[1].x - p[0].x) * fD;
			pNode->bStart = FALSE;
			pOffsetArray->Add(pNode);
			ptstart = pNode->pt;
			
			fD = (p[2].x - p[1].x) * (p[2].x - p[1].x) + (p[2].y - p[1].y) * (p[2].y - p[1].y);
			fD = 1.f / sqrtf(fD);
			ptend.x = p[1].x - ndirect * fDis * (p[2].y - p[1].y) * fD;
			ptend.y = p[1].y + ndirect * fDis * (p[2].x - p[1].x) * fD;
			
			SYOC_makearc(pOffsetArray, p[1], ptstart, ptend, fDis);
		}
		else
		{
			// 如果是角内, 添加对角线上一个点
			point2f v1, v2;
			v1 = p[0] - p[1];
			v2 = p[2] - p[1];

			// 归一化
			fD = v1.x * v1.x + v1.y * v1.y;
			fD = 1.f / sqrtf(fD);
			v1.x *= fD;
			v1.y *= fD;

			fD = v2.x * v2.x + v2.y * v2.y;
			fD = 1.f / sqrtf(fD);
			v2.x *= fD;
			v2.y *= fD;

			// 对角线向量并归一化
			v1 = v1 + v2;
			fD = v1.x * v1.x + v1.y * v1.y;
			fD = 1.f / sqrtf(fD);

			// 添加点
			pNode = new OffsetNode;
			pNode->pt.x = p[1].x + fDis * v1.x * fD;
			pNode->pt.y = p[1].y + fDis * v1.y * fD;
			pNode->bStart = FALSE;
			pOffsetArray->Add(pNode);
		}
	}
	
	// 另外还需要考虑的是闭合非闭合. 
	for(int i=1; i<nPtCount-2; i++)
	{
		prePt = (point2f*)SYAR_GetAt(pptSrcArr,i-1);
		p[0] = *prePt;
		prePt = (point2f*)SYAR_GetAt(pptSrcArr,i);
		p[1] = *prePt;
		prePt = (point2f*)SYAR_GetAt(pptSrcArr,i+1);
		p[2] = *prePt;
		prePt = (point2f*)SYAR_GetAt(pptSrcArr,i+2);
		p[3] = *prePt;

		//fRight = (curPt->x - prePt->x)*(nextPt->y - prePt->y) - (curPt->y - prePt->y)*(nextPt->x - prePt->x);
		//if( abs(fRight) < ABS_FLOAT_0)
		//{
		//	continue;
		//}

		// 1. 先求AngleBisector i, i+1.
		// 1.1 如果两个角平分线平行, 直接求对应点
		//if ( SYOC_SameDir(p) ) //其实就是p[i-1]p[i]与p[i+1]p[i+2]平行(有向向量), 也即归一化后点积为1.不能用叉积,否则平行但相反时出错
		//{
			SYOC_CalShiftPt(p, fDis, ndirect, Q1, Q2 );
		//}
		//else
		//{
		//	SYOC_CalShiftPt2(p, fDis, ndirect, Q1, Q2 );
		//}
		
		// 添加进结果
		////////////////////////////////////////////////////////////////////////// begin
		// 此处将要修改:　如果是角外，　生成圆弧
		// 判断是否角外: 判断Q1与p2是否位于p0p1同侧.不同侧则为角外.
		if ( SYOC_PtOnRight(p[0],p[1],Q1)^SYOC_PtOnRight(p[0],p[1],p[2]) ) 
		{
			// 如果是角外, 生成圆弧
			// 1 计算圆弧的起终点
			//   1.1 p[0]p[1]在p[1]出偏移得到ptstart
			float fD = (p[1].x - p[0].x) * (p[1].x - p[0].x) + (p[1].y - p[0].y) * (p[1].y - p[0].y);
			fD = 1.f / sqrtf(fD);
			pNode = new OffsetNode;
			pNode->pt.x = p[1].x - ndirect * fDis * (p[1].y - p[0].y) * fD;
			pNode->pt.y = p[1].y + ndirect * fDis * (p[1].x - p[0].x) * fD;
			pNode->bStart = FALSE;
			pOffsetArray->Add(pNode);
			ptstart = pNode->pt;
			//   1.2 p[1]p[2]在p[1]出偏移得到ptend
			fD = (p[1].x - p[2].x) * (p[1].x - p[2].x) + (p[1].y - p[2].y) * (p[1].y - p[2].y);
			fD = 1.f / sqrtf(fD);
			ptend.x = p[1].x - ndirect * fDis * (p[2].y - p[1].y) * fD;
			ptend.y = p[1].y + ndirect * fDis * (p[2].x - p[1].x) * fD;
			// 2 调用SYOC_makearc生成并添加圆弧离散点
			SYOC_makearc(pOffsetArray, p[1], ptstart, ptend, fDis);
		}
		else
		{
			pNode = new OffsetNode;
			pNode->pt = Q1;
			pNode->bStart = FALSE;
			pOffsetArray->Add(pNode);
		}
		//////////////////////////////////////////////////////////////////////////  end

		if (i == nPtCount-3) 
		{
			if ( SYOC_PtOnRight(p[1],p[2],Q2)^SYOC_PtOnRight(p[1],p[2],p[3]) ) 
			{
				float fD = (p[1].x - p[2].x) * (p[1].x - p[2].x) + (p[1].y - p[2].y) * (p[1].y - p[2].y);
				fD = 1.f / sqrtf(fD);
				pNode = new OffsetNode;
				pNode->pt.x = p[2].x - ndirect * fDis * (p[2].y - p[1].y) * fD;
				pNode->pt.y = p[2].y + ndirect * fDis * (p[2].x - p[1].x) * fD;
				pNode->bStart = FALSE;
				pOffsetArray->Add(pNode);
				ptstart = pNode->pt;
			
				fD = (p[3].x - p[2].x) * (p[3].x - p[2].x) + (p[3].y - p[2].y) * (p[3].y - p[2].y);
				fD = 1.f / sqrtf(fD);
				ptend.x = p[2].x - ndirect * fDis * (p[3].y - p[2].y) * fD;
				ptend.y = p[2].y + ndirect * fDis * (p[3].x - p[2].x) * fD;

				SYOC_makearc(pOffsetArray, p[2], ptstart, ptend, fDis);
			}
			else
			{
				pNode = new OffsetNode;
				pNode->pt = Q2;
				pNode->bStart = FALSE;
				pOffsetArray->Add(pNode);
			}
		}
	}
	
	// 如上循环中的算法, 最少需要四点
	// 当输入点总数为3个点时, 忽略了中间一个点对应点的生成
	if (nPtCount == 3)
	{
		prePt = (point2f*)SYAR_GetAt(pptSrcArr,0);
		p[0] = *prePt;
		prePt = (point2f*)SYAR_GetAt(pptSrcArr,1);
		p[1] = *prePt;
		prePt = (point2f*)SYAR_GetAt(pptSrcArr,2);
		p[2] = *prePt;
		// 凹侧生成一个点, 凸侧生成圆弧.
		//给定一个端点的方向, 求点. 首先, 求对角线的单位向量. 然后求单位向量对应的点, 到线的距离. 然后求出所求的点是多少倍的单位向量.
	
		//先求单位向量到线段的投影, 然后用勾股定理求出距离
		point2f v0, v1;
		v0.x = p[1].x - p[0].x;
		v0.y = p[1].y - p[0].y;
		v1.x = p[2].x - p[1].x;
		v1.y = p[2].y - p[1].y;
		
		// 归一化向量
		SYOC_Identify(v0);
		SYOC_Identify(v1);
		
		// 转化成对角线方向, 注意! 方向可能是反向
		v0 = v1 - v0;
		
		SYOC_Identify(v0); // 对角线方向归一化向量

		// 凹侧生成一个点
		pNode = new OffsetNode;
		pNode->pt.x = p[1].x + ndirect * fDis * v0.x;
		pNode->pt.y = p[1].y + ndirect * fDis * v0.y;
		pNode->bStart = FALSE;
		pOffsetArray->Add(pNode);
		// 凸侧生成圆弧.

	}

	// 再加终点
	prePt = (point2f*)SYAR_GetAt(pptSrcArr,nPtCount-2);
	p[0] = *prePt;
	prePt = (point2f*)SYAR_GetAt(pptSrcArr,nPtCount-1);
	p[1] = *prePt;
	prePt = (point2f*)SYAR_GetAt(pptSrcArr,0);
	p[2] = *prePt;
	if ( abs( p[1].x - p[2].x ) + abs (p[1].y - p[2].y) < ABS_FLOAT_0)
	{
		//// 闭合
	}
	else
	{
		// 非闭合
		fD = (p[1].x - p[0].x) * (p[1].x - p[0].x) + (p[1].y - p[0].y) * (p[1].y - p[0].y);
		fD = 1.f / sqrtf(fD);
		pNode = new OffsetNode;
		pNode->pt.x = p[1].x - ndirect * fDis * (p[1].y - p[0].y) * fD;
		pNode->pt.y = p[1].y + ndirect * fDis * (p[1].x - p[0].x) * fD;
		pNode->bStart = FALSE;
		pOffsetArray->Add(pNode);
		ptstart = pNode->pt;

		ptend.x = p[1].x + fDis * (p[1].x - p[0].x) * fD;
		ptend.y = p[1].y + fDis * (p[1].y - p[0].y) * fD;
		SYOC_makearc(pOffsetArray, p[1], ptstart, ptend, fDis);
	}

}//end of SYOC_ShiftData

int SYOC_makearc(OUT point2f * pptPoly, OUT int &nPolyCount, point2f ptcenter,point2f ptstart,point2f ptend,float rad)
{
	point2f ptv1 = ptstart - ptcenter;
	if(SYOC_arc_near_line(ptcenter,ptstart,ptend,rad))
	{
		if (pptPoly)
		{
			//point2f ptout;
			//int nIndex = _SYOC_LineCross(&pptPoly[nPolyCount - 1],&ptend,pptPoly,nPolyCount,&ptout,1);
			//if(nIndex > 0)
			//{
			//	//nPolyCount = nIndex;
			//	//pptPoly[nPolyCount++] = ptout;
			//	//pptPoly[nPolyCount++] = ptend;
			//	nPolyCount = nIndex;
			//	memcpy(&pptPoly[nIndex + 1],&pptPoly[nIndex],sizeof(point2f) * (nPolyCount - nIndex));
			//	pptPoly[nIndex] = ptout;
			//	nPolyCount++;
			//	pptPoly[nPolyCount++] = ptout;
			//	pptPoly[nPolyCount++] = ptend;

			//}
			//else
				pptPoly[nPolyCount++] = ptend;
		}
		else
		{
			nPolyCount++;
		}

		return 1;
	}

	point2f ptmid = (ptstart + ptend)*0.5f;
	point2f pvector = ptmid - ptcenter;
	if(pvector.x == 0.0f &&pvector.y == 0.0f)
	{
		point2f ptv = (ptend - ptstart) * 0.5f;
		float f = ptv.x;
		ptv.x = -ptv.y;
		ptv.y = f;
		ptmid = ptv + ptcenter;
	}
	else
	{
		float fdisrate = 1.0f/sqrt(pvector.x * pvector.x + pvector.y * pvector.y);
		pvector = pvector * fdisrate;
		pvector *= rad;
		ptmid = pvector + ptcenter;
		if((*(DWORD*)(&ptmid.x)) == 0xffc00000)
		{
			int a = 0;
		}
	}
	SYOC_makearc(pptPoly, nPolyCount,ptcenter,ptstart,ptmid,rad);
	SYOC_makearc(pptPoly, nPolyCount,ptcenter,ptmid,ptend,rad);
	return 1;
} // END OF SYOC_makearc
void Perpendicular_offset(point2f* pout,point2f* base,vector2f v,float flength,int ndirect)
{
	float fratio = flength/float2Mode(&v);
	v *= fratio;
	pout->x = base->x - ndirect * v.y;
	pout->y = base->y + ndirect * v.x;
}

//------------------------------------------------------------------
//功能：偏移得到偏移曲线
//输入：
//输出：
//返回值：
//------------------------------------------------------------------
void SYOC_ShiftCubeData(OUT point2f * pptShift, 
					OUT int& nShiftCount, 
					IN const point2f * pptPoly, 
					IN const int nPolyCount,
					IN float fCubeOffset,
					IN float fAngle,
					IN int ndirect)
{
	float fRight = 0;
	if(nPolyCount < 2) 
	{
		return;
	}
	nShiftCount = 0;
	vector2f vOffset;
	vOffset.x = fCubeOffset * cos(fAngle);
	vOffset.y = fCubeOffset * sin(fAngle);

	point2f p[2]; 

	BOOL bVisible = FALSE;
	//3个点，3个点的有效性前面已经处理完成
	int nStart = 0;
	//找到一个起始点,起始点的条件是前一个点不可见,当前点可见
	BOOL bFirst = TRUE;
	for(int i = 0;i< nPolyCount;i++)
	{
		p[0] = pptPoly[i];
		p[1] = pptPoly[(i + 1)%nPolyCount];
		vector2f v1 = p[0] + vOffset;
		//如果是内侧
		BOOL bOnRight = SYOC_PtOnRight(p[0],p[1],v1);
		if((!bOnRight && ndirect == -1)||(bOnRight && ndirect == 1))
		{
			nStart = (i+1)%nPolyCount;
			bVisible = TRUE;
			break;
		}
		bFirst = FALSE;
	}
	
	
	for(int i = 0; i < nPolyCount; i++)
	{
		int nindex = (i + nStart)%nPolyCount;
		p[0] = pptPoly[nindex];
		p[1] = pptPoly[(nindex+1)%nPolyCount];
		vector2f v1 = p[0] + vOffset;
		BOOL bOnRight = SYOC_PtOnRight(p[0],p[1],v1);
		if((!bOnRight && ndirect == -1)||(bOnRight && ndirect == 1))
		{
			if(!bVisible)
			{
				pptShift[nShiftCount++] = p[0];
				bVisible = TRUE;
			}
			pptShift[nShiftCount++] = v1;						
		}
		else
		{
			if(bVisible)
			{
				pptShift[nShiftCount++] = v1;
				bVisible = FALSE;
			}
			pptShift[nShiftCount++] = p[0];
		}
	}
}//end of SYOC_ShiftData

//线段相交
int _SYOC_LineCross(point2f* ptstart,point2f* ptend,point2f * pptShift ,int nShiftCount,point2f* ptout,int nstart)
{
	vector2f vline1 = *ptend - *ptstart;
	int nmincross = -1;
	for(int i = nShiftCount - 2;i >= nstart ;i --)
	{
		float minx = min(ptstart->x,ptend->x);
		float maxx = max(ptstart->x,ptend->x);
		float miny = min(ptstart->y,ptend->y);
		float maxy = max(ptstart->y,ptend->y);
		if((pptShift[i - 1].x < minx && pptShift[i].x < minx)
			||(pptShift[i - 1].x > maxx && pptShift[i].x > maxx )
			||(pptShift[i - 1].y < miny && pptShift[i].y < miny)
			||(pptShift[i - 1].y > maxy && pptShift[i].y > maxy ))
		{
			continue;
		}

		if( SYOC_PtOnRight(*ptstart,*ptend,pptShift[i]) != SYOC_PtOnRight(*ptstart,*ptend,pptShift[i - 1])
			&&SYOC_PtOnRight(pptShift[i - 1],pptShift[i],*ptstart) != SYOC_PtOnRight(pptShift[i - 1],pptShift[i],*ptend))
		{
			
			vector2f  vline2 = pptShift[i] - pptShift[i - 1];
			float t = (ptstart->x * vline1.y - ptstart->y * vline1.x - pptShift[i - 1].x * vline1.y + pptShift[i - 1].y * vline1.x)/(vline2.x * vline1.y - vline2.y * vline1.x);
			ptout->x = vline2.x * t + pptShift[i - 1].x;
			ptout->y = vline2.y * t + pptShift[i - 1].y;
			if(nmincross < 0)
				nmincross = i;
			else
				nmincross = min(nmincross,i);
		}
	}
	return nmincross;
}

//------------------------------------------------------------------
//功能：偏移得到偏移曲线
//输入：
//输出：
//返回值：
//------------------------------------------------------------------
void SYOC_ShiftData(OUT point2f * pptShift, 
					OUT int &nShiftCount, 
					IN const point2f * pptPoly, 
					IN const int nPolyCount,
					IN float fDis,
				    IN int ndirect)
{
	// 对于每一段
	// 1. 先求AngleBisector i, i+1.
	//    1.1 若二者平行, 直接求Qi, Qi+1. 继续下一段
	//    1.2 否则, 求出当前线段的MCC
	// 2. 判断MCC在PiPi+1左侧还是右侧
	// 3. 求PiPi+1对应点QiQi+1
	// 4. 判断PiPi+1对应点QiQi+1是否超过MCC (即fDis > d, d 为MCC到直线PiPi+1的距离)
	//    4.1 如超过, 则令对应点为MCC. 
	//    4.2 否则, 求出对应点

	float fRight = 0;
	//float fV1,fV2;//Pi+1Pi 及 Pi+1Pi+2 长度
	if(nPolyCount < 2) 
	{
		return;
	}
	point2f *prePt = NULL, *curPt = NULL, *nextPt = NULL;
	point2f p[4]; // p[1], p[2]为当前线段的两个端点
	point2f Q1, Q2; //p[1], p[2]偏移后得到的点
	OffsetNode * pNode = NULL;
	point2f ptstart, ptend;
	vector2f vstep;
	float fD = 0.f;

	//3个点，3个点的有效性前面已经处理完成
	p[0] = pptPoly[nPolyCount-1];
	p[1] = pptPoly[0];
	p[2] = pptPoly[1];
	
	int nStart = 0;
	int nCount = nPolyCount - 1;
	int nmodCount = nPolyCount - 1;
	BOOL bClose = TRUE;
	// 先加起点
	// 如果不闭合
	if ( abs( p[1].x - p[0].x ) + abs (p[1].y - p[0].y) > ABS_FLOAT_0)
	{
		// 非闭合
		//求垂直
		Perpendicular_offset(&ptend,&p[1],p[2] - p[1],fDis,ndirect);

		vstep = p[2] - p[1];
		float fratio = fDis/float2Mode(&vstep);
		vstep *= fratio;
		ptstart.x = p[1].x - vstep.x;
		ptstart.y = p[1].y - vstep.y;

		SYOC_makearc(pptShift, nShiftCount, p[1], ptstart, ptend, fDis);
		nStart = 1;
		bClose = FALSE;
		nmodCount = nPolyCount;
	}
	
	// 另外还需要考虑的是闭合非闭合. 
	for(int i = nStart; i < nCount; i++)
	{
		p[0] = pptPoly[(i-1+ nmodCount)%nmodCount];
		p[1] = pptPoly[i];
		p[2] = pptPoly[(i+1)%nmodCount];
		p[3] = pptPoly[(i+2)%nmodCount];

		// 1. 先求AngleBisector i, i+1.
		// 1.1 如果两个角平分线平行, 直接求对应点
			//添加角平分线
		SYOC_CalShiftPt(p, fDis, ndirect, Q1, Q2 );
		
		// 添加进结果
		////////////////////////////////////////////////////////////////////////// begin
		// 此处将要修改:　如果是角外，　生成圆弧
		// 判断是否角外: 判断Q1与p2是否位于p0p1同侧.不同侧则为角外.
		if ( SYOC_PtOnRight(p[0],p[1],Q1) != SYOC_PtOnRight(p[0],p[1],p[2])) 
		{
			Perpendicular_offset(&ptstart,&p[1],p[1] - p[0],fDis,ndirect);
			if (pptShift)
			{
				pptShift[nShiftCount] = ptstart;
			}
			nShiftCount++;

			Perpendicular_offset(&ptend,&p[1],p[2] - p[1],fDis,ndirect);
			SYOC_makearc(pptShift, nShiftCount, p[1], ptstart, ptend, fDis);
		}
		else
		{
			Perpendicular_offset(&ptstart,&p[1],p[1] - p[0],fDis,ndirect);
			if (pptShift)
			{
				pptShift[nShiftCount] = ptstart;
			}
			nShiftCount++;
			Perpendicular_offset(&ptend,&p[1],p[2] - p[1],fDis,ndirect);
			if (pptShift)
			{
				pptShift[nShiftCount] = ptstart;
			}
			nShiftCount++;
		}
	}
	
	// 再加终点
	if(!bClose)
	{

		p[0] = pptPoly[nPolyCount-2];
		p[1] = pptPoly[nPolyCount-1];
		Perpendicular_offset(&ptstart,&p[1],p[1] - p[0],fDis,ndirect);

		if (pptShift)
		{
			pptShift[nShiftCount] = ptstart;
		}
		nShiftCount++;


		vstep = p[1] - p[0];
		float fratio = fDis/float2Mode(&vstep);
		vstep *= fratio;
		ptend.x = p[1].x + vstep.x;
		ptend.y = p[1].y + vstep.y;

		SYOC_makearc(pptShift, nShiftCount, p[1], ptstart, ptend, fDis);
	}
	else
	{

		if (pptShift)
		{
			point2f ptout;
			int nIndex = _SYOC_LineCross(&pptShift[nShiftCount - 1],&pptShift[0],pptShift,nShiftCount,&ptout,2);
			if(nIndex > 0)
			{
				nShiftCount = nIndex;
				pptShift[nShiftCount++] = ptout;
				pptShift[nShiftCount++] = pptShift[0];
			}
			else
				pptShift[nShiftCount++] = pptShift[0];
		}
		else
		{
			nShiftCount++;
		}

		//if(pptShift)
		//	pptShift[nShiftCount] = pptShift[0];
		//nShiftCount ++;
	}

}//end of SYOC_ShiftData


//------------------------------------------------------------------
//功能：释放vectorgraph
//输入：
//输出：
//------------------------------------------------------------------
void SYOC_ReleaseGraph(vectorgraph* graph)
{
	if (graph == NULL)
	{
		return;
	}
	for (int i=0; i<graph->ncell; i++)
	{
		delete [] graph->cell[i].point;
		delete [] graph->cell[i].pttype;
		memset(&graph->cell[i], 0, sizeof(vectorcell));
	}
	if(graph->ncell&&graph->cell)
	{
		delete [] graph->cell;
		graph->cell = NULL;
	}
	memset(graph, 0, sizeof(vectorgraph));
} // END OF SYOC_ReleaseGraph

//------------------------------------------------------------------
//函数 SYOC_Poly2Fig 说明
//功能：将Poly转化为vectorgraph
//输入：
//输出：
//------------------------------------------------------------------
void SYOC_Poly2Fig(OUT vectorgraph* pGraphOffset, IN COffsetNodeArray *pOffsetArray)
{
	int i, j;
	int nSize = (int)pOffsetArray->GetSize();
	if (nSize<2) 
	{
		return;
	}

	int nCellSize = 0;
	OffsetNode * pOffsetNode = NULL;
	for (i=0; i<nSize; i++)
	{
		pOffsetNode = (OffsetNode *)pOffsetArray->GetAt(i);
		if (pOffsetNode->bStart) 
		{
			nCellSize++;
		}
	}

	if (nCellSize<1) 
	{
		return;
	}

	SYOC_ReleaseGraph(pGraphOffset);
	pGraphOffset->ncell = nCellSize;
	pGraphOffset->cell = new vectorcell[ nCellSize ];
	vectorcell * pCurveCell;
	pGraphOffset->bound.left = 65535.0f;
	pGraphOffset->bound.top = 65535.0f;
	pGraphOffset->bound.right = -65535.0f;
	pGraphOffset->bound.bottom = -65535.0f;

	// 填写Cell->npoint
	int nCellPt = 1;
	j = 0; // 第j个Cell
	for (i=1; i<nSize; i++)
	{
		nCellPt++;
		pOffsetNode = (OffsetNode *)pOffsetArray->GetAt(i);
		if (pOffsetNode->bStart) 
		{
			pCurveCell = &pGraphOffset->cell[j++];
			pCurveCell->npoint = nCellPt-1;
			nCellPt = 1;
		}
	}
	if (i == nSize) 
	{
		pCurveCell = &pGraphOffset->cell[j];
		pCurveCell->npoint = nCellPt;
	}

	int k = 0;//源数据index
	for (i = 0; i<nCellSize; i++) 
	{
		pCurveCell = &pGraphOffset->cell[i];
		pCurveCell->type = close_anticlockwise_region;//uncloss_line;//
		pCurveCell->width = 1; // 暂时统一为1
		pCurveCell->bound.left = 65535.0f;
		pCurveCell->bound.top = 65535.0f;
		pCurveCell->bound.right = -65535.0f;
		pCurveCell->bound.bottom = -65535.0f;
		pCurveCell->point = new point2f[ pCurveCell->npoint ];
		pCurveCell->pttype = new unsigned long[ pCurveCell->npoint ];
		for(j = 0; j<pCurveCell->npoint; j++)
		{
			pOffsetNode = (OffsetNode *)pOffsetArray->GetAt(k++);

			pCurveCell->point[ j ].x = pOffsetNode->pt.x;
			pCurveCell->point[ j ].y = pOffsetNode->pt.y;
			pCurveCell->bound.left = min(pCurveCell->point[ j ].x,pCurveCell->bound.left);
			pCurveCell->bound.top = min(pCurveCell->point[ j ].y,pCurveCell->bound.top);
			pCurveCell->bound.right = max(pCurveCell->point[ j ].x,pCurveCell->bound.right);
			pCurveCell->bound.bottom = max(pCurveCell->point[ j ].y,pCurveCell->bound.bottom);
		}

		for (j = 0; j< pCurveCell->npoint; j++)
		{
			pCurveCell->pttype[j] = point_line;
		}
		pGraphOffset->bound.left = min(pGraphOffset->bound.left,pCurveCell->bound.left);
		pGraphOffset->bound.top = min(pGraphOffset->bound.top,pCurveCell->bound.top);
		pGraphOffset->bound.right = max(pGraphOffset->bound.right,pCurveCell->bound.right);
		pGraphOffset->bound.bottom = max(pGraphOffset->bound.bottom,pCurveCell->bound.bottom);
	}
	// 然后生成各Cell数据
} // END OF SYOC_Poly2Fig

//------------------------------------------------------------------
//函数 SYOffsetCurve 说明：
//功能：输入曲线pGraphIn 向外offset距离fDis后得到的点列
//输入：ptSrcArr: 给定点列。
//输出：ptDesArr: 向外offset距离fDis后得到的点列
//注意！
//------------------------------------------------------------------
BASEGRAPH_DATARENDER_API void SYOffsetCurve(IN vectorgraph* pGraphIn,  // 输入曲线
				   IN float fDis,  // 偏移距离
				   OUT vectorgraph* pGraphOffset // 输出偏移曲线
				  )
{
	if (pGraphIn == NULL || pGraphIn->ncell < 1) 
	{
		return;
	}
	// 提取定点数据，形成CPtNodeArray& ptPolyArray
	int i, j;
	int nSize ;
	vectorcell* pCell = NULL;
	COffsetNodeArray OffsetArray;

	// 判断顺逆
	// 1 ，根据各cell的bound，找出最外环
	float fMaxCellWidth = 0.0f;
	float fCurCellWidth = 0.0f;
	int nOutCell = 0;
	for( i = 0; i < pGraphIn->ncell; i++ )
	{
		pCell = &pGraphIn->cell[i];
		fCurCellWidth = pCell->bound.right - pCell->bound.left;
		if (fMaxCellWidth < fCurCellWidth ) 
		{
			fMaxCellWidth = fCurCellWidth;
			nOutCell = i;
		}
	}
	// 2 ，判断最外环是否逆时针，不是的话，带一个标志位，
	//         在各种处理前，把环路数据（已转化为多边形）反转
	//BOOL bAntiClockwise = SYOC_IsAntiClockwise(&pGraphIn->cell[nOutCell]);

	for (i=0; i<pGraphIn->ncell; i++) 
	{
		// 对于graph中的每一个cell,要先转化成多边形曲线.
		pCell = &pGraphIn->cell[i];
		if (pCell->npoint < 3) 
		{
			continue;
		}

		SYArray ptrPoly;
		SYAR_GetSize(&ptrPoly);
		SYOC_Cell2PtfPoly(pCell, &ptrPoly,NULL,NULL);

		// 去掉重合点
		SYOC_DelOverlap(&ptrPoly,(int*)NULL);
		// 预处理：去掉连续共线点
		SYOC_DelColine(&ptrPoly,(int*)NULL);

		//偏移得到初步偏移曲线
		int nPtCount = (int)SYAR_GetSize(&ptrPoly);
		float fShiftDis = fDis; 
		//if(!bAntiClockwise) // 顺时针 bAntiClockwise
		//{
		//	for(int i = 1; i < nPtCount; i++)
		//	{
		//		point2f* pPt = (point2f*)ptrPoly.GetAt(i);
		//		ptrPoly.RemoveAt(i);
		//		ptrPoly.InsertAt(0, pPt);
		//	}
		//}

		// 先记录开始点
		int nStart = (int)OffsetArray.GetSize();
		// 然后做offset,
		SYOC_ShiftData(&ptrPoly, &OffsetArray, fDis, 1);

		// 反向做一次
		for(int i = 1; i < nPtCount; i++)
		{
			point2f* pPt = (point2f*)SYAR_GetAt(&ptrPoly,i);
			SYAR_RemoveAt(&ptrPoly,i,1);
			SYAR_InsertAt(&ptrPoly,0, pPt,1);
		}

		SYOC_ShiftData(&ptrPoly, &OffsetArray, fDis, 1);

		// 将新加回路的第一个点的bStart设置为TRUE. 
		OffsetNode *pNode = (OffsetNode *)OffsetArray.GetAt(nStart);
		if (pNode) 
		{
			pNode->bStart = TRUE;
		}

		//释放ptrPoly
		nSize = (int)SYAR_GetSize(&ptrPoly);
		for (j=0; j<nSize; j++)
		{
			point2f * pptf = (point2f *)SYAR_GetAt(&ptrPoly,j);
			delete pptf;
			pptf = NULL;
		}
		SYAR_RemoveAll(&ptrPoly);
	} // end of outer-for
	//// 得到的是多边形数组,形成cell,填入到pGraphOffset
	SYOC_Poly2Fig(pGraphOffset, &OffsetArray);
	// 释放
	nSize = (int)OffsetArray.GetSize();
	for (j=0; j<nSize; j++)
	{
		OffsetNode *pOffsetNode = (OffsetNode *)OffsetArray.GetAt(j);
		delete pOffsetNode ;
		pOffsetNode = NULL;
	}
	OffsetArray.RemoveAll();
} // END OF SYOffsetCurve

//------------------------------------------------------------------
//函数 SYOC_Poly2Cell 说明
//功能：将Poly转化为vectorgraph
//输入：
//输出：
//------------------------------------------------------------------
void SYOC_Poly2Cell(OUT vectorcell* pCurveCell, IN COffsetNodeArray *pOffsetArray)
{
	int nSize = (int)pOffsetArray->GetSize();
	if (nSize<2) 
	{
		return;
	}

	int j = 0;
	OffsetNode * pOffsetNode = NULL;

	pCurveCell->type = close_anticlockwise_region;//uncloss_line;//
	pCurveCell->width = 1; // 暂时统一为1
	pCurveCell->bound.left = 65535.0f;
	pCurveCell->bound.top = 65535.0f;
	pCurveCell->bound.right = -65535.0f;
	pCurveCell->bound.bottom = -65535.0f;
	pCurveCell->npoint = (int)pOffsetArray->GetSize();
	pCurveCell->point = new point2f[ pCurveCell->npoint ];
	pCurveCell->pttype = new unsigned long[ pCurveCell->npoint ];
	for(j = 0; j<pCurveCell->npoint; j++)
	{
		pOffsetNode = (OffsetNode *)pOffsetArray->GetAt(j);

		pCurveCell->point[ j ].x = pOffsetNode->pt.x;
		pCurveCell->point[ j ].y = pOffsetNode->pt.y;
		pCurveCell->bound.left = min(pCurveCell->point[ j ].x,pCurveCell->bound.left);
		pCurveCell->bound.top = min(pCurveCell->point[ j ].y,pCurveCell->bound.top);
		pCurveCell->bound.right = max(pCurveCell->point[ j ].x,pCurveCell->bound.right);
		pCurveCell->bound.bottom = max(pCurveCell->point[ j ].y,pCurveCell->bound.bottom);
	}

	for (j = 0; j< pCurveCell->npoint; j++)
	{
		pCurveCell->pttype[j] = point_line;
	}

} // END OF SYOC_Poly2Cell

BASEGRAPH_DATARENDER_API void SYOffsetCellCube(IN vectorcell* pCellIn,  // 输入曲线
									IN vector2f* vOffset,
									IN float fCubOffset,  // 偏移距离
									IN float fAngle,
									OUT vectorcell* pCellOffset, // 输出偏移曲线
									IN const int ndirect
									)
{
	if (pCellIn == NULL || pCellIn->npoint <= 0 || (pCellIn->pttype[0] == point_curve && pCellIn->npoint < 3)) 
	{
		return;
	}
	if(fCubOffset <= 0)
	{
		fCubOffset = 0.1f;
	}
	vectorcell* pCell = pCellIn;
	int nPolySize = 0;

	// 1.1 试算大小
	SYOC_Cell2PtfPoly(pCell, NULL, nPolySize);
	// 1.2 new出数组, 然后生成实际数据
	if (pCellIn->type != uncloss_line)
	{
		//如果是闭合回路, 再加入起始点.
		nPolySize++;
	}
	point2f * pPoly = new point2f[nPolySize];
	memset(pPoly,0,sizeof(point2f) * nPolySize);
	nPolySize = 0;
	SYOC_Cell2PtfPoly(pCell, pPoly, nPolySize);
	// 预处理，删除重复点
	SYOC_DelRedundance(pPoly, nPolySize);

	//如果是非闭合回路最后一个节点等于第一个？
	if (pCellIn->type != uncloss_line)
	{
		//如果是闭合回路, 再加入起始点.
		pPoly[nPolySize] = pPoly[0];
		nPolySize++;
	}

	if(nPolySize < 2)
		return;

	pCellOffset->npoint = nPolySize* 2;
	pCellOffset->point = new point2f[nPolySize* 2];

	SYOC_ShiftCubeData(pCellOffset->point, 
		pCellOffset->npoint, 
		pPoly, 
		nPolySize,
		fCubOffset,
		fAngle,
		ndirect);

	//回灌为Cell
	pCellOffset->type = close_anticlockwise_region;
	pCellOffset->width = 1; // 暂时统一为1
	pCellOffset->bound.left = 65535.0f;
	pCellOffset->bound.top = 65535.0f;
	pCellOffset->bound.right = -65535.0f;
	pCellOffset->bound.bottom = -65535.0f;

	pCellOffset->pttype = new unsigned long[ pCellOffset->npoint ];
	memset(pCellOffset->pttype,0,sizeof(unsigned long) * pCellOffset->npoint);
	int j = 0;
	for(j = 0; j<pCellOffset->npoint; j++)
	{
		if(vOffset)
			pCellOffset->point[ j ] += *vOffset;
		pCellOffset->bound.left = min(pCellOffset->point[ j ].x,pCellOffset->bound.left);
		pCellOffset->bound.top = min(pCellOffset->point[ j ].y,pCellOffset->bound.top);
		pCellOffset->bound.right = max(pCellOffset->point[ j ].x,pCellOffset->bound.right);
		pCellOffset->bound.bottom = max(pCellOffset->point[ j ].y,pCellOffset->bound.bottom);
	}

	for (j = 0; j< pCellOffset->npoint; j++)
	{
		pCellOffset->pttype[j] = point_line;
	}
	delete [] pPoly;
	pPoly = NULL;

}
//找到相互连接点
BASEGRAPH_DATARENDER_API void CorrectFindLink(point2f *pPoly,DWORD * pdwType,int nCount,int nStart)
{
	int nIndex = nStart;
	for(;;)
	{
		while((pdwType[nIndex]&0xff000000))//找出比较点
		{
			nIndex = pdwType[pdwType[nIndex]&0x00ffffff];
			if(nIndex == nStart)
			{
				break;
			}
			nIndex = (nIndex + 1)%nCount;
			if(nIndex == nStart)
				return;
		}
		int nCompareIndex = (nIndex + 1)%nCount;
		for(;;)
		{
			if(nCompareIndex == nIndex)
			{
				break;
			}
			if(pPoly[nCompareIndex].x == pPoly[nIndex].x
				&&pPoly[nCompareIndex].y == pPoly[nIndex].y)
			{
				pdwType[nIndex] = (0x01000000&nCompareIndex);
				pdwType[nCompareIndex] = (0x01000000&nIndex);
				CorrectFindLink(pPoly,pdwType,nCount,nIndex + 1);
				CorrectFindLink(pPoly,pdwType,nCount,nCompareIndex + 1);
				break;
			}
			nCompareIndex  = (nCompareIndex + 1)%nCount;
		}
		nIndex = (nIndex + 1)%nCount;
		if(nIndex == nStart)
			break;
	}
	
}
BASEGRAPH_DATARENDER_API void CorrectPoly(point2f *pPoly,int nCount)
{
	//得到最小点
	int nMinI = 0;
	float fMinX = 9999.0;
	for(int i = 0;i< nCount;i ++)
	{
		if(pPoly[i].x < fMinX)
		{
			fMinX = pPoly[i].x;
			nMinI = i;
		}
	}

	DWORD* dwType = new DWORD[nCount];
	memset(dwType,0,sizeof(DWORD) * nCount);
	CorrectFindLink(pPoly,dwType,nCount,0);
}
//------------------------------------------------------------------
//函数 SYOffsetCell 说明：
//功能：输入曲线pCellIn 向外offset距离fDis后得到的cell
//------------------------------------------------------------------
BASEGRAPH_DATARENDER_API void SYOffsetCell(IN vectorcell* pCellIn,  // 输入曲线
									IN vector2f* voffset,
									 IN float fDis,  // 偏移距离
									 OUT vectorcell* pCellOffset, // 输出偏移曲线
									 IN const int ndirect
									 )
{
	if (pCellIn == NULL || pCellIn->npoint <= 0 || (pCellIn->pttype[0] == point_curve && pCellIn->npoint < 3)) 
	{
		return;
	}
	if(fDis <= 0)
	{
		fDis = 0.1f;
	}
	// 对于graph中的每一个cell,要先转化成多边形曲线.
	vectorcell* pCell = pCellIn;
	int nPolySize = 0;
	CSYPoint2fPtrArray ptrPoly;
	// 1.1 试算大小
	SYOC_Cell2PtfPoly(pCell, NULL, nPolySize);
	// 1.2 new出数组, 然后生成实际数据
	if (pCellIn->type != uncloss_line)
	{
		//如果是闭合回路, 再加入起始点.
		nPolySize++;
	}
	point2f * pPoly = new point2f[nPolySize];
	memset(pPoly,0,sizeof(point2f) * nPolySize);
	nPolySize = 0;
	
	SYOC_Cell2PtfPoly(pCell, pPoly, nPolySize);
	
	// 预处理，删除重复点
	SYOC_DelRedundance(pPoly, nPolySize);

	//如果是非闭合回路最后一个节点等于第一个？
	if (pCellIn->type != uncloss_line)
	{
		//如果是闭合回路, 再加入起始点.
		pPoly[nPolySize] = pPoly[0];
		nPolySize++;
	}

	if(nPolySize < 2)
	{
		SAFEDELETE_ARRAY(pPoly);
		return;
	}

	//偏移得到初步偏移曲线
	float fShiftDis = fDis; 

	// 2.1 首先计算大小
	int nShiftSize = 0;
	SYOC_ShiftData(NULL,nShiftSize, pPoly, nPolySize, fDis, ndirect);

	// 2.2 new出pCellOffset的点数组, 然后再来一次填充好

	pCellOffset->npoint = nShiftSize;
	pCellOffset->point = new point2f[ pCellOffset->npoint ];
	memset(pCellOffset->point, 0, sizeof(point2f) * pCellOffset->npoint);


	nShiftSize = 0;
	SYOC_ShiftData(pCellOffset->point,nShiftSize, pPoly, nPolySize, fDis, ndirect);
	//释放
	SAFEDELETE_ARRAY(pPoly);
	pCellOffset->npoint = nShiftSize;
	
	//回灌为Cell
	pCellOffset->type = close_anticlockwise_region;
	pCellOffset->width = 1; // 暂时统一为1
	pCellOffset->bound.left = 65535.0f;
	pCellOffset->bound.top = 65535.0f;
	pCellOffset->bound.right = -65535.0f;
	pCellOffset->bound.bottom = -65535.0f;
	//pCellOffset->npoint = nShiftSize;
	//pCellOffset->point = new point2f[ pCellOffset->npoint ];
	pCellOffset->pttype = new unsigned long[ pCellOffset->npoint ];
	memset(pCellOffset->pttype,0,sizeof(unsigned long) * pCellOffset->npoint);
	int j = 0;
	for(j = 0; j<pCellOffset->npoint; j++)
	{
		if(voffset)
			pCellOffset->point[ j ] += *voffset;
		pCellOffset->bound.left = min(pCellOffset->point[ j ].x,pCellOffset->bound.left);
		pCellOffset->bound.top = min(pCellOffset->point[ j ].y,pCellOffset->bound.top);
		pCellOffset->bound.right = max(pCellOffset->point[ j ].x,pCellOffset->bound.right);
		pCellOffset->bound.bottom = max(pCellOffset->point[ j ].y,pCellOffset->bound.bottom);
	}

	for (j = 0; j< pCellOffset->npoint; j++)
	{
		pCellOffset->pttype[j] = point_line;
	}

} // END OF SYOffsetCell

void SYOC_AddCurve2PtfPoly(OUT CSYPoint2fPtrArray * pptrPoly, OUT CSYPoint2fPtrArray * pptrPolyNorm, IN point2f * ctrlPoints)
{
	// 接近直线，则直接加入
	// 否则分成两段，递归调用
	point2f *ptfNew;
	if ( SYOC_NearLine(ctrlPoints) )
	{
		ptfNew = new point2f;
		ptfNew->x = ctrlPoints[3].x;
		ptfNew->y = ctrlPoints[3].y;
		pptrPoly->Add(ptfNew);

		// 法线
		ptfNew = new point2f;
		ptfNew->x = ctrlPoints[2].y - ctrlPoints[3].y;
		ptfNew->y = ctrlPoints[3].x - ctrlPoints[2].x;
		pptrPolyNorm->Add(ptfNew);

		ptfNew = new point2f;
		ptfNew->x = ctrlPoints[2].y - ctrlPoints[3].y;
		ptfNew->y = ctrlPoints[3].x - ctrlPoints[2].x;
		pptrPolyNorm->Add(ptfNew);
	}
	else
	{
		point2f ptfFormer[4];
		point2f ptfLatter[4];

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

		SYOC_AddCurve2PtfPoly(pptrPoly, pptrPolyNorm, ptfFormer ) ;
		SYOC_AddCurve2PtfPoly(pptrPoly, pptrPolyNorm, ptfLatter ) ;
	}
} // END OF SYOC_AddCurve2PtfPoly

int SYOC_makearc(CSYPoint2fPtrArray & aptShift, CSYPoint2fPtrArray & aptShiftNorm, CSYDWORDPtrArray &aOrigIndex, point2f ptcenter,point2f ptstart,point2f ptend,float rad, int index)
{
	point2f ptv1 = ptstart - ptcenter;
	if(SYOC_arc_near_line(ptcenter,ptstart,ptend,rad))
	{
		point2f *pptf = new point2f;
		*pptf = ptend;
		aptShift.Add(pptf);

		DWORD* pdw = new DWORD;
		*pdw = index;
		aOrigIndex.Add(pdw);

		// 法线: 非圆弧处, 与原同. 圆弧处, 为径向
		float fD = (ptend.x - ptcenter.x) * (ptend.x - ptcenter.x) + (ptend.y - ptcenter.y) * (ptend.y - ptcenter.y);
		fD = 1/sqrtf(fD);
		point2f ptfTemp;
		ptfTemp.x = (ptend.x - ptcenter.x) * fD;
		ptfTemp.y = (ptend.y - ptcenter.y) * fD;
		pptf = new point2f;
		*pptf = ptfTemp;
		aptShiftNorm.Add(pptf);

		pptf = new point2f;
		*pptf = ptfTemp;
		aptShiftNorm.Add(pptf);

		return 1;
	}

	point2f ptmid = (ptstart + ptend)*0.5f;
	point2f pvector = ptmid - ptcenter;
	if(pvector.x == 0.0f &&pvector.y == 0.0f)
	{
		point2f ptv = (ptend - ptstart) * 0.5f;
		float f = ptv.x;
		ptv.x = -ptv.y;
		ptv.y = f;
		ptmid = ptv + ptcenter;
	}
	else
	{
		float fdisrate = 1.0f/sqrt(pvector.x * pvector.x + pvector.y * pvector.y);
		pvector = pvector * fdisrate;
		pvector *= rad;
		ptmid = pvector + ptcenter;
		if((*(DWORD*)(&ptmid.x)) == 0xffc00000)
		{
			int a = 0;
		}
	}
	SYOC_makearc(aptShift, aptShiftNorm, aOrigIndex, ptcenter,ptstart,ptmid,rad, index);
	SYOC_makearc(aptShift, aptShiftNorm, aOrigIndex, ptcenter,ptmid,ptend,rad, index);
	return 1;
} // END OF SYOC_makearc

//------------------------------------------------------------------
//功能：偏移得到偏移曲线
//输入：
//输出：
//返回值：
//------------------------------------------------------------------
void SYOC_ShiftData(IN SYArray &aptOrig, 
				   IN SYArray &aptOrigNorm, //原始边离散点
				   OUT CSYPoint2fPtrArray &aptShift, //偏移边离散点
				   OUT CSYPoint2fPtrArray &aptShiftNorm, //偏移边离散点
				   OUT CSYDWORDPtrArray &aOrigIndex,
				   IN float fDis,
				   IN int ndirect
				  )
{
	// 对于每一段
	// 1. 先求AngleBisector i, i+1.
	//    1.1 若二者平行, 直接求Qi, Qi+1. 继续下一段
	//    1.2 否则, 求出当前线段的MCC
	// 2. 判断MCC在PiPi+1左侧还是右侧
	// 3. 求PiPi+1对应点QiQi+1
	// 4. 判断PiPi+1对应点QiQi+1是否超过MCC (即fDis > d, d 为MCC到直线PiPi+1的距离)
	//    4.1 如超过, 则令对应点为MCC. 
	//    4.2 否则, 求出对应点

	float fRight = 0;
	//float fV1,fV2;//Pi+1Pi 及 Pi+1Pi+2 长度
	int nPtCount = (int)SYAR_GetSize(&aptOrig);
	if(nPtCount<2) 
	{
		return;
	}
	point2f *prePt = NULL, *curPt = NULL, *nextPt = NULL;
	point2f p[4]; // p[1], p[2]为当前线段的两个端点
	point2f Q1, Q2; //p[1], p[2]偏移后得到的点
	//OffsetNode * pNode = NULL;
	point2f* pptf = NULL;
	point2f* pptfNorm = NULL;

	// 先加起点
	prePt = (point2f*)SYAR_GetAt(&aptOrig,0);
	p[0] = *prePt;
	prePt = (point2f*)SYAR_GetAt(&aptOrig,1);
	p[1] = *prePt;
	float fD = (p[1].x - p[0].x) * (p[1].x - p[0].x) + (p[1].y - p[0].y) * (p[1].y - p[0].y);
	fD = sqrtf(fD);
	pptf = new point2f;
	pptf->x = p[0].x - ndirect * fDis * (p[1].y - p[0].y) / fD;
	pptf->y = p[0].y + ndirect * fDis * (p[1].x - p[0].x) / fD;
	aptShift.Add(pptf);

	DWORD * pdw = new DWORD;
	*pdw = 0;
	aOrigIndex.Add(pdw);

	// 法线: 非圆弧处, 与原同. 圆弧处, 为径向
	pptfNorm = (point2f*)SYAR_GetAt(&aptOrigNorm,0);
	pptf = new point2f;
	*pptf = *pptfNorm;
	aptShiftNorm.Add(pptf);

	pptfNorm = (point2f*)SYAR_GetAt(&aptOrigNorm,1);
	pptf = new point2f;
	*pptf = *pptfNorm;
	aptShiftNorm.Add(pptf);


	// 另外还需要考虑的是闭合非闭合. 
	for(int i=1; i<nPtCount-2; i++)
	{
		prePt = (point2f*)SYAR_GetAt(&aptOrig,i-1);
		p[0] = *prePt;
		prePt = (point2f*)SYAR_GetAt(&aptOrig,i);
		p[1] = *prePt;
		prePt = (point2f*)SYAR_GetAt(&aptOrig,i+1);
		p[2] = *prePt;
		prePt = (point2f*)SYAR_GetAt(&aptOrig,i+2);
		p[3] = *prePt;

		//fRight = (curPt->x - prePt->x)*(nextPt->y - prePt->y) - (curPt->y - prePt->y)*(nextPt->x - prePt->x);
		//if( abs(fRight) < ABS_FLOAT_0)
		//{
		//	continue;
		//}

		// 1. 先求AngleBisector i, i+1.
		// 1.1 如果两个角平分线平行, 直接求对应点
		//if ( SYOC_SameDir(p) ) //其实就是p[i-1]p[i]与p[i+1]p[i+2]平行(有向向量), 也即归一化后点积为1.不能用叉积,否则平行但相反时出错
		//{
			SYOC_CalShiftPt(p, fDis, ndirect, Q1, Q2 );
		//}
		//else
		//{
		//	SYOC_CalShiftPt2(p, fDis, ndirect, Q1, Q2 );
		//}
		
		// 添加进结果
		////////////////////////////////////////////////////////////////////////// begin
		// 此处将要修改:　如果是角外，　生成圆弧
		// 判断是否角外: 判断Q1与p2是否位于p0p1同侧.不同侧则为角外.
		if ( SYOC_PtOnRight(p[0],p[1],Q1)^SYOC_PtOnRight(p[0],p[1],p[2]) ) 
		{
			// 如果是角外, 生成圆弧
			// 1 计算圆弧的起终点
			point2f ptstart, ptend;
			//   1.1 p[0]p[1]在p[1]出偏移得到ptstart
			float fD = (p[1].x - p[0].x) * (p[1].x - p[0].x) + (p[1].y - p[0].y) * (p[1].y - p[0].y);
			fD = sqrtf(fD);
			pptf = new point2f;
			pptf->x = p[1].x - ndirect * fDis * (p[1].y - p[0].y) / fD;
			pptf->y = p[1].y + ndirect * fDis * (p[1].x - p[0].x) / fD;
			aptShift.Add(pptf);

			ptstart = *pptf;

			pdw = new DWORD;
			*pdw = i;
			aOrigIndex.Add(pdw);

			// 法线 与原同
			pptfNorm = (point2f*)SYAR_GetAt(&aptOrigNorm,2*i);
			pptf = new point2f;
			*pptf = *pptfNorm;
			aptShiftNorm.Add(pptf);

			pptfNorm = (point2f*)SYAR_GetAt(&aptOrigNorm,2*i+1);
			pptf = new point2f;
			*pptf = *pptfNorm;
			aptShiftNorm.Add(pptf);

			//   1.2 p[1]p[2]在p[1]出偏移得到ptend
			fD = (p[1].x - p[2].x) * (p[1].x - p[2].x) + (p[1].y - p[2].y) * (p[1].y - p[2].y);
			fD = sqrtf(fD);
			ptend.x = p[1].x - ndirect * fDis * (p[2].y - p[1].y) / fD;
			ptend.y = p[1].y + ndirect * fDis * (p[2].x - p[1].x) / fD;
			// 2 调用SYOC_makearc生成并添加圆弧离散点
			SYOC_makearc(aptShift, aptShiftNorm, aOrigIndex, p[1], ptstart, ptend, fDis, i);
		}
		else
		{
			pptf = new point2f;
			*pptf = Q1;
			aptShift.Add(pptf);

			pdw = new DWORD;
			*pdw = i;
			aOrigIndex.Add(pdw);

			// 法线 与原同
			pptfNorm = (point2f*)SYAR_GetAt(&aptOrigNorm,2*i);
			pptf = new point2f;
			*pptf = *pptfNorm;
			aptShiftNorm.Add(pptf);

			pptfNorm = (point2f*)SYAR_GetAt(&aptOrigNorm,2*i+1);
			pptf = new point2f;
			*pptf = *pptfNorm;
			aptShiftNorm.Add(pptf);
		}
		//////////////////////////////////////////////////////////////////////////  end

		if (i == nPtCount-3) 
		{
			if ( SYOC_PtOnRight(p[1],p[2],Q2)^SYOC_PtOnRight(p[1],p[2],p[3]) ) 
			{
				point2f ptstart, ptend;

				float fD = (p[1].x - p[2].x) * (p[1].x - p[2].x) + (p[1].y - p[2].y) * (p[1].y - p[2].y);
				fD = sqrtf(fD);
				pptf = new point2f;
				pptf->x = p[2].x - ndirect * fDis * (p[2].y - p[1].y) / fD;
				pptf->y = p[2].y + ndirect * fDis * (p[2].x - p[1].x) / fD;
				aptShift.Add(pptf);

				ptstart = *pptf;

				pdw = new DWORD;
				*pdw = i+1;
				aOrigIndex.Add(pdw);

				// 法线 与原同
				pptfNorm = (point2f*)SYAR_GetAt(&aptOrigNorm,2*(i+1));
				pptf = new point2f;
				*pptf = *pptfNorm;
				aptShiftNorm.Add(pptf);

				pptfNorm = (point2f*)SYAR_GetAt(&aptOrigNorm,2*(i+1)+1);
				pptf = new point2f;
				*pptf = *pptfNorm;
				aptShiftNorm.Add(pptf);

				fD = (p[3].x - p[2].x) * (p[3].x - p[2].x) + (p[3].y - p[2].y) * (p[3].y - p[2].y);
				fD = sqrtf(fD);
				ptend.x = p[2].x - ndirect * fDis * (p[3].y - p[2].y) / fD;
				ptend.y = p[2].y + ndirect * fDis * (p[3].x - p[2].x) / fD;

				SYOC_makearc(aptShift, aptShiftNorm, aOrigIndex, p[2], ptstart, ptend, fDis, i+1);
			}
			else
			{
				pptf = new point2f;
				*pptf = Q2;
				aptShift.Add(pptf);

				pdw = new DWORD;
				*pdw = i+1;
				aOrigIndex.Add(pdw);

				// 法线 与原同
				pptfNorm = (point2f*)SYAR_GetAt(&aptOrigNorm,2*(i+1));
				pptf = new point2f;
				*pptf = *pptfNorm;
				aptShiftNorm.Add(pptf);

				pptfNorm = (point2f*)SYAR_GetAt(&aptOrigNorm,2*(i+1)+1);
				pptf = new point2f;
				*pptf = *pptfNorm;
				aptShiftNorm.Add(pptf);
			}
		}
	}
	// 再加终点
	prePt = (point2f*)SYAR_GetAt(&aptOrig,nPtCount-2);
	p[0] = *prePt;
	prePt = (point2f*)SYAR_GetAt(&aptOrig,nPtCount-1);
	p[1] = *prePt;
	fD = (p[1].x - p[0].x) * (p[1].x - p[0].x) + (p[1].y - p[0].y) * (p[1].y - p[0].y);
	fD = sqrtf(fD);
	pptf = new point2f;
	pptf->x = p[1].x - ndirect * fDis * (p[1].y - p[0].y) / fD;
	pptf->y = p[1].y + ndirect * fDis * (p[1].x - p[0].x) / fD;
	aptShift.Add(pptf);

	pdw = new DWORD;
	*pdw = nPtCount-1;
	aOrigIndex.Add(pdw);

	// 法线 与原同
	pptfNorm = (point2f*)SYAR_GetAt(&aptOrigNorm,2*(nPtCount-1));
	pptf = new point2f;
	*pptf = *pptfNorm;
	aptShiftNorm.Add(pptf);

	pptfNorm = (point2f*)SYAR_GetAt(&aptOrigNorm,2*(nPtCount-1)+1);
	pptf = new point2f;
	*pptf = *pptfNorm;
	aptShiftNorm.Add(pptf);
}//end of SYOC_ShiftData

//------------------------------------------------------------------
//函数 SYOffsetCell 说明：
//功能：输入曲线pCellIn 向外offset距离fDis后得到的cell
//------------------------------------------------------------------
BASEGRAPH_DATARENDER_API void SYOffsetCell(IN vectorcell* pCellIn,  // 输入曲线
									IN float fDis,  // 偏移距离
									OUT SYArray &aptOrig, //原始边离散点
									OUT SYArray &aptOrigNorm, //原始边离散点
									OUT CSYPoint2fPtrArray &aptShift, //偏移边离散点
									OUT CSYPoint2fPtrArray &aptShiftNorm, //偏移边离散点
									OUT CSYDWORDPtrArray &aOrigIndex,
									IN const int ndirect
									)
{
	if (pCellIn == NULL || pCellIn->npoint <= 0) 
	{
		return;
	}
	if( (pCellIn->pttype[0] == point_curve_start || pCellIn->pttype[0] == point_curve) && pCellIn->npoint < 3)
	{
		return;
	}

	if(fDis <= 0)
		fDis = 1.0f;
	// 提取定点数据，形成CPtNodeArray& ptPolyArray
	int j;
	int nSize ;
	vectorcell* pCell = NULL;
	COffsetNodeArray OffsetArray;

	// 对于graph中的每一个cell,要先转化成多边形曲线.
	pCell = pCellIn;
	SYOC_Cell2PtfPoly(pCell, &aptOrig,&aptOrigNorm,NULL);
//	SYOC_Cell2PtfPoly(pCell, &aptOrig, &aptOrigNorm);

	// 去掉重合点
	SYOC_DelOverlap(&aptOrig, &aptOrigNorm);

	nSize = (int)SYAR_GetSize(&aptOrig);
	if(nSize < 2)
		return;
	// 预处理：去掉连续共线点
	SYOC_DelColine(&aptOrig, &aptOrigNorm);
	nSize = (int)SYAR_GetSize(&aptOrig);
	if(nSize < 2)
		return;

	nSize = (int)SYAR_GetSize(&aptOrig);

	if (nSize < 2) 
	{
		return;
	}

	if (pCellIn->type != uncloss_line) 
	{
		//如果是闭合回路, 再加入起始点. 或者最后一段曲线?
		point2f * ppt0 = (point2f *)SYAR_GetAt(&aptOrig,0);
		point2f * ppt1 = (point2f *)SYAR_GetAt(&aptOrig,nSize - 1);

		if ( abs(ppt0->x - ppt1->x) < ABS_FLOAT_0 && abs(ppt0->y - ppt1->y) < ABS_FLOAT_0 ) 
		{
		}
		else
		{
			point2f *pptTemp = new point2f;
			*pptTemp = *ppt0;
			SYAR_Add(&aptOrig,pptTemp);

			pptTemp = new point2f;
			pptTemp->x = ppt0->y - ppt1->y;
			pptTemp->y = ppt1->x - ppt0->x;
			SYAR_Add(&aptOrigNorm,pptTemp);
			
			pptTemp = new point2f;
			pptTemp->x = ppt0->y - ppt1->y;
			pptTemp->y = ppt1->x - ppt0->x;
			SYAR_Add(&aptOrigNorm,pptTemp);

			// 记得三角化时干掉
		}
	}

	//偏移得到初步偏移曲线
	int nPtCount = (int)SYAR_GetSize(&aptOrig);
	float fShiftDis = fDis; 

	// 先记录开始点
	int nStart = (int)OffsetArray.GetSize();
	// 然后做offset,
	//SYOC_ShiftData(&aptOrig, &OffsetArray, fDis, ndirect);
	SYOC_ShiftData(aptOrig, aptOrigNorm, aptShift, aptShiftNorm, aOrigIndex, fDis, ndirect);

	nSize = (int)OffsetArray.GetSize();
	if (nSize < 2) 
	{
		return;
	}
	// 将新加回路的第一个点的bStart设置为TRUE. 
	OffsetNode *pNode = (OffsetNode *)OffsetArray.GetAt(nStart);
	if (pNode) 
	{
		pNode->bStart = TRUE;
	}

	//// 得到的是多边形数组,形成cell,填入到pGraphOffset
	//SYOC_Poly2Cell(pCellOffset, &OffsetArray);
	
	// 释放
	//释放ptrPoly
	//nSize = (int)ptrPoly.GetSize();
	//for (j=0; j<nSize; j++)
	//{
	//	point2f * pptf = (point2f *)ptrPoly.GetAt(j);
	//	delete pptf;
	//	pptf = NULL;
	//}
	//ptrPoly.RemoveAll();

	nSize = (int)OffsetArray.GetSize();
	for (j=0; j<nSize; j++)
	{
		OffsetNode *pOffsetNode = (OffsetNode *)OffsetArray.GetAt(j);
		delete pOffsetNode ;
		pOffsetNode = NULL;
	}
	OffsetArray.RemoveAll();
}
BASEGRAPH_DATARENDER_API BOOL SYReleaseOffsetCell(vectorcell* pcell)
{
	delete [] pcell->point;
	delete [] pcell->pttype;
	return 1;
}

BASEGRAPH_DATARENDER_API void SYOC_CloseNearLine()
{
	g_nNearLineClosed = TRUE;
}

BASEGRAPH_DATARENDER_API void SYOC_OpenNearLine()
{
	g_nNearLineClosed = FALSE;
}
