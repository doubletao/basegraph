#ifndef __BaseGraphMATERIAL_MODULE_API_H
#define __BaseGraphMATERIAL_MODULE_API_H
#include "DataRender\DrawTextDef.h"
#include "DataRender\DataRenderDataDef.h"
#include "ExportSceneRender.h"
//////////////////////////////////////////////////////////////////////////
#define	BaseGraphMATERIAL_DIFFUSE_COLOR		1
#define	BaseGraphMATERIAL_SPECTURE_COLOR		2
#define	BaseGraphMATERIAL_AMBEINT_COLOR		4
#define	BaseGraphMATERIAL_EMISSIVE_COLOR		8
#define	BaseGraphMATERIAL_FACTOR				0x10
#define	BaseGraphMATERIAL_ALPHA				0x20
#define	BaseGraphMATERIAL_DISABLE				0x40
#define	BaseGraphMATERIAL_PHY_ALL				0x7f
#define  BaseGraphMATERIAL_TEXPARAM_TEXOFFSETX	1
#define  BaseGraphMATERIAL_TEXPARAM_TEXOFFSETY	2
#define  BaseGraphMATERIAL_TEXPARAM_TEXSCALEX	4
#define  BaseGraphMATERIAL_TEXPARAM_TEXSCALEY	8
#define  BaseGraphMATERIAL_TEXPARAM_CENTERX	0x10
#define  BaseGraphMATERIAL_TEXPARAM_CENTERY	0x20
#define  BaseGraphMATERIAL_TEXPARAM_TEXROTATE	0x40
#define  BaseGraphMATERIAL_TEXPARAM_AUTORUN	0x80
#define  BaseGraphMATERIAL_TEXPARAM_SPEED		0x100
#define  BaseGraphMATERIAL_TEXPARAM_RUNDIRX	0x200
#define  BaseGraphMATERIAL_TEXPARAM_RUNDIRY	0x400
#define  BaseGraphMATERIAL_TEXPARAM_REPEAT		0x800
#define  BaseGraphMATERIAL_TEXPARAM_DISABLE	0x1000
#define  BaseGraphMATERIAL_TEX_COUNT			5
enum BaseGraphMatCallBackType
{
	BaseGraphMaterial_GetBuffer,
	BaseGraphMaterial_GetBufferSize,
};
typedef BOOL (*BaseGraphMatBufferCallBack)(BaseGraphMatCallBackType nType,OUT void* pOut,int hDestTex,void* pCallBackParam,void* pEnvParam);
enum BaseGraphMaterialTexType
{
	BaseGraphMaterial_DiffuseTex,		//材质
	BaseGraphMaterial_SpecTex,		//高光
	BaseGraphMaterial_EnvMapTex,		//环境
	BaseGraphMaterial_EmissiveTex,	//光影
	BaseGraphMaterial_BumpTex,		//凹凸
	BaseGraphMaterial_TexTypeCount,
};
enum BaseGraphMaterialShaderType
{
	BaseGraphMaterial_DiffuseShader,		//材质
	BaseGraphMaterial_SpecShader,			//高光
	BaseGraphMaterial_EnvMapShader,		//环境
	BaseGraphMaterial_EmissiveShader,		//光影
	BaseGraphMaterial_BumpShader,			//凹凸
	BaseGraphMaterial_LightShader,
	BaseGraphMaterial_ShaderTypeCount,
};
enum BaseGraphTexParamDefType
{
	BaseGraphTex_Offset_ParamDef,
	BaseGraphTex_Scale_ParamDef,
	BaseGraphTex_Center_ParamDef,
	BaseGraphTex_Rotate_ParamDef,
	BaseGraphTex_Private_ParamDef,
	BaseGraphTex_ParamDefCount,
};
enum BaseGraphMaterialChangeType
{
	BaseGraphMaterial_NoChange				= 0x00000000,
	BaseGraphMaterial_PhyDataChange		= 0x00000001,
	BaseGraphMaterial_TexCountChange		= 0x00000002,

	BaseGraphMaterial_DiffTexChange		= 0x00000004,
	BaseGraphMaterial_SpecTexChange		= 0x00000008,
	BaseGraphMaterial_EnvMapTexChange		= 0x00000010,	
	BaseGraphMaterial_EmissiveTexChange	= 0x00000020,
	BaseGraphMaterial_BumpTexChange		= 0x00000040,

