#include "stdafx.h"
#include "DataRender/SimpleSpaceConvert.h"
#include "ObjectCalculator.h"
#include "CalculatorManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
CSpaceConvertMatrix::CSpaceConvertMatrix( ObjectCalculator *pCalculator )
{
	m_pCalculator = pCalculator;
	m_bNeedAdjust = FALSE;
}

CSpaceConvertMatrix::~CSpaceConvertMatrix()
{
}
matrix4f * CSpaceConvertMatrix::getWorld()
{
	return &m_matWorld;
}

matrix4f * CSpaceConvertMatrix::getAdjust()
{
	if( !m_bNeedAdjust )
		return NULL;
	return &m_matAdjust2D;
}
matrix4f * CSpaceConvertMatrix::getAdjustInv()
{
	if( !m_bNeedAdjust )
		return NULL;
	return &m_matAdjust2DInv;
}
matrix4f CSpaceConvertMatrix::GetOriginWorld()
{
	return *m_pCalculator->GetRenderMatV();
}

void CSpaceConvertMatrix::InitMatrix()
{
	m_bNeedAdjust = FALSE;
	m_matWorld = GetOriginWorld();
}
/////////////////////转换工具（支持2D层摄像机)//////////
CSimpleSpaceConvert::CSimpleSpaceConvert( ObjectCalculator *pCalculator, BOOL bForEdit, BOOL bUse2DReal /*= TRUE*/, CSpaceConvertMatrix *pConvertMatrix /*= NULL*/ )
{
	m_pConvertMatrix = NULL;
	m_pParentConvert = NULL;
	ZeroMemory( &m_ConvertParam, sizeof(m_ConvertParam));

	m_pCalculator = pCalculator;
	m_pConvertMatrix = pConvertMatrix;
	m_bReleaseConvert = (pConvertMatrix == NULL);
	if( !m_pConvertMatrix )
	{
		m_pConvertMatrix = new CSpaceConvertMatrix(m_pCalculator);
	}
	m_pConvertMatrix->InitMatrix();
#if 0
	CBaseLayerEnv* pLayerEnv = pCalculator->GetLayerEnv();
	ObjectCalculator* pCameraCalculator=NULL;
	if(pLayerEnv)
	{
		if (bForEdit)
			pCameraCalculator = pLayerEnv->GetEditCameraEnv();
		else
			pCameraCalculator = pLayerEnv->GetRenderCameraEnv();
		//由于是整个场景坐标系下面的转换，所以需要用EditSpaceConvert
		if( bUse2DReal )
			pLayerEnv->GetEditSpaceConvertParam(&m_ConvertParam,pCameraCalculator, m_pConvertMatrix->getWorld()); 
		else
			pLayerEnv->GetRenderSpaceConvertParam(&m_ConvertParam,pCameraCalculator, m_pConvertMatrix->getWorld());
		
	}
	ObjectCalculator *pParent = pCalculator->GetParent();
	while( pParent != NULL )
	{
		if( !pParent->GetItem()->IsPassGlobalMatrix())
		{
			m_pParentConvert = new CParentSpaceConvert( pParent, bForEdit, bUse2DReal  );
			break;
		}
		pParent = pParent->GetParent();
	}
	if(pCameraCalculator)
		m_fViewWorkPlaneDis = ((CBaseCGCameraItem*)pCameraCalculator->GetItem())->GetViewWorkPlaneDis();
#endif
	m_b3DRenderType = TRUE;
}

void CSimpleSpaceConvert::Convert3DPtTo2D( const float3 *ptSrc, float3 *pDest )
{
	float3 vNewPt;
	SpaceConvert(&vNewPt,ptSrc, &m_ConvertParam, SY3DVEC_TO_2D);

	matrix4f *pAdjust2D = m_pConvertMatrix->getAdjust();
	if( pAdjust2D )
	{
		vNewPt *= *pAdjust2D;
	}
	if( !m_pParentConvert )
		*pDest = vNewPt;
	else
	{
		m_pParentConvert->ConvertParent2DTo2D( &vNewPt, pDest);
	}
}
void CSimpleSpaceConvert::ConvertVectorGraphSpace( vectorgraph* pGraph, float fSignY )
{
	if (pGraph->ncell > 0)
	{
		pGraph->bound.left  = 65535.0f;
		pGraph->bound.top   = 65535.0f;
		pGraph->bound.right = -65535.0f;
		pGraph->bound.bottom= -65535.0f;
	}
	vector3f vTempVectorF;
	vector3f vVectorF;
	for (int i=0; i<pGraph->ncell; i++)
	{
		pGraph->cell[i].bound.left = 65535.0f;
		pGraph->cell[i].bound.top = 65535.0f;
		pGraph->cell[i].bound.right = -65535.0f;
		pGraph->cell[i].bound.bottom = -65535.0f;
		for(int k = 0; k < pGraph->cell[i].npoint; k++)
		{
			vTempVectorF.x = pGraph->cell[i].point[k].x;
			vTempVectorF.y = pGraph->cell[i].point[k].y * fSignY;
			vTempVectorF.z = 0.0f;

			Convert3DPtTo2D( &vTempVectorF, &vVectorF );

			pGraph->cell[i].point[k].x = vVectorF.x;
			pGraph->cell[i].point[k].y = vVectorF.y;

			pGraph->cell[i].bound.left  = min(pGraph->cell[i].bound.left,vVectorF.x);
			pGraph->cell[i].bound.top   = min(pGraph->cell[i].bound.top,vVectorF.y);
			pGraph->cell[i].bound.right = max(pGraph->cell[i].bound.right,vVectorF.x);
			pGraph->cell[i].bound.bottom= max(pGraph->cell[i].bound.bottom,vVectorF.y);
		}

		pGraph->bound.left  = min(pGraph->bound.left,pGraph->cell[i].bound.left);
		pGraph->bound.top   = min(pGraph->bound.top,pGraph->cell[i].bound.top);
		pGraph->bound.right = max(pGraph->bound.right,pGraph->cell[i].bound.right);
		pGraph->bound.bottom= max(pGraph->bound.bottom,pGraph->cell[i].bound.bottom);
	}
}

