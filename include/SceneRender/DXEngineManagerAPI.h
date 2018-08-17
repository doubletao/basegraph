#pragma once
//////////////////////////////////////////////////////////////////////////
#define VS_FILE			_T("\\VS\\Easy.VS")
#define PS_FILE			_T("\\PS\\Easy.PS")
#define VS_FILE_FOG		_T("\\VS\\Easy2.VS")
#define PS_FILE_FOG		_T("\\PS\\Easy2.PS")
#define DEFAULT_LINE_NUM	10
#include "SceneRender\DXEngineDataDef.h"

struct BaseGraphEasyVertex
{
	float3	pos;
	DWORD	color;
	float2	tex;
};
//////////////////////////////////////////////////////////////////////////
//texture操作
struct CSingleResource
{
	int  hResource;
	int  nFactor;
	bool bUsed;
};
class BASEGRAPH_SCENE_RENDER_API CTextureSet
{
public:
	CTextureSet();
	~CTextureSet();
public:
	BOOL Init( IN int hDevice, IN int nWidth, IN int nHeight,int nAAFactor );
	int  GetDevice() {return m_hDevice;};
	int  GetWidth() {return m_nWidth;};
	int  GetHeight() {return m_nHeight;};
	int	 GetMaxFactor(){return m_nMaxFactor;}
	int  GetCount() {return (int)BaseAR_GetSize(&m_arResource);};
	int  GetResource(IN int nIndex);
	int  GetResourceFactor(IN int nIndex);
	bool ResourceIsUsed(IN int nIndex);
	void SetResourceUsed( IN int nIndex, IN bool bUsed );
	int AddResource(int hResource,int nFactor);
	void RemoveResource(int nIndex);
	void Lock(){EnterCriticalSection(&m_Lock);}
	void UnLock(){LeaveCriticalSection(&m_Lock);}
protected:
	CSingleResource* GetSingleResource(int nIndex){return (CSingleResource*)BaseAR_GetAt(&m_arResource,nIndex);}
	int  m_hDevice;
	int  m_nWidth;
	int  m_nHeight;
	BaseARray m_arResource;//CSingleResource数组
	int	 m_nMaxFactor;
	CRITICAL_SECTION m_Lock;
};
//////////////////////////////////////////////////////////////////////////
class CBase3DLineManager
{
public:
	CBase3DLineManager(int nMaxVertexSize = DEFAULT_LINE_NUM, int nMaxIndexSize = DEFAULT_LINE_NUM);
	~CBase3DLineManager();
	void InitialManager(int hDevice);
	void ReleaseManager(int hDevice);
	void MoveTo(int hDevice,const float3* vStartPos,DWORD* color = NULL);
	void LineTo(int hDevice,const float3* vEndPos,DWORD* color = NULL);
	void DrawCircle(int hDevice,float fRad,const float3* pCenter,const matrix4f* pMatrix ,DWORD* pcolor = NULL);
	void DrawEllipse(int hDevice,float fRad_A,float fRad_B,const float3* pCenter,const matrix4f* pMatrix,DWORD* pcolor = NULL);
	DWORD SetColor(DWORD color);
	void Render(int hDevice,matrix4f* pViewPorjMat);
	void RenderFog(int hDevice,matrix4f* pViewMat,matrix4f* pPorjMat);
	void GetCurentPosition(float3* vNowPos);
	void Release(int hDevice);
	void DrawSquare(int hDevice,const float3* center, float length,const matrix4f* pMatrix);
	void DrawBox(int hDevice, const float3& vMin,const float3 vMax, const matrix4f& pMat,DWORD* pColor = NULL);
	void DrawCone( int hDevice,float fUpRad,float fDownRad,float fHeight,const float3* pCenter,const matrix4f* pMatrix ,DWORD* pcolor = NULL);