	BaseGraphMaterial_DiffuseParamChange	= 0x00000080,
	BaseGraphMaterial_SpecParamChange		= 0x00000100,	
	BaseGraphMaterial_EnvMapParamChange	= 0x00000200,
	BaseGraphMaterial_EmissiveParamChange	= 0x00000400,
	BaseGraphMaterial_BumpParamChange		= 0x00000800,	
	BaseGraphMaterial_ShaderChange			= 0x00002000,	
};
enum BaseGraphMaterialTexSrcType
{
	BaseGraphMaterial_Invid = -1,
	BaseGraphMaterial_PicFile,
	BaseGraphMaterial_MovFile,
	BaseGraphMaterial_Object,
	BaseGraphMaterial_Channel,
	BaseGraphMaterial_Buffer,
};
enum BaseGraphMaterialShaderParamType
{
	BaseGraphMaterial_DiffuseShaderParam,	//材质
	BaseGraphMaterial_SpecShaderParam,		//高光
	BaseGraphMaterial_EnvMapShaderParam,	//环境
	BaseGraphMaterial_EmissiveShaderParam,	//光影
	BaseGraphMaterial_BumpShaderParam,		//凹凸
	BaseGraphMaterial_PhyParam,
	BaseGraphMaterial_LightParam,
	BaseGraphMaterial_CameraParam,
	BaseGraphMaterial_ShaderParamCount,

};
enum BaseGraphMaterialParamDefType
{
	BaseGraphMaterial_Phy_ParamDef,
	BaseGraphMaterial_Phy_Diffuse_ParamDef,
	BaseGraphMaterial_Phy_Ambient_ParamDef,
	BaseGraphMaterial_Phy_Specular_ParamDef,
	BaseGraphMaterial_Phy_Emissive_ParamDef,
	BaseGraphMaterial_Phy_Alpha_ParamDef,

	BaseGraphDiffuseTex_Offset_ParamDef,
	BaseGraphDiffuseTex_Scale_ParamDef,
	BaseGraphDiffuseTex_Center_ParamDef,
	BaseGraphDiffuseTex_Rotate_ParamDef,
	BaseGraphDiffuseTex_Private_ParamDef,

	BaseGraphSpecTex_Offset_ParamDef,
	BaseGraphSpecTex_Scale_ParamDef,
	BaseGraphSpecTex_Center_ParamDef,
	BaseGraphSpecTex_Rotate_ParamDef,
	BaseGraphSpecTex_Private_ParamDef,

	BaseGraphEnvMapTex_Offset_ParamDef,
	BaseGraphEnvMapTex_Scale_ParamDef,
	BaseGraphEnvMapTex_Center_ParamDef,
	BaseGraphEnvMapTex_Rotate_ParamDef,
	BaseGraphEnvMapTex_Private_ParamDef,

	BaseGraphEmissiveTex_Offset_ParamDef,
	BaseGraphEmissiveTex_Scale_ParamDef,
	BaseGraphEmissiveTex_Center_ParamDef,
	BaseGraphEmissiveTex_Rotate_ParamDef,
	BaseGraphEmissiveTex_Private_ParamDef,

	BumpTex_Offset_ParamDef,
	BumpTex_Scale_ParamDef,
	BumpTex_Center_ParamDef,
	BumpTex_Rotate_ParamDef,
	BumpTex_Private_ParamDef,
};

#define 	BaseGraphMaterial_ALLChange		(0xfff|BaseGraphMaterial_TexCountChange | BaseGraphMaterial_ShaderChange)
struct BaseGraphPhyData
{
	int			disable;
	DWORD		Diffuse;        /* Diffuse color RGBA */
	DWORD		Ambient;        /* Ambient color RGB */
	DWORD		Specular;       /* Specular 'shininess' */
	DWORD		Emissive;       /* Emissive color RGB */
	float		Power;          /* Sharpness if specular highlight */
	float		fAlpha;
	BaseGraphPhyData(){Reset();}
	BASEGRAPH_DATARENDER_API void Reset();
};

