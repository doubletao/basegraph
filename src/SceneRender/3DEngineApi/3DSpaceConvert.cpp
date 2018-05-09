#include "stdafx.h"
#include "SceneRender\3DSpaceConvert.h"
#include "SceneRender\3DInterSection.h"

#ifdef _DEBUG
#define new SY_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL BASEGRAPH_SCENE_RENDER_API SpaceConvert( float3* pOutVec, /*目的向量 */ 
									const float3* pInVec, /*源向量 */ 
									const SYSpaceConvertParam* pParam, /*空间状态 */ 
									int nType, /*转 类型 */ 
									const float* pZValue ,
									const float3* pPlaneNormal ,	//投影到平面上时，表示平面的法向量
									const float3* pPlaneCenter )	//投影到平面上时，表示平面的中心位置			
{
	if (nType == SY2DVEC_TO_3D_VIEWPLANE || nType == SY2DVEC_TO_3D_WORLD)
	{
		D3DXVec3Unproject( (D3DXVECTOR3*)pOutVec, (D3DXVECTOR3*)pInVec, (D3D10_VIEWPORT *)pParam->pViewPort, (D3DXMATRIXA16*)pParam->pProjMat, (D3DXMATRIXA16*)pParam->pViewMat, (D3DXMATRIXA16*)pParam->pWorldMat );
#ifdef _DEBUG
		//////////////////////////////////////////////////////////////////////////
		matrix4f worldProj,worldProjInv; 
		worldProj = *pParam->pWorldMat * *pParam->pViewMat * *pParam->pProjMat;
		matrix4f_inverse(&worldProjInv,&worldProj);

		float4 v3D;
		v3D.x = ((pInVec->x - pParam->pViewPort->dwX) / pParam->pViewPort->dwWidth * 2 - 1 ) * pParam->fZNear;
		v3D.y = (1 - (pInVec->y - pParam->pViewPort->dwY) / pParam->pViewPort->dwHeight * 2 ) * pParam->fZNear;
		v3D.z = 0;
		v3D.w = pParam->fZNear;

		v3D *= worldProjInv; 
		//////////////////////////////////////////////////////////////////////////
#endif // _DEBUG

		if (nType == SY2DVEC_TO_3D_VIEWPLANE)
		{
			*pOutVec *= *pParam->pWorldMat;
			*pOutVec *= *pParam->pViewMat;

			if ( pParam->nProjType == SY_PERSPECTIVE_PROJ && pZValue ) 
			{
				pOutVec->x = pOutVec->x * *pZValue / pOutVec->z;
				pOutVec->y = pOutVec->y * *pZValue / pOutVec->z;
				pOutVec->z = *pZValue;
			}
			else if ( pParam->nProjType == SY_ORTHO_PROJ && pZValue)
			{
				pOutVec->z = *pZValue;
			}
		}
	}
	else if (nType == SY2DVEC_TO_3D_PLANE)
	{
		if(!pPlaneCenter || !pPlaneNormal)		
			return false;

		D3DXVec3Unproject( (D3DXVECTOR3*)pOutVec, (D3DXVECTOR3*)pInVec, (D3D10_VIEWPORT *)pParam->pViewPort, (D3DXMATRIXA16*)pParam->pProjMat, (D3DXMATRIXA16*)pParam->pViewMat, (D3DXMATRIXA16*)pParam->pWorldMat );
		SYRay ray;
		ray.vStart = *pOutVec;
		matrix4f worldInv,viewInv;
		matrix4f_inverse(&worldInv,pParam->pWorldMat);
		if ( pParam->nProjType == SY_PERSPECTIVE_PROJ )
		{
			ray.vDir  = *pOutVec - *(pParam->pEyePos) * worldInv;
		}
		else if ( pParam->nProjType == SY_ORTHO_PROJ )
		{
			matrix4f_inverse(&viewInv,pParam->pViewMat);
			static const float3 viewDir = {0,0,1};
			float3 temp;
			float3NormalMulMat(&temp,&viewDir,&viewInv);
			float3NormalMulMat(&ray.vDir,&temp,&worldInv);
		}
		float3Normalize(&ray.vDir,&ray.vDir);
		SYHitPoint hitPoint;
		if(GetRayPlaneInt(&ray,pPlaneCenter,pPlaneNormal,&hitPoint) == SY_NO_HIT)
			return FALSE;
		*pOutVec = hitPoint[0];
	}
	else if (nType == SY3DVEC_TO_2D)
	{
		float4 outVec,inVec;
		float4Set(&inVec,pInVec->x,pInVec->y,pInVec->z,1);
		if(0)
			outVec = inVec * *pParam->pWorldMat * *pParam->pViewMat;
		outVec = inVec * *pParam->pWorldMat * *pParam->pViewMat * *pParam->pProjMat;
		outVec /= fabs(outVec.w);

		pOutVec->x = (outVec.x + 1) / 2.f * pParam->pViewPort->dwWidth + pParam->pViewPort->dwX;
		pOutVec->y = (1 - outVec.y ) / 2.f * pParam->pViewPort->dwHeight + pParam->pViewPort->dwY;
		pOutVec->z = outVec.z;
		float4Set(&outVec,pOutVec->x,pOutVec->y,pOutVec->z,outVec.w);
		pOutVec->z = 0;
	}
	else if(nType == SY3DVEC_TO_VIEWPLANE)
	{
		if(0)
		{
			matrix4f WorldInv,viewInv;
			matrix4f_inverse(&WorldInv,pParam->pWorldMat);
			matrix4f_inverse(&viewInv,pParam->pViewMat);
			float3 vViewPt = *pInVec * *pParam->pWorldMat * *pParam->pViewMat;
			float fPtZValue = vViewPt.z;

			*pOutVec = vViewPt * *pZValue / fPtZValue ;
			*pOutVec * viewInv * WorldInv;
		}
		if(pParam->nProjType == SY_ORTHO_PROJ)
		{
			matrix4f WorldInv;
			matrix4f_inverse(&WorldInv,pParam->pWorldMat);
			matrix4f ViewInv;
			matrix4f_inverse(&ViewInv,pParam->pViewMat);
			float3 vPt = *pInVec * *pParam->pWorldMat * *pParam->pViewMat;
			vPt.z = *pZValue;
			*pOutVec = vPt * ViewInv * WorldInv;
		}
		else if(pParam->nProjType == SY_PERSPECTIVE_PROJ)
		{
			matrix4f WorldInv;
			matrix4f_inverse(&WorldInv,pParam->pWorldMat);
			float3 vEyeInWorld = *pParam->pEyePos * WorldInv;
			float fPtZValue = (*pInVec * *pParam->pWorldMat * *pParam->pViewMat).z;
			*pOutVec = (*pInVec - vEyeInWorld) * *pZValue / fPtZValue + vEyeInWorld;
		}
	}

	return TRUE;

}

