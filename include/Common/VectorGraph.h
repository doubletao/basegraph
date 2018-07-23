#ifndef __VECTORGRAPH_INC__
#define __VECTORGRAPH_INC__

//define 1:图的种类curvecell.type
#define uncloss_line				0			//非闭合,是线
#define close_line					1			//闭合曲线
#define close_anticlockwise_region	3			//闭合，方向逆时针
#define close_clockwise_region		4			//闭合,方向顺时针

#ifndef SAFEDELETE_ARRAY
#define SAFEDELETE_ARRAY(p)						{ if(p) delete []p; p = NULL;}
#endif

//define 2:点的种类
#define point_line  0
#define point_conic 1
#define point_curve 2
#define point_conic_start 3
#define point_curve_start 4

typedef struct tag_vectorcell{
	unsigned version;
	int type;				//曲线种类 见define 1
	float width;			//如果是曲线
	unsigned long style;	//风格
	int npoint;				//点个数
	point2f*	point;		//点数组
	unsigned long* pttype;	//点种类数组和点一一对应,见define 2
	rectf bound;
	tag_vectorcell(){Reset();}
	void Reset()
	{
		memset(this, 0, sizeof(tag_vectorcell));
	}
}vectorcell;

typedef struct tag_vectorgraph
{
	unsigned version;
	int ncell;				//图形单元个数
	vectorcell* cell;		//图形单元数组
	rectf bound;
	tag_vectorgraph(){Reset();}
	void Reset()
	{
		memset(this, 0, sizeof(vectorgraph));
	}
}vectorgraph;


typedef struct tag_vectorcell3d{
	unsigned version;
	int type;				//曲线种类 见define 1
	float width;			//如果是曲线
	int npoint;				//点个数
	point3lf*	point;			//点数组
	unsigned long* pttype;	//点种类数组和点一一对应,见define 2
	point3f tl;
	point3f rb;
}vectorcell3d;

typedef struct tag_vectorgraph3d
{
	unsigned version;
	int ncell;				//图形单元个数
	vectorcell3d* cell;		//图形单元数组
	point3lf tl;
	point3lf rb;
}vectorgraph3d;



BASEGRAPH_COMMON_API void __cdecl vg_copy(OUT vectorgraph* des, IN const vectorgraph* src);
BASEGRAPH_COMMON_API void __cdecl vg_copy_with_null_cell(vectorgraph* pdes, const vectorgraph* psrc);
BASEGRAPH_COMMON_API void __cdecl cell_copy(vectorcell* pdescell,vectorcell* psrccell);
BASEGRAPH_COMMON_API void __cdecl vg_zoom(vectorgraph* vg,float zoomx,float zoomy,float zoomz,float centerx,float centery,float centerz);
BASEGRAPH_COMMON_API void __cdecl vg_offset(vectorgraph* vg,float fx,float fy,float fz);
BASEGRAPH_COMMON_API void __cdecl vg_sym(vectorgraph* vg,float fx);
BASEGRAPH_COMMON_API void __cdecl vg_merge(vectorgraph* des, const vectorgraph* src1,vectorgraph* src2);
BASEGRAPH_COMMON_API void __cdecl vg_reverse(vectorgraph* vg);
BASEGRAPH_COMMON_API void __cdecl vg_rotate_angle(vectorgraph* vg,float x,float y,float z,float xoffset,float yoffset,float zoffset);
BASEGRAPH_COMMON_API void __cdecl vg_rotate_matrix(vectorgraph* vg,matrix4f* matrix);
BASEGRAPH_COMMON_API void __cdecl vg_fromarray(vectorgraph* pvg,SYArray* ptar);

BASEGRAPH_COMMON_API int __cdecl vg_tobezier(SYArray* ptarray,vectorgraph* pvg,size_t nIndex);
BASEGRAPH_COMMON_API void __cdecl vg_from_point2f_bezier(vectorgraph* pvg,SYArray* ptarray);
BASEGRAPH_COMMON_API void __cdecl vg_from_point3fpoly(vectorgraph* pvg,SYArray* ptarray);
BASEGRAPH_COMMON_API void __cdecl cell_release(vectorcell* cell);
BASEGRAPH_COMMON_API void __cdecl vg_release(vectorgraph* graph);
BASEGRAPH_COMMON_API bool __cdecl vg_remove_cell(vectorgraph* vg, vectorcell* cell);
#endif