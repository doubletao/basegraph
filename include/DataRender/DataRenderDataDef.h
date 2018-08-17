#pragma once
#include "DataRender\DrawTextDef.h"

enum EmHittestRetType
{
	Hittest_NULL,			//什么都没点中
	Hittest_Object,			//点中物件
	Hittest_CtrlPoint,		//点中控制点
};

enum EmCtrlPointIndex
{
	CtrlPoint_NULL,
	CtrlPoint_LeftTop,			//左上
	CtrlPoint_MiddelTop,		//中上
	CtrlPoint_RightTop,			//右上
	CtrlPoint_MiddleRight,		//中右
	CtrlPoint_RightBottom,		//右下
	CtrlPoint_MiddleBottom,		//中下
	CtrlPoint_LeftBottom,		//左下
	CtrlPoint_MiddelLeft,		//中左
};

typedef struct tag_sy_render_submodel
{
	DWORD	vstart;
	DWORD	vcount;
	DWORD	istart;
	DWORD	icount;
	int		nTri_Type;
	int		nTri_Num;
	void*	pVideoMaterial;
	BOOL		bShaderChange;
	int	    hPixShader[Common_POST_PROCESS_COUNT];
	bool		bSharedMaterial; //标志是否是共享的材质
}BaseGraphRenderSubModel;

typedef struct tag_common_vertex3d		// 3d vertex declaration
{
	union
	{
		// dynamic vertex coordinates
		struct { float  x, y, z; };
		vector3f	vDyn;
	};
	union
	{	// normal
		struct { float  nx, ny, nz; };
		vector3f	vNorm;
	};
	float	tu, tv;				// texture coordinates

	vector3f tangent;
	vector3f binormal;
} common_vertex3d, BaseGraphVertex3D, *LPCommonVERTEX3D;
//triangle type
enum BaseGraphTriType
{
	Common_TRIANGLE_LIST = 1,
	Common_TRIANGLE_STRIP,
	Common_TRIANGLE_FAN,
	Common_TRI_TYPE_NO_USE = 0xffffffff,
};
typedef struct tag_common_submodel		// sub model definition
{
	DWORD		vstart;
	DWORD		vcount;
	DWORD		istart;
	DWORD		icount;
	BaseGraphTriType 	nTri_Type;
	int     	nTri_Num;
	void*   	pMaterial;
} BaseGraphSubModel, *LPCommonSUBMODEL;
struct BaseGraphModelInstance
{
	matrix4f	matInstance;
	float4		TexOffset;
};
//逻辑顶点，比如立方体有8个顶点
typedef struct tag_common_logic_vertex
{
	unsigned long	type;
	vector3f		vbase;
	vector3f		vctrl1;
	vector3f		vctrl2;
}common_logic_vertex;
//逻辑线
typedef struct tag_common_logic_line
{
	int*	ivertex;
	int		ncount;
}common_logic_line;
typedef struct tag_common_logic_face
{
	int*	iline;
	int		ncount;
}common_logic_face;
typedef struct tag_common_logic_cube
{
	int*	iface;
	int		ncount;
}common_logic_cube;
typedef struct tag_symodel
{
	int				vcount;		// vertex
	LPCommonVERTEX3D	v;          // 点坐标 法向量
	int			    icount;		// triangle surfaces indexes
	DWORD*		    indecs;

	int		 	    nSeqCount;		// global sequences
	int*			pnSeq;

	DWORD*		    pdwBoneIndex;	// bone indexes for each vertexes, the array size is "vcount"
	DWORD*		    pdwBoneWeight;	// weight indexes for each vertexes, the array size is "vcount"

	BaseARray		    bonearray;					//骨骼信息

	int			    nBoneGroupCount;	// bone group definition
	DWORD*		    pdwBoneGroup;		// 4 bytes of one cell, each byte indicates a bone index

	int			    nBoneIndexCount;	// bone indexes definition
	WORD*		    pwBoneIndex;

	int				nviewsub;
	LPCommonSUBMODEL	viewsub;


	//多实例渲染
	int				nInstanceCount;
	BaseGraphModelInstance* pInstance;

	//逻辑顶点
	int				logicvcount;
	common_logic_vertex*	logicvertex;

	//逻辑线
	int				logiclcount;
	common_logic_line*	logicline;

	//逻辑面
	int				logicfcount;
	common_logic_face*	logicface;

	//逻辑体
	int				logicccount;
	common_logic_cube*	logiccube;
	tag_symodel() { Reset(); }
	BASEGRAPH_DATARENDER_API void Reset();
} BaseGraphModel, *LPCommonMODEL;