enum BaseGraphMatTexRunDir
{
	BaseGraphMatRunLeft,
	BaseGraphMatRunRigth,
	BaseGraphMatRunTop,
	BaseGraphMatRunBottom,
};

struct BaseGraphMatTex
{
	int disable;
	int nType;
	int nSrcType;
	struct
	{
		TCHAR* pTexPath;
		TCHAR* pTexSrc;
		int		nChannel;
		CommonBufferDef32* pBufferDef;
	};
	float2		TexOffset;
	float2		TexScale;
	float2		Center;
	float		TexRotate;
	BOOL		bAutoRun;
	double 		dSpeed;
	float2		vRunDir;
	BOOL		bRepeat;
	void*		pData;
	BaseGraphMatTex(){Reset();}
	BASEGRAPH_DATARENDER_API void Reset();
};
struct BaseGraphMatTexArray
{
	union{
		struct
		{
			BaseGraphMatTex*	pDiffuseTex;
			BaseGraphMatTex*	pSpecTex;
			BaseGraphMatTex*	pEnvMapTex;
			BaseGraphMatTex*	pEmissiveTex;
			BaseGraphMatTex*	pBumpTex;
		};
		BaseGraphMatTex* ppTex[BaseGraphMaterial_TexTypeCount];
	};
};
struct BaseGraphJoinShader
{
	int		nType;
	char*	strHead;		//shader头，包括全局变量纹理等寄存器的定义
	char*	strBody;		//shader执行部分
	BaseGraphJoinShader(){Reset();}
	BASEGRAPH_DATARENDER_API void Reset();
};
struct BaseGraphShaderArray
{
	union{
		struct
		{
			BaseGraphJoinShader*	pDiffuseTexShader;
			BaseGraphJoinShader*	pSpecTexShader;
			BaseGraphJoinShader*	pEnvMapTexShader;
			BaseGraphJoinShader*	pEmissiveTexShader;
			BaseGraphJoinShader*	pBumpTexShader;
			BaseGraphJoinShader*	pLightShader;
		};
		BaseGraphJoinShader* ppShader[BaseGraphMaterial_ShaderTypeCount];
	};
};
struct BaseGraphMaterial
{
	DWORD			m_nVersion;
	int				nMaterialType;
	BaseGraphPhyData		m_PhyData;
	BaseGraphMatTexArray	m_TexArray;
	BaseGraphShaderArray	m_ShaderArray;
	BaseGraphMaterial(){Reset();}
	BASEGRAPH_DATARENDER_API void Reset();
};
typedef struct tag_BaseGraphMaterialParam
{
	//struct tag_common
	//{
	//	param_def	Diffuse;
	//	param_def	Ambient;
	//	param_def	Specular;
	//	param_def	Emissive;
	//	param_def	fAlpha;
	//}common;
	BaseARray texParam[BaseGraphMATERIAL_TEX_COUNT];//曲线数组
}BaseGraphMaterialParam,*LPCommonMaterialParam;
typedef struct tag_BaseGraphSubModelParam
{
	int nSubModel;
	BaseGraphMaterialParam param;
}BaseGraphSubModelParam,*LPCommonSubModelParam;
struct BaseGraphMovieResource
{
	//IVideoSource*	pVideoSource;
	int				nMovieWidth;
	int				nMovHeight;
	int				nTotalFrame;
	BOOL			bIsProgress;
	int				hTex[2];
	int				nCur;
};

