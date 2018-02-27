#pragma once
//宏定义
enum SYDeviceType
{
	SY_Soft_Device,
	SY_DX10_Device,
	SY_Device_Count,	
};
enum SYDeviceCaps
{
	SYDevice_AAFactor,
};
enum SYPrimitiveTypes
{
	LINE_LIST,
	TRIANGLELIST,
	TRIANGLESTRIP,
};
enum SYConvertType
{
	SY_RGBA_To_BGRA,
	SYAdjustAlpha,
	SY_UYVU_RGBA_Key,
	SY_UYVYFrame_RGBA,   
	SY_RGBA_YUAYVA4224,
	SY_RGBA_UYVY,
	SY_RGBA_TO_Alpha,
	SY_RGB_Alpha_To_Key,
	SY_RGBA_To_FRAME,
};

enum SYVertexType
{
	SYVertex_POS_TEX,
	SYVertex_POS_COLOR_TEX,
	SYVertex_POS_NORMAL_TEX,
	SYVertex_POS_NORMAL_TEX_TANG_BI,
	SYVertex_POS,
	SYVertex_Vertex_Instance_Mat_TexOffset,
	SYVertex_Effect_Explode,
	SYVertex_TEX,
	SYVertex_Effect_Tile,
};
enum SYShaderType
{
	SYShaderType_VertexShader,
	SYShaderType_GeometryShader,
	SYShaderType_PixelShader,	
};
enum SYBufferType
{
	SYVertexBuffer,
	SYIndexBuffer,
	SYConstBuffer,
};
enum SYClearRTType
{
	SY_CLEAR_RENDERTARGET_ONLY,
	SY_CLEAR_DEPTH_ONLY,
	SY_CLEAR_ALL,
};

enum SYTextureType
{
	SYTexture_MemTexture,	//gpu和cpu都能访问的buffer
	SYTexture_Normal2DTex,	//普通的二维纹理
	SYTexture_MS2DTex,		//超采样二维纹理;
	SYTexture_Depth,		//深度buffer;
	SYTexture_MSDepth,		//超采样深度buffer;
	SYTexture_2DReadOnly,
	SYTexture_2DDumpReadOnly,
	SYTexture_MipMap,
	SYTexture_ShareBuffer,
	SYTexture_R16G16Tex,
	SYTexture_R16G16MemTexture,//gpu和cpu都能访问的buffer
};

enum	SYRenderState
{
	SYDepthState,
	SYBlendState,
	SYRasterizerState,
	SYRenderState_Count,
};

enum SYDepthStateType
{
	SYDepthState_Default = 0,
	SYDepthEnable_LessEqualPass,
	SYDepthEnable_LessEqualPass_DepthNoWrite,
	SYDepthLessEqual_Mask_01,
	SYDepthLessEqual_Mask_FF = SYDepthLessEqual_Mask_01 + 0xfe,

	SYDepthLessEqual_NoWrite_Mask_01,
	SYDepthLessEqual_NoWrite_Mask_FF = SYDepthLessEqual_NoWrite_Mask_01 + 0xfe,

	SYDepthDisable_WriteMask_01,
	SYDepthDisable_WriteMask_FF = SYDepthDisable_WriteMask_01 + 0xfe,

	SYDepthDisable,
	SYDepthStateCount,
};

enum SYBlendStateType
{
	SYBlendState_Default = 0,
	SYBlendDisable,
	SYBlend_Mix,
	SYBlend_Trans_Mix,
	SYBlend_Add,
	SYBlend_Enhance,

	SYBlend_Effect_Fire1,
	SYBlend_Effect_Fire2,
	SYBlend_Test,

	SYBlendStateCount,
};