void CSimpleSpaceConvert::Convert3DPtToView( const float3 *ptSrc, float3 *pDest )
{
	*pDest = *ptSrc * getWorldMatrix() * *m_ConvertParam.pViewMat;
}

void CSimpleSpaceConvert::ConvertViewPtTo3D( const float3 *ptSrc, float3 *pDest )
{
	matrix4f matWorldInv, matViewInv;
	matrix4f_inverse(&matViewInv, m_ConvertParam.pViewMat); 
	matrix4f_inverse(&matWorldInv, &getWorldMatrix()); 
	*pDest = *ptSrc * matViewInv *matWorldInv; 
}

void CSimpleSpaceConvert::Convert3DPtToWorld( const float3 *pSrc, float3 *pDest )
{
	*pDest = *pSrc * getWorldMatrix();
}
void CSimpleSpaceConvert::ConvertWorldPtTo3D( const float3 *pSrc, float3 *pDest )
{
	matrix4f matWorldInv;
	matrix4f_inverse(&matWorldInv, &getWorldMatrix()); 
	*pDest = *pSrc * matWorldInv; 
}
void CSimpleSpaceConvert::Convert2DPtTo3D_ZViewPlane( const float3 *ptSrc, float3 *ptDest, float fZValue )
{
	float3 ptReal = *ptSrc;
	if( m_pParentConvert )
	{
		m_pParentConvert->Convert2DToParent2D( &ptReal, &ptReal );
	}
	matrix4f *pAdjust2DInv = m_pConvertMatrix->getAdjustInv();
	if( pAdjust2DInv )
	{
		ptReal *= *pAdjust2DInv;
	}
	SpaceConvert(ptDest, &ptReal, &m_ConvertParam, SY2DVEC_TO_3D_VIEWPLANE, &fZValue );
}
void CSimpleSpaceConvert::Convert2DPtTo3D_NearWorldPlane( const float3 *ptSrc, float3 *ptDest )
{
	float3 ptReal = *ptSrc;
	if( m_pParentConvert )
	{
		m_pParentConvert->Convert2DToParent2D( &ptReal, &ptReal );
	}
	matrix4f *pAdjust2DInv = m_pConvertMatrix->getAdjustInv();
	if( pAdjust2DInv )
	{
		ptReal *= *pAdjust2DInv;
	}
	SpaceConvert(ptDest, &ptReal, &m_ConvertParam, SY2DVEC_TO_3D_WORLD );
}
BOOL CSimpleSpaceConvert::Convert2DPtTo3D_Plane( const float3 *ptSrc, float3 *ptDest, float3 *vPlaneCenter, float3 *vPlaneNormal )
{
	float3 ptReal = *ptSrc;
	if( m_pParentConvert )
	{
		m_pParentConvert->Convert2DToParent2D( &ptReal, &ptReal );
	}
	matrix4f *pAdjust2DInv = m_pConvertMatrix->getAdjustInv();
	if( pAdjust2DInv )
	{
		ptReal *= *pAdjust2DInv;
	}
	return SpaceConvert(ptDest, &ptReal, &m_ConvertParam, SY2DVEC_TO_3D_PLANE, NULL, vPlaneNormal, vPlaneCenter );
}
matrix4f CSimpleSpaceConvert::getWorldMatrix()
{
	if( !m_pParentConvert )
		return *m_pConvertMatrix->getWorld();
	
	return *m_pConvertMatrix->getWorld() * m_pParentConvert->getParentWorldMatrix();
}

matrix4f CSimpleSpaceConvert::getViewProjectMatrix()
{
	matrix4f mat = *m_ConvertParam.pViewMat * *m_ConvertParam.pProjMat;

	matrix4f *pAdjust2D = m_pConvertMatrix->getAdjustInv();
	if( pAdjust2D )
	{
		mat *= *pAdjust2D;
	}

	return mat;
}

matrix4f CSimpleSpaceConvert::getViewMatrix()
{
	return *m_ConvertParam.pViewMat;
}

void CSimpleSpaceConvert::GetRayDirection( OUT SYRay *pRay, IN float3 *ptHit )
{
	float3 vec3DPoint;
	matrix4f viewInv,worldInv;
	matrix4f_inverse(&viewInv,m_ConvertParam.pViewMat);
	matrix4f_inverse(&worldInv,m_ConvertParam.pWorldMat);
	if(m_ConvertParam.nProjType == Common_PERSPECTIVE_PROJ)
	{
		Convert2DPtTo3D_NearWorldPlane( ptHit, &pRay->vStart );
		//vec3DPoint -= *pParam->m_pEyePos;
		pRay->vDir = pRay->vStart - *m_ConvertParam.pEyePos * worldInv;
	}
	else if (m_ConvertParam.nProjType == Common_ORTHO_PROJ)
	{
		vec3DPoint.x = 0;
		vec3DPoint.y = 0;
		vec3DPoint.z = 1;
		Convert2DPtTo3D_NearWorldPlane( ptHit, &pRay->vStart );
		pRay->vDir = vec3DPoint * viewInv * worldInv;
		pRay->vDir = pRay->vDir - *m_ConvertParam.pEyePos * worldInv;
	}
	float3Normalize(&pRay->vDir,&pRay->vDir);
}

