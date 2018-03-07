#ifndef __SYMATERIAL_MODULE_API_H
#define __SYMATERIAL_MODULE_API_H
#include "DataRender\DrawTextDef.h"
#include "DataRender\DataRenderDataDef.h"
//////////////////////////////////////////////////////////////////////////
#define	SYMATERIAL_DIFFUSE_COLOR		1
#define	SYMATERIAL_SPECTURE_COLOR		2
#define	SYMATERIAL_AMBEINT_COLOR		4
#define	SYMATERIAL_EMISSIVE_COLOR		8
#define	SYMATERIAL_FACTOR				0x10
#define	SYMATERIAL_ALPHA				0x20
#define	SYMATERIAL_DISABLE				0x40
#define	SYMATERIAL_PHY_ALL				0x7f
#define  SYMATERIAL_TEXPARAM_TEXOFFSETX	1
#define  SYMATERIAL_TEXPARAM_TEXOFFSETY	2
#define  SYMATERIAL_TEXPARAM_TEXSCALEX	4
#define  SYMATERIAL_TEXPARAM_TEXSCALEY	8
#define  SYMATERIAL_TEXPARAM_CENTERX	0x10
#define  SYMATERIAL_TEXPARAM_CENTERY	0x20
#define  SYMATERIAL_TEXPARAM_TEXROTATE	0x40
#define  SYMATERIAL_TEXPARAM_AUTORUN	0x80
#define  SYMATERIAL_TEXPARAM_SPEED		0x100
#define  SYMATERIAL_TEXPARAM_RUNDIRX	0x200
#define  SYMATERIAL_TEXPARAM_RUNDIRY	0x400
#define  SYMATERIAL_TEXPARAM_REPEAT		0x800
#define  SYMATERIAL_TEXPARAM_DISABLE	0x1000
#define  SYMATERIAL_TEX_COUNT			5
enum SYMatCallBackType
{
	SYMaterial_GetBuffer,
	SYMaterial_GetBufferSize,
};
typedef BOOL (*SYMatBufferCallBack)(SYMatCallBackType nType,OUT void* pOut,int hDestTex,void* pCallBackParam,void* pEnvParam);
enum SYMaterialTexType
{
	SYMaterial_DiffuseTex,		//材质
	SYMaterial_SpecTex,		//高光
	SYMaterial_EnvMapTex,		//环境
	SYMaterial_EmissiveTex,	//光影
	SYMaterial_BumpTex,		//凹凸
	SYMaterial_TexTypeCount,
};
enum SYMaterialShaderType
{
	SYMaterial_DiffuseShader,		//材质
	SYMaterial_SpecShader,			//高光
	SYMaterial_EnvMapShader,		//环境
	SYMaterial_EmissiveShader,		//光影
	SYMaterial_BumpShader,			//凹凸
	SYMaterial_LightShader,
	SYMaterial_ShaderTypeCount,
};
enum SYTexParamDefType
{
	SYTex_Offset_ParamDef,
	SYTex_Scale_ParamDef,
	SYTex_Center_ParamDef,
	SYTex_Rotate_ParamDef,
	SYTex_Private_ParamDef,
	SYTex_ParamDefCount,
};
enum SYMaterialChangeType
{
	SYMaterial_NoChange				= 0x00000000,
	SYMaterial_PhyDataChange		= 0x00000001,
	SYMaterial_TexCountChange		= 0x00000002,

	SYMaterial_DiffTexChange		= 0x00000004,
	SYMaterial_SpecTexChange		= 0x00000008,
	SYMaterial_EnvMapTexChange		= 0x00000010,	
	SYMaterial_EmissiveTexChange	= 0x00000020,
	SYMaterial_BumpTexChange		= 0x00000040,

