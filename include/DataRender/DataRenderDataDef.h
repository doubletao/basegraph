#pragma once
#include "DataRender\DrawTextDef.h"

//是否兼容历史版本字宽2倍处理
#define SYTEXT_FONTWIDTH_X2

BASEGRAPH_DATARENDER_API BOOL		SYIsHalfAngleSpaceNew();
BASEGRAPH_DATARENDER_API void		SYSetHalfAngleSpaceNew(BOOL bSupport);
BASEGRAPH_DATARENDER_API BOOL		SYIsSupportBidiRender();
BASEGRAPH_DATARENDER_API void		SYSetSupportBidiRender(BOOL bSupport);
BASEGRAPH_DATARENDER_API BOOL		SYIsSupportKorea();
BASEGRAPH_DATARENDER_API void		SYSetSupportKorea(BOOL bSupport);
BASEGRAPH_DATARENDER_API void		SYSetEnableFeedBackFont(BOOL bEnable);
BASEGRAPH_DATARENDER_API BOOL		SYIsEnableFeedBackFont();

extern BASEGRAPH_DATARENDER_API BOOL		SY_bDigitUseLocalFont;		//数字是否使用本地字体
extern BASEGRAPH_DATARENDER_API BOOL		SY_bOnlyUseLocalFont;		//所有字只是用本地字体设置
extern BASEGRAPH_DATARENDER_API int			g_nSmallFontFactor;		//小字加粗系数
extern BASEGRAPH_DATARENDER_API PI_Quality	g_nSmallFontQuality;		//小字抗闪烁质量
extern BASEGRAPH_DATARENDER_API BOOL		g_bSmallFontCalcBorder;	//小字清晰
extern BASEGRAPH_DATARENDER_API int			g_nSBCCaseReplaceSpace;	//全角空格替换成若干半角空格，0不替换。
extern BASEGRAPH_DATARENDER_API BOOL		g_bUseChnTradBoldFunc;

BASEGRAPH_DATARENDER_API void		SYSetTitleWordRefine(BOOL bTileWordRefine);
BASEGRAPH_DATARENDER_API BOOL		SYIsTitleWordRefine();


#define IS_NOT_ASCII_CHAR(ch)	((DWORD)(ch) > 255)
#define SYGETCHNTRADBOLDFUNC	(g_bUseChnTradBoldFunc)
BASEGRAPH_DATARENDER_API void		SYSetChnTradBoldFunc(BOOL bTradBoldFunc);

//标题字转换过来时bUseBorderEditMode默认值
extern BASEGRAPH_DATARENDER_API BOOL g_bWordTransUseBorderEditMode;
BASEGRAPH_DATARENDER_API void		SYWordTransUseBorderEditMode(BOOL bUseBorderEditMode);
//////////////////////////////////////////////////////////////////////////
//单个字形信息
typedef struct tag_SYWordInfo
{
	TCHAR				wChar;					//字符编码
	int					nCharIndex;				//字符在段中的索引
	WORD				wGlyphIndex;			//字形索引
	TCHAR				szFontName[LF_FACESIZE];//实际使用的字体
												//SCRIPT_ANALYSIS关键数据副本
	WORD				fRTL;					//Rendering direction
												//SCRIPT_VISATTR关键数据副本
	WORD				uJustification;
	WORD				fClusterStart;
	WORD				fDiacritic;
	WORD				fZeroWidth;

	//尺寸数据
	float				fDistance;				//字间距
	float				fAdvanceWidth;			//Advance Width
	point2f				ptGOffset;				//字形偏移（变体后字形偏移）
	rectf				rtCell;					//字占位矩形

	bool				bGlyphIndex;			//用Unicode码取字模还是用字形索引取字模
	bool				bVRollGlyph;			//竖向排版时，本字形是经过旋转旋转得来的。
	bool				bVisible;				//可视字符（回车换行符等）
	bool				bHVNest;				//横/竖排嵌套
	int					nChgFlag;				//变化标记（见SYGlyphChg_X定义）
	unsigned long long	uCheckCode;				//校验码（64位，暂时仅使用32位。）
	vectorgraph			vgGraph;				//原始字模图形
}SYWordInfo, *LPSYWordInfo;
typedef tag_SYWordInfo SYWordInfo, *LPSYWordInfo;

