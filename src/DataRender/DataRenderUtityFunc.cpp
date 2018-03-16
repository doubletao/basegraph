#include "stdafx.h"
#include "DataRender\DataRenderUtityFunc.h"
#include "DataRender\AlgorithmRip.h"
#include "DataRender\OffsetCurve.h"
#include "DataRender\3DGeometry.h"
#include "DataRender\Arc2BezierAPI.h"
#include "CalculatorTitleWord.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define		SYLINE_SOLID					0
#define		SYLINE_DASH						1
#define		SYLINE_DASHDOT					2
#define		SYLINE_LDASH					3
#define		SYLINE_LDASHDOT					4
#define		SYLINE_RIPPLE					5

#define		CHANNEL_ALPHA					0
#define		CHANNEL_COUNT					5
#define		SHADOW_INSERT					0x20
#define		CHANNEL_MATERIAL				1
#define		CHANNEL_FACELIGHT				3
#define		CHANNEL_CUBELIGHT				4

#define STATCEFFECT_TYPE_NONE		0
#define STATCEFFECT_TYPE_CONCAVE	1		//凹
#define STATCEFFECT_TYPE_CONVEX		2		//凸
#define STATCEFFECT_TYPE_LIGHT		3		//亮边

typedef CSYArray<CSYPoint2fPtrArray*> CPtfArrArr; // pointf二维数组
LPSYARRAY calc_midderbuf_array = NULL;
int maxsize = 0;

BASEGRAPH_DATARENDER_API int sy_release_unit(SYUnit * punit)
{
	if (!punit)
		return 0;

	if (punit->type == UNIT_2D)
	{
		int nloadlen = sy_release_vectorgraph(&punit->u.u2d.graph);
	}
	else
	{
		if (punit->u.u3d.pmod)
		{
			int nloadlen = sy_release_moddef(punit->u.u3d.pmod);
			SAFE_DELETE(punit->u.u3d.pmod);
		}
	}

	return 1;
}

BASEGRAPH_DATARENDER_API int sy_init_unit(SYUnit * punit, BOOL dwtype)
{
	memset(punit, 0, sizeof(SYUnit));
	matrix4f_identity(&punit->MatVToItem);
	//matrix4f_identity(&punit->MatNToItem);
	if (dwtype)
		punit->type = UNIT_3D;

	punit->parentbone = -1;
	return 1;
}

BASEGRAPH_DATARENDER_API int sy_copy_unit(SYUnit * pdesunit, SYUnit * psrcunit)
{
	if (!psrcunit)
		return 0;
	//先释放
	sy_release_unit(pdesunit);
	pdesunit->type = psrcunit->type;
	pdesunit->dwUserID = psrcunit->dwUserID;
	pdesunit->parentbone = psrcunit->parentbone;
	pdesunit->pos = psrcunit->pos;
	pdesunit->MatVToItem = psrcunit->MatVToItem;
	_tcscpy_s(pdesunit->szname, psrcunit->szname);

	if (pdesunit->type == UNIT_2D)
	{
		sy_release_vectorgraph(&pdesunit->u.u2d.graph);
		SYCopy_VectorGraph(&pdesunit->u.u2d.graph, &psrcunit->u.u2d.graph);
		memcpy(&pdesunit->u.u2d.staticeffect, &psrcunit->u.u2d.staticeffect, sizeof(static_effect));
	}
	else
	{
		if (!pdesunit->u.u3d.pmod)
		{
			pdesunit->u.u3d.pmod = new SYModel;
			sy_init_moddef(pdesunit->u.u3d.pmod);
		}
		sy_copy_moddef(pdesunit->u.u3d.pmod, psrcunit->u.u3d.pmod);
	}
	return 1;
}

BASEGRAPH_DATARENDER_API void sy_set_unit_material(SYUnit * p3DUnit, int nSubModelIndex, void * pMaterial)
{
	SYModel* pmod = p3DUnit->u.u3d.pmod;
	if (!pmod)
		return;
	if (nSubModelIndex >= pmod->nviewsub)
		return;
	if (nSubModelIndex < 0)
		return;
	pmod->viewsub[nSubModelIndex].pMaterial = pMaterial;
}

BASEGRAPH_DATARENDER_API void * sy_get_unit_material(SYUnit * p3DUnit, int nSubModelIndex)
{
	SYModel* pmod = p3DUnit->u.u3d.pmod;
	if (!pmod)
		return NULL;
	if (nSubModelIndex >= pmod->nviewsub)
		return NULL;
	if (nSubModelIndex < 0)
		return NULL;
	return pmod->viewsub[nSubModelIndex].pMaterial;
}

BASEGRAPH_DATARENDER_API void sy_release_unit_material(SYUnit * p3DUnit)
{
	SYModel* pMod = p3DUnit->u.u3d.pmod;
	if (pMod != NULL)
	{
		for (int m = 0; m < pMod->nviewsub; ++m)
		{
			SYMaterial_Release((SYMaterial*)pMod->viewsub[m].pMaterial);
			pMod->viewsub[m].pMaterial = NULL;
		}
	}
}

BASEGRAPH_DATARENDER_API void sy_set_unit_material_diffuse(SYUnit * p3DUnit, DWORD dwColor)
{
	SYModel* pMod = p3DUnit->u.u3d.pmod;
	if (pMod != NULL)
	{
		for (int i = 0; i < pMod->nviewsub; ++i)
		{
			SYMaterial* pMaterial = (SYMaterial*)pMod->viewsub[i].pMaterial;
			if (pMaterial != NULL)
			{
				pMaterial->m_PhyData.Diffuse = dwColor;
			}
		}
	}
}

BASEGRAPH_DATARENDER_API SYUnit * sy_create_3dunit_from_2dunit(SYUnit * punit2d, SYUnit * pUnit3D)
{
	if (pUnit3D == NULL)
	{
		pUnit3D = new SYUnit;
	}
	sy_init_unit(pUnit3D, TRUE);
	pUnit3D->MatVToItem = punit2d->MatVToItem;
	pUnit3D->dwUserID = punit2d->dwUserID;
	return pUnit3D;
}

BASEGRAPH_DATARENDER_API SYItemRenderUnit * CreateRenderUnit(BOOL b3dtype)
{
	SYItemRenderUnit* pRenderUnit = new SYItemRenderUnit;
	memset(pRenderUnit, 0, sizeof(SYItemRenderUnit));
	pRenderUnit->m_bChanged = TRUE;
	pRenderUnit->m_bNotRender = FALSE;
	matrix4f_identity(&pRenderUnit->m_VSelfMatrix);
	if (!b3dtype)
		pRenderUnit->graph = NULL;
	else
		matrix4f_identity(&pRenderUnit->unit3D.Mat_RenderV);

	CString strGUID = CommonFunction::CreateGUID();
	memcpy(pRenderUnit->szID, strGUID.GetBuffer(), strGUID.GetLength() * sizeof(TCHAR));
	return pRenderUnit;
}

BASEGRAPH_DATARENDER_API void ReleaseRenderUnit(SYItemRenderUnit * pRenderUnit, BOOL b3dType, int hDevice, void * lpCalc)
{
	CalculatorTitleword * pCalc = (CalculatorTitleword *)lpCalc;
	//djt 暂时不引入renderview
	//pCalc->DeleteRenderUnitView(pRenderUnit->szID);
	if ((pRenderUnit->m_pUnit != NULL && pRenderUnit->m_pUnit->type) || b3dType)
	{
		assert(false);
		//djt 3D先不管
		//Release3DRenderUnit(hDevice, &pRenderUnit->unit3D);
	}
	else
	{
		if (pRenderUnit->graph)
		{
			sy_release_vectorgraph(pRenderUnit->graph);
			delete pRenderUnit->graph;
			pRenderUnit->graph = NULL;
		}
		if (pRenderUnit->unit2D.buffer)
		{
			delete[] pRenderUnit->unit2D.buffer;
			pRenderUnit->unit2D.buffer = NULL;
		}
	}
	sy_release_unit(pRenderUnit->m_pUnit);
	SAFE_DELETE(pRenderUnit->m_pUnit);
}

BASEGRAPH_DATARENDER_API void SYStaticEffect_Init(static_effect * pEffect)
{
	if (pEffect == NULL)
		return;
	memset(pEffect, 0, sizeof(static_effect));
}

BASEGRAPH_DATARENDER_API void SYStaticEffect_Release(static_effect * pEffect)
{
	if (pEffect == NULL)
		return;
	memset(pEffect, 0, sizeof(static_effect));
}

BASEGRAPH_DATARENDER_API void SYStaticEffect_Copy(static_effect * pDstEffect, const static_effect * pSrcEffect)
{
	if (pDstEffect == NULL || pSrcEffect == NULL)
		return;
	SYStaticEffect_Release(pDstEffect);
	memcpy(pDstEffect, pSrcEffect, sizeof(static_effect));
}

BASEGRAPH_DATARENDER_API void SYStaticEffect_SetSolidFaceColor(static_effect * pEffect, COLORREF color)
{
	//目前效果中的颜色是GBRA格式，所以此处先进行转换RGBA
	const unsigned char red = color & 0xff;
	const unsigned char blue = (color >> 16) & 0xff;
	color &= 0xff00ff00;
	color |= (red << 16) | blue;
	pEffect->face.color.type = SYCOLOR_HOMO;
	pEffect->face.color.color = color;
}

BASEGRAPH_DATARENDER_API void SYStaticEffect_SetDefaultSolidColor(static_effect * pEffect, COLORREF color)
{
	//目前效果中的颜色是GBRA格式，所以此处先进行转换RGBA
	const unsigned char red = color & 0xff;
	const unsigned char blue = (color >> 16) & 0xff;
	color &= 0xff00ff00;
	color |= (red << 16) | blue;

	memset(&pEffect->face.color, 0, sizeof(SYColor));
	pEffect->face.color.color = color;
	pEffect->face.art.color = 0xffffffffff;
	pEffect->face.art.metal.highlight = 0xff000000;

	pEffect->border.disable = TRUE;
	pEffect->border.color.color = 0xffffffffff;
	pEffect->border.art.color = 0xff000000;
	pEffect->border.art.metal.highlight = 0xff000000;

	pEffect->shadow.disable = TRUE;
	pEffect->shadow.color.color = 0xffffffffff;
	pEffect->shadow.art.color = 0xff000000;
	pEffect->shadow.art.metal.highlight = 0xff000000;

	pEffect->cube.disable = TRUE;
	pEffect->cube.color.color = 0xffffffffff;
	pEffect->cube.art.color = 0xff000000;
	pEffect->cube.art.metal.highlight = 0xff000000;

	pEffect->bordercube.disable = TRUE;
	pEffect->bordercube.color.color = 0xffffffffff;
	pEffect->bordercube.art.color = 0xff000000;
	pEffect->bordercube.art.metal.highlight = 0xff000000;

	pEffect->bordershadow.disable = TRUE;
	pEffect->bordershadow.color.color = 0xffffffffff;
	pEffect->bordershadow.art.color = 0xff000000;
	pEffect->bordershadow.art.metal.highlight = 0xff000000;

	pEffect->facelight.ambience = 0xffffffffff;
	pEffect->facelight.reflectcolor = 0xffffffffff;

	pEffect->cubelight.ambience = 0xffffffffff;
	pEffect->cubelight.reflectcolor = 0xffffffffff;

	pEffect->dyntex.disable = TRUE;

	pEffect->channel.dwReverse2 = 1;
	pEffect->channel.dwReverse2 = 1;
	pEffect->facelight.dwReverse2 = 1;
	pEffect->cubelight.dwReverse2 = 1;
	pEffect->material.dwReverse2 = 1;
	pEffect->appendmaterial.dwReverse2 = 1;
}

BASEGRAPH_DATARENDER_API void sy_staticeffect_release(static_effect * pEffect)
{
	if (pEffect == NULL)
		return;
	memset(pEffect, 0, sizeof(static_effect));
}

BASEGRAPH_DATARENDER_API BOOL sy_staticeffect_compare(const static_effect * pEffect1, const static_effect * pEffect2)
{
	if (pEffect1 == NULL || pEffect2 == NULL)
		return FALSE;

	//暂时用内存块比较
	if (memcmp(pEffect1, pEffect2, sizeof(static_effect)) == 0)
	{
		return TRUE;
	}
	return FALSE;
}

BASEGRAPH_DATARENDER_API void sy_staticeffect_copy(static_effect * pDstEffect, const static_effect * pSrcEffect)
{
	if (pDstEffect == NULL || pSrcEffect == NULL)
		return;
	sy_staticeffect_release(pDstEffect);
	memcpy(pDstEffect, pSrcEffect, sizeof(static_effect));
}

BASEGRAPH_DATARENDER_API void sy_special_effect_copy(syspecial_effect * pDstEffect, const syspecial_effect * pSrcEffect)
{
	if (pDstEffect == NULL || pSrcEffect == NULL)
		return;

	if (pSrcEffect->m_GraphType == FIRE_GRAPH)
	{
		memcpy(&pDstEffect->m_FireColor, &pSrcEffect->m_FireColor, sizeof(SYColor));
		pDstEffect->m_nFireType = pSrcEffect->m_nFireType;
		pDstEffect->m_nAngle = pSrcEffect->m_nAngle;
		pDstEffect->m_nDegree = pSrcEffect->m_nDegree;
	}
	else
	{
		sy_special_effect_release(pDstEffect);
		pDstEffect->m_nVersion = pSrcEffect->m_nVersion;
		pDstEffect->m_GraphType = pSrcEffect->m_GraphType;
		sy_staticeffect_copy(&pDstEffect->m_BKPublicAttr, &pSrcEffect->m_BKPublicAttr);
		pDstEffect->m_BKRect = pSrcEffect->m_BKRect;
		pDstEffect->m_bUnitary = pSrcEffect->m_bUnitary;
		pDstEffect->m_nAlignMode = pSrcEffect->m_nAlignMode;
	}
}

BASEGRAPH_DATARENDER_API void sy_special_effect_release(syspecial_effect * pEffect)
{
	if (pEffect->m_GraphType == FIRE_GRAPH)
	{
		memset(&pEffect->m_FireColor, 0, sizeof(SYColor));
	}
	else
	{
		sy_staticeffect_release(&pEffect->m_BKPublicAttr);
		memset(&pEffect->m_BKRect, 0, sizeof(rectf));
	}
}

BASEGRAPH_DATARENDER_API int sy_release_vectorgraph(vectorgraph * pgraph)
{
	if (pgraph != NULL)
	{
		for (int i = 0; i < pgraph->ncell; i++)
		{
			sy_release_cell(&pgraph->cell[i]);
		}
		if (pgraph->cell)
		{
			delete[] pgraph->cell;
		}
		pgraph->cell = NULL;
		pgraph->ncell = 0;
	}
	return 1;
}

BASEGRAPH_DATARENDER_API int SYCopy_VectorGraph(vectorgraph * pdesgraph, vectorgraph * psrcgraph)
{
	graph_release(pdesgraph);

	// 拷贝数据
	memcpy(pdesgraph, psrcgraph, sizeof(vectorgraph));
	if (pdesgraph->ncell > 0)
	{
		pdesgraph->cell = new vectorcell[pdesgraph->ncell];
		for (int i = 0; i<pdesgraph->ncell; i++)
		{
			memcpy(&pdesgraph->cell[i], &psrcgraph->cell[i], sizeof(vectorcell));
			if (pdesgraph->cell[i].npoint > 0)
			{
				pdesgraph->cell[i].point = new pointf[pdesgraph->cell[i].npoint];
				memcpy(pdesgraph->cell[i].point, psrcgraph->cell[i].point, sizeof(pointf)*pdesgraph->cell[i].npoint);
				pdesgraph->cell[i].pttype = new unsigned long[pdesgraph->cell[i].npoint];
				memcpy(pdesgraph->cell[i].pttype, psrcgraph->cell[i].pttype, sizeof(unsigned long)*pdesgraph->cell[i].npoint);
			}
		}
	}
	return 1;
}

BASEGRAPH_DATARENDER_API void graph_release(vectorgraph * graph)
{
	if (graph == NULL)
	{
		return;
	}
	for (int i = 0; i<graph->ncell; i++)
	{
		if (graph->cell[i].npoint > 0)
		{
			delete[] graph->cell[i].point;
			delete[] graph->cell[i].pttype;
			graph->cell[i].point = 0;
			graph->cell[i].pttype = 0;
		}
		memset(&graph->cell[i], 0, sizeof(vectorcell));
	}
	if (graph->ncell&&graph->cell)
	{
		delete[] graph->cell;
		graph->cell = 0;
	}
	memset(graph, 0, sizeof(vectorgraph));
}

BASEGRAPH_DATARENDER_API void CopyVectorGraph(vectorgraph * pDesGraph, const vectorgraph * pSrcGraph)
{
	//ASSERT(pDesGraph != NULL);
	//ASSERT(pSrcGraph != NULL);

	// 先释放目标vectorgraph
	graph_release(pDesGraph);

	// 拷贝数据
	memcpy(pDesGraph, pSrcGraph, sizeof(vectorgraph));
	if (pDesGraph->ncell > 0)
	{
		pDesGraph->cell = new vectorcell[pDesGraph->ncell];
		for (int i = 0; i<pDesGraph->ncell; i++)
		{
			memcpy(&pDesGraph->cell[i], &pSrcGraph->cell[i], sizeof(vectorcell));
			if (pDesGraph->cell[i].npoint > 0)
			{
				pDesGraph->cell[i].point = new pointf[pDesGraph->cell[i].npoint];
				memcpy(pDesGraph->cell[i].point, pSrcGraph->cell[i].point, sizeof(pointf)*pDesGraph->cell[i].npoint);
				pDesGraph->cell[i].pttype = new unsigned long[pDesGraph->cell[i].npoint];
				memcpy(pDesGraph->cell[i].pttype, pSrcGraph->cell[i].pttype, sizeof(unsigned long)*pDesGraph->cell[i].npoint);
			}
		}
	}
}

BASEGRAPH_DATARENDER_API void SYVectorGraph_Offset(vectorgraph * pGraph, float nOffsetX, float nOffsetY)
{
	if (pGraph == NULL)
		return;

	int nCell = pGraph->ncell;
	int nPoint = 0;
	vectorcell* pCell = NULL;
	for (int i = 0; i<nCell; i++)
	{
		pCell = &pGraph->cell[i];
		nPoint = pCell->npoint;
		for (int j = 0; j<nPoint; j++)
		{
			pCell->point[j].x += nOffsetX;
			pCell->point[j].y += nOffsetY;
		}
		pCell->bound.left += nOffsetX;
		pCell->bound.right += nOffsetX;
		pCell->bound.top += nOffsetY;
		pCell->bound.bottom += nOffsetY;
	}

	pGraph->bound.left += nOffsetX;
	pGraph->bound.right += nOffsetX;
	pGraph->bound.top += nOffsetY;
	pGraph->bound.bottom += nOffsetY;
}

BASEGRAPH_DATARENDER_API void SYVectorGraph_Release(vectorgraph * pGraph)
{
	if (pGraph == NULL)
		return;
	for (int i = 0; i < pGraph->ncell; ++i)
	{
		SYCurveCell_Release(&pGraph->cell[i]);
	}
	delete[] pGraph->cell;
	memset(pGraph, 0, sizeof(vectorgraph));
}

BASEGRAPH_DATARENDER_API BOOL CreateRegionVectorGraphFromPoints(vectorgraph * pGraph, point2f pt[], int nCount, BOOL bClockWise)
{
	if (pGraph == NULL || nCount < 3)
		return FALSE;

	memset(pGraph, 0, sizeof(vectorgraph));
	pGraph->ncell = 1;
	pGraph->cell = new vectorcell[1];
	pGraph->version = 1;
	pGraph->bound.left = 65535;
	pGraph->bound.right = -65535;
	pGraph->bound.top = 65535;
	pGraph->bound.bottom = -65535;

	vectorcell* pCell = &(pGraph->cell[0]);
	pCell->version = 1;
	if (!bClockWise)
		pCell->type = close_anticlockwise_region;
	else
		pCell->type = close_clockwise_region;
	pCell->width = 1;
	pCell->bound = pGraph->bound;
	pCell->npoint = nCount;
	pCell->point = new point2f[nCount];
	pCell->pttype = new unsigned long[nCount];
	for (int i = 0; i < nCount; i++)
	{
		pCell->point[i] = pt[i];
		pCell->pttype[i] = point_line;
		pCell->bound.left = min(pt[i].x, pCell->bound.left);
		pCell->bound.right = max(pt[i].x, pCell->bound.right);
		pCell->bound.top = min(pt[i].y, pCell->bound.top);
		pCell->bound.bottom = max(pt[i].y, pCell->bound.bottom);
	}

	pGraph->bound = pCell->bound;
	return TRUE;
}

// 把windows的TTPOLYGONHEADER结构转换成vectorgraph结构
#define SYTTPDENOMINATOR ( double(1.0 / 65536.0) )
BASEGRAPH_DATARENDER_API void SYVectorGraph_AssignTTPOLYGONHEADER(vectorgraph * pGraph, TTPOLYGONHEADER * pSrcOutline, int nSize)
{
	if (pGraph == NULL || pSrcOutline == NULL)
		return;

	// 得到点数
	TTPOLYGONHEADER* pTempTT = pSrcOutline;
	int nTempSize = nSize;

	// 计算Cell数目
	int nCellCount = 0;
	while (nTempSize > 0)
	{
		nTempSize -= pTempTT->cb;
		nCellCount++;
		pTempTT = (TTPOLYGONHEADER*)((BYTE*)pTempTT + pTempTT->cb);
	}

	if (nCellCount == 0)
		return;

	pGraph->ncell = nCellCount;
	pGraph->cell = new vectorcell[nCellCount];
	memset(pGraph->cell, 0, sizeof(vectorcell) * nCellCount);

	pGraph->bound.left = 65535.0f;
	pGraph->bound.top = 65535.0f;
	pGraph->bound.right = -65535.0f;
	pGraph->bound.bottom = -65535.0f;

	int nIndex = 0;
	int nCellIndex = 0;
	pTempTT = pSrcOutline;
	nTempSize = nSize;
	while (nTempSize > 0)
	{
		int nPointCount = 1;
		int nCb = pTempTT->cb;
		nCb = nCb - sizeof(TTPOLYGONHEADER);
		TTPOLYCURVE* pSrcCurve = (TTPOLYCURVE*)((BYTE*)pTempTT + sizeof(TTPOLYGONHEADER));
		while (nCb > 0)
		{
			nPointCount += pSrcCurve->cpfx;
			int nCurLength = sizeof(TTPOLYCURVE) + (pSrcCurve->cpfx - 1) * sizeof(POINTFX);
			pSrcCurve = (TTPOLYCURVE*)(((BYTE*)pSrcCurve) + nCurLength);
			nCb -= nCurLength;
		}

		vectorcell* pCelln = &(pGraph->cell[nCellIndex]);
		pCelln->type = close_anticlockwise_region;
		pCelln->width = 0;
		pCelln->npoint = nPointCount;
		pCelln->point = new pointf[nPointCount];
		pCelln->pttype = new unsigned long[nPointCount];

		point2f*& pCellPt = pCelln->point;
		pCellPt[0].x = (float)(*((long*)&pTempTT->pfxStart.x) * SYTTPDENOMINATOR);
		pCellPt[0].y = -(float)(*((long*)&pTempTT->pfxStart.y) * SYTTPDENOMINATOR);
		pCelln->pttype[0] = 0;

		rectf* pCellRt = &pCelln->bound;
		pCellRt->left = pCellPt[0].x;
		pCellRt->right = pCellRt->left;
		pCellRt->top = pCellPt[0].y;
		pCellRt->bottom = pCellRt->top;
		nIndex = 1;
		nCb = pTempTT->cb;
		nCb = nCb - sizeof(TTPOLYGONHEADER);
		pSrcCurve = (TTPOLYCURVE*)((BYTE*)pTempTT + sizeof(TTPOLYGONHEADER));
		while (nCb > 0)
		{
			switch (pSrcCurve->wType)
			{
			case TT_PRIM_LINE:
				for (int i = 0; i<pSrcCurve->cpfx; i++)
				{
					pCellPt[nIndex].x = (float)(*((long*)&pSrcCurve->apfx[i].x) * SYTTPDENOMINATOR);
					pCellPt[nIndex].y = -(float)(*((long*)&pSrcCurve->apfx[i].y) * SYTTPDENOMINATOR);
					pCelln->pttype[nIndex - 1] = point_line;
					pCelln->pttype[nIndex] = point_line;
					pCellRt->left = min(pCellRt->left, pCellPt[nIndex].x);
					pCellRt->right = max(pCellRt->right, pCellPt[nIndex].x);
					pCellRt->top = min(pCellRt->top, pCellPt[nIndex].y);
					pCellRt->bottom = max(pCellRt->bottom, pCellPt[nIndex].y);
					nIndex++;
				}
				break;

			case TT_PRIM_QSPLINE:
				for (int i = 0; i<pSrcCurve->cpfx; i++)
				{
					pCellPt[nIndex].x = (float)(*((long*)&pSrcCurve->apfx[i].x) * SYTTPDENOMINATOR);
					pCellPt[nIndex].y = -(float)(*((long*)&pSrcCurve->apfx[i].y) * SYTTPDENOMINATOR);
					if (i == 0)
						pCelln->pttype[nIndex - 1] = point_conic_start;
					else
						pCelln->pttype[nIndex - 1] = point_conic;
					pCelln->pttype[nIndex] = point_conic;
					pCellRt->left = min(pCellRt->left, pCellPt[nIndex].x);
					pCellRt->right = max(pCellRt->right, pCellPt[nIndex].x);
					pCellRt->top = min(pCellRt->top, pCellPt[nIndex].y);
					pCellRt->bottom = max(pCellRt->bottom, pCellPt[nIndex].y);
					nIndex++;
				}
				break;
			}

			int nCurLength = sizeof(TTPOLYCURVE) + (pSrcCurve->cpfx - 1) * sizeof(POINTFX);
			pSrcCurve = (TTPOLYCURVE*)(((BYTE*)pSrcCurve) + nCurLength);
			nCb -= nCurLength;
		}
		nTempSize -= pTempTT->cb;
		pTempTT = (TTPOLYGONHEADER*)pSrcCurve;

		pGraph->bound.left = min(pGraph->bound.left, pCellRt->left);
		pGraph->bound.top = min(pGraph->bound.top, pCellRt->top);
		pGraph->bound.right = max(pGraph->bound.right, pCellRt->right);
		pGraph->bound.bottom = max(pGraph->bound.bottom, pCellRt->bottom);

		nCellIndex++;
	}
}

BASEGRAPH_DATARENDER_API void MergeVectorGraph(vectorgraph * pDesGraph, const vectorgraph * pSrcGraph1, vectorgraph * pSrcGraph2)
{
	// 先释放目标vectorgraph
	graph_release(pDesGraph);

	int ncellcount = pSrcGraph1->ncell + pSrcGraph2->ncell;
	if (ncellcount < 0)
		return;
	// 拷贝数据
	//memcpy(pDesGraph, pSrcGraph, sizeof(vectorgraph));
	pDesGraph->cell = new vectorcell[ncellcount];
	int i = 0;
	for (; i < pSrcGraph1->ncell; i++)
	{
		CopyCell(&pDesGraph->cell[i], &pSrcGraph1->cell[i]);
	}
	for (; i < ncellcount; i++)
	{
		CopyCell(&pDesGraph->cell[i], &pSrcGraph2->cell[i - pSrcGraph1->ncell]);
	}
	pDesGraph->ncell = ncellcount;
	pDesGraph->bound.left = min(pSrcGraph1->bound.left, pSrcGraph2->bound.left);
	pDesGraph->bound.top = min(pSrcGraph1->bound.top, pSrcGraph2->bound.top);
	pDesGraph->bound.right = max(pSrcGraph1->bound.right, pSrcGraph2->bound.right);
	pDesGraph->bound.bottom = max(pSrcGraph1->bound.bottom, pSrcGraph2->bound.bottom);
}