	SYMaterial_DiffuseParamChange	= 0x00000080,
	SYMaterial_SpecParamChange		= 0x00000100,	
	SYMaterial_EnvMapParamChange	= 0x00000200,
	SYMaterial_EmissiveParamChange	= 0x00000400,
	SYMaterial_BumpParamChange		= 0x00000800,	
	SYMaterial_ShaderChange			= 0x00002000,	
};
enum SYMaterialTexSrcType
{
	SYMaterial_Invid = -1,
	SYMaterial_PicFile,
	SYMaterial_MovFile,
	SYMaterial_Object,
	SYMaterial_Channel,
	SYMaterial_Buffer,
};
enum SYMaterialShaderParamType
{
	SYMaterial_DiffuseShaderParam,	//材质
	SYMaterial_SpecShaderParam,		//高光
	SYMaterial_EnvMapShaderParam,	//环境
	SYMaterial_EmissiveShaderParam,	//光影
	SYMaterial_BumpShaderParam,		//凹凸
	SYMaterial_PhyParam,
	SYMaterial_LightParam,
	SYMaterial_CameraParam,
	SYMaterial_ShaderParamCount,

};
enum SYMaterialParamDefType
{
	SYMaterial_Phy_ParamDef,
	SYMaterial_Phy_Diffuse_ParamDef,
	SYMaterial_Phy_Ambient_ParamDef,
	SYMaterial_Phy_Specular_ParamDef,
	SYMaterial_Phy_Emissive_ParamDef,
	SYMaterial_Phy_Alpha_ParamDef,

	SYDiffuseTex_Offset_ParamDef,
	SYDiffuseTex_Scale_ParamDef,
	SYDiffuseTex_Center_ParamDef,
	SYDiffuseTex_Rotate_ParamDef,
	SYDiffuseTex_Private_ParamDef,

	SYSpecTex_Offset_ParamDef,
	SYSpecTex_Scale_ParamDef,
	SYSpecTex_Center_ParamDef,
	SYSpecTex_Rotate_ParamDef,
	SYSpecTex_Private_ParamDef,

	SYEnvMapTex_Offset_ParamDef,
	SYEnvMapTex_Scale_ParamDef,
	SYEnvMapTex_Center_ParamDef,
	SYEnvMapTex_Rotate_ParamDef,
	SYEnvMapTex_Private_ParamDef,

	SYEmissiveTex_Offset_ParamDef,
	SYEmissiveTex_Scale_ParamDef,
	SYEmissiveTex_Center_ParamDef,
	SYEmissiveTex_Rotate_ParamDef,
	SYEmissiveTex_Private_ParamDef,

	SYBumpTex_Offset_ParamDef,
	SYBumpTex_Scale_ParamDef,
	SYBumpTex_Center_ParamDef,
	SYBumpTex_Rotate_ParamDef,
	SYBumpTex_Private_ParamDef,
};

#define 	SYMaterial_ALLChange		(0xfff|SYMaterial_TexCountChange | SYMaterial_ShaderChange)
struct SYPhyData
{
	int			disable;
	DWORD		Diffuse;        /* Diffuse color RGBA */
	DWORD		Ambient;        /* Ambient color RGB */
	DWORD		Specular;       /* Specular 'shininess' */
	DWORD		Emissive;       /* Emissive color RGB */
	float		Power;          /* Sharpness if specular highlight */
	float		fAlpha;
	SYPhyData(){Reset();}
	BASEGRAPH_DATARENDER_API void Reset();
};

enum SYMatTexRunDir
{
	SYMatRunLeft,
	SYMatRunRigth,
	SYMatRunTop,
	SYMatRunBottom,
};

