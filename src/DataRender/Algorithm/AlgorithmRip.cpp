#include "stdafx.h"
#include "DataRender\AlgorithmRip.h"
#include <math.h>
#include "DataRender\OffsetCurve.h"
#include "DataRender\RipRender.h"
#include "DataRender\Arc2BezierAPI.h"
#include "DataRender\LineAPI.h"
#include <stdio.h>
#include "..\..\..\include\DataRender\DataRenderUtityFunc.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
int		g_rip_nsubpixel = 8;
int		g_rip_nsubsh = 3;
int		g_rip_table_size = 16;
int*	g_riptable;
int*	g_rip_speed_querytable;
int		g_nTotalErea;

int  Filter[16][16]=
{
	{0,0,0,0,2,2,2,2,  2,2,2,2,0,0,0,0},
	{0,0,2,2,2,2,2,2,  2,2,2,2,2,2,0,0},
	{0,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,0},
	{0,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,0},
	{2,2,2,2,3,3,3,3,  3,3,3,3,2,2,2,2},
	{2,2,2,2,3,3,3,3,  3,3,3,3,2,2,2,2},
	{2,2,2,2,3,3,3,3,  3,3,3,3,2,2,2,2},
	{2,2,2,2,3,3,3,3,  3,3,3,3,2,2,2,2},

	{2,2,2,2,3,3,3,3,  3,3,3,3,2,2,2,2},
	{2,2,2,2,3,3,3,3,  3,3,3,3,2,2,2,2},
	{2,2,2,2,3,3,3,3,  3,3,3,3,2,2,2,2},
	{2,2,2,2,3,3,3,3,  3,3,3,3,2,2,2,2},
	{0,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,0},
	{0,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,0},
	{0,0,2,2,2,2,2,2,  2,2,2,2,2,2,0,0},
	{0,0,0,0,2,2,2,2,  2,2,2,2,0,0,0,0},
};
//半径
#define Common_PIXEL_START_OFFSET	((g_rip_table_size-g_rip_nsubpixel)>>1)
//扩展物理像素
#define Common_PIXEL_EXT_COUNT	((((g_rip_table_size-g_rip_nsubpixel)>>1)>>g_rip_nsubsh) +1)
////////////////////////////////////////////////////////////////////////
//这个结构用来存储计算过程中的临时行缓冲
struct rip_linebuf
{
	int				nidleindex;
	BOOL			bused;
	int				nline;	//行
	int				ncursuby;//当前子行
	int				nmin_x;
	int				nmax_x;
	BOOL			bWrite;
	unsigned long*	buf;//临时缓冲
	int				nwidth;//宽度
	rip_linebuf*	pNext;//链
	rip_linebuf*	pPrv;
};
struct rip_segline 
{
	int scecount;
};
struct rip_segclass
{
	int type;
	int start;
	int count;
};
struct rip_seg
{
	int i;
	int l;
	int r;
};
//////////////////////////////////////////////////////////////////////////
//这个类用来管理行的临时缓冲
class bufpool_mgr
{
public:
	bufpool_mgr(int nmaxslot,int nmaxcount,int bufsize,unsigned char*& pool,unsigned char* endpool);
	~bufpool_mgr();
public:
	int				m_nMinIndex;
	int				m_nMaxIndex;

	int				m_nMinIdel;
	//是空闲buf
	rip_linebuf*	m_pidelbuf;
	int				m_nbufcount;
	//是有效buf池
	rip_linebuf**	m_plinebuf;
	int				m_nslotcount;
	BOOL			m_bNeedRelease;

	void*			m_dwSegBuffer;
	int				m_nSize;

public:
	rip_linebuf* get_linebuf(int nindex);
	void release_linebuf(int nindex);
};

