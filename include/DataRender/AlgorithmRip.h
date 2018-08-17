#ifndef __Common_RIP_INC__
#define __Common_RIP_INC__

//#include "CommonBufferDC.h"

//得到图形外包矩形
int BASEGRAPH_DATARENDER_API rip_getfigurebound(IN	vectorgraph* figure,OUT	rectf* rect);
int BASEGRAPH_DATARENDER_API rip_getcellbound(IN	vectorcell* cell,OUT	rectf* rect);
//扩展边界
int BASEGRAPH_DATARENDER_API figure_extend(OUT vectorgraph* desfigure,IN  vectorgraph* srcfigure,IN float fwidth);
int BASEGRAPH_DATARENDER_API figure_extend(OUT vectorgraph* desfigure,IN  vectorgraph* srcfigure,IN vector2f* vOffset,IN float fwidth, IN int ndirect = 0,IN BOOL forceline = FALSE);
int BASEGRAPH_DATARENDER_API figure_extend_Single(OUT vectorgraph*	desfigure,IN vectorgraph* srcfigure,IN	vector2f* vOffset, IN float fwidth,IN int ndirect = 0,IN BOOL bForceLine = FALSE);
//扩充立体边
int BASEGRAPH_DATARENDER_API figure_cube_extend(OUT vectorgraph* desfigure,IN  vectorgraph*	srcfigure,IN vector2f* vOffset,IN  float fcubewidth,IN  float fangle,IN  int ndirect);

BASEGRAPH_DATARENDER_API void rip_figure_X(IN vectorgraph* figure,IN OUT void* buf,IN int bits,IN unsigned long color,
	IN int nwidth,IN int nheight,IN float fscale,IN pointf* ptOrigin,IN unsigned char* pool = NULL,IN unsigned int poolsize = 0);

//光栅化图形
//////////////////////////////////////////////////////////////////////////
//figure	需要光栅化的图形
//buf		光栅化的目标buf
//nwidth	buf宽度
//nheight	buf高度
//ptOrigin	光栅化后原figure中心参考点在buf中的位置 
BASEGRAPH_DATARENDER_API void  rip_figure(IN vectorgraph* figure,IN OUT	unsigned long* buf,IN unsigned long color,IN int nwidth,IN int nheight,IN float fscale,OUT	pointf* ptOrigin);
//光栅化一个图形掩模
BASEGRAPH_DATARENDER_API void  rip_figure_mask(IN vectorgraph*	figure,IN OUT	unsigned char* buf,IN	int	nwidth,
					 IN	int	nheight,IN float	fscale,OUT pointf* ptOrigin,IN	unsigned char* pool,IN	 unsigned int	 poolsize);
BASEGRAPH_DATARENDER_API void  render_mul_alpha(IN OUT unsigned char*	alpha,IN	int	nwidth,IN int nheight,IN int srcalpha);
//合并图像和透明度,包括一次Mix
BASEGRAPH_DATARENDER_API void  render_merge_bufalpha(IN OUT	unsigned long*	desbuf,IN unsigned char* alpha,IN int	nwidth,IN int nheight);
BASEGRAPH_DATARENDER_API void  render_merge_alpha(IN OUT unsigned long*	desbuf,IN	char alpha,IN	int	 nwidth,IN	int	 nheight);

BASEGRAPH_DATARENDER_API void  get_buf_alpha( IN OUT unsigned char*	alpha,IN  unsigned long* srcbuf,IN	int	nwidth,IN int nheight);
BASEGRAPH_DATARENDER_API void  render_fill_alpha( IN OUT unsigned long*	desbuf,IN unsigned char alpha,IN int nwidth,IN int nheight);
BASEGRAPH_DATARENDER_API void  render_merge_bufalpha_not(IN OUT	unsigned long* desbuf,IN int nx,IN int ny,IN int nbufwidth,IN int nbufheight,IN unsigned char* alpha,IN int nwidth,IN int nheight);
int BASEGRAPH_DATARENDER_API   cell_analyzedirect( vectorcell* cell);
//光栅化一个图形单元
//////////////////////////////////////////////////////////////////////////
//cell		需要光栅化的曲线单元
//buf		光栅化的目标buf
//nwidth	buf宽度
//nheight	buf高度
//ptOrigin	光栅化后原figure中心参考点在buf中的位置 
BASEGRAPH_DATARENDER_API void  rip_vectorcell(IN vectorcell* cell,IN OUT unsigned long* buf,IN unsigned long color,IN int nwidth,IN int nheight,OUT pointf* ptOrigin);
int		BASEGRAPH_DATARENDER_API rip_init_table();
int		BASEGRAPH_DATARENDER_API rip_release_table();
void	BASEGRAPH_DATARENDER_API rip_extend_bound(int* pleft,int* ptop,int* pbottom,int* pright);
void	BASEGRAPH_DATARENDER_API rip_extend_fbound(rectf* prect,int* pleft,int* ptop,int* pright,int* pbottom);
void	BASEGRAPH_DATARENDER_API rip_extend_fbound_long(rectf* prect,long* pleft,long* ptop,long* pright,long* pbottom);
void	BASEGRAPH_DATARENDER_API rip_figure_mul_matrix(vectorgraph,void* pmatrix);
void	BASEGRAPH_DATARENDER_API rip_offsetfigure(IN vectorgraph* figure,float fx,float fy);
void	BASEGRAPH_DATARENDER_API rip_scalefigure(IN	vectorgraph* figure,float fx,float fy);
void	BASEGRAPH_DATARENDER_API rip_recalcbound(vectorgraph* figure);
void	BASEGRAPH_DATARENDER_API rip_mul_matrix(vectorgraph* pgraph,matrix4f* pmatrix);
//void	BASEGRAPH_DATARENDER_API rip_draw_figure(IN vectorgraph*	figure,	IN float offsetx,IN float offsety,IN CCommonBufferDC* bufferDC);
int		BASEGRAPH_DATARENDER_API cell_extend(OUT vectorcell*	descell,IN  vectorcell* srccell,IN  int ndirect,IN  float fwidth);
void	BASEGRAPH_DATARENDER_API cell_reverse(vectorcell* pcell);
void	BASEGRAPH_DATARENDER_API cell_merge(vectorcell* descell, const vectorcell* srccell1,vectorcell* srccell2);
int		BASEGRAPH_DATARENDER_API cell_extend_close(OUT vectorcell* descell,IN	vectorcell* srccell,IN vector2f* vOffset,IN	int ndirect, IN float fwidth);
//////////////////////////////////////////////////////////////////////////
#endif
