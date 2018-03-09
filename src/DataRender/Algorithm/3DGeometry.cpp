#include "stdafx.h"
#include "DataRender\3DGeometry.h"
#include "DataRender\AlgorithmRip.h"
#include "DataRender\OffsetCurve.h"
#include "DataRender\DataRenderDataDef.h"
#include "DataRender\BevelPoly.h"
#include "DataRender\DataRenderUtityFunc.h"
#include "DataRender\TriangulateApp.h"
#include "DataRender\3DPieModeling.h"
#include <vector>
#include <stdio.h>
#include <atlstr.h>
#include <list>
#include <algorithm>
using std::vector;

struct SYTriangle
{
	int uiIndex[3];
	float3 tangent;
	float3 binormal;
};
enum SYModel_PLANE_VERTEX_TYPE
{
	SY3DPLANE_XY = 0,
	SY3DPLANE_XZ,
	SY3DPLANE_YZ,
};
class Autolock;
class ArrayVer
{
public:
	ArrayVer(LPCRITICAL_SECTION plock)
	{
		m_pArVerOfTri = NULL;
		m_nVNum = 0;
		m_pTriangle = NULL;
		m_nFaceNum = 0;
		m_pLock = plock;
		InitializeCriticalSection(m_pLock);
	}
	~ArrayVer()
	{
		SAFEDELETE_ARRAY(m_pArVerOfTri);
		SAFEDELETE_ARRAY(m_pTriangle);
		DeleteCriticalSection(m_pLock); 
	}
	CSYIntArray*	m_pArVerOfTri;
	int				m_nVNum;
	SYTriangle*		m_pTriangle;
	int				m_nFaceNum;
	LPCRITICAL_SECTION m_pLock;
};
static CRITICAL_SECTION g_csLock;
static ArrayVer g_arVer(&g_csLock);
int CalFaceNum(SYTriType nType,int nINum)
{
	switch(nType)
	{
	case SY_TRIANGLE_LIST:
		return nINum / 3;
	case SY_TRIANGLE_STRIP:
		return nINum - 2;
	case SY_TRIANGLE_FAN:
		return nINum - 2;
	}
	return 0;
}
void GetTriIndex(SYTriangle* pTriangle,DWORD* pIndex,int nFaceIndex,SYTriType nType,int nVStart)
{
	if(nType == SY_TRIANGLE_LIST)
	{
		int nStartIndex = nFaceIndex * 3;
		pTriangle->uiIndex[0] = pIndex[nStartIndex ] - nVStart;
		pTriangle->uiIndex[1] = pIndex[nStartIndex + 1] - nVStart;
		pTriangle->uiIndex[2] = pIndex[nStartIndex + 2] - nVStart;
	}
	else if(nType == SY_TRIANGLE_STRIP)
	{
		pTriangle->uiIndex[0] = pIndex[nFaceIndex] - nVStart;
		pTriangle->uiIndex[1] = pIndex[nFaceIndex + 1] - nVStart;
		pTriangle->uiIndex[2] = pIndex[nFaceIndex + 2] - nVStart;
	}
	else if(nType == SY_TRIANGLE_FAN)
	{
		int nStartIndex = nFaceIndex + 1;
		pTriangle->uiIndex[0] = pIndex[0] - nVStart;
		pTriangle->uiIndex[1] = pIndex[nStartIndex] - nVStart;
		pTriangle->uiIndex[2] = pIndex[nStartIndex + 1] - nVStart;
	}
}
void CalTriFrame(SYTriangle* pTriangle,SYVertex3D* pVertex)
{
	if(!pTriangle)
		return;
	SYVertex3D* pV[3]={NULL,NULL,NULL};
	pV[0] = &(pVertex[pTriangle->uiIndex[0]]);
	pV[1] = &(pVertex[pTriangle->uiIndex[1]]);
	pV[2] = &(pVertex[pTriangle->uiIndex[2]]);

	float3 P = pV[1]->vDyn - pV[0]->vDyn;
	float3 Q = pV[2]->vDyn - pV[0]->vDyn;
	float s1 = pV[1]->tu - pV[0]->tu;
	float t1 = pV[1]->tv - pV[0]->tv;
	float s2 = pV[2]->tu - pV[0]->tu;
	float t2 = pV[2]->tv - pV[0]->tv;

	float ftemp = 0.0f;
	if(fabsf(s1*t2 - s2*t1) <= ABS_FLOAT_0)
	{
		ftemp = 1.0f;
	}
	else
	{
		ftemp = 1.0f/(s1*t2 - s2*t1 );
	}

	pTriangle->tangent = t2 * P - t1 * Q;

	pTriangle->tangent = pTriangle->tangent*ftemp;
	pTriangle->binormal = s1 * Q - s2 * P;

	pTriangle->binormal = pTriangle->binormal*ftemp;
}
void OrthTangentsAndBinormals(int nVNum,SYVertex3D* pVertex)
{
	for(int i=0; i < nVNum; i++ )
	{
		float3 tmpTan = pVertex[i].tangent;
		float3 tmpNorm = pVertex[i].vNorm;
		float3 tmpBin = pVertex[i].binormal;


		float3 newT = tmpTan - (float3DotProduct(&tmpNorm , &tmpTan)  * tmpNorm );
		float3 newB = tmpBin - (float3DotProduct(&tmpNorm , &tmpBin) * tmpNorm)
			- (float3DotProduct(&newT,&tmpBin)*newT);

		if(newT.x != 0 || newT.y != 0 || newT.z != 0)
			float3Normalize(&(pVertex[i].tangent), &newT);
		else
			float3Set(&pVertex[i].tangent,0,0,0);
		if(newB.x != 0 || newB.y != 0 || newB.z != 0)
			float3Normalize(&(pVertex[i].binormal), &newB);		
		else
			float3Set(&pVertex[i].binormal,0,0,0);

		float lenTan = float3Mode(&(pVertex[i].tangent));
		float lenBin = float3Mode(&(pVertex[i].binormal));

		if( (lenTan <= ABS_FLOAT_0) || (lenBin <= ABS_FLOAT_0)  ) //should be approx 1.0f
		{	
			if(lenTan > 0.5f)
			{
				float3CrossProduct(&(pVertex[i].binormal), &(pVertex[i].vNorm), &(pVertex[i].tangent) );

			}
			else if(lenBin > 0.5)
			{
				float3CrossProduct(&(pVertex[i].tangent), &(pVertex[i].binormal), &(pVertex[i].vNorm) );
			}
			else
			{
				static float3 xAxis = { 1.0f , 0.0f , 0.0f};
				static float3 yAxis = { 0.0f , 1.0f , 0.0f};
				float3 startAxis;

				if( float3DotProduct(&xAxis, &(pVertex[i].vNorm) )  <  float3DotProduct(&yAxis, &(pVertex[i].vNorm) ) )
				{
					startAxis = xAxis;
				}
				else
				{
					startAxis = yAxis;
				}

				float3CrossProduct(&(pVertex[i].tangent), &(pVertex[i].vNorm), &startAxis );
				float3CrossProduct(&(pVertex[i].binormal), &(pVertex[i].vNorm), &(pVertex[i].tangent) );
			}
		}
		else
		{
			if( float3DotProduct(&(pVertex[i].binormal), &(pVertex[i].tangent) )  > 0.999f )
				float3CrossProduct(&(pVertex[i].binormal), &(pVertex[i].vNorm), &(pVertex[i].tangent) );
		}
	}
}
typedef std::vector<SYVertex3D> CurvePtArray;

void CreatePlaneBevelFace(float3* pV,float* pF,UINT* pDIM,SYVertex3D* pVB,UINT& uiVBIndex,DWORD* pIB,UINT& uiIBIndex,float fLength,float fHeight,float fThick,int nDivision);
void CreatePlaneBevelFace2(float3* pV,float* pF,UINT* pDIM,BOOL bBevel,SYVertex3D* pVB,UINT& uiVBIndex,DWORD* pIB,UINT& uiIBIndex,float fLength,float fHeight,float fThick,int nDivision);
void CreateNormalPrimitive(SYVertex3D* pVertex,SYVertex3D** ppVB,LPDWORD* ppIB,UINT& uiVBIndex,UINT& uiIBIndex);

void CreateBevelSphere(UINT uiPrecision,SYVertex3D** ppVB,LPDWORD* ppIB,UINT& nF,UINT& nV,BOOL bCenterSplitMode);
void CreateBevelCylinder(UINT uiPrecision,UINT uiSegments,SYVertex3D** ppVB,LPDWORD* ppIB,UINT& nF,UINT& nV,BOOL bCenterSplitMode);
BOOL  SYCreateTangent(SYVertex3D* pVertex,int nVNum,DWORD* pIndex,int nINum,int nVStart,SYTriType nType)
{
	// 防止多个物体同时操作时冲突
	Autolock lock(&g_csLock);
	int nFaceNum = CalFaceNum(nType,nINum);
	// nFaceNum, vNum数量比较大时，减少开辟空间所用时间
	if (g_arVer.m_pTriangle == NULL || g_arVer.m_nFaceNum != nFaceNum)
	{
		SAFEDELETE_ARRAY(g_arVer.m_pTriangle);
		g_arVer.m_pTriangle = new SYTriangle[nFaceNum];
		g_arVer.m_nFaceNum = nFaceNum;
	}
	SYTriangle* pTriangle = g_arVer.m_pTriangle;
	if (g_arVer.m_pArVerOfTri == NULL || g_arVer.m_nVNum != nVNum)
	{
		SAFEDELETE_ARRAY(g_arVer.m_pArVerOfTri);
		g_arVer.m_nVNum = nVNum;
		g_arVer.m_pArVerOfTri = new CSYIntArray[nVNum];
		for (int i = 0; i < nVNum;i++)
		{
			(g_arVer.m_pArVerOfTri + i)->SetSize(10);
			(g_arVer.m_pArVerOfTri + i)->SetCurSize(0);
		}	
	}
	CSYIntArray* pArVerOfTri = g_arVer.m_pArVerOfTri;//每个顶点对应的第几个三角形面
	for(int i = 0; i< nFaceNum;i++)
	{
		GetTriIndex(pTriangle + i,pIndex,i,nType,nVStart);
		CalTriFrame(pTriangle + i,pVertex);
	}
	for (int i = 0; i < nVNum;i++)
	{
		(pArVerOfTri + i)->SetCurSize(0);
	}
 	for (int k=0; k<3;k++)
 	{
 		for(int i = 0; i < nFaceNum;i++)
 		{	
 			assert(pTriangle[i].uiIndex[k] < nVNum);
 			(pArVerOfTri + pTriangle[i].uiIndex[k])->Add(i);
 		}
 	}

	float3 tempV,tempV2;
	for(int i = 0; i < nVNum; i++)
	{
		int nSize = (int)(pArVerOfTri + i)->GetSize();

		if(nSize == 0)
			continue;
		int nFaceIndex = (int)(pArVerOfTri + i)->GetAt(0);//(int)SYAR_GetAt(pArVerOfTri + i,0);
		tempV = pTriangle[nFaceIndex].tangent;
		tempV2 = pTriangle[nFaceIndex].binormal;
		for(int k = 1; k < nSize; k++)
		{
			int nFaceIndex = (int)(pArVerOfTri + i)->GetAt(k);//(int)SYAR_GetAt(pArVerOfTri + i,k);
			assert(nFaceIndex < nFaceNum);
			tempV += pTriangle[nFaceIndex].tangent;
			tempV2 += pTriangle[nFaceIndex].binormal;
		}
		if(tempV.x != 0 || tempV.y != 0 || tempV.z != 0)
			float3Normalize(&tempV,&tempV);
		float3Normalize(&tempV2,&tempV2);
		pVertex[i].tangent = tempV;
		pVertex[i].binormal = tempV2;
	}
	OrthTangentsAndBinormals(nVNum,pVertex);
	//SAFEDELETE_ARRAY(pTriangle);
	//for(int i = 0; i < nVNum;i++)
	//{
	//	//SYAR_RemoveAll(pArVerOfTri + i);
	//	(pArVerOfTri + i)->RemoveAll();
	//}
	//SAFEDELETE_ARRAY(pArVerOfTri);
	return TRUE;
}
void NewVertexIndex( IN OUT SYModel* pMod, IN int nGraphType , int nLevel = 1)
{
	int nSubPartCount = 0, nVertexCount = 0, nIndexCount = 0;

	const SYSubModel* pSubDes = NULL;
	int sphereVertexNum = g_nSphere_Vertex_Num * nLevel * nLevel;
	int sphereIndexNum = ( g_nSphere_Circle_Num * nLevel - 1 ) * ( g_nSphere_Per_Circle_VertexNum * nLevel - 1 ) * 6;
	SYSubModel stuSphereSubs[SPHERE_MATERIAL_COUNT] = 
	{
		{ 0, sphereVertexNum, 0, sphereIndexNum, SY_TRIANGLE_LIST, sphereIndexNum / 3, SYHTable_null },
	};

	switch (nGraphType)
	{
	case SYModelRENDER_CONE:
		nSubPartCount = CONE_MATERIAL_COUNT;
		pSubDes       = g_stuConeSubs;
		break;
	case SYModelRENDER_SEGMENT:
		nSubPartCount = SEGMENT_MATERIAL_COUNT;
		pSubDes       = g_stuSegmentSubs;
		break;
	case SYModelRENDER_3DFACETS:
		nSubPartCount = FACETS_MATERIAL_COUNT;
		pSubDes       = g_stu3DFacetsSubs;
		break;
	case SYModelRENDER_SPHERE:
		nSubPartCount = SPHERE_MATERIAL_COUNT;
		pSubDes       = stuSphereSubs;
		break;
	case SYModelRENDER_QUAD:
		nSubPartCount = QUAD_MATERIAL_COUNT;
		pSubDes       = g_stuQuadSubs;
		break;
	case SYModelRENDER_BOXWIDTHBEVEL:
		nSubPartCount = BOXWITHBEVEL_MATERIAL_COUNT;
		pSubDes       = g_stuBoxWidthBevelSubs;
		break;
	case SYModelRENDER_CONEWIDTHBEVEL:
		nSubPartCount = CONEWITHBEVEL_MATERIAL_COUNT;
		pSubDes       = g_stuConeWidthBevelSubs;
		break;
	case SYModelRENDER_TUBE:
		nSubPartCount	= TUBE_MATERIAL_COUNT;
		pSubDes			= g_stuTubeSubs;
		break;
	}

	pMod->nviewsub = nSubPartCount;//子部分
	pMod->viewsub  = new SYSubModel[pMod->nviewsub];
	memset( pMod->viewsub, 0, pMod->nviewsub * sizeof(SYSubModel) );

	for ( int i = 0; i < nSubPartCount; i++ )
	{
		memcpy( &pMod->viewsub[i], &pSubDes[i], sizeof(SYSubModel) );
		nVertexCount += pMod->viewsub[i].vcount;
		nIndexCount  += pMod->viewsub[i].icount;
	}

	pMod->vcount = nVertexCount;
	pMod->v      = new SYVertex3D[pMod->vcount];
	memset( pMod->v, 0, pMod->vcount * sizeof(SYVertex3D) );
	pMod->icount = nIndexCount;//总的索引数
	pMod->indecs = new DWORD[pMod->icount];
	memset( pMod->indecs, 0, pMod->icount * sizeof(DWORD) );
};
void SetPlaneIndexAndUV( IN OUT SYModel* pMod, IN int nIndexSt, IN int nVertexSt, IN int nRowNum, IN int nColumnNum, IN int nType )
{
	int nTriIndex = 0;
	//01
	//23
	if ( nType == SY3DPLANE_ROW_FIRST )
	{
		for ( int i = 1; i < nRowNum; i++ )
		{
			for ( int j = 1; j < nColumnNum; j++ )
			{
				nTriIndex = ( i - 1 ) * ( nColumnNum - 1 ) + ( j - 1 );

				pMod->indecs[ nIndexSt + 6 * nTriIndex + 0 ] = ( i - 1 ) * nColumnNum + ( j - 1 ) + nVertexSt;
				pMod->indecs[ nIndexSt + 6 * nTriIndex + 1 ] = ( i - 1 ) * nColumnNum + ( j - 0 ) + nVertexSt;
				pMod->indecs[ nIndexSt + 6 * nTriIndex + 2 ] = ( i - 0 ) * nColumnNum + ( j - 0 ) + nVertexSt;

				pMod->indecs[ nIndexSt + 6 * nTriIndex + 3 ] = ( i - 1 ) * nColumnNum + ( j - 1 ) + nVertexSt;
				pMod->indecs[ nIndexSt + 6 * nTriIndex + 4 ] = ( i - 0 ) * nColumnNum + ( j - 0 ) + nVertexSt;
				pMod->indecs[ nIndexSt + 6 * nTriIndex + 5 ] = ( i - 0 ) * nColumnNum + ( j - 1 ) + nVertexSt;
			}
		}

		for ( int i = 0; i < nRowNum; i++ )
		{
			for ( int j = 0; j < nColumnNum; j++ )
			{
				pMod->v[ nVertexSt + i * nColumnNum + j ].tu  = (float)j / ( nColumnNum - 1 );
				pMod->v[ nVertexSt + i * nColumnNum + j ].tv  = (sin((-0.5 + (float)i / ( nRowNum - 1 )) * PAI) + 1) / 2;
			}
		}
	}
	//02
	//13
	else if ( nType == SY3DPLANE_COLUMN_FIRST )
	{
		for ( int j = 1; j < nColumnNum; j++ )
		{
			for ( int i = 1; i < nRowNum; i++ )
			{
				nTriIndex = ( j - 1 ) * ( nRowNum - 1 ) + ( i - 1 );

				pMod->indecs[ nIndexSt + 6 * nTriIndex + 0 ] = ( j - 1 ) * nRowNum + ( i - 1 ) + nVertexSt;
				pMod->indecs[ nIndexSt + 6 * nTriIndex + 1 ] = ( j - 0 ) * nRowNum + ( i - 1 ) + nVertexSt;
				pMod->indecs[ nIndexSt + 6 * nTriIndex + 2 ] = ( j - 1 ) * nRowNum + ( i - 0 ) + nVertexSt;

				pMod->indecs[ nIndexSt + 6 * nTriIndex + 3 ] = ( j - 1 ) * nRowNum + ( i - 0 ) + nVertexSt;
				pMod->indecs[ nIndexSt + 6 * nTriIndex + 4 ] = ( j - 0 ) * nRowNum + ( i - 1 ) + nVertexSt;
				pMod->indecs[ nIndexSt + 6 * nTriIndex + 5 ] = ( j - 0 ) * nRowNum + ( i - 0 ) + nVertexSt;
			}
		}

		for ( int j = 0; j < nColumnNum; j++ )
		{
			for ( int i = 0; i < nRowNum; i++ )
			{
				pMod->v[ nVertexSt + j * nRowNum + i ].tu  = (float)j / ( nColumnNum - 1 );
				pMod->v[ nVertexSt + j * nRowNum + i ].tv  = (float)i / ( nRowNum - 1 );
			}
		}
	}
};

BOOL SYInitialSegment(IN OUT SYModel* pMod)
{
	if ( pMod->indecs == NULL )
	{
		NewVertexIndex( pMod, SYModelRENDER_SEGMENT );
		SetPlaneIndexAndUV( pMod, g_stuSegmentSubs[SEGMENT_MATERIAL_PARAMCHG].istart, g_stuSegmentSubs[SEGMENT_MATERIAL_PARAMCHG].vstart, SY_SEGMENT_ROW_NUM, SY_SEGMENT_COLUMN_NUM, 
			SY3DPLANE_ROW_FIRST );
	}
	return TRUE;
}
BOOL SYInitialBevel(IN OUT SYModel* pMod)
{
	if ( pMod->viewsub == NULL )
	{
		pMod->nviewsub = 1;
		pMod->viewsub  = new SYSubModel[pMod->nviewsub];
		memset( pMod->viewsub, 0, pMod->nviewsub * sizeof(SYSubModel) );
		for ( int i = 0; i < pMod->nviewsub; i++ )
		{
			pMod->viewsub[i].nTri_Type = SY_TRIANGLE_LIST;
		}
	}
	return TRUE;
}

void ModifyVertexIndex( IN OUT SYModel* pMod, IN int nGraphType , int nLevel = 1)
{
	int newVertexNum = 0, newIndexNum = 0, newTriNum = 0;
	switch (nGraphType)
	{
	case SYModelRENDER_SPHERE:
		newVertexNum = g_nSphere_Vertex_Num * nLevel * nLevel;
		newIndexNum =( g_nSphere_Circle_Num * nLevel - 1 ) * ( g_nSphere_Per_Circle_VertexNum * nLevel - 1 ) * 6; 
		newTriNum = newIndexNum / 3;
		if (newIndexNum == pMod->icount)
			return;
		break;
	default:
		return;
	}
 	SAFE_DELETE(pMod->indecs);
 	SAFE_DELETE(pMod->v);

	pMod->vcount = newVertexNum ;
	pMod->v      = new SYVertex3D[pMod->vcount];
	memset( pMod->v, 0, pMod->vcount * sizeof(SYVertex3D) );
	pMod->icount = newIndexNum;//总的索引数
	pMod->indecs = new DWORD[pMod->icount];
	memset( pMod->indecs, 0, pMod->icount * sizeof(DWORD) );
}
BOOL SYInitialSphere(IN OUT SYModel* pMod, int nLevel /* = 1*/)
{
	if ( pMod->indecs != NULL ){
		ModifyVertexIndex(pMod,SYModelRENDER_SPHERE,nLevel);
	}
	else
		NewVertexIndex( pMod, SYModelRENDER_SPHERE ,nLevel);
	SetPlaneIndexAndUV( pMod, g_stuSphereSubs[SPHERE_MATERIAL_PARAMCHG].istart, g_stuSphereSubs[SPHERE_MATERIAL_PARAMCHG].vstart, 
		g_nSphere_Circle_Num * nLevel, (SY_SPHERE_CIRCLE_SEGMENT + 1) * nLevel, SY3DPLANE_ROW_FIRST );

	return TRUE;
}

void SetSegmentPoint( IN OUT SYVertex3D* pPlanePt, IN int nRowNum, IN int nColumnNum, IN float fLeftWidth, IN float fWidth, IN float fTopHeight, IN float fHeight, IN vector3f* pvNorm, IN float fValue, IN int nType )
{
	if ( nType == SY3DPLANE_XY )//行优先
	{
		for( int i = 0; i < nRowNum; i++ )//第i行
		{
			for( int j = 0; j < nColumnNum; j++ )//第j列
			{
				pPlanePt[ i * nColumnNum + j ].x   = fLeftWidth + j * fWidth / ( nColumnNum - 1 );
				pPlanePt[ i * nColumnNum + j ].y   = fTopHeight - i * fHeight / ( nRowNum - 1 );
				pPlanePt[ i * nColumnNum + j ].z   = fValue;
			}
		}
	}
	else if ( nType == SY3DPLANE_XZ )
	{
		for( int i = 0; i < nRowNum; i++ )
		{
			for( int j = 0; j < nColumnNum; j++ )
			{
				pPlanePt[ i * nColumnNum + j ].x   = fLeftWidth + j * fWidth / ( nColumnNum - 1 );
				pPlanePt[ i * nColumnNum + j ].y   = fValue;
				pPlanePt[ i * nColumnNum + j ].z   = fTopHeight - i * fHeight / ( nColumnNum - 1 );

			}
		}
	}
	for( int i = 0; i < nRowNum; i++ )
	{
		for( int j = 0; j < nColumnNum; j++ )
		{
			pPlanePt[ i * nColumnNum + j ].nx  = pvNorm->x;
			pPlanePt[ i * nColumnNum + j ].ny  = pvNorm->y;
			pPlanePt[ i * nColumnNum + j ].nz  = pvNorm->z;
		}
	}
}
BOOL SYCreateSegmentGraph( IN OUT SYModel* pMod, IN float fLeftWidth, IN float fRightWidth, IN float fTopHeight, IN float fBottomHeight )
{
	float fWidth = fRightWidth - fLeftWidth, fHeight = fTopHeight - fBottomHeight;
	vector3f vNorm = { 0.f, 0.f, -1.f };
	SetSegmentPoint( &pMod->v[g_stuSegmentSubs[SEGMENT_MATERIAL_PARAMCHG].vstart], SY_SEGMENT_ROW_NUM, SY_SEGMENT_COLUMN_NUM, fLeftWidth, fWidth, fTopHeight, fHeight, &vNorm, 0.f, SY3DPLANE_XY );
	return TRUE;
}
void SY2DCoordTo3D(pointf* p2DCoordPt,point3f* p3DCoordPt,point3f* pOffset)
{
	float3 v2DPt = {p2DCoordPt->x,-p2DCoordPt->y,0};
	*p3DCoordPt = v2DPt - *pOffset; 
}

int GetMinYPt(IN vectorcell* pCurveCell )
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

BOOL OutIsLeft(IN pointf pt0,IN pointf pt1,IN pointf pt2)
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

BOOL IsAntiClockwise(IN vectorcell* pCurveCell )
{
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
BOOL CalOuterClockwise(vectorgraph* pFigure)
{
	vectorcell* pCurveCell = NULL;

	float fMaxCellWidth   = 0.0f;
	float fCurCellWidth   = 0.0f;
	int nOutCell          = 0;

	for( int i = 0; i < pFigure->ncell; i++ )
	{
		pCurveCell = &pFigure->cell[i];
		fCurCellWidth = pCurveCell->bound.right - pCurveCell->bound.left;
		if (fMaxCellWidth < fCurCellWidth ) 
		{
			fMaxCellWidth = fCurCellWidth;
			nOutCell = i;
		}
	}
	BOOL bAntiClockwise = IsAntiClockwise(&pFigure->cell[nOutCell]);
	return bAntiClockwise;
}
void CreateF(IN vectorgraph * pFigure, IN OUT CSYPoint3fPtrArray* pptrTri, IN OUT CSYPoint3fPtrArray* pptrTriNorm,IN int nBevelTriSize, IN float fBevelHeight, IN float fFlankHeight, IN const int nBevelType, IN const BOOL bAntiClockwise )
{
	int i, j;
	int nSize;
	float fZero = 0.002f;
	pointf * pptf1 = NULL;
	pointf * pptf2 = NULL;
	vectorcell* pCurveCell = NULL;
	point3f* lpPtf3D1 = NULL,*lpPtf3D2 = NULL,*lpPtfUV1 = NULL,*lpPtfUV2 = NULL;
	point3f* lpPtf3DNew = NULL;
	point3f prvnormal = {0,0,0};
	float fTotalHeight = ( 2 * fBevelHeight + fFlankHeight ) ;
	if (nBevelType == 1 || nBevelType == 3 || nBevelType == 4) 
	{
		for (i=1; i<nBevelTriSize; i+=3) 
		{
			lpPtf3D1 = (point3f*)pptrTri->GetAt(i);
			lpPtf3D2 = (point3f*)pptrTri->GetAt(i+1);

			if ( lpPtf3D1->y >= fBevelHeight - fZero && lpPtf3D2->y >= fBevelHeight  - fZero
				|| lpPtf3D1->y <= -fBevelHeight  + fZero && lpPtf3D2->y <= -fBevelHeight  + fZero
				) 
			{
				// 第一个
				lpPtf3DNew = new point3f;
				*lpPtf3DNew = *lpPtf3D1;
				pptrTri->Add(lpPtf3DNew);

				lpPtf3DNew = new point3f;
				*lpPtf3DNew = *lpPtf3D1;
				lpPtf3DNew->y = -( fTotalHeight + lpPtf3D1->y );
				pptrTri->Add(lpPtf3DNew);

				lpPtf3DNew = new point3f;
				*lpPtf3DNew = *lpPtf3D2;
				lpPtf3DNew->y = -( fTotalHeight + lpPtf3D2->y );
				pptrTri->Add(lpPtf3DNew);

				// 第二个
				lpPtf3DNew = new point3f;
				*lpPtf3DNew = *lpPtf3D1;
				pptrTri->Add(lpPtf3DNew);

				lpPtf3DNew = new point3f;
				*lpPtf3DNew = *lpPtf3D2;
				lpPtf3DNew->y = -( fTotalHeight + lpPtf3D2->y );
				pptrTri->Add(lpPtf3DNew);

				lpPtf3DNew = new point3f;
				*lpPtf3DNew = *lpPtf3D2;
				pptrTri->Add(lpPtf3DNew);


				// 法线：令ｙ＝０
				lpPtf3D1 = (point3f*)pptrTriNorm->GetAt(i);
				lpPtf3D2 = (point3f*)pptrTriNorm->GetAt(i+1);

				if(abs(lpPtf3D1->x - prvnormal.x) > ABS_FLOAT_0 
					|| abs(lpPtf3D1->y - prvnormal.y) > ABS_FLOAT_0
					||abs(lpPtf3D1->z - prvnormal.z) > ABS_FLOAT_0)
				{
					int a = 0;
				}
				prvnormal = *lpPtf3D2;
				// 第一个
				lpPtf3DNew = new point3f;
				lpPtf3DNew->x = lpPtf3D1->x;
				lpPtf3DNew->y = 0;
				lpPtf3DNew->z = lpPtf3D1->z;
				float3Normalize( lpPtf3DNew, lpPtf3DNew );
				pptrTriNorm->Add(lpPtf3DNew);

				lpPtf3DNew = new point3f;
				lpPtf3DNew->x = lpPtf3D1->x;
				lpPtf3DNew->y = 0;
				lpPtf3DNew->z = lpPtf3D1->z;
				float3Normalize( lpPtf3DNew, lpPtf3DNew );
				pptrTriNorm->Add(lpPtf3DNew);

				lpPtf3DNew = new point3f;
				lpPtf3DNew->x = lpPtf3D2->x;
				lpPtf3DNew->y = 0;
				lpPtf3DNew->z = lpPtf3D2->z;
				float3Normalize( lpPtf3DNew, lpPtf3DNew );
				pptrTriNorm->Add(lpPtf3DNew);

				// 第二个
				lpPtf3DNew = new point3f;
				lpPtf3DNew->x = lpPtf3D1->x;
				lpPtf3DNew->y = 0;
				lpPtf3DNew->z = lpPtf3D1->z;
				float3Normalize( lpPtf3DNew, lpPtf3DNew );
				pptrTriNorm->Add(lpPtf3DNew);

				lpPtf3DNew = new point3f;
				lpPtf3DNew->x = lpPtf3D2->x;
				lpPtf3DNew->y = 0;
				lpPtf3DNew->z = lpPtf3D2->z;
				float3Normalize( lpPtf3DNew, lpPtf3DNew );
				pptrTriNorm->Add(lpPtf3DNew);

				lpPtf3DNew = new point3f;
				lpPtf3DNew->x = lpPtf3D2->x;
				lpPtf3DNew->y = 0;
				lpPtf3DNew->z = lpPtf3D2->z;
				float3Normalize( lpPtf3DNew, lpPtf3DNew );
				pptrTriNorm->Add(lpPtf3DNew);

			}
		} // end of for
	}
	else if (nBevelType == 2) 
	{
		for (i=1; i<nBevelTriSize; i+=3) 
		{
			lpPtf3D1 = (point3f*)pptrTri->GetAt(i);
			lpPtf3D2 = (point3f*)pptrTri->GetAt(i+1);
			if ( lpPtf3D1->y >= fBevelHeight  - fZero && lpPtf3D2->y >= fBevelHeight  - fZero
				|| lpPtf3D1->y <= -fBevelHeight  + fZero && lpPtf3D2->y <= -fBevelHeight  + fZero
				) 
			{
				// 第一个
				lpPtf3DNew = new point3f;
				lpPtf3DNew->x = lpPtf3D1->x;
				lpPtf3DNew->y = lpPtf3D1->y;
				lpPtf3DNew->z = lpPtf3D1->z;
				pptrTri->Add(lpPtf3DNew);

				lpPtf3DNew = new point3f;
				lpPtf3DNew->x = lpPtf3D1->x;
				lpPtf3DNew->y = -( fTotalHeight + lpPtf3D1->y );
				lpPtf3DNew->z = lpPtf3D1->z;
				pptrTri->Add(lpPtf3DNew);

				lpPtf3DNew = new point3f;
				lpPtf3DNew->x = lpPtf3D2->x;
				lpPtf3DNew->y = -( fTotalHeight + lpPtf3D2->y );
				lpPtf3DNew->z = lpPtf3D2->z;
				pptrTri->Add(lpPtf3DNew);
				// 第二个
				lpPtf3DNew = new point3f;
				lpPtf3DNew->x = lpPtf3D1->x;
				lpPtf3DNew->y = lpPtf3D1->y;
				lpPtf3DNew->z = lpPtf3D1->z;
				pptrTri->Add(lpPtf3DNew);

				lpPtf3DNew = new point3f;
				lpPtf3DNew->x = lpPtf3D2->x;
				lpPtf3DNew->y = -( fTotalHeight + lpPtf3D2->y );
				lpPtf3DNew->z = lpPtf3D2->z;
				pptrTri->Add(lpPtf3DNew);

				lpPtf3DNew = new point3f;
				lpPtf3DNew->x = lpPtf3D2->x;
				lpPtf3DNew->y = lpPtf3D2->y;
				lpPtf3DNew->z = lpPtf3D2->z;
				pptrTri->Add(lpPtf3DNew);

				// 法线：令ｙ＝０
				lpPtf3D1 = (point3f*)pptrTriNorm->GetAt(i);
				lpPtf3D2 = (point3f*)pptrTriNorm->GetAt(i+1);

				// 第一个
				lpPtf3DNew = new point3f;
				lpPtf3DNew->x = -lpPtf3D1->x;
				lpPtf3DNew->y = 0;
				lpPtf3DNew->z = -lpPtf3D1->z;
				float3Normalize( lpPtf3DNew, lpPtf3DNew );
				pptrTriNorm->Add(lpPtf3DNew);

				lpPtf3DNew = new point3f;
				lpPtf3DNew->x = -lpPtf3D1->x;
				lpPtf3DNew->y = 0;
				lpPtf3DNew->z = -lpPtf3D1->z;
				float3Normalize( lpPtf3DNew, lpPtf3DNew );
				pptrTriNorm->Add(lpPtf3DNew);

				lpPtf3DNew = new point3f;
				lpPtf3DNew->x = -lpPtf3D2->x;
				lpPtf3DNew->y = 0;
				lpPtf3DNew->z = -lpPtf3D2->z;
				float3Normalize( lpPtf3DNew, lpPtf3DNew );
				pptrTriNorm->Add(lpPtf3DNew);

				// 第二个
				lpPtf3DNew = new point3f;
				lpPtf3DNew->x = -lpPtf3D1->x;
				lpPtf3DNew->y = 0;
				lpPtf3DNew->z = -lpPtf3D1->z;

				float3Normalize( lpPtf3DNew, lpPtf3DNew );
				pptrTriNorm->Add(lpPtf3DNew);

				lpPtf3DNew = new point3f;
				lpPtf3DNew->x = -lpPtf3D2->x;
				lpPtf3DNew->y = 0;
				lpPtf3DNew->z = -lpPtf3D2->z;
				float3Normalize( lpPtf3DNew, lpPtf3DNew );
				pptrTriNorm->Add(lpPtf3DNew);

				lpPtf3DNew = new point3f;
				lpPtf3DNew->x = -lpPtf3D2->x;
				lpPtf3DNew->y = 0;
				lpPtf3DNew->z = -lpPtf3D2->z;
				float3Normalize( lpPtf3DNew, lpPtf3DNew );
				pptrTriNorm->Add(lpPtf3DNew);

			}
		} // end of for
	}
	else // 上下底面形成侧面．
	{
		int ndirect;
		if(bAntiClockwise)
			ndirect = 1;
		else
			ndirect = -1;

		int calccount = 1;
		vectorcell cellline[2];
		memset(&cellline,0,sizeof(vectorcell) * 2);
		for( i = 0; i < pFigure->ncell; i++ )
		{
			pCurveCell = &pFigure->cell[i];

			calccount = 1;
			if(pCurveCell->type == close_line)
			{
				calccount = 2;
				SYOffsetCell(&pFigure->cell[i],  // 输入曲线
					NULL,
					pFigure->cell[i].width*0.5f,  // 偏移距离
					&cellline[0], // 输出偏移曲线
					ndirect
					);
				SYOffsetCell(&pFigure->cell[i],  // 输入曲线
					NULL,
					pFigure->cell[i].width*0.5f,  // 偏移距离
					&cellline[1], // 输出偏移曲线
					-ndirect
					);
				cell_reverse(&cellline[1]);
				pCurveCell = &cellline[0];
			}
			else if(pCurveCell->type == uncloss_line)
			{
				calccount = 1;
				if(pCurveCell->width <= 0)
				{
					continue;
				}
				vector2f vOffset = {0,0};
				cell_extend_close(&cellline[0],
					&pFigure->cell[i],
					&vOffset,
					ndirect,
					0);
				pCurveCell = &cellline[0];
			}
			for(int k = 0;k < calccount;k ++)
			{
				// 这里还应该加上一段函数，把cell变成多边形曲线
				SYArray ptrPoly;
				SYArray ptrPolyNorm;
				SYAR_Init(&ptrPoly);
				SYAR_Init(&ptrPolyNorm);
				SYOC_Cell2PtfPoly(pCurveCell, &ptrPoly, &ptrPolyNorm,NULL);
				nSize = (int)SYAR_GetSize(&ptrPoly);
				if (nSize<3) 
				{
					//注意释放
					nSize = (int)SYAR_GetSize(&ptrPoly);
					for (j=0; j<nSize; j++)
					{
						pointf * pptf = (pointf * )SYAR_GetAt(&ptrPoly,j);
						delete pptf;
						pptf = NULL;
					}
					SYAR_RemoveAll(&ptrPoly);

					nSize = (int)SYAR_GetSize(&ptrPolyNorm);
					for (j=0; j<nSize; j++)
					{
						pointf * pptf = (pointf * )SYAR_GetAt(&ptrPolyNorm,j);
						delete pptf;
						pptf = NULL;
					}
					SYAR_RemoveAll(&ptrPolyNorm);

					cell_release(&cellline[k]);
					memset(&cellline[k],0,sizeof(vectorcell));

					continue;
				}

				if (!bAntiClockwise) 
				{
					//AntiPoly(&ptrPoly,&ptrPolyNorm);
				}

				for ( j = 0; j < nSize; j++ )
				{
					pptf1 = (pointf * )SYAR_GetAt(&ptrPoly,j);
					pptf2 = (pointf * )SYAR_GetAt(&ptrPoly,(j+1)%nSize);
					// 第一个面片
					point3f* p1 = new point3f;
					float3fFromfloat2fEx(p1,pptf1,0,Plane_XZ);

					point3f* p2 = new point3f;
					float3fFromfloat2fEx(p2,pptf1,-fFlankHeight,Plane_XZ);

					point3f* p3 = new point3f;
					float3fFromfloat2fEx(p3,pptf2,-fFlankHeight,Plane_XZ);
					if(bAntiClockwise)
					{
						pptrTri->Add(p1);
						pptrTri->Add(p2);
						pptrTri->Add(p3);
					}
					else
					{
						pptrTri->Add(p1);
						pptrTri->Add(p3);
						pptrTri->Add(p2);
					}

					// 第二个面片
					p1 = new point3f;
					float3fFromfloat2fEx(p1,pptf1,0,Plane_XZ);
					p2 = new point3f;
					float3fFromfloat2fEx(p2,pptf2,-fFlankHeight,Plane_XZ);
					p3 = new point3f;
					float3fFromfloat2fEx(p3,pptf2,0,Plane_XZ);
					if(bAntiClockwise)
					{
						pptrTri->Add(p1);
						pptrTri->Add(p2);
						pptrTri->Add(p3);
					}
					else
					{
						pptrTri->Add(p1);
						pptrTri->Add(p3);
						pptrTri->Add(p2);
					}
					// 法线
					pptf1 = (pointf * )SYAR_GetAt(&ptrPolyNorm,2*j);  
					pptf2 = (pointf * )SYAR_GetAt(&ptrPolyNorm,2*j+1);
					// 第一个面片
					p1 = new point3f;
					float3fFromfloat2fEx(p1,pptf1,0,Plane_XZ);
					float3Normalize( (vector3f*)p1, (vector3f*)p1 );

					p2 = new point3f;
					float3fFromfloat2fEx(p2,pptf1,0,Plane_XZ);
					float3Normalize( p2, p2 );

					p3 = new point3f;
					float3fFromfloat2fEx(p3,pptf2,0,Plane_XZ);
					float3Normalize( (vector3f*)p3, (vector3f*)p3 );
					if(bAntiClockwise)
					{
						pptrTriNorm->Add(p1);
						pptrTriNorm->Add(p2);
						pptrTriNorm->Add(p3);
					}
					else
					{
						*p1 = -(*p1);
						*p2 = -(*p2);
						*p3 = -(*p3);
						pptrTriNorm->Add(p1);
						pptrTriNorm->Add(p3);
						pptrTriNorm->Add(p2);
					}


					// 第二个面片
					p1 = new point3f;
					float3fFromfloat2fEx(p1,pptf1,0,Plane_XZ);
					float3Normalize( p1, p1 );

					p2 = new point3f;
					float3fFromfloat2fEx(p2,pptf2,0,Plane_XZ);
					float3Normalize( p2, p2 );

					p3 = new point3f;
					float3fFromfloat2fEx(p3,pptf2,0,Plane_XZ);
					float3Normalize( p3, p3 );

					if(bAntiClockwise)
					{
						pptrTriNorm->Add(p1);
						pptrTriNorm->Add(p2);
						pptrTriNorm->Add(p3);
					}
					else
					{
						*p1 = -(*p1);
						*p2 = -(*p2);
						*p3 = -(*p3);
						pptrTriNorm->Add(p1);
						pptrTriNorm->Add(p2);
						pptrTriNorm->Add(p3);
					}
				}
				// 最后一个点，要特殊处理一下
				{
					pptf1 = (pointf * )SYAR_GetAt(&ptrPoly,nSize - 1);
					pptf2 = (pointf * )SYAR_GetAt(&ptrPoly,0);

					point3f* p1 = new point3f;
					float3fFromfloat2fEx(p1,pptf1,0,Plane_XZ);
					point3f* p2 = new point3f;
					float3fFromfloat2fEx(p2,pptf1,-fFlankHeight,Plane_XZ);
					point3f* p3 = new point3f;
					float3fFromfloat2fEx(p3,pptf2,-fFlankHeight,Plane_XZ);
					if(bAntiClockwise)
					{
						pptrTri->Add(p1);
						pptrTri->Add(p2);
						pptrTri->Add(p3);
					}
					else
					{
						pptrTri->Add(p1);
						pptrTri->Add(p3);
						pptrTri->Add(p2);
					}

					//pptrTri->Add(lpPtf3DNew);

					// 第二个面片
					p1 = new point3f;
					float3fFromfloat2fEx(p1,pptf1,0,Plane_XZ);
					p2= new point3f;
					float3fFromfloat2fEx(p2,pptf2,-fFlankHeight,Plane_XZ);
					p3 = new point3f;
					float3fFromfloat2fEx(p3,pptf2,0,Plane_XZ);
					if(bAntiClockwise)
					{
						pptrTri->Add(p1);
						pptrTri->Add(p2);
						pptrTri->Add(p3);
					}
					else
					{
						pptrTri->Add(p1);
						pptrTri->Add(p3);
						pptrTri->Add(p2);
					}

					// 法线
					pptf1 = (pointf * )SYAR_GetAt(&ptrPolyNorm,2*nSize - 2);
					pptf2 = (pointf * )SYAR_GetAt(&ptrPolyNorm,2*nSize - 1);
					// 第一个面片
					p1 = new point3f;
					float3fFromfloat2fEx(p1,pptf1,0,Plane_XZ);
					float3Normalize( p1, p1 );

					p2 = new point3f;
					float3fFromfloat2fEx(p2,pptf1,0,Plane_XZ);
					float3Normalize( p2, p2 );

					p3 = new point3f;
					float3fFromfloat2fEx(p3,pptf2,0,Plane_XZ);
					float3Normalize( p3, p3 );

					if(bAntiClockwise)
					{
						pptrTriNorm->Add(p1);
						pptrTriNorm->Add(p2);
						pptrTriNorm->Add(p3);
					}
					else
					{
						*p1 = -(*p1);
						*p2 = -(*p2);
						*p3 = -(*p3);
						pptrTriNorm->Add(p1);
						pptrTriNorm->Add(p3);
						pptrTriNorm->Add(p2);
					}

					// 第二个面片
					p1 = new point3f;
					float3fFromfloat2fEx(p1,pptf1,0,Plane_XZ);
					float3Normalize( (vector3f*)p1, (vector3f*)p1);

					p2 = new point3f;
					float3fFromfloat2fEx(p2,pptf2,0,Plane_XZ);
					float3Normalize( (vector3f*)p2, (vector3f*)p2 );

					p3 = new point3f;
					float3fFromfloat2fEx(p3,pptf2,0,Plane_XZ);
					float3Normalize( (vector3f*)p3, (vector3f*)p3 );
					if(bAntiClockwise)
					{
						pptrTriNorm->Add(p1);
						pptrTriNorm->Add(p2);
						pptrTriNorm->Add(p3);
					}
					else
					{
						*p1 = -(*p1);
						*p2 = -(*p2);
						*p3 = -(*p3);
						pptrTriNorm->Add(p1);
						pptrTriNorm->Add(p3);
						pptrTriNorm->Add(p2);
					}
				}
				// 释放
				nSize = (int)SYAR_GetSize(&ptrPoly);
				for (j=0; j<nSize; j++)
				{
					pointf * pptf = (pointf * )SYAR_GetAt(&ptrPoly,j);
					delete pptf;
					pptf = NULL;
				}
				SYAR_RemoveAll(&ptrPoly);

				nSize = (int)SYAR_GetSize(&ptrPolyNorm);
				for (j=0; j<nSize; j++)
				{
					pointf * pptf = (pointf * )SYAR_GetAt(&ptrPolyNorm,j);
					delete pptf;
					pptf = NULL;
				}
				SYAR_RemoveAll(&ptrPolyNorm);

				cell_release(&cellline[k]);
				memset(&cellline[k],0,sizeof(vectorcell));
				pCurveCell ++;
			}
		}

	} // end of else
} // END OF CreateF
void CreateNewBevel(IN OUT CSYPoint3fPtrArray* pptrTri, IN OUT CSYPoint3fPtrArray* pptrTriNorm, IN int nBevelTriSize, IN int nTriPtSize, IN float fBevelHeight, IN const int nBevelType )
{
	if (nBevelType == 3) 
	{
		float fTemp = 1.f / fBevelHeight;
		for (int i=0; i<nBevelTriSize; i++) 
		{
			point3f* lpPtf3D1 = (point3f*)pptrTri->GetAt(i);
			point3f* lpPtf3D2 = (point3f*)pptrTriNorm->GetAt(i);

			lpPtf3D2->y = 1.f - abs(lpPtf3D1->y) * fTemp;

			if (lpPtf3D2->y > 1.f) 
			{
				lpPtf3D2->y = 1.f;
			}
			if (lpPtf3D2->y < 0.f) 
			{
				lpPtf3D2->y = 0.f;
			}

			float j = lpPtf3D2->x;
			float k = lpPtf3D2->z;
			float temp = sqrt( (1 - lpPtf3D2->y * lpPtf3D2->y ) / (j*j + k*k) );
			lpPtf3D2->x *= temp;
			lpPtf3D2->z *= temp;

			if (i%3 == 0) 
			{
				lpPtf3D1 = (point3f*)pptrTriNorm->GetAt(i + nTriPtSize);
			}
			else if (i%3 == 1) 
			{
				lpPtf3D1 = (point3f*)pptrTriNorm->GetAt(i + 1 + nTriPtSize);
			}
			else if (i%3 == 2) 
			{
				lpPtf3D1 = (point3f*)pptrTriNorm->GetAt(i - 1 + nTriPtSize);
			}

			lpPtf3D1->x = lpPtf3D2->x;
			lpPtf3D1->y = -lpPtf3D2->y;
			lpPtf3D1->z = lpPtf3D2->z;

		} // end of for
	}
	else if (nBevelType == 4)
	{
		float fTemp = 1.f / fBevelHeight;
		for (int i=0; i<nBevelTriSize; i++) 
		{
			point3f* lpPtf3D1 = (point3f*)pptrTri->GetAt(i);
			point3f* lpPtf3D2 = (point3f*)pptrTriNorm->GetAt(i);

			lpPtf3D2->y = abs(lpPtf3D1->y) * fTemp;

			if (lpPtf3D2->y > 1.f) 
			{
				lpPtf3D2->y = 1.f;
			}
			if (lpPtf3D2->y < 0.f) 
			{
				lpPtf3D2->y = 0.f;
			}

			float j = lpPtf3D2->x;
			float k = lpPtf3D2->z;
			float temp = sqrt( (1 - lpPtf3D2->y * lpPtf3D2->y ) / (j*j + k*k) );
			lpPtf3D2->x *= temp;
			lpPtf3D2->z *= temp;

			if (i%3 == 0) 
			{
				lpPtf3D1 = (point3f*)pptrTriNorm->GetAt(i + nTriPtSize);
			}
			else if (i%3 == 1) 
			{
				lpPtf3D1 = (point3f*)pptrTriNorm->GetAt(i + 1 + nTriPtSize);
			}
			else if (i%3 == 2) 
			{
				lpPtf3D1 = (point3f*)pptrTriNorm->GetAt(i - 1 + nTriPtSize);
			}

			lpPtf3D1->x = lpPtf3D2->x;
			lpPtf3D1->y = -lpPtf3D2->y;
			lpPtf3D1->z = lpPtf3D2->z;
		} // end of for
	}
} // END OF CreateNewBevel
void AddSym( IN OUT CSYPoint3fPtrArray* pptrTri, IN OUT CSYPoint3fPtrArray* pptrTriNorm, CSYPoint3fPtrArray* pptrUV,IN float fBevelHeight, IN float fFlankHeight, IN BOOL bBevel )
{
	float fTotalHeight = 2 * fBevelHeight + fFlankHeight;
	if (!bBevel)
	{
		fTotalHeight = fFlankHeight;
	}
	int nTriPtSize = (int)pptrTri->GetSize();

	for( int i = 0; i < nTriPtSize; i += 3 )
	{
		point3f* lpPtf3D = (point3f*)pptrTri->GetAt(i);
		point3f* lpPtf3DNew = new point3f;
		lpPtf3DNew->x = lpPtf3D->x;
		lpPtf3DNew->y = -( fTotalHeight + lpPtf3D->y );
		lpPtf3DNew->z = lpPtf3D->z;
		pptrTri->Add(lpPtf3DNew);

		lpPtf3D = (point3f*)pptrTri->GetAt(i+2);
		lpPtf3DNew = new point3f;
		lpPtf3DNew->x = lpPtf3D->x;
		lpPtf3DNew->y = -( fTotalHeight + lpPtf3D->y );
		lpPtf3DNew->z = lpPtf3D->z;
		pptrTri->Add(lpPtf3DNew);

		lpPtf3D = (point3f*)pptrTri->GetAt(i+1);
		lpPtf3DNew = new point3f;
		lpPtf3DNew->x = lpPtf3D->x;
		lpPtf3DNew->y = -( fTotalHeight + lpPtf3D->y );
		lpPtf3DNew->z = lpPtf3D->z;
		pptrTri->Add(lpPtf3DNew);

		//uv
		if(pptrUV && pptrUV->GetSize() > i)
		{
			lpPtf3D = (point3f*)pptrUV->GetAt(i);
			lpPtf3DNew = new point3f;
			*lpPtf3DNew = *lpPtf3D;
			pptrUV->Add(lpPtf3DNew);

			lpPtf3D = (point3f*)pptrUV->GetAt(i + 2);
			lpPtf3DNew = new point3f;
			*lpPtf3DNew = *lpPtf3D;
			pptrUV->Add(lpPtf3DNew);

			lpPtf3D = (point3f*)pptrUV->GetAt(i + 1);
			lpPtf3DNew = new point3f;
			*lpPtf3DNew = *lpPtf3D;
			pptrUV->Add(lpPtf3DNew);
		}

		//// 法线
		lpPtf3D = (point3f*)pptrTriNorm->GetAt(i);
		lpPtf3DNew = new point3f;
		lpPtf3DNew->x = lpPtf3D->x;
		lpPtf3DNew->y = -lpPtf3D->y;
		lpPtf3DNew->z = lpPtf3D->z;
		pptrTriNorm->Add(lpPtf3DNew);

		lpPtf3D = (point3f*)pptrTriNorm->GetAt(i+2);
		lpPtf3DNew = new point3f;
		lpPtf3DNew->x = lpPtf3D->x;
		lpPtf3DNew->y = -lpPtf3D->y;
		lpPtf3DNew->z = lpPtf3D->z;
		pptrTriNorm->Add(lpPtf3DNew);

		lpPtf3D = (point3f*)pptrTriNorm->GetAt(i+1);
		lpPtf3DNew = new point3f;
		lpPtf3DNew->x = lpPtf3D->x;
		lpPtf3DNew->y = -lpPtf3D->y;
		lpPtf3DNew->z = lpPtf3D->z;
		pptrTriNorm->Add(lpPtf3DNew);
	}
}

//-----------------------------------------------------------------------------
// 新的算法: 
// 1. 首先生成偏移后的边, 并且这条边上的每一个点, 记录对应原始边上的点序号
// 2. 得到这样偏移后的边之后, 再根据这样的两个边生成三角形面片.
//    2. 1 倒角部分, 通过顶点对应关系, 很容易生成. 数目是固定的(需要计算一次. 干脆就加一个成员?).
//    2. 2 侧面, 通过偏移后的边生成. 数目也是固定的
//-----------------------------------------------------------------------------
void GenerateBevel( IN vectorgraph * pFigure, 
				   IN float fBevelWidth, 
				   IN float fBevelHeight, 
				   IN float fFlankHeight, 
				   IN const int nBevelType, 
				   OUT CSYPoint3fPtrArray *pptrTri, 
				   OUT CSYPoint3fPtrArray *pptrTriNorm,
				   OUT CSYPoint3fPtrArray* pptrUV,
				   OUT int* uface,
				   OUT int* dface,
				   OUT int* side)
{
	if(*uface)
		*uface = 0;
	int i, j;
	int nTriPtSize = 0;
	vectorcell* pCurveCell = NULL;
	point3f* lpPtf3D = NULL, *lpPtf3DNew = NULL, *ptf3DRet = NULL, *pTriPoint0 = NULL, *pTriPoint1 = NULL, 
		*pTriPoint2 = NULL;
	SYBevelPoly tpBevelPoly;

	float fMaxCellWidth = 0.0f;
	float fCurCellWidth = 0.0f;
	int nOutCell = 0;
	for( i = 0; i < pFigure->ncell; i++ )
	{
		pCurveCell = &pFigure->cell[i];
		fCurCellWidth = pCurveCell->bound.right - pCurveCell->bound.left;
		if (fMaxCellWidth < fCurCellWidth ) 
		{
			fMaxCellWidth = fCurCellWidth;
			nOutCell = i;
		}
	}
	BOOL bAntiClockwise = 1 - CalOuterClockwise(pFigure); //IsAntiClockwise(&pFigure->cell[nOutCell]);

	int ndirect = bAntiClockwise ? 1 : -1;
	BOOL bBevel2 = (nBevelType == 2);

	int calccount = 1;
	vectorcell cellline[2];
	memset(&cellline,0,sizeof(vectorcell) * 2);
	vector2f vOffset = {0,0};
	if (nBevelType > 0)
	{
		for( i = 0; i < pFigure->ncell; i++ )
		{
			calccount = 1;
			pCurveCell = &pFigure->cell[i];
			if(pCurveCell->type == close_line)
			{
				calccount = 2;
				cell_line_extend_close(
					&cellline[0],
					&cellline[1],
					&pFigure->cell[i],
					&vOffset,
					ndirect,
					pFigure->cell[i].width);
				pCurveCell = &cellline[0];
			}
			else if(pCurveCell->type == uncloss_line)
			{
				calccount = 1;
				if(pCurveCell->width <= 0)
				{
					continue;
				}

				vector2f vOffset = {0,0};
				cell_extend_close(&cellline[0],
					&pFigure->cell[i],
					&vOffset,
					ndirect,
					0);
				pCurveCell = &cellline[0];
			}
			for(int k = 0;k < calccount;k++)
			{
				tpBevelPoly.BevelPolyAPI( pCurveCell, fBevelWidth, fBevelHeight, ptf3DRet, 
					nTriPtSize, bAntiClockwise );
				for ( j = 0; j < nTriPtSize; j+=6 )
				{
					lpPtf3D = new point3f;
					//*lpPtf3D = ptf3DRet[j];
					lpPtf3D->x = ptf3DRet[j].x;
					if (bBevel2) 
					{
						lpPtf3D->y = -ptf3DRet[j].y;
					}
					else
					{
						lpPtf3D->y = ptf3DRet[j].y;
					}
					lpPtf3D->z = ptf3DRet[j].z;
					pptrTri->Add(lpPtf3D);

					lpPtf3D = new point3f;
					//*lpPtf3D = ptf3DRet[j+1];
					lpPtf3D->x = ptf3DRet[j+1].x;
					if (bBevel2) 
					{
						lpPtf3D->y = -ptf3DRet[j+1].y;
					}
					else
					{
						lpPtf3D->y = ptf3DRet[j+1].y;
					}
					lpPtf3D->z = ptf3DRet[j+1].z;
					pptrTri->Add(lpPtf3D);

					lpPtf3D = new point3f;
					//*lpPtf3D = ptf3DRet[j+2];
					lpPtf3D->x = ptf3DRet[j+2].x;
					if (bBevel2) 
					{
						lpPtf3D->y = -ptf3DRet[j+2].y;
					}
					else
					{
						lpPtf3D->y = ptf3DRet[j+2].y;
					}
					lpPtf3D->z = ptf3DRet[j+2].z;
					pptrTri->Add(lpPtf3D);

					// 法线
					lpPtf3D = new point3f;
					if (bBevel2) 
					{
						lpPtf3D->x = -ptf3DRet[j+3].x;
						lpPtf3D->y = ptf3DRet[j+3].y;
						lpPtf3D->z = -ptf3DRet[j+3].z;
					}
					else
					{
						*lpPtf3D = ptf3DRet[j+3];
					}
					*lpPtf3D = -(*lpPtf3D);
					pptrTriNorm->Add(lpPtf3D);

					lpPtf3D = new point3f;
					if (bBevel2) 
					{
						lpPtf3D->x = -ptf3DRet[j+4].x;
						lpPtf3D->y = ptf3DRet[j+4].y;
						lpPtf3D->z = -ptf3DRet[j+4].z;
					}
					else
					{
						*lpPtf3D = ptf3DRet[j+4];
					}

					*lpPtf3D = -(*lpPtf3D);
					pptrTriNorm->Add(lpPtf3D);

					lpPtf3D = new point3f;
					if (bBevel2) 
					{
						lpPtf3D->x = -ptf3DRet[j+5].x;
						lpPtf3D->y = ptf3DRet[j+5].y;
						lpPtf3D->z = -ptf3DRet[j+5].z;
					}
					else
					{
						*lpPtf3D = ptf3DRet[j+5];
					}
					*lpPtf3D = -(*lpPtf3D);
					pptrTriNorm->Add(lpPtf3D);
				}
				delete[] ptf3DRet;
				ptf3DRet = NULL;
				nTriPtSize = 0;
				cell_release(&cellline[k]);
				memset(&cellline[k],0,sizeof(vectorcell));
				pCurveCell ++;
			}
		}

	}
	int nBevelTriSize = (int)pptrTri->GetSize();
	SYTriangulateApp tri;
	tri.SYTriangulateLine( pFigure, pptrTri, pptrUV,bAntiClockwise );
	tri.SYTriangulateGL( pFigure, pptrTri, pptrUV,bAntiClockwise );

	nTriPtSize = (int)pptrTri->GetSize();
	// 法线
	for ( i=nBevelTriSize; i < nTriPtSize ; i++)
	{
		lpPtf3D = new point3f;
		lpPtf3D->x = 0;
		lpPtf3D->y = 1;
		lpPtf3D->z = 0;
		pptrTriNorm->Add(lpPtf3D);
	}

	if(dface)
		*dface = nTriPtSize;
	//添加对称面
	//SYProgress_SectionBegin(0.95f,0.05f);
	AddSym( pptrTri, pptrTriNorm,pptrUV,fBevelHeight, fFlankHeight, nBevelType > 0 );
	//SYProgress_SetValue(0.3f);

	if(side)
		*side = (int)pptrTri->GetSize();
	//创建侧面
	int nPrvCount = pptrTriNorm->GetSize();
	CreateF(pFigure, pptrTri, pptrTriNorm, nBevelTriSize, fBevelHeight,fFlankHeight, nBevelType, bAntiClockwise );
	int nCurCount = pptrTriNorm->GetSize();
#ifdef _DEBUG
	point3f* pprv = NULL;
	for(int i = nPrvCount;i < nCurCount;i += 6)
	{
		if(pprv)
		{
			point3f* pf = pptrTriNorm->GetAt(i + 2);
			if(abs(pf->x - pprv->x) > ABS_FLOAT_0 
				|| abs(pf->y - pprv->y) > ABS_FLOAT_0
				|| abs(pf->z - pprv->z) > ABS_FLOAT_0)
			{
				//int a = 0;
				ASSERT(0);
			}
			pf = pptrTriNorm->GetAt(i + 5);
			if(abs(pf->x - pprv->x) > ABS_FLOAT_0 
				|| abs(pf->y - pprv->y) > ABS_FLOAT_0
				|| abs(pf->z - pprv->z) > ABS_FLOAT_0)
			{
				ASSERT(0);
				//int a = 0;
			}
		}
		
	}
#endif // _DEBUG
	CreateNewBevel(pptrTri, pptrTriNorm, nBevelTriSize, nTriPtSize, fBevelHeight, nBevelType );
} // END OF GenerateBevel
void GetBoundRt( CSYPoint3fPtrArray *pptrTri, rectf &rt )
{
	int nSize = (int)pptrTri->GetSize();
	rt.left   = 99999.f;
	rt.top    = 99999.f;
	rt.right  = -99999.f;
	rt.bottom = -99999.f;

	for ( int i = 0; i < nSize; i++ )
	{
		point3f* lpPtf3D = (point3f*)pptrTri->GetAt(i);
		if (lpPtf3D->x < rt.left ) 
		{
			rt.left = lpPtf3D->x;
		}
		if (lpPtf3D->x > rt.right ) 
		{
			rt.right = lpPtf3D->x;
		}
		if (lpPtf3D->z < rt.top ) 
		{
			rt.top = lpPtf3D->z;
		}
		if (lpPtf3D->z > rt.bottom ) 
		{
			rt.bottom = lpPtf3D->z;
		}
	}
}
BOOL SYCreateBevelGraph( IN OUT SYModel* pMod, IN vectorgraph* p2DGraph, IN float fHeight, IN float fBevelWidth, IN float fBevelHeight, IN int nBevelType)
{
	if(fBevelWidth == 0 || fBevelHeight == 0)
		nBevelType = SY_BEVEL_NONE;
	float fTotalHeight = (nBevelType == SY_BEVEL_NONE) ? fHeight : fHeight + fBevelHeight * 2;
	if ( fTotalHeight < 0.001 )
		return FALSE;

	vector3f SrcPoint = { 0, 0, 0 }, DesVec = { 0, 0, 0 };
	float3 vOffset,v3DPoint;
	vOffset.x = 0;//1920 / 2.f;
	vOffset.y = 0;//-1080 / 2.f;
	vOffset.z = 0;
	for( int k = 0; k < p2DGraph->ncell; k++ )
	{
		vectorcell* pCurveCell = &p2DGraph->cell[k];
		for ( int j = 0; j < pCurveCell->npoint; j++ )
		{
			SY2DCoordTo3D(&pCurveCell->point[j],&v3DPoint,&vOffset);
			pCurveCell->point[j].x = v3DPoint.x;
			pCurveCell->point[j].y = v3DPoint.y;

		}
		// 变换bound
		pointf vLT,vRB;
		vLT.x = pCurveCell->bound.left;
		vLT.y = pCurveCell->bound.top;
		SY2DCoordTo3D(&vLT,&v3DPoint,&vOffset);

		pCurveCell->bound.left = v3DPoint.x;
		pCurveCell->bound.top = v3DPoint.y;

		vRB.x = pCurveCell->bound.right;
		vRB.y = pCurveCell->bound.bottom;
		SY2DCoordTo3D(&vRB,&v3DPoint,&vOffset);

		pCurveCell->bound.right = v3DPoint.x;
		pCurveCell->bound.bottom = v3DPoint.y;
	}

	if (p2DGraph->ncell > 0)
	{
		// 变换bound
		pointf vLT,vRB;
		vLT.x = p2DGraph->bound.left;
		vLT.y = p2DGraph->bound.top;
		SY2DCoordTo3D(&vLT,&v3DPoint,&vOffset);
		p2DGraph->bound.left = v3DPoint.x;
		p2DGraph->bound.top = v3DPoint.y;

		vRB.x = p2DGraph->bound.right;
		vRB.y = p2DGraph->bound.bottom;
		SY2DCoordTo3D(&vRB,&v3DPoint,&vOffset);
		p2DGraph->bound.right = v3DPoint.x;
		p2DGraph->bound.bottom = v3DPoint.y;
	}
	CSYPoint3fPtrArray ptrTri, ptrTriNorm,ptrTriUV;
	int nStartF,nStartB,nSide;

	GenerateBevel( p2DGraph, fBevelWidth, fBevelHeight, fHeight, nBevelType, &ptrTri, &ptrTriNorm ,&ptrTriUV,&nStartF,&nStartB,&nSide);

	int nTriPtSize = (int)ptrTri.GetSize();
	pMod->vcount   = nTriPtSize;
	SAFE_DELETE(pMod->v)
		pMod->v        = new SYVertex3D[nTriPtSize];
	memset( pMod->v, 0, nTriPtSize * sizeof(SYVertex3D) );

	rectf rt;
	GetBoundRt( &ptrTri, rt );
	float fBoundWidth  = rt.right - rt.left;
	float fBoundHeight = rt.bottom - rt.top;
	int nUVSize = ptrTriUV.GetSize();


	vector3f vVertexInViewSpace = { 0.f, 0.f, 0.f };
	for ( int k = 0; k < nTriPtSize; k += 3 )
	{
		int pIndex[3] = {0,2,1};
		for(int i = 0; i < 3; i++)
		{
			int nSrcIndex = k + pIndex[i];
			int nDstIndex = k + i;
			point3f* pTriPoint     = (point3f*)ptrTri.GetAt(nSrcIndex);
			point3f* pTriPointNorm = (point3f*)ptrTriNorm.GetAt(nSrcIndex);

			if(nSrcIndex >= nUVSize)
			{
				(pMod->v)[nDstIndex].tu        = (pTriPoint->x - rt.left) / fBoundWidth;
				(pMod->v)[nDstIndex].tv        = 1 - (pTriPoint->z - rt.top) / fBoundHeight;
			}
			else
			{
				point3f* pTriPoint     = (point3f*)ptrTriUV.GetAt(nSrcIndex);
				(pMod->v)[nDstIndex].tu        = pTriPoint->x;
				(pMod->v)[nDstIndex].tv        = 1 - pTriPoint->y;
			}

			vVertexInViewSpace.x       = pTriPoint->x;//view unit space
			vVertexInViewSpace.y       = pTriPoint->z;
			vVertexInViewSpace.z       = pTriPoint->y ;

			(pMod->v)[nDstIndex].vDyn  = vVertexInViewSpace;

			(pMod->v)[nDstIndex].nx  = pTriPointNorm->x;//view unit space
			(pMod->v)[nDstIndex].ny  = pTriPointNorm->z;
			(pMod->v)[nDstIndex].nz  = pTriPointNorm->y;


		}
	}

	pMod->icount = nTriPtSize;
	SAFE_DELETE(pMod->indecs)
		pMod->indecs = new DWORD[nTriPtSize];
	memset( pMod->indecs, 0, nTriPtSize*sizeof(DWORD) );
	for ( int k = 0; k < nTriPtSize; k++ )
		pMod->indecs[k] = k;

	if(pMod->viewsub)
		delete[] pMod->viewsub;
	pMod->nviewsub = 3;
	pMod->viewsub = new SYSubModel[3];
	memset(pMod->viewsub,0,sizeof(SYSubModel) * 3);
	pMod->viewsub[0].vstart = nStartF;
	pMod->viewsub[0].vcount = nStartB - nStartF;
	pMod->viewsub[0].istart = nStartF;
	pMod->viewsub[0].icount    = pMod->viewsub[0].vcount;
	pMod->viewsub[0].nTri_Num  = pMod->viewsub[0].vcount / 3;
	pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST;


	pMod->viewsub[1].vstart = nStartB;
	pMod->viewsub[1].istart = nStartB;
	pMod->viewsub[1].vcount = nSide - nStartB;
	pMod->viewsub[1].icount    = pMod->viewsub[1].vcount;
	pMod->viewsub[1].nTri_Num  = pMod->viewsub[1].vcount / 3;
	pMod->viewsub[1].nTri_Type = SY_TRIANGLE_LIST;

	pMod->viewsub[2].vstart = nSide;
	pMod->viewsub[2].istart = nSide;
	pMod->viewsub[2].vcount = nTriPtSize - nSide;
	pMod->viewsub[2].icount    = pMod->viewsub[2].vcount;
	pMod->viewsub[2].nTri_Num  = pMod->viewsub[2].vcount / 3;
	pMod->viewsub[2].nTri_Type = SY_TRIANGLE_LIST;


	for( int k = 0; k < nTriPtSize; k++ )
	{
		point3f* lpPtf3D = (point3f*)ptrTri.GetAt(k);
		SAFE_DELETE(lpPtf3D);
	}
	ptrTri.RemoveAll();

	for( int k = 0; k < nTriPtSize; k++ )
	{
		point3f* lpPtf3D = (point3f*)ptrTriNorm.GetAt(k);
		SAFE_DELETE(lpPtf3D);
	}
	ptrTriNorm.RemoveAll();

	for( int k = 0; k < nUVSize; k++ )
	{
		point3f* lpPtf3D = (point3f*)ptrTriUV.GetAt(k);
		SAFE_DELETE(lpPtf3D);
	}
	ptrTriUV.RemoveAll();
	return TRUE;
}

void SYCreateModelTangent(SYModel* pMod)
{
	for(int i = 0;i < pMod->nviewsub;i++)
	{
		DWORD* pIndex = pMod->indecs + pMod->viewsub[i].istart;
		SYVertex3D* pVertex = pMod->v + pMod->viewsub[i].vstart;
		SYCreateTangent(pVertex ,pMod->viewsub[i].vcount,pIndex,pMod->viewsub[i].icount,pMod->viewsub[i].vstart,pMod->viewsub[i].nTri_Type);
	}
}

BOOL SYModel_CreateSegment( IN OUT SYModel* pMod, IN float fLeftWidth, IN float fRightWidth, IN float fTopHeight, IN float fBottomHeight )
{
	if(pMod==NULL)
		return FALSE;
	SYInitialSegment(pMod);
	SYCreateSegmentGraph( pMod, fLeftWidth, fRightWidth, fTopHeight, fBottomHeight );
	SYCreateModelTangent(pMod);
	return TRUE;
}

BOOL SYModel_CreateBevel( IN OUT SYModel* pMod, IN vectorgraph* p2DGraph, IN float fHeight, IN float fBevelWidth, IN float fBevelHeight, IN const int nBevelType) 
{
	SYInitialBevel(pMod);
	SYCreateBevelGraph( pMod, p2DGraph, fHeight, fBevelWidth, fBevelHeight, nBevelType);
	SYCreateModelTangent(pMod);
	return TRUE;
}
void SetSpherePoint( IN OUT SYVertex3D* pSpherePt, IN float fRadius, IN int nSphereCircleNum, IN int nSphere_Per_Circle_VertexNum, IN BOOL bFinished )
{
	float flat_angle(0.f), flong_angle(0.f);
	float fFanNum = (float)( nSphere_Per_Circle_VertexNum - 1 );

	for ( int i = 0; i < nSphereCircleNum; i++ )//latitude
	{
		flat_angle = (float)PAI * ( i / (float)( nSphereCircleNum - 1 ));//[ 0, 180 ]

		for ( int j = 0; j < nSphere_Per_Circle_VertexNum; j++ )//longitude
		{
			flong_angle = PAI + 2 * (float)PAI * ( j / fFanNum );//[ 0, 360 ]

			pSpherePt[ i * nSphere_Per_Circle_VertexNum + j ].x   = fRadius * sinf(flat_angle) * cosf(flong_angle);
			pSpherePt[ i * nSphere_Per_Circle_VertexNum + j ].y   = fRadius * cosf(flat_angle);
			pSpherePt[ i * nSphere_Per_Circle_VertexNum + j ].z   = fRadius * sinf(flat_angle) * sinf(flong_angle);
			//pSpherePt[ i * nSphere_Per_Circle_VertexNum + j ].w   = 1.f;

			pSpherePt[ i * nSphere_Per_Circle_VertexNum + j ].nx  = pSpherePt[ i * nSphere_Per_Circle_VertexNum + j ].x;
			pSpherePt[ i * nSphere_Per_Circle_VertexNum + j ].ny  = pSpherePt[ i * nSphere_Per_Circle_VertexNum + j ].y;
			pSpherePt[ i * nSphere_Per_Circle_VertexNum + j ].nz  = pSpherePt[ i * nSphere_Per_Circle_VertexNum + j ].z;
			//pSpherePt[ i * nSphere_Per_Circle_VertexNum + j ].nw  = 0.f;

			float3Normalize( &pSpherePt[ i * nSphere_Per_Circle_VertexNum + j ].vNorm, &pSpherePt[ i * nSphere_Per_Circle_VertexNum + j ].vNorm );
		}
	}
}
BOOL SYCreateSphereGraph(IN OUT SYModel* pMod, IN float fRad ,BOOL bSphereTex, IN int nLevel /* = 1 */){
	SetSpherePoint( &pMod->v[g_stuSphereSubs[SPHERE_MATERIAL_PARAMCHG].vstart], fRad, g_nSphere_Circle_Num * nLevel, g_nSphere_Per_Circle_VertexNum * nLevel, FALSE );
	if(!bSphereTex)
	{
		for(int i = 0; i < pMod->vcount; i++)
		{
			pMod->v[i].tu = (pMod->v[i].x / fRad + 1) * 0.5;
			pMod->v[i].tv = (pMod->v[i].y / fRad + 1) * 0.5;
		}
	}
	return TRUE;
}
BOOL SYModel_CreateWidthLine(IN OUT vector3f* pFourVertex, const vector3f* pStart,const vector3f* pEnd, float width, const SYSpaceConvertParam* pProj)
{
	vector3f screenStart,screenEnd, viewStart = *pStart, viewEnd = *pEnd;
	vector3f screenPoint[4];
	SYSpaceConvertParam newProj = *pProj;
	SpaceConvert(&screenStart,pStart,&newProj,SY3DVEC_TO_2D);
	SpaceConvert(&screenEnd,pEnd,&newProj,SY3DVEC_TO_2D);
	float deltX = screenEnd.x - screenStart.x;
	float deltY = screenEnd.y - screenStart.y;
	float c = sqrt(deltX * deltX + deltY * deltY);
	float sinValue = deltX / c * width / 2, cosValue = deltY / c * width / 2;
	 
	screenPoint[0].x = screenStart.x - cosValue;
	screenPoint[0].y = screenStart.y + sinValue;
	screenPoint[0].z = 0;

	screenPoint[1].x = screenEnd.x - cosValue;
	screenPoint[1].y = screenEnd.y + sinValue;
	screenPoint[1].z = 0;

	screenPoint[2].x = screenStart.x + cosValue;
	screenPoint[2].y = screenStart.y - sinValue;
	screenPoint[2].z = 0;

	screenPoint[3].x = screenEnd.x + cosValue;
	screenPoint[3].y = screenEnd.y - sinValue;
	screenPoint[3].z = 0;
 
	matrix4f viewInv, world_Inv;
	matrix4f_inverse(&viewInv,newProj.pViewMat);
	matrix4f_inverse(&world_Inv,newProj.pWorldMat);

	viewStart *= *newProj.pWorldMat * *newProj.pViewMat;
	viewEnd *= *newProj.pWorldMat * *newProj.pViewMat;

	SpaceConvert(pFourVertex,screenPoint,&newProj,SY2DVEC_TO_3D_VIEWPLANE,&viewStart.z);
	*(pFourVertex) *= viewInv ;
	*(pFourVertex) *= world_Inv;
	SpaceConvert(pFourVertex + 2, screenPoint + 2, &newProj,SY2DVEC_TO_3D_VIEWPLANE,&viewStart.z);
	*(pFourVertex + 2) *= viewInv * world_Inv;

	SpaceConvert(pFourVertex + 1, screenPoint + 1, &newProj,SY2DVEC_TO_3D_VIEWPLANE,&viewEnd.z );
	*(pFourVertex + 1) *= viewInv * world_Inv;
	SpaceConvert(pFourVertex + 3, screenPoint + 3, &newProj,SY2DVEC_TO_3D_VIEWPLANE,&viewEnd.z );
	*(pFourVertex + 3) *= viewInv * world_Inv;

	return TRUE;
}
BOOL CreatePlaneBevelBox(SYModel* pMod,float fLength,float fHeight,float fThick,int nDivision,int uiType,float fTransitionLen,float fTransitionHeight, BOOL bBevel)
{
	//create box
	UINT uiFVNum = 0;

	if(bBevel)
	{
		if(uiType == 1)
		{
			pMod->vcount = (nDivision+1)*(nDivision+1)*6 + (4+4)*24 + uiFVNum*8;
			//nF = nDivision*nDivision*12 + (2+2)*24 + uiFVNum*8;
			pMod->icount = (nDivision*nDivision*12 + (2+2)*24 + uiFVNum*8) * 3;
		}
		else
		{
			pMod->vcount =(nDivision+1)*(nDivision+1)*6 + 88 + uiFVNum*8;
			//nF = nDivision*nDivision*12 + 40 + uiFVNum*8;
			pMod->icount = (nDivision*nDivision*12 + 40 + uiFVNum*8) * 3; 
		}
	}
	else
	{
		pMod->vcount = (nDivision+1)*(nDivision+1)*6 + uiFVNum*8;
		pMod->icount =  (nDivision*nDivision*12 + uiFVNum*8) * 3;
		//nF = nDivision*nDivision*12 + uiFVNum*8;
	}

	pMod->v = new SYVertex3D[pMod->vcount]; 
	pMod->indecs = new DWORD[pMod->icount];
	//pAttrTable = new D3DX10_ATTRIBUTE_RANGE[6];
	pMod->nviewsub = 6;
	pMod->viewsub = new SYSubModel[6];

	float fTrans[3] = {0.f,0.f,0.f};
	float fTransL,fTransH,fTransW;
	fTransL = fTransitionLen;
	fTransH = fTransitionHeight;
	fTransW = fTransitionLen;
	fTrans[0] = min(fTransL,fLength/2.f);
	fTrans[1] = min(fTransH,fHeight/2.f);
	fTrans[2] = min(fTransW,fThick/2.f);

	float fL = fLength - 2*fTransL;
	float fH = fHeight - 2*fTransH;
	float fW = fThick - 2*fTransW;

	float fInvFLTex = (float)(1.f/(sqrtf(2.f)*fTransL + fL));
	float fInvFHTex = (float)(1.f/(sqrtf(2.f)*fTransH + fH));
	float  fInvFWTex = (float)(1.f/(sqrtf(2.f)*fTransW + fW));
	float fInvFLTexTop = (float)(1.f/(2*sqrtf(2.f)*fTransL + fL));
	float fInvFWTexTop = (float)(1.f/(2*sqrtf(2.f)*fTransW + fW));

	if(uiType == 2)
	{
		fInvFHTex = (float)(1.f/fH);
	}

	UINT uiIBIndex ,uiVBIndex ;
	uiIBIndex = uiVBIndex = 0;
	float3 v[3];
	UINT Dim[3]={0,1,2};
	//0 front
	UINT uiFaceID = 0;
	float3Set(v, fL/2.f,fH/2.f,-fThick/2.f);
	float3Set(v + 1,fL/2.f,fHeight/2.f,-fW/2.f);
	float3Set(v + 2,fLength/2.f,fH/2.f,-fW/2.f);
	
	if(uiType == 1)
		CreatePlaneBevelFace(v,fTrans,Dim,pMod->v, uiVBIndex,pMod->indecs,uiIBIndex, fLength, fHeight, fThick, nDivision);
	else
		CreatePlaneBevelFace2(v,fTrans,Dim,bBevel, pMod->v, uiVBIndex,pMod->indecs,uiIBIndex, fLength, fHeight, fThick, nDivision);

	UINT uiFFaceCount = (uiType == 1) ? (nDivision*nDivision*2+16) : (nDivision*nDivision*2+4);
	UINT uiFFVCnt = (uiType == 1) ? ((nDivision+1)*(nDivision+1) + 32) : ((nDivision+1)*(nDivision+1) + 8);
	if(!bBevel)
	{
		uiFFaceCount = nDivision*nDivision*2;
		uiFFVCnt = (nDivision+1)*(nDivision+1);
	}

// 	for(int iii = 0; iii < uiFVNum; iii++)
// 	{
// 		CreateNormalPrimitive(&pMod->v[iii],&pMod->v,&pMod->indecs,uiVBIndex,uiIBIndex);
// 	}


	UINT uiFaceStart = 0;
// 	pAttrTable[0].AttribId = 0;
// 	pAttrTable[0].FaceStart = uiFaceStart;
// 	pAttrTable[0].FaceCount = uiFFaceCount;
// 	pAttrTable[0].VertexStart = 0;
// 	pAttrTable[0].VertexCount = pMod->vcount;
// 	uiFaceStart += pAttrTable[0].FaceCount;

	pMod->viewsub[0].vstart= 0;
	pMod->viewsub[0].vcount =  pMod->vcount;
	pMod->viewsub[0].istart = uiFaceStart*3;
	pMod->viewsub[0].icount = uiFFaceCount*3;
	pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[0].nTri_Num = uiFFaceCount;
	pMod->viewsub[0].pMaterial = NULL;
	uiFaceStart += uiFFaceCount;


	//1 back
	uiFaceID = 1;
	DWORD *pIB = pMod->indecs;
	UINT uiFi;
	for( uiFi = 0; uiFi < uiFFaceCount; uiFi++)
	{
		pIB[uiIBIndex]=pIB[uiFi*3] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiFi*3 + 2] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiFi*3 + 1] + uiVBIndex;
		uiIBIndex++;
	}
	SYVertex3D* pVB = pMod->v;

	for(uiFi = 0; uiFi < uiFFVCnt; uiFi++)
	{
		float3Set(&pVB[uiVBIndex].vDyn, pVB[uiFi].x, pVB[uiFi].y, -pVB[uiFi].z);	
		float3Set(&pVB[uiVBIndex].vNorm, pVB[uiFi].nx, pVB[uiFi].ny, -pVB[uiFi].nz);
	
		pVB[uiVBIndex].tu = 0.5f - pVB[uiVBIndex].vDyn.x * fInvFLTex;
		pVB[uiVBIndex].tv = 0.5f - pVB[uiVBIndex].vDyn.y * fInvFHTex;
		//pVB[uiVBIndex].FaceID = uiFaceID;

		uiVBIndex ++;
	}

	pMod->viewsub[1].vstart= 0;
	pMod->viewsub[1].vcount =  pMod->vcount;
	pMod->viewsub[1].istart = uiFaceStart*3;
	pMod->viewsub[1].icount = uiFFaceCount*3;
	pMod->viewsub[1].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[1].nTri_Num = uiFFaceCount;
	pMod->viewsub[1].pMaterial = NULL;
	uiFaceStart += uiFFaceCount; //pAttrTable[1].FaceCount;

	//2 bottom
	uiFaceID = 2;
	UINT uiBeginBI = uiIBIndex;
	UINT uiBeginBVI = uiVBIndex;

	float3Set(v, -fL/2.f,-fHeight/2.f,fW/2.f);
	float3Set(v + 1, -fL/2.f,-fH/2.f,fThick/2.f);
	float3Set(v + 2, -fLength/2.f,-fH/2.f,fW/2.f);
	Dim[0]=0;Dim[1]=2;Dim[2]=1;
	if(uiType == 1)
		CreatePlaneBevelFace(v,fTrans,Dim,pMod->v, uiVBIndex,pMod->indecs,uiIBIndex, fLength, fHeight, fThick, nDivision);
	else
		CreatePlaneBevelFace2(v,fTrans,Dim,bBevel, pMod->v,uiVBIndex,pMod->indecs,uiIBIndex, fLength, fHeight, fThick, nDivision);

	UINT uiEndI = uiIBIndex;
	UINT uiEndVI = uiVBIndex;
	UINT uiBFaceCount = (uiType == 1) ? uiFFaceCount : (nDivision*nDivision*2+12);
	UINT uiBFVCnt = (uiType == 1) ? uiFFVCnt : ((nDivision+1)*(nDivision+1) + 28);
	if(!bBevel)
	{
		uiBFaceCount = uiFFaceCount;
		uiBFVCnt = uiFFVCnt;
	}

	for(int iii = (int)(uiBeginBVI+14); iii < (int)(uiBeginBVI+14+uiFVNum); iii++)
	{
		CreateNormalPrimitive(&pMod->v[iii],&pMod->v,&pMod->indecs,uiVBIndex,uiIBIndex);
	}


	pMod->viewsub[2].vstart= 0;
	pMod->viewsub[2].vcount =  pMod->vcount;
	pMod->viewsub[2].istart = uiFaceStart*3;
	pMod->viewsub[2].icount = (uiBFaceCount + uiFVNum*8)*3;
	pMod->viewsub[2].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[2].nTri_Num = uiBFaceCount + uiFVNum*8;
	pMod->viewsub[2].pMaterial = NULL;
	uiFaceStart += uiBFaceCount + uiFVNum*8 ;

	//3 right
	uiFaceID = 3;
	UINT uiBeginRI = uiIBIndex;
	UINT uiBeginRVI = uiVBIndex;
	 float3Set(v, fLength/2.f,fH/2.f,fW/2.f);
	float3Set(v + 1,fL/2.f,fHeight/2.f,fW/2.f);
	 float3Set(v + 2,fL/2.f,fH/2.f,fThick/2.f);
	Dim[0]=2;Dim[1]=1;Dim[2]=0;
	if(uiType == 1)
		CreatePlaneBevelFace(v,fTrans,Dim,pMod->v, uiVBIndex,pMod->indecs,uiIBIndex, fLength, fHeight, fThick, nDivision);
	else
		CreatePlaneBevelFace2(v,fTrans,Dim,bBevel, pMod->v, uiVBIndex,pMod->indecs,uiIBIndex, fLength, fHeight, fThick, nDivision);

	UINT uiEndRI = uiIBIndex;
	UINT uiEndRVI = uiVBIndex;

	pMod->viewsub[3].vstart= 0;
	pMod->viewsub[3].vcount =  pMod->vcount;
	pMod->viewsub[3].istart = uiFaceStart*3;
	pMod->viewsub[3].icount = uiFFaceCount*3;
	pMod->viewsub[3].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[3].nTri_Num = uiFFaceCount;
	pMod->viewsub[3].pMaterial = NULL;
	uiFaceStart += uiFFaceCount;

	//4 top
	uiFaceID = 4;
// 	pIB = pMod->indecs;
// 	DWORD * pIB = pIB;
	for(uiFi = uiBeginBI/3; uiFi < uiEndI/3; uiFi++)
	{
		pIB[uiIBIndex]=pIB[uiFi*3] + uiVBIndex - uiBeginBVI;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiFi*3+2] + uiVBIndex - uiBeginBVI;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiFi*3+1] + uiVBIndex - uiBeginBVI;
		uiIBIndex++;
	}

	for(uiFi = uiBeginBVI; uiFi < uiEndVI; uiFi++)
	{
		//need to flip bottom's tv
		pVB[uiFi].tv = 1.0f - pVB[uiFi].tv;

		float3Set(&pVB[uiVBIndex].vDyn, pVB[uiFi].x, -pVB[uiFi].y, pVB[uiFi].z);	
		float3Set(&pVB[uiVBIndex].vNorm, pVB[uiFi].nx, -pVB[uiFi].ny, pVB[uiFi].nz);


		if(uiType == 1)
		{
			pVB[uiVBIndex].tu = pVB[uiVBIndex].x * fInvFLTex + 0.5f;
			pVB[uiVBIndex].tv = 0.5f - pVB[uiVBIndex].z * fInvFWTex;
		}
		else
		{
			pVB[uiVBIndex].tu = pVB[uiVBIndex].x * fInvFLTexTop + 0.5f;
			pVB[uiVBIndex].tv = 0.5f - pVB[uiVBIndex].z * fInvFWTexTop;
		}
		//pVB[uiVBIndex].FaceID = uiFaceID;
		uiVBIndex ++;
	}
// 	pAttrTable[4].AttribId = 4;
// 	pAttrTable[4].FaceStart = uiFaceStart;
// 	pAttrTable[4].FaceCount = uiBFaceCount;
// 	pAttrTable[4].VertexStart = 0;
// 	pAttrTable[4].VertexCount = pMod->vcount;
// 	uiFaceStart += pAttrTable[4].FaceCount;

	pMod->viewsub[4].vstart= 0;
	pMod->viewsub[4].vcount =  pMod->vcount;
	pMod->viewsub[4].istart = uiFaceStart*3;
	pMod->viewsub[4].icount = uiBFaceCount*3;
	pMod->viewsub[4].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[4].nTri_Num = uiBFaceCount;
	pMod->viewsub[4].pMaterial = NULL;
	uiFaceStart += uiBFaceCount;
	//5 left
	uiFaceID = 5;
	
	pIB =  pMod->indecs;
	for(uiFi = uiBeginRI/3; uiFi < uiEndRI/3; uiFi++)
	{
		pIB[uiIBIndex]=pIB[uiFi*3] + uiVBIndex - uiBeginRVI;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiFi*3+2] + uiVBIndex- uiBeginRVI;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiFi*3+1] + uiVBIndex- uiBeginRVI;
		uiIBIndex++;
	}
	for(uiFi = uiBeginRVI; uiFi < uiEndRVI; uiFi++)
	{
		float3Set(&pVB[uiVBIndex].vDyn, -pVB[uiFi].x, pVB[uiFi].y, pVB[uiFi].z);	
		float3Set(&pVB[uiVBIndex].vNorm, -pVB[uiFi].nx, pVB[uiFi].ny, pVB[uiFi].nz);

		
		pVB[uiVBIndex].tu = 0.5f - pVB[uiVBIndex].z * fInvFWTex;
		pVB[uiVBIndex].tv = 0.5f - pVB[uiVBIndex].y * fInvFHTex;
		//pVB[uiVBIndex].FaceID = uiFaceID;
		uiVBIndex ++;
	}
// 	pAttrTable[5].AttribId = 5;
// 	pAttrTable[5].FaceStart = uiFaceStart;
// 	pAttrTable[5].FaceCount = uiFFaceCount;
// 	pAttrTable[5].VertexStart = 0;
// 	pAttrTable[5].VertexCount = pMod->vcount;

	pMod->viewsub[5].vstart= 0;
	pMod->viewsub[5].vcount =  pMod->vcount;
	pMod->viewsub[5].istart = uiFaceStart*3;
	pMod->viewsub[5].icount = uiFFaceCount*3;
	pMod->viewsub[5].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[5].nTri_Num = uiFFaceCount;
	pMod->viewsub[5].pMaterial = NULL;

	return TRUE;
}
void CreateNormalPrimitive(SYVertex3D* pVertex,SYVertex3D** ppVB,LPDWORD* ppIB,UINT& uiVBIndex,UINT& uiIBIndex)
{
	if(!pVertex)
		return;
	float3 Nx = pVertex->vNorm;
	float3 Nz;
	if(Nx.y > ABS_FLOAT_0)
	{
		Nx.y = 0;
		float3Normalize(&Nx,&Nx);
		float3CrossProduct(&Nz,&Nx,&pVertex->vNorm);
		float3Normalize(&Nz,&Nz);
	}
	else
	{
		float3 test = {0,1,0};
		float3CrossProduct(&Nz,&Nx,&test);
		float3Normalize(&Nz,&Nz);
	}
	float3 Ny;
	Nx = pVertex->vNorm;
	float3CrossProduct(&Ny,&Nz,&Nx);
	float3Normalize(&Ny,&Ny);
	matrix4f mRot;
	mRot._11 = Nx.x;mRot._12 = Nx.y;mRot._13 = Nx.z;mRot._14 = 0;
	mRot._21 = Ny.x;mRot._22 = Ny.y;mRot._23 = Ny.z;mRot._24 = 0;
	mRot._31 = Nz.x;mRot._32 = Nz.y;mRot._33 = Nz.z;mRot._34 = 0;
	mRot._41 = 0;mRot._42 = 0;mRot._43 = 0;mRot._44 = 1.f;

	//matrix4fInverse(&mRot,NULL,&mRot);
	mRot._41 = pVertex->vDyn.x;
	mRot._42 = pVertex->vDyn.y;
	mRot._43 = pVertex->vDyn.z;

	matrix4f mRotvNorm = mRot;
	matrix4f_inverse(&mRotvNorm,&mRotvNorm);
	matrix4f_transpose(&mRotvNorm,&mRotvNorm);


	SYVertex3D V4[4]; 
	float3Set(&V4[0].vDyn,0,0,0.002f);
	float3Set(&V4[0].vNorm,0,0,1);

	float3Set(&V4[1].vDyn,0,0.002f,0);
	float3Set(&V4[1].vNorm,0,1,0);

	float3Set(&V4[2].vDyn,0,0,-0.002f);
	float3Set(&V4[2].vNorm,0,0,-1);

	float3Set(&V4[3].vDyn,0,-0.002f,0);
	float3Set(&V4[3].vNorm,0,-1,0);

	int nVIndexBegin = uiVBIndex;

	for(int i = 0; i < 4; i++)
	{
		(*ppVB)[uiVBIndex].vDyn = V4[i].vDyn;
		(*ppVB)[uiVBIndex].vNorm = V4[i].vNorm;
		(*ppVB)[uiVBIndex].vDyn *= mRot;
		float3NormalMulMat(&(*ppVB)[uiVBIndex].vNorm,&(*ppVB)[uiVBIndex].vNorm,&mRotvNorm);
		//D3DXVec3TransformCoord(&(*ppVB)[uiVBIndex].vDyn,&(*ppVB)[uiVBIndex].vDyn,&mRot);
		//D3DXVec3TransformvNorm(&(*ppVB)[uiVBIndex].vNorm,&(*ppVB)[uiVBIndex].vNorm,&mRotvNorm);
		uiVBIndex ++;
		V4[i].vDyn.x += 0.05f; 
		(*ppVB)[uiVBIndex].vDyn = V4[i].vDyn;
		(*ppVB)[uiVBIndex].vNorm = V4[i].vNorm;
		(*ppVB)[uiVBIndex].vDyn *= mRot;
		float3NormalMulMat(&(*ppVB)[uiVBIndex].vNorm,&(*ppVB)[uiVBIndex].vNorm,&mRotvNorm);

		//D3DXVec3TransformCoord(&(*ppVB)[uiVBIndex].vDyn,&(*ppVB)[uiVBIndex].vDyn,&mRot);
		//D3DXVec3TransformvNorm(&(*ppVB)[uiVBIndex].vNorm,&(*ppVB)[uiVBIndex].vNorm,&mRotvNorm);
		uiVBIndex++;
	}

	int a,b,c,d;
	for(int i = 0; i < 4; i++)
	{
		a = 2*i+nVIndexBegin;
		b = a + 1;
		c = a + 2;
		d = a + 3;
		if(c >= (int)uiVBIndex)
		{
			c = c - uiVBIndex + nVIndexBegin;
			d = c + 1;
		}
		(*ppIB)[uiIBIndex++] = a;
		(*ppIB)[uiIBIndex++] = b;
		(*ppIB)[uiIBIndex++] = d;
		(*ppIB)[uiIBIndex++] = a;
		(*ppIB)[uiIBIndex++] = d;
		(*ppIB)[uiIBIndex++] = c;
	}
}

void CreatePlaneBevelFace(float3* pV,float* pF,UINT* pDIM,SYVertex3D* pVB,UINT& uiVBIndex,DWORD* pIB,UINT& uiIBIndex,float fLength,float fHeight,float fThick,int nDivision)
{
	float fL = fLength - 2*pF[0];
	float fH = fHeight - 2*pF[1];
	float fW = fThick - 2*pF[2];

	float fInvTex[3];
	fInvTex[0] = (float)(1.f/(sqrtf(2.f)*pF[0] + fL));
	fInvTex[1]  = (float)(1.f/(sqrtf(2.f)*pF[1] + fH));
	fInvTex[2] = (float)(1.f/(sqrtf(2.f)*pF[2] + fW));

	float3 v[4];float3 vS[4];float3 vP[3];
	float3 vNext,vPre;

	//top Plane

	v[0] =v[1]=v[2]=v[3]= pV[0];	v[1] = (pV[0] + pV[1]) * 0.5f;
	float* pV1 = (float*)(&v[1]);
	float* pV2 = (float*)(&v[2]);
	float* pV0 = (float*)(&v[0]);
	float* pV3 = (float*)(&v[3]);

	pV2[pDIM[0]] = -pV1[pDIM[0]];  
	pV2[pDIM[1]] = pV1[pDIM[1]]; 
	pV2[pDIM[2]] = pV1[pDIM[2]];

	pV3[pDIM[0]] = -pV0[pDIM[0]];  
	pV3[pDIM[1]] = pV0[pDIM[1]]; 
	pV3[pDIM[2]] = pV0[pDIM[2]];


	vS[0] = pV[0]; vS[1] = v[1];vS[2] = (pV[0] + pV[1] + pV[2])/3.f;
	vS[3] = (pV[0] + pV[2]) * 0.5f;

	vP[0] = v[0];vP[1] = v[3];
	
	UINT uiFi;
	for(uiFi = 0; uiFi < 2; uiFi++)
	{
		pIB[uiIBIndex]=uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=uiFi + 2 + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=uiFi + 1 + uiVBIndex;
		uiIBIndex++;
	}
	for(uiFi = 0; uiFi < 4; uiFi++)
	{
		float* pPosition = (float*)(&pVB[uiVBIndex].vDyn);
		
		pVB[uiVBIndex].vDyn= v[uiFi];


		vNext = v[(uiFi+1)%4] - v[uiFi];
		vPre = v[(uiFi-1)&3] - v[uiFi];
		float3CrossProduct(&pVB[uiVBIndex].vNorm,&vPre,&vNext);
		float3Normalize(&pVB[uiVBIndex].vNorm,&pVB[uiVBIndex].vNorm);
		pVB[uiVBIndex].tu = pPosition[pDIM[0]] * fInvTex[pDIM[0]] + 0.5f;
		pVB[uiVBIndex].tv = 0.5f - pPosition[pDIM[1]] * fInvTex[pDIM[1]];
		//pVB[uiVBIndex].FaceID = uiFaceID;
		uiVBIndex ++;
	}
	
	//bottom Plane
	UINT uiBeginI = uiIBIndex - 6;
	UINT uiBeginVI = uiVBIndex - 4;

	float* pPosition ;
	float* pPosition2;
	float* pnormal  ;
	float* pnormal2 ;
	
	for(uiFi = 0; uiFi < 2;uiFi++)
	{
		pIB[uiIBIndex]=pIB[uiBeginI] + uiVBIndex - uiBeginVI;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiBeginI+2] + uiVBIndex- uiBeginVI;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiBeginI+1] + uiVBIndex- uiBeginVI;
		uiIBIndex++;
		uiBeginI+=3;
	}
	for(uiFi = 0; uiFi < 4; uiFi++)
	{
		pPosition = (float*)(&pVB[uiVBIndex].vDyn);
		pPosition2 = (float*)(&pVB[uiBeginVI].vDyn);
		pnormal = (float*)(&pVB[uiVBIndex].vNorm);
		pnormal2 = (float*)(&pVB[uiBeginVI].vNorm);

		pPosition[pDIM[0]] = pPosition2[pDIM[0]];
		pPosition[pDIM[1]] = -pPosition2[pDIM[1]];
		pPosition[pDIM[2]] = pPosition2[pDIM[2]];
		pnormal[pDIM[0]] = pnormal2[pDIM[0]];
		pnormal[pDIM[1]] = -pnormal2[pDIM[1]];
		pnormal[pDIM[2]] = pnormal2[pDIM[2]];
		pVB[uiVBIndex].tu = pPosition[pDIM[0]] * fInvTex[pDIM[0]] + 0.5f;
		pVB[uiVBIndex].tv = 0.5f - pPosition[pDIM[1]] * fInvTex[pDIM[1]];
		//pVB[uiVBIndex].FaceID = uiFaceID;
		uiVBIndex ++;
		uiBeginVI ++;
	}
//return;
	//right Plane
	v[1] = (pV[0] + pV[2]) * 0.5f;

	pV2[pDIM[0]] = pV1[pDIM[0]];  
	pV2[pDIM[1]] = -pV1[pDIM[1]]; 
	pV2[pDIM[2]] = pV1[pDIM[2]];

	pV3[pDIM[0]] = pV0[pDIM[0]];  
	pV3[pDIM[1]] = -pV0[pDIM[1]]; 
	pV3[pDIM[2]] = pV0[pDIM[2]];


	vP[2] = v[3];
	for(uiFi = 0; uiFi < 2;uiFi++)
	{
		pIB[uiIBIndex]=uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=uiVBIndex+1+uiFi;
		uiIBIndex++;
		pIB[uiIBIndex]=uiVBIndex+2+uiFi;
		uiIBIndex++;
	}
	for(uiFi = 0; uiFi < 4; uiFi++)
	{
		pPosition = (float*)(&pVB[uiVBIndex].vDyn);

		pVB[uiVBIndex].vDyn = v[uiFi];
	
		vNext = v[(uiFi+1)%4] - v[uiFi];
		vPre = v[(uiFi-1)&3] - v[uiFi];
		float3CrossProduct(&pVB[uiVBIndex].vNorm,&vNext,&vPre);
		float3Normalize(&pVB[uiVBIndex].vNorm,&pVB[uiVBIndex].vNorm);

		pVB[uiVBIndex].tu = pPosition[pDIM[0]] * fInvTex[pDIM[0]] + 0.5f;
		pVB[uiVBIndex].tv = 0.5f - pPosition[pDIM[1]] * fInvTex[pDIM[1]];
		//pVB[uiVBIndex].FaceID = uiFaceID;
		uiVBIndex++;
	}
	//left Plane
	uiBeginI = uiIBIndex - 6;
	uiBeginVI = uiVBIndex - 4;
	for(uiFi = 0; uiFi < 2;uiFi++)
	{
		pIB[uiIBIndex]=pIB[uiBeginI] + uiVBIndex - uiBeginVI;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiBeginI+2] + uiVBIndex - uiBeginVI;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiBeginI+1] + uiVBIndex - uiBeginVI;
		uiIBIndex++;
		uiBeginI += 3;
	}
	for(uiFi = 0; uiFi < 4; uiFi++)
	{
		pPosition = (float*)(&pVB[uiVBIndex].vDyn);
		pPosition2 = (float*)(&pVB[uiBeginVI].vDyn);
		pnormal = (float*)(&pVB[uiVBIndex].vNorm);
		pnormal2 = (float*)(&pVB[uiBeginVI].vNorm);

		pPosition[pDIM[0]] = -pPosition2[pDIM[0]];
		pPosition[pDIM[1]] = pPosition2[pDIM[1]];
		pPosition[pDIM[2]] = pPosition2[pDIM[2]];

		pnormal[pDIM[0]] = -pnormal2[pDIM[0]];
		pnormal[pDIM[1]] = pnormal2[pDIM[1]];
		pnormal[pDIM[2]] = pnormal2[pDIM[2]];

		pVB[uiVBIndex].tu = pPosition[pDIM[0]] * fInvTex[pDIM[0]] + 0.5f;
		pVB[uiVBIndex].tv = 0.5f - pPosition[pDIM[1]] * fInvTex[pDIM[1]];
		//pVB[uiVBIndex].FaceID = uiFaceID;

		uiVBIndex ++;
		uiBeginVI ++;
	}

	//righttop Plane
	for(uiFi = 0; uiFi < 2;uiFi++)
	{
		pIB[uiIBIndex]=uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=uiVBIndex+uiFi+1;
		uiIBIndex++;
		pIB[uiIBIndex]=uiVBIndex+uiFi+2;
		uiIBIndex++;
	}
	for(uiFi = 0; uiFi < 4; uiFi ++)
	{
		pPosition = (float*)(&pVB[uiVBIndex].vDyn);

		pVB[uiVBIndex].vDyn = vS[uiFi];
	
		vNext = vS[(uiFi+1)%4] - vS[uiFi];
		vPre = vS[(uiFi-1)&3] - vS[uiFi];
		float3CrossProduct(&pVB[uiVBIndex].vNorm,&vNext,&vPre);
		float3Normalize(&pVB[uiVBIndex].vNorm,&pVB[uiVBIndex].vNorm);
		pVB[uiVBIndex].tu = pPosition[pDIM[0]] * fInvTex[pDIM[0]] + 0.5f;
		pVB[uiVBIndex].tv = 0.5f - pPosition[pDIM[1]] * fInvTex[pDIM[1]];
		//pVB[uiVBIndex].FaceID = uiFaceID;
		uiVBIndex++;
	}
	//lefttop Plane
	uiBeginI = uiIBIndex - 6;
	uiBeginVI = uiVBIndex - 4;

	for(uiFi = 0; uiFi < 2;uiFi++)
	{
		pIB[uiIBIndex]=pIB[uiBeginI] + uiVBIndex - uiBeginVI;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiBeginI+2] + uiVBIndex- uiBeginVI;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiBeginI+1] + uiVBIndex- uiBeginVI;
		uiIBIndex++;
		uiBeginI += 3;
	}
	for(uiFi = 0; uiFi < 4; uiFi ++)
	{
		pPosition = (float*)(&pVB[uiVBIndex].vDyn);
		pPosition2 = (float*)(&pVB[uiBeginVI].vDyn);
		pnormal = (float*)(&pVB[uiVBIndex].vNorm);
		pnormal2 = (float*)(&pVB[uiBeginVI].vNorm);

		pPosition[pDIM[0]] = -pPosition2[pDIM[0]];
		pPosition[pDIM[1]] = pPosition2[pDIM[1]];
		pPosition[pDIM[2]] = pPosition2[pDIM[2]];

		pnormal[pDIM[0]] = -pnormal2[pDIM[0]];
		pnormal[pDIM[1]] = pnormal2[pDIM[1]];
		pnormal[pDIM[2]] = pnormal2[pDIM[2]];


		pVB[uiVBIndex].tu = pPosition[pDIM[0]] * fInvTex[pDIM[0]] + 0.5f;
		pVB[uiVBIndex].tv = 0.5f - pPosition[pDIM[1]] * fInvTex[pDIM[1]];

		//pVB[uiVBIndex].FaceID = uiFaceID;

		uiVBIndex ++;
		uiBeginVI ++;
	}

	//rightbottom Plane
	uiBeginI = uiBeginI - 6; 
	uiBeginVI = uiBeginVI - 4;

	for(uiFi = 0; uiFi < 2;uiFi++)
	{
		pIB[uiIBIndex]=pIB[uiBeginI] + uiVBIndex - uiBeginVI;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiBeginI+2] + uiVBIndex- uiBeginVI;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiBeginI+1] + uiVBIndex- uiBeginVI;
		uiIBIndex++;
		uiBeginI += 3;
	}
	for(uiFi = 0; uiFi < 4; uiFi ++)
	{
		pPosition = (float*)(&pVB[uiVBIndex].vDyn);
		pPosition2 = (float*)(&pVB[uiBeginVI].vDyn);
		pnormal = (float*)(&pVB[uiVBIndex].vNorm);
		pnormal2 = (float*)(&pVB[uiBeginVI].vNorm);

		pPosition[pDIM[0]] = pPosition2[pDIM[0]];
		pPosition[pDIM[1]] = -pPosition2[pDIM[1]];
		pPosition[pDIM[2]] = pPosition2[pDIM[2]];

		pnormal[pDIM[0]] = pnormal2[pDIM[0]];
		pnormal[pDIM[1]] = -pnormal2[pDIM[1]];
		pnormal[pDIM[2]] = pnormal2[pDIM[2]];


		pVB[uiVBIndex].tu = pPosition[pDIM[0]] * fInvTex[pDIM[0]] + 0.5f;
		pVB[uiVBIndex].tv = 0.5f - pPosition[pDIM[1]] * fInvTex[pDIM[1]];

		//pVB[uiVBIndex].FaceID = uiFaceID;

		uiVBIndex ++;
		uiBeginVI ++;
	}

	//leftbottom Plane
	uiBeginI = uiBeginI - 6;
	uiBeginVI = uiBeginVI - 4;

	for(uiFi = 0; uiFi < 6;uiFi++)
	{
		pIB[uiIBIndex]=pIB[uiBeginI] + uiVBIndex - uiBeginVI;
		uiIBIndex++;
		uiBeginI ++;
	}

	for(uiFi = 0; uiFi < 4; uiFi ++)
	{
		pPosition = (float*)(&pVB[uiVBIndex].vDyn);
		pPosition2 = (float*)(&pVB[uiBeginVI].vDyn);
		pnormal = (float*)(&pVB[uiVBIndex].vNorm);
		pnormal2 = (float*)(&pVB[uiBeginVI].vNorm);

		pPosition[pDIM[0]] = -pPosition2[pDIM[0]];
		pPosition[pDIM[1]] = -pPosition2[pDIM[1]];
		pPosition[pDIM[2]] = pPosition2[pDIM[2]];

		pnormal[pDIM[0]] = -pnormal2[pDIM[0]];
		pnormal[pDIM[1]] = -pnormal2[pDIM[1]];
		pnormal[pDIM[2]] = pnormal2[pDIM[2]];


		pVB[uiVBIndex].tu = pPosition[pDIM[0]] * fInvTex[pDIM[0]] + 0.5f;
		pVB[uiVBIndex].tv = 0.5f - pPosition[pDIM[1]] * fInvTex[pDIM[1]];

		//pVB[uiVBIndex].FaceID = uiFaceID;

		uiVBIndex ++;
		uiBeginVI ++;
	}

	float3 fVStep = (vP[2]-vP[1]) / (float)(nDivision);
	int nIdx = 0,a,b,c,d;

	float3 vNormal = {0.f,0.f,0.f};

	float* pVP = (float*)(&vP[0]);
	float* pVNormal = (float*)&vNormal;
	pVNormal[pDIM[2]] = pVP[pDIM[2]] < 0 ? -1.f : 1.f; //vP[0][pDIM[2]] < 0 ? -1.f : 1.f; 

	nIdx = 0;
	int nX;
	for (nX=0;nX<nDivision;nX++)
	{
		int nCBase0 = nX * (nDivision+1);
		int nCBase1 = nCBase0 + (nDivision+1);
		for (int nY=0;nY<nDivision;nY++)
		{
			a = nCBase0 + nY + uiVBIndex; b = a + 1;
			c = nCBase1 + nY + uiVBIndex; d = c + 1;
			pIB[uiIBIndex++] = a;
			pIB[uiIBIndex++] = c;
			pIB[uiIBIndex++] = b;
			pIB[uiIBIndex++] = c;
			pIB[uiIBIndex++] = d;
			pIB[uiIBIndex++] = b;
		}
	}

	for (nX=0;nX<=nDivision;nX++)
	{
		float* pVP = (float*)(&vP[1]);
		float* pfVStep = (float *)&fVStep;
		float xx = pVP[pDIM[0]] + pfVStep[pDIM[0]]*nX;
		for (int nY=0;nY<=nDivision;nY++)
		{
			pPosition = (float*)(&pVB[uiVBIndex].vDyn);
			pnormal = (float*)(&pVB[uiVBIndex].vNorm);

			pPosition[pDIM[0]] = xx;
			pPosition[pDIM[1]] = pVP[pDIM[1]] + pfVStep[pDIM[1]]*nY;
			pPosition[pDIM[2]] = pVP[pDIM[2]];
			pVB[uiVBIndex].vNorm = vNormal;
			pVB[uiVBIndex].tu = pPosition[pDIM[0]] * fInvTex[pDIM[0]] + 0.5f;
			pVB[uiVBIndex].tv = 0.5f - pPosition[pDIM[1]] * fInvTex[pDIM[1]];

			//pVB[uiVBIndex].FaceID = uiFaceID;

			uiVBIndex++;
		}
	}
}

void CreatePlaneBevelFace2(float3* pV,float* pF,UINT* pDIM,BOOL bBevel,SYVertex3D* pVB,UINT& uiVBIndex,DWORD* pIB,UINT& uiIBIndex,float fLength,float fHeight,float fThick,int nDivision)
{
	float fL = fLength - 2*pF[0];
	float fH = fHeight - 2*pF[1];
	float fW = fThick - 2*pF[2];

	float fInvTex[3];
	fInvTex[0] = (float)(1.f/(sqrtf(2.f)*pF[0] + fL));
	fInvTex[1]  = (float)(1.f/fH);
	fInvTex[2] = (float)(1.f/(sqrtf(2.f)*pF[2] + fW));

	float3 v[4];float3 vS[4];float3 vP[3];
	float3 vNext,vPre;

	//top Plane
	v[0] = pV[0];	v[1] = pV[1];
	float* pV1 = (float*)(&v[1]);
	float* pV2 = (float*)(&v[2]);
	float* pV0 = (float*)(&v[0]);
	float* pV3 = (float*)(&v[3]);

	pV2[pDIM[0]] = -pV1[pDIM[0]];
	pV2[pDIM[1]] = pV1[pDIM[1]];
	pV2[pDIM[2]] = pV1[pDIM[2]];

	pV3[pDIM[0]] = -pV0[pDIM[0]];
	pV3[pDIM[1]] = pV0[pDIM[1]];
	pV3[pDIM[2]] = pV0[pDIM[2]];

	vS[0] = pV[0]; vS[1] = pV[1];vS[2] = pV[2];

	vP[0] = v[0];vP[1] = v[3];
	if(!bBevel)
	{
		pV3[pDIM[0]] = pV0[pDIM[0]];
		pV3[pDIM[1]] = -pV0[pDIM[1]];
		pV3[pDIM[2]] = pV0[pDIM[2]];

		vP[2] = v[3];
	}

	UINT uiFi = 0;
	UINT uiBeginI = 0;
	UINT uiBeginVI = 0;
	BOOL bTop = (pDIM[0] == 0 && pDIM[1]== 2);
	if(bTop)
	{
		fInvTex[0] = (float)(1.f/(2 * sqrtf(2.f)*pF[0] + fL));
		fInvTex[1]  = (float)(1.f/(2 * sqrtf(2.f)*pF[1] + fH));
		fInvTex[2] = (float)(1.f/(2 * sqrtf(2.f)*pF[2] + fW));
	}

	float* pPosition ;
	float* pPosition2 ;
	float* pnormal  ;
	float* pnormal2 ;

	if(bBevel)
	{
		if(bTop)
		{
			for(uiFi = 0; uiFi < 2; uiFi++)
			{
				pIB[uiIBIndex]=uiVBIndex;
				uiIBIndex++;
				pIB[uiIBIndex]=uiFi + 2 + uiVBIndex;
				uiIBIndex++;
				pIB[uiIBIndex]=uiFi + 1 + uiVBIndex;
				uiIBIndex++;
			}
			for(uiFi = 0; uiFi < 4; uiFi++)
			{
				pPosition = (float*)(&pVB[uiVBIndex].vDyn);

				pVB[uiVBIndex].vDyn = v[uiFi];
				vNext = v[(uiFi+1)%4] - v[uiFi];
				vPre = v[(uiFi-1)&3] - v[uiFi];
				float3CrossProduct(&pVB[uiVBIndex].vNorm,&vPre,&vNext);
				float3Normalize(&pVB[uiVBIndex].vNorm,&pVB[uiVBIndex].vNorm);
				pVB[uiVBIndex].tu = pPosition[pDIM[0]] * fInvTex[pDIM[0]] + 0.5f;
				pVB[uiVBIndex].tv = 0.5f - pPosition[pDIM[1]] * fInvTex[pDIM[1]];

				//pVB[uiVBIndex].FaceID = uiFaceID;

				uiVBIndex ++;
			}
			//bottom Plane
			uiBeginI = uiIBIndex - 6;
			uiBeginVI = uiVBIndex - 4;

			for(uiFi = 0; uiFi < 2;uiFi++)
			{
				pIB[uiIBIndex]=pIB[uiBeginI] + uiVBIndex - uiBeginVI;
				uiIBIndex++;
				pIB[uiIBIndex]=pIB[uiBeginI+2] + uiVBIndex- uiBeginVI;
				uiIBIndex++;
				pIB[uiIBIndex]=pIB[uiBeginI+1] + uiVBIndex- uiBeginVI;
				uiIBIndex++;
				uiBeginI+=3;
			}
			for(uiFi = 0; uiFi < 4; uiFi++)
			{
				pPosition = (float*)(&pVB[uiVBIndex].vDyn);
				pPosition2 = (float*)(&pVB[uiBeginVI].vDyn);
				pnormal = (float*)(&pVB[uiVBIndex].vNorm);
				pnormal2 = (float*)(&pVB[uiBeginVI].vNorm);

				pPosition[pDIM[0]] = pPosition2[pDIM[0]];
				pPosition[pDIM[1]] = -pPosition2[pDIM[1]];
				pPosition[pDIM[2]] = pPosition2[pDIM[2]];
				pnormal[pDIM[0]] = pnormal2[pDIM[0]];
				pnormal[pDIM[1]] = -pnormal2[pDIM[1]];
				pnormal[pDIM[2]] = pnormal2[pDIM[2]];
				pVB[uiVBIndex].tu = pPosition[pDIM[0]] * fInvTex[pDIM[0]] + 0.5f;
				pVB[uiVBIndex].tv = 0.5f - pPosition[pDIM[1]] * fInvTex[pDIM[1]];

				//pVB[uiVBIndex].FaceID = uiFaceID;

				uiVBIndex ++;
				uiBeginVI ++;
			}
		}

		//right Plane
		if(bTop)
			v[1] = pV[2];
		else
			v[1] = (pV[0] + pV[2]) * 0.5f;

		pV2[pDIM[0]] = pV1[pDIM[0]];
		pV2[pDIM[1]] = -pV1[pDIM[1]];
		pV2[pDIM[2]] = pV1[pDIM[2]];

		pV3[pDIM[0]] = pV0[pDIM[0]];
		pV3[pDIM[1]] = -pV0[pDIM[1]];
		pV3[pDIM[2]] = pV0[pDIM[2]];


		vP[2] = v[3];
		for(uiFi = 0; uiFi < 2;uiFi++)
		{
			pIB[uiIBIndex]=uiVBIndex;
			uiIBIndex++;
			pIB[uiIBIndex]=uiVBIndex+1+uiFi;
			uiIBIndex++;
			pIB[uiIBIndex]=uiVBIndex+2+uiFi;
			uiIBIndex++;
		}
		for(uiFi = 0; uiFi < 4; uiFi++)
		{
			pPosition = (float*)(&pVB[uiVBIndex].vDyn);
			
			pVB[uiVBIndex].vDyn = v[uiFi];
			vNext = v[(uiFi+1)%4] - v[uiFi];
			vPre = v[(uiFi-1)&3] - v[uiFi];
			float3CrossProduct(&pVB[uiVBIndex].vNorm,&vNext,&vPre);
			float3Normalize(&pVB[uiVBIndex].vNorm,&pVB[uiVBIndex].vNorm);

			pVB[uiVBIndex].tu = pPosition[pDIM[0]] * fInvTex[pDIM[0]] + 0.5f;
			pVB[uiVBIndex].tv = 0.5f - pPosition[pDIM[1]] * fInvTex[pDIM[1]];

			//pVB[uiVBIndex].FaceID = uiFaceID;

			uiVBIndex++;
		}
		//left Plane
		uiBeginI = uiIBIndex - 6;
		uiBeginVI = uiVBIndex - 4;
		for(uiFi = 0; uiFi < 2;uiFi++)
		{
			pIB[uiIBIndex]=pIB[uiBeginI] + uiVBIndex - uiBeginVI;
			uiIBIndex++;
			pIB[uiIBIndex]=pIB[uiBeginI+2] + uiVBIndex - uiBeginVI;
			uiIBIndex++;
			pIB[uiIBIndex]=pIB[uiBeginI+1] + uiVBIndex - uiBeginVI;
			uiIBIndex++;
			uiBeginI += 3;
		}
		for(uiFi = 0; uiFi < 4; uiFi++)
		{
			pPosition = (float*)(&pVB[uiVBIndex].vDyn);
			pPosition2 = (float*)(&pVB[uiBeginVI].vDyn);
			pnormal = (float*)(&pVB[uiVBIndex].vNorm);
			pnormal2 = (float*)(&pVB[uiBeginVI].vNorm);

			pPosition[pDIM[0]] = -pPosition2[pDIM[0]];
			pPosition[pDIM[1]] = pPosition2[pDIM[1]];
			pPosition[pDIM[2]] = pPosition2[pDIM[2]];

			pnormal[pDIM[0]] = -pnormal2[pDIM[0]];
			pnormal[pDIM[1]] = pnormal2[pDIM[1]];
			pnormal[pDIM[2]] = pnormal2[pDIM[2]];

			pVB[uiVBIndex].tu = pPosition[pDIM[0]] * fInvTex[pDIM[0]] + 0.5f;
			pVB[uiVBIndex].tv = 0.5f - pPosition[pDIM[1]] * fInvTex[pDIM[1]];

			//pVB[uiVBIndex].FaceID = uiFaceID;

			uiVBIndex ++;
			uiBeginVI ++;
		}

		if(bTop)
		{
			float3 vNormal;
			//righttop Plane
			for(uiFi = 0; uiFi < 1;uiFi++)
			{
				pIB[uiIBIndex]=uiVBIndex;
				uiIBIndex++;
				pIB[uiIBIndex]=uiVBIndex+uiFi+1;
				uiIBIndex++;
				pIB[uiIBIndex]=uiVBIndex+uiFi+2;
				uiIBIndex++;
			}
			for(uiFi = 0; uiFi < 3; uiFi ++)
			{
				pPosition = (float*)(&pVB[uiVBIndex].vDyn);
				
				pVB[uiVBIndex].vDyn = vS[uiFi];
				if(uiFi == 0)
				{
					vNext = vS[1] - vS[0];
					vPre = vS[2] - vS[1];
					float3CrossProduct(&vNormal,&vNext,&vPre);
					float3Normalize(&vNormal,&vNormal);
				}
				pVB[uiVBIndex].vNorm = vNormal;

				pVB[uiVBIndex].tu = pPosition[pDIM[0]] * fInvTex[pDIM[0]] + 0.5f;
				pVB[uiVBIndex].tv = 0.5f - pPosition[pDIM[1]] * fInvTex[pDIM[1]];

				//pVB[uiVBIndex].FaceID = uiFaceID;

				uiVBIndex++;
			}
			//lefttop Plane
			uiBeginI = uiIBIndex - 3;
			uiBeginVI = uiVBIndex - 3;

			for(uiFi = 0; uiFi < 1;uiFi++)
			{
				pIB[uiIBIndex]=pIB[uiBeginI] + uiVBIndex - uiBeginVI;
				uiIBIndex++;
				pIB[uiIBIndex]=pIB[uiBeginI+2] + uiVBIndex- uiBeginVI;
				uiIBIndex++;
				pIB[uiIBIndex]=pIB[uiBeginI+1] + uiVBIndex- uiBeginVI;
				uiIBIndex++;
				uiBeginI += 3;
			}
			for(uiFi = 0; uiFi < 3; uiFi ++)
			{
				pPosition = (float*)(&pVB[uiVBIndex].vDyn);
				pPosition2 = (float*)(&pVB[uiBeginVI].vDyn);
				pnormal = (float*)(&pVB[uiVBIndex].vNorm);
				pnormal2 = (float*)(&pVB[uiBeginVI].vNorm);

				pPosition[pDIM[0]] = -pPosition2[pDIM[0]];
				pPosition[pDIM[1]] = pPosition2[pDIM[1]];
				pPosition[pDIM[2]] = pPosition2[pDIM[2]];

				pnormal[pDIM[0]] = -pnormal2[pDIM[0]];
				pnormal[pDIM[1]] = pnormal2[pDIM[1]];
				pnormal[pDIM[2]] = pnormal2[pDIM[2]];


				pVB[uiVBIndex].tu = pPosition[pDIM[0]] * fInvTex[pDIM[0]] + 0.5f;
				pVB[uiVBIndex].tv = 0.5f - pPosition[pDIM[1]] * fInvTex[pDIM[1]];

				//pVB[uiVBIndex].FaceID = uiFaceID;

				uiVBIndex ++;
				uiBeginVI ++;
			}

			//rightbottom Plane
			uiBeginI = uiBeginI - 3; 
			uiBeginVI = uiBeginVI - 3;

			for(uiFi = 0; uiFi < 1;uiFi++)
			{
				pIB[uiIBIndex]=pIB[uiBeginI] + uiVBIndex - uiBeginVI;
				uiIBIndex++;
				pIB[uiIBIndex]=pIB[uiBeginI+2] + uiVBIndex- uiBeginVI;
				uiIBIndex++;
				pIB[uiIBIndex]=pIB[uiBeginI+1] + uiVBIndex- uiBeginVI;
				uiIBIndex++;
				uiBeginI += 3;
			}
			for(uiFi = 0; uiFi < 3; uiFi ++)
			{
				pPosition = (float*)(&pVB[uiVBIndex].vDyn);
				pPosition2 = (float*)(&pVB[uiBeginVI].vDyn);
				pnormal = (float*)(&pVB[uiVBIndex].vNorm);
				pnormal2 = (float*)(&pVB[uiBeginVI].vNorm);

				pPosition[pDIM[0]] = pPosition2[pDIM[0]];
				pPosition[pDIM[1]] = -pPosition2[pDIM[1]];
				pPosition[pDIM[2]] = pPosition2[pDIM[2]];

				pnormal[pDIM[0]] = pnormal2[pDIM[0]];
				pnormal[pDIM[1]] = -pnormal2[pDIM[1]];
				pnormal[pDIM[2]] = pnormal2[pDIM[2]];


				pVB[uiVBIndex].tu = pPosition[pDIM[0]] * fInvTex[pDIM[0]] + 0.5f;
				pVB[uiVBIndex].tv = 0.5f - pPosition[pDIM[1]] * fInvTex[pDIM[1]];

				//pVB[uiVBIndex].FaceID = uiFaceID;

				uiVBIndex ++;
				uiBeginVI ++;
			}

			//leftbottom Plane
			uiBeginI = uiBeginI - 3;
			uiBeginVI = uiBeginVI - 3;

			for(uiFi = 0; uiFi < 3;uiFi++)
			{
				pIB[uiIBIndex]=pIB[uiBeginI] + uiVBIndex - uiBeginVI;
				uiIBIndex++;
				uiBeginI ++;
			}

			for(uiFi = 0; uiFi < 3; uiFi ++)
			{
				pPosition = (float*)(&pVB[uiVBIndex].vDyn);
				pPosition2 = (float*)(&pVB[uiBeginVI].vDyn);
				pnormal = (float*)(&pVB[uiVBIndex].vNorm);
				pnormal2 = (float*)(&pVB[uiBeginVI].vNorm);

				pPosition[pDIM[0]] = -pPosition2[pDIM[0]];
				pPosition[pDIM[1]] = -pPosition2[pDIM[1]];
				pPosition[pDIM[2]] = pPosition2[pDIM[2]];

				pnormal[pDIM[0]] = -pnormal2[pDIM[0]];
				pnormal[pDIM[1]] = -pnormal2[pDIM[1]];
				pnormal[pDIM[2]] = pnormal2[pDIM[2]];


				pVB[uiVBIndex].tu = pPosition[pDIM[0]] * fInvTex[pDIM[0]] + 0.5f;
				pVB[uiVBIndex].tv = 0.5f - pPosition[pDIM[1]] * fInvTex[pDIM[1]];

				//pVB[uiVBIndex].FaceID = uiFaceID;

				uiVBIndex ++;
				uiBeginVI ++;
			}
		}
	}

	float3 fVStep = (vP[2]-vP[1]) / (float)(nDivision);
	int nIdx = 0,a,b,c,d;

	float3 vNormal = {0.f,0.f,0.f};
	float* pVNormal = (float*)&vNormal;
	float* pVp = (float*)&vP[0];
	pVNormal[pDIM[2]] = pVp[pDIM[2]] < 0 ? -1.f : 1.f; 
	//vNormal[pDIM[2]] =  vP[0][pDIM[2]] < 0 ? -1.f : 1.f; 

	nIdx = 0;
	int nX;
	for (nX=0;nX<nDivision;nX++)
	{
		int nCBase0 = nX * (nDivision+1);
		int nCBase1 = nCBase0 + (nDivision+1);
		for (int nY=0;nY<nDivision;nY++)
		{
			a = nCBase0 + nY + uiVBIndex; b = a + 1;
			c = nCBase1 + nY + uiVBIndex; d = c + 1;
			pIB[uiIBIndex++] = a;
			pIB[uiIBIndex++] = c;
			pIB[uiIBIndex++] = b;
			pIB[uiIBIndex++] = c;
			pIB[uiIBIndex++] = d;
			pIB[uiIBIndex++] = b;
		}
	}

	for (nX=0;nX<=nDivision;nX++)
	{
		float* pVP = (float*)(&vP[1]);
		float* pfVStep = (float *)&fVStep;
		float xx = pVP[pDIM[0]] + pfVStep[pDIM[0]]*nX;



		for (int nY=0;nY<=nDivision;nY++)
		{
			pPosition = (float*)(&pVB[uiVBIndex].vDyn);
			pPosition2 = (float*)(&pVB[uiBeginVI].vDyn);
			pnormal = (float*)(&pVB[uiVBIndex].vNorm);
			pnormal2 = (float*)(&pVB[uiBeginVI].vNorm);

			pPosition[pDIM[0]] = xx;
			pPosition[pDIM[1]] = pVP[pDIM[1]] + pfVStep[pDIM[1]]*nY;
			pPosition[pDIM[2]] = pVP[pDIM[2]];
			pVB[uiVBIndex].vNorm = vNormal;
			pVB[uiVBIndex].tu = pPosition[pDIM[0]] * fInvTex[pDIM[0]] + 0.5f;
			pVB[uiVBIndex].tv = 0.5f - pPosition[pDIM[1]] * fInvTex[pDIM[1]];


			uiVBIndex++;
		}
	}
}

void CreateBevelSphere(UINT uiPrecision,SYVertex3D** ppVB,LPDWORD* ppIB,UINT& nF,UINT& nV,BOOL bCenterSplitMode)
{
	assert(uiPrecision > 0 && ppVB && ppIB);
	if(!ppVB || !ppIB) 
		return;
	
	UINT uPrecisionFactor = 1;
	if(!bCenterSplitMode)
	{
		UINT uSlices,uStacks;
		uSlices = uStacks = uiPrecision * uPrecisionFactor;
		if((int)uSlices <= 0 )
			uSlices = uStacks = uPrecisionFactor;
		nV = (uSlices + 1) * (uStacks + 1);
		nF = uSlices * uStacks * 2;
		*ppVB = new SYVertex3D[nV];
		*ppIB = new DWORD[nF*3];

		float fHalfPai = ((float)FLOAT_PI/ 2.0f);
		float fTh = (float)fHalfPai/uSlices;	
		float fPh = fTh;
		float fThSum = fHalfPai;
		float fPhSum = fHalfPai;

		float x,y,z,r;
		float R = 0.5f;
		int nIndex = 0;
		UINT i;
		for ( i=0;i<=uSlices;i++)
		{
			fPhSum = fHalfPai;
			y = R * sinf(fThSum);
			r = R * cosf(fThSum);
			for (UINT j=0;j<=uStacks;j++)
			{
				x = r * cosf(fPhSum);
				z = r * sinf(fPhSum);
				//vector3f pt =  {x, y, z};
				//D3DXVECTOR3 pt = D3DXVECTOR3(x,y,z);
				//(*ppVB)[nIndex++].vDyn = pt;
				float3Set(&(((*ppVB)[nIndex++]).vDyn), x, y, z);
				fPhSum -= fPh;
			}
			fThSum -= fTh;
		}

		int a,b,c,d;
		nIndex = 0;
		for (i=0;i<uSlices;i++)
		{
			for (UINT j=0;j<uStacks;j++)
			{
				a = i * (uStacks+1) + j;
				b = a + 1;
				c = a + (uStacks+1);
				d = c + 1;
				(*ppIB)[nIndex++] = a;
				(*ppIB)[nIndex++] = d;
				(*ppIB)[nIndex++] = b;
				(*ppIB)[nIndex++] = d;
				(*ppIB)[nIndex++] = a;
				(*ppIB)[nIndex++] = c;
			}
		}
	}
	else
	{
		UINT uSegs = uiPrecision * uPrecisionFactor;
		if((int)uSegs <= 0 )
			uSegs = uPrecisionFactor;
		nV = (uSegs + 1) * (uSegs + 1);
		nF = uSegs * uSegs * 2;
		*ppVB = new SYVertex3D[nV];
		*ppIB = new DWORD[nF*3];

		float fQuadPai = (float)(FLOAT_PI / 4.0f);	
		float fAngleStep = (float)fQuadPai/uSegs;
		float fAlpha = 0.f;	float fBeta = 0.f;	
		float R = 0.5f;int nIndex = 0;

		/*D3DXVECTOR3*/float3 VSphere;
		UINT i;
		for (i=0;i<=uSegs;i++)
		{
			if(i == uSegs)
				fAlpha = fQuadPai;
			fBeta = 0.f;
			float fSA = sinf(fAlpha);
			float fCA = cosf(fAlpha);
			for (UINT j=0;j<=uSegs;j++)
			{
				if(j == uSegs)
					fBeta = fQuadPai;
				float fSBeta = sinf(fBeta);
				float fCBeta = cosf(fBeta);
				VSphere.x = (float)(fSBeta*fCA);
				VSphere.y = (float)(fCBeta*fCA);
				VSphere.z = (float)(fCBeta*fSA);

				float3Normalize(&VSphere,&VSphere);
				float3Scale(&VSphere,R);
// 				D3DXVec3Normalize(&VSphere,&VSphere);
// 				D3DXVec3Scale(&VSphere,&VSphere,R);
// 				D3DXVECTOR3 pt = D3DXVECTOR3(VSphere.x,VSphere.y,VSphere.z);
// 				(*ppVB)[nIndex++].position = pt;
				float3Set(&(((*ppVB)[nIndex++]).vDyn), VSphere.x,VSphere.y,VSphere.z);
				
				fBeta += fAngleStep;
			}
			fAlpha += fAngleStep;
		}

		int a,b,c,d;
		nIndex = 0;
		for (i=0;i<uSegs;i++)
		{
			for (UINT j=0;j<uSegs;j++)
			{
				a = i * (uSegs+1) + j;
				b = a + 1;
				c = a + (uSegs+1);
				d = c + 1;
				(*ppIB)[nIndex++] = a;
				(*ppIB)[nIndex++] = b;
				(*ppIB)[nIndex++] = d;
				(*ppIB)[nIndex++] = d;
				(*ppIB)[nIndex++] = c;
				(*ppIB)[nIndex++] = a;
			}
		}
	}
}


void CreateBevelCylinder(UINT uiPrecision,UINT uiSegments,SYVertex3D** ppVB,LPDWORD* ppIB,UINT& nF,UINT& nV,BOOL bCenterSplitMode)
{
	assert(uiPrecision > 0 && ppVB && ppIB);
	if(!ppVB || !ppIB) 
		return;

	UINT uPrecisionFactor = 1;
	UINT uSlices = uiPrecision * uPrecisionFactor;
	if((int)uSlices <= 0 )
		uSlices = uPrecisionFactor;
	if((int)uiSegments <= 0)
		uiSegments = 1;

	nV = (uSlices + 1) * (uiSegments+1);
	nF = uSlices * uiSegments * 2;
	*ppVB = new SYVertex3D[nV];
	*ppIB = new DWORD[nF*3];

	float fAngle = bCenterSplitMode ? (float)(FLOAT_PI / 4.0f / uSlices) : (float)(FLOAT_PI / 2.0f / uSlices); 
	float fAngleSum = 0.f;
	float x,z,y;
	float R = 0.5f;
	float fStep = 1.f/uiSegments;

	for (UINT ui=0;ui <= uSlices;ui++)
	{
		if(ui == uSlices)
		{
			fAngleSum = bCenterSplitMode ? (float)(FLOAT_PI / 4.0f) : (float)(FLOAT_PI / 2.0f);
		}
		x = R * sinf(fAngleSum);
		z = R * cosf(fAngleSum);
		fAngleSum += fAngle;
		for(UINT uS = 0; uS <= uiSegments; uS++)
		{
			if(uS != uiSegments)
				y = -0.5f + uS * fStep;
			else
				y = 0.5f;

			//(*ppVB)[(uiSegments+1)*ui + uS] = MODELVERTEX(x,y,z,0.f,1.f,0.f,0.f,0.f);
			float3Set(&(((*ppVB)[(uiSegments+1)*ui + uS]).vDyn), x, y, z);
			float3Set(&(((*ppVB)[(uiSegments+1)*ui + uS]).vNorm), 0.f,1.f,0.f);
			((*ppVB)[(uiSegments+1)*ui + uS]).tu = 0.f;
			((*ppVB)[(uiSegments+1)*ui + uS]).tv = 0.f;
			
		}
	}

	UINT a,b,c,d;
	int nIndex = 0;
	for (UINT uj=0;uj<uSlices;uj++)
	{
		for(UINT uS = 0; uS < uiSegments; uS++)
		{
			a = uj * (uiSegments+1) + uS;
			b = a + 1;
			c = a + (uiSegments + 1);
			d = c + 1;
			(*ppIB)[nIndex++] = a;
			(*ppIB)[nIndex++] = b;
			(*ppIB)[nIndex++] = d;
			(*ppIB)[nIndex++] = d;
			(*ppIB)[nIndex++] = c;
			(*ppIB)[nIndex++] = a;
		}
	}
}

BOOL CreateSphereBevelBox(SYModel* pMod,float m_fLength,float m_fHeight,float m_fThick,int m_nDivision,float uPartType,float m_fTransitionLen,float m_fTransitionHeight,UINT nTSPrecission)
{
	//Create elements,these elements can be shared between box models!
	UINT uiBSVCnt = 0,uiBSFCnt = 0,uiBCVCnt = 0,uiBCFCnt = 0,uiBHCVCnt = 0,uiBHCFCnt = 0;
	SYVertex3D *pBSVB = NULL,*pBCVB = NULL,*pBHCVB = NULL;
	DWORD *pBSIB = NULL,*pBCIB = NULL,*pBHCIB = NULL;
	if(m_fTransitionLen > ABS_FLOAT_0)
	{
		//1.Create Bevel sphere
		CreateBevelSphere(nTSPrecission*2,
			&pBSVB,&pBSIB,uiBSFCnt,uiBSVCnt,FALSE);

		//2.Create bevel half cylinder
		CreateBevelCylinder(nTSPrecission,m_nDivision,
			&pBHCVB,&pBHCIB,uiBHCFCnt,uiBHCVCnt,TRUE);

		//3.Create bevel cylinder
		CreateBevelCylinder(nTSPrecission*2,m_nDivision,
			&pBCVB,&pBCIB,uiBCFCnt,uiBCVCnt,FALSE);

	}

	UINT uiFVNum = 0;//((m_nDivision+1)*(m_nDivision+1) + (uiBSVCnt+uiBCVCnt)*4);

	//4. Create plane
	UINT uiPlaneVCnt = (m_nDivision+1)*(m_nDivision+1);
	UINT uiPlaneFCnt = m_nDivision*m_nDivision*2;
	/*MODELVERTEX*/SYVertex3D* pPlaneVB = new SYVertex3D[uiPlaneVCnt];
	DWORD* pPlaneIB = new DWORD[uiPlaneFCnt*3];
	float fStep = 1.f / (m_nDivision);
	float px,py;
	for (int nR=0;nR<m_nDivision+1;nR++)
	{
		int nRIndex = nR * (m_nDivision+1);
		py = fStep * nR - .5f;
		for (int nC=0;nC<m_nDivision+1;nC++)
		{
			px = fStep * nC - .5f;
			//pPlaneVB[nRIndex+nC] = //SYVertex3D(,,);

			float3Set(&pPlaneVB[nRIndex+nC].vDyn, px,py,0.f);	
			float3Set(&pPlaneVB[nRIndex+nC].vNorm, 0.f,0.f,1.f);

			pPlaneVB[nRIndex+nC].tu = 0.f;
			pPlaneVB[nRIndex+nC].tv = 0.f;
			//pVB[m_uiVBIndex].FaceID = m_uiFaceID;
		}
	}
	int nIdx = 0,a,b,c,d;
	for (int nY=0;nY<m_nDivision;nY++)
	{
		int nCBase0 = nY * (m_nDivision+1);
		int nCBase1 = nCBase0 + (m_nDivision+1);
		for (int nX=0;nX<m_nDivision;nX++)
		{
			a = nCBase0 + nX; b = a + 1;
			c = nCBase1 + nX; d = c + 1;
			pPlaneIB[nIdx++] = a;
			pPlaneIB[nIdx++] = b;
			pPlaneIB[nIdx++] = c;
			pPlaneIB[nIdx++] = c;
			pPlaneIB[nIdx++] = b;
			pPlaneIB[nIdx++] = d;
		}
	}

	//create box
	pMod->vcount/*m_nV*/ = (m_nDivision+1)*(m_nDivision+1)*6 + (uiBSVCnt+uiBCVCnt)*8 + uiBHCVCnt*8 + uiFVNum*8;
	pMod->icount/*m_nF*/ = (m_nDivision*m_nDivision*12 + (uiBSFCnt+uiBCFCnt)*8 + uiBHCFCnt*8 +uiFVNum*8) * 3;

// 	m_pVB = new MODELVERTEX[m_nV];
// 	m_pIB = new DWORD[m_nF*3];
// 	m_pAttrTable = new D3DX10_ATTRIBUTE_RANGE[6];

	pMod->v = new SYVertex3D[pMod->vcount]; 
	pMod->indecs = new DWORD[pMod->icount];
	pMod->nviewsub = 6;
	pMod->viewsub = new SYSubModel[6];

	float fL = m_fLength - 2*m_fTransitionLen;
	float fH = m_fHeight - 2*m_fTransitionLen;
	float fW = m_fThick - 2*m_fTransitionLen;

	UINT uiVBIndex = 0;	
	UINT uiIBIndex = 0;
	UINT uiFaceStart = 0;

	//0 front
	UINT m_uiFaceID = 0;
	float xx,yy,zz;
	float fInvFLTex = (float)(1.f/(sqrtf(2.f)*m_fTransitionLen + fL));
	float fInvFHTex = (float)(1.f/fH);
	float fInvFWTex = (float)(1.f/(sqrtf(2.f)*m_fTransitionLen + fW));

	float fInvFLTexTop = (float)(1.f/(2*sqrtf(2.f)*m_fTransitionLen + fL));
	float fInvFWTexTop = (float)(1.f/(2*sqrtf(2.f)*m_fTransitionLen + fW));


	//right cylinder
	DWORD *pIB = pMod->indecs;
	UINT uiFi;
	for( uiFi = 0; uiFi < uiBHCFCnt*3;uiFi++)
	{
		pIB[uiIBIndex]=pBHCIB[uiFi] + uiVBIndex;
		uiIBIndex++;
	}
	SYVertex3D *pVB = pMod->v;
	for(uiFi = 0; uiFi < uiBHCVCnt; uiFi++)
	{
		xx = pBHCVB[uiFi].x;
		yy = pBHCVB[uiFi].y;
		zz = pBHCVB[uiFi].z;
		//right Cylinder
		float3Set(&pVB[uiVBIndex].vDyn, xx, yy, -zz);
		float3Set(&pVB[uiVBIndex].vNorm, pVB[uiVBIndex].x, 0, pVB[uiVBIndex].z);

		float3Normalize(&pVB[uiVBIndex].vNorm, &pVB[uiVBIndex].vNorm);

		pVB[uiVBIndex].x *=  2 * m_fTransitionLen;
		pVB[uiVBIndex].y *= fH;
		pVB[uiVBIndex].z *= 2 * m_fTransitionLen;
		
		pVB[uiVBIndex].x += fL / 2.f;
		pVB[uiVBIndex].z -= fW / 2.f;

		pVB[uiVBIndex].tu = pVB[uiVBIndex].x * fInvFLTex + 0.5f;
		pVB[uiVBIndex].tv = 0.5f - pVB[uiVBIndex].y * fInvFHTex;

		//m_pVB[uiVBIndex].FaceID = m_uiFaceID;

		uiVBIndex++;
	}
	//left Cylinder 
	for(uiFi = 0; uiFi < uiBHCFCnt;uiFi++)
	{
		pIB[uiIBIndex]=pBHCIB[uiFi*3] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pBHCIB[uiFi*3+2] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pBHCIB[uiFi*3+1] + uiVBIndex;
		uiIBIndex++;
	}
	UINT uiBeginVI = uiVBIndex - uiBHCVCnt;
	for(uiFi = 0; uiFi < uiBHCVCnt; uiFi++)
	{
		float3Set(&pVB[uiVBIndex].vDyn, -pVB[uiBeginVI].x, pVB[uiBeginVI].y, pVB[uiBeginVI].z);
		float3Set(&pVB[uiVBIndex].vNorm, -pVB[uiBeginVI].nx, pVB[uiBeginVI].ny, pVB[uiBeginVI].nz);

		pVB[uiVBIndex].tu = pVB[uiVBIndex].x * fInvFLTex + 0.5f;
		pVB[uiVBIndex].tv = 0.5f - pVB[uiVBIndex].y * fInvFHTex;

		//m_pVB[uiVBIndex].FaceID = m_uiFaceID;

		uiVBIndex ++;
		uiBeginVI ++;
	}
	for(uiFi = 0; uiFi < uiPlaneFCnt; uiFi++)
	{
		pIB[uiIBIndex]=pPlaneIB[uiFi*3] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pPlaneIB[uiFi*3 + 2] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pPlaneIB[uiFi*3 +1] + uiVBIndex;
		uiIBIndex++;

	}
	for(uiFi = 0; uiFi < uiPlaneVCnt; uiFi++)
	{
		float3Set(&pVB[uiVBIndex].vDyn, pPlaneVB[uiFi].x * fL, pPlaneVB[uiFi].y * fH, pPlaneVB[uiFi].z - m_fThick/2.f);
		float3Set(&pVB[uiVBIndex].vNorm,0.f, 0.f, -1.f);

		pVB[uiVBIndex].tu = pVB[uiVBIndex].x * fInvFLTex + 0.5f;
		pVB[uiVBIndex].tv = 0.5f - pVB[uiVBIndex].y * fInvFHTex;

	//	m_pVB[uiVBIndex].FaceID = m_uiFaceID;

		uiVBIndex ++;
	}
	UINT uiFrontVCnt = uiVBIndex;
	UINT uiFrontIndexCnt = uiIBIndex;
	UINT uiFrontFaceCnt = m_nDivision*m_nDivision*2 + uiBHCFCnt*2;
	assert(uiFrontFaceCnt * 3 == uiIBIndex);

	for(int iii = 0; iii < (int)uiFVNum; iii++)
	{
		//CreateNormalPrimitive(&m_pVB[iii],&m_pVB,&m_pIB,uiVBIndex,uiIBIndex);
		CreateNormalPrimitive(&pMod->v[iii],&pMod->v,&pMod->indecs,uiVBIndex,uiIBIndex);
	}


	UINT uiFaceOff = 8*uiFVNum;

// 	m_pAttrTable[0].AttribId = 0;
// 	m_pAttrTable[0].FaceStart = uiFaceStart;
// 	m_pAttrTable[0].FaceCount = uiFrontFaceCnt + uiFaceOff;
// 	m_pAttrTable[0].VertexStart = 0;
// 	m_pAttrTable[0].VertexCount = m_nV;
// 	uiFaceStart += m_pAttrTable[0].FaceCount;

	pMod->viewsub[0].vstart= 0;
	pMod->viewsub[0].vcount =  pMod->vcount;
	pMod->viewsub[0].istart = uiFaceStart*3;
	pMod->viewsub[0].icount = (uiFrontFaceCnt + uiFaceOff)*3;
	pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[0].nTri_Num  = uiFrontFaceCnt + uiFaceOff;
	pMod->viewsub[0].pMaterial = NULL;
	uiFaceStart += uiFrontFaceCnt + uiFaceOff/*uiPlaneFCnt*/;

	//1 back
	m_uiFaceID = 1;
	for(uiFi = 0; uiFi < uiFrontFaceCnt; uiFi++)
	{
		pIB[uiIBIndex]= pIB[uiFi*3] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiFi*3 + 2] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiFi*3 + 1] + uiVBIndex;
		uiIBIndex++;
	}
	for(uiFi = 0; uiFi < uiFrontVCnt; uiFi++)
	{
		float3Set(&pVB[uiVBIndex].vDyn, pVB[uiFi].x, pVB[uiFi].y, -pVB[uiFi].z);
		float3Set(&pVB[uiVBIndex].vNorm,pVB[uiFi].nx, pVB[uiFi].ny, -pVB[uiFi].nz);
		pVB[uiVBIndex].tu = 0.5f - pVB[uiVBIndex].x * fInvFLTex;
		pVB[uiVBIndex].tv = 0.5f - pVB[uiVBIndex].y * fInvFHTex;

		//m_pVB[uiVBIndex].FaceID = m_uiFaceID;

		uiVBIndex ++;
	}
// 	m_pAttrTable[1].AttribId = 1;
// 	m_pAttrTable[1].FaceStart = uiFaceStart;
// 	m_pAttrTable[1].FaceCount = uiFrontFaceCnt;
// 	m_pAttrTable[1].VertexStart = 0;
// 	m_pAttrTable[1].VertexCount = m_nV;
// 	uiFaceStart += m_pAttrTable[1].FaceCount;
	pMod->viewsub[1].vstart= 0;
	pMod->viewsub[1].vcount =  pMod->vcount;
	pMod->viewsub[1].istart = uiFaceStart*3;
	pMod->viewsub[1].icount = uiFrontFaceCnt*3;
	pMod->viewsub[1].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[1].nTri_Num = uiFrontFaceCnt;
	pMod->viewsub[1].pMaterial = NULL;
	uiFaceStart += uiFrontFaceCnt/*uiPlaneFCnt*/;

	//2 bottom
	m_uiFaceID = 2;
	/////////////////////////
	//back cylinder
	for(uiFi = 0; uiFi < uiBCFCnt;uiFi++)
	{
		pIB[uiIBIndex]=pBCIB[uiFi*3] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pBCIB[uiFi*3 +2] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pBCIB[uiFi*3 + 1] + uiVBIndex;
		uiIBIndex++;
	}
	for(uiFi = 0; uiFi < uiBCVCnt; uiFi++)
	{
		xx = pBCVB[uiFi].x;
		yy = pBCVB[uiFi].y;
		zz = pBCVB[uiFi].z;
		float3Set(&pVB[uiVBIndex].vDyn, yy, -xx, zz);
		float3Set(&pVB[uiVBIndex].vNorm,pVB[uiVBIndex].x, pVB[uiVBIndex].y, pVB[uiVBIndex].z);
		pVB[uiVBIndex].nx = 0;
		float3Normalize(&pVB[uiVBIndex].vNorm,&pVB[uiVBIndex].vNorm);

		pVB[uiVBIndex].x *= fL;
		pVB[uiVBIndex].y *= 2 * m_fTransitionLen;
		pVB[uiVBIndex].z *= 2 * m_fTransitionLen;

		pVB[uiVBIndex].y -= fH / 2.f;
		pVB[uiVBIndex].z += fW / 2.f;
		pVB[uiVBIndex].tu = pVB[uiVBIndex].x * fInvFLTexTop + 0.5f;
		pVB[uiVBIndex].tv = 0.5f + pVB[uiVBIndex].z * fInvFWTexTop;

		//m_pVB[uiVBIndex].FaceID = m_uiFaceID;

		uiVBIndex ++;
	}
	//front Cylinder
	for(uiFi = 0; uiFi < uiBCFCnt*3;uiFi++)
	{
		pIB[uiIBIndex]=pBCIB[uiFi] + uiVBIndex;
		uiIBIndex++;
	}
	uiBeginVI = uiVBIndex - uiBCVCnt;
	for(uiFi = 0; uiFi < uiBCVCnt; uiFi++)
	{
		pVB[uiVBIndex].x = pVB[uiBeginVI].x;
		pVB[uiVBIndex].y = pVB[uiBeginVI].y;
		pVB[uiVBIndex].z = -pVB[uiBeginVI].z;
		pVB[uiVBIndex].nx = pVB[uiBeginVI].nx;
		pVB[uiVBIndex].ny = pVB[uiBeginVI].ny;
		pVB[uiVBIndex].nz = -pVB[uiBeginVI].nz;
		pVB[uiVBIndex].tu = pVB[uiVBIndex].x * fInvFLTexTop + 0.5f;
		pVB[uiVBIndex].tv = 0.5f + pVB[uiVBIndex].z * fInvFWTexTop;

		//m_pVB[uiVBIndex].FaceID = m_uiFaceID;

		uiVBIndex ++;
		uiBeginVI ++;
	}

	//right cylinder
	for(uiFi = 0; uiFi < uiBCFCnt;uiFi++)
	{
		pIB[uiIBIndex]=pBCIB[uiFi*3] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pBCIB[uiFi*3+2] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pBCIB[uiFi*3+1] + uiVBIndex;
		uiIBIndex++;

	}
	for(uiFi = 0; uiFi < uiBCVCnt; uiFi++)
	{
		xx = pBCVB[uiFi].x;
		yy = pBCVB[uiFi].y;
		zz = pBCVB[uiFi].z;
		pVB[uiVBIndex].x = xx;
		pVB[uiVBIndex].y = -zz;
		pVB[uiVBIndex].z = yy;

		float3Set(&pVB[uiVBIndex].vNorm, pVB[uiVBIndex].x, pVB[uiVBIndex].y, pVB[uiVBIndex].z);
		pVB[uiVBIndex].nz = 0;
		float3Normalize(&pVB[uiVBIndex].vNorm,&pVB[uiVBIndex].vNorm);
	
		pVB[uiVBIndex].x *= 2 * m_fTransitionLen;
		pVB[uiVBIndex].y *= 2 * m_fTransitionLen;
		pVB[uiVBIndex].z *= fW;

		pVB[uiVBIndex].y -= fH / 2.f;
		pVB[uiVBIndex].x += fL / 2.f;
		pVB[uiVBIndex].tu = pVB[uiVBIndex].x * fInvFLTexTop + 0.5f;
		pVB[uiVBIndex].tv = 0.5f + pVB[uiVBIndex].z * fInvFWTexTop;

		//m_pVB[uiVBIndex].FaceID = m_uiFaceID;
		uiVBIndex ++;
	}
	//left Cylinder
	for(uiFi = 0; uiFi < uiBCFCnt*3;uiFi++)
	{
		pIB[uiIBIndex]=pBCIB[uiFi] + uiVBIndex;
		uiIBIndex++;
	}
	uiBeginVI = uiVBIndex - uiBCVCnt;
	for(uiFi = 0; uiFi < uiBCVCnt; uiFi++)
	{
		pVB[uiVBIndex].x = -pVB[uiBeginVI].x;
		pVB[uiVBIndex].y =  pVB[uiBeginVI].y;
		pVB[uiVBIndex].z =  pVB[uiBeginVI].z;
		pVB[uiVBIndex].nx = -pVB[uiBeginVI].nx;
		pVB[uiVBIndex].ny =  pVB[uiBeginVI].ny;
		pVB[uiVBIndex].nz =  pVB[uiBeginVI].nz;
		pVB[uiVBIndex].tu =  pVB[uiVBIndex].x * fInvFLTexTop + 0.5f;
		pVB[uiVBIndex].tv = 0.5f + pVB[uiVBIndex].z * fInvFWTexTop;

		//m_pVB[uiVBIndex].FaceID = m_uiFaceID;

		uiVBIndex ++;
		uiBeginVI ++;
	}

	//backright sphere
	for(uiFi = 0; uiFi < uiBSFCnt;uiFi++)
	{
		pIB[uiIBIndex]=pBSIB[uiFi*3] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pBSIB[uiFi*3+2] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pBSIB[uiFi*3+1] + uiVBIndex;
		uiIBIndex++;
	}
	for(uiFi = 0; uiFi < uiBSVCnt; uiFi ++)
	{
		xx = pBSVB[uiFi].x;
		yy = pBSVB[uiFi].y;
		zz = pBSVB[uiFi].z;

		pVB[uiVBIndex].x = xx;
		pVB[uiVBIndex].y = -yy;
		pVB[uiVBIndex].z = zz;

		float3Normalize(&pVB[uiVBIndex].vNorm,&pVB[uiVBIndex].vDyn);

		pVB[uiVBIndex].x *= 2 * m_fTransitionLen;
		pVB[uiVBIndex].x += fL/2.f;
		pVB[uiVBIndex].y *= 2 * m_fTransitionLen;
		pVB[uiVBIndex].y -= fH/2.f;
		pVB[uiVBIndex].z *= 2 * m_fTransitionLen;
		pVB[uiVBIndex].z += fW/2.f;

		pVB[uiVBIndex].tu = pVB[uiVBIndex].x * fInvFLTexTop + 0.5f;
		pVB[uiVBIndex].tv = 0.5f + pVB[uiVBIndex].z * fInvFWTexTop;

		//pVB[uiVBIndex].FaceID = m_uiFaceID;

		uiVBIndex++;
	}
	//frontright sphere
	for(uiFi = 0; uiFi < uiBSFCnt*3;uiFi++)
	{
		pIB[uiIBIndex]=pBSIB[uiFi] + uiVBIndex;
		uiIBIndex++;
	}
	uiBeginVI = uiVBIndex - uiBSVCnt;
	for(uiFi = 0; uiFi < uiBSVCnt; uiFi ++)
	{
		pVB[uiVBIndex].x =  pVB[uiBeginVI].x;
		pVB[uiVBIndex].y =  pVB[uiBeginVI].y;
		pVB[uiVBIndex].z = -pVB[uiBeginVI].z;

		pVB[uiVBIndex].nx =  pVB[uiBeginVI].nx;
		pVB[uiVBIndex].ny =  pVB[uiBeginVI].ny;
		pVB[uiVBIndex].nz = -pVB[uiBeginVI].nz;

		pVB[uiVBIndex].tu = pVB[uiVBIndex].x * fInvFLTexTop + 0.5f;
		pVB[uiVBIndex].tv = 0.5f + pVB[uiVBIndex].z * fInvFWTexTop;

		//m_pVB[uiVBIndex].FaceID = m_uiFaceID;

		uiVBIndex ++;
		uiBeginVI ++;
	}

	//backleft sphere
	for(uiFi = 0; uiFi < uiBSFCnt*3;uiFi++)
	{
		pIB[uiIBIndex]=pBSIB[uiFi] + uiVBIndex;
		uiIBIndex++;
	}
	uiBeginVI = uiBeginVI - uiBSVCnt;
	for(uiFi = 0; uiFi < uiBSVCnt; uiFi ++)
	{
		pVB[uiVBIndex].x = -pVB[uiBeginVI].x;
		pVB[uiVBIndex].y =  pVB[uiBeginVI].y;
		pVB[uiVBIndex].z =  pVB[uiBeginVI].z;

		pVB[uiVBIndex].nx = -pVB[uiBeginVI].nx;
		pVB[uiVBIndex].ny =  pVB[uiBeginVI].ny;
		pVB[uiVBIndex].nz =  pVB[uiBeginVI].nz;


		pVB[uiVBIndex].tu = pVB[uiVBIndex].x * fInvFLTexTop + 0.5f;
		pVB[uiVBIndex].tv = 0.5f + pVB[uiVBIndex].z * fInvFWTexTop;

		//m_pVB[uiVBIndex].FaceID = m_uiFaceID;

		uiVBIndex ++;
		uiBeginVI ++;
	}

	//frontleft sphere
	for(uiFi = 0; uiFi < uiBSFCnt;uiFi++)
	{
		pIB[uiIBIndex]=pBSIB[uiFi*3] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pBSIB[uiFi*3+2] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pBSIB[uiFi*3+1] + uiVBIndex;
		uiIBIndex++;
	}
	uiBeginVI = uiBeginVI - uiBSVCnt;
	for(uiFi = 0; uiFi < uiBSVCnt; uiFi ++)
	{
		pVB[uiVBIndex].x = -pVB[uiBeginVI].x;
		pVB[uiVBIndex].y =  pVB[uiBeginVI].y;
		pVB[uiVBIndex].z = -pVB[uiBeginVI].z;

		pVB[uiVBIndex].nx = -pVB[uiBeginVI].nx;
		pVB[uiVBIndex].ny =  pVB[uiBeginVI].ny;
		pVB[uiVBIndex].nz = -pVB[uiBeginVI].nz;

		pVB[uiVBIndex].tu = pVB[uiVBIndex].x * fInvFLTexTop + 0.5f;
		pVB[uiVBIndex].tv = 0.5f + pVB[uiVBIndex].z * fInvFWTexTop;

		//m_pVB[uiVBIndex].FaceID = m_uiFaceID;

		uiVBIndex ++;
		uiBeginVI ++;
	}

	for(uiFi = 0; uiFi < uiPlaneFCnt; uiFi++)
	{
		pIB[uiIBIndex]=pPlaneIB[uiFi*3] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pPlaneIB[uiFi*3 + 2] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pPlaneIB[uiFi*3 +1] + uiVBIndex;
		uiIBIndex++;

	}

	for(uiFi = 0; uiFi < uiPlaneVCnt; uiFi++)
	{
		xx = pPlaneVB[uiFi].x;
		yy = pPlaneVB[uiFi].y;
		zz = pPlaneVB[uiFi].z;
		pVB[uiVBIndex].x = xx;
		pVB[uiVBIndex].y = zz;
		pVB[uiVBIndex].z = -yy;

		pVB[uiVBIndex].x *= fL;
		pVB[uiVBIndex].y -= m_fHeight/2.f;
		pVB[uiVBIndex].z *= fW;

		pVB[uiVBIndex].nx = 0.f;
		pVB[uiVBIndex].ny = -1.f;
		pVB[uiVBIndex].nz = 0.f;

		pVB[uiVBIndex].tu = pVB[uiVBIndex].x * fInvFLTexTop + 0.5f;
		pVB[uiVBIndex].tv = 0.5f + pVB[uiVBIndex].z * fInvFWTexTop;

	//	m_pVB[uiVBIndex].FaceID = m_uiFaceID;

		uiVBIndex ++;
	}


	UINT uiBFCnt = m_nDivision*m_nDivision*2 + (uiBSFCnt + uiBCFCnt)*4;
// 	m_pAttrTable[2].AttribId = 2;
// 	m_pAttrTable[2].FaceStart = uiFaceStart;
// 	m_pAttrTable[2].FaceCount = uiBFCnt;
// 	m_pAttrTable[2].VertexStart = 0;
// 	m_pAttrTable[2].VertexCount = m_nV;
//	uiFaceStart += m_pAttrTable[2].FaceCount;

	pMod->viewsub[2].vstart= 0;
	pMod->viewsub[2].vcount =  pMod->vcount;
	pMod->viewsub[2].istart = uiFaceStart*3;
	pMod->viewsub[2].icount = uiBFCnt*3;
	pMod->viewsub[2].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[2].nTri_Num = uiBFCnt;
	pMod->viewsub[2].pMaterial = NULL;
	uiFaceStart += uiBFCnt;


	UINT uiBFEndIndex = uiIBIndex;
	UINT uiBFBeginIndex = uiIBIndex - uiBFCnt*3;
	UINT uiBFEndVIndex = uiVBIndex;
	UINT uiBFBeginVIndex = uiVBIndex - (m_nDivision+1)*(m_nDivision+1) - (uiBSVCnt+uiBCVCnt)*4;

	//3 right
	m_uiFaceID = 3;
	//right cylinder
	UINT uiBeginRI = uiIBIndex;
	UINT uiBeginRVI = uiVBIndex;
	for(UINT uiFi = 0; uiFi < uiBHCFCnt*3;uiFi++)
	{
		pIB[uiIBIndex]=pBHCIB[uiFi] + uiVBIndex;
		uiIBIndex++;
	}
	for(uiFi = 0; uiFi < uiBHCVCnt; uiFi++)
	{
		xx = pBHCVB[uiFi].x;
		yy = pBHCVB[uiFi].y;
		zz = pBHCVB[uiFi].z;
		//right Cylinder
		pVB[uiVBIndex].x = zz;
		pVB[uiVBIndex].y = yy;
		pVB[uiVBIndex].z = xx;

		//pVB[uiVBIndex].normal = pVB[uiVBIndex].position;
		pVB[uiVBIndex].nx = pVB[uiVBIndex].x;
		pVB[uiVBIndex].ny = pVB[uiVBIndex].y;
		pVB[uiVBIndex].nz = pVB[uiVBIndex].z;
		pVB[uiVBIndex].ny = 0;

		float3Normalize(&pVB[uiVBIndex].vNorm,&pVB[uiVBIndex].vNorm);

		pVB[uiVBIndex].x *= 2 * m_fTransitionLen;
		pVB[uiVBIndex].y *= fH;
		pVB[uiVBIndex].z *= 2 * m_fTransitionLen;

		pVB[uiVBIndex].x += fL / 2.f;
		pVB[uiVBIndex].z += fW / 2.f;

		pVB[uiVBIndex].tu = pVB[uiVBIndex].z * fInvFWTex + 0.5f;
		pVB[uiVBIndex].tv = 0.5f - pVB[uiVBIndex].y * fInvFHTex;

		//pVB[uiVBIndex].FaceID = m_uiFaceID;

		uiVBIndex++;
	}
	//left Cylinder
	for(uiFi = 0; uiFi < uiBHCFCnt;uiFi++)
	{
		pIB[uiIBIndex]=pBHCIB[uiFi*3] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pBHCIB[uiFi*3+2] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pBHCIB[uiFi*3+1] + uiVBIndex;
		uiIBIndex++;
	}
	uiBeginVI = uiVBIndex - uiBHCVCnt;
	for(uiFi = 0; uiFi < uiBHCVCnt; uiFi++)
	{
		pVB[uiVBIndex].x =  pVB[uiBeginVI].x;
		pVB[uiVBIndex].y =  pVB[uiBeginVI].y;
		pVB[uiVBIndex].z = -pVB[uiBeginVI].z;

		pVB[uiVBIndex].nx = pVB[uiBeginVI].nx;
		pVB[uiVBIndex].ny = pVB[uiBeginVI].ny;
		pVB[uiVBIndex].nz = -pVB[uiBeginVI].nz;

		pVB[uiVBIndex].tu = pVB[uiVBIndex].z * fInvFWTex + 0.5f;
		pVB[uiVBIndex].tv = 0.5f - pVB[uiVBIndex].y * fInvFHTex;

		//m_pVB[uiVBIndex].FaceID = m_uiFaceID;

		uiVBIndex ++;
		uiBeginVI ++;
	}
	for(uiFi = 0; uiFi < uiPlaneFCnt; uiFi++)
	{
		pIB[uiIBIndex]=pPlaneIB[uiFi*3] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pPlaneIB[uiFi*3 + 2] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pPlaneIB[uiFi*3 +1] + uiVBIndex;
		uiIBIndex++;

	}
	for(uiFi = 0; uiFi < uiPlaneVCnt; uiFi++)
	{
		pVB[uiVBIndex].z = pPlaneVB[uiFi].x * fW;
		pVB[uiVBIndex].y = pPlaneVB[uiFi].y * fH;
		pVB[uiVBIndex].x = m_fLength/2.f;

		pVB[uiVBIndex].nx = 1.f;
		pVB[uiVBIndex].ny = 0.f;
		pVB[uiVBIndex].nz = 0.f;

		pVB[uiVBIndex].tu = pVB[uiVBIndex].z * fInvFWTex + 0.5f;
		pVB[uiVBIndex].tv = 0.5f - pVB[uiVBIndex].y * fInvFHTex;
		//m_pVB[uiVBIndex].FaceID = m_uiFaceID;
		uiVBIndex ++;
	}
	UINT uiEndRI = uiIBIndex;
	UINT uiEndRVI = uiVBIndex;
// 	m_pAttrTable[3].AttribId = 3;
// 	m_pAttrTable[3].FaceStart = uiFaceStart;
// 	m_pAttrTable[3].FaceCount = uiFrontFaceCnt;
// 	m_pAttrTable[3].VertexStart = 0;
// 	m_pAttrTable[3].VertexCount = m_nV;
// 	uiFaceStart += m_pAttrTable[3].FaceCount;

	pMod->viewsub[3].vstart= 0;
	pMod->viewsub[3].vcount =  pMod->vcount;
	pMod->viewsub[3].istart = uiFaceStart*3;
	pMod->viewsub[3].icount = uiFrontFaceCnt*3;
	pMod->viewsub[3].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[3].nTri_Num = uiFrontFaceCnt;
	pMod->viewsub[3].pMaterial = NULL;
	uiFaceStart += uiFrontFaceCnt;

	//4 top
	m_uiFaceID = 4;
	UINT uiVBOffSet = uiVBIndex - uiBFBeginVIndex;
	UINT uiBFBeginFCnt = uiBFBeginIndex/3;
	UINT uiBFEndFCnt = uiBFEndIndex/3;
	for(uiFi = uiBFBeginFCnt; uiFi < uiBFEndFCnt; uiFi++)
	{
		pIB[uiIBIndex]=pIB[uiFi*3] + uiVBOffSet;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiFi*3+2] + uiVBOffSet;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiFi*3+1] + uiVBOffSet;
		uiIBIndex++;
	}

	for(uiFi = uiBFBeginVIndex; uiFi < uiBFEndVIndex; uiFi++)
	{
		pVB[uiVBIndex].x = pVB[uiFi].x;
		pVB[uiVBIndex].y =-pVB[uiFi].y;
		pVB[uiVBIndex].z = pVB[uiFi].z;

		pVB[uiVBIndex].nx = pVB[uiFi].nx;
		pVB[uiVBIndex].ny =-pVB[uiFi].ny;
		pVB[uiVBIndex].nz = pVB[uiFi].nz;

		pVB[uiVBIndex].tu = pVB[uiVBIndex].x * fInvFLTexTop + 0.5f;
		pVB[uiVBIndex].tv = 0.5f - pVB[uiVBIndex].z * fInvFWTexTop;

		//m_pVB[uiVBIndex].FaceID = m_uiFaceID;
		uiVBIndex ++;
	}
// 	m_pAttrTable[4].AttribId = 4;
// 	m_pAttrTable[4].FaceStart = uiFaceStart;
// 	m_pAttrTable[4].FaceCount = uiBFCnt;
// 	m_pAttrTable[4].VertexStart = 0;
// 	m_pAttrTable[4].VertexCount = m_nV;
// 	uiFaceStart += m_pAttrTable[4].FaceCount;

	pMod->viewsub[4].vstart= 0;
	pMod->viewsub[4].vcount =  pMod->vcount;
	pMod->viewsub[4].istart = uiFaceStart*3;
	pMod->viewsub[4].icount = uiBFCnt*3;
	pMod->viewsub[4].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[4].nTri_Num = uiBFCnt;
	pMod->viewsub[4].pMaterial = NULL;
	uiFaceStart += uiBFCnt;

	//5 left
	m_uiFaceID = 5;
	UINT viOff = uiVBIndex-uiBeginRVI;
	for(uiFi = 0; uiFi < uiFrontFaceCnt; uiFi++)
	{
		UINT uiIndex = uiFi*3+uiBeginRI;
		pIB[uiIBIndex]=pIB[uiIndex] + viOff;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiIndex+2] + viOff;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiIndex+1] + viOff;
		uiIBIndex++;
	}
	for(uiFi = uiBeginRVI; uiFi < uiEndRVI; uiFi++)
	{
		pVB[uiVBIndex].x =-pVB[uiFi].x;
		pVB[uiVBIndex].y = pVB[uiFi].y;
		pVB[uiVBIndex].z = pVB[uiFi].z;

		pVB[uiVBIndex].nx = -pVB[uiFi].nx;
		pVB[uiVBIndex].ny =  pVB[uiFi].ny;
		pVB[uiVBIndex].nz =  pVB[uiFi].nz;

		pVB[uiVBIndex].tu = 0.5f - pVB[uiVBIndex].z * fInvFWTex;
		pVB[uiVBIndex].tv = 0.5f - pVB[uiVBIndex].y * fInvFHTex;
		//m_pVB[uiVBIndex].FaceID = m_uiFaceID;
		uiVBIndex ++;
	}
// 	m_pAttrTable[5].AttribId = 5;
// 	m_pAttrTable[5].FaceStart = uiFaceStart;
// 	m_pAttrTable[5].FaceCount = uiFrontFaceCnt;
// 	m_pAttrTable[5].VertexStart = 0;
// 	m_pAttrTable[5].VertexCount = m_nV;

	pMod->viewsub[5].vstart= 0;
	pMod->viewsub[5].vcount =  pMod->vcount;
	pMod->viewsub[5].istart = uiFaceStart*3;
	pMod->viewsub[5].icount = uiFrontFaceCnt*3;
	pMod->viewsub[5].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[5].nTri_Num = uiFrontFaceCnt;
	pMod->viewsub[5].pMaterial = NULL;

	if(pBSVB)
	{
		delete[] pBSVB;
		pBSVB = NULL;
	}
	if(pBSIB)
	{
		delete[] pBSIB;
		pBSIB = NULL;
	}
	if(pBCVB)
	{
		delete[] pBCVB;
		pBCVB = NULL;
	}
	if(pBCIB)
	{
		delete[] pBCIB;
		pBCIB = NULL;
	}
	if(pBHCVB)
	{
		delete[] pBHCVB;
		pBHCVB = NULL;
	}
	if(pBHCIB)
	{
		delete[] pBHCIB;
		pBHCIB = NULL;
	}
	if(pPlaneVB)
	{
		delete[] pPlaneVB;
		pPlaneVB = NULL;
	}
	if(pPlaneIB)
	{
		delete[] pPlaneIB;
		pPlaneIB = NULL;
	}

	return TRUE;
}

//该函数以后要修改的
void _TransFaceCoord(eFACETYPE FaceType,  SYVertex3D *pVB, const UINT uiVBIndex)
{
	float fTemp;
	if(FaceType == FACETYPE_BOTTOM)
	{
		fTemp = pVB[uiVBIndex].y;
		pVB[uiVBIndex].y = pVB[uiVBIndex].z;
		pVB[uiVBIndex].z = -fTemp;

		fTemp = pVB[uiVBIndex].ny;
		pVB[uiVBIndex].ny = pVB[uiVBIndex].nz;
		pVB[uiVBIndex].nz = -fTemp;
	}
	else if(FaceType == FACETYPE_RIGHT)
	{
		fTemp = pVB[uiVBIndex].x; 
		pVB[uiVBIndex].x = -pVB[uiVBIndex].z;
		pVB[uiVBIndex].z = fTemp;

		fTemp = pVB[uiVBIndex].nx; 
		pVB[uiVBIndex].nx = -pVB[uiVBIndex].nz;
		pVB[uiVBIndex].nz = fTemp;
	}
}

void _CalcTexCoord(eFACETYPE FaceType,float fInvfL,float fInvfH,float fInvfW, SYVertex3D *pVB, const UINT uiVBIndex)
{
	switch(FaceType)
	{
	default:
	case FACETYPE_TOP:
		{
			pVB[uiVBIndex].tu = pVB[uiVBIndex].x * fInvfL + 0.5f;
			pVB[uiVBIndex].tv = 0.5f - pVB[uiVBIndex].y * fInvfH;
		}
		break;
	case FACETYPE_BOTTOM:
		{
			pVB[uiVBIndex].tu = pVB[uiVBIndex].x * fInvfL + 0.5f;
			pVB[uiVBIndex].tv = 0.5f - pVB[uiVBIndex].z * fInvfW;
		}
		break;
	case FACETYPE_RIGHT:
		{
			pVB[uiVBIndex].tu = pVB[uiVBIndex].z * fInvfW + 0.5f;
			pVB[uiVBIndex].tv = 0.5f - pVB[uiVBIndex].y * fInvfH;
		}
		break;
	}
}

void BuildSSPhereOneFace(float fL,float fH,float fW,eFACETYPE FaceType, const float fTransitionLen, SYVertex3D *pVB, UINT &uiVBIndex,  DWORD *pIB, UINT  &uiIBIndex,
				DWORD *pBCIB, const UINT uiBCFCnt, 
				SYVertex3D *pBCVB, const UINT uiBCVCnt, 
				const UINT uiBSFCnt,  DWORD *pBSIB, 
				SYVertex3D *pBSVB, const UINT uiBSVCnt,
				DWORD *pPlaneIB, const UINT uiPlaneFCnt,
				SYVertex3D *pPlaneVB, const UINT uiPlaneVCnt,
				float fLength,float fHeight,float fThick)
{
	float xx,yy,zz;
	float fInvFLTex = (float)(1.f/(sqrtf(2.f)* fTransitionLen + fL));
	float fInvFHTex = (float)(1.f/(sqrtf(2.f)* fTransitionLen + fH));
	float  fInvFWTex = (float)(1.f/(sqrtf(2.f)* fTransitionLen + fW));
	//top cylinder
	UINT uiFi;
	for(uiFi = 0; uiFi < uiBCFCnt;uiFi++)
	{
		pIB[uiIBIndex]=pBCIB[uiFi*3] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pBCIB[uiFi*3+2] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pBCIB[uiFi*3+1] + uiVBIndex;
		uiIBIndex++;
	}
	for(uiFi = 0; uiFi < uiBCVCnt; uiFi++)
	{
		xx = pBCVB[uiFi].x;
		yy = pBCVB[uiFi].y;
		zz = pBCVB[uiFi].z;
		pVB[uiVBIndex].x = yy;
		pVB[uiVBIndex].y = xx;
		pVB[uiVBIndex].z = -zz;
		pVB[uiVBIndex].nx = pVB[uiVBIndex].x;
		pVB[uiVBIndex].ny = pVB[uiVBIndex].y;
		pVB[uiVBIndex].nz = pVB[uiVBIndex].z;
		pVB[uiVBIndex].nx = 0;
		float3Normalize(&pVB[uiVBIndex].vNorm,&pVB[uiVBIndex].vNorm);


		pVB[uiVBIndex].x *= fL;
		pVB[uiVBIndex].y *= 2 * fTransitionLen;
		pVB[uiVBIndex].z *= 2 * fTransitionLen;

		pVB[uiVBIndex].y += fH / 2.f;
		pVB[uiVBIndex].z -= fW / 2.f;

		_TransFaceCoord(FaceType, pVB, uiVBIndex);
		_CalcTexCoord(FaceType,fInvFLTex,fInvFHTex,fInvFWTex, pVB, uiVBIndex);
		//pVB[uiVBIndex].FaceID = uiFaceID;
		uiVBIndex ++;
	}
	//bottom Cylinder
	for(uiFi = 0; uiFi < uiBCFCnt*3;uiFi++)
	{
		pIB[uiIBIndex]=pBCIB[uiFi] + uiVBIndex;
		uiIBIndex++;
	}
	UINT uiBeginVI = uiVBIndex - uiBCVCnt;
	for(uiFi = 0; uiFi < uiBCVCnt; uiFi++)
	{
		if(FaceType == FACETYPE_TOP || FaceType == FACETYPE_RIGHT)
		{
			pVB[uiVBIndex].x = pVB[uiBeginVI].x;
			pVB[uiVBIndex].y =-pVB[uiBeginVI].y;
			pVB[uiVBIndex].z = pVB[uiBeginVI].z;
			pVB[uiVBIndex].nx = pVB[uiBeginVI].nx;
			pVB[uiVBIndex].ny =-pVB[uiBeginVI].ny;
			pVB[uiVBIndex].nz = pVB[uiBeginVI].nz;
		}
		else if(FaceType == FACETYPE_BOTTOM)
		{
			pVB[uiVBIndex].x = pVB[uiBeginVI].x;
			pVB[uiVBIndex].y = pVB[uiBeginVI].y;
			pVB[uiVBIndex].z =-pVB[uiBeginVI].z;
			pVB[uiVBIndex].nx = pVB[uiBeginVI].nx;
			pVB[uiVBIndex].ny = pVB[uiBeginVI].ny;
			pVB[uiVBIndex].nz =-pVB[uiBeginVI].nz;
		}
		_CalcTexCoord(FaceType,fInvFLTex,fInvFHTex,fInvFWTex, pVB, uiVBIndex);

		//pVB[uiVBIndex].FaceID = uiFaceID;
		uiVBIndex ++;
		uiBeginVI ++;
	}

	//right cylinder
	for(uiFi = 0; uiFi < uiBCFCnt*3;uiFi++)
	{
		pIB[uiIBIndex]=pBCIB[uiFi] + uiVBIndex;
		uiIBIndex++;
	}
	for(uiFi = 0; uiFi < uiBCVCnt; uiFi++)
	{
		xx = pBCVB[uiFi].x;
		yy = pBCVB[uiFi].y;
		zz = pBCVB[uiFi].z;
		//right Cylinder
		pVB[uiVBIndex].x = xx;
		pVB[uiVBIndex].y = yy;
		pVB[uiVBIndex].z = -zz;

		pVB[uiVBIndex].nx = pVB[uiVBIndex].x;
		pVB[uiVBIndex].ny = pVB[uiVBIndex].y;
		pVB[uiVBIndex].nz = pVB[uiVBIndex].z;
		pVB[uiVBIndex].ny = 0;

		float3Normalize(&pVB[uiVBIndex].vNorm,&pVB[uiVBIndex].vNorm);
		pVB[uiVBIndex].x *= 2 * fTransitionLen;
		pVB[uiVBIndex].y *= fH;
		pVB[uiVBIndex].z *= 2 * fTransitionLen;

		pVB[uiVBIndex].x += fL / 2.f;
		pVB[uiVBIndex].z -= fW / 2.f;

		_TransFaceCoord(FaceType, pVB, uiVBIndex);
		_CalcTexCoord(FaceType,fInvFLTex,fInvFHTex,fInvFWTex, pVB, uiVBIndex);

		//pVB[uiVBIndex].FaceID = uiFaceID;
		uiVBIndex++;
	}
	//left Cylinder
	for(uiFi = 0; uiFi < uiBCFCnt;uiFi++)
	{
		pIB[uiIBIndex]=pBCIB[uiFi*3] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pBCIB[uiFi*3+2] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pBCIB[uiFi*3+1] + uiVBIndex;
		uiIBIndex++;
	}
	uiBeginVI = uiVBIndex - uiBCVCnt;
	for(uiFi = 0; uiFi < uiBCVCnt; uiFi++)
	{
		if(FaceType == FACETYPE_TOP || FaceType == FACETYPE_BOTTOM)
		{
			pVB[uiVBIndex].x =-pVB[uiBeginVI].x;
			pVB[uiVBIndex].y = pVB[uiBeginVI].y;
			pVB[uiVBIndex].z = pVB[uiBeginVI].z;

			pVB[uiVBIndex].nx =-pVB[uiBeginVI].nx;
			pVB[uiVBIndex].ny = pVB[uiBeginVI].ny;
			pVB[uiVBIndex].nz = pVB[uiBeginVI].nz;
		}
		else if(FaceType == FACETYPE_RIGHT)
		{
			pVB[uiVBIndex].x = pVB[uiBeginVI].x;
			pVB[uiVBIndex].y = pVB[uiBeginVI].y;
			pVB[uiVBIndex].z = -pVB[uiBeginVI].z;

			pVB[uiVBIndex].nx = pVB[uiBeginVI].nx;
			pVB[uiVBIndex].ny = pVB[uiBeginVI].ny;
			pVB[uiVBIndex].nz =-pVB[uiBeginVI].nz;
		}
		_CalcTexCoord(FaceType,fInvFLTex,fInvFHTex,fInvFWTex, pVB, uiVBIndex);

		//pVB[uiVBIndex].FaceID = uiFaceID;
		uiVBIndex ++;
		uiBeginVI ++;
	}

	//righttop sphere
	for(uiFi = 0; uiFi < uiBSFCnt*3;uiFi++)
	{
		pIB[uiIBIndex]=pBSIB[uiFi] + uiVBIndex;
		uiIBIndex++;
	}
	for(uiFi = 0; uiFi < uiBSVCnt; uiFi ++)
	{
		xx = pBSVB[uiFi].x;
		yy = pBSVB[uiFi].y;
		zz = pBSVB[uiFi].z;

		pVB[uiVBIndex].x = zz;
		pVB[uiVBIndex].y = xx;
		pVB[uiVBIndex].z = -yy;

		float3Normalize(&pVB[uiVBIndex].vNorm,&pVB[uiVBIndex].vDyn);

		pVB[uiVBIndex].x *= 2 * fTransitionLen;
		pVB[uiVBIndex].x += fL/2.f;
		pVB[uiVBIndex].y *= 2 * fTransitionLen;
		pVB[uiVBIndex].y += fH/2.f;
		pVB[uiVBIndex].z *= 2 * fTransitionLen;
		pVB[uiVBIndex].z -= fW/2.f;

		_TransFaceCoord(FaceType, pVB, uiVBIndex);
		_CalcTexCoord(FaceType,fInvFLTex,fInvFHTex,fInvFWTex, pVB, uiVBIndex);
		//pVB[uiVBIndex].FaceID = uiFaceID;

		uiVBIndex++;
	}
	//lefttop sphere
	for(uiFi = 0; uiFi < uiBSFCnt;uiFi++)
	{
		pIB[uiIBIndex]=pBSIB[uiFi*3] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pBSIB[uiFi*3+2] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pBSIB[uiFi*3+1] + uiVBIndex;
		uiIBIndex++;
	}
	uiBeginVI = uiVBIndex - uiBSVCnt;
	for(uiFi = 0; uiFi < uiBSVCnt; uiFi ++)
	{
		if(FaceType == FACETYPE_TOP || FaceType == FACETYPE_BOTTOM)
		{
			pVB[uiVBIndex].x =-pVB[uiBeginVI].x;
			pVB[uiVBIndex].y = pVB[uiBeginVI].y;
			pVB[uiVBIndex].z = pVB[uiBeginVI].z;

			pVB[uiVBIndex].nx =-pVB[uiBeginVI].nx;
			pVB[uiVBIndex].ny = pVB[uiBeginVI].ny;
			pVB[uiVBIndex].nz = pVB[uiBeginVI].nz;
		}
		else if(FaceType == FACETYPE_RIGHT)
		{
			pVB[uiVBIndex].x = pVB[uiBeginVI].x;
			pVB[uiVBIndex].y = pVB[uiBeginVI].y;
			pVB[uiVBIndex].z =-pVB[uiBeginVI].z;

			pVB[uiVBIndex].nx = pVB[uiBeginVI].nx;
			pVB[uiVBIndex].ny = pVB[uiBeginVI].ny;
			pVB[uiVBIndex].nz =-pVB[uiBeginVI].nz;
		}	
		_CalcTexCoord(FaceType,fInvFLTex,fInvFHTex,fInvFWTex, pVB, uiVBIndex);
		//pVB[uiVBIndex].FaceID = uiFaceID;

		uiVBIndex ++;
		uiBeginVI ++;
	}

	//rightbottom sphere
	for(uiFi = 0; uiFi < uiBSFCnt;uiFi++)
	{
		pIB[uiIBIndex]=pBSIB[uiFi*3] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pBSIB[uiFi*3+2] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pBSIB[uiFi*3+1] + uiVBIndex;
		uiIBIndex++;
	}
	uiBeginVI = uiBeginVI - uiBSVCnt;
	for(uiFi = 0; uiFi < uiBSVCnt; uiFi ++)
	{
		if(FaceType == FACETYPE_TOP || FaceType == FACETYPE_RIGHT)
		{
			pVB[uiVBIndex].x = pVB[uiBeginVI].x;
			pVB[uiVBIndex].y =-pVB[uiBeginVI].y;
			pVB[uiVBIndex].z = pVB[uiBeginVI].z;

			pVB[uiVBIndex].nx = pVB[uiBeginVI].nx;
			pVB[uiVBIndex].ny =-pVB[uiBeginVI].ny;
			pVB[uiVBIndex].nz = pVB[uiBeginVI].nz;
		}
		else if(FaceType == FACETYPE_BOTTOM)
		{
			pVB[uiVBIndex].x = pVB[uiBeginVI].x;
			pVB[uiVBIndex].y = pVB[uiBeginVI].y;
			pVB[uiVBIndex].z = -pVB[uiBeginVI].z;

			pVB[uiVBIndex].nx = pVB[uiBeginVI].nx;
			pVB[uiVBIndex].ny = pVB[uiBeginVI].ny;
			pVB[uiVBIndex].nz =-pVB[uiBeginVI].nz;
		}
		_CalcTexCoord(FaceType,fInvFLTex,fInvFHTex,fInvFWTex, pVB, uiVBIndex);
		//pVB[uiVBIndex].FaceID = uiFaceID;

		uiVBIndex ++;
		uiBeginVI ++;
	}

	//leftbottom sphere
	for(uiFi = 0; uiFi < uiBSFCnt*3;uiFi++)
	{
		pIB[uiIBIndex]=pBSIB[uiFi] + uiVBIndex;
		uiIBIndex++;
	}
	uiBeginVI = uiBeginVI - uiBSVCnt;
	for(uiFi = 0; uiFi < uiBSVCnt; uiFi ++)
	{
		if(FaceType == FACETYPE_TOP)
		{
			pVB[uiVBIndex].x = -pVB[uiBeginVI].x;
			pVB[uiVBIndex].y = -pVB[uiBeginVI].y;
			pVB[uiVBIndex].z =  pVB[uiBeginVI].z;

			pVB[uiVBIndex].nx = -pVB[uiBeginVI].nx;
			pVB[uiVBIndex].ny = -pVB[uiBeginVI].ny;
			pVB[uiVBIndex].nz =  pVB[uiBeginVI].nz;
		}
		else if(FaceType == FACETYPE_BOTTOM)
		{
			pVB[uiVBIndex].x = -pVB[uiBeginVI].x;
			pVB[uiVBIndex].y =  pVB[uiBeginVI].y;
			pVB[uiVBIndex].z = -pVB[uiBeginVI].z;

			pVB[uiVBIndex].nx = -pVB[uiBeginVI].nx;
			pVB[uiVBIndex].ny =  pVB[uiBeginVI].ny;
			pVB[uiVBIndex].nz = -pVB[uiBeginVI].nz;
		}
		else if(FaceType == FACETYPE_RIGHT)
		{
			pVB[uiVBIndex].x =  pVB[uiBeginVI].x;
			pVB[uiVBIndex].y = -pVB[uiBeginVI].y;
			pVB[uiVBIndex].z = -pVB[uiBeginVI].z;

			pVB[uiVBIndex].nx =  pVB[uiBeginVI].nx;
			pVB[uiVBIndex].ny = -pVB[uiBeginVI].ny;
			pVB[uiVBIndex].nz = -pVB[uiBeginVI].nz;
		}

		_CalcTexCoord(FaceType,fInvFLTex,fInvFHTex,fInvFWTex, pVB, uiVBIndex);

		//pVB[uiVBIndex].FaceID = uiFaceID;

		uiVBIndex ++;
		uiBeginVI ++;
	}

	for(uiFi = 0; uiFi < uiPlaneFCnt; uiFi++)
	{
		pIB[uiIBIndex]=pPlaneIB[uiFi*3] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pPlaneIB[uiFi*3 + 2] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pPlaneIB[uiFi*3 +1] + uiVBIndex;
		uiIBIndex++;

	}
	for(uiFi = 0; uiFi < uiPlaneVCnt; uiFi++)
	{
		pVB[uiVBIndex].x = pPlaneVB[uiFi].x * fL;
		pVB[uiVBIndex].y = pPlaneVB[uiFi].y * fH;
		pVB[uiVBIndex].z = pPlaneVB[uiFi].z;

		pVB[uiVBIndex].nx = 0.f;
		pVB[uiVBIndex].ny = 0.f;
		pVB[uiVBIndex].nz = -1.f;
		_TransFaceCoord(FaceType, pVB, uiVBIndex);
	

		if(FaceType == FACETYPE_TOP)
			pVB[uiVBIndex].z = -fThick/2.f;
		else if(FaceType == FACETYPE_BOTTOM)
			pVB[uiVBIndex].y = -fHeight/2.f;
		else if(FaceType == FACETYPE_RIGHT)
			pVB[uiVBIndex].x = fLength/2.f;
		_CalcTexCoord(FaceType,fInvFLTex,fInvFHTex,fInvFWTex, pVB, uiVBIndex);
		//pVB[uiVBIndex].FaceID = uiFaceID;

		uiVBIndex ++;
	}
}


BOOL CreateSplitSphereBevelBox(SYModel* pMod,float fLength,float fHeight,float fThick,int nDivision,float uPartType,float fTransitionLen,float fTransitionHeight, UINT nTSPrecission)
{
	//Create elements,these elements can be shared between box models!
	
	UINT uiBSVCnt = 0,uiBSFCnt = 0,uiBCVCnt = 0,uiBCFCnt = 0,uiBHCVCnt = 0,uiBHCFCnt = 0;
	SYVertex3D *pBSVB = NULL,*pBCVB = NULL,*pBHCVB = NULL;
	DWORD *pBSIB = NULL,*pBCIB = NULL,*pBHCIB = NULL;

	if(fTransitionLen > ABS_FLOAT_0)
	{
		//1.Create Bevel sphere
		CreateBevelSphere(nTSPrecission,
			&pBSVB,&pBSIB,uiBSFCnt,uiBSVCnt,TRUE);
// 
		//2. Create bevel cylinder
		CreateBevelCylinder(nTSPrecission,nDivision,
			&pBCVB,&pBCIB,uiBCFCnt,uiBCVCnt,TRUE);

	}

	UINT uiFVNum = 0;//((nDivision+1)*(nDivision+1) + (uiBSVCnt+uiBCVCnt)*4);

	//3. Create plane
	UINT uiPlaneVCnt = (nDivision+1)*(nDivision+1);
	UINT uiPlaneFCnt = nDivision*nDivision*2;
	SYVertex3D *pPlaneVB = new SYVertex3D[uiPlaneVCnt];
	DWORD * pPlaneIB = new DWORD[uiPlaneFCnt*3];
	float fStep = 1.f / (nDivision);
	float px,py;
	for (int nR=0;nR<nDivision+1;nR++)
	{
		int nRIndex = nR * (nDivision+1);
		py = fStep * nR - .5f;
		for (int nC=0;nC<nDivision+1;nC++)
		{
			px = fStep * nC - .5f;

			float3Set(&pPlaneVB[nRIndex+nC].vDyn, px,py,0.f);
			float3Set(&pPlaneVB[nRIndex+nC].vNorm, 0.f,0.f,1.f);

			pPlaneVB[nRIndex+nC].tu = 0.f;
			pPlaneVB[nRIndex+nC].tv = 0.f;

		}
	}
	int nIdx = 0,a,b,c,d;
	for (int nY=0;nY<nDivision;nY++)
	{
		int nCBase0 = nY * (nDivision+1);
		int nCBase1 = nCBase0 + (nDivision+1);
		for (int nX=0;nX<nDivision;nX++)
		{
			a = nCBase0 + nX; b = a + 1;
			c = nCBase1 + nX; d = c + 1;
			pPlaneIB[nIdx++] = a;
			pPlaneIB[nIdx++] = b;
			pPlaneIB[nIdx++] = c;
			pPlaneIB[nIdx++] = c;
			pPlaneIB[nIdx++] = b;
			pPlaneIB[nIdx++] = d;
		}
	}

	//create box
	/*nV*/pMod->vcount = (nDivision+1)*(nDivision+1)*6 + (uiBSVCnt+uiBCVCnt)*24 + uiFVNum*8;
	/*nF*/pMod->icount = (nDivision*nDivision*12 + (uiBSFCnt+uiBCFCnt)*24 +uiFVNum*8) * 3;

	pMod->v = new SYVertex3D[pMod->vcount]; 
	pMod->indecs = new DWORD[pMod->icount];
	pMod->nviewsub = 6;
	pMod->viewsub = new SYSubModel[6];

// 	pVB = new MODELVERTEX[nV];
// 	pIB = new DWORD[nF*3];
// 	pAttrTable = new D3DX10_ATTRIBUTE_RANGE[6];
	SYVertex3D *pVB = pMod->v;
	DWORD *	   pIB = pMod->indecs;

	float fL = fLength - 2*fTransitionLen;
	float fH = fHeight - 2*fTransitionLen;
	float fW = fThick - 2*fTransitionLen;

	UINT uiVBIndex = 0;
	UINT uiIBIndex = 0;

	float fInvFLTex = (float)(1.f/(sqrtf(2.f)*fTransitionLen + fL));
	float fInvFHTex = (float)(1.f/(sqrtf(2.f)*fTransitionLen + fH));
	float  fInvFWTex = (float)(1.f/(sqrtf(2.f)*fTransitionLen + fW));

	//0 front
	UINT uiFaceID = 0;
	BuildSSPhereOneFace(fL,fH,fW,FACETYPE_TOP, fTransitionLen, pVB, uiVBIndex, pIB, uiIBIndex,
		pBCIB, uiBCFCnt, 
		pBCVB, uiBCVCnt, 
		uiBSFCnt,  pBSIB, 
		pBSVB, uiBSVCnt,
		pPlaneIB, uiPlaneFCnt,
		pPlaneVB, uiPlaneVCnt,
		fLength,fHeight,fThick);


	UINT uiFVCnt = uiVBIndex;
	UINT uiIndexCnt = uiIBIndex;
	UINT uiFaceCnt = nDivision*nDivision*2 + (uiBSFCnt+uiBCFCnt)*4;
	assert(uiFaceCnt * 3 == uiIBIndex);

	
	for(int iii = 0; iii < (int)uiFVNum; iii++)
	{
		CreateNormalPrimitive(&pVB[iii],&pVB,&pIB,uiVBIndex,uiIBIndex);
	}


	UINT uiFaceOff = 8*uiFVNum;
	UINT uiFaceStart = 0;

// 	m_pAttrTable[0].AttribId = 0;
// 	m_pAttrTable[0].FaceStart = uiFaceStart;
// 	m_pAttrTable[0].FaceCount = uiFaceCnt + uiFaceOff;
// 	m_pAttrTable[0].VertexStart = 0;
// 	m_pAttrTable[0].VertexCount = m_nV;
// 	uiFaceStart += m_pAttrTable[0].FaceCount;

	pMod->viewsub[0].vstart= 0;
	pMod->viewsub[0].vcount =  pMod->vcount;
	pMod->viewsub[0].istart = uiFaceStart*3;
	pMod->viewsub[0].icount = (uiFaceCnt + uiFaceOff)*3;
	pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[0].nTri_Num  = uiFaceCnt + uiFaceOff;
	pMod->viewsub[0].pMaterial = NULL;
	uiFaceStart += uiFaceCnt + uiFaceOff;


	//1 back
	uiFaceID = 1;
	UINT uiFi = 0;
	for(uiFi = 0; uiFi < uiFaceCnt; uiFi++)
	{
		pIB[uiIBIndex]= pIB[uiFi*3] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiFi*3 + 2] + uiVBIndex;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiFi*3 + 1] + uiVBIndex;
		uiIBIndex++;
	}
	for(uiFi = 0; uiFi < uiFVCnt; uiFi++)
	{
		pVB[uiVBIndex].x = pVB[uiFi].x;
		pVB[uiVBIndex].y = pVB[uiFi].y;
		pVB[uiVBIndex].z = -pVB[uiFi].z;

		pVB[uiVBIndex].nx = pVB[uiFi].nx;
		pVB[uiVBIndex].ny = pVB[uiFi].ny;
		pVB[uiVBIndex].nz = -pVB[uiFi].nz;

		pVB[uiVBIndex].tu = 0.5f - pVB[uiVBIndex].x * fInvFLTex;
		pVB[uiVBIndex].tv = 0.5f - pVB[uiVBIndex].y * fInvFHTex;

		//pVB[uiVBIndex].FaceID = uiFaceID;
		uiVBIndex ++;
	}
// 	m_pAttrTable[1].AttribId = 1;
// 	m_pAttrTable[1].FaceStart = uiFaceStart;
// 	m_pAttrTable[1].FaceCount = uiFaceCnt;
// 	m_pAttrTable[1].VertexStart = 0;
// 	m_pAttrTable[1].VertexCount = m_nV;
// 	uiFaceStart += m_pAttrTable[1].FaceCount;

	pMod->viewsub[1].vstart= 0;
	pMod->viewsub[1].vcount =  pMod->vcount;
	pMod->viewsub[1].istart = uiFaceStart*3;
	pMod->viewsub[1].icount = uiFaceCnt*3;
	pMod->viewsub[1].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[1].nTri_Num  = uiFaceCnt;
	pMod->viewsub[1].pMaterial = NULL;
	uiFaceStart += uiFaceCnt;

	//2 bottom
	uiFaceID = 2;
	UINT uiBottomBI = uiIBIndex;
	UINT uiBottomBVI = uiVBIndex;
	BuildSSPhereOneFace(fL,fW,fH,FACETYPE_BOTTOM, fTransitionLen, pVB, uiVBIndex, pIB, uiIBIndex,
		pBCIB, uiBCFCnt, 
		pBCVB, uiBCVCnt, 
		uiBSFCnt,  pBSIB, 
		pBSVB, uiBSVCnt,
		pPlaneIB, uiPlaneFCnt,
		pPlaneVB, uiPlaneVCnt,
		fLength,fHeight,fThick);
	UINT uiBottomEI = uiIBIndex;
	UINT uiBottomEVI = uiVBIndex;

// 	m_pAttrTable[2].AttribId = 2;
// 	m_pAttrTable[2].FaceStart = uiFaceStart;
// 	m_pAttrTable[2].FaceCount = uiFaceCnt;
// 	m_pAttrTable[2].VertexStart = 0;
// 	m_pAttrTable[2].VertexCount = m_nV;
// 	uiFaceStart += m_pAttrTable[2].FaceCount;

	pMod->viewsub[2].vstart= 0;
	pMod->viewsub[2].vcount =  pMod->vcount;
	pMod->viewsub[2].istart = uiFaceStart*3;
	pMod->viewsub[2].icount = uiFaceCnt*3;
	pMod->viewsub[2].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[2].nTri_Num  = uiFaceCnt;
	pMod->viewsub[2].pMaterial = NULL;
	uiFaceStart += uiFaceCnt;

	//3 right
	uiFaceID = 3;
	UINT uiRightBI = uiIBIndex;
	UINT uiRightBVI = uiVBIndex;
	BuildSSPhereOneFace(fW,fH,fL,FACETYPE_RIGHT, fTransitionLen, pVB, uiVBIndex, pIB, uiIBIndex,
		pBCIB, uiBCFCnt, 
		pBCVB, uiBCVCnt, 
		uiBSFCnt,  pBSIB, 
		pBSVB, uiBSVCnt,
		pPlaneIB, uiPlaneFCnt,
		pPlaneVB, uiPlaneVCnt,
		fLength,fHeight,fThick);
	UINT uiRightEI = uiIBIndex;
	UINT uiRightEVI = uiVBIndex;

// 	m_pAttrTable[3].AttribId = 3;
// 	m_pAttrTable[3].FaceStart = uiFaceStart;
// 	m_pAttrTable[3].FaceCount = uiFaceCnt;
// 	m_pAttrTable[3].VertexStart = 0;
// 	m_pAttrTable[3].VertexCount = m_nV;
// 	uiFaceStart += m_pAttrTable[3].FaceCount;

	pMod->viewsub[3].vstart= 0;
	pMod->viewsub[3].vcount =  pMod->vcount;
	pMod->viewsub[3].istart = uiFaceStart*3;
	pMod->viewsub[3].icount = uiFaceCnt*3;
	pMod->viewsub[3].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[3].nTri_Num  = uiFaceCnt;
	pMod->viewsub[3].pMaterial = NULL;
	uiFaceStart += uiFaceCnt;

	//4 top
	uiFaceID = 4;
	UINT uiVOff = uiVBIndex - uiBottomBVI;
	for(uiFi = 0; uiFi < uiFaceCnt; uiFi++)
	{
		UINT uiIndex = uiFi*3+uiBottomBI;
		pIB[uiIBIndex]=pIB[uiIndex] + uiVOff;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiIndex+2] + uiVOff;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiIndex+1] + uiVOff;
		uiIBIndex++;
	}
	for(uiFi = uiBottomBVI; uiFi < uiBottomEVI; uiFi++)
	{
		pVB[uiFi].tv = 1.0f - pVB[uiFi].tv;
		pVB[uiVBIndex].x = pVB[uiFi].x;
		pVB[uiVBIndex].y =-pVB[uiFi].y;
		pVB[uiVBIndex].z = pVB[uiFi].z;

		pVB[uiVBIndex].nx = pVB[uiFi].nx;
		pVB[uiVBIndex].ny =-pVB[uiFi].ny;
		pVB[uiVBIndex].nz = pVB[uiFi].nz;

		pVB[uiVBIndex].tu = pVB[uiVBIndex].x * fInvFLTex + 0.5f;
		pVB[uiVBIndex].tv = 0.5f - pVB[uiVBIndex].z * fInvFWTex;

		//pVB[uiVBIndex].FaceID = uiFaceID;
		uiVBIndex ++;
	}
// 	m_pAttrTable[4].AttribId = 4;
// 	m_pAttrTable[4].FaceStart = uiFaceStart;
// 	m_pAttrTable[4].FaceCount = uiFaceCnt;
// 	m_pAttrTable[4].VertexStart = 0;
// 	m_pAttrTable[4].VertexCount = m_nV;
// 	uiFaceStart += m_pAttrTable[4].FaceCount;

	pMod->viewsub[4].vstart= 0;
	pMod->viewsub[4].vcount =  pMod->vcount;
	pMod->viewsub[4].istart = uiFaceStart*3;
	pMod->viewsub[4].icount = uiFaceCnt*3;
	pMod->viewsub[4].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[4].nTri_Num  = uiFaceCnt;
	pMod->viewsub[4].pMaterial = NULL;
	uiFaceStart += uiFaceCnt;

	//5 left
	uiFaceID = 5;
	uiVOff = uiVBIndex - uiRightBVI;
	for(uiFi = 0; uiFi < uiFaceCnt; uiFi++)
	{
		UINT uiIndex = uiFi*3+uiRightBI;
		pIB[uiIBIndex]=pIB[uiIndex] + uiVOff;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiIndex+2] + uiVOff;
		uiIBIndex++;
		pIB[uiIBIndex]=pIB[uiIndex+1] + uiVOff;
		uiIBIndex++;
	}
	for(uiFi = uiRightBVI; uiFi < uiRightEVI; uiFi++)
	{
		pVB[uiVBIndex].x = -pVB[uiFi].x;
		pVB[uiVBIndex].y = pVB[uiFi].y;
		pVB[uiVBIndex].z = pVB[uiFi].z;

		pVB[uiVBIndex].nx =-pVB[uiFi].nx;
		pVB[uiVBIndex].ny = pVB[uiFi].ny;
		pVB[uiVBIndex].nz = pVB[uiFi].nz;

		pVB[uiVBIndex].tu = 0.5f - pVB[uiVBIndex].z * fInvFWTex;
		pVB[uiVBIndex].tv = 0.5f - pVB[uiVBIndex].y * fInvFHTex;

		//pVB[uiVBIndex].FaceID = uiFaceID;

		uiVBIndex ++;
	}
// 	m_pAttrTable[5].AttribId = 5;
// 	m_pAttrTable[5].FaceStart = uiFaceStart;
// 	m_pAttrTable[5].FaceCount = uiFaceCnt;
// 	m_pAttrTable[5].VertexStart = 0;
// 	m_pAttrTable[5].VertexCount = m_nV;
// 	uiFaceStart += m_pAttrTable[5].FaceCount;	

	pMod->viewsub[5].vstart= 0;
	pMod->viewsub[5].vcount =  pMod->vcount;
	pMod->viewsub[5].istart = uiFaceStart*3;
	pMod->viewsub[5].icount = uiFaceCnt*3;
	pMod->viewsub[5].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[5].nTri_Num  = uiFaceCnt;
	pMod->viewsub[5].pMaterial = NULL;
	uiFaceStart += uiFaceCnt;

	//Temp code test,but not be efficient and will cause memory fragment
	if(pBSVB)
	{
		delete[] pBSVB;
		pBSVB = NULL;
	}
	if(pBSIB)
	{
		delete[] pBSIB;
		pBSIB = NULL;
	}
	if(pBCVB)
	{
		delete[] pBCVB;
		pBCVB = NULL;
	}
	if(pBCIB)
	{
		delete[] pBCIB;
		pBCIB = NULL;
	}
	if(pPlaneVB)
	{
		delete[] pPlaneVB;
		pPlaneVB = NULL;
	}
	if(pPlaneIB)
	{
		delete[] pPlaneIB;
		pPlaneIB = NULL;
	}
	return TRUE;
}


void _CreateCornerCurves(
	CurvePtArray& Curve,CurvePtArray& Curve2,float3& vPre,
	float3& vC,float3& vNext,float fHAngle, 
	const float fTransitionLen, 
	const float fHeight, 
	float3 *BevelCenter, 
	int nTransitionPresition)
{
	float fAngle = 0.f;
	float fAngleStep =(float)(fHAngle*2.f/ nTransitionPresition);

	float fLen = fTransitionLen * tanf(fHAngle);
	float3 v2 = {vC.x - vPre.x, vC.y - vPre.y, vC.z - vPre.z};
	float3Normalize(&v2,&v2);
	float3 v1 = {vC.x - v2.x * fLen, vC.y - v2.y * fLen, vC.z - v2.z * fLen};
	
	float3 vNormal ={ v2.x, v2.y, v2.z};
	vNormal.z = vNormal.x;
	vNormal.x = vNormal.y;
	vNormal.y = -vNormal.z;
	vNormal.z = 0.f;
	float3Normalize(&vNormal,&vNormal);
 
	float t = fTransitionLen /fHeight;
	float3 v4 = {vNext.x -vC.x, vNext.y-vC.y,vNext.z-vC.z};


	BOOL bLeft = fabs(v4.y) < ABS_FLOAT_0;

	float3 vNormal2 = -vNormal;
	
	float3 vCenter = v1 + vNormal * fTransitionLen;
	float3 v3 = v1 - vCenter;
	float3Normalize(&v3,&v3);

	if(bLeft)
	{
		v4 = vC*(1.f-t)+t*vPre;
	}
	else
	{
		vNormal2 = vNext-vC;
		vNormal2.z = vNormal2.x;
		vNormal2.x = -vNormal2.y;
		vNormal2.y = vNormal2.z;
		vNormal2.z = 0.f;
		float3Normalize(&vNormal2,&vNormal2);

		v4 = vC*(1.f-t)+t*vNext;
	}
	float3 v4C = v4 - vCenter,v5;
	float3Normalize(&v4C,&v4C);
	v5 = v4C * fTransitionLen + vCenter;
	BevelCenter[2] += vCenter; 
	matrix4f matRot;

	float3 vTemp = v3;
	BOOL bAddExtra = FALSE;
	SYVertex3D vPt2;vPt2.vDyn = v4;vPt2.vNorm = vNormal2;
	SYVertex3D vPt3;vPt3.vDyn = v5;vPt3.vNorm = v4C;
	for(int i = 0; i <=  nTransitionPresition; i++)
	{
		vTemp = v3;  
		matrix4f_fromeuler_lyxz(&matRot, 0, 0, -fAngle);
		float3NormalMulMat(&vTemp,&vTemp,&matRot);
		//D3DXMatrixRotationZ(&matRot,-fAngle);						
		//D3DXVec3TransformNormal(&vTemp,&vTemp,&matRot);       
		SYVertex3D vPt;
		vPt.vNorm = vTemp;
		vPt.vDyn = vCenter + fTransitionLen *vTemp;

		if(bLeft)
		{
			BOOL bExtra = FALSE;
			if( i == 0)
			{
				if(fabs(vPt.y)-fabs(v4.y)< -ABS_FLOAT_0) 
				{
					bExtra = TRUE;
				}
			}
			if(bExtra)
			{
				if(fabs(vPt.y)-fabs(v4.y)< -ABS_FLOAT_0)
				{
					Curve2.push_back(vPt);
				}
				else
				{
					if(fabs(vPt.y)-fabs(v4.y)> ABS_FLOAT_0)
					{
						Curve2.push_back(vPt3);
						Curve.push_back(vPt3);
						Curve.push_back(vPt);
					}
					else
					{
						Curve2.push_back(vPt);
						Curve.push_back(vPt);
					}
				}
			}
			else
			{
				if(fabs(vPt.y)-fabs(v4.y) > ABS_FLOAT_0)
				{
					if(!bAddExtra)
					{
						Curve2.push_back(vPt2);	
						bAddExtra = TRUE;
					}
					if(!Curve.size() && vPt.vDyn != v4)
					{
						Curve.push_back(vPt2);
					}
				}
				else
				{
					if(!bAddExtra)
					{
						Curve2.push_back(vPt);	
						bAddExtra = TRUE;
					}
				}
				Curve.push_back(vPt);
			}
		}
		else
		{
			if(fabs(vPt.y)-fabs(v4.y)< -ABS_FLOAT_0)
			{
				if(!bAddExtra && vPt.vDyn != v5)
				{
					Curve2.push_back(vPt3);
					Curve.push_back(vPt3);
					bAddExtra = TRUE;
				}
				Curve2.push_back(vPt);
			}
			else
			{
				Curve.push_back(vPt);
				if(i ==  nTransitionPresition)
				{
					if(fabs(vPt.y)-fabs(v4.y)> ABS_FLOAT_0)
					{
						Curve.push_back(vPt2);
						Curve2.push_back(vPt2);
					}
					else
					{
						Curve2.push_back(vPt);
					}
				}
			}
		}
		fAngle += fAngleStep;
	}
}

void _CreateCurves(const float fRU, const float fRD, const float frU, const float frD,
				   const float fHeight,
				   float& fTransitionLen,
			       CurvePtArray &TopCurve, CurvePtArray &OuterCurve, CurvePtArray &BottomCurve, CurvePtArray &InnerCurve,
			       float3 *BevelCenter, 
				   int nTransitionPresition)
{
	float3  vOU = {fRU,fHeight/2.f,0.f};
	float3  vOD = {fRD,-fHeight/2.f,0.f};
	float3  vIU = {frU,fHeight/2.f,0.f};
	float3  vID = {frD,-fHeight/2.f,0.f};


	float3 v1 = {-vOU.x, -vOU.y, -vOU.z}; v1.y = 0;
	float3 v2;
	float3Set(&v2, vOD.x - vOU.x, vOD.y - vOU.y, vOD.z - vOU.z);
	
	// 由于acosf(),这里需要归一
	float3Normalize(&v1, &v1);
	float3Normalize(&v2, &v2);
	float fBiAngle1 = (float)(0.5f*(FLOAT_PI - acosf(float3DotProduct(&v1,&v2))));
	float fBiAngle2 = 0.f;
	if(frU > ABS_FLOAT_0)
	{
		v1 = vIU;v1.y = 0;
		v2 = vID - vIU;
		
		float3Normalize(&v1, &v1);
		float3Normalize(&v2, &v2);

		fBiAngle2 = (float)(0.5f*(FLOAT_PI - acosf(float3DotProduct(&v1,&v2))));
	}

	float fMaxTransLen = (fRU - frU) / (tanf(fBiAngle2) + tanf(fBiAngle1));
	if(fMaxTransLen - fHeight/2.f > ABS_FLOAT_0)
		fMaxTransLen = fHeight/2.f;
	if(fTransitionLen > fMaxTransLen)
		fTransitionLen = fMaxTransLen;

	BevelCenter[2].x = BevelCenter[2].y = BevelCenter[2].z = 0.f;
	CurvePtArray LocalInnerCurve;
	if( frU > ABS_FLOAT_0)
		_CreateCornerCurves(TopCurve,LocalInnerCurve,vID,vIU,vOU,fBiAngle2, fTransitionLen, fHeight, BevelCenter, nTransitionPresition);
	else
	{
		SYVertex3D vPt;
		float3Set(&vPt.vDyn, vIU.x, vIU.y, vIU.z);
		float3Set(&vPt.vNorm, 0.f,1.f,0.f);
		TopCurve.push_back(vPt);
		float3Set(&vPt.vNorm, -1.f,0.f,0.f);
		LocalInnerCurve.push_back(vPt);
	}
	_CreateCornerCurves(TopCurve,OuterCurve,vIU,vOU,vOD,fBiAngle1, fTransitionLen, fHeight, BevelCenter, nTransitionPresition);

	float fBiAngle3 = (float)(0.5f*FLOAT_PI - fBiAngle1);
	float fBiAngle4 = 0.f;

	if( frD > ABS_FLOAT_0)
		fBiAngle4 = (float)(0.5f*FLOAT_PI - fBiAngle2);

	float fMaxTransLen2 = ( fRD -  frD) / (tanf(fBiAngle3) + tanf(fBiAngle4));
	if(fMaxTransLen2 -  fHeight/2.f > ABS_FLOAT_0)
		fMaxTransLen2 =  fHeight/2.f;
	if(fTransitionLen > fMaxTransLen2)
		fTransitionLen = fMaxTransLen2;

	_CreateCornerCurves(BottomCurve,OuterCurve,vOU,vOD,vID,fBiAngle3, fTransitionLen, fHeight, BevelCenter, nTransitionPresition);
	if( frD > ABS_FLOAT_0)
		_CreateCornerCurves(BottomCurve,InnerCurve,vOD,vID,vIU,fBiAngle4, fTransitionLen, fHeight, BevelCenter, nTransitionPresition);
	else
	{
		SYVertex3D vPt;
		float3Set(&vPt.vDyn, vID.x, vID.y, vID.z);
		float3Set(&vPt.vNorm, 0.f, -1.f,0.f);
		BottomCurve.push_back(vPt);
		float3Set(&vPt.vNorm, -1.f,0.f,0.f);
		InnerCurve.push_back(vPt);
	}

	for(size_t i = 0; i < LocalInnerCurve.size(); i ++)
	{
		InnerCurve.push_back(LocalInnerCurve[i]);
	}
	LocalInnerCurve.clear();
	BevelCenter[2].x /= 4.f;
	BevelCenter[2].y /= 4.f;
	BevelCenter[2].z /= 4.f;
}

void _CreateRotYShape(
	std::vector<SYVertex3D>& Curve,
	float fDiameter,float fOffSet,
	eCYLTEXCTYPE TexCoordType,UINT uivTexDim,BOOL bFlip,BOOL bPlane, 
	const float fBeginAngle, const float fEndAngle,
	UINT uSlices,
	SYVertex3D* pVB,UINT& uiVBIndex,DWORD* pIB,UINT& uiIBIndex,
	float3 *BevelCenter,
	std::vector<float3 *> &PlaneVectors1, std::vector<float3 *> &PlaneVectors2
	)
{
	int nSize = (int)Curve.size();
	if(nSize < 2)
	{
		assert(0);
		return;
	}
	float fTotalAngle = fEndAngle - fBeginAngle;
	BOOL bNeedPlane = (fabs(fTotalAngle - FLOAT_PI*2) > ABS_FLOAT_0);
	float fAngleStep = (float)(fTotalAngle/ uSlices); 
	int nScale = bPlane ? 2 : 1;
	float nx,nz,fAngle; fAngle = fBeginAngle;
	int nPreBase,nNextBase,a,b,c,d;
	float fHalfPAI = (float)FLOAT_PI*0.5f;

	for (int i=0;i<(int) uSlices;i++)
	{
		nPreBase = i * nSize * nScale;
		nNextBase = nPreBase + nSize;
		for(int j = 0; j < nSize-1; j++)
		{
			a = nPreBase + j + uiVBIndex; b = a +1;
			c = nNextBase +j + uiVBIndex; d = c+1;
			if(!bFlip)
			{
				pIB[uiIBIndex++] = a;pIB[uiIBIndex++] = b;	pIB[uiIBIndex++] = d;
				pIB[uiIBIndex++] = a;pIB[uiIBIndex++] = d;	pIB[uiIBIndex++] = c;
			}
			else
			{
				pIB[uiIBIndex++] = a;pIB[uiIBIndex++] = d;pIB[uiIBIndex++] = b;
				pIB[uiIBIndex++] = a;pIB[uiIBIndex++] = c;pIB[uiIBIndex++] = d;
			}
		}
	}

	float fBevelR = 0.f;
	matrix4f matRotY;
	SYVertex3D** PreVec = new SYVertex3D*[nSize];
	for (int i=0;i<=(int) uSlices;i++)
	{
		nx = sinf(fAngle);
		nz = cosf(fAngle);

		for(int j = 0; j < nSize; j++)
		{
			//Calc the point
			fBevelR = Curve[j].x;
			pVB[uiVBIndex].x = nx * fBevelR;
			pVB[uiVBIndex].z = nz * fBevelR;
			pVB[uiVBIndex].y = Curve[j].y;
			vector3f vTmp1 = {0.f,1.f,0.f}, vTmp2 = {0.f,-1.f,0.f};
			if(Curve[j].vNorm == vTmp1 ||
				Curve[j].vNorm == vTmp2)
			{
				pVB[uiVBIndex].vNorm = Curve[j].vNorm;
			}
			else
			{
				matrix4f_fromeuler_lyxz(&matRotY, (fAngle-fHalfPAI), 0, 0);
				float3NormalMulMat(&pVB[uiVBIndex].vNorm,&Curve[j].vNorm,&matRotY);
				//D3DXMatrixRotationY(&matRotY,fAngle-fHalfPAI);                      
				//D3DXVec3TransformNormal(&pVB[uiVBIndex].normal,&Curve[j].normal,&matRotY);
			}

			if(fDiameter < ABS_FLOAT_0)
				fDiameter = 0.1f;

			switch(TexCoordType)
			{
			case CYLTEXCTYPE_CYLINDER:
				{
					pVB[uiVBIndex].tu = (float)((float)i/ uSlices);
					pVB[uiVBIndex].tv = 1.f - (float3GetElement(pVB[uiVBIndex].vDyn, uivTexDim) -fOffSet)/ fDiameter;
					break;
				}
			case CYLTEXCTYPE_CYLINDER_REV:
				{
					pVB[uiVBIndex].tu = 1.f - (float)((float)i/ uSlices);
					pVB[uiVBIndex].tv = 1.f - (float3GetElement(pVB[uiVBIndex].vDyn, uivTexDim)-fOffSet)/ fDiameter;
					break;
				}
			case CYLTEXCTYPE_SPHERE:
				{
					pVB[uiVBIndex].tu = (float)((float)i/ uSlices);
					float fR = float3Mode(&pVB[uiVBIndex].vDyn); //D3DXVec3Length(&pVB[uiVBIndex].position);
					float fTh = 1.f;
					if(fR > 0)
						fTh = acosf(pVB[uiVBIndex].y / fR);
					pVB[uiVBIndex].tv = (float)(1.f - fTh / FLOAT_PI);
					break;
				}
			case CYLTEXCTYPE_PRISM_PLANE:
				{
					pVB[uiVBIndex].tu = 0.f;
					pVB[uiVBIndex].tv = 1.f - fabs(/*pVB[uiVBIndex].position[uivTexDim]*/float3GetElement(pVB[uiVBIndex].vDyn, uivTexDim)-fOffSet)/fDiameter;
					break;
				}
			default:
				{
					pVB[uiVBIndex].tu = pVB[uiVBIndex].x / fDiameter;
					pVB[uiVBIndex].tv = 1.f - (/*pVB[uiVBIndex].position[uivTexDim]*/float3GetElement(pVB[uiVBIndex].vDyn, uivTexDim)-fOffSet)/fDiameter;
					break;
				}
			}
			if(bNeedPlane)
			{
				if(i == 0)
				{
					if(!PlaneVectors1.size())
					{
						fBevelR = BevelCenter[2].x;
						BevelCenter[0].x = nx * fBevelR;
						BevelCenter[0].z = nz * fBevelR;
						BevelCenter[0].y = BevelCenter[2].y;
						PlaneVectors1.push_back(&BevelCenter[0]);
					}
					PlaneVectors1.push_back(&pVB[uiVBIndex].vDyn);
				}
				else if( i ==  uSlices)
				{
					if(!PlaneVectors2.size())
					{
						fBevelR = BevelCenter[2].x;
						BevelCenter[1].x = nx * fBevelR;
						BevelCenter[1].z = nz * fBevelR;
						BevelCenter[1].y = BevelCenter[2].y;
						PlaneVectors2.push_back(&BevelCenter[1]);
					}
					PlaneVectors2.push_back(&pVB[uiVBIndex].vDyn);
				}
			}
			if(bPlane)
			{
				PreVec[j] = &pVB[uiVBIndex];
			}
			uiVBIndex++;
		}
		if(i ==  uSlices-1)
			fAngle = fEndAngle;
		else
			fAngle += fAngleStep;

		if(bPlane && i >= 1 && i <= (int) uSlices)
		{
			for(int j = 0; j < nSize; j++)
			{
				if(PreVec[j] == NULL)
					continue;
				if(TexCoordType == CYLTEXCTYPE_PRISM_PLANE)
					PreVec[j]->tu = 1.f;
			}

			if(i != (int) uSlices)
			{
				for(int j = 0; j < nSize; j++)
				{
					if(PreVec[j] == NULL)
						continue;
					pVB[uiVBIndex].vDyn = PreVec[j]->vDyn;
					pVB[uiVBIndex].vNorm = PreVec[j]->vNorm;
					if(TexCoordType == CYLTEXCTYPE_PRISM_PLANE)
						pVB[uiVBIndex].tu = 0.f;
					else
						pVB[uiVBIndex].tu = PreVec[j]->tu;
					pVB[uiVBIndex].tv = PreVec[j]->tv;
					uiVBIndex++;
				}
			}
		}
	}
	delete[] PreVec;
	PreVec = NULL;
}

void _CreateRotLineYShape(SYVertex3D* pV,float fDiameter,float fOffSet,eCYLTEXCTYPE TexCoordType,UINT uivTexDim,BOOL bFlip,BOOL bPlane,
						  const float fBeginAngle, const float fEndAngle,
						  UINT uSlices,
						  SYVertex3D* pVB,UINT& uiVBIndex,DWORD* pIB,UINT& uiIBIndex,
						  std::vector<float3 *> &PlaneVectors1, std::vector<float3 *> &PlaneVectors2)
{
	int nSize = 2;
	float fTotalAngle = fEndAngle - fBeginAngle;
	float fAngleStep = (float)(fTotalAngle/ uSlices); 
	float nx,nz,fAngle; fAngle = fBeginAngle;
	int nPreBase,nNextBase,a,b,c,d;
	int nScale = bPlane ? 2 : 1;
	BOOL bNeedPlane = (fabs(fTotalAngle - FLOAT_PI*2) > ABS_FLOAT_0);
	float fHalfPAI = (float)FLOAT_PI*0.5f;

	for (int i=0;i<(int) uSlices;i++)
	{
		nPreBase = i * nSize * nScale;
		nNextBase = nPreBase + nSize;
		for(int j = 0; j < nSize-1; j++)
		{
			a = nPreBase + j + uiVBIndex; b = a +1;
			c = nNextBase +j + uiVBIndex; d = c+1;
			if(!bFlip)
			{
				pIB[uiIBIndex++] = a;pIB[uiIBIndex++] = b;	pIB[uiIBIndex++] = d;
				pIB[uiIBIndex++] = a;pIB[uiIBIndex++] = d;	pIB[uiIBIndex++] = c;
			}
			else
			{
				pIB[uiIBIndex++] = a;pIB[uiIBIndex++] = d;pIB[uiIBIndex++] = b;
				pIB[uiIBIndex++] = a;pIB[uiIBIndex++] = c;pIB[uiIBIndex++] = d;
			}
		}
	}

	float fBevelR = 0.f;
	matrix4f matRotY;
	SYVertex3D* PreVec[2] = {NULL,NULL};
	for (int i=0;i<=(int) uSlices;i++)
	{
		nx = sinf(fAngle);
		nz = cosf(fAngle);
		for(int j = 0; j < nSize; j++)
		{
			//Calc the point
			fBevelR = pV[j].x;
			pVB[uiVBIndex].x = nx * fBevelR;
			pVB[uiVBIndex].z = nz * fBevelR;
			pVB[uiVBIndex].y = pV[j].y;
			if(!bPlane)
			{
				vector3f vTmp1 = {0.f,1.f,0.f},vTmp2 = {0.f,-1.f,0.f};
				if(pV[j].vNorm == vTmp1 ||
					pV[j].vNorm == vTmp2)
				{
					pVB[uiVBIndex].vNorm = pV[j].vNorm;
				}
				else
				{
					matrix4f_fromeuler_lyxz(&matRotY, (fAngle-fHalfPAI), 0, 0);
					float3NormalMulMat(&pVB[uiVBIndex].vNorm,&pV[j].vNorm,&matRotY);
// 					D3DXMatrixRotationY(&matRotY,fAngle-fHalfPAI);                          
// 					D3DXVec3TransformNormal(&pVB[uiVBIndex].normal,&pV[j].normal,&matRotY);
				}
			}

			if(fDiameter < ABS_FLOAT_0)
				fDiameter = 0.1f;

			switch(TexCoordType)
			{
			case CYLTEXCTYPE_CYLINDER:
				{
					pVB[uiVBIndex].tu = (float)((float)i/ uSlices);
					pVB[uiVBIndex].tv = 1.f - fabs(/*pVB[uiVBIndex].position[uivTexDim]*/float3GetElement(pVB[uiVBIndex].vDyn, uivTexDim) -fOffSet)/fDiameter;
					break;
				}
			case CYLTEXCTYPE_CYLINDER_REV:
				{
					pVB[uiVBIndex].tu = 1.f - (float)((float)i/ uSlices);
					pVB[uiVBIndex].tv = 1.f - fabs(/*pVB[uiVBIndex].position[uivTexDim]*/float3GetElement(pVB[uiVBIndex].vDyn, uivTexDim)-fOffSet)/fDiameter;
					break;
				}
			case CYLTEXCTYPE_SPHERE:
				{
					pVB[uiVBIndex].tu = (float)((float)i/ uSlices);
					float fR = float3Mode(&pVB[uiVBIndex].vDyn);
					float fTh = 1.f;
					if(fR > 0)
						fTh = acosf(pVB[uiVBIndex].y / fR);
					pVB[uiVBIndex].tv = (float)(1.f - fTh / FLOAT_PI);
					break;
				}
			case CYLTEXCTYPE_PRISM_PLANE:
				{
					pVB[uiVBIndex].tu = 0.f;
					pVB[uiVBIndex].tv = 1.f - fabs(/*pVB[uiVBIndex].position[uivTexDim]*/float3GetElement(pVB[uiVBIndex].vDyn, uivTexDim)-fOffSet)/fDiameter;
					break;
				}
			default:
				{
					pVB[uiVBIndex].tu = 0.5f + pVB[uiVBIndex].x / fDiameter;
					pVB[uiVBIndex].tv = 0.5f - (/*pVB[uiVBIndex].position[uivTexDim]*/float3GetElement(pVB[uiVBIndex].vDyn, uivTexDim)-fOffSet)/ fDiameter;
					break;
				}
			}
			if(bNeedPlane)
			{
				if(i == 0 && j == 0)
				{
					PlaneVectors1.push_back(&pVB[uiVBIndex].vDyn);
				}
			}
			if(bPlane)
			{
				PreVec[j] = &pVB[uiVBIndex];
			}
			uiVBIndex++;
		}
		if(i ==  uSlices-1)
			fAngle = fEndAngle;
		else
			fAngle += fAngleStep;
		if (bPlane && i == 0 && TexCoordType == CYLTEXCTYPE_PRISM_PLANE)
		{
			//棱柱物件的第0个侧面需手动置uv, 因为此时的PreVec[]不是上一个侧面的
			assert(uiVBIndex - 2 >= 0);
			pVB[uiVBIndex - 1].tu = 1;
			pVB[uiVBIndex - 2].tu = 1;
		}
		if(bPlane && i >= 1 && i <= (int) uSlices)
		{
			//calcu Normal
			{
				UINT uiIndex = uiVBIndex - 1;
				assert(uiIndex-3 >= 0);
				float3 v3 = pVB[uiIndex].vDyn;
				float3 v2 = pVB[uiIndex-1].vDyn;
				float3 v1 = pVB[uiIndex-2].vDyn;
				float3 v0 = pVB[uiIndex-3].vDyn;

				float3 v4 = v0 - v3;
				float3 v5 = v2 - v1;
				float3CrossProduct(&pVB[uiIndex].vNorm,&v5,&v4);
				float3Normalize(&pVB[uiIndex].vNorm,&pVB[uiIndex].vNorm);
				pVB[uiIndex-1].vNorm = pVB[uiIndex].vNorm;
				pVB[uiIndex-2].vNorm = pVB[uiIndex].vNorm;
				pVB[uiIndex-3].vNorm = pVB[uiIndex].vNorm;

				if(TexCoordType == CYLTEXCTYPE_PRISM_PLANE)
					pVB[uiIndex-1].tu = pVB[uiIndex].tu = 0.f;
			}

			if(i != (int) uSlices)
			{
				for(int j = 0; j < nSize; j++)
				{
					if(PreVec[j] == NULL)
						continue;
					pVB[uiVBIndex].vDyn = PreVec[j]->vDyn;
					if(TexCoordType == CYLTEXCTYPE_PRISM_PLANE)
						pVB[uiVBIndex].tu = 1.f;
					else
						pVB[uiVBIndex].tu = PreVec[j]->tu;
					pVB[uiVBIndex].tv = PreVec[j]->tv;
					uiVBIndex++;
				}
			}
		}
		if(bNeedPlane)
		{
			if(i ==  uSlices)
			{
				PlaneVectors2.push_back(&pVB[uiVBIndex-2].vDyn);
			}
		}
	}
}

void _CalcuLineNormal(vector3f* pOut,vector3f* pIn1,vector3f* pIn2)
{
	if(!pOut || !pIn1 || !pIn2) 
		return;
	*pOut = *pIn2 - *pIn1;
	float xTemp = pOut->x;
	pOut->x = -pOut->y;
	pOut->y = xTemp;
	pOut->z = 0.f;
	float3Normalize(pOut,pOut);
}

// 球
BOOL SYModel_CreateSphere(IN OUT SYModel* pMod, IN float fRad ,BOOL bShpereTex,IN int nLevel /* = 1 */){
	SYInitialSphere(pMod,nLevel);
	SYCreateSphereGraph( pMod, fRad ,bShpereTex,nLevel);	
	SYCreateModelTangent(pMod);
	return TRUE;
}

//创建立方体
BOOL SYModel_CreateBevelBox(SYModel* pMod,float fLength,float fHeight,float fThick,
							int uPartType,
							int nTransitionType,float fTransitionLen,float fTransitionHeight,
							UINT nTSPrecission)
{
	float fMaxTransLen = (float)min(fLength,min(fHeight,fThick))/2.f;
	if(fTransitionLen > fMaxTransLen)
	{
		fTransitionLen = fMaxTransLen;
	}
	int nDivision = 3;
	if(nTransitionType == TRANSITION_PLANE && fTransitionLen > ABS_FLOAT_0  && fTransitionHeight > ABS_FLOAT_0) //导角类型
	{
		int uiType = !uPartType? 1: 2;
		CreatePlaneBevelBox(pMod, fLength, fHeight, fThick, nDivision, uiType, fTransitionLen, fTransitionHeight, TRUE);
	}
	else if(nTransitionType == TRANSITION_ARC && fTransitionLen > ABS_FLOAT_0)
	{
 		if(!uPartType)
 		{
 			CreateSplitSphereBevelBox(pMod, fLength, fHeight, fThick, nDivision, uPartType, fTransitionLen, fTransitionHeight, nTSPrecission);
 		}
 		else
 		{ 
 			CreateSphereBevelBox(pMod, fLength, fHeight, fThick, nDivision, uPartType, fTransitionLen, fTransitionHeight, nTSPrecission);
 		}
	}
	else //无导角
	{
		fTransitionLen = fTransitionHeight = 0.f;
		CreatePlaneBevelBox(pMod, fLength, fHeight, fThick, nDivision, 2, fTransitionLen, fTransitionHeight,FALSE);
	}

	SYCreateModelTangent(pMod);
	return TRUE;
}

//创建柱体
BOOL SYModel_CreateCylinder(SYModel* pMod, BOOL bPrism,float fHeight, 
							UINT uSlices,  
							float fRU, float fRD, float frU, float frD,
							float fTransitionLen, float fTransitionHeight,
							int nTransitionType, int nTransitionPresition, 
							float fStartAng, float fEndAng)
{
	float fBeginAngle = 0.f;
	float fEndAngle = (float)(FLOAT_PI*2);
	
	float f0 = fStartAng,f1 = min(fEndAng,360.f);
	float3 BevelCenter[3];
	float3Set(&BevelCenter[2], 0.f,0.f,0.f);
    
	fBeginAngle = f0 * FLOAT_PI/180.f;
	fEndAngle = f1 * FLOAT_PI/180.f;

	float fBevelScale = 1.f;
	if(bPrism)
	{
		float fAngle = (float)(FLOAT_PI*(0.5f - 1.f/(float)uSlices));
		float fCos = cosf(fAngle);
		if(fCos > ABS_FLOAT_0)
			fBevelScale = (float)1.f/fCos;
	}

	if(fRU < 1.0e-6)
		fRU = 10.0f;
	if(frU < 0)
		frU = 0;
	if(fRD < 1.0e-6)
		fRD = 10.0f;
	if(frD < 0)
		frD = 0;
	if((frU - fRU > ABS_FLOAT_0) || abs(frU - fRU) <= ABS_FLOAT_0 )
		fRU = frU + 10.0f;
	if(frD - fRD > ABS_FLOAT_0 || abs(frD - fRD) <= ABS_FLOAT_0 )
		fRD = frD + 10.0f;


	BOOL bBevel = ((fTransitionLen > ABS_FLOAT_0) && ( nTransitionType != TRANSITION_NONE) );
	BOOL bHaveUpInner =  frU > ABS_FLOAT_0;
	BOOL bHaveUpOutter =  fRU > ABS_FLOAT_0;
	BOOL bHaveDownInner =  frD > ABS_FLOAT_0;
	BOOL bHaveDownOutter =  fRD > ABS_FLOAT_0;

	if(!bBevel)
	{
		if(fTransitionLen > ABS_FLOAT_0)
			fTransitionLen = 0.f;
		if(fTransitionHeight > ABS_FLOAT_0)
			fTransitionHeight = 0.f;
	}

	BOOL bNeedPlane = FALSE;
	if(fabs(fEndAngle - fBeginAngle - FLOAT_PI*2) > ABS_FLOAT_0)
		bNeedPlane = TRUE;

	if(bNeedPlane)
	{
		if(uSlices < 2)
			uSlices = 2;
	}
	else
	{
		if(uSlices < 3)
			uSlices = 3;
	}


	BOOL bPlaneBevel = (	 nTransitionType == TRANSITION_PLANE || 
		nTransitionType == TRANSITION_NONE ||!bBevel);

	pMod->nviewsub = 6;
	pMod->viewsub = new SYSubModel[6];

	UINT uiVBIndex = 0, uiIBIndex = 0;
	CurvePtArray TopCurve, BottomCurve, OuterCurve, InnerCurve ; //SYVertex3D;
 	
	std::vector<float3 *> PlaneVectors1, PlaneVectors2;

	//create curves
	if(!bPlaneBevel)
	{
		_CreateCurves(fRU, fRD, frU, frD,
			fHeight,
			fTransitionLen,
			TopCurve, OuterCurve, BottomCurve, InnerCurve,
			BevelCenter, 
			nTransitionPresition);


		UINT uiCurPts = TopCurve.size()+ BottomCurve.size() + OuterCurve.size() +InnerCurve.size();
		/*nV*/pMod->vcount = ( uSlices + 1) * uiCurPts;
		UINT uiCurPts2 = uiCurPts; 
		if(TopCurve.size())
			uiCurPts2 -= 1;
		if(BottomCurve.size())
			uiCurPts2 -= 1;
		if(OuterCurve.size())
			uiCurPts2 -= 1;
		if(InnerCurve.size())
			uiCurPts2 -= 1;
		/*nF*/pMod->icount =  (uSlices * uiCurPts2 * 2) * 3;
		if(bNeedPlane)
		{
			/*nV*/pMod->vcount += (uiCurPts+1)*2;
			/*nF*/pMod->icount += ((uiCurPts-1)*2) * 3;
		}

		/*pVB*/pMod->v = new SYVertex3D[pMod->vcount];
		/*pIB*/pMod->indecs = new DWORD[pMod->icount];

		DWORD *pIB = pMod->indecs;
		SYVertex3D* pVB = pMod->v;

		UINT uiFaceStart = 0;
		float fDiameter =  fRU- frU;
		float fStart =  frU;

		//top 0
		fDiameter = 2* fRU;
		fStart = 0.f;
		_CreateRotYShape(TopCurve,fDiameter,fStart,CYLTEXCTYPE_PLANE,2,FALSE, FALSE,
			fBeginAngle,fEndAngle,
			uSlices,
			pVB,uiVBIndex,pIB,uiIBIndex,
			BevelCenter,
			PlaneVectors1, PlaneVectors2);

		pMod->viewsub[0].vstart= 0;
		pMod->viewsub[0].vcount =  pMod->vcount;
		pMod->viewsub[0].istart = uiFaceStart*3;
		pMod->viewsub[0].icount = (TopCurve.size()-1)* uSlices*2 *3;
		pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST ;
		pMod->viewsub[0].nTri_Num = (TopCurve.size() - 1)* uSlices*2 ;
		pMod->viewsub[0].pMaterial = NULL;
		uiFaceStart += (TopCurve.size()-1)* uSlices*2 ;

		//outer 1
		fDiameter =  fHeight - 2 * fTransitionLen;//fTransitionHeight;
		fStart = - fHeight*0.5f + fTransitionLen;
		_CreateRotYShape(OuterCurve,fDiameter,fStart,CYLTEXCTYPE_CYLINDER_REV,1,FALSE,FALSE,
			fBeginAngle,fEndAngle,
			uSlices,
			pVB,uiVBIndex,pIB,uiIBIndex,
			BevelCenter,
			PlaneVectors1, PlaneVectors2);

		pMod->viewsub[1].vstart= 0;
		pMod->viewsub[1].vcount =  pMod->vcount;
		pMod->viewsub[1].istart = uiFaceStart*3;
		pMod->viewsub[1].icount = (OuterCurve.size()-1)* uSlices*2 *3;
		pMod->viewsub[1].nTri_Type = SY_TRIANGLE_LIST ;
		pMod->viewsub[1].nTri_Num = (OuterCurve.size()-1)* uSlices*2 ;
		pMod->viewsub[1].pMaterial = NULL;
		uiFaceStart += (OuterCurve.size()-1)* uSlices*2 ;

		//bottom 2
		fDiameter = 2* fRD;
		fStart =  frD;
		_CreateRotYShape(BottomCurve,fDiameter,fStart,CYLTEXCTYPE_PLANE,2,FALSE, FALSE,
			fBeginAngle,fEndAngle,
			uSlices,
			pVB,uiVBIndex,pIB,uiIBIndex,
			BevelCenter,
			PlaneVectors1, PlaneVectors2);

		pMod->viewsub[2].vstart= 0;
		pMod->viewsub[2].vcount =  pMod->vcount;
		pMod->viewsub[2].istart = uiFaceStart*3;
		pMod->viewsub[2].icount = (BottomCurve.size()-1)* uSlices*2 *3;
		pMod->viewsub[2].nTri_Type = SY_TRIANGLE_LIST ;
		pMod->viewsub[2].nTri_Num = (BottomCurve.size()-1)* uSlices*2 ;
		pMod->viewsub[2].pMaterial = NULL;
		uiFaceStart += (BottomCurve.size()-1)* uSlices*2 ;

		//Inner 3
		fDiameter =  fHeight - 2 * fTransitionLen;//fTransitionHeight;
		fStart = - fHeight*0.5f + fTransitionLen;
		_CreateRotYShape(InnerCurve,fDiameter,fStart,CYLTEXCTYPE_CYLINDER,1,FALSE,FALSE,
			fBeginAngle,fEndAngle,
			uSlices,
			pVB,uiVBIndex,pIB,uiIBIndex,
			BevelCenter,
			PlaneVectors1, PlaneVectors2);

		pMod->viewsub[3].vstart= 0;
		pMod->viewsub[3].vcount =  pMod->vcount;
		pMod->viewsub[3].istart = uiFaceStart*3;
		pMod->viewsub[3].icount = (InnerCurve.size()-1)* uSlices*2 *3;
		pMod->viewsub[3].nTri_Type = SY_TRIANGLE_LIST ;
		pMod->viewsub[3].nTri_Num = (InnerCurve.size()-1)* uSlices*2 ;
		pMod->viewsub[3].pMaterial = NULL;
		uiFaceStart += (InnerCurve.size()-1)* uSlices*2 ;

		if(bNeedPlane)
		{
			fDiameter = max(( fRU- frU),( fRD- frD));
			fStart = min( frU, frD);
			float3 vNormal = {0.f,0.f,-1.f};
			float3 v1, v2, v3, v4;
			float3Set(&v1, (*PlaneVectors1[0]).x, (*PlaneVectors1[0]).y, (*PlaneVectors1[0]).z);
			float3Set(&v2, (*PlaneVectors1[1]).x, (*PlaneVectors1[1]).y, (*PlaneVectors1[1]).z);
			float3Set(&v3, (*PlaneVectors1[2]).x, (*PlaneVectors1[2]).y, (*PlaneVectors1[2]).z);
			float3Set(&v4, v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
			float3Set(&v1, v3.x - v2.x, v3.y - v2.y, v3.z - v2.z);

			float3CrossProduct(&vNormal,&v1,&v4);
			float3Normalize(&vNormal,&vNormal);


			float3 vNormal2 = {0.f,0.f,-1.f};
			float3Set(&v1, (*PlaneVectors2[0]).x, (*PlaneVectors2[0]).y, (*PlaneVectors2[0]).z);
			float3Set(&v2, (*PlaneVectors2[1]).x, (*PlaneVectors2[1]).y, (*PlaneVectors2[1]).z);
			float3Set(&v3, (*PlaneVectors2[2]).x, (*PlaneVectors2[2]).y, (*PlaneVectors2[2]).z);
			float3Set(&v4, v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
			float3Set(&v1, v3.x - v2.x, v3.y - v2.y, v3.z - v2.z);


			UINT uiFaceCnt = uiCurPts-1;int i;
			for(i = 0; i < (int)uiFaceCnt; i++)
			{
				pIB[uiIBIndex] = uiVBIndex;
				uiIBIndex++;
				pIB[uiIBIndex] = uiVBIndex+i+2;
				uiIBIndex++;
				pIB[uiIBIndex] = uiVBIndex+i+1;
				uiIBIndex++;
			}
			for(i = 0; i < (int)PlaneVectors1.size(); i++)
			{
				float3Set(&pVB[uiVBIndex].vDyn, (*PlaneVectors1[i]).x, (*PlaneVectors1[i]).y, (*PlaneVectors1[i]).z);
				float3Set(&pVB[uiVBIndex].vNorm,  -vNormal.x,  -vNormal.y,  -vNormal.z);
				float3Set(&v1,  pVB[uiVBIndex].x,  pVB[uiVBIndex].y, pVB[uiVBIndex].z);
				v1.y = 0.f;
				//pVB[uiVBIndex].position = *PlaneVectors1[i];
				//pVB[uiVBIndex].normal = -vNormal;
				//v1 = pVB[uiVBIndex].position;v1.y = 0.f;
				pVB[uiVBIndex].tu = (float)((float3Mode(&v1) - fStart)/fDiameter);
				pVB[uiVBIndex].tv = 0.5f - (float)(pVB[uiVBIndex].y/ fHeight);
				uiVBIndex++;
			}
			PlaneVectors1.clear();

			pMod->viewsub[4].vstart= 0;
			pMod->viewsub[4].vcount =  pMod->vcount;
			pMod->viewsub[4].istart = uiFaceStart*3;
			pMod->viewsub[4].icount = uiFaceCnt *3;
			pMod->viewsub[4].nTri_Type = SY_TRIANGLE_LIST ;
			pMod->viewsub[4].nTri_Num = uiFaceCnt ;
			pMod->viewsub[4].pMaterial = NULL;
			uiFaceStart += uiFaceCnt ;

			for(int i = 0; i < (int)uiFaceCnt; i++)
			{
				pIB[uiIBIndex] = uiVBIndex;
				uiIBIndex++;
				pIB[uiIBIndex] = uiVBIndex+i+1;
				uiIBIndex++;
				pIB[uiIBIndex] = uiVBIndex+i+2;
				uiIBIndex++;
			}
			for(i = 0; i < (int)PlaneVectors2.size(); i++)
			{
				float3Set(&pVB[uiVBIndex].vDyn, (*PlaneVectors2[i]).x, (*PlaneVectors2[i]).y, (*PlaneVectors2[i]).z);
				float3Set(&pVB[uiVBIndex].vNorm,  vNormal2.x,  vNormal2.y,  vNormal2.z);
				float3Set(&v1,  pVB[uiVBIndex].x,  pVB[uiVBIndex].y, pVB[uiVBIndex].z);
				v1.y = 0.f;
			
				pVB[uiVBIndex].tu = (float)((float3Mode(&v1) - fStart)/fDiameter);
				pVB[uiVBIndex].tv = 0.5f - (float)(pVB[uiVBIndex].y/ fHeight);
				uiVBIndex++;
			}
			PlaneVectors2.clear();

			pMod->viewsub[5].vstart= 0;
			pMod->viewsub[5].vcount =  pMod->vcount;
			pMod->viewsub[5].istart = uiFaceStart*3;
			pMod->viewsub[5].icount = uiFaceCnt *3;
			pMod->viewsub[5].nTri_Type = SY_TRIANGLE_LIST ;
			pMod->viewsub[5].nTri_Num = uiFaceCnt ;
			pMod->viewsub[5].pMaterial = NULL;
			//uiFaceStart += uiFaceCnt ;
		}
		else
		{
			pMod->viewsub[4].vstart= 0;
			pMod->viewsub[4].vcount =  pMod->vcount;
			pMod->viewsub[4].istart = 0;
			pMod->viewsub[4].icount = 0;
			pMod->viewsub[4].nTri_Type = SY_TRIANGLE_LIST ;
			pMod->viewsub[4].nTri_Num = 0 ;
			pMod->viewsub[4].pMaterial = NULL;
			
			pMod->viewsub[5].vstart= 0;
			pMod->viewsub[5].vcount =  pMod->vcount;
			pMod->viewsub[5].istart = 0;
			pMod->viewsub[5].icount = 0;
			pMod->viewsub[5].nTri_Type = SY_TRIANGLE_LIST ;
			pMod->viewsub[5].nTri_Num = 0 ;
			pMod->viewsub[5].pMaterial = NULL;

		}

		TopCurve.clear();
		OuterCurve.clear();
		BottomCurve.clear();
		InnerCurve.clear();
	}
	else
	{
		UINT uiFactor = 16;
		UINT uiPlaneTri = 12;
		if(!bHaveUpInner || !bBevel)
		{
			uiFactor -= 2;
			uiPlaneTri -= 2;
		}
		if(!bHaveDownInner || !bBevel)
		{
			uiFactor -= 2;
			uiPlaneTri -= 2;
		}
		if(!bHaveUpOutter || !bBevel)
		{
			uiFactor -= 2;
			uiPlaneTri -= 2;
		}
		if(!bHaveDownOutter || !bBevel)
		{
			uiFactor -= 2;
			uiPlaneTri -= 2;
		}

		UINT uiFVNum = 0;//( uSlices + 1)*2;

		pMod->vcount = ( uSlices + 1) * uiFactor+uiFVNum*8;
		if(bPrism)
		{
			pMod->vcount += uiFactor * ( uSlices-1);
		}
		pMod->icount =  (uSlices * uiFactor + uiFVNum*8) * 3;

		if(bNeedPlane)
		{
			pMod->vcount += uiFactor;//bBevel ? 16 : 8;
			pMod->icount += uiPlaneTri * 3;//bBevel ? 12 : 4;  
		}

		pMod->v = new SYVertex3D[pMod->vcount]; 
		pMod->indecs = new DWORD[pMod->icount];

		SYVertex3D *pVB = pMod->v;
		DWORD *pIB = pMod->indecs;

		if(fTransitionHeight -  fHeight/2.f > ABS_FLOAT_0)
			fTransitionHeight =  fHeight/2.f;

		float t = fTransitionHeight/ fHeight;
		float t1 = 1.f-t;
		float fTranScale = ( frU > ABS_FLOAT_0) ? 0.5f : 1.f;
		float fMaxTransLen = fTranScale * (( fRU -  frU)*t1+t*( fRD -  frD));
		fTransitionLen *= fBevelScale;
		if(fTransitionLen > fMaxTransLen)
			fTransitionLen = fMaxTransLen/fBevelScale;
		float Y =  fHeight/2.f-fTransitionHeight; 
		float3 v[8];
		if(bHaveUpInner)
		{
			float3Set(&v[0], t1* frU+t* frD,Y,0.f);
			v[1].x = v[0].x + fTransitionLen;v[1].y =  fHeight/2.f;v[1].z = 0.f;

		}
		else
		{
			float3Set(&v[0], 0.f, fHeight/2.f,0.f);
			float3Set(&v[1], 0.f, fHeight/2.f,0.f);	
		}

		float3Set(&v[3], t1* frU+t* frD,Y,0.f);
	    v[3].x = (t1* fRU+t* fRD);
		v[2].x = v[3].x - fTransitionLen;v[2].y =  fHeight/2.f;v[2].z = 0.f;

		float fTranScaleB = ( frD > ABS_FLOAT_0) ? 0.5f : 1.f;
		float fMaxTransLenB = fTranScaleB *(( fRD -  frD)*t1+t*( fRU -  frU));
		fTransitionLen *= fBevelScale;
		if(fTransitionLen > fMaxTransLenB)
			fTransitionLen = fMaxTransLenB/fBevelScale;
		float Y2 = - fHeight/2.f+fTransitionHeight; 
		float3Set(&v[4], t1* fRD+t* fRU,Y2,0.f);
		v[5].x = v[4].x - fTransitionLen;v[5].y = - fHeight/2.f; v[5].z=0.f;
		if(bHaveDownInner)
		{
			v[7] = v[4]; v[7].x = t1* frD+t* frU;
			v[6] = v[5]; v[6].x = v[7].x + fTransitionLen;
		}
		else
		{
			vector3f vTmp = {0.f,- fHeight/2.f,0.f};
			v[7] = v[6] = vTmp;
		}

		UINT uiFaceStart = 0;

		SYVertex3D vPt[2];
		float fDiameter =  fRU- frU;
		float fStart =  frU;
		//top
		fDiameter = 2 *  fRU;
		fStart = 0;
		vPt[0].vDyn = v[0];
		vPt[1].vDyn = v[1];
		UINT uiSegCnt = 0;
		if(bHaveUpInner && bBevel)
		{
			_CalcuLineNormal(&vPt[0].vNorm,&v[0],&v[1]);
			vPt[1].vNorm = vPt[0].vNorm;
			_CreateRotLineYShape(vPt,fDiameter,fStart,CYLTEXCTYPE_PLANE,2,FALSE,bPrism,
				fBeginAngle, fEndAngle,
				uSlices,
				pVB,uiVBIndex,pIB,uiIBIndex,
				PlaneVectors1, PlaneVectors2);
			uiSegCnt++;
		}

		vector3f vTmp = {0.f,1.f,0.f};
		vPt[0].vDyn = v[1];vPt[0].vNorm = vTmp;
		vPt[1].vDyn = v[2];vPt[1].vNorm = vPt[0].vNorm;
		_CreateRotLineYShape(vPt,fDiameter,fStart,CYLTEXCTYPE_PLANE,2,FALSE,bPrism,
			fBeginAngle, fEndAngle,
			uSlices,
			pVB,uiVBIndex,pIB,uiIBIndex,
			PlaneVectors1, PlaneVectors2);
		uiSegCnt++;

		if(bBevel)
		{
			vPt[0].vDyn = v[2];_CalcuLineNormal(&vPt[0].vNorm,&v[2],&v[3]);
			vPt[1].vDyn = v[3];vPt[1].vNorm = vPt[0].vNorm;
			_CreateRotLineYShape(vPt,fDiameter,fStart,CYLTEXCTYPE_PLANE,2,FALSE,bPrism,
				fBeginAngle, fEndAngle,
				uSlices,
				pVB,uiVBIndex,pIB,uiIBIndex,
				PlaneVectors1, PlaneVectors2);
			uiSegCnt++;
		}

		pMod->viewsub[0].vstart= 0;
		pMod->viewsub[0].vcount =  pMod->vcount;
		pMod->viewsub[0].istart = uiFaceStart*3;
		pMod->viewsub[0].icount = uSlices*uiSegCnt*2 *3;
		pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST ;
		pMod->viewsub[0].nTri_Num = uSlices*uiSegCnt*2 ;
		pMod->viewsub[0].pMaterial = NULL;
		uiFaceStart += uSlices*uiSegCnt*2 ;

		//outer
		fDiameter =  fHeight - 2*fTransitionHeight;
		fStart = Y2;

		vPt[0].vDyn = v[3];_CalcuLineNormal(&vPt[0].vNorm,&v[3],&v[4]);
		vPt[1].vDyn = v[4];vPt[1].vNorm = vPt[0].vNorm;
		_CreateRotLineYShape(vPt,fDiameter,fStart,CYLTEXCTYPE_CYLINDER_REV,1,FALSE,bPrism,
			fBeginAngle, fEndAngle,
			uSlices,
			pVB,uiVBIndex,pIB,uiIBIndex,
			PlaneVectors1, PlaneVectors2);

		pMod->viewsub[1].vstart= 0;
		pMod->viewsub[1].vcount =  pMod->vcount;
		pMod->viewsub[1].istart = uiFaceStart*3;
		pMod->viewsub[1].icount = (uSlices*2 + uiFVNum *8) *3;
		pMod->viewsub[1].nTri_Type = SY_TRIANGLE_LIST ;
		pMod->viewsub[1].nTri_Num = uSlices*2 + uiFVNum *8 ;
		pMod->viewsub[1].pMaterial = NULL;
		uiFaceStart += uSlices*2 + uiFVNum *8 ;


		//bottom
		uiSegCnt = 0;
		fDiameter = 2* fRD;
		fStart = 0.f;

		if(bBevel)
		{
			float3Set(&vPt[0].vDyn, v[4].x, v[4].y, v[4].z);
			_CalcuLineNormal(&vPt[0].vNorm,&v[4],&v[5]);
			float3Set(&vPt[1].vDyn, v[5].x, v[5].y, v[5].z);
			float3Set(&vPt[1].vNorm,vPt[0].nx,vPt[0].ny, vPt[0].nz);
			_CreateRotLineYShape(vPt,fDiameter,fStart,CYLTEXCTYPE_PLANE,2,FALSE,bPrism,
				fBeginAngle, fEndAngle,
				uSlices,
				pVB,uiVBIndex,pIB,uiIBIndex,
				PlaneVectors1, PlaneVectors2);
			uiSegCnt++;
		}
		
		float3Set(&vPt[0].vDyn, v[5].x, v[5].y, v[5].z);
		float3Set(&vPt[0].vNorm, 0.f,-1.f,0.f);
		float3Set(&vPt[1].vDyn, v[6].x, v[6].y, v[6].z);
		float3Set(&vPt[1].vNorm, vPt[0].nx, vPt[0].ny, vPt[0].nz);
		_CreateRotLineYShape(vPt,fDiameter,fStart,CYLTEXCTYPE_PLANE,2,FALSE,bPrism,
			fBeginAngle, fEndAngle,
			uSlices,
			pVB,uiVBIndex,pIB,uiIBIndex,
			PlaneVectors1, PlaneVectors2);
		uiSegCnt++;

		float3Set(&vPt[0].vDyn, v[6].x, v[6].y, v[6].z);
		float3Set(&vPt[1].vDyn, v[7].x, v[7].y, v[7].z);
		if(bHaveDownInner && bBevel)
		{
			_CalcuLineNormal(&vPt[0].vNorm,&v[6],&v[7]);
			float3Set(&vPt[1].vNorm, vPt[0].nx, vPt[0].ny, vPt[0].nz);
			_CreateRotLineYShape(vPt,fDiameter,fStart,CYLTEXCTYPE_PLANE,2,FALSE,bPrism,
				fBeginAngle, fEndAngle,
				uSlices,
				pVB,uiVBIndex,pIB,uiIBIndex,
				PlaneVectors1, PlaneVectors2);
			uiSegCnt++;
		}
		pMod->viewsub[2].vstart= 0;
		pMod->viewsub[2].vcount =  pMod->vcount;
		pMod->viewsub[2].istart = uiFaceStart*3;
		pMod->viewsub[2].icount = (uSlices*uiSegCnt*2) *3;
		pMod->viewsub[2].nTri_Type = SY_TRIANGLE_LIST ;
		pMod->viewsub[2].nTri_Num = uSlices*uiSegCnt*2 ;
		pMod->viewsub[2].pMaterial = NULL;
		uiFaceStart +=uSlices*uiSegCnt*2;

		//Inner
		fDiameter =  fHeight - 2*fTransitionHeight;
		fStart = Y2;


		float3Set(&vPt[0].vDyn, v[7].x, v[7].y, v[7].z);
		_CalcuLineNormal(&vPt[0].vNorm,&v[7],&v[0]);
		float3Set(&vPt[1].vDyn, v[0].x, v[0].y, v[0].z);
		float3Set(&vPt[1].vNorm, vPt[0].nx, vPt[0].ny, vPt[0].nz);
		_CreateRotLineYShape(vPt,fDiameter,fStart,CYLTEXCTYPE_CYLINDER,1,FALSE,bPrism,
			fBeginAngle, fEndAngle,
			uSlices,
			pVB,uiVBIndex,pIB,uiIBIndex,
			PlaneVectors1, PlaneVectors2);

		pMod->viewsub[3].vstart= 0;
		pMod->viewsub[3].vcount =  pMod->vcount;
		pMod->viewsub[3].istart = uiFaceStart*3;
		pMod->viewsub[3].icount = uSlices*2 *3;
		pMod->viewsub[3].nTri_Type = SY_TRIANGLE_LIST ;
		pMod->viewsub[3].nTri_Num = uSlices*2 ;
		pMod->viewsub[3].pMaterial = NULL;
		uiFaceStart +=uSlices*2;

		if(bNeedPlane)
		{
			//Plane 0
			fDiameter = max(( fRU- frU),( fRD- frD));
			fStart = min( frU, frD);
			assert(PlaneVectors1.size() >= 4);
			assert(PlaneVectors2.size() >= 4);
			float3 vNormal = {0.f,0.f,-1.f};
			float3 v1, v2, v3, v4;
			float3Set(&v1, (*PlaneVectors1[0]).x, (*PlaneVectors1[0]).y, (*PlaneVectors1[0]).z);
			float3Set(&v2, (*PlaneVectors1[1]).x, (*PlaneVectors1[1]).y, (*PlaneVectors1[1]).z);
			float3Set(&v3, (*PlaneVectors1[2]).x, (*PlaneVectors1[2]).y, (*PlaneVectors1[2]).z);
			float3Set(&v4, v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
			float3Set(&v1, v3.x - v2.x, v3.y - v2.y, v3.z - v2.z);
			float3CrossProduct(&vNormal,&v4,&v1);
			float3Normalize(&vNormal,&vNormal);


			float3 vNormal2 = {0.f,0.f,-1.f};
			float3Set(&v1, (*PlaneVectors2[0]).x, (*PlaneVectors2[0]).y, (*PlaneVectors2[0]).z);
			float3Set(&v2, (*PlaneVectors2[1]).x, (*PlaneVectors2[1]).y, (*PlaneVectors2[1]).z);
			float3Set(&v3, (*PlaneVectors2[2]).x, (*PlaneVectors2[2]).y, (*PlaneVectors2[2]).z);
			float3Set(&v4, v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
			float3Set(&v1, v3.x - v2.x, v3.y - v2.y, v3.z - v2.z);
			float3CrossProduct(&vNormal2,&v1,&v4);
			float3Normalize(&vNormal2,&vNormal2);

			{
				UINT uiFaceCnt = uiPlaneTri/2;
				UINT uiIndex[18] = {0,1,2,0,2,3,0,3,4,0,4,5,0,5,6,0,6,7};
				UINT uiIndexCnt = 0;int i;
				for(i = 0; i < (int)uiFaceCnt; i++)
				{
					pIB[uiIBIndex] = uiIndex[3*i]+uiVBIndex;
					uiIBIndex++;
					pIB[uiIBIndex] = uiIndex[3*i+2]+uiVBIndex;
					uiIBIndex++;
					pIB[uiIBIndex] = uiIndex[3*i+1]+uiVBIndex;
					uiIBIndex++;
					uiIndexCnt += 3;
				}
				for(i = 0; i < (int)PlaneVectors1.size(); i++)
				{
					float3Set(&pVB[uiVBIndex].vDyn, PlaneVectors1[i]->x, PlaneVectors1[i]->y, PlaneVectors1[i]->z);
					float3Set(&pVB[uiVBIndex].vNorm, vNormal.x, vNormal.y, vNormal.z);
					float3Set(&v1,pVB[uiVBIndex].x, pVB[uiVBIndex].y, pVB[uiVBIndex].z);
					v1.y = 0.f;
					pVB[uiVBIndex].tu = (float)((float3Mode(&v1) - fStart)/fDiameter);
					pVB[uiVBIndex].tv = 0.5f - (float)(pVB[uiVBIndex].y/ fHeight);
					uiVBIndex++;
				}
				PlaneVectors1.clear();

				pMod->viewsub[4].vstart= 0;
				pMod->viewsub[4].vcount =  pMod->vcount;
				pMod->viewsub[4].istart = uiFaceStart*3;
				pMod->viewsub[4].icount = uiFaceCnt *3;
				pMod->viewsub[4].nTri_Type = SY_TRIANGLE_LIST ;
				pMod->viewsub[4].nTri_Num = uiFaceCnt ;
				pMod->viewsub[4].pMaterial = NULL;
				uiFaceStart += uiFaceCnt;

				for(int i = 0; i < (int)uiIndexCnt; i++)
				{
					pIB[uiIBIndex] = uiIndex[i]+uiVBIndex;
					uiIBIndex++;
				}
				for(i = 0; i < (int)PlaneVectors2.size(); i++)
				{
					float3Set(&pVB[uiVBIndex].vDyn, (*PlaneVectors2[i]).x, (*PlaneVectors2[i]).y, (*PlaneVectors2[i]).z);
					float3Set(&pVB[uiVBIndex].vNorm, vNormal2.x, vNormal2.y, vNormal2.z);
					pVB[uiVBIndex].tu = (float)((float3Mode(&v1) - fStart)/fDiameter);
					pVB[uiVBIndex].tv = 0.5f - (float)(pVB[uiVBIndex].y/ fHeight);
					uiVBIndex++;
				}
				PlaneVectors2.clear();
				pMod->viewsub[5].vstart= 0;
				pMod->viewsub[5].vcount =  pMod->vcount;
				pMod->viewsub[5].istart = uiFaceStart*3;
				pMod->viewsub[5].icount = uiFaceCnt *3;
				pMod->viewsub[5].nTri_Type = SY_TRIANGLE_LIST ;
				pMod->viewsub[5].nTri_Num = uiFaceCnt ;
				pMod->viewsub[5].pMaterial = NULL;
				//uiFaceStart += uiFaceCnt;
			}
		}
		else
		{
			pMod->viewsub[4].vstart= 0;
			pMod->viewsub[4].vcount =  pMod->vcount;
			pMod->viewsub[4].istart = 0;
			pMod->viewsub[4].icount = 0;
			pMod->viewsub[4].nTri_Type = SY_TRIANGLE_LIST ;
			pMod->viewsub[4].nTri_Num = 0 ;
			pMod->viewsub[4].pMaterial = NULL;

			pMod->viewsub[5].vstart= 0;
			pMod->viewsub[5].vcount =  pMod->vcount;
			pMod->viewsub[5].istart = 0;
			pMod->viewsub[5].icount = 0;
			pMod->viewsub[5].nTri_Type = SY_TRIANGLE_LIST ;
			pMod->viewsub[5].nTri_Num = 0 ;
			pMod->viewsub[5].pMaterial = NULL;

		}
	}

	SYCreateModelTangent(pMod);
	return TRUE;
}

//创建棱柱
BOOL SYModel_CreatePrism(SYModel* pMod, float fHeight,
							UINT uSlices, 
							float fRU, float fRD, float frU, float frD,
							float fTransitionLen, float fTransitionHeight,
							int nTransitionType, int nTransitionPresition
							)
{
	assert(pMod);
	float fBeginAngle = 0.f;
	float fEndAngle = (float)(FLOAT_PI*2);

	float fBevelScale = 1.f;
	float fAngle = (float)(FLOAT_PI*(0.5f - 1.f/(float)uSlices));
	float fCos = cosf(fAngle);
	if(fCos > ABS_FLOAT_0)
		fBevelScale = (float)1.f/fCos;

	if(fRU < 1.0e-6)
		fRU = 0.005f;
	if(frU < 0)
		frU = 0;
	if(fRD < 1.0e-6)
		fRD = 0.005f;
	if(frD < 0)
		frD = 0;
	if(frU - fRU > ABS_FLOAT_0 )
		fRU = frU + 0.005f;
	if(frD - fRD > ABS_FLOAT_0 )
		fRD = frD + 0.005f;


	BOOL bBevel = ((fTransitionLen > ABS_FLOAT_0) && ( nTransitionType != TRANSITION_NONE) );
	BOOL bHaveUpInner =  frU > ABS_FLOAT_0;
	BOOL bHaveUpOutter =  fRU > ABS_FLOAT_0;
	BOOL bHaveDownInner =  frD > ABS_FLOAT_0;
	BOOL bHaveDownOutter =  fRD > ABS_FLOAT_0;

	if(!bBevel)
	{
		if(fTransitionLen > ABS_FLOAT_0)
			fTransitionLen = 0.f;
		if(fTransitionHeight > ABS_FLOAT_0)
			fTransitionHeight = 0.f;
	}

	BOOL bNeedPlane = FALSE;
	if(fabs(fEndAngle - fBeginAngle - FLOAT_PI*2) > ABS_FLOAT_0)
		bNeedPlane = TRUE;

	if(bNeedPlane)
	{
		if(uSlices < 2)
			uSlices = 2;
	}
	else
	{
		if(uSlices < 3)
			uSlices = 3;
	}


	BOOL bPlaneBevel = (	 nTransitionType == TRANSITION_PLANE || 
		nTransitionType == TRANSITION_NONE ||!bBevel);

	UINT uiAttrCnt = uSlices*4;

	pMod->nviewsub = uiAttrCnt+2;
	pMod->viewsub = new SYSubModel[uiAttrCnt+2];

	UINT uiVBIndex = 0;
	UINT uiIBIndex = 0;

	UINT uiTopFaceCnt = 0;
	UINT uiOutFaceCnt = 0;
	UINT uiBottomFaceCnt = 0;
	UINT uiInnerFaceCnt = 0;
	UINT uiFaceCnt = 0;
	UINT uiTotalTopFaceCnt = 0;
	UINT uiTotalOuterFaceCnt = 0;
	UINT uiTotalBottomFaceCnt = 0;
	UINT uiTotalInnerFaceCnt = 0;

	CurvePtArray TopCurve, BottomCurve, OuterCurve, InnerCurve ; //SYVertex3D;
	float3 BevelCenter[3];
	float3Set(&BevelCenter[2], 0.f,0.f,0.f);
	std::vector<float3 *> PlaneVectors1, PlaneVectors2;
	//create curves
	if(!bPlaneBevel)
	{
		_CreateCurves(fRU, fRD, frU, frD,
			fHeight,
			fTransitionLen,
			TopCurve, OuterCurve, BottomCurve, InnerCurve,
			BevelCenter, 
			nTransitionPresition);
		UINT uiCurPts = TopCurve.size()+ BottomCurve.size() + OuterCurve.size() +InnerCurve.size();
		/*nV*/pMod->vcount =  2 * uSlices * uiCurPts;//pts*(s+1) + pts*(s-1)
		UINT uiCurPts2 = uiCurPts; 
		if(TopCurve.size())
			uiCurPts2 -= 1;
		if(BottomCurve.size())
			uiCurPts2 -= 1;
		if(OuterCurve.size())
			uiCurPts2 -= 1;
		if(InnerCurve.size())
			uiCurPts2 -= 1;
		/*nF*/pMod->icount =  (uSlices * uiCurPts2 * 2) * 3;
		if(bNeedPlane)
		{
			/*nV*/pMod->vcount += (uiCurPts+1)*2;
			/*nF*/pMod->icount += (uiCurPts-1)*2 * 3;
		}

		pMod->v = new SYVertex3D[pMod->vcount];
		pMod->indecs = new DWORD[pMod->icount];

		DWORD *pIB = pMod->indecs;
		SYVertex3D* pVB = pMod->v;

		UINT uiFaceStart = 0;
		float fDiameter =  fRU- frU;
		float fStart =  frU;

		//top 0
		fDiameter = 2* fRU;
		fStart = 0.f;
		//CreateRotYShape(TopCurve,fDiameter,fStart,CYLTEXCTYPE_PLANE,2,FALSE,TRUE);
	    _CreateRotYShape(TopCurve,fDiameter,fStart,CYLTEXCTYPE_PLANE,2,FALSE,TRUE,
			fBeginAngle,fEndAngle,
			uSlices,
			pVB,uiVBIndex,pIB,uiIBIndex,
			BevelCenter,
			PlaneVectors1, PlaneVectors2);


		uiTotalTopFaceCnt = (TopCurve.size()-1)* uSlices*2;
		//outer 1
		fDiameter =  fHeight - 2 * fTransitionLen;//fTransitionHeight;
		fStart = -fHeight*0.5f + fTransitionLen;
		_CreateRotYShape(OuterCurve,fDiameter,fStart,CYLTEXCTYPE_PRISM_PLANE,1,FALSE,TRUE,
			fBeginAngle,fEndAngle,
			uSlices,
			pVB,uiVBIndex,pIB,uiIBIndex,
			BevelCenter,
			PlaneVectors1, PlaneVectors2);
		uiTotalOuterFaceCnt = (OuterCurve.size()-1)* uSlices*2;

		//bottom 2
		fDiameter = 2* fRD;
		fStart =  frD;
		_CreateRotYShape(BottomCurve,fDiameter,fStart,CYLTEXCTYPE_PLANE,2,FALSE,TRUE,
			fBeginAngle,fEndAngle,
			uSlices,
			pVB,uiVBIndex,pIB,uiIBIndex,
			BevelCenter,
			PlaneVectors1, PlaneVectors2);
		uiTotalBottomFaceCnt = (BottomCurve.size()-1)* uSlices*2;

		//Inner 3
		fDiameter =  fHeight - 2 * fTransitionLen;//fTransitionHeight;
		fStart = -fHeight*0.5f + fTransitionLen;
		_CreateRotYShape(InnerCurve,fDiameter,fStart,CYLTEXCTYPE_PRISM_PLANE,1,FALSE,TRUE,
			fBeginAngle,fEndAngle,
			uSlices,
			pVB,uiVBIndex,pIB,uiIBIndex,
			BevelCenter,
			PlaneVectors1, PlaneVectors2);
		uiTotalInnerFaceCnt = (InnerCurve.size()-1)* uSlices*2;

		uiTopFaceCnt = (TopCurve.size()-1)*2;
		uiOutFaceCnt = (OuterCurve.size()-1)*2;
		uiBottomFaceCnt = (BottomCurve.size()-1)*2;
		uiInnerFaceCnt = (InnerCurve.size()-1)*2;

		if(bNeedPlane)
		{
			fDiameter = max(( fRU- frU),( fRD- frD));
			fStart = min( frU, frD);
			vector3f vTmp = {0.f,0.f,-1.f};
			vector3f vNormal = vTmp;
			vector3f v1 = *PlaneVectors1[0];
			vector3f v2 = *PlaneVectors1[1];
			vector3f v3 = *PlaneVectors1[2];
			vector3f v4 = v1 - v2;
			 v1 = v3 - v2;
			float3CrossProduct(&vNormal,&v1,&v4);
			float3Normalize(&vNormal,&vNormal);

			float3 vNormal2 = {0.f,0.f,-1.f};
			v1 = *PlaneVectors2[0];
			v2 = *PlaneVectors2[1];
			v3 = *PlaneVectors2[2];
			v4 = v1 - v2;
			v1 = v3 - v2;
			float3CrossProduct(&vNormal2,&v1,&v4);
			float3Normalize(&vNormal2,&vNormal2);

			uiFaceCnt = uiCurPts-1;int i;
			for(i = 0; i < (int)uiFaceCnt; i++)
			{
				pIB[uiIBIndex] = uiVBIndex;
				uiIBIndex++;
				pIB[uiIBIndex] = uiVBIndex+i+2;
				uiIBIndex++;
				pIB[uiIBIndex] = uiVBIndex+i+1;
				uiIBIndex++;
			}
			for(i = 0; i < (int)PlaneVectors1.size(); i++)
			{
				pVB[uiVBIndex].vDyn = *PlaneVectors1[i];
				pVB[uiVBIndex].vNorm = -vNormal;
				v1 = pVB[uiVBIndex].vDyn;v1.y = 0.f;
				pVB[uiVBIndex].tu = (float)((float3Mode(&v1) - fStart)/fDiameter);
				pVB[uiVBIndex].tv = 0.5f - (float)(pVB[uiVBIndex].y/ fHeight);
				uiVBIndex++;
			}
			PlaneVectors1.clear();

			for(int i = 0; i < (int)uiFaceCnt; i++)
			{
				pIB[uiIBIndex] = uiVBIndex;
				uiIBIndex++;
				pIB[uiIBIndex] = uiVBIndex+i+1;
				uiIBIndex++;
				pIB[uiIBIndex] = uiVBIndex+i+2;
				uiIBIndex++;
			}
			for(i = 0; i < (int)PlaneVectors2.size(); i++)
			{
				pVB[uiVBIndex].vDyn = *PlaneVectors2[i];
				pVB[uiVBIndex].vNorm = vNormal2;
				v1 = pVB[uiVBIndex].vDyn;v1.y = 0.f;
				pVB[uiVBIndex].tu = (float)((float3Mode(&v1) - fStart)/fDiameter);
				pVB[uiVBIndex].tv = 0.5f - (float)(pVB[uiVBIndex].y/ fHeight);
				uiVBIndex++;
			}
			PlaneVectors2.clear();
		}
		TopCurve.clear();
		OuterCurve.clear();
		BottomCurve.clear();
		InnerCurve.clear();
	}
	else
	{
		UINT uiFactor = 16;
		UINT uiPlaneTri = 12;
		if(!bHaveUpInner || !bBevel)
		{
			uiFactor -= 2;
			uiPlaneTri -= 2;
		}
		if(!bHaveDownInner || !bBevel)
		{
			uiFactor -= 2;
			uiPlaneTri -= 2;
		}
		if(!bHaveUpOutter || !bBevel)
		{
			uiFactor -= 2;
			uiPlaneTri -= 2;
		}
		if(!bHaveDownOutter || !bBevel)
		{
			uiFactor -= 2;
			uiPlaneTri -= 2;
		}

		UINT uiFVNum = 0;//( m_ModelInfo.uSlices + 1)*2;

		pMod->vcount = ( uSlices + 1) * uiFactor+uiFVNum*8;
		pMod->vcount += uiFactor * ( uSlices-1);
		pMod->icount =  (uSlices * uiFactor + uiFVNum*8) * 3;

		if(bNeedPlane)
		{
			pMod->vcount += uiFactor;//bBevel ? 16 : 8;
			pMod->icount += uiPlaneTri * 3;//bBevel ? 12 : 4;  
		}

		pMod->v = new SYVertex3D[pMod->vcount]; 
		pMod->indecs = new DWORD[pMod->icount];

		SYVertex3D *pVB = pMod->v;
		DWORD	   *pIB = pMod->indecs;

	

		if(fTransitionHeight -  fHeight/2.f > ABS_FLOAT_0)
			fTransitionHeight =  fHeight/2.f;

		float t = fTransitionHeight/ fHeight;
		float t1 = 1.f-t;
		float fTranScale = ( frU > ABS_FLOAT_0) ? 0.5f : 1.f;
		float fMaxTransLen = fTranScale * (( fRU -  frU)*t1+t*( fRD -  frD));
		fTransitionLen *= fBevelScale;
		if(fTransitionLen > fMaxTransLen)
			fTransitionLen = fMaxTransLen/fBevelScale;
		float Y =  fHeight/2.f-fTransitionHeight; 
		float3 v[8], vTmp;
		if(bHaveUpInner)
		{	
			float3Set(&vTmp, t1* frU+t* frD,Y,0.f);
			v[0] = vTmp;
			v[1].x = v[0].x + fTransitionLen;v[1].y =  fHeight/2.f;v[1].z = 0.f;
		}
		else
		{
			float3Set(&vTmp,0.f, fHeight/2.f,0.f);
			v[1] = v[0] = vTmp;
		}
		float3Set(&vTmp,t1* frU+t* frD,Y,0.f);
		v[3] = vTmp; v[3].x = (t1* fRU+t* fRD);
		v[2].x = v[3].x - fTransitionLen;v[2].y =  fHeight/2.f;v[2].z = 0.f;

		float fTranScaleB = ( frD > ABS_FLOAT_0) ? 0.5f : 1.f;
		float fMaxTransLenB = fTranScaleB *(( fRD -  frD)*t1+t*( fRU -  frU));
		fTransitionLen *= fBevelScale;
		if(fTransitionLen > fMaxTransLenB)
			fTransitionLen = fMaxTransLenB/fBevelScale;
		float Y2 = - fHeight/2.f+fTransitionHeight; 
		float3Set(&vTmp,t1* fRD+t* fRU,Y2,0.f);
		v[4] = vTmp;
		v[5].x = v[4].x - fTransitionLen;v[5].y = - fHeight/2.f; v[5].z=0.f;
		if(bHaveDownInner)
		{
			v[7] = v[4]; v[7].x = t1* frD+t* frU;
			v[6] = v[5]; v[6].x = v[7].x + fTransitionLen;
		}
		else
		{
			float3Set(&vTmp,0.f,- fHeight/2.f,0.f);
			v[7] = v[6] = vTmp;
		}

		UINT uiFaceStart = 0;

		SYVertex3D vPt[2];
		float fDiameter =  fRU- frU;
		float fStart =  frU;
		//top
		fDiameter = 2 *  fRU;
		fStart = 0;
		vPt[0].vDyn = v[0];
		vPt[1].vDyn = v[1];
		UINT uiSegCnt = 0;
		if(bHaveUpInner && bBevel)
		{
			_CalcuLineNormal(&vPt[0].vNorm,&v[0],&v[1]);
			vPt[1].vNorm = vPt[0].vNorm;
			_CreateRotLineYShape(vPt,fDiameter,fStart,CYLTEXCTYPE_PLANE,2,FALSE,TRUE,
				fBeginAngle, fEndAngle,
				uSlices,
				pVB,uiVBIndex,pIB,uiIBIndex,
				PlaneVectors1, PlaneVectors2);

			uiSegCnt++;
		}
		//else
		//{
		//	vPt[0].normal = D3DXVECTOR3(-1.f,0.f,0.f);
		//	vPt[1].normal = D3DXVECTOR3(0.f,1.f,0.f);
		//}

		float3Set(&vTmp, 0.f,1.f,0.f);
		vPt[0].vDyn = v[1];vPt[0].vNorm = vTmp;
		vPt[1].vDyn = v[2];vPt[1].vNorm = vPt[0].vNorm;
		_CreateRotLineYShape(vPt,fDiameter,fStart,CYLTEXCTYPE_PLANE,2,FALSE,TRUE,
			fBeginAngle, fEndAngle,
			uSlices,
			pVB,uiVBIndex,pIB,uiIBIndex,
			PlaneVectors1, PlaneVectors2);
		uiSegCnt++;

		if(bBevel)
		{
			vPt[0].vDyn = v[2];_CalcuLineNormal(&vPt[0].vNorm,&v[2],&v[3]);
			vPt[1].vDyn = v[3];vPt[1].vNorm = vPt[0].vNorm;
			_CreateRotLineYShape(vPt,fDiameter,fStart,CYLTEXCTYPE_PLANE,2,FALSE,TRUE,
				fBeginAngle, fEndAngle,
				uSlices,
				pVB,uiVBIndex,pIB,uiIBIndex,
				PlaneVectors1, PlaneVectors2);
			uiSegCnt++;
		}

		uiTotalTopFaceCnt = uSlices*uiSegCnt*2;
		uiTopFaceCnt = uiSegCnt*2;

		//outer
		fDiameter =  fHeight - 2*fTransitionHeight;
		fStart = Y2;

		vPt[0].vDyn = v[3];_CalcuLineNormal(&vPt[0].vNorm,&v[3],&v[4]);
		vPt[1].vDyn = v[4];vPt[1].vNorm = vPt[0].vNorm;
		_CreateRotLineYShape(vPt,fDiameter,fStart,CYLTEXCTYPE_PRISM_PLANE,1,FALSE,TRUE,
			fBeginAngle, fEndAngle,
			uSlices,
			pVB,uiVBIndex,pIB,uiIBIndex,
			PlaneVectors1, PlaneVectors2);

		//for(int iii = uiFVNum; iii < 2*uiFVNum; iii++)
		//{
		//	pBevelTools->CreateNormalPrimitive(&pVB[iii],&pVB,&pIB,uiVBIndex,uiIBIndex);
		//}

		uiTotalOuterFaceCnt = uSlices*2 + uiFVNum *8;

		//bottom
		uiSegCnt = 0;
		fDiameter = 2* fRD;
		fStart = 0.f;

		if(bBevel)
		{
			vPt[0].vDyn = v[4];_CalcuLineNormal(&vPt[0].vNorm,&v[4],&v[5]);
			vPt[1].vDyn = v[5];vPt[1].vNorm =  vPt[0].vNorm;
			_CreateRotLineYShape(vPt,fDiameter,fStart,CYLTEXCTYPE_PLANE,2,FALSE,TRUE,
				fBeginAngle, fEndAngle,
				uSlices,
				pVB,uiVBIndex,pIB,uiIBIndex,
				PlaneVectors1, PlaneVectors2);
			uiSegCnt++;
		}

		float3Set(&vTmp, 0.f,-1.f,0.f);
		vPt[0].vDyn= v[5];vPt[0].vNorm = vTmp;
		vPt[1].vDyn= v[6];vPt[1].vNorm = vPt[0].vNorm;
		_CreateRotLineYShape(vPt,fDiameter,fStart,CYLTEXCTYPE_PLANE,2,FALSE,TRUE,
			fBeginAngle, fEndAngle,
			uSlices,
			pVB,uiVBIndex,pIB,uiIBIndex,
			PlaneVectors1, PlaneVectors2);
		uiSegCnt++;

		vPt[0].vDyn = v[6];
		vPt[1].vDyn = v[7];
		if(bHaveDownInner && bBevel)
		{
			_CalcuLineNormal(&vPt[0].vNorm,&v[6],&v[7]);
			vPt[1].vNorm =  vPt[0].vNorm;
			_CreateRotLineYShape(vPt,fDiameter,fStart,CYLTEXCTYPE_PLANE,2,FALSE,TRUE,
				fBeginAngle, fEndAngle,
				uSlices,
				pVB,uiVBIndex,pIB,uiIBIndex,
				PlaneVectors1, PlaneVectors2);
			uiSegCnt++;
		}
		//else
		//{
		//	vPt[0].normal = D3DXVECTOR3(0.f,-1.f,0.f);
		//	vPt[1].normal = D3DXVECTOR3(-1.f,0.f,0.f);
		//}
		uiTotalBottomFaceCnt =  uSlices*uiSegCnt*2;
		uiBottomFaceCnt = uiSegCnt*2;

		//Inner
		fDiameter =  fHeight - 2*fTransitionHeight;
		fStart = Y2;

		vPt[0].vDyn = v[7];_CalcuLineNormal(&vPt[0].vNorm,&v[7],&v[0]);
		vPt[1].vDyn = v[0];vPt[1].vNorm =  vPt[0].vNorm;
		_CreateRotLineYShape(vPt,fDiameter,fStart,CYLTEXCTYPE_PRISM_PLANE,1,FALSE,TRUE,
			fBeginAngle, fEndAngle,
			uSlices,
			pVB,uiVBIndex,pIB,uiIBIndex,
			PlaneVectors1, PlaneVectors2);

		uiTotalInnerFaceCnt =  uSlices*2;

		uiInnerFaceCnt = 2;
		uiOutFaceCnt = 2;


		if(bNeedPlane)
		{
			//Plane 0
			fDiameter = max(( fRU- frU),( fRD- frD));
			fStart = min( frU, frD);

			assert(PlaneVectors1.size() >= 4);
			assert(PlaneVectors2.size() >= 4);
			float3 vNormal = {0.f,0.f,-1.f};
			float3 v1, v2, v3, v4;
			v1 = *PlaneVectors1[0];
			v2 = *PlaneVectors1[1];
			v3 = *PlaneVectors1[2];
			v4 = v1 - v2;
			v1 = v3 - v2;
			float3CrossProduct(&vNormal,&v4,&v1);
			float3Normalize(&vNormal,&vNormal);

			float3 vNormal2 = {0.f,0.f,-1.f};
			v1 = *PlaneVectors2[0];
			v2 = *PlaneVectors2[1];
			v3 = *PlaneVectors2[2];
			v4 = v1 - v2;
			v1 = v3 - v2;
			float3CrossProduct(&vNormal2,&v1,&v4);
			float3Normalize(&vNormal2,&vNormal2);

			{
				uiFaceCnt = uiPlaneTri/2;
				UINT uiIndex[18] = {0,1,2,0,2,3,0,3,4,0,4,5,0,5,6,0,6,7};
				UINT uiIndexCnt = 0;int i;
				for(i = 0; i < (int)uiFaceCnt; i++)
				{
					pIB[uiIBIndex] = uiIndex[3*i]+uiVBIndex;
					uiIBIndex++;
					pIB[uiIBIndex] = uiIndex[3*i+2]+uiVBIndex;
					uiIBIndex++;
					pIB[uiIBIndex] = uiIndex[3*i+1]+uiVBIndex;
					uiIBIndex++;
					uiIndexCnt += 3;
				}
				for(i = 0; i < (int)PlaneVectors1.size(); i++)
				{
					pVB[uiVBIndex].vDyn = *PlaneVectors1[i];
					pVB[uiVBIndex].vNorm = vNormal;
					v1 = pVB[uiVBIndex].vNorm;
					v1.y = 0.f;
					pVB[uiVBIndex].tu = (float)((float3Mode(&v1) - fStart)/fDiameter);
					pVB[uiVBIndex].tv = 0.5f - (float)(pVB[uiVBIndex].y/ fHeight);
					uiVBIndex++;
				}
				PlaneVectors1.clear();

				for(int i = 0; i < (int)uiIndexCnt; i++)
				{
					pIB[uiIBIndex] = uiIndex[i]+uiVBIndex;
					uiIBIndex++;
				}
				for(i = 0; i < (int)PlaneVectors2.size(); i++)
				{
					pVB[uiVBIndex].vDyn = *PlaneVectors2[i];
					pVB[uiVBIndex].vNorm = vNormal2;
					v1 = pVB[uiVBIndex].vDyn;v1.y = 0.f;
					pVB[uiVBIndex].tu = (float)((float3Mode(&v1) - fStart)/fDiameter);
					pVB[uiVBIndex].tv = 0.5f - (float)(pVB[uiVBIndex].y/ fHeight);
					uiVBIndex++;
				}
			}
		}
	}

	UINT uiBottomFaceOffSet = uiTotalTopFaceCnt + uiTotalOuterFaceCnt;
	UINT uiInnerFaceOffset = uiTotalTopFaceCnt + uiTotalOuterFaceCnt+uiTotalBottomFaceCnt;
	UINT uiPlaneFaceOffset = uiTotalTopFaceCnt + uiTotalOuterFaceCnt +
		uiTotalBottomFaceCnt + uiTotalInnerFaceCnt;

	int nAttriId = 0;
	for(int i = 0; i < (int)uSlices; i++)
	{
		nAttriId = 4*i;

		pMod->viewsub[nAttriId].vstart= 0;
		pMod->viewsub[nAttriId].vcount =  pMod->vcount;
		pMod->viewsub[nAttriId].istart = i*uiTopFaceCnt*3;
		pMod->viewsub[nAttriId].icount = uiTopFaceCnt *3;
		pMod->viewsub[nAttriId].nTri_Type = SY_TRIANGLE_LIST ;
		pMod->viewsub[nAttriId].nTri_Num = uiTopFaceCnt ;
		pMod->viewsub[nAttriId].pMaterial = NULL;

		nAttriId++;
		pMod->viewsub[nAttriId].vstart= 0;
		pMod->viewsub[nAttriId].vcount =  pMod->vcount;
		pMod->viewsub[nAttriId].istart = (uiTotalTopFaceCnt + i*uiOutFaceCnt)*3;
		pMod->viewsub[nAttriId].icount =  uiOutFaceCnt*3;
		pMod->viewsub[nAttriId].nTri_Type = SY_TRIANGLE_LIST ;
		pMod->viewsub[nAttriId].nTri_Num = uiOutFaceCnt ;
		pMod->viewsub[nAttriId].pMaterial = NULL;
		
		nAttriId++;
		pMod->viewsub[nAttriId].vstart= 0;
		pMod->viewsub[nAttriId].vcount =  pMod->vcount;
		pMod->viewsub[nAttriId].istart = (uiBottomFaceOffSet+ i*uiBottomFaceCnt)*3;
		pMod->viewsub[nAttriId].icount =  uiBottomFaceCnt*3;
		pMod->viewsub[nAttriId].nTri_Type = SY_TRIANGLE_LIST ;
		pMod->viewsub[nAttriId].nTri_Num = uiBottomFaceCnt ;
		pMod->viewsub[nAttriId].pMaterial = NULL;
		
		nAttriId++;
		pMod->viewsub[nAttriId].vstart= 0;
		pMod->viewsub[nAttriId].vcount =  pMod->vcount;
		pMod->viewsub[nAttriId].istart = (uiInnerFaceOffset + i * uiInnerFaceCnt)*3;
		pMod->viewsub[nAttriId].icount =  uiInnerFaceCnt*3;
		pMod->viewsub[nAttriId].nTri_Type = SY_TRIANGLE_LIST ;
		pMod->viewsub[nAttriId].nTri_Num = uiInnerFaceCnt ;
		pMod->viewsub[nAttriId].pMaterial = NULL;

	}

	if(bNeedPlane)
	{
		for(int i = (int)uiAttrCnt; i < (int)uiAttrCnt+2; i++)
		{
			pMod->viewsub[i].vstart= 0;
			pMod->viewsub[i].vcount =  pMod->vcount;
			pMod->viewsub[i].istart = (uiPlaneFaceOffset+i*uiFaceCnt)*3;
			pMod->viewsub[i].icount =  uiFaceCnt*3;
			pMod->viewsub[i].nTri_Type = SY_TRIANGLE_LIST ;
			pMod->viewsub[i].nTri_Num = uiFaceCnt ;
			pMod->viewsub[i].pMaterial = NULL;
		}
	}
	else
	{
		for(int i = (int)uiAttrCnt; i < (int)uiAttrCnt+2; i++)
		{	
			pMod->viewsub[i].vstart= 0;
			pMod->viewsub[i].vcount =  pMod->vcount;
			pMod->viewsub[i].istart = 0;
			pMod->viewsub[i].icount =  0;
			pMod->viewsub[i].nTri_Type = SY_TRIANGLE_LIST ;
			pMod->viewsub[i].nTri_Num = 0 ;
			pMod->viewsub[i].pMaterial = NULL;
		}
	}
	return TRUE;
}

//创建圆环
BOOL SYModel_CreateRing(SYModel* pMod, 
					   UINT uSlices, UINT uStacks, //精度，界面精度
					   float fRU,  float frU,//半径，截面半径
					  float fStartAng, float fEndAng )//起始角度，终止角度
{
	BOOL bNeedPlane = (fabs(fEndAng - fStartAng - FLOAT_PI*2) > ABS_FLOAT_0);

	pMod->vcount = (uSlices + 1) * (uStacks + 1); //+ 2 * (uStacks + 1 + 1);//两个截面
	pMod->icount = (uSlices * uStacks * 2) * 3 ;//+ 2 * (/*uStacks + 2*/uStacks * 3);

	if(bNeedPlane)
	{
		pMod->vcount += 2 * (uStacks + 1 + 1);
		pMod->icount += 2 * (/*uStacks + 2*/uStacks * 3);
	}
	pMod->v = new SYVertex3D[pMod->vcount]; 
	pMod->indecs = new DWORD[pMod->icount];

	SYVertex3D *pVB = pMod->v;
	DWORD *pIB = pMod->indecs;
	
	if(bNeedPlane)
	{
		pMod->nviewsub = 3;
		pMod->viewsub = new SYSubModel[3];//3个
	}
	else
	{
		pMod->nviewsub = 1;
		pMod->viewsub = new SYSubModel[1];//3个
	}
	

	//float fTh = FLOAT_PI * 2 / uSlices;//需要关于起始角度吗？？
	float fTh = (fEndAng - fStartAng)/ uSlices;
	float fPh = FLOAT_PI * 2 / uStacks;
	//float fThSum = FLOAT_PI;//endAngel
	float fThSum = fEndAng;
	float fPhSum = FLOAT_PI;

	vector3f* vPos = new vector3f[uStacks+1];	
	vector3f* vPoint = new vector3f[uStacks+1];	
	float2*  vTex = new float2[uStacks + 1];

	vector3f* vNors = new vector3f[uStacks+1];	
	vector3f* vMormal = new vector3f[uStacks+1];	

	float x,z;
	vector3f vTmp;
	for (UINT j=0;j<uStacks+1;j++)//截面vpos
	{
		x = frU * cosf(fPhSum);
		z = -frU * sinf(fPhSum);
		fPhSum -= fPh;
		float3Set(&vTmp, x,0,z);
		vPos[j] = vTmp;
		float3Normalize(&vNors[j],&vPos[j]);
		vPos[j].x += fRU;
		vTex[j].x = (x + frU) / (2 * frU);
		vTex[j].y = (z + frU) / (2 * frU);
	}

	int nIndex = 0;UINT i;
	for (i=0;i<uSlices+1;i++)//圆环
	{
		matrix4f matRot;
		//D3DXMatrixRotationZ(&matRot,fThSum);
		matrix4f_fromeuler_lyxz(&matRot, 0, 0, -fThSum);
		
		int k = 0;
		for (k = 0; k < uStacks+1; k ++)
		{
			vPoint[k] = vPos[k] * matRot;
			vMormal[k] = vNors[k] * matRot;
// 			D3DXVec3TransformCoordArray(vPoint,sizeof(D3DXVECTOR3),vPos,sizeof(D3DXVECTOR3),
// 				&matRot,uStacks+1);
			//float3MulMatQ
			//vPoint[k].x = vPos[k].x * matRot._11 + vPos[k].y * matRot._21 + vPos[k].z * matRot._31 + 1.f * matRot._41;
			//vPoint[k].y = vPos[k].x * matRot._12 + vPos[k].y * matRot._22 + vPos[k].z * matRot._32 + 1.f * matRot._42;
			//vPoint[k].z = vPos[k].x * matRot._13 + vPos[k].y * matRot._23 + vPos[k].z * matRot._33 + 1.f * matRot._43;
		}
		for (UINT j=0;j<uStacks+1;j++)
		{
			pVB[nIndex].vDyn = vPoint[j];
			float3Normalize(&pVB[nIndex].vNorm,&vMormal[j]);

			pVB[nIndex].tu = (float)i / uSlices;
			pVB[nIndex].tv = (float)j / uStacks;

			nIndex++;
		}
		fThSum -= fTh;
	}


	if(bNeedPlane)
	{
		vector3f vPosCenter = {fRU,0,0};
		float3 vPosNormal = {0,1,0}; 
		matrix4f matRot;
		matrix4f_fromeuler_lyxz(&matRot, 0, 0, -fStartAng);	

		int nStartIndex = nIndex;
		pVB[nIndex].vDyn = vPosCenter * matRot;
		float3NormalMulMat(&pVB[nIndex].vNorm,&vPosNormal,&matRot);
		pVB[nIndex].tu = 0.5;
		pVB[nIndex++].tv = 0.5;
		int minX = fRU - frU;
		for(int i = 0;i<(int)uStacks+1;i++)//截面
		{
			pVB[nIndex].vDyn = vPos[i] * matRot;
			pVB[nIndex].vNorm = pVB[nStartIndex].vNorm;
			pVB[nIndex].tu = vTex[i].x;
			pVB[nIndex].tv = vTex[i].y;
			nIndex++;
		}
		matrix4f_fromeuler_lyxz(&matRot,0,0,-fEndAng);

		nStartIndex = nIndex;
		pVB[nIndex].vDyn = vPosCenter * matRot;
		vPosNormal.y = -1;
		float3NormalMulMat(&pVB[nIndex].vNorm,&vPosNormal,&matRot);
		pVB[nIndex].tu = 0.5;
		pVB[nIndex++].tv = 0.5;
		for(int i = 0;i<(int)uStacks+1;i++)//截面,需要一个截面变成负的
		{
			pVB[nIndex].vDyn = vPos[i] * matRot;
			pVB[nIndex].vNorm = pVB[nStartIndex].vNorm;
			pVB[nIndex].tu = vTex[i].x;
			pVB[nIndex].tv = vTex[i].y;

			nIndex++;
		}
	}
	delete []vPos;
	vPos = NULL;
	delete []vPoint;
	vPoint = NULL;
	delete []vTex;
	vTex = NULL;
	delete []vNors;
	vNors = NULL;
	delete []vMormal;
	vMormal = NULL;
	nIndex = 0;
	int a,b,c,d;
	for (i=0;i<uSlices;i++)
	{
		for (UINT j=0;j<uStacks;j++)
		{
			a = i * (uStacks+1) + j;
			b = a + 1;
			c = a + (uStacks+1);
			d = c + 1;
			pIB[nIndex++] = a;
			pIB[nIndex++] = d;
			pIB[nIndex++] = b;
			pIB[nIndex++] = d;
			pIB[nIndex++] = a;
			pIB[nIndex++] = c;
		}
	}
	

	if(bNeedPlane)
	{
		int nCircleIndex = (uSlices + 1) * (uStacks + 1);
		for(int j = 0;j<uStacks;j++)
		{
			pIB[nIndex++] = nCircleIndex;
			pIB[nIndex++] = nCircleIndex+ j + 2;
			pIB[nIndex++] = nCircleIndex+ j + 1;
		}
		nCircleIndex += uStacks + 2;

		for(int j = 0;j<uStacks;j++)
		{
			pIB[nIndex++] = nCircleIndex;
			pIB[nIndex++] = nCircleIndex+ j + 1;
			pIB[nIndex++] = nCircleIndex+ j + 2;
		}
		nCircleIndex += uStacks + 2;
	}

	pMod->viewsub[0].vstart= 0;
	pMod->viewsub[0].vcount =  pMod->vcount;
	pMod->viewsub[0].istart = 0;
	pMod->viewsub[0].icount = (uSlices * uStacks * 2) * 3/* pMod->icount*/;
	pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[0].nTri_Num = pMod->viewsub[0].icount/* pMod->icount*/ / 3;
	pMod->viewsub[0].pMaterial = NULL;

	if(bNeedPlane)
	{
		pMod->viewsub[1].vstart= 0;
		pMod->viewsub[1].vcount =  pMod->vcount;
		pMod->viewsub[1].istart = pMod->viewsub[0].icount;
		pMod->viewsub[1].icount =  uStacks *3;
		pMod->viewsub[1].nTri_Type = SY_TRIANGLE_LIST ;
		pMod->viewsub[1].nTri_Num = uStacks;
		pMod->viewsub[1].pMaterial = NULL;

		pMod->viewsub[2].vstart= 0;
		pMod->viewsub[2].vcount =  pMod->vcount;
		pMod->viewsub[2].istart = pMod->viewsub[1].icount + pMod->viewsub[0].icount;
		pMod->viewsub[2].icount =  uStacks *3;
		pMod->viewsub[2].nTri_Type = SY_TRIANGLE_LIST ;
		pMod->viewsub[2].nTri_Num = uStacks;
		pMod->viewsub[2].pMaterial = NULL;
	}
	
	return TRUE;
}

//柱图
BOOL  SYModel_CreateBar(SYModel* pMod,UINT uSharpType,float fRadius,UINT uFragMent,BOOL bText)
{
	UINT nV = 0;
	UINT nF = 0;

	if(bText)
	{
		pMod->vcount = 8;//32;
		pMod->icount = 12;//16*3;
		pMod->v = new SYVertex3D[pMod->vcount];
		pMod->indecs = new DWORD[pMod->icount];
		pMod->nviewsub = 1;
		pMod->viewsub = new SYSubModel[1];

		pMod->v[0].x = -0.5f*fRadius;
		pMod->v[0].y = 0.5f*fRadius+fRadius;
		pMod->v[0].z = 0.0f*fRadius;
		pMod->v[0].nx = 0.0f;
		pMod->v[0].ny = 0.0f;
		pMod->v[0].nz = -1.0f;
		pMod->v[0].tu = 0.0f;
		pMod->v[0].tv = 0.0f;//pVB[24] = MODELVERTEX(-0.5f,	0.5f,	0.0f,	0.0f,	0.0f,	-1.0f,	0.0f,	0.0f);

		pMod->v[1].x = 0.5f*fRadius;
		pMod->v[1].y = 0.5f*fRadius+fRadius;
		pMod->v[1].z = 0.0f*fRadius;
		pMod->v[1].nx = 0.0f;
		pMod->v[1].ny = 0.0f;
		pMod->v[1].nz = -1.0f;
		pMod->v[1].tu = 1.0f;
		pMod->v[1].tv = 0.0f;//pVB[25] = MODELVERTEX(0.5f,	0.5f,	0.0f,	0.0f,	0.0f,	-1.0f,	1.0f,	0.0f);

		pMod->v[2].x = 0.5f*fRadius;
		pMod->v[2].y = -0.5f*fRadius+fRadius;
		pMod->v[2].z = 0.0f*fRadius;
		pMod->v[2].nx = 0.0f;
		pMod->v[2].ny = 0.0f;
		pMod->v[2].nz = -1.0f;
		pMod->v[2].tu = 1.0f;
		pMod->v[2].tv = 1.0f;//pVB[26] = MODELVERTEX(0.5f,	-0.5f,	0.0f,	0.0f,	0.0f,	-1.0f,	1.0f,	1.0f);

		pMod->v[3].x = -0.5f*fRadius;
		pMod->v[3].y = -0.5f*fRadius+fRadius;
		pMod->v[3].z = 0.0f*fRadius;
		pMod->v[3].nx = 0.0f;
		pMod->v[3].ny = 0.0f;
		pMod->v[3].nz = -1.0f;
		pMod->v[3].tu = 0.0f;
		pMod->v[3].tv = 1.0f;//pVB[27] = MODELVERTEX(-0.5f,	-0.5f,	0.0f,	0.0f,	0.0f,	-1.0f,	0.0f,	1.0f);

		pMod->v[4].x = -0.5f*fRadius;
		pMod->v[4].y = 0.5f*fRadius+fRadius;
		pMod->v[4].z = 0.0f*fRadius;
		pMod->v[4].nx = 0.0f;
		pMod->v[4].ny = 0.0f;
		pMod->v[4].nz = 1.0f;
		pMod->v[4].tu = 0.0f;
		pMod->v[4].tv = 0.0f;//pVB[28] = MODELVERTEX(-0.5f,	0.5f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,	0.0f);

		pMod->v[5].x = 0.5f*fRadius;
		pMod->v[5].y = 0.5f*fRadius+fRadius;
		pMod->v[5].z = 0.0f*fRadius;
		pMod->v[5].nx = 0.0f;
		pMod->v[5].ny = 0.0f;
		pMod->v[5].nz = 1.0f;
		pMod->v[5].tu = 1.0f;
		pMod->v[5].tv = 0.0f;//pVB[29] = MODELVERTEX(0.5f,	0.5f,	0.0f,	0.0f,	0.0f,	1.0f,	1.0f,	0.0f);

		pMod->v[6].x = 0.5f*fRadius;
		pMod->v[6].y = -0.5f*fRadius+fRadius;
		pMod->v[6].z = 0.0f*fRadius;
		pMod->v[6].nx = 0.0f;
		pMod->v[6].ny = 0.0f;
		pMod->v[6].nz = 1.0f;
		pMod->v[6].tu = 1.0f;
		pMod->v[6].tv = 1.0f;//pVB[30] = MODELVERTEX(0.5f,	-0.5f,	0.0f,	0.0f,	0.0f,	1.0f,	1.0f,	1.0f);

		pMod->v[7].x = -0.5f*fRadius;
		pMod->v[7].y = -0.5f*fRadius+fRadius;
		pMod->v[7].z = 0.0f*fRadius;
		pMod->v[7].nx = 0.0f;
		pMod->v[7].ny = 0.0f;
		pMod->v[7].nz = 1.0f;
		pMod->v[7].tu = 0.0f;
		pMod->v[7].tv = 1.0f;//pVB[31] = MODELVERTEX(-0.5f,	-0.5f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,	1.0f);
		pMod->indecs[0] = 0;	pMod->indecs[1] = 1;	pMod->indecs[2] = 2;
		pMod->indecs[3] = 2;	pMod->indecs[4] = 3;	pMod->indecs[5] = 0;
		pMod->indecs[6] = 4;	pMod->indecs[7] = 5;	pMod->indecs[8] = 5;
		pMod->indecs[9] = 5;	pMod->indecs[10] = 4;	pMod->indecs[11] = 7;

		pMod->viewsub[0].vstart= 0;
		pMod->viewsub[0].vcount =  pMod->vcount;
		pMod->viewsub[0].istart = 0;//nFMesh*3;
		pMod->viewsub[0].icount = 12;
		pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST ;
		pMod->viewsub[0].nTri_Num = 4;//4;
		pMod->viewsub[0].pMaterial = NULL;	

	}

	if(uSharpType == 0) //Circle
	{
		if(bText)
		{
			
		}
		else
		{
			pMod->vcount = (uFragMent +1) * 4 + 2;
			pMod->icount = (uFragMent * 4 )*3;
			pMod->v = new SYVertex3D[pMod->vcount];
			pMod->indecs = new DWORD[pMod->icount];
			pMod->nviewsub = 1;
			pMod->viewsub = new SYSubModel[1];

			float fAngle = (float)PAI * 2.0f / (float)uFragMent; 
			float fAngleSum = 0;
			float x,z,nx,nz,tu,tv,tu1;

			int nIdx = 0;
			

			for (UINT j=0;j<=uFragMent;j++)
			{
				nx = sinf(fAngleSum);
				nz = cosf(fAngleSum);
				fAngleSum += fAngle;
				x = nx * 0.5f;
				z = nz * 0.5f;
				tu = x + 0.5f;
				tv = 0.5f - z;
				tu1 = 1.f - fAngleSum / ((float)PAI * 2.0f);
				nIdx = j;
				pMod->v[nIdx].x = x*fRadius;
				pMod->v[nIdx].y = 0.5f;
				pMod->v[nIdx].z = z*fRadius;
				pMod->v[nIdx].nx = 0.0f;
				pMod->v[nIdx].ny = 1.0f;
				pMod->v[nIdx].nz = 0.0f;
				pMod->v[nIdx].tu = tu;
				pMod->v[nIdx].tv = tv;

				nIdx += (uFragMent + 1);
				pMod->v[nIdx].x = x*fRadius;
				pMod->v[nIdx].y = 0.5f;
				pMod->v[nIdx].z = z*fRadius;
				pMod->v[nIdx].nx = nx;
				pMod->v[nIdx].ny = 0.0f;
				pMod->v[nIdx].nz = nz;
				pMod->v[nIdx].tu = tu1;
				pMod->v[nIdx].tv = 0.0f;

				nIdx += (uFragMent + 1);
				pMod->v[nIdx].x = x*fRadius;
				pMod->v[nIdx].y = -0.5f;//
				pMod->v[nIdx].z = z*fRadius;
				pMod->v[nIdx].nx = nx;
				pMod->v[nIdx].ny = 0.0f;
				pMod->v[nIdx].nz = nz;
				pMod->v[nIdx].tu = tu1;
				pMod->v[nIdx].tv = 1.0f;

				nIdx += (uFragMent + 1);
				pMod->v[nIdx].x = x*fRadius;
				pMod->v[nIdx].y = -0.5f;//
				pMod->v[nIdx].z = z*fRadius;
				pMod->v[nIdx].nx = 0.0f;
				pMod->v[nIdx].ny = -1.0f;
				pMod->v[nIdx].nz = 0.0f;
				pMod->v[nIdx].tu = tu;
				pMod->v[nIdx].tv = tv;
			}
			++nIdx;
			pMod->v[nIdx].x = 0.0f*fRadius;
			pMod->v[nIdx].y = 0.5f;
			pMod->v[nIdx].z = 0.0f*fRadius;
			pMod->v[nIdx].nx = 0.0f;
			pMod->v[nIdx].ny = 1.0f;
			pMod->v[nIdx].nz = 0.0f;
			pMod->v[nIdx].tu = 0.5f;
			pMod->v[nIdx].tv = 0.5f;

			++nIdx;
			pMod->v[nIdx].x = 0.0f*fRadius;
			pMod->v[nIdx].y = -0.5f;
			pMod->v[nIdx].z = 0.0f*fRadius;
			pMod->v[nIdx].nx = 0.0f;
			pMod->v[nIdx].ny = -1.0f;
			pMod->v[nIdx].nz = 0.0f;
			pMod->v[nIdx].tu = 0.5f;
			pMod->v[nIdx].tv = 0.5f;

			UINT nPart0 = 0;
			UINT nCount0 = uFragMent;
			int j=0,nIndex =0;
			for (j=0;j<nCount0;j++)
			{
				pMod->indecs[nIndex] = nIdx-1;	
				++nIndex;
				pMod->indecs[nIndex] = j ;
				++nIndex;
				pMod->indecs[nIndex] = j + 1;
				++nIndex;
			}

			UINT a,b,c,d;
			for (j=nCount0+1;j<nCount0+nCount0+1;j++)
			{
				a= j;
				b =j+1;
				c = j+nCount0+2;
				d= c-1;
				pMod->indecs[nIndex] = a;	
				++nIndex;
				pMod->indecs[nIndex] = c ;
				++nIndex;
				pMod->indecs[nIndex] = b;
				++nIndex;
				pMod->indecs[nIndex] = a;	
				++nIndex;
				pMod->indecs[nIndex] = d ;
				++nIndex;
				pMod->indecs[nIndex] = c;
				++nIndex;
			}

			for (j=nCount0+nCount0+nCount0+3;j<nCount0+nCount0+nCount0+nCount0+3;j++)
			{
				pMod->indecs[nIndex] = nIdx;	
				++nIndex;
				pMod->indecs[nIndex] = j+1 ;
				++nIndex;
				pMod->indecs[nIndex] = j;
				++nIndex;
			}
			
			UINT nVMesh = pMod->vcount;
			UINT nFMesh = pMod->icount/3;

			
				pMod->viewsub[0].vstart= 0;
				pMod->viewsub[0].vcount =  pMod->vcount;
				pMod->viewsub[0].istart =0;
				pMod->viewsub[0].icount = pMod->icount;
				pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST ;
				pMod->viewsub[0].nTri_Num = nFMesh;
				pMod->viewsub[0].pMaterial = NULL;
			
		}
	}
	else if(uSharpType == 1) //Rectangle
	{
		if(bText)
		{
			
		}
		else
		{
			pMod->vcount = 24;//32;
			pMod->icount = 36;//16*3;
			pMod->v = new SYVertex3D[pMod->vcount];
			pMod->indecs = new DWORD[pMod->icount];
			pMod->nviewsub = 1;
			pMod->viewsub = new SYSubModel[1];

			pMod->v[0].x = 0.5f*fRadius;
			pMod->v[0].y = 0.5f;
			pMod->v[0].z = 0.5f*fRadius;
			pMod->v[0].nx = 0.0f;
			pMod->v[0].ny = 1.0f;
			pMod->v[0].nz = 0.0f;
			pMod->v[0].tu = 1.0f;
			pMod->v[0].tv = 0.0f;

			pMod->v[1].x = 0.5f*fRadius;
			pMod->v[1].y = 0.5f;
			pMod->v[1].z = -0.5f*fRadius;
			pMod->v[1].nx = 0.0f;
			pMod->v[1].ny = 1.0f;
			pMod->v[1].nz = 0.0f;
			pMod->v[1].tu = 1.0f;
			pMod->v[1].tv = 1.0f;//pVB[1]  = MODELVERTEX(0.5f,	0.5f,	-0.5f,	0.0f,	1.0f,	0.0f,	1.0f,	1.0f);

			pMod->v[2].x = -0.5f*fRadius;
			pMod->v[2].y = 0.5f;
			pMod->v[2].z = -0.5f*fRadius;
			pMod->v[2].nx = 0.0f;
			pMod->v[2].ny = 1.0f;
			pMod->v[2].nz = 0.0f;
			pMod->v[2].tu = 0.0f;
			pMod->v[2].tv = 1.0f;//pVB[2]  = MODELVERTEX(-0.5f,	0.5f,	-0.5f,	0.0f,	1.0f,	0.0f,	0.0f,	1.0f);

			pMod->v[3].x = -0.5f*fRadius;
			pMod->v[3].y = 0.5f;
			pMod->v[3].z = 0.5f*fRadius;
			pMod->v[3].nx = 0.0f;
			pMod->v[3].ny = 1.0f;
			pMod->v[3].nz = 0.0f;
			pMod->v[3].tu = 0.0f;
			pMod->v[3].tv = 0.0f;//pVB[3]  = MODELVERTEX(-0.5f,	0.5f,	0.5f,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f);

			pMod->v[4].x = 0.5f*fRadius;
			pMod->v[4].y = 0.5f;
			pMod->v[4].z = -0.5f*fRadius;
			pMod->v[4].nx = 0.0f;
			pMod->v[4].ny = 0.0f;
			pMod->v[4].nz = -1.0f;
			pMod->v[4].tu = 1.0f;
			pMod->v[4].tv = 0.0f;//pVB[4]  = MODELVERTEX(0.5f,	0.5f,	-0.5f,	0.0f,	0.0f,	-1.0f,	1.0f,	0.0f);

			pMod->v[5].x = 0.5f*fRadius;
			pMod->v[5].y = -0.5f;
			pMod->v[5].z = -0.5f*fRadius;
			pMod->v[5].nx = 0.0f;
			pMod->v[5].ny = 0.0f;
			pMod->v[5].nz = -1.0f;
			pMod->v[5].tu = 1.0f;
			pMod->v[5].tv = 1.0f;//pVB[5]  = MODELVERTEX(0.5f,	0.0f,	-0.5f,	0.0f,	0.0f,	-1.0f,	1.0f,	1.0f);

			pMod->v[6].x = -0.5f*fRadius;
			pMod->v[6].y = -0.5f;
			pMod->v[6].z = -0.5f*fRadius;
			pMod->v[6].nx = 0.0f;
			pMod->v[6].ny = 0.0f;
			pMod->v[6].nz = -1.0f;
			pMod->v[6].tu = 0.0f;
			pMod->v[6].tv = 1.0f;//pVB[6]  = MODELVERTEX(-0.5f,	0.0f,	-0.5f,	0.0f,	0.0f,	-1.0f,	0.0f,	1.0f);

			pMod->v[7].x = -0.5f*fRadius;
			pMod->v[7].y = 0.5f;
			pMod->v[7].z = -0.5f*fRadius;
			pMod->v[7].nx = 0.0f;
			pMod->v[7].ny = 0.0f;
			pMod->v[7].nz = -1.0f;
			pMod->v[7].tu = 0.0f;
			pMod->v[7].tv = 0.0f;//pVB[7]  = MODELVERTEX(-0.5f,	0.5f,	-0.5f,	0.0f,	0.0f,	-1.0f,	0.0f,	0.0f);

			pMod->v[8].x = 0.5f*fRadius;
			pMod->v[8].y = 0.5f;
			pMod->v[8].z = 0.5f*fRadius;
			pMod->v[8].nx = 0.0f;
			pMod->v[8].ny = 0.0f;
			pMod->v[8].nz = 1.0f;
			pMod->v[8].tu = 0.0f;
			pMod->v[8].tv = 0.0f;//pVB[8]  = MODELVERTEX(0.5f,	0.5f,	0.5f,	0.0f,	0.0f,	1.0f,	0.0f,	0.0f);

			pMod->v[9].x = -0.5f*fRadius;
			pMod->v[9].y = 0.5f;
			pMod->v[9].z = 0.5f*fRadius;
			pMod->v[9].nx = 0.0f;
			pMod->v[9].ny = 0.0f;
			pMod->v[9].nz = 1.0f;
			pMod->v[9].tu = 1.0f;
			pMod->v[9].tv = 0.0f;//pVB[9]  = MODELVERTEX(-0.5f,	0.5f,	0.5f,	0.0f,	0.0f,	1.0f,	1.0f,	0.0f);

			pMod->v[10].x = -0.5f*fRadius;
			pMod->v[10].y = -0.5f;
			pMod->v[10].z = 0.5f*fRadius;
			pMod->v[10].nx = 0.0f;
			pMod->v[10].ny = 0.0f;
			pMod->v[10].nz = 1.0f;
			pMod->v[10].tu = 1.0f;
			pMod->v[10].tv = 1.0f;//pVB[10] = MODELVERTEX(-0.5f,	0.0f,	0.5f,	0.0f,	0.0f,	1.0f,	1.0f,	1.0f);

			pMod->v[11].x = 0.5f*fRadius;
			pMod->v[11].y = -0.5f;
			pMod->v[11].z = 0.5f*fRadius;
			pMod->v[11].nx = 0.0f;
			pMod->v[11].ny = 0.0f;
			pMod->v[11].nz = 1.0f;
			pMod->v[11].tu = 0.0f;
			pMod->v[11].tv = 1.0f;//pVB[11] = MODELVERTEX(0.5f,	0.0f,	0.5f,	0.0f,	0.0f,	1.0f,	0.0f,	1.0f);

			pMod->v[12].x = 0.5f*fRadius;
			pMod->v[12].y = -0.5f;
			pMod->v[12].z = 0.5f*fRadius;
			pMod->v[12].nx = 1.0f;
			pMod->v[12].ny = 0.0f;
			pMod->v[12].nz = 0.0f;
			pMod->v[12].tu = 1.0f;
			pMod->v[12].tv = 1.0f;//pVB[12] = MODELVERTEX(0.5f,	0.0f,	0.5f,	1.0f,	0.0f,	0.0f,	1.0f,	1.0f);

			pMod->v[13].x = 0.5f*fRadius;
			pMod->v[13].y = -0.5f;
			pMod->v[13].z = -0.5f*fRadius;
			pMod->v[13].nx = 1.0f;
			pMod->v[13].ny = 0.0f;
			pMod->v[13].nz = 0.0f;
			pMod->v[13].tu = 0.0f;
			pMod->v[13].tv = 1.0f;//pVB[13] = MODELVERTEX(0.5f,	0.0f,	-0.5f,	1.0f,	0.0f,	0.0f,	0.0f,	1.0f);

			pMod->v[14].x = 0.5f*fRadius;
			pMod->v[14].y = 0.5f;
			pMod->v[14].z = -0.5f*fRadius;
			pMod->v[14].nx = 1.0f;
			pMod->v[14].ny = 0.0f;
			pMod->v[14].nz = 0.0f;
			pMod->v[14].tu = 0.0f;
			pMod->v[14].tv = 0.0f;//pVB[14] = MODELVERTEX(0.5f,	0.5f,	-0.5f,	1.0f,	0.0f,	0.0f,	0.0f,	0.0f);

			pMod->v[15].x = 0.5f*fRadius;
			pMod->v[15].y = 0.5f;
			pMod->v[15].z = 0.5f*fRadius;
			pMod->v[15].nx = 1.0f;
			pMod->v[15].ny = 0.0f;
			pMod->v[15].nz = 0.0f;
			pMod->v[15].tu = 1.0f;
			pMod->v[15].tv = 0.0f;//pVB[15] = MODELVERTEX(0.5f,	0.5f,	0.5f,	1.0f,	0.0f,	0.0f,	1.0f,	0.0f);

			pMod->v[16].x = -0.5f*fRadius;
			pMod->v[16].y = 0.5f;
			pMod->v[16].z = 0.5f*fRadius;
			pMod->v[16].nx = -1.0f;
			pMod->v[16].ny = 0.0f;
			pMod->v[16].nz = 0.0f;
			pMod->v[16].tu = 0.0f;
			pMod->v[16].tv = 0.0f;//pVB[16] = MODELVERTEX(-0.5f,	0.5f,	0.5f,	-1.0f,	0.0f,	0.0f,	0.0f,	0.0f);

			pMod->v[17].x = -0.5f*fRadius;
			pMod->v[17].y = 0.5f;
			pMod->v[17].z = -0.5f*fRadius;
			pMod->v[17].nx = -1.0f;
			pMod->v[17].ny = 0.0f;
			pMod->v[17].nz = 0.0f;
			pMod->v[17].tu = 1.0f;
			pMod->v[17].tv = 0.0f;//pVB[17] = MODELVERTEX(-0.5f,	0.5f,	-0.5f,	-1.0f,	0.0f,	0.0f,	1.0f,	0.0f);

			pMod->v[18].x = -0.5f*fRadius;
			pMod->v[18].y = -0.5f;
			pMod->v[18].z = -0.5f*fRadius;
			pMod->v[18].nx = -1.0f;
			pMod->v[18].ny = 0.0f;
			pMod->v[18].nz = 0.0f;
			pMod->v[18].tu = 1.0f;
			pMod->v[18].tv = 1.0f;//pVB[18] = MODELVERTEX(-0.5f,	0.0f,	-0.5f,	-1.0f,	0.0f,	0.0f,	1.0f,	1.0f);

			pMod->v[19].x = -0.5f*fRadius;
			pMod->v[19].y = -0.5f;
			pMod->v[19].z = 0.5f*fRadius;
			pMod->v[19].nx = -1.0f;
			pMod->v[19].ny = 0.0f;
			pMod->v[19].nz = 0.0f;
			pMod->v[19].tu = 0.0f;
			pMod->v[19].tv = 1.0f;//pVB[19] = MODELVERTEX(-0.5f,	0.0f,	0.5f,	-1.0f,	0.0f,	0.0f,	0.0f,	1.0f);	

			pMod->v[20].x = 0.5f*fRadius;
			pMod->v[20].y = -0.5f;
			pMod->v[20].z = -0.5f*fRadius;
			pMod->v[20].nx = 0.0f;
			pMod->v[20].ny = -1.0f;
			pMod->v[20].nz = 0.0f;
			pMod->v[20].tu = 1.0f;
			pMod->v[20].tv = 0.0f;//pVB[20] = MODELVERTEX(0.5f,	0.0f,	-0.5f,	0.0f,	-1.0f,	0.0f,	1.0f,	0.0f);

			pMod->v[21].x = 0.5f*fRadius;
			pMod->v[21].y = -0.5f;
			pMod->v[21].z = 0.5f*fRadius;
			pMod->v[21].nx = 0.0f;
			pMod->v[21].ny = -1.0f;
			pMod->v[21].nz = 0.0f;
			pMod->v[21].tu = 1.0f;
			pMod->v[21].tv = 1.0f;//pVB[21] = MODELVERTEX(0.5f,	0.0f,	0.5f,	0.0f,	-1.0f,	0.0f,	1.0f,	1.0f);

			pMod->v[22].x = -0.5f*fRadius;
			pMod->v[22].y = -0.5f;
			pMod->v[22].z = 0.5f*fRadius;
			pMod->v[22].nx = 0.0f;
			pMod->v[22].ny = -1.0f;
			pMod->v[22].nz = 0.0f;
			pMod->v[22].tu = 0.0f;
			pMod->v[22].tv = 1.0f;//pVB[22] = MODELVERTEX(-0.5f,	0.0f,	0.5f,	0.0f,	-1.0f,	0.0f,	0.0f,	1.0f);

			pMod->v[23].x = -0.5f*fRadius;
			pMod->v[23].y = -0.5f;
			pMod->v[23].z = -0.5f*fRadius;
			pMod->v[23].nx = 0.0f;
			pMod->v[23].ny = -1.0f;
			pMod->v[23].nz = 0.0f;
			pMod->v[23].tu = 0.0f;
			pMod->v[23].tv = 0.0f;//pVB[23] = MODELVERTEX(-0.5f,	0.0f,	-0.5f,	0.0f,	-1.0f,	0.0f,	0.0f,	0.0f);

			pMod->indecs[0] = 0;		pMod->indecs[1] = 1;		pMod->indecs[2] = 2;
			pMod->indecs[3] = 2;		pMod->indecs[4] = 3;		pMod->indecs[5] = 0;

			pMod->indecs[6] = 4;		pMod->indecs[7] = 5;		pMod->indecs[8] = 6;
			pMod->indecs[9] = 6;		pMod->indecs[10] = 7;	pMod->indecs[11] = 4;
			pMod->indecs[12] = 8;	pMod->indecs[13] = 9;	pMod->indecs[14] = 10;
			pMod->indecs[15] = 10;	pMod->indecs[16] = 11;	pMod->indecs[17] = 8;
			pMod->indecs[18] = 12;	pMod->indecs[19] = 13;	pMod->indecs[20] = 14;
			pMod->indecs[21] = 14;	pMod->indecs[22] = 15;	pMod->indecs[23] = 12;
			pMod->indecs[24] = 16;	pMod->indecs[25] = 17;	pMod->indecs[26] = 18;
			pMod->indecs[27] = 18;	pMod->indecs[28] = 19;	pMod->indecs[29] = 16;

			pMod->indecs[30] = 20;	pMod->indecs[31] = 21;	pMod->indecs[32] = 22;
			pMod->indecs[33] = 22;	pMod->indecs[34] = 23;	pMod->indecs[35] = 20;
		}
		
		////////////////////////////////////////////////////////////////////////////
		UINT nVMesh = pMod->vcount - 8;
		UINT nFMesh = pMod->icount/3 - 4;

		if (!bText)
		{
			pMod->viewsub[0].vstart= 0;
			pMod->viewsub[0].vcount =  pMod->vcount;
			pMod->viewsub[0].istart = 0;//nFMesh*3;
			pMod->viewsub[0].icount = 36;
			pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST ;
			pMod->viewsub[0].nTri_Num = 12;//4;
			pMod->viewsub[0].pMaterial = NULL;
			/*pMod->viewsub[0].vstart= 0;
			pMod->viewsub[0].vcount =  pMod->vcount;
			pMod->viewsub[0].istart = 0*3;
			pMod->viewsub[0].icount = nFMesh*3;
			pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST ;
			pMod->viewsub[0].nTri_Num = nFMesh;
			pMod->viewsub[0].pMaterial = NULL;*/
		}
		
	}
	return TRUE;
}

//翻牌
void MODELVERTEX(SYVertex3D& V,float x,float y,float z,float nx,float ny,float nz,float tu,float tv)
{
	V.x = x;
	V.y = y;
	V.z = z;
	V.nx = nx;
	V.ny = ny;
	V.nz = nz;
	V.tu = tu;
	V.tv = tv;
};


BOOL  SYModel_CreateTablet_TwoFace(SYModel* pMod,
								   float fWidth,
								   float fHeight,
								   float fThick,
								   int	 nCol,
								   int	 nRow,
								   int	 col_count,
								   int	 row_count,
								   BOOL bSharedTex)
{
	int nSubset = 3;

	pMod->vcount = 24;
	pMod->icount = 36;
	pMod->v = new SYVertex3D[pMod->vcount];
	pMod->indecs = new DWORD[pMod->icount];

	pMod->nviewsub = nSubset;
	pMod->viewsub = new SYSubModel[pMod->nviewsub];

	//bottom
	MODELVERTEX(pMod->v[0],0.5f*fWidth,	-0.5f*fHeight,	-0.5f*fThick,	0.0f,	-1.0f,	0.0f,	1.0f,	0.0f);
	MODELVERTEX(pMod->v[1],0.5f*fWidth,	-0.5f*fHeight,	0.5f*fThick,	0.0f,	-1.0f,	0.0f,	1.0f,	1.0f);
	MODELVERTEX(pMod->v[2],-0.5f*fWidth,	-0.5f*fHeight,	0.5f*fThick,	0.0f,	-1.0f,	0.0f,	0.0f,	1.0f);
	MODELVERTEX(pMod->v[3],-0.5f*fWidth,	-0.5f*fHeight,	-0.5f*fThick,	0.0f,	-1.0f,	0.0f,	0.0f,	0.0f);

	//right
	MODELVERTEX(pMod->v[4],0.5f*fWidth,	-0.5f*fHeight,	0.5f*fThick,	1.0f,	0.0f,	0.0f,	1.0f,	0.0f);
	MODELVERTEX(pMod->v[5],0.5f*fWidth,	-0.5f*fHeight,	-0.5f*fThick,	1.0f,	0.0f,	0.0f,	1.0f,	1.0f);
	MODELVERTEX(pMod->v[6],0.5f*fWidth,	0.5f*fHeight,	-0.5f*fThick,	1.0f,	0.0f,	0.0f,	0.0f,	1.0f);
	MODELVERTEX(pMod->v[7],0.5f*fWidth,	0.5f*fHeight,	0.5f*fThick,	1.0f,	0.0f,	0.0f,	0.0f,	0.0f);

	//top
	MODELVERTEX(pMod->v[8],0.5f*fWidth,	0.5f*fHeight,	0.5f*fThick,	0.0f,	1.0f,	0.0f,	1.0f,	0.0f);
	MODELVERTEX(pMod->v[9],0.5f*fWidth,	0.5f*fHeight,	-0.5f*fThick,	0.0f,	1.0f,	0.0f,	1.0f,	1.0f);
	MODELVERTEX(pMod->v[10],-0.5f*fWidth,	0.5f*fHeight,	-0.5f*fThick,	0.0f,	1.0f,	0.0f,	0.0f,	1.0f);
	MODELVERTEX(pMod->v[11],-0.5f*fWidth,	0.5f*fHeight,	0.5f*fThick,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f);

	//left
	MODELVERTEX(pMod->v[12],-0.5f*fWidth,	0.5f*fHeight,	0.5f*fThick,	-1.0f,	0.0f,	0.0f,	1.0f,	0.0f);
	MODELVERTEX(pMod->v[13],-0.5f*fWidth,	0.5f*fHeight,	-0.5f*fThick,	-1.0f,	0.0f,	0.0f,	1.0f,	1.0f);
	MODELVERTEX(pMod->v[14],-0.5f*fWidth,	-0.5f*fHeight,	-0.5f*fThick,	-1.0f,	0.0f,	0.0f,	0.0f,	1.0f);
	MODELVERTEX(pMod->v[15],-0.5f*fWidth,	-0.5f*fHeight,	0.5f*fThick,	-1.0f,	0.0f,	0.0f,	0.0f,	0.0f);

	float u0 = 0;
	float v0 = 0;
	float u1 = 1;
	float v1 = 1;

	if(bSharedTex)
	{
		u0 = float(nCol)/col_count;
		v0 = float(nRow)/row_count;

		u1 = float(nCol + 1)/col_count;
		v1 = float(nRow + 1)/row_count;
	}

	//front
	MODELVERTEX(pMod->v[16],0.5f*fWidth,	-0.5f*fHeight,	0.5f*fThick,	0.0f,	0.0f,	1.0f,	u1,		v0);
	MODELVERTEX(pMod->v[17],0.5f*fWidth,	0.5f*fHeight,	0.5f*fThick,	0.0f,	0.0f,	1.0f,	u1,		v1);
	MODELVERTEX(pMod->v[18],-0.5f*fWidth,	0.5f*fHeight,	0.5f*fThick,	0.0f,	0.0f,	1.0f,	u0,		v1);
	MODELVERTEX(pMod->v[19],-0.5f*fWidth,	-0.5f*fHeight,	0.5f*fThick,	0.0f,	0.0f,	1.0f,	u0,		v0);

	//back,逆时针方向
	MODELVERTEX(pMod->v[20],0.5f*fWidth,	0.5f*fHeight,	-0.5f*fThick,	0.0f,	0.0f,	-1.0f,	u1,		v0);
	MODELVERTEX(pMod->v[21],0.5f*fWidth,	-0.5f*fHeight,	-0.5f*fThick,	0.0f,	0.0f,	-1.0f,	u1,		v1);
	MODELVERTEX(pMod->v[22],-0.5f*fWidth,	-0.5f*fHeight,	-0.5f*fThick,	0.0f,	0.0f,	-1.0f,	u0,		v1);
	MODELVERTEX(pMod->v[23],-0.5f*fWidth,	0.5f*fHeight,	-0.5f*fThick,	0.0f,	0.0f,	-1.0f,	u0,		v0);


	//front
	pMod->indecs[30] = 16;
	pMod->indecs[31] = 17;
	pMod->indecs[32] = 18;
	pMod->indecs[33] = 18;
	pMod->indecs[34] = 19;
	pMod->indecs[35] = 16;

	pMod->viewsub[0].vstart= 0;
	pMod->viewsub[0].vcount = 4;
	pMod->viewsub[0].istart = 24;
	pMod->viewsub[0].icount = 6;
	pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST;
	pMod->viewsub[0].nTri_Num = 2;
	pMod->viewsub[0].pMaterial = NULL;

	//back
	pMod->indecs[24] = 20;
	pMod->indecs[25] = 21;
	pMod->indecs[26] = 22;
	pMod->indecs[27] = 22;
	pMod->indecs[28] = 23;
	pMod->indecs[29] = 20;

	pMod->viewsub[1].vstart= 0;
	pMod->viewsub[1].vcount = 4;
	pMod->viewsub[1].istart = 30;
	pMod->viewsub[1].icount = 6;
	pMod->viewsub[1].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[1].nTri_Num = 2;
	pMod->viewsub[1].pMaterial = NULL;


	//around
	pMod->indecs[0] = 0;	
	pMod->indecs[1] = 1;	
	pMod->indecs[2] = 2;
	pMod->indecs[3] = 2;
	pMod->indecs[4] = 3;	
	pMod->indecs[5] = 0;

	pMod->indecs[6] = 4;	
	pMod->indecs[7] = 5;	
	pMod->indecs[8] = 6;
	pMod->indecs[9] = 6;	
	pMod->indecs[10] = 7;	
	pMod->indecs[11] = 4;

	pMod->indecs[12] = 8;
	pMod->indecs[13] = 9;	
	pMod->indecs[14] = 10;
	pMod->indecs[15] = 10;	
	pMod->indecs[16] = 11;	
	pMod->indecs[17] = 8;

	pMod->indecs[18] = 12;	
	pMod->indecs[19] = 13;	
	pMod->indecs[20] = 14;
	pMod->indecs[21] = 14;	
	pMod->indecs[22] = 15;	
	pMod->indecs[23] = 12;

	pMod->viewsub[2].vstart= 0;
	pMod->viewsub[2].vcount = 16;
	pMod->viewsub[2].istart = 0;
	pMod->viewsub[2].icount = 24;
	pMod->viewsub[2].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[2].nTri_Num = 8;
	pMod->viewsub[2].pMaterial = NULL;

	return TRUE;
}
	
BOOL  SYModel_CreateTablet_TriFace(SYModel* pMod,
								   float fWidth,
								   float fHeight,
								   float fThick,
								   int	 nCol,
								   int	 nRow,
								   int	 col_count,
								   int	 row_count,
								   BOOL bSharedTex)
{
	int nSubset = 4;

	pMod->vcount = 18;
	pMod->icount = 24;

	pMod->v = new SYVertex3D[pMod->vcount];
	pMod->indecs = new DWORD[pMod->icount];

	pMod->nviewsub = nSubset;
	pMod->viewsub = new SYSubModel[pMod->nviewsub];

	float z1 = -sqrtf(3) / 6.f;
	float z2 = sqrtf(3) / 3.f;

// 	z1 += 0.04f;
// 	z2 -=  -0.08f;

	float u0 = 0;
	float v0 = 0;
	float u1 = 1;
	float v1 = 1;

	if(bSharedTex)
	{
		u0 = float(nCol)/col_count;
		v0 = float(nRow)/row_count;

		u1 = float(nCol + 1)/col_count;
		v1 = float(nRow + 1)/row_count;
	}

	MODELVERTEX(pMod->v[0],-0.5f*fWidth,-0.5f*fHeight,	z1*fHeight,	-1.0f,	0.0f,	0.0f,	0.0f,	1.0f);
	MODELVERTEX(pMod->v[1],-0.5f*fWidth,0.5f*fHeight,	z1*fHeight,	-1.0f,	0.0f,	0.0f,	1.0f,	1.0f);
	MODELVERTEX(pMod->v[2],-0.5f*fWidth,0.0f*fHeight,	z2*fHeight,	-1.0f,	0.0f,	0.0f,	0.5f,	0.0f);

	MODELVERTEX(pMod->v[3],0.5f*fWidth,	-0.5f*fHeight,	z1*fHeight,	1.0f,	0.0f,	0.0f,	0.0f,	1.0f);
	MODELVERTEX(pMod->v[4],0.5f*fWidth,	0.5f*fHeight,	z1*fHeight,	1.0f,	0.0f,	0.0f,	1.0f,	1.0f);
	MODELVERTEX(pMod->v[5],0.5f*fWidth,	0.0f*fHeight,	z2*fHeight,	1.0f,	0.0f,	0.0f,	0.5f,	0.0f);

	MODELVERTEX(pMod->v[6],0.5f*fWidth,	0.5f*fHeight,	z1*fHeight,	0.0f,	0.0f,	-1.0f,	u1,		v0);
	MODELVERTEX(pMod->v[7],0.5f*fWidth,	-0.5f*fHeight,	z1*fHeight,	0.0f,	0.0f,	-1.0f,	u1,		v1);
	MODELVERTEX(pMod->v[8],-0.5f*fWidth,-0.5f*fHeight,	z1*fHeight,	0.0f,	0.0f,	-1.0f,	u0,		v1);
	MODELVERTEX(pMod->v[9],-0.5f*fWidth, 0.5f*fHeight,	z1*fHeight,	0.0f,	0.0f,	-1.0f,	u0,		v0);

	MODELVERTEX(pMod->v[10],0.5f*fWidth,	0.0f*fHeight,	z2*fHeight,	0.0f,	0.866025f,	0.5f,	u1,	v0);
	MODELVERTEX(pMod->v[11],0.5f*fWidth,	0.5f*fHeight,	z1*fHeight,	0.0f,	0.866025f,	0.5f,	u1,	v1);
	MODELVERTEX(pMod->v[12],-0.5f*fWidth,	0.5f*fHeight,	z1*fHeight,	0.0f,	0.866025f,	0.5f,	u0,	v1);
	MODELVERTEX(pMod->v[13],-0.5f*fWidth,	0.0f*fHeight,	z2*fHeight,	0.0f,	0.866025f,	0.5f,	u0,	v0);

	MODELVERTEX(pMod->v[14],0.5f*fWidth,	-0.5f*fHeight,	z1*fHeight,	0.0f,	-0.866025f,	0.5f,	u1,	v0);
	MODELVERTEX(pMod->v[15],0.5f*fWidth,	0.0f*fHeight,	z2*fHeight,	0.0f,	-0.866025f,	0.5f,	u1,	v1);
	MODELVERTEX(pMod->v[16],-0.5f*fWidth,	0.0f*fHeight,	z2*fHeight,	0.0f,	-0.866025f,	0.5f,	u0,	v1);
	MODELVERTEX(pMod->v[17],-0.5f*fWidth,	-0.5f*fHeight,	z1*fHeight,	0.0f,	-0.866025f,	0.5f,	u0,	v0);

	pMod->indecs[0] = 0;	
	pMod->indecs[1] = 2;	
	pMod->indecs[2] = 1;

	pMod->indecs[3] = 3;	
	pMod->indecs[4] = 4;	
	pMod->indecs[5] = 5;

	pMod->viewsub[3].vstart= 0;
	pMod->viewsub[3].vcount = 6;
	pMod->viewsub[3].istart = 0;
	pMod->viewsub[3].icount = 6;
	pMod->viewsub[3].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[3].nTri_Num = 2;
	pMod->viewsub[3].pMaterial = NULL;

	pMod->indecs[6] = 6;	
	pMod->indecs[7] = 7;	
	pMod->indecs[8] = 8;
	pMod->indecs[9] = 8;	
	pMod->indecs[10] = 9;	
	pMod->indecs[11] = 6;

	pMod->viewsub[0].vstart= 0;
	pMod->viewsub[0].vcount = 6;
	pMod->viewsub[0].istart = 6;
	pMod->viewsub[0].icount = 6;
	pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[0].nTri_Num = 2;
	pMod->viewsub[0].pMaterial = NULL;

	pMod->indecs[12] = 10;	
	pMod->indecs[13] = 11;	
	pMod->indecs[14] = 12;
	pMod->indecs[15] = 12;	
	pMod->indecs[16] = 13;	
	pMod->indecs[17] = 10;

	pMod->viewsub[1].vstart= 0;
	pMod->viewsub[1].vcount = 6;
	pMod->viewsub[1].istart = 12;
	pMod->viewsub[1].icount = 6;
	pMod->viewsub[1].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[1].nTri_Num = 2;
	pMod->viewsub[1].pMaterial = NULL;

	pMod->indecs[18] = 14;	
	pMod->indecs[19] = 15;	
	pMod->indecs[20] = 16;
	pMod->indecs[21] = 16;	
	pMod->indecs[22] = 17;	
	pMod->indecs[23] = 14;

	pMod->viewsub[2].vstart= 0;
	pMod->viewsub[2].vcount = 6;
	pMod->viewsub[2].istart = 18;
	pMod->viewsub[2].icount = 6;
	pMod->viewsub[2].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[2].nTri_Num = 2;
	pMod->viewsub[2].pMaterial = NULL;
	
	return TRUE;
}



BOOL  SYModel_CreateTablet_Text_TwoFace(SYModel* pMod,
									   float fWidth,
									   float fHeight,
									   float fThick,
									   SIZE* txt_sz,
									   BOOL bRotY)
{
	pMod->nviewsub = 2;
	pMod->viewsub = new SYSubModel[pMod->nviewsub];

	pMod->vcount = 8;
	pMod->icount = 12;

	pMod->v = new SYVertex3D[pMod->vcount];
	pMod->indecs = new DWORD[pMod->icount];

	//////////////////////////////////////////////////////////////////////////
	//Text
	MODELVERTEX(pMod->v[0],0.5f*txt_sz[0].cx,	0.5*txt_sz[0].cy,	-0.501f*fThick,	0.0f,	0.0f,	-1.0f,	1.0f,	0.0f);
	MODELVERTEX(pMod->v[1],0.5f*txt_sz[0].cx,	-0.5*txt_sz[0].cy,	-0.501f*fThick,	0.0f,	0.0f,	-1.0f,	1.0f,	1.0f);
	MODELVERTEX(pMod->v[2],-0.5f*txt_sz[0].cx, -0.5*txt_sz[0].cy,	-0.501f*fThick,	0.0f,	0.0f,	-1.0f,	0.0f,	1.0f);
	MODELVERTEX(pMod->v[3],-0.5f*txt_sz[0].cx, 0.5*txt_sz[0].cy,	-0.501f*fThick,	0.0f,	0.0f,	-1.0f,	0.0f,	0.0f);

	MODELVERTEX(pMod->v[4],0.5f*txt_sz[1].cx,	-0.5f*txt_sz[1].cy,	0.501f*fThick,	0.0f,	0.0f,	1.0f,	1.0f,	0.0f);
	MODELVERTEX(pMod->v[5],0.5f*txt_sz[1].cx,	0.5f*txt_sz[1].cy,	0.501f*fThick,	0.0f,	0.0f,	1.0f,	1.0f,	1.0f);
	MODELVERTEX(pMod->v[6],-0.5f*txt_sz[1].cx, 0.5f*txt_sz[1].cy,	0.501f*fThick,	0.0f,	0.0f,	1.0f,	0.0f,	1.0f);
	MODELVERTEX(pMod->v[7],-0.5f*txt_sz[1].cx, -0.5f*txt_sz[1].cy,	0.501f*fThick,	0.0f,	0.0f,	1.0f,	0.0f,	0.0f);

	if(bRotY)
	{
		pMod->v[4].tv = 1 - pMod->v[4].tv;
		pMod->v[5].tv = 1 - pMod->v[5].tv;
		pMod->v[6].tv = 1 - pMod->v[6].tv;
		pMod->v[7].tv = 1 - pMod->v[7].tv;

		pMod->v[4].tu = 1 - pMod->v[4].tu;
		pMod->v[5].tu = 1 - pMod->v[5].tu;
		pMod->v[6].tu = 1 - pMod->v[6].tu;
		pMod->v[7].tu = 1 - pMod->v[7].tu;
	}
	////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Text
	pMod->indecs[0] = 0;	
	pMod->indecs[1] = 1;	
	pMod->indecs[2] = 2;
	pMod->indecs[3] = 2;	
	pMod->indecs[4] = 3;	
	pMod->indecs[5] = 0;

	pMod->viewsub[0].vstart= 0;
	pMod->viewsub[0].vcount =  pMod->vcount;
	pMod->viewsub[0].istart = 0;
	pMod->viewsub[0].icount =6;
	pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[0].nTri_Num = 2;
	pMod->viewsub[0].pMaterial = NULL;

	pMod->indecs[6] = 4;	
	pMod->indecs[7] = 5;	
	pMod->indecs[8] = 6;
	pMod->indecs[9] = 6;	
	pMod->indecs[10] = 7;	
	pMod->indecs[11] = 4;

	pMod->viewsub[1].vstart= 0;
	pMod->viewsub[1].vcount = pMod->vcount;
	pMod->viewsub[1].istart = 6;
	pMod->viewsub[1].icount = 2*3;
	pMod->viewsub[1].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[1].nTri_Num = 2;
	pMod->viewsub[1].pMaterial = NULL;

	return TRUE;

}

BOOL  SYModel_CreateTablet_Text_TriFace(SYModel* pMod,
										float fWidth,
										float fHeight,
										float fThick,
										SIZE* txt_sz)
{
	pMod->nviewsub = 3;
	pMod->viewsub = new SYSubModel[pMod->nviewsub];

	pMod->vcount = 12;
	pMod->icount = 18;

	pMod->v = new SYVertex3D[pMod->vcount];
	pMod->indecs = new DWORD[pMod->icount];

	float z1 = -sqrtf(3) / 6.f;
	float z2 = sqrtf(3) / 3.f;

	float z4 = sqrtf(3) / 12.f*fHeight;
	float z5 = sqrtf(3) / 4.f;

	float off = 0.05f*fHeight;

	{
		float l = sqrtf(3)/6.f*fHeight + off;
		float3 center = {0,0,-l};
		
		float tx = txt_sz[0].cx/2;
		float ty = txt_sz[0].cy/2;

		MODELVERTEX(pMod->v[0],center.x + tx,	center.y + ty,	center.z,	0.0f,	0.0f,	-1.0f,	1.0f,	0.0f);
		MODELVERTEX(pMod->v[1],center.x + tx,	center.y - ty,	center.z,	0.0f,	0.0f,	-1.0f,	1.0f,	1.0f);
		MODELVERTEX(pMod->v[2],center.x - tx,	center.y - ty,	center.z,	0.0f,	0.0f,	-1.0f,	0.0f,	1.0f);
		MODELVERTEX(pMod->v[3],center.x - tx,	center.y + ty,	center.z,	0.0f,	0.0f,	-1.0f,	0.0f,	0.0f);
	}
	

	{
		float l = sqrtf(3)/6.f*fHeight + off;

		float s = sqrtf(3)*0.5f;
		float c = 0.5f;

		float3 center = {0,s*l,c*l};

		float tx = txt_sz[1].cx/2;
		float ty = txt_sz[1].cy/2;

		MODELVERTEX(pMod->v[4],center.x + tx,	center.y - ty*c,		center.z+ ty*s,	0.0f,	0.866025f,	0.5f,	1.0f,	0.0f);
		MODELVERTEX(pMod->v[5],center.x + tx,	center.y + ty*c,	center.z- ty*s,	0.0f,	0.866025f,	0.5f,	1.0f,	1.0f);
		MODELVERTEX(pMod->v[6],center.x - tx,	center.y + ty*c,	center.z- ty*s,	0.0f,	0.866025f,	0.5f,	0.0f,	1.0f);
		MODELVERTEX(pMod->v[7],center.x - tx,	center.y - ty*c,		center.z+ ty*s,	0.0f,	0.866025f,	0.5f,	0.0f,	0.0f);

	}

	{
		float l = sqrtf(3)/6.f*fHeight + off;

		float s = sqrtf(3)*0.5f;
		float c = 0.5f;

		float3 center = {0,-s*l,c*l};

		float tx = txt_sz[2].cx/2;
		float ty = txt_sz[2].cy/2;

		MODELVERTEX(pMod->v[8],center.x + tx,	center.y - ty*c, 	center.z- ty*s,	0.0f,	-0.866025f,	0.5f,	1.0f,	0.0f);
		MODELVERTEX(pMod->v[9],center.x + tx,	center.y + ty*c,	center.z+ ty*s,	0.0f,	-0.866025f,	0.5f,	1.0f,	1.0f);
		MODELVERTEX(pMod->v[10],center.x - tx,	center.y + ty*c,	center.z+ ty*s,	0.0f,	-0.866025f,	0.5f,	0.0f,	1.0f);
		MODELVERTEX(pMod->v[11],center.x - tx,	center.y - ty*c,		center.z- ty*s,	0.0f,	-0.866025f,	0.5f,	0.0f,	0.0f);

	}
	pMod->indecs[0] = 0;	
	pMod->indecs[1] = 1;	
	pMod->indecs[2] = 2;
	pMod->indecs[3] = 2;	
	pMod->indecs[4] = 3;	
	pMod->indecs[5] = 0;

	pMod->indecs[6] = 4;	
	pMod->indecs[7] = 5;	
	pMod->indecs[8] = 6;
	pMod->indecs[9] = 6;	
	pMod->indecs[10] = 7;	
	pMod->indecs[11] = 4;

	pMod->indecs[12] = 8;	
	pMod->indecs[13] = 9;	
	pMod->indecs[14] = 10;
	pMod->indecs[15] = 10;	
	pMod->indecs[16] = 11;	
	pMod->indecs[17] = 8;

	pMod->viewsub[0].vstart= 0;
	pMod->viewsub[0].vcount = pMod->vcount;
	pMod->viewsub[0].istart = 0;
	pMod->viewsub[0].icount = 6;
	pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[0].nTri_Num = 2;
	pMod->viewsub[0].pMaterial = NULL;

	pMod->viewsub[1].vstart= 0;
	pMod->viewsub[1].vcount = pMod->vcount;
	pMod->viewsub[1].istart = 6;
	pMod->viewsub[1].icount = 6;
	pMod->viewsub[1].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[1].nTri_Num = 2;
	pMod->viewsub[1].pMaterial = NULL;

	pMod->viewsub[2].vstart= 0;
	pMod->viewsub[2].vcount = pMod->vcount;
	pMod->viewsub[2].istart = 12;
	pMod->viewsub[2].icount = 6;
	pMod->viewsub[2].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[2].nTri_Num = 2;
	pMod->viewsub[2].pMaterial = NULL;

	return TRUE;
}


BOOL  SYModel_CreateTablet(SYModel* pMod,
						   UINT uModelType,
						   UINT uStacks,
						   int  nPartCount,
						   int  nCount,
						   float fLength,
						   float fWeight,
						   float fHeight,
						   BOOL bText,
						   int nIndex /*= 0*/)
{
	if( uModelType == 0) //2-sides,立方体的前面和背面,其他面,只有前面和背面有字
	{
		int nSubPart = 3; //每个牌有3面
		if ( bText)
			nSubPart = 2; //牌上的字有2个面
		if(!bText)
		{
			pMod->vcount = 24*nPartCount;
			pMod->icount = 36*nPartCount;
			pMod->v = new SYVertex3D[pMod->vcount];
			pMod->indecs = new DWORD[pMod->icount];
			
			pMod->nviewsub = nSubPart*nPartCount;
			pMod->viewsub = new SYSubModel[pMod->nviewsub];
		}
		else
		{
			pMod->vcount = 8*nPartCount;
			pMod->icount = 12*nPartCount;
			pMod->v = new SYVertex3D[pMod->vcount];
			pMod->indecs = new DWORD[pMod->icount];

			pMod->nviewsub = nSubPart;
			pMod->viewsub = new SYSubModel[pMod->nviewsub];

		}

		float fUStep = 1.f/(float)uStacks;
		float fVStep = 1.f/(float)((nPartCount + uStacks - 1) / uStacks);
		if(!bText)
		{
			//for(int i=0,nBase=0;i<nPartCount;i++,nBase+=32)
			int i=0,nBase=0,nIdx=0;
			for( ; i <nPartCount;++i)
			{
				int nR = i / uStacks;
				int nC = i % uStacks;
				float u0 = fUStep * nC;
				float u1 = fUStep * (nC + 1);
				float v0 = fVStep * nR;
				float v1 = fVStep * (nR + 1);


				//展开,顺时针方向
				//bottom
				MODELVERTEX(pMod->v[nIdx+0],0.5f*fLength,	-0.5f*fHeight,	-0.5f*fWeight,	0.0f,	-1.0f,	0.0f,	1.0f,	0.0f);
				MODELVERTEX(pMod->v[nIdx+1],0.5f*fLength,	-0.5f*fHeight,	0.5f*fWeight,	0.0f,	-1.0f,	0.0f,	1.0f,	1.0f);
				MODELVERTEX(pMod->v[nIdx+2],-0.5f*fLength,	-0.5f*fHeight,	0.5f*fWeight,	0.0f,	-1.0f,	0.0f,	0.0f,	1.0f);
				MODELVERTEX(pMod->v[nIdx+3],-0.5f*fLength,	-0.5f*fHeight,	-0.5f*fWeight,	0.0f,	-1.0f,	0.0f,	0.0f,	0.0f);

				//right
				MODELVERTEX(pMod->v[nIdx+4],0.5f*fLength,	-0.5f*fHeight,	0.5f*fWeight,	1.0f,	0.0f,	0.0f,	1.0f,	0.0f);
				MODELVERTEX(pMod->v[nIdx+5],0.5f*fLength,	-0.5f*fHeight,	-0.5f*fWeight,	1.0f,	0.0f,	0.0f,	1.0f,	1.0f);
				MODELVERTEX(pMod->v[nIdx+6],0.5f*fLength,	0.5f*fHeight,	-0.5f*fWeight,	1.0f,	0.0f,	0.0f,	0.0f,	1.0f);
				MODELVERTEX(pMod->v[nIdx+7],0.5f*fLength,	0.5f*fHeight,	0.5f*fWeight,	1.0f,	0.0f,	0.0f,	0.0f,	0.0f);

				//top
				MODELVERTEX(pMod->v[nIdx+8],0.5f*fLength,	0.5f*fHeight,	0.5f*fWeight,	0.0f,	1.0f,	0.0f,	1.0f,	0.0f);
				MODELVERTEX(pMod->v[nIdx+9],0.5f*fLength,	0.5f*fHeight,	-0.5f*fWeight,	0.0f,	1.0f,	0.0f,	1.0f,	1.0f);
				MODELVERTEX(pMod->v[nIdx+10],-0.5f*fLength,	0.5f*fHeight,	-0.5f*fWeight,	0.0f,	1.0f,	0.0f,	0.0f,	1.0f);
				MODELVERTEX(pMod->v[nIdx+11],-0.5f*fLength,	0.5f*fHeight,	0.5f*fWeight,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f);

				//left
				MODELVERTEX(pMod->v[nIdx+12],-0.5f*fLength,	0.5f*fHeight,	0.5f*fWeight,	-1.0f,	0.0f,	0.0f,	1.0f,	0.0f);
				MODELVERTEX(pMod->v[nIdx+13],-0.5f*fLength,	0.5f*fHeight,	-0.5f*fWeight,	-1.0f,	0.0f,	0.0f,	1.0f,	1.0f);
				MODELVERTEX(pMod->v[nIdx+14],-0.5f*fLength,	-0.5f*fHeight,	-0.5f*fWeight,	-1.0f,	0.0f,	0.0f,	0.0f,	1.0f);
				MODELVERTEX(pMod->v[nIdx+15],-0.5f*fLength,	-0.5f*fHeight,	0.5f*fWeight,	-1.0f,	0.0f,	0.0f,	0.0f,	0.0f);

				//back,逆时针方向
				MODELVERTEX(pMod->v[nIdx+16],0.5f*fLength,	0.5f*fHeight,	-0.5f*fWeight,	0.0f,	0.0f,	-1.0f,	u1,		v0);
				MODELVERTEX(pMod->v[nIdx+17],0.5f*fLength,	-0.5f*fHeight,	-0.5f*fWeight,	0.0f,	0.0f,	-1.0f,	u1,		v1);
				MODELVERTEX(pMod->v[nIdx+18],-0.5f*fLength,	-0.5f*fHeight,	-0.5f*fWeight,	0.0f,	0.0f,	-1.0f,	u0,		v1);
				MODELVERTEX(pMod->v[nIdx+19],-0.5f*fLength,	0.5f*fHeight,	-0.5f*fWeight,	0.0f,	0.0f,	-1.0f,	u0,		v0);

				//front
				MODELVERTEX(pMod->v[nIdx+20],0.5f*fLength,	-0.5f*fHeight,	0.5f*fWeight,	0.0f,	0.0f,	1.0f,	u1,		v0);
				MODELVERTEX(pMod->v[nIdx+21],0.5f*fLength,	0.5f*fHeight,	0.5f*fWeight,	0.0f,	0.0f,	1.0f,	u1,		v1);
				MODELVERTEX(pMod->v[nIdx+22],-0.5f*fLength,	0.5f*fHeight,	0.5f*fWeight,	0.0f,	0.0f,	1.0f,	u0,		v1);
				MODELVERTEX(pMod->v[nIdx+23],-0.5f*fLength,	-0.5f*fHeight,	0.5f*fWeight,	0.0f,	0.0f,	1.0f,	u0,		v0);
			
				
				//front
				pMod->indecs[i*nBase+30] = nIdx+20;	pMod->indecs[i*nBase+31] = nIdx+21;	pMod->indecs[i*nBase+32] = nIdx+22;
				pMod->indecs[i*nBase+33] = nIdx+22;	pMod->indecs[i*nBase+34] = nIdx+23;	pMod->indecs[i*nBase+35] = nIdx+20;

				pMod->viewsub[i*3+0].vstart= 0;
				pMod->viewsub[i*3+0].vcount =  pMod->vcount;
				pMod->viewsub[i*3+0].istart = i*nBase+30;
				pMod->viewsub[i*3+0].icount =6;
				pMod->viewsub[i*3+0].nTri_Type = SY_TRIANGLE_LIST ;
				pMod->viewsub[i*3+0].nTri_Num = 2;
				pMod->viewsub[i*3+0].pMaterial = NULL;

				//back
				pMod->indecs[i*nBase+24] = nIdx+16;	pMod->indecs[i*nBase+25] = nIdx+17;	pMod->indecs[i*nBase+26] = nIdx+18;
				pMod->indecs[i*nBase+27] = nIdx+18;	pMod->indecs[i*nBase+28] = nIdx+19;	pMod->indecs[i*nBase+29] = nIdx+16;

				pMod->viewsub[i*3+1].vstart= 0;
				pMod->viewsub[i*3+1].vcount =  pMod->vcount;
				pMod->viewsub[i*3+1].istart = i*nBase+24;
				pMod->viewsub[i*3+1].icount =6;
				pMod->viewsub[i*3+1].nTri_Type = SY_TRIANGLE_LIST ;
				pMod->viewsub[i*3+1].nTri_Num = 2;
				pMod->viewsub[i*3+1].pMaterial = NULL;

				pMod->indecs[nBase+0] = nIdx+0;	pMod->indecs[nBase+1] = nIdx+1;	pMod->indecs[nBase+2] = nIdx+2;
				pMod->indecs[nBase+3] = nIdx+2;	pMod->indecs[nBase+4] = nIdx+3;	pMod->indecs[nBase+5] = nIdx+0;
				pMod->indecs[nBase+6] = nIdx+4;	pMod->indecs[nBase+7] = nIdx+5;	pMod->indecs[nBase+8] = nIdx+6;
				pMod->indecs[nBase+9] = nIdx+6;	pMod->indecs[nBase+10] = nIdx+7;	pMod->indecs[nBase+11] = nIdx+4;
				pMod->indecs[nBase+12] = nIdx+8;	pMod->indecs[nBase+13] = nIdx+9;	pMod->indecs[nBase+14] = nIdx+10;
				pMod->indecs[nBase+15] = nIdx+10;	pMod->indecs[nBase+16] = nIdx+11;	pMod->indecs[nBase+17] = nIdx+8;
				pMod->indecs[nBase+18] = nIdx+12;	pMod->indecs[nBase+19] = nIdx+13;	pMod->indecs[nBase+20] = nIdx+14;
				pMod->indecs[nBase+21] = nIdx+14;	pMod->indecs[nBase+22] = nIdx+15;	pMod->indecs[nBase+23] = nIdx+12;

				pMod->viewsub[i*3+2].vstart= 0;
				pMod->viewsub[i*3+2].vcount =  pMod->vcount;
				pMod->viewsub[i*3+2].istart = i*nBase+0;
				pMod->viewsub[i*3+2].icount =24;
				pMod->viewsub[i*3+2].nTri_Type = SY_TRIANGLE_LIST ;
				pMod->viewsub[i*3+2].nTri_Num = 8;
				pMod->viewsub[i*3+2].pMaterial = NULL;
				nBase +=36;
				nIdx += 24;
			
			}

		}
		else
		{
			//////////////////////////////////////////////////////////////////////////
			//Text
			MODELVERTEX(pMod->v[0],0.5f*fLength,	0.5*fHeight,	-0.501f*fWeight,	0.0f,	0.0f,	-1.0f,	1.0f,	0.0f);
			MODELVERTEX(pMod->v[1],0.5f*fLength,	-0.5*fHeight,	-0.501f*fWeight,	0.0f,	0.0f,	-1.0f,	1.0f,	1.0f);
			MODELVERTEX(pMod->v[2],-0.5f*fLength,	-0.5*fHeight,	-0.501f*fWeight,	0.0f,	0.0f,	-1.0f,	0.0f,	1.0f);
			MODELVERTEX(pMod->v[3],-0.5f*fLength,	0.5*fHeight,	-0.501f*fWeight,	0.0f,	0.0f,	-1.0f,	0.0f,	0.0f);

			MODELVERTEX(pMod->v[4],0.5f*fLength,	-0.5f*fHeight,	0.501f*fWeight,	0.0f,	0.0f,	1.0f,	1.0f,	0.0f);
			MODELVERTEX(pMod->v[5],0.5f*fLength,	0.5f*fHeight,	0.501f*fWeight,	0.0f,	0.0f,	1.0f,	1.0f,	1.0f);
			MODELVERTEX(pMod->v[6],-0.5f*fLength,	0.5f*fHeight,	0.501f*fWeight,	0.0f,	0.0f,	1.0f,	0.0f,	1.0f);
			MODELVERTEX(pMod->v[7],-0.5f*fLength,	-0.5f*fHeight,	0.501f*fWeight,	0.0f,	0.0f,	1.0f,	0.0f,	0.0f);
			////////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			//Text
			pMod->indecs[0] = 0;	pMod->indecs[1] = 1;	pMod->indecs[2] = 2;
			pMod->indecs[3] = 2;	pMod->indecs[4] = 3;	pMod->indecs[5] = 0;
			
			pMod->viewsub[0].vstart= 0;
			pMod->viewsub[0].vcount =  pMod->vcount;
			pMod->viewsub[0].istart = 0;
			pMod->viewsub[0].icount =6;
			pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST ;
			pMod->viewsub[0].nTri_Num = 2;
			pMod->viewsub[0].pMaterial = NULL;

			pMod->indecs[6] = 4;	pMod->indecs[7] = 5;	pMod->indecs[8] = 6;
			pMod->indecs[9] = 6;	pMod->indecs[10] = 7;	pMod->indecs[11] = 4;

			pMod->viewsub[1].vstart= 0;
			pMod->viewsub[1].vcount = pMod->vcount;
			pMod->viewsub[1].istart = 6;
			pMod->viewsub[1].icount = 2*3;
			pMod->viewsub[1].nTri_Type = SY_TRIANGLE_LIST ;
			pMod->viewsub[1].nTri_Num = 2;
			pMod->viewsub[1].pMaterial = NULL;
			//////////////////////////////////////////////////////////////////////////
		}		
		
	}
	else if(uModelType == 1) //3-sides,三棱柱
	{
		int nSubPart = 4; //每个牌有4面
		if ( bText)
			nSubPart = 3; //牌上的字有3个面
		if(bText)
		{
			pMod->nviewsub = nSubPart*nPartCount;
			pMod->viewsub = new SYSubModel[nSubPart*nPartCount];

			pMod->vcount = 30*nPartCount;
			pMod->icount = 14*nPartCount*3;

		}
		else
		{
			pMod->nviewsub = nSubPart*nPartCount;
			pMod->viewsub = new SYSubModel[nSubPart*nPartCount];

		}

		pMod->vcount = 30*nPartCount;
		pMod->icount = 14*nPartCount*3;
		pMod->v = new SYVertex3D[pMod->vcount];
		pMod->indecs = new DWORD[pMod->icount];
		
		pMod->nviewsub = nSubPart*nPartCount;
		pMod->viewsub = new SYSubModel[nSubPart*nPartCount];

		float z1 = -sqrtf(3) / 6.f;
		float z2 = sqrtf(3) / 3.f;

		//if(PROJECT_HEIGHT > 576)
		{
			z1 = -sqrtf(3) / 6.f + 0.04f;
			z2 = sqrtf(3) / 3.f - 0.08f;
		}
		/*else
		{
		z1 = -sqrtf(3) / 6.f + 0.03f;
		z2 = sqrtf(3) / 3.f - 0.06f;
		}*/

		float fUStep = 1.f/(float)uStacks;
		float fVStep = 1.f/(float)((nPartCount + uStacks - 1) / uStacks);
		for(int i=0,nBase=0;i<nPartCount;i++,nBase+=30)
		{
			int nR = i / uStacks;
			int nC = i % uStacks;
			float u0 = fUStep * nC;
			float u1 = fUStep * (nC + 1);
			float v0 = fVStep * nR;
			float v1 = fVStep * (nR + 1);

			MODELVERTEX(pMod->v[nBase+0],-0.5f*fLength,	-0.5f*fHeight,	z1*fWeight,	-1.0f,	0.0f,	0.0f,	0.0f,	1.0f);
			MODELVERTEX(pMod->v[nBase+1],-0.5f*fLength,	0.5f*fHeight,	z1*fWeight,	-1.0f,	0.0f,	0.0f,	1.0f,	1.0f);
			MODELVERTEX(pMod->v[nBase+2],-0.5f*fLength,	0.0f*fHeight,	z2*fWeight,	-1.0f,	0.0f,	0.0f,	0.5f,	0.0f);

			MODELVERTEX(pMod->v[nBase+3],0.5f*fLength,	-0.5f*fHeight,	z1*fWeight,	1.0f,	0.0f,	0.0f,	0.0f,	1.0f);
			MODELVERTEX(pMod->v[nBase+4],0.5f*fLength,	0.5f*fHeight,	z1*fWeight,	1.0f,	0.0f,	0.0f,	1.0f,	1.0f);
			MODELVERTEX(pMod->v[nBase+5],0.5f*fLength,	0.0f*fHeight,	z2*fWeight,	1.0f,	0.0f,	0.0f,	0.5f,	0.0f);

			MODELVERTEX(pMod->v[nBase+6],0.5f*fLength,	0.5f*fHeight,	z1*fWeight,	0.0f,	0.0f,	-1.0f,	u1,		v0);
			MODELVERTEX(pMod->v[nBase+7],0.5f*fLength,	-0.5f*fHeight,	z1*fWeight,	0.0f,	0.0f,	-1.0f,	u1,		v1);
			MODELVERTEX(pMod->v[nBase+8],-0.5f*fLength,	-0.5f*fHeight,	z1*fWeight,	0.0f,	0.0f,	-1.0f,	u0,		v1);
			MODELVERTEX(pMod->v[nBase+9],-0.5f*fLength,	0.5f*fHeight,	z1*fWeight,	0.0f,	0.0f,	-1.0f,	u0,		v0);

			MODELVERTEX(pMod->v[nBase+10],0.5f*fLength,	0.0f*fHeight,	z2*fWeight,	0.0f,	0.866025f,	0.5f,	u1,	v0);
			MODELVERTEX(pMod->v[nBase+11],0.5f*fLength,	0.5f*fHeight,	z1*fWeight,	0.0f,	0.866025f,	0.5f,	u1,	v1);
			MODELVERTEX(pMod->v[nBase+12],-0.5f*fLength,	0.5f*fHeight,	z1*fWeight,	0.0f,	0.866025f,	0.5f,	u0,	v1);
			MODELVERTEX(pMod->v[nBase+13],-0.5f*fLength,	0.0f*fHeight,	z2*fWeight,	0.0f,	0.866025f,	0.5f,	u0,	v0);

			MODELVERTEX(pMod->v[nBase+14],0.5f*fLength,	-0.5f*fHeight,	z1*fWeight,	0.0f,	-0.866025f,	0.5f,	u1,	v0);
			MODELVERTEX(pMod->v[nBase+15],0.5f*fLength,	0.0f*fHeight,	z2*fWeight,	0.0f,	-0.866025f,	0.5f,	u1,	v1);
			MODELVERTEX(pMod->v[nBase+16],-0.5f*fLength,	0.0f*fHeight,	z2*fWeight,	0.0f,	-0.866025f,	0.5f,	u0,	v1);
			MODELVERTEX(pMod->v[nBase+17],-0.5f*fLength,	-0.5f*fHeight,	z1*fWeight,	0.0f,	-0.866025f,	0.5f,	u0,	v0);
			//////////////////////////////////////////////////////////////////////////
			//Text
			z1 *= 1.002f;
			z2 *= 1.002f;

			MODELVERTEX(pMod->v[nBase+18],0.5f*fLength,	0.501f*fHeight,	 z1*fWeight,	0.0f,	0.0f,	-1.0f,	1.0f,	0.0f);
			MODELVERTEX(pMod->v[nBase+19],0.5f*fLength,	-0.501f*fHeight, z1*fWeight,	0.0f,	0.0f,	-1.0f,	1.0f,	1.0f);
			MODELVERTEX(pMod->v[nBase+20],-0.5f*fLength,	-0.501f*fHeight, z1*fWeight,	0.0f,	0.0f,	-1.0f,	0.0f,	1.0f);
			MODELVERTEX(pMod->v[nBase+21],-0.5f*fLength,	0.501f*fHeight,	 z1*fWeight,	0.0f,	0.0f,	-1.0f,	0.0f,	0.0f);

			MODELVERTEX(pMod->v[nBase+22],0.5f*fLength,	0.0f*fHeight,	 z2*fWeight,	0.0f,	0.866025f,	0.5f,	1.0f,	0.0f);
			MODELVERTEX(pMod->v[nBase+23],0.5f*fLength,	0.501f*fHeight,	 z1*fWeight,	0.0f,	0.866025f,	0.5f,	1.0f,	1.0f);
			MODELVERTEX(pMod->v[nBase+24],-0.5f*fLength,	0.501f*fHeight,	 z1*fWeight,	0.0f,	0.866025f,	0.5f,	0.0f,	1.0f);
			MODELVERTEX(pMod->v[nBase+25],-0.5f*fLength,	0.0f*fHeight,	 z2*fWeight,	0.0f,	0.866025f,	0.5f,	0.0f,	0.0f);

			MODELVERTEX(pMod->v[nBase+26],0.5f*fLength,	-0.501f*fHeight, z1*fWeight,	0.0f,	-0.866025f,	0.5f,	1.0f,	0.0f);
			MODELVERTEX(pMod->v[nBase+27],0.5f*fLength,	0.0f*fHeight,	 z2*fWeight,	0.0f,	-0.866025f,	0.5f,	1.0f,	1.0f);
			MODELVERTEX(pMod->v[nBase+28],-0.5f*fLength,	0.0f*fHeight,	 z2*fWeight,	0.0f,	-0.866025f,	0.5f,	0.0f,	1.0f);
			MODELVERTEX(pMod->v[nBase+29],-0.5f*fLength,	-0.501f*fHeight, z1*fWeight,	0.0f,	-0.866025f,	0.5f,	0.0f,	0.0f);
			//////////////////////////////////////////////////////////////////////////
		}

		for(int i=0,nBase=0,nIdx=0;i<nPartCount;i++,nBase+=42,nIdx+=30)
		{
			pMod->indecs[nBase+0] = nIdx+0;	pMod->indecs[nBase+1] = nIdx+2;	pMod->indecs[nBase+2] = nIdx+1;
			pMod->indecs[nBase+3] = nIdx+3;	pMod->indecs[nBase+4] = nIdx+4;	pMod->indecs[nBase+5] = nIdx+5;
			pMod->indecs[nBase+6] = nIdx+6;	pMod->indecs[nBase+7] = nIdx+7;	pMod->indecs[nBase+8] = nIdx+8;
			pMod->indecs[nBase+9] = nIdx+8;	pMod->indecs[nBase+10] = nIdx+9;	pMod->indecs[nBase+11] = nIdx+6;
			pMod->indecs[nBase+12] = nIdx+10;	pMod->indecs[nBase+13] = nIdx+11;	pMod->indecs[nBase+14] = nIdx+12;
			pMod->indecs[nBase+15] = nIdx+12;	pMod->indecs[nBase+16] = nIdx+13;	pMod->indecs[nBase+17] = nIdx+10;
			pMod->indecs[nBase+18] = nIdx+14;	pMod->indecs[nBase+19] = nIdx+15;	pMod->indecs[nBase+20] = nIdx+16;
			pMod->indecs[nBase+21] = nIdx+16;	pMod->indecs[nBase+22] = nIdx+17;	pMod->indecs[nBase+23] = nIdx+14;
			//////////////////////////////////////////////////////////////////////////
			//Text
			pMod->indecs[nBase+24] = nIdx+18;	pMod->indecs[nBase+25] = nIdx+19;	pMod->indecs[nBase+26] = nIdx+20;
			pMod->indecs[nBase+27] = nIdx+20;	pMod->indecs[nBase+28] = nIdx+21;	pMod->indecs[nBase+29] = nIdx+18;
			pMod->indecs[nBase+30] = nIdx+22;	pMod->indecs[nBase+31] = nIdx+23;	pMod->indecs[nBase+32] = nIdx+24;
			pMod->indecs[nBase+33] = nIdx+24;	pMod->indecs[nBase+34] = nIdx+25;	pMod->indecs[nBase+35] = nIdx+22;
			pMod->indecs[nBase+36] = nIdx+26;	pMod->indecs[nBase+37] = nIdx+27;	pMod->indecs[nBase+38] = nIdx+28;
			pMod->indecs[nBase+39] = nIdx+28;	pMod->indecs[nBase+40] = nIdx+29;	pMod->indecs[nBase+41] = nIdx+26;
			//////////////////////////////////////////////////////////////////////////
		}

		for(int i=0,nIdx=0;i<nPartCount;i++,nIdx+=42)
		{
			int nID = i * nSubPart;
			if ( !bText)
			{
				//Tablet_Flank
				pMod->viewsub[nID].vstart= 0;
				pMod->viewsub[nID].vcount =  30*nPartCount;
				pMod->viewsub[nID].istart = (nIdx+0)/3*3;
				pMod->viewsub[nID].icount = 2*3;
				pMod->viewsub[nID].nTri_Type = SY_TRIANGLE_LIST ;
				pMod->viewsub[nID].nTri_Num = 2;
				pMod->viewsub[nID].pMaterial = NULL;

				nID++;
				//Tablet_Side_0
				pMod->viewsub[nID].vstart= 0;
				pMod->viewsub[nID].vcount =  30*nPartCount;
				pMod->viewsub[nID].istart = (nIdx+6)/3*3;
				pMod->viewsub[nID].icount = 2*3;
				pMod->viewsub[nID].nTri_Type = SY_TRIANGLE_LIST ;
				pMod->viewsub[nID].nTri_Num = 2;
				pMod->viewsub[nID].pMaterial = NULL;

				nID++;
				//Tablet_Side_1
				pMod->viewsub[nID].vstart= 0;
				pMod->viewsub[nID].vcount =  30*nPartCount;
				pMod->viewsub[nID].istart = (nIdx+12)/3*3;
				pMod->viewsub[nID].icount = 2*3;
				pMod->viewsub[nID].nTri_Type = SY_TRIANGLE_LIST ;
				pMod->viewsub[nID].nTri_Num = 2;
				pMod->viewsub[nID].pMaterial = NULL;

				nID++;
				//Tablet_Side_2
				pMod->viewsub[nID].vstart= 0;
				pMod->viewsub[nID].vcount =  30*nPartCount;
				pMod->viewsub[nID].istart = (nIdx+18)/3*3;
				pMod->viewsub[nID].icount = 2*3;
				pMod->viewsub[nID].nTri_Type = SY_TRIANGLE_LIST ;
				pMod->viewsub[nID].nTri_Num = 2;
				pMod->viewsub[nID].pMaterial = NULL;
			}
			else
			{
				if ( nIndex == 0)
				{
					//Tablet_Caption_0
					pMod->viewsub[nID].vstart= 0;
					pMod->viewsub[nID].vcount =  30*nPartCount;
					pMod->viewsub[nID].istart = (nIdx+24)/3*3;
					pMod->viewsub[nID].icount = 2*3;
					pMod->viewsub[nID].nTri_Type = SY_TRIANGLE_LIST ;
					pMod->viewsub[nID].nTri_Num = 2;
					pMod->viewsub[nID].pMaterial = NULL;
					nID++;

				}
				else if ( nIndex ==1)
				{
					//Tablet_Caption_1
					pMod->viewsub[nID].vstart= 0;
					pMod->viewsub[nID].vcount =  30*nPartCount;
					pMod->viewsub[nID].istart = (nIdx+36)/3*3;
					pMod->viewsub[nID].icount = 2*3;
					pMod->viewsub[nID].nTri_Type = SY_TRIANGLE_LIST ;
					pMod->viewsub[nID].nTri_Num = 2;
					pMod->viewsub[nID].pMaterial = NULL;
					nID++;

				}
				else
				{
					//Tablet_Caption_2
					pMod->viewsub[nID].vstart= 0;
					pMod->viewsub[nID].vcount =  30*nPartCount;
					pMod->viewsub[nID].istart = (nIdx+30)/3*3;
					pMod->viewsub[nID].icount = 2*3;
					pMod->viewsub[nID].nTri_Type = SY_TRIANGLE_LIST ;
					pMod->viewsub[nID].nTri_Num = 2;
					pMod->viewsub[nID].pMaterial = NULL;
					nID++;
				}
			}
		}
	}
	return TRUE;
}
//星
BOOL  SYModel_CreateStar(SYModel* pMod,UINT uStacks,UINT uSlices,int  nPartCount,float fRU,float frU,float fRD,
						 float frD,float fTLength,float fTHeight,float fThick)
{
	UINT nSinglePoint = (uStacks + uSlices) * 2;
	UINT nBorderPoint = nSinglePoint * nPartCount + 1;
	UINT nTopPoint = nBorderPoint * uSlices + 1;
	UINT nBorderFace = (nBorderPoint - 1) * 2;
	UINT nTopFace = nBorderFace * (uSlices - 1) + nBorderPoint - 1;

	UINT nV = (nBorderPoint + nTopPoint) * 2;
	UINT nF = nBorderFace + nTopFace * 2;
	pMod->vcount = nV;
	pMod->icount = nF*3;
	pMod->nviewsub = 3;
	pMod->viewsub = new SYSubModel[pMod->nviewsub];

	SYVertex3D* pVB = new SYVertex3D[pMod->vcount];
	DWORD* pIB = new DWORD[pMod->icount];	

	pMod->v = pVB; 
	pMod->indecs = pIB;
	

	float R = fRU;
	float r = frU;
	float ro = fRD;
	float ri = frD;
	float rt = fTLength;
	float rb = fTHeight;
	float n = nPartCount;
	BOOL bArcTop = fabs(rt) > ABS_DOUBLE_0;
	BOOL bArcBottom = fabs(rb) > ABS_DOUBLE_0;

	vector3f pA ;
	float3Set(&pA, 0,R,0);
	float aB = FLOAT_PI / n;
	vector3f pB;
	float3Set(&pB, 0,0,0);
	pB.x = - r * sin(aB);
	pB.y = r * cos(aB);
	float aA = atan(r * sin(aB) / (R - r * cos(aB)));
	vector3f pC;
	float3Set(&pC, 0,0,0);
	pC.y = R - ro / sin(aA);
	float aC = FLOAT_PI / 2.f - aA;
	vector3f pD;
	float3Set(&pD, 0,0,0);
	pD.x = - ro * sin(aC);
	pD.y = pC.y + ro * cos(aC);
	float aD = aA + aB;
	float lBE = ri / sin(aD);
	float aE = FLOAT_PI / 2.f - aB;
	vector3f pE;
	float3Set(&pE, 0,0,0);
	pE.x = pB.x - lBE * cos(aE);
	pE.y = pB.y + lBE * sin(aE);
	float lBF = ri / tan(aD);
	float lAB = sqrt(pow((pA.x - pB.x),2.f) + pow((pA.y - pB.y),2.f));
	float lAF = lAB - lBF;
	vector3f pF;
	float3Set(&pF, 0,0,0);
	pF.x = pB.x * lAF / lAB;
	pF.y = R - (R - pB.y) * lAF / lAB;
	float fRt = 0;
	float fRb = 0;
	vector3f pRt;
	float3Set(&pRt, 0,0,0);
	vector3f pRb;
	float3Set(&pRb, 0,0,0);
	float fRFake = pC.y + ro;
	if(bArcTop)
	{
		fRt = sqrt(pow(rt,2.f) + pow(fRFake,2.f)) / 2.f / cos(atan(fRFake / rt)); 
		pRt.z = -fThick/2.f - rt + fRt;
	}
	if(bArcBottom)
	{
		fRb = sqrt(pow(rb,2.f) + pow(fRFake,2.f)) / 2.f / cos(atan(fRFake / rb)); 
		pRb.z = fThick/2.f + rb - fRb;
	}

	vector3f* vPos = new vector3f[nBorderPoint];	
	vector3f* vNors = new vector3f[nBorderPoint];	

	int nIdx = 0;
	//Part1 LpModelInfo->uSlices
	float fStepX = (pD.x - pF.x) / (float)uSlices;
	float fStepY = (pD.y - pF.y) / (float)uSlices;
	vector3f nor;
	float3Set(&nor,-cos(aA),sin(aA),0);
	for(int i=0;i<uSlices+1;i++)
	{
		float3Set(&vPos[nIdx],pF.x + fStepX * i,pF.y + fStepY * i,0);
		vNors[nIdx++] = nor;
	}
	//Part2 LpModelInfo->uStacks
	float fSumA = FLOAT_PI - aA;
	float fStepA = (fSumA - aA) / (float)uStacks;
	for(int i=0;i<uStacks-1;i++)
	{
		fSumA -= fStepA;
		float sinSum = sin(fSumA);
		float cosSum = cos(fSumA);
		//vPos[nIdx] = vector3f(pC.x + ro * cosSum,pC.y + ro * sinSum,0);
		float3Set(&vPos[nIdx],pC.x + ro * cosSum,pC.y + ro * sinSum,0);
		//vNors[nIdx++] = vector3f(cosSum,sinSum,0);
		float3Set(&vNors[nIdx++],cosSum,sinSum,0);
	}
	//Part3 LpModelInfo->uSlices
	nor.x = -nor.x;
	for(int i=0;i<uSlices+1;i++)
	{
		//vPos[nIdx] = vector3f(-pD.x + fStepX * i,pD.y - fStepY * i,0);
		float3Set(&vPos[nIdx],-pD.x + fStepX * i,pD.y - fStepY * i,0);
		vNors[nIdx++] = nor;
	}
	//Part4 LpModelInfo->uStacks
	matrix4f matRot;
	matrix4f_fromeuler_lyxz(&matRot, 0, 0, FLOAT_PI -aB);
//	D3DXMatrixRotationZ(&matRot,FLOAT_PI - aB);

	fSumA = FLOAT_PI - aD;
	fStepA = (fSumA - aD) / (float)uStacks;
	fSumA = aD;
	for(int i=0;i<uStacks-1;i++)
	{
		fSumA += fStepA;
		float sinSum = sin(fSumA);
		float cosSum = cos(fSumA);
		//vPos[nIdx] = vector3f(ri * cosSum,ri * sinSum,0);
		float3Set(&vPos[nIdx],ri * cosSum,ri * sinSum,0);
		//vNors[nIdx] = -vector3f(cosSum,sinSum,0);
		float3Set(&vNors[nIdx],-cosSum,-sinSum,0);
		float3MulMatQ(&vPos[nIdx],&vPos[nIdx],&matRot);
	//	D3DXVec3TransformCoord(&vPos[nIdx],&vPos[nIdx],&matRot);
		float3MulMatQ(&vNors[nIdx],&vNors[nIdx],&matRot);
	//	D3DXVec3TransformCoord(&vNors[nIdx],&vNors[nIdx],&matRot);
		vPos[nIdx].x -= pE.x; 
		vPos[nIdx++].y += pE.y; 
	}

	//build border
	nIdx = nSinglePoint;
	fSumA = FLOAT_PI * 2;
	fStepA = aB * 2.f;	
	for(int i=1;i<n;i++)
	{
		fSumA -= fStepA;
		//D3DXMatrixRotationZ(&matRot,fSumA);
		matrix4f_fromeuler_lyxz(&matRot, 0, 0, fSumA);
	//	D3DXVec3TransformCoordArray(&vPos[nIdx],sizeof(vector3f),vPos,sizeof(vector3f),
	//		&matRot,nSinglePoint);
//		D3DXVec3TransformCoordArray(&vNors[nIdx],sizeof(vector3f),vNors,sizeof(vector3f),
//			&matRot,nSinglePoint);
		vector3f* vPoint = &vPos[nIdx];
		vector3f* vNorPoint = &vNors[nIdx];
		for (int k = 0 ; k <nSinglePoint; ++k)
		{
			vPoint[k].x = vPos[k].x * matRot._11 + vPos[k].y * matRot._21 + vPos[k].z * matRot._31 + 1.f * matRot._41;
			vPoint[k].y = vPos[k].x * matRot._12 + vPos[k].y * matRot._22 + vPos[k].z * matRot._32 + 1.f * matRot._42;
			vPoint[k].z = vPos[k].x * matRot._13 + vPos[k].y * matRot._23 + vPos[k].z * matRot._33 + 1.f * matRot._43;

			vNorPoint[k].x = vNors[k].x * matRot._11 + vNors[k].y * matRot._21 + vNors[k].z * matRot._31 + 1.f * matRot._41;
			vNorPoint[k].y = vNors[k].x * matRot._12 + vNors[k].y * matRot._22 + vNors[k].z * matRot._32 + 1.f * matRot._42;
			vNorPoint[k].z = vNors[k].x * matRot._13 + vNors[k].y * matRot._23 + vNors[k].z * matRot._33 + 1.f * matRot._43;
		}
		nIdx += nSinglePoint;
	}

	vPos[nIdx] = vPos[0];
	vNors[nIdx] = vNors[0];

	//build pv
	nIdx = 0;
	//pVB[nIdx++] = MODELVERTEX(0,	0,	-fThick/2.f - rt,	0,	0,	-1.f,	.5f,	.5f);	
	pVB[nIdx].x = 0;
	pVB[nIdx].y = 0;
	pVB[nIdx].z = -fThick/2.f - rt;
	float3Set(&pVB[nIdx].vNorm ,0,0,-1.f);
	pVB[nIdx].tu = .5f;
	pVB[nIdx].tv = .5f;
	nIdx++;
	float fSum = 1.f;
	float fRate = 1.f / uSlices;
	for(int j=0;j<uSlices;j++)
	{
		for(int i=0;i<nBorderPoint;i++)
		{
			pVB[nIdx].vDyn = vPos[i] * fSum;
			if(bArcTop)
			{
				pVB[nIdx].z = sqrt(pow(fRt,2.f) - pow(pVB[nIdx].x,2.f) - 
					pow(pVB[nIdx].y,2.f)) * (-rt / fabs(rt));
				float3Normalize(&pVB[nIdx].vNorm,&pVB[nIdx].vDyn);				
				pVB[nIdx].z += pRt.z;
			}
			else
			{
				pVB[nIdx].z = -fThick/2.f;
				//pVB[nIdx].vNorm = vector3f(0,0,-1.f);
				float3Set(&pVB[nIdx].vNorm ,0,0,-1.f);
			}
			pVB[nIdx].tu = .5f + pVB[nIdx].x/2/R;
			pVB[nIdx].tv = .5f - pVB[nIdx].y/2/R;
			nIdx++;
		}
		fSum -= fRate;
	}

	//pVB[nIdx++] = MODELVERTEX(0,	0,	fThick/2.f + rb,	0,	0,	1.f,	.5f,	.5f);	
	pVB[nIdx].x = 0;
	pVB[nIdx].y = 0;
	pVB[nIdx].z = fThick/2.f +rb;
	float3Set(&pVB[nIdx].vNorm ,0,0,1.f);
	pVB[nIdx].tu = .5f;
	pVB[nIdx].tv = .5f;
	nIdx++;
	fSum = 1.f;
	for(int j=0;j<uSlices;j++)
	{
		for(int i=0;i<nBorderPoint;i++)
		{
			pVB[nIdx].vDyn = vPos[i] * fSum;
			if(bArcBottom)
			{
				pVB[nIdx].z = sqrt(pow(fRb,2.f) - pow(pVB[nIdx].x,2.f) - 
					pow(pVB[nIdx].y,2.f)) * (rb / fabs(rb));
			//	D3DXVec3Normalize(&pVB[nIdx].vNorm,&pVB[nIdx].vDyn);
				float3Normalize(&pVB[nIdx].vNorm,&pVB[nIdx].vDyn);
				pVB[nIdx].z += pRb.z;
			}
			else
			{
				pVB[nIdx].z = fThick/2.f;
				//pVB[nIdx].vNorm= vector3f(0,0,1.f);
				float3Set(&pVB[nIdx].vNorm,0,0,1.f);
			}
			pVB[nIdx].tu = .5f + pVB[nIdx].x/2/R;
			pVB[nIdx].tv = .5f - pVB[nIdx].y/2/R;
			nIdx++;
		}
		fSum -= fRate;
	}

	for(int i=0;i<nBorderPoint;i++)
	{
		pVB[nIdx].vDyn = vPos[i];
		if(bArcTop)
		{
			pVB[nIdx].z = sqrt(pow(fRt,2.f) - pow(pVB[nIdx].x,2.f) - 
				pow(pVB[nIdx].y,2.f)) * (-rt / fabs(rt));
			pVB[nIdx].z += pRt.z;
		}
		else
		{
			pVB[nIdx].z = -fThick/2.f;
		}
		pVB[nIdx].vNorm = vNors[i];
		pVB[nIdx].tu = .5f + pVB[nIdx].x/2/R;
		pVB[nIdx].tv = .5f - pVB[nIdx].y/2/R;
		nIdx++;
	}

	for(int i=0;i<nBorderPoint;i++)
	{
		pVB[nIdx].vDyn = vPos[i];
		if(bArcBottom)
		{
			pVB[nIdx].z = sqrt(pow(fRb,2.f) - pow(pVB[nIdx].x,2.f) - 
				pow(pVB[nIdx].y,2.f)) * (rb / fabs(rb));
			pVB[nIdx].z += pRb.z;
		}
		else
		{
			pVB[nIdx].z = fThick/2.f;
		}
		pVB[nIdx].vNorm = vNors[i];
		pVB[nIdx].tu = .5f + pVB[nIdx].x/2/R;
		pVB[nIdx].tv = .5f - pVB[nIdx].y/2/R;
		nIdx++;
	}

	delete []vPos;
	vPos = NULL;
	delete []vNors;
	vNors = NULL;

	int a,b,c,d;
	nIdx = 0;
	int nOrg = 0;
	int nBase = 1;
	for(int i=0;i<uSlices-1;i++)
	{
		for (int j=0;j<nBorderPoint-1;j++)
		{
			a = nBase + j;
			b = nBase + j + 1;
			c = nBase + j + nBorderPoint;
			d = nBase + j + nBorderPoint + 1;

			pIB[nIdx++] = d;
			pIB[nIdx++] = a;
			pIB[nIdx++] = b;
			pIB[nIdx++] = d;
			pIB[nIdx++] = c;
			pIB[nIdx++] = a;
		}
		nBase += nBorderPoint;
	}

	for (int j=0;j<nBorderPoint-1;j++)
	{
		a = nBase + j;
		b = nBase + j + 1;

		pIB[nIdx++] = nOrg;
		pIB[nIdx++] = a;
		pIB[nIdx++] = b;
	}
	nBase += nBorderPoint;

	nOrg = nBase;
	nBase++;
	for(int i=0;i<uSlices-1;i++)
	{
		for (int j=0;j<nBorderPoint-1;j++)
		{
			a = nBase + j;
			b = nBase + j + 1;
			c = nBase + j + nBorderPoint;
			d = nBase + j + nBorderPoint + 1;

			pIB[nIdx++] = d;
			pIB[nIdx++] = b;
			pIB[nIdx++] = a;
			pIB[nIdx++] = d;
			pIB[nIdx++] = a;
			pIB[nIdx++] = c;
		}
		nBase += nBorderPoint;
	}

	for (int j=0;j<nBorderPoint-1;j++)
	{
		a = nBase + j;
		b = nBase + j + 1;

		pIB[nIdx++] = nOrg;
		pIB[nIdx++] = b;
		pIB[nIdx++] = a;
	}
	nBase += nBorderPoint;

	for (int j=0;j<nBorderPoint-1;j++)
	{
		a = nBase + j;
		b = nBase + j + 1;
		c = nBase + j + nBorderPoint;
		d = nBase + j + nBorderPoint + 1;

		pIB[nIdx++] = d;
		pIB[nIdx++] = b;
		pIB[nIdx++] = a;
		pIB[nIdx++] = d;
		pIB[nIdx++] = a;
		pIB[nIdx++] = c;
	}
	// 顶面
	pMod->viewsub[0].vstart= 0;
	pMod->viewsub[0].vcount =  nV;
	pMod->viewsub[0].istart = 0;
	pMod->viewsub[0].icount =  nTopFace*3;
	pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[0].nTri_Num = nTopFace;
	pMod->viewsub[0].pMaterial = NULL;

	// 底面
	pMod->viewsub[1].vstart= 0;
	pMod->viewsub[1].vcount =  nV;
	pMod->viewsub[1].istart = nTopFace*3;
	pMod->viewsub[1].icount =  nTopFace*3;
	pMod->viewsub[1].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[1].nTri_Num = nTopFace;
	pMod->viewsub[1].pMaterial = NULL;
	// 侧面 
	pMod->viewsub[2].vstart= 0;
	pMod->viewsub[2].vcount =  nV;
	pMod->viewsub[2].istart = nTopFace*6;
	pMod->viewsub[2].icount =  nBorderFace*3;
	pMod->viewsub[2].nTri_Type = SY_TRIANGLE_LIST ;
	pMod->viewsub[2].nTri_Num = nBorderFace;
	pMod->viewsub[2].pMaterial = NULL;
	return TRUE;
}


//饼图
BOOL  SYModel_CreatePie(SYModel* pMod,
						UINT	uSharp,
						UINT	uFragMent,
						float	fAngle0,
						float	fAngle1,
						float	fRadius,
						float	fThick)
{

	/*
	参数0:起始角度
	参数1:终止角度
	参数3:z值
	参数4:固定法线x
	参数5:固定法线y
	参数6:固定法线z
	参数7:纹理坐标u
	参数8:纹理坐标v
	参数9:半径(区分原点)
	参数10:法线开关(0:使用固定法线,1:动侧面法线计算,2:弧面法线计算,
                        3:动侧面法线计算)
	*/

	float thick = fThick;
	float radius =  fRadius;


	UINT arc_point_count = UINT(abs(fAngle1 - fAngle0)/PAI*180.0f + 1);

	//上面点 = arc_point_count+1 （原点 + 弧点）
	//下面点 = arc_point_count+1 （原点 + 弧点）
	//左面 = 4
	//左面 = 4
	//弧面 = arc_point_count*2
	UINT vcount = (arc_point_count +1)*2 + 4*2 + arc_point_count*2;

	//上面三角形 = arc_point_count-1
	//下面三角形 = arc_point_count-1

	//左面三角形 = 2
	//左面三角形 = 2
	//弧面三角形 = (arc_point_count-1)*2

	UINT icount = ((arc_point_count-1)*4 + 4)*3;


	pMod->vcount = vcount;
	pMod->icount = icount;
	pMod->nviewsub = 5;
	pMod->viewsub = new SYSubModel[pMod->nviewsub];

	pMod->v = new SYVertex3D[vcount];
	pMod->indecs = new DWORD[icount];

	SYVertex3D*  pv =  pMod->v;
	DWORD*		 pi =  pMod->indecs;

	//制作index时的顶点基数
	UINT		 vbase = 0;

	//两点间的间隔角度
	float	da = (fAngle1 - fAngle0)/(arc_point_count -1);

	//上面
	{	
		//圆心
		SYVertex3D v0;
		v0.x = 0;
		v0.y = 0;
		v0.z = -thick;
	
		v0.nx = 0;
		v0.ny = 0;
		v0.nz = -1;

		v0.tu = 0.5f;
		v0.tv = 0.5f;	

		v0.tangent.x = 0;
		v0.tangent.y = 0;

		*pv++ = v0;
		++vbase;

		//弧点
		{
			SYVertex3D v;
			v.z = -thick;

			v.nx = 0;
			v.ny = 0;
			v.nz = -1;

			v.tangent.x = 0;
			v.tangent.y = 0;

			//arc_point_count个弧点
			for(UINT i = 0 ; i< arc_point_count; ++i)
			{
				float a0 = fAngle0 + i*da;

				v.x = fAngle0;	//起始角度
				v.y = a0;		//终止角度
				
				float c = cosf(a0);
				float s = sinf(a0);
					
				v.tu = c/2.0f + 0.5f;
				v.tv = 0.5f - s/2.0f ;

				v.tangent.x = radius; //参数9:半径(区分原点)

				*pv++ = v;
			}
			
			//arc_point_count－1	个 三角形
			for(UINT i = 0 ; i< arc_point_count-1; ++i)
			{
				*pi++ = 0;

				if(da > 0)
				{
					*pi++ = (i + vbase + 1);
					*pi++ = (i + vbase);
				}
				else
				{
					*pi++ = (i + vbase);
					*pi++ = (i + vbase + 1);
				}
			}
		}

		vbase += arc_point_count;

		pMod->viewsub[0].vstart = 0;
		pMod->viewsub[0].vcount = arc_point_count + 1;

		pMod->viewsub[0].istart = 0;
		pMod->viewsub[0].icount = (arc_point_count-1)*3;

		pMod->viewsub[0].nTri_Num = arc_point_count-1;
		pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST;
		pMod->viewsub[0].pMaterial = NULL;
	}


	//下面
	{	

		//圆心
		SYVertex3D v0;
		v0.x = 0;
		v0.y = 0;
		v0.z = 0;

		v0.nx = 0;
		v0.ny = 0;
		v0.nz = 1;

		v0.tu = 0.5f;
		v0.tv = 0.5f;	

		v0.tangent.x = 0;
		v0.tangent.y = 0;

		*pv++ = v0;
		++vbase;
		//
		{
			SYVertex3D v;
			v.z = 0;

			v.nx = 0;
			v.ny = 0;
			v.nz = 1;

			v.tangent.x = 0;
			v.tangent.y = 0;


			for(UINT i = 0 ; i< arc_point_count; ++i)
			{
				float a0 = fAngle0 + i*da;

				v.x = fAngle0;	//起始角度
				v.y = a0;		//终止角度

				float c = cosf(a0);
				float s = sinf(a0);

				v.tu = c/2.0f + 0.5f;
				v.tv = 0.5f - s/2.0f ;

				v.tangent.x = radius; //参数9:半径(区分原点)

				*pv++ = v;
			}

			for(UINT i = 0 ; i< arc_point_count-1; ++i)
			{
				*pi++ = vbase - 1;

				if(da <= 0)
				{
					*pi++ = (i + vbase + 1);
					*pi++ = (i + vbase);
				}
				else
				{
					*pi++ = (i + vbase);
					*pi++ = (i + vbase + 1);
				}
			}
		}

		vbase += arc_point_count;

		pMod->viewsub[1].vstart = pMod->viewsub[0].vstart + pMod->viewsub[0].vcount;
		pMod->viewsub[1].vcount = arc_point_count + 1;

		pMod->viewsub[1].istart = pMod->viewsub[0].istart + pMod->viewsub[0].icount;
		pMod->viewsub[1].icount = (arc_point_count-1)*3;

		pMod->viewsub[1].nTri_Num = arc_point_count-1;
		pMod->viewsub[1].nTri_Type = SY_TRIANGLE_LIST;
        pMod->viewsub[1].pMaterial= NULL;
	}


	//弧面
	{	
		SYVertex3D v;

		v.nx = 0;
		v.ny = 0;
		v.nz = 0;

		v.tangent.x = 0;
		v.tangent.y = 0;

		//arc_point_count*2个点
		for(UINT i = 0 ; i<arc_point_count; ++i)
		{
			float a0 = fAngle0 + i*da;

			float c = cosf(a0);
			float s = sinf(a0);

            v.x = fAngle0;	//起始角度
            v.y = a0;		//终止角度
			v.z = -thick;

			v.tu = c/2.0f + 0.5f;
			v.tv = 0.5f - s/2.0f ;

			v.tangent.x = radius; //参数9:半径(区分原点)
			v.tangent.y = 2.0f; //法线开关(0:使用固定法线,1:动侧面法线计算,2:弧面法线计算)

			*pv++ = v;

            v.z = 0;
            *pv++ = v;
		}

		//（arc_point_count-1）*2个三角形
		for(UINT j = 0 ; j<arc_point_count-1; ++j)
		{

            if(fAngle1 > fAngle0)
            {
			    *pi++ = vbase + j*2;
			    *pi++ = vbase + j*2 + 2;
                *pi++ = vbase + j*2 + 1;

			    *pi++ = vbase + j*2 + 1;
			    *pi++ = vbase + j*2 + 2;
                *pi++ = vbase + j*2 + 3;
            }
            else
            {
                *pi++ = vbase + j*2;
                *pi++ = vbase + j*2 + 1;
                *pi++ = vbase + j*2 + 2;

                *pi++ = vbase + j*2 + 2;
                *pi++ = vbase + j*2 + 1;
                *pi++ = vbase + j*2 + 3;
            }
		}
	
		vbase += arc_point_count*2;

        pMod->viewsub[2].vstart = pMod->viewsub[1].vstart + pMod->viewsub[1].vcount;
		pMod->viewsub[2].vcount = arc_point_count*2;

		pMod->viewsub[2].istart = pMod->viewsub[1].istart + pMod->viewsub[1].icount;
		pMod->viewsub[2].icount = (arc_point_count-1)*6;

		pMod->viewsub[2].nTri_Num = (arc_point_count-1)*2;
		pMod->viewsub[2].nTri_Type = SY_TRIANGLE_LIST;

        pMod->viewsub[2].pMaterial = NULL;
	}

	//0侧面
	{	
		//4个点
		{
			float c = cosf(fAngle0);
			float s = sinf(fAngle0);
			
			SYVertex3D v0,v1,v2,v3;

			v0.x = 0;
			v0.y = 0;
			v0.z = -thick;

			v0.nx = s;
			v0.ny = -c;
			v0.nz = 0;

			v0.tu = 0.5f;
			v0.tv = 0.5f;	

			v0.tangent.x = 0;//参数9:半径(区分原点)
			v0.tangent.y = 0;//法线开关(0:使用固定法线,1:动侧面法线计算,2:弧面法线计算)

			v1.x = 0;
			v1.y = 0;
			v1.z = 0;

			v1.nx = s;
			v1.ny = -c;
			v1.nz = 0;

			v1.tu = 0.5f;
			v1.tv = 0.5f;	

			v1.tangent.x = 0;//参数9:半径(区分原点)
			v1.tangent.y = 0;//法线开关(0:使用固定法线,1:动侧面法线计算,2:弧面法线计算)
			
			v2.x = fAngle0;
			v2.y = fAngle0;
			v2.z = -thick;

            v2.nx = s;
            v2.ny = -c;
            v2.nz = 0;

			v2.tu = c/2.0f + 0.5f;
			v2.tv = 0.5f - s/2.0f ;

			v2.tangent.x = radius;//参数9:半径(区分原点)
			v2.tangent.y = 0;//法线开关(0:使用固定法线,1:动侧面法线计算,2:弧面法线计算)

			v3.x = fAngle0;
			v3.y = fAngle0;
			v3.z = 0;

			v3.nx = s;
			v3.ny = -c;
			v3.nz = 0;

			v3.tu = c/2.0f + 0.5f;
			v3.tv = 0.5f - s/2.0f ;

			v3.tangent.x = radius;//参数9:半径(区分原点)
			v3.tangent.y = 0;//法线开关(0:使用固定法线,1:动侧面法线计算,2:弧面法线计算)

            if(fAngle1 <= fAngle0)
             {
                 v1.nx = -v1.nx;
                 v1.ny = -v1.ny;
                 v1.nz = -v1.nz;

                 v2.nx = -v2.nx;
                 v2.ny = -v2.ny;
                 v2.nz = -v2.nz;

                 v3.nx = -v3.nx;
                 v3.ny = -v3.ny;
                 v3.nz = -v3.nz;

                 v0.nx = -v0.nx;
                 v0.ny = -v0.ny;
                 v0.nz = -v0.nz; 
             }

             *pv++ = v0;
             *pv++ = v1;
             *pv++ = v2;
             *pv++ = v3;
		}

		//2个三角形
        
        if(fAngle1 > fAngle0)
		{
			*pi++ = vbase;
            *pi++ = vbase + 2;
			*pi++ = vbase + 1;
			
			*pi++ = vbase + 1;
			*pi++ = vbase + 2;
            *pi++ = vbase + 3;
		}
        else
        {
            *pi++ = vbase;
            *pi++ = vbase + 1;
            *pi++ = vbase + 2;

            *pi++ = vbase + 2;
            *pi++ = vbase + 1;
            *pi++ = vbase + 3;
        }
        
		vbase += 4;

		pMod->viewsub[3].vstart = pMod->viewsub[2].vstart + pMod->viewsub[2].vcount;
		pMod->viewsub[3].vcount = 4;

		pMod->viewsub[3].istart = pMod->viewsub[2].istart + pMod->viewsub[2].icount;
		pMod->viewsub[3].icount = 6;

		pMod->viewsub[3].nTri_Num = 2;
		pMod->viewsub[3].nTri_Type = SY_TRIANGLE_LIST;

        pMod->viewsub[3].pMaterial = NULL;
	}

	//1侧面
	{	
		//4个点
		{
			SYVertex3D v0,v1,v2,v3;

			v0.x = fAngle0;
			v0.y = fAngle1;
			v0.z = -thick;

			v0.nx = 0;
			v0.ny = 0;
			v0.nz = 0;

			v0.tu = 0.5f;
			v0.tv = 0.5f;	

			v0.tangent.x = 0;//参数9:半径(区分原点)
			v0.tangent.y = 1;//法线开关(0:使用固定法线,1:动侧面法线计算,2:弧面法线计算)

			v1.x = fAngle0;
			v1.y = fAngle1;
			v1.z = 0;

			v1.nx = 0;
			v1.ny = 0;
			v1.nz = 0;

			v1.tu = 0.5f;
			v1.tv = 0.5f;	

			v1.tangent.x = 0;//参数9:半径(区分原点)
			v1.tangent.y = 1;//法线开关(0:使用固定法线,1:动侧面法线计算,2:弧面法线计算)

			v2.x = fAngle0;
			v2.y = fAngle1;
			v2.z = -thick;

			v2.nx = 0;
			v2.ny = 0;
			v2.nz = 0;

			v2.tu = 0;
			v2.tv = 0 ;

			v2.tangent.x = radius;//参数9:半径(区分原点)
			v2.tangent.y = 1;//法线开关(0:使用固定法线,1:动侧面法线计算,2:弧面法线计算)

			v3.x = fAngle0;
			v3.y = fAngle1;
			v3.z = 0;

			v3.nx = 0;
			v3.ny = 0;
			v3.nz = 0;

			v3.tu = 0;
			v3.tv = 0;

			v3.tangent.x = radius;//参数9:半径(区分原点)
			v3.tangent.y = 1;//法线开关(0:使用固定法线,1:动侧面法线计算,2:弧面法线计算
                                        // 3: 动侧面法线计算)

            if(fAngle1 <= fAngle0)
            {
                v0.tangent.y = 3;
                v1.tangent.y = 3;
                v2.tangent.y = 3;
                v3.tangent.y = 3;
            }

            *pv++ = v0;
            *pv++ = v1;
            *pv++ = v2;
			*pv++ = v3;
		}

		//2个三角形
        if(fAngle1 > fAngle0)
		{
			*pi++ = vbase;
			*pi++ = vbase + 1;
			*pi++ = vbase + 2;

			*pi++ = vbase + 1;
			*pi++ = vbase + 3;
			*pi++ = vbase + 2;
		}
        else
        {
            *pi++ = vbase;
            *pi++ = vbase + 2;
            *pi++ = vbase + 1;

            *pi++ = vbase + 1;
            *pi++ = vbase + 2;
            *pi++ = vbase + 3;
        }

		vbase += 4;

		pMod->viewsub[4].vstart = pMod->viewsub[3].vstart + pMod->viewsub[3].vcount;
		pMod->viewsub[4].vcount = 4;

		pMod->viewsub[4].istart = pMod->viewsub[3].istart + pMod->viewsub[3].icount;
		pMod->viewsub[4].icount = 6;

		pMod->viewsub[4].nTri_Num = 2;
		pMod->viewsub[4].nTri_Type = SY_TRIANGLE_LIST;

        pMod->viewsub[4].pMaterial = NULL;
	}

	return TRUE;
}

BOOL  SYModel_CreateTitlePlane(SYModel* pMod)
{
    pMod->vcount = 4;
    pMod->icount = 6;
    pMod->nviewsub = 1;
    pMod->viewsub = new SYSubModel[pMod->nviewsub];

    pMod->v = new SYVertex3D[pMod->vcount];
    pMod->indecs = new DWORD[pMod->icount];

    pMod->v[0].x = -0.5f;
    pMod->v[0].y = -0.5f;
    pMod->v[0].z = 0;

    pMod->v[0].nx = 0;
    pMod->v[0].ny = 0;
    pMod->v[0].nz = -1.0f;

    pMod->v[0].tu = 0;
    pMod->v[0].tv = 1;

    pMod->v[0].tangent.x = 1.0f;
    pMod->v[0].tangent.y = 0;

    pMod->v[1].x = 0.5f;
    pMod->v[1].y = -0.5f;
    pMod->v[1].z = 0;

    pMod->v[1].nx = 0;
    pMod->v[1].ny = 0;
    pMod->v[1].nz = -1.0f;

    pMod->v[1].tu = 1;
    pMod->v[1].tv = 1;

    pMod->v[1].tangent.x = 1.0f;
    pMod->v[1].tangent.y = 1;

    pMod->v[2].x = -0.5f;
    pMod->v[2].y = 0.5f;
    pMod->v[2].z = 0;

    pMod->v[2].nx = 0;
    pMod->v[2].ny = 0;
    pMod->v[2].nz = -1.0f;

    pMod->v[2].tu = 0;
    pMod->v[2].tv = 0;

    pMod->v[2].tangent.x = 1.0f;
    pMod->v[2].tangent.y = 0;

    pMod->v[3].x = 0.5f;
    pMod->v[3].y = 0.5f;
    pMod->v[3].z = 0;

    pMod->v[3].nx = 0;
    pMod->v[3].ny = 0;
    pMod->v[3].nz = -1.0f;

    pMod->v[3].tu = 1;
    pMod->v[3].tv = 0;

    pMod->v[3].tangent.x = 1.0f;
    pMod->v[3].tangent.y = 0;

    pMod->indecs[0] = 0;
    pMod->indecs[1] = 2;
    pMod->indecs[2] = 1;

    pMod->indecs[3] = 3;
    pMod->indecs[4] = 1;
    pMod->indecs[5] = 2;

    pMod->viewsub[0].vstart = 0;
    pMod->viewsub[0].vcount = 4;

    pMod->viewsub[0].istart = 0;
    pMod->viewsub[0].icount = 6;

    pMod->viewsub[0].nTri_Num = 2;
    pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST;

    pMod->viewsub[0].pMaterial = NULL;

    return TRUE;
}

BOOL SYModel_CreateChamferPie(	SYModel* pMod,
								UINT uSharp,
								UINT uFragMent,
								float fAngle0,
								float fAngle1,
								float fRadius,
								float fInnerRadius,
								float fThick,
								UINT uChamferType,
								float fChamferLen, 
								float fChamferHeight,
								UINT uChamferPrecission,
								UINT uChamferLayout)
{
	//参数校正
	fInnerRadius = min(fInnerRadius,fRadius - 1);
	fThick =  max(fThick,2);

	uChamferPrecission = max(1, min(uChamferPrecission,10));
	uFragMent =  max(1, min(uFragMent,10));

	CSYPieModeling modeling(uSharp,uFragMent,fAngle0,fAngle1,
		fRadius,fInnerRadius,fThick,uChamferType,fChamferLen,fChamferHeight,
		uChamferPrecission,uChamferLayout);

	modeling.CreateModel(pMod);

	return TRUE;
}

BOOL  SYModel_CreateXYPlane(SYModel* pMod,float width,float height,float3 OrginPt)
{
	pMod->vcount = 8;
	pMod->icount = 12;
	pMod->nviewsub = 1;
	pMod->viewsub = new SYSubModel[pMod->nviewsub];

	pMod->v = new SYVertex3D[pMod->vcount];
	pMod->indecs = new DWORD[pMod->icount];

	for(int i = 0 ;i< pMod->vcount;  ++i)
	{
		pMod->v[i].tangent.x = 1.0f;
		pMod->v[i].tangent.y = 0.0f;

		pMod->v[i].ny = 0;
		pMod->v[i].nx = 0;
		pMod->v[i].nz = -1;

		if(i> 3)
			pMod->v[i].nz = 1;

		pMod->v[i].x = OrginPt.x;
		pMod->v[i].y = OrginPt.y;
		pMod->v[i].z = OrginPt.z;
	}

// 	pMod->v[0].x += 0;
// 	pMod->v[0].y += 0;

	pMod->v[0].tu = 0;
	pMod->v[0].tv = 1;
	
	pMod->v[1].x += width;

	pMod->v[1].tu = 1;
	pMod->v[1].tv = 1;
		
	pMod->v[2].y = height;

	pMod->v[2].tu = 0;
	pMod->v[2].tv = 0;

	pMod->v[2].tangent.x = 1.0f;
	pMod->v[2].tangent.y = 0;

	pMod->v[3].x += width;
	pMod->v[3].y += height;

	pMod->v[3].tu = 1;
	pMod->v[3].tv = 0;

// 	pMod->v[4].x += 0;
// 	pMod->v[4].y += 0;

	pMod->v[4].tu = 0;
	pMod->v[4].tv = 1;

	pMod->v[5].x += width;
	
	pMod->v[5].tu = 1;
	pMod->v[5].tv = 1;

	pMod->v[6].y += height;

	pMod->v[6].tu = 0;
	pMod->v[6].tv = 0;


	pMod->v[7].x += width;
	pMod->v[7].y += height;

	pMod->v[7].tu = 1;
	pMod->v[7].tv = 0;

	pMod->indecs[0] = 0;
	pMod->indecs[1] = 2;
	pMod->indecs[2] = 1;

	pMod->indecs[3] = 3;
	pMod->indecs[4] = 1;
	pMod->indecs[5] = 2;

	pMod->indecs[6] = 4;
	pMod->indecs[7] = 5;
	pMod->indecs[8] = 6;

	pMod->indecs[9] = 7;
	pMod->indecs[10] = 6;
	pMod->indecs[11] = 5;

	pMod->viewsub[0].vstart = 0;
	pMod->viewsub[0].vcount = 8;

	pMod->viewsub[0].istart = 0;
	pMod->viewsub[0].icount = 12;

	pMod->viewsub[0].nTri_Num = 4;
	pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST;

	pMod->viewsub[0].pMaterial = NULL;

	return TRUE;
}

BOOL  SYModel_CreateYZPlane(SYModel* pMod,float width,float height, float3 OrginPt)
{
	pMod->vcount = 8;
	pMod->icount = 12;
	pMod->nviewsub = 1;
	pMod->viewsub = new SYSubModel[pMod->nviewsub];

	pMod->v = new SYVertex3D[pMod->vcount];
	pMod->indecs = new DWORD[pMod->icount];

	for(int i = 0 ;i< pMod->vcount;  ++i)
	{
		pMod->v[i].tangent.x = 1.0f;
		pMod->v[i].tangent.y = 0.0f;
						
		pMod->v[i].ny = 0;
		pMod->v[i].nz = 0;
		pMod->v[i].nx = -1;

		if(i> 3)
			pMod->v[i].nx = 1;

		pMod->v[i].x = OrginPt.x;
		pMod->v[i].y = OrginPt.y;
		pMod->v[i].z = OrginPt.z;
	}

	pMod->v[0].y += 0;
	pMod->v[0].z += 0;

	pMod->v[0].tu = 1;
	pMod->v[0].tv = 1;
	
	pMod->v[1].y += 0;
	pMod->v[1].z += -width;

	pMod->v[1].tu = 0;
	pMod->v[1].tv = 1;

	pMod->v[2].y += height;
	pMod->v[2].z += 0;

	pMod->v[2].tu = 1;
	pMod->v[2].tv = 0;

	pMod->v[3].y += height;
	pMod->v[3].z += -width;

	pMod->v[3].tu = 0;
	pMod->v[3].tv = 0;
	
	pMod->v[4].y += 0;
	pMod->v[4].z += 0;

	pMod->v[4].tu = 1;
	pMod->v[4].tv = 1;

	pMod->v[5].y += 0;
	pMod->v[5].z += -width;

	pMod->v[5].tu = 0;
	pMod->v[5].tv = 1;

	pMod->v[6].y += height;
	pMod->v[6].z += 0;

	pMod->v[6].tu = 1;
	pMod->v[6].tv = 0;

	pMod->v[7].y += height;
	pMod->v[7].z += -width;

	pMod->v[7].tu = 0;
	pMod->v[7].tv = 0;


	pMod->indecs[0] = 0;
	pMod->indecs[1] = 2;
	pMod->indecs[2] = 1;

	pMod->indecs[3] = 3;
	pMod->indecs[4] = 1;
	pMod->indecs[5] = 2;

	pMod->indecs[6] = 4;
	pMod->indecs[7] = 5;
	pMod->indecs[8] = 6;

	pMod->indecs[9] = 7;
	pMod->indecs[10] = 6;
	pMod->indecs[11] = 5;

	pMod->viewsub[0].vstart = 0;
	pMod->viewsub[0].vcount = 8;

	pMod->viewsub[0].istart = 0;
	pMod->viewsub[0].icount = 12;

	pMod->viewsub[0].nTri_Num = 4;
	pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST;

	pMod->viewsub[0].pMaterial = NULL;

	return TRUE;
}

BOOL  SYModel_CreateZXPlane(SYModel* pMod,float width,float height, float3 OrginPt)
{
	pMod->vcount = 8;
	pMod->icount = 12;
	pMod->nviewsub = 1;
	pMod->viewsub = new SYSubModel[pMod->nviewsub];

	pMod->v = new SYVertex3D[pMod->vcount];
	pMod->indecs = new DWORD[pMod->icount];

	for(int i = 0 ;i< pMod->vcount;  ++i)
	{
		pMod->v[i].tangent.x = 1.0f;
		pMod->v[i].tangent.y = 0.0f;

		pMod->v[i].ny = 1;
		pMod->v[i].nz = 0;
		pMod->v[i].nx = 0;

		if(i> 3)
			pMod->v[i].ny = -1;

		pMod->v[i].x = OrginPt.x;
		pMod->v[i].y = OrginPt.y;
		pMod->v[i].z = OrginPt.z;
	}


	pMod->v[0].z += -height;

	pMod->v[0].tu = 0;
	pMod->v[0].tv = 1;

	pMod->v[1].x += width;
	pMod->v[1].z += -height;

	pMod->v[1].tu = 1;
	pMod->v[1].tv = 1;

	pMod->v[2].tu = 0;
	pMod->v[2].tv = 0;

	pMod->v[3].x += width;

	pMod->v[3].tu = 1;
	pMod->v[3].tv = 0;

	pMod->v[4].z += -height;

	pMod->v[4].tu = 0;
	pMod->v[4].tv = 1;
	
	pMod->v[5].x += width;
	pMod->v[5].z += -height;

	pMod->v[5].tu = 1;
	pMod->v[5].tv = 1;

	pMod->v[6].tu = 0;
	pMod->v[6].tv = 0;

	pMod->v[7].x += width;

	pMod->v[7].tu = 1;
	pMod->v[7].tv = 0;



	pMod->indecs[0] = 0;
	pMod->indecs[1] = 2;
	pMod->indecs[2] = 1;

	pMod->indecs[3] = 3;
	pMod->indecs[4] = 1;
	pMod->indecs[5] = 2;

	pMod->indecs[6] = 4;
	pMod->indecs[7] = 5;
	pMod->indecs[8] = 6;

	pMod->indecs[9] = 7;
	pMod->indecs[10] = 6;
	pMod->indecs[11] = 5;
	
	pMod->viewsub[0].vstart = 0;
	pMod->viewsub[0].vcount = 8;

	pMod->viewsub[0].istart = 0;
	pMod->viewsub[0].icount = 12;

	pMod->viewsub[0].nTri_Num = 4;
	pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST;

	pMod->viewsub[0].pMaterial = NULL;

	return TRUE;
}

BOOL SYModel_CreateXYLabels(SYModel* pMod,float width,float height, float3 OrginPt, UINT segment, float labelWidth)
{
	int label_count = segment + 1;

	float inval = (height - labelWidth*label_count)/segment;

	if(inval < 1.0f)
	{
		OrginPt.z -= 1;

		return SYModel_CreateXYPlane(pMod,width,height,OrginPt);
	}
	
	pMod->vcount = 4*label_count;
	pMod->icount = 6*label_count ;
	pMod->nviewsub = 1;
	pMod->viewsub = new SYSubModel[pMod->nviewsub];

	pMod->v = new SYVertex3D[pMod->vcount];
	pMod->indecs = new DWORD[pMod->icount];

	for(int i = 0 ;i< pMod->vcount;  ++i)
	{
		pMod->v[i].tangent.x = 1.0f;
		pMod->v[i].tangent.y = 0.0f;

		pMod->v[i].ny = 0;
		pMod->v[i].nx = 0;
		pMod->v[i].nz = -1;

		pMod->v[i].x = OrginPt.x;
		pMod->v[i].y = OrginPt.y;
		pMod->v[i].z = OrginPt.z - 1;
	}

	for(int i = 0 ;i< label_count;  ++i)
	{
		float delta = i*(inval + labelWidth);
		int idx = i*4;

		pMod->v[idx].x += 0;
		pMod->v[idx].y += delta;
		
		pMod->v[idx].tu = 0;
		pMod->v[idx].tv = 1;
		
		pMod->v[idx+1].x += width;
		pMod->v[idx+1].y += delta;
		
		pMod->v[idx+1].tu = 1;
		pMod->v[idx+1].tv = 1;

		if(i == label_count - 1)
		{
			pMod->v[idx+2].x += 0;
			pMod->v[idx+2].y += height;
			pMod->v[idx+2].z =  OrginPt.z;

			pMod->v[idx+2].tu = 0;
			pMod->v[idx+2].tv = 0;

			pMod->v[idx+3].x += width;
			pMod->v[idx+3].y += height;
			pMod->v[idx+3].z =  OrginPt.z;

			pMod->v[idx+3].tu = 1;
			pMod->v[idx+3].tv = 0;
		}
		else
		{
			pMod->v[idx+2].x += 0;
			pMod->v[idx+2].y += delta+labelWidth;
			

			pMod->v[idx+2].tu = 0;
			pMod->v[idx+2].tv = 0;

			pMod->v[idx+3].x += width;
			pMod->v[idx+3].y += delta+labelWidth;

			pMod->v[idx+3].tu = 1;
			pMod->v[idx+3].tv = 0;
		}
		

		int i_idx = i*6;

		pMod->indecs[i_idx] = idx;
		pMod->indecs[i_idx+1] = idx+2;
		pMod->indecs[i_idx+2] = idx+1;

		pMod->indecs[i_idx+3] = idx+3;
		pMod->indecs[i_idx+4] = idx+1;
		pMod->indecs[i_idx+5] = idx+2;
	}

	pMod->viewsub[0].vstart = 0;
	pMod->viewsub[0].vcount = pMod->vcount ;

	pMod->viewsub[0].istart = 0;
	pMod->viewsub[0].icount = pMod->icount;

	pMod->viewsub[0].nTri_Num = label_count*2;
	pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST;

	pMod->viewsub[0].pMaterial = NULL;

	return TRUE;
}

BOOL SYModel_CreateYZLabels(SYModel* pMod,float width,float height, float3 OrginPt, UINT segment, float labelWidth)
{
	int label_count = segment + 1;
	
	float inval = (height - labelWidth*label_count)/segment;

	if(inval < 1.0f)
	{
		OrginPt.x += 1;

		return SYModel_CreateYZPlane(pMod,width,height,OrginPt);
	}
		
	pMod->vcount = 4*label_count;
	pMod->icount = 6*label_count ;
	pMod->nviewsub = 1;
	pMod->viewsub = new SYSubModel[pMod->nviewsub];

	pMod->v = new SYVertex3D[pMod->vcount];
	pMod->indecs = new DWORD[pMod->icount];

	for(int i = 0 ;i< pMod->vcount;  ++i)
	{
		pMod->v[i].tangent.x = 1.0f;
		pMod->v[i].tangent.y = 0.0f;

		pMod->v[i].ny = 0;
		pMod->v[i].nz = 0;
		pMod->v[i].nx = 1;

		pMod->v[i].x = OrginPt.x + 1;
		pMod->v[i].y = OrginPt.y;
		pMod->v[i].z = OrginPt.z;
	}

	for(int i = 0 ;i< label_count;  ++i)
	{
		float delta = i*(inval + labelWidth);
		int idx = i*4;

		pMod->v[idx].y += delta;
		pMod->v[idx].z += 0;

		pMod->v[idx].tu = 1;
		pMod->v[idx].tv = 1;

		pMod->v[idx+1].y += delta;
		pMod->v[idx+1].z += -width;

		pMod->v[idx+1].tu = 1;
		pMod->v[idx+1].tv = 0;


		if(i == label_count - 1)
		{
			pMod->v[idx+2].y += height;
			pMod->v[idx+2].z += 0;
			pMod->v[idx+2].x = OrginPt.x;

			pMod->v[idx+2].tu = 1;
			pMod->v[idx+2].tv = 0;

			pMod->v[idx+3].y += height;
			pMod->v[idx+3].z += -width;
			pMod->v[idx+3].x = OrginPt.x;

			pMod->v[idx+3].tu = 0;
			pMod->v[idx+3].tv = 0;
		}
		else
		{
			pMod->v[idx+2].y += delta+labelWidth;
			pMod->v[idx+2].z += 0;

			pMod->v[idx+2].tu = 1;
			pMod->v[idx+2].tv = 0;

			pMod->v[idx+3].y += delta+labelWidth;
			pMod->v[idx+3].z += -width;

			pMod->v[idx+3].tu = 0;
			pMod->v[idx+3].tv = 0;
		}
		
		
		int i_idx = i*6;

		pMod->indecs[i_idx] = idx;
		pMod->indecs[i_idx+1] = idx+1;
		pMod->indecs[i_idx+2] = idx+2;

		pMod->indecs[i_idx+3] = idx+3;
		pMod->indecs[i_idx+4] = idx+2;
		pMod->indecs[i_idx+5] = idx+1;

	}
	
	pMod->viewsub[0].vstart = 0;
	pMod->viewsub[0].vcount = pMod->vcount ;

	pMod->viewsub[0].istart = 0;
	pMod->viewsub[0].icount = pMod->icount;

	pMod->viewsub[0].nTri_Num = label_count*2;
	pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST;

	pMod->viewsub[0].pMaterial = NULL;

	return TRUE;

}
	
BOOL SYModel_CreateZXLabels(SYModel* pMod,float width,float height, float3 OrginPt, UINT segment, float labelWidth)
{
	int label_count = segment + 1;

	float inval = (width - labelWidth*label_count)/segment;

	if(inval < 1.0f)
	{
		OrginPt.y += 1;

		return SYModel_CreateZXPlane(pMod,width,height,OrginPt);
	}

	pMod->vcount = 4*label_count;
	pMod->icount = 6*label_count ;
	pMod->nviewsub = 1;
	pMod->viewsub = new SYSubModel[pMod->nviewsub];

	pMod->v = new SYVertex3D[pMod->vcount];
	pMod->indecs = new DWORD[pMod->icount];


	for(int i = 0 ;i< pMod->vcount;  ++i)
	{
		pMod->v[i].tangent.x = 1.0f;
		pMod->v[i].tangent.y = 0.0f;

		pMod->v[i].ny = 1;
		pMod->v[i].nz = 0;
		pMod->v[i].nx = 0;

		pMod->v[i].x = OrginPt.x;
		pMod->v[i].y = OrginPt.y + 1;
		pMod->v[i].z = OrginPt.z;
	}
	
	for(int i = 0 ;i< label_count;  ++i)
	{
		float delta = i*(inval + labelWidth);
		int idx = i*4;
	
		pMod->v[idx].x += delta;
		pMod->v[idx].z += -height;
		
		pMod->v[idx].tu = 0;
		pMod->v[idx].tv = 0;

		pMod->v[idx+1].x += delta;
		pMod->v[idx+1].z += 0;

		pMod->v[idx+1].tu = 1;
		pMod->v[idx+1].tv = 0;

		if(i == label_count - 1)
		{
			pMod->v[idx+2].x += delta + labelWidth;
			pMod->v[idx+2].z += -height;
			pMod->v[idx+2].y = OrginPt.y;

			pMod->v[idx+2].tu = 0;
			pMod->v[idx+2].tv = 1;

			pMod->v[idx+3].x += delta + labelWidth;
			pMod->v[idx+3].z += 0;
			pMod->v[idx+3].y = OrginPt.y;

			pMod->v[idx+3].tu = 1;
			pMod->v[idx+3].tv = 1;
		}
		else
		{
			pMod->v[idx+2].x += delta + labelWidth;
			pMod->v[idx+2].z += -height;

			pMod->v[idx+2].tu = 0;
			pMod->v[idx+2].tv = 1;

			pMod->v[idx+3].x += delta + labelWidth;
			pMod->v[idx+3].z += 0;

			pMod->v[idx+3].tu = 1;
			pMod->v[idx+3].tv = 1;
		}
		

		int i_idx = i*6;

		pMod->indecs[i_idx] = idx;
		pMod->indecs[i_idx+1] = idx+1;
		pMod->indecs[i_idx+2] = idx+2;

		pMod->indecs[i_idx+3] = idx+3;
		pMod->indecs[i_idx+4] = idx+2;
		pMod->indecs[i_idx+5] = idx+1;

	}

	pMod->viewsub[0].vstart = 0;
	pMod->viewsub[0].vcount = pMod->vcount ;

	pMod->viewsub[0].istart = 0;
	pMod->viewsub[0].icount = pMod->icount;

	pMod->viewsub[0].nTri_Num = label_count*2;
	pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST;

	pMod->viewsub[0].pMaterial = NULL;

	return TRUE;
}
	
BOOL  SYModel_CreatePlaneLineModel(SYModel* pMod,float width,float z,float2* ptArray, int count)
{
	if(count == 1)
		return FALSE;

	//过滤无效点
	std::vector<float> lenth;
	std::vector<float2> points;

	lenth.push_back(0);
	points.push_back(ptArray[0]);
	
	for(int i = 1; i< count;i++)
	{		
		float dx =  ptArray[i].x -  ptArray[i-1].x;
		float dy =  ptArray[i].y -  ptArray[i-1].y;

		float l = sqrt(dx*dx + dy*dy);

		if(l < FLT_EPSILON)
			continue;

		lenth.push_back(l);
		points.push_back(ptArray[i]);
	}

	int seg = int(points.size() - 1);

	if(seg == 0)
		return FALSE;
	
	float sum_l = 0;

	for_each(lenth.begin(),lenth.end(),[&](float l){sum_l += l;});
	for_each(lenth.begin(),lenth.end(),[=](float& l){l = l/sum_l;});
		
	pMod->vcount = 8*seg;
	pMod->icount = 12*seg ;
	pMod->nviewsub = 1;
	pMod->viewsub = new SYSubModel[pMod->nviewsub];

	float z0 = z - width/2.0;
	float z1 = z0 + width;

	pMod->v = new SYVertex3D[pMod->vcount];
	pMod->indecs = new DWORD[pMod->icount];
	
	for(int i = 0; i< seg;i++)
	{
		int idx = i*4;

		float dx =  points[i+1].x -  points[i].x;
		float dy =  points[i+1].y -  points[i].y;

		float l = sqrt(dx*dx + dy*dy);

		if(l < FLT_EPSILON)
			continue;	

		dx = dx/l;
		dy = dy/l;

		pMod->v[idx].x = points[i].x;
		pMod->v[idx].y = points[i].y;
		pMod->v[idx].z = z0;

		pMod->v[idx].nx = -dy;
		pMod->v[idx].ny = dx;
		pMod->v[idx].nz = 0;

		pMod->v[idx].tv = 0;
		pMod->v[idx].tu = lenth[i];
		
		pMod->v[idx+1].x = points[i].x;
		pMod->v[idx+1].y = points[i].y;
		pMod->v[idx+1].z = z1;

		pMod->v[idx+1].nx = -dy;
		pMod->v[idx+1].ny = dx;
		pMod->v[idx+1].nz = 0;

		pMod->v[idx+1].tv = 1;
		pMod->v[idx+1].tu = lenth[i];

		pMod->v[idx+2].x = points[i+1].x;
		pMod->v[idx+2].y = points[i+1].y;
		pMod->v[idx+2].z = z0;

		pMod->v[idx+2].nx = -dy;
		pMod->v[idx+2].ny = dx;
		pMod->v[idx+2].nz = 0;

		pMod->v[idx+2].tv = 0;
		pMod->v[idx+2].tu = lenth[i+1];
		
		pMod->v[idx+3].x = points[i+1].x;
		pMod->v[idx+3].y = points[i+1].y;
		pMod->v[idx+3].z = z1;

		pMod->v[idx+3].nx = -dy;
		pMod->v[idx+3].ny = dx;
		pMod->v[idx+3].nz = 0;

		pMod->v[idx+3].tv = 1;
		pMod->v[idx+3].tu = lenth[i+1];

		int i_idx = i*6;

		pMod->indecs[i_idx] = idx;
		pMod->indecs[i_idx+1] = idx+1;
		pMod->indecs[i_idx+2] = idx+2;

		pMod->indecs[i_idx+3] = idx+3;
		pMod->indecs[i_idx+4] = idx+2;
		pMod->indecs[i_idx+5] = idx+1;
	}

	for(int i = 0; i<4*seg;++i)
	{
		int idx = i + seg*4;
		pMod->v[idx]= pMod->v[i];
		pMod->v[idx].nx = - pMod->v[idx].nx;
		pMod->v[idx].ny = - pMod->v[idx].ny;
	}

	for(int i = 0; i< seg;i++)
	{
		int v_idx = i*4 + seg*4;
		int i_idx = i*6 + seg*6;

		pMod->indecs[i_idx] = v_idx;
		pMod->indecs[i_idx+1] = v_idx+2;
		pMod->indecs[i_idx+2] = v_idx+1;

		pMod->indecs[i_idx+3] = v_idx+2;
		pMod->indecs[i_idx+4] = v_idx+3;
		pMod->indecs[i_idx+5] = v_idx+1;
	}
	
	pMod->viewsub[0].vstart = 0;
	pMod->viewsub[0].vcount = pMod->vcount ;

	pMod->viewsub[0].istart = 0;
	pMod->viewsub[0].icount = pMod->icount;

	pMod->viewsub[0].nTri_Num = seg*4;
	pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST;

	pMod->viewsub[0].pMaterial = NULL;




	return TRUE;
}

BOOL  SYModel_CreatePlaneLineModel2(SYModel* pMod,float height,float z,float2* ptArray, int count)
{
	if(count == 1)
		return FALSE;

	//过滤无效点
	std::vector<float> lenth;
	std::vector<float2> points;
	std::vector<float2> directs;

	lenth.push_back(0);
	points.push_back(ptArray[0]);

	for(int i = 1; i< count;i++)
	{		
		float dx =  ptArray[i].x -  ptArray[i-1].x;
		float dy =  ptArray[i].y -  ptArray[i-1].y;

		float l = sqrt(dx*dx + dy*dy);

		if(l < FLT_EPSILON)
			continue;

		lenth.push_back(l);
		points.push_back(ptArray[i]);

		float2 dir;

		dir.x = dx/l;
		dir.y = dy/l;

		directs.push_back(dir);
	}

	int seg = int(points.size() - 1);
	if(seg == 0)
		return FALSE;

	//计算长度
	float sum_l = 0;

	std::vector<float> sum;
	std::vector<float> tu;

	for_each(lenth.begin(),lenth.end(),
		[&](float l){	
			sum_l += l; 
			sum.push_back(sum_l);});

	for_each(sum.begin(),sum.end(),
			[&](float& l){
				float t = l/sum_l;
				tu.push_back(t);});

	int face_point_count = 2; //截面点为2个
	int face_count = (int)points.size(); //截面数与数据点数一样

	float3* faces = new float3[face_count*face_point_count];

	float3* ppt = faces;
	int idx = 0;

	//第0个面
	{
		float3 pt0,pt1;

		pt0.x = points[0].x;
		pt0.y = points[0].y;
		pt0.z = z;

		pt1 = pt0;

		float2 dir3;
		float l = height/2.0f;


		dir3.x = -directs[0].y;
		dir3.y = directs[0].x;

		pt1.x += l*dir3.x;
		pt0.x -= l*dir3.x;

		pt1.y += l*dir3.y;
		pt0.y -= l*dir3.y;
		

// 		if(abs(directs[0].x)<FLT_EPSILON)
// 		{
// 			pt1.y += height/2.0f;
// 			pt0.y -= height/2.0f;
// 		}
// 		else
// 		{
// 			float dy = height/directs[0].x/2.0f;
// 
// 			pt1.y += dy;
// 			pt0.y -= dy;
// 		}
// 		
		
		ppt[idx++] = pt0;
		ppt[idx++] = pt1;
	
	}


	for(int i = 1; i< face_count-1;i++)
	{		
		float3 pt0,pt1;

		pt0.x = points[i].x;
		pt0.y = points[i].y;
		pt0.z = z;
		
		pt1 =  pt0;

		float2 dir0 = directs[i-1];
		float2 dir1 = directs[i];

		float dotv = (dir0.x*dir1.x + dir1.y*dir0.y);

		float2 dir3 = dir1;
		dir3.x += -dir0.x;
		dir3.y += -dir0.y;

		float l = sqrt(dir3.x*dir3.x + dir3.y*dir3.y);
		dir3.x /= l;
		dir3.y /= l;

		if(dotv > 0 )
		{			
			float theta =  PAI - acosf(dotv);
			l = height/sinf(theta/2.0f)/2.0f;

			if(dir3.y<0)
			{
				dir3.x = -dir3.x;
				dir3.y = -dir3.y;
			}

			pt1.x += l*dir3.x;
			pt1.y += l*dir3.y;

			pt0.x -= l*dir3.x;
			pt0.y -= l*dir3.y;

		}
		else if(dotv < 0 )
		{
			float theta = PAI + acosf(dotv) ;
			l = height/sinf(theta/2.0f)/2.0f;

			if(dir3.y<0)
			{
				dir3.x = -dir3.x;
				dir3.y = -dir3.y;
			}

			pt1.x += l*dir3.x;
			pt1.y += l*dir3.y;

			pt0.x -= l*dir3.x;
			pt0.y -= l*dir3.y;

		}
		else
		{
			pt1.y += height/2.0f;
			pt0.y -= height/2.0f;
		}
	

		ppt[idx++]  = pt0;
		ppt[idx++]  = pt1;
	
	}

	//最后一个面

	{
		float3 pt0,pt1;

		pt0.x = points[face_count-1].x;
		pt0.y = points[face_count-1].y;
		pt0.z = z;

		pt1  = pt0;

		float2 dir3;
		float l = height/2.0f;

		dir3.x = -directs[face_count-2].y;
		dir3.y = directs[face_count-2].x;

		
		pt0.x -= l*dir3.x;
		pt0.y -= l*dir3.y;

		pt1.y += l*dir3.y;
		pt1.x += l*dir3.x;
		

// 		if(abs(directs[0].x)<FLT_EPSILON)
// 		{
// 			pt1.y += height/2.0f;
// 			pt0.y -= height/2.0f;
// 		}
// 		else
// 		{
// 			float dy = height/directs[face_count-2].x/2.0f;
// 			pt1.y += dy;
// 			pt0.y -= dy;
// 		}
		
		ppt[idx++]  = pt0;
		ppt[idx++]  = pt1;

	}

	//建模

	int tri_count = seg*4;

	pMod->vcount = face_count*4;
	pMod->icount = tri_count*3 ;
	pMod->nviewsub = 1;
	pMod->viewsub = new SYSubModel[pMod->nviewsub];

	pMod->v = new SYVertex3D[pMod->vcount];
	pMod->indecs = new DWORD[pMod->icount];

	int v_idx = 0;
	int i_idx = 0;

	for(int i = 0; i<face_count*2;++i)
	{	
		pMod->v[v_idx].x =  faces[i].x;
		pMod->v[v_idx].y =  faces[i].y;
		pMod->v[v_idx].z =  faces[i].z;
		
		pMod->v[v_idx].nx = 0;
		pMod->v[v_idx].ny = 0;
		pMod->v[v_idx].nz = -1;

		pMod->v[v_idx].tu = tu[i>>1];
		pMod->v[v_idx].tv = float(i%2);

		++v_idx;
	}

	for(int i = 0; i<face_count*2;++i)
	{	
		pMod->v[v_idx].x =  faces[i].x;
		pMod->v[v_idx].y =  faces[i].y;
		pMod->v[v_idx].z =  faces[i].z;

		pMod->v[v_idx].nx = 0;
		pMod->v[v_idx].ny = 0;
		pMod->v[v_idx].nz = 1;

		pMod->v[v_idx].tu = tu[i>>1];
		pMod->v[v_idx].tv = float(i%2);

		++v_idx;
	}

	for(int i = 0; i<seg;++i)
	{
		pMod->indecs[i_idx++] = i*2;
		pMod->indecs[i_idx++] = i*2+1;
		pMod->indecs[i_idx++] = i*2+2;

		pMod->indecs[i_idx++] = i*2+1;
		pMod->indecs[i_idx++] = i*2+3;
		pMod->indecs[i_idx++] = i*2+2;
	}

	for(int i = 0; i<seg;++i)
	{
		pMod->indecs[i_idx++] = face_count*2 + i*2;
		pMod->indecs[i_idx++] = face_count*2 + i*2+2;
		pMod->indecs[i_idx++] = face_count*2 + i*2+1;

		pMod->indecs[i_idx++] = face_count*2 + i*2+1;
		pMod->indecs[i_idx++] = face_count*2 + i*2+2;
		pMod->indecs[i_idx++] = face_count*2 + i*2+3;
	}
	
	delete[] faces;
	faces = NULL;

	pMod->viewsub[0].vstart = 0;
	pMod->viewsub[0].vcount = pMod->vcount ;

	pMod->viewsub[0].istart = 0;
	pMod->viewsub[0].icount = pMod->icount;
	pMod->viewsub[0].nTri_Num = tri_count;
	pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST;

	pMod->viewsub[0].pMaterial = NULL;

	return TRUE;
}


BOOL  SYModel_CreateCubeLineModel(SYModel* pMod,float width,float height,float z,float2* ptArray, int count)
{
	if(count == 1)
		return FALSE;

	//过滤无效点
	std::vector<float> lenth;
	std::vector<float2> points;
	std::vector<float2> directs;

	lenth.push_back(0);
	points.push_back(ptArray[0]);

	for(int i = 1; i< count;i++)
	{		
		float dx =  ptArray[i].x -  ptArray[i-1].x;
		float dy =  ptArray[i].y -  ptArray[i-1].y;

		float l = sqrt(dx*dx + dy*dy);

		if(l < FLT_EPSILON)
			continue;

		lenth.push_back(l);
		points.push_back(ptArray[i]);

		float2 dir;

		dir.x = dx/l;
		dir.y = dy/l;

		directs.push_back(dir);
	}

	int seg = int(points.size() - 1);
	if(seg == 0)
		return FALSE;
		
	
	//计算长度
	float sum_l = 0;

	std::vector<float> sum;
	std::vector<float> tu;

	for_each(lenth.begin(),lenth.end(),
		[&](float l){	
			sum_l += l; 
			sum.push_back(sum_l);});

	for_each(sum.begin(),sum.end(),
		[&](float& l){
		float t = l/sum_l;
		tu.push_back(t);});

	int face_point_count = 4; //截面点为4个
	int face_count = (int)points.size(); //截面数与数据点数一样

	float3* faces = new float3[face_count*face_point_count];
	
	float z0 = z - width/2.0f;
	float z1 = z0 + width;

	float3* ppt = faces;
	int idx = 0;

	//第0个面
	{
		float3 pt0,pt1,pt2,pt3;
						
		pt0.x = points[0].x;
		pt0.y = points[0].y;

		pt1 = pt2 = pt3 = pt0;

		pt0.z = z0;
		pt1.z = z1;
		pt2.z = z1;
		pt3.z = z0;


		float2 dir3;
		float l = height/2.0f;
		
		dir3.x = -directs[0].y;
		dir3.y = directs[0].x;

		pt0.x -= l*dir3.x;
		pt0.y -= l*dir3.y;

		pt1.x -= l*dir3.x;
		pt1.y -= l*dir3.y;
		
		pt2.x += l*dir3.x;
		pt2.y += l*dir3.y;

		pt3.x += l*dir3.x;
		pt3.y += l*dir3.y;


		ppt[idx++] = pt0;
		ppt[idx++] = pt1;
		ppt[idx++] = pt2;
		ppt[idx++] = pt3;
	}


	for(int i = 1; i< face_count-1;i++)
	{		
		float3 pt0,pt1,pt2,pt3;

		pt0.x = points[i].x;
		pt0.y = points[i].y;

		pt1 = pt2 = pt3 = pt0;

		pt0.z = z0;
		pt1.z = z1;
		pt2.z = z1;
		pt3.z = z0;
		
		float2 dir0 = directs[i-1];
		float2 dir1 = directs[i];

		float dotv = (dir0.x*dir1.x + dir1.y*dir0.y);
		
		float2 dir3 = dir1;
		dir3.x += -dir0.x;
		dir3.y += -dir0.y;

		float l = sqrt(dir3.x*dir3.x + dir3.y*dir3.y);
		dir3.x /= l;
		dir3.y /= l;

		if(dotv > 0 )
		{			
			float theta =  PAI - acosf(dotv);
			l = height/sinf(theta/2.0f)/2.0f;

			if(dir3.y<0)
			{
				dir3.x = -dir3.x;
				dir3.y = -dir3.y;
			}


			pt2.x += l*dir3.x;
			pt3.x += l*dir3.x;

			pt2.y += l*dir3.y;
			pt3.y += l*dir3.y;


			pt0.x -= l*dir3.x;
			pt1.x -= l*dir3.x;

			pt0.y -= l*dir3.y;
			pt1.y -= l*dir3.y;

		}
		else if(dotv < 0 )
		{
			float theta = PAI + acosf(dotv) ;
			l = height/sinf(theta/2.0f)/2.0f;

			if(dir3.y<0)
			{
				dir3.x = -dir3.x;
				dir3.y = -dir3.y;
			}

			pt2.x += l*dir3.x;
			pt3.x += l*dir3.x;

			pt2.y += l*dir3.y;
			pt3.y += l*dir3.y;

			pt0.x -= l*dir3.x;
			pt1.x -= l*dir3.x;

			pt0.y -= l*dir3.y;
			pt1.y -= l*dir3.y;

		}
		else
		{
			float dy = height/2.0f;

			pt2.y += dy;
			pt3.y += dy;

			pt0.y -= dy;
			pt1.y -= dy;
		}


		ppt[idx++]  = pt0;
		ppt[idx++]  = pt1;
		ppt[idx++]  = pt2;
		ppt[idx++]  = pt3;

	}

	//最后一个面
	
	{
		float3 pt0,pt1,pt2,pt3;

		pt0.x = points[face_count-1].x;
		pt0.y = points[face_count-1].y;

		pt1 = pt2 = pt3 = pt0;

		pt0.z = z0;
		pt1.z = z1;
		pt2.z = z1;
		pt3.z = z0;

		float2 dir3;
		float l = height/2.0f;

		dir3.x = -directs[face_count-2].y;
		dir3.y = directs[face_count-2].x;

		pt0.x -= l*dir3.x;
		pt0.y -= l*dir3.y;

		pt1.x -= l*dir3.x;
		pt1.y -= l*dir3.y;

		pt2.x += l*dir3.x;
		pt2.y += l*dir3.y;

		pt3.x += l*dir3.x;
		pt3.y += l*dir3.y;

		ppt[idx++]  = pt0;
		ppt[idx++]  = pt1;
		ppt[idx++]  = pt2;
		ppt[idx++]  = pt3;
	}


	//建模
	int tri_count = seg*8 + 4;

	pMod->vcount = 8 + seg*16;
	pMod->icount = tri_count*3 ;
	pMod->nviewsub = 1;
	pMod->viewsub = new SYSubModel[pMod->nviewsub];

// 	float z0 = z - width/2.0;
// 	float z1 = z0 + width;

	pMod->v = new SYVertex3D[pMod->vcount];
	pMod->indecs = new DWORD[pMod->icount];

	int v_idx = 0;
	int i_idx = 0;

	//第0个面
	{
		pMod->indecs[i_idx++] = v_idx;
		pMod->indecs[i_idx++] = v_idx+1;
		pMod->indecs[i_idx++] = v_idx+2;

		pMod->indecs[i_idx++] = v_idx;
		pMod->indecs[i_idx++] = v_idx+2;
		pMod->indecs[i_idx++] = v_idx+3;
				
		for(int i = 0;i<face_point_count;++i)
		{
			pMod->v[v_idx].x = faces[i].x;
			pMod->v[v_idx].y = faces[i].y;
			pMod->v[v_idx].z = faces[i].z;

			pMod->v[v_idx].nx = -1;
			pMod->v[v_idx].ny = 0;
			pMod->v[v_idx].nz = 0;

			pMod->v[v_idx].tu = 0;
			pMod->v[v_idx].tv = 0;

			if( i < 2 )
				pMod->v[v_idx].tv = 1;

			++v_idx;
		}

	}

	for(int i = 0; i< seg;++i)
	{
		float3* f0 = faces + i*face_point_count; 
		float3* f1 = faces + (i+1)*face_point_count; 	

		for(int k = 0;k<face_point_count;++k)
		{
			int v_base = v_idx;

			float3 pt0,pt1,pt2;

			int l = (k+1)%face_point_count;

			float3Set(&pt0,f0[k].x,f0[k].y,f0[k].z);
			float3Set(&pt1,f0[l].x,f0[l].y,f0[l].z);
			float3Set(&pt2,f1[k].x,f0[k].y,f0[k].z);

			float3 n1 = pt1 - pt0;
			float3 n0 = pt2 - pt0;
			float3 nv;

			float3CrossProduct(&nv,&n0,&n1);
			float3Normalize(&nv,&nv);

			pMod->v[v_idx].x = f0[k].x;
			pMod->v[v_idx].y = f0[k].y;
			pMod->v[v_idx].z = f0[k].z;

			pMod->v[v_idx].nx = nv.x;
			pMod->v[v_idx].ny = nv.y;
			pMod->v[v_idx].nz = nv.z;

			pMod->v[v_idx].tu = tu[i];
			pMod->v[v_idx].tv = 0;
			++v_idx;

			pMod->v[v_idx].x = f0[l].x;
			pMod->v[v_idx].y = f0[l].y;
			pMod->v[v_idx].z = f0[l].z;

			pMod->v[v_idx].nx = nv.x;
			pMod->v[v_idx].ny = nv.y;
			pMod->v[v_idx].nz = nv.z;

			pMod->v[v_idx].tu = tu[i];
			pMod->v[v_idx].tv = 1;
			++v_idx;

			pMod->v[v_idx].x = f1[l].x;
			pMod->v[v_idx].y = f1[l].y;
			pMod->v[v_idx].z = f1[l].z;

			pMod->v[v_idx].nx = nv.x;
			pMod->v[v_idx].ny = nv.y;
			pMod->v[v_idx].nz = nv.z;

			pMod->v[v_idx].tu = tu[i+1];
			pMod->v[v_idx].tv = 1;
			++v_idx;
			
			pMod->v[v_idx].x = f1[k].x;
			pMod->v[v_idx].y = f1[k].y;
			pMod->v[v_idx].z = f1[k].z;

			pMod->v[v_idx].nx = nv.x;
			pMod->v[v_idx].ny = nv.y;
			pMod->v[v_idx].nz = nv.z;

			pMod->v[v_idx].tu = tu[i+1];
			pMod->v[v_idx].tv = 0;
			++v_idx;
						
			pMod->indecs[i_idx++] = v_base;
			pMod->indecs[i_idx++] = v_base+2;
			pMod->indecs[i_idx++] = v_base+1;

			pMod->indecs[i_idx++] = v_base;
			pMod->indecs[i_idx++] = v_base+3;
			pMod->indecs[i_idx++] = v_base+2;

		}

	}


	//最后个面
	{
		pMod->indecs[i_idx++] = v_idx;
		pMod->indecs[i_idx++] = v_idx+2;
		pMod->indecs[i_idx++] = v_idx+1;

		pMod->indecs[i_idx++] = v_idx;
		pMod->indecs[i_idx++] = v_idx+3;
		pMod->indecs[i_idx++] = v_idx+2;

		for(int i = 0;i<face_point_count;++i)
		{
			float3* pt = faces + face_point_count*(face_count-1) + i;

			pMod->v[v_idx].x = pt->x;
			pMod->v[v_idx].y = pt->y;
			pMod->v[v_idx].z = pt->z;

			pMod->v[v_idx].nx = 1;
			pMod->v[v_idx].ny = 0;
			pMod->v[v_idx].nz = 0;

			pMod->v[v_idx].tu = 1;
			pMod->v[v_idx].tv = 0;

			if( i < 2 )
				pMod->v[v_idx].tv = 1;

			++v_idx;
		}

	}

	delete[] faces;
	faces = NULL;

	pMod->viewsub[0].vstart = 0;
	pMod->viewsub[0].vcount = pMod->vcount ;

	pMod->viewsub[0].istart = 0;
	pMod->viewsub[0].icount = pMod->icount;

	pMod->viewsub[0].nTri_Num = tri_count;
	pMod->viewsub[0].nTri_Type = SY_TRIANGLE_LIST;

	pMod->viewsub[0].pMaterial = NULL;


	return TRUE;
}




#define OBJ_COMMENT			_T("#")
#define OBJ_SPACE					_T(" ")
#define OBJ_FACESPACE			_T("/")
#define OBJ_SYLIB					_T("mtllib ")
#define OBJ_VERTICES				_T("v ")
#define OBJ_NORMALS				_T("vn ")
#define OBJ_COORDS				_T("vt ")
#define OBJ_MESH					_T("g ")
#define OBJ_MATRIAL				_T("usemtl ")
#define OBJ_SUBSET					_T("s ")
#define OBJ_FACE						_T("f ")

#define SYL_MATRIAL				_T("newmtl ")
#define SYL_ALAPH					_T("d ")
#define SYL_ALAPH2				_T("Tr ")
#define SYL_POWER				_T("illum ")
#define SYL_AMBIENT				_T("Ka ")
#define SYL_DIFFUSE				_T("Kd ")
#define SYL_SPECULAR			_T("Ks ")
#define SYL_USE_SPECULAR	_T("illum ")
#define SYL_EMISSIVE				_T("Ke ")
#define SYL_AMBIENT_MAP	_T("map_Ka ")
#define SYL_DIFFUSE_MAP		_T("map_Kd ")
#define SYL_SPECULAR_MAP	_T("map_Ks ")
#define SYL_EMISSIVE_MAP	_T("map_Ke ")
#define SYL_BUMP_MAP			_T("map_Bump ")

struct OBJMATLIB
{
	CString						Name;
	//D3DMATERIAL9		MatD3D;
	float4   Diffuse;        /* Diffuse color RGBA */
	float4   Ambient;        /* Ambient color RGB */
	float4   Specular;       /* Specular 'shininess' */
	float4   Emissive;       /* Emissive color RGB */
	float           Power;          /* Sharpness if specular highlight */

	CString						MapKa;
	CString						MapKd;
	CString						MapKs;
	CString						MapKe;
	CString						MapBump;

	OBJMATLIB() 
	{
		ZeroMemory(&Ambient,sizeof(float4));
		ZeroMemory(&Specular,sizeof(float4));
		ZeroMemory(&Emissive,sizeof(float4));
		Diffuse.x = 1;
		Diffuse.y = 1;
		Diffuse.z = 1;
		Diffuse.w = 1.f;
		Power = 1;
	};

	OBJMATLIB & operator = (const OBJMATLIB & other)
	{
		Name = other.Name;			
		Diffuse = other.Diffuse;
		Ambient = other.Ambient;
		Specular = other.Specular;
		Emissive = other.Emissive;

		MapKa = other.MapKa;
		MapKd = other.MapKd;
		MapKs = other.MapKs;
		MapKe = other.MapKe;
		MapBump = other.MapBump;
		Power = other.Power;
		return *this;
	};
};

TCHAR*  DeleteLeadingSpace(TCHAR* pText,int nMaxCount)
{
	if(nMaxCount <= 0)
		nMaxCount = _tcslen(pText);
	for(int i = 0; i < nMaxCount;i++)
	{
		if(pText[i] != ' ')
			return pText;
	}
	return NULL;
}

BOOL IsKey(LPCTSTR szText,LPCTSTR szKey)
{
	if(_tcsncmp(szText,szKey,_tcslen(szKey)) == 0)
		return TRUE;
	return FALSE;
}

BOOL GetKey(CString& szText,LPCTSTR szKey)
{
	szText = szText.Right(szText.GetLength() - _tcslen(szKey));
	szText.Trim();
	return TRUE;
}

BOOL GetKey(CString& szText,LPCTSTR szKey,std::vector<CString>& ArgList)
{
	szText = szText.Right(szText.GetLength() - _tcslen(szKey));
	szText.Trim();

	ArgList.clear();
	CString szArg;
	int nPos = szText.Find(OBJ_SPACE);
	while( nPos > -1 )
	{
		szArg = szText.Left(nPos);
		ArgList.push_back(szArg);
		szText = szText.Right(szText.GetLength() - szArg.GetLength() - _tcslen(OBJ_SPACE));		
		szText.TrimLeft();
		nPos = szText.Find(OBJ_SPACE);
	}
	ArgList.push_back(szText);

	return TRUE;
}
BOOL GetFace(CString& szText, std::vector<int>& ArgList)
{
	ArgList.clear();
	CString szArg;
	int nPos = szText.Find(OBJ_FACESPACE);
	while( nPos > -1 )
	{
		szArg = szText.Left(nPos);
		ArgList.push_back(_ttoi(szArg));
		szText = szText.Right(szText.GetLength() - szArg.GetLength() - _tcslen(OBJ_FACESPACE));		
		szText.TrimLeft();
		nPos = szText.Find(OBJ_FACESPACE);
	}
	ArgList.push_back(_ttoi(szText));

	return TRUE;
}

SYVertex3D TransFace(vector<int>& FaceList, 
	vector<float3>& PostionList,
	vector<float3>& NormalList,
	vector<float2>& TextureCoordList,
	int nPostionListCount,
	int nNormalListCount,
	int nTextureCoordList)
{
	SYVertex3D v;				
	ZeroMemory(&v,sizeof(SYVertex3D));

	if(FaceList.size() > 0)
	{
		if(FaceList[0] > 0)
		{
			int n = FaceList[0]-1;
			if(n < nPostionListCount && n >= 0)
				v.vDyn = PostionList[n];
			else
				float3Set(&v.vDyn,0,0,0);
		}
		else
		{
			int n = nPostionListCount + FaceList[0];
			if(n < nPostionListCount && n >= 0)
				v.vDyn = PostionList[n];
			else
				float3Set(&v.vDyn,0,0,0);
		}
	}
	else
	{
		assert(0);
		float3Set(&v.vDyn,0,0,0);
	}

	if(FaceList.size() > 1)
	{
		if(FaceList[1] > 0)
		{
			int n = FaceList[1]-1;
			if(n < nTextureCoordList && n >= 0)
			{
				v.tu = TextureCoordList[n].x;
				v.tv = TextureCoordList[n].y;
			}
			else
			{
				v.tu = 0;
				v.tv = 0;
			}
		}
		else
		{
			int n = nTextureCoordList + FaceList[1];
			if(n < nTextureCoordList && n >= 0)
			{
				v.tu = TextureCoordList[n].x;
				v.tv = TextureCoordList[n].y;
			}
			else
			{
				v.tu = 0;
				v.tv = 0;
			}
		}
	}

	if(FaceList.size() > 2)
	{
		if(FaceList[2] > 0)
		{
			int n = FaceList[2]-1;
			if(n < nNormalListCount && n >= 0)
				v.vNorm = NormalList[n];
		}
		else

		{
			int n = nNormalListCount + FaceList[2];
			if(n < nNormalListCount && n >= 0)
				v.vNorm = NormalList[n];
		}
	}
	if(v.tu > 1 || v.tu < 0 || v.tv > 1 || v.tv < 0)
	{
		//	assert(0);
		v.tu = v.tu - floor(v.tu);
		v.tv = v.tv - floor(v.tv);
	}
	return v;
}

BOOL ReadMatLib(CString& szLibFile,vector<OBJMATLIB>& m_pMatLibList)
{
	FILE* pLibFile = NULL;
	_tfopen_s(&pLibFile,szLibFile,_T("r"));
	if(!pLibFile)
		return FALSE;

	OBJMATLIB* pLastMATLIB = NULL;
	BOOL bUseSpecular = TRUE;
	CString szPath = CommonFunction::GetFilePath(szLibFile);
	vector<CString> ArgList;
	TCHAR szLine[2048];
	while(_fgetts(szLine,1024,pLibFile))
	{
		CString szText(szLine);
		szText.Trim();

		if(IsKey(szText,SYL_MATRIAL))
		{
			GetKey(szText,SYL_MATRIAL);

			pLastMATLIB = NULL;
			for(int i=0;i<m_pMatLibList.size();i++)
			{
				if(szText.CompareNoCase(m_pMatLibList[i].Name) == 0)
				{
					pLastMATLIB = &m_pMatLibList[i];
					break;
				}
			}

			continue;
		}

		if(!pLastMATLIB)
			continue;

		if(IsKey(szText,SYL_ALAPH))
		{
			GetKey(szText,SYL_ALAPH);
			pLastMATLIB->Diffuse.w = _tstof(szText);
			continue;
		}

		if(IsKey(szText,SYL_POWER))
		{
			GetKey(szText,SYL_POWER);
			pLastMATLIB->Power = _tstof(szText);
			continue;
		}

		if(IsKey(szText,SYL_AMBIENT))
		{
			GetKey(szText,SYL_AMBIENT,ArgList);
			if(ArgList.size() > 0)
				pLastMATLIB->Ambient.x = _tstof(ArgList[0]);
			if(ArgList.size() > 1)
				pLastMATLIB->Ambient.y = _tstof(ArgList[1]);
			if(ArgList.size() > 2)
				pLastMATLIB->Ambient.z = _tstof(ArgList[2]);
			continue;
		}

		if(IsKey(szText,SYL_DIFFUSE))
		{
			GetKey(szText,SYL_DIFFUSE,ArgList);
			if(ArgList.size() > 0)
				pLastMATLIB->Diffuse.x = _tstof(ArgList[0]);
			if(ArgList.size() > 1)
				pLastMATLIB->Diffuse.y = _tstof(ArgList[1]);
			if(ArgList.size() > 2)
				pLastMATLIB->Diffuse.z = _tstof(ArgList[2]);
			continue;
		}

		if(IsKey(szText,SYL_SPECULAR))
		{
			GetKey(szText,SYL_SPECULAR,ArgList);
			if(ArgList.size() > 0)
				pLastMATLIB->Specular.x = _tstof(ArgList[0]);
			if(ArgList.size() > 1)
				pLastMATLIB->Specular.y = _tstof(ArgList[1]);
			if(ArgList.size() > 2)
				pLastMATLIB->Specular.z = _tstof(ArgList[2]);
			continue;
		}

		if(IsKey(szText,SYL_USE_SPECULAR))
		{
			GetKey(szText,SYL_USE_SPECULAR);
			bUseSpecular = _tstof(szText) == 2;
			continue;
		}

		if(IsKey(szText,SYL_EMISSIVE))
		{
			GetKey(szText,SYL_EMISSIVE,ArgList);
			if(ArgList.size() > 0)
				pLastMATLIB->Emissive.x = _tstof(ArgList[0]);
			if(ArgList.size() > 1)
				pLastMATLIB->Emissive.y = _tstof(ArgList[1]);
			if(ArgList.size() > 2)
				pLastMATLIB->Emissive.z = _tstof(ArgList[2]);
			continue;
		}

		if(IsKey(szText,SYL_AMBIENT_MAP))
		{
			GetKey(szText,SYL_AMBIENT_MAP);
			if(!szText.IsEmpty())
			{
				if(szText.Mid(1,1) != _T(":") && szText.Left(2) != _T("\\\\"))
					szText = szPath + szText;
				pLastMATLIB->MapKa = szText;
			}
			continue;
		}

		if(IsKey(szText,SYL_DIFFUSE_MAP))
		{
			GetKey(szText,SYL_DIFFUSE_MAP);
			if(!szText.IsEmpty())
			{
				if(szText.Mid(1,1) != _T(":") && szText.Left(2) != _T("\\\\"))
					szText = szPath + szText;
				pLastMATLIB->MapKd = szText;
			}
			continue;
		}

		if(IsKey(szText,SYL_SPECULAR_MAP))
		{
			GetKey(szText,SYL_SPECULAR_MAP);
			if(!szText.IsEmpty())
			{
				if(szText.Mid(1,1) != _T(":") && szText.Left(2) != _T("\\\\"))
					szText = szPath + szText;
				pLastMATLIB->MapKs = szText;
			}
			continue;
		}

		if(IsKey(szText,SYL_EMISSIVE_MAP))
		{
			GetKey(szText,SYL_EMISSIVE_MAP);
			if(!szText.IsEmpty())
			{
				if(szText.Mid(1,1) != _T(":") && szText.Left(2) != _T("\\\\"))
					szText = szPath + szText;
				pLastMATLIB->MapKe = szText;
			}
			continue;
		}
		if(IsKey(szText,SYL_BUMP_MAP))
		{
			GetKey(szText,SYL_BUMP_MAP);
			if(!szText.IsEmpty())
			{
				if(szText.Mid(1,1) != _T(":") && szText.Left(2) != _T("\\\\"))
					szText = szPath + szText;
				pLastMATLIB->MapBump = szText;
			}
			continue;
		}	
	}

	fclose(pLibFile);
	return TRUE;
}

SYModel* MakeModel( SYModel* pNewModel, 
	SYVertex3D* pVB,
	DWORD* pIB,
	DWORD* pAB,
	UINT nV,
	UINT nF,
	BOOL bCalcNormal)
{
	//SYModel* pNewModel = new SYModel;
	//sy_init_moddef(pNewModel);
	pNewModel->vcount = nV;
	pNewModel->v = new SYVertex3D[nV];
	memcpy(pNewModel->v,pVB,sizeof(SYVertex3D) * nV);
	pNewModel->icount = nF * 3;
	pNewModel->indecs = new DWORD[pNewModel->icount];
	memset(pNewModel->indecs,0,sizeof(DWORD) * pNewModel->icount);
	vector<int> subCount;
	for(int i = 0; i < nF; i++)
	{
		int nMatIndex = pAB[i];
		if(subCount.size() <= nMatIndex)
		{
			subCount.resize(nMatIndex + 1);
		}
		subCount[nMatIndex]++;
	}
	pNewModel->nviewsub = subCount.size();
	pNewModel->viewsub = new SYSubModel[pNewModel->nviewsub];
	memset(pNewModel->viewsub,0,sizeof(SYSubModel) * pNewModel->nviewsub);
	int vCount = 0;
	for(int i = 0; i < pNewModel->nviewsub;i++)
	{
		pNewModel->viewsub[i].nTri_Type = SY_TRIANGLE_LIST;
		pNewModel->viewsub[i].istart = vCount;
		pNewModel->viewsub[i].nTri_Num = 0;
		vCount += subCount[i] * 3;
	}

	for (int i = 0;i < nF; i++)
	{
		int nOffset = pNewModel->viewsub[pAB[i]].istart + 3 * pNewModel->viewsub[pAB[i]].nTri_Num;
		pNewModel->indecs[nOffset + 0] = pIB[i * 3 + 0];
		pNewModel->indecs[nOffset + 1] = pIB[i * 3 + 2];
		pNewModel->indecs[nOffset + 2] = pIB[i * 3 + 1];
		pNewModel->viewsub[pAB[i]].nTri_Num++;
	}
	//SYCreateTangent(pNewModel->v,nV,pNewModel->indecs,pNewModel->icount,0,SY_TRIANGLE_LIST);
	return pNewModel;
}