typedef struct  tag_BaseGraphTypesetInterface
{
	tag_BaseGraphTypesetInterface()
	{
		memset(this, 0, sizeof(tag_BaseGraphTypesetInterface));
	}
	//物件及其它锁
	void (*Lock)(LPVOID pInterfaceParam);
	void (*UnLock)(LPVOID pInterfaceParam);
	void (*OnAddUnit)(BOOL bKeepBalance, LPVOID pInterfaceParam);
	void (*OnInsertUnit)(int nIndex, int nCount, BOOL bKeepBalance, LPVOID pInterfaceParam);
	void (*OnDeleteUnit)(int nIndex, int nCount, BOOL bKeepBalance, LPVOID pInterfaceParam);
	void (*OnDeleteAllUnit)(LPVOID pArInterfaceParam);
	void (*OnRefreshUnit)(int nIndex, int nCount, LPVOID pInterfaceParam);
	void (*OnRefreshAllUnit)(LPVOID pInterfaceParam, int nLevel);
	//2DUnit转3DUnit
	void (*Modify2DUnitTo3DUnit)(BaseGraphUnit* punit3d, BaseGraphUnit* punit2d,BaseGraphMaterial** p3dcolor,int nSubFace,LPCommonStretchParam pStretch);
	//从属性页取属性，参数为空的属性不取。
	BaseGraphTypesetInterface_GetPropFromPropPage GetPropFromPropPage;

	//Item类的接口是否已经初始化
	BOOL bInitialized;
}BaseGraphTypesetInterface, *LPCommonTypesetInterface;
struct BaseGraphMovieSeekData
{
	LPVOID			pVideoSeekData;
	int				nWidth;
	int				nHeight;
	int				nTotalFrame;
	CCommonBuffer		movie_buffer;
};
struct BaseGraphSpecTexPrivateData 
{
	float fFactor;
	BaseGraphSpecTexPrivateData(){Reset();}
	BASEGRAPH_DATARENDER_API void Reset();
};
typedef BaseGraphSpecTexPrivateData BaseGraphEnvMapTexPrivateData;
typedef BaseGraphSpecTexPrivateData BaseGraphEmissiveTexPrivateData;
struct BumpTexPrivateData
{
	float fFactor;
	BOOL  bReverse;
	BumpTexPrivateData(){Reset();}
	BASEGRAPH_DATARENDER_API void Reset();
};
struct BaseGraphMatVideoTex
{
	int				disable;
	int				nType;
	int				hTex;
	BOOL			bTexNeedRelease;
	BaseGraphMovieResource* pResource;
	BaseGraphMovieSeekData* pSeekData;
	int				hTexCB;
	float2			CurTexOffset;
	float2			CurTexScale;
	float2			CurCenter;
	float			CurTexRotate;
	matrix4f		TexMat;
	void*			pCurData;
};
struct BaseGraphMatVideoTexArray
{
	union
	{
		struct
		{
			BaseGraphMatVideoTex*	pDiffuseVTex;
			BaseGraphMatVideoTex*	pSpecVTex;
			BaseGraphMatVideoTex*	pEnvMapVTex;
			BaseGraphMatVideoTex*	pEmissiveVTex;
			BaseGraphMatVideoTex*	pBumpVTex;
		};
		BaseGraphMatVideoTex* ppVTex[BaseGraphMaterial_TexTypeCount];
	};
};
struct BaseGraphPhyRenderData
{
	float4		Diffuse;        /* Diffuse color RGBA */
	float4		Ambient;        /* Ambient color RGB */
	float4		Specular;       /* Specular 'shininess' */
	float4		Emissive;       /* Emissive color RGB */
	float		Power;          /* Sharpness if specular highlight */
	float		fAlpha;
	int			disable;
};

struct BaseGraphVideoMaterial
{
	int					m_hDevice;
	float				m_fAlpha;//最终的alpha,包括物件的alpha
	BaseGraphPhyRenderData		m_PhyData;
	BaseGraphPhyRenderData		m_PhyDataStatic;
	BaseGraphMatVideoTexArray	m_VideoTexAr;
	int					m_hPhyCB;
	char*				m_pShaderStr;
	TCHAR*				m_pShaderName;
	int					m_nChangeType;
	double				m_dTime;
	double				m_dTotalTime;
	int					m_nLightNum;
	BOOL				m_bTexClamp;
	BOOL				m_bEdit;
	BaseGraphMaterial*			m_pMaterial;
};


