/****************************************************************************/
/*		求解射线与规则几何体的相交点的相关方法							*/
/*BOOL GetPoint_Intersection(IN void* paramin,OUT void* paramout,int type)	*/
/*返回值：真表示有交点，假表示无交点									*/
/*type:表示相交类型。													*/
/*其中cone,sphere的相交，其座标系为cone和sphere的自身坐标系，并非世界坐标	*/
/****************************************************************************/
#include "SceneRender\3DSpaceConvert.h"
#include "SceneRender\DXEngineDataDef.h"
#ifndef		Common_INTERSECTION_INC__
#define		Common_INTERSECTION_INC__
#define		POINT_DEVIATION		8.f
#define		INTERSECTION_FAR	2
#define		NO_INTERSECTION		0
#define		INTERSECTION_NEAR	1
#define		INTERSECTION_FAR	2
#define		INTERSECTION_TWO	3
#define		INTERSECTION_EDGE	4
//定义相交类型
//射线与圆柱相交
#define		RAY_TO_CONE			0 
#define		RAY_TO_SPHERA		1
#define		RAY_TO_QUADRANGULAR	2
//射线与面片相交
#define		RAY_TO_SEGMENT		3
#define		RAY_TO_3DCIRCLE		4
#define		RAY_TO_LINE_SEG		5
#define		RAY_TO_PLANE		6
#define		RAY_TO_TRIANGLE		7
#define		RAY_TO_3DPOINT		8
#define		RAY_TO_RAY_SEG		9
//与四面体相交
#define		RAY_TO_TETRAHEDRON	10
//与管状体相交
#define		RAY_TO_TUBE			11;
BASEGRAPH_SCENE_RENDER_API int	GetPoint_Intersection(IN const void* paramin,OUT Point_Intersection* pOut,const int type);
BASEGRAPH_SCENE_RENDER_API void	GetCubeIntersection(const vector3f* center,float fHeight,DX_QuadRang_Intersection* pInt,const matrix4f* pMatrix = NULL);
BASEGRAPH_SCENE_RENDER_API void	Generate_Quatn_BetweenDir( OUT BaseGraphQuatn* pqQuatn, IN vector3f* pvStartDir, IN vector3f* pvEndDir );
BASEGRAPH_SCENE_RENDER_API BOOL	IntersectTriangle( const vector3f* orig,const vector3f* dir,const vector3f* v0,const vector3f* v1,const vector3f* v2, vector3f* pOutPt );
class BASEGRAPH_SCENE_RENDER_API BaseGraphHitPoint
{
public:
	BaseGraphHitPoint()
	{ 
		BaseAR_Init(&m_arHitPoint);
	}
	~BaseGraphHitPoint();
	int AddPoint(const float3* pHitPoint);
	bool DelPoint(int nIndex);
	const float3* GetPoint(int nIndex)const 
	{
		if (nIndex >= GetPointCount())
			return NULL;
		return (float3*)BaseAR_GetAt((LPBaseARRAY)&m_arHitPoint,nIndex);
	}
	int GetPointCount()const
	{ 
		return (int)BaseAR_GetCount(&m_arHitPoint);
	}
	bool DelLast()
	{
		return DelPoint(GetPointCount() - 1);
	} 
	const float3& operator [](int nIndex)const;
	int GetNearestPt( matrix4f *mat )
	{
		if( m_arHitPoint.m_nSize <= 0 )
			return -1;

		if( m_arHitPoint.m_nSize == 1 )
			return 0;

		int nearestPoint = 0;
		float3 tempViewPoint = *GetPoint(nearestPoint) * *mat;
		int nCount = GetPointCount();
		for (int i = 1; i < nCount; i++)
		{
			float3 nowViewPoint = *GetPoint(i) * *mat;
			if (nowViewPoint.z < tempViewPoint.z)
			{
				tempViewPoint = nowViewPoint;
				nearestPoint = i;
			}
		}

		return nearestPoint;
	};
protected:
	BaseARray m_arHitPoint;//float3 BaseARray
};
struct BaseGraphRay
{
	float3 vStart;
	float3 vDir;
};
typedef struct BaseGraphRay BaseGraphLine;
enum BaseGraphHitType
{
	Common_NO_HIT = 0,
	Common_NORMAL_HIT,
	Common_HIT_EDGE,
};
BASEGRAPH_SCENE_RENDER_API void GetRayDirection(BaseGraphRay* pRay,const float3* pScreenPoint,const BaseGraphSpaceConvertParam* pParam);
BASEGRAPH_SCENE_RENDER_API int GetSegmentCircleInt(const float3* pSegStart,const float3* pSegEnd,float fRadius,const float3* pCenter,const float3* pNormal,BaseGraphHitPoint* pHitPoint);
//球的中心在圆点，射线在球的空间内
BASEGRAPH_SCENE_RENDER_API int GetRaySphereInt(const BaseGraphRay* pRay,float fRadius,BaseGraphHitPoint* pHitPoint);
//射线与平面相交
BASEGRAPH_SCENE_RENDER_API int GetRayPlaneInt(const BaseGraphRay* pRay,const float3* pCenter,const float3* pNormal,BaseGraphHitPoint* pHitPoint);
//射线与平面圆相交
BASEGRAPH_SCENE_RENDER_API BOOL GetCircleInt( const BaseGraphRay* pRay,const float3* pCenter,const float3* pNormal,float Radius,BaseGraphHitPoint* pHitPoint );
//射线与点相交
BASEGRAPH_SCENE_RENDER_API BOOL GetPointInt( const BaseGraphRay* pRay,const float3* pPoint,float fDeviation = POINT_DEVIATION );
BASEGRAPH_SCENE_RENDER_API int GetRayConeInt( const BaseGraphRay* pRay,const DX_Cone_Parameter* pCone,BaseGraphHitPoint* pHitPoint );
BASEGRAPH_SCENE_RENDER_API int GetRayBoxInt( const BaseGraphRay* pRay,const float3* pMin,const float3* pMax,BaseGraphHitPoint* pHitPoint );