void CSimpleSpaceConvert::Convert2DPtTo3D_ZPlane( const float3 *ptSrc, float3 *ptDest )
{
	float3 vZero={0,0,0};
	float3 vNormal={0,0,1};
	Convert2DPtTo3D_Plane( ptSrc, ptDest, &vZero, &vNormal );
}

void CSimpleSpaceConvert::get3DTo2DMatrix( matrix4f *mat )
{
	*mat = *m_ConvertParam.pWorldMat * *m_ConvertParam.pViewMat * *m_ConvertParam.pProjMat;

	matrix4f *pAdjust2D = m_pConvertMatrix->getAdjust();
	if( pAdjust2D )
	{
		*mat *= *pAdjust2D;
	}

	if( !m_pParentConvert )
		return;

	matrix4f matParent;
	m_pParentConvert->getParent2DTo2DMatrix( &matParent );

	*mat = *mat * matParent;
}

CSimpleSpaceConvert::~CSimpleSpaceConvert()
{
	if(m_bReleaseConvert)
		SAFE_DELETE( m_pConvertMatrix );
	SAFE_DELETE( m_pParentConvert );
}

float CSimpleSpaceConvert::GetViewWorkPlaneDis()
{
	return m_fViewWorkPlaneDis;
}

void CSimpleSpaceConvert::ComputeNew3DptToViewPlan( const float3 *ptSrc, float3 *ptDest, float fZValue )
{
	SpaceConvert(ptDest, ptSrc, &m_ConvertParam, SY3DVEC_TO_VIEWPLANE, &fZValue );
}

BOOL CSimpleSpaceConvert::CheckPtInView( const float3* vPt,int nCount )
{
	for(int i = 0;i < nCount; i++)
	{
		float3 ptInView;
		Convert3DPtToView(vPt + i,&ptInView);
		if(ptInView.z > 0)
			return TRUE;
	}
	return FALSE;
}

/************************************************************************/
/* CUnitSCMatrix                                                        */
/************************************************************************/
CUnitSpaceConvertMatrix::CUnitSpaceConvertMatrix( ObjectCalculator *pCalculator, LPBaseARRAY pary3DUnit, int nIndex ):CSpaceConvertMatrix( pCalculator )
{
	m_pary3DUnit = pary3DUnit;
	m_nUnitIndex = nIndex;
}

CUnitSpaceConvertMatrix::~CUnitSpaceConvertMatrix()
{

}

matrix4f CUnitSpaceConvertMatrix::GetOriginWorld()
{
	SYItemRenderUnit* pRenderUnit  = (SYItemRenderUnit*)BaseAR_GetAt( m_pary3DUnit, m_nUnitIndex );
	matrix4f matWorld = pRenderUnit->unit3D.Mat_RenderV;
	return matWorld;
}

/************************************************************************/
/* CParentSpaceConvert                                                  */
/* 滚屏子物件对应的2D空间为滚屏层的Buffer空间，因此需要二次变换到场景Real空间*/
/************************************************************************/
CParentSpaceConvert::CParentSpaceConvert( ObjectCalculator *pCalculator, BOOL bForEdit, BOOL bUse2DReal )
{
	m_pSpaceConvert = CreateSimpleSpaceConvert( pCalculator, bForEdit, bUse2DReal );

	m_ScrollMatrix = pCalculator->GetScrollMatrix();
	m_pCalculator = pCalculator;
	m_pCalculator->GetSceneCenter( m_vSceneCenter );	
}
CParentSpaceConvert::~CParentSpaceConvert()
{
	ReleaseSimpleSpaceConvert( m_pSpaceConvert );
}
void CParentSpaceConvert::Convert2DToParent2D( const float3 *ptSrc, float3 *ptDest )
{
	float3 tmpDest;
	m_pSpaceConvert->Convert2DPtTo3D_ZPlane( ptSrc, &tmpDest);
	SceneToBuffer( ptDest, &tmpDest, m_vSceneCenter.x, m_vSceneCenter.y, 1 );
	return;
}