	void BeginRender();
	void clearLine();
protected:
	BaseGraphEasyVertex*	m_pLineVertex;
	BaseGraphEasyVertex*	m_pNowVertex;
	DWORD	m_dColor;
	int		m_hLineVertex;
	int		m_NumVertex;
	int		m_nVedioVertexNum;
	int		m_MaxVertexSize;
	int		m_hMatrixBuffer;
	int		m_hVertexShader;
	int		m_hPixShader;
	int		m_hMatrixBufferfog;
	int		m_hVertexShaderfog;
	int		m_hPixShaderfog;
	int		m_hDevice;
};
//////////////////////////////////////////////////////////////////////////
//TEXTURE操作
BASEGRAPH_SCENE_RENDER_API BOOL		DXTextureManager_Init( IN int hDevice, IN int nBufferWidth, IN int nBufferHeight, IN int nAAFactor );
BASEGRAPH_SCENE_RENDER_API BOOL		DXTextureManager_Release();
BASEGRAPH_SCENE_RENDER_API int		DXTextureManager_QueryTexture( IN DXTextureType nTexType,int nAAFactor);//nAAFactor纹理大小的倍数
//the width and height of the texture queryed out may be different from the param; 
BASEGRAPH_SCENE_RENDER_API int		DXTextureManager_QueryTexture( IN DXTextureType nTexType,int nWidth,int nHeight);
BASEGRAPH_SCENE_RENDER_API BOOL		DXTextureManager_UnQueryTexture(IN int hTexture,BOOL bForceRelease = TRUE);

BASEGRAPH_SCENE_RENDER_API int		DXTMGetTexBufferWidth();
BASEGRAPH_SCENE_RENDER_API int		DXTMGetTexBufferHeight();
BASEGRAPH_SCENE_RENDER_API BOOL		DXTextureManager_ResetBufferSize( int nBufferWidth,int nBufferHeight );
//设备相关操作
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_Init(DXDeviceType nType,LPCTSTR lpPathShader);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_Release();
BASEGRAPH_SCENE_RENDER_API int		DX3DEngine_GetType();
BASEGRAPH_SCENE_RENDER_API int		DX3DEngine_CreateDevice(int nScreenWidth,int nScreenHeight,BOOL bDoubleDevice = TRUE);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_ReleaseDevice(int hDevice);
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_Lock(int hDevice);
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_UnLock(int hDevice);
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_ResLock(int hDevice);
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_ResUnLock(int hDevice);
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_GetDeviceRemoveReason(int hDevice);
BASEGRAPH_SCENE_RENDER_API int		DX3DEngine_CreateContext(int hDevice);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_ReleaseContext(int hDevice,int hContext);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_SetContext(int hDevice,int hContext);

//获得swapchain的宽高，刷新率，是否全屏方式
BASEGRAPH_SCENE_RENDER_API int		DX3DEngine_GetSwapChainDesc(int hDevice,int nSwapchain,int& nWidth,int& nHeight,BOOL& bWindowed );
//////////////////////////////////////////////////////////////////////////
//纹理等显存资源相关操作
BASEGRAPH_SCENE_RENDER_API int 		DX3DEngine_CreateBuffer( int hDevice,int nSize,int nNum,void* pBuffer,DXBufferType nType,int nPriType = -1 );
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_ReleaseBuffer( int hDevice,int hBuffer );
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_ModifyBuffer( int hDevice,int hBuffer,void* pBuffer );
BASEGRAPH_SCENE_RENDER_API int		DX3DEngine_CreateSwapChain(int hDevice,HWND hWnd,int nWidth,int nHeight,BOOL bWindowed = TRUE);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_ReleaseSwapChain(int hDevice,int hSwapChain);
BASEGRAPH_SCENE_RENDER_API int		DX3DEngine_GetSwapChainBuffer(int hDevice,int hSwapChain);
BASEGRAPH_SCENE_RENDER_API int		DX3DEngine_CreateTexture(int hDevice,int nWidth,int nHeight,DXTextureType nType);
BASEGRAPH_SCENE_RENDER_API int		DX3DEngine_CreateTexFromBuffer(int hDevice,DWORD* pBuffer,int nWidth,int nHeight,DXTextureType nType);
BASEGRAPH_SCENE_RENDER_API int		DX3DEngine_CreateTextureFromFile(int hDevice,TCHAR* pPath,BOOL bNormalMap);
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_ResizeSwapChain(int hDevice,int hSwapChain,int nWidth,int nHeight);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_GetTextureInfo(int hDevice,int hTexture,int* nWidth, int* nHeight,int* nType);
BASEGRAPH_SCENE_RENDER_API int		DX3DEngine_GetTextureMipMap(int hDevice,int hTexture);
BASEGRAPH_SCENE_RENDER_API bool		DX3DEngine_ReleaseTexture(int hDevice,int hTexture);
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_ClearTexture(int hDevice,int hTexture,DWORD color = 0);//如果为普通纹理，pData的类型为DWORD,如果为深度buffer,pData的类型为float
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_ClearDepth(int hDevice,int hTexture,float depth = 1.f);//如果为普通纹理，pData的类型为DWORD,如果为深度buffer,pData的类型为float
BASEGRAPH_SCENE_RENDER_API void*		DX3DEngine_GetMemTexBufferPtr(int hDevice, int hTex, DWORD* pPitch);
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_CopyTexture( int hDevice,int hDesTexture,int nOffsetX,int nOffsetY,int nDestWidth,int nDestHeight,
								 int hSrcTexture,int nSrcOffsetX,int nSrcOffsetY,int nSrcWidth,int nSrcHeight );//要拷贝区域的大//NO SCALE
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_DumpTexture(int hDevice,int hTexture, LPCTSTR szFile = NULL);//调试用
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_DumpRenderTarget(int hDevice,LPCTSTR szFile = NULL);//调试用
BASEGRAPH_SCENE_RENDER_API size_t	DX3DEngine_GetTextureMemSize(int hDevice,int hTexture);//调试用
//出于效率考虑，不完全支持buffer的放缩下行，如果目的的有效区域超过屏幕或者工程大小，下行会失败
//可以先进行不放缩下行，然后再对内存buffer进行缩放
BASEGRAPH_SCENE_RENDER_API BOOL    	DX3DEngine_Download( OUT DWORD* pDesBuffer, 
						 IN int hDevice,int hTransTex,//用于下行的中间buffer,buffer类型必须为mem
						 IN int nDesOffestX, IN int nDesOffestY, IN int nDesWidth, IN int nDesHeight, IN int nDestBufferWidth, IN int nDestBufferHeight,
						 int hSrcTex,
						 IN int nSrcOffestX, IN int nSrcOffestY, IN int nSrcWidth, IN int nSrcHeight, BUFFER_MODE eMode = BUFFER_COPY) ;
