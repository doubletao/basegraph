#pragma once
//////////////////////////////////////////////////////////////////////////
class ObjectCalculator;
/************************************************************************/
/* 物件自身空间的变换阵                                                 */
/************************************************************************/
class BASEGRAPH_DATARENDER_API CSpaceConvertMatrix
{
public:
	CSpaceConvertMatrix( ObjectCalculator *pCalculator );
	virtual ~CSpaceConvertMatrix();
	virtual void InitMatrix();
	matrix4f	*getWorld();
	matrix4f	*getAdjust();
	matrix4f	*getAdjustInv();
protected:
	virtual matrix4f GetOriginWorld();
protected:
	matrix4f m_matWorld;
	matrix4f m_matAdjust2D;
	matrix4f m_matAdjust2DInv;
	BOOL	 m_bNeedAdjust;
	ObjectCalculator *m_pCalculator;
};
/************************************************************************/
/* Unit空间的变换阵                                                     */
/************************************************************************/
class BASEGRAPH_DATARENDER_API CUnitSpaceConvertMatrix : public CSpaceConvertMatrix
{
public:
	CUnitSpaceConvertMatrix( ObjectCalculator *pCalculator, LPSYARRAY pary3DUnit, int nIndex );
	virtual ~CUnitSpaceConvertMatrix();
protected:
	virtual matrix4f GetOriginWorld();
private:
	LPSYARRAY m_pary3DUnit;
	int		  m_nUnitIndex;
};

class CParentSpaceConvert;
class CSimpleSpaceConvert;
BASEGRAPH_DATARENDER_API CSimpleSpaceConvert *CreateSimpleSpaceConvert( ObjectCalculator *pCalculator, BOOL bForEdit, BOOL bUse2DReal = TRUE, CSpaceConvertMatrix *pConvertMatrix = NULL );
BASEGRAPH_DATARENDER_API void ReleaseSimpleSpaceConvert( CSimpleSpaceConvert *pConvert );
/*
物件有三种渲染模式 2D模式和3D模式 3DIn2D模式
当2D模式下存在几种空间：自身空间, 2D Real空间
当3D模式下存在几种空间: 自身空间, 世界空间，摄像机空间,2D REAL空间
*/