enum SYRasterizerStateType
{
	SYRasterizer_Default = 0,
	SYRas_Line_MultiSample,
	SYRas_Line_NoMultiSample,
	SYRas_Solid_CullBack_MultiSample,
	SYRas_Solid_CullBack_NoMultiSample,
	SYRas_Solid_CullFront_MultiSample,
	SYRas_Solid_CullFront_NoMultiSample,
	SYRas_Solid_NoCullBack_MultiSample,
	SYRas_Solid_NoCullBack_NoMultiSample,
	SYRas_WireFrame_CullBack_MultiSample,
	SYRas_WireFrame_CullBack_NoMultiSample,
	SYRas_WireFrame_NoCullBack_MultiSample,
	SYRas_WireFrame_NoCullBack_NoMultiSample,
	SYRasStateCount,
};

enum SYSampleStateType
{
	SYSample_Default = 0,
	SYSample_Point_Clamp,
	SYSample_Point_Wrap,
	SYSample_Point_Mirror,
	SYSample_Point_Border,

	SYSample_Line_Clamp,
	SYSample_Line_Wrap,
	SYSample_Line_Mirror,
	SYSample_Line_Border,

	SYSample_Anisotropic_Clamp,
	SYSample_Anisotropic_Wrap,
	SYSample_Anisotropic_Mirror,
	SYSample_Anisotropic_Border,

	SYSample_NoMipMap_Anisotropic_Clamp,
	SYSample_NoMipMap_Anisotropic_Wrap,
	SYSample_NoMipMap_Anisotropic_Mirror,
	SYSample_NoMipMap_Anisotropic_Border,
	SYSampleStateCount,
};


enum SYEffectVariableType
{
	SYVar_BOOL = 0,
	SYVar_INT,
	SYVar_FLOAT,

	SYVar_BOOL_Array,
	SYVar_INT_Array,
	SYVar_FLOAT_Array,

	SYVar_BOOL_Vector,
	SYVar_INT_Vector,
	SYVar_FLOAT_Vector,

	SYVar_BOOL_Vector_Array,
	SYVar_INT_Vector_Array,
	SYVar_FLOAT_Vector_Array,

	SYVar_Matrix,
	SYVar_Matrix_Array,
};

struct SYHalDef32
{
	int		m_hTex;
	RECT	m_rtBound;
	int		m_nOriginX;
	int		m_nOriginY;
	BOOL 	m_bRelease;
	SYHalDef32()
	{
		memset(this, 0, sizeof(SYHalDef32));
	}
};
struct SYEffectTechDesc
{
	LPCTSTR pstrName;
	int vertexType;
};

struct SYDistortionParam
{
	float fRadial;//径向畸变系数
	float fCentrifugal1;//离心畸变系数
	float fCentrifugal2;//离心畸变系数
	float fPrism1;	//薄棱镜畸变系数
	float fPrism2;//薄棱镜畸变系数
	float fCenterX;//中心偏移[0-1]
	float fCenterY;//中心偏移[0-1]
};