//出于效率考虑，不完全支持buffer的缩放上行，如果源的有效区域超过屏幕或者工程大小，上行会失败
//可以先进行不缩放上行，然后再对内存buffer进行缩放
BASEGRAPH_SCENE_RENDER_API BOOL    	DX3DEngine_UpLoad( OUT int hTex, 
						 IN int hDevice,int hTransTex,//用于下行的中间buffer,buffer类型必须为mem 
						 IN int nDesOffestX, IN int nDesOffestY, IN int nDesWidth, IN int nDesHeight,
						 IN DWORD* pBuffer, 
						 IN int nSrcOffestX, IN int nSrcOffestY, IN int nSrcWidth, IN int nSrcHeight, IN int nSrcBufferWidth, IN int nSrcBufferHeight,BOOL bEngineLock = FALSE); 
BASEGRAPH_SCENE_RENDER_API BOOL 		DX3DEngine_ComputeNormalMap(int hDevice,int hDestTex,int hSrcTex);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_GenerateMip(int hDevice,int hTexture);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_MuliSampleTexToNormalTex(int hDevice,int hMultiSampleTex,int hNormalTex);
//////////////////////////////////////////////////////////////////////////
//渲染相关操作
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_SetViewPort(int hDevice,const RECT* pRect);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_GetViewPort(int hDevice, RECT* pRect);

BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_SetScissor(int hDevice, const RECT* pRect);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_GetScissor(int hDevice, RECT* pRect);

/*================================================================================
BeginRender和endRender之间，BeginRenderTexture和endRenderTexture之间的流程是线程安全的，
因此在使用这些的时候要注意线程死锁情况。
================================================================================*/
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_BeginRender( IN int hDevice, IN BOOL bAntiAlias = FALSE );
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_EndRender(IN int hDevice);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_BeginRenderTexture(int hDevice);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_EndRenderTexture(int hDevice);

BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_SetRenderTarget(int hDevice,int hRenderTarget,int hDepth);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_GetRenderTarget(int hDevice,int* phRenderTarget,int* phDepth);
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_ClearScreen( int hDevice, DWORD nColor, float fZValue, DXClearRTType nClearTarget );
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_RenderTexture( int hDevice,DXHalDef32* pTex,const matrix4f* pWorldMatrix,const RECT* prtClip ,BOOL bAdjustColor,float fAlpha=1.0);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_RenderTexture( int hDevice,int hTex,float fAlpha,const RECT* pSrcRt,const RECT* pDesRt ,BOOL bAdjustColor,FIELD field = BOTH_FIELD,BOOL bStretch = TRUE);
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_Render(int hDevice,int hVertexBuffer,int hIndexBuffer,int nVertexType ,int nStrart,int PrimitiveCount,int nVertexCount);//nVertexCount兼容dx9
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_Render(int hDevice,int hVertexBuffer,int hIndexBuffer,int hVertexShader,int hGeometryShader,int hPixShader,int nStrart,int nVertexCount);
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_RenderInstance(int hDevice,int hVertexBuffer,int hInstanceBuffer,int hIndexBuffer,int nVertexType ,int nStart,int PrimitiveCount,int nVertexCount,int nInstanceCount);//nVertexCount兼容dx9
BASEGRAPH_SCENE_RENDER_API int		DX3DEngine_Present(int hDevice,int hSwapChain,int nTimeSyn = 0);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_RenderTexture( int hDevice, int hTex,matrix4f* pMatrix)	;

