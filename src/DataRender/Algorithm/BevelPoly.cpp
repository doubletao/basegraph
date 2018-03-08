#include "StdAfx.h"
#include "DataRender\BevelPoly.h"
#include "DataRender\OffsetCurve.h"

const float g_fScaleAAA = 1.01f;

SYBevelPoly::SYBevelPoly(void)
{
}

SYBevelPoly::~SYBevelPoly(void)
{
}

void SYBevelPoly::BevelPolyAPI(IN vectorcell * pCurveCell, IN float fWidth, IN float fHeight, OUT point3f* &ptf3DRet, OUT int &nTriPtSize, IN BOOL bAntiClockwise ) 
{
	if (pCurveCell->type == uncloss_line || pCurveCell->npoint < 3 ) 
	{
		return;
	}
	SYArray ptrPoly;
	SYArray ptrPolyNorm;
	SYAR_Init(&ptrPoly);
	SYAR_Init(&ptrPolyNorm);
	SYOC_Cell2PtfPoly(pCurveCell, &ptrPoly, &ptrPolyNorm,NULL);
	int nSize = (int)SYAR_GetSize(&ptrPoly);
	if (nSize<3) 
	{
		return;
	}
	int i;

	if (!bAntiClockwise) 
	{
		// 反向
		point2f ptfTemp;
		point2f* pptf1 ;
		point2f* pptf2 ;
		for (i=0; i<nSize/2; i++)
		{
			pptf1 = (point2f*)SYAR_GetAt(&ptrPoly,i);
			pptf2 = (point2f*)SYAR_GetAt(&ptrPoly,nSize - 1 -i);
			ptfTemp.x = pptf1->x;
			ptfTemp.y = pptf1->y;

			pptf1->x = pptf2->x;
			pptf1->y = pptf2->y;

			pptf2->x = ptfTemp.x;
			pptf2->y = ptfTemp.y;
		}
		pptf2 = (point2f*)SYAR_RemoveAt(&ptrPoly,nSize - 1,1);
		//pptf2 = (point2f*)SYAR_GetAt(&ptrPoly,nSize - 1);
		SYAR_InsertAt(&ptrPoly,0,pptf2,1);
//		point2f* pRemovePt = (point2f*)SYAR_GetAt(&ptrPoly,nSize);
//		SAFE_DELETE(pRemovePt);
//		SYAR_RemoveAt(&ptrPoly,nSize,1);

		int nNormalSize = SYAR_GetSize(&ptrPolyNorm);
		for (i=0; i<nNormalSize/2; i++)
		{
			pptf1 = (point2f*)SYAR_GetAt(&ptrPolyNorm,i);
			pptf2 = (point2f*)SYAR_GetAt(&ptrPolyNorm,nNormalSize - 1 -i);
			ptfTemp = *pptf1;
			*pptf1 = *pptf2;
			*pptf1 *= -1;
			*pptf2 = ptfTemp;
			*pptf2 *= -1;
		}
	}

	
	CSYPoint3fPtrArray ptrArrRude4; 
	CSYPoint3fPtrArray ptrArrRude3; 
	//CPtrArray ptrArrBox4; // 面片投影的矩形包围盒
	//CPtrArray ptrArrBox3; // 面片投影的矩形包围盒
	CSYIndex3Array ptrArrIndex3; //三角形面片的指示信息

	CSYPoint3fPtrArray ptrTri;

//#ifdef _DEBUG
//		LARGE_INTEGER t_freqency;
//		LARGE_INTEGER t_start;
//		
//		LARGE_INTEGER t_CutEnd;
//		LARGE_INTEGER t_GenerateRudeBevelEnd;
//		QueryPerformanceFrequency(&t_freqency);
//		QueryPerformanceCounter(&t_start);
//#endif // _DEBUG

	GenerateRudeBevel4(&ptrPoly, fWidth, fHeight, &ptrArrRude4/*, &ptrArrBox4*/);
	GenerateRudeBevel3(&ptrPoly, fWidth, fHeight, &ptrArrRude3, /*&ptrArrBox3, */&ptrArrIndex3);

//	QueryPerformanceCounter(&t_GenerateRudeBevelEnd);

	CutToTri4(&ptrPoly, &ptrPolyNorm, &ptrArrRude4, /*&ptrArrBox4, */&ptrArrRude3, /*&ptrArrBox3, */&ptrArrIndex3, fWidth, fHeight, &ptrTri);
	CutToTri3(&ptrPoly, &ptrPolyNorm, &ptrArrRude4, /*&ptrArrBox4, */&ptrArrRude3, /*&ptrArrBox3, */&ptrArrIndex3, fWidth, fHeight, &ptrTri);

//#ifdef _DEBUG
//		QueryPerformanceCounter(&t_CutEnd);
//		double rudemillisecond = (t_GenerateRudeBevelEnd.QuadPart - t_start.QuadPart)*double(1000)/(t_freqency.QuadPart);
//		double cutmillisecond = (t_CutEnd.QuadPart - t_start.QuadPart)*double(1000)/(t_freqency.QuadPart);
//		
//		printf("SYBevelPoly: 粗切: %.3gms, 细切: %.3gms\n",rudemillisecond,cutmillisecond);
//#endif // _DEBUG

	//转换成数组
	nSize = (int)ptrTri.GetSize();
	nTriPtSize = nSize;
	delete []ptf3DRet;
	ptf3DRet = new point3f[nSize];
	for(i=0; i<nSize; i++)
	{
		point3f* pptf3d = (point3f*)ptrTri.GetAt(i);
		ptf3DRet[i].x = pptf3d->x;
		ptf3DRet[i].y = pptf3d->y;
		ptf3DRet[i].z = pptf3d->z;
	}

	//释放
	nSize = (int)ptrArrRude4.GetSize();
	for (i=0; i<nSize; i++)
	{
		point3f* pptf3d = (point3f*)ptrArrRude4.GetAt(i);
		delete pptf3d;
		pptf3d = NULL;
	}
	ptrArrRude4.RemoveAll();

	nSize = (int)ptrArrRude3.GetSize();
	for (i=0; i<nSize; i++)
	{
		point3f* pptf3d = (point3f*)ptrArrRude3.GetAt(i);
		delete pptf3d;
		pptf3d = NULL;
	}
	ptrArrRude3.RemoveAll();

	nSize = (int)ptrArrIndex3.GetSize();
	for (i=0; i<nSize; i++)
	{
		Index3 *pIndex3 = (Index3 *)ptrArrIndex3.GetAt(i);
		delete pIndex3;
		pIndex3 = NULL;
	}
	ptrArrIndex3.RemoveAll();

	nSize = (int)ptrTri.GetSize();
	for (i=0; i<nSize; i++)
	{
		point3f* pptf3d = (point3f*)ptrTri.GetAt(i);
		delete pptf3d;
		pptf3d = NULL;
	}
	ptrTri.RemoveAll();

	nSize = (int)SYAR_GetSize(&ptrPoly);
	for (i=0; i<nSize; i++)
	{
		pointf * pptf = (pointf * )SYAR_GetAt(&ptrPoly,i);
		delete pptf;
		pptf = NULL;
	}
	SYAR_RemoveAll(&ptrPoly);

	nSize = (int)SYAR_GetSize(&ptrPolyNorm);
	for (i=0; i<nSize; i++)
	{
		pointf * pptf = (pointf * )SYAR_GetAt(&ptrPolyNorm,i);
		delete pptf;
		pptf = NULL;
	}
	SYAR_RemoveAll(&ptrPolyNorm);

} // END OF BevelPolyAPI 