void BASEGRAPH_SCENE_RENDER_API CalProjectMat( matrix4f* pOutProjMat,
								   float left,float right,float bottom,float top,	//平面的有效宽高
								   float zNear,float zFar ,
								   SYPROJTYPE nType)
{
	switch (nType)
	{
	case SY_PERSPECTIVE_PROJ:
		D3DXMatrixPerspectiveOffCenterLH( (D3DXMATRIX*)pOutProjMat, left, right, bottom, top, zNear, zFar );
		break;
	case SY_ORTHO_PROJ:
		D3DXMatrixOrthoOffCenterLH((D3DXMATRIX*)pOutProjMat,left,right,bottom,top,zNear,zFar);
		break;
	}
}

BASEGRAPH_SCENE_RENDER_API void CalBillboardMatrix(matrix4f* pSrcMatrix, const matrix4f* pViewMat,const matrix4f* AdditionRotate)
{
	float3 vScale,vPos;
	matrix4f viewInv;

	matrix4f_inverse(&viewInv,pViewMat);

	*pSrcMatrix = *pSrcMatrix * *pViewMat;

	matrix4f_decompose(pSrcMatrix,&vScale,NULL,&vPos);
	matrix4f_identity(pSrcMatrix);
	pSrcMatrix->_11 = vScale.x;
	pSrcMatrix->_22 = vScale.y;
	pSrcMatrix->_33 = vScale.z;

	pSrcMatrix->_41 = vPos.x;
	pSrcMatrix->_42 = vPos.y;
	pSrcMatrix->_43 = vPos.z;

	if(AdditionRotate)
		*pSrcMatrix = *AdditionRotate * *pSrcMatrix * viewInv ;
	else
		*pSrcMatrix =  *pSrcMatrix * viewInv ;
}

