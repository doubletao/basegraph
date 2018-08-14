#ifndef __Common_3DRENDERAPI_INC__
#define __Common_3DRENDERAPI_INC__
#include "DataRender\DataRenderDataDef.h"

//////////////////////////////////////////////////////////////////////////
#define FLOAT_PI 3.14159265358979323846
//for Bevel
class CSCGFileReader;
BASEGRAPH_DATARENDER_API void  SYCreateModelTangent(SYModel* pMod);
BASEGRAPH_DATARENDER_API BOOL  SYCreateTangent(SYVertex3D* pVertex,int nVNum,DWORD* pIndex,int nINum,int nStart,SYTriType nType);
BASEGRAPH_DATARENDER_API BOOL  SYInitialBevel(IN OUT SYModel* pMod);
BASEGRAPH_DATARENDER_API BOOL  SYInitialSphere(IN OUT SYModel* pMod, int nLevel = 1);
BASEGRAPH_DATARENDER_API BOOL  SYCreateSegmentGraph( IN OUT SYModel* pMod, IN float fLeftWidth, IN float fRightWidth, IN float fTopHeight, IN float fBottomHeight );
BASEGRAPH_DATARENDER_API BOOL  SYCreateBevelGraph( IN OUT SYModel* pMod, IN vectorgraph* p2DGraph, IN float fHeight, IN float fBevelWidth, IN float fBevelHeight, IN int nBevelType);
BASEGRAPH_DATARENDER_API BOOL  SYCreateSphereGraph( IN OUT SYModel* pMod, IN float fRad ,BOOL bSphereTex, IN int nLevel );
BASEGRAPH_DATARENDER_API BOOL  SYInitialSegment(IN OUT SYModel* pMod);
BASEGRAPH_DATARENDER_API BOOL  SYModel_CreateSegment( IN OUT SYModel* pMod, IN float fLeftWidth = 0.f, IN float fRightWidth = 0.f, IN float fTopHeight = 0.f, IN float fBottomHeight = 0.f );
BASEGRAPH_DATARENDER_API BOOL  SYModel_CreateBevel( IN OUT SYModel* pMod, IN vectorgraph* p2DGraph = NULL, IN float fHeight = NULL, IN float fBevelWidth = NULL, IN float fBevelHeight = NULL, IN const int nBevelType = NULL);
BASEGRAPH_DATARENDER_API BOOL  SYModel_CreateWidthLine( IN OUT vector3f* pFourVertex, const vector3f* pStart,const vector3f* pEnd, float width, const SYSpaceConvertParam* pProj);
// 球
BASEGRAPH_DATARENDER_API BOOL  SYModel_CreateSphere( IN OUT SYModel* pMod, IN float fRad ,IN BOOL bShpereTex,IN int nLevel = 1);
//倒角立方体
BASEGRAPH_DATARENDER_API BOOL	SYModel_CreateBevelBox(SYModel* pMod,float fLength,float fHeight,float fThick,
							int uPartType,
							int nTransitionType,float fTransitionLen,float fTransitionHeight,
							UINT nTSPrecission);
//柱体
BASEGRAPH_DATARENDER_API BOOL  SYModel_CreateCylinder(SYModel* pMod, BOOL bPrism,float fHeight, 
							 UINT uSlices,  
							 float fRU, float fRD, float frU, float frD,
							 float fTransitionLen, float fTransitionHeight,
							 int nTransitionType, int nTransitionPresition,
							 float fStartAng, float fEndAng);
//棱体
BASEGRAPH_DATARENDER_API BOOL SYModel_CreatePrism(SYModel* pMod, float fHeight, 
							 UINT uSlices, 
							 float fRU, float fRD, float frU, float frD,
							 float fTransitionLen, float fTransitionHeight,
							 int nTransitionType, int nTransitionPresition
							 );
//创建圆环
BASEGRAPH_DATARENDER_API BOOL SYModel_CreateRing(SYModel* pMod, UINT uSlices, UINT uStacks, 
						float fRU,  float frU,float fStartAng, float fEndAng);
