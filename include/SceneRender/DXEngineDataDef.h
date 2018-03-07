#pragma once

//宏定义
enum DXDeviceType
{
	DX_Soft_Device,
	DX_DX10_Device,
	DX_Device_Count,	
};
enum DXDeviceCaps
{
	DXDevice_AAFactor,
};
enum DXPrimitiveTypes
{
	LINE_LIST,
	TRIANGLELIST,
	TRIANGLESTRIP,
};
enum ColorConvertType
{
	Color_RGBA_To_BGRA,
	ColorAdjustAlpha,
	Color_UYVU_RGBA_Key,
	Color_UYVYFrame_RGBA,   
	Color_RGBA_YUAYVA4224,
	Color_RGBA_UYVY,
	Color_RGBA_TO_Alpha,
	Color_RGB_Alpha_To_Key,
	Color_RGBA_To_FRAME,
};

enum DXVertexType
{
	DXVertex_POS_TEX,
	DXVertex_POS_COLOR_TEX,
	DXVertex_POS_NORMAL_TEX,
	DXVertex_POS_NORMAL_TEX_TANG_BI,
	DXVertex_POS,
	DXVertex_Vertex_Instance_Mat_TexOffset,
	DXVertex_Effect_Explode,
	DXVertex_TEX,
	DXVertex_Effect_Tile,
};
enum DXShaderType
{
	DXShaderType_VertexShader,
	DXShaderType_GeometryShader,
	DXShaderType_PixelShader,	
};
enum DXBufferType
{
	DXVertexBuffer,
	DXIndexBuffer,
	DXConstBuffer,
};
enum DXClearRTType
{
	DX_CLEAR_RENDERTARGET_ONLY,
	DX_CLEAR_DEPTH_ONLY,
	DX_CLEAR_ALL,
};

enum DXTextureType
{
	DXTexture_MemTexture,	//gpu和cpu都能访问的buffer
	DXTexture_Normal2DTex,	//普通的二维纹理
	DXTexture_MS2DTex,		//超采样二维纹理;
	DXTexture_Depth,		//深度buffer;
	DXTexture_MSDepth,		//超采样深度buffer;
	DXTexture_2DReadOnly,
	DXTexture_2DDumpReadOnly,
	DXTexture_MipMap,
	DXTexture_ShareBuffer,
	DXTexture_R16G16Tex,
	DXTexture_R16G16MemTexture,//gpu和cpu都能访问的buffer
};

enum DXRenderState
{
	DXDepthState,
	DXBlendState,
	DXRasterizerState,
	DXRenderState_Count,
};

struct DX_Ray_Parameter
{
	vector3f	startPoint;
	vector3f	Direction;//必须是归一化过的
};

struct DX_QuadRang_Intersection
{
	vector3f		firstPoint;
	vector3f		secondPoint;
	vector3f		thirdPoint;
	vector3f		forthPoint;
	vector3f		fifthPoint;
	vector3f		sixthPoint;
	vector3f		seventhPoint;
	vector3f		eighthPoint;
	DX_Ray_Parameter Ray;
};

struct Point_Intersection
{
	vector3f		nearPoint;
	vector3f		farPoint;
};

struct DX_Triangle_Param
{
	vector3f		v0;
	vector3f		v1;
	vector3f		v2;
};

struct DX_Triangle_Intersection
{
	DX_Triangle_Param	triangle;
	DX_Ray_Parameter	ray;
};