BASEGRAPH_DATARENDER_API void ReverseVectorGraph(vectorgraph * pDesGraph)
{
	pointf pttemp;
	int ncellcount = pDesGraph->ncell;
	for (int i = 0; i < ncellcount; i++)
	{
		vectorcell* pcell = &pDesGraph->cell[i];
		unsigned long* type = new unsigned long[pcell->npoint];
		int reccount = pcell->npoint / 2;
		for (int i = 0; i< reccount; i++)
		{
			pttemp = pcell->point[i];
			pcell->point[i] = pcell->point[pcell->npoint - 1 - i];
			pcell->point[pcell->npoint - 1 - i] = pttemp;

			type[i] = pcell->pttype[(pcell->npoint + pcell->npoint - 1 - i - 1) % pcell->npoint];
			type[pcell->npoint - i - 1] = pcell->pttype[(pcell->npoint + i - 1) % pcell->npoint];
		}
		for (int i = 0; i < reccount; i++)
		{
			pcell->pttype[i] = type[i];
		}
		delete[] type;
		type = NULL;
	}
}

BASEGRAPH_DATARENDER_API int sy_release_moddef(SYModel * pmod)
{
	for (int i = 0; i < pmod->nviewsub; i++)
		sy_release_viewsub(1, &pmod->viewsub[i]);

	SAFEDELETE_ARRAY(pmod->v);
	SAFEDELETE_ARRAY(pmod->indecs);
	SAFEDELETE_ARRAY(pmod->viewsub);
	SAFEDELETE_ARRAY(pmod->pInstance);

	pmod->icount = 0;
	pmod->vcount = 0;
	pmod->nviewsub = 0;
	pmod->nInstanceCount = 0;

	int bcount = (int)SYAR_GetSize(&pmod->bonearray);
	if (bcount > 0)
	{
		if (pmod->pdwBoneIndex)
		{
			delete[]  pmod->pdwBoneIndex;
			pmod->pdwBoneIndex = NULL;
		}
		if (pmod->pdwBoneWeight)
		{
			delete[] pmod->pdwBoneWeight;
			pmod->pdwBoneWeight = NULL;
		}
		SYBoneDef* pBone = NULL;
		for (int i = 0; i < bcount; i++)
		{
			pBone = (SYBoneDef*)SYAR_RemoveAt(&pmod->bonearray, 0, 1);
			sy_release_bonedef(pBone);
			SAFE_DELETE(pBone);
		}
		SYAR_RemoveAll(&pmod->bonearray);
	}

	return 1;
}

BASEGRAPH_DATARENDER_API int sy_release_viewsub(int hDevice, SYSubModel * pSubModel)
{
	if (pSubModel->pMaterial)
	{
		SYMaterial_Release((SYMaterial*)pSubModel->pMaterial);
		delete (SYMaterial*)pSubModel->pMaterial;
		pSubModel->pMaterial = NULL;
	}
	return 1;
}

BASEGRAPH_DATARENDER_API void sy_copy_mod_viewsub(SYModel * pDst, const SYModel * pSrc)
{
	if (pDst->nviewsub != pSrc->nviewsub)
	{
		for (int i = 0; i < pDst->nviewsub; i++)
			sy_release_viewsub(1, &pDst->viewsub[i]);
		SAFEDELETE_ARRAY(pDst->viewsub);
		pDst->nviewsub = pSrc->nviewsub;
		if (pDst->nviewsub > 0)
		{
			pDst->viewsub = new SYSubModel[pDst->nviewsub];
			memset(pDst->viewsub, 0, pDst->nviewsub * sizeof(SYSubModel));
		}
	}

	for (int i = 0; i < pDst->nviewsub; i++)
	{
		SYSubModel* pDstSubModel = &pDst->viewsub[i];
		SYSubModel* pSrcSubModel = &pSrc->viewsub[i];

		pDstSubModel->icount = pSrcSubModel->icount;
		pDstSubModel->vstart = pSrcSubModel->vstart;
		pDstSubModel->vcount = pSrcSubModel->vcount;
		pDstSubModel->istart = pSrcSubModel->istart;
		pDstSubModel->nTri_Type = pSrcSubModel->nTri_Type;
		pDstSubModel->nTri_Num = pSrcSubModel->nTri_Num;
		if (!pDstSubModel->pMaterial)
		{
			pDstSubModel->pMaterial = new SYMaterial;
			SYMaterial_Init((SYMaterial*)pDstSubModel->pMaterial);
		}
		SYMaterial_Copy((SYMaterial*)pDstSubModel->pMaterial, (SYMaterial*)pSrcSubModel->pMaterial);
	}
}

BASEGRAPH_DATARENDER_API void sy_init_moddef(SYModel * pmod)
{
	memset(pmod, 0, sizeof(SYModel));
}

BASEGRAPH_DATARENDER_API void sy_copy_moddef(SYModel * pDst, const SYModel * pSrc)
{
	pDst->logicvcount = pSrc->logicvcount;
	pDst->logiclcount = pSrc->logiclcount;
	pDst->logicfcount = pSrc->logicfcount;
	pDst->logicccount = pSrc->logicccount;
	if (pDst->vcount != pSrc->vcount)		// vertex
	{
		SAFEDELETE_ARRAY(pDst->v);
		SAFEDELETE_ARRAY(pDst->pdwBoneWeight);
		SAFEDELETE_ARRAY(pDst->pdwBoneIndex);

		pDst->vcount = pSrc->vcount;
		if (pDst->vcount > 0)
		{
			pDst->v = new SYVertex3D[pDst->vcount];
			if (pSrc->pdwBoneWeight)
				pDst->pdwBoneWeight = new DWORD[pDst->vcount];	// weight indexes for each vertexes, the array size is "vcount"
			if (pSrc->pdwBoneIndex)
				pDst->pdwBoneIndex = new DWORD[pDst->vcount];
		}
	}
	memcpy(pDst->v, pSrc->v, pDst->vcount * sizeof(SYVertex3D));
	if (pSrc->pdwBoneWeight)
		memcpy(pDst->pdwBoneWeight, pSrc->pdwBoneWeight, pSrc->vcount * sizeof(DWORD));
	if (pSrc->pdwBoneIndex)//当boneindexcount==0时，有地址
		memcpy(pDst->pdwBoneIndex, pSrc->pdwBoneIndex, pSrc->vcount * sizeof(DWORD));

	if (pDst->icount != pSrc->icount)		// triangle surfaces indexes
	{
		SAFEDELETE_ARRAY(pDst->indecs);
		pDst->icount = pSrc->icount;		// triangle surfaces indexes
		if (pDst->icount > 0)
			pDst->indecs = new DWORD[pSrc->icount];
	}
	memcpy(pDst->indecs, pSrc->indecs, pSrc->icount * sizeof(DWORD));

	if (pDst->nInstanceCount != pSrc->nInstanceCount)		// triangle surfaces indexes
	{
		SAFEDELETE_ARRAY(pDst->pInstance);
		pDst->nInstanceCount = pSrc->nInstanceCount;		// triangle surfaces indexes
															//if(pDst->nInstanceCount > 1)
		pDst->pInstance = new SYModelInstance[pDst->nInstanceCount];
	}
	memcpy(pDst->pInstance, pSrc->pInstance, pDst->nInstanceCount * sizeof(SYModelInstance));

	if (pDst->nSeqCount != pSrc->nSeqCount)		// global sequences
	{
		pDst->nSeqCount = pSrc->nSeqCount;		// global sequences
		if (pDst->nSeqCount > 0)
			pDst->pnSeq = new int[pDst->nSeqCount];
	}
	memcpy(pDst->pnSeq, pSrc->pnSeq, pSrc->nSeqCount * sizeof(int));
	//Copy View Sub
	sy_copy_mod_viewsub(pDst, pSrc);

	int nDstBoneSize = (int)SYAR_GetSize(&pDst->bonearray);
	for (int i = 0; i < nDstBoneSize; i++)
	{
		SYBoneDef* pDstBone = (SYBoneDef*)SYAR_GetAt(&pDst->bonearray, i);
		sy_release_bonedef(pDstBone);
		SAFE_DELETE(pDstBone);
	}
	SYAR_Clear(&pDst->bonearray);

	int nBoneSize = (int)SYAR_GetSize(&pSrc->bonearray);
	for (int i = 0; i < nBoneSize; i++)
	{
		//bone array
		SYBoneDef* pScrBone = (SYBoneDef*)SYAR_GetAt(&pSrc->bonearray, i);
		SYBoneDef* pBone = new SYBoneDef;
		sy_copy_bone(pBone, pScrBone);
		SYAR_Add(&pDst->bonearray, pBone);
	}

	pDst->nBoneGroupCount = pSrc->nBoneGroupCount;	// bone group definition
	if (pDst->nBoneGroupCount > 0)
	{
		SAFEDELETE_ARRAY(pDst->pdwBoneGroup);
		pDst->pdwBoneGroup = new DWORD[pDst->nBoneGroupCount];		// 4 bytes of one cell, each byte indicates a bone index
		memcpy(pDst->pdwBoneGroup, pSrc->pdwBoneGroup, pDst->nBoneGroupCount * sizeof(DWORD));
	}

	if (pDst->nBoneIndexCount > 0)	// bone indexes definition
	{
		SAFEDELETE_ARRAY(pDst->pwBoneIndex);
		pDst->pwBoneIndex = new WORD[pDst->nBoneIndexCount];		// 4 bytes of one cell, each byte indicates a bone index
		memcpy(pDst->pwBoneIndex, pSrc->pwBoneIndex, pDst->nBoneIndexCount * sizeof(WORD));
	}
	//pDst->pboneoffVMatrix = 0;//some problem
	//pDst->pboneoffNMatrix = 0;//some problem
	//逻辑顶点
	if (pDst->logicvcount > 0)
	{
		SAFEDELETE_ARRAY(pDst->logicvertex);
		pDst->logicvertex = new sylogic_vertex[pDst->logicvcount];
		memcpy(pDst->logicvertex, pSrc->logicvertex, pDst->logicvcount * sizeof(sylogic_vertex));
	}
	//逻辑线
	if (pDst->logiclcount > 0)
	{
		SAFEDELETE_ARRAY(pDst->logicline);
		pDst->logicline = new sylogic_line[pDst->logiclcount];
		memcpy(pDst->logicline, pSrc->logicline, pDst->logiclcount * sizeof(sylogic_line));
	}
	//逻辑面
	if (pDst->logicfcount > 0)
	{
		SAFEDELETE_ARRAY(pDst->logicface);
		pDst->logicface = new sylogic_face[pDst->logicfcount];
		memcpy(pDst->logicface, pSrc->logicface, pDst->logicfcount * sizeof(sylogic_face));
	}

	//逻辑体
	if (pDst->logicccount > 0)
	{
		SAFEDELETE_ARRAY(pDst->logiccube);
		pDst->logiccube = new sylogic_cube[pDst->logicccount];
		memcpy(&pDst->logiccube, &pSrc->logiccube, pDst->logicccount * sizeof(sylogic_cube));
	}
}

BASEGRAPH_DATARENDER_API int sy_release_cell(vectorcell * pcell)
{
	if (pcell&&pcell->npoint)
	{
		delete[] pcell->point;
		delete[] pcell->pttype;
		pcell->point = NULL;
		pcell->pttype = NULL;
	}
	return 1;
}

BASEGRAPH_DATARENDER_API int sy_release_bonedef(SYBoneDef * pbone)
{
	if (!pbone)
		return 0;
	return 1;
}

BASEGRAPH_DATARENDER_API void sy_copy_bone(SYBoneDef * pDesBone, SYBoneDef * pSrcBone)
{
	memset(pDesBone, 0, sizeof(SYBoneDef));
	pDesBone->nAnimID = pSrcBone->nAnimID;
	pDesBone->geoid = pSrcBone->geoid;
	pDesBone->flags = pSrcBone->flags;
	pDesBone->rotationOrder = pSrcBone->rotationOrder;
	pDesBone->linkMode = pSrcBone->linkMode;
	pDesBone->version = pSrcBone->version;
	pDesBone->parent = pSrcBone->parent;
	pDesBone->pivot = pSrcBone->pivot;
	pDesBone->pboneoffVMatrix = pSrcBone->pboneoffVMatrix;
	_tcscpy_s(pDesBone->name, pSrcBone->name);
}

BASEGRAPH_DATARENDER_API void sy_lightinfo_copy(BaseGraph_DataBase::lightinfo * pDes, const BaseGraph_DataBase::lightinfo * pSrc)
{
	pDes->ambience = pSrc->ambience;
	pDes->ambience_degree = pSrc->ambience_degree;
	pDes->reflectcolor = pSrc->reflectcolor;
	pDes->dwReverse1 = pSrc->dwReverse1;
	pDes->dwReverse2 = pSrc->dwReverse2;
	_tcscpy_s(pDes->szPath, pSrc->szPath);
}

BASEGRAPH_DATARENDER_API void sy_dynamictexture_copy(BaseGraph_DataBase::dynamictexture * pDes, const BaseGraph_DataBase::dynamictexture * pSrc)
{
	pDes->disable = pSrc->disable;
	_tcscpy_s(pDes->szPath, pSrc->szPath);
	pDes->speed = pSrc->speed;
	pDes->offsetx = pSrc->offsetx;
	pDes->offsety = pSrc->offsety;
	pDes->vRunDirx = pSrc->vRunDirx;
	pDes->vRunDiry = pSrc->vRunDiry;
	pDes->scalex = pSrc->scalex;
	pDes->scaley = pSrc->scaley;
	pDes->centerx = pSrc->centerx;
	pDes->centery = pSrc->centery;
	pDes->rotate = pSrc->rotate;
	pDes->alpha = pSrc->alpha;
	pDes->bRepeat = pSrc->bRepeat;
	pDes->bUseAlpha = pSrc->bUseAlpha;
}

BASEGRAPH_DATARENDER_API void sy_channelinfo_copy(BaseGraph_DataBase::channelinfo * pDes, const BaseGraph_DataBase::channelinfo * pSrc)
{
	_tcscpy_s(pDes->szPath, pSrc->szPath);
	pDes->weight = pSrc->weight;
	pDes->channelid = pSrc->channelid;
	pDes->dwReverse1 = pSrc->dwReverse1;
	pDes->dwReverse2 = pSrc->dwReverse2;
}

BASEGRAPH_DATARENDER_API void sy_materialinfo_copy(BaseGraph_DataBase::materialinfo * pDes, const BaseGraph_DataBase::materialinfo * pSrc)
{
	_tcscpy_s(pDes->szPath, pSrc->szPath);
	pDes->dwReverse1 = pSrc->dwReverse1;
	pDes->dwReverse2 = pSrc->dwReverse2;
}

BASEGRAPH_DATARENDER_API void SYRender_GetGraph_RenderInfo(vectorgraph * pGraph, float fscale, int & w, int & h, RECT & rtBound)
{
	rectf rtff;
	rtff = pGraph->bound;
	SYRectf_Scale(&rtff, fscale);
	rip_extend_fbound(&rtff, (int*)&rtBound.left,
		(int*)&rtBound.top,
		(int*)&rtBound.right,
		(int*)&rtBound.bottom);

	//宽高
	w = rtBound.right - rtBound.left;
	h = abs(rtBound.bottom - rtBound.top);
}

BOOL SYRender_LoadChannel(static_effect* peffect, sy_render_channel** pDesChannel)
{
	TCHAR szRealPath[MAX_PATH];
	//if (_tcslen(peffect->facelight.szPath))
	//{
	//	sy_render_channel* pChannel = new sy_render_channel;
	//	GetFileManager()->TagPathTrans(peffect->facelight.szPath, szRealPath, GetFileManager()->lpParam);
	//	BOOL bResult = FALSE;
	//	if (peffect->facelight.dwReverse2 != 1)
	//		bResult = _SYRender_ReadChannel_Mask(szRealPath, pChannel);
	//	if (!bResult)
	//	{
	//		delete pChannel;
	//	}
	//	else
	//	{
	//		pChannel->type = CHANNEL_FACELIGHT;
	//		pChannel->ambience = peffect->facelight.ambience;
	//		pChannel->reflectcolor = peffect->facelight.reflectcolor;
	//		pChannel->ambience_degree = peffect->facelight.ambience_degree;
	//		pDesChannel[CHANNEL_FACELIGHT] = pChannel;
	//	}
	//}
	//if (_tcslen(peffect->cubelight.szPath))
	//{
	//	sy_render_channel* pChannel = new sy_render_channel;
	//	GetFileManager()->TagPathTrans(peffect->cubelight.szPath, szRealPath, GetFileManager()->lpParam);
	//	BOOL bResult = FALSE;

	//	if (peffect->cubelight.dwReverse2 != 1)
	//		bResult = _SYRender_ReadChannel_Mask(szRealPath, pChannel);
	//	if (!bResult)
	//	{
	//		delete pChannel;
	//	}
	//	else
	//	{
	//		pChannel->type = CHANNEL_CUBELIGHT;
	//		pChannel->ambience = peffect->cubelight.ambience;
	//		pChannel->reflectcolor = peffect->cubelight.reflectcolor;
	//		pChannel->ambience_degree = peffect->cubelight.ambience_degree;
	//		pDesChannel[CHANNEL_CUBELIGHT] = pChannel;
	//	}

	//}
	//if (_tcslen(peffect->material.szPath))
	//{
	//	sy_render_channel* pChannel = new sy_render_channel;
	//	GetFileManager()->TagPathTrans(peffect->material.szPath, szRealPath, GetFileManager()->lpParam);
	//	BOOL bResult = FALSE;

	//	if (peffect->material.dwReverse2 != 1)
	//		bResult = _SYRender_ReadChannel_Mask(szRealPath, pChannel);
	//	if (!bResult)
	//	{
	//		delete pChannel;
	//	}
	//	else
	//	{
	//		pChannel->type = CHANNEL_MATERIAL;
	//		pDesChannel[CHANNEL_MATERIAL] = pChannel;
	//	}

	//}
	//if (_tcslen(peffect->channel.szPath))
	//{
	//	sy_render_channel* pChannel = new sy_render_channel;
	//	GetFileManager()->TagPathTrans(peffect->channel.szPath, szRealPath, GetFileManager()->lpParam);

	//	BOOL bResult = FALSE;
	//	//set dwReverse2 to disable mask channel
	//	if (peffect->channel.dwReverse2 != 1)
	//		bResult = _SYRender_ReadChannel_Mask(szRealPath, pChannel, &peffect->channel);
	//	if (!bResult)
	//	{
	//		delete pChannel;
	//	}
	//	else
	//	{
	//		pChannel->type = CHANNEL_ALPHA;
	//		pDesChannel[CHANNEL_ALPHA] = pChannel;
	//	}
	//}
	//if (_tcslen(peffect->appendmaterial.szPath))
	//{
	//	sy_render_channel* pChannel = new sy_render_channel;
	//	GetFileManager()->TagPathTrans(peffect->appendmaterial.szPath, szRealPath, GetFileManager()->lpParam);

	//	BOOL bResult = FALSE;
	//	if (peffect->appendmaterial.dwReverse2 != 1)
	//		bResult = _SYRender_ReadChannel_Mask(szRealPath, pChannel);
	//	if (!bResult)
	//	{
	//		delete pChannel;
	//	}
	//	else
	//	{
	//		pChannel->type = CHANNEL_APPENDMATERIAL;
	//		pDesChannel[CHANNEL_APPENDMATERIAL] = pChannel;
	//	}
	//}
	return TRUE;
}

void makemask_invert(unsigned char* pdesmask, unsigned char* psrcmask, int npitch, int nheight)
{
	unsigned long * tempdes = (unsigned long*)pdesmask;
	unsigned long * tempsrc = (unsigned long*)psrcmask;
	int nwidth = npitch >> 2;
	for (int i = 0; i <nheight; i++)
	{
		for (int j = 0; j < nwidth; j++)
			tempdes[j] = ~tempsrc[j];
		tempdes += nwidth;
		tempsrc += nwidth;
	}
}

void makemask_sub(unsigned char* pdesmask,
	int npitch,
	int nheight,
	unsigned char* psrcmask,
	RECT bound,
	int srcoffsetx,
	int srcoffsety,
	int type)
{
	//	if(srcoffsetx > 0 && srcoffsety > 0)
	{
		int srcx = bound.right - bound.left;
		int srcy = bound.bottom - bound.top;
		//		unsigned char * tempdes = (pdesmask + srcoffsetx + npitch * srcoffsety);
		//		unsigned char * temporgsrc = (psrcorgmask + srcoffsetx + npitch * srcoffsety);
		unsigned char * tempsrc = psrcmask - bound.left - bound.top * srcx;
		unsigned char * tempdes = pdesmask;
		unsigned char * tempclip = pdesmask;

		int xoffset = srcoffsetx / 2;
		int yoffset = srcoffsety / 2;
		int noffset = (yoffset * srcx + xoffset);
		int  factor = 1;


		BYTE bs = 0, be = 0;
		for (int i = 0; i < nheight; i++)
		{
			for (int k = 0; k < npitch; k++)
			{
				if (!tempclip[k])
					continue;

				if (i - yoffset  < bound.top || i - yoffset >= bound.bottom
					|| k - xoffset   < bound.left || k - xoffset   > bound.right)
					bs = 0;
				else
					bs = tempsrc[k - noffset];

				if (i + yoffset >= bound.bottom || i + yoffset < bound.top
					|| k + xoffset >= bound.right || k + xoffset < bound.left)
					be = 0;
				else
					be = tempsrc[k + noffset];

				int dif = be - bs;
				int ndif = abs(dif);
				factor = 1;
				if (type != BRIGHT_EDGE && dif < 0) factor = -1;
				tempdes[k] = 127 + factor * (((ndif >> 1) * tempclip[k] + 0xff) >> 8);
			}
			tempdes += npitch;
			tempclip += npitch;
			tempsrc += srcx;
		}
	}
}

void makemask_and(unsigned char* pdesmask, unsigned char* psrcorgmask, unsigned char* psrcmask, int srcoffsetx, int srcoffsety, int npitch, int nheight)
{
	if (srcoffsetx > 0 && srcoffsety > 0)
	{
		unsigned char * tempdes = (pdesmask + srcoffsetx + npitch * srcoffsety);
		unsigned char * temporgsrc = (psrcorgmask + srcoffsetx + npitch * srcoffsety);
		unsigned char * tempsrc = psrcmask;

		int nbytewidth = npitch - srcoffsetx;
		int nbyteheight = nheight - srcoffsety;
		for (int i = 0; i <nbyteheight; i++)
		{
			for (int j = 0; j < nbytewidth; j++)
				tempdes[j] = (temporgsrc[j] & tempsrc[j]);
			tempdes += npitch;
			temporgsrc += npitch;
			tempsrc += npitch;
		}
	}
	else
	{
		unsigned char * tempdes = (unsigned char *)pdesmask;
		unsigned char * temporgsrc = (unsigned char *)psrcorgmask;
		unsigned char * tempsrc = (unsigned char *)(psrcmask - srcoffsetx - npitch * srcoffsety);
		int nbytewidth = npitch + srcoffsetx;
		int nbyteheight = nheight + srcoffsety;
		for (int i = 0; i <nbyteheight; i++)
		{
			for (int j = 0; j < nbytewidth; j++)
				tempdes[j] = (temporgsrc[j] & tempsrc[j]);
			tempdes += npitch;
			temporgsrc += npitch;
			tempsrc += npitch;
		}
	}
}
void makemask_up128(unsigned char* pdesmask, int npitch, int nheight)
{
	for (int i = 0; i < nheight; i++)
	{
		for (int j = 0; j < npitch; j++)
		{
			if (pdesmask[j])
				pdesmask[j] = (pdesmask[j] >> 1) + 128;
		}
		pdesmask += npitch;
	}

}
void makemask_down128(unsigned char* pdesmask, int npitch, int nheight)
{
	for (int i = 0; i < nheight; i++)
	{
		for (int j = 0; j < npitch; j++)
		{
			pdesmask[j] = (pdesmask[j] >> 1);
		}
		pdesmask += npitch;
	}
}

void makemask_or(unsigned char* pdesmask, unsigned char* psrcorgmask, unsigned char* psrcmask, int npitch, int nheight)
{
	unsigned char * tempdes = pdesmask;
	unsigned char * temporgsrc = psrcorgmask;
	unsigned char * tempsrc = psrcmask;

	int nbytewidth = npitch;
	int nbyteheight = nheight;
	for (int i = 0; i <nbyteheight; i++)
	{
		for (int j = 0; j < nbytewidth; j++)
			tempdes[j] = (temporgsrc[j] | tempsrc[j]);
		tempdes += npitch;
		temporgsrc += npitch;
		tempsrc += npitch;
	}
}

void makemask_convex(unsigned long* buffer, int nwidth, int nheight, unsigned char* pmaskhighlight, int npitch, int ntype, unsigned long color)
{
	unsigned long* pdesbuf = buffer;
	unsigned char* phmask = pmaskhighlight;
	unsigned long dwColor;
	long lrate;
	int lrateR, lrateG, lrateB;
	int R, G, B;
	int cvR = GetRGBA_R(color);
	int cvG = GetRGBA_G(color);
	int cvB = GetRGBA_B(color);
	for (int i = 0; i < nheight; i++)
	{
		for (int j = 0; j < nwidth; j++)
		{
			//if(phmask[j]==0 && plmask[j] == 0)
			//	continue;
			dwColor = buffer[j];
			if ((dwColor & 0xff000000) == 0)
				continue;
			R = GetRGBA_R(dwColor);
			G = GetRGBA_G(dwColor);
			B = GetRGBA_B(dwColor);

			//if(plmask[j])
			{
				lrate = (phmask[j] - 127) * 4 / 3;
				lrateR = (lrate * cvR + 0xff) >> 8;
				lrateG = (lrate * cvG + 0xff) >> 8;
				lrateB = (lrate * cvB + 0xff) >> 8;
				if (ntype == STATCEFFECT_TYPE_CONCAVE)
				{
					R -= lrateR;
					G -= lrateG;
					B -= lrateB;
				}
				else if (ntype == STATCEFFECT_TYPE_CONVEX)
				{
					R += lrateR;
					G += lrateG;
					B += lrateB;
				}
			}
			//if(phmask[j])
			//{
			//	hrate = (phmask[j]>>2) * 3;//pdesmask[j] * 2;
			//	//if(rate > 40)
			//	//	rate = (255 >>2) *3;
			//	//else
			//	//	rate = rate * (((255 >>2) *3)/40);
			//	if(ntype == STATCEFFECT_TYPE_CONVEX )
			//	{
			//		R += hrate;
			//		G += hrate;
			//		B += hrate;
			//	}
			//	else if(ntype == STATCEFFECT_TYPE_CONCAVE)
			//	{
			//		R -= hrate;
			//		G -= hrate;
			//		B -= hrate;
			//	}
			//	
			//}
			if (ntype == STATCEFFECT_TYPE_LIGHT)
			{
				R += abs((int)lrateR);
				G += abs((int)lrateG);
				B += abs((int)lrateB);
			}
			if (R < 0)
				R = 0;
			if (G < 0)
				G = 0;
			if (B < 0)
				B = 0;

			if (R > 255)
				R = 255;
			if (G > 255)
				G = 255;
			if (B > 255)
				B = 255;
			buffer[j] = RGBA(R, G, B, GetRGBA_A(dwColor));
		}
		phmask += npitch;
		buffer += nwidth;


	}
}

