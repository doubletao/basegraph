#include "stdafx.h"
#include "Common\CommonDataContainer.h"
#include "Common\VectorDef.h"
#include "Common\QuatnDef.h"
#include "Common\PlaneDef.h"
#include "Common\vectorgraph.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
void vg_copy(vectorgraph* des, const vectorgraph* src)
{
	vg_release(des);

	// 拷贝数据
	memcpy(des, src, sizeof(vectorgraph));
	if (des->ncell > 0)
	{
		des->cell = new vectorcell[des->ncell];
		for (int i=0; i<des->ncell; i++)
		{
			memcpy(&des->cell[i], &src->cell[i], sizeof(vectorcell));
			if (src->cell[i].npoint > 0)
			{
				des->cell[i].point = new point2f[src->cell[i].npoint];
				memcpy(des->cell[i].point, src->cell[i].point, sizeof(point2f)*src->cell[i].npoint);
				des->cell[i].pttype = new unsigned long[des->cell[i].npoint];
				memcpy(des->cell[i].pttype, src->cell[i].pttype, sizeof(unsigned long)*src->cell[i].npoint);
			}
		}
	}
}

// 拷贝vectorgraph(只拷贝非空cell)
void vg_copy_with_null_cell(vectorgraph* pdes, const vectorgraph* psrc)
{
	// 先释放目标vectorgraph
	vg_release(pdes);

	// 拷贝数据
	memcpy(pdes, psrc, sizeof(vectorgraph));
	pdes->ncell = 0;
	for (int i = 0; i < psrc->ncell; i++) 
	{
		if (psrc->cell[i].npoint > 2 || (psrc->cell[i].npoint > 1 && psrc->cell[i].type == uncloss_line)) 
		{
			(pdes->ncell)++;
		}
	}
	if (pdes->ncell > 0)
	{
		pdes->cell = new vectorcell[pdes->ncell];
		int j = 0;
		for (int i=0; i<pdes->ncell; i++)
		{
			while(j < psrc->ncell && (psrc->cell[j].npoint < 3 && psrc->cell[i].type != uncloss_line)) 
			{
				j++;
			}
			if (j >= psrc->ncell) 
			{
				break;
			}
			memcpy(&pdes->cell[i], &psrc->cell[j], sizeof(vectorcell));
			if (pdes->cell[i].npoint > 2 ||(psrc->cell[i].npoint > 1 && psrc->cell[i].type == uncloss_line))
			{
				pdes->cell[i].point = new pointf[pdes->cell[i].npoint];
				memcpy(pdes->cell[i].point, psrc->cell[j].point, sizeof(pointf)*pdes->cell[i].npoint);
				pdes->cell[i].pttype = new unsigned long[pdes->cell[i].npoint];
				memcpy(pdes->cell[i].pttype, psrc->cell[j].pttype, sizeof(unsigned long)*pdes->cell[i].npoint);
			}
			j++;
		}
	}
}

//cell copy
void cell_copy(vectorcell* pdescell,vectorcell* psrccell)
{
	memcpy(pdescell, psrccell, sizeof(vectorcell));
	if (psrccell->npoint > 0)
	{
		pdescell->point = new point2f[psrccell->npoint];
		memcpy(pdescell->point, psrccell->point, sizeof(point2f)*psrccell->npoint);
		pdescell->pttype = new unsigned long[psrccell->npoint];
		memcpy(pdescell->pttype, psrccell->pttype, sizeof(unsigned long)*psrccell->npoint);
	}
}