struct DX_Plane_Parameter
{
	vector3f point;
	vector3f normal;
};
struct DX_Plane_Intersection
{
	DX_Plane_Parameter	Plane;
	DX_Ray_Parameter	Ray;
};
struct DX_3DCircle_Parameter
{
	vector3f Normal;
	vector3f Centre;
	float Radius;
};
struct DX_3DCircle_Intersection
{
	DX_3DCircle_Parameter Circle;
	DX_Ray_Parameter Ray;
};
struct DX_LineSeg_Parameter
{
	vector3f start;
	vector3f end;
};
struct DX_LineSeg_Intersection
{
	DX_LineSeg_Parameter line;
	DX_Ray_Parameter Ray;
};
struct DX_Sphere_Intersection
{
	float		fSphereRadius;
	DX_Ray_Parameter		Ray;
};
//四面体结构，四个顶点
struct DX_Tetrahedron_Intersection
{
	vector3f			firstPoint;
	vector3f			secondPoint;
	vector3f			thirdPoint;
	vector3f			forthPoint;
	DX_Ray_Parameter	Ray;
};
struct DX_Segment_Intersection
{
	vector3f			firstPoint;
	vector3f			secondPoint;
	vector3f			thirdPoint;
	vector3f			forthPoint;
	DX_Ray_Parameter	Ray;
};

struct DX_3DPoint_Intersection
{
	vector3f			vPoint;
	DX_Ray_Parameter	ray;
};

struct DX_Cone_Parameter//圆锥特征参数
{
	float fBaseRadius;
	float fHeight;
	float fTopRadius;
	float fStartAngle;//start angle == 0
	float fEndAngle;//when begin end angle = 2pi
};

enum DXTriType
{
	DX_TRIANGLE_LIST = 1,
	DX_TRIANGLE_STRIP,
	DX_TRIANGLE_FAN,
	DX_TRI_TYPE_NO_USE = 0xffffffff,
};

enum DX_POST_PROCESS_TYPE
{
	DX_POST_DEFAULT_PROCESS,
	DX_POST_NORMAL_PROCESS,
	DX_POST_POSITION_PROCESS,
	DX_POST_DEPTH_PROCESS,
	DX_POST_TRANSPARENT_PROCESS,
	DX_POST_PROCESS_COUNT,
};

typedef struct tag_dx_render_submodel
{
	DWORD	vstart;
	DWORD	vcount;
	DWORD	istart;
	DWORD	icount;
	int		nTri_Type;
	int		nTri_Num;
	void*	pVideoMaterial;
	BOOL		bShaderChange;
	int	    hPixShader[DX_POST_PROCESS_COUNT];
	bool		bSharedMaterial; //标志是否是共享的材质
}DXRenderSubModel;

typedef struct tag_dxvertex3d		// 3d vertex declaration
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
} dxvertex3d, DXVertex3D, *LPDXVERTEX3D;

enum DXDepthStateType
{
	DXDepthState_Default = 0,
	DXDepthEnable_LessEqualPass,
	DXDepthEnable_LessEqualPass_DepthNoWrite,
	DXDepthLessEqual_Mask_01,
	DXDepthLessEqual_Mask_FF = DXDepthLessEqual_Mask_01 + 0xfe,

	DXDepthLessEqual_NoWrite_Mask_01,
	DXDepthLessEqual_NoWrite_Mask_FF = DXDepthLessEqual_NoWrite_Mask_01 + 0xfe,

	DXDepthDisable_WriteMask_01,
	DXDepthDisable_WriteMask_FF = DXDepthDisable_WriteMask_01 + 0xfe,

	DXDepthDisable,
	DXDepthStateCount,
};

enum DXBlendStateType
{
	DXBlendState_Default = 0,
	DXBlendDisable,
	DXBlend_Mix,
	DXBlend_Trans_Mix,
	DXBlend_Add,
	DXBlend_Enhance,

	DXBlend_Effect_Fire1,
	DXBlend_Effect_Fire2,
	DXBlend_Test,

	DXBlendStateCount,
};

enum DXRasterizerStateType
{
	DXRasterizer_Default = 0,
	DXRas_Line_MultiSample,
	DXRas_Line_NoMultiSample,
	DXRas_Solid_CullBack_MultiSample,
	DXRas_Solid_CullBack_NoMultiSample,
	DXRas_Solid_CullFront_MultiSample,
	DXRas_Solid_CullFront_NoMultiSample,
	DXRas_Solid_NoCullBack_MultiSample,
	DXRas_Solid_NoCullBack_NoMultiSample,
	DXRas_WireFrame_CullBack_MultiSample,
	DXRas_WireFrame_CullBack_NoMultiSample,
	DXRas_WireFrame_NoCullBack_MultiSample,
	DXRas_WireFrame_NoCullBack_NoMultiSample,
	DXRasStateCount,
};