void CParentSpaceConvert::ConvertParent2DTo2D( const float3 *ptSrc, float3 *ptDest )
{
	float3 pt3D;
	BufferToScene( &pt3D, ptSrc, m_vSceneCenter.x, m_vSceneCenter.y, 1 );
	m_pSpaceConvert->Convert3DPtTo2D( &pt3D, ptDest );
}
matrix4f  CParentSpaceConvert::getParentWorldMatrix()
{
	matrix4f matWorld = m_pSpaceConvert->getWorldMatrix();
	matWorld = m_ScrollMatrix * matWorld;
	return matWorld;	
}
void CParentSpaceConvert::getParent2DTo2DMatrix( matrix4f* mat )
{
	matrix4f_from_scale( mat,1,-1,1);
	mat->_41 = -m_vSceneCenter.x;
	mat->_42 = m_vSceneCenter.y;
	matrix4f mat2;
	m_pSpaceConvert->get3DTo2DMatrix( &mat2 );
	*mat = *mat * mat2;
}
CSimpleSpaceConvert2D::CSimpleSpaceConvert2D( ObjectCalculator *pCalculator, BOOL bForEdit, BOOL bUse2DReal, CSpaceConvertMatrix *pConvertMatrix  )
	:CSimpleSpaceConvert( pCalculator, bForEdit, bUse2DReal, pConvertMatrix )
{
	m_pCalculator->GetSceneCenter( m_vSceneCenter );
	
}
CSimpleSpaceConvert2D::~CSimpleSpaceConvert2D()
{

}
void CSimpleSpaceConvert2D::Convert3DPtTo2D( const float3 *ptSrc, float3 *pDest )
{
	float3 vNewPt;
	//SpaceConvert(&vNewPt,ptSrc, &m_ConvertParam, SY3DVEC_TO_2D);
	matrix4f matWorld = *m_pConvertMatrix->getWorld();
	float3 pos = { matWorld._41, matWorld._42, 0 };
	matWorld._41 = matWorld._42 = matWorld._43 = 0;
	vNewPt = *ptSrc * matWorld;
	vNewPt += pos;
	vNewPt.z = 0;
	vNewPt.x += m_vSceneCenter.x;
	vNewPt.y = m_vSceneCenter.y - vNewPt.y;
	if( !m_pParentConvert )
		*pDest = vNewPt;
	else
	{
		m_pParentConvert->ConvertParent2DTo2D( &vNewPt, pDest);
	}
}
void CSimpleSpaceConvert2D::Convert2DPtTo3D_ZViewPlane( const float3 *ptSrc, float3 *ptDest, float fZValue )
{
	float3 ptReal = *ptSrc;
	if( m_pParentConvert )
	{
		m_pParentConvert->Convert2DToParent2D( &ptReal, &ptReal );
	}
	float3 vNewPt = ptReal;
	vNewPt.z = fZValue;
	vNewPt.x -= m_vSceneCenter.x;
	vNewPt.y = m_vSceneCenter.y - vNewPt.y;
	*ptDest = vNewPt;
}
void CSimpleSpaceConvert2D::GetRayDirection( OUT SYRay *pRay, IN float3 *ptHit )
{
	float3 vec3DPoint;
	matrix4f viewInv,worldInv;
	matrix4f_inverse(&viewInv,m_ConvertParam.pViewMat);
	matrix4f matWorld = *m_ConvertParam.pWorldMat;
	float3 pos = { matWorld._41, matWorld._42, 0 };
	matWorld._41 = matWorld._42 = matWorld._43 = 0;
	matrix4f_inverse(&worldInv,&matWorld);
	if(m_ConvertParam.nProjType == Common_PERSPECTIVE_PROJ)
	{
		Convert2DPtTo3D_NearWorldPlane( ptHit, &pRay->vStart );
		//vec3DPoint -= *pParam->m_pEyePos;
		pRay->vDir = pRay->vStart - *m_ConvertParam.pEyePos * worldInv;
	}
	else if (m_ConvertParam.nProjType == Common_ORTHO_PROJ)
	{
		vec3DPoint.x = 0;
		vec3DPoint.y = 0;
		vec3DPoint.z = 1;
		Convert2DPtTo3D_NearWorldPlane( ptHit, &pRay->vStart );
		pRay->vDir = vec3DPoint * viewInv * worldInv;
		pRay->vDir = pRay->vDir - *m_ConvertParam.pEyePos * worldInv;
	}
	float3Normalize(&pRay->vDir,&pRay->vDir);
}
void CSimpleSpaceConvert2D::Convert2DPtTo3D_NearWorldPlane( const float3 *ptSrc, float3 *ptDest )
{
	float3 ptReal = *ptSrc;
	if( m_pParentConvert )
	{
		m_pParentConvert->Convert2DToParent2D( &ptReal, &ptReal );
	}
	float3 vNewPt = ptReal;
	vNewPt.z = 0;
	vNewPt.x -= m_vSceneCenter.x;
	vNewPt.y = m_vSceneCenter.y - vNewPt.y;
	matrix4f matWorld = *m_pConvertMatrix->getWorld(), matWorldInv;
	float3 pos = { matWorld._41, matWorld._42, 0 };
	matWorld._41 = matWorld._42 = matWorld._43 = 0;
	vNewPt -= pos;
	matrix4f_inverse( &matWorldInv, &matWorld );
	vNewPt = vNewPt * matWorldInv;
	*ptDest = vNewPt;
}
BOOL CSimpleSpaceConvert2D::Convert2DPtTo3D_Plane( const float3 *ptSrc, float3 *ptDest, float3 *vPlaneCenter, float3 *vPlaneNormal )
{
	float3 ptReal = *ptSrc;
	if( m_pParentConvert )
	{
		m_pParentConvert->Convert2DToParent2D( &ptReal, &ptReal );
	}
	float3 vNewPt = ptReal;
	vNewPt.z = 0;
	vNewPt.x -= m_vSceneCenter.x;
	vNewPt.y = m_vSceneCenter.y - vNewPt.y;
	matrix4f matWorld = *m_pConvertMatrix->getWorld(), matWorldInv;
	float3 pos = { matWorld._41, matWorld._42, 0 };
	matWorld._41 = matWorld._42 = matWorld._43 = 0;
	matrix4f_inverse( &matWorldInv, &matWorld );
	float3 center = *vPlaneCenter *matWorld;
	center += pos;
	float3 vplane_normal;
	float3NormalMulMat(&vplane_normal, vPlaneNormal, &matWorld);
	SYRay ray;
	ray.vStart = vNewPt;
	ray.vStart.z = -m_fViewWorkPlaneDis;
	float3Set( &ray.vDir, 0,0,1 );
	SYHitPoint hitPoint;
	if(GetRayPlaneInt(&ray,&center,&vplane_normal,&hitPoint) != Common_NO_HIT)
	{
		*ptDest = hitPoint[0];
		*ptDest -= pos;
		*ptDest *= matWorldInv;
		return TRUE;
	}
	ray.vDir.z = -1;
	if(GetRayPlaneInt(&ray,&center,&vplane_normal,&hitPoint) != Common_NO_HIT)
	{
		*ptDest = hitPoint[0];
		*ptDest -= pos;
		*ptDest *= matWorldInv;
		return TRUE;
	}
	return FALSE;
}