void _ausdilationcircletable(int* lptable, int nwidth)
{
	memset(lptable, 0, sizeof(int) *nwidth * nwidth);
	float fRad = nwidth / 2.0 - 0.5;
	int ndRad = SY_FToI(fRad * fRad);
	int nindex = 0;
	for (int i = 0; i < nwidth; i++)
	{
		for (int k = 0; k< nwidth; k++)
		{
			if ((k - fRad) * (k - fRad) + (i - fRad) * (i - fRad) <= ndRad)
			{
				lptable[nindex] = 1;
			}
			nindex++;
		}
	}
}
unsigned int _auxmaxmap(LPBYTE lpDes, LPSIZE pSize, int x, int y, int* lptable, int degrees)
{
	// 离边缘太远的地方不计算，减少工作量
	BYTE	dwColor = *(lpDes + (pSize->cx * y + x));
	if (dwColor != 0)
	{
		return dwColor;
	}
	int nmax = 0;
	int nrad = degrees / 2;
	int nxminoffset = -nrad;
	if (x + nxminoffset < 0)
		nxminoffset = -x;
	int nxmaxoffset = nrad;
	if (x + nxmaxoffset >= pSize->cx)
	{
		nxmaxoffset = pSize->cx - x;
		return dwColor;
	}
	if (nxminoffset >= nxmaxoffset)
	{
		return dwColor;
	}

	int nyminoffset = -nrad;
	if (y + nyminoffset < 0)
		nyminoffset = -y;
	int nymaxoffset = nrad;
	if (y + nymaxoffset >= pSize->cy)
	{
		nymaxoffset = pSize->cy - y;
		return dwColor;
	}
	if (nyminoffset >= nymaxoffset)
	{
		return dwColor;
	}

	unsigned int nmaxvalue = 0;
	int* pTableline = lptable + (nyminoffset + nrad) * degrees;
	LPBYTE pDesline = lpDes + (y + nyminoffset) * pSize->cx;
	for (int i = nyminoffset; i < nymaxoffset; i++)
	{
		for (int k = nxminoffset; k < nxmaxoffset; k++)
		{
			nmaxvalue = max(nmaxvalue, pDesline[x + k] * pTableline[k + nrad]);
		}
		pTableline += degrees;
		pDesline += pSize->cx;
	}
	return nmaxvalue;
}
void auxDilation(LPBYTE lpDes, LPSIZE pSize, float fdegrees)
{
	int degrees = SY_FToI(fdegrees);
	if (degrees < 1)
		degrees = 1;
	degrees = degrees * 2;

	BYTE* lpD = new BYTE[pSize->cx * pSize->cy];
	memset(lpD, 0, pSize->cx * pSize->cy);
	int* lptable = new int[(degrees + 1) * (degrees + 1)];
	_ausdilationcircletable(lptable, degrees + 1);
	BYTE* lptempdes = lpD;
	for (int i = 0; i < pSize->cy; i++)
	{
		for (int k = 0; k < pSize->cx; k++)
		{
			lptempdes[k] = _auxmaxmap(lpDes, pSize, k, i, lptable, degrees + 1);
		}
		lptempdes += pSize->cx;
	}
	memcpy(lpDes, lpD, pSize->cx * pSize->cy);
	delete[] lpD;
	delete[] lptable;
	lpD = NULL;
	lptable = NULL;
}
int auxCreateFilterMap(int* pGuass, int degrees)
{
	//	1 - 32 ==> 4 - 256
	{
		degrees = 14 * degrees + 48;
		degrees *= degrees;
		degrees /= 31 * 31;
	}
	//	高斯分布函数表达式
	//	
	//	f(d) = (1/sqrt(2*PI*t)) * exp( -d2 / 2*t )
	//	f( d1 + d2 ) = (sqrt(2*PI*t))( f(d1) + f(d2) )

	double dGuass[101];
	//	 计算高斯分布值
	{
		double dt = degrees / 2.0;		//	t
		double dm = 1 / sqrt(2 * PAIF*dt);	//	1/sqrt(2*PI*t)
		for (int d = 0; d <= 100; d++)
		{
			dGuass[d] = dm * exp(-d*d / (2 * dt));
		}
	}
	//	 对高斯分布值求和
	double dTotal = 0;
	{
		for (int x = 1; x <= 100; x++)
			for (int y = 0; y <= 100; y++)
			{
				dTotal += dGuass[x] * dGuass[y];
			}
		dTotal = 1 / (dTotal * 4 + dGuass[0] * dGuass[0]);	//	(x+1)*(x+1) = 4*(x+1)*x+1
	}
	int i = 0;
	for (i = 0; i <= 100; i++)
	{
		pGuass[i] = (int)(dGuass[i] * 0x100 / dTotal);
		if (pGuass[i] == 0)break;
	}
	return i;
}
void auxExternFilterMap(LPBYTE lpDes, LPSIZE pSize, int* pGuass, int nSign)
{
	if (nSign > 2 * pSize->cx)nSign = pSize->cx / 2;
	if (nSign == 0)return;

	int nDataLength = pSize->cx*pSize->cy;
	int* pTemp = new int[nDataLength * 2];
	memset(pTemp, 0, nDataLength * 2 * sizeof(int));

	//	水平方向BLUR
	{
		LPBYTE	pSrc = lpDes;
		LPINT	pDes = pTemp + nDataLength;
		int		nExt = nSign - 1;
		int	x, y, xextl = nExt, xextr = pSize->cx - nExt;
		for (y = 0; y < pSize->cy; y++, pSrc += pSize->cx, pDes += pSize->cx)
		{
			for (x = 0; x < pSize->cx; x++)
			{
				register int nSrc = pSrc[x];
				if (nSrc)
				{
					int leftext = x < xextl ? x : nExt;
					int rightext = x > xextr ? (pSize->cx - x) : nExt;
					for (int i = -leftext; i < 0; i++)
					{
						pDes[x + i] += pGuass[-i] * nSrc;
					}
					for (int i = 0; i < rightext; i++)
					{
						pDes[x + i] += pGuass[i] * nSrc;
					}
				}
			}
		}
	}

	//	垂直方向BLUR
	{
		LPINT	pSrc = pTemp + nDataLength;
		LPINT	pDes = pTemp;
		int		nExt = nSign - 1;
		int	x, y, yextt = nExt, yextb = pSize->cy - nExt;
		for (x = 0; x < pSize->cx; x++, pSrc++, pDes++)
		{
			for (y = 0; y < pSize->cy; y++)
			{
				register int nSrc = pSrc[y*pSize->cx];
				if (nSrc)
				{
					int topext = y < yextt ? y : nExt;
					int bottomext = y > yextb ? (pSize->cy - y) : nExt;
					LPINT pDesTemp = pDes + (y - topext)*pSize->cx;
					for (int i = -topext; i < 0; i++, pDesTemp += pSize->cx)
					{
						*pDesTemp += pGuass[-i] * nSrc;
					}
					for (int i = 0; i < bottomext; i++, pDesTemp += pSize->cx)
					{
						*pDesTemp += pGuass[i] * nSrc;
					}
				}
			}
		}
	}
	{
		register int nLength = nDataLength;
		while (nLength--)
		{
			if (pTemp[nLength] > 0xFF0000)lpDes[nLength] = 0xFF;
			else lpDes[nLength] = (BYTE)(pTemp[nLength] >> 16);
		}
	}
	delete pTemp;
}
void auxGuassianBlur(LPBYTE lpDes, LPSIZE pSize, int degrees)
{
	//	ASSERT( lpDes != NULL && degrees > 0 && degrees <= 100 );

	int nSide = degrees * 2 + 1;
	int nGuass[101];

	//	 由于计算中会有亮度损失,要恢复亮度	
	int nTotalPre = 0, nTotalFex = 0;
	int nLength = pSize->cx*pSize->cy;
	while (nLength--)nTotalPre += lpDes[nLength];

	int  nSign = auxCreateFilterMap(nGuass, degrees);

	auxExternFilterMap(lpDes, pSize, nGuass, nSign);

	nLength = pSize->cx*pSize->cy;
	while (nLength--)nTotalFex += lpDes[nLength];

	BYTE	LmulTable[256];
	double dScales = nTotalPre / (double)nTotalFex;
	for (int i = 0; i < 256; i++)
	{
		if (nTotalFex) {
			int nLastL = (int)(i * dScales);
			LmulTable[i] = nLastL > 255 ? 255 : (BYTE)nLastL;
		}
		else {
			LmulTable[i] = i;
		}
	}
	nLength = pSize->cx*pSize->cy;
	while (nLength--)lpDes[nLength] = LmulTable[lpDes[nLength]];
}
void effect_convex(unsigned char* pmask,
	int nmaskpitch,
	unsigned long* pbuffer,
	int nwidth,
	int nheight,
	int nborder,
	int nsoft,
	int ntype,
	float fangle,
	unsigned long color)
{

	int desx = nmaskpitch + nsoft * 2;
	int desy = nheight + nsoft * 2;
	RECT bound = { -nsoft,-nsoft,nmaskpitch + nsoft,nheight + nsoft };
	//mask
	unsigned char* pdesmask = new unsigned char[nmaskpitch * nheight];
	unsigned char* popermask = new unsigned char[desx * desy];
	memcpy(pdesmask, pmask, sizeof(unsigned char) * nmaskpitch * nheight);
	memset(popermask, 0, sizeof(unsigned char) * desx * desy);

	unsigned char* ptempsrc = pmask;
	unsigned char* ptempdes = popermask + nsoft * desx + nsoft;
	for (int i = 0; i < nheight; i++)
	{
		memcpy(ptempdes, ptempsrc, nmaskpitch);
		ptempdes += desx;
		ptempsrc += nmaskpitch;
	}

	SIZE size = { desx,desy };
	int nOutX = 0;
	int nOutY = 0;
	int nOutWidth = desx;
	int nOutHeight = desy;
	Gauss_RLE(popermask,	// 目标缓冲区:包含输入缓冲区、输出缓冲区
		1,		// 象素位数
		0,		// 输入缓冲区起点坐标 
		0,
		desx,		// 输入缓冲区宽、高
		desy,
		desx,	// 目标缓冲区宽、高
		desy,
		1,
		nOutX,		// 输出缓冲区起点坐标
		nOutY,
		nOutWidth,	// 输出缓冲区宽、高
		nOutHeight,
		(float)nsoft,
		(float)nsoft, -1);

	int nOffsetX, nOffsetY;
	nOffsetX = nborder * cos(fangle * PAI / 180);
	nOffsetY = nborder * sin(fangle * PAI / 180);
	makemask_sub(pdesmask, nmaskpitch, nheight, popermask, bound, nOffsetX, nOffsetY, ntype);

	if (FALSE)
	{
		DWORD* pdwBuffer = new DWORD[nmaskpitch * nheight];
		for (int i = 0; i < nheight; i++)
		{
			for (int j = 0; j < nmaskpitch; j++)
			{
				BYTE b = pdesmask[i * nmaskpitch + j];
				pdwBuffer[i * nmaskpitch + j] = 0xff000000 | b << 16 | b << 8 | b;
			}
		}
		BufferFunction::WriteBitmap(_T("E:\\3.bmp"), nmaskpitch, nheight, 32, (BYTE*)pdwBuffer);
	}
	makemask_convex(pbuffer, nwidth, nheight, pdesmask, nmaskpitch, ntype, color);
	delete[] pdesmask;
	delete[] popermask;
	pdesmask = NULL;
	popermask = NULL;
}

BOOL SYRender_DrawShadow(CSYBuffer* pSrcPool,
	CSYBuffer* pSrcMaskPool,
	RECT* rtSrc,
	RECT* rtDes,
	int	nOffsetX,
	int	nOffsetY,
	BaseGraph_DataBase::shadowinfo* pinfo,
	CSYBuffer*& pResultPool,
	IN  float fscale,
	IN  int nFrame, BOOL bBaseline, BOOL bSubSrc)
{
	if (pinfo->color.type == SYCOLOR_NONE)
		return FALSE;

	int nWidth = rtDes->right - rtDes->left;
	int nHeight = rtDes->bottom - rtDes->top;

	pResultPool = SYData_QueryCalcBuffer(nWidth * nHeight, TRUE);

	DWORD* pBuffer = pResultPool->m_dwBuffer;
	memset(pBuffer, 0, sizeof(DWORD) * nWidth * nHeight);

	RECT rtFill = { 0,0,nWidth,nHeight };

	SYColor_FillColor(&pinfo->color,
		pBuffer,
		nWidth,
		nHeight,
		rtFill.left,
		rtFill.top,
		rtFill.right,
		rtFill.bottom,
		fscale);
	if (FALSE)
	{
		BufferFunction::WriteBitmap(_T("E:\\shadow1.bmp"), nWidth, nHeight, 32, (BYTE*)pBuffer);
	}
	BYTE* pSrcMask = NULL;
	DWORD* pSrcDMask = pSrcPool->m_dwBuffer;
	if (pSrcMaskPool)
	{
		pSrcMask = (BYTE*)pSrcMaskPool->m_dwBuffer;
	}
	else
	{
		pSrcMask = new BYTE[pSrcPool->m_nWidth * pSrcPool->m_nHeight];
		BYTE* pMaskTemp = pSrcMask;

		for (int i = 0; i < pSrcPool->m_nHeight; i++)
		{
			for (int j = 0; j < pSrcPool->m_nWidth; j++)
			{
				pMaskTemp[j] = (pSrcDMask[j] >> 24) & 0xff;
			}
			pMaskTemp += pSrcPool->m_nWidth;
			pSrcDMask += pSrcPool->m_nWidth;
		}
	}
	if (FALSE)
	{
		DWORD* dwBuffer = new DWORD[pSrcPool->m_nWidth * pSrcPool->m_nHeight];
		int nCount = pSrcPool->m_nWidth * pSrcPool->m_nHeight;
		for (int i = 0; i < nCount; i++)
		{
			dwBuffer[i] = pSrcMask[i] | (pSrcMask[i] << 8) | (pSrcMask[i] << 16) | 0xff000000;
		}
		BufferFunction::WriteBitmap(_T("E:\\shadow1.bmp"), pSrcPool->m_nWidth, pSrcPool->m_nHeight, 32, (BYTE*)dwBuffer);
		delete[] dwBuffer;
		dwBuffer = NULL;
	}
	BYTE* pDesMask = new BYTE[nWidth * nHeight];
	if (pinfo->effect_style == SHADOW_INSERT)
		memset(pDesMask, 0xff, nWidth * nHeight);
	else
		memset(pDesMask, 00, nWidth * nHeight);
	SYKey_ProcBuffer(pDesMask,			// 目标键
		rtSrc->left - rtDes->left + nOffsetX * fscale,				// 操作区域
		rtSrc->top - rtDes->top + nOffsetY * fscale,
		rtSrc->right - rtSrc->left,
		rtSrc->bottom - rtSrc->top,
		nWidth,
		nHeight,
		nWidth,			// 目标键缓存一行的字节宽度
		pSrcMask,				// 源键
		0,
		0,
		rtSrc->right - rtSrc->left,
		rtSrc->bottom - rtSrc->top,
		pSrcPool->m_nWidth,			// 键幅面大小
		pSrcPool->m_nHeight,
		pSrcPool->m_nWidth,			// 源键缓存一行的字节宽度
		pinfo->effect_style == SHADOW_INSERT ? SY_KEY_MULTIPLY_NOTSRC : SY_KEY_REPLACE,			// 键操作模式
		BUFFER_PARITY_BOTH);	// 裁剪区域（可选项）

	if (FALSE)
	{
		DWORD* dwBuffer = new DWORD[nWidth * nHeight];
		int nCount = nWidth * nHeight;
		for (int i = 0; i < nCount; i++)
		{
			dwBuffer[i] = pDesMask[i] | (pDesMask[i] << 8) | (pDesMask[i] << 16) | 0xff000000;
		}
		BufferFunction::WriteBitmap(_T("E:\\shadow1.bmp"), nWidth, nHeight, 32, (BYTE*)dwBuffer);
		delete[] dwBuffer;
		dwBuffer = NULL;
	}

	if (pinfo->soft)
	{
		void auxGuassianBlur(LPBYTE lpDes, LPSIZE pSize, int degrees);
		SIZE size;
		size.cx = nWidth;
		size.cy = nHeight;
		if (FALSE)
		{
			DWORD* dwBuffer = new DWORD[nWidth * nHeight];
			int nCount = nWidth * nHeight;
			for (int i = 0; i < nCount; i++)
			{
				dwBuffer[i] = pDesMask[i] | (pDesMask[i] << 8) | (pDesMask[i] << 16) | 0xff000000;
			}
			BufferFunction::WriteBitmap(_T("E:\\shadow1.bmp"), nWidth, nHeight, 32, (BYTE*)dwBuffer);
			delete[] dwBuffer;
			dwBuffer = NULL;
		}

		auxDilation(pDesMask, &size, pinfo->soft * fscale / 2);
		if (FALSE)
		{
			DWORD* dwBuffer = new DWORD[nWidth * nHeight];
			int nCount = nWidth * nHeight;
			for (int i = 0; i < nCount; i++)
			{
				dwBuffer[i] = pDesMask[i] | (pDesMask[i] << 8) | (pDesMask[i] << 16) | 0xff000000;
			}
			BufferFunction::WriteBitmap(_T("E:\\shadow1.bmp"), nWidth, nHeight, 32, (BYTE*)dwBuffer);
			delete[] dwBuffer;
			dwBuffer = NULL;
		}

		auxGuassianBlur(pDesMask, &size, pinfo->soft * fscale);
		if (FALSE)
		{
			DWORD* dwBuffer = new DWORD[nWidth * nHeight];
			int nCount = nWidth * nHeight;
			for (int i = 0; i < nCount; i++)
			{
				dwBuffer[i] = pDesMask[i] | (pDesMask[i] << 8) | (pDesMask[i] << 16) | 0xff000000;
			}
			BufferFunction::WriteBitmap(_T("E:\\shadow1.bmp"), nWidth, nHeight, 32, (BYTE*)dwBuffer);
			delete[] dwBuffer;
			dwBuffer = NULL;
		}
	}
	int nsrcX = 0, nsrcY = 0, srcWidth = nWidth, srcHeight = nHeight;
	if (pinfo->effect_style == SHADOW_INSERT)
	{
		nsrcX = rtSrc->left - rtDes->left;				// 操作区域
		nsrcY = rtSrc->top - rtDes->top;
		srcWidth = rtSrc->right - rtSrc->left;
		srcHeight = rtSrc->bottom - rtSrc->top;
	}

	if (bSubSrc && pinfo->effect_style != SHADOW_INSERT)
	{
		SYKey_ProcBuffer(pDesMask,			// 目标
			rtSrc->left - rtDes->left,				// 操作区域
			rtSrc->top - rtDes->top,
			rtSrc->right - rtSrc->left,
			rtSrc->bottom - rtSrc->top,
			nWidth,
			nHeight,
			nWidth,			// 目标键缓存一行的字节宽度
			pSrcMask,				// 源键
			0,
			0,
			rtSrc->right - rtSrc->left,
			rtSrc->bottom - rtSrc->top,
			pSrcPool->m_nWidth,			// 键幅面大小
			pSrcPool->m_nHeight,
			pSrcPool->m_nWidth,			// 源键缓存一行的字节宽度
			SY_KEY_SAT_SUB,			// 键操作模式
			BUFFER_PARITY_BOTH);	// 裁剪区域（可选项）
	}

	SYBGRA32_ProcKey(pBuffer,			// 目标
		nsrcX,					// 目标操作区域
		nsrcY,
		srcWidth,		// 输入缓冲区宽、高
		srcHeight,
		nWidth,		// 输入缓冲区宽、高
		nHeight,
		pDesMask,				// 键缓存
		nsrcX,
		nsrcY,
		srcWidth,		// 输入缓冲区宽、高
		srcHeight,
		nWidth,		// 输入缓冲区宽、高
		nHeight,
		nWidth,				// 键缓存一行的字节宽度
		SY_KEY_MULTIPLY,			// 键操作模式
		BUFFER_PARITY_BOTH);	// 裁剪区域（可选项）
	if (pDesMask)
		delete[] pDesMask;
	pDesMask = NULL;
	if (FALSE)
	{
		BufferFunction::WriteBitmap(_T("E:\\shadow1.bmp"), nWidth, nHeight, 32, (BYTE*)pBuffer);
	}

	if (pinfo->effect_fadeout)
	{
		DWORD* cAlpha = new DWORD[nHeight];
		memset(cAlpha, 0xff, sizeof(DWORD) * nHeight);

		int nValidHeight = (int)(pinfo->effect_fadeout / 256 * nHeight);
		if (pinfo->effect_fyscale + 1 > 0)
		{
			for (int i = 0; i < nValidHeight; i++)
			{
				cAlpha[nHeight - i - 1] = (i * 255) / nValidHeight;
			}
		}
		else
		{
			for (int i = 0; i < nValidHeight; i++)
			{
				cAlpha[i] = (i * 255) / nValidHeight;
			}
		}

		SYBGRA32_FillAlpha_With_H(pBuffer,		// 目标
			0,					// 填充区域
			0,
			nWidth,
			nHeight,
			nWidth,			// 缓存大小
			nHeight,
			NULL,			// 裁剪区域（可选项）
			cAlpha,				// Alpha值
			SY_FILL_SCALE_ALPHA_ALL,		// 填充模式
			0x00000000);			// 掩码颜色值（依赖模式的可选项）
		delete[] cAlpha;
		cAlpha = NULL;
	}



	if (pinfo->effect_style == SHADOW_INSERT)
	{

		CSYBuffer* pResultCurPool = SYData_QueryCalcBuffer(nWidth * nHeight, TRUE);
		SY_DMemSet(pResultCurPool->m_dwBuffer, 0, nWidth * nHeight);

		SYBGRA32_ProcKey(pBuffer,			// 目标
			rtSrc->left - rtDes->left,				// 操作区域
			rtSrc->top - rtDes->top,
			rtSrc->right - rtSrc->left,
			rtSrc->bottom - rtSrc->top,
			nWidth,
			nHeight,
			pSrcMask,				// 键缓存
			0,
			0,
			rtSrc->right - rtSrc->left,
			rtSrc->bottom - rtSrc->top,
			rtSrc->right - rtSrc->left,
			rtSrc->bottom - rtSrc->top,
			rtSrc->right - rtSrc->left,
			SY_KEY_MULTIPLY,			// 键操作模式
			BUFFER_PARITY_BOTH);	// 裁剪区域（可选项）

		SY_BitBlt(pResultCurPool->m_dwBuffer,
			rtSrc->left - rtDes->left,				// 操作区域
			rtSrc->top - rtDes->top,
			rtSrc->right - rtSrc->left,
			rtSrc->bottom - rtSrc->top,
			nWidth,
			nHeight,
			pSrcPool->m_dwBuffer,
			0,
			0,
			rtSrc->right - rtSrc->left,
			rtSrc->bottom - rtSrc->top,
			rtSrc->right - rtSrc->left,
			rtSrc->bottom - rtSrc->top,
			BUFFER_COPY, BUFFER_PARITY_BOTH);		// 填充值

		SY_BitBlt(pResultCurPool->m_dwBuffer,
			rtSrc->left - rtDes->left,				// 操作区域
			rtSrc->top - rtDes->top,
			rtSrc->right - rtSrc->left,
			rtSrc->bottom - rtSrc->top,
			nWidth,
			nHeight,
			pBuffer,
			rtSrc->left - rtDes->left,				// 操作区域
			rtSrc->top - rtDes->top,
			rtSrc->right - rtSrc->left,
			rtSrc->bottom - rtSrc->top,
			nWidth,
			nHeight,
			BUFFER_MIX,
			BUFFER_PARITY_BOTH);
		SYData_ReleaseCalcBuffer(pResultPool);
		pResultPool = pResultCurPool;
	}
	if (FALSE)
	{
		BufferFunction::WriteBitmap(_T("E:\\shadow1.bmp"), nWidth, nHeight, 32, (BYTE*)pResultPool->m_dwBuffer);
	}
	if (!pSrcMaskPool)
	{
		delete[] pSrcMask;
		pSrcMask = NULL;
	}
	return 1;
}

int  _SYRender_GetCubeChannel(sy_render_channel** pDesChannel, sy_render_channel** pSrcChannel)
{
	int nCount = 0;
	if (pSrcChannel[CHANNEL_MATERIAL])
	{
		pDesChannel[nCount++] = pSrcChannel[CHANNEL_MATERIAL];
	}

	if (pSrcChannel[CHANNEL_CUBELIGHT])
	{
		pDesChannel[nCount++] = pSrcChannel[CHANNEL_CUBELIGHT];
	}
	return nCount;

}

BOOL _SYRender_MakeTable(sy_render_channel** pChannels, int nCount, LPLONG* &pContrastTable, LPLONG* &ppXTable, LPLONG* &ppYTable, int nDesWidth, int nDesHeight)
{
	if (nCount <= 0)
		return TRUE;

	const SIZE sSize = { nDesWidth,nDesHeight };
	LONG nDataLength = (sizeof(LPLONG) * 2 + sizeof(LONG_PTR)*(sSize.cx + sSize.cy)) * nCount;
	pContrastTable = (LPLONG*)new BYTE[nDataLength / sizeof(BYTE)];
	if (!pContrastTable)
		return FALSE;

	ppXTable = (LPLONG*)pContrastTable;
	ppYTable = ppXTable + nCount;

	ppXTable[0] = (LPLONG)(ppYTable + nCount);
	ppYTable[0] = ppXTable[0] + sSize.cx;

	for (int nIndex = 1; nIndex < nCount; nIndex++)
	{
		ppXTable[nIndex] = ppXTable[nIndex - 1] + (sSize.cx + sSize.cy);
		ppYTable[nIndex] = ppYTable[nIndex - 1] + (sSize.cx + sSize.cy);
	}
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		const sy_render_channel*	pThisChannel = pChannels[nIndex];

		LPLONG	pXTable = ppXTable[nIndex];
		LPLONG	pYTable = ppYTable[nIndex];

		for (int x = sSize.cx - 1; x >= 0; x--)
			*(pXTable + x) = x * pThisChannel->width / sSize.cx;
		for (int y = sSize.cy - 1; y >= 0; y--)
			*(pYTable + y) = (y * pThisChannel->height / sSize.cy)*pThisChannel->width;

	}


	return TRUE;
}

BOOL MaterialColor(DWORD &dwColor,
	int ndegree)
{
	DWORD R, G, B;
	if (ndegree >= 0x80)
	{
		ndegree -= 0x80;

		R = GetRGBA_R(dwColor) + ((ndegree * (0xFF - GetRGBA_R(dwColor))) >> 8);
		R = min(R, 255);
		G = GetRGBA_G(dwColor) + ((ndegree * (0xFF - GetRGBA_G(dwColor))) >> 8);
		G = min(G, 255);
		B = GetRGBA_B(dwColor) + ((ndegree * (0xFF - GetRGBA_B(dwColor))) >> 8);
		B = min(B, 255);
	}
	else
	{
		ndegree = 0x80 - ndegree;
		R = GetRGBA_R(dwColor) - ((ndegree * (GetRGBA_R(dwColor))) >> 8);
		R = min(R, 255);
		G = GetRGBA_G(dwColor) - ((ndegree * (GetRGBA_G(dwColor))) >> 8);
		G = min(G, 255);
		B = GetRGBA_B(dwColor) - ((ndegree * (GetRGBA_B(dwColor))) >> 8);
		B = min(B, 255);
	}
	dwColor = RGBA(R, G, B, GetAValue(dwColor));
	return 1;
}