enum DXSampleStateType
{
	DXSample_Default = 0,
	DXSample_Point_Clamp,
	DXSample_Point_Wrap,
	DXSample_Point_Mirror,
	DXSample_Point_Border,

	DXSample_Line_Clamp,
	DXSample_Line_Wrap,
	DXSample_Line_Mirror,
	DXSample_Line_Border,

	DXSample_Anisotropic_Clamp,
	DXSample_Anisotropic_Wrap,
	DXSample_Anisotropic_Mirror,
	DXSample_Anisotropic_Border,

	DXSample_NoMipMap_Anisotropic_Clamp,
	DXSample_NoMipMap_Anisotropic_Wrap,
	DXSample_NoMipMap_Anisotropic_Mirror,
	DXSample_NoMipMap_Anisotropic_Border,
	DXSampleStateCount,
};


enum DXEffectVariableType
{
	DXVar_BOOL = 0,
	DXVar_INT,
	DXVar_FLOAT,

	DXVar_BOOL_Array,
	DX_INT_Array,
	DXVar_FLOAT_Array,

	DXVar_BOOL_Vector,
	DXVar_INT_Vector,
	DXVar_FLOAT_Vector,

	DXVar_BOOL_Vector_Array,
	DXVar_INT_Vector_Array,
	DXVar_FLOAT_Vector_Array,

	DXVar_Matrix,
	DXVar_Matrix_Array,
};

struct DXHalDef32
{
	int		m_hTex;
	RECT	m_rtBound;
	int		m_nOriginX;
	int		m_nOriginY;
	BOOL 	m_bRelease;
	DXHalDef32()
	{
		memset(this, 0, sizeof(DXHalDef32));
	}
};

struct DXEffectTechDesc
{
	LPCTSTR pstrName;
	int vertexType;
};

struct DXDistortionParam
{
	float fRadial;//径向畸变系数
	float fCentrifugal1;//离心畸变系数
	float fCentrifugal2;//离心畸变系数
	float fPrism1;	//薄棱镜畸变系数
	float fPrism2;//薄棱镜畸变系数
	float fCenterX;//中心偏移[0-1]
	float fCenterY;//中心偏移[0-1]
};

struct DXRenderInfo
{
	size_t				nRenderCount;
	size_t				nTriangleCount;
	size_t				nRenderPixCount;
	size_t				nUploadTotalTime;
	size_t				nUploadTotalSize;
	size_t				nUploadCount;
};

enum ENGINE_CMD_DEFINE
{
	//设备相关操作
	RENDERENGINE_INIT,
	RENDERENGINE_RELEASE,
	RENDERENGINE_CREATEDEVICE,
	RENDERENGINE_RELEASEDEVICE,
	RENDERENGINE_ENGINE_LOCK,
	RENDERENGINE_ENGINE_UNLOCK,
	RENDERENGINE_ENGINE_RES_LOCK,
	RENDERENGINE_ENGINE_RES_UNLOCK,
	RENDERENGINE_RENDER_LOCK,
	RENDERENGINE_RENDER_UNLOCK,
	RENDERENGINE_GET_REMOVE_REASON,
	RENDERENGINE_CREATE_CONTEXT,
	RENDERENGINE_RELEASE_CONTEXT,
	RENDERENGINE_SET_CONTEXT,