BASEGRAPH_SCENE_RENDER_API BOOL GetTriangleInt( const BaseGraphRay* pRay, const float3*  v0, const float3*  v1, const float3*  v2, BaseGraphHitPoint* pHitPoint );
BASEGRAPH_SCENE_RENDER_API BOOL GetRaySegment(const BaseGraphRay* pRay,const float3& v1,const float3& v2,const float3& v3,const float3& v4,BaseGraphHitPoint* pHitPoint);//顺时针
BASEGRAPH_SCENE_RENDER_API int BaseGraph3DModelHitTest( int nSubModel,
					  const DXRenderSubModel* pSubModelDes,
					  const DWORD* pIndex,
					  const DXVertex3D* pVertex,          //物件顶点
					  const float3* pvScreenPt,	//屏幕坐标
					  const BaseGraphSpaceConvertParam* pParam,//其中的WORLDMatrix应为物件的世界矩阵
					  float3* pOutPt = NULL,BOOL isHitReturn = TRUE);
BASEGRAPH_SCENE_RENDER_API int BaseGraph3DRayHitModel( int nSubModel,
									const DXRenderSubModel* pSubModelDes,
									const DWORD* pIndex,
									const DXVertex3D* pVertex,          //物件顶点
									const BaseGraphRay* pRay,	//屏幕坐标
									const BaseGraphSpaceConvertParam* pParam,//其中的WORLDMatrix应为物件的世界矩阵
									float3* pOutPt = NULL,BOOL isHitReturn = TRUE);
//二维向量在三维方向上的投影长度
BASEGRAPH_SCENE_RENDER_API float BaseGraphMouseMoveProjToDir( IN const BaseGraphSpaceConvertParam* pSpaceParam,
											IN const float3* p2DStart, IN const float3* p2DEnd, 
											const BaseGraphLine* pLine,const float3* pNormal = NULL);

//2D平面上两个点在指定三维平面上的夹角
BASEGRAPH_SCENE_RENDER_API float BaseGraph3DProjAngle(const BaseGraphSpaceConvertParam* pSpaceParam,
					  IN const float3* p2DStart, //2D平面起始点
					  IN const float3* p2DEnd, //2D平面结束点
					  const float3* pCenter,const float3* pNormal);//3D平面
//通过最大值，最小值，生成矩形盒,pEightPt至少包含8个点
BASEGRAPH_SCENE_RENDER_API void CreateBox( float3* pEightPt,const float3& vMin,const float3& vMax,float fWidth = 0,float fHeight = 0,float fThick = 0 );
#endif