void CSimpleSpaceConvert2D::Convert3DPtToView( const float3 *ptSrc, float3 *pDest )
{
	matrix4f matWorld = *m_pConvertMatrix->getWorld();
	float3 pos = { matWorld._41, matWorld._42, 0 };

	matWorld._41 = matWorld._42 = matWorld._43 = 0;
	float3 vNewPt = *ptSrc * matWorld;

	vNewPt += pos;

	vNewPt.z = m_fViewWorkPlaneDis;
	*pDest = vNewPt;
}

void CSimpleSpaceConvert2D::ConvertViewPtTo3D( const float3 *ptSrc, float3 *pDest )
{
	matrix4f matWorld = *m_pConvertMatrix->getWorld(), matWorldInv;
	float3 pos = { matWorld._41, matWorld._42, 0 };
	matWorld._41 = matWorld._42 = matWorld._43 = 0;
	matrix4f_inverse( &matWorldInv, &matWorld );
	float3 vNewPt = *ptSrc;
	vNewPt -= pos;
	vNewPt.z = 0; 
	vNewPt *= matWorldInv;
	*pDest = vNewPt;
}

void CSimpleSpaceConvert2D::Convert3DPtToWorld( const float3 *pSrc, float3 *pDest )
{
	matrix4f matWorld = *m_pConvertMatrix->getWorld();
	float3 pos = { matWorld._41, matWorld._42, 0 };
	matWorld._41 = matWorld._42 = matWorld._43 = 0;
	*pDest = *pSrc * matWorld;
	*pDest += pos;
}

void CSimpleSpaceConvert2D::ConvertWorldPtTo3D( const float3 *pSrc, float3 *pDest )
{
	matrix4f matWorld = *m_pConvertMatrix->getWorld(), matWorldInv;
	float3 pos = { matWorld._41, matWorld._42, 0 };
	matWorld._41 = matWorld._42 = matWorld._43 = 0;
	matrix4f_inverse( &matWorldInv, &matWorld );
	*pDest = *pSrc - pos;
	*pDest = *pDest * matWorldInv;

}
CSimpleSpaceConvert * CreateSimpleSpaceConvert( ObjectCalculator *pCalculator, BOOL bForEdit, BOOL bUse2DReal /*= TRUE */, CSpaceConvertMatrix *pConvertMatrix /*= NULL  */)
{
	if( pCalculator->Get3DType() == BaseGraph_DataBase::ITEM_FORMTYPE_2D )
		return new CSimpleSpaceConvert2D( pCalculator, bForEdit, bUse2DReal, pConvertMatrix );
	else if( pCalculator->Get3DType() == BaseGraph_DataBase::ITEM_FORMTYPE_3DIN2D)
		return new CSimpleSpaceConvert3DIn2D( pCalculator, bForEdit, bUse2DReal, pConvertMatrix );
	else if (pCalculator->Get3DType()== BaseGraph_DataBase::ITEM_FORMTYPE_2DRENDERBY3D)
		return new CSimpleSpaceConvert2DRenderBy3D(pCalculator,bForEdit,bUse2DReal,pConvertMatrix);
	else if( pCalculator->Get3DType() == BaseGraph_DataBase::ITEM_FORMTYPE_3D )
		return new CSimpleSpaceConvert( pCalculator, bForEdit, bUse2DReal, pConvertMatrix );
	else
		return new CSimpleSpaceConvert( pCalculator, bForEdit, bUse2DReal, pConvertMatrix );
}

void ReleaseSimpleSpaceConvert( CSimpleSpaceConvert *pConvert )
{
	if( pConvert != NULL )
	{
		delete pConvert;
	}
}

