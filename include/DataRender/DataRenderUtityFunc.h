#pragma once
#include "DataRender\DataRenderDataDef.h"
#include "DataRender\MaterialDef.h"

//uint
BASEGRAPH_DATARENDER_API int sy_release_unit(SYUnit* punit);
BASEGRAPH_DATARENDER_API int sy_init_unit(SYUnit* punit, BOOL  b3D);
BASEGRAPH_DATARENDER_API int sy_copy_unit(SYUnit* pdes, SYUnit* psrc);
BASEGRAPH_DATARENDER_API void		sy_set_unit_material(SYUnit* p3DUnit, int nSubModelIndex, void* pMaterial);
BASEGRAPH_DATARENDER_API void*	sy_get_unit_material(SYUnit* p3DUnit, int nSubModelIndex);
BASEGRAPH_DATARENDER_API void		sy_release_unit_material(SYUnit* p3DUnit);
BASEGRAPH_DATARENDER_API void		sy_set_unit_material_diffuse(SYUnit* p3DUnit, DWORD dwColor);
BASEGRAPH_DATARENDER_API SYUnit*	sy_create_3dunit_from_2dunit(SYUnit* punit2d, SYUnit* pUnit3D);
//Effect
BASEGRAPH_DATARENDER_API void SYStaticEffect_Init(static_effect* pEffect);
BASEGRAPH_DATARENDER_API void SYStaticEffect_Release(static_effect* pEffect);
BASEGRAPH_DATARENDER_API void SYStaticEffect_Copy(static_effect* pDstEffect, const static_effect* pSrcEffect);
BASEGRAPH_DATARENDER_API void SYStaticEffect_SetSolidFaceColor(static_effect* pEffect, COLORREF color);
BASEGRAPH_DATARENDER_API void SYStaticEffect_SetDefaultSolidColor(static_effect* pEffect, COLORREF color);
BASEGRAPH_DATARENDER_API void sy_staticeffect_release(static_effect* pEffect);
BASEGRAPH_DATARENDER_API BOOL sy_staticeffect_compare(const static_effect* pEffect1, const static_effect* pEffect2);
BASEGRAPH_DATARENDER_API void sy_staticeffect_copy(static_effect* pDstEffect, const static_effect* pSrcEffect);
BASEGRAPH_DATARENDER_API void sy_special_effect_copy(syspecial_effect* pDstEffect, const syspecial_effect* pSrcEffect);
BASEGRAPH_DATARENDER_API void sy_special_effect_release(syspecial_effect* pEffect);
//vectorgraph
BASEGRAPH_DATARENDER_API int sy_release_vectorgraph(vectorgraph* pgraph);
BASEGRAPH_DATARENDER_API int  SYCopy_VectorGraph(vectorgraph* pdesgraph, vectorgraph* psrcgraph);
BASEGRAPH_DATARENDER_API void graph_release(vectorgraph* graph);
BASEGRAPH_DATARENDER_API void CopyVectorGraph(vectorgraph* pDesGraph, const vectorgraph* pSrcGraph);
BASEGRAPH_DATARENDER_API BOOL CreateRegionVectorGraphFromPoints(vectorgraph* pGraph, point2f pt[], int nCount, BOOL bClockWise);
BASEGRAPH_DATARENDER_API void	SYVectorGraph_AssignTTPOLYGONHEADER(vectorgraph* pGraph, TTPOLYGONHEADER* pSrcOutline, int nSize);//取字模函数  HTH,目前采用的TURETYPE来取
//model
BASEGRAPH_DATARENDER_API int sy_release_moddef(SYModel* pmod);
BASEGRAPH_DATARENDER_API int sy_release_viewsub(int hDevice, SYSubModel* pSubModel);
BASEGRAPH_DATARENDER_API void sy_copy_mod_viewsub(SYModel* pDst, const SYModel* pSrc);
BASEGRAPH_DATARENDER_API void sy_init_moddef(SYModel* pmod);
BASEGRAPH_DATARENDER_API void sy_copy_moddef(SYModel* pDst, const SYModel* pSrc);
//cell
BASEGRAPH_DATARENDER_API int sy_release_cell(vectorcell* pcell);
BASEGRAPH_DATARENDER_API int  cell_line_extend_close(OUT vectorcell* descell1, OUT vectorcell* descell2, IN vectorcell*	srccell, vector2f* vOffset, IN int ndirect, IN float fwidth);
BASEGRAPH_DATARENDER_API int  cell_extend_tri(OUT SYArray*	ptarray, OUT SYArray*	lenarray, IN  vectorcell*	srccell, IN  int ndirect, IN  float fwidth);
	//bone