//////////////////////////////////////////////////////////////////////////
//浸染状态相关操作
BASEGRAPH_SCENE_RENDER_API int		DX3DEngine_CreateShaderFromString(int hDevice,BOOL bEngineLock,LPCTSTR shaderName,LPCSTR shaderCode,DXShaderType nType,DXVertexType* pVertexType = NULL/*FOR VS*/);
BASEGRAPH_SCENE_RENDER_API int		DX3DEngine_CreateShaderFromFile(int hDevice,LPCTSTR shaderName,LPCTSTR shaderFilePath,DXShaderType nType,DXVertexType* pVertexType = NULL/*FOR VS*/);
BASEGRAPH_SCENE_RENDER_API int		DX3DEngine_CreateShaderFromCompliedFile(int hDevice,LPCTSTR shaderName,LPCTSTR shaderFilePath,DXShaderType nType,DXVertexType* pVertexType = NULL/*FOR VS*/);
BASEGRAPH_SCENE_RENDER_API int		DX3DEngine_GetShader(int hDevice,LPCTSTR shaderName);//会增加引用计数，需要release;
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_ReleaseShader(int hDevice,int hShader);
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_SetConstBuffer(int hDevice,int hBuffer,int nRegisterIndex,DXShaderType nShaderType);
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_SetShaderTexture(int hDevice,int hTexture,int nRegisterIndex,DXShaderType nShaderType);
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_SetVertexShader(int hDevice,int hVertexShader);
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_SetGeometreShader(int hDevice,int hVertexShader);
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_SetPixShader(int hDevice,int hVertexShader);
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_SetRenderState(int hDevice,int nType,int nValue);
BASEGRAPH_SCENE_RENDER_API int		DX3DEngine_GetRenderState(int hDevice,int nType);
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_SetSampleState(int hDevice,int nSampler,int nState,int nShaderType);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_GetRenderInfo( int hDevice, DXRenderInfo* pInfo);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_ClearRenderInfo( int hDevice);

//线段管理器
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_CreateLineManage(int hDevice);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_ReleaseLineManage(int hDevice);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_SetColor(int hDevice,DWORD Color );
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_MoveTo(int hDevice,const float3* vPos,DWORD* pColor = NULL);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_LineTo(int hDevice,const float3* vPos,DWORD* pColor = NULL);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_RenderLine(int hDevice,matrix4f* pViewProjMat);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_RenderLineFog(int hDevice,matrix4f* pViewMat,matrix4f* pProjMat);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_DrawCircle( int hDevice,float fRad,const float3* pCenter,const matrix4f* pMatrix ,DWORD* pcolor = NULL);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_DrawEllipse(int hDevice,float fRad_A,float fRad_B,const float3* pCenter,const matrix4f* pMatrix ,DWORD* pcolor = NULL);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_Square(int hDevice, const vector3f* center, float length,const matrix4f* pMatrix);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_DrawBox( int hDevice, const float3& vMin,const float3& vMax,const matrix4f& pMatrix ,DWORD* pcolor = NULL);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_DrawCone( int hDevice,float fUpRad,float fDownRad,float fHeight,const float3* pCenter,const matrix4f* pMatrix ,DWORD* pcolor = NULL);

//Mesh相关操作
BASEGRAPH_SCENE_RENDER_API int		DX3DEngine_CreateMeshFromXFile(int hDevice,LPCTSTR lpszFileName);
BASEGRAPH_SCENE_RENDER_API int		DX3DEngine_ReleaseMesh(int hDevice,int hMesh);
BASEGRAPH_SCENE_RENDER_API int		DX3DEngine_HitMesh(int hDevice,int hMesh,const float3* pStart,const float3* pDir,float3* pHitPoint = NULL,float* pfDist = NULL);
BASEGRAPH_SCENE_RENDER_API int		DX3DEngine_GetMeshBox(int hDevice,int hMesh,float3* pMin,float3* pMax);
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_RenderMesh(int hDevice,int hMesh, int nDrawCount = -1, unsigned int* nSubset = NULL);
BASEGRAPH_SCENE_RENDER_API int		DX3DEngine_RenderEasyMesh( int hDevice,DWORD color,int hVertex,int hIndex, int nVertexType ,int nStrart,int PrimitiveCount,int nVertexCount );
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_GetMeshInfo(int hDevice,int hMesh, int* nSubsetCount );