BASEGRAPH_DATARENDER_API void		BaseGraphMaterialModuleInit(LPCTSTR pShaderPath,BaseGraphMatBufferCallBack pObject = NULL,void* pObjParam = NULL,BaseGraphMatBufferCallBack pChannel = NULL,void* pChannelParam = NULL);
BASEGRAPH_DATARENDER_API void		BaseGraphMaterialModuleRelease();
//内存材质处理函数
BASEGRAPH_DATARENDER_API void		BaseGraphMaterial_Init( BaseGraphMaterial* pMaterial,DWORD InitColor = 0xff777777 ,BOOL bLight = TRUE);
BASEGRAPH_DATARENDER_API void		BaseGraphMaterial_Release(BaseGraphMaterial* pMaterial);
BASEGRAPH_DATARENDER_API int		BaseGraphMaterial_Copy( OUT BaseGraphMaterial*  pDesMaterial, IN const BaseGraphMaterial* pSrcMaterial );
BASEGRAPH_DATARENDER_API BOOL		BaseGraphMaterial_Compare( const BaseGraphMaterial* pMaterial1, const BaseGraphMaterial* pMaterial2 );
BASEGRAPH_DATARENDER_API BOOL		BaseGraphMatTex_Compare( const BaseGraphMatTex* pMatTe1, const BaseGraphMatTex* pMatTe2);
BASEGRAPH_DATARENDER_API BOOL		BaseGraphJoinShader_Compare( const BaseGraphJoinShader* pShader1, const BaseGraphJoinShader* pShader2);

BASEGRAPH_DATARENDER_API void		BaseGraphMaterial_CreateParam(BaseGraphMaterialParam* pMatParam ,BaseGraphMaterial*  pMaterial );
BASEGRAPH_DATARENDER_API void		BaseGraphMaterial_CopyParam(BaseGraphMaterialParam* pDestMatParam,const BaseGraphMaterialParam* pSrcMatParam   );
BASEGRAPH_DATARENDER_API int		BaseGraphMaterial_AddKeyValue(double dFrame,BaseGraphMaterialParam* pMatParam, BaseGraphMaterial* pMaterial,void* pValue,int nParamType );
BASEGRAPH_DATARENDER_API void		BaseGraphMaterial_ReleaseParam(BaseGraphMaterialParam* pMatParam ,BaseGraphMaterial*  pMaterial );

BASEGRAPH_DATARENDER_API int		BaseGraphMaterial_AddTex(BaseGraphMaterial* pMaterial,int nTexType);
BASEGRAPH_DATARENDER_API int		BaseGraphMaterial_RemoveTex(BaseGraphMaterial* pMaterial,int nTexType);
BASEGRAPH_DATARENDER_API void		BaseGraphMaterial_RemoveShader(BaseGraphMaterial* pMaterial,int nShaderType);
BASEGRAPH_DATARENDER_API void		BaseGraphMaterial_AddShader(BaseGraphJoinShader* pShader ,int nShaderType);

BASEGRAPH_DATARENDER_API void*		BaseGraphMaterial_GetTexSrc(BaseGraphMaterial* pMaterial,int nTexType);
BASEGRAPH_DATARENDER_API int		BaseGraphMaterial_GetTexSrcType(BaseGraphMaterial* pMaterial,int nIndex);
BASEGRAPH_DATARENDER_API int		BaseGraphMaterial_SetTexSrc(BaseGraphMaterial* pMat,int nTexIndex,int nSrcType,void* pData);

BASEGRAPH_DATARENDER_API int		BaseGraphMaterial_SetTexRepeat(BaseGraphMaterial* pMat,int nTexIndex,BOOL bRepeat);
BASEGRAPH_DATARENDER_API int		BaseGraphMaterial_SetTexOffset(BaseGraphMaterial* pMat,int nTexIndex,float2* pOffset);
BASEGRAPH_DATARENDER_API int		BaseGraphMaterial_SetTexScale(BaseGraphMaterial* pMat,int nTexIndex,float2* pScale);
BASEGRAPH_DATARENDER_API int		BaseGraphMaterial_SetTexCenter(BaseGraphMaterial* pMat,int nTexIndex,float2* pCenter);
BASEGRAPH_DATARENDER_API int		BaseGraphMaterial_SetTexRotate(BaseGraphMaterial* pMat,int nTexIndex,float fRotate);
BASEGRAPH_DATARENDER_API int		BaseGraphMaterial_SetTexPrivateData(BaseGraphMaterial* pMat,int nTexIndex,void* pData);
BASEGRAPH_DATARENDER_API int		BaseGraphMaterial_SetPhyColor(BaseGraphMaterial* pMat,DWORD color);
BASEGRAPH_DATARENDER_API int		BaseGraphMaterial_SetPhyParam(BaseGraphMaterial* pMat,const BaseGraphPhyData* pData);
BASEGRAPH_DATARENDER_API int		BaseGraphMaterial_SetPhyParam(BaseGraphMaterial* pMat,const BaseGraphPhyRenderData* pData);
BASEGRAPH_DATARENDER_API int		BaseGraphUpdateDynamicTex(double dTotalFrame,int hDevice,BaseGraphMatVideoTex* pVideoTex,BaseGraphMatTex* pTex);//负责准备计算数据