void vg_zoom(vectorgraph* vg,float zoomx,float zoomy,float zoomz,float centerx,float centery,float centerz)
{
	for (int i=0; i<vg->ncell; i++)
	{
		for(int k = 0; k < vg->cell[i].npoint; k ++)
		{
			vg->cell[i].point[k].x = (vg->cell[i].point[k].x - centerx) * zoomx + centerx;
			vg->cell[i].point[k].y = (vg->cell[i].point[k].y - centery) * zoomy + centery;

		}
		vg->cell[i].bound.left  = (vg->cell[i].bound.left - centerx) * zoomx + centerx;
		vg->cell[i].bound.right = (vg->cell[i].bound.right - centerx) * zoomx + centerx;
		vg->cell[i].bound.top   = (vg->cell[i].bound.top - centery) * zoomy + centery;
		vg->cell[i].bound.bottom= (vg->cell[i].bound.bottom - centery) * zoomy + centery;
	}
	vg->bound.left  = (vg->bound.left - centerx) * zoomx + centerx;
	vg->bound.right = (vg->bound.right - centerx) * zoomx + centerx;
	vg->bound.top   = (vg->bound.top - centery) * zoomy + centery;
	vg->bound.bottom= (vg->bound.bottom - centery) * zoomy + centery;
}

void vg_offset(vectorgraph* vg,float fx,float fy,float fz)
{
	for (int i=0; i<vg->ncell; i++)
	{
		for(int k = 0; k < vg->cell[i].npoint; k ++)
		{
			vg->cell[i].point[k].x = vg->cell[i].point[k].x + fx;
			vg->cell[i].point[k].y = vg->cell[i].point[k].y + fy;

		}
		vg->cell[i].bound.left  += fx;
		vg->cell[i].bound.right += fx;
		vg->cell[i].bound.top   += fy;
		vg->cell[i].bound.bottom+= fy;
	}
	vg->bound.left  += fx;
	vg->bound.right += fx;
	vg->bound.top   += fy;
	vg->bound.bottom+= fy;
}
void vg_sym(vectorgraph* vg,float fx)
{
	for (int i=0; i<vg->ncell; i++)
	{
		vg->cell[i].bound.left  = 65535;
		vg->cell[i].bound.right = -65535;
		for(int k = 0; k < vg->cell[i].npoint; k ++)
		{
			vg->cell[i].point[k].x = 2* fx - vg->cell[i].point[k].x;
			vg->cell[i].bound.left  = min(vg->cell[i].bound.left, vg->cell[i].point[k].x);
			vg->cell[i].bound.right = max(vg->cell[i].bound.right, vg->cell[i].point[k].x);
		}
		//vg->cell[i].bound.left  = 2* fx - vg->cell[i].bound.right;
		//vg->cell[i].bound.right = 2* fx - vg->cell[i].bound.left;
	}
	vg->bound.left  = 2* fx - vg->bound.right;
	vg->bound.right = 2* fx - vg->bound.left;
}
void vg_merge(vectorgraph* des, const vectorgraph* src1,vectorgraph* src2)
{
	vg_release(des);

	int ncellcount = src1->ncell + src2->ncell;
	if(ncellcount < 0)
		return;
	// 拷贝数据
	//memcpy(des, pSrcGraph, sizeof(vectorgraph));
	des->cell = new vectorcell[ncellcount];
	int i = 0;
	for(;i < src1->ncell;i ++)
	{
		cell_copy(&des->cell[i],&src1->cell[i]);
	}
	for(;i < ncellcount;i++)
	{
		cell_copy(&des->cell[i],&src2->cell[i - src1->ncell]);
	}
	des->ncell = ncellcount;
	des->bound.left = min(src1->bound.left,src2->bound.left);
	des->bound.top = min(src1->bound.top,src2->bound.top);
	des->bound.right = max(src1->bound.right,src2->bound.right);
	des->bound.bottom = max(src1->bound.bottom,src2->bound.bottom);
}

void vg_reverse(vectorgraph* vg)
{
	point2f pttemp;
	int ncellcount = vg->ncell;
	for(int i = 0;i < ncellcount;i ++)
	{
		vectorcell* pcell = &vg->cell[i];
		unsigned long* type = new unsigned long[pcell->npoint];
		int reccount = pcell->npoint/2;
		for(int i = 0;i< reccount;i ++)
		{
			pttemp = pcell->point[i];
			pcell->point[i] = pcell->point[pcell->npoint - 1 - i];
			pcell->point[pcell->npoint - 1 - i] = pttemp;

			type[i] = pcell->pttype[(pcell->npoint+ pcell->npoint - 1 - i - 1)%pcell->npoint];
			type[pcell->npoint - i - 1] =pcell->pttype[ (pcell->npoint + i -1)%pcell->npoint];
		}
		for(int i = 0;i < reccount;i ++)
		{
			pcell->pttype[i] = type[i];
		}
		delete[] type;
	}
}

void vg_rotate_matrix(vectorgraph* vg,matrix4f* matrix)
{
	//空graph不应该做任何处理。
	if (vg->ncell > 0)
	{
		vg->bound.left  = 65535.0f;
		vg->bound.top   = 65535.0f;
		vg->bound.right = -65535.0f;
		vg->bound.bottom= -65535.0f;
	}
	point3f vectorf;
	for (int i=0; i<vg->ncell; i++)
	{
		vg->cell[i].bound.left = 65535.0f;
		vg->cell[i].bound.top = 65535.0f;
		vg->cell[i].bound.right = -65535.0f;
		vg->cell[i].bound.bottom = -65535.0f;
		for(int k = 0; k < vg->cell[i].npoint; k ++)
		{
			vectorf.x = vg->cell[i].point[k].x;
			vectorf.y = vg->cell[i].point[k].y;
			vectorf.z = 0.0f;

			vectorf *= *matrix;

			vg->cell[i].point[k].x = vectorf.x;
			vg->cell[i].point[k].y = vectorf.y;

			vg->cell[i].bound.left  = min(vg->cell[i].bound.left,vectorf.x);
			vg->cell[i].bound.top   = min(vg->cell[i].bound.top,vectorf.y);
			vg->cell[i].bound.right = max(vg->cell[i].bound.right,vectorf.x);
			vg->cell[i].bound.bottom= max(vg->cell[i].bound.bottom,vectorf.y);
		}

		if(vg->cell[i].type == uncloss_line || vg->cell[i].type == close_line)
		{
			vg->cell[i].bound.left -= vg->cell[i].width;
			vg->cell[i].bound.top -= vg->cell[i].width;
			vg->cell[i].bound.right += vg->cell[i].width;
			vg->cell[i].bound.bottom += vg->cell[i].width;
		}

		vg->bound.left  = min(vg->bound.left,vg->cell[i].bound.left);
		vg->bound.top   = min(vg->bound.top,vg->cell[i].bound.top);
		vg->bound.right = max(vg->bound.right,vg->cell[i].bound.right);
		vg->bound.bottom= max(vg->bound.bottom,vg->cell[i].bound.bottom);
	}
}


void vg_rotate_angle(vectorgraph* vg,float x,float y,float z,float xoffset,float yoffset,float zoffset)
{
	point3f veuler = {x,y,z};
	SYQuatn vQ;
	matrix4f vMatrix;
	SYQuatn_FromEuler_LYXZ(&vQ,x,y,z);
	SYQuatn_ToMatrix4(&vMatrix,&vQ);
	vMatrix._41 = -xoffset;
	vMatrix._42 = -yoffset;
	vMatrix._43 = -zoffset;
	vg_rotate_matrix(vg,&vMatrix);
}

void cell_release(vectorcell* cell)
{

	delete [] cell->point;
	delete [] cell->pttype;
}
void vg_release(vectorgraph* graph)
{
	if (graph == NULL)
	{
		return;
	}
	for (int i=0; i<graph->ncell; i++)
	{
		cell_release(&graph->cell[i]);
		memset(&graph->cell[i], 0, sizeof(vectorcell));
	}
	if(graph->ncell && graph->cell)
	{
		delete [] graph->cell;
		graph->cell = NULL;
	}
	memset(graph, 0, sizeof(vectorgraph));
}
void __cdecl  vg_fromarray(vectorgraph* pvg,BaseARray* ptarray)
{
	int nCount = (int)BaseAR_GetSize(ptarray);
	pvg->ncell = 1;
	pvg->version = 1;
	pvg->cell = new vectorcell;
	pvg->cell->version = 1;
	pvg->cell->npoint = nCount;
	pvg->cell->point = new point2f[pvg->cell->npoint];
	pvg->cell->pttype = new unsigned long[pvg->cell->npoint];
	pvg->cell->bound.left = 65535.0f;
	pvg->cell->bound.top = 65535.0f;
	pvg->cell->bound.bottom = -65535.0f;
	pvg->cell->bound.right = -65535.0f;
	for(int i = 0;i < nCount;i ++)
	{
		point2lf* p2lf = (point2lf*)BaseAR_GetAt(ptarray,i);
		pvg->cell->point[i].x = (float)p2lf->x;
		pvg->cell->point[i].y = (float)p2lf->y;
		pvg->cell->pttype[i] = point_line;

		pvg->cell->bound.left = (float)min(pvg->cell->bound.left,p2lf->x);
		pvg->cell->bound.right = (float)max(pvg->cell->bound.right,p2lf->x);
		pvg->cell->bound.top = (float)min(pvg->cell->bound.top,p2lf->y);
		pvg->cell->bound.bottom = (float)max(pvg->cell->bound.bottom,p2lf->y);

	}
	pvg->bound = pvg->cell->bound;
	pvg->cell->type = close_anticlockwise_region;
	pvg->cell->width = 1;
}
int __cdecl vg_tobezier(BaseARray* ptarray,vectorgraph* pvg,size_t nIndex)
{
	int cellcount = pvg->ncell;
	if(!cellcount || nIndex >= cellcount)
		return 0;
	vectorcell* pcell = &pvg->cell[nIndex];
	for(int i = 0;i < pcell->npoint;i ++)
	{
		if(pcell->pttype[i] == point_line)
		{
			point3f* pbase1 = new point3f;
			pbase1->x = pcell->point[i].x;
			pbase1->y = pcell->point[i].y;
			pbase1->z = 0.0f;

			if(i == pcell->npoint - 1)
			{
				BaseAR_Add(ptarray,pbase1);
				break;
			}
			point3f* ctrl1 = new point3f;
			point3f* ctrl2 = new point3f;
			ctrl1->x = pbase1->x = pcell->point[i].x;
			ctrl1->y = pbase1->y = pcell->point[i].y;
			ctrl1->z = 0;
			ctrl2->x = pcell->point[i + 1].x;
			ctrl2->y = pcell->point[i + 1].y;
			ctrl2->z = 0;
			BaseAR_Add(ptarray,ctrl1);
			BaseAR_Add(ptarray,ctrl2);
		}
		else if(pcell->pttype[i] == point_curve_start)
		{
			point3f* pbase1 = new point3f;
			pbase1->x = pcell->point[i].x;
			pbase1->y = pcell->point[i].y;
			pbase1->z = 0;
			BaseAR_Add(ptarray,pbase1);

			int ncurvecount = 1;
			int k = i + 1;
			
			while(k < pcell->npoint && pcell->pttype[k] == point_curve)
			{
				point3f* ppt = new point3f;
				ppt->x = pcell->point[k].x;
				ppt->y = pcell->point[k].y;
				ppt->z = 0.0f;
				BaseAR_Add(ptarray,ppt);
				k++;
			}
			i = k - 1;
		}
	}
	return (int)BaseAR_GetSize(ptarray);

}
void __cdecl vg_from_point2f_bezier(vectorgraph* pvg,BaseARray* ptarray)
{
	int ncount = (int)BaseAR_GetSize(ptarray);
	memset(pvg,0,sizeof(vectorgraph));
	pvg->ncell  = 1;
	pvg->cell = new vectorcell;
	pvg->cell->npoint = ncount;
	pvg->cell->point = new point2f[ncount];
	pvg->cell->pttype = new unsigned long[ncount];
	pvg->cell->type = uncloss_line;
	pvg->cell->version = 1;
	pvg->cell->bound.left = 65535.0f;
	pvg->cell->bound.top = 65535.0f;
	pvg->cell->bound.right = -65535.0f;
	pvg->cell->bound.bottom = -65535.0f;
	point2f* pfirst = (point2f*)BaseAR_GetAt(ptarray,0);
	pvg->cell->point[0] = *pfirst;
	pvg->cell->pttype[0] = point_curve_start;
	int nindex = 0;
	for(int i = 1;i < ncount;i ++)
	{
		point2f* pt = (point2f*)BaseAR_GetAt(ptarray,i);
		pvg->cell->point[i].x = pt->x;
		pvg->cell->point[i].y = pt->y;
		pvg->cell->pttype[i] = point_curve;

		pvg->cell->bound.left = min(pvg->cell->bound.left,pt->x);
		pvg->cell->bound.right = max(pvg->cell->bound.right,pt->x);
		pvg->cell->bound.top = min(pvg->cell->bound.top,pt->y);
		pvg->cell->bound.bottom = max(pvg->cell->bound.bottom,pt->y);
	}
	pvg->bound = pvg->cell->bound;
}
void __cdecl vg_from_point3fpoly(vectorgraph* pvg,BaseARray* ptarray)
{
	int ncount = (int)BaseAR_GetSize(ptarray);
	memset(pvg,0,sizeof(vectorgraph));
	pvg->ncell  = 1;
	pvg->cell = new vectorcell;
	pvg->cell->npoint = ncount;
	pvg->cell->point = new point2f[ncount];
	pvg->cell->pttype = new unsigned long[ncount];
	pvg->cell->type = close_anticlockwise_region;
	pvg->cell->version = 1;
	pvg->cell->bound.left = 65535.0f;
	pvg->cell->bound.top = 65535.0f;
	pvg->cell->bound.right = -65535.0f;
	pvg->cell->bound.bottom = -65535.0f;
	int nindex = 0;
	for(int i = 0;i < ncount;i ++,nindex ++)
	{
		point3f* pt = (point3f*)BaseAR_GetAt(ptarray,i);
		pvg->cell->point[nindex].x = pt->x;
		pvg->cell->point[nindex].y = pt->y;
		pvg->cell->pttype[nindex] = point_line;
		pvg->cell->bound.left = min(pvg->cell->bound.left,pt->x);
		pvg->cell->bound.right = max(pvg->cell->bound.right,pt->x);
		pvg->cell->bound.top = min(pvg->cell->bound.top,pt->y);
		pvg->cell->bound.bottom = max(pvg->cell->bound.bottom,pt->y);
	}
	pvg->bound = pvg->cell->bound;
}

bool __cdecl vg_remove_cell(vectorgraph* vg, vectorcell* cell)
{
	//从vectorgraph中移除vectorcell
	for (int nc = 0; nc < vg->ncell; ++nc)
	{
		if (&(vg->cell[nc]) == cell)
		{
			int nNewCellCnt = vg->ncell - 1;
			if (nNewCellCnt > 0)
			{
				vectorcell* pNewCell = new vectorcell[nNewCellCnt];
				memset(pNewCell, 0, sizeof(vectorcell) * nNewCellCnt);
				int nCopy = 0;
				for (int c = 0; c < vg->ncell; ++c)
				{
					vectorcell* pOldCell = &(vg->cell[c]);
					if (pOldCell != cell)
					{
						cell_copy(&(pNewCell[nCopy]), pOldCell);
						++nCopy;
					}
					cell_release(pOldCell);
				}

				delete [] vg->cell;
				vg->cell = pNewCell;
				vg->ncell = nNewCellCnt;
			}
			else
			{
				delete [] vg->cell;
				vg->cell = NULL;
				vg->ncell = 0;
			}
			return true;
		}
	}

	return false;
}