//柱图
BASEGRAPH_DATARENDER_API BOOL  SYModel_CreateBar(SYModel* pMod,UINT uSharpType,float fRadius,UINT uFragMent,BOOL bText);
//翻牌
BASEGRAPH_DATARENDER_API BOOL  SYModel_CreateTablet(SYModel* pMod,UINT uModelType,UINT uStacks,
						int  nPartCount,int  nCount,float fLength,float fWeight,float fHeight,BOOL bText,int nIndex = 0);
//星
BASEGRAPH_DATARENDER_API BOOL  SYModel_CreateStar(SYModel* pMod,UINT uStacks,UINT uSlices,int  nPartCount,float fRU,float frU,float fRD,
						float frD,float fTLength,float fTHeight,float fThick);
//饼图
BASEGRAPH_DATARENDER_API BOOL  SYModel_CreatePie(SYModel* pMod,UINT uSharp,UINT uFragMent,float fAngle0,float fAngle1,float fRadius,float fThick);
//图上的标题字
BASEGRAPH_DATARENDER_API BOOL  SYModel_CreateTitlePlane(SYModel* pMod);
BASEGRAPH_DATARENDER_API BOOL SYModel_CreateChamferPie(	SYModel* pMod,
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
BASEGRAPH_DATARENDER_API BOOL  SYModel_CreateRingPie(SYModel* pMod,UINT uSharp,UINT uFragMent,float fAngle0,float fAngle1,float fRadius,float fThick);
BASEGRAPH_DATARENDER_API BOOL  SYModel_CreateTablet_TwoFace(SYModel* pMod,float fWidth,
									   float fHeight,float fThick,int	 nCol,
									   int	 nRow,int	 col_count,
									   int	 row_count,BOOL bSharedTex);
BASEGRAPH_DATARENDER_API BOOL  SYModel_CreateTablet_TriFace(SYModel* pMod,float fWidth, float fHeight, float fThick,
									   int	 nCol,int	nRow,int	col_count,int	 row_count, BOOL bSharedTex);
BASEGRAPH_DATARENDER_API BOOL  SYModel_CreateTablet_Text_TwoFace(SYModel* pMod,float fWidth,float fHeight,float fThick,SIZE* txt_sz,BOOL bRotY);
BASEGRAPH_DATARENDER_API BOOL  SYModel_CreateTablet_Text_TriFace(SYModel* pMod,float fWidth,float fHeight,float fThick,SIZE* txt_sz);
BASEGRAPH_DATARENDER_API BOOL  SYModel_CreateXYPlane(SYModel* pMod,float width,float height, float3 OrginPt);
BASEGRAPH_DATARENDER_API BOOL  SYModel_CreateYZPlane(SYModel* pMod,float width,float height, float3 OrginPt);
BASEGRAPH_DATARENDER_API BOOL  SYModel_CreateZXPlane(SYModel* pMod,float width,float height, float3 OrginPt);
BASEGRAPH_DATARENDER_API BOOL  SYModel_CreateXYLabels(SYModel* pMod,float width,float height, float3 orginPt, UINT segment, float labelWidth);
BASEGRAPH_DATARENDER_API BOOL  SYModel_CreateYZLabels(SYModel* pMod,float width,float height, float3 OrginPt, UINT segment, float labelWidth);
BASEGRAPH_DATARENDER_API BOOL  SYModel_CreateZXLabels(SYModel* pMod,float width,float height, float3 OrginPt, UINT segment, float labelWidth);
BASEGRAPH_DATARENDER_API BOOL  SYModel_CreatePlaneLineModel(SYModel* pMod,float width,float z,float2* ptArray, int count);
BASEGRAPH_DATARENDER_API BOOL  SYModel_CreatePlaneLineModel2(SYModel* pMod,float height,float z,float2* ptArray, int count);
BASEGRAPH_DATARENDER_API BOOL  SYModel_CreateCubeLineModel(SYModel* pMod,float width,float Height,float z,float2* ptArray, int count);
//////////////////////////////////////////////////////////////////////////
#endif
