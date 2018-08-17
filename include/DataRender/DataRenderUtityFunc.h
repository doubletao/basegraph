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
BASEGRAPH_DATARENDER_API SYItemRenderUnit * CreateRenderUnit(BOOL b3dtype);
BASEGRAPH_DATARENDER_API void ReleaseRenderUnit(SYItemRenderUnit* pRenderUnit, BOOL b3dType, int hDevice, void* lpCalc);
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
BASEGRAPH_DATARENDER_API void SYVectorGraph_Offset(vectorgraph* pGraph, float nOffsetX, float nOffsetY);
BASEGRAPH_DATARENDER_API void SYVectorGraph_Release(vectorgraph* pGraph);
BASEGRAPH_DATARENDER_API BOOL CreateRegionVectorGraphFromPoints(vectorgraph* pGraph, point2f pt[], int nCount, BOOL bClockWise);
BASEGRAPH_DATARENDER_API void SYVectorGraph_AssignTTPOLYGONHEADER(vectorgraph* pGraph, TTPOLYGONHEADER* pSrcOutline, int nSize);//取字模函数  HTH,目前采用的TURETYPE来取
BASEGRAPH_DATARENDER_API void MergeVectorGraph(vectorgraph* pDesGraph, const vectorgraph* pSrcGraph1, vectorgraph* pSrcGraph2);
BASEGRAPH_DATARENDER_API void ReverseVectorGraph(vectorgraph* pDesGraph);
//model
BASEGRAPH_DATARENDER_API int sy_release_moddef(SYModel* pmod);
BASEGRAPH_DATARENDER_API int sy_release_viewsub(int hDevice, SYSubModel* pSubModel);
BASEGRAPH_DATARENDER_API void sy_copy_mod_viewsub(SYModel* pDst, const SYModel* pSrc);
BASEGRAPH_DATARENDER_API void sy_init_moddef(SYModel* pmod);
BASEGRAPH_DATARENDER_API void sy_copy_moddef(SYModel* pDst, const SYModel* pSrc);
//cell
BASEGRAPH_DATARENDER_API int sy_release_cell(vectorcell* pcell);
BASEGRAPH_DATARENDER_API int cell_line_extend_close(OUT vectorcell* descell1, OUT vectorcell* descell2, IN vectorcell*	srccell, vector2f* vOffset, IN int ndirect, IN float fwidth);
BASEGRAPH_DATARENDER_API int cell_extend_tri(OUT BaseARray*	ptarray, OUT BaseARray*	lenarray, IN  vectorcell*	srccell, IN  int ndirect, IN  float fwidth);
BASEGRAPH_DATARENDER_API void SYCurveCell_Init(vectorcell* pCell);
BASEGRAPH_DATARENDER_API void SYCurveCell_Release(vectorcell* pCell);
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
BASEGRAPH_DATARENDER_API BOOL SYRenderGraphToOpBuff(IN vectorgraph* pGraph, IN static_effect* effect, OUT CCommonBuffer*& pResultPool, OUT RECT &rtOutBound, OUT int &nWidth, OUT int &nHeight, OUT int &nPitch, IN float fscale, OUT pointf& ptOrg, IN double dFrame, IN BOOL bShadowBaseline = FALSE, IN sy_render_channel** pChannelPool = FALSE);
BASEGRAPH_DATARENDER_API void Data_ReleaseCalcBuffer(CCommonBuffer* pBuffer);
BASEGRAPH_DATARENDER_API void SYRender_GenCubeGraph(vectorgraph* des, vectorgraph* src, vector2f* voffset, BaseGraph_DataBase::cubeinfo* pinfo);
BASEGRAPH_DATARENDER_API void SYRender_GenBorderGraph(vectorgraph* des, vectorgraph* src, vector2f* voffset, float fwidth, float finterval, int nLineStyle, BOOL bSingleLine);
BASEGRAPH_DATARENDER_API CCommonBuffer* Data_QueryCalcBuffer(int nSize, BOOL bMustSuccess = TRUE);
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
	CCommonBuffer** pRetMaskPool = NULL,
	int* nRetMaskWidth = NULL,
	int* nRetMaskHeight = NULL);