void SYCaclTransMatrixFromTwoPointInReal(matrix4f *dest, rectf *rtBound, const float3 *pt0, const float3 *pt1, ObjectCalculator *pParentCalculator)
{
	CSimpleSpaceConvert *pSpaceConvert = CreateSimpleSpaceConvert( pParentCalculator, TRUE, TRUE );
	float fWorkPlaneDis = pSpaceConvert->GetViewWorkPlaneDis();

	float3 ptLeftTop, ptRightBottom ;
	ptLeftTop.x = min(pt0->x, pt1->x);
	ptLeftTop.y = min(pt0->y, pt1->y);
	ptRightBottom.x = max(pt0->x, pt1->x);
	ptRightBottom.y = max(pt0->y, pt1->y);
	ptLeftTop.z = ptRightBottom.z = 0;

	if (ptRightBottom.x - ptLeftTop.x < SYITEM_DIMENSION_MIN)
	{
		ptRightBottom.x = ptLeftTop.x + SYITEM_DIMENSION_MIN;
	}
	if ( ptRightBottom.y - ptLeftTop.y < SYITEM_DIMENSION_MIN)
	{
		ptRightBottom.y = ptLeftTop.y + SYITEM_DIMENSION_MIN;
	}


	float3 ptSrc[4] = {
		{ ptLeftTop.x, ptLeftTop.y, 0 },
		{ ptRightBottom.x, ptLeftTop.y, 0 },
		{ ptRightBottom.x, ptRightBottom.y, 0 },
		{ ptLeftTop.x, ptRightBottom.y, 0 },
	};
	float3 pt[4];
	for( int i = 0; i<4; i++ )
	{
		pSpaceConvert->Convert2DPtTo3D_ZViewPlane( &ptSrc[i], &pt[i], fWorkPlaneDis );
		pSpaceConvert->ConvertViewPtTo3D( &pt[i], &pt[i] );
	}

	float width = float3Distance( &pt[0], &pt[1] );
	float height = float3Distance( &pt[1], &pt[2] );

	float3 pos = 0.5*(pt[0]+pt[2]);

	for( int i = 0; i<4; i++ )
		pt[i] -= pos;


	float3 vDirUp =0.5 *( pt[0] + pt[1] );
	float3 vDirRight=0.5 *( pt[1] + pt[2] );
	float3 vDirLook;
	float3CrossProduct( &vDirLook,   &vDirRight, &vDirUp );
	float3 vLookAt = vDirLook;
	float3 vEye = { 0,0,0 };
	matrix4f matItemWorldInv;
	CalViewMat( &matItemWorldInv, &vDirUp, &vEye, &vLookAt );

	matrix4f matItemWorld;// = matRot0 * matRot1;
	matrix4f_inverse( &matItemWorld, &matItemWorldInv );

	matItemWorld._41 = pos.x - pParentCalculator->GetScrollMatrix()._41;
	matItemWorld._42 = pos.y - pParentCalculator->GetScrollMatrix()._42;
	matItemWorld._43 = pos.z - pParentCalculator->GetScrollMatrix()._43;

	*dest = matItemWorld;

	rtBound->left = -0.5f*width;
	rtBound->top = -0.5f*height;
	rtBound->right = rtBound->left + width;
	rtBound->bottom = rtBound->top + height;

	ReleaseSimpleSpaceConvert( pSpaceConvert );
}

void BASEGRAPH_DATARENDER_API SYCaclTransMatrixFromTwoPointInReal(matrix4f * dest, rectf * rtBound, const float3 * pt0, const float3 * pt1, CString strParentID)
{
	ObjectCalculator * pCalculator = CalculatorManager::Instance().GetObjCalculator(strParentID);
	if (nullptr != pCalculator)
	{
		SYCaclTransMatrixFromTwoPointInReal(dest, rtBound, pt0, pt1, pCalculator);
	}
	else
	{
		assert(false);
	}
}

CSimpleSpaceConvert3DIn2D::CSimpleSpaceConvert3DIn2D( ObjectCalculator *pCalculator, BOOL bForEdit, BOOL bUse2DReal, CSpaceConvertMatrix *pConvertMatrix )
	:CSimpleSpaceConvert( pCalculator, bForEdit, bUse2DReal, pConvertMatrix )
{
	m_pCalculator->GetSceneCenter( m_vSceneCenter );
}

CSimpleSpaceConvert3DIn2D::~CSimpleSpaceConvert3DIn2D()
{

}

void CSimpleSpaceConvert3DIn2D::Convert3DPtTo2D( const float3 *ptSrc, float3 *pDest )
{
	float3 vNewPt;

	matrix4f matWorld = *m_pConvertMatrix->getWorld();
	float3 pos = { matWorld._41, matWorld._42, 0 };

	matWorld._41 = matWorld._42 = matWorld._43 = 0;

	SYSpaceConvertParam param = m_ConvertParam;
	param.pWorldMat = &matWorld;
	SpaceConvert( &vNewPt, ptSrc, &param, SY3DVEC_TO_2D );

	vNewPt.x += pos.x;
	vNewPt.y += -pos.y;

	if( !m_pParentConvert )
		*pDest = vNewPt;
	else
	{
		m_pParentConvert->ConvertParent2DTo2D( &vNewPt, pDest);
	}
}

void CSimpleSpaceConvert3DIn2D::Convert2DPtTo3D_ZViewPlane( const float3 *ptSrc, float3 *ptDest, float fZValue )
{
	float3 ptReal = *ptSrc;
	if( m_pParentConvert )
	{
		m_pParentConvert->Convert2DToParent2D( &ptReal, &ptReal );
	}
	matrix4f matWorld = *m_pConvertMatrix->getWorld();
	float3 pos = { matWorld._41, matWorld._42, 0 };
	matWorld._41 = matWorld._42 = matWorld._43 = 0;

	ptReal.x -= pos.x;
	ptReal.y -= -pos.y;

	float3 vNewPt = ptReal;
	//SpaceConvert(&vNewPt,ptSrc, &m_ConvertParam, SY3DVEC_TO_2D);
	vNewPt.z = fZValue;
	vNewPt.x -= m_vSceneCenter.x;
	vNewPt.y = m_vSceneCenter.y - vNewPt.y;
	*ptDest = vNewPt;
}