//------------------------------------------------------------------
//函数 NearLine 说明
//功能：三次Bezier曲线控制顶点接近一条线段
//输入：
//输出：
//------------------------------------------------------------------
BOOL SYBevelPoly::NearLine(IN pointf* ptf ) 
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
void SYBevelPoly::AddCurve2PtfPoly(OUT CSYPoint2fPtrArray * pptrPoly, OUT CSYPoint2fPtrArray * pptrPolyNorm, IN pointf * ctrlPoints)
{
	// 接近直线，则直接加入
	// 否则分成两段，递归调用
	pointf *ptfNew;
	if ( NearLine(ctrlPoints) )
	{
		pointf ptNormal;
		for(int i = 2;i >= 0;i --)
		{
			ptNormal.x = ctrlPoints[i].y - ctrlPoints[3].y;
			ptNormal.y = ctrlPoints[3].x - ctrlPoints[i].x;

			if (abs(ptNormal.x) > ABS_FLOAT_0 || abs(ptNormal.y) > ABS_FLOAT_0)
			{
				ptfNew = new pointf;
				*ptfNew = ctrlPoints[3];
				pptrPoly->Add(ptfNew);

				ptfNew = new pointf;
				*ptfNew = ptNormal;
				pptrPolyNorm->Add(ptfNew);
				ptfNew = new pointf;
				*ptfNew = ptNormal;
				pptrPolyNorm->Add(ptfNew);
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

		AddCurve2PtfPoly(pptrPoly, pptrPolyNorm, ptfFormer ) ;
		AddCurve2PtfPoly(pptrPoly, pptrPolyNorm, ptfLatter ) ;
	}
} // END OF AddCurve2PtfPoly

void SYBevelPoly::GenerateRudeBevel4(IN SYArray * pptrPoly, IN float fWidth, IN float fHeight, 
								 OUT CSYPoint3fPtrArray * pptrArrRude4/*, OUT CPtrArray * pptrArrBox4*/ )
{
	int nSize = (int)SYAR_GetSize(pptrPoly);

	pointf* ppt0 = NULL;
	pointf* ppt1 = NULL;
	pointf ptfFormer0, ptfFormer1;
	pointf ptf, ptf0, ptf1;
	point3f* pptf3d = NULL;
	rectf * pRectf = NULL;
	float r;

	for (int i=0; i<nSize; i++)
	{
		ppt0 = (pointf*)SYAR_GetAt(pptrPoly,i);
		if (i<nSize-1) 
		{
			ppt1 = (pointf*)SYAR_GetAt(pptrPoly,i+1);
		}
		else
		{
			ppt1 = (pointf*)SYAR_GetAt(pptrPoly,0);
		}		

		r = 1.f / sqrt( (float)( (ppt1->x - ppt0->x)*(ppt1->x - ppt0->x) + (ppt0->y - ppt1->y)*(ppt0->y - ppt1->y) ) );
		ptf.x = -fWidth * (ppt0->y - ppt1->y) * r;
		ptf.y = -fWidth * (ppt1->x - ppt0->x) * r;

		ptf0.x = ppt0->x - ptf.x;
		ptf0.y = ppt0->y - ptf.y;

		ptf1.x = ppt1->x - ptf.x;
		ptf1.y = ppt1->y - ptf.y;

		ptfFormer0.x = (float)ppt0->x;
		ptfFormer0.y = (float)ppt0->y;
		ptfFormer1.x = (float)ppt1->x;
		ptfFormer1.y = (float)ppt1->y;

		//// 投影的矩形外包
		//pRectf = new rectf;
		//GetRectEnvelop(ptfFormer0, ptfFormer1, ptf0, ptf1, pRectf);

		//加入
		pptf3d = new point3f;
		pptf3d->x = ptfFormer1.x;
		pptf3d->z = 0.0f;
		pptf3d->y = ptfFormer1.y;
		pptrArrRude4->Add(pptf3d);

		pptf3d = new point3f;
		pptf3d->x = ptf1.x;
		pptf3d->z = -fHeight;
		pptf3d->y = ptf1.y;
		pptrArrRude4->Add(pptf3d);

		pptf3d = new point3f;
		pptf3d->x = ptf0.x;
		pptf3d->z = -fHeight;
		pptf3d->y = ptf0.y;
		pptrArrRude4->Add(pptf3d);

		pptf3d = new point3f;
		pptf3d->x = ptfFormer0.x;
		pptf3d->z = 0.0f;
		pptf3d->y = ptfFormer0.y;
		pptrArrRude4->Add(pptf3d);
	}
} // END OF GenerateRudeBevel4

void SYBevelPoly::GenerateRudeBevel3(IN SYArray * pptrPoly, IN float fWidth, IN float fHeight, 
								 OUT CSYPoint3fPtrArray * pptrArrRude3, /*OUT CPtrArray * pptrArrBox3, */OUT CSYIndex3Array *pptrArrIndex3 )
{
	int nSize = (int)SYAR_GetSize(pptrPoly);
	pointf *prePt = NULL;
	pointf *curPt = NULL;
	pointf *nextPt = NULL;
	point3f* pptf3d = NULL;
	Index3 * pIndex3 = NULL;
	int nCount = 0;
	for (int i=0; i<nSize; i++)
	{
		if (i == 0) 
		{
			prePt = (pointf*)SYAR_GetAt(pptrPoly,nSize-1);
			curPt = (pointf*)SYAR_GetAt(pptrPoly,0);
			nextPt = (pointf*)SYAR_GetAt(pptrPoly,1);
		}
		else if(i == nSize-1)
		{
			prePt = (pointf*)SYAR_GetAt(pptrPoly,nSize-2);
			curPt = (pointf*)SYAR_GetAt(pptrPoly,nSize-1);
			nextPt = (pointf*)SYAR_GetAt(pptrPoly,0);
		}
		else
		{
			prePt = (pointf*)SYAR_GetAt(pptrPoly,i-1);
			curPt = (pointf*)SYAR_GetAt(pptrPoly,i);
			nextPt = (pointf*)SYAR_GetAt(pptrPoly,i+1);
		}

		float fRight = (curPt->x - prePt->x)*(nextPt->y - prePt->y) - (curPt->y - prePt->y)*(nextPt->x - prePt->x);
		if ( fRight<=0 ) 
		{
			pptf3d = new point3f;
			pptf3d->x = (float)curPt->x;
			pptf3d->z = 0.0f;
			pptf3d->y = (float)curPt->y;
			pptrArrRude3->Add(pptf3d);

			pointf ptfCenter; 
			ptfCenter.x = (float)curPt->x;
			ptfCenter.y = (float)curPt->y;

			pointf ptfStartPt;
			float r = 1.f / sqrt( (float)( (curPt->x - prePt->x)*(curPt->x - prePt->x) + (prePt->y - curPt->y)*(prePt->y - curPt->y) ) );
			pointf ptf;
			ptf.x = fWidth * (prePt->y - curPt->y) * r;
			ptf.y = fWidth * (curPt->x - prePt->x) * r;

			ptfStartPt.x = curPt->x + ptf.x;
			ptfStartPt.y = curPt->y + ptf.y;

			pointf ptfEndPt;  
			r = 1.f / sqrt( (float)( (nextPt->x - curPt->x)*(nextPt->x - curPt->x) + (curPt->y - nextPt->y)*(curPt->y - nextPt->y) ) );
			ptf.x = fWidth * (curPt->y - nextPt->y) * r;
			ptf.y = fWidth * (nextPt->x - curPt->x) * r;

			ptfEndPt.x = curPt->x + ptf.x;
			ptfEndPt.y = curPt->y + ptf.y;
			pptf3d = new point3f;
			pptf3d->x = ptfStartPt.x;
			pptf3d->z = -fHeight;
			pptf3d->y = ptfStartPt.y;
			pptrArrRude3->Add(pptf3d);
			MakeASlope(pptrArrRude3, &ptfCenter, &ptfStartPt, &ptfEndPt, fWidth, fHeight, 1);

			// 顶点序号
			pIndex3 = new Index3;
			pIndex3->nPolyIndex = i;
			pIndex3->nStartIndex = nCount;
			nCount = (int)pptrArrRude3->GetSize();
			pIndex3->nNumCounts = nCount - pIndex3->nStartIndex;
			pptrArrIndex3->Add(pIndex3);
		} // end of if ( fRight<=0 ) 
	}// end of for-loop 
} // END OF GenerateRudeBevel3

////////////////////////////////////////////////////////////////////////////
//// 三个pointf型顶点的外包矩形
//void SYBevelPoly::GetRectEnvelop(IN pointf ptf0, IN pointf ptf1, IN pointf ptf2, rectf * pRectf )
//{
//	pRectf->left = ptf0.x;
//	pRectf->right = ptf0.x;
//	pRectf->top = ptf0.y;
//	pRectf->bottom = ptf0.y;
//
//	//1
//	if (ptf1.x < pRectf->left) 
//	{
//		pRectf->left = ptf1.x;
//	}
//	if (ptf1.x > pRectf->right) 
//	{
//		pRectf->right = ptf1.x;
//	}
//	if (ptf1.y < pRectf->top) 
//	{
//		pRectf->top = ptf1.y;
//	}
//	if (ptf1.y > pRectf->bottom) 
//	{
//		pRectf->bottom = ptf1.y;
//	}
//
//	//2
//	if (ptf2.x < pRectf->left) 
//	{
//		pRectf->left = ptf2.x;
//	}
//	if (ptf2.x > pRectf->right) 
//	{
//		pRectf->right = ptf2.x;
//	}
//	if (ptf2.y < pRectf->top) 
//	{
//		pRectf->top = ptf2.y;
//	}
//	if (ptf2.y > pRectf->bottom) 
//	{
//		pRectf->bottom = ptf2.y;
//	}
//} // END OF GetRectEnvelop
//
//------------------------------------------------------------------
//功能：制作斜边
//输入：
//输出：pt2
//注意！鉴于只是供本程序内部使用时的特点,程序中没有考虑没有交点和两个交点时的情况
//------------------------------------------------------------------
int SYBevelPoly::MakeASlope(OUT CSYPoint3fPtrArray *pptArr,
						IN pointf* pCenter,
						IN pointf* pStartPt,
						IN pointf* pEndPt,  
						IN float fDis,      
						IN float fHeight, 
						IN int nDirect 
						)
{
	pointf ptMid,ptCm;
	float fSdis = (pStartPt->x - pEndPt->x)*(pStartPt->x - pEndPt->x) + (pStartPt->y - pEndPt->y)*(pStartPt->y - pEndPt->y);
	if(fSdis>0.0025f*g_fScaleAAA)
	{
		ptMid.x = (pEndPt->x+pStartPt->x)*0.5f;
		ptMid.y = (pEndPt->y+pStartPt->y)*0.5f;

		float fDeltX = pEndPt->x-pStartPt->x;
		float fDeltY = pEndPt->y-pStartPt->y;

		float fDisC = sqrt( (ptMid.x - pCenter->x)*(ptMid.x - pCenter->x) +(ptMid.y - pCenter->y)*(ptMid.y - pCenter->y) );
		float fdDisC = fDis/fDisC;

		if(fDisC > 0.000001*g_fScaleAAA)
		{
			ptCm.x = pCenter->x+(ptMid.x-pCenter->x)*fdDisC;
			ptCm.y = pCenter->y+(ptMid.y-pCenter->y)*fdDisC;
		}
		else
		{
			ptCm.x = pCenter->x+(-1*nDirect)*(pStartPt->y-pEndPt->y)*0.5f;
			ptCm.y = pCenter->y+(nDirect)*(pStartPt->x-pEndPt->x)*0.5f;
		}

		if(fabs(fDis-fDisC)>0.2f*g_fScaleAAA)
		{
			MakeASlope(pptArr,pCenter, pStartPt,&ptCm, fDis, fHeight, nDirect );
			MakeASlope(pptArr,pCenter, &ptCm, pEndPt, fDis, fHeight, nDirect );
			return 1;
		}
	}
	point3f* pPtf3D = new point3f;
	pPtf3D->x = pEndPt->x;
	pPtf3D->z = -fHeight;
	pPtf3D->y = pEndPt->y;
	pptArr->Add(pPtf3D);
	return 1;
} // END OF MakeASlope

//////////////////////////////////////////////////////////////////////////
void SYBevelPoly::CutUnSeenByOne(IN point3f* ptfFish, IN point3f* ptfKnife, IN int nFishSize, IN int nKnifeSize,
							 IN OUT CSYPoint2fPtrArray * pptrCutLineSegArr)
{
	int i;
	pointf* ptfFishProj = new pointf[nFishSize];
	for(i=0; i<nFishSize; i++)
	{
		ptfFishProj[i].x = ptfFish[i].x;
		ptfFishProj[i].y = ptfFish[i].y;
	}
	rectf rt1;
	GetEnvelop(ptfFishProj, nFishSize, rt1);
	pointf* ptfKnifeProj = new pointf[nKnifeSize];
	for(i=0; i<nKnifeSize; i++)
	{
		ptfKnifeProj[i].x = ptfKnife[i].x;
		ptfKnifeProj[i].y = ptfKnife[i].y;
	}
	rectf rt2;
	GetEnvelop(ptfKnifeProj, nKnifeSize, rt2);
	if(!RectOverlap(rt1, rt2))
	{
		delete []ptfFishProj;
		ptfFishProj = NULL;
		delete []ptfKnifeProj ;
		ptfKnifeProj = NULL;
		return;
	}

	float fA, fB, fC;
	ProjCrossLine(ptfFish[0], ptfFish[1], ptfFish[2], 
		ptfKnife[0], ptfKnife[1], ptfKnife[2],
		fA, fB, fC);

	pointf ptfS, ptfE ;
	pointf ptfInS, ptfInE ;
	if( CutLine(fA, fB, fC,
		ptfKnifeProj, nKnifeSize,
		ptfS, ptfE )	
		)
	{
		if( CutLine(fA, fB, fC,
			ptfFishProj, nFishSize,
			ptfInS, ptfInE )
			)
		{
			//
			pointf ptfM, ptfInM;
			ptfM.x = (ptfS.x + ptfE.x) * 0.5f;
			ptfM.y = (ptfS.y + ptfE.y) * 0.5f;
			ptfInM.x = (ptfInS.x + ptfInE.x) * 0.5f;
			ptfInM.y = (ptfInS.y + ptfInE.y) * 0.5f;
			float r = sqrt((float)((ptfM.x -ptfInM.x)*(ptfM.x -ptfInM.x) + (ptfM.y -ptfInM.y)*(ptfM.y -ptfInM.y)));
			float r1 = sqrt((float)((ptfE.x -ptfS.x)*(ptfE.x -ptfS.x) + (ptfE.y -ptfS.y)*(ptfE.y -ptfS.y)));
			float r2 = sqrt((float)((ptfInE.x -ptfInS.x)*(ptfInE.x -ptfInS.x) + (ptfInE.y -ptfInS.y)*(ptfInE.y -ptfInS.y)));
			if (2 * r < (r1 + r2) ) 
			{
				float fMaxX, fMinX;
				float fMaxY, fMinY;
				pointf ptfTemp;
				GetMinMaxXY(fMinX, fMaxX, fMinY, fMaxY, ptfInS, ptfInE, ptfS, ptfE);
				if (fMaxX - fMinX > fMaxY - fMinY) 
				{
					if (ptfInE.x < ptfInS.x) 
					{
						ptfTemp.x = ptfInS.x;
						ptfTemp.y = ptfInS.y;
						ptfInS.x = ptfInE.x;
						ptfInS.y = ptfInE.y;
						ptfInE.x = ptfTemp.x;
						ptfInE.y = ptfTemp.y;
					}
					if (ptfE.x < ptfS.x) 
					{
						ptfTemp.x = ptfS.x;
						ptfTemp.y = ptfS.y;
						ptfS.x = ptfE.x;
						ptfS.y = ptfE.y;
						ptfE.x = ptfTemp.x;
						ptfE.y = ptfTemp.y;
					}
					if (ptfS.x > ptfInS.x) 
					{
						ptfInS.x = ptfS.x;
						ptfInS.y = ptfS.y;
					}
					if (ptfE.x < ptfInE.x) 
					{
						ptfInE.x = ptfE.x;
						ptfInE.y = ptfE.y;
					}
				}
				else 
				{
					if (ptfInE.y < ptfInS.y) 
					{
						ptfTemp.x = ptfInS.x;
						ptfTemp.y = ptfInS.y;
						ptfInS.x = ptfInE.x;
						ptfInS.y = ptfInE.y;
						ptfInE.x = ptfTemp.x;
						ptfInE.y = ptfTemp.y;
					}
					if (ptfE.y < ptfS.y) 
					{
						ptfTemp.x = ptfS.x;
						ptfTemp.y = ptfS.y;
						ptfS.x = ptfE.x;
						ptfS.y = ptfE.y;
						ptfE.x = ptfTemp.x;
						ptfE.y = ptfTemp.y;
					}
					if (ptfS.y > ptfInS.y) 
					{
						ptfInS.x = ptfS.x;
						ptfInS.y = ptfS.y;
					}
					if (ptfE.y < ptfInE.y) 
					{
						ptfInE.x = ptfE.x;
						ptfInE.y = ptfE.y;
					}
				}
				////选择大的S 和小的E
				//float r3 = sqrt((float)((ptfE.x -ptfInS.x)*(ptfE.x -ptfInS.x) + (ptfE.y -ptfInS.y)*(ptfE.y -ptfInS.y)));
				//if (r3 > r1) // 选择ptfS
				//{
				//	ptfInS.x = ptfS.x;
				//	ptfInS.y = ptfS.y;

				//	r2 = sqrt((float)((ptfInE.x -ptfInS.x)*(ptfInE.x -ptfInS.x) + (ptfInE.y -ptfInS.y)*(ptfInE.y -ptfInS.y)));
				//	r3 = sqrt((float)((ptfE.x -ptfInS.x)*(ptfE.x -ptfInS.x) + (ptfE.y -ptfInS.y)*(ptfE.y -ptfInS.y)));
				//}
				////然后看哪个终点更近
				//if (r2>r3) { // 选择ptfE
				//	ptfInE.x = ptfE.x;
				//	ptfInE.y = ptfE.y;
				//}
				pointf * pptf = new pointf;
				pptf->x = ptfInS.x;
				pptf->y = ptfInS.y;
				pptrCutLineSegArr->Add(pptf);
				pptf = new pointf;
				pptf->x = ptfInE.x;
				pptf->y = ptfInE.y;
				pptrCutLineSegArr->Add(pptf);
			}
		}
		//////////////////////////////////////////////////////////////////////////
	}
	delete []ptfFishProj;
	delete []ptfKnifeProj ;
	ptfFishProj = NULL;
	ptfKnifeProj = NULL;

} // END OF CutUnSeenByOne

//////////////////////////////////////////////////////////////////////////
void SYBevelPoly::CutToTri4(IN SYArray * pptrPoly, IN SYArray * pptrPolyNorm, IN  CSYPoint3fPtrArray * pptrArrRude4, /*IN CPtrArray * pptrArrBox4,*/
						IN CSYPoint3fPtrArray * pptrArrRude3, /*IN CPtrArray * pptrArrBox3, */IN CSYIndex3Array *pptrArrIndex3,
						IN float fWidth, IN float fHeight, OUT CSYPoint3fPtrArray * pptrTri ) 
{
	int i, j;
	//int k;
	CSYPoint2fPtrArray ptrCutLineSegArr;
	CSYIntersectNodeArray ptrIntersectPtArr;
	point3f ptfFish[4];
	//point3f ptfKnife[4]; 
	int nFishSize;
	int nKnifeSize;
	point3f* pptf3d;
	pointf* ppt0 = NULL;
	pointf* ppt1 = NULL;
	pointf ptfFormer0, ptfFormer1;
	int nSize = (int)pptrArrRude4->GetSize() / 4; 
	for (i=0; i<nSize; i++) 
	{
		nFishSize = 4;
		nKnifeSize = 4;
		for(j=0; j<4; j++)
		{
			pptf3d = (point3f*)pptrArrRude4->GetAt(4*i + j);
			ptfFish[j].x = pptf3d->x;
			ptfFish[j].y = pptf3d->y;
			ptfFish[j].z = pptf3d->z;
		}

		///////////////////////////////////////////////////////////////////////////////////////////////// begin
		// 初步提速注释掉
		//for(j=0; j<nSize; j++)
		//{
		//	if (i == j) 
		//	{
		//		continue;
		//	}
		//	for(k=0; k<4; k++)
		//	{
		//		pptf3d = (point3f*)pptrArrRude4->GetAt(4*j + k);
		//		ptfKnife[k].x = pptf3d->x;
		//		ptfKnife[k].y = pptf3d->y;
		//		ptfKnife[k].z = pptf3d->z;
		//	}
		//	CutUnSeenByOne(ptfFish, ptfKnife, nFishSize, nKnifeSize, &ptrCutLineSegArr);
		//} // end of for 

		//nKnifeSize = 3;
		//int nConeSize = (int)pptrArrIndex3->GetSize();
		//Index3 *pIndex3 = NULL;
		//int nTriSize;
		//for ( j=0; j<nConeSize; j++)
		//{
		//	pIndex3 = (Index3 *)pptrArrIndex3->GetAt(j);
		//	if ( (i < nSize - 1) && pIndex3->nPolyIndex != i && pIndex3->nPolyIndex != (i+1) 
		//		|| (i == nSize - 1) && pIndex3->nPolyIndex != i && pIndex3->nPolyIndex != 0 
		//		)
		//	{
		//		nTriSize = pIndex3->nNumCounts - 2;
		//		pptf3d = (point3f*)pptrArrRude3->GetAt(pIndex3->nStartIndex); 
		//		ptfKnife[0].x = pptf3d->x;
		//		ptfKnife[0].y = pptf3d->y;
		//		ptfKnife[0].z = pptf3d->z;
		//		for ( k = pIndex3->nStartIndex + 1; k <= pIndex3->nStartIndex + nTriSize; k++ )
		//		{
		//			pptf3d = (point3f*)pptrArrRude3->GetAt(k);
		//			ptfKnife[1].x = pptf3d->x;
		//			ptfKnife[1].y = pptf3d->y;
		//			ptfKnife[1].z = pptf3d->z;

		//			pptf3d = (point3f*)pptrArrRude3->GetAt(k+1);
		//			ptfKnife[2].x = pptf3d->x;
		//			ptfKnife[2].y = pptf3d->y;
		//			ptfKnife[2].z = pptf3d->z;

		//			CutUnSeenByOne(ptfFish, ptfKnife, nFishSize, nKnifeSize, &ptrCutLineSegArr);
		//		}
		//	}
		//}// end of for 

		//IntersectEachOther(&ptrCutLineSegArr,  &ptrIntersectPtArr);

		///////////////////////////////////////////////////////////////////////////////////////////////// end

		ppt0 = (pointf *)SYAR_GetAt(pptrPoly,i);
		if (i == nSize-1) 
		{
			ppt1 = (pointf *)SYAR_GetAt(pptrPoly,0);
		}
		else
		{
			ppt1 = (pointf *)SYAR_GetAt(pptrPoly,i+1);
		}

		ptfFormer0.x = (float)ppt0->x;
		ptfFormer0.y = (float)ppt0->y;
		ptfFormer1.x = (float)ppt1->x;
		ptfFormer1.y = (float)ppt1->y;

		pointf *pptfNorm0 = (pointf * )SYAR_GetAt(pptrPolyNorm,2*i);
		pointf *pptfNorm1 = (pointf * )SYAR_GetAt(pptrPolyNorm,2*i+1);
		GetResultTri4(ptfFormer0, ptfFormer1, *pptfNorm0, *pptfNorm1, &ptrCutLineSegArr,  &ptrIntersectPtArr, fWidth, fHeight, pptrTri );
		// 释放ptrCutLineSegArr
		pointf* ppttTemp = NULL;
		int nCount = (int)ptrCutLineSegArr.GetSize();
		for (int ii=0; ii<nCount; ii++)
		{
			ppttTemp = (pointf *)ptrCutLineSegArr.GetAt(ii);
			delete ppttTemp;
			ppttTemp = NULL;
		}
		ptrCutLineSegArr.RemoveAll();
		// 释放ptrIntersectPtArr
		IntersectNode *pInterNode = NULL;
		nCount = (int)ptrIntersectPtArr.GetSize();
		for (int ii=0; ii<nCount; ii++)
		{
			pInterNode = (IntersectNode *)ptrIntersectPtArr.GetAt(ii);
			delete pInterNode;
			pInterNode = NULL;
		}
		ptrIntersectPtArr.RemoveAll();

	} // end of for 

} // END OF CutToTri4

//////////////////////////////////////////////////////////////////////////
void SYBevelPoly::CutToTri3(IN SYArray * pptrPoly, IN SYArray * pptrPolyNorm, IN  CSYPoint3fPtrArray * pptrArrRude4, /*IN CPtrArray * pptrArrBox4,*/
						IN CSYPoint3fPtrArray * pptrArrRude3, /*IN CPtrArray * pptrArrBox3, */IN CSYIndex3Array *pptrArrIndex3,
						IN float fWidth, IN float fHeight, OUT CSYPoint3fPtrArray * pptrTri ) 
{
	int i;
	//int j,k;
	int ii;
	CSYPoint2fPtrArray ptrCutLineSegArr;
	CSYIntersectNodeArray ptrIntersectPtArr;
	point3f ptfFish[3];
	//point3f ptfKnife[4]; 
	int nFishSize = 3;
	int nKnifeSize = 4;
	point3f* pptf3d;
	pointf* ppt0 = NULL;
	pointf* ppt1 = NULL;
	pointf ptfConeHead;
	pointf ptfFormer0, ptfFormer1;

	int nSize = (int)pptrArrRude4->GetSize() / 4; 

	int nConeSize = (int)pptrArrIndex3->GetSize(); 
	Index3 *pIndex3 = NULL;
	int nTriSize;
	for ( i=0; i<nConeSize; i++)
	{
		pIndex3 = (Index3 *)pptrArrIndex3->GetAt(i);
		nTriSize = pIndex3->nNumCounts - 2;
		pptf3d = (point3f*)pptrArrRude3->GetAt(pIndex3->nStartIndex); 
		ptfFish[0].x = pptf3d->x;
		ptfFish[0].y = pptf3d->y;
		ptfFish[0].z = pptf3d->z;
		////////////////////////////////////////////////////////////////////////// begin 
		int nResultNum = (int)pptrTri->GetSize();
		//////////////////////////////////////////////////////////////////////////  end
		for ( ii = pIndex3->nStartIndex + 1; ii <= pIndex3->nStartIndex + nTriSize; ii++ ) 
		{
			pptf3d = (point3f*)pptrArrRude3->GetAt(ii);
			ptfFish[1].x = pptf3d->x;
			ptfFish[1].y = pptf3d->y;
			ptfFish[1].z = pptf3d->z;

			pptf3d = (point3f*)pptrArrRude3->GetAt(ii+1);
			ptfFish[2].x = pptf3d->x;
			ptfFish[2].y = pptf3d->y;
			ptfFish[2].z = pptf3d->z;

			///////////////////////////////////////////////////////////////////////////////////////////////// begin
			// 初步提速注释掉
			//nKnifeSize = 4;
			//for(j=0; j<nSize; j++)
			//{
			//	if ( (pIndex3->nPolyIndex > 0) && (pIndex3->nPolyIndex == j || (pIndex3->nPolyIndex-1) == j )
			//		|| (pIndex3->nPolyIndex == 0) && (pIndex3->nPolyIndex == j || j == nSize - 1 )
			//		)
			//	{
			//		continue;
			//	}
			//	for(k=0; k<4; k++)
			//	{
			//		pptf3d = (point3f*)pptrArrRude4->GetAt(4*j + k);
			//		ptfKnife[k].x = pptf3d->x;
			//		ptfKnife[k].y = pptf3d->y;
			//		ptfKnife[k].z = pptf3d->z;
			//	}
			//	CutUnSeenByOne(ptfFish, ptfKnife, nFishSize, nKnifeSize, &ptrCutLineSegArr);
			//} // end of for 

			//nKnifeSize = 3;
			//Index3 *pIndex3K = NULL;
			//int nTriSizeK;
			//for ( j=0; j<nConeSize; j++)
			//{
			//	pIndex3K = (Index3 *)pptrArrIndex3->GetAt(j);
			//	if (pIndex3->nPolyIndex == pIndex3K->nPolyIndex) 
			//	{
			//		continue;
			//	}
			//	else
			//	{
			//		nTriSizeK = pIndex3K->nNumCounts - 2;
			//		pptf3d = (point3f*)pptrArrRude3->GetAt(pIndex3K->nStartIndex); 
			//		ptfKnife[0].x = pptf3d->x;
			//		ptfKnife[0].y = pptf3d->y;
			//		ptfKnife[0].z = pptf3d->z;
			//		for ( k = pIndex3K->nStartIndex + 1; k <= pIndex3K->nStartIndex + nTriSizeK; k++ )
			//		{
			//			pptf3d = (point3f*)pptrArrRude3->GetAt(k);
			//			ptfKnife[1].x = pptf3d->x;
			//			ptfKnife[1].y = pptf3d->y;
			//			ptfKnife[1].z = pptf3d->z;

			//			pptf3d = (point3f*)pptrArrRude3->GetAt(k+1);
			//			ptfKnife[2].x = pptf3d->x;
			//			ptfKnife[2].y = pptf3d->y;
			//			ptfKnife[2].z = pptf3d->z;

			//			CutUnSeenByOne(ptfFish, ptfKnife, nFishSize, nKnifeSize, &ptrCutLineSegArr);
			//		}
			//	}
			//}// end of for 

			//IntersectEachOther(&ptrCutLineSegArr,  &ptrIntersectPtArr);
			///////////////////////////////////////////////////////////////////////////////////////////////// end

			ptfConeHead.x = ptfFish[0].x;
			ptfConeHead.y = ptfFish[0].y;

			ptfFormer0.x = ptfFish[1].x;
			ptfFormer0.y = ptfFish[1].y;
			ptfFormer1.x = ptfFish[2].x;
			ptfFormer1.y = ptfFish[2].y;

			pointf *pptfNorm0;
			if (pIndex3->nPolyIndex != 0 ) 
			{
				pptfNorm0 = (pointf * )SYAR_GetAt(pptrPolyNorm,2*pIndex3->nPolyIndex-1);
			}
			else
			{
				pptfNorm0 = (pointf * )SYAR_GetAt(pptrPolyNorm,SYAR_GetSize(pptrPolyNorm)-1);
			}
			pointf *pptfNorm = (pointf * )SYAR_GetAt(pptrPolyNorm,2*pIndex3->nPolyIndex);
			GetResultTri3(ptfConeHead, *pptfNorm, ptfFormer0, ptfFormer1, &ptrCutLineSegArr,  &ptrIntersectPtArr, fWidth, fHeight, pptrTri );
			// 释放ptrCutLineSegArr
			pointf* ppttTemp = NULL;
			int nCount = (int)ptrCutLineSegArr.GetSize();
			for (int ii=0; ii<nCount; ii++)
			{
				ppttTemp = (pointf *)ptrCutLineSegArr.GetAt(ii);
				delete ppttTemp;
				ppttTemp = NULL;
			}
			ptrCutLineSegArr.RemoveAll();
			// 释放ptrIntersectPtArr
			IntersectNode *pInterNode = NULL;
			nCount = (int)ptrIntersectPtArr.GetSize();
			for (int ii=0; ii<nCount; ii++)
			{
				pInterNode = (IntersectNode *)ptrIntersectPtArr.GetAt(ii);
				delete pInterNode;
				pInterNode = NULL;
			}
			ptrIntersectPtArr.RemoveAll();

		} // end of inner-for 

		//////////////////////////////////////////////////////////////////////////
		//int nResultNum = (int)pptrTri->GetSize();
		int nNewResultNum = (int)pptrTri->GetSize();
		if (nNewResultNum > nResultNum) 
		{
			pointf *pptfNorm0;
			if (pIndex3->nPolyIndex != 0 ) 
			{
				pptfNorm0 = (pointf * )SYAR_GetAt(pptrPolyNorm,2*pIndex3->nPolyIndex-1);
			}
			else
			{
				pptfNorm0 = (pointf * )SYAR_GetAt(pptrPolyNorm,SYAR_GetSize(pptrPolyNorm)-1);
			}
			pointf *pptfNorm1 = (pointf * )SYAR_GetAt(pptrPolyNorm,2*pIndex3->nPolyIndex);

			float fdHeight = 1.f / fHeight;
			pptf3d = (point3f*)pptrTri->GetAt(nResultNum+4);
			vector3f v3DNormal; 
			float fR =  sqrtf( pptfNorm0->x * pptfNorm0->x + pptfNorm0->y * pptfNorm0->y );
			v3DNormal.x = pptfNorm0->x;
			v3DNormal.y = fR * fWidth * fdHeight;
			v3DNormal.z = pptfNorm0->y;
			// 皈依化
			float3Normalize( &v3DNormal,  &v3DNormal );
			pptf3d->x = -v3DNormal.x;
			pptf3d->y = -v3DNormal.y;
			pptf3d->z = -v3DNormal.z;

			pptf3d = (point3f*)pptrTri->GetAt(nNewResultNum-1);
			fR =  sqrtf( pptfNorm1->x * pptfNorm1->x + pptfNorm1->y * pptfNorm1->y );
			v3DNormal.x = pptfNorm1->x;
			v3DNormal.y = fR * fWidth * fdHeight;
			v3DNormal.z = pptfNorm1->y;
			// 皈依化
			float3Normalize( &v3DNormal,  &v3DNormal );
			pptf3d->x = -v3DNormal.x;
			pptf3d->y = -v3DNormal.y;
			pptf3d->z = -v3DNormal.z;

		}
		//////////////////////////////////////////////////////////////////////////
		
	} // end of outer-for  

} // END OF CutToTri3

//////////////////////////////////////////////////////////////////////////
//线段组求交
// 求交的结果放在一个Array中：
// pointf 点坐标
void SYBevelPoly::IntersectEachOther(IN CSYPoint2fPtrArray * pptrLineSegArr,  // 输入线段顶点Array
								 OUT CSYIntersectNodeArray * pptrIntersectPtArr ) // 交点Array
{
	int i, j;
	pointf *ppt1S = NULL;
	pointf *ppt1E = NULL;
	pointf *ppt2S = NULL;
	pointf *ppt2E = NULL;
	pointf ptfIntersect;
	IntersectNode *pInterNode = NULL;
	int nSize = (int)pptrLineSegArr->GetSize();
	for (i=0; i<nSize; i+=2)
	{
		ppt1S = (pointf *)pptrLineSegArr->GetAt(i);
		ppt1E = (pointf *)pptrLineSegArr->GetAt(i+1);
		for(j=i+2; j<nSize; j+=2)
		{
			ppt2S = (pointf *)pptrLineSegArr->GetAt(j);
			ppt2E = (pointf *)pptrLineSegArr->GetAt(j+1);

			if (SamePtf(*ppt1S, *ppt2S) || SamePtf(*ppt1S, *ppt2E)
				|| SamePtf(*ppt1E, *ppt2S) || SamePtf(*ppt1E, *ppt2E) )
			{
				continue;
			}

			// 有
			if ( SingleLineIntersection(*ppt1S, *ppt1E, *ppt2S, *ppt2E, ptfIntersect ) ) 
			{
				pInterNode = new IntersectNode;
				pInterNode->ptf.x = ptfIntersect.x;
				pInterNode->ptf.y = ptfIntersect.y;
				pInterNode->nIndex1 = i;
				pInterNode->nIndex2 = j;
				pptrIntersectPtArr->Add(pInterNode);
			} // 有
		} // end of inner-for
	} // end of outer-for 
} // END OF IntersectEachOther

void SYBevelPoly::GetResultTri4(IN pointf ptfFormer0, IN pointf ptfFormer1,  
							IN pointf ptfConeHeadNorm0,  
							IN pointf ptfConeHeadNorm1,  
							IN CSYPoint2fPtrArray *pptrCutLineSegArr,  
							IN CSYIntersectNodeArray *ptrIntersectPtArr, 
							IN float fWidth, IN float fHeight, 
							OUT CSYPoint3fPtrArray *pptrTri ) 
{
	ResultNode * pResultNode = NULL;
	CSYResultNodeArray ptrResultNodeArr;
	int i, j;
	pointf * pptf = NULL;
	pointf * pptfS = NULL;
	pointf * pptfE = NULL;
	pointf ptfP; 
	float fParam;
	pointf ptf, ptf0, ptf1;
	pointf ptfIntersect;
	BOOL bCut = FALSE;
	ResultNode * pNode2 = NULL;
	
	int nSize = (int)pptrCutLineSegArr->GetSize();
	for (i=0; i<nSize; i++)
	{
		pptf = (pointf *)pptrCutLineSegArr->GetAt(i);
		Perpendicular(ptfFormer0, ptfFormer1, *pptf, ptfP, fParam );
		bCut = FALSE;
		for (j=0; j<nSize; j+=2)
		{
			if (i == j || i == j+1) //if (i == j/2) 
			{
				continue;
			}
			pptfS = (pointf *)pptrCutLineSegArr->GetAt(j);
			pptfE = (pointf *)pptrCutLineSegArr->GetAt(j+1);
			if ( SingleLineIntersection(*pptf, ptfP, *pptfS, *pptfE, ptfIntersect ) ) 
			{
				if (!SamePtf(*pptf, ptfIntersect))  
				{
					bCut = TRUE;
					break;
				}
			}// end of if
		}// end of inner-for
		if (bCut == FALSE)  
		{
			pResultNode = new ResultNode;
			pResultNode->ptf.x = pptf->x;
			pResultNode->ptf.y = pptf->y;
			pResultNode->ptfP.x = ptfP.x;
			pResultNode->ptfP.y = ptfP.y;
			pResultNode->fParam = fParam;
			//ptrResultNodeArr.Add(pResultNode); // 改为排序式插入
			InsertNode(&ptrResultNodeArr, pResultNode);
		}
	}// end of outer-for

	IntersectNode *pInterNode = NULL;
	int nInterSize = (int)ptrIntersectPtArr->GetSize();
	for (i=0; i<nInterSize; i++)
	{
		pInterNode = (IntersectNode *)ptrIntersectPtArr->GetAt(i);
		Perpendicular(ptfFormer0, ptfFormer1, pInterNode->ptf, ptfP, fParam );
		bCut = FALSE;
		for (j=0; j<nSize; j+=2)
		{
			if (pInterNode->nIndex1 == j || pInterNode->nIndex2 == j) 
			{
				continue;
			}
			pptfS = (pointf *)pptrCutLineSegArr->GetAt(j);
			pptfE = (pointf *)pptrCutLineSegArr->GetAt(j+1);
			if ( SingleLineIntersection(pInterNode->ptf, ptfP, *pptfS, *pptfE, ptfIntersect ) ) 
			{
				if (!SamePtf(pInterNode->ptf, ptfIntersect))
				{
					bCut = TRUE;
					break;
				}
			}// end of if
		}// end of inner-for

		if (bCut == FALSE) 
		{
			pResultNode = new ResultNode;
			pResultNode->ptf.x = pInterNode->ptf.x;
			pResultNode->ptf.y = pInterNode->ptf.y;
			pResultNode->ptfP.x = ptfP.x;
			pResultNode->ptfP.y = ptfP.y;
			pResultNode->fParam = fParam;
			//ptrResultNodeArr.Add(pResultNode); // 改为排序式插入
			InsertNode(&ptrResultNodeArr, pResultNode);
		}
	}// end of outer-for

	vector3f v3DNormal; 
	vector3f v3DNormal0; 
	vector3f v3DNormal1; 
	//if (nSize < 1)  // 
	{
		float r = sqrt( (float)( (ptfFormer1.x - ptfFormer0.x)*(ptfFormer1.x - ptfFormer0.x) + (ptfFormer0.y - ptfFormer1.y)*(ptfFormer0.y - ptfFormer1.y) ) );
		float fTemp = 1.f / r;
		ptf.x = -fWidth * (ptfFormer0.y - ptfFormer1.y) * fTemp;
		ptf.y = -fWidth * (ptfFormer1.x - ptfFormer0.x) * fTemp;

		ptf0.x = ptfFormer0.x - ptf.x;
		ptf0.y = ptfFormer0.y - ptf.y;

		ptf1.x = ptfFormer1.x - ptf.x;
		ptf1.y = ptfFormer1.y - ptf.y;

		
		//vector3f v3D0;
		//v3D0.x = ptfFormer0.x - ptfFormer1.x;
		//v3D0.y = 0.0f;
		//v3D0.z = ptfFormer0.y - ptfFormer1.y;
		//vector3f v3D1;
		//v3D1.x = ptf.x;
		//v3D1.y = fHeight; // fWidth * fWidth / fHeight;
		//v3D1.z = ptf.y;
		//float3CrossProduct( &v3DNormal, &v3D0, &v3D1);
		////////////////////////////////////////////////////////////////////////// begin
		// 新的生成方式
		//ptfConeHeadNorm;
		float fdHeight = 1.f / fHeight;
		float fR0 =  sqrtf( ptfConeHeadNorm0.x * ptfConeHeadNorm0.x + ptfConeHeadNorm0.y * ptfConeHeadNorm0.y );
		v3DNormal0.x = -ptfConeHeadNorm0.x;
		v3DNormal0.y = -fR0 * fWidth * fdHeight;
		v3DNormal0.z = -ptfConeHeadNorm0.y;
		////////////////////////////////////////////////////////////////////////// end
		float3Normalize( &v3DNormal0,  &v3DNormal0 );
		////////////////////////////////////////////////////////////////////////// begin
		// 新的生成方式
		//ptfConeHeadNorm;
		float fR1 =  sqrtf( ptfConeHeadNorm1.x * ptfConeHeadNorm1.x + ptfConeHeadNorm1.y * ptfConeHeadNorm1.y );
		v3DNormal1.x = -ptfConeHeadNorm1.x;
		v3DNormal1.y = -fR1 * fWidth * fdHeight;
		v3DNormal1.z = -ptfConeHeadNorm1.y;
		////////////////////////////////////////////////////////////////////////// end
		float3Normalize( &v3DNormal1,  &v3DNormal1 );

		pResultNode = new ResultNode;
		pResultNode->ptf.x = ptf0.x;
		pResultNode->ptf.y = ptf0.y;
		pResultNode->ptfP.x = ptfFormer0.x;
		pResultNode->ptfP.y = ptfFormer0.y;
		pResultNode->fParam = 0.0f;
		if (ptrResultNodeArr.GetSize() == 0)
		{
			ptrResultNodeArr.Add(pResultNode);
		}
		else
		{
			ptrResultNodeArr.InsertAt(0, pResultNode);
		}

		pResultNode = new ResultNode;
		pResultNode->ptf.x = ptf1.x;
		pResultNode->ptf.y = ptf1.y;
		pResultNode->ptfP.x = ptfFormer1.x;
		pResultNode->ptfP.y = ptfFormer1.y;
		pResultNode->fParam = 1.0f;
		ptrResultNodeArr.Add(pResultNode);
		nSize = (int)ptrResultNodeArr.GetSize();
		pNode2 = (ResultNode * )ptrResultNodeArr.GetAt(nSize - 2);
		if (pResultNode->fParam - pNode2->fParam < 0.0001 && pResultNode->fParam - pNode2->fParam > -0.0001 ) 
		{
			ptrResultNodeArr.RemoveAt(nSize - 1);
			delete pResultNode;
			pResultNode = NULL;
		}
		pNode2 = (ResultNode * )ptrResultNodeArr.GetAt(0);
		pResultNode = (ResultNode * )ptrResultNodeArr.GetAt(1);
		if (pResultNode->fParam - pNode2->fParam < 0.0001 && pResultNode->fParam - pNode2->fParam > -0.0001 ) 
		{
			ptrResultNodeArr.RemoveAt(0);
			delete pNode2;
			pNode2 = NULL;
		}
	} // end of if (nSize < 1)  

	float fFactor = - fHeight / fWidth;
	point3f* pptf3d = NULL;
	float fD1, fD2;
	nSize = (int)ptrResultNodeArr.GetSize();
	for (i=0; i<nSize - 1; i++)
	{
		pResultNode = (ResultNode *)ptrResultNodeArr.GetAt(i);
		pNode2 = (ResultNode *)ptrResultNodeArr.GetAt(i+1);
		fD1 = sqrt( (float)( (pResultNode->ptf.x - pResultNode->ptfP.x ) * (pResultNode->ptf.x - pResultNode->ptfP.x ) 
			+ (pResultNode->ptf.y - pResultNode->ptfP.y ) * (pResultNode->ptf.y - pResultNode->ptfP.y ) ) );
		fD2 = sqrt( (float)( (pNode2->ptf.x - pNode2->ptfP.x ) * (pNode2->ptf.x - pNode2->ptfP.x ) 
			+ (pNode2->ptf.y - pNode2->ptfP.y ) * (pNode2->ptf.y - pNode2->ptfP.y ) ) );
		// 第一个
		pptf3d = new point3f;
		pptf3d->x = pResultNode->ptfP.x;
		pptf3d->y = 0;
		pptf3d->z = pResultNode->ptfP.y;
		pptrTri->Add(pptf3d);

		pptf3d = new point3f;
		pptf3d->x = pResultNode->ptf.x;
		pptf3d->y = fD1 * fFactor;
		pptf3d->z = pResultNode->ptf.y;
		pptrTri->Add(pptf3d);

		pptf3d = new point3f;
		pptf3d->x = pNode2->ptf.x;
		pptf3d->y = fD2 * fFactor;
		pptf3d->z = pNode2->ptf.y;
		pptrTri->Add(pptf3d);

		v3DNormal.x = (1 - pResultNode->fParam) * v3DNormal0.x + pResultNode->fParam * v3DNormal1.x;
		v3DNormal.y = (1 - pResultNode->fParam) * v3DNormal0.y + pResultNode->fParam * v3DNormal1.y;
		v3DNormal.z = (1 - pResultNode->fParam) * v3DNormal0.z + pResultNode->fParam * v3DNormal1.z;
		////////////////////////////////////////////////////////////////////////// end
		// 皈依化
		float3Normalize( &v3DNormal,  &v3DNormal );

		pptf3d = new point3f;
		pptf3d->x = v3DNormal.x;
		pptf3d->y = v3DNormal.y;
		pptf3d->z = v3DNormal.z;
		pptrTri->Add(pptf3d);

		pptf3d = new point3f;
		pptf3d->x = v3DNormal.x;
		pptf3d->y = v3DNormal.y;
		pptf3d->z = v3DNormal.z;
		pptrTri->Add(pptf3d);

		v3DNormal.x = (1 - pNode2->fParam) * v3DNormal0.x + pNode2->fParam * v3DNormal1.x;
		v3DNormal.y = (1 - pNode2->fParam) * v3DNormal0.y + pNode2->fParam * v3DNormal1.y;
		v3DNormal.z = (1 - pNode2->fParam) * v3DNormal0.z + pNode2->fParam * v3DNormal1.z;
		////////////////////////////////////////////////////////////////////////// end
		// 皈依化
		float3Normalize( &v3DNormal,  &v3DNormal );

		pptf3d = new point3f;
		pptf3d->x = v3DNormal.x;
		pptf3d->y = v3DNormal.y;
		pptf3d->z = v3DNormal.z;
		pptrTri->Add(pptf3d);

		// 第二个
		pptf3d = new point3f;
		pptf3d->x = pResultNode->ptfP.x;
		pptf3d->y = 0;
		pptf3d->z = pResultNode->ptfP.y;
		pptrTri->Add(pptf3d);

		pptf3d = new point3f;
		pptf3d->x = pNode2->ptf.x;
		pptf3d->y = fD2 * fFactor;
		pptf3d->z = pNode2->ptf.y;
		pptrTri->Add(pptf3d);

		pptf3d = new point3f;
		pptf3d->x = pNode2->ptfP.x;
		pptf3d->y = 0;
		pptf3d->z = pNode2->ptfP.y;
		pptrTri->Add(pptf3d);

		v3DNormal.x = (1 - pResultNode->fParam) * v3DNormal0.x + pResultNode->fParam * v3DNormal1.x;
		v3DNormal.y = (1 - pResultNode->fParam) * v3DNormal0.y + pResultNode->fParam * v3DNormal1.y;
		v3DNormal.z = (1 - pResultNode->fParam) * v3DNormal0.z + pResultNode->fParam * v3DNormal1.z;
		////////////////////////////////////////////////////////////////////////// end
		// 皈依化
		float3Normalize( &v3DNormal,  &v3DNormal );

		pptf3d = new point3f;
		pptf3d->x = v3DNormal.x;
		pptf3d->y = v3DNormal.y;
		pptf3d->z = v3DNormal.z;
		pptrTri->Add(pptf3d);

		v3DNormal.x = (1 - pNode2->fParam) * v3DNormal0.x + pNode2->fParam * v3DNormal1.x;
		v3DNormal.y = (1 - pNode2->fParam) * v3DNormal0.y + pNode2->fParam * v3DNormal1.y;
		v3DNormal.z = (1 - pNode2->fParam) * v3DNormal0.z + pNode2->fParam * v3DNormal1.z;
		////////////////////////////////////////////////////////////////////////// end
		// 皈依化
		float3Normalize( &v3DNormal,  &v3DNormal );

		pptf3d = new point3f;
		pptf3d->x = v3DNormal.x;
		pptf3d->y = v3DNormal.y;
		pptf3d->z = v3DNormal.z;
		pptrTri->Add(pptf3d);

		pptf3d = new point3f;
		pptf3d->x = v3DNormal.x;
		pptf3d->y = v3DNormal.y;
		pptf3d->z = v3DNormal.z;
		pptrTri->Add(pptf3d);
	} // end of for

	//释放
	nSize = (int)ptrResultNodeArr.GetSize();
	for (i=0; i<nSize; i++)
	{
		pResultNode = (ResultNode *)ptrResultNodeArr.GetAt(i);
		delete pResultNode;
		pResultNode = NULL;
	}
	ptrResultNodeArr.RemoveAll();
} // END OF GetResultTri4

//////////////////////////////////////////////////////////////////////////
void SYBevelPoly::GetResultTri3(IN pointf ptfConeHead,  
							IN pointf ptfConeHeadNorm,  
							IN pointf ptfFormer0, IN pointf ptfFormer1,  
							IN CSYPoint2fPtrArray *pptrCutLineSegArr,  
							IN CSYIntersectNodeArray *ptrIntersectPtArr, 
							IN float fWidth, IN float fHeight, 
							OUT CSYPoint3fPtrArray *pptrTri ) 
{
	ResultNode * pResultNode = NULL;
	CSYResultNodeArray ptrResultNodeArr;
	int i, j;
	pointf * pptf = NULL;
	pointf * pptfS = NULL;
	pointf * pptfE = NULL;
	pointf ptfP; 
	float fParam; 
	pointf ptfIntersect;
	BOOL bCut = FALSE;
	ResultNode * pNode2 = NULL;
	int nSize = (int)pptrCutLineSegArr->GetSize();
	for (i=0; i<nSize; i++)
	{
		pptf = (pointf *)pptrCutLineSegArr->GetAt(i);
		Perpendicular(ptfFormer0, ptfFormer1, *pptf, ptfP, fParam );
		bCut = FALSE;
		for (j=0; j<nSize; j+=2)
		{
			if (i == j || i == j+1) //if (i == j/2) 
			{
				continue;
			}
			pptfS = (pointf *)pptrCutLineSegArr->GetAt(j);
			pptfE = (pointf *)pptrCutLineSegArr->GetAt(j+1);
			if ( SingleLineIntersection(*pptf, ptfConeHead, *pptfS, *pptfE, ptfIntersect ) ) 
			{
				if (!SamePtf(*pptf, ptfIntersect))  
				{
					bCut = TRUE;
					break;
				}
			}// end of if
		}// end of inner-for
		if (bCut == FALSE)  
		{
			pResultNode = new ResultNode;
			pResultNode->ptf.x = pptf->x;
			pResultNode->ptf.y = pptf->y;
			pResultNode->ptfP.x = ptfP.x;
			pResultNode->ptfP.y = ptfP.y;
			pResultNode->fParam = fParam;
			//ptrResultNodeArr.Add(pResultNode); // 改为排序式插入
			InsertNode(&ptrResultNodeArr, pResultNode);
		}
	}// end of outer-for

	IntersectNode *pInterNode = NULL;
	nSize = (int)ptrIntersectPtArr->GetSize();
	for (i=0; i<nSize; i++)
	{
		pInterNode = (IntersectNode *)ptrIntersectPtArr->GetAt(i);
		Perpendicular(ptfFormer0, ptfFormer1, pInterNode->ptf, ptfP, fParam );
		bCut = FALSE;
		for (j=0; j<nSize; j+=2)
		{
			if (pInterNode->nIndex1 == j || pInterNode->nIndex2 == j) 
			{
				continue;
			}
			pptfS = (pointf *)pptrCutLineSegArr->GetAt(j);
			pptfE = (pointf *)pptrCutLineSegArr->GetAt(j+1);
			if ( SingleLineIntersection(pInterNode->ptf, ptfConeHead, *pptfS, *pptfE, ptfIntersect ) ) 
			{
				if (!SamePtf(pInterNode->ptf, ptfIntersect))  
				{
					bCut = TRUE;
					break;
				}
			}// end of if
		}// end of inner-for

		if (bCut == FALSE)  
		{
			pResultNode = new ResultNode;
			pResultNode->ptf.x = pInterNode->ptf.x;
			pResultNode->ptf.y = pInterNode->ptf.y;
			pResultNode->ptfP.x = ptfP.x;
			pResultNode->ptfP.y = ptfP.y;
			pResultNode->fParam = fParam;
			//ptrResultNodeArr.Add(pResultNode); // 改为排序式插入
			InsertNode(&ptrResultNodeArr, pResultNode);
		}
	}// end of outer-for

	//nSize = (int)ptrResultNodeArr.GetSize();

	vector3f v3DNormal; 
	pointf ptfMid;
	ptfMid.x = (ptfFormer0.x + ptfFormer1.x) * 0.5f;
	ptfMid.y = (ptfFormer0.y + ptfFormer1.y) * 0.5f;
	// 先皈依化　，然后再乘以长度fWidth
	float  fR = sqrtf( (ptfMid.x - ptfConeHead.x) * (ptfMid.x - ptfConeHead.x) + (ptfMid.y - ptfConeHead.y) * (ptfMid.y - ptfConeHead.y) );
	ptfMid.x = ptfConeHead.x + fWidth * (ptfMid.x - ptfConeHead.x) / fR;
	ptfMid.y = ptfConeHead.y + fWidth * (ptfMid.y - ptfConeHead.y) / fR;
	fR = sqrtf( (ptfMid.x - ptfConeHead.x) * (ptfMid.x - ptfConeHead.x) + (ptfMid.y - ptfConeHead.y) * (ptfMid.y - ptfConeHead.y) );
	v3DNormal.x = ptfMid.x - ptfConeHead.x;
	v3DNormal.y = fR * fWidth / fHeight;
	v3DNormal.z = ptfMid.y - ptfConeHead.y;
	//////////////////////////////////////////////////////////////////////////// begin
	//// 新的生成方式
	////ptfConeHeadNorm;
	//float fR =  sqrtf( ptfConeHeadNorm.x * ptfConeHeadNorm.x + ptfConeHeadNorm.y * ptfConeHeadNorm.y );
	//v3DNormal.x = ptfConeHeadNorm.x;
	//v3DNormal.y = fR * fWidth / fHeight;
	//v3DNormal.z = ptfConeHeadNorm.y;
	//////////////////////////////////////////////////////////////////////////// end
	// 皈依化
	float3Normalize( &v3DNormal,  &v3DNormal );

	pointf ptfTempt;
	float fPara;
	//if (nSize < 1)  
	{
		//float r = sqrt( (float)( (ptfFormer1.x - ptfFormer0.x)*(ptfFormer1.x - ptfFormer0.x) + (ptfFormer0.y - ptfFormer1.y)*(ptfFormer0.y - ptfFormer1.y) ) );
		pResultNode = new ResultNode;
		pResultNode->ptf.x = ptfFormer0.x;
		pResultNode->ptf.y = ptfFormer0.y;
		pResultNode->ptfP.x = ptfFormer0.x;
		pResultNode->ptfP.y = ptfFormer0.y;
		pResultNode->fParam = 0.0f;
		nSize = (int)ptrResultNodeArr.GetSize();
		if (nSize != 0)
		{
			ptrResultNodeArr.InsertAt(0, pResultNode);
		}
		else
		{
			ptrResultNodeArr.Add(pResultNode);
		}

		pResultNode = new ResultNode;
		pResultNode->ptf.x = ptfFormer1.x;
		pResultNode->ptf.y = ptfFormer1.y;
		pResultNode->ptfP.x = ptfFormer1.x;
		pResultNode->ptfP.y = ptfFormer1.y;
		pResultNode->fParam = 1.0f;
		ptrResultNodeArr.Add(pResultNode);

		nSize = (int)ptrResultNodeArr.GetSize();
		if (nSize > 2) 
		{
			pNode2 = (ResultNode * )ptrResultNodeArr.GetAt(nSize - 2);

			int nDelTemp = 0;
			Perpendicular(ptfConeHead, pResultNode->ptf, pNode2->ptf, ptfTempt, fPara );
			if ( (pNode2->ptf.x - ptfTempt.x ) * (pNode2->ptf.x - ptfTempt.x ) + (pNode2->ptf.y - ptfTempt.y ) * (pNode2->ptf.y - ptfTempt.y ) < 0.01f*g_fScaleAAA  )
				//if (pResultNode->fParam - pNode2->fParam < 0.0001 && pResultNode->fParam - pNode2->fParam > -0.0001 ) 
			{
				nDelTemp = 1;
			}
			pNode2 = (ResultNode * )ptrResultNodeArr.GetAt(0);
			pResultNode = (ResultNode * )ptrResultNodeArr.GetAt(1);
			Perpendicular(ptfConeHead, pNode2->ptf, pResultNode->ptf, ptfTempt, fPara );
			if ( (pResultNode->ptf.x - ptfTempt.x ) * (pResultNode->ptf.x - ptfTempt.x ) + (pResultNode->ptf.y - ptfTempt.y ) * (pResultNode->ptf.y - ptfTempt.y ) < 0.01f*g_fScaleAAA  )
				//if ( PToLDist(pResultNode->ptf, ptfConeHead, pNode2->ptf) < 0.5  )
				//if (pResultNode->fParam - pNode2->fParam < 0.0001 && pResultNode->fParam - pNode2->fParam > -0.0001 ) 
			{
				nDelTemp += 2;
			}
			if (nDelTemp == 1 || nDelTemp == 3) 
			{
				pResultNode = (ResultNode * )ptrResultNodeArr.GetAt(nSize - 1);
				ptrResultNodeArr.RemoveAt(nSize - 1);
				delete pResultNode;
				pResultNode = NULL;
			}
			if (nDelTemp == 2 || nDelTemp == 3) 
			{
				pResultNode = (ResultNode * )ptrResultNodeArr.GetAt(0);
				ptrResultNodeArr.RemoveAt(0);
				delete pResultNode;
				pResultNode = NULL;
			}
		}		
	} // end of if (nSize < 1)  

	//Perpendicular(ptfFormer0, ptfFormer1, ptfConeHead, ptfTempt, fPara );
	ptfTempt.x = (ptfFormer0.x + ptfFormer1.x)/2;
	ptfTempt.y = (ptfFormer0.y + ptfFormer1.y)/2;
	float fRealWidth = sqrt( (float)( (ptfConeHead.x - ptfTempt.x)*(ptfConeHead.x - ptfTempt.x) + (ptfConeHead.y - ptfTempt.y)*(ptfConeHead.y - ptfTempt.y) ) ); 
	float fFactor = - fHeight / fRealWidth;
	point3f* pptf3d = NULL;
	float fD1, fD2;
	nSize = (int)ptrResultNodeArr.GetSize();
	for (i=0; i<nSize - 1; i++)
	{
		pResultNode = (ResultNode *)ptrResultNodeArr.GetAt(i);
		pNode2 = (ResultNode *)ptrResultNodeArr.GetAt(i+1);
		//Perpendicular(ptfConeHead, pResultNode->ptf, pNode2->ptf, ptfTempt, fPara );
		//if ( (pNode2->ptf.x - ptfTempt.x ) * (pNode2->ptf.x - ptfTempt.x ) + (pNode2->ptf.y - ptfTempt.y ) * (pNode2->ptf.y - ptfTempt.y ) > 1.0f  )
		{
			fD1 = sqrt( (float)( (pResultNode->ptf.x - pResultNode->ptfP.x ) * (pResultNode->ptf.x - pResultNode->ptfP.x ) 
				+ (pResultNode->ptf.y - pResultNode->ptfP.y ) * (pResultNode->ptf.y - pResultNode->ptfP.y ) ) );
			fD2 = sqrt( (float)( (pNode2->ptf.x - pNode2->ptfP.x ) * (pNode2->ptf.x - pNode2->ptfP.x ) 
				+ (pNode2->ptf.y - pNode2->ptfP.y ) * (pNode2->ptf.y - pNode2->ptfP.y ) ) );
			fD1 = fRealWidth - fD1;
			fD2 = fRealWidth - fD2;
	
			pptf3d = new point3f; 
			pptf3d->x = ptfConeHead.x;
			pptf3d->y = 0;
			pptf3d->z = ptfConeHead.y;
			pptrTri->Add(pptf3d);

			pptf3d = new point3f;
			pptf3d->x = pResultNode->ptf.x;
			pptf3d->y = fD1 * fFactor;
			pptf3d->z = pResultNode->ptf.y;
			pptrTri->Add(pptf3d);

			pptf3d = new point3f;
			pptf3d->x = pNode2->ptf.x;
			pptf3d->y = fD2 * fFactor;
			pptf3d->z = pNode2->ptf.y;
			pptrTri->Add(pptf3d);

			pptf3d = new point3f;
			pptf3d->x = -v3DNormal.x;
			pptf3d->y = -v3DNormal.y;
			pptf3d->z = -v3DNormal.z;
			pptrTri->Add(pptf3d);
			
			float fDis = sqrt( (float)( (pResultNode->ptf.x - ptfConeHead.x)*(pResultNode->ptf.x - ptfConeHead.x) + (pResultNode->ptf.y - ptfConeHead.y)*(pResultNode->ptf.y - ptfConeHead.y) ) );
			pptf3d = new point3f;
			pptf3d->x = -(pResultNode->ptf.x - ptfConeHead.x);
			pptf3d->y = -fDis * fWidth / fHeight;
			pptf3d->z = -(pResultNode->ptf.y - ptfConeHead.y);
			float3Normalize( pptf3d,  pptf3d );
			pptrTri->Add(pptf3d);

			fDis = sqrt( (float)( (pNode2->ptf.x - ptfConeHead.x)*(pNode2->ptf.x - ptfConeHead.x) + (pNode2->ptf.y - ptfConeHead.y)*(pNode2->ptf.y - ptfConeHead.y) ) );
			pptf3d = new point3f;
			pptf3d->x = -(pNode2->ptf.x - ptfConeHead.x);
			pptf3d->y = -fDis * fWidth / fHeight;
			pptf3d->z = -(pNode2->ptf.y - ptfConeHead.y);
			float3Normalize( pptf3d,  pptf3d );
			pptrTri->Add(pptf3d);

		} // end of if 

	} // end of for

	//释放
	nSize = (int)ptrResultNodeArr.GetSize();
	for (i=0; i<nSize; i++)
	{
		pResultNode = (ResultNode *)ptrResultNodeArr.GetAt(i);
		delete pResultNode;
		pResultNode = NULL;
	}
	ptrResultNodeArr.RemoveAll();
} // END OF GetResultTri3
//
////////////////////////////////////////////////////////////
//// 求点p到线段l所在直线的距离 
////////////////////////////////////////////////////////////
//double SYBevelPoly::PToLDist(IN pointf ptf, IN pointf ptfLS, IN pointf ptfLE) 
//{ 
//	float dMultiply = (ptf.x - ptfLS.x)*(ptfLE.y - ptfLS.y) - (ptfLE.x - ptfLS.x)*(ptf.y - ptfLS.y);
//	return abs(dMultiply) / sqrt((float)( (ptfLE.x - ptfLS.x)*(ptfLE.x - ptfLS.x) + (ptfLE.y - ptfLS.y)*(ptfLE.y - ptfLS.y) ));
//} // End of PToLDist
//
//////////////////////////////////////////////////////////////////////////
void SYBevelPoly::Perpendicular(IN pointf  a, IN pointf b, IN pointf c, OUT pointf &ptfP, OUT float & fParam )
{
	pointf  ab;
	ab.x = b.x - a.x;
	ab.y = b.y - a.y;
	pointf ac;
	ac.x = c.x - a.x;
	ac.y = c.y - a.y;
	float f ;
	f = ab.x * ac.x + ab.y * ac.y;

	float d;
	d = ab.x * ab.x + ab.y * ab.y;
	f = f/d; 
	ptfP.x = a.x + f * ab.x;
	ptfP.y = a.y + f * ab.y;

	fParam = f;
} // END OF Perpendicular

//////////////////////////////////////////////////////////////////////////
void SYBevelPoly::InsertNode(IN OUT CSYResultNodeArray *pptrResultNodeArr, IN ResultNode * pNode)
{
	const float fZero = 0.0001f;
	int i;
	ResultNode * pResultNode;
	int nSize = (int)pptrResultNodeArr->GetSize();
	if (nSize == 0) 
	{
		pptrResultNodeArr->Add(pNode);
		return;
	}
	for(i=0; i<nSize; i++)
	{
		pResultNode = (ResultNode *)pptrResultNodeArr->GetAt(i);
		if (pNode->fParam < pResultNode->fParam ) 
		{ 
			break;
		}
	} // end of for
	if (i==nSize)
	{
		pptrResultNodeArr->Add(pNode);
	}
	else
	{
		pptrResultNodeArr->InsertAt(i, pNode);
	}
} // END OF InsertNode

//////////////////////////////////////////////////////////////////////////
// 功能: 求外包矩形
void SYBevelPoly::GetEnvelop(IN pointf *ptf, IN int nSize, OUT rectf &rt)
{
	rt.left = 65535;
	rt.top = 65535;
	rt.right = -65535;
	rt.bottom = -65535;

	for(int i=0; i<nSize; i++)
	{
		if (ptf[i].x < rt.left) 
		{
			rt.left = ptf[i].x;
		}
		if (ptf[i].x > rt.right) 
		{
			rt.right = ptf[i].x;
		}
		if (ptf[i].y < rt.top) 
		{
			rt.top = ptf[i].y;
		}
		if (ptf[i].y > rt.bottom) 
		{
			rt.bottom = ptf[i].y;
		}
	}
} // END OF GetEnvelop

//////////////////////////////////////////////////////////////////////////
// 矩形相交
BOOL SYBevelPoly::RectOverlap(IN rectf rt1, IN rectf rt2)
{
	//先判断是否合法矩形
#ifdef DEBUG
	if(rt1.bottom <= rt1.top || rt1.right <= rt1.left)
		return FALSE;
	if(rt2.bottom <= rt2.top || rt2.right <= rt2.left)
		return FALSE;
#endif
	pointf ptCenter1, ptCenter2;
	ptCenter1.x = rt1.left + rt1.right ;
	ptCenter1.y = rt1.top + rt1.bottom ;
	ptCenter2.x = rt2.left + rt2.right ;
	ptCenter2.y = rt2.top + rt2.bottom ;
	if (abs(ptCenter1.x - ptCenter2.x) < (rt1.right - rt1.left + rt2.right - rt2.left) 
		&&abs(ptCenter1.y - ptCenter2.y) < (rt1.bottom - rt1.top + rt2.bottom - rt2.top) 
		) 
	{
		return TRUE;
	}
	return FALSE;
} // END OF RectOverlap

//////////////////////////////////////////////////////////////////////////
void SYBevelPoly::ProjCrossLine(IN point3f ptf1, IN point3f ptf2, IN point3f ptf3, 
				   IN point3f ptf4, IN point3f ptf5, IN point3f ptf6,
				   OUT float &fA, OUT float &fB, OUT float &fC)
{
	float A0, B0, C0;
	float A1, B1, C1;
	point3f n01, n02; 
	point3f n11, n12; 
	n01.x = ptf2.x - ptf1.x;
	n01.y = ptf2.y - ptf1.y;
	n01.z = ptf2.z - ptf1.z;
	n02.x = ptf3.x - ptf2.x;
	n02.y = ptf3.y - ptf2.y;
	n02.z = ptf3.z - ptf2.z;

	n11.x = ptf5.x - ptf4.x;
	n11.y = ptf5.y - ptf4.y;
	n11.z = ptf5.z - ptf4.z;
	n12.x = ptf6.x - ptf5.x;
	n12.y = ptf6.y - ptf5.y;
	n12.z = ptf6.z - ptf5.z;

	A0 = n01.y * n02.z - n01.z * n02.y; 
	B0 = n01.z * n02.x - n01.x * n02.z;
	C0 = n01.x * n02.y - n01.y * n02.x;

	A1 = n11.y * n12.z - n11.z * n12.y; 
	B1 = n11.z * n12.x - n11.x * n12.z;
	C1 = n11.x * n12.y - n11.y * n12.x;

	fA = A0 * C1 - A1 * C0;
	fB = B0 * C1 - B1 * C0;
	fC = -A0 * C1 * ptf1.x + A1 * C0 * ptf4.x 
		- B0 * C1 * ptf1.y + B1 * C0 * ptf4.y
		- C0 * C1 * ptf1.z + C1 * C0 * ptf4.z;
	
} // END OF ProjCrossLine

//////////////////////////////////////////////////////////////////////////
BOOL SYBevelPoly::CutLine(IN float fA, IN float fB, IN float fC,
			 IN pointf* ptf, IN int nSize,
			 OUT pointf &ptfS, OUT pointf &ptfE )
{
	pointf ptfCross;
	ptfS.x = -65535;
	ptfS.y = -65535;
	ptfE.x = -65535;
	ptfE.y = -65535;
	if (LineCrossLineSeg(fA, fB, fC, ptf[nSize-1], ptf[0], ptfCross )) 
	{
		ptfS.x = ptfCross.x;
		ptfS.y = ptfCross.y;
	}
	for(int i=0; i<nSize-1; i++)
	{
		if (LineCrossLineSeg(fA, fB, fC, ptf[i], ptf[i+1], ptfCross )) 
		{
			if (ptfS.x != -65535 && ptfS.y != -65535)
			{
				if ((ptfCross.x - ptfS.x) * (ptfCross.x - ptfS.x) + (ptfCross.y - ptfS.y) * (ptfCross.y - ptfS.y) > 0.01f*g_fScaleAAA) 
				{
					ptfE.x = ptfCross.x;
					ptfE.y = ptfCross.y;
				}
			}
			else
			{
				ptfS.x = ptfCross.x;
				ptfS.y = ptfCross.y;
			}

		}
	}
	if (ptfS.x != -65535 && ptfS.y != -65535 &&
		ptfE.x != -65535 && ptfE.y != -65535) 
	{
		return TRUE;
	}
	return FALSE;
} // END OF CutLine

//////////////////////////////////////////////////////////////////////////
// 输出：若有交点，则输出交点
// 返回：若有交点，返回ＴＲＵＥ；否则返回ＦＡＬＳＥ
//
BOOL SYBevelPoly::LineCrossLineSeg(IN float fA, IN float fB, IN float fC,
					  IN pointf ptfS, IN pointf ptfE, 
					  OUT pointf & ptfCross )
{
	float fErr = 0.00001f;
	pointf V; 
	float fD = sqrt( (float)( (ptfE.x - ptfS.x)*(ptfE.x - ptfS.x) + (ptfE.y - ptfS.y)*(ptfE.y - ptfS.y) ) );
	float fdD = 1.f / fD;
	V.x = (ptfE.x - ptfS.x) * fdD;
	V.y = (ptfE.y - ptfS.y) * fdD;
	float fTemp = fA * V.x + fB * V.y;
	if (fTemp < 0.00001 && fTemp > -0.00001 ) 
	{
		return FALSE;
	}

	float k = (-fA * ptfS.x - fB * ptfS.y - fC) / fTemp;
	if (k>=-fErr && k<=fD + fErr) 
	{
		ptfCross.x = ptfS.x + k * V.x;
		ptfCross.y = ptfS.y + k * V.y;
		return TRUE;
	}
	return FALSE;
} // END OF LineCrossLineSeg

void SYBevelPoly::GetMinMaxXY(OUT float &fMinX, OUT float &fMaxX, OUT float &fMinY, OUT float &fMaxY,
						  IN pointf ptfInS, IN pointf ptfInE, IN pointf ptfS, IN pointf ptfE)
{
	fMinX = fMaxX = ptfInS.x;
	fMinY = fMaxY = ptfInS.y;

	if (ptfInE.x < fMinX) fMinX = ptfInE.x;
	if (ptfInE.x > fMaxX) fMaxX = ptfInE.x;
	if (ptfInE.y < fMinY) fMinY = ptfInE.y;
	if (ptfInE.y > fMaxY) fMaxY = ptfInE.y;

	if (ptfS.x < fMinX) fMinX = ptfS.x;
	if (ptfS.x > fMaxX) fMaxX = ptfS.x;
	if (ptfS.y < fMinY) fMinY = ptfS.y;
	if (ptfS.y > fMaxY) fMaxY = ptfS.y;

	if (ptfE.x < fMinX) fMinX = ptfE.x;
	if (ptfE.x > fMaxX) fMaxX = ptfE.x;
	if (ptfE.y < fMinY) fMinY = ptfE.y;
	if (ptfE.y > fMaxY) fMaxY = ptfE.y;
} // END OF GetMinMaxXY

BOOL SYBevelPoly::SamePtf(pointf ptf1, pointf ptf2)
{
	float fD = (ptf1.x - ptf2.x) * (ptf1.x - ptf2.x) + (ptf1.y - ptf2.y) * (ptf1.y - ptf2.y);
	if (fD < 0.1*g_fScaleAAA) 
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
} // end of SamePtf

BOOL SYBevelPoly::SingleLineIntersection(
									 IN pointf startPt1, 
									 IN pointf endPt1, 
									 IN pointf startPt2, 
									 IN pointf endPt2,
									 OUT pointf &ptCross
										)
{
	pointf v1,v2;
	v1.x = endPt1.x - startPt1.x;
	v1.y = endPt1.y - startPt1.y;
	v2.x = endPt2.x - startPt2.x;
	v2.y = endPt2.y - startPt2.y;

	float fDenominator = v1.x * v2.y - v1.y * v2.x;
	if ( abs(fDenominator)< 0.000001 ) 
	{
		return FALSE;
	}

	if (SamePtf(startPt1, startPt2) || SamePtf(startPt1, endPt2) )
	{
		ptCross.x = startPt1.x ;
		ptCross.y = startPt1.y ;
		return TRUE;
	}
	if (SamePtf(endPt1, startPt2) || SamePtf(endPt1, endPt2) )
	{
		ptCross.x = endPt1.x ;
		ptCross.y = endPt1.y ;
		return TRUE;
	}

	float fScale = v1.y *(startPt2.x - startPt1.x) - v1.x *(startPt2.y - startPt1.y);
	if(fDenominator>0) 
	{
		if (fScale >= 0 && fScale<= fDenominator/* - 0.00001*/) 
		{
			fScale = v2.y *(startPt2.x - startPt1.x) - v2.x *(startPt2.y - startPt1.y);
			if(fScale >= 0 && fScale<= fDenominator/* - 0.00001*/) 
			{
				fScale /= fDenominator;
				ptCross.x = startPt1.x + fScale*(endPt1.x - startPt1.x);
				ptCross.y = startPt1.y + fScale*(endPt1.y - startPt1.y);
				return TRUE;
			}
		}
	}
	else
	{
		if (fScale <= 0 && fScale>= fDenominator) 
		{
			fScale = v2.y *(startPt2.x - startPt1.x) - v2.x *(startPt2.y - startPt1.y);
			if(fScale <= 0 && fScale>= fDenominator) 
			{
				fScale /= fDenominator;
				ptCross.x = startPt1.x + fScale*(endPt1.x - startPt1.x);
				ptCross.y = startPt1.y + fScale*(endPt1.y - startPt1.y);
				return TRUE;
			}
		}
	}

	return FALSE;

}//End of SingleLineIntersection

void SYBevelPoly::GetAngle(IN pointf a, IN pointf b, OUT float & angle)
{
	float fZero = 0.00001f;
	float ra = sqrtf(a.x * a.x + a.y * a.y);
	float rb = sqrtf(b.x * b.x + b.y * b.y);
	float dotMuti = a.x * b.x + a.y * b.y;
	if ( dotMuti > fZero || dotMuti < -fZero) 
	{
		angle = acos( ra * rb / dotMuti );
	}
	else
	{
		angle = PAI_HALFF; 
	}
} // END OF GetAngle