BOOL LightColor(DWORD &dwColor,
	int ndegree,
	DWORD rR, DWORD rG, DWORD rB,
	DWORD aR, DWORD aG, DWORD aB,
	DWORD scale)
{

	//dwTempBuffer[k] = dwTempBuffer[k] 
	//+ (((ndegree * plight->reflectcolor) >> 8 + plight->ambience) * plight->ambience_degree);

	if (ndegree >= 0x80)
	{
		ndegree -= 0x80;
		DWORD R = ((GetRGBA_R(dwColor) + ((ndegree * rR) >> 8) + aR) * scale) >> 8;
		R = min(R, 255);
		DWORD G = ((GetRGBA_G(dwColor) + ((ndegree * rG) >> 8) + aG) * scale) >> 8;
		G = min(G, 255);
		DWORD B = ((GetRGBA_B(dwColor) + ((ndegree * rB) >> 8) + aB) * scale) >> 8;
		B = min(B, 255);
		dwColor = RGBA(R, G, B, GetAValue(dwColor));
	}
	else
	{
		DWORD R = ((GetRGBA_R(dwColor) + aR) * scale) >> 8;
		R = min(R, 255);
		DWORD G = ((GetRGBA_G(dwColor) + aG) * scale) >> 8;
		G = min(G, 255);
		DWORD B = ((GetRGBA_B(dwColor) + aB) * scale) >> 8;
		B = min(B, 255);
		dwColor = RGBA(R, G, B, GetAValue(dwColor));
	}
	return 1;
}

//在buffer上加入光效
BOOL SYRenderLightToOpBuff(IN OUT	CSYBuffer*& pResultPool,
	IN 		RECT rtOutBound,
	IN		RECT rtInValid,
	IN		sy_render_channel** pChannelPool,
	IN		int nCount,
	IN		LPLONG* lpXTable,
	IN		LPLONG* lpYTable
)
{
	int nDesWidth = rtInValid.right - rtInValid.left;
	int nDesHeight = rtInValid.bottom - rtInValid.top;
	int nBufferWidth = rtOutBound.right - rtOutBound.left;
	int nBufferHeight = rtOutBound.bottom - rtOutBound.top;

	DWORD rR, rG, rB, aR, aG, aB;

	DWORD* dwTempBuffer = pResultPool->m_dwBuffer + (rtInValid.top - rtOutBound.top) * nBufferWidth + rtInValid.left - rtOutBound.left;
	//	BYTE*	bTempBuffer = pbBuffer;
	for (int i = 0; i < nDesHeight; i++)
	{
		for (int k = 0; k < nDesWidth; k++)
		{
			if ((dwTempBuffer[k] & 0xff000000) == 0)
				continue;
			for (int n = 0; n < nCount; n++)
			{
				LPLONG	pXStart = *(lpXTable + n);
				LPLONG	pYStart = *(lpYTable + n);
				const sy_render_channel*	thisChannel = pChannelPool[n];
				int nPos = *(pYStart + i) + *(pXStart + k);
				int ndegree = *(thisChannel->buffer + nPos);

				if (thisChannel->type == CHANNEL_MATERIAL)
				{
					MaterialColor(dwTempBuffer[k],
						ndegree);

				}
				else if (thisChannel->type == CHANNEL_FACELIGHT
					|| thisChannel->type == CHANNEL_CUBELIGHT)
				{
					rR = GetRGBA_R(thisChannel->reflectcolor);
					rG = GetRGBA_G(thisChannel->reflectcolor);
					rB = GetRGBA_B(thisChannel->reflectcolor);

					aR = GetRGBA_R(thisChannel->ambience);
					aG = GetRGBA_G(thisChannel->ambience);
					aB = GetRGBA_B(thisChannel->ambience);

					LightColor(dwTempBuffer[k],
						ndegree,
						rR, rG, rB,
						aR, aG, aB,
						thisChannel->ambience_degree);
				}
				else if (thisChannel->type == CHANNEL_ALPHA)
				{
					dwTempBuffer[k] = (dwTempBuffer[k] & 0x00ffffff) | (((ndegree * GetAValue(dwTempBuffer[k])) << 16) & 0xff000000);
				}
			}

		}
		dwTempBuffer += nBufferWidth;
	}
	return 1;
}

int _SYRender_GetFaceChannel(sy_render_channel** pDesChannel, sy_render_channel** pSrcChannel)
{
	int nCount = 0;

	if (pSrcChannel[CHANNEL_ALPHA])
	{
		pDesChannel[nCount++] = pSrcChannel[CHANNEL_ALPHA];
	}
	if (pSrcChannel[CHANNEL_MATERIAL])
	{
		pDesChannel[nCount++] = pSrcChannel[CHANNEL_MATERIAL];
	}

	if (pSrcChannel[CHANNEL_FACELIGHT])
	{
		pDesChannel[nCount++] = pSrcChannel[CHANNEL_FACELIGHT];
	}
	return nCount;
}

BASEGRAPH_DATARENDER_API BOOL SYRenderGraphToOpBuff(IN vectorgraph * pGraph, IN static_effect * effect, OUT CSYBuffer *& pResultPool, OUT RECT & rtOutBound, OUT int & nWidth, OUT int & nHeight, OUT int & nPitch, IN float fscale, OUT pointf & ptOrg, IN double dFrame, IN BOOL bShadowBaseline, IN sy_render_channel ** pChannelPool)
{
	int facew = 0, faceh = 0;
	RECT rtFaceBound = { 0 };
	RECT rtBorderBound = { 0 };
	RECT rtBorderCubeBound = { 0 };
	RECT rtCubeBound = { 0 };
	RECT rtShadowBound = { 0 };
	RECT rtAllFaceBound = { 0 };
	if (pGraph->ncell <= 0)
		return FALSE;
	SYRender_GetGraph_RenderInfo(pGraph, fscale, facew, faceh, rtFaceBound);
	if (facew == 0 || faceh == 0 || IsRectEmpty(&rtFaceBound))
		return FALSE;

	sy_render_channel** pMaskPool = pChannelPool;

	sy_render_channel** pDesMaskPool = new sy_render_channel*[CHANNEL_COUNT];
	memset(pDesMaskPool, 0, sizeof(sy_render_channel*) * CHANNEL_COUNT);

	if (!pChannelPool)
	{
		pMaskPool = new sy_render_channel*[CHANNEL_COUNT];
		memset(pMaskPool, 0, sizeof(sy_render_channel*) * CHANNEL_COUNT);
	}

	SYRender_LoadChannel(effect, pMaskPool);

	vectorgraph BorderOut;
	vectorgraph CubeOut, AroundCubeOut;
	memset(&BorderOut, 0, sizeof(vectorgraph));
	memset(&CubeOut, 0, sizeof(vectorgraph));
	memset(&AroundCubeOut, 0, sizeof(vectorgraph));

	vectorgraph* pFaceGraph = pGraph;
	vector2f vborderOffset = { 0,0 };
	vector2f vcubeborderOffset = { 0,0 };

	rtOutBound = rtFaceBound;
	rtAllFaceBound = rtFaceBound;

	// 此值暂时无用，之前版本错误，有些模板存有错误值，特此修正
	effect->shadow.effect_fxtitle = 0;
	effect->bordershadow.effect_fxtitle = 0;

	//面
	if (TRUE/*!effect->face.disable && effect->face.color.type != SYCOLOR_NONE*/)
	{
		//生成立体边轮廓
		if (pFaceGraph && !effect->cube.disable && effect->cube.color.type != SYCOLOR_NONE && !effect->cube.disable && abs(effect->cube.width)>ABS_FLOAT_0)
		{
			memset(&CubeOut, 0, sizeof(vectorgraph));
			SYRender_GenCubeGraph(&CubeOut, pGraph, NULL, &effect->cube);
			SYRender_GetGraph_RenderInfo(&CubeOut, fscale, nWidth, nHeight, rtCubeBound);
			UnionRect(&rtOutBound, &rtOutBound, &rtCubeBound);

			vborderOffset.x += effect->cube.width * cos(effect->cube.angle);// * fscale;
			vborderOffset.y += effect->cube.width * sin(effect->cube.angle);// * fscale;

			UnionRect(&rtAllFaceBound, &rtAllFaceBound, &rtCubeBound);
		}
		//影信息
		if (effect->shadow.color.type != SYCOLOR_NONE
			&& !effect->shadow.disable
			&& (effect->shadow.effect_fyscale != 0 || effect->shadow.effect_fxtitle != 0 || effect->shadow.soft > 0 || effect->shadow.offsetx != 0 || effect->shadow.offsety != 0))
		{
			rtShadowBound = rtFaceBound;
			//memset(&BorderShadow,0,sizeof(vectorgraph));
			//SYRender_GenBorderGraph(&BorderShadow,pGraph,&vborderOffset,effect->shadow.soft/2, &effect->border.linestyle,FALSE);

			OffsetRect(&rtShadowBound, effect->shadow.offsetx * fscale, effect->shadow.offsety * fscale);
			OffsetRect(&rtShadowBound, vborderOffset.x * fscale, vborderOffset.y * fscale);
			if (effect->shadow.soft)
			{
				int nSoft = SY_FToI(effect->shadow.soft * fscale + 1);
				InflateRect(&rtShadowBound, nSoft, nSoft);
			}
			UnionRect(&rtOutBound, &rtOutBound, &rtShadowBound);
			UnionRect(&rtAllFaceBound, &rtAllFaceBound, &rtShadowBound);
		}

	}

	//生成边轮廓
	if (!effect->border.disable&&effect->border.color.type != SYCOLOR_NONE)
	{
		//生成边轮廓
		memset(&BorderOut, 0, sizeof(vectorgraph));
		if (effect->border.width == 0)
		{
			SYCopy_VectorGraph(&BorderOut, pGraph);
		}
		else
			SYRender_GenBorderGraph(&BorderOut, pGraph, &vborderOffset, effect->border.width, effect->border.interval, effect->border.linestyle, FALSE);

		SYRender_GetGraph_RenderInfo(&BorderOut, fscale, nWidth, nHeight, rtBorderBound);

		UnionRect(&rtOutBound, &rtOutBound, &rtBorderBound);

		rtBorderCubeBound = rtOutBound;
		//生成边的立体轮廓
		if (!effect->bordercube.disable && effect->bordercube.color.type != SYCOLOR_NONE && abs(effect->bordercube.width)>ABS_FLOAT_0)
		{
			memset(&AroundCubeOut, 0, sizeof(vectorgraph));
			SYRender_GenCubeGraph(&AroundCubeOut, &BorderOut, NULL, &effect->bordercube);
			SYRender_GetGraph_RenderInfo(&AroundCubeOut, fscale, nWidth, nHeight, rtBorderCubeBound);
			//graph_release(&AroundCubeOut);
			OffsetRect(&rtBorderCubeBound, vborderOffset.x * fscale, vborderOffset.y * fscale);

			// 			vcubeborderOffset.x += (effect->bordercube.width * cos(effect->bordercube.angle));
			// 			vcubeborderOffset.y += (effect->bordercube.width * sin(effect->bordercube.angle));

			UnionRect(&rtOutBound, &rtOutBound, &rtBorderCubeBound);
		}
		if (effect->bordershadow.color.type != SYCOLOR_NONE
			&& !effect->bordershadow.disable
			&& (effect->bordershadow.effect_fyscale != 0
				|| effect->bordershadow.effect_fxtitle != 0
				|| effect->bordershadow.soft > 0
				|| effect->bordershadow.offsetx != 0
				|| effect->bordershadow.offsety != 0))
		{
			rtShadowBound = rtBorderCubeBound;
			OffsetRect(&rtShadowBound, effect->bordershadow.offsetx * fscale, effect->bordershadow.offsety * fscale);
			OffsetRect(&rtShadowBound, vcubeborderOffset.x * fscale, vcubeborderOffset.y * fscale);

			if (effect->bordershadow.soft)
			{
				int nSoft = SY_FToI(effect->bordershadow.soft * fscale + 1);
				InflateRect(&rtShadowBound, nSoft, nSoft);
			}
			UnionRect(&rtOutBound, &rtOutBound, &rtShadowBound);
		}
	}

	DWORD* pFaceBuffer = NULL;
	DWORD* pBorderBuffer = NULL;
	nWidth = facew;
	nHeight = faceh;
	nPitch = facew;

	BOOL bDrawFace = FALSE;
	int nAllFaceWidth = rtAllFaceBound.right - rtAllFaceBound.left;
	int nAllFaceHeight = rtAllFaceBound.bottom - rtAllFaceBound.top;
	CSYBuffer* pFaceSystemBuffer = NULL;
	BOOL	bFaceValid = TRUE;

	CSYBuffer* pFaceMask = NULL;
	int nFaceMaskWidth = 0;
	int nFaceMaskHeight = 0;
	//面系操作
	if (TRUE)
	{
		int nFWidth = rtFaceBound.right - rtFaceBound.left;
		int nFHeight = rtFaceBound.bottom - rtFaceBound.top;
		//effect->face.effect_type = STATCEFFECT_TYPE_CONVEX;
		//effect->face.effect_width = 20;
		//effect->face.soft = 5;
		//effect->face.avcolor = 0xff000000;
		CSYBuffer* pFacePool = SYData_QueryCalcBuffer(nFWidth * nFHeight, TRUE);
		pFaceBuffer = pFacePool->m_dwBuffer;
		//绘制面
		SY_DMemSet(pFaceBuffer, 0, nFWidth * nFHeight);
		if (effect->face.disable || effect->face.color.type == SYCOLOR_NONE)
		{
			sycolor color;
			SYColor_CreateHomo(&color, 0xffffffff);
			SYRender_DrawGraph(pFaceGraph,
				pFaceBuffer,
				nFWidth,
				nFHeight,
				fscale,
				rtFaceBound,
				&color,
				0,
				1,
				0,
				0,
				effect->shadow.avcolor,
				dFrame,
				&pFaceMask,
				&nFaceMaskWidth,
				&nFaceMaskHeight);
		}
		else
		{
			SYRender_DrawGraph(pFaceGraph,
				pFaceBuffer,
				nFWidth,
				nFHeight,
				fscale,
				rtFaceBound,
				&effect->face.color,
				effect->face.effect_type,
				effect->face.effect_width,
				effect->face.soft,
				effect->face.art.cu_angle,
				effect->face.avcolor,
				dFrame,
				&pFaceMask,
				&nFaceMaskWidth,
				&nFaceMaskHeight);
		}
		if (FALSE)
		{
			BufferFunction::WriteBitmap(_T("E:\\2.bmp"), nFWidth, nFHeight, 32, (BYTE*)pFaceBuffer);
		}
		pFacePool->m_nWidth = nFWidth;
		pFacePool->m_nHeight = nFHeight;
		pFacePool->m_rtBound = rtFaceBound;
		double tTime1;
		if (FALSE)
		{
			BufferFunction::WriteBitmap(_T("E:\\2.bmp"), nFWidth, nFHeight, 32, (BYTE*)pFacePool->m_dwBuffer);
		}
		if (effect->face.art.type == BEVAL_3D)
			Light(pFaceBuffer,
				nFWidth,
				nFHeight,
				&effect->face.art.metal
			);
		bDrawFace = TRUE;

		if (!effect->shadow.disable && effect->shadow.color.type != SYCOLOR_NONE
			&& (effect->shadow.effect_fyscale != 0 || effect->shadow.effect_fxtitle != 0 || effect->shadow.soft > 0 || effect->shadow.offsetx != 0 || effect->shadow.offsety != 0))
		{
			int nOffsetX = effect->shadow.offsetx + vborderOffset.x;
			int nOffsetY = effect->shadow.offsety + vborderOffset.y;

			SYRender_DrawShadow(pFacePool,
				pFaceMask,
				&rtFaceBound,
				&rtAllFaceBound,
				nOffsetX,
				nOffsetY,
				&effect->shadow,
				pFaceSystemBuffer,
				fscale,
				dFrame,
				bShadowBaseline, !effect->face.disable);
			if (FALSE)
			{
				BufferFunction::WriteBitmap(_T("E:\\shadow.bmp"), rtOutBound.right - rtOutBound.left, rtOutBound.bottom - rtOutBound.top, 32, (BYTE*)pFaceSystemBuffer->m_dwBuffer);
				BufferFunction::WriteBitmap(_T("E:\\face.bmp"), nFWidth, nFHeight, 32, (BYTE*)pFacePool->m_dwBuffer);
			}
		}

		if (!effect->cube.disable && effect->cube.color.type != SYCOLOR_NONE && abs(effect->cube.width)>ABS_FLOAT_0)
		{
			int nCubeWidth = rtCubeBound.right - rtCubeBound.left;
			int nCubebHeight = rtCubeBound.bottom - rtCubeBound.top;
			RECT rtIn = rtCubeBound;
			if (!pFaceSystemBuffer)
			{
				nCubeWidth = rtAllFaceBound.right - rtAllFaceBound.left;
				nCubebHeight = rtAllFaceBound.bottom - rtAllFaceBound.top;
				rtIn = rtAllFaceBound;
			}

			CSYBuffer* pCubePool = SYData_QueryCalcBuffer(nCubeWidth * nCubebHeight, TRUE);
			SYRender_DrawGraph(
				&CubeOut,
				pCubePool->m_dwBuffer,
				nCubeWidth,
				nCubebHeight,
				fscale,
				rtIn,
				&effect->cube.color,
				effect->cube.effect_type,
				effect->cube.effect_width,
				effect->cube.art.soft,
				effect->cube.art.cu_angle,
				effect->cube.avcolor,
				dFrame);
			SYBGRA32_ProcKey(pCubePool->m_dwBuffer,			// 目标
				rtFaceBound.left - rtIn.left,					// 目标操作区域
				rtFaceBound.top - rtIn.top,
				rtFaceBound.right - rtFaceBound.left,
				rtFaceBound.bottom - rtFaceBound.top,
				nCubeWidth,
				nCubebHeight,
				(LPBYTE)pFaceMask->m_dwBuffer,				// 键缓存
				0,					// 键操作区域
				0,
				nFWidth,
				nFHeight,
				nFWidth,
				nFHeight,
				nFWidth,				// 键缓存一行的字节宽度
				SY_KEY_MULTIPLY_NOTSRC,			// 键操作模式
				BUFFER_PARITY_BOTH,	// 奇偶场类型
				NULL);	// 裁剪区域（可选项）

			int nMaskCount = _SYRender_GetCubeChannel(pDesMaskPool, pMaskPool);
			LPLONG* pContrastTable = NULL, *ppXTable = NULL, *ppYTable = NULL;
			// MakeTable的缓存大小为当前颜色种类（周边，边。。。）大小，比面的大小要大，否则SYRenderLightToOpBuff内存溢出
			//_SYRender_MakeTable(pDesMaskPool,nMaskCount,pContrastTable,ppXTable,ppYTable,nFWidth,nFHeight);
			_SYRender_MakeTable(pDesMaskPool, nMaskCount, pContrastTable, ppXTable, ppYTable, rtCubeBound.right - rtCubeBound.left, rtCubeBound.bottom - rtCubeBound.top);

			SYRenderLightToOpBuff(
				pCubePool,
				rtIn,
				rtCubeBound,
				pDesMaskPool,
				nMaskCount,
				ppXTable,
				ppYTable
			);
			if (pContrastTable)
			{
				delete[] pContrastTable;
				pContrastTable = NULL;
			}

			graph_release(&CubeOut);
			if (pFaceSystemBuffer)
			{
				SY_BitBlt(pFaceSystemBuffer->m_dwBuffer,
					rtCubeBound.left - rtAllFaceBound.left,
					rtCubeBound.top - rtAllFaceBound.top,
					nCubeWidth,
					nCubebHeight,
					rtAllFaceBound.right - rtAllFaceBound.left,
					rtAllFaceBound.bottom - rtAllFaceBound.top,
					pCubePool->m_dwBuffer,
					0,
					0,
					nCubeWidth,
					nCubebHeight,
					nCubeWidth,
					nCubebHeight,
					BUFFER_MIX,
					BUFFER_PARITY_BOTH);
				SYData_ReleaseCalcBuffer(pCubePool);
			}
			else
				pFaceSystemBuffer = pCubePool;
		}
		if (pFaceSystemBuffer)
		{
			if (!effect->face.disable&& effect->face.color.type != SYCOLOR_NONE && !(!effect->shadow.disable && effect->shadow.effect_style == SHADOW_INSERT))
			{
				SY_BitBlt(pFaceSystemBuffer->m_dwBuffer,
					rtFaceBound.left - rtAllFaceBound.left,
					rtFaceBound.top - rtAllFaceBound.top,
					nFWidth,
					nFHeight,
					nAllFaceWidth,
					nAllFaceHeight,
					pFacePool->m_dwBuffer,
					0,
					0,
					nFWidth,
					nFHeight,
					nFWidth,
					nFHeight,
					BUFFER_MIX,
					BUFFER_PARITY_BOTH);
			}
			SYData_ReleaseCalcBuffer(pFacePool);
			pFacePool = NULL;
		}
		else if (!effect->face.disable)
			pFaceSystemBuffer = pFacePool;
		else
		{
			SYData_ReleaseCalcBuffer(pFacePool);
			pFacePool = NULL;
		}
	}
	//边系操作
	if (!effect->border.disable && effect->border.color.type != SYCOLOR_NONE)
	{
		CSYBuffer* pBorderMaskPool = NULL;
		int nBorderMaskWidth = 0;
		int nBorderMaskHeight = 0;
		//矩形
		int nBorderWidth = rtOutBound.right - rtOutBound.left;
		int nBorderHeight = rtOutBound.bottom - rtOutBound.top;
		int nOffsetX = effect->shadow.offsetx + vborderOffset.x;
		int nOffsetY = effect->shadow.offsety + vborderOffset.y;

		//请求边界池
		CSYBuffer* pBorderPool = SYData_QueryCalcBuffer(nBorderWidth * nBorderHeight, TRUE);
		pBorderPool->m_nWidth = nBorderWidth;
		pBorderPool->m_nHeight = nBorderHeight;
		pBorderPool->m_rtBound = rtOutBound;

		pBorderBuffer = pBorderPool->m_dwBuffer;
		memset(pBorderPool->m_dwBuffer, 0, nBorderWidth * nBorderHeight * sizeof(DWORD));
		SYRender_DrawGraph(
			&BorderOut,
			pBorderBuffer,
			nBorderWidth,
			nBorderHeight,
			fscale,
			rtOutBound,
			&effect->border.color,
			effect->border.effect_type,
			effect->border.effect_width,
			effect->border.soft,
			effect->border.art.cu_angle,
			effect->border.avcolor,
			dFrame,
			&pBorderMaskPool,
			&nBorderMaskWidth,
			&nBorderMaskHeight
		);
		graph_release(&BorderOut);

		if (FALSE)
		{
			BufferFunction::WriteBitmap(_T("E:\\2.tga"), nBorderWidth, nBorderHeight, 32, (BYTE*)pBorderBuffer);
		}

		int nMaskCount = _SYRender_GetFaceChannel(pDesMaskPool, pMaskPool);
		LPLONG* pContrastTable = NULL, *ppXTable = NULL, *ppYTable = NULL;
		_SYRender_MakeTable(pDesMaskPool, nMaskCount, pContrastTable, ppXTable, ppYTable, rtBorderBound.right - rtBorderBound.left, rtBorderBound.bottom - rtBorderBound.top);

		SYRenderLightToOpBuff(
			pBorderPool,
			rtOutBound,
			rtBorderBound,
			pDesMaskPool,
			nMaskCount,
			ppXTable,
			ppYTable
		);
		if (pContrastTable)
		{
			delete[] pContrastTable;
			pContrastTable = NULL;
		}

		if (effect->border.art.type == BEVAL_3D)
			Light(pBorderBuffer,
				nBorderWidth,
				nBorderHeight,
				&effect->border.art.metal
			);
		if (FALSE)
		{
			BufferFunction::WriteBitmap(_T("E:\\2.bmp"), nBorderWidth, nBorderHeight, 32, (BYTE*)pBorderBuffer);
		}
		if (!effect->bordershadow.disable && effect->bordershadow.color.type != SYCOLOR_NONE
			&& (effect->bordershadow.effect_fyscale != 0
				|| effect->bordershadow.effect_fxtitle != 0
				|| effect->bordershadow.soft > 0
				|| effect->bordershadow.offsetx != 0
				|| effect->bordershadow.offsety != 0))
		{
			int nSWidth = nWidth, nSHeight = nHeight, nSPitch = nPitch;
			int nOffsetX = effect->bordershadow.offsetx + vcubeborderOffset.x;
			int nOffsetY = effect->bordershadow.offsety + vcubeborderOffset.y;

			SYRender_DrawShadow(pBorderPool, pBorderMaskPool, &rtOutBound, &rtOutBound, nOffsetX, nOffsetY, &effect->bordershadow, pResultPool, fscale, dFrame, bShadowBaseline, !effect->border.disable);
		}

		if (!effect->bordercube.disable && effect->bordercube.color.type != SYCOLOR_NONE && abs(effect->bordercube.width)>ABS_FLOAT_0)
		{
			int nBCubeWidth = rtOutBound.right - rtOutBound.left;
			int nBCubeHeight = rtOutBound.bottom - rtOutBound.top;
			RECT rtIn = rtOutBound;

			CSYBuffer* pCubePool = SYData_QueryCalcBuffer(nBCubeWidth * nBCubeHeight, TRUE);
			pCubePool->m_nWidth = nBCubeWidth;
			pCubePool->m_nHeight = nBCubeHeight;
			pCubePool->m_rtBound = rtOutBound;

			SYRender_DrawGraph(
				&AroundCubeOut,
				pCubePool->m_dwBuffer,
				nBCubeWidth,
				nBCubeHeight,
				fscale,
				rtIn,
				&effect->bordercube.color,
				effect->bordercube.effect_type,
				effect->bordercube.effect_width,
				effect->bordercube.art.soft,
				effect->bordercube.art.cu_angle,
				effect->bordercube.avcolor,
				dFrame);
			graph_release(&AroundCubeOut);

			int nMaskCount = _SYRender_GetCubeChannel(pDesMaskPool, pMaskPool);
			LPLONG* pContrastTable = NULL, *ppXTable = NULL, *ppYTable = NULL;
			_SYRender_MakeTable(pDesMaskPool, nMaskCount, pContrastTable, ppXTable, ppYTable, rtBorderCubeBound.right - rtBorderCubeBound.left, rtBorderCubeBound.bottom - rtBorderCubeBound.top);

			SYRenderLightToOpBuff(
				pCubePool,
				rtIn,
				rtBorderCubeBound,
				pDesMaskPool,
				nMaskCount,
				ppXTable,
				ppYTable
			);
			if (pContrastTable)
			{
				delete[] pContrastTable;
				pContrastTable = NULL;
			}
			if (pResultPool)
			{
				// 没有偏移，否则有周边阴影情况下周边立体边位置不对
				SY_BitBlt(pResultPool->m_dwBuffer,
					0,//rtBorderCubeBound.left - rtOutBound.left,
					0,//rtBorderCubeBound.top - rtOutBound.top,
					nBCubeWidth,
					nBCubeHeight,
					rtOutBound.right - rtOutBound.left,
					rtOutBound.bottom - rtOutBound.top,
					pCubePool->m_dwBuffer,
					0,
					0,
					nBCubeWidth,
					nBCubeHeight,
					nBCubeWidth,
					nBCubeHeight,
					BUFFER_MIX,
					BUFFER_PARITY_BOTH);

				if (FALSE)
				{
					BufferFunction::WriteBitmap(_T("E:\\2.bmp"), nBCubeWidth, nBCubeHeight, 32, (BYTE*)pCubePool->m_dwBuffer);
					BufferFunction::WriteBitmap(_T("E:\\3.bmp"), nBCubeWidth, nBCubeHeight, 32, (BYTE*)pResultPool->m_dwBuffer);
				}
				SYData_ReleaseCalcBuffer(pCubePool);
			}
			else
				pResultPool = pCubePool;
		}
		if (pResultPool)
		{
			SY_BitBlt(pResultPool->m_dwBuffer,
				rtOutBound.left - rtOutBound.left,
				rtOutBound.top - rtOutBound.top,
				nBorderWidth,
				nBorderHeight,
				rtOutBound.right - rtOutBound.left,
				rtOutBound.bottom - rtOutBound.top,
				pBorderPool->m_dwBuffer,
				0,
				0,
				nBorderWidth,
				nBorderHeight,
				nBorderWidth,
				nBorderHeight,
				BUFFER_MIX,
				BUFFER_PARITY_BOTH);
			SYData_ReleaseCalcBuffer(pBorderPool);
		}
		else
			pResultPool = pBorderPool;

		if (pBorderMaskPool)
		{
			SYData_ReleaseCalcBuffer(pBorderMaskPool);
			pBorderMaskPool = NULL;
		}

	}
	if (pResultPool)
	{
		if (pFaceSystemBuffer)
		{
			SY_BitBlt(pResultPool->m_dwBuffer,
				rtAllFaceBound.left - rtOutBound.left,
				rtAllFaceBound.top - rtOutBound.top,
				nAllFaceWidth,
				nAllFaceHeight,
				rtOutBound.right - rtOutBound.left,
				rtOutBound.bottom - rtOutBound.top,
				pFaceSystemBuffer->m_dwBuffer,
				0,
				0,
				nAllFaceWidth,
				nAllFaceHeight,
				nAllFaceWidth,
				nAllFaceHeight,
				BUFFER_MIX,
				BUFFER_PARITY_BOTH);
			SYData_ReleaseCalcBuffer(pFaceSystemBuffer);
		}
	}
	else
		pResultPool = pFaceSystemBuffer;

	if (FALSE)
	{
		BufferFunction::WriteBitmap(_T("E:\\2.tga"), rtOutBound.right - rtOutBound.left, rtOutBound.bottom - rtOutBound.top, 32, (BYTE*)pResultPool->m_dwBuffer);
	}

	if (pFaceMask)
	{
		SYData_ReleaseCalcBuffer(pFaceMask);
		pFaceMask = NULL;
	}

	int nMaskCount = _SYRender_GetFaceChannel(pDesMaskPool, pMaskPool);
	LPLONG* pContrastTable = NULL, *ppXTable = NULL, *ppYTable = NULL;
	int nResultWidth = rtOutBound.right - rtOutBound.left, nResultHeight = rtOutBound.bottom - rtOutBound.top;
	_SYRender_MakeTable(pDesMaskPool, nMaskCount, pContrastTable, ppXTable, ppYTable, nResultWidth, nResultHeight);

	if (pResultPool && nMaskCount > 0)
	{
		SYRenderLightToOpBuff(
			pResultPool,
			rtOutBound,
			rtOutBound,
			pDesMaskPool,
			nMaskCount,
			ppXTable,
			ppYTable
		);
	}
	if (pContrastTable)
	{
		delete[] pContrastTable;
		pContrastTable = NULL;
	}

	nWidth = rtOutBound.right - rtOutBound.left;
	nHeight = rtOutBound.bottom - rtOutBound.top;
	nPitch = nWidth;
	ptOrg.x = -rtOutBound.left;
	ptOrg.y = -rtOutBound.top;

	for (int i = 0; i < CHANNEL_COUNT; i++)
	{
		if (pMaskPool[i])
		{
			if (pMaskPool[i]->buffer)
			{
				delete[] pMaskPool[i]->buffer;
				pMaskPool[i]->buffer = NULL;
			}
			SAFE_DELETE(pMaskPool[i]);
		}
	}

	if (pMaskPool)
	{
		delete[] pMaskPool;
		pMaskPool = NULL;
	}

	if (pDesMaskPool)
	{
		delete[] pDesMaskPool;
		pDesMaskPool = NULL;
	}

	return 1;
}