//二维形体
typedef struct tag_sy2dunit
{
	vectorgraph		graph;//轮廓
	static_effect		staticeffect;	//静态效果
	tag_sy2dunit() { Reset(); }
	BASEGRAPH_DATARENDER_API void Reset();
}SY2DUnit;
enum	eunittype
{
	UNIT_2D = 0,
	UNIT_3D,
};
typedef struct tag_sy3dunit
{
	//单元定义,mod,和整体关系
	BaseGraphModel*	pmod;//模型顶点
	tag_sy3dunit() { Reset(); }
	BASEGRAPH_DATARENDER_API void Reset();
}SY3DUnit;
//物件中的一个单元
typedef struct tag_common_unit
{
	eunittype type;
	DWORD	dwUserID;//用户自定义标识
	struct
	{
		SY2DUnit 	u2d;
		SY3DUnit 	u3d;
	}u;
	struct tag_common_unit* parent;//父节点
	int				parentbone;//绑定在父的那个节点上
	vector3f		pos;	//位置
	matrix4f		MatVToItem;//unit space to item space
							   //针对自身的原点的动作
	TCHAR			szname[32];//名称
	GUID			UnitID;
	tag_common_unit() { Reset(); }
	BASEGRAPH_DATARENDER_API void Reset();
}BaseGraphUnit;
//骨骼信息// bone of 3d model
typedef struct tag_common_bonedef
{
	int				nAnimID;	// animation ID
	int             rotationOrder;//Rotation order XYZ = 1,YXZ = 0;
	int             linkMode;  //骨骼连接方式，flusterlinkmode
	int				flags;
	int             version;
	short			parent;	// bone parameters of parent
	short			geoid;
	vector3f		pivot;	// center point
	TCHAR			name[128];//name
	matrix4f		pboneoffVMatrix;//boneMatrix
	tag_common_bonedef() { Reset(); }
	BASEGRAPH_DATARENDER_API void Reset();
} BoneDef, *LPCommonBONEDEF;
struct BaseGraphRenderStaticEffect
{
	matrix4f DynamicMat;
	BOOL	 bRepeat;
	float	 fAlpha;
};
struct BaseGraphRender2DUnit
{
	//面
	vectorgraph* graph;
	DWORD*		 buffer;
	int			 width;
	int			 height;
	RECT		 bound;
	BaseHash		 channel;
	BaseGraphRenderStaticEffect effect;
};
typedef struct tag_common_render3dunit
{
	LPCommonVERTEX3D	pVertex;
	int				nVertexCount;
	DWORD*		    pIndex;
	int				nIndexCount;
	int				hIndex;
	int				hVertex;
	int				hVertexShader;
	int				hVSParam;
	int				nSubModelCount;
	BaseGraphRenderSubModel* pSubModel;
	matrix4f*		pboneVMatrix;//for ani
								 //多实例渲染
	int				nInstanceCount;
	BaseGraphModelInstance* pInstance;
	int				hInstance;

	vector3f		vUnitMinVertex;//for render
	vector3f		vUnitMaxVertex;
	RECT			unitBound;
	matrix4f		Mat_RenderV;//从unit到最顶层的矩阵
} BaseGraphRender3DUnit, *LPCommonRENDER3DUNIT;
struct BaseGraphItemRenderUnit
{
	BaseGraphUnit*	m_pUnit;
	BOOL	m_bChanged;
	BOOL	m_bNotRender;
	int		m_dwMatrixType;
	void*	m_GpuMem;
	matrix4f m_VSelfMatrix;
	rectf	m_frtBound;
	struct
	{
		BaseGraphRender2DUnit	unit2D;
		vectorgraph*	graph;
		BaseGraphRender3DUnit	unit3D;
	};
	TCHAR szID[RENDERUNITIDLENGTH + 1];
};
//渲染通道
typedef struct tga_common_renderchannel
{
	unsigned long type;
	//buffer
	BYTE*	buffer;
	int		width;
	int		height;
	//光
	unsigned long	reflectcolor;
	unsigned long	ambience;
	unsigned long	ambience_degree;
}sy_render_channel;

enum GRAPH_ART_PROC_TYPE
{
	ARTNULL,             //无
	CONCAVO,			//凹
	CONVEX,				//凸
	BRIGHT_EDGE,		//亮边
	BEVAL_3D 			//立体边
};

enum ELIGHT_TYPE
{
	INVALID_LIGHT,
	SPOT_LIGHT,
	DIRECTION_LIGHT,
};

