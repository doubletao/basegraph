#ifndef __Common_3DRENDERAPI_INC__
#define __Common_3DRENDERAPI_INC__
#include "DataRender\DataRenderDataDef.h"

//////////////////////////////////////////////////////////////////////////
#define FLOAT_PI 3.14159265358979323846
//for Bevel
class CSCGFileReader;
BASEGRAPH_DATARENDER_API void  BaseGraphCreateModelTangent(BaseGraphModel* pMod);
BASEGRAPH_DATARENDER_API BOOL  BaseGraphCreateTangent(BaseGraphVertex3D* pVertex,int nVNum,DWORD* pIndex,int nINum,int nStart,BaseGraphTriType nType);
BASEGRAPH_DATARENDER_API BOOL  BaseGraphInitialBevel(IN OUT BaseGraphModel* pMod);
BASEGRAPH_DATARENDER_API BOOL  BaseGraphInitialSphere(IN OUT BaseGraphModel* pMod, int nLevel = 1);
BASEGRAPH_DATARENDER_API BOOL  BaseGraphCreateSegmentGraph( IN OUT BaseGraphModel* pMod, IN float fLeftWidth, IN float fRightWidth, IN float fTopHeight, IN float fBottomHeight );
BASEGRAPH_DATARENDER_API BOOL  BaseGraphCreateBevelGraph( IN OUT BaseGraphModel* pMod, IN vectorgraph* p2DGraph, IN float fHeight, IN float fBevelWidth, IN float fBevelHeight, IN int nBevelType);
BASEGRAPH_DATARENDER_API BOOL  BaseGraphCreateSphereGraph( IN OUT BaseGraphModel* pMod, IN float fRad ,BOOL bSphereTex, IN int nLevel );
BASEGRAPH_DATARENDER_API BOOL  BaseGraphInitialSegment(IN OUT BaseGraphModel* pMod);
BASEGRAPH_DATARENDER_API BOOL  BaseGraphModel_CreateSegment( IN OUT BaseGraphModel* pMod, IN float fLeftWidth = 0.f, IN float fRightWidth = 0.f, IN float fTopHeight = 0.f, IN float fBottomHeight = 0.f );
BASEGRAPH_DATARENDER_API BOOL  BaseGraphModel_CreateBevel( IN OUT BaseGraphModel* pMod, IN vectorgraph* p2DGraph = NULL, IN float fHeight = NULL, IN float fBevelWidth = NULL, IN float fBevelHeight = NULL, IN const int nBevelType = NULL);
BASEGRAPH_DATARENDER_API BOOL  BaseGraphModel_CreateWidthLine( IN OUT vector3f* pFourVertex, const vector3f* pStart,const vector3f* pEnd, float width, const BaseGraphSpaceConvertParam* pProj);
// 球
BASEGRAPH_DATARENDER_API BOOL  BaseGraphModel_CreateSphere( IN OUT BaseGraphModel* pMod, IN float fRad ,IN BOOL bShpereTex,IN int nLevel = 1);
//倒角立方体
BASEGRAPH_DATARENDER_API BOOL	BaseGraphModel_CreateBevelBox(BaseGraphModel* pMod,float fLength,float fHeight,float fThick,
							int uPartType,
							int nTransitionType,float fTransitionLen,float fTransitionHeight,
							UINT nTSPrecission);
//柱体
BASEGRAPH_DATARENDER_API BOOL  BaseGraphModel_CreateCylinder(BaseGraphModel* pMod, BOOL bPrism,float fHeight, 
							 UINT uSlices,  
							 float fRU, float fRD, float frU, float frD,
							 float fTransitionLen, float fTransitionHeight,
							 int nTransitionType, int nTransitionPresition,
							 float fStartAng, float fEndAng);
//棱体
BASEGRAPH_DATARENDER_API BOOL BaseGraphModel_CreatePrism(BaseGraphModel* pMod, float fHeight, 
							 UINT uSlices, 
							 float fRU, float fRD, float frU, float frD,
							 float fTransitionLen, float fTransitionHeight,
							 int nTransitionType, int nTransitionPresition
							 );
//创建圆环
BASEGRAPH_DATARENDER_API BOOL BaseGraphModel_CreateRing(BaseGraphModel* pMod, UINT uSlices, UINT uStacks, 
						float fRU,  float frU,float fStartAng, float fEndAng);