static int m_Buffer_Ref = 0;
BASEGRAPH_DATARENDER_API void SYData_ReleaseCalcBuffer(CSYBuffer * pBuffer)
{
	pBuffer->UnQuery();
	m_Buffer_Ref--;
}

BASEGRAPH_DATARENDER_API void SYRender_GenCubeGraph(vectorgraph * des, vectorgraph * src, vector2f * voffset, BaseGraph_DataBase::cubeinfo * pinfo)
{
	figure_cube_extend(des, src, voffset, pinfo->width, pinfo->angle, 0);
}

int  conic_getminpt(point2f* pt, float* t, float* miny, float* minx);

void  _curve_calcvalue(OUT point2f* ptout, point2f* pt, float t);

int  curve_getminpt(point2f* pt, float* t, float* miny, float* minx);

int  cell_get_mindirect(vectorcell*	cell, float* fminy, float* fminx, int* ndirect);

int  figure_analyzedirect(vectorgraph* graph);

float GetCurveDistance(const CSYPoint2fPtrArray* pptrArr)
{
	int nSize = pptrArr->GetSize();
	if (nSize <= 0)
	{
		return 0.0;
	}
	else
	{
		float fDis = 0.0;
		for (int i = 0; i < nSize - 1; i++)
		{
			pointf* ptf1 = pptrArr->GetAt(i);
			pointf* ptf2 = pptrArr->GetAt(i + 1);
			fDis += GetDistace(ptf1, ptf2);
		}
		return fDis;
	}
}

/**
*@brief 短划线
*/
void GetImagPointsDASH(IN const CSYPoint2fPtrArray& pptfArr1, IN const float fLineWidth, OUT int& nCell, OUT CPtfArrArr& pptfArrArr, float *fIntervalAr, int intCount)
{
	nCell = 0;
	int nSize = pptfArr1.GetSize();
	if (nSize <= 0)
	{
		return;
	}
	pointf *pptTemp1, *pptTemp2;

	float fDis = GetCurveDistance(&pptfArr1);


	CSYPoint2fPtrArray* ptfArrTemp = new CSYPoint2fPtrArray;;

	pointf* pptImg = new pointf;
	pptImg->x = (pptfArr1.GetAt(0))->x;
	pptImg->y = (pptfArr1.GetAt(0))->y;
	ptfArrTemp->Add(pptImg);

	pointf startpt = *pptImg;
	int	nStartPt = 0;

	float fDivPre = 0.0;
	float fDivStep = 0.0;

	int nIntervalIndex = 0;
	//间隔
	float fInterval = fIntervalAr[nIntervalIndex];
	BOOL bDash = TRUE;
	while (nStartPt < nSize)
	{
		int nNext = nStartPt + 1;
		pptTemp2 = pptfArr1.GetAt(nNext);
		float fLineLength = GetDistace(&startpt, pptTemp2);

		if (fLineLength + fDivPre >= fInterval)
		{
			fDivStep = (fInterval - fDivPre);
			pointf ptfDir;
			ptfDir = (*pptTemp2 - startpt) / fLineLength;
			startpt = startpt + ptfDir * fDivStep;

			if (!bDash) {
				ptfArrTemp = new CSYPoint2fPtrArray;
			}
			else
				pptfArrArr.Add(ptfArrTemp);

			pptImg = new pointf;
			*pptImg = startpt;
			ptfArrTemp->Add(pptImg);

			if (fLineLength + fDivPre == fInterval)
			{
				nStartPt = nNext;
			}

			nCell += bDash;
			fDivPre = 0;
			bDash = !bDash;
			fDis -= fDivStep;
			nIntervalIndex = (nIntervalIndex + 1) % intCount;
			fInterval = fIntervalAr[nIntervalIndex];
			continue;
		}
		startpt = *pptTemp2;
		fDivPre += fLineLength;
		if (bDash)
		{
			pptImg = new pointf;
			*pptImg = startpt;
			ptfArrTemp->Add(pptImg);
		}
		fDis -= fLineLength;
		nStartPt = nNext;
	}
	if (bDash)
	{
		pptfArrArr.Add(ptfArrTemp);
	}
	nCell += bDash;
}

/**
*@brief 划线-点
*/
void GetImagPointsDASHDOT(IN const CSYPoint2fPtrArray& pptfArr1, IN const float fLineWidth, OUT int& nCell, OUT CPtfArrArr& pptfArrArr)
{
	int nSize;
	nSize = pptfArr1.GetSize();
	if (nSize <= 0)
	{
		nCell = 0;
		return;
	}
	pointf *pptTemp1 = nullptr;
	pointf *pptTemp2 = nullptr;

	float fDis = GetCurveDistance(&pptfArr1);
	nCell = 0;

	CSYPoint2fPtrArray* ptfArrTemp = nullptr;
	pointf* pptImg = new pointf;
	pptImg->x = (pptfArr1.GetAt(0))->x;
	pptImg->y = (pptfArr1.GetAt(0))->y;

	nSize = pptfArr1.GetSize();

	float fDivSum = 0.0;
	float fDivPre = 0.0;
	float fDivStep = 0.0;

	float fInterval1 = 4.0f * fLineWidth;
	float fInterval2 = 2.0f * fLineWidth;
	float fInterval3 = 1.0f * fLineWidth;
	float fInterval = fInterval1;
	float fIntervalPre = fInterval1;
	int nCount = 0;

	int nFlag = 0;
	while (fDis > fInterval)
	{
		if (abs(fIntervalPre - fInterval1) < ABS_FLOAT_0 || abs(fIntervalPre - fInterval3) < ABS_FLOAT_0)
		{
			ptfArrTemp = new CSYPoint2fPtrArray;
			ptfArrTemp->Add(pptImg);
		}

		for (int i = 0; i < nSize - 1; i++)
		{
			pptTemp1 = pptfArr1.GetAt(i);
			pptTemp2 = pptfArr1.GetAt(i + 1);
			fDivPre = fDivSum;
			fDivSum += GetDistace(pptTemp2, pptTemp1);
			if (fDivSum >= fInterval)
			{
				fDivSum = 0.0;
				nFlag = i;
				break;
			}

			if ((abs(fIntervalPre - fInterval1) < ABS_FLOAT_0 || abs(fIntervalPre - fInterval3) < ABS_FLOAT_0) && (i + 1) > nFlag)
			{
				pptImg = new pointf;
				pptImg->x = pptTemp2->x;
				pptImg->y = pptTemp2->y;
				ptfArrTemp->Add(pptImg);
			}
		}
		fDivStep = (fInterval - fDivPre);
		pointf ptfDir;
		ptfDir.x = (pptTemp2->x - pptTemp1->x) / GetDistace(pptTemp2, pptTemp1);
		ptfDir.y = (pptTemp2->y - pptTemp1->y) / GetDistace(pptTemp2, pptTemp1);
		pptImg = new pointf;
		pptImg->x = pptTemp1->x + fDivStep * ptfDir.x;
		pptImg->y = pptTemp1->y + fDivStep * ptfDir.y;

		if (abs(fIntervalPre - fInterval1) < ABS_FLOAT_0 || abs(fIntervalPre - fInterval3) < ABS_FLOAT_0)
		{
			ptfArrTemp->Add(pptImg);
			pptfArrArr.Add(ptfArrTemp);
		}

		fDivPre = 0.0;

		if (abs(fIntervalPre - fInterval1) < ABS_FLOAT_0)
		{
			nCell++;
			fInterval += fInterval2;
			fIntervalPre = fInterval2;
		}
		else if (abs(fIntervalPre - fInterval2) < ABS_FLOAT_0)
		{
			nCount++;
			if (nCount % 2 == 0)
			{
				fInterval += fInterval1;
				fIntervalPre = fInterval1;
			}
			else
			{
				fInterval += fInterval3;
				fIntervalPre = fInterval3;
			}
		}
		else if (abs(fIntervalPre - fInterval3) < ABS_FLOAT_0)
		{
			nCell++;
			fInterval += fInterval2;
			fIntervalPre = fInterval2;
		}
	}

	// 添加最后一段小线段，长度可能小于等分长度
	nCell++;

	ptfArrTemp = new CSYPoint2fPtrArray;
	pointf* ptTemp = new pointf;
	ptTemp->x = pptImg->x;
	ptTemp->y = pptImg->y;
	ptfArrTemp->Add(ptTemp);

	pptImg = new pointf;
	pptImg->x = ((pointf*)pptfArr1.GetAt(nSize - 1))->x;
	pptImg->y = ((pointf*)pptfArr1.GetAt(nSize - 1))->y;
	ptfArrTemp->Add(pptImg);

	pptfArrArr.Add(ptfArrTemp);
}

/**
*@brief 长划线
*/
void GetImagPointsLDASH(IN const CSYPoint2fPtrArray& pptfArr1, IN const float fLineWidth, OUT int& nCell, OUT CPtfArrArr& pptfArrArr)
{
	int nSize;
	nSize = pptfArr1.GetSize();
	if (nSize <= 0)
	{
		nCell = 0;
		return;
	}
	pointf *pptTemp1 = nullptr;
	pointf *pptTemp2 = nullptr;

	float fDis = GetCurveDistance(&pptfArr1);
	nCell = 0;

	CSYPoint2fPtrArray* ptfArrTemp = nullptr;
	pointf* pptImg = new pointf;
	pptImg->x = (pptfArr1.GetAt(0))->x;
	pptImg->y = (pptfArr1.GetAt(0))->y;

	nSize = pptfArr1.GetSize();

	float fDivSum = 0.0;
	float fDivPre = 0.0;
	float fDivStep = 0.0;

	float fInterval = 8.0f * fLineWidth;
	int nCount = 0;

	int nFlag = 0;
	while (fDis > fInterval)
	{
		if (nCount % 2 == 0)
		{
			ptfArrTemp = new CSYPoint2fPtrArray;
			ptfArrTemp->Add(pptImg);
		}

		for (int i = 0; i < nSize - 1; i++)
		{
			pptTemp1 = pptfArr1.GetAt(i);
			pptTemp2 = pptfArr1.GetAt(i + 1);
			fDivPre = fDivSum;
			fDivSum += GetDistace(pptTemp2, pptTemp1);
			if (fDivSum >= fInterval)
			{
				fDivSum = 0.0;
				nFlag = i;
				break;
			}

			if (nCount % 2 == 0 && (i + 1) > nFlag)
			{
				pptImg = new pointf;
				pptImg->x = pptTemp2->x;
				pptImg->y = pptTemp2->y;
				ptfArrTemp->Add(pptImg);
			}
		}
		fDivStep = (fInterval - fDivPre);
		pointf ptfDir;
		ptfDir.x = (pptTemp2->x - pptTemp1->x) / GetDistace(pptTemp2, pptTemp1);
		ptfDir.y = (pptTemp2->y - pptTemp1->y) / GetDistace(pptTemp2, pptTemp1);

		pptImg = new pointf;
		pptImg->x = pptTemp1->x + fDivStep * ptfDir.x;
		pptImg->y = pptTemp1->y + fDivStep * ptfDir.y;
		if (nCount % 2 == 0)
		{
			ptfArrTemp->Add(pptImg);
		}

		fDivPre = 0.0;

		if (nCount % 2 == 0)
		{
			pptfArrArr.Add(ptfArrTemp);
		}

		// 跨过2个像素间隔
		nCount++;

		if (nCount % 2 == 0)
		{
			fInterval += 8.0f * fLineWidth;
			nCell++;
		}
		else
		{
			fInterval += 2.0f * fLineWidth;
		}
	}

	// 添加最后一段小线段，长度可能小于等分长度
	nCell++;

	ptfArrTemp = new CSYPoint2fPtrArray;
	pointf* ptTemp = new pointf;
	ptTemp->x = pptImg->x;
	ptTemp->y = pptImg->y;
	ptfArrTemp->Add(ptTemp);

	pptImg = new pointf;
	pptImg->x = ((pointf*)pptfArr1.GetAt(nSize - 1))->x;
	pptImg->y = ((pointf*)pptfArr1.GetAt(nSize - 1))->y;
	ptfArrTemp->Add(pptImg);

	pptfArrArr.Add(ptfArrTemp);
}

/**
*@brief 长划线-点
*/
void GetImagPointsLDASHDOT(IN const CSYPoint2fPtrArray& pptfArr1, IN const float fLineWidth, OUT int& nCell, OUT CPtfArrArr& pptfArrArr)
{
	int nSize;
	nSize = pptfArr1.GetSize();
	if (nSize <= 0)
	{
		nCell = 0;
		return;
	}
	pointf *pptTemp1 = nullptr;
	pointf *pptTemp2 = nullptr;

	float fDis = GetCurveDistance(&pptfArr1);
	nCell = 0;

	CSYPoint2fPtrArray* ptfArrTemp = nullptr;
	pointf* pptImg = new pointf;
	pptImg->x = (pptfArr1.GetAt(0))->x;
	pptImg->y = (pptfArr1.GetAt(0))->y;

	nSize = pptfArr1.GetSize();

	float fDivSum = 0.0;
	float fDivPre = 0.0;
	float fDivStep = 0.0;

	float fInterval1 = 8.0f * fLineWidth;
	float fInterval2 = 2.0f * fLineWidth;
	float fInterval3 = 1.0f * fLineWidth;
	float fInterval = fInterval1;
	float fIntervalPre = fInterval1;
	int nCount = 0;

	int nFlag = 0;
	while (fDis > fInterval)
	{
		if (abs(fIntervalPre - fInterval1) < ABS_FLOAT_0 || abs(fIntervalPre - fInterval3) < ABS_FLOAT_0)
		{
			ptfArrTemp = new CSYPoint2fPtrArray;
			ptfArrTemp->Add(pptImg);
		}

		for (int i = 0; i < nSize - 1; i++)
		{
			pptTemp1 = pptfArr1.GetAt(i);
			pptTemp2 = pptfArr1.GetAt(i + 1);
			fDivPre = fDivSum;
			fDivSum += GetDistace(pptTemp2, pptTemp1);
			if (fDivSum >= fInterval)
			{
				fDivSum = 0.0;
				nFlag = i;
				break;
			}

			if ((abs(fIntervalPre - fInterval1) < ABS_FLOAT_0 || abs(fIntervalPre - fInterval3) < ABS_FLOAT_0) && (i + 1) > nFlag)
			{
				pptImg = new pointf;
				pptImg->x = pptTemp2->x;
				pptImg->y = pptTemp2->y;
				ptfArrTemp->Add(pptImg);
			}
		}
		fDivStep = (fInterval - fDivPre);
		pointf ptfDir;
		ptfDir.x = (pptTemp2->x - pptTemp1->x) / GetDistace(pptTemp2, pptTemp1);
		ptfDir.y = (pptTemp2->y - pptTemp1->y) / GetDistace(pptTemp2, pptTemp1);
		pptImg = new pointf;
		pptImg->x = pptTemp1->x + fDivStep * ptfDir.x;
		pptImg->y = pptTemp1->y + fDivStep * ptfDir.y;

		if (abs(fIntervalPre - fInterval1) < ABS_FLOAT_0 || abs(fIntervalPre - fInterval3) < ABS_FLOAT_0)
		{
			ptfArrTemp->Add(pptImg);
			pptfArrArr.Add(ptfArrTemp);
		}

		fDivPre = 0.0;

		if (abs(fIntervalPre - fInterval1) < ABS_FLOAT_0)
		{
			nCell++;
			fInterval += fInterval2;
			fIntervalPre = fInterval2;
		}
		else if (abs(fIntervalPre - fInterval2) < ABS_FLOAT_0)
		{
			nCount++;
			if (nCount % 2 == 0)
			{
				fInterval += fInterval1;
				fIntervalPre = fInterval1;
			}
			else
			{
				fInterval += fInterval3;
				fIntervalPre = fInterval3;
			}
		}
		else if (abs(fIntervalPre - fInterval3) < ABS_FLOAT_0)
		{
			nCell++;
			fInterval += fInterval2;
			fIntervalPre = fInterval2;
		}
	}

	// 添加最后一段小线段，长度可能小于等分长度
	nCell++;

	ptfArrTemp = new CSYPoint2fPtrArray;
	pointf* ptTemp = new pointf;
	ptTemp->x = pptImg->x;
	ptTemp->y = pptImg->y;
	ptfArrTemp->Add(ptTemp);

	pptImg = new pointf;
	pptImg->x = ((pointf*)pptfArr1.GetAt(nSize - 1))->x;
	pptImg->y = ((pointf*)pptfArr1.GetAt(nSize - 1))->y;
	ptfArrTemp->Add(pptImg);

	pptfArrArr.Add(ptfArrTemp);
}

void GetPointsRIPPLE(IN const CSYPoint2fPtrArray& pptfArr1, IN const float fLineWidth, OUT int& nCell, OUT CPtfArrArr& pptfArrArr)
{
	int nSize;
	nSize = pptfArr1.GetSize();
	if (nSize <= 0)
	{
		nCell = 0;
		return;
	}
	int i;
	pointf *pptTemp1 = nullptr;
	pointf *pptTemp2 = nullptr;

	float fDis = GetCurveDistance(&pptfArr1);
	nCell = 1;

	float fDivSum = 0.0;
	float fDivPre = 0.0;
	float fDivStep = 0.0;

	float fInterval1 = 8.0f * fLineWidth;

	float fInterval = fInterval1;
	int nCount = 0;
	int nFlag = 0;

	CPtfArrArr ptArrArrTemp;

	CSYPoint2fPtrArray* ptfArrTemp;
	pointf* pptImg = NULL;
	pointf pptImagTemp;
	pptImagTemp.x = (pptfArr1.GetAt(0))->x;
	pptImagTemp.y = (pptfArr1.GetAt(0))->y;

	// 切分Bezier曲线
	while (fDis > fInterval)
	{
		ptfArrTemp = new CSYPoint2fPtrArray;
		pointf* ptTemp3 = new pointf;
		ptTemp3->x = pptImagTemp.x;
		ptTemp3->y = pptImagTemp.y;
		ptfArrTemp->Add(ptTemp3);

		for (int i = 0; i < nSize - 1; i++)
		{
			pptTemp1 = pptfArr1.GetAt(i);
			pptTemp2 = pptfArr1.GetAt(i + 1);
			fDivPre = fDivSum;
			fDivSum += GetDistace(pptTemp2, pptTemp1);
			if (fDivSum >= fInterval)
			{
				fDivSum = 0.0;
				nFlag = i;
				break;
			}

			if ((i + 1) > nFlag)
			{
				pptImg = new pointf;
				pptImg->x = pptTemp2->x;
				pptImg->y = pptTemp2->y;
				pptImagTemp.x = pptImg->x;
				pptImagTemp.y = pptImg->y;
				ptfArrTemp->Add(pptImg);
			}
		}
		fDivStep = (fInterval - fDivPre);
		pointf ptfDir;
		ptfDir.x = (pptTemp2->x - pptTemp1->x) / GetDistace(pptTemp2, pptTemp1);
		ptfDir.y = (pptTemp2->y - pptTemp1->y) / GetDistace(pptTemp2, pptTemp1);
		pptImg = new pointf;
		pptImg->x = pptTemp1->x + fDivStep * ptfDir.x;
		pptImg->y = pptTemp1->y + fDivStep * ptfDir.y;
		pptImagTemp.x = pptImg->x;
		pptImagTemp.y = pptImg->y;
		ptfArrTemp->Add(pptImg);

		ptArrArrTemp.Add(ptfArrTemp);

		fDivPre = 0.0;

		fInterval += fInterval1;
	}

	ptfArrTemp = new CSYPoint2fPtrArray;
	pointf* ptTemp = new pointf;
	ptTemp->x = pptImagTemp.x;
	ptTemp->y = pptImagTemp.y;
	ptfArrTemp->Add(ptTemp);

	pptImg = new pointf;
	pptImg->x = ((pointf*)pptfArr1.GetAt(nSize - 1))->x;
	pptImg->y = ((pointf*)pptfArr1.GetAt(nSize - 1))->y;
	ptfArrTemp->Add(pptImg);

	ptArrArrTemp.Add(ptfArrTemp);

	// 生成波浪线
	nSize = ptArrArrTemp.GetSize();
	pointf ptfStart, ptfEnd;
	pointf ptf;
	pointf ptfBase;
	float nHeight = 4.0f * fLineWidth; // 振幅
	CSYPoint2fPtrArray* ptfArrRipple = new CSYPoint2fPtrArray;

	if (nSize > 0)
	{
		pptTemp1 = new pointf;
		pptTemp1->x = ptArrArrTemp.GetAt(0)->GetAt(0)->x;
		pptTemp1->y = ptArrArrTemp.GetAt(0)->GetAt(0)->y;
		ptfArrRipple->Add(pptTemp1);
	}

	for (i = 0; i < nSize; i++)
	{
		ptfArrTemp = ptArrArrTemp.GetAt(i);
		int nCount = ptfArrTemp->GetSize();
		pointf* ptf1 = (pointf*)ptfArrTemp->GetAt(0);
		pointf* ptf2 = (pointf*)ptfArrTemp->GetAt(nCount - 1);
		float fDisTemp = GetDistace(ptf1, ptf2);

		if (abs(fDisTemp - fInterval1) < ABS_FLOAT_0)
		{
			ptfStart.x = ptfArrTemp->GetAt(0)->x;
			ptfStart.y = ptfArrTemp->GetAt(0)->y;

			ptfEnd.x = ptfArrTemp->GetAt(nCount - 1)->x;
			ptfEnd.y = ptfArrTemp->GetAt(nCount - 1)->y;

			float r = (float)sqrt((double)((ptfEnd.x - ptfStart.x)*(ptfEnd.x - ptfStart.x)
				+ (ptfStart.y - ptfEnd.y)*(ptfStart.y - ptfEnd.y)));
			if (r < 0.000001 && r>-0.000001)
			{
				continue;
			}
			ptf.x = (ptfStart.y - ptfEnd.y) / r;
			ptf.y = (ptfEnd.x - ptfStart.x) / r;

			ptfBase.x = ptfStart.x + 0.25f * (ptfEnd.x - ptfStart.x);
			ptfBase.y = ptfStart.y + 0.25f * (ptfEnd.y - ptfStart.y);

			pptTemp1 = new pointf;
			pptTemp1->x = ptfBase.x + nHeight * ptf.x;
			pptTemp1->y = ptfBase.y + nHeight * ptf.y;
			ptfArrRipple->Add(pptTemp1);

			ptfBase.x = ptfStart.x + 0.75f * (ptfEnd.x - ptfStart.x);
			ptfBase.y = ptfStart.y + 0.75f * (ptfEnd.y - ptfStart.y);

			pptTemp1 = new pointf;
			pptTemp1->x = ptfBase.x - nHeight * ptf.x;
			pptTemp1->y = ptfBase.y - nHeight * ptf.y;
			ptfArrRipple->Add(pptTemp1);

			ptfBase.x = ptfStart.x + 1.0f * (ptfEnd.x - ptfStart.x);
			ptfBase.y = ptfStart.y + 1.0f * (ptfEnd.y - ptfStart.y);

			pptTemp1 = new pointf;
			pptTemp1->x = ptfBase.x;
			pptTemp1->y = ptfBase.y;
			ptfArrRipple->Add(pptTemp1);
		}
		else
		{
			for (int k = 0; k < nCount - 1; k++)
			{
				ptfStart.x = ptfArrTemp->GetAt(k)->x;
				ptfStart.y = ptfArrTemp->GetAt(k)->y;

				ptfEnd.x = ptfArrTemp->GetAt(k + 1)->x;
				ptfEnd.y = ptfArrTemp->GetAt(k + 1)->y;

				float r = (float)sqrt((double)((ptfEnd.x - ptfStart.x)*(ptfEnd.x - ptfStart.x)
					+ (ptfStart.y - ptfEnd.y)*(ptfStart.y - ptfEnd.y)));
				if (r < 0.000001 && r>-0.000001)
				{
					continue;
				}
				ptf.x = (ptfStart.y - ptfEnd.y) / r;
				ptf.y = (ptfEnd.x - ptfStart.x) / r;

				ptfBase.x = ptfStart.x + 0.25f * (ptfEnd.x - ptfStart.x);
				ptfBase.y = ptfStart.y + 0.25f * (ptfEnd.y - ptfStart.y);

				pptTemp1 = new pointf;
				pptTemp1->x = ptfBase.x + nHeight * ptf.x;
				pptTemp1->y = ptfBase.y + nHeight * ptf.y;
				ptfArrRipple->Add(pptTemp1);

				ptfBase.x = ptfStart.x + 0.75f * (ptfEnd.x - ptfStart.x);
				ptfBase.y = ptfStart.y + 0.75f * (ptfEnd.y - ptfStart.y);

				pptTemp1 = new pointf;
				pptTemp1->x = ptfBase.x - nHeight * ptf.x;
				pptTemp1->y = ptfBase.y - nHeight * ptf.y;
				ptfArrRipple->Add(pptTemp1);

				ptfBase.x = ptfStart.x + 1.0f * (ptfEnd.x - ptfStart.x);
				ptfBase.y = ptfStart.y + 1.0f * (ptfEnd.y - ptfStart.y);

				pptTemp1 = new pointf;
				pptTemp1->x = ptfBase.x;
				pptTemp1->y = ptfBase.y;
				ptfArrRipple->Add(pptTemp1);
			}
		}
	}

	pptfArrArr.Add(ptfArrRipple);

	// 释放资源
	nSize = ptArrArrTemp.GetSize();
	for (int i = 0; i < nSize; i++)
	{
		CSYPoint2fPtrArray* ptArr = (CSYPoint2fPtrArray*)ptArrArrTemp.GetAt(i);
		int nTemp = ptArr->GetSize();
		for (int k = 0; k < nTemp; k++)
		{
			pointf* ptTemp = (pointf*)ptArr->GetAt(k);
			delete ptTemp;
			ptTemp = NULL;
		}
		ptArr->RemoveAll();

		delete ptArr;
		ptArr = NULL;
	}
	ptArrArrTemp.RemoveAll();
}