	//纹理相关操作
	RENDERENGINE_CREATE_TEXTURE,
	RENDERENGINE_CREATESWAPCHAIN,
	RENDERENGINE_RELEASE_SWAPCHAIN,
	RENDERENGINE_GET_SWAPCHAIN_BUFFER_TEX,
	//RENDERENGINE_CREATE_MIPMAP_TEXTURE,
	RENDERENGINE_CREATE_TEXTURE_FROM_FILE,
	RENDERENGINE_RELEASE_TEXTURE,
	RENDERENGINE_CLEAR_TEXTURE,
	RENDERENGINE_GET_TEXTURE,
	RENDERENGINE_GET_TEXTURE_VIEW,
	RENDERENGINE_GET_TEXTURE_INFO,
	RENDERENGINE_RESIZE_SWAPCHAIN,
	RENDERENGINE_COPY_TEXTURE,
	//RENDERENGINE_COPY_TEXTURE_FROM_BUFFER,
	RENDERENGINE_DUMP_TEXTURE,
	RENDERENGINE_DOWNLOAD,
	RENDERENGINE_UPLOAD,
	RENDERENGINE_NORMAL_MAP,
	RENDERENGINE_GENERATE_MIPMAP,
	RENDERENGINE_GET_MESYEX_BUFFER,
	RENDERENGINE_GET_SWAPCHAINDESC,
	RENDERENGINE_MULTI_NORMAL,
	RENDERENGINE_RENDER_TEXTURE_3D,

	//渲染相关函数
	RENDERENGINE_SETVIEWPORT,
	RENDERENGINE_GETVIEWPORT,
	RENDERENGINE_SET_SCISSOR,
	RENDERENGINE_GET_SCISSOR,
	RENDERENGINE_SET_RENDERTARGET,
	RENDERENGINE_GET_RENDERTARGET,
	RENDERENGINE_CLEAR_SCREEN,
	RENDERENGINE_BEGIN_RENDER_TEXTURE,
	RENDERENGINE_RENDER_TEXTURE,
	RENDERENGINE_RENDER_2D,
	RENDERENGINE_END_RENDER_TEXTURE,
	RENDERENGINE_BEGIN_RENDER,
	RENDERENGINE_END_RENDER,
	RENDERENGINE_RENDER,
	RENDERENGINE_EASY_RENDER,
	RENDERENGINE_RENDER_INSTANCE,
	RENDERENGINE_PRESENT,
	//	RENDERENGINE_RENDER_TEXTURE_3D,

	//渲染状态相关函数
	RENDERENGINE_CREATESHADER,
	RENDERENGINE_RELEASESHADER,
	RENDERENGINE_CREATE_BUFFER,
	RENDERENGINE_RELEASE_BUFFER,
	RENDERENGINE_MODIFY_BUFFER,
	RENDERENGINE_SET_CONST_BUFFER,
	RENDERENGINE_SET_SHADER,
	RENDERENGINE_SET_SHADER_TEXTURE,
	RENDERENGINE_SET_RENDER_STATE,
	RENDERENGINE_GET_RENDER_STATE,
	RENDERENGINE_SET_SAMPLER_STATE,
	RENDERENGINE_GET_RENDER_INFO,
	RENDERENGINE_CLEAR_RENDER_INFO,

	//高级函数
	RENDERENGINE_CREATE_MODEL_FROM_X_FILE,
	RENDERENGINE_RELEASE_MESH,
	RENDERENGINE_HITTEST_MESH,
	RENDERENGINE_GET_MESH_BOX,
	RENDERENGINE_RENDER_MESH,
	RENDERENGINE_GET_MESH_INFO,

	RENDERENGINE_TEXTURE_PROCESS,