struct SYMatTex
{
	int disable;
	int nType;
	int nSrcType;
	struct
	{
		TCHAR* pTexPath;
		TCHAR* pTexSrc;
		int		nChannel;
		SYBufferDef32* pBufferDef;
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
	SYMatTex(){Reset();}
	BASEGRAPH_DATARENDER_API void Reset();
};
struct SYMatTexArray
{
	union{
		struct
		{
			SYMatTex*	pDiffuseTex;
			SYMatTex*	pSpecTex;
			SYMatTex*	pEnvMapTex;
			SYMatTex*	pEmissiveTex;
			SYMatTex*	pBumpTex;
		};
		SYMatTex* ppTex[SYMaterial_TexTypeCount];
	};
};
struct SYJoinShader
{
	int		nType;
	char*	strHead;		//shader头，包括全局变量纹理等寄存器的定义
	char*	strBody;		//shader执行部分
	SYJoinShader(){Reset();}
	BASEGRAPH_DATARENDER_API void Reset();
};
struct SYShaderArray
{
	union{
		struct
		{
			SYJoinShader*	pDiffuseTexShader;
			SYJoinShader*	pSpecTexShader;
			SYJoinShader*	pEnvMapTexShader;
			SYJoinShader*	pEmissiveTexShader;
			SYJoinShader*	pBumpTexShader;
			SYJoinShader*	pLightShader;
		};
		SYJoinShader* ppShader[SYMaterial_ShaderTypeCount];
	};
};
struct SYMaterial
{
	DWORD			m_nVersion;
	int				nMaterialType;
	SYPhyData		m_PhyData;
	SYMatTexArray	m_TexArray;
	SYShaderArray	m_ShaderArray;
	SYMaterial(){Reset();}
	BASEGRAPH_DATARENDER_API void Reset();
};
typedef struct tag_SYMaterialParam
{
	//struct tag_common
	//{
	//	param_def	Diffuse;
	//	param_def	Ambient;
	//	param_def	Specular;
	//	param_def	Emissive;
	//	param_def	fAlpha;
	//}common;
	SYArray texParam[SYMATERIAL_TEX_COUNT];//曲线数组
}SYMaterialParam,*LPSYMaterialParam;
typedef struct tag_SYSubModelParam
{
	int nSubModel;
	SYMaterialParam param;
}SYSubModelParam,*LPSYSubModelParam;
struct SYMovieResource
{
	//IVideoSource*	pVideoSource;
	int				nMovieWidth;
	int				nMovHeight;
	int				nTotalFrame;
	BOOL			bIsProgress;
	int				hTex[2];
	int				nCur;
};

typedef struct  tag_SYTypesetInterface
{
	tag_SYTypesetInterface()
	{
		memset(this, 0, sizeof(tag_SYTypesetInterface));
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
	void (*Modify2DUnitTo3DUnit)(SYUnit* punit3d, SYUnit* punit2d,SYMaterial** p3dcolor,int nSubFace,LPSYStretchParam pStretch);
	//从属性页取属性，参数为空的属性不取。
	SYTypesetInterface_GetPropFromPropPage GetPropFromPropPage;

	//Item类的接口是否已经初始化
	BOOL bInitialized;
}SYTypesetInterface, *LPSYTypesetInterface;
struct SYMovieSeekData
{
	LPVOID			pVideoSeekData;
	int				nWidth;
	int				nHeight;
	int				nTotalFrame;
	CSYBuffer		movie_buffer;
};
struct SYSpecTexPrivateData 
{
	float fFactor;
	SYSpecTexPrivateData(){Reset();}
	BASEGRAPH_DATARENDER_API void Reset();
};
typedef SYSpecTexPrivateData SYEnvMapTexPrivateData;
typedef SYSpecTexPrivateData SYEmissiveTexPrivateData;
struct SYBumpTexPrivateData
{
	float fFactor;
	BOOL  bReverse;
	SYBumpTexPrivateData(){Reset();}
	BASEGRAPH_DATARENDER_API void Reset();
};
struct SYMatVideoTex
{
	int				disable;
	int				nType;
	int				hTex;
	BOOL			bTexNeedRelease;
	SYMovieResource* pResource;
	SYMovieSeekData* pSeekData;
	int				hTexCB;
	float2			CurTexOffset;
	float2			CurTexScale;
	float2			CurCenter;
	float			CurTexRotate;
	matrix4f		TexMat;
	void*			pCurData;
};
struct SYMatVideoTexArray
{
	union
	{
		struct
		{
			SYMatVideoTex*	pDiffuseVTex;
			SYMatVideoTex*	pSpecVTex;
			SYMatVideoTex*	pEnvMapVTex;
			SYMatVideoTex*	pEmissiveVTex;
			SYMatVideoTex*	pBumpVTex;
		};
		SYMatVideoTex* ppVTex[SYMaterial_TexTypeCount];
	};
};
struct SYPhyRenderData
{
	float4		Diffuse;        /* Diffuse color RGBA */
	float4		Ambient;        /* Ambient color RGB */
	float4		Specular;       /* Specular 'shininess' */
	float4		Emissive;       /* Emissive color RGB */
	float		Power;          /* Sharpness if specular highlight */
	float		fAlpha;
	int			disable;
};

struct SYVideoMaterial
{
	int					m_hDevice;
	float				m_fAlpha;//最终的alpha,包括物件的alpha
	SYPhyRenderData		m_PhyData;
	SYPhyRenderData		m_PhyDataStatic;
	SYMatVideoTexArray	m_VideoTexAr;
	int					m_hPhyCB;
	char*				m_pShaderStr;
	TCHAR*				m_pShaderName;
	int					m_nChangeType;
	double				m_dTime;
	double				m_dTotalTime;
	int					m_nLightNum;
	BOOL				m_bTexClamp;
	BOOL				m_bEdit;
	SYMaterial*			m_pMaterial;
};


BASEGRAPH_DATARENDER_API void		SYMaterialModuleInit(LPCTSTR pShaderPath,SYMatBufferCallBack pObject = NULL,void* pObjParam = NULL,SYMatBufferCallBack pChannel = NULL,void* pChannelParam = NULL);
BASEGRAPH_DATARENDER_API void		SYMaterialModuleRelease();
//内存材质处理函数
BASEGRAPH_DATARENDER_API void		SYMaterial_Init( SYMaterial* pMaterial,DWORD InitColor = 0xff777777 ,BOOL bLight = TRUE);
BASEGRAPH_DATARENDER_API void		SYMaterial_Release(SYMaterial* pMaterial);
BASEGRAPH_DATARENDER_API int		SYMaterial_Copy( OUT SYMaterial*  pDesMaterial, IN const SYMaterial* pSrcMaterial );
BASEGRAPH_DATARENDER_API BOOL		SYMaterial_Compare( const SYMaterial* pMaterial1, const SYMaterial* pMaterial2 );
BASEGRAPH_DATARENDER_API BOOL		SYMatTex_Compare( const SYMatTex* pMatTe1, const SYMatTex* pMatTe2);
BASEGRAPH_DATARENDER_API BOOL		SYJoinShader_Compare( const SYJoinShader* pShader1, const SYJoinShader* pShader2);

BASEGRAPH_DATARENDER_API void		SYMaterial_CreateParam(SYMaterialParam* pMatParam ,SYMaterial*  pMaterial );
BASEGRAPH_DATARENDER_API void		SYMaterial_CopyParam(SYMaterialParam* pDestMatParam,const SYMaterialParam* pSrcMatParam   );
BASEGRAPH_DATARENDER_API int		SYMaterial_AddKeyValue(double dFrame,SYMaterialParam* pMatParam, SYMaterial* pMaterial,void* pValue,int nParamType );
BASEGRAPH_DATARENDER_API void		SYMaterial_ReleaseParam(SYMaterialParam* pMatParam ,SYMaterial*  pMaterial );

BASEGRAPH_DATARENDER_API int		SYMaterial_AddTex(SYMaterial* pMaterial,int nTexType);
BASEGRAPH_DATARENDER_API int		SYMaterial_RemoveTex(SYMaterial* pMaterial,int nTexType);
BASEGRAPH_DATARENDER_API void		SYMaterial_RemoveShader(SYMaterial* pMaterial,int nShaderType);
BASEGRAPH_DATARENDER_API void		SYMaterial_AddShader(SYJoinShader* pShader ,int nShaderType);

BASEGRAPH_DATARENDER_API void*		SYMaterial_GetTexSrc(SYMaterial* pMaterial,int nTexType);
BASEGRAPH_DATARENDER_API int		SYMaterial_GetTexSrcType(SYMaterial* pMaterial,int nIndex);
BASEGRAPH_DATARENDER_API int		SYMaterial_SetTexSrc(SYMaterial* pMat,int nTexIndex,int nSrcType,void* pData);

BASEGRAPH_DATARENDER_API int		SYMaterial_SetTexRepeat(SYMaterial* pMat,int nTexIndex,BOOL bRepeat);
BASEGRAPH_DATARENDER_API int		SYMaterial_SetTexOffset(SYMaterial* pMat,int nTexIndex,float2* pOffset);
BASEGRAPH_DATARENDER_API int		SYMaterial_SetTexScale(SYMaterial* pMat,int nTexIndex,float2* pScale);
BASEGRAPH_DATARENDER_API int		SYMaterial_SetTexCenter(SYMaterial* pMat,int nTexIndex,float2* pCenter);
BASEGRAPH_DATARENDER_API int		SYMaterial_SetTexRotate(SYMaterial* pMat,int nTexIndex,float fRotate);
BASEGRAPH_DATARENDER_API int		SYMaterial_SetTexPrivateData(SYMaterial* pMat,int nTexIndex,void* pData);
BASEGRAPH_DATARENDER_API int		SYMaterial_SetPhyColor(SYMaterial* pMat,DWORD color);
BASEGRAPH_DATARENDER_API int		SYMaterial_SetPhyParam(SYMaterial* pMat,const SYPhyData* pData);
BASEGRAPH_DATARENDER_API int		SYMaterial_SetPhyParam(SYMaterial* pMat,const SYPhyRenderData* pData);
BASEGRAPH_DATARENDER_API int		SYUpdateDynamicTex(double dTotalFrame,int hDevice,SYMatVideoTex* pVideoTex,SYMatTex* pTex);//负责准备计算数据

//材质显存数据处理
BASEGRAPH_DATARENDER_API void		SYMaterial_CreateVideoMat( SYVideoMaterial* pVideoMat,SYMaterial* pMaterial ,int hDevice,BOOL bEdit /*= FALSE*/);
BASEGRAPH_DATARENDER_API void		SYMaterial_ReleaseVideoMat(SYVideoMaterial* pVideoMat);
BASEGRAPH_DATARENDER_API int		SYMaterial_VideoMatUpdateCopyVideo(SYVideoMaterial* pVideoMatDes, double dFrame,float fFrameRate,float fAlpha,int nLightNum,SYMaterialParam* param,void* pEnvParam,SYVideoMaterial* pVideoMatSrc);
BASEGRAPH_DATARENDER_API int		SYMaterial_UpdateFrame( SYVideoMaterial* pVideoMat,double dFrame,float fFrameRate,float fAlpha,int nLightNum,SYMaterialParam* param,void* pEnvParam, LPRECT pBound = NULL);
BASEGRAPH_DATARENDER_API BOOL		SYMaterial_UpdateTex(SYBufferDef32* pSrcBufferDef,DXHalDef32* pSrcTexDef,int hSrcTex, int nTexIndex,double dTime, float fFrameRate,SYVideoMaterial* pVideoMat,void* param,BOOL isForceUpdate = FALSE, LPRECT pBound = NULL);
BASEGRAPH_DATARENDER_API BOOL		SYMaterial_UpdateTexCopyVideo(SYBufferDef32* pSrcBufferDef,DXHalDef32* pSrcTexDef,int hSrcTex, int nTexIndex,double dTime, float fFrameRate,SYVideoMaterial* pVideoMat,SYVideoMaterial* pVideoMatSrc,void* param,BOOL isForceUpdate = FALSE);
BASEGRAPH_DATARENDER_API BOOL		SYMaterial_SetTex(int hTex,const RECT& pValidRt,int nOriginX, int nOriginY, SYVideoMaterial* pVideoMat, int nTexIndex,BOOL bNeedRelease = TRUE);
BASEGRAPH_DATARENDER_API BOOL		SYMaterial_SetTex(int hTex,const rectf& pValidRt,int nOriginX, int nOriginY, SYVideoMaterial* pVideoMat, int nTexIndex,BOOL bNeedRelease = TRUE);

BASEGRAPH_DATARENDER_API void		SYMaterial_SetParamToDevice( SYVideoMaterial* pVideoMat, BOOL bAdd);
BASEGRAPH_DATARENDER_API void		SYMaterial_SetChangeType(SYVideoMaterial* pVideoMat,int nChangeType);
BASEGRAPH_DATARENDER_API size_t		SYMaterial_GetVideoMemSize(SYVideoMaterial* pVideoMat);
BASEGRAPH_DATARENDER_API void		SYMaterial_SetStaticPhyParam(SYVideoMaterial* pMat,SYPhyRenderData* pData,int nType);
BASEGRAPH_DATARENDER_API int		SYMaterial_SetTexParam(SYMaterial* pMat,int nTexIndex,SYMatTex*	pTexParam,int nType);
BASEGRAPH_DATARENDER_API void		SYMaterial_PhyRenderDataToPhy(SYPhyData* pData,SYPhyRenderData* pSrcData); 
BASEGRAPH_DATARENDER_API void		SYMaterial_PhyToPhyRenderData(SYPhyRenderData* pData,SYPhyData* pSrcData); 
#endif