//柱图
BASEGRAPH_DATARENDER_API BOOL  BaseGraphModel_CreateBar(BaseGraphModel* pMod,UINT uSharpType,float fRadius,UINT uFragMent,BOOL bText);
//翻牌
BASEGRAPH_DATARENDER_API BOOL  BaseGraphModel_CreateTablet(BaseGraphModel* pMod,UINT uModelType,UINT uStacks,
						int  nPartCount,int  nCount,float fLength,float fWeight,float fHeight,BOOL bText,int nIndex = 0);
//星
BASEGRAPH_DATARENDER_API BOOL  BaseGraphModel_CreateStar(BaseGraphModel* pMod,UINT uStacks,UINT uSlices,int  nPartCount,float fRU,float frU,float fRD,
						float frD,float fTLength,float fTHeight,float fThick);
//饼图
BASEGRAPH_DATARENDER_API BOOL  BaseGraphModel_CreatePie(BaseGraphModel* pMod,UINT uSharp,UINT uFragMent,float fAngle0,float fAngle1,float fRadius,float fThick);
//图上的标题字
BASEGRAPH_DATARENDER_API BOOL  BaseGraphModel_CreateTitlePlane(BaseGraphModel* pMod);
BASEGRAPH_DATARENDER_API BOOL BaseGraphModel_CreateChamferPie(	BaseGraphModel* pMod,
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
													UINT uChamferLayout);
BASEGRAPH_DATARENDER_API BOOL  BaseGraphModel_CreateRingPie(BaseGraphModel* pMod,UINT uSharp,UINT uFragMent,float fAngle0,float fAngle1,float fRadius,float fThick);
BASEGRAPH_DATARENDER_API BOOL  BaseGraphModel_CreateTablet_TwoFace(BaseGraphModel* pMod,float fWidth,
									   float fHeight,float fThick,int	 nCol,
									   int	 nRow,int	 col_count,
									   int	 row_count,BOOL bSharedTex);
BASEGRAPH_DATARENDER_API BOOL  BaseGraphModel_CreateTablet_TriFace(BaseGraphModel* pMod,float fWidth, float fHeight, float fThick,
									   int	 nCol,int	nRow,int	col_count,int	 row_count, BOOL bSharedTex);
BASEGRAPH_DATARENDER_API BOOL  BaseGraphModel_CreateTablet_Text_TwoFace(BaseGraphModel* pMod,float fWidth,float fHeight,float fThick,SIZE* txt_sz,BOOL bRotY);
BASEGRAPH_DATARENDER_API BOOL  BaseGraphModel_CreateTablet_Text_TriFace(BaseGraphModel* pMod,float fWidth,float fHeight,float fThick,SIZE* txt_sz);
BASEGRAPH_DATARENDER_API BOOL  BaseGraphModel_CreateXYPlane(BaseGraphModel* pMod,float width,float height, float3 OrginPt);
BASEGRAPH_DATARENDER_API BOOL  BaseGraphModel_CreateYZPlane(BaseGraphModel* pMod,float width,float height, float3 OrginPt);
BASEGRAPH_DATARENDER_API BOOL  BaseGraphModel_CreateZXPlane(BaseGraphModel* pMod,float width,float height, float3 OrginPt);
BASEGRAPH_DATARENDER_API BOOL  BaseGraphModel_CreateXYLabels(BaseGraphModel* pMod,float width,float height, float3 orginPt, UINT segment, float labelWidth);
BASEGRAPH_DATARENDER_API BOOL  BaseGraphModel_CreateYZLabels(BaseGraphModel* pMod,float width,float height, float3 OrginPt, UINT segment, float labelWidth);
BASEGRAPH_DATARENDER_API BOOL  BaseGraphModel_CreateZXLabels(BaseGraphModel* pMod,float width,float height, float3 OrginPt, UINT segment, float labelWidth);
BASEGRAPH_DATARENDER_API BOOL  BaseGraphModel_CreatePlaneLineModel(BaseGraphModel* pMod,float width,float z,float2* ptArray, int count);
BASEGRAPH_DATARENDER_API BOOL  BaseGraphModel_CreatePlaneLineModel2(BaseGraphModel* pMod,float height,float z,float2* ptArray, int count);
BASEGRAPH_DATARENDER_API BOOL  BaseGraphModel_CreateCubeLineModel(BaseGraphModel* pMod,float width,float Height,float z,float2* ptArray, int count);
//////////////////////////////////////////////////////////////////////////
#endif