void BASEGRAPH_SCENE_RENDER_API CalBillboardMatrix( matrix4f* pSrcMatrix, int nCount,size_t nOffset,const matrix4f* pWorldMat, const matrix4f* pViewMat )
{
	matrix4f mat = *pViewMat;
	if(pWorldMat)
		mat = *pWorldMat * mat;
	matrix4f matInv;
	matrix4f_inverse(&matInv,&mat);

	BYTE * pBuffer = (BYTE*)pSrcMatrix;
	for(int i = 0; i < nCount ; i++)
	{
		matrix4f* pMat = (matrix4f*)(pBuffer + nOffset * i);
		*pMat *= mat;
		matrix4f pos;
		matrix4f_issolvesrp(pMat,pMat,NULL,&pos);
		memcpy(pMat->m[3],pos.m[3],sizeof(float) * 4);
		*pMat *= matInv;
	}
}

void BASEGRAPH_SCENE_RENDER_API CalViewMat( matrix4f* pOutViewMat,const float3* pUp,const float3* pEyePos,const float3* pLookAt )
{
	D3DXMatrixLookAtLH((D3DXMATRIX*)pOutViewMat,(D3DXVECTOR3*)pEyePos,(D3DXVECTOR3*)pLookAt,(D3DXVECTOR3*)pUp);
}

void BASEGRAPH_SCENE_RENDER_API SYConvertBoxTo2DRect(rectf* pRect,const SYSpaceConvertParam* param,const float3* vMin,const float3* vMax)
{
	float3 vPoint[8];

	vPoint[0].x = vMin->x ;
	vPoint[0].y = vMax->y ;
	vPoint[0].z = vMax->z ;
                       
	vPoint[1].x = vMax->x ;
	vPoint[1].y = vMax->y ;
	vPoint[1].z = vMax->z ;
	                   
	vPoint[2].x = vMax->x ;
	vPoint[2].y = vMax->y ;
	vPoint[2].z = vMin->z ;
	                   
	vPoint[3].x = vMin->x ;
	vPoint[3].y = vMax->y ;
	vPoint[3].z = vMin->z ;
	                   
	vPoint[4].x = vMin->x ;
	vPoint[4].y = vMin->y ;
	vPoint[4].z = vMax->z ;
	                   
	vPoint[5].x = vMax->x ;
	vPoint[5].y = vMin->y ;
	vPoint[5].z = vMax->z ;
	                   
	vPoint[6].x = vMax->x ;
	vPoint[6].y = vMin->y ;
	vPoint[6].z = vMin->z ;
	                   
	vPoint[7].x = vMin->x ;
	vPoint[7].y = vMin->y ;
	vPoint[7].z = vMin->z ;

	float3 v2DPoint;
	SpaceConvert(&v2DPoint,&vPoint[0],param,SY3DVEC_TO_2D);
	pRect->left = pRect->right = v2DPoint.x;
	pRect->top = pRect->bottom = v2DPoint.y;
	for ( int j = 1; j < 8; j++ )
	{
		SpaceConvert(&v2DPoint,&vPoint[j],param,SY3DVEC_TO_2D);
		pRect->bottom = max( pRect->bottom, v2DPoint.y );
		pRect->top    = min( pRect->top,    v2DPoint.y );
		pRect->right  = max( pRect->right,  v2DPoint.x );
		pRect->left   = min( pRect->left,   v2DPoint.x );
	}
}

void BASEGRAPH_SCENE_RENDER_API SceneToBuffer( float3* pBufferPt,const float3* pScenePt,float fCenterX,float fCenterY,float fScale )
{
	float3Set(pBufferPt,pScenePt->x + fCenterX,-pScenePt->y + fCenterY,0);
	*pBufferPt *= fScale;
}

void BASEGRAPH_SCENE_RENDER_API BufferToScene( float3* pScenePt,const float3* pBufferPt,float fCenterX,float fCenterY,float fScale )
{
	float3Set(pScenePt,pBufferPt->x - fCenterX,-(pBufferPt->y - fCenterY),0);
	*pScenePt /= fScale;
}

void BASEGRAPH_SCENE_RENDER_API SceneToBuffer(float2 * pBufferPt, const float2 * pScenePt, float fCenterX, float fCenterY, float fScale)
{
	float2Set(pBufferPt, pScenePt->x + fCenterX, -pScenePt->y + fCenterY);
	*pBufferPt *= fScale;
}

void BASEGRAPH_SCENE_RENDER_API BufferToScene(float2 * pScenePt, const float2 * pBufferPt, float fCenterX, float fCenterY, float fScale)
{
	float2Set(pScenePt, pBufferPt->x - fCenterX, -(pBufferPt->y - fCenterY));
	*pScenePt /= fScale;
}