/////////////////////转换工具（可以支持2D层摄像机)//////////
class BASEGRAPH_DATARENDER_API CSimpleSpaceConvert
{
	friend BASEGRAPH_DATARENDER_API CSimpleSpaceConvert *CreateSimpleSpaceConvert( ObjectCalculator *pCalculator, BOOL bForEdit, BOOL bUse2DReal, CSpaceConvertMatrix *pConvertMatrix );
	friend BASEGRAPH_DATARENDER_API void ReleaseSimpleSpaceConvert( CSimpleSpaceConvert *pConvert );
protected:
	CSimpleSpaceConvert( ObjectCalculator *pCalculator, BOOL bForEdit, BOOL bUse2DReal = TRUE, CSpaceConvertMatrix *pConvertMatrix=NULL );
	virtual ~CSimpleSpaceConvert();
public:
	/*
	bUsedEditCamera 这里有两种摄像机，编辑线程和渲染线程使用不同的摄像机。
					一定要根据线程选择。如果在编辑线程做渲染操作，需要选择编辑摄像机。因为在编辑线程，渲染摄像机是不确定的。
	bUse2DReal		表示使用哪个二维坐标。一种是场景坐标系，仅和场景的大小相关，一种是渲染幅面下的坐标，和渲染矩形相关,有缩放和偏移。
	pConvertMatrix  指定3D空间。可以有多种3D空间，默认为物件自身空间，还可以为Unit空间，父空间,场景空间
	*/
	//将某空间下的3D点转换
	virtual void Convert3DPtTo2D( const float3 *ptSrc, float3 *pDest );
	virtual void Convert3DPtToView( const float3 *ptSrc, float3 *pDest );
	virtual void ConvertViewPtTo3D( const float3 *ptSrc, float3 *pDest );
	virtual void Convert3DPtToWorld( const float3 *pSrc, float3 *pDest);
	virtual void ConvertWorldPtTo3D( const float3 *pSrc, float3 *pDest);
	virtual void Convert2DPtTo3D_ZViewPlane( const float3 *ptSrc, float3 *ptDest, float fZValue);	//返回View空间下
	virtual void Convert2DPtTo3D_NearWorldPlane( const float3 *ptSrc, float3 *ptDest );
	virtual BOOL Convert2DPtTo3D_Plane( const float3 *ptSrc, float3 *ptDest, float3 *vPlaneCenter, float3 *vPlaneNormal );
	void Convert2DPtTo3D_ZPlane( const float3 *ptSrc, float3 *ptDest);
	void ConvertVectorGraphSpace( vectorgraph* pGraph, float fSignY );
	void ComputeNew3DptToViewPlan( const float3 *ptSrc, float3 *ptDest, float fZValue );
	virtual void GetRayDirection( OUT SYRay *pRay, IN float3 *ptHit );
	SYSpaceConvertParam *getSpaceConvertParam(){ return &m_ConvertParam;}
	matrix4f getWorldMatrix();
	matrix4f getViewProjectMatrix();
	matrix4f getViewMatrix();
	void get3DTo2DMatrix( matrix4f *mat );
	float GetViewWorkPlaneDis();
	BOOL CheckPtInView(const float3* vPt,int nCount);
	ObjectCalculator *m_pCalculator;
	float m_fViewWorkPlaneDis;
protected:
	SYSpaceConvertParam	m_ConvertParam;
	CSpaceConvertMatrix *	m_pConvertMatrix;
	CParentSpaceConvert *	m_pParentConvert;	/*如果不时在场景空间中需要二次转换*/
	BOOL					m_b3DRenderType;
	BOOL					m_bReleaseConvert;
};
class CParentSpaceConvert
{
public:
	CParentSpaceConvert( ObjectCalculator *pCalculator, BOOL bForEdit, BOOL bUse2DReal );
	~CParentSpaceConvert();
	void Convert2DToParent2D( const float3 *ptSrc, float3 *ptDest );
	void ConvertParent2DTo2D( const float3 *ptSrc, float3 *ptDest );
	matrix4f getParentWorldMatrix();
	void getViewProjMatrix( matrix4f *matViewProj );
	void getParent2DTo2DMatrix( matrix4f* mat );
private:
	ObjectCalculator*	m_pCalculator;
	float3		m_vSceneCenter;
	CSimpleSpaceConvert*	m_pSpaceConvert;
	matrix4f		m_ScrollMatrix;
	matrix4f		m_matBufferToScene;
};
class BASEGRAPH_DATARENDER_API CSimpleSpaceConvert2D : public CSimpleSpaceConvert
{
public:
	/*
	bUsedEditCamera 这里有两种摄像机，编辑线程和渲染线程使用不同的摄像机。
					一定要根据线程选择。如果在编辑线程做渲染操作，需要选择编辑摄像机。因为在编辑线程，渲染摄像机是不确定的。
	bUse2DReal		表示使用哪个二维坐标。一种是场景坐标系，仅和场景的大小相关，一种是渲染幅面下的坐标，和渲染矩形相关,有缩放和偏移。
	pConvertMatrix  指定3D空间。可以有多种3D空间，默认为物件自身空间，还可以为Unit空间，父空间,场景空间
	*/
	CSimpleSpaceConvert2D( ObjectCalculator *pCalculator, BOOL bForEdit, BOOL bUse2DReal, CSpaceConvertMatrix *pConvertMatrix );
	virtual ~CSimpleSpaceConvert2D();
	virtual void Convert3DPtTo2D( const float3 *ptSrc, float3 *pDest );
	virtual void Convert3DPtToView( const float3 *ptSrc, float3 *pDest );
	virtual void ConvertViewPtTo3D( const float3 *ptSrc, float3 *pDest );
	virtual void Convert3DPtToWorld( const float3 *pSrc, float3 *pDest);
	virtual void ConvertWorldPtTo3D( const float3 *pSrc, float3 *pDest);