void CSimpleSpaceConvert3DIn2D::Convert2DPtTo3D_NearWorldPlane( const float3 *ptSrc, float3 *ptDest )
{
	float3 ptReal = *ptSrc;
	if( m_pParentConvert )
	{
		m_pParentConvert->Convert2DToParent2D( &ptReal, &ptReal );
	}
	matrix4f matWorld = *m_pConvertMatrix->getWorld();
	float3 pos = { matWorld._41, matWorld._42, 0 };
	matWorld._41 = matWorld._42 = matWorld._43 = 0;
	ptReal.x -= pos.x;
	ptReal.y -= -pos.y;
	float3 vNewPt = ptReal;
	vNewPt.z = 0;
	vNewPt.x -= m_vSceneCenter.x;
	vNewPt.y = m_vSceneCenter.y - vNewPt.y;
	matrix4f matViewInv;
	matrix4f_inverse( &matViewInv, m_ConvertParam.pViewMat);
	vNewPt *= matViewInv;
	matrix4f matWorldInv;
	matrix4f_inverse( &matWorldInv, &matWorld);
	vNewPt *=  matWorldInv;
	//vNewPt.z = m_ConvertParam.pEyePos->z;
	*ptDest = vNewPt;
}

BOOL CSimpleSpaceConvert3DIn2D::Convert2DPtTo3D_Plane( const float3 *ptSrc, float3 *ptDest, float3 *vPlaneCenter, float3 *vPlaneNormal )
{
	float3 ptReal = *ptSrc;
	if( m_pParentConvert )
	{
		m_pParentConvert->Convert2DToParent2D( &ptReal, &ptReal );
	}
	matrix4f matWorld = *m_pConvertMatrix->getWorld();
	float3 pos = { matWorld._41, matWorld._42, 0 };
	matWorld._41 = matWorld._42 = matWorld._43 = 0;

	ptReal.x -= pos.x;
	ptReal.y -= -pos.y;

	float3 vNewPt = ptReal;
	//SpaceConvert(&vNewPt,ptSrc, &m_ConvertParam, SY3DVEC_TO_2D);
	vNewPt.z = 0;
	vNewPt.x -= m_vSceneCenter.x;
	vNewPt.y = m_vSceneCenter.y - vNewPt.y;

	matrix4f matViewInv;
	matrix4f_inverse( &matViewInv, m_ConvertParam.pViewMat);
	vNewPt *= matViewInv;

	matrix4f matWorldInv;
	matrix4f_inverse( &matWorldInv, &matWorld);
	vNewPt *=  matWorldInv;

	SYRay ray;
	float3Set( &ray.vStart, vNewPt.x, vNewPt.y, vNewPt.z );
	static const float3 viewDir = {0,0,1};
	float3 temp;
	float3NormalMulMat(&temp,&viewDir,&matViewInv);
	float3NormalMulMat(&ray.vDir,&temp, &matWorldInv);
	float3Normalize(&ray.vDir,&ray.vDir);
	SYHitPoint hitPoint;
	if(GetRayPlaneInt(&ray,vPlaneCenter,vPlaneNormal,&hitPoint) == Common_NO_HIT)
	{
		return FALSE;
	}
	*ptDest = hitPoint[0];
	return TRUE;
}
void CSimpleSpaceConvert3DIn2D::ConvertViewPtTo3D( const float3 *ptSrc, float3 *pDest )
{
	matrix4f matWorldInv, matViewInv;
	matrix4f_inverse(&matViewInv, m_ConvertParam.pViewMat); 
	matrix4f_to_normal_matrix(&matWorldInv,&getWorldMatrix());
	matrix4f_inverse(&matWorldInv,&matWorldInv); 

	*pDest = *ptSrc * matViewInv *matWorldInv; 
}
///CSimpleSpaceConvert2DRenderBy3D///////////////////////////////////////////////////////////////////////

CSimpleSpaceConvert2DRenderBy3D::CSimpleSpaceConvert2DRenderBy3D( ObjectCalculator *pCalculator, BOOL bForEdit, BOOL bUse2DReal, CSpaceConvertMatrix *pConvertMatrix )
	:CSimpleSpaceConvert( pCalculator, bForEdit, bUse2DReal, pConvertMatrix )
{
	m_pCalculator->GetSceneCenter( m_vSceneCenter );
}

CSimpleSpaceConvert2DRenderBy3D::~CSimpleSpaceConvert2DRenderBy3D()
{

}

void CSimpleSpaceConvert2DRenderBy3D::Convert3DPtTo2D( const float3 *ptSrc, float3 *pDest )
{
	float3 vNewPt;

	matrix4f matWorld = *m_pConvertMatrix->getWorld();
	float3 pos = { matWorld._41, matWorld._42, 0 };

	matWorld._41 = matWorld._42 = matWorld._43 = 0;

	SYSpaceConvertParam param = m_ConvertParam;
	param.pWorldMat = &matWorld;
	SpaceConvert( &vNewPt, ptSrc, &param, SY3DVEC_TO_2D );

	vNewPt.x += pos.x;
	vNewPt.y += -pos.y;

	if( !m_pParentConvert )
		*pDest = vNewPt;
	else
	{
		m_pParentConvert->ConvertParent2DTo2D( &vNewPt, pDest);
	}
}