struct SYRenderInfo
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
	SYTextureType nType;
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
struct SYDumpIn
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
struct SYCreateShadeIn
{
	LPCTSTR		shaderName;
	LPCSTR		shaderCode;
	LPCTSTR		shaderFile;
	SYShaderType	nType;
	SYVertexType*	pType;
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
	SYClearRTType	nType;
};
struct getRenderTargetIn
{
	int* phRT;
	int* phDepth;
};
enum SYTextureProcessType
{
	SYTextureConvertFormat,
	SYTextureBlur_5Pt,
	SYTextureBlur_9Pt,
	SY_FIRST_KEY_MIX,
	SY_UYVY_BRIGHT,
};
struct SYTexProcessIn
{
	int			hSrcTex;
	int			hSrcKey;
	int			hDestTex;
	const RECT*	pSrcRt;
	const RECT*	pDstRt;
	void*		pData;
	SYTextureProcessType nType;
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
struct SYEasyRenderIn
{
	int hVertexBuffer;
	int hIndexBuffer;
	int hVertexShader;
	int hGeometryShader;
	int hPixShader;
	int nStart;
	int nVertexCount;
};
struct SYRenderIn
{
	int hVertexBuffer;
	int hIndexBuffer;
	int	nVertexType;
	int nStart;
	int PrimitiveCount;
	int nVertexCount;
};
struct SYRenderInstanceIn
{
	SYRenderIn RenderData;
	int hInstanceBuffer;
	int nInstanceCount;
};
struct SYModifyBufferIn
{
	int hBuffer;
	void* pBuffer;
};
struct SYSetShaderIn
{
	SYShaderType nShaderType;
	int hShader;
};
struct SYRenderMeshIn
{
	int hMesh;
	int nDrawCount;//-1为全部
	const unsigned int* pnSubset;
};
struct SYGetMeshInfoOut
{
	int nSubSetCount;
};
struct SYHitTestMeshOut
{
	float	fDist;
	int		nHitPart;
};
struct SYRenderStateIn
{
	int nType;
	int nValue;
};
struct SYSampleStateIn
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
struct SYSetConstBufferIn
{
	int hConstBuffer;
	int hRegIndex;
	SYShaderType nShaderType;
};
typedef struct SYSetConstBufferIn SYSetShaderTexIn;

struct SYHitTestMeshIn
{
	int hMesh;
	const float3* pStart;
	const float3* pDir;
};
struct SYGetMeshBoxIn
{
	int hMesh;
	float3* pMin;
	float3* pMax;
};
struct SYTextOutIn
{
	LPCTSTR strText;
	RECT* rtText;
	DWORD dwColor;
};
struct SYRender2D
{
	SYHalDef32* pTex;
	const matrix4f* pWorldMatrix;
	const RECT* pDstRt;
	BOOL bAdustColor;
	float fAlpha;
};
struct SYGetMemTexBufferOut
{
	void* ptr;
	DWORD pitch;
};
struct SYCreateEffectUnitIn
{
	LPCTSTR			pEffectName;
	SYEffectTechDesc* pTechDesc;
	int				nTechs;
};
struct SYCreateBufferIn
{ 
	int nVertexSize;
	int nVertexNum;
	void* pBuffer;
	SYBufferType nType;
	int nPriType; 
};
struct SYEffectRenderIn
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
struct SYEffectRenderInstanceIn
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
struct SYEffectSetTextureIn
{
	int			hEffect;
	int			hTex;
	const char*	szTexName;
};
struct SYEffectSetConstBufferIn
{
	int			hEffect;
	int			hConstBuffer;
	const char*	szCBName;
};
struct SYDynamicTextureIn
{
	SYHalDef32*	pTarget;
	SYHalDef32*	pMask;
	SYHalDef32*	pSrc;
	matrix4f*	pSrcMat;
	BOOL		bRepeat;
	BOOL		bUseColorAlpha;//动态贴图是否使用颜色属性的透明度
	SYDynamicTextureIn()
	{
		pTarget = NULL;
		pMask = NULL;
		pSrc = NULL;
		pSrcMat = NULL;
		bRepeat = FALSE;
		bUseColorAlpha = FALSE;
	}
};
struct SYRenderTex
{
	int				hSrcTex;
	int				hMaskTex;
	const matrix4f*	pMatrix;
};
struct SY3DVisionTex
{
	int			hDstTex;
	const RECT*	pDstRt;
	int			hLeftTex;
	int			hRightTex;
	const RECT*	pSrcRt;
};
struct SYMaskRenderIn
{
	int		hTarget;
	int		hSrc;
	int		hMask;
	rectf*	pCutRt;
	RECT*	pDstRt;
};
struct SYEffectSetVariableIn
{
	int			hEffect;
	const void*	pVariable;
	int			nCount;
	SYEffectVariableType nType;
	const char*	szVarName;
};
typedef struct shaderhash
{
	int hHandle;
	int nCount;
}HandleHash;