	virtual void Convert2DPtTo3D_ZViewPlane( const float3 *ptSrc, float3 *ptDest, float fZValue);
	virtual void Convert2DPtTo3D_NearWorldPlane( const float3 *ptSrc, float3 *ptDest );
	virtual BOOL Convert2DPtTo3D_Plane( const float3 *ptSrc, float3 *ptDest, float3 *vPlaneCenter, float3 *vPlaneNormal );
	virtual void GetRayDirection( OUT SYRay *pRay, IN float3 *ptHit );
private:
	float3 m_vSceneCenter;
};
class  BASEGRAPH_DATARENDER_API CSimpleSpaceConvert3DIn2D : public CSimpleSpaceConvert
{
public:

	/*
	bUsedEditCamera 这里有两种摄像机，编辑线程和渲染线程使用不同的摄像机。
					一定要根据线程选择。如果在编辑线程做渲染操作，需要选择编辑摄像机。因为在编辑线程，渲染摄像机是不确定的。
	bUse2DReal		表示使用哪个二维坐标。一种是场景坐标系，仅和场景的大小相关，一种是渲染幅面下的坐标，和渲染矩形相关,有缩放和偏移。
	pConvertMatrix  指定3D空间。可以有多种3D空间，默认为物件自身空间，还可以为Unit空间，父空间,场景空间
	*/
	CSimpleSpaceConvert3DIn2D( ObjectCalculator *pCalculator, BOOL bForEdit, BOOL bUse2DReal, CSpaceConvertMatrix *pConvertMatrix );
	virtual ~CSimpleSpaceConvert3DIn2D();
	virtual void Convert3DPtTo2D( const float3 *ptSrc, float3 *pDest );

	virtual void Convert2DPtTo3D_ZViewPlane( const float3 *ptSrc, float3 *ptDest, float fZValue);
	virtual void Convert2DPtTo3D_NearWorldPlane( const float3 *ptSrc, float3 *ptDest );
	virtual BOOL Convert2DPtTo3D_Plane( const float3 *ptSrc, float3 *ptDest, float3 *vPlaneCenter, float3 *vPlaneNormal );
	virtual void ConvertViewPtTo3D( const float3 *ptSrc, float3 *pDest );
private:
	float3 m_vSceneCenter;
};
class  BASEGRAPH_DATARENDER_API CSimpleSpaceConvert2DRenderBy3D : public CSimpleSpaceConvert
{
public:

	/*
	bUsedEditCamera 这里有两种摄像机，编辑线程和渲染线程使用不同的摄像机。
					一定要根据线程选择。如果在编辑线程做渲染操作，需要选择编辑摄像机。因为在编辑线程，渲染摄像机是不确定的。
	bUse2DReal		表示使用哪个二维坐标。一种是场景坐标系，仅和场景的大小相关，一种是渲染幅面下的坐标，和渲染矩形相关,有缩放和偏移。
	pConvertMatrix  指定3D空间。可以有多种3D空间，默认为物件自身空间，还可以为Unit空间，父空间,场景空间
	*/
	CSimpleSpaceConvert2DRenderBy3D( ObjectCalculator *pCalculator, BOOL bForEdit, BOOL bUse2DReal, CSpaceConvertMatrix *pConvertMatrix );
	virtual ~CSimpleSpaceConvert2DRenderBy3D();
	virtual void Convert3DPtTo2D( const float3 *ptSrc, float3 *pDest );

	virtual void Convert2DPtTo3D_ZViewPlane( const float3 *ptSrc, float3 *ptDest, float fZValue);
	virtual void Convert2DPtTo3D_NearWorldPlane( const float3 *ptSrc, float3 *ptDest );
	virtual BOOL Convert2DPtTo3D_Plane( const float3 *ptSrc, float3 *ptDest, float3 *vPlaneCenter, float3 *vPlaneNormal );
	virtual void ConvertViewPtTo3D( const float3 *ptSrc, float3 *pDest );
	virtual void GetRayDirection( OUT SYRay *pRay, IN float3 *ptHit );
private:
	float3 m_vSceneCenter;
};

void BASEGRAPH_DATARENDER_API SYCaclTransMatrixFromTwoPointInReal(matrix4f *dest, rectf *rtBound, const float3 *pt0, const float3 *pt1, CString strParentID);