void GetImagPointsByStyle(IN const CSYPoint2fPtrArray& pptfArr1, IN const int nStyle, IN const float fLineWidth, IN const float fLineInterval, OUT int& nCell, OUT CPtfArrArr& pptfArrArr)
{
	switch (nStyle)
	{
	case SYLINE_DASH:
	{
		float fIntervalAr[] = { fLineInterval * 4.0f,fLineInterval * 2.0f };
		GetImagPointsDASH(pptfArr1, fLineWidth, nCell, pptfArrArr, fIntervalAr, 2);
	}
	break;
	case SYLINE_DASHDOT:
	{
		float fIntervalAr[] = { fLineInterval * 4.0f,fLineInterval * 2.0f,fLineInterval * 1.0f,fLineInterval * 2.0f };
		GetImagPointsDASH(pptfArr1, fLineWidth, nCell, pptfArrArr, fIntervalAr, 4);
		//GetImagPointsDASHDOT(pptfArr1, fLineWidth, nCell, pptfArrArr);
	}
	break;
	case SYLINE_LDASH:
	{
		float fIntervalAr[] = { fLineInterval * 8.0f,fLineInterval * 2.0f };
		GetImagPointsDASH(pptfArr1, fLineWidth, nCell, pptfArrArr, fIntervalAr, 2);
		//GetImagPointsLDASH(pptfArr1, fLineWidth, nCell, pptfArrArr);
	}
	break;
	case SYLINE_LDASHDOT:
	{
		float fIntervalAr[] = { fLineInterval * 8.0f,fLineInterval * 2.0f,fLineInterval * 1.0f,fLineInterval * 2.0f };
		GetImagPointsDASH(pptfArr1, fLineWidth, nCell, pptfArrArr, fIntervalAr, 4);
		//GetImagPointsLDASH(pptfArr1, fLineWidth, nCell, pptfArrArr);
	}
	//GetImagPointsLDASHDOT(pptfArr1, fLineWidth, nCell, pptfArrArr);
	break;
	case SYLINE_RIPPLE:
		GetPointsRIPPLE(pptfArr1, fLineWidth, nCell, pptfArrArr);
		break;
	default:
		break;
	}
}

// 获取虚线描边
void GetImageBorderGraph(vectorgraph* des, vectorgraph* src, vector2f* vOffset, float fwidth, float finterval, int nLineStyle)
{
	CSYPoint2fPtrArray pptfCurvePts;
	vectorcell **pCell = new vectorcell*[src->ncell];
	int* numCell = new int[src->ncell];

	int ndirect = figure_analyzedirect(src);
	for (int i = 0; i < src->ncell; i++)
	{
		//改变算法支持园逼近，HTH
		vectorcell celltemp1;
		vectorcell celltemp2;
		cell_line_extend_close(
			&celltemp1,
			&celltemp2,
			&src->cell[i],
			vOffset,
			ndirect,
			src->cell[i].width);
		for (int k = 0; k < celltemp1.npoint; k++)
		{
			pointf* ptfTemp = new pointf;
			ptfTemp->x = celltemp1.point[k].x;
			ptfTemp->y = celltemp1.point[k].y;
			pptfCurvePts.Add(ptfTemp);
		}
		cell_release(&celltemp1);
		cell_release(&celltemp2);
		////<<<<<<<<<<<<<<<<<<<<<<
		// 根据类型获取Cell中点
		vectorcell * pCurveCell;
		CPtfArrArr pptfArrArrTemp;
		int ptType;
		GetImagPointsByStyle(pptfCurvePts, nLineStyle, fwidth, finterval, numCell[i], pptfArrArrTemp);
		int nSize = pptfArrArrTemp.GetSize();
		if (nSize == 1)
			int a = 0;
		pCell[i] = new vectorcell[nSize];
		for (int m = 0; m < nSize; m++)
		{
			pCurveCell = &pCell[i][m];
			pCurveCell->type = uncloss_line;
			pCurveCell->width = fwidth;
			pCurveCell->bound.left = 65535.0f;
			pCurveCell->bound.top = 65535.0f;
			pCurveCell->bound.right = -65535.0f;
			pCurveCell->bound.bottom = -65535.0f;

			CSYPoint2fPtrArray* ptfArrTemp = (CSYPoint2fPtrArray*)pptfArrArrTemp.GetAt(m);

			pCurveCell->npoint = ptfArrTemp->GetSize();
			if (nLineStyle == SYLINE_RIPPLE)
			{
				ptType = point_curve;
			}
			else
			{
				ptType = point_line;
			}

			pCurveCell->point = new pointf[pCurveCell->npoint];
			pCurveCell->pttype = new unsigned long[pCurveCell->npoint];

			for (int n = 0; n < ptfArrTemp->GetSize(); n++)
			{
				pointf* pPtf1 = (pointf*)ptfArrTemp->GetAt(n);
				pCurveCell->point[n] = *pPtf1;
				pCurveCell->pttype[n] = ptType;
			}
			// 计算曲线外包矩形
			for (int k = 0; k < pCurveCell->npoint; k++)
			{
				if (pCurveCell->type == close_line || pCurveCell->type == uncloss_line)
				{
					pCurveCell->bound.left = min(pCurveCell->point[k].x - pCurveCell->width, pCurveCell->bound.left);
					pCurveCell->bound.top = min(pCurveCell->point[k].y - pCurveCell->width, pCurveCell->bound.top);
					pCurveCell->bound.right = max(pCurveCell->point[k].x + pCurveCell->width, pCurveCell->bound.right);
					pCurveCell->bound.bottom = max(pCurveCell->point[k].y + pCurveCell->width, pCurveCell->bound.bottom);
				}
				else
				{
					pCurveCell->bound.left = min(pCurveCell->point[k].x, pCurveCell->bound.left);
					pCurveCell->bound.top = min(pCurveCell->point[k].y, pCurveCell->bound.top);
					pCurveCell->bound.right = max(pCurveCell->point[k].x, pCurveCell->bound.right);
					pCurveCell->bound.bottom = max(pCurveCell->point[k].y, pCurveCell->bound.bottom);
				}
			}
		}
		// 释放资源
		nSize = pptfArrArrTemp.GetSize();
		for (int i = 0; i < nSize; i++)
		{
			CSYPoint2fPtrArray* ptArr = (CSYPoint2fPtrArray*)pptfArrArrTemp.GetAt(i);
			int nTemp = ptArr->GetSize();
			for (int k = 0; k < nTemp; k++)
			{
				pointf* ptTemp = (pointf*)ptArr->GetAt(k);
				delete ptTemp;
				ptTemp = NULL;
			}
			ptArr->RemoveAll();

			delete ptArr;
			ptArr = NULL;
		}
		pptfArrArrTemp.RemoveAll();

		nSize = pptfCurvePts.GetSize();
		for (int i = 0; i < nSize; i++)
		{
			pointf* ptff1 = pptfCurvePts.GetAt(i);
			delete ptff1;
			ptff1 = NULL;
		}
		pptfCurvePts.RemoveAll();
	}
	// 填充graph
	for (int i = 0; i < src->ncell; i++)
	{
		des->ncell += numCell[i];
	}
	des->cell = new vectorcell[des->ncell];
	des->bound.left = 65535.0f;
	des->bound.top = 65535.0f;
	des->bound.right = -65535.0f;
	des->bound.bottom = -65535.0f;

	int numPre = 0;
	for (int i = 0; i < src->ncell; i++)
	{
		if (i > 0)
		{
			numPre += numCell[i - 1];
		}
		for (int k = 0; k < numCell[i]; k++)
		{
			vectorcell* pCurveCell1 = &des->cell[numPre + k];
			if (des->ncell <= (numPre + k))
			{
				ASSERT(0);
			}
			vectorcell pCurveCell2 = pCell[i][k];
			CopyCell(pCurveCell1, &pCurveCell2);
			delete[] pCurveCell2.point;
			pCurveCell2.point = NULL;
			delete[] pCurveCell2.pttype;
			pCurveCell2.pttype = NULL;
			des->bound.left = min(des->bound.left, pCurveCell2.bound.left);
			des->bound.top = min(des->bound.top, pCurveCell2.bound.top);
			des->bound.right = max(des->bound.right, pCurveCell2.bound.right);
			des->bound.bottom = max(des->bound.bottom, pCurveCell2.bound.bottom);
		}

		delete[] pCell[i];
		pCell[i] = NULL;
	}

	delete[] pCell;
	pCell = NULL;

	delete[] numCell;
	numCell = NULL;
}

BASEGRAPH_DATARENDER_API void SYRender_GenBorderGraph(vectorgraph * des, vectorgraph * src, vector2f * voffset, float fwidth, float finterval, int nLineStyle, BOOL bSingleLine)
{
	////////////////////////////////////////////////////////////////////////// begin
	// 根据描边类型生成Graph
	if (nLineStyle == SYLINE_SOLID || abs(finterval) <= ABS_FLOAT_0)
	{
		if (bSingleLine)
			figure_extend_Single(des, src, voffset, fwidth, 0, FALSE);
		else
			figure_extend(des, src, voffset, fwidth, 0, FALSE);
	}
	else
	{
		GetImageBorderGraph(des, src, voffset, fwidth, finterval, nLineStyle);
	}
}

BASEGRAPH_DATARENDER_API CSYBuffer * SYData_QueryCalcBuffer(int nSize, BOOL bMustSuccess)
{
	int nCalcCount = (int)SYAR_GetSize(calc_midderbuf_array);
start:
	if (nSize == 0)
	{
		for (int k = 0; k < nCalcCount; k++)
		{
			CSYBuffer* pBuffer = (CSYBuffer*)SYAR_GetAt(calc_midderbuf_array, k);
			pBuffer->Lock();
			if (!pBuffer->IsUsed())
			{
				if (pBuffer->m_nMaxSize < maxsize)
				{
					pBuffer->UnLock();
					break;
					//return 0;
				}
				else
				{
					pBuffer->Query();
					m_Buffer_Ref++;
					pBuffer->UnLock();
					return pBuffer;
				}
			}

			pBuffer->UnLock();
		}

		if (bMustSuccess)
		{
			CSYBuffer* pBuffer = new CSYBuffer;
			pBuffer->Create(maxsize);
			pBuffer->Query();
			m_Buffer_Ref++;
			pBuffer->SetAutoDelete(TRUE);
			return pBuffer;
		}
		return 0;
	}
	for (int k = nCalcCount - 1; k >= 0; k--)
	{
		CSYBuffer* pBuffer = (CSYBuffer*)SYAR_GetAt(calc_midderbuf_array, k);
		pBuffer->Lock();
		if (!pBuffer->IsUsed())
		{
			if (pBuffer->m_nMaxSize >= nSize)
			{
				pBuffer->Query();
				m_Buffer_Ref++;
				pBuffer->UnLock();
				return pBuffer;
			}
		}
		pBuffer->UnLock();
	}
	if (bMustSuccess)
	{
		CSYBuffer* pBuffer = new CSYBuffer;
		pBuffer->Create(nSize);
		pBuffer->Query();
		pBuffer->SetAutoDelete(TRUE);
		m_Buffer_Ref++;
		return pBuffer;
		goto start;
	}
	return NULL;
}

BASEGRAPH_DATARENDER_API BOOL SYRender_DrawGraph(vectorgraph * pGraph, DWORD * pBuffer, int w, int h, float fscale, RECT rtBorderBound, sycolor * pcolor, DWORD effect_type, DWORD effect_width, DWORD effect_soft, float effect_angle, DWORD effect_color, int nframe, CSYBuffer ** pRetMaskPool, int * nRetMaskWidth, int * nRetMaskHeight)
{
	//LARGE_INTEGER t_freqency;
	//LARGE_INTEGER t_start;
	//LARGE_INTEGER t_end;
	//QueryPerformanceFrequency(&t_freqency);

	pointf ptOrg;
	ptOrg.x = (float)-rtBorderBound.left;
	ptOrg.y = (float)-rtBorderBound.top;

	//掩模信息
	CSYBuffer* pMaskPool = SYData_QueryCalcBuffer((w * h >> 2) + 1);
	BYTE* pMask = (BYTE*)pMaskPool->m_dwBuffer;
	memset(pMask, 0, w * h);

	//光栅化掩模
	CSYBuffer* pfigurePool = SYData_QueryCalcBuffer(w*h, TRUE);
	//vectorgraphfix Graph_Fix;
	//rip_transfigure_to_fix(pGraph,&Graph_Fix);
	//raster_render_figure(
	//	&Graph_Fix,		//图形结构
	//	pMask,8,w,h,w,fscale,NULL,(unsigned char*)pfigurePool->m_dwBuffer,pfigurePool->m_nSize * 4);

	//QueryPerformanceCounter(&t_start);
	rip_figure_mask(pGraph, pMask, w, h, fscale, &ptOrg, (unsigned char*)pfigurePool->m_dwBuffer, pfigurePool->m_nMaxSize * 4);
	int ncount = w * h;
	for (int i = 0; i < ncount; i++)
	{
		pfigurePool->m_dwBuffer[i] = pMask[i] | (pMask[i] << 8) | (pMask[i] << 16) | 0xff;
	}
	if (FALSE)
	{
		BufferFunction::WriteBitmap(_T("E:\\2.bmp"), w, h, 32, (BYTE*)pfigurePool->m_dwBuffer);
	}

	SYData_ReleaseCalcBuffer(pfigurePool);

	//如果颜色存在，就渲染颜色
	if (pcolor)
	{
		SYColor_FillColor(pcolor, pBuffer, w, h, 0, 0, w, h, fscale, nframe);
	}

	render_merge_bufalpha(pBuffer, pMask, w, h);
	if (FALSE)
	{
		BufferFunction::WriteBitmap(_T("E:\\2.bmp"), w, h, 32, (BYTE*)pfigurePool->m_dwBuffer);
		BufferFunction::WriteBitmap(_T("E:\\3.bmp"), w, h, 32, (BYTE*)pBuffer);
	}

	//凹凸效果
	if (effect_type != STATCEFFECT_TYPE_NONE
		&& effect_width > 0)
	{
		effect_convex(pMask,
			w,
			pBuffer,
			w,
			h,
			effect_width*fscale,
			effect_soft,
			effect_type,
			effect_angle,
			effect_color);
	}
	if (!pRetMaskPool)
		SYData_ReleaseCalcBuffer(pMaskPool);
	else
	{
		*pRetMaskPool = pMaskPool;
		*nRetMaskWidth = w;
		*nRetMaskHeight = h;
	}
	return 1;
}

void OCT(LPBYTE lpSrc, LPBYTE lpDes, POINT PtOffset, SIZE& SzSrc, SIZE& SzDes)
{
	memset(lpDes, 0, SzDes.cx* SzDes.cy);
	LPBYTE lpSrcL = lpSrc;
	LPBYTE lpDesL = lpDes + PtOffset.y * SzDes.cx + PtOffset.x;
	int nCopyY = SzSrc.cy;
	int nCopyX = SzSrc.cx;
	for (int i = 0; i <nCopyY; i++)
	{
		memcpy(lpDesL, lpSrcL, nCopyX);
		lpSrcL += SzSrc.cx;
		lpDesL += SzDes.cx;
	}
}

int type = 0;
float V_0 = 41;
float V_sqrt2 = V_0*sqrtf(2);
float V_sqrt5 = V_0*sqrtf(5);
float setV1(int x, int y, float map[], int width, int offset, int xmax, int ymax)
{
	float min, v;
	int r1, r2, r3, r4, r5, r0, r6;

	r1 = offset - width - width - 2;
	r0 = r1 - width;
	r2 = r1 + width;
	r3 = r2 + width;
	r4 = r3 + width;
	r5 = r4 + width;
	r6 = r5 + width;

	if (y == 0 || x == 0 || y == ymax + 2 || x == xmax + 2)
		return map[offset] = V_0;

	v = map[r2 + 2] + V_0;
	min = v;

	v = map[r3 + 1] + V_0;
	if (v < min)
		min = v;

	v = map[r3 + 3] + V_0;
	if (v < min)
		min = v;

	v = map[r4 + 2] + V_0;
	if (v < min)
		min = v;

	v = map[r2 + 1] + V_sqrt2;
	if (v < min)
		min = v;

	v = map[r2 + 3] + V_sqrt2;
	if (v < min)
		min = v;

	v = map[r4 + 1] + V_sqrt2;
	if (v < min)
		min = v;

	v = map[r4 + 3] + V_sqrt2;
	if (v < min)
		min = v;

	if (y == 1 || x == 1 || y == ymax + 1 || x == xmax + 1)
		return map[offset] = min;

	v = map[r1 + 1] + V_sqrt5;
	if (v < min)
		min = v;

	v = map[r1 + 3] + V_sqrt5;
	if (v < min)
		min = v;

	v = map[r2 + 4] + V_sqrt5;
	if (v < min)
		min = v;

	v = map[r4 + 4] + V_sqrt5;
	if (v < min)
		min = v;

	v = map[r5 + 3] + V_sqrt5;
	if (v < min)
		min = v;

	v = map[r5 + 1] + V_sqrt5;
	if (v < min)
		min = v;

	v = map[r4] + V_sqrt5;
	if (v < min)
		min = v;

	v = map[r2] + V_sqrt5;
	if (v < min)
		min = v;
	return map[offset] = min;
}
float setV2(float map[], int width, int offset)
{
	float min, v;
	int r1, r2, r3, r4, r5, r0, r6;

	r1 = offset - width - width - 2;
	r0 = r1 - width;
	r2 = r1 + width;
	r3 = r2 + width;
	r4 = r3 + width;
	r5 = r4 + width;
	r6 = r5 + width;

	v = map[r2 + 2] + V_0;
	min = v;
	v = map[r3 + 1] + V_0;
	if (v < min)
		min = v;
	v = map[r3 + 3] + V_0;
	if (v < min)
		min = v;
	v = map[r4 + 2] + V_0;
	if (v < min)
		min = v;

	v = map[r2 + 1] + V_sqrt2;
	if (v < min)
		min = v;
	v = map[r2 + 3] + V_sqrt2;
	if (v < min)
		min = v;
	v = map[r4 + 1] + V_sqrt2;
	if (v < min)
		min = v;
	v = map[r4 + 3] + V_sqrt2;
	if (v < min)
		min = v;

	v = map[r1 + 1] + V_sqrt5;
	if (v < min)
		min = v;
	v = map[r1 + 3] + V_sqrt5;
	if (v < min)
		min = v;
	v = map[r2 + 4] + V_sqrt5;
	if (v < min)
		min = v;
	v = map[r4 + 4] + V_sqrt5;
	if (v < min)
		min = v;
	v = map[r5 + 3] + V_sqrt5;
	if (v < min)
		min = v;
	v = map[r5 + 1] + V_sqrt5;
	if (v < min)
		min = v;
	v = map[r4] + V_sqrt5;
	if (v < min)
		min = v;
	v = map[r2] + V_sqrt5;
	if (v < min)
		min = v;

	return map[offset] = min;
}

float distanceMap(float* map, int width, int height, BOOL bRev)
{
	int xmax = width - 3;
	int ymax = height - 3;
	float max = 0;
	float v;
	if (bRev)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int offset = x + y * width;
				if (map[offset] > 0)
				{
					if (x < 2 || x > xmax || y < 2 || y > ymax)
						v = setV1(x, y, map, width, offset, xmax, ymax);
					else
						v = setV2(map, width, offset);
					if (v > max)
						max = v;
				}
			}
		}
		for (int y = height - 1; y >= 0; y--)
		{
			for (int x = width - 1; x >= 0; x--)
			{
				int offset = x + y * width;
				if (map[offset] > 0)
				{
					if (x < 2 || x > xmax || y < 2 || y > ymax)
						v = setV1(x, y, map, width, offset, xmax, ymax);
					else
						v = setV2(map, width, offset);
					if (v > max)
						max = v;
				}
			}
		}
	}
	else
	{
		for (int x = width - 1; x >= 0; x--)
		{
			for (int y = 0; y < height; y++)
			{
				int offset = x + y * width;
				if (map[offset] > 0)
				{
					if (x < 2 || x > xmax || y < 2 || y > ymax)
						v = setV1(x, y, map, width, offset, xmax, ymax);
					else
						v = setV2(map, width, offset);
					if (v > max)
						max = v;
				}
			}
		}
		for (int x = 0; x < width; x++)
		{
			for (int y = height - 1; y >= 0; y--)
			{
				int offset = x + y * width;
				if (map[offset] > 0)
				{
					if (x < 2 || x > xmax || y < 2 || y > ymax)
						v = setV1(x, y, map, width, offset, xmax, ymax);
					else
						v = setV2(map, width, offset);
					if (v > max)
						max = v;
				}
			}
		}
	}
	return max;
}
void makeMap(LPBYTE pixels, float* map, int width, int height, int nBevalWidth)
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int offset = x + y * width;
			map[offset] = pixels[offset] * V_0 / (float)nBevalWidth;
		}
	}
}

template<class DATATYPE>
void COT(DATATYPE* lpSrc, DATATYPE* lpDes, POINT PtOffset, SIZE& SzSrc, SIZE& SzDes)
{
	DATATYPE* lpSrcL = lpSrc + PtOffset.y * SzSrc.cx + PtOffset.x;
	DATATYPE* lpDesL = lpDes;
	int nCopyY = SzDes.cy;
	int nCopyX = SzDes.cx;
	for (int i = 0; i <nCopyY; i++)
	{
		memcpy(lpDesL, lpSrcL, nCopyX * sizeof(DATATYPE));
		lpSrcL += SzSrc.cx;
		lpDesL += SzDes.cx;
	}
}

float circleUp(float x)
{
	x = 1 - x;
	return (float)sqrt(1 - x*x);
}

void NormalBlur(float* lpSrc, LPSIZE pSize)
{
	float* lpTemp;
	float* lpData = lpSrc;
	{
		LONG nDataLength = pSize->cy * pSize->cx;
		lpTemp = new float[nDataLength];
		memcpy(lpTemp, lpData, nDataLength * 4);
	}

	float* lpSrcThis = lpTemp + pSize->cx + 1;
	float* lpSrcThisT = lpSrcThis - pSize->cx;
	float* lpSrcThisB = lpSrcThis + pSize->cx;

	float* lpDesThis = lpData + pSize->cx + 1;
	LONG	nHeight = pSize->cy - 2;
	LONG	nWidthX = pSize->cx - 2;

	while (nHeight--)
	{
		register LONG nWidth = nWidthX;
		while (nWidth--)
		{
			*lpDesThis = (((*(lpSrcThis - 1)) + (*(lpSrcThis + 1)) + (*lpSrcThis)
				+ (*(lpSrcThisT - 1)) + (*(lpSrcThisT + 1)) + (*lpSrcThisT)
				+ (*(lpSrcThisB - 1)) + (*(lpSrcThisB + 1)) + (*lpSrcThisB)) / 9
				);
			lpSrcThis++;
			lpDesThis++;
			lpSrcThisT++;
			lpSrcThisB++;
		}
		lpSrcThis += 2;
		lpDesThis += 2;
		lpSrcThisT += 2;
		lpSrcThisB += 2;
	}
	delete lpTemp;
}

DWORD CalcLightColor(DWORD				dwColor,
	float				fPhV_0Color[3],
	float               fSpecularn,
	vector3f&			vPt,
	vector3f&			vVNormal,
	LPLight_Source		lppNormalLight,
	int                 nLightCount,
	vector3f&			vEye
)
{
	DWORD dwRetColor;
	LPLight_Source   pLight = 0;
	vector3f		 vV = vPt;
	vV -= vEye;
	float3Normalize(&vV, &vV);


	float fIdR = 0.0f, fIdG = 0.0f, fIdB = 0.0f, fIksR = 0.0f, fIksG = 0.0f, fIksB = 0.0f;
	for (int nLight = 0; nLight<nLightCount; nLight++)
	{
		pLight = lppNormalLight + nLight;
		vector3f vL = pLight->sPosW;
		vL -= vPt;
		float3Normalize(&vL, &vL);

		float fNL = float3DotProduct(&vVNormal, &vL);
		{
			vL -= vV;
			float3Normalize(&vL, &vL);
			float fNH = float3DotProduct(&vVNormal, &vL);

			vector3f vR = vVNormal;
			vR *= 2.0f*fNH;
			vR -= vL;
			fNH = float3DotProduct(&vVNormal, &vR);

			float fNHp = (float)pow(fNH, fSpecularn);
			if (nLight)
			{
				if (fNL>0.0f)
				{
					fIdR += fNL*(*(pLight->fI));
					fIdG += fNL*(*(pLight->fI + 1));
					fIdB += fNL*(*(pLight->fI + 2));
				}
				if (fNH>0.0f)
				{
					fIksR += (*(fPhV_0Color))*fNHp*(*(pLight->fI));
					fIksG += (*(fPhV_0Color + 1))*fNHp*(*(pLight->fI + 1));
					fIksB += (*(fPhV_0Color + 2))*fNHp*(*(pLight->fI + 2));
				}

			}
			else
			{
				if (fNL>0.0f)
				{
					fIdR = fNL*(*(pLight->fI));
					fIdG = fNL*(*(pLight->fI + 1));
					fIdB = fNL*(*(pLight->fI + 2));
				}
				if (fNH>0.0f)
				{
					fIksR = (*(fPhV_0Color))*fNHp*(*(pLight->fI));
					fIksG = (*(fPhV_0Color + 1))*fNHp*(*(pLight->fI + 1));
					fIksB = (*(fPhV_0Color + 2))*fNHp*(*(pLight->fI + 2));
				}
			}
		}
	}
	fIdR = fIdR * GetBGRA_R(dwColor);
	fIdG = fIdG * GetBGRA_G(dwColor);
	fIdB = fIdB * GetBGRA_B(dwColor);
	fIdR += fIksR;
	fIdG += fIksG;
	fIdB += fIksB;
	if (fIdG>255.0f)
		fIdG = 255.0f;
	if (fIdB>255.0f)
		fIdB = 255.0f;
	if (fIdR>255.0f)
		fIdR = 255.0f;
	dwRetColor = BGRA((BYTE)fIdB, (BYTE)fIdG, (BYTE)fIdR, GetAValue(dwRetColor));
	return dwRetColor;
}