	RENDERENGINE_CREATE_EFFECTUNIT,
	RENDERENGINE_RELEASE_EFFECTUNIT,
	RENDERENGINE_EFFECT_GET_QUADVB,
	RENDERENGINE_EFFECT_SET_TEXTURE,
	RENDERENGINE_EFFECT_SET_CONST_BUFFER,
	RENDERENGINE_EFFECT_SET_VARIABLE,
	RENDERENGINE_EFFECT_RENDER,
	RENDERENGINE_EFFECT_RENDER_INSTANCE,
	//RENDERENGINE_EFFECT_DOWNSAMPLE,
	RENDERENGINE_TEXTOUT,//调试时输出字符
	RENDERENGINE_RENDER_MASK,
	RENDERENGINE_RENDER_DYNAMIC_TEX,
	RENDERENGINE_RENDER_3DVISION,

};
struct MultiToNormal
{
	int hSrcTex;
	int hDesTex;
};
// field definition
enum FIELD
{	
	FIRST_FIELD		= 1,	// odd field顶场先
	SECOND_FIELD	= 2,	// even field底场先
	BOTH_FIELD		= 3,	// both fields帧模式
};
struct createTextureIn
{
	int nWidth;
	int nHeight;
	DXTextureType nType;
	DWORD* pBuffer;
};
struct createSwapChainIn
{
	HWND hWnd;
	int nWidth;
	int nHeight;
	BOOL bWindowed;
};
struct copyTexture
{
	int hSrcTex;
	int nSrcOffsetX;
	int nSrcOffsetY;
	int nSrcWidth;
	int nSrcHeight;
	int hDesTex;
	int nDesOffsetX;
	int nDesOffsetY;
	int nDesWidth;
	int nDesHeight;
	int eMode;
};
struct DXDumpIn
{
	int		hTexture;
	LPCTSTR szFile;
};
struct downloadBufferIn
{
	DWORD*	dstBuffer;
	int		dstOffsetX;
	int		dstOffsetY;
	int		dstWidth;
	int		dstHeight;
	int		dstBufferWidth;
	int		dstBufferHeight;
	int		hSrcTex;
	int		srcOffsetX;
	int		srcOffsetY;
	int		hTransTex;
	int		eMode;
};
struct setRenderTarget
{
	int hRenderTarget;
	int hDepth;
};
struct clearTextureIn
{
	int hTexture;
	void* pData;
};
struct getTextureInfoOut
{
	int* nWidth;
	int* nHeight;
	int* nType;
	int* nMipMap;
};
struct DXCreateShadeIn
{
	LPCTSTR		shaderName;
	LPCSTR		shaderCode;
	LPCTSTR		shaderFile;
	DXShaderType	nType;
	DXVertexType*	pType;
	BOOL			bLock;
	BOOL			bComplied;
	size_t		compliedBinSize;
};
struct ResizeSwapChainIn
{
	int hSwapChain;
	int nWidth;
	int nHeight;
};
struct clearScreenIn
{ 
	DWORD		nColor;
	float		fZValue;
	DXClearRTType	nType;
};
struct getRenderTargetIn
{
	int* phRT;
	int* phDepth;
};
enum DXTextureProcessType
{
	DXTextureConvertFormat,
	DXTextureBlur_5Pt,
	DXTextureBlur_9Pt,
	DX_FIRST_KEY_MIX,
	DX_UYVY_BRIGHT,
};
struct DXTexProcessIn
{
	int			hSrcTex;
	int			hSrcKey;
	int			hDestTex;
	const RECT*	pSrcRt;
	const RECT*	pDstRt;
	void*		pData;
	DXTextureProcessType nType;
	FIELD		nSrcField;
	FIELD		nDstField;
};
struct uploadBufferIn
{
	DWORD*	srcBuffer;
	int		srcOffsetX;
	int		srcOffsetY;
	int		srcWidth;
	int		srcHeight;
	int		srcBufferWidth;
	int		srcBufferHeight;

	int		hDestTex;
	int		destOffsetX;
	int		destOffsetY;
	int		hTransTex;
	BOOL	bLock;
};
struct renderTextureIn
{
	int			hTex;
	const RECT*	pSrcRt;
	const RECT*	pDstRt;
	BOOL		bAdjustColor;
	BOOL 		bStretch;
	float		fAlpha;
	FIELD		nField;
};
struct DXEasyRenderIn
{
	int hVertexBuffer;
	int hIndexBuffer;
	int hVertexShader;
	int hGeometryShader;
	int hPixShader;
	int nStart;
	int nVertexCount;
};
struct DXRenderIn
{
	int hVertexBuffer;
	int hIndexBuffer;
	int	nVertexType;
	int nStart;
	int PrimitiveCount;
	int nVertexCount;
};
struct DXRenderInstanceIn
{
	DXRenderIn RenderData;
	int hInstanceBuffer;
	int nInstanceCount;
};
struct DXModifyBufferIn
{
	int hBuffer;
	void* pBuffer;
};
struct DXSetShaderIn
{
	DXShaderType nShaderType;
	int hShader;
};
struct DXRenderMeshIn
{
	int hMesh;
	int nDrawCount;//-1为全部
	const unsigned int* pnSubset;
};
struct DXGetMeshInfoOut
{
	int nSubSetCount;
};
struct DXHitTestMeshOut
{
	float	fDist;
	int		nHitPart;
};
struct DXRenderStateIn
{
	int nType;
	int nValue;
};
struct DXSampleStateIn
{
	int nSampler;
	int nState;
	int nShaderType;
};
struct createTextureFromFileIn
{
	TCHAR* pPath;
	BOOL bNormalMap;
};
struct DXSetConstBufferIn
{
	int hConstBuffer;
	int hRegIndex;
	DXShaderType nShaderType;
};
typedef struct DXSetConstBufferIn DXSetShaderTexIn;