//材质显存数据处理
BASEGRAPH_DATARENDER_API void		BaseGraphMaterial_CreateVideoMat( BaseGraphVideoMaterial* pVideoMat,BaseGraphMaterial* pMaterial ,int hDevice,BOOL bEdit /*= FALSE*/);
BASEGRAPH_DATARENDER_API void		BaseGraphMaterial_ReleaseVideoMat(BaseGraphVideoMaterial* pVideoMat);
BASEGRAPH_DATARENDER_API int		BaseGraphMaterial_VideoMatUpdateCopyVideo(BaseGraphVideoMaterial* pVideoMatDes, double dFrame,float fFrameRate,float fAlpha,int nLightNum,BaseGraphMaterialParam* param,void* pEnvParam,BaseGraphVideoMaterial* pVideoMatSrc);
BASEGRAPH_DATARENDER_API int		BaseGraphMaterial_UpdateFrame( BaseGraphVideoMaterial* pVideoMat,double dFrame,float fFrameRate,float fAlpha,int nLightNum,BaseGraphMaterialParam* param,void* pEnvParam, LPRECT pBound = NULL);
BASEGRAPH_DATARENDER_API BOOL		BaseGraphMaterial_UpdateTex(CommonBufferDef32* pSrcBufferDef,DXHalDef32* pSrcTexDef,int hSrcTex, int nTexIndex,double dTime, float fFrameRate,BaseGraphVideoMaterial* pVideoMat,void* param,BOOL isForceUpdate = FALSE, LPRECT pBound = NULL);
BASEGRAPH_DATARENDER_API BOOL		BaseGraphMaterial_UpdateTexCopyVideo(CommonBufferDef32* pSrcBufferDef,DXHalDef32* pSrcTexDef,int hSrcTex, int nTexIndex,double dTime, float fFrameRate,BaseGraphVideoMaterial* pVideoMat,BaseGraphVideoMaterial* pVideoMatSrc,void* param,BOOL isForceUpdate = FALSE);
BASEGRAPH_DATARENDER_API BOOL		BaseGraphMaterial_SetTex(int hTex,const RECT& pValidRt,int nOriginX, int nOriginY, BaseGraphVideoMaterial* pVideoMat, int nTexIndex,BOOL bNeedRelease = TRUE);
BASEGRAPH_DATARENDER_API BOOL		BaseGraphMaterial_SetTex(int hTex,const rectf& pValidRt,int nOriginX, int nOriginY, BaseGraphVideoMaterial* pVideoMat, int nTexIndex,BOOL bNeedRelease = TRUE);

BASEGRAPH_DATARENDER_API void		BaseGraphMaterial_SetParamToDevice( BaseGraphVideoMaterial* pVideoMat, BOOL bAdd);
BASEGRAPH_DATARENDER_API void		BaseGraphMaterial_SetChangeType(BaseGraphVideoMaterial* pVideoMat,int nChangeType);
BASEGRAPH_DATARENDER_API size_t		BaseGraphMaterial_GetVideoMemSize(BaseGraphVideoMaterial* pVideoMat);
BASEGRAPH_DATARENDER_API void		BaseGraphMaterial_SetStaticPhyParam(BaseGraphVideoMaterial* pMat,BaseGraphPhyRenderData* pData,int nType);
BASEGRAPH_DATARENDER_API int		BaseGraphMaterial_SetTexParam(BaseGraphMaterial* pMat,int nTexIndex,BaseGraphMatTex*	pTexParam,int nType);
BASEGRAPH_DATARENDER_API void		BaseGraphMaterial_PhyRenderDataToPhy(BaseGraphPhyData* pData,BaseGraphPhyRenderData* pSrcData); 
BASEGRAPH_DATARENDER_API void		BaseGraphMaterial_PhyToPhyRenderData(BaseGraphPhyRenderData* pData,BaseGraphPhyData* pSrcData); 
#endif