BASEGRAPH_DATARENDER_API int sy_release_bonedef(SYBoneDef* pbone);
BASEGRAPH_DATARENDER_API void sy_copy_bone(SYBoneDef*	pDesBone, SYBoneDef*	pSrcBone);
//color
BASEGRAPH_DATARENDER_API void sy_lightinfo_copy(BaseGraph_DataBase::lightinfo* pDes, const BaseGraph_DataBase::lightinfo* pSrc);
BASEGRAPH_DATARENDER_API void sy_dynamictexture_copy(BaseGraph_DataBase::dynamictexture* pDes, const BaseGraph_DataBase::dynamictexture* pSrc);
BASEGRAPH_DATARENDER_API void sy_channelinfo_copy(BaseGraph_DataBase::channelinfo* pDes, const BaseGraph_DataBase::channelinfo* pSrc);
BASEGRAPH_DATARENDER_API void sy_materialinfo_copy(BaseGraph_DataBase::materialinfo* pDes, const BaseGraph_DataBase::materialinfo* pSrc);
//draw
BASEGRAPH_DATARENDER_API void SYRender_GetGraph_RenderInfo(vectorgraph* pGraph, float fscale, int& w, int& h, RECT& rtBound);
BASEGRAPH_DATARENDER_API BOOL SYRenderGraphToOpBuff(IN vectorgraph* pGraph, IN static_effect* effect, OUT CSYBuffer*& pResultPool, OUT RECT &rtOutBound, OUT int &nWidth, OUT int &nHeight, OUT int &nPitch, IN float fscale, OUT pointf& ptOrg, IN double dFrame, IN BOOL bShadowBaseline = FALSE, IN sy_render_channel** pChannelPool = FALSE);
BASEGRAPH_DATARENDER_API void SYData_ReleaseCalcBuffer(CSYBuffer* pBuffer);
BASEGRAPH_DATARENDER_API void SYRender_GenCubeGraph(vectorgraph* des, vectorgraph* src, vector2f* voffset, BaseGraph_DataBase::cubeinfo* pinfo);
BASEGRAPH_DATARENDER_API void SYRender_GenBorderGraph(vectorgraph* des, vectorgraph* src, vector2f* voffset, float fwidth, float finterval, int nLineStyle, BOOL bSingleLine);
BASEGRAPH_DATARENDER_API CSYBuffer* SYData_QueryCalcBuffer(int nSize, BOOL bMustSuccess = TRUE);
BASEGRAPH_DATARENDER_API BOOL SYRender_DrawGraph(vectorgraph* pGraph,
	DWORD* pBuffer,
	int w,
	int h,
	float fscale,
	RECT rtBorderBound,
	sycolor* pcolor,
	DWORD effect_type,
	DWORD effect_width,
	DWORD effect_soft,
	float effect_angle,
	DWORD effectcolor,
	int nframe,
	CSYBuffer** pRetMaskPool = NULL,
	int* nRetMaskWidth = NULL,
	int* nRetMaskHeight = NULL);
void Light(DWORD* dwBuffer, int nWidth, int nHeight, BaseGraph_DataBase::metalinfo* pinfo);
BASEGRAPH_DATARENDER_API int  cell_line_extend_close(OUT vectorcell* descell1, OUT vectorcell* descell2, IN vectorcell*	srccell, vector2f* vOffset, IN int ndirect, IN float fwidth);
BASEGRAPH_DATARENDER_API void	GetImagPointsByStyle(IN const SYArray& pptfArr1, IN const int nStyle, IN const float fLineWidth, OUT int& nCell, OUT SYArray& pptfArr);
BASEGRAPH_DATARENDER_API void	GetImagPointsDASH(IN const SYArray& pptfArr1, IN const float fLineWidth, OUT int& nCell, OUT SYArray& pptfArr, float *fIntervalAr, int intCount);
BASEGRAPH_DATARENDER_API void	GetImagPointsDASHDOT(IN const SYArray& pptfArr1, IN const float fLineWidth, OUT int& nCell, OUT SYArray& pptfArr);
BASEGRAPH_DATARENDER_API void	GetImagPointsLDASH(IN const SYArray& pptfArr1, IN const float fLineWidth, OUT int& nCell, OUT SYArray& pptfArr);
BASEGRAPH_DATARENDER_API void	GetImagPointsLDASHDOT(IN const SYArray& pptfArr1, IN const float fLineWidth, OUT int& nCell, OUT SYArray& pptfArr);
BASEGRAPH_DATARENDER_API void	GetPointsRIPPLE(IN const SYArray& pptfArr1, IN const float fLineWidth, OUT int& nCell, OUT SYArray& pptfArr);
BASEGRAPH_DATARENDER_API float GetDistace(const pointf* ptf1, const pointf* ptf2);
BASEGRAPH_DATARENDER_API float	GetCurveDistance(const SYArray* pptrArr);
BASEGRAPH_DATARENDER_API void CopyCell(vectorcell* pdescell, vectorcell* psrccell);
BASEGRAPH_DATARENDER_API void  effect_convex(unsigned char* pmask,
	int nmaskpitch,
	unsigned long* pbuffer,
	int nwidth,
	int nheight,
	int nborder,
	int nsoft,
	int ntype,
	float fangle,
	unsigned long color);
void	Modify3DUnitFrom2DUnit(SYUnit* punit3d, SYUnit* punit2d, SYMaterial** p3dcolor, int nSubFace, LPSYStretchParam pStretch);