struct DXHitTestMeshIn
{
	int hMesh;
	const float3* pStart;
	const float3* pDir;
};
struct DXGetMeshBoxIn
{
	int hMesh;
	float3* pMin;
	float3* pMax;
};
struct DXTextOutIn
{
	LPCTSTR strText;
	RECT* rtText;
	DWORD dwColor;
};
struct DXRender2D
{
	DXHalDef32* pTex;
	const matrix4f* pWorldMatrix;
	const RECT* pDstRt;
	BOOL bAdustColor;
	float fAlpha;
};
struct DXGetMemTexBufferOut
{
	void* ptr;
	DWORD pitch;
};
struct DXCreateEffectUnitIn
{
	LPCTSTR			pEffectName;
	DXEffectTechDesc* pTechDesc;
	int				nTechs;
};
struct DXCreateBufferIn
{ 
	int nVertexSize;
	int nVertexNum;
	void* pBuffer;
	DXBufferType nType;
	int nPriType; 
};
struct DXEffectRenderIn
{
	int		hEffect;
	LPCTSTR	techName;
	int		hVertexBuffer;
	int		hIndexBuffer;
	int		nVertexType;
	int		nPrimitiveCount;
	int		nStart;
	int		nVertexCount;
	int		nIndexCount;
};
struct DXEffectRenderInstanceIn
{
	int		hEffect;
	LPCTSTR techName;
	int		hVertexBuffer;
	int		hIndexBuffer;
	int		nVertexType;
	int		nPrimitiveCount;
	int		nStart;
	int		nVertexCount;
	int		nIndexCount;
	int		hInstanceBuf;
	int		nInstanceCount;
};
struct DXEffectSetTextureIn
{
	int			hEffect;
	int			hTex;
	const char*	szTexName;
};
struct DXEffectSetConstBufferIn
{
	int			hEffect;
	int			hConstBuffer;
	const char*	szCBName;
};
struct DXDynamicTextureIn
{
	DXHalDef32*	pTarget;
	DXHalDef32*	pMask;
	DXHalDef32*	pSrc;
	matrix4f*	pSrcMat;
	BOOL		bRepeat;
	BOOL		bUseColorAlpha;//动态贴图是否使用颜色属性的透明度
	DXDynamicTextureIn()
	{
		pTarget = NULL;
		pMask = NULL;
		pSrc = NULL;
		pSrcMat = NULL;
		bRepeat = FALSE;
		bUseColorAlpha = FALSE;
	}
};
struct DXRenderTex
{
	int				hSrcTex;
	int				hMaskTex;
	const matrix4f*	pMatrix;
};
struct DX3DVisionTex
{
	int			hDstTex;
	const RECT*	pDstRt;
	int			hLeftTex;
	int			hRightTex;
	const RECT*	pSrcRt;
};
struct DXMaskRenderIn
{
	int		hTarget;
	int		hSrc;
	int		hMask;
	rectf*	pCutRt;
	RECT*	pDstRt;
};
struct DXEffectSetVariableIn
{
	int			hEffect;
	const void*	pVariable;
	int			nCount;
	DXEffectVariableType nType;
	const char*	szVarName;
};
typedef struct shaderhash
{
	int hHandle;
	int nCount;
}HandleHash;