struct  SLight_Source
{
	ELIGHT_TYPE eType;//position ||direction
	vector3f	sPosW; //world cooidnate 
	vector3f	sPosV; //view  cooidnate 
	float		fI[3];//density
	float		fk;//distance decrease constant
	GUID		gID;//ID
};
typedef SLight_Source FAR* LPLight_Source;

#define		Common_SPHERE_CIRCLE_SEGMENT		8
const int g_nSphere_Circle_Num = 3 + (int)floor((Common_SPHERE_CIRCLE_SEGMENT - 4) / 2.f);
const int g_nSphere_Per_Circle_VertexNum = Common_SPHERE_CIRCLE_SEGMENT + 1;
const int g_nSphere_Vertex_Num = g_nSphere_Circle_Num * g_nSphere_Per_Circle_VertexNum;
const int g_nSphere_Index_Num = (g_nSphere_Circle_Num * 3 - 1) * (g_nSphere_Per_Circle_VertexNum * 3 - 1) * 6;

enum SPHERE_PARAM_PART
{
	SPHERE_MATERIAL_PARAMCHG = 0,
	SPHERE_MATERIAL_COUNT,
	SPHERE_RADIUS_CHG,
	SPHERE_PARAM_COUNT,
};

enum BaseGraphModel_GRAPHTYPE
{
	BaseGraphModelRENDER_CONE = 0,
	BaseGraphModelRENDER_SEGMENT,
	BaseGraphModelRENDER_3DFACETS,
	BaseGraphModelRENDER_SPHERE,
	BaseGraphModelRENDER_QUAD,
	BaseGraphModelRENDER_BOXWIDTHBEVEL,
	BaseGraphModelRENDER_CONEWIDTHBEVEL,
	BaseGraphModelRENDER_TUBE,
};

enum CONE_ITEM_PART
{
	CONE_BASE_MATERIAL_PARAMCHG = 0,
	CONE_FAN_MATERIAL_PARAMCHG,
	CONE_TOP_MATERIAL_PARAMCHG,
	CONE_SECTION_MATERIAL_PARAMCHG,
	CONE_MATERIAL_COUNT,
	CONE_BASE_RADIUS_CHG,
	CONE_TOP_RADIUS_CHG,
	CONE_HEIGHT_CHG,
	CONE_END_ANGLE_CHG,
	CONE_START_ANGLE_CHG,
	CONE_AUTOKEY,
	CONE_ITEM_PARAM_COUNT,
};

#define		Common_CONE_FAN_NUM					64
#define		Common_CONE_LAYER_NUM				3
#define		BOXWITHBEVEL_CIRCLEPTNUM		5//圆角矩形的圆角的剖分程度
#define		Common_ROUNDRECT_NUM				2 * BOXWITHBEVEL_CIRCLEPTNUM//圆角矩形的数量，两个倒角
#define		Common_FIRSBaseART_ARC				0//第一根横弧
#define		Common_SECONDPART_ARC				1//第一根竖弧
#define		Common_THIRDPART_ARC				2//第二根横弧
#define		Common_FOUTHPART_ARC				3//第二根竖弧
#define		Common_PER_ARC_VERNUM				2//每根弧的顶点数

// 线型
#define		BaseGraphLINE_SOLID					0
#define		BaseGraphLINE_DASH						1
#define		BaseGraphLINE_DASHDOT					2
#define		BaseGraphLINE_LDASH					3
#define		BaseGraphLINE_LDASHDOT					4
#define		BaseGraphLINE_RIPPLE					5

#define		CHANNEL_ALPHA					0
#define		CHANNEL_MATERIAL				1
#define		CHANNEL_APPENDMATERIAL			2
#define		CHANNEL_FACELIGHT				3
#define		CHANNEL_CUBELIGHT				4
#define		CHANNEL_COUNT					5
#define		SHADOW_INSERT					0x20
#define		TRANSITION_NONE					0
#define		TRANSITION_PLANE				1
#define		TRANSITION_ARC					2
#define		TRANSITION_ANTIARC				3

#define		Common_CONE_FAN_NUM					64
#define		Common_CONE_LAYER_NUM				3
#define		BOXWITHBEVEL_CIRCLEPTNUM		5//圆角矩形的圆角的剖分程度
#define		Common_ROUNDRECT_NUM				2 * BOXWITHBEVEL_CIRCLEPTNUM//圆角矩形的数量，两个倒角
#define		Common_FIRSBaseART_ARC				0//第一根横弧
#define		Common_SECONDPART_ARC				1//第一根竖弧
#define		Common_THIRDPART_ARC				2//第二根横弧
#define		Common_FOUTHPART_ARC				3//第二根竖弧
#define		Common_PER_ARC_VERNUM				2//每根弧的顶点数