//查询字符是否需要旋转
BASEGRAPH_DATARENDER_API BOOL	SYIsNeedRollInVDisplay(TCHAR szCharName);
//字体是否存在
BASEGRAPH_DATARENDER_API BOOL	SYIsFontExist(TCHAR* szFaceName);
//是否是可视字符
BASEGRAPH_DATARENDER_API bool	SYIsVisibleChar(TCHAR ch);
//字符ch是否使用本地字体
BASEGRAPH_DATARENDER_API BOOL	SYIsUseLocalFont(TCHAR ch);
//由SYFont创建HFONT
BASEGRAPH_DATARENDER_API HFONT 	SYFont_CreateFont(LPSYFont pFont, TCHAR ch, LPCTSTR szFont, BOOL bHint);
BASEGRAPH_DATARENDER_API HFONT	SYFont_CreateScaleFont(LPSYFont pFont, TCHAR ch, LPCTSTR szFont, int nScaleW, int nScaleH);

BASEGRAPH_DATARENDER_API void	SYWordInfoInit(LPSYWordInfo pWordInfo);
BASEGRAPH_DATARENDER_API void	SYWordInfoRelease(LPSYWordInfo pWordInfo);

BASEGRAPH_DATARENDER_API BOOL	SYWordInfoFill(HDC hDC,					//如果NULL则函数内部申请
	LPSYWordInfo pWordInfo,		//WordInfo不持有任何未释放的空间
	TCHAR* pChar,				//非空时强制使用该Char取字模
	LPSYFont pFont,
	BOOL bHorizontal);


BASEGRAPH_DATARENDER_API LPWSTR	SYCopyToWChar(LPWSTR lpwsz, LPCTSTR str, UINT CodePage = CP_ACP);
BASEGRAPH_DATARENDER_API LPSTR	SYCopyToChar(LPSTR lpsz, LPCTSTR str, UINT CodePage = CP_ACP);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	int	    hPixShader[SY_POST_PROCESS_COUNT];
	bool		bSharedMaterial; //标志是否是共享的材质
}SYRenderSubModel;