//////////////////////////////////////////////////////////////////////////
bufpool_mgr::bufpool_mgr(int nmaxslot,int nmaxcount,int bufsize,unsigned char*& pool,unsigned char* endpool)
{
	int nmaxsize = sizeof(rip_linebuf*) * nmaxslot 
		+ sizeof(rip_linebuf) * nmaxcount
		+ sizeof(DWORD) * bufsize * nmaxcount;

	m_bNeedRelease = FALSE;
	if(pool + nmaxsize >= endpool)
		m_bNeedRelease = TRUE;
	m_nslotcount = nmaxslot;
	if(m_bNeedRelease)
		m_plinebuf = new rip_linebuf*[nmaxslot];
	else
		m_plinebuf = (rip_linebuf**)pool_malloc(&pool,sizeof(rip_linebuf*) * nmaxslot,m_bNeedRelease);
	memset(m_plinebuf,0,sizeof(rip_linebuf*)*nmaxslot);

	m_nbufcount = nmaxcount;

	if(m_bNeedRelease)
		m_pidelbuf = new rip_linebuf[m_nbufcount];
	else
		m_pidelbuf = (rip_linebuf*)pool_malloc(&pool,sizeof(rip_linebuf) * nmaxcount,m_bNeedRelease);
	memset(m_pidelbuf,0,sizeof(rip_linebuf)*nmaxcount);
	
	for(int i = 0;i < nmaxcount;i ++)
	{
		if(m_bNeedRelease)
			m_pidelbuf[i].buf = new DWORD[bufsize];
		else
			m_pidelbuf[i].buf = (DWORD*)pool_malloc(&pool,sizeof(DWORD) * bufsize,m_bNeedRelease);;//new DWORD[bufsize];
		m_pidelbuf[i].nidleindex = i;
		memset(m_pidelbuf[i].buf,0,sizeof(DWORD)*bufsize);
		m_pidelbuf[i].nwidth = bufsize;
	}
	m_nSize = bufsize * 6 * 4;
	m_dwSegBuffer = new BYTE[m_nSize];
	m_nMinIdel = 0;
}
bufpool_mgr::~bufpool_mgr()
{
	if(m_bNeedRelease)
	{
		if(m_plinebuf)
		{
			delete[] m_plinebuf;
			m_plinebuf = NULL;
		}
		if(m_pidelbuf)
		{
			for(int i = 0 ;i < m_nbufcount;i ++)
			{
				delete[] m_pidelbuf[i].buf;
				m_pidelbuf[i].buf = NULL;
			}
			delete m_pidelbuf;
			m_pidelbuf = NULL;
		}
	}
	if(m_dwSegBuffer)
		delete[] m_dwSegBuffer;
	m_dwSegBuffer = NULL;
}
rip_linebuf* bufpool_mgr::get_linebuf(int nindex)
{
	if(nindex < 0 ||nindex >= m_nslotcount)
		return NULL;
	if(m_plinebuf[nindex])
		return m_plinebuf[nindex];
	if(m_nMinIdel < 0 || m_nMinIdel >= m_nbufcount)
		return NULL;

	m_plinebuf[nindex] = &m_pidelbuf[m_nMinIdel];
	m_plinebuf[nindex]->bused = TRUE;
	if(nindex == 0 || !m_plinebuf[nindex - 1] )
	{
		m_nMinIndex = nindex;
	}
	m_nMaxIndex = nindex;

	BOOL bFind = FALSE;
	for (int i = m_nMinIdel + 1;i < m_nbufcount;i ++)
	{
		if(!m_pidelbuf[i].bused)
		{
			m_nMinIdel = i;
			bFind = TRUE;
			break;
		}
	}
	if(!bFind)
		m_nMinIdel = m_nbufcount;

	return m_plinebuf[nindex];
}
void bufpool_mgr::release_linebuf(int nindex)
{
	if(m_plinebuf[nindex])
	{
		if(m_nMinIdel > m_plinebuf[nindex]->nidleindex)
			m_nMinIdel = m_plinebuf[nindex]->nidleindex;
		m_plinebuf[nindex]->bused = FALSE;
		m_plinebuf[nindex]->bWrite = FALSE;
		memset(m_plinebuf[nindex]->buf,0,sizeof(DWORD)*m_plinebuf[nindex]->nwidth);
		m_plinebuf[nindex] = 0;
		if(nindex + 1 < m_nslotcount && m_plinebuf[nindex + 1] )
			m_nMinIndex = nindex + 1;
		else
		{
			m_nMinIndex = -1;
			m_nMaxIndex = -1;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
int		GetAddTableValue(int nLine,int nStart,int nEnd)
{
	int nAddValue=0;
	int* ptable = g_riptable + nLine * g_rip_table_size;
	for(int i = nStart;i <= nEnd;i ++)
	{
		nAddValue+=ptable[i];
	}
	return nAddValue;
}
int		rip_init_table()
{
	g_riptable = new int[g_rip_table_size * g_rip_table_size];
	g_rip_speed_querytable = new int[g_rip_table_size * g_rip_table_size * g_rip_table_size];
	memset(g_rip_speed_querytable,0,sizeof(int)*g_rip_table_size * g_rip_table_size * g_rip_table_size);
	float fGrade = 0;
	float fTableRad = g_rip_table_size * 0.5f;
	float ftableCenterX = (g_rip_table_size -1) * 0.5f;
	float ftableCenterY = (g_rip_table_size -1) * 0.5f;

	int* ptable = g_riptable;
	float* fTempTable = new float[g_rip_table_size * g_rip_table_size];
	float * pfloattable = fTempTable;
	int* pfloatsrc = (int*)Filter;

	for(int i=0;i<g_rip_table_size ;i++)
	{
		for(int j=0;j<g_rip_table_size ;j++)
		{
			float fx =(float)(i-ftableCenterX);
			float fy =(float)(j-ftableCenterY);
			float fdis = sqrt((fx*fx+fy*fy));
			pfloattable[j] = (1.0f-fdis/fTableRad) * 100;
			if(pfloattable[j]<0.0f)
				pfloattable[j] = 0;
			pfloattable[j] = pfloatsrc[j];
			fGrade+=pfloattable[j];
		}
		pfloattable += g_rip_table_size;
		pfloatsrc += g_rip_table_size;
	}
	g_nTotalErea = Common_FToI(fGrade);
	pfloattable = fTempTable;
	ptable = g_riptable;
	float fscale = 4096.0f/fGrade;
	for(int i=0;i<g_rip_table_size ;i++)
	{
		for(int j=0;j<g_rip_table_size ;j++)
		{
			ptable[j] = (int)ceil(pfloattable[j] * fscale);
			if(ptable[j]<0.0f)
				ptable[j] = 0;
			fGrade+=ptable[j];
		}
		ptable += g_rip_table_size;
		pfloattable += g_rip_table_size;
	}
	g_nTotalErea = 4096;
	
	int* pspeedtable = g_rip_speed_querytable;
	for(int i=0;i<g_rip_table_size ;i++)
	{
		for(int j=0;j<g_rip_table_size ;j++)
		{
			for(int k= j+1;k<g_rip_table_size ;k++)
			{
				pspeedtable[k] = GetAddTableValue(i,j,k);
			}
			pspeedtable += g_rip_table_size;
		}
	}
	delete []fTempTable;
	fTempTable = NULL;
	return TRUE;
}

int rip_release_table()
{
	if(g_riptable)
		delete g_riptable;
	if(g_rip_speed_querytable)
		delete g_rip_speed_querytable;
	return 1;
}
struct rip_edge_node
{
	pointf	start;	//开始
	pointf	end;		//结束
	int		dir;		//方向
	float	fk;		//x/y斜率
	float	fcurx;
	int		canrelease;
	//链表
	rip_edge_node* pNext;
	rip_edge_node* pPrv;
	rip_edge_node* pXNext;
	rip_edge_node* pXPrv;
	rip_edge_node()
	{
		memset(this, 0, sizeof(rip_edge_node));
	}
};

typedef rip_edge_node rip_edge_node_x_list;
rip_edge_node* rip_malloc_node(unsigned char*& pool,unsigned char* poolend);
//把矩形根据光栅化的要求进行扩展
void rip_extend_bound(int* pleft,int* ptop,int* pbottom,int* pright)
{
	int nextend = Common_PIXEL_START_OFFSET/4;
	*pleft -= nextend;
	*ptop  -= nextend;
	*pbottom += nextend;
	*pright += nextend;
}

void	rip_extend_fbound(rectf* prect,int* pleft,int* ptop,int* pright,int* pbottom)
{
	int nextend = (Common_PIXEL_START_OFFSET>>2) + ((Common_PIXEL_START_OFFSET&0x00000003)?1:0);
	*pleft = (int)floor(prect->left);
	*ptop = (int)floor(prect->top);
	*pright = (int)ceil(prect->right);
	*pbottom = (int)ceil(prect->bottom);

	*pleft -= nextend + 1;
	*ptop  -= nextend + 1;
	*pbottom += nextend + 1;
	*pright += nextend + 1;
}
void	rip_extend_fbound_long(rectf* prect,long* pleft,long* ptop,long* pright,long* pbottom)
{
	int nextend = (Common_PIXEL_START_OFFSET>>2) + ((Common_PIXEL_START_OFFSET&0x00000003)?1:0);
	*pleft = (int)floor(prect->left);
	*ptop = (int)floor(prect->top);
	*pright = (int)ceil(prect->right);
	*pbottom = (int)ceil(prect->bottom);

	*pleft -= nextend;
	*ptop  -= nextend;
	*pbottom += nextend;
	*pright += nextend;
}
void BASEGRAPH_DATARENDER_API rip_figure_mul_matrix(vectorgraph, void * pmatrix)
{
	assert(false);
}
//得到
int rip_getcellbound(IN	vectorcell* cell,
					  OUT	rectf* rect)
{
	if(cell->npoint <= 0)
		return 0;
	rect->left = rect->right = cell->point[0].x;
	rect->top = rect->bottom = cell->point[0].y;
	for(int i = 0 ; i < cell->npoint; i++)
	{
		rect->left = min(rect->left,cell->point[i].x);
		rect->right = max(rect->right,cell->point[i].x);
		rect->top = min(rect->top,cell->point[i].y);
		rect->bottom = max(rect->bottom,cell->point[i].y);
	}
	return 1;
}
void rip_scalefigure(IN	vectorgraph* figure,float fx,float fy)
{
	for(int i = 0 ;i< figure->ncell;i++)
	{
		for(int j = 0 ; j < figure->cell[i].npoint;j ++)
		{
			figure->cell[i].point[j].x *= fx;
			figure->cell[i].point[j].y *= fy;
		}
		figure->cell[i].bound.left *= fx;
		figure->cell[i].bound.right *= fx;
		figure->cell[i].bound.top *= fy;
		figure->cell[i].bound.bottom *= fy;
	}
	figure->bound.left *= fx;
	figure->bound.right *= fx;
	figure->bound.top *= fy;
	figure->bound.bottom *= fy;
}
void rip_offsetfigure(IN	vectorgraph* figure,float fx,float fy)
{
	for(int i = 0 ;i< figure->ncell;i++)
	{
		for(int j = 0 ; j < figure->cell[i].npoint;j ++)
		{
			figure->cell[i].point[j].x += fx;
			figure->cell[i].point[j].y += fy;
		}
		figure->cell[i].bound.left += fx;
		figure->cell[i].bound.right += fx;
		figure->cell[i].bound.top += fy;
		figure->cell[i].bound.bottom += fy;
	}
	figure->bound.left += fx;
	figure->bound.right += fx;
	figure->bound.top += fy;
	figure->bound.bottom += fy;
}
int rip_getfigurebound(IN	vectorgraph* figure,OUT	rectf* rect)
{
	memcpy(rect,&figure->bound,sizeof(rectf));
	return 1;
}
void rip_add_linebuf_node(rip_linebuf*& header,rip_linebuf* pnode)
{
	if(!header)
	{
		header = pnode;
		pnode->pNext = NULL;
		pnode->pPrv = NULL;
		return ;
	}
	rip_linebuf* pLast = header;
	while(pLast)
	{
		if(pLast->pNext)
		{
			pLast = pLast->pNext;
			continue;
		}
		pLast->pNext = pnode;
		pnode->pPrv = pLast;
		pnode->pNext = NULL;
		break;
	}
}
//在横向链表插入节点
void rip_insertnode(rip_edge_node** pNodeHeader,rip_edge_node* pnode)
{
	if(!*pNodeHeader)
	{
		pnode->pPrv = NULL;
		pnode->pNext = NULL;
		*pNodeHeader = pnode;
	}
	else
	{
		rip_edge_node* pheader = *pNodeHeader;
		while(pheader->start.x < pnode->start.x)
		{
			if(pheader->pNext)
			{
				pheader = pheader->pNext;
			}
			else
			{
				pheader->pNext = pnode;
				pnode->pPrv = pheader;
				pnode->pNext = NULL;
				return;
			}
		}
		if(pheader->start.x == pnode->start.x)
		{
			if(pheader->fk < pheader->fk)
			{
				pnode->pNext = pheader->pNext;
				pheader->pNext = pnode;
				pnode->pPrv = pheader;
				return ;
			}
		}
		pnode->pPrv = pheader->pPrv;
		pheader->pPrv = pnode;
		pnode->pNext = pheader;
		if(pheader == *pNodeHeader)
		{
			*pNodeHeader = pnode;
		}
		else
			pnode->pPrv->pNext = pnode;
	}
}
BOOL rip_insert_x_before(rip_edge_node_x_list*& pNodexheader,rip_edge_node_x_list* pnode_x)
{
	pnode_x->pXPrv = NULL;
	pnode_x->pXNext = NULL;
	if(pNodexheader)
	{
		pnode_x->pXNext = pNodexheader;
		pNodexheader->pXPrv = pnode_x;
		pnode_x->pXPrv = NULL;
	}
	pNodexheader = pnode_x;
	return TRUE;
}
BOOL rip_remove_x_node(rip_edge_node_x_list*& pNodexheader,rip_edge_node_x_list* pnode_x)
{
	if(pnode_x == pNodexheader)
	{
		pNodexheader = pnode_x->pXNext;
		if(pNodexheader)
			pNodexheader->pXPrv = NULL;
	}
	else
	{
		rip_edge_node_x_list* pNext = pnode_x->pXNext;
		if(pnode_x->pXNext)
			pnode_x->pXNext->pXPrv = pnode_x->pXPrv;
		if(pnode_x->pXPrv)
			pnode_x->pXPrv->pXNext = pnode_x->pXNext;
	}
	return TRUE;
}
//x方向节点排序
void rip_sort_x_node(rip_edge_node_x_list*& pNodexheader)
{
	rip_edge_node_x_list* presultheader = NULL;
	rip_edge_node_x_list* pnode_x = pNodexheader;
	while(pnode_x)
	{
		rip_edge_node_x_list* pnode_x_cmp = pnode_x->pXNext;
		while(pnode_x_cmp)
		{
			//找最大值
			if(pnode_x->fcurx < pnode_x_cmp->fcurx)
			{
				pnode_x = pnode_x_cmp;
			}
			pnode_x_cmp = pnode_x_cmp->pXNext;
		}
		rip_remove_x_node(pNodexheader,pnode_x);
		rip_insert_x_before(presultheader,pnode_x);
		pnode_x = pNodexheader;
	}
	pNodexheader = presultheader;
}

void rip_remove_node_x(rip_edge_node_x_list*& pNodexheader,int y)
{
	if(!pNodexheader)
		return;
	rip_edge_node_x_list* pnode_x = pNodexheader;
	while(pnode_x)
	{
		//如果该线段已经计算结束,那么需要删除该线段
		if(pnode_x->end.y < y + 1)
		{
			rip_remove_x_node(pNodexheader,pnode_x);
			rip_edge_node_x_list* ptemp = pnode_x;
			pnode_x = pnode_x->pXNext;
			ptemp->pXNext = NULL;
			ptemp->pXPrv = NULL;
			continue;
		}
		pnode_x = pnode_x->pXNext;
	}
}
void rip_update_node_x(rip_edge_node_x_list*& pNodexheader,int y)
{
	if(!pNodexheader)
		return;
	rip_edge_node_x_list* pnode_x = pNodexheader;
	while(pnode_x)
	{
		//计算该线段的当前x位置
		pnode_x->fcurx += pnode_x->fk;
		pnode_x = pnode_x->pXNext;
	}
}

//添临时行缓冲
//DWORD* dwlinebuf 行缓冲
//int nsubstartx 填充行开始
//int nsubendx 填充结束
#define mod_g_rip_nsubpixel(x) ((g_rip_nsubpixel==4)?((x)&0x3):((x)%g_rip_nsubpixel))
#define div_g_rip_nsubpixel(x) ((g_rip_nsubpixel==4)?((x)>>2):((x)/g_rip_nsubpixel))
void rip_calcfillseg(int nsubstartx,int nsubendx,void* segbuf,int& min,int& max)
{
	////////////////////////////////////////////////////////////////////////
	//新加部分，清晰计算过程
	int nstart_subleft = nsubstartx - Common_PIXEL_START_OFFSET;
	int nstart_subright = nsubstartx + Common_PIXEL_START_OFFSET;
	int nstart_left = (nstart_subleft) >> g_rip_nsubsh;
	int nstart_right = nstart_subright >> g_rip_nsubsh;
	int nstart_full = nstart_right + 1;
	if(nstart_subright > nsubendx)
		nstart_subright  =  nsubendx - 1;

	int nend_subleft = nsubendx - Common_PIXEL_START_OFFSET;
	int nend_subright = nsubendx + Common_PIXEL_START_OFFSET;
	int nend_left = (nend_subleft) >> g_rip_nsubsh;
	int nend_right = nend_subright >> g_rip_nsubsh;
	int nend_full = nend_left - 1;

	rip_segline* pline = (rip_segline*)segbuf;
	pline->scecount = 3;
	pline ++;
	rip_segclass* pclass1 = (rip_segclass*)pline;
	pclass1->count = nstart_right - nstart_left + 1;
	pclass1->type = 0;
	pclass1->start = nstart_left;
	pclass1++;
	rip_seg* pseg = (rip_seg*)pclass1;

	min = min(nstart_left,nend_left);
	max = max(nstart_right,nend_right);

	for(int i = nstart_left ;i <= nstart_right;i ++)
	{
		
		int start = (i << g_rip_nsubsh) - Common_PIXEL_START_OFFSET;
		pseg->r = min((i << g_rip_nsubsh) + g_rip_nsubpixel + Common_PIXEL_START_OFFSET - 1,
			nsubendx);
		pseg->i = i;
		pseg->r = pseg->r - start;
		pseg->l = nsubstartx - start;
		pseg++;
	}
	rip_segclass* pclass2 = (rip_segclass*)pseg;
	pclass2->count = nend_right - nend_left + 1;
	pclass2->type = 0;
	pclass2->start = nend_left;
	pclass2 ++;
	pseg = (rip_seg*)pclass2;
	//后
	for(int i = nend_left ;i <= nend_right;i ++)
	{
		int start = (i << g_rip_nsubsh) - Common_PIXEL_START_OFFSET;
		pseg->l = max(start,nsubstartx);
		pseg->r = min(nsubendx,(i << g_rip_nsubsh) + g_rip_nsubpixel + Common_PIXEL_START_OFFSET - 1);
		pseg->i = i;
		pseg->r = pseg->r - start;
		pseg->l = pseg->l - start;
		pseg++;
	}
	rip_segclass* pclass3 = (rip_segclass*)pseg;
	pclass3->start = nstart_full;
	pclass3->count = nend_full - nstart_full + 1;
	pclass3->type = 1;
}

void rip_x_filllinebuf_byseg(DWORD* dwlinebuf,int nlinewidth,void* segbuf,int suby)
{
	int* psubtable = g_rip_speed_querytable + suby * g_rip_table_size * g_rip_table_size;
	int* ptempsubtable = psubtable;

	rip_segline* pline = (rip_segline*)segbuf;
	rip_segclass* pclass = (rip_segclass*)(pline+1);
	for(int i = 0;i < pline->scecount;i ++)
	{
		rip_seg* pseg = (rip_seg*)(pclass + 1);
		if(pclass->type == 0)
		{
			for(int k = 0;k < pclass->count;k ++)
			{
				ptempsubtable = psubtable + g_rip_table_size * pseg->l + pseg->r;
				dwlinebuf[pseg->i] += *ptempsubtable;
				pseg++;
			}
			
		}
		else if(pclass->count > 0)
		{
			ptempsubtable = psubtable + g_rip_table_size - 1;
			DWORD* plinebuf = &dwlinebuf[pclass->start];

			int count = pclass->count;
			int acount = pclass->count>>2;
			int mcount = count - (acount<<2);
			_rip_x_filllinebuf_x64(ptempsubtable, plinebuf,	acount, mcount);
		}
		pclass = (rip_segclass*)pseg;
	}
}

void rip_x_filllinebuf(DWORD* dwlinebuf,int nlinewidth,int nsubstartx,int nsubendx,int suby,int &min,int& max)
{
	int* psubtable = g_rip_speed_querytable + suby * g_rip_table_size * g_rip_table_size;
	int* ptempsubtable = psubtable;

	int tablerad = g_rip_table_size >> 1;

////////////////////////////////////////////////////////////////////////
//新加部分，清晰计算过程
	int nstart_subleft = nsubstartx - Common_PIXEL_START_OFFSET;
	int nstart_subright = nsubstartx + Common_PIXEL_START_OFFSET;
	int nstart_left = (nstart_subleft) >> g_rip_nsubsh;
	int nstart_right = nstart_subright >> g_rip_nsubsh;
	int nstart_full = nstart_right + 1;
	if(nstart_subright > nsubendx)
		nstart_subright  =  nsubendx - 1;


	int nend_subleft = nsubendx - Common_PIXEL_START_OFFSET;
	int nend_subright = nsubendx + Common_PIXEL_START_OFFSET;
	int nend_left = (nend_subleft) >> g_rip_nsubsh;
	int nend_right = nend_subright >> g_rip_nsubsh;
	int nend_full = nend_left - 1;
	

	min = min(nstart_left,nend_left);
	max = max(nstart_right,nend_right);
	int nL , nR;
	//前
	for(int i = nstart_left ;i <= nstart_right;i ++)
	{
		int start = (i << g_rip_nsubsh) - Common_PIXEL_START_OFFSET;
		nR = min((i << g_rip_nsubsh) + g_rip_nsubpixel + Common_PIXEL_START_OFFSET - 1,
			nsubendx);
		nR = nR - start;
		nL = nsubstartx - start;
		ptempsubtable = psubtable + g_rip_table_size * nL + nR;
		dwlinebuf[i] += *ptempsubtable;
	}

	//后
	for(int i = nend_left ;i <= nend_right;i ++)
	{
		int start = (i << g_rip_nsubsh) - Common_PIXEL_START_OFFSET;
		nL = max(start,nsubstartx);
		nR = min(nsubendx,(i << g_rip_nsubsh) + g_rip_nsubpixel + Common_PIXEL_START_OFFSET - 1);
		nR = nR - start;
		nL = nL - start;
		ptempsubtable = psubtable + g_rip_table_size * nL + nR;
		dwlinebuf[i] += *ptempsubtable;
	}


	ptempsubtable = psubtable + g_rip_table_size - 1;
	DWORD* plinebuf = &dwlinebuf[nstart_full];
	int count = nend_full - nstart_full + 1;
	int acount = count/4;

	int mcount = count - (acount<<2);
	if(count > 0)
	{
		_rip_x_filllinebuf_x64(ptempsubtable, plinebuf,	acount, mcount);
	}
}
void rip_x_node(rip_edge_node_x_list*& pNodexheader,
				int y,int suby,
				bufpool_mgr* pmgr,
				int& nminx,
				int& nmaxx)
{
	int tempmin,tempmax;
	int nsub = suby;
	rip_linebuf* pbuf = NULL;
	int ndirref = 0;
	int startx = 0,endx = 0;
	int ntestcount = 0;
	rip_edge_node_x_list* pNode = pNodexheader;
	while(pNode)
	{
		if(ndirref == 0)
			startx = (int)pNode->fcurx;

		ndirref += pNode->dir;

		if(ndirref == 0)
			endx = (int)pNode->fcurx;

		pNode = pNode->pXNext;
		if(ndirref!=0)
			continue;
		startx = max(0,startx);
		endx = max(0,endx);
		rip_calcfillseg(startx,endx,pmgr->m_dwSegBuffer,tempmin,tempmax);
		nminx = min(tempmin,nminx);
		nmaxx = max(tempmax,nmaxx);
		for(int k = pmgr->m_nMinIndex; k <= pmgr->m_nMaxIndex; k ++)
		{
			rip_linebuf* pbuf = pmgr->get_linebuf(k);
			if(!pbuf)
				continue;

			pbuf->nmin_x = min(tempmin,pbuf->nmin_x);
			pbuf->nmax_x = max(tempmax,pbuf->nmax_x);

			//计算最大最小值
			pbuf->bWrite = TRUE;
			//printf("%d-Before-->pmgr->m_dwSegBuffer = 0x%08x\n",k,(DWORD)pmgr->m_dwSegBuffer);
			//rip_x_filllinebuf(pbuf->buf,pbuf->nwidth,startx,endx,pbuf->ncursuby,tempmin,tempmax);
			rip_x_filllinebuf_byseg(pbuf->buf,pbuf->nwidth,pmgr->m_dwSegBuffer,pbuf->ncursuby);
			//printf("%d-After-->pmgr->m_dwSegBuffer = 0x%08x\n",k,(DWORD)pmgr->m_dwSegBuffer);
		}
		ntestcount++;
	}
	for(int k = pmgr->m_nMinIndex; k <= pmgr->m_nMaxIndex; k ++)
	{
		rip_linebuf* pbuf = pmgr->get_linebuf(k);
		if(!pbuf)
			continue;
		pbuf->ncursuby++;
	}
}
//添加新的结点到x方向链表
void rip_addnode_x(rip_edge_node_x_list*& pNodexheader,rip_edge_node* pnodeheader,int y)
{
	rip_edge_node* pnode = pnodeheader;
	while(pnode)
	{
		if( pnode->end.y < y + 1  || (int)pnode->start.y == (int)pnode->end.y)
		{
			pnode = pnode->pNext;
			continue;
		}
		rip_edge_node_x_list* pnode_x = pnode;//new rip_edge_node_x_list;
		//pnode_x->pnode = pnode;
		pnode_x->pXNext = NULL;
		pnode_x->pXPrv = NULL;
		pnode_x->fcurx = pnode->start.x;
		rip_insert_x_before(pNodexheader,pnode);
		pnode = pnode->pNext;
	}
}
static const SYSSE_DECLARE(DWORD) gc_dwMaskDLB_temp[4] = { 0x00ff00ff, 0x00ff00ff, 0x000ff0ff, 0x00ff00ff };
static const SYSSE_DECLARE(DWORD) gc_dwScale_temp[4] = { 0x0ff00ff0, 0x0ff00ff0, 0x00ff0ff0, 0x00ff0ff0 };

void rip_update_buf_byte(unsigned char*	buf,
					int nwidth,
					int nheight,
					int ny,
					int nsuby,
					bufpool_mgr* pmgr,
					int nrad,
					int ntop,int bForceWrite)
{

	//计算是否有行出
	rip_linebuf* pbuf = NULL;
	unsigned char* pmaskbuf = NULL;

	long lalpha = 0;
	int nIndex = pmgr->m_nMinIndex;
	BOOL bWrite = TRUE;
	while(bWrite)
	{
		pbuf = pmgr->get_linebuf(nIndex);
		if(!pbuf)
			break;
		//一行的运算结束,需要累加到目标缓冲中
		if((bForceWrite &&pbuf->bWrite) || (pbuf && pbuf->ncursuby >= g_rip_table_size) )
		{
			if(pbuf->nmin_x > pbuf->nmax_x)
			{
				int t = pbuf->nmin_x;
				pbuf->nmax_x = t;
				pbuf->nmin_x = pbuf->nmax_x;
			}
			int nminstart = max(0,pbuf->nmin_x);
			nminstart = min(nwidth,nminstart);
			int nmaxend = min(pbuf->nmax_x,nwidth - 1);
			nmaxend = max(0,nmaxend);


			BYTE*  pdesbuf = buf + nwidth * (pmgr->m_nMinIndex) + nminstart;
			DWORD* psrcbuf = pbuf->buf + nminstart;

			int count = nmaxend - nminstart + 1;
			if(((DWORD)pdesbuf)&0x3)
			{
				int temcount = min(count,((DWORD)pdesbuf)&0x3);
				for(int i = 0;i < temcount;i ++)
				{
					if(*psrcbuf > (DWORD)g_nTotalErea)
					{
						*pdesbuf = 255;
					}
					else if(*psrcbuf == 0)
					{
						*pdesbuf = 0;
					}
					else
						*pdesbuf = ((255**psrcbuf+0x0ff0)>>12)&0xff;
					//*pdesbuf = (BYTE)lalpha;
					psrcbuf += 1;
					pdesbuf += 1;
					count -- ;
				}
			}
			int acount = count/4;
			int mcount = count%4;
			if(count > 0)
			{
				_rip_update_buf_byte_x64(g_nTotalErea,psrcbuf,pdesbuf,gc_dwScale_temp,acount,mcount);
			}
			pmgr->release_linebuf(pmgr->m_nMinIndex);
		}
		if(bForceWrite)
		{
			nIndex ++;
			bWrite = TRUE;
		}
		else
			bWrite = FALSE;
	}
	if(bForceWrite)
		return;
	//如果没有现存
	if(pmgr->m_nMaxIndex >= 0 && pmgr->m_nMaxIndex < nheight - 1)
	{
		if(((pmgr->m_nMaxIndex + 1 - ny) << g_rip_nsubsh/** g_rip_nsubpixel*/) - nsuby -1<= nrad)
		{
			pbuf = pmgr->get_linebuf(pmgr->m_nMaxIndex + 1);
 			pbuf->ncursuby = 0;
			pbuf->bWrite = FALSE;
			pbuf->nmin_x = pbuf->nwidth;
			pbuf->nmax_x = 0;
		}
	}
	else if(pmgr->m_nMaxIndex < 0 && pmgr->m_nMaxIndex < 0)
	{
		//算出下一行
		//前面已经累加不用再次计算
		ny += (nsuby + 1)>>g_rip_nsubsh;///g_rip_nsubpixel;
		nsuby = (nsuby + 1)%g_rip_nsubpixel;

		int nenda = ((nrad + g_rip_nsubpixel - 1)>>g_rip_nsubsh/*/g_rip_nsubpixel*/) + ny;

		for(int k = ny ; k <= nenda ; k ++)
		{
			if(k >= nheight)
				break;
			rip_linebuf* pbuf = pmgr->get_linebuf(k);
			pbuf->ncursuby = 0;//(ny << g_rip_nsubsh /* * g_rip_nsubpixel*/) + nsuby - ((k << g_rip_nsubsh /* *g_rip_nsubpixel*/) - nrad) + 1;
			pbuf->bWrite = FALSE;
			pbuf->nmin_x = pbuf->nwidth;
			pbuf->nmax_x = 0;
		}
	}
}

void rip_update_buf(unsigned long*	buf,
				unsigned long	color,
				int nwidth,
				int nheight,
				int ny,
				int nsuby,
				bufpool_mgr* pmgr,
				int nrad,
				int ntop)
{

	//计算是否有行出
	rip_linebuf* pbuf = NULL;
	DWORD* pmaskbuf = NULL;
	
	long lalpha = 0;
	unsigned long resultcolor = color&0x00ffffff;
	unsigned long resultalpha = (color&0xff000000) >> 24;
	pbuf = pmgr->get_linebuf(pmgr->m_nMinIndex);
//	void* ptable = bgra32_line_getkatable();
	//一行的运算结束,需要累加到目标缓冲中
	if(pbuf && pbuf->ncursuby >= g_rip_table_size )
	{
		if(pbuf->nmin_x > pbuf->nmax_x)
		{
			int t = pbuf->nmin_x;
			pbuf->nmax_x = t;
			pbuf->nmin_x = pbuf->nmax_x;
		}
		int nminstart = max(0,pbuf->nmin_x);
		nminstart = min(nwidth,nminstart);
		int nmaxend = min(pbuf->nmax_x,nwidth);
		nmaxend = max(0,nmaxend);
		pmaskbuf = buf + pbuf->nwidth * (pmgr->m_nMinIndex) + nminstart;
		for(int j = nminstart;j < nmaxend;j ++)
		{
			if(pbuf->buf[j] > (DWORD)g_nTotalErea)
			{
				pbuf->buf[j] = g_nTotalErea;
			}
			lalpha = ((((((255*pbuf->buf[j])<<12)&0xff000000)>>24 ) * resultalpha )<<16)&0xff000000;
			
			pbuf->buf[j] = lalpha|resultcolor;
		}
		int nlen = (nmaxend - nminstart)<<2;
		
		int na16 = nlen>>4;
		int nm16 = nlen&0xf;
		DWORD* dwSrcbuf = pbuf->buf + nminstart;
		BGRA32_LineMix(pmaskbuf,		// 目标
			dwSrcbuf,			// 源
			na16,			// 宽度除以 16 的商
			nm16);
//		bgra32_line_mix(pmaskbuf,dwSrcbuf,na16,nm16,ptable);
		pmgr->release_linebuf(pmgr->m_nMinIndex);
	}

	//如果没有现存
	if(pmgr->m_nMaxIndex >= 0 && pmgr->m_nMaxIndex < nheight - 1)
	{
		if(((pmgr->m_nMaxIndex + 1 - ny) << g_rip_nsubsh/** g_rip_nsubpixel*/) - nsuby -1<= nrad)
		{
			pbuf = pmgr->get_linebuf(pmgr->m_nMaxIndex + 1);
			pbuf->ncursuby = 0;
			pbuf->nmin_x = pbuf->nwidth;
			pbuf->nmax_x = 0;
		}
	}
	else 
	{
		//算出下一行
		//前面已经累加不用再次计算
		//ny += (nsuby + 1)>>g_rip_nsubsh;///g_rip_nsubpixel;
		//nsuby = (nsuby + 1)&0x2;//g_rip_nsubpixel;
		
		//int nenda = ((nrad + g_rip_nsubpixel - 1)>>g_rip_nsubsh/*/g_rip_nsubpixel*/) + ny;
		
		//for(int k = ny ; k <= nenda ; k ++)
		//{
		//	if(k >= nheight)
		//		break;
		//	rip_linebuf* pbuf = pmgr->get_linebuf(k);
		//	pbuf->ncursuby = (ny << g_rip_nsubsh /* * g_rip_nsubpixel*/) + nsuby - ((k << g_rip_nsubsh /* *g_rip_nsubpixel*/) - nrad);
		//}
	}
	
}
void rip_vectorcell(IN	 vectorcell*	cell, IN OUT	unsigned long* buf,IN unsigned long color,IN int nwidth,IN int nheight,OUT pointf* ptOrigin)
{
	//垂直扫描线
	rip_edge_node** pNodeY = new rip_edge_node*[nheight*16];
	memset(pNodeY,0,sizeof(rip_edge_node*)*nheight*16);
	rip_linebuf*	pLineBuf_Header = NULL;

	//最大缓冲行
	int nMaxLine = 1;

	//Rad计算数据覆盖半径,以子像素为单位
	//////////////////////////////////////////////////////////////////////////
	int nRad = (g_rip_table_size - g_rip_nsubpixel)/2;
	int nhalfcount = nRad >> g_rip_nsubsh /*/g_rip_nsubpixel*/;
	if(nRad%g_rip_nsubpixel)
		nhalfcount += 1;
//	int nhalfcount = (nRad + (g_rip_nsubpixel - nRad%g_rip_nsubpixel))/g_rip_nsubpixel;
	//////////////////////////////////////////////////////////////////////////
	//nMaxLine是最大行数，单位是物理行数
	if(nhalfcount)
	{
		nMaxLine = nhalfcount * 2;
		nMaxLine = nMaxLine + 1;
	}
	///////////////////////////////////////////////////////////
	unsigned long* pmaskbuf = buf;
	rip_edge_node_x_list* pNode_x_header = NULL;

	int nmin_y = nheight * g_rip_nsubpixel,nmax_y = 0;
	for(int i = 0 ; i < cell->npoint; i ++)
	{
		if(cell->point[i].y == cell->point[(i+1)%cell->npoint].y)
			continue;
		rip_edge_node* pnode = new rip_edge_node;

		//扩大g_nsubpixel倍
		pnode->start.x = (cell->point[i].x + ptOrigin->x)*g_rip_nsubpixel;
		pnode->start.y = (cell->point[i].y + ptOrigin->y)*g_rip_nsubpixel;
		pnode->end.x = (cell->point[(i+1)%cell->npoint].x + ptOrigin->x)*g_rip_nsubpixel;
		pnode->end.y = (cell->point[(i+1)%cell->npoint].y + ptOrigin->y)*g_rip_nsubpixel;
		

		//保持小的点为开始点
		if(pnode->end.y < pnode->start.y)
		{
			float y = pnode->start.y;
			float x = pnode->start.x;
			pnode->start.y = pnode->end.y;
			pnode->start.x = pnode->end.x;
			pnode->end.y = y;
			pnode->end.x = x;
		}

		int y = (int)pnode->start.y;

		nmin_y = min(y - nRad,nmin_y);
		nmax_y = max((int)(pnode->end.y+1+nRad),nmax_y);
		
		//斜率
		pnode->fk = (pnode->end.x - pnode->start.x)/(pnode->end.y - pnode->start.y);
		//插入节点
		rip_insertnode(&pNodeY[y],pnode);
	}
	if(nmin_y > nmax_y)
		return;
	//创建一个buf池
	unsigned char* pool = NULL;
	bufpool_mgr mgr(nheight,nMaxLine,nwidth,pool,NULL);
	//一开始就把开始的影响行加入进去,这时候只有当前行以后的行
	
	int nstarta = nmin_y/g_rip_nsubpixel;
	int nstartm = nmin_y%g_rip_nsubpixel;
	int nenda = (nRad + g_rip_nsubpixel - 1)/g_rip_nsubpixel + nstarta;

	int nlinesub = nstartm;
	for(int k = nstarta ; k <= nenda ; k ++)
	{
		rip_linebuf* pbuf = mgr.get_linebuf(k);
		pbuf->ncursuby = nmin_y - (k * g_rip_nsubpixel - nRad);
		int a = 0;
	}
	//最小影响行和最大影响行
	mgr.m_nMinIndex = nstarta;
	mgr.m_nMaxIndex = nenda;

	//逐行光栅化
	int nmin_x = nwidth,nmax_x = nwidth;
	int nmaxy = max(nheight,nmax_y);
	int y = 0;
	int linestarty = 0;

	for(int h = 0 ; h < nheight ; h ++)
	{	
		//有效像素行
		for(int i = 0 ; i < g_rip_nsubpixel ; i++)
		{
			if(y < 0)
			{
				y ++;
				continue;
			}
			//最小位置
			if(y < nmin_y )
			{
				y ++;
				continue;
			}

			//最大位置
			if(y > nmax_y)
				break;

			//更新行参数
			rip_update_node_x(pNode_x_header,y);

			//新行加入行链表
			rip_addnode_x(pNode_x_header,pNodeY[y],y);

			//移出不需要的节点
			rip_remove_node_x(pNode_x_header,y);

			//排序
			rip_sort_x_node(pNode_x_header);
			nmin_x = max(0,nmin_x);
			nmax_x = max(0,nmax_x);
			nmin_x = min(0,nwidth);
			nmax_x = min(0,nwidth);
			rip_x_node(pNode_x_header,h,i,&mgr,nmin_x,nmax_x);
			y++;
			rip_update_buf(buf,color,nwidth,nheight,h,i,&mgr,nRad,0);
		}
		linestarty += g_rip_nsubpixel;
	}
}

//根据提供信息产生nod
void rip_calcnode(OUT rip_edge_node* pnode,
				IN pointf* pstart,
				IN pointf* pend,
				IN pointf* ptOrigin,
				IN float fscale,
				IN int nsubpixel)
{
	//扩大g_nsubpixel倍
	pnode->start.x = (pstart->x * fscale + ptOrigin->x) * nsubpixel;
	pnode->start.y = (pstart->y  * fscale + ptOrigin->y)* nsubpixel;
	pnode->end.x = (pend->x * fscale  + ptOrigin->x) * nsubpixel;
	pnode->end.y = (pend->y * fscale + ptOrigin->y) * nsubpixel;
	pnode->pXNext = NULL;
	pnode->pXPrv = NULL;

	if(pnode->end.y > pnode->start.y)
		pnode->dir = 1;
	else if(pnode->end.y < pnode->start.y)
		pnode->dir = -1;
	else
		pnode->dir = 0;

	if(pnode->end.y < pnode->start.y)
	{
		float y = pnode->start.y;
		float x = pnode->start.x;
		pnode->start.y = pnode->end.y;
		pnode->start.x = pnode->end.x;
		pnode->end.y = y;
		pnode->end.x = x;
	}

	//斜率
	pnode->fk = (pnode->end.x - pnode->start.x)/(pnode->end.y - pnode->start.y);
}


BOOL	Common_Beizer3IsNearLine(pointf* pt0,pointf* pt1,pointf* pt2,pointf* pt3,float fscale)
{
	//判断是否接近一条直线,如果接近一条直线,那么就用直线代替否则分割该曲线
	//直线的参数方程
	//x = x0+t*(x1-x0)
	//y = y0+t*(y1-y0)
	//t = (x-x0)/(x1-x0)
	//直线的一般方程
	//y = y0+x*((y1-y0)/(x1-x0))

	//判断控制点是否在两个基本点之间。只要t>=0&&t<=1
	float f30X  = (pt3->x-pt0->x) * fscale;
	float f10X  = (pt1->x-pt0->x) * fscale;
	float f20X  = (pt2->x-pt0->x) * fscale;
	//避免除以0
	if(fabs(f30X)<ABS_FLOAT_0)
	{
		if(fabs(f10X)<0.5f&&fabs(f20X)<=0.5f)
			return TRUE;
		else
			return FALSE;
	}
	/*	if(f10X>f30X||(f10X>0&&f30X<0)||(f10X<0&&f30X>0)||
	f20X>f30X||(f20X>0&&f30X<0)||(f20X<0&&f30X>0))
	{
	//t<0||t>1
	return FALSE;

	}
	else*/
	{
		float t1 = f10X/f30X;
		float fdy = (pt3->y-pt0->y) * fscale;
		float fy1 = pt0->y * fscale + t1*fdy;
		if(fabs(pt1->y * fscale - fy1)<0.1f)//Y相差
		{
			float t2 = f20X/f30X;
			float fy2 = pt0->y * fscale + t2*fdy;
			if(fabs(pt2->y * fscale-fy2) < 0.1f)//Y相差
			{
				return TRUE;
				//接近直线
			}
		}
		return FALSE;
		//需要分割

	}
}

BOOL	Common_Beizer2IsNearLine(pointf* pp1,pointf* pp2,pointf* pp3,float fscale)
{

	pointf pointfdes = ((*pp1 + *pp3) - (*pp2) * 2 )/4.0f;
	if(fabs(pointfdes.x * fscale) < 2 && fabs(pointfdes.y * fscale)<2)
		return 1;
	return 0;
}
BOOL	Common_Beizer3IsNearLine(pointf* pt,float fscale)
{
	return Common_Beizer3IsNearLine(&pt[0],&pt[1],&pt[2],&pt[3],fscale);
}

BOOL SplitBezier2(pointf* psrc,pointf* pdes1,pointf* pdes2)
{
	pdes1[0] = psrc[0];
	pdes1[1] = (psrc[0] + psrc[1])/2.0f;
	pdes2[2] = psrc[2];
	pdes2[1] = (psrc[1] + psrc[2])/2.0f;
	pdes2[0] = pdes1[2] = (pdes1[1] + pdes2[1])/2.0f;
	return 1;
}
BOOL SplitBezier3(pointf* pOPoint,pointf* pSplit1,pointf* pSplit2)
{
	float fMid01X = (pOPoint[0].x+pOPoint[1].x)*0.5f;
	float fMid01Y = (pOPoint[0].y+pOPoint[1].y)*0.5f;

	float fMid12X = (pOPoint[1].x+pOPoint[2].x)*0.5f;
	float fMid12Y = (pOPoint[1].y+pOPoint[2].y)*0.5f;

	float fMid23X = (pOPoint[2].x+pOPoint[3].x)*0.5f;
	float fMid23Y = (pOPoint[2].y+pOPoint[3].y)*0.5f;

	float fMid0112X = (fMid01X+fMid12X)*0.5f;
	float fMid0112Y = (fMid01Y+fMid12Y)*0.5f;

	float fMid1223X = (fMid23X+fMid12X)*0.5f;
	float fMid1223Y = (fMid23Y+fMid12Y)*0.5f;

	float fInterPtX = (fMid0112X+fMid1223X)*0.5f;
	float fInterPtY = (fMid0112Y+fMid1223Y)*0.5f;

	pSplit1[0].x = pOPoint[0].x;
	pSplit1[0].y = pOPoint[0].y;

	pSplit1[1].x = fMid01X;
	pSplit1[1].y = fMid01Y;

	pSplit1[2].x = fMid0112X;
	pSplit1[2].y = fMid0112Y;

	pSplit2[0].x = pSplit1[3].x = fInterPtX;
	pSplit2[0].y = pSplit1[3].y = fInterPtY;

	pSplit2[1].x = fMid1223X;
	pSplit2[1].y = fMid1223Y;

	pSplit2[2].x = fMid23X;
	pSplit2[2].y = fMid23Y;

	pSplit2[3].x = pOPoint[3].x;
	pSplit2[3].y = pOPoint[3].y;

	return 1;
}
void rip_splitconic_node_ex(pointf* pconicf,SYList* pNodeList,pointf* ptOrigin,float fscale,int nsubpixel,unsigned char*&	pool,unsigned char*	poolend)
{
	pointf mid = pconicf[2]*0.25;
	if(fabs(mid.x)>0.05|| fabs(mid.y)>0.05)
	{
		pointf split[3];
		split[0] = pconicf[0];
		split[1] = (pconicf[0] + pconicf[1])*0.5f - mid;
		split[2] = mid;
		rip_splitconic_node_ex(split,pNodeList,ptOrigin,fscale,nsubpixel,pool,poolend);
		split[0] = split[1];
		split[1] = pconicf[1];
		rip_splitconic_node_ex(split,pNodeList,ptOrigin,fscale,nsubpixel,pool,poolend);
	}
	else
	{
		rip_edge_node* pnode = rip_malloc_node(pool,poolend);
		//根据信息产生新的节点
		rip_calcnode(pnode,&pconicf[0],&pconicf[1],ptOrigin,fscale,nsubpixel);
		SYList_AddTail(pNodeList,(LT_DATA_PTR)pnode,&pool,poolend);
	}
}
void rip_splitconic_node(pointf* pconicf,SYList* pNodeList,pointf* ptOrigin,float fscale,int nsubpixel,unsigned char*&	pool,unsigned char*	poolend)
{
	//分别看两条曲线是否接近直线，如果接近，那么分割完成否则继续分割
	BOOL bNearline = Common_Beizer2IsNearLine(&pconicf[0],&pconicf[1],&pconicf[2],fscale);
	if(bNearline)
	{
		rip_edge_node* pnode = rip_malloc_node(pool,poolend);

		//根据信息产生新的节点
		rip_calcnode(pnode,&pconicf[0],&pconicf[1],ptOrigin,fscale,nsubpixel);
		SYList_AddTail(pNodeList,(LT_DATA_PTR)pnode,&pool,poolend);
		pnode = rip_malloc_node(pool,poolend);
		rip_calcnode(pnode,&pconicf[1],&pconicf[2],ptOrigin,fscale,nsubpixel);
		SYList_AddTail(pNodeList,(LT_DATA_PTR)pnode,&pool,poolend);
	}
	else
	{
		pointf pttempbefore[3],pttempafter[3];
		SplitBezier2(pconicf,pttempbefore,pttempafter);
		rip_splitconic_node(pttempbefore,pNodeList,ptOrigin,fscale,nsubpixel,pool,poolend);
		rip_splitconic_node(pttempafter,pNodeList,ptOrigin,fscale,nsubpixel,pool,poolend);
	}
}

void rip_conic_line(pointf* inpoint,int ncount,SYList* pNodeList,pointf* ptOrigin,float fscale,int nsubpixel,unsigned char*&	pool,unsigned char*	poolend)
{
	pointf pf[3];
	pointf ptcalc[3];
	//存储分割的临时曲线
	int nremaincount = ncount;
	int nindex = 0;
	pf[0] = inpoint[nindex];
	nindex ++;
	for(;;)
	{
		if(nremaincount < 3)
			break;
		
		pf[1] = inpoint[nindex];

		if(nindex + 1 == ncount - 1)
		{
			pf[2] = inpoint[nindex + 1];
		}
		else
		{
			pf[2] = (inpoint[nindex] + inpoint[nindex + 1])/2;
		}
		ptcalc[0] = pf[0];
		ptcalc[1] = pf[2];
		ptcalc[2] = pf[2] + pf[0] - pf[1] * 2;
		rip_splitconic_node_ex(ptcalc,pNodeList,ptOrigin,fscale,nsubpixel,pool ,poolend);
//		rip_splitconic_node(pf,pNodeList,ptOrigin,fscale,nsubpixel,pool ,poolend);

		//把输入4个点分割为新的两条曲线
		pf[0] = pf[2];
		nremaincount -= 1;
		nindex += 1;
	}

}

//计算
void rip_splitbezier_node(pointf* inpoint,SYList* pNodeList,pointf* ptOrigin,float fscale,int nsubpixel,unsigned char*&	pool,unsigned char*	poolend)
{
	//分别看两条曲线是否接近直线，如果接近，那么分割完成否则继续分割
	int a;
	BOOL bNearline = Common_Beizer3IsNearLine(inpoint,fscale);
	if(bNearline)
	{
		rip_edge_node* pnode = rip_malloc_node(pool,poolend);
		if(pnode == 0)
		{
			assert(FALSE);
			a = GetLastError();
		}
		//根据信息产生新的节点
		rip_calcnode(
			pnode,
			&inpoint[0],
			&inpoint[3],
			ptOrigin,
			fscale,
			nsubpixel);
		SYList_AddTail(pNodeList,(LT_DATA_PTR)pnode,&pool,poolend);
	}
	else
	{
		pointf pttempbefore[4],pttempafter[4];
		SplitBezier3(inpoint,pttempbefore,pttempafter);
		rip_splitbezier_node(pttempbefore,pNodeList,ptOrigin,fscale,nsubpixel,pool,poolend);
		rip_splitbezier_node(pttempafter,pNodeList,ptOrigin,fscale,nsubpixel,pool,poolend);
	}
}

//把bezier曲线转为直线
void rip_bezier_line(pointf* inpoint,int nallcount,int nstart,int ncount,SYList* pNodeList,pointf* ptOrigin,float fscale,int nsubpixel,unsigned char*&	pool,unsigned char*	poolend)
{	
	//存储分割的临时曲线
	pointf ptf[4];
	int nremaincount = ncount;
	int nindex = nstart;
	for(;;)
	{
		if(nremaincount < 4)
			break;
		for(int i = 0;i < 4;i ++)
		{
			ptf[i] = inpoint[(nindex + i) % nallcount];
		}
		rip_splitbezier_node(ptf,pNodeList,ptOrigin,fscale,nsubpixel,pool,poolend);
		//把输入4个点分割为新的两条曲线
		nremaincount -= 3;
		nindex += 3;
	}
}
void rip_figure_mask(IN vectorgraph* figure,IN OUT unsigned char* buf,IN int nwidth,IN int nheight,
				IN float fscale,OUT pointf* ptOrigin,IN unsigned char* pool,IN unsigned int	poolsize)
{
	rip_figure_X(figure,
				buf,
				8,
				0,
				nwidth,
				nheight,
				fscale,
				ptOrigin,
				pool,
				poolsize);
	return;
}
void rip_figure(IN vectorgraph*	 figure,IN OUT unsigned long* buf,IN unsigned long color,IN int nwidth,
			IN int nheight,IN float	fscale,OUT pointf* ptOrigin)
{
	rip_figure_X(figure,
				buf,
				32,
				color,
				nwidth,
				nheight,
				fscale,
				ptOrigin);
	return;
}
void v2_mulm4x4(pointf* v,matrix4f* m)
{
	pointf vr;
	vr.x = v->x*m->_11+v->y*m->_21+1.0f*m->_41;
	vr.y = v->x*m->_12+v->y*m->_22+1.0f*m->_42;
	memcpy(v,&vr,sizeof(pointf));
}

void	rip_cell_matrix(vectorcell* pdescell,matrix4f* pmatrix)
{
	for ( int i = 0 ; i< pdescell->npoint; i++)
	{
		v2_mulm4x4(&pdescell->point[i],(matrix4f*)pmatrix);
	}
}

void	rip_mul_matrix(vectorgraph* pgraph,matrix4f* pmatrix)
{
	for( int i = 0; i < pgraph->ncell ; i++)
	{
		rip_cell_matrix(&pgraph->cell[i],pmatrix);
	}
}

int  cell_get_group_ptcount(vectorcell*	srccell, int nstart, int type)
{
	int nptcount = srccell->npoint;
	int ncurvercount = 1;
	int nend = 0;
	for (int i = nstart + 1; i < nptcount; i++)
	{
		ncurvercount++;
		if (srccell->pttype[i] != type)
		{
			nend = 1;
			break;
		}
	}
	if (!nend)
	{
		ncurvercount += 1;
	}
	return ncurvercount;
}

int  conic_extend(BaseARray* ptArray, pointf pt1, pointf pt2, pointf pt3, pointf startoffset, pointf endoffset, float fwidth, int ndirect, BaseARray* lenArray = NULL, float* fAllLen = NULL);

int  conic_group_extend(OUT BaseARray* ptArray, IN vectorcell* cell, IN  int nstart, float fwidth, int ndirect, BaseARray* plenArray, float* pfalllen)
{

	pointf pt1, pt2, pt3;
	pointf ptstartoffset, ptendoffset, ptnormal;
	int nindex = nstart;
	int ncurvercount = cell_get_group_ptcount(cell, nstart, point_conic);
	//first curver,for conic,the first curver is particular
	//it need three point ,pt1,pt2,pt3,for the first ,pt1 is  the first pt,
	//pt2 is the second point,pt3 is the mid of the second and third;
	//for the mid curver,pt1 is the mid of the first and the second,the pt2 is
	//the second ,the pt3 is the mid of the second and third;
	//for the last curver ,pt1 is the mid of the first and the second,the pt2 is
	//the second ,the pt3 is the last pt;

	pt1 = cell->point[nindex];
	pt2 = cell->point[nindex + 1];
	if (ncurvercount == 3)
	{
		pt3 = cell->point[nindex + 2];
	}
	else
		pt3 = (cell->point[nindex + 1] + cell->point[nindex + 2]) * 0.5f;

	do {

		//if(pt1 == pt2 || pt2 == pt3)
		//{
		//	pointf& 
		//	line_extend(OUT pointf& ptoutstart,pointf& ptoutend,
		//		IN pointf& ptinstart,pointf& ptinend,
		//		IN int ndirect,IN float fwidth)
		//}
		//计算开始点和终止点的偏移
		if (pt1 == pt2)
		{
			if (pt2 == pt3)
			{
				goto next_conic_loop;
			}
			line_vertical_normalizing(&ptnormal, &pt1, &pt3, ndirect);
		}
		else
		{
			line_vertical_normalizing(&ptnormal, &pt1, &pt2, ndirect);
		}
		ptstartoffset = pt1 + ptnormal * fwidth;


		if (pt2 == pt3)
		{
			ptendoffset = pt3 + ptnormal * fwidth;
		}
		else
		{
			line_vertical_normalizing(&ptnormal, &pt2, &pt3, ndirect);
			ptendoffset = pt3 + ptnormal * fwidth;
		}

		if (*(DWORD*)&ptstartoffset.x == 0xffc00000)
		{
			assert(FALSE);
			//_asm int 3;
		}
		if (*(DWORD*)&ptendoffset.x == 0xffc00000)
		{
			assert(FALSE);
			//_asm int 3;
		}

		//扩展
		conic_extend(ptArray, pt1, pt2, pt3, ptstartoffset, ptendoffset, fwidth, ndirect, plenArray, pfalllen);



		//下一组点
	next_conic_loop:
		nindex++;
		if (nindex > nstart + ncurvercount - 3)
			break;
		pt1 = pt3;
		pt2 = cell->point[(nindex + 1) % cell->npoint];
		if (nindex > nstart + ncurvercount - 3)
			break;
		else if (nindex == nstart + ncurvercount - 3)
			pt3 = cell->point[(nindex + 2) % cell->npoint];
		else
			pt3 = (cell->point[(nindex + 1) % cell->npoint] + cell->point[(nindex + 2) % cell->npoint]) * 0.5f;

	} while (nindex <= nstart + ncurvercount - 3);


	//if reach last,return -1;
	if (nstart + ncurvercount >= cell->npoint)
		return -1;

	//else if not ,return the index of next point.
	return nstart + ncurvercount - 1;
}

int  bezier_group_extend(OUT BaseARray* ptArray, IN vectorcell* cell, IN  int nstart, float fwidth, int ndirect, BaseARray* lenArray, float* falllen, pointf* ptlast = NULL);

int cell_makecell(LPBaseARRAY ptarray, vectorcell* pcell)
{
	int narcount = (int)BaseAR_GetSize(ptarray);
	pcell->npoint = narcount;
	pcell->point = new pointf[narcount];
	pcell->pttype = new unsigned long[narcount];
	pcell->bound.left = 65535.0f;
	pcell->bound.top = 65535.0f;
	pcell->bound.right = -65535.0f;
	pcell->bound.bottom = -65535.0f;
	for (int i = 0; i < pcell->npoint; i++)
	{
		pointf* ptresult = (pointf*)BaseAR_GetAt(ptarray, i);
		pcell->point[i] = *ptresult;
		pcell->pttype[i] = point_line;
		pcell->bound.left = min(pcell->bound.left, ptresult->x);
		pcell->bound.top = min(pcell->bound.top, ptresult->y);
		pcell->bound.right = max(pcell->bound.right, ptresult->x);
		pcell->bound.bottom = max(pcell->bound.bottom, ptresult->y);
		delete ptresult;
	}
	BaseAR_RemoveAll(ptarray);
	BaseAR_Release(ptarray);
	return 1;
}

int BASEGRAPH_DATARENDER_API cell_extend(OUT vectorcell * descell, IN vectorcell * srccell, IN int ndirect, IN float fwidth)
{
	pointf ptprvline[2];
	BOOL	bPrvLine = FALSE;
	if (fwidth <= 0)
		fwidth = 1.0f;
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


	LPBaseARRAY ptarray = BaseAR_Create(); // 临时array, 用于保存偏移后的顶点, 最后再转化成cell
	pointf ptend;

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
		line_vertical_normalizing
		(&ptnormal, &srccell->point[nprvindex], &srccell->point[0], ndirect);
		// 对起点进行偏移
		ptlastoffset = srccell->point[0] + ptnormal * fwidth;
		ptlast = &ptlastoffset;

		ptprvline[0] = srccell->point[nprvindex] + ptnormal * fwidth;
		ptprvline[1] = ptlastoffset;
		/*pointf* pt = new pointf;
		*pt = ptlastoffset;*/
		//BaseAR_Add(ptarray,pt);

		bPrvLine = TRUE;

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
		pointf* ptstart = new pointf;
		pointf  ptuncloseend;
		line_extend(*ptstart, ptuncloseend, srccell->point[nindex], srccell->point[(nindex + 1) % nptcount], ndirect, fwidth);
		BaseAR_Add(ptarray, ptstart);
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
			if (srccell->point[i] == srccell->point[(i + 1) % nptcount] || bPrvLine)
			{
			}
			else
			{
				int ncalcStart = i;
				while (srccell->point[ncalcStart] == srccell->point[(ncalcStart + 1) % nptcount])
				{
					ncalcStart++;
				}
				line_vertical_normalizing(&ptnormal, &srccell->point[ncalcStart], &srccell->point[(ncalcStart + 1) % nptcount], ndirect);
				ptoffset = srccell->point[ncalcStart] + ptnormal * fwidth;
				float fdis1 = (ptlast->x - srccell->point[ncalcStart].x)*(ptlast->x - srccell->point[ncalcStart].x) + (ptlast->y - srccell->point[ncalcStart].y)*(ptlast->y - srccell->point[ncalcStart].y);
				float fdis2 = (ptoffset.x - srccell->point[ncalcStart].x)*(ptoffset.x - srccell->point[ncalcStart].x) + (ptoffset.y - srccell->point[ncalcStart].y)*(ptoffset.y - srccell->point[ncalcStart].y);
				if (fabs(1 - fdis1 / fdis2)>0.0005)
				{
					assert(FALSE);
					//_asm int 3;
				}
				cell_makearc(ptarray, srccell->point[i], *ptlast, ptoffset, fwidth);
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
			line_extend(ptstart, ptend, srccell->point[i], srccell->point[(i + 1) % nptcount], ndirect, fwidth);
			if (bPrvLine)
			{
				pointf ptout;
				if (IsSegmentIntersection(ptprvline[0], ptprvline[1], ptstart, ptend, &ptout))
				{
					pointf* ptnewout = new pointf;
					*ptnewout = ptout;
					BaseAR_Add(ptarray, ptnewout);
				}
				else
				{
					cell_makearc(ptarray, srccell->point[i], ptprvline[1], ptstart, fwidth);
				}
			}
			ptprvline[0] = ptstart;
			ptprvline[1] = ptend;
			bPrvLine = TRUE;
			if (*(DWORD*)&ptend.x == 0xffc00000)
			{
				int a = 0;
			}
			i++;
		}
		else if (pttype == point_conic_start)			//如果是2次曲线
		{
			if (bPrvLine)
			{
				int ncalcStart = i;
				while (srccell->point[ncalcStart] == srccell->point[(ncalcStart + 1) % nptcount])
				{
					ncalcStart++;
				}
				line_vertical_normalizing(&ptnormal, &srccell->point[ncalcStart], &srccell->point[(ncalcStart + 1) % nptcount], ndirect);
				pointf ptoffset = srccell->point[i] + ptnormal * fwidth;
				float fdis1 = (ptprvline[1].x - srccell->point[ncalcStart].x)*(ptprvline[1].x - srccell->point[ncalcStart].x) + (ptprvline[1].y - srccell->point[ncalcStart].y)*(ptprvline[1].y - srccell->point[ncalcStart].y);
				float fdis2 = (ptoffset.x - srccell->point[ncalcStart].x)*(ptoffset.x - srccell->point[ncalcStart].x) + (ptoffset.y - srccell->point[ncalcStart].y)*(ptoffset.y - srccell->point[ncalcStart].y);
				if (fabs(1 - fdis1 / fdis2)>0.0005)
				{
					assert(FALSE);
					//_asm int 3;
				}
				cell_makearc(ptarray, srccell->point[i], ptprvline[1], ptoffset, fwidth);
			}
			i = conic_group_extend(ptarray, srccell, i, fwidth, ndirect, NULL, 0);
			bPrvLine = FALSE;
		}
		else if (pttype == point_curve || pttype == point_curve_start)			//如果是3次曲线
		{
			if (bPrvLine)
			{
				int ncalcStart = i;
				while (srccell->point[ncalcStart] == srccell->point[(ncalcStart + 1) % nptcount])
				{
					ncalcStart++;
				}
				line_vertical_normalizing(&ptnormal, &srccell->point[ncalcStart], &srccell->point[(ncalcStart + 1) % nptcount], ndirect);
				pointf ptoffset = srccell->point[i] + ptnormal * fwidth;
				float fdis1 = (ptprvline[1].x - srccell->point[ncalcStart].x)*(ptprvline[1].x - srccell->point[ncalcStart].x) + (ptprvline[1].y - srccell->point[ncalcStart].y)*(ptprvline[1].y - srccell->point[ncalcStart].y);
				float fdis2 = (ptoffset.x - srccell->point[ncalcStart].x)*(ptoffset.x - srccell->point[ncalcStart].x) + (ptoffset.y - srccell->point[ncalcStart].y)*(ptoffset.y - srccell->point[ncalcStart].y);
				if (fabs(1 - fdis1 / fdis2)>0.0005)
				{
					//2016.12.08，djt，此处含义不明，在导入svg物件时此处经常报错，暂时注释掉
					//assert(FALSE);
					//_asm int 3;
				}
				cell_makearc(ptarray, srccell->point[i], ptprvline[1], ptoffset, fwidth);
			}
			//			bezier_group_extend(OUT BaseARray* ptArray,IN vectorcell* cell,IN  int nstart,float fwidth,int ndirect,BaseARray* lenArray,float* falllen,pointf* ptlast = NULL)
			//pointf ptemp = ptprvline[1];
			i = bezier_group_extend(ptarray, srccell, i, fwidth, ndirect, NULL, NULL, NULL);
			bPrvLine = FALSE;
			int nCount = BaseAR_GetSize(ptarray);
			if (nCount >= 2)
			{
				ptprvline[0] = *(pointf*)BaseAR_GetAt(ptarray, nCount - 2);
				ptprvline[1] = *(pointf*)BaseAR_GetAt(ptarray, nCount - 1);
				bPrvLine = TRUE;
			}

		}
		if (i < 0)
			break;
		ptlast = (pointf*)BaseAR_GetAt(ptarray, BaseAR_GetSize(ptarray) - 1);
	}
	if (srccell->type == uncloss_line && bPrvLine)
	{
		pointf* ptadd = new pointf;
		*ptadd = ptprvline[1];
		BaseAR_Add(ptarray, ptadd);
	}
	// 格式转换:　array --> cell
	cell_makecell(ptarray, descell);
	//for(int i = 0;i < ptarray->m_nSize;i ++)
	//{
	//	delete (pointf * )BaseAR_GetAt(ptarray,i);
	//}
	//BaseAR_Release(ptarray);
	descell->type = srccell->type;
	descell->width = srccell->width;
	return 1;
}

void BASEGRAPH_DATARENDER_API cell_reverse(vectorcell * pcell)
{
	pointf pttemp;
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

void BASEGRAPH_DATARENDER_API cell_merge(vectorcell * descell, const vectorcell * srccell1, vectorcell * srccell2)
{
	descell->npoint = srccell1->npoint + srccell2->npoint;
	descell->point = new pointf[descell->npoint];
	descell->pttype = new unsigned long[descell->npoint];
	memcpy(descell->point, srccell1->point, sizeof(pointf) * srccell1->npoint);

	memcpy(descell->point + srccell1->npoint, srccell2->point, sizeof(pointf) * srccell2->npoint);
	memcpy(descell->pttype, srccell1->pttype, sizeof(unsigned long) * srccell1->npoint);
	memcpy(descell->pttype + srccell1->npoint, srccell2->pttype, sizeof(unsigned long) * srccell2->npoint);
	descell->bound.left = min(srccell1->bound.left, srccell2->bound.left);
	descell->bound.top = min(srccell1->bound.top, srccell2->bound.top);
	descell->bound.right = max(srccell1->bound.right, srccell2->bound.right);
	descell->bound.bottom = max(srccell1->bound.bottom, srccell2->bound.bottom);
}

//void BASEGRAPH_DATARENDER_API rip_draw_figure(IN vectorgraph * figure, IN float offsetx, IN float offsety, IN CCommonBufferDC * bufferDC)
//{
//	vectorcell cellline;
//	memset(&cellline, 0, sizeof(vectorcell));
//	for (int k = 0; k < figure->ncell; k++)
//	{
//		vectorcell* cell = &figure->cell[k];
//		if (cell->type == uncloss_line || cell->type == close_line)
//		{
//			// new code:
//			int ndirect = 1;
//			vectorcell firstCell, seccell;
//			memset(&firstCell, 0, sizeof(vectorcell));
//			memset(&seccell, 0, sizeof(vectorcell));
//			SYOffsetCell(&figure->cell[k],  // 输入曲线
//				NULL,
//				figure->cell[k].width*0.5f,  // 偏移距离
//				&firstCell, // 输出偏移曲线
//				ndirect
//			);
//
//			SYOffsetCell(&figure->cell[k],  // 输入曲线
//				NULL,
//				figure->cell[k].width*0.5f,  // 偏移距离
//				&seccell, // 输出偏移曲线
//				-ndirect
//			);
//
//			cell_reverse(&seccell);
//			cell_merge(&cellline, &firstCell, &seccell);
//			cell = &cellline;
//			SYReleaseOffsetCell(&firstCell);
//			SYReleaseOffsetCell(&seccell);
//			cell = &cellline;
//			////////////////////////////////////////////////////////////////////////// end
//		}
//		for (int i = 0; i < cell->npoint;)
//		{
//			if (cell->pttype[i] == point_line)
//			{
//				//如果线段在一个像素内忽略
//				if (cell->point[i].y == cell->point[(i + 1) % cell->npoint].y
//					&& cell->point[i].x == cell->point[(i + 1) % cell->npoint].x)
//				{
//					i++;
//					continue;
//				}
//				//rip_edge_node* pnode = rip_malloc_node(pool,pool_end);
//				bufferDC->MoveTo(cell->point[i].x + offsetx, cell->point[i].y + offsety);
//				bufferDC->LineTo(cell->point[(i + 1) % cell->npoint].x + offsetx, cell->point[(i + 1) % cell->npoint].y + offsety);
//				i++;
//			}
//			else if (cell->pttype[i] == point_curve) //如果是3次Bezier,那么需要分割曲线为小的线段
//			{
//				//计算总共有多少个曲线节点，注意最后一个曲线节点的属性可能不是曲线
//				//还要注意闭合情况
//				int ncuvercount = 0;
//				for (int n = i; n < cell->npoint; n++)
//				{
//					ncuvercount++;
//					if (cell->pttype[n] != point_curve)
//						break;
//				}
//				rip_draw_bezier(bufferDC, &cell->point[i], ncuvercount, offsetx, offsety);
//				//依次插入
//
//				if (i + ncuvercount >= cell->npoint)
//					i += ncuvercount;
//				else
//					i += (ncuvercount - 1);
//			}
//			else if (cell->pttype[i] == point_conic_start) //如果是2次Bezier,那么需要分割曲线为小的线段
//			{
//				int ncuvercount = 1;
//				for (int n = i + 1; n < cell->npoint; n++)
//				{
//					ncuvercount++;
//					if (cell->pttype[n] != point_conic)
//						break;
//				}
//
//				//把曲线分割为小直线段
//				rip_draw_conic(bufferDC, IN &cell->point[i], ncuvercount, offsetx, offsety);
//				DWORD dwOldColor = bufferDC->SetPenColor(0xffff0000);
//				bufferDC->MoveTo(cell->point[i].x + offsetx, cell->point[i].y + offsety);
//				bufferDC->DrawCircle(cell->point[i].x + offsetx, cell->point[i].y + offsety, 2);
//				for (int kk = 0; kk < ncuvercount; kk++)
//				{
//					bufferDC->DrawCircle(cell->point[kk].x + offsetx, cell->point[kk].y + offsety, 2);
//					//bufferDC->LineTo(cell->point[kk].x + offsetx,cell->point[kk].y + offsety);
//				}
//				bufferDC->SetPenColor(dwOldColor);
//				//rip_conic_line(&cell->point[i],ncuvercount,&NodeList,ptOrigin,fscale,g_rip_nsubpixel,pool,pool_end);
//				//rip_bezier_line
//				if (i + ncuvercount >= cell->npoint)
//				{
//					//					bufferDC->DrawCircle(cell->point[i + ncuvercount -1].x,cell->point[i + ncuvercount -1].y,2);
//					bufferDC->MoveTo(cell->point[i + ncuvercount - 1].x + offsetx, cell->point[i + ncuvercount - 1].y + offsety);
//					bufferDC->LineTo(cell->point[(i + ncuvercount) % cell->npoint].x + offsetx, cell->point[(i + ncuvercount) % cell->npoint].y + offsety);
//					i += ncuvercount;
//
//				}
//				else
//					i += (ncuvercount - 1);
//			}
//			else
//			{
//				//__asm int 3;
//				assert(0);
//			}
//		}
//	}
//}

rip_edge_node* rip_malloc_node(unsigned char*&	pool,unsigned char* poolend)
{
	rip_edge_node* pnode = 0;
	if(pool + sizeof(rip_edge_node) < poolend)
	{
		rip_edge_node* pnode = (rip_edge_node*)pool;
		memset(pnode, 0, sizeof(rip_edge_node));
		pnode->canrelease = 0;
		pool += sizeof(rip_edge_node);
		return pnode;
	}
	pnode = new rip_edge_node;
	pnode->canrelease = 1;
	return pnode;
}

//void rip_draw_splitbezier(IN OUT CCommonBufferDC* bufferDC,IN pointf* pt,float offsetx,float offsety)
//{
//	BOOL bNearline = Common_Beizer3IsNearLine(pt,1.0f);
//	if(bNearline)
//	{
//		//根据信息产生新的节点
//		bufferDC->MoveTo(pt[0].x + offsetx,pt[0].y + offsety);
//		bufferDC->LineTo(pt[3].x + offsetx,pt[3].y + offsety);
//	}
//	else
//	{
//		pointf pttempbefore[4],pttempafter[4];
//		SplitBezier3(pt,pttempbefore,pttempafter);
//		rip_draw_splitbezier(bufferDC,pttempbefore,offsetx,offsety);
//		rip_draw_splitbezier(bufferDC,pttempafter,offsetx,offsety);
//	}
//}
//BASEGRAPH_DATARENDER_API void  rip_draw_bezier(IN OUT CCommonBufferDC* bufferDC,IN pointf* pt,int count,float offsetx,float offsety)
//{
//	int nremaincount = count;
//	int nindex = 0;
//	for(;;)
//	{
//		if(nremaincount < 4)
//			break;
//		rip_draw_splitbezier(bufferDC,&pt[nindex],offsetx,offsety);
//		//把输入4个点分割为新的两条曲线
//		nremaincount -= 3;
//		nindex += 3;
//	}
//}
//void rip_draw_splitconic(CCommonBufferDC* bufferDC,pointf* pconicf,float offsetx,float offsety)
//{
//	pointf mid = pconicf[2]*0.25;
//	if(fabs(mid.x)>0.05|| fabs(mid.y)>0.05)
//	{
//		pointf split[3];
//		split[0] = pconicf[0];
//		split[1] = (pconicf[0] + pconicf[1])*0.5f - mid;
//		split[2] = mid;
//		rip_draw_splitconic(bufferDC,split,offsetx,offsety);
//		split[0] = split[1];
//		split[1] = pconicf[1];
//		rip_draw_splitconic(bufferDC,split,offsetx,offsety);
//	}
//	else
//	{
//		bufferDC->MoveTo(pconicf[0].x + offsetx,pconicf[0].y + offsety);
//		bufferDC->LineTo(pconicf[1].x + offsetx,pconicf[1].y + offsety);
//	}
//}
//BASEGRAPH_DATARENDER_API void  rip_draw_conic(IN OUT CCommonBufferDC* bufferDC,IN pointf* pt,int count,float offsetx,float offsety)
//{
//	pointf pf[3];
//	pointf ptcalc[3];
//	//存储分割的临时曲线
//	int nremaincount = count;
//	int nindex = 0;
//	pf[0] = pt[nindex];
//	nindex ++;
//	for(;;)
//	{
//		if(nremaincount < 3)
//			break;
//
//		pf[1] = pt[nindex];
//
//		if(nindex + 1 == count - 1)
//		{
//			pf[2] = pt[nindex + 1];
//		}
//		else
//		{
//			pf[2] = (pt[nindex] + pt[nindex + 1])/2;
//		}
//		ptcalc[0] = pf[0];
//		ptcalc[1] = pf[2];
//		ptcalc[2] = pf[2] + pf[0] - pf[1] * 2;
//		rip_draw_splitconic(bufferDC,ptcalc,offsetx,offsety);
//
//		//把输入4个点分割为新的两条曲线
//		pf[0] = pf[2];
//		nremaincount -= 1;
//		nindex += 1;
//	}
//}
int BASEGRAPH_DATARENDER_API cell_extend_close(OUT vectorcell* descell, IN vectorcell* srccell,
	IN vector2f* vOffset, IN int ndirect, IN float fwidth)
{
	int nfirstsize = 0;
	vectorcell dessrc;
	memset(&dessrc, 0, sizeof(vectorcell));
	//if(srccell->type == uncloss_line)
	{
		vectorcell firstCell, seccell;
		memset(&firstCell, 0, sizeof(vectorcell));
		memset(&seccell, 0, sizeof(vectorcell));

		int nret = cell_extend(&firstCell,
			srccell,
			-ndirect,
			srccell->width / 2.0f + fwidth);
		nfirstsize = firstCell.npoint;
		if (nret)
		{
			nret = cell_extend(&seccell,
				srccell,
				ndirect,
				srccell->width / 2.0f + fwidth);

			cell_reverse(&seccell);
			cell_merge(descell, &firstCell, &seccell);
			cell_release(&firstCell);
			cell_release(&seccell);
		}
	}

	descell->type = close_anticlockwise_region;
	return nfirstsize;
}

//计算形变插值
BASEGRAPH_DATARENDER_API void  rip_figure_X(IN vectorgraph* figure,IN OUT void*	buf,IN int bits,IN unsigned long color,IN int nwidth,
					IN int nheight,IN	float fscale,IN pointf* ptOrigin,IN	unsigned char* pool,IN	unsigned int	poolsize)
{
	if(figure->ncell <= 0)
	{
		return;
	}
#ifdef _DEBUG
	LARGE_INTEGER t_freqency;
	LARGE_INTEGER t_start;
	LARGE_INTEGER t_end;
	QueryPerformanceFrequency(&t_freqency);
#endif

	BYTE* pool_end = pool + poolsize;
	BYTE* pool_idel_ptr = 0;
	//计算图形的范围
	rectf boundf;
	//探测是否有曲线类型，如果有必须计算正方向
	int ndirect = 0;
	for(int i = 0;i< figure->ncell;i ++)
	{
		if(figure->cell[i].type == uncloss_line || figure->cell[i].type == close_line)
		{
			ndirect = cell_analyzedirect(&figure->cell[0]);
			break;
		}
	}

	if(figure->bound.right <= figure->bound.left || figure->bound.bottom <= figure->bound.top)
	{
		return;
	}
	//得到图形的外包矩形
	int left,top,right,bottom,leftsub,topsub,rightsub,bottomsub;
	rip_getfigurebound(figure,&boundf);

	//添加放缩影响
	BaseRectf_Scale(&boundf,fscale);

	rip_extend_fbound(&boundf,&left,&top,&right,&bottom);

	//对矩形进行偏移
	left += Common_FToI(ptOrigin->x),right += Common_FToI(ptOrigin->x),top  += Common_FToI(ptOrigin->y),bottom += Common_FToI(ptOrigin->y);

	//得到扩大g_rip_nsubpixel倍后矩形
	leftsub  = left * g_rip_nsubpixel,topsub = top * g_rip_nsubpixel,rightsub = right * g_rip_nsubpixel,bottomsub = bottom * g_rip_nsubpixel;

	int nfigureheight = bottom - top + 1;
	int nfigurewidth =	right - left + 1;


	//水平扫描线缓冲,从图像最顶点开始
	int nNodeYH = bottomsub - topsub + 1;
	int nedgenode_size = sizeof(rip_edge_node *)* nNodeYH;
	BOOL bReleaseNodeY = pool + nedgenode_size >= pool_end;

	rip_edge_node** pNodeY = NULL;
	if(bReleaseNodeY)
	{
		pNodeY = new rip_edge_node*[nNodeYH];
	}
	else
		pNodeY = (rip_edge_node**)pool_malloc(&pool,nedgenode_size,bReleaseNodeY);
	memset(pNodeY,0,nedgenode_size);
	rip_linebuf*	pLineBuf_Header = NULL;

	//最大缓冲行
	int nMaxLine = 1;

	//Rad计算数据覆盖半径,以子像素为单位
	//////////////////////////////////////////////////////////////////////////
	int nRad = (g_rip_table_size - g_rip_nsubpixel)/2;
	int nhalfcount = nRad/g_rip_nsubpixel;
	if(nRad%g_rip_nsubpixel)
		nhalfcount += 1;
	//	int nhalfcount = (nRad + (g_rip_nsubpixel - nRad%g_rip_nsubpixel))/g_rip_nsubpixel;
	//////////////////////////////////////////////////////////////////////////
	//nMaxLine是最大行数，单位是物理行数
	if(nhalfcount)
	{
		nMaxLine = nhalfcount * 2;
		nMaxLine = nMaxLine + 1;
	}
	///////////////////////////////////////////////////////////
	rip_edge_node_x_list* pNode_x_header = NULL;

	int nmin_y = nheight * g_rip_nsubpixel - 1,nmax_y = 0;

	//创建一个buf池
	bufpool_mgr mgr(nheight,nMaxLine + 1,nwidth,pool,pool_end);
	//rip_linebuf* pbuf = mgr.get_linebuf(0);
	//if(pbuf->buf == NULL ||pbuf->buf == (unsigned long*)0xcdcdcdcd)
	//{
	//	__asm int 3;
	//}
	//printf("pbuf->buf =0x%08x\n",pbuf->buf);
	//一段曲线产生的线段个数
	int calccount = 1;
	vectorcell cellline[2];
	memset(&cellline,0,sizeof(vectorcell) * 2);
	for(int k = 0; k < figure->ncell;k ++)
	{
		vectorcell* cell = &figure->cell[k];

		if(cell->type == close_line)
		{

			calccount = 2;
			cell_extend(
				&cellline[0], // 输出偏移曲线
				&figure->cell[k],  // 输入曲线
				ndirect,
				figure->cell[k].width*0.5f  // 偏移距离
				);

			cell_extend(
				&cellline[1], // 输出偏移曲线
				&figure->cell[k],  // 输入曲线
				-ndirect,
				figure->cell[k].width*0.5f);

			cell_reverse(&cellline[1]);
			cell = &cellline[0];
		}
		if(cell->type == uncloss_line)
		{
			calccount = 1;
			//ndirect==0容易造成越界 HTH
			if(cell->width <= 0 || ndirect==0)
			{
				continue;
			}
			//如果是曲线,需要产生封闭回路
			vector2f vOffset = {0,0};
			cell_extend_close(&cellline[0],
				&figure->cell[k],
				&vOffset,
				ndirect,
				0);
			cell = &cellline[0];
		}
		for(int j = 0;j < calccount;j ++)
		{	
			for(int i = 0 ; i < cell->npoint;)
			{
				//if(i == 62)
				//	int a = 0;
				if(cell->pttype[i] == point_line)
				{
					//如果线段在一个像素内忽略
					const int loop_i = (i + 1)%cell->npoint;
					if(cell->point[i].y == cell->point[loop_i].y)
					{
						i ++;
						continue;
					}

					rip_edge_node* pnode = rip_malloc_node(pool,pool_end);
					//根据信息产生新的节点
					rip_calcnode(
						pnode,
						&cell->point[i],
						&cell->point[loop_i],//&cell->point[(i+1)%cell->npoint],
						ptOrigin,
						fscale,
						g_rip_nsubpixel);

					int y = (int)floor(pnode->start.y);
					//纵向最小最大填充行
					nmin_y = min( y ,nmin_y);
					nmax_y = max((int)(pnode->end.y + nRad),nmax_y);

					//插入节点
					//assert(y - topsub >= 0);
					if (y - topsub >= 0 && y - topsub < nNodeYH)
					{
						rip_insertnode(&pNodeY[y - topsub],pnode);
					}
					i ++;
				}
				else if(cell->pttype[i] == point_curve || cell->pttype[i] == point_curve_start) //如果是3次Bezier,那么需要分割曲线为小的线段
				{
					//计算总共有多少个曲线节点，注意最后一个曲线节点的属性可能不是曲线
					//还要注意闭合情况
					int ncuvercount = 0;
					for(int n = i;n < cell->npoint;n ++)
					{
						ncuvercount ++;
						if(cell->pttype[n] != point_curve)
							break;

						//首尾连接
						if(n == cell->npoint - 1)
						{
							ncuvercount ++;
							break;
						}
					}

					SYList NodeList;
					SYList_Init(&NodeList);

					//把曲线分割为小直线段
					rip_bezier_line(cell->point,cell->npoint,i,ncuvercount,&NodeList,ptOrigin,fscale,g_rip_nsubpixel,pool,pool_end);

					//依次插入
					LISTPOSITION pos = SYList_GetHeadPosition(&NodeList);
					while(pos)
					{
						rip_edge_node* pnode = (rip_edge_node*)SYList_GetNext(&NodeList,&pos);

						int y = (int)floor(pnode->start.y);
						nmin_y = min(y,nmin_y);
						nmax_y = max((int)(pnode->end.y + nRad),nmax_y);
						if (y - topsub >= 0 && y - topsub < nNodeYH)
						{
							rip_insertnode(&pNodeY[y - topsub],pnode);
						}
						else
						{
							//曲线分割算法过程中可能产生一些干扰点，导致y值溢出，先干掉
							//assert(FALSE);
						}
					}

					SYList_RemoveAll(&NodeList);

					if( i + ncuvercount  >= cell->npoint)
						i += ncuvercount;
					else
						i += (ncuvercount - 1);
				}
				else if(cell->pttype[i] == point_conic_start) //如果是2次Bezier,那么需要分割曲线为小的线段
				{
					int ncuvercount = 1;
					for(int n = i + 1;n < cell->npoint;n ++)
					{
						ncuvercount ++;
						if(cell->pttype[n] != point_conic)
							break;
					}

					SYList NodeList;
					SYList_Init(&NodeList);

					//把曲线分割为小直线段
					rip_conic_line(&cell->point[i],ncuvercount,&NodeList,ptOrigin,fscale,g_rip_nsubpixel,pool,pool_end);
					//rip_bezier_line

					//依次插入
					LISTPOSITION pos = SYList_GetHeadPosition(&NodeList);
					while(pos)
					{
						rip_edge_node* pnode = (rip_edge_node*)SYList_GetNext(&NodeList,&pos);

						int y = (int)floor(pnode->start.y);
						nmin_y = min(y,nmin_y);
						nmax_y = max((int)(pnode->end.y + nRad),nmax_y);

						rip_insertnode(&pNodeY[y - topsub],pnode);
					}
					SYList_RemoveAll(&NodeList);


					if( i + ncuvercount  >= cell->npoint)
					{
						rip_edge_node* pnode = rip_malloc_node(pool,pool_end);

						//根据信息产生新的节点
						rip_calcnode(
							pnode,
							&cell->point[i + ncuvercount -1],
							&cell->point[(i + ncuvercount)%cell->npoint],
							ptOrigin,
							fscale,
							g_rip_nsubpixel);

						int y = (int)floor(pnode->start.y);
						//纵向最小最大填充行
						nmin_y = min( y ,nmin_y);
						nmax_y = max((int)(pnode->end.y + nRad),nmax_y);

						//插入节点
						assert(y - topsub >= 0);
						rip_insertnode(&pNodeY[y - topsub],pnode);

						i += ncuvercount;

					}
					else
						i += (ncuvercount - 1);
				}
				else
				{
					//__asm int 3;
					assert(FALSE);
				}
			}
			cell_release(&cellline[j]);
			memset(&cellline[j],0,sizeof(vectorcell));
			cell++;
		}

	}
	//计算到目前为止nmin_y,nmax_y以及节点中的坐标都是在buf坐标下
	//存储数组位置是按图形左上角点为原点的
	if(nmin_y > nmax_y)
		return;
	//一开始就把开始被影响的行加入进去,这时候只有当前行以后的行
	if(nmin_y < 0)
		nmin_y = 0;

	//得到最小行的实际行数和余数
	//int nstarta = (nmin_y - nRad)/g_rip_nsubpixel;
	int nstarta = (g_rip_nsubpixel == 4) ? ((nmin_y - nRad)>>2) : (nmin_y - nRad)/g_rip_nsubpixel;
	//int nstartm = (nmin_y - nRad)%g_rip_nsubpixel;
	int nstartm = (nmin_y - nRad) - g_rip_nsubpixel*nstarta;
	int nenda = (nmin_y + nRad)/g_rip_nsubpixel;
	//nstarta = min(nstarta,nheight  - 1);
	//nstarta = max(nstarta,0);
	//nenda = min(nenda,nheight - 1);
	//nenda = max(nenda,0);
	int nlinesub = nstartm;
	mgr.m_nMinIndex = nstarta;
	mgr.m_nMaxIndex = nenda;

	for(int k = nstarta;k <= nenda;k ++)
	{
		if(k < 0)
			continue;
		if( k > nheight - 1)
			continue;

		rip_linebuf* pbuf = mgr.get_linebuf(k);
		pbuf->ncursuby = nmin_y - ((k << g_rip_nsubsh/** g_rip_nsubpixel*/) - nRad);
		pbuf->nmin_x = right;
		pbuf->nmax_x = left;
		int a = 0;
	}

	mgr.m_nMinIndex = mgr.m_nMinIndex< 0 ? 0 : mgr.m_nMinIndex;
	mgr.m_nMinIndex = mgr.m_nMinIndex> nheight ? nheight : mgr.m_nMinIndex;

	mgr.m_nMaxIndex = mgr.m_nMaxIndex< 0 ? 0 : mgr.m_nMaxIndex;
	mgr.m_nMaxIndex = mgr.m_nMaxIndex> nheight ? nheight : mgr.m_nMaxIndex;

	//最小影响行和最大影响行
	//逐行光栅化
	int nmin_x = right,nmax_x = left;
	int nmaxy = max(nheight,nmax_y);
	int y = 0;
	int linestarty = 0;

	//横向扫描线,半径,最小y.
	//把在范围之外的节点移入数组，以便加入计算
	for(int l = 0 ;l < nmin_y - topsub ; l ++)
	{
		if( l < nNodeYH)
			rip_addnode_x(pNode_x_header,pNodeY[l],l + topsub);
		//移出不需要的节点
		rip_remove_node_x(pNode_x_header,l + topsub);
	}

	int nTick1 = 0,nTick2 = 0;
	int nTick = GetTickCount();
	int nlasth = 0;
	int nlasti = 0;
	//这个循环是此函数的热点
	for(int h = 0 ; h < nheight ; h ++)
	{	
		if (h == nheight - 4)
			int a = 0;
		//有效像素行
		for(int i = 0 ; i < g_rip_nsubpixel ; i++)
		{
			//最小位置
			if(y < nmin_y || y < 0 )
			{
				y ++;
				continue;
			}

			//最大位置
			if(y >= nmax_y )
				break;

			//更新行参数
			//这一段链表处理也是这个循环中的时间热点之一
			rip_update_node_x(pNode_x_header,y);
			//新行加入行链表
			if( y >= topsub && y - topsub < nNodeYH)
				rip_addnode_x(pNode_x_header,pNodeY[y - topsub],y);

			//移出不需要的节点
			rip_remove_node_x(pNode_x_header,y);

			//排序
			rip_sort_x_node(pNode_x_header);

			//此函数调用是这个循环中的热点
			nmin_x = max(0,nmin_x);
			nmax_x = max(0,nmax_x);
			nmin_x = min(nmin_x,nwidth);
			nmax_x = min(nmax_x,nwidth);

			rip_x_node(pNode_x_header,h,i,&mgr,nmin_x,nmax_x);
			y++;
			nlasth = h;
			nlasti = i;
			if(bits == 8)
			{
				rip_update_buf_byte((unsigned char*)buf,nwidth,nheight,h,i,&mgr,nRad,top,FALSE);
				
			}
			else if(bits == 32)
				rip_update_buf((unsigned long*)buf,color,nwidth,nheight,h,i,&mgr,nRad,0);
		}
		linestarty += g_rip_nsubpixel;
	}

	if(bits == 8)
		rip_update_buf_byte((unsigned char*)buf,nwidth,nheight,nlasth,nlasti,&mgr,nRad,top,TRUE);
	else if(bits == 32)
		rip_update_buf((unsigned long*)buf,color,nwidth,nheight,nlasth,nlasti,&mgr,nRad,0);
	if(pNode_x_header)
	{
		rip_remove_node_x(pNode_x_header,nNodeYH + 5);
	}

	if(pNodeY)
	{
		for( int i = 0 ;i< nNodeYH; i++)
		{

			rip_edge_node* pnode = pNodeY[i];
			rip_edge_node* pnodenext = NULL;
			while(pnode)
			{
				pnodenext = pnode->pNext;
				if(pnode->canrelease)
					delete pnode;
				pnode = pnodenext;
			}
			pNodeY[i] = NULL;
		}
		if(bReleaseNodeY)
			delete[] pNodeY;
	}

}

//合并图像和透明度,包括一次Mix
BASEGRAPH_DATARENDER_API void  render_merge_bufalpha_line(IN OUT unsigned long* desbuf,IN unsigned char* alpha,IN int na16,IN int nm16)
{
	//_asm{
	//	mov			ecx, na16;
	//	mov			edi, des;
	//	mov			esi, alpha;
	//	test		ecx, ecx;
	//	jz			loop_8;
	//	movups		xmm1, [edi];
	//	movd		xmm2, [esi];
	//}
}
BASEGRAPH_DATARENDER_API void  render_mul_alpha(	IN	unsigned char*	alpha,
										IN	int				nwidth,
										IN	int				nheight,
										IN	int				srcalpha)
{
	//合并Alpha
	BYTE * pTempMaskBuffer = alpha;
	DWORD dwsrcAlpha  = 0;
	for(int n = 0;n < nheight ; n ++)
	{
		for(int k = 0;k < nwidth ; k ++)
		{
			dwsrcAlpha = pTempMaskBuffer[k];
			dwsrcAlpha = (dwsrcAlpha * srcalpha + 0xff) >> 8;
			pTempMaskBuffer[k] = (BYTE)dwsrcAlpha;
		}
		pTempMaskBuffer += nwidth;
	}
}
//合并图像和透明度,包括一次Mix
BASEGRAPH_DATARENDER_API void  render_merge_bufalpha(IN OUT	unsigned long*	desbuf,
											IN		unsigned char*	alpha,
											IN		int				nwidth,
											IN		int				nheight)
{
	//合并Alpha
	BYTE * pTempMaskBuffer = alpha;
	DWORD * pTempDesBuffer = desbuf;

	//做C级别的优化  对于典型的一个二维图表，570×540的buffer，从2.8ms减到0.98ms
	BYTE* pMask = pTempMaskBuffer;
	DWORD* pDes = pTempDesBuffer;
	int nSize = nwidth * nheight;
	for(int i = 0;i < nSize;i ++)
	{
		if(*pMask != 255)
		{
			const int dst_alpha = (*pDes)>>24;
			*pDes &= 0x00ffffff;
			if(*pMask != 0)
			{
				if(dst_alpha == 255)
					*pDes |= ((*pMask) <<24);
				else if(dst_alpha != 0)
					*pDes |= (((dst_alpha * (*pMask) + 0xff) >> 8) <<24);
			}
		}
		++pMask;
		++pDes;
	}
}

BASEGRAPH_DATARENDER_API void  render_merge_alpha(IN OUT	unsigned long*	desbuf,
										 IN		char				alpha,
										 IN		int				nwidth,
										 IN		int				nheight)
{
	//合并Alpha
	DWORD * pTempDesBuffer = desbuf;
	DWORD dwsrcAlpha  = alpha;
	DWORD dwdesAlpha = 0;
	for(int n = 0;n < nheight ; n ++)
	{
		for(int k = 0;k < nwidth ; k ++)
		{
			dwdesAlpha = ((pTempDesBuffer[k])&0xff000000) >>24;
			dwdesAlpha = (dwdesAlpha * dwsrcAlpha + 0xff) >> 8;
			pTempDesBuffer[k]  = ((dwdesAlpha <<24)&0xff000000)|((pTempDesBuffer[k])&0x00ffffff);
		}
		pTempDesBuffer += nwidth;
	}
}


BASEGRAPH_DATARENDER_API void  render_fill_alpha(IN OUT	unsigned long*	desbuf,
										IN		unsigned char		alpha,
										IN		int				nwidth,
										IN		int				nheight)
{
	//合并Alpha
	DWORD * pTempDesBuffer = desbuf;
	DWORD dwsrcAlpha  = alpha;
	DWORD dwdesAlpha = 0;
	for(int n = 0;n < nheight ; n ++)
	{
		for(int k = 0;k < nwidth ; k ++)
		{
			dwdesAlpha = ((pTempDesBuffer[k])&0xff000000) >>24;
			if(dwdesAlpha <= dwsrcAlpha)
			{
				//dwdesAlpha = (dwdesAlpha * dwsrcAlpha + 0xff) >> 8;
				dwdesAlpha = dwsrcAlpha;
				pTempDesBuffer[k]  = ((dwdesAlpha <<24)&0xff000000)|((pTempDesBuffer[k])&0x00ffffff);
			}
		}
		pTempDesBuffer += nwidth;
	}
}


BASEGRAPH_DATARENDER_API void  render_merge_bufalpha_not(IN OUT	unsigned long*	desbuf,
												IN		int				nx,
												IN		int				ny,
												IN		int				nbufwidth,
												IN		int				nbufheight,
												IN		unsigned char*	alpha,
												IN		int				nwidth,
												IN		int				nheight)
{
	//合并Alpha
	BYTE * pTempMaskBuffer = alpha;
	DWORD * pTempDesBuffer = desbuf + ny * nbufwidth + nx;
	DWORD dwsrcAlpha  = 0;
	DWORD dwdesAlpha = 0;
	for(int n = 0;n < nheight ; n ++)
	{
		for(int k = 0;k < nwidth ; k ++)
		{
			dwsrcAlpha = pTempMaskBuffer[k];
			dwdesAlpha = ((pTempDesBuffer[k])&0xff000000) >>24;
			dwsrcAlpha = (dwsrcAlpha<<1) > 255?255:(dwsrcAlpha<<1);
			dwdesAlpha = (dwdesAlpha * (255 - dwsrcAlpha) + 0xff) >> 8;

			pTempDesBuffer[k]  = ((dwdesAlpha <<24)&0xff000000)|((pTempDesBuffer[k])&0x00ffffff);
		}
		pTempDesBuffer += nbufwidth;
		pTempMaskBuffer += nwidth;
	}
}

int BASEGRAPH_DATARENDER_API cell_analyzedirect(vectorcell * cell)
{
	if (cell->npoint == 0)
		return 0;
	int nminyindex = 0;
	float miny = 65535.0f;
	for (int i = 0; i < cell->npoint; i++)
	{
		if (miny > cell->point[i].y)
		{
			miny = cell->point[i].y;
			nminyindex = i;
		}
	}
	pointf ptcur = cell->point[nminyindex];
	//分析方向
	int nprv = (nminyindex + cell->npoint - 1) % cell->npoint;
	while (ptcur == cell->point[nprv])
	{
		nprv = (nprv + cell->npoint - 1) % cell->npoint;
		if (nprv == nminyindex)
			return 0;
	}
	int nnext = (nminyindex + 1) % cell->npoint;
	while (ptcur == cell->point[nnext])
	{
		nnext = (nnext + 1) % cell->npoint;
		if (nnext == nminyindex || nnext == nprv)
			return 0;
	}
	pointf ptprv = cell->point[nprv];

	pointf ptnext = cell->point[nnext];

	pointf ptvector1 = ptcur - ptprv;
	pointf ptvector2 = ptnext - ptcur;
	pointf ptresult;
	float2Cross(ptresult, ptvector1, ptvector2);
	if (ptresult.y < 0)
		return -1;
	if (ptresult.y > 0)
		return 1;
	else if (ptresult.y == 0)
	{
		if (ptnext.x < ptcur.x)
			return 1;
		else
			return -1;
	}
	return 0;
}

BASEGRAPH_DATARENDER_API void  get_buf_alpha(IN OUT	unsigned char*	alpha,
									IN  		unsigned long*	srcbuf,
									IN		int				nwidth,
									IN		int				nheight)
{
	BYTE* pMask = alpha;
	DWORD* pSrc = srcbuf;

	DWORD dwSrcAlpha = 0;
	for(int n = 0;n < nheight ; n ++)
	{
		for(int k = 0;k < nwidth ; k ++)
		{
			dwSrcAlpha = ((pSrc[k]) & 0xff000000) >> 24;
			pMask[k]  = dwSrcAlpha == 0 ? 0 : 0xff;
		}
		pSrc += nwidth;
		pMask += nwidth;
	}
}

void	BASEGRAPH_DATARENDER_API rip_recalcbound(vectorgraph* figure)
{
	figure->bound.left = 65535.0f;
	figure->bound.top = 65535.0f;
	figure->bound.right = -65535.0f;
	figure->bound.bottom = -65535.0f;
	for(int i=0; i < figure->ncell; i++)
	{
		int ptcount =  figure->cell[i].npoint;
		figure->cell[i].bound.left = 65535.0f;
		figure->cell[i].bound.top = 65535.0f;
		figure->cell[i].bound.right = -65535.0f;
		figure->cell[i].bound.bottom = -65535.0f;
		for(int j = 0; j<ptcount; j++)
		{
			figure->cell[i].bound.left = min(figure->cell[i].bound.left,figure->cell[i].point[j].x);
			figure->cell[i].bound.top = min(figure->cell[i].bound.top,figure->cell[i].point[j].y);
			figure->cell[i].bound.right = max(figure->cell[i].bound.right,figure->cell[i].point[j].x);
			figure->cell[i].bound.bottom = max(figure->cell[i].bound.bottom,figure->cell[i].point[j].y);
		}
		figure->bound.left = min(figure->bound.left,figure->cell[i].bound.left);
		figure->bound.top = min(figure->bound.top,figure->cell[i].bound.top);
		figure->bound.right = max(figure->bound.right,figure->cell[i].bound.right);
		figure->bound.bottom = max(figure->bound.bottom,figure->cell[i].bound.bottom);
	}
}
long BASEGRAPH_DATARENDER_API rip_trans_float_to_fix(float f)
{
	return Common_FToI(f * 64);
}

//void BASEGRAPH_DATARENDER_API rip_draw_figure(IN vectorgraph*	figure,	IN float offsetx,IN float offsety,IN CCommonBufferDC* bufferDC)
//{
//	vectorcell cellline;
//	memset(&cellline,0,sizeof(vectorcell));
//	for(int k = 0; k < figure->ncell;k ++)
//	{
//		vectorcell* cell = &figure->cell[k];
//		if(cell->type == uncloss_line || cell->type == close_line)
//		{
//			// new code:
//			int ndirect = 1;
//			vectorcell firstCell,seccell;
//			memset(&firstCell,0,sizeof(vectorcell));
//			memset(&seccell,0,sizeof(vectorcell));
//			SYOffsetCell(&figure->cell[k],  // 输入曲线
//				NULL,
//				figure->cell[k].width*0.5f,  // 偏移距离
//				&firstCell, // 输出偏移曲线
//				ndirect
//				);
//
//			SYOffsetCell(&figure->cell[k],  // 输入曲线
//				NULL,
//				figure->cell[k].width*0.5f,  // 偏移距离
//				&seccell, // 输出偏移曲线
//				-ndirect
//				);
//
//			cell_reverse(&seccell);
//			cell_merge(&cellline,&firstCell,&seccell);
//			cell = &cellline;
//			SYReleaseOffsetCell(&firstCell);
//			SYReleaseOffsetCell(&seccell);
//			cell = &cellline;
//			////////////////////////////////////////////////////////////////////////// end
//		}
//		for(int i = 0 ; i < cell->npoint;)
//		{
//			if(cell->pttype[i] == point_line)
//			{
//				//如果线段在一个像素内忽略
//				if(cell->point[i].y == cell->point[(i+1)%cell->npoint].y 
//					&& cell->point[i].x == cell->point[(i+1)%cell->npoint].x)
//				{
//					i ++;
//					continue;
//				}
//				//rip_edge_node* pnode = rip_malloc_node(pool,pool_end);
//				bufferDC->MoveTo(cell->point[i].x + offsetx,cell->point[i].y + offsety);
//				bufferDC->LineTo(cell->point[(i+1)%cell->npoint].x + offsetx,cell->point[(i+1)%cell->npoint].y + offsety);
//				i ++;
//			}
//			else if(cell->pttype[i] == point_curve) //如果是3次Bezier,那么需要分割曲线为小的线段
//			{
//				//计算总共有多少个曲线节点，注意最后一个曲线节点的属性可能不是曲线
//				//还要注意闭合情况
//				int ncuvercount = 0;
//				for(int n = i;n < cell->npoint;n ++)
//				{
//					ncuvercount ++;
//					if(cell->pttype[n] != point_curve)
//						break;
//				}
//				rip_draw_bezier(bufferDC,&cell->point[i],ncuvercount,offsetx,offsety);
//				//依次插入
//
//				if( i + ncuvercount  >= cell->npoint)
//					i += ncuvercount;
//				else
//					i += (ncuvercount - 1);
//			}
//			else if(cell->pttype[i] == point_conic_start) //如果是2次Bezier,那么需要分割曲线为小的线段
//			{
//				int ncuvercount = 1;
//				for(int n = i + 1;n < cell->npoint;n ++)
//				{
//					ncuvercount ++;
//					if(cell->pttype[n] != point_conic)
//						break;
//				}
//
//				//把曲线分割为小直线段
//				rip_draw_conic(bufferDC,IN &cell->point[i],ncuvercount,offsetx,offsety);
//				DWORD dwOldColor = bufferDC->SetPenColor(0xffff0000);
//				bufferDC->MoveTo(cell->point[i].x + offsetx,cell->point[i].y + offsety);
//				bufferDC->DrawCircle(cell->point[i].x + offsetx,cell->point[i].y + offsety,2);
//				for(int kk = 0;kk < ncuvercount;kk ++)
//				{
//					bufferDC->DrawCircle(cell->point[kk].x + offsetx,cell->point[kk].y + offsety,2);
//					//bufferDC->LineTo(cell->point[kk].x + offsetx,cell->point[kk].y + offsety);
//				}
//				bufferDC->SetPenColor(dwOldColor);
//				//rip_conic_line(&cell->point[i],ncuvercount,&NodeList,ptOrigin,fscale,g_rip_nsubpixel,pool,pool_end);
//				//rip_bezier_line
//				if( i + ncuvercount  >= cell->npoint)
//				{
//					//					bufferDC->DrawCircle(cell->point[i + ncuvercount -1].x,cell->point[i + ncuvercount -1].y,2);
//					bufferDC->MoveTo(cell->point[i + ncuvercount -1].x + offsetx,cell->point[i + ncuvercount -1].y + offsety);
//					bufferDC->LineTo(cell->point[(i + ncuvercount)%cell->npoint].x + offsetx,cell->point[(i + ncuvercount)%cell->npoint].y + offsety);
//					i += ncuvercount;
//
//				}
//				else
//					i += (ncuvercount - 1);
//			}
//			else
//			{
//				//__asm int 3;
//				assert(0);
//			}
//		}
//	}
//}
void  _curve_calcvalue(OUT point2f* ptout, point2f* pt, float t)
{
	ptout->y = (1 - t)*(1 - t)*(1 - t)* pt[0].y
		+ 3 * t * (1 - t) * (1 - t) * pt[1].y
		+ 3 * t * t * (1 - t) * pt[2].y
		+ t * t * t * pt[3].y;

	ptout->x = (1 - t)*(1 - t)*(1 - t)* pt[0].x
		+ 3 * t * (1 - t) * (1 - t) * pt[1].x
		+ 3 * t * t * (1 - t) * pt[2].x
		+ t * t * t * pt[3].x;
	return;
}

int  curve_getminpt(point2f* pt, float* t, float* miny, float* minx)
{
	*miny = pt[0].y;
	*minx = pt[0].x;
	*t = 0.0f;
	if (pt[3].y < *miny)
	{
		*miny = pt[1].y;
		*minx = pt[1].x;
		*t = 1.f;
	}

	float a = 3 * pt[1].y - 3 * pt[2].y + pt[3].y - pt[0].y;
	float b = 2 * pt[0].y - 4 * pt[1].y + 2 * pt[2].y;
	float c = pt[1].y - pt[0].y;

	float fd = (b * b - 4 * a * c);
	if (fd < 0)
	{
		return 1;
	}
	fd = sqrt(fd);

	float mint = (-b + fd) / 2;
	if (mint > 0 && mint < 1.0f)
	{
		point2f ptout;
		_curve_calcvalue(&ptout, pt, mint);
		if (ptout.y < *miny)
		{
			*miny = ptout.y;
			*minx = ptout.x;
			*t = mint;
		}
	}
	mint = (-b - fd) / 2;
	if (mint > 0 && mint < 1.0f)
	{
		point2f ptout;
		_curve_calcvalue(&ptout, pt, mint);
		if (ptout.y < *miny)
		{
			*miny = ptout.y;
			*minx = ptout.x;
			*t = mint;
		}
	}
	return 1;
}

int  conic_getminpt(point2f* pt, float* t, float* miny, float* minx)
{
	float fBase = (pt[0].y - 2 * pt[1].y + pt[2].y);
	if (abs(fBase) <ABS_FLOAT_0)
	{
		*t = 0.f;
		*miny = -65535.0f;
		*minx = -65535.0f;
		return -1;
	}
	*t = (pt[0].y - pt[1].y) / fBase;
	if (*t <= 0)
	{
		*miny = pt[0].x;
		*miny = pt[0].y;
	}
	else if (*t >= 1.0f)
	{
		*miny = pt[1].x;
		*miny = pt[1].y;
	}
	else
	{
		*miny = (1 - *t) * (1 - *t) * pt[0].y + 2 * *t * (1 - *t) * pt[1].y + *t * *t * pt[2].y;
		*minx = (1 - *t) * (1 - *t) * pt[0].x + 2 * *t * (1 - *t) * pt[1].x + *t * *t * pt[2].x;
	}
	return 1;
}

int  cell_get_mindirect(vectorcell*	cell, float* fminy, float* fminx, int* ndirect)
{
	if (cell->npoint == 0)
		return 0;
	*fminx = 65535.0f, *fminy = 65535.0f;
	point2f pt[3];
	int nindex;

	for (int i = 0; i < cell->npoint; i++)
	{
		if (cell->pttype[i] == point_line)
		{
			if (*fminy > cell->point[i].y)
			{
				*fminy = cell->point[i].y;
				nindex = i;
				pt[1] = cell->point[i];
				int nprv = (i - 1 + cell->npoint) % cell->npoint;
				int nnext = (i + 1) % cell->npoint;
				do
				{
					if (nprv == i)
						return 0;
					//_asm int 3;
					pt[0] = cell->point[nprv];
					nprv = (nprv - 1 + cell->npoint) % cell->npoint;
				} while (float2Equal(&pt[1], &pt[0]));

				do
				{
					if (nnext == i)
						return 0;
					//_asm int 3;
					pt[2] = cell->point[nnext];
					nnext = (nnext + 1) % cell->npoint;
				} while (float2Equal(&pt[1], &pt[2]));

				if (SYOC_PtOnRight(pt[0], pt[1], pt[2]))
				{
					*ndirect = -1;
				}
				else
					*ndirect = 1;

			}
		}
		else if (cell->pttype[i] == point_conic_start)
		{
			int nconiccount = 1;
			for (int k = i + 1; k < cell->npoint; k++)
			{
				if (cell->pttype[k] == point_conic)
					nconiccount++;
			}
			nconiccount += 1;


			point2f pt[3];

			for (int k = 1; k < nconiccount - 1; k++)
			{
				if (k == 0)
					pt[0] = cell->point[i + k - 1];
				else
					pt[0] = (cell->point[i + k] + cell->point[(i + k - 1 + cell->npoint) % cell->npoint]) / 2;
				pt[1] = cell->point[i + k];

				if (k == nconiccount - 2)
					pt[2] = cell->point[(i + k + 1) % cell->npoint];
				else
					pt[2] = (cell->point[i + k] + cell->point[(i + k + 1) % cell->npoint]) / 2;
				float t, y, x;
				conic_getminpt(pt, &t, &y, &x);
				if (*fminy > y)
				{
					if (t < ABS_FLOAT_0)
						*ndirect = -1;
					//else if(abs(t - 1) < ABS_FLOAT_0)
					//	*ndirect = -1;
					else
					{
						if (SYOC_PtOnRight(pt[0], pt[1], pt[2]))
							*ndirect = -1;
						else
							*ndirect = 1;
					}


					*fminy = y;
					*fminx = x;
					nindex = i + k;
				}
			}
		}
		else if (cell->pttype[i] == point_curve_start || cell->pttype[i] == point_curve)
		{
			int ncurvecount = 1;
			for (int k = i + 1; k < cell->npoint; k++)
			{
				if (cell->pttype[k] == point_curve)
				{
					ncurvecount++;
				}
				else
					break;
			}
			ncurvecount += 1;

			point2f pt[4];
			int nindex;
			for (int k = 0; k < ncurvecount - 1; k += 3)
			{
				pt[0] = cell->point[(i + k) % cell->npoint];
				pt[1] = cell->point[(i + k + 1) % cell->npoint];
				pt[2] = cell->point[(i + k + 2) % cell->npoint];
				pt[3] = cell->point[(i + k + 3) % cell->npoint];

				float t, y, x;

				curve_getminpt(pt, &t, &y, &x);
				if (*fminy > y)
				{
					if (t < ABS_FLOAT_0)
					{
						*ndirect = -1;

						if (SYOC_PtOnRight(pt[0], pt[1], pt[2]))
							*ndirect = -1;
						else
							*ndirect = 1;
					}
					else if (abs(t - 1) < ABS_FLOAT_0)
					{
						*ndirect = 1;
						if (SYOC_PtOnRight(pt[0], pt[1], pt[2]))
							*ndirect = -1;
						else
							*ndirect = 1;
						//if(SYOC_PtOnRight(pt[2],pt[3],cell->point[(i + k + 1) % cell->npoint]))
						//	*ndirect = -1;
						//else
						//	*ndirect = -1;
					}
					else
					{
						if (SYOC_PtOnRight(pt[0], pt[1], pt[2]))
							*ndirect = 1;
						else
							*ndirect = -1;

					}

					*fminy = y;
					*fminx = x;
					nindex = i + k;
				}
			}
			i += ncurvecount - 1;
		}
	}
	return 1;
}


int  figure_analyzedirect(vectorgraph* graph)
{
	float fminy = 65535.0f;
	int nDirect = 0;
	for (int i = 0; i < graph->ncell; i++)
	{
		vectorcell* pcell = &graph->cell[i];
		float x, y;
		int d;
		cell_get_mindirect(pcell, &y, &x, &d);
		if (y < fminy)
		{
			nDirect = d;
			fminy = y;
		}
	}
	return nDirect;
}

int  cell_line_extend_close(OUT vectorcell*	descell1,
	OUT vectorcell*	descell2,
	IN  vectorcell*	srccell,
	vector2f* vOffset,
	IN  int ndirect,
	IN  float fwidth)
{
	vectorcell dessrc;
	memset(&dessrc, 0, sizeof(vectorcell));

	int nret = cell_extend(descell1,
		srccell,
		ndirect,
		fwidth / 2);

	nret = cell_extend(descell2,
		srccell,
		-ndirect,
		fwidth / 2);


	//SYOffsetCell(srccell,  // 输入曲线
	//	vOffset,
	//	fwidth * 0.5f,  // 偏移距离
	//	descell1, // 输出偏移曲线
	//	ndirect
	//	);

	//SYOffsetCell(srccell,  // 输入曲线
	//	vOffset,
	//	fwidth * 0.5f,  // 偏移距离
	//	descell2, // 输出偏移曲线
	//	-ndirect
	//	);

	if (ndirect > 0)
		cell_reverse(descell1);
	else
		cell_reverse(descell2);

	descell1->type = close_anticlockwise_region;
	descell2->type = close_anticlockwise_region;
	return 1;
}

int  figure_extend(
	OUT	vectorgraph*		desfigure,
	IN	vectorgraph*		srcfigure,
	IN	vector2f* vOffset,
	IN	float fwidth,
	IN	int ndirect,
	IN	BOOL bForceLine)
{

	//生成新的cell;
	int ncellcount = srcfigure->ncell;
	desfigure->ncell = srcfigure->ncell;

	if (!srcfigure->cell)
		return 0;
	if (!ndirect)
		ndirect = figure_analyzedirect(srcfigure);

	desfigure->bound.left = 65535.0f;
	desfigure->bound.top = 65535.0f;
	desfigure->bound.right = -65535.0f;
	desfigure->bound.bottom = -65535.0f;
	int nDesCount = 0;
	for (int i = 0; i < ncellcount; i++)
	{
		if (srcfigure->cell[i].type == uncloss_line)
		{
			nDesCount += 2;
		}
		else if (srcfigure->cell[i].type == close_line || bForceLine)
		{
			nDesCount += 4;
		}
		else
			nDesCount += 2;

	}
	desfigure->ncell = nDesCount;
	desfigure->cell = new vectorcell[desfigure->ncell];
	memset(desfigure->cell, 0, sizeof(vectorcell) * desfigure->ncell);

	int nDesIndex = 0;
	for (int i = 0; i < ncellcount; i++)
	{
		//if(srcfigure->cell[i].type == uncloss_line)
		//{
		//	fwidth = -fwidth * ndirect;
		//}
		if (srcfigure->cell[i].npoint <= 1)
			continue;
		if (srcfigure->cell[i].type == uncloss_line)
		{
			vectorcell celltemp;
			cell_extend_close(
				&celltemp,
				&srcfigure->cell[i],
				vOffset,
				ndirect,
				0);

			cell_line_extend_close(
				&desfigure->cell[nDesIndex],
				&desfigure->cell[nDesIndex + 1],
				&celltemp,
				vOffset,
				ndirect,
				fwidth);

			cell_release(&celltemp);

			//cell_copy(&desfigure->cell[nDesIndex],&srcfigure->cell[i]);
			//rip_offsetfigure(desfigure,vOffset->x,vOffset->y);

			//desfigure->cell[nDesIndex].width += fwidth;
			//desfigure->cell[nDesIndex].bound.left -= fwidth;
			//desfigure->cell[nDesIndex].bound.top -= fwidth;
			//desfigure->cell[nDesIndex].bound.right += fwidth;
			//desfigure->cell[nDesIndex].bound.bottom += fwidth;

			desfigure->bound.left = min(desfigure->bound.left, desfigure->cell[nDesIndex].bound.left);
			desfigure->bound.top = min(desfigure->bound.top, desfigure->cell[nDesIndex].bound.top);
			desfigure->bound.right = max(desfigure->bound.right, desfigure->cell[nDesIndex].bound.right);
			desfigure->bound.bottom = max(desfigure->bound.bottom, desfigure->cell[nDesIndex].bound.bottom);
			desfigure->bound.left = min(desfigure->bound.left, desfigure->cell[nDesIndex + 1].bound.left);
			desfigure->bound.top = min(desfigure->bound.top, desfigure->cell[nDesIndex + 1].bound.top);
			desfigure->bound.right = max(desfigure->bound.right, desfigure->cell[nDesIndex + 1].bound.right);
			desfigure->bound.bottom = max(desfigure->bound.bottom, desfigure->cell[nDesIndex + 1].bound.bottom);
			nDesIndex += 2;
		}
		else if (srcfigure->cell[i].type == close_line || bForceLine)
		{
			vectorcell celltemp1, celltemp2;
			cell_line_extend_close(
				&celltemp1,
				&celltemp2,
				&srcfigure->cell[i],
				vOffset,
				ndirect,
				srcfigure->cell[i].width);

			cell_line_extend_close(
				&desfigure->cell[nDesIndex],
				&desfigure->cell[nDesIndex + 1],
				&celltemp1,
				vOffset,
				ndirect,
				fwidth);

			cell_line_extend_close(
				&desfigure->cell[nDesIndex + 2],
				&desfigure->cell[nDesIndex + 3],
				&celltemp2,
				vOffset,
				ndirect,
				fwidth);

			cell_release(&celltemp1);
			cell_release(&celltemp2);
			//cell_copy(&desfigure->cell[nDesIndex],&srcfigure->cell[i]);
			//rip_offsetfigure(desfigure,vOffset->x,vOffset->y);

			//desfigure->cell[nDesIndex].width += fwidth;
			//desfigure->cell[nDesIndex].bound.left -= fwidth;
			//desfigure->cell[nDesIndex].bound.top -= fwidth;
			//desfigure->cell[nDesIndex].bound.right += fwidth;
			//desfigure->cell[nDesIndex].bound.bottom += fwidth;

			for (int k = 0; k < 4; k++)
			{
				desfigure->bound.left = min(desfigure->bound.left, desfigure->cell[nDesIndex + k].bound.left);
				desfigure->bound.top = min(desfigure->bound.top, desfigure->cell[nDesIndex + k].bound.top);
				desfigure->bound.right = max(desfigure->bound.right, desfigure->cell[nDesIndex + k].bound.right);
				desfigure->bound.bottom = max(desfigure->bound.bottom, desfigure->cell[nDesIndex + k].bound.bottom);
			}

			nDesIndex += 4;
		}
		else
		{
			cell_line_extend_close(
				&desfigure->cell[nDesIndex],
				&desfigure->cell[nDesIndex + 1],
				&srcfigure->cell[i],
				vOffset,
				ndirect,
				fwidth);

			desfigure->bound.left = min(desfigure->bound.left, desfigure->cell[nDesIndex].bound.left);
			desfigure->bound.top = min(desfigure->bound.top, desfigure->cell[nDesIndex].bound.top);
			desfigure->bound.right = max(desfigure->bound.right, desfigure->cell[nDesIndex].bound.right);
			desfigure->bound.bottom = max(desfigure->bound.bottom, desfigure->cell[nDesIndex].bound.bottom);
			desfigure->bound.left = min(desfigure->bound.left, desfigure->cell[nDesIndex + 1].bound.left);
			desfigure->bound.top = min(desfigure->bound.top, desfigure->cell[nDesIndex + 1].bound.top);
			desfigure->bound.right = max(desfigure->bound.right, desfigure->cell[nDesIndex + 1].bound.right);
			desfigure->bound.bottom = max(desfigure->bound.bottom, desfigure->cell[nDesIndex + 1].bound.bottom);
			nDesIndex += 2;
		}
	}
	rip_offsetfigure(desfigure, vOffset->x, vOffset->y);
	rip_recalcbound(desfigure);
	return 1;
}

int  figure_extend_Single(
	OUT	vectorgraph*		desfigure,
	IN	vectorgraph*		srcfigure,
	IN	vector2f* vOffset,
	IN	float fwidth,
	IN	int ndirect,
	IN	BOOL bForceLine)
{

	//生成新的cell;
	int ncellcount = srcfigure->ncell;
	desfigure->ncell = srcfigure->ncell;

	if (!srcfigure->cell)
		return 0;
	if (!ndirect)
		ndirect = figure_analyzedirect(srcfigure);

	desfigure->bound.left = 65535.0f;
	desfigure->bound.top = 65535.0f;
	desfigure->bound.right = -65535.0f;
	desfigure->bound.bottom = -65535.0f;
	int nDesCount = 0;
	for (int i = 0; i < ncellcount; i++)
	{
		if (srcfigure->cell[i].type == uncloss_line)
		{
			nDesCount += 1;
		}
		else if (srcfigure->cell[i].type == close_line || bForceLine)
		{
			nDesCount += 2;
		}
		else
			nDesCount += 1;

	}
	desfigure->ncell = nDesCount;
	desfigure->cell = new vectorcell[desfigure->ncell];
	memset(desfigure->cell, 0, sizeof(vectorcell) * desfigure->ncell);

	int nDesIndex = 0;
	for (int i = 0; i < ncellcount; i++)
	{
		if (srcfigure->cell[i].type == uncloss_line)
		{
			fwidth = -fwidth * ndirect;
		}
		if (srcfigure->cell[i].type == uncloss_line)
		{
			cell_extend_close(
				&desfigure->cell[nDesIndex],
				&srcfigure->cell[i],
				vOffset,
				ndirect,
				fwidth);

			rip_offsetfigure(desfigure, vOffset->x, vOffset->y);


			desfigure->bound.left = min(desfigure->bound.left, desfigure->cell[nDesIndex].bound.left);
			desfigure->bound.top = min(desfigure->bound.top, desfigure->cell[nDesIndex].bound.top);
			desfigure->bound.right = max(desfigure->bound.right, desfigure->cell[nDesIndex].bound.right);
			desfigure->bound.bottom = max(desfigure->bound.bottom, desfigure->cell[nDesIndex].bound.bottom);
			nDesIndex += 1;
		}
		else if (srcfigure->cell[i].type == close_line || bForceLine)
		{
			cell_line_extend_close(
				&desfigure->cell[nDesIndex],
				&desfigure->cell[nDesIndex + 1],
				&srcfigure->cell[i],
				vOffset,
				ndirect,
				fwidth);
			rip_offsetfigure(desfigure, vOffset->x, vOffset->y);


			for (int k = 0; k < 2; k++)
			{
				desfigure->bound.left = min(desfigure->bound.left, desfigure->cell[nDesIndex + k].bound.left);
				desfigure->bound.top = min(desfigure->bound.top, desfigure->cell[nDesIndex + k].bound.top);
				desfigure->bound.right = max(desfigure->bound.right, desfigure->cell[nDesIndex + k].bound.right);
				desfigure->bound.bottom = max(desfigure->bound.bottom, desfigure->cell[nDesIndex + k].bound.bottom);
			}

			nDesIndex += 2;
		}
		else
		{
			cell_extend_close(
				&desfigure->cell[nDesIndex],
				&srcfigure->cell[i],
				vOffset,
				ndirect,
				fwidth);

			rip_offsetfigure(desfigure, vOffset->x, vOffset->y);

			desfigure->bound.left = min(desfigure->bound.left, desfigure->cell[nDesIndex].bound.left);
			desfigure->bound.top = min(desfigure->bound.top, desfigure->cell[nDesIndex].bound.top);
			desfigure->bound.right = max(desfigure->bound.right, desfigure->cell[nDesIndex].bound.right);
			desfigure->bound.bottom = max(desfigure->bound.bottom, desfigure->cell[nDesIndex].bound.bottom);
			nDesIndex += 1;
		}
	}
	return 1;
}

int  cell_extend_cube_close(OUT vectorcell*		descell,
	IN  vectorcell*	srccell,
	IN  vector2f* vOffset,
	IN  int ndirect,
	IN  float fcubeOffset,
	IN	float fangle)
{
	vectorcell dessrc;
	memset(&dessrc, 0, sizeof(vectorcell));
	SYOffsetCellCube(srccell, vOffset, fcubeOffset, fangle, descell, ndirect);
	return 1;
}

int BASEGRAPH_DATARENDER_API figure_cube_extend(OUT vectorgraph * desfigure, IN vectorgraph * srcfigure, IN vector2f * voffset, IN float fcubewidth, IN float fangle, IN int ndirect)
{
	int ncellcount = srcfigure->ncell;
	if (!srcfigure->cell)
		return 0;

	if (!ndirect)
		ndirect = figure_analyzedirect(srcfigure);

	int nDesCount = 0;
	for (int i = 0; i < ncellcount; i++)
	{
		if (srcfigure->cell[i].type == uncloss_line)
		{
			nDesCount += 1;
		}
		else if (srcfigure->cell[i].type == close_line)
		{
			nDesCount += 2;
		}
		else
			nDesCount += 1;
	}

	desfigure->ncell = nDesCount;
	desfigure->cell = new vectorcell[nDesCount];
	memset(desfigure->cell, 0, sizeof(vectorcell) * nDesCount);

	desfigure->bound.left = 65535.0f;
	desfigure->bound.top = 65535.0f;
	desfigure->bound.right = -65535.0f;
	desfigure->bound.bottom = -65535.0f;
	//依次处理每一个单元


	int ndescell = 0;
	for (int i = 0; i < ncellcount; i++)
	{
		vectorcell celltemp[2];
		int nCount = 0;
		if (srcfigure->cell[i].type == uncloss_line)
		{
			cell_extend_close(
				&celltemp[0],
				&srcfigure->cell[i],
				voffset,
				ndirect,
				0);
			nCount = 1;
		}
		else if (srcfigure->cell[i].type == close_line)
		{
			cell_line_extend_close(
				&celltemp[0],
				&celltemp[1],
				&srcfigure->cell[i],
				voffset,
				ndirect,
				srcfigure->cell[i].width);
			nCount = 2;
		}
		if (nCount == 0)
		{
			cell_extend_cube_close(&desfigure->cell[ndescell],
				&srcfigure->cell[i],
				voffset,
				ndirect,
				fcubewidth,
				fangle);
			desfigure->bound.left = min(desfigure->bound.left, desfigure->cell[i].bound.left);
			desfigure->bound.top = min(desfigure->bound.top, desfigure->cell[i].bound.top);
			desfigure->bound.right = max(desfigure->bound.right, desfigure->cell[i].bound.right);
			desfigure->bound.bottom = max(desfigure->bound.bottom, desfigure->cell[i].bound.bottom);
			ndescell += 1;
		}
		else
		{
			for (int k = 0; k <nCount; k++)
			{
				cell_extend_cube_close(&desfigure->cell[ndescell],
					&celltemp[k],
					voffset,
					ndirect,
					fcubewidth,
					fangle);
				desfigure->bound.left = min(desfigure->bound.left, desfigure->cell[ndescell].bound.left);
				desfigure->bound.top = min(desfigure->bound.top, desfigure->cell[ndescell].bound.top);
				desfigure->bound.right = max(desfigure->bound.right, desfigure->cell[ndescell].bound.right);
				desfigure->bound.bottom = max(desfigure->bound.bottom, desfigure->cell[ndescell].bound.bottom);
				ndescell += 1;
				cell_release(&celltemp[k]);
			}
		}

	}
	if (voffset)
	{
		vg_offset(desfigure, voffset->x, voffset->y, 0);
	}
	return 1;
}