void LightProc(LPBYTE		       lpMask,
	LPDWORD		       lpSrcBuf,
	LPSIZE			   lpSz,
	int				   type,
	SLight_Source       Light[],
	int                 nLight,
	vector3f				vEye,
	DWORD			   dwHighLight,
	float			   fSpecular,
	int				   nW,
	int				   nH
)
{
	int* inPixels = (int*)lpSrcBuf;
	int nExt = 8;
	SIZE SzExt = *lpSz;
	SzExt.cx += nExt * 2;
	SzExt.cy += nExt * 2;//扩展2倍

						 //浮点缓存
	float* map = new float[SzExt.cx *SzExt.cy];
	float* fMap = new float[SzExt.cx *SzExt.cy];

	int nS = SzExt.cx * SzExt.cy;
	LPBYTE lpData = new BYTE[nS];

	//拷贝到目标buffer
	POINT ptExt = { nExt,nExt };
	OCT(lpMask, lpData, ptExt, (SIZE&)*lpSz, SzExt);


	float* mapT = new float[SzExt.cx *SzExt.cy];
	float max = 0;
	for (int i = 0; i<4; i++)
	{
		//高斯滤波，除去高频分量
		SY_GuassianBlur(lpData, &SzExt, 2);//up
		float maxT;
		if (i == 0)
		{
			makeMap(lpData, mapT, SzExt.cx, SzExt.cy, 101 - nW);
			maxT = distanceMap(mapT, SzExt.cx, SzExt.cy, i % 2);
		}
		else
		{
			makeMap(lpData, map, SzExt.cx, SzExt.cy, 101 - nW);
			maxT = distanceMap(map, SzExt.cx, SzExt.cy, i % 2);
			nS = SzExt.cx * SzExt.cy;
			while (nS--)
			{
				mapT[nS] += map[nS];
			}
		}
		max += maxT;

	}
	delete[]map;
	map = mapT;
	NormalBlur(map, &SzExt);
	nS = SzExt.cx * SzExt.cy;
	while (nS--)
	{
		float v = map[nS] / max;
		switch (type)
		{
		case 1:
			fMap[nS] = (circleUp(v));
			break;
		default:
			fMap[nS] = v;
			break;
		}
	}

	float				fPhV_0Color[3] = { (float)GetBGRA_R(dwHighLight),(float)GetBGRA_G(dwHighLight),(float)GetBGRA_B(dwHighLight) };
	//	float				fPhV_0Color[3] = { 1.f *255,1.f*255,1.f*255};

	//	float               fSpecularn = 35.5f;//up
	float               fSpecularn = fSpecular;

	SLight_Source       Light_Source[3];
	/*
	vector3f			vEye(lpSz->cx/2 , lpSz->cy/2,100.f);
	Light_Source[0].sPosW = vector3f(lpSz->cx/2 , -lpSz->cy*4,1000.f);
	Light_Source[1].sPosW = vector3f(lpSz->cx/2 , lpSz->cy/2,1000.f);
	Light_Source[2].sPosW = vector3f(lpSz->cx/2 , lpSz->cy*4,1000.f);
	vEye.x = g_vEye.x * lpSz->cx;
	vEye.y = g_vEye.y * lpSz->cy;
	vEye.fZ =  g_vEye.fZ;

	for(i=0;i<3;i++)
	{
	Light_Source[i].sPosW.fX = g_Light_Source[i].sPosW.fX * lpSz->cx;
	Light_Source[i].sPosW.y = g_Light_Source[i].sPosW.y * lpSz->cy;
	Light_Source[i].sPosW.fZ = g_Light_Source[i].sPosW.fZ;
	}
	*/

	for (int i = 0; i<nLight; i++)
	{
		Light_Source[i] = Light[i];
		Light_Source[i].sPosW.x *= SzExt.cx;
		Light_Source[i].sPosW.y *= SzExt.cy;
	}
	vEye.x = vEye.x * SzExt.cx;
	vEye.y = vEye.y * SzExt.cy;


	LPDWORD lpSrc = lpSrcBuf;
	float* fM = fMap;

	fM = fMap;
	float f2 = nH* 0.002f;


	vector3f*  vNL = new vector3f[SzExt.cx * SzExt.cy];
	vector3f*  vpN = vNL;

	for (int y = 0; y<SzExt.cy; y++)
	{
		for (int x = 0; x<SzExt.cx; x++, fM++, vpN++)
		{
			if (x <1 || x >= SzExt.cx - 1 || y <1 || y >= SzExt.cy - 1)
				continue;
			vector3f v1 = { f2,0, *(fM + 1) - *(fM - 1) };
			vector3f v2 = { 0,f2, *(fM + SzExt.cx) - *(fM - SzExt.cx) };
			float3CrossProduct(vpN, &v1, &v2);
			//CrossProduct (vector3f(f2,0, *(fM+1)-*(fM-1)),vector3f(0,f2, *(fM+SzExt.cx)-*(fM-SzExt.cx)),*vpN);
		}
	}
	COT(fMap, map, ptExt, SzExt, (SIZE&)*lpSz);

	vector3f*  vNL1 = new vector3f[SzExt.cx * SzExt.cy];

	COT(vNL, vNL1, ptExt, SzExt, (SIZE&)*lpSz);
	delete[]vNL;
	vNL = vNL1;

	fM = map;
	vpN = vNL;

	LPBYTE lpM = lpMask;

	for (int y = 0; y<lpSz->cy; y++)
	{
		for (int x = 0; x<lpSz->cx; x++, lpSrc++, lpM++, fM++, vpN++)
		{
			if (*lpM == 0 || x <= 1 || x >= lpSz->cx - 1 || y <= 1 || y >= lpSz->cy - 1)
				continue;
			DWORD	dwColor = *lpSrc;

			vector3f				vPt = { (float)x,(float)y, *fM };
			vector3f				vVNormal = *vpN;

			vPt.z += *(fM + 1);
			vVNormal += *(vpN + 1);

			vPt.z += *(fM + 1 + lpSz->cx);
			vVNormal += *(vpN + 1 + lpSz->cx);

			vPt.z += *(fM + lpSz->cx);
			vVNormal += *(vpN + lpSz->cx);

			vPt.z *= 0.25f;
			vPt.x *= f2;
			vPt.y *= f2;

			float fLen = (vVNormal.x*vVNormal.x) + (vVNormal.y*vVNormal.y) + (vVNormal.z*vVNormal.z);
			float3Normalize(&vVNormal, &vVNormal);
			if (fLen > 0.0f)
			{
				dwColor = CalcLightColor(dwColor,
					fPhV_0Color,
					fSpecularn,
					vPt,
					vVNormal,
					Light_Source,
					nLight,
					vEye
				);
			}
			else
			{
				dwColor = 0;
			}
			BYTE bA = GetAValue(*lpSrc);
			dwColor = (dwColor & 0x00ffffff) | (bA << 24);
			*lpSrc = dwColor;

		}
	}
	delete[]vNL;
	delete[]map;
	delete[]fMap;
	delete[]lpData;
	delete[]lpMask;
	vNL = NULL;
	map = NULL;
	fMap = NULL;
	lpData = NULL;
	lpMask = NULL;
}

void Light(DWORD * dwBuffer, int nWidth, int nHeight, BaseGraph_DataBase::metalinfo * pinfo)
{
	LPBYTE lpMask = new BYTE[nWidth * nHeight];
	LPBYTE lpTemp = lpMask;
	DWORD* dwTemp = dwBuffer;
	for (int i = 0; i < nHeight; i++)
	{
		for (int k = 0; k < nWidth; k++)
		{
			lpTemp[k] = (BYTE)((dwTemp[k] & 0xff000000) >> 24);
		}
		lpTemp += nWidth;
		dwTemp += nWidth;
	}


	SLight_Source       Light_Source[3];
	memset(Light_Source, 0, sizeof(SLight_Source));
	Light_Source[0].sPosW.x = 0.5f;
	Light_Source[0].sPosW.y = -4.0f;
	Light_Source[0].sPosW.z = 1000.0f;

	Light_Source[1].sPosW.x = 0.5f;
	Light_Source[1].sPosW.y = 0.5f;
	Light_Source[1].sPosW.z = 1000.0f;

	int nL = 0;
	for (int i = 0; i<3; i++)
	{
		Light_Source[i].fI[0] = .4f;
		Light_Source[i].fI[1] = .4f;
		Light_Source[i].fI[2] = .4f;
		Light_Source[i].fk = 1.0f;

		if (pinfo->light[i].enable)
		{
			Light_Source[nL].sPosW = pinfo->light[i].pos;
			Light_Source[nL].fI[0] = pinfo->light[i].strength[0];
			Light_Source[nL].fI[1] = pinfo->light[i].strength[1];
			Light_Source[nL].fI[2] = pinfo->light[i].strength[2];
			nL++;
		}
	}
	float	fSpecular = 13.f;
	SIZE size;
	size.cx = nWidth;
	size.cy = nHeight;

	LightProc(
		lpMask,
		dwBuffer,
		&size,
		pinfo->barc,
		Light_Source,
		nL,
		pinfo->eyepos,
		pinfo->highlight,
		fSpecular,
		pinfo->width,
		pinfo->height
	);
}

/**
*@brief 短划线
*/
void GetImagPointsDASH(IN const SYArray& pptfArr1, IN const float fLineWidth, OUT int& nCell, OUT SYArray& pptfArr)
{
	int nSize;
	nSize = (int)SYAR_GetSize((SYArray*)&pptfArr1);
	if (nSize <= 0)
	{
		nCell = 0;
		return;
	}
	pointf *pptTemp1 = nullptr;
	pointf *pptTemp2 = nullptr;

	float fDis = GetCurveDistance(&pptfArr1);
	nCell = 1;

	pointf* pptImg;
	pptImg = new pointf;
	pptImg->x = ((pointf*)SYAR_GetAt((SYArray*)&pptfArr1, 0))->x;
	pptImg->y = ((pointf*)SYAR_GetAt((SYArray*)&pptfArr1, 0))->y;
	SYAR_Add((SYArray*)&pptfArr, pptImg);

	nSize = (int)SYAR_GetSize((SYArray*)&pptfArr1);

	float fDivSum = 0.0;
	float fDivPre = 0.0;
	float fDivStep = 0.0;

	float fInterval = 4.0f * fLineWidth;
	int nCount = 0;

	// 切分Bezier曲线
	while (fDis > fInterval)
	{
		for (int i = 0; i < nSize - 1; i++)
		{
			pptTemp1 = (pointf*)SYAR_GetAt((SYArray*)&pptfArr1, i);
			pptTemp2 = (pointf*)SYAR_GetAt((SYArray*)&pptfArr1, i + 1);
			fDivPre = fDivSum;
			fDivSum += GetDistace(pptTemp2, pptTemp1);
			if (fDivSum >= fInterval)
			{
				fDivSum = 0.0;
				break;
			}
		}
		fDivStep = (fInterval - fDivPre);
		pointf ptfDir;
		ptfDir.x = (pptTemp2->x - pptTemp1->x) / GetDistace(pptTemp2, pptTemp1);
		ptfDir.y = (pptTemp2->y - pptTemp1->y) / GetDistace(pptTemp2, pptTemp1);
		pptImg = new pointf;
		pptImg->x = pptTemp1->x + fDivStep * ptfDir.x;
		pptImg->y = pptTemp1->y + fDivStep * ptfDir.y;
		SYAR_Add(&pptfArr, pptImg);
		fDivPre = 0.0;

		// 跨过2个像素间隔
		nCount++;

		if (nCount % 2 == 0)
		{
			fInterval += 4.0f * fLineWidth;
			nCell++;
		}
		else
		{
			fInterval += 2.0f * fLineWidth;
		}
	}

	pptImg = new pointf;
	pptImg->x = ((pointf*)SYAR_GetAt((SYArray*)&pptfArr1, nSize - 1))->x;
	pptImg->y = ((pointf*)SYAR_GetAt((SYArray*)&pptfArr1, nSize - 1))->y;
	SYAR_Add(&pptfArr, pptImg);
}

BASEGRAPH_DATARENDER_API void GetImagPointsByStyle(IN const SYArray & pptfArr1, IN const int nStyle, IN const float fLineWidth, OUT int & nCell, OUT SYArray & pptfArr)
{
	switch (nStyle)
	{
	case SYLINE_DASH:
		GetImagPointsDASH(pptfArr1, fLineWidth, nCell, pptfArr);
		break;
	case SYLINE_DASHDOT:
		GetImagPointsDASHDOT(pptfArr1, fLineWidth, nCell, pptfArr);
		break;
	case SYLINE_LDASH:
		GetImagPointsLDASH(pptfArr1, fLineWidth, nCell, pptfArr);
		break;
	case SYLINE_LDASHDOT:
		GetImagPointsLDASHDOT(pptfArr1, fLineWidth, nCell, pptfArr);
		break;
	case SYLINE_RIPPLE:
		GetPointsRIPPLE(pptfArr1, fLineWidth, nCell, pptfArr);
		break;
	default:
		break;
	}
}

BASEGRAPH_DATARENDER_API void	GetImagPointsDASH(IN const SYArray& pptfArr1, IN const float fLineWidth, OUT int& nCell, OUT SYArray& pptfArr, float *fIntervalAr, int intCount)
{
	int nSize;
	nSize = (int)SYAR_GetSize((SYArray*)&pptfArr1);
	if (nSize <= 0)
	{
		nCell = 0;
		return;
	}
	pointf *pptTemp1 = nullptr;
	pointf *pptTemp2 = nullptr;

	float fDis = GetCurveDistance(&pptfArr1);
	nCell = 1;

	pointf* pptImg;
	pptImg = new pointf;
	pptImg->x = ((pointf*)SYAR_GetAt((SYArray*)&pptfArr1, 0))->x;
	pptImg->y = ((pointf*)SYAR_GetAt((SYArray*)&pptfArr1, 0))->y;
	SYAR_Add((SYArray*)&pptfArr, pptImg);

	nSize = (int)SYAR_GetSize((SYArray*)&pptfArr1);

	float fDivSum = 0.0;
	float fDivPre = 0.0;
	float fDivStep = 0.0;

	float fInterval = 4.0f * fLineWidth;
	int nCount = 0;

	// 切分Bezier曲线
	while (fDis > fInterval)
	{
		for (int i = 0; i < nSize - 1; i++)
		{
			pptTemp1 = (pointf*)SYAR_GetAt((SYArray*)&pptfArr1, i);
			pptTemp2 = (pointf*)SYAR_GetAt((SYArray*)&pptfArr1, i + 1);
			fDivPre = fDivSum;
			fDivSum += GetDistace(pptTemp2, pptTemp1);
			if (fDivSum >= fInterval)
			{
				fDivSum = 0.0;
				break;
			}
		}
		fDivStep = (fInterval - fDivPre);
		pointf ptfDir;
		ptfDir.x = (pptTemp2->x - pptTemp1->x) / GetDistace(pptTemp2, pptTemp1);
		ptfDir.y = (pptTemp2->y - pptTemp1->y) / GetDistace(pptTemp2, pptTemp1);
		pptImg = new pointf;
		pptImg->x = pptTemp1->x + fDivStep * ptfDir.x;
		pptImg->y = pptTemp1->y + fDivStep * ptfDir.y;
		SYAR_Add(&pptfArr, pptImg);
		fDivPre = 0.0;

		// 跨过2个像素间隔
		nCount++;

		if (nCount % 2 == 0)
		{
			fInterval += 4.0f * fLineWidth;
			nCell++;
		}
		else
		{
			fInterval += 2.0f * fLineWidth;
		}
	}

	pptImg = new pointf;
	pptImg->x = ((pointf*)SYAR_GetAt((SYArray*)&pptfArr1, nSize - 1))->x;
	pptImg->y = ((pointf*)SYAR_GetAt((SYArray*)&pptfArr1, nSize - 1))->y;
	SYAR_Add(&pptfArr, pptImg);
}
BASEGRAPH_DATARENDER_API void	GetImagPointsDASHDOT(IN const SYArray& pptfArr1, IN const float fLineWidth, OUT int& nCell, OUT SYArray& pptfArr)
{
	int nSize;
	nSize = (int)SYAR_GetSize((SYArray*)&pptfArr1);
	if (nSize <= 0)
	{
		nCell = 0;
		return;
	}
	pointf *pptTemp1 = nullptr;
	pointf *pptTemp2 = nullptr;

	float fDis = GetCurveDistance(&pptfArr1);
	nCell = 0;

	pointf* pptImg;
	pptImg = new pointf;
	pptImg->x = ((pointf*)SYAR_GetAt((SYArray*)&pptfArr1, 0))->x;
	pptImg->y = ((pointf*)SYAR_GetAt((SYArray*)&pptfArr1, 0))->y;
	SYAR_Add((SYArray*)&pptfArr, pptImg);

	nSize = (int)SYAR_GetSize((SYArray*)&pptfArr1);

	float fDivSum = 0.0;
	float fDivPre = 0.0;
	float fDivStep = 0.0;

	float fInterval1 = 4.0f * fLineWidth;
	float fInterval2 = 2.0f * fLineWidth;
	float fInterval3 = 1.0f * fLineWidth;
	float fInterval = fInterval1;
	float fIntervalPre = fInterval1;
	int nCount = 0;

	// 切分Bezier曲线
	while (fDis > fInterval)
	{
		for (int i = 0; i < nSize - 1; i++)
		{
			pptTemp1 = (pointf*)SYAR_GetAt((SYArray*)&pptfArr1, i);
			pptTemp2 = (pointf*)SYAR_GetAt((SYArray*)&pptfArr1, i + 1);
			fDivPre = fDivSum;
			fDivSum += GetDistace(pptTemp2, pptTemp1);
			if (fDivSum >= fInterval)
			{
				fDivSum = 0.0;
				break;
			}
		}
		fDivStep = (fInterval - fDivPre);
		pointf ptfDir;
		ptfDir.x = (pptTemp2->x - pptTemp1->x) / GetDistace(pptTemp2, pptTemp1);
		ptfDir.y = (pptTemp2->y - pptTemp1->y) / GetDistace(pptTemp2, pptTemp1);
		pptImg = new pointf;
		pptImg->x = pptTemp1->x + fDivStep * ptfDir.x;
		pptImg->y = pptTemp1->y + fDivStep * ptfDir.y;
		SYAR_Add((SYArray*)&pptfArr, pptImg);
		fDivPre = 0.0;

		if (abs(fIntervalPre - fInterval1) < ABS_FLOAT_0)
		{
			nCell++;
			fInterval += fInterval2;
			fIntervalPre = fInterval2;
		}
		else if (abs(fIntervalPre - fInterval2) < ABS_FLOAT_0)
		{
			nCount++;
			if (nCount % 2 == 0)
			{
				fInterval += fInterval1;
				fIntervalPre = fInterval1;
			}
			else
			{
				fInterval += fInterval3;
				fIntervalPre = fInterval3;
			}
		}
		else if (abs(fIntervalPre - fInterval3) < ABS_FLOAT_0)
		{
			nCell++;
			fInterval += fInterval2;
			fIntervalPre = fInterval2;
		}
	}

	pptImg = new pointf;
	pptImg->x = ((pointf*)SYAR_GetAt((SYArray*)&pptfArr1, nSize - 1))->x;
	pptImg->y = ((pointf*)SYAR_GetAt((SYArray*)&pptfArr1, nSize - 1))->y;
	SYAR_Add((SYArray*)&pptfArr, pptImg);
}
BASEGRAPH_DATARENDER_API void	GetImagPointsLDASH(IN const SYArray& pptfArr1, IN const float fLineWidth, OUT int& nCell, OUT SYArray& pptfArr)
{
	int nSize;
	nSize = (int)SYAR_GetSize((SYArray*)&pptfArr1);
	if (nSize <= 0)
	{
		nCell = 0;
		return;
	}
	pointf *pptTemp1 = nullptr;
	pointf *pptTemp2 = nullptr;

	float fDis = GetCurveDistance(&pptfArr1);
	nCell = 1;

	pointf* pptImg;
	pptImg = new pointf;
	pptImg->x = ((pointf*)SYAR_GetAt((SYArray*)&pptfArr1, 0))->x;
	pptImg->y = ((pointf*)SYAR_GetAt((SYArray*)&pptfArr1, 0))->y;
	SYAR_Add((SYArray*)&pptfArr, pptImg);

	nSize = (int)SYAR_GetSize((SYArray*)&pptfArr1);

	float fDivSum = 0.0;
	float fDivPre = 0.0;
	float fDivStep = 0.0;

	float fInterval = 8.0f * fLineWidth;
	int nCount = 0;

	// 切分Bezier曲线
	while (fDis > fInterval)
	{
		for (int i = 0; i < nSize - 1; i++)
		{
			pptTemp1 = (pointf*)SYAR_GetAt((SYArray*)&pptfArr1, i);
			pptTemp2 = (pointf*)SYAR_GetAt((SYArray*)&pptfArr1, i + 1);
			fDivPre = fDivSum;
			fDivSum += GetDistace(pptTemp2, pptTemp1);
			if (fDivSum >= fInterval)
			{
				fDivSum = 0.0;
				break;
			}
		}
		fDivStep = (fInterval - fDivPre);
		pointf ptfDir;
		ptfDir.x = (pptTemp2->x - pptTemp1->x) / GetDistace(pptTemp2, pptTemp1);
		ptfDir.y = (pptTemp2->y - pptTemp1->y) / GetDistace(pptTemp2, pptTemp1);
		pptImg = new pointf;
		pptImg->x = pptTemp1->x + fDivStep * ptfDir.x;
		pptImg->y = pptTemp1->y + fDivStep * ptfDir.y;
		SYAR_Add((SYArray*)&pptfArr, pptImg);
		fDivPre = 0.0;

		// 跨过2个像素间隔
		nCount++;

		if (nCount % 2 == 0)
		{
			fInterval += 8.0f * fLineWidth;
			nCell++;
		}
		else
		{
			fInterval += 2.0f * fLineWidth;
		}
	}

	pptImg = new pointf;
	pptImg->x = ((pointf*)SYAR_GetAt((SYArray*)&pptfArr1, nSize - 1))->x;
	pptImg->y = ((pointf*)SYAR_GetAt((SYArray*)&pptfArr1, nSize - 1))->y;
	SYAR_Add((SYArray*)&pptfArr, pptImg);
}
BASEGRAPH_DATARENDER_API void	GetImagPointsLDASHDOT(IN const SYArray& pptfArr1, IN const float fLineWidth, OUT int& nCell, OUT SYArray& pptfArr)
{
	int nSize;
	nSize = (int)SYAR_GetSize((SYArray*)&pptfArr1);
	if (nSize <= 0)
	{
		nCell = 0;
		return;
	}
	pointf *pptTemp1 = nullptr;
	pointf *pptTemp2 = nullptr;

	float fDis = GetCurveDistance(&pptfArr1);
	nCell = 0;

	pointf* pptImg;
	pptImg = new pointf;
	pptImg->x = ((pointf*)SYAR_GetAt((SYArray*)&pptfArr1, 0))->x;
	pptImg->y = ((pointf*)SYAR_GetAt((SYArray*)&pptfArr1, 0))->y;
	SYAR_Add((SYArray*)&pptfArr, pptImg);

	nSize = (int)SYAR_GetSize((SYArray*)&pptfArr1);

	float fDivSum = 0.0;
	float fDivPre = 0.0;
	float fDivStep = 0.0;

	float fInterval1 = 8.0f * fLineWidth;
	float fInterval2 = 2.0f * fLineWidth;
	float fInterval3 = 1.0f * fLineWidth;
	float fInterval = fInterval1;
	float fIntervalPre = fInterval1;
	int nCount = 0;

	// 切分Bezier曲线
	while (fDis > fInterval)
	{
		for (int i = 0; i < nSize - 1; i++)
		{
			pptTemp1 = (pointf*)SYAR_GetAt((SYArray*)&pptfArr1, i);
			pptTemp2 = (pointf*)SYAR_GetAt((SYArray*)&pptfArr1, i + 1);
			fDivPre = fDivSum;
			fDivSum += GetDistace(pptTemp2, pptTemp1);
			if (fDivSum >= fInterval)
			{
				fDivSum = 0.0;
				break;
			}
		}
		fDivStep = (fInterval - fDivPre);
		pointf ptfDir;
		ptfDir.x = (pptTemp2->x - pptTemp1->x) / GetDistace(pptTemp2, pptTemp1);
		ptfDir.y = (pptTemp2->y - pptTemp1->y) / GetDistace(pptTemp2, pptTemp1);
		pptImg = new pointf;
		pptImg->x = pptTemp1->x + fDivStep * ptfDir.x;
		pptImg->y = pptTemp1->y + fDivStep * ptfDir.y;
		SYAR_Add((SYArray*)&pptfArr, pptImg);
		fDivPre = 0.0;

		if (abs(fIntervalPre - fInterval1) < ABS_FLOAT_0)
		{
			nCell++;
			fInterval += fInterval2;
			fIntervalPre = fInterval2;
		}
		else if (abs(fIntervalPre - fInterval2) < ABS_FLOAT_0)
		{
			nCount++;
			if (nCount % 2 == 0)
			{
				fInterval += fInterval1;
				fIntervalPre = fInterval1;
			}
			else
			{
				fInterval += fInterval3;
				fIntervalPre = fInterval3;
			}
		}
		else if (abs(fIntervalPre - fInterval3) < ABS_FLOAT_0)
		{
			nCell++;
			fInterval += fInterval2;
			fIntervalPre = fInterval2;
		}
	}

	pptImg = new pointf;
	pptImg->x = ((pointf*)SYAR_GetAt((SYArray*)&pptfArr1, nSize - 1))->x;
	pptImg->y = ((pointf*)SYAR_GetAt((SYArray*)&pptfArr1, nSize - 1))->y;
	SYAR_Add((SYArray*)&pptfArr, pptImg);
}
BASEGRAPH_DATARENDER_API void	GetPointsRIPPLE(IN const SYArray& pptfArr1, IN const float fLineWidth, OUT int& nCell, OUT SYArray& pptfArr)
{
	int nSize = (int)SYAR_GetSize((SYArray*)&pptfArr1);
	if (nSize <= 0)
	{
		nCell = 0;
		return;
	}
	SYArray pptfTemp;
	int i;
	pointf *pptTemp1 = nullptr;
	pointf *pptTemp2 = nullptr;

	float fDis = GetCurveDistance(&pptfArr1);
	nCell = 1;

	pointf* pptImg;
	pptImg = new pointf;
	pptImg->x = ((pointf*)SYAR_GetAt((SYArray*)&pptfArr1, 0))->x;
	pptImg->y = ((pointf*)SYAR_GetAt((SYArray*)&pptfArr1, 0))->y;
	SYAR_Add((SYArray*)&pptfTemp, pptImg);

	float fDivSum = 0.0;
	float fDivPre = 0.0;
	float fDivStep = 0.0;

	float fInterval = 8.0f * fLineWidth;
	int nCount = 0;

	// 切分Bezier曲线
	while (fDis > fInterval)
	{
		for (int i = 0; i < nSize - 1; i++)
		{
			pptTemp1 = (pointf*)SYAR_GetAt((SYArray*)&pptfArr1, i);
			pptTemp2 = (pointf*)SYAR_GetAt((SYArray*)&pptfArr1, i + 1);
			fDivPre = fDivSum;
			fDivSum += GetDistace(pptTemp2, pptTemp1);
			if (fDivSum >= fInterval)
			{
				fDivSum = 0.0;
				break;
			}
		}
		fDivStep = (fInterval - fDivPre);
		pointf ptfDir;
		ptfDir.x = (pptTemp2->x - pptTemp1->x) / GetDistace(pptTemp2, pptTemp1);
		ptfDir.y = (pptTemp2->y - pptTemp1->y) / GetDistace(pptTemp2, pptTemp1);
		pptImg = new pointf;
		pptImg->x = pptTemp1->x + fDivStep * ptfDir.x;
		pptImg->y = pptTemp1->y + fDivStep * ptfDir.y;
		SYAR_Add((SYArray*)&pptfTemp, pptImg);
		fDivPre = 0.0;

		fInterval += 8.0f * fLineWidth;
	}

	pptImg = new pointf;
	pptImg->x = ((pointf*)SYAR_GetAt((SYArray*)&pptfArr1, nSize - 1))->x;
	pptImg->y = ((pointf*)SYAR_GetAt((SYArray*)&pptfArr1, nSize - 1))->y;
	SYAR_Add((SYArray*)&pptfTemp, pptImg);

	nSize = (int)SYAR_GetSize((SYArray*)&pptfTemp);

	pointf ptfStart, ptfEnd;
	pointf ptf;
	pointf ptfBase;
	float nHeight = 4.0f * fLineWidth; // 振幅

	pptTemp1 = new pointf;
	*pptTemp1 = *(pointf*)SYAR_GetAt((SYArray*)&pptfTemp, 0);

	SYAR_Add((SYArray*)&pptfArr, pptTemp1);

	for (i = 0; i < nSize - 1; i++)
	{
		ptfStart = *(pointf*)SYAR_GetAt((SYArray*)&pptfTemp, i);

		ptfEnd = *(pointf*)SYAR_GetAt((SYArray*)&pptfTemp, i + 1);

		float r = (float)sqrt((double)((ptfEnd.x - ptfStart.x)*(ptfEnd.x - ptfStart.x)
			+ (ptfStart.y - ptfEnd.y)*(ptfStart.y - ptfEnd.y)));
		if (r < 0.000001 && r>-0.000001)
		{
			continue;
		}
		ptf.x = (ptfStart.y - ptfEnd.y) / r;
		ptf.y = (ptfEnd.x - ptfStart.x) / r;

		ptfBase.x = ptfStart.x + 0.25f * (ptfEnd.x - ptfStart.x);
		ptfBase.y = ptfStart.y + 0.25f * (ptfEnd.y - ptfStart.y);

		pptTemp1 = new pointf;
		pptTemp1->x = ptfBase.x + nHeight * ptf.x;
		pptTemp1->y = ptfBase.y + nHeight * ptf.y;
		SYAR_Add((SYArray*)&pptfArr, pptTemp1);

		ptfBase.x = ptfStart.x + 0.75f * (ptfEnd.x - ptfStart.x);
		ptfBase.y = ptfStart.y + 0.75f * (ptfEnd.y - ptfStart.y);

		pptTemp1 = new pointf;
		pptTemp1->x = ptfBase.x - nHeight * ptf.x;
		pptTemp1->y = ptfBase.y - nHeight * ptf.y;
		SYAR_Add((SYArray*)&pptfArr, pptTemp1);

		ptfBase.x = ptfStart.x + 1.0f * (ptfEnd.x - ptfStart.x);
		ptfBase.y = ptfStart.y + 1.0f * (ptfEnd.y - ptfStart.y);

		pptTemp1 = new pointf;
		pptTemp1->x = ptfBase.x;
		pptTemp1->y = ptfBase.y;
		SYAR_Add((SYArray*)&pptfArr, pptTemp1);
	}

	// 释放pptfTemp
	nSize = (int)SYAR_GetSize((SYArray*)&pptfTemp);
	for (i = 0; i < nSize; i++)
	{
		pointf* ptfTemp = (pointf*)SYAR_GetAt((SYArray*)&pptfTemp, i);
		delete ptfTemp;
		ptfTemp = NULL;
	}
	SYAR_RemoveAll((SYArray*)&pptfTemp);
}