void CSimpleSpaceConvert2DRenderBy3D::Convert2DPtTo3D_ZViewPlane( const float3 *ptSrc, float3 *ptDest, float fZValue )
{
	float3 ptReal = *ptSrc;
	if( m_pParentConvert )
	{
		m_pParentConvert->Convert2DToParent2D( &ptReal, &ptReal );
	}
	matrix4f matWorld = *m_pConvertMatrix->getWorld();
	float3 pos = { matWorld._41, matWorld._42, 0 };
	matWorld._41 = matWorld._42 = matWorld._43 = 0;

	ptReal.x -= pos.x;
	ptReal.y -= -pos.y;

	float3 vNewPt = ptReal;
	//SpaceConvert(&vNewPt,ptSrc, &m_ConvertParam, SY3DVEC_TO_2D);
	vNewPt.z = fZValue;
	vNewPt.x -= m_vSceneCenter.x;
	vNewPt.y = m_vSceneCenter.y - vNewPt.y;
	*ptDest = vNewPt;
}

void CSimpleSpaceConvert2DRenderBy3D::Convert2DPtTo3D_NearWorldPlane( const float3 *ptSrc, float3 *ptDest )
{
	float3 ptReal = *ptSrc;
	if( m_pParentConvert )
	{
		m_pParentConvert->Convert2DToParent2D( &ptReal, &ptReal );
	}
	float3 vNewPt = ptReal;
	vNewPt.z = 0;
	vNewPt.x -= m_vSceneCenter.x;
	vNewPt.y = m_vSceneCenter.y - vNewPt.y;
	matrix4f matWorld = *m_pConvertMatrix->getWorld(), matWorldInv;
	float3 pos = { matWorld._41, matWorld._42, 0 };
	matWorld._41 = matWorld._42 = matWorld._43 = 0;
	vNewPt -= pos;
	matrix4f_inverse( &matWorldInv, &matWorld );
	vNewPt = vNewPt * matWorldInv;
	*ptDest = vNewPt;
}

BOOL CSimpleSpaceConvert2DRenderBy3D::Convert2DPtTo3D_Plane( const float3 *ptSrc, float3 *ptDest, float3 *vPlaneCenter, float3 *vPlaneNormal )
{
	float3 ptReal = *ptSrc;
	if( m_pParentConvert )
	{
		m_pParentConvert->Convert2DToParent2D( &ptReal, &ptReal );
	}
	matrix4f *pAdjust2DInv = m_pConvertMatrix->getAdjustInv();
	if( pAdjust2DInv )
	{
		ptReal *= *pAdjust2DInv;
	}

	matrix4f matWorld = *m_pConvertMatrix->getWorld();
	float3 pos = { matWorld._41, matWorld._42, 0 };
	matWorld._41 = matWorld._42 = matWorld._43 = 0;
	ptReal.x -= pos.x;
	ptReal.y -= -pos.y;
	SYSpaceConvertParam param = m_ConvertParam;
	param.pWorldMat = &matWorld;

	return SpaceConvert(ptDest, &ptReal, &param, SY2DVEC_TO_3D_PLANE, NULL, vPlaneNormal, vPlaneCenter );
}
void CSimpleSpaceConvert2DRenderBy3D::GetRayDirection( OUT SYRay *pRay, IN float3 *ptHit )
{
	float3 vec3DPoint;
	matrix4f viewInv,worldInv;
	matrix4f_inverse(&viewInv,m_ConvertParam.pViewMat);
	matrix4f matWorld = *m_ConvertParam.pWorldMat;
	float3 pos = { matWorld._41, matWorld._42, 0 };
	matWorld._41 = matWorld._42 = matWorld._43 = 0;
	matrix4f_inverse(&worldInv,&matWorld);
	if(m_ConvertParam.nProjType == Common_PERSPECTIVE_PROJ)
	{
		Convert2DPtTo3D_NearWorldPlane( ptHit, &pRay->vStart );
		//vec3DPoint -= *pParam->m_pEyePos;
		pRay->vDir = pRay->vStart - *m_ConvertParam.pEyePos * worldInv;
	}
	else if (m_ConvertParam.nProjType == Common_ORTHO_PROJ)
	{
		vec3DPoint.x = 0;
		vec3DPoint.y = 0;
		vec3DPoint.z = 1;
		Convert2DPtTo3D_NearWorldPlane( ptHit, &pRay->vStart );
		pRay->vDir = vec3DPoint * viewInv * worldInv;
		pRay->vDir = pRay->vDir - *m_ConvertParam.pEyePos * worldInv;
	}
	float3Normalize(&pRay->vDir,&pRay->vDir);
}
void CSimpleSpaceConvert2DRenderBy3D::ConvertViewPtTo3D( const float3 *ptSrc, float3 *pDest )
{
	matrix4f matWorldInv, matViewInv;
	matrix4f_inverse(&matViewInv, m_ConvertParam.pViewMat); 
	matrix4f_to_normal_matrix(&matWorldInv,&getWorldMatrix());
	matrix4f_inverse(&matWorldInv,&matWorldInv); 

	*pDest = *ptSrc * matViewInv *matWorldInv; 
}
//////////////////////////////////////////////////////////////////////////