//高级渲染
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_RGBA_To_BGRA(int hDevice,int hSrcTex,const RECT* pSrcRt,int hDestTex,const RECT* pDesRt);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_ConvertFormat(int hDevice,int hSrcTex,FIELD srcField,const RECT* pSrcRt,int hDestTex,FIELD dstField,ColorConvertType nType);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_MakeFrame(int hDevice,int hFirstTex,int hSecondTex,const RECT* pRect,int hDestTex);
//key的幅面大小与field大小
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_ConvertFormatWithKey(int hDevice,int hSrcTex,int hSrcKey,FIELD srcField,const RECT* pSrcRt,int hDestTex,FIELD dstField,ColorConvertType nType);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_Blur_5Pt(int hDevice,int hSrcTex,const RECT* pSrcRt,int hDestTex,const RECT* pDesRt,const DXDistortionParam* pParam);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_Blur_9Pt(int hDevice,int hSrcTex,const RECT* pSrcRt,int hDestTex,const RECT* pDesRt,const DXDistortionParam* pParam);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_RenderMask(int hDevice,int hSrcTex,rectf& cutRt,int hMaskTex,DXHalDef32* pTargetTex);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_RenderDynamicTex( int hDevice,matrix4f* pMat,BOOL bRepeat,BOOL bUseColorAlpha,DXHalDef32* pSrcTex,DXHalDef32* pMaskTex,DXHalDef32* pTargetTex );
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_Render3DVisionTex(int hDevice,int hDstTex,const RECT* pDstRt,int hLeftTex,int hRightTex,const RECT* pSrcRt);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_FrontKeyOut(int hDevice,int hFrontTex,int hBackTex,const RECT* pSrcRt,int hDestTex,const RECT* pDestRt);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_ProcessBright( int hDevice,int eTexType,int hTex,const RECT* pSrcRt,int hDestTex,const RECT* pDestRt,float fBrightGain );  //eTexType:DY_COLOR_FORMAT
BASEGRAPH_SCENE_RENDER_API int		DX3DEngine_CreateEffectUnit(int hDevice,LPCTSTR EffectName,DXEffectTechDesc* techDesc,int nTechs);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_ReleaseEffectUnit(int hDevice,int hEffect);
BASEGRAPH_SCENE_RENDER_API int		DX3DEngine_GetQuadVB(int hDevice);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_EffectSetTexture(int hDevice,int hEffect,int hTex,const char* texName);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_EffectSetConstBuffer(int hDevice,int hEffect,int hCB,const char* bufName);
BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_EffectSetVariable(int hDevice,int hEffect,void* pScalar, int nCount, DXEffectVariableType nType,const char* varName);
BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_EffectRender(int hDevice,
												int hEffect,
												LPCTSTR TechniqueName,
												int hVertexBuffer,
												int hIndexBuffer,
												int nVertexType,
												int nPrimitiveCount,
												int nStart,
												int nVertexCount,
												int nIndexCount);

BASEGRAPH_SCENE_RENDER_API BOOL		DX3DEngine_EffectRenderInstance(int hDevice,
												int hEffect,
												LPCTSTR TechniqueName,
												int hVertexBuffer,
												int hIndexBuffer,
												int nVertexType,
												int nPrimitiveCount,
												int nStart,
												int nVertexCount,
												int nIndexCount,
												int hInstanceBuffer,
												int nInstanceCount);

BASEGRAPH_SCENE_RENDER_API void		DX3DEngine_TextOut(int hDevice,LPCTSTR strText,RECT* rtText,DWORD dwColor);
BASEGRAPH_SCENE_RENDER_API void		EngineManagerSetOptimal(BOOL bOptimal=TRUE);

BASEGRAPH_SCENE_RENDER_API BOOL RenderEngineDX10_Main(int ncmd, int hDevice, void* paramin, void* paramout);
BASEGRAPH_SCENE_RENDER_API void RenderEngineDX10SetOptimal(BOOL bOptimal = TRUE);
BASEGRAPH_SCENE_RENDER_API BOOL RenderSoftEngine_Main(int ncmd, int hDevice, void* paramin, void* paramout);
//////////////////////////////////////////////////////////////////////////