BASEGRAPH_DATARENDER_API float GetDistace(const pointf* ptf1, const pointf* ptf2)
{
	return (sqrt((ptf1->x - ptf2->x) * (ptf1->x - ptf2->x) + (ptf1->y - ptf2->y) * (ptf1->y - ptf2->y)));
}

BASEGRAPH_DATARENDER_API float GetCurveDistance(const SYArray * pptrArr)
{
	int nSize = (int)SYAR_GetSize((SYArray*)pptrArr);
	if (nSize <= 0)
	{
		return 0.0;
	}
	else
	{
		float fDis = 0.0;
		for (int i = 0; i < nSize - 1; i++)
		{
			pointf* ptf1 = (pointf*)SYAR_GetAt((SYArray*)pptrArr, i);
			pointf* ptf2 = (pointf*)SYAR_GetAt((SYArray*)pptrArr, i + 1);
			fDis += GetDistace(ptf1, ptf2);
		}
		return fDis;
	}
}

BASEGRAPH_DATARENDER_API void CopyCell(vectorcell * pdescell, vectorcell * psrccell)
{
	memcpy(pdescell, psrccell, sizeof(vectorcell));
	if (psrccell->npoint > 0)
	{
		pdescell->point = new pointf[psrccell->npoint];
		memcpy(pdescell->point, psrccell->point, sizeof(pointf)*psrccell->npoint);
		pdescell->pttype = new unsigned long[psrccell->npoint];
		memcpy(pdescell->pttype, psrccell->pttype, sizeof(unsigned long)*psrccell->npoint);
	}
}

void Modify3DUnitFrom2DUnit(SYUnit * punit3d, SYUnit * punit2d, SYMaterial ** p3dcolor, int nSubFace, LPSYStretchParam pStretchParam)
{
	if (pStretchParam == NULL)
		return;

	LPSYStretchParam pStretch = (LPSYStretchParam)pStretchParam;
	float3Set(&punit3d->pos, punit2d->pos.x, punit2d->pos.y, 0);
	SYModel* pMod = new SYModel;
	sy_init_moddef(pMod);
	vectorgraph* pFigure = &punit2d->u.u2d.graph;
	vectorgraph NewFigure;
	memset(&NewFigure, 0, sizeof(vectorgraph));
	vg_copy_with_null_cell(&NewFigure, pFigure);
	if (NewFigure.ncell == 0)
	{
		pMod->nviewsub = 1;
		pMod->viewsub = new SYSubModel[pMod->nviewsub];
		memset(pMod->viewsub, 0, pMod->nviewsub * sizeof(SYSubModel));
	}
	else
	{
		SY_BEVEL_TYPE eBevelType = pStretch->eBevelType;
		float fRealHeaght = eBevelType == SY_BEVEL_NONE ? pStretch->fThick : pStretch->fThick - pStretch->fBevelThick * 2;
		SYModel_CreateBevel(pMod, &NewFigure, max(0, fRealHeaght), pStretch->fBevelThick, pStretch->fBevelLen, eBevelType);
	}
	//未指定3D色时，使用2D颜色创建材质。
	SYMaterial* pMaterial;
	if (p3dcolor == NULL)
	{
		DWORD faceColor = SYColor_GetRGBColor(&punit2d->u.u2d.staticeffect.face.color);
		for (int i = 0; i < pMod->nviewsub; ++i)
		{
			pMaterial = (SYMaterial*)sy_get_unit_material(punit3d, i);
			if (pMaterial != NULL)
			{
				pMod->viewsub[i].pMaterial = pMaterial;
				sy_set_unit_material(punit3d, i, NULL);
			}
			else
			{
				pMaterial = new SYMaterial;
				SYMaterial_Init(pMaterial, faceColor);
				pMod->viewsub[i].pMaterial = pMaterial;
			}
		}
	}
	else
	{
		SYMaterial** pSrcMaterial = p3dcolor;
		//Copy Material
		for (int i = 0; i < pMod->nviewsub; ++i)
		{
			pMaterial = (SYMaterial*)pMod->viewsub[i].pMaterial;
			if (pMaterial == NULL)
			{
				pMaterial = new SYMaterial;
				memset(pMaterial, 0, sizeof(SYMaterial));
			}
			else
			{
				SYMaterial_Release(pMaterial);
			}
			if (i < nSubFace)
				SYMaterial_Copy(pMaterial, pSrcMaterial[i]);
			else
				SYMaterial_Copy(pMaterial, pSrcMaterial[0]);
			pMod->viewsub[i].pMaterial = pMaterial;
		}
	}

	if (punit3d->u.u3d.pmod)
	{
		sy_release_moddef(punit3d->u.u3d.pmod);
		delete punit3d->u.u3d.pmod;
	}
	punit3d->u.u3d.pmod = pMod;
	vg_release(&NewFigure);
}

BASEGRAPH_DATARENDER_API void CalcBufferProjectSize(rectf & rtDes, vector3f * pDesVector3D, RECT & rtSrc, matrix4f * pTransMatrix)
{
	rectf rt = { rtSrc.left,rtSrc.top,rtSrc.right,rtSrc.bottom };
	CalcBufferProjectSize(rtDes, pDesVector3D, rt, pTransMatrix);
}

BASEGRAPH_DATARENDER_API void CalcBufferProjectSize(rectf & rtDes, vector3f * pDesVector3D, rectf & rtSrc, matrix4f * pTransMatrix)
{
	rtDes.left = 65535.0f;
	rtDes.top = 65535.0f;
	rtDes.right = -65535.0f;
	rtDes.bottom = -65535.0f;
	float3Set(pDesVector3D + 0, rtSrc.left, -rtSrc.top, 0);
	float3Set(pDesVector3D + 1, rtSrc.right, -rtSrc.top, 0);
	float3Set(pDesVector3D + 2, rtSrc.right, -rtSrc.bottom, 0);
	float3Set(pDesVector3D + 3, rtSrc.left, -rtSrc.bottom, 0);

	for (int i = 0; i < 4; i++)
	{
		pDesVector3D[i] *= *pTransMatrix;
		rtDes.left = min(rtDes.left, pDesVector3D[i].x);
		rtDes.top = min(rtDes.top, pDesVector3D[i].y);
		rtDes.right = max(rtDes.right, pDesVector3D[i].x);
		rtDes.bottom = max(rtDes.bottom, pDesVector3D[i].y);
	}
}

BASEGRAPH_DATARENDER_API void CalcPointProject(vector4f * pV4D, matrix4f * pTransMatrix, matrix4f * pProjectMatrix, vector4f * pvCenter)
{
	matrix4f	vCenterMatrix;
	vector4f vDes;
	float4MulMatQ(&vDes,			// 目标向量指针
		pV4D,			// 源向量指针
		pTransMatrix);	// 矩阵指针
	*pV4D = vDes;
	//calc_trans_point4D(pV4D,pTransMatrix);
	pV4D->x -= pvCenter->x;
	pV4D->y -= pvCenter->y;
	pV4D->z -= pvCenter->z;

	matrix4f_identity(&vCenterMatrix);
	vCenterMatrix._41 = pvCenter->x;
	vCenterMatrix._42 = pvCenter->y;
	vCenterMatrix._43 = pvCenter->z;

	float4MulMatQ(&vDes,			// 目标向量指针
		pV4D,			// 源向量指针
		&vCenterMatrix);	// 矩阵指针
	*pV4D = vDes;
}

BASEGRAPH_DATARENDER_API void calc_trans_group_tovector(OUT vector3f * pvpos, OUT vector3f * pvscale, OUT SYQuatn * pTcb, OUT vector3f * pEur, OUT vector3f * pcenter, IN double dframe, IN vector3f * pamendpos, IN vector3f * pamendscale, IN vector3f * pamendrotate)
{
	return;
}

BASEGRAPH_DATARENDER_API void calc_trans_group(OUT matrix4f * pvmatrix, OUT vector3f * pvpos, OUT vector3f * pvscale, OUT vector3f * pAngle, OUT vector3f * pcenter, IN double dFrame, IN vector3f * pivot, int nRotateOrder, IN vector3f * pamendpos, IN vector3f * pamendscale, IN vector3f * pamendrotate)
{
	//中心变换矩阵
	matrix4f matpivotE;
	matrix4f_identity(pvmatrix);

	if (dFrame != -1)
	{
		calc_trans_group_tovector(pvpos, pvscale, NULL, pAngle, pcenter, dFrame, pamendpos, pamendscale, pamendrotate);
		calc_trans_vector_tomatrix(pvmatrix, pvpos, pvscale, pAngle, pcenter, NULL, NULL, nRotateOrder);
	}

	if (pivot)
	{
		float3ToTransMatQ(&matpivotE, pivot);
		matrix4f_mul(pvmatrix, pvmatrix, &matpivotE);
	}
}

int  cell_get_group_ptcount(vectorcell*	srccell, int nstart, int type);

//单条二次曲线的扩展
int  conic_extend(SYArray* ptArray, pointf pt1, pointf pt2, pointf pt3, pointf startoffset, pointf endoffset, float fwidth, int ndirect, SYArray* lenArray = NULL, float* fAllLen = NULL)
{
	pointf pt12mid, pt23mid, pt13mid, ptmid;
	pointf ptmidnormal, ptmidoffset;

	pt12mid = (pt1 + pt2) * 0.5f;
	pt23mid = (pt2 + pt3) * 0.5f;
	pt13mid = (pt1 + pt3) * 0.5f;
	ptmid = (pt12mid + pt23mid) * 0.5f;
	float fdis = (ptmid.x - pt13mid.x) * (ptmid.x - pt13mid.x) + (ptmid.y - pt13mid.y) * (ptmid.y - pt13mid.y);
	if (fdis < 0.1f)
	{
		pointf* ptnewend = new pointf;
		*ptnewend = endoffset;
		if (*(DWORD*)&ptnewend->x == 0xffc00000)
		{
			assert(FALSE);
		}
		SYAR_Add(ptArray, ptnewend);
		if (lenArray)
		{
			point2f* pother = new point2f;
			*pother = pt3 + pt3 - *ptnewend;
			SYAR_Add(ptArray, pother);
			float len = float2Distance(&pt1, &pt2);
			*fAllLen += len;
			SYAR_Add(lenArray, (SYAR_ELEMENT)*(void**)fAllLen);


		}
	}
	else
	{
		line_vertical_normalizing(&ptmidnormal, &pt1, &pt3, ndirect);
		ptmidoffset = ptmid + ptmidnormal * fwidth;
		if (*(DWORD*)&ptmidoffset.x == 0xffc00000)
		{
			assert(FALSE);
			//_asm int 3;
		}
		conic_extend(ptArray, pt1, pt12mid, ptmid, startoffset, ptmidoffset, fwidth, ndirect, lenArray, fAllLen);
		conic_extend(ptArray, ptmid, pt23mid, pt3, ptmidoffset, endoffset, fwidth, ndirect, lenArray, fAllLen);
	}
	return 1;
}

int  conic_group_extend(OUT SYArray* ptArray, IN vectorcell* cell, IN  int nstart, float fwidth, int ndirect, SYArray* plenArray, float* pfalllen);

int  bezier_extend(OUT SYArray* ptArray,
	IN pointf pt1,
	IN pointf pt2,
	IN pointf pt3,
	IN pointf pt4,
	IN pointf pt1offset,
	IN pointf pt4offset,
	float fwidth,
	int ndirect, SYArray* lenArray, float* falllen)
{
	pointf pt12mid, pt23mid, pt34mid, pt14mid, ptmid;
	pointf ptmidleft, ptmidright;
	pointf ptmidnormal, ptmidoffset;

	pt12mid = (pt1 + pt2) * 0.5f;
	pt23mid = (pt2 + pt3) * 0.5f;
	pt34mid = (pt3 + pt4) * 0.5f;
	pt14mid = (pt1 + pt4) * 0.5f;
	ptmidleft = (pt12mid + pt23mid) * 0.5f;
	ptmidright = (pt23mid + pt34mid) * 0.5f;

	ptmid = (ptmidleft + ptmidright) * 0.5f;

	float fdis = (ptmid.x - pt14mid.x) * (ptmid.x - pt14mid.x) + (ptmid.y - pt14mid.y) * (ptmid.y - pt14mid.y);
	if (fdis < 0.01f)
	{
		//write to array
		pointf* ptnewend = new pointf;
		*ptnewend = pt4offset;

		SYAR_Add(ptArray, ptnewend);
		if (lenArray)
		{
			*falllen += float2Distance(&pt1, &pt4);
			SYAR_Add(lenArray, *(void**)falllen);

			point2f* ptother = new point2f;
			*ptother = pt4 + (pt4 - *ptnewend);
			SYAR_Add(ptArray, ptother);

		}

	}
	else
	{
		//mid point offset
		line_vertical_normalizing(&ptmidnormal, &ptmidleft, &ptmid, ndirect);
		ptmidoffset = ptmid + ptmidnormal * fwidth;
		if ((*(DWORD*)(&ptmidoffset.x)) == 0xffc00000)
		{
			int a = 0;
		}

		//continue;
		bezier_extend(ptArray, pt1, pt12mid, ptmidleft, ptmid, pt1offset, ptmidoffset, fwidth, ndirect, lenArray, falllen);
		bezier_extend(ptArray, ptmid, ptmidright, pt34mid, pt4, ptmidoffset, pt4offset, fwidth, ndirect, lenArray, falllen);
	}

	return 1;
}

//三次bezier曲线的扩展
int  bezier_group_extend(OUT SYArray* ptArray, IN vectorcell* cell, IN  int nstart, float fwidth, int ndirect, SYArray* lenArray, float* falllen, pointf* ptlast = NULL)
{
	int ncurvercount = cell_get_group_ptcount(cell, nstart, point_curve);

	//如果是非闭合曲线,需要在这里重新计算数目的合法性
	if (cell->type == uncloss_line)
	{
		if (nstart + ncurvercount > cell->npoint)
		{
			ncurvercount = cell->npoint - nstart;
		}
	}

	pointf pt1, pt2, pt3, pt4;
	pointf ptnormal;
	ptnormal.x = 0;
	ptnormal.y = 0;
	pointf startoffset, endoffset;
	int nindex = nstart;

	do {


		pt1 = cell->point[nindex%cell->npoint];
		pt2 = cell->point[(nindex + 1) % cell->npoint];
		pt3 = cell->point[(nindex + 2) % cell->npoint];
		pt4 = cell->point[(nindex + 3) % cell->npoint];

		if (pt1 == pt2 &&pt3 == pt4 && pt1 == pt4)
		{
			nindex += 3;
			continue;
		}
		if (pt1 == pt2)
		{
			if (pt2 != pt3)
				line_vertical_normalizing(&ptnormal, &pt2, &pt3, ndirect);
		}
		else
			line_vertical_normalizing(&ptnormal, &pt1, &pt2, ndirect);
		startoffset = pt1 + ptnormal * fwidth;

		if (ptlast&&startoffset == *ptlast)
		{
		}
		else if (ptlast)
		{
			SYArray ptarcarray;
			SYAR_Init(&ptarcarray);
			cell_makearc(&ptarcarray, pt1, *ptlast, startoffset, fwidth);
			int naddcount = SYAR_GetSize(&ptarcarray);
			for (int i = 0; i < naddcount; i++)
			{
				point2f* pt = (point2f*)SYAR_GetAt(&ptarcarray, i);
				SYAR_Add(ptArray, pt);
				if (lenArray)
				{
					point2f* pother = new point2f;
					*pother = pt1 + (pt1 - *pt);
					SYAR_Add(ptArray, pother);
					SYAR_Add(lenArray, *(void**)falllen);
				}
			}
			SYAR_RemoveAll(&ptarcarray);
		}

		if (pt3 == pt4)
		{
			if (pt2 == pt4)
				line_vertical_normalizing(&ptnormal, &pt1, &pt4, ndirect);
			else
				line_vertical_normalizing(&ptnormal, &pt2, &pt4, ndirect);
		}
		else
			line_vertical_normalizing(&ptnormal, &pt3, &pt4, ndirect);
		endoffset = pt4 + ptnormal * fwidth;

		bezier_extend(ptArray, pt1, pt2, pt3, pt4, startoffset, endoffset, fwidth, ndirect, lenArray, falllen);
		if (lenArray)
			ptlast = (pointf*)SYAR_GetAt(ptArray, SYAR_GetSize(ptArray) - 2);
		else
			ptlast = (pointf*)SYAR_GetAt(ptArray, SYAR_GetSize(ptArray) - 1);
		nindex += 3;

	} while (nindex <= nstart + ncurvercount - 4);

	if (nstart + ncurvercount >= cell->npoint)
		return -1;
	return nstart + ncurvercount - 1;
}

BASEGRAPH_DATARENDER_API int cell_extend_tri(OUT SYArray * ptarray, OUT SYArray * lenarray, IN vectorcell * srccell, IN int ndirect, IN float fwidth)
{
	if (fwidth <= 0)
		fwidth = 1.0f;
	float falllen = 0;
	//以下这几种情况退出
	unsigned long pttype = point_line;
	int nptcount = srccell->npoint;
	if (nptcount <= 0)
		return 0;
	if (srccell->pttype[0] == point_conic_start && nptcount <= 3)
	{
		return 0;
	}
	else if (srccell->pttype[0] == point_curve && nptcount <= 3)
	{
		return 0;
	}
	else if (srccell->pttype[0] == point_line && nptcount <= 1)
	{
		return 0;
	}

	pointf *ptend;
	int nalllen = 0;
	//cell's last offsetpt ,for calc last->start arc
	pointf* ptlast = NULL;
	pointf ptnormal;
	pointf ptlastoffset;
	if (srccell->type != uncloss_line) // 闭合情况下, 对闭合的最后一段线段进行偏移处理(其实是srccell->point[0]偏移后的点) 为做圆弧准备出ptlast点
	{
		// nprvindex指向最后一个点, 并且这个点不与起始点重合, 
		int nprvindex = nptcount - 1;
		while (srccell->point[0] == srccell->point[nprvindex])
		{
			nprvindex--;
			if (nprvindex <= 0)
				return 0;
		}

		// 终点与起点线段的垂线(已归一化)
		line_vertical_normalizing(&ptnormal, &srccell->point[nprvindex], &srccell->point[0], ndirect);
		// 对起点进行偏移
		ptlastoffset = srccell->point[0] + ptnormal * fwidth;
		ptlast = &ptlastoffset;
	}
	else // 非闭合
	{
		// 先找到第一段线段(起讫点不重合)
		int nindex = 0;
		pointf* ptin = &srccell->point[nindex];
		while (srccell->point[nindex] == srccell->point[(nindex + 1) % nptcount])
		{
			nindex++;
			continue;
		}
		// 对该线段进行偏移, 并将线段起点偏移后的点, 加入数组ptarray中
		point2f* ptstart = new point2f;
		pointf ptend;
		line_extend(*(point2f*)ptstart, ptend, srccell->point[nindex], srccell->point[(nindex + 1) % nptcount], ndirect, fwidth);

		//添加正反两个点
		SYAR_Add(ptarray, ptstart);
		if (lenarray)
		{
			point2f* ptother = new point2f;
			*ptother = srccell->point[nindex] + (srccell->point[nindex] - *ptstart);
			SYAR_Add(ptarray, ptother);
			SYAR_Add(lenarray, *(void**)&falllen);
		}
	}
	int nprecount = nptcount;
	if (srccell->type == uncloss_line)
	{
		nprecount = nptcount - 1;
	}
	for (int i = 0; i < nprecount;)
	{
		//make arc
		if (ptlast)
		{
			pointf ptoffset;
			if (srccell->point[i] == srccell->point[(i + 1) % nptcount])
			{
			}
			else
			{
				line_vertical_normalizing(&ptnormal, &srccell->point[i], &srccell->point[(i + 1) % nptcount], ndirect);
				ptoffset = srccell->point[i] + ptnormal * fwidth;
				float fdis1 = (ptlast->x - srccell->point[i].x)*(ptlast->x - srccell->point[i].x) + (ptlast->y - srccell->point[i].y)*(ptlast->y - srccell->point[i].y);
				float fdis2 = (ptoffset.x - srccell->point[i].x)*(ptoffset.x - srccell->point[i].x) + (ptoffset.y - srccell->point[i].y)*(ptoffset.y - srccell->point[i].y);
				if (fabs(1 - fdis1 / fdis2)>0.0005)
				{
					assert(FALSE);
					//_asm int 3;
				}
				SYArray ptarcarray;
				SYAR_Init(&ptarcarray);
				cell_makearc(&ptarcarray, srccell->point[i], *ptlast, ptoffset, fwidth);
				int naddcount = SYAR_GetSize(&ptarcarray);
				for (int n = 0; n < naddcount - 1; n++)
				{
					point2f* pt = (point2f*)SYAR_GetAt(&ptarcarray, n);
					SYAR_Add(ptarray, pt);
					if (lenarray)
					{
						point2f* pother = new point2f;
						*pother = srccell->point[i] + (srccell->point[i] - *pt);
						SYAR_Add(ptarray, pother);
						SYAR_Add(lenarray, *(void**)&falllen);
					}
				}
				point2f* ptArcEnd = (point2f*)SYAR_GetAt(&ptarcarray, naddcount - 1);
				delete ptArcEnd;
				SYAR_RemoveAll(&ptarcarray);

			}
		}
		pttype = srccell->pttype[i];
		if (pttype == point_line)				//如果是直线
		{
			pointf* ptin = &srccell->point[i];
			if (srccell->point[i] == srccell->point[(i + 1) % nptcount])
			{
				i++;
				continue;
			}
			pointf ptstart;
			ptend = new pointf;
			line_extend(ptstart, *ptend, srccell->point[i], srccell->point[(i + 1) % nptcount], ndirect, fwidth);
			if (*(DWORD*)&ptend->x == 0xffc00000)
			{
				int a = 0;
			}
			SYAR_Add(ptarray, ptend);
			if (lenarray)
			{
				int nSecond = (i + 1) % nptcount;
				falllen += float2Distance(&srccell->point[i], &srccell->point[nSecond]);
				SYAR_Add(lenarray, *(void**)&falllen);

				point2f* ptother = new point2f;
				*ptother = srccell->point[nSecond] + (srccell->point[nSecond] - *ptend);
				SYAR_Add(ptarray, ptother);
				int a = 0;
			}
			i++;
		}
		else if (pttype == point_conic_start)			//如果是2次曲线
		{
			i = conic_group_extend(ptarray, srccell, i, fwidth, ndirect, lenarray, &falllen);
		}
		else if (pttype == point_curve)			//如果是3次曲线
		{
			i = bezier_group_extend(ptarray, srccell, i, fwidth, ndirect, lenarray, &falllen);
		}
		if (i < 0)
			break;
		ptlast = (pointf*)SYAR_GetAt(ptarray, SYAR_GetSize(ptarray) - 2);
	}
	// 格式转换:　array --> cell
	//cell_makecell(ptarray,descell);

	/*descell->type = srccell->type;
	descell->width = srccell->width;*/
	return 1;
}

BASEGRAPH_DATARENDER_API void SYCurveCell_Init(vectorcell * pCell)
{
	if (pCell == NULL)
		return;
	memset(pCell, 0, sizeof(vectorcell));
}

BASEGRAPH_DATARENDER_API void SYCurveCell_Release(vectorcell * pCell)
{
	if (pCell == NULL)
		return;
	if (pCell->point)
	{
		delete[] pCell->point;
		pCell->point = NULL;
	}
	if (pCell->pttype)
	{
		delete[] pCell->pttype;
		pCell->pttype = NULL;
	}
	SYCurveCell_Init(pCell);
}