void BASEGRAPH_SCENE_RENDER_API SceneToBufferMat( matrix4f* pMat,float fCenterX,float fCenterY)
{
	matrix4f_from_scale(pMat,1,-1,1);
	pMat->_41 = fCenterX;
	pMat->_42 = fCenterY;
}

void BASEGRAPH_SCENE_RENDER_API Cal2DViewProjMat(matrix4f* pOutViewProjMat,const RECT* validRT)
{
	float fLeft =	(float)validRT->left;
	float fRight =	(float)validRT->right;
	float fTop =	(float)validRT->top;
	float fBottom = (float)validRT->bottom;
	D3DXMatrixOrthoOffCenterLH((D3DXMATRIX*)pOutViewProjMat,fLeft,fRight,fBottom,fTop,0.01f,1000);
}

void BASEGRAPH_SCENE_RENDER_API ChangeZPos( float3* pPos,const matrix4f* pWorldPos,const matrix4f* pViewMat,float fZValue )
{
	matrix4f mat = *pViewMat;
	if(pWorldPos)
		mat = *pWorldPos * mat;
	*pPos *= mat;
	matrix4f_inverse(&mat,&mat);
	*pPos = *pPos * fZValue / pPos->z;
	*pPos *= mat;
}

#include <smmintrin.h>
void BASEGRAPH_SCENE_RENDER_API KeepModelSize( matrix4f* pMat,const matrix4f* pWorldPos,const matrix4f* pViewMat,float fZValue )
{
	float3 vPos;
	matrix4f_to_pos_vector(pMat,&vPos);
	matrix4f mat = *pViewMat;
	if(pWorldPos)
		mat = *pWorldPos * mat;
	vPos *= mat;
	float vScale = vPos.z / fZValue;
	__m128 factor = _mm_set_ps1(vScale);
	for(int i = 0;i < 3; i++)
	{
		__m128 m = _mm_loadu_ps(pMat->m[i]);
		m = _mm_mul_ps(m,factor);
		_mm_storeu_ps(pMat->m[i],m);
	}
}

void BASEGRAPH_SCENE_RENDER_API KeepModelSize( matrix4f* pMat,int nCount,size_t offset,const matrix4f* pWorldPos,const matrix4f* pViewMat,float fZValue )
{
	matrix4f mat = *pViewMat;
	if(pWorldPos)
		mat = *pWorldPos * mat;
	__m128 m0 = _mm_loadu_ps(mat.m[0]);
	__m128 m1 = _mm_loadu_ps(mat.m[1]);
	__m128 m2 = _mm_loadu_ps(mat.m[2]);
	__m128 m3 = _mm_loadu_ps(mat.m[3]);

	BYTE* pBuffer = (BYTE*)pMat;
	for(int i = 0; i < nCount; i++)
	{
		matrix4f* pRealMat = (matrix4f*)(pBuffer + i * offset);
		__m128 vec0 = _mm_set_ps1(pRealMat->m[3][0]);
		__m128 vec1 = _mm_set_ps1(pRealMat->m[3][1]);
		__m128 vec2 = _mm_set_ps1(pRealMat->m[3][2]);
		__m128 vec3 = _mm_set_ps1(pRealMat->m[3][3]);
		__m128 rt = _mm_mul_ps(vec0,m0);
		rt = _mm_add_ps(rt,_mm_mul_ps(vec1,m1));
		rt = _mm_add_ps(rt,_mm_mul_ps(vec2,m2));
		rt = _mm_add_ps(rt,_mm_mul_ps(vec3,m3));
		__m128 factor = _mm_set_ps1(fZValue);
		factor = _mm_div_ss(_mm_shuffle_ps(rt,rt,_MM_SHUFFLE(2,2,2,2)),factor);
		factor = _mm_shuffle_ps(factor,factor,_MM_SHUFFLE(0,0,0,0));
		for(int i = 0;i < 3; i++)
		{
			__m128 m = _mm_loadu_ps(pRealMat->m[i]);
			m = _mm_mul_ps(m,factor);
			_mm_storeu_ps(pRealMat->m[i],m);
		}
	}
}

void SYCalBoxNearFar( float* pNear,float* pFar,const matrix4f& matWorldView, const float3& vMin,const float3& vMax )
{
	float3 ptBound[8];
	CreateBox(ptBound,vMin,vMax);
	for (int j = 0; j < 8; j++)
	{
		float3 fPt = ptBound[j] * matWorldView;
		*pFar = max(fPt.z , *pFar);
		*pNear = min(fPt.z , *pNear);
	}
}