typedef struct tag_syvertex3d		// 3d vertex declaration
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
} syvertex3d, SYVertex3D, *LPSYVERTEX3D;
//triangle type
enum SYTriType
{
	SY_TRIANGLE_LIST = 1,
	SY_TRIANGLE_STRIP,
	SY_TRIANGLE_FAN,
	SY_TRI_TYPE_NO_USE = 0xffffffff,
};
typedef struct tag_sysubmodel		// sub model definition
{
	DWORD		vstart;
	DWORD		vcount;
	DWORD		istart;
	DWORD		icount;
	SYTriType 	nTri_Type;
	int     	nTri_Num;
	void*   	pMaterial;
} SYSubModel, *LPSYSUBMODEL;
struct SYModelInstance
{
	matrix4f	matInstance;
	float4		TexOffset;
};
//逻辑顶点，比如立方体有8个顶点
typedef struct tag_sylogic_vertex
{
	unsigned long	type;
	vector3f		vbase;
	vector3f		vctrl1;
	vector3f		vctrl2;
}sylogic_vertex;
//逻辑线
typedef struct tag_sylogic_line
{
	int*	ivertex;
	int		ncount;
}sylogic_line;
typedef struct tag_sylogic_face
{
	int*	iline;
	int		ncount;
}sylogic_face;
typedef struct tag_sylogic_cube
{
	int*	iface;
	int		ncount;
}sylogic_cube;
typedef struct tag_symodel
{
	int				vcount;		// vertex
	LPSYVERTEX3D	v;          // 点坐标 法向量
	int			    icount;		// triangle surfaces indexes
	DWORD*		    indecs;

	int		 	    nSeqCount;		// global sequences
	int*			pnSeq;

	DWORD*		    pdwBoneIndex;	// bone indexes for each vertexes, the array size is "vcount"
	DWORD*		    pdwBoneWeight;	// weight indexes for each vertexes, the array size is "vcount"

	SYArray		    bonearray;					//骨骼信息

	int			    nBoneGroupCount;	// bone group definition
	DWORD*		    pdwBoneGroup;		// 4 bytes of one cell, each byte indicates a bone index

	int			    nBoneIndexCount;	// bone indexes definition
	WORD*		    pwBoneIndex;

	int				nviewsub;
	LPSYSUBMODEL	viewsub;


	//多实例渲染
	int				nInstanceCount;
	SYModelInstance* pInstance;

	//逻辑顶点
	int				logicvcount;
	sylogic_vertex*	logicvertex;

	//逻辑线
	int				logiclcount;
	sylogic_line*	logicline;

	//逻辑面
	int				logicfcount;
	sylogic_face*	logicface;

	//逻辑体
	int				logicccount;
	sylogic_cube*	logiccube;
	tag_symodel() { Reset(); }
	BASEGRAPH_DATARENDER_API void Reset();
} SYModel, *LPSYMODEL;

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
	SYModel*	pmod;//模型顶点
	tag_sy3dunit() { Reset(); }
	BASEGRAPH_DATARENDER_API void Reset();
}SY3DUnit;
//物件中的一个单元
typedef struct tag_syunit
{
	eunittype type;
	DWORD	dwUserID;//用户自定义标识
	struct
	{
		SY2DUnit 	u2d;
		SY3DUnit 	u3d;
	}u;
	struct tag_syunit* parent;//父节点
	int				parentbone;//绑定在父的那个节点上
	vector3f		pos;	//位置
	matrix4f		MatVToItem;//unit space to item space
							   //针对自身的原点的动作
	TCHAR			szname[32];//名称
	GUID			UnitID;
	//Purpose:增加轮廓点动画
	SYArray			arrayPointCurve;//轮廓点关键帧(具体存储的是LPSYFigurePoint)
	tag_syunit() { Reset(); }
	BASEGRAPH_DATARENDER_API void Reset();
}SYUnit;
struct SYRenderStaticEffect
{
	matrix4f DynamicMat;
	BOOL	 bRepeat;
	float	 fAlpha;
};
struct SYRender2DUnit
{
	//面
	vectorgraph* graph;
	DWORD*		 buffer;
	int			 width;
	int			 height;
	RECT		 bound;
	SYHash		 channel;
	SYRenderStaticEffect effect;
};
typedef struct tag_syrender3dunit
{
	LPSYVERTEX3D	pVertex;
	int				nVertexCount;
	DWORD*		    pIndex;
	int				nIndexCount;
	int				hIndex;
	int				hVertex;
	int				hVertexShader;
	int				hVSParam;
	int				nSubModelCount;
	SYRenderSubModel* pSubModel;
	matrix4f*		pboneVMatrix;//for ani
								 //多实例渲染
	int				nInstanceCount;
	SYModelInstance* pInstance;
	int				hInstance;

	vector3f		vUnitMinVertex;//for render
	vector3f		vUnitMaxVertex;
	RECT			unitBound;
	matrix4f		Mat_RenderV;//从unit到最顶层的矩阵
} SYRender3DUnit, *LPSYRENDER3DUNIT;
struct SYItemRenderUnit
{
	SYUnit*	m_pUnit;
	BOOL	m_bChanged;
	BOOL	m_bNotRender;
	int		m_dwMatrixType;
	void*	m_GpuMem;
	matrix4f m_VSelfMatrix;
	rectf	m_frtBound;
	struct
	{
		SYRender2DUnit	unit2D;
		vectorgraph*	graph;
		SYRender3DUnit	unit3D;
	};
	TCHAR szID[RENDERUNITIDLENGTH + 1];
};