void Light(DWORD* dwBuffer, int nWidth, int nHeight, BaseGraph_DataBase::metalinfo* pinfo);
BASEGRAPH_DATARENDER_API int  cell_line_extend_close(OUT vectorcell* descell1, OUT vectorcell* descell2, IN vectorcell*	srccell, vector2f* vOffset, IN int ndirect, IN float fwidth);
BASEGRAPH_DATARENDER_API void	GetImagPointsByStyle(IN const BaseARray& pptfArr1, IN const int nStyle, IN const float fLineWidth, OUT int& nCell, OUT BaseARray& pptfArr);
BASEGRAPH_DATARENDER_API void	GetImagPointsDASH(IN const BaseARray& pptfArr1, IN const float fLineWidth, OUT int& nCell, OUT BaseARray& pptfArr, float *fIntervalAr, int intCount);
BASEGRAPH_DATARENDER_API void	GetImagPointsDASHDOT(IN const BaseARray& pptfArr1, IN const float fLineWidth, OUT int& nCell, OUT BaseARray& pptfArr);
BASEGRAPH_DATARENDER_API void	GetImagPointsLDASH(IN const BaseARray& pptfArr1, IN const float fLineWidth, OUT int& nCell, OUT BaseARray& pptfArr);
BASEGRAPH_DATARENDER_API void	GetImagPointsLDASHDOT(IN const BaseARray& pptfArr1, IN const float fLineWidth, OUT int& nCell, OUT BaseARray& pptfArr);
BASEGRAPH_DATARENDER_API void	GetPointsRIPPLE(IN const BaseARray& pptfArr1, IN const float fLineWidth, OUT int& nCell, OUT BaseARray& pptfArr);
BASEGRAPH_DATARENDER_API float GetDistace(const pointf* ptf1, const pointf* ptf2);
BASEGRAPH_DATARENDER_API float	GetCurveDistance(const BaseARray* pptrArr);
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
void	Modify3DUnitFrom2DUnit(SYUnit* punit3d, SYUnit* punit2d, SYMaterial** p3dcolor, int nSubFace, LPCommonStretchParam pStretch);

/************************************************************************/
/*rectf& rtDes目标矩形                                                  */
/*pDesVector4D 目标顶点                                                 */
/*rtSrc 源矩形													*/
/*pTransMatrix变换矩阵												*/
/*pCenterMatrix中心变换矩阵										*/
/*pProjectMatrix投影矩阵											*/
/*int nSrcWidth,int nSrcHeight目标大小								*/
/*dwProjectType投影种类											*/
/************************************************************************/
BASEGRAPH_DATARENDER_API void CalcBufferProjectSize(rectf& rtDes, vector3f* pDesVector3D, RECT& rtSrc, matrix4f* pTransMatrix);
BASEGRAPH_DATARENDER_API void CalcBufferProjectSize(rectf& rtDes, vector3f* pDesVector3D, rectf& rtSrc, matrix4f* pTransMatrix);
BASEGRAPH_DATARENDER_API void CalcPointProject(vector4f* pV4D, matrix4f* pTransMatrix, matrix4f* pProjectMatrix, vector4f* pvCenter);
//trans
BASEGRAPH_DATARENDER_API void calc_trans_group_tovector(OUT vector3f *pvpos, OUT vector3f *pvscale, OUT SYQuatn* pTcb, OUT vector3f* pEur, OUT vector3f *pcenter, IN double dframe, IN vector3f* pamendpos = NULL, IN vector3f* pamendscale = NULL, IN vector3f* pamendrotate = NULL);
BASEGRAPH_DATARENDER_API void calc_trans_group(OUT matrix4f* pvmatrix, OUT vector3f *pvpos, /*同时输出插值的向量 */ OUT vector3f *pvscale, OUT vector3f* pAngle, OUT vector3f *pcenter, IN double dFrame, IN vector3f* pivot, /*形变参考点坐标 */ int nRotateOrder = ROTATE_ORDER_YXZ, IN vector3f* pamendpos = NULL, IN vector3f* pamendscale = NULL, IN vector3f* pamendrotate = NULL);