#define		Common_PER_ROUNDRECT_VERTEX_MAX_NUM	(4 * BOXWITHBEVEL_CIRCLEPTNUM + 4 * Common_PER_ARC_VERNUM + 1 )//每个圆角矩形的最大顶点数

#define		Common_TUBE_CIRLE_POINT_NUM			64		
#define		Common_TUBE_LAYER_NUM				3
#define		Common_TUBE_TRIP_NUM				(Common_TUBE_CIRLE_POINT_NUM  * 2)											
#define		Common_TUBE_PROFILE_PT_NUM			(Common_TUBE_LAYER_NUM  * Common_TUBE_CIRLE_POINT_NUM)
#define		Common_TUBE_PROFILE_INDEX_NUM		((Common_TUBE_LAYER_NUM - 1) * Common_TUBE_TRIP_NUM)
#define		Common_TUBE_PROFILE_TRI_NUM			(Common_TUBE_PROFILE_INDEX_NUM - 2)

#define		Common_SEGMENT_ROW_NUM				2
#define		Common_SEGMENT_COLUMN_NUM			2

#define		CONEWITHBEVEL_MATERIAL_COUNT	3
#define		Common_SPHERE_CIRCLE_SEGMENT		8
#define		Common_QUADRANG_ROW_NUM				2
#define		Common_QUADRANG_COLUMN_NUM			2
#define		Common_QUADRANG_LAYER_NUM			2
#define		Common_TB_NUM						( Common_CONE_FAN_NUM + 2 )  //顶面和底面的顶点索引数
#define		Common_FAN_COLUMN					( Common_CONE_FAN_NUM + 5 )  //侧面列数
#define		Common_FAN_ROW						Common_ROUNDRECT_NUM     //侧面行数
#define		BOXWITHBEVEL_MATERIAL_COUNT		3
// default length of name of curve
#define		BaseGraphCURVE_NAME_LEN				16	
#define		Common_NAMELENGTH					128
#define		SIMPSHAPE_NAME_MAXLEN			32
#define		BaseGraphACTION_RESERVED_DWORD			6
#define		EVEN_TASK_MAXSIZE				2
//物件尺寸最小值
//一般用于标题字、滚屏等，曲线等物件不受此限制。
#define BaseGraphITEM_DIMENSION_MIN				4
#define OBJECT_FLAG_SIZE					32
#define OBJECT_SIGN_VALID					0x00000001		 //有效
#define OBJECT_SIGN_MANUAL					0x00000002		 //手动
#define OBJECT_SIGN_DIRECTPLAY				0x00000004		 //直接上屏
#define OBJECT_SIGN_PLAY					0x00000008		 //正在播出标志
#define OBJECT_SIGN_LOCK					0x00000010		 //锁定
#define OBJECT_SIGN_VISIBLE					0x00000020		 //可见
#define OBJECT_SIGN_MASK					0x00000040		 //Mask
#define OBJECT_SIGN_DYNCREATE				0x00000080		 //动态创建标志(播放状态下使用脚本等方式创建的对象，需要在播放停止时删除)
#define OBJECT_SIGN_CANRENDER				0x00000100		 //是否可以渲染
#define OBJECT_SIGN_MOTIONBLUR				0x00000200		 //动态模糊
#define OBJECT_SIGN_REALTIME				0x00000400		 //使用真实时间的标志，表示不受暂停的影响
#define OBJECT_SIGN_PLAYHIDE				0x00000800        //物件播放态下是否隐藏，用于占位符
#define OBJECT_SIGN_CANREPLACE				0x00001000        //物件播放态下是否隐藏，用于占位符
#define OBJECT_SIGN_OWNERRENDER				0x00002000        //内部渲染子

#define OBJECT_SIGN_SOLO					0x00004000        //SOLO状态
#define OBJECT_SIGN_REPLACEABLE				0x00008000		 //可替换, 物件名为替换名。
#define OBJECT_SIGN_WHOLERENDER				0x00010000		 //整体渲染
#define OBJECT_SIGN_ENABLE_LIGHT			0x00020000		 //是否有光源，若无则为自发光
#define OBJECT_SIGN_LIGHT_FOLLOW_ITEM		0x00040000		 //灯光是否随着物件一起动

//这些标志对应于common_object中sign_2
#define OBJECT_SIGN_GRAVITY					0x00000001		 //是否重力物件
#define OBJECT_SIGN_SELFPARAM				0x00000002		 //是否自身动画
#define OBJECT_SIGN_BILLBOARD				0x00000004		 //是否BillBoard
#define OBJECT_SIGN_TRANSPARENCE			0x00000008		 //是否透明渲染
#define OBJECT_SIGN_RECIVEINPUT				0x00000010		 //是否接受消息响应
#define OBJECT_SIGN_SEPARATE				0x00000020		 //是否作为分层物件，分层物件渲染前会清掉ZBUFFER
#define OBJECT_SIGN_DISABLERUN				0x00000040		 //是否禁止运行播出
#define OBJECT_SIGN_TIMELIMEHIDE			0x00000080		 //是否在时码区显示物件
#define MAX_PREDEFIND_MSG_ID				0x00000800		//最大预定义消息ID

#define BaseGraphUNIT_USERID_UNDEF				0		//未定义
#define BaseGraphUNIT_USERID_UNDERLINE			1		//下划线
#define BaseGraphUNIT_USERID_TEXT				2		//文字
#define BaseGraphUNIT_USERID_HEADARROW			5		//头箭头
#define BaseGraphUNIT_USERID_TAILARROW			6		//尾箭头
#define BaseGraphUNIT_USERID_SPECIAL			10		//特殊效果（浮雕、自绘背景、火焰等）

#define ALIGN_MM	0	// 中心点
#define ALIGN_TL	1	// 左上
#define ALIGN_ML	2	// 左中
#define ALIGN_BL	3	// 左下
#define ALIGN_BM	4	// 下中
#define ALIGN_BR	5	// 下右
#define ALIGN_MR	6	// 右中
#define ALIGN_TR	7	// 上右
#define ALIGN_TM	8	// 上中

#define Common_STATICEFFECT_FACE	0
#define Common_STATICEFFECT_BORDER	1
#define Common_STATICEFFECT_SHADOW	2
#define Common_STATICEFFECT_CUBE	4
#define OBJECT_NAME_SIZE		32

#define Common_STATICEFFECT_MASK_FACECOLOR		0x0000001
#define Common_STATICEFFECT_MASK_BORDERCOLOR	0x0000002
#define Common_STATICEFFECT_MASK_BORDERDISABLE	0x0000004
#define Common_STATICEFFECT_MASK_BORDERWIDTH	0x0000008
#define Common_STATICEFFECT_MASK_BORDERSTYLE	0x0000010
#define Common_STATICEFFECT_MASK_CUBECOLOR		0x0000020
#define Common_STATICEFFECT_MASK_CUBEWIDTH		0x0000040
#define Common_STATICEFFECT_MASK_SHADOWCOLOR	0x0000080
#define Common_STATICEFFECT_MASK_SHADOWOFFSETX	0x0000100
#define Common_STATICEFFECT_MASK_SHADOWOFFSETY	0x0000200
#define Common_STATICEFFECT_MASK_SHADOWSOFT		0x0000400

enum eCYLTEXCTYPE
{
	CYLTEXCTYPE_CYLINDER_REV = 0,
	CYLTEXCTYPE_CYLINDER = 1,
	CYLTEXCTYPE_SPHERE = 2,
	CYLTEXCTYPE_PLANE = 3,
	CYLTEXCTYPE_PRISM_PLANE = 4,
};
//for 3DBOX
enum eFACETYPE
{
	FACETYPE_TOP = 0,
	FACETYPE_BOTTOM = 1,
	FACETYPE_RIGHT = 2,
};
enum SEGMENT_PART
{
	SEGMENT_MATERIAL_PARAMCHG = 0,
	SEGMENT_MATERIAL_COUNT,
	SEGMENT_WIDTH_CHG,
	SEGMENT_HEIGHT_CHG,
	SEGMENG_PAGEROLL_RADIUS,
	SEGMENG_PAGEROLL_PROCESS,
	SEGMENG_PAGEROLL_ANGLE,
	SEGMENG_PAGEROLL_CHECK,
	SEGMENT_FIT_TO_TEX,
	SEGMENT_PARAM_COUNT,
};
enum BaseGraphModel_PLANE_INDEX_TYPE
{
	SY3DPLANE_ROW_FIRST = 0,
	SY3DPLANE_COLUMN_FIRST,
};
#define		Common_SEGMENT_ROW_NUM				2
#define		Common_SEGMENT_COLUMN_NUM			2
const int g_nSegment_Vertex_Num = Common_SEGMENT_ROW_NUM * Common_SEGMENT_COLUMN_NUM;//总的顶点个数
const int g_nSegment_Index_Num = (Common_SEGMENT_ROW_NUM - 1) * (Common_SEGMENT_COLUMN_NUM - 1) * 6;//总的顶点个数
const BaseGraphSubModel g_stuSegmentSubs[SEGMENT_MATERIAL_COUNT] =
{
	{ 0, g_nSegment_Vertex_Num, 0, g_nSegment_Index_Num, Common_TRIANGLE_LIST, g_nSegment_Index_Num / 3, BseeHTable_null },
};
// 3D面
enum FACETS_PART
{
	FACETS_MATERIAL_FRONT_PARAMCHG = 0,
	FACETS_MATERIAL_BACK_PARAMCHG,
	FACETS_MATERIAL_COUNT,
};
const int g_n3DFacets_Vertex_Num = Common_SEGMENT_ROW_NUM * Common_SEGMENT_COLUMN_NUM;//总的顶点个数
const int g_n3DFacets_Index_Num = (Common_SEGMENT_ROW_NUM - 1) * (Common_SEGMENT_COLUMN_NUM - 1) * 6;//总的顶点个数
const BaseGraphSubModel g_stu3DFacetsSubs[FACETS_MATERIAL_COUNT] =
{
	{ 0, g_n3DFacets_Vertex_Num, 0, g_n3DFacets_Index_Num, Common_TRIANGLE_LIST, g_n3DFacets_Index_Num / 3, BseeHTable_null },
	{ g_n3DFacets_Vertex_Num, g_n3DFacets_Vertex_Num, g_n3DFacets_Index_Num, g_n3DFacets_Index_Num, Common_TRIANGLE_LIST, g_n3DFacets_Index_Num / 3, BseeHTable_null },
};
enum QUADRANG_PARAM_PART
{
	QUAD_TOP_MATERIAL_PARAMCHG = 0,
	QUAD_RIGHT_MATERIAL_PARAMCHG,
	QUAD_BASE_MATERIAL_PARAMCHG,
	QUAD_BACK_MATERIAL_PARAMCHG,
	QUAD_LEFT_MATERIAL_PARAMCHG,
	QUAD_FRONT_MATERIAL_PARAMCHG,
	QUAD_MATERIAL_COUNT,
	QUAD_BASE_WIDTH,
	QUAD_BASE_HEIGHT,
	QUAD_HEIGHT,
	QUAD_TOP_WIDTH,
	QUAD_TOP_HEIGHT,
	QUAD_AUTOKEY,
	QUAD_PARAM_COUNT,
};
#define		BOXWITHBEVEL_MATERIAL_COUNT		3
#define		CONEWITHBEVEL_MATERIAL_COUNT	3
const LPTSTR boxwidthbevelMatString[BOXWITHBEVEL_MATERIAL_COUNT] =
{
	_T("底面"),
	_T("侧面"),
	_T("顶面"),
};
const LPTSTR conewidthbevelMatString[CONEWITHBEVEL_MATERIAL_COUNT] =
{
	_T("顶面"),
	_T("侧面"),
	_T("底面"),
};

const BaseGraphSubModel g_stuSphereSubs[SPHERE_MATERIAL_COUNT] =
{
	{ 0, g_nSphere_Vertex_Num, 0, g_nSphere_Index_Num, Common_TRIANGLE_LIST, g_nSphere_Index_Num / 3, BseeHTable_null },
};
#define		Common_QUADRANG_ROW_NUM				2
#define		Common_QUADRANG_COLUMN_NUM			2
#define		Common_QUADRANG_LAYER_NUM			2
const int g_nQuad_FrontBackVertex_Num = Common_QUADRANG_LAYER_NUM * Common_QUADRANG_COLUMN_NUM;
const int g_nQuad_RightLeftVertex_Num = Common_QUADRANG_LAYER_NUM * Common_QUADRANG_ROW_NUM;
const int g_nQuad_BaseTopVertex_Num = Common_QUADRANG_ROW_NUM   * Common_QUADRANG_COLUMN_NUM;

const BaseGraphSubModel g_stuQuadSubs[QUAD_MATERIAL_COUNT] =
{
	{ 0,                                                                                             g_nQuad_FrontBackVertex_Num, 0,                                                                                                  g_nQuad_FrontBackVertex_Num + 2, Common_TRIANGLE_LIST, (g_nQuad_FrontBackVertex_Num + 2) / 3, BseeHTable_null },
	{ g_nQuad_FrontBackVertex_Num,                                                                   g_nQuad_RightLeftVertex_Num, g_nQuad_FrontBackVertex_Num + 2,                                                                    g_nQuad_RightLeftVertex_Num + 2, Common_TRIANGLE_LIST, (g_nQuad_RightLeftVertex_Num + 2) / 3, BseeHTable_null },
	{ g_nQuad_FrontBackVertex_Num + g_nQuad_RightLeftVertex_Num,                                     g_nQuad_FrontBackVertex_Num, g_nQuad_FrontBackVertex_Num + g_nQuad_RightLeftVertex_Num + 4,                                      g_nQuad_FrontBackVertex_Num + 2, Common_TRIANGLE_LIST, (g_nQuad_FrontBackVertex_Num + 2) / 3, BseeHTable_null },
	{ 2 * g_nQuad_FrontBackVertex_Num + g_nQuad_RightLeftVertex_Num,                                 g_nQuad_RightLeftVertex_Num, 2 * g_nQuad_FrontBackVertex_Num + g_nQuad_RightLeftVertex_Num + 6,                                  g_nQuad_RightLeftVertex_Num + 2, Common_TRIANGLE_LIST, (g_nQuad_RightLeftVertex_Num + 2) / 3, BseeHTable_null },
	{ 2 * g_nQuad_FrontBackVertex_Num + 2 * g_nQuad_RightLeftVertex_Num,                             g_nQuad_BaseTopVertex_Num,   2 * g_nQuad_FrontBackVertex_Num + 2 * g_nQuad_RightLeftVertex_Num + 8,                              g_nQuad_BaseTopVertex_Num + 2,   Common_TRIANGLE_LIST, (g_nQuad_BaseTopVertex_Num + 2) / 3,   BseeHTable_null },
	{ 2 * g_nQuad_FrontBackVertex_Num + 2 * g_nQuad_RightLeftVertex_Num + g_nQuad_BaseTopVertex_Num, g_nQuad_BaseTopVertex_Num,   2 * g_nQuad_FrontBackVertex_Num + 2 * g_nQuad_RightLeftVertex_Num + g_nQuad_BaseTopVertex_Num + 10, g_nQuad_BaseTopVertex_Num + 2,   Common_TRIANGLE_LIST, (g_nQuad_BaseTopVertex_Num + 2) / 3,   BseeHTable_null },
};
const int g_nCone_CirclePt_Num = Common_CONE_FAN_NUM + 2;//圆的顶点个数
const int g_nCone_CircleIndex_Num = Common_CONE_FAN_NUM + 2;//圆的顶点索引个数

const int g_nCone_FanPt_Num = (Common_CONE_FAN_NUM + 1) * Common_CONE_LAYER_NUM;
const int g_nCone_PerSubFan_IndexNum = ((Common_CONE_LAYER_NUM - 2) * 2 + 2) * 3;//每个侧面子部分含有的索引个数
const int g_nCone_FanIndex_Num = Common_CONE_FAN_NUM * g_nCone_PerSubFan_IndexNum;//侧面的顶点索引个数

const int g_nCone_SecPt_Num = 4 * Common_CONE_LAYER_NUM;//剖面顶点个数
const int g_nCone_SecIndex_Num = 3 * 4 * (Common_CONE_LAYER_NUM - 1);//剖面的顶点索引个数

const BaseGraphSubModel g_stuConeSubs[CONE_MATERIAL_COUNT] =
{
	{ 0,                                            g_nCone_CirclePt_Num, 0,                                                  g_nCone_CircleIndex_Num, Common_TRIANGLE_FAN,   g_nCone_CircleIndex_Num - 2, BseeHTable_null },
	{ g_nCone_CirclePt_Num,                         g_nCone_FanPt_Num,    g_nCone_CircleIndex_Num,                            g_nCone_FanIndex_Num,    Common_TRIANGLE_LIST,  g_nCone_FanIndex_Num / 3,    BseeHTable_null },
	{ g_nCone_CirclePt_Num + g_nCone_FanPt_Num,     g_nCone_CirclePt_Num, g_nCone_CircleIndex_Num + g_nCone_FanIndex_Num,     g_nCone_CircleIndex_Num, Common_TRIANGLE_FAN,   g_nCone_CircleIndex_Num - 2, BseeHTable_null },
	{ 2 * g_nCone_CirclePt_Num + g_nCone_FanPt_Num, g_nCone_SecPt_Num,    2 * g_nCone_CircleIndex_Num + g_nCone_FanIndex_Num, g_nCone_SecIndex_Num,    Common_TRIANGLE_LIST,  g_nCone_SecIndex_Num / 3,    BseeHTable_null },
};

const BaseGraphSubModel g_stuBoxWidthBevelSubs[BOXWITHBEVEL_MATERIAL_COUNT] =
{
	{ 0, Common_PER_ROUNDRECT_VERTEX_MAX_NUM + 1, 0, Common_PER_ROUNDRECT_VERTEX_MAX_NUM + 1, Common_TRIANGLE_FAN, Common_PER_ROUNDRECT_VERTEX_MAX_NUM - 1, BseeHTable_null },//顶面圆
	{ Common_PER_ROUNDRECT_VERTEX_MAX_NUM + 1, Common_PER_ROUNDRECT_VERTEX_MAX_NUM * Common_ROUNDRECT_NUM, Common_PER_ROUNDRECT_VERTEX_MAX_NUM + 1, (Common_ROUNDRECT_NUM - 1) * (Common_PER_ROUNDRECT_VERTEX_MAX_NUM - 1) * 2 * 3, Common_TRIANGLE_LIST, (Common_ROUNDRECT_NUM - 1) * (Common_PER_ROUNDRECT_VERTEX_MAX_NUM - 1) * 2, BseeHTable_null },
	{ Common_PER_ROUNDRECT_VERTEX_MAX_NUM * (Common_ROUNDRECT_NUM + 1) + 1, Common_PER_ROUNDRECT_VERTEX_MAX_NUM + 1, Common_PER_ROUNDRECT_VERTEX_MAX_NUM + 1 + (Common_ROUNDRECT_NUM - 1) * (Common_PER_ROUNDRECT_VERTEX_MAX_NUM - 1) * 2 * 3, Common_PER_ROUNDRECT_VERTEX_MAX_NUM + 1, Common_TRIANGLE_FAN, Common_PER_ROUNDRECT_VERTEX_MAX_NUM - 1, BseeHTable_null },
};
enum TUBE_ITEM_PART
{
	TUBE_BASE_MATERIAL_PARAMCHG = 0,
	TUBE_TOP_MATERIAL_PARAMCHG,
	TUBE_INNER_MATERIAL_PARAMCHG,
	TUBE_OUTER_MATERIAL_PARAMCHG,
	TUBE_MATERIAL_COUNT,
	TUBE_INNER_RADIUS_CHG,
	TUBE_OUTER_RADIUS_CHG,
	TUBE_HEIGHT_CHG,
	TUBE_END_ANGLE_CHG,
	TUBE_START_ANGLE_CHG,
	TUBE_AUTOKEY,
	TUBE_ITEM_PARAM_COUNT,
};
const BaseGraphSubModel g_stuTubeSubs[TUBE_MATERIAL_COUNT] =
{
	{ 0,											Common_TUBE_TRIP_NUM,		0,											Common_TUBE_TRIP_NUM,			Common_TRIANGLE_STRIP,Common_TUBE_TRIP_NUM - 2,		BseeHTable_null },
	{ Common_TUBE_TRIP_NUM,							Common_TUBE_TRIP_NUM,		Common_TUBE_TRIP_NUM,								Common_TUBE_TRIP_NUM,			Common_TRIANGLE_STRIP,Common_TUBE_TRIP_NUM - 2,		BseeHTable_null },
	{ Common_TUBE_TRIP_NUM * 2,							Common_TUBE_PROFILE_PT_NUM,Common_TUBE_TRIP_NUM * 2,							Common_TUBE_PROFILE_INDEX_NUM,	Common_TRIANGLE_STRIP,Common_TUBE_PROFILE_TRI_NUM,	BseeHTable_null },
	{ Common_TUBE_TRIP_NUM * 2 + Common_TUBE_PROFILE_PT_NUM,	Common_TUBE_PROFILE_PT_NUM,Common_TUBE_TRIP_NUM * 2 + Common_TUBE_PROFILE_INDEX_NUM,Common_TUBE_PROFILE_INDEX_NUM,	Common_TRIANGLE_STRIP,Common_TUBE_PROFILE_TRI_NUM,	BseeHTable_null },
};
const BaseGraphSubModel g_stuConeWidthBevelSubs[CONEWITHBEVEL_MATERIAL_COUNT] =
{
	{ 0, Common_TB_NUM, 0, Common_TB_NUM, Common_TRIANGLE_FAN, Common_TB_NUM - 2, BseeHTable_null },//顶面圆
	{ Common_TB_NUM, Common_FAN_COLUMN * Common_FAN_ROW, Common_TB_NUM, (Common_FAN_COLUMN - 1) * (Common_FAN_ROW - 1) * 6, Common_TRIANGLE_LIST, (Common_FAN_COLUMN - 1) * (Common_FAN_ROW - 1) * 2, BseeHTable_null },
	{ Common_TB_NUM + Common_FAN_COLUMN * Common_FAN_ROW, Common_TB_NUM, Common_TB_NUM + (Common_FAN_COLUMN - 1) * (Common_FAN_ROW - 1) * 6, Common_TB_NUM, Common_TRIANGLE_FAN, Common_TB_NUM - 2, BseeHTable_null },
};
