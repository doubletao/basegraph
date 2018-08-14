#ifndef SYRenderSoftEngine_h__
#define SYRenderSoftEngine_h__

#include "assert.h"
#include "SceneRender\DXEngineDataDef.h"
#include <map>
//宏定义
#define MULTI_THREAD_COUNT 4
struct MixThreadParam
{
	int				threadid;
	HANDLE			hThread;

	HANDLE			hStart;
	HANDLE			hStop;
	HANDLE			hComplete;

	unsigned long*	des;
	int				desx;
	int				desy;
	int				des_buf_width;
	int				des_buf_height;
	unsigned long*	src;
	int				srcx;
	int				srcy;
	int				srccx;
	int				srccy;
	int				src_buf_width;
	int				src_buf_height;
	RECT* 			pCull;
	BUFFER_MODE  eMode;
};


class CDeviceLock
{
public:
	CDeviceLock(){
		InitializeCriticalSection(&m_DeviceLock);
	}
	~CDeviceLock(){
		DeleteCriticalSection(&m_DeviceLock);
	}
	void Lock()
	{
		EnterCriticalSection(&m_DeviceLock);
	}
	void UnLock()
	{
		LeaveCriticalSection(&m_DeviceLock);
	}
	CRITICAL_SECTION		m_DeviceLock;
};

class DXSingleLock
{
public:
	DXSingleLock(CDeviceLock& pLock,bool bAutoLock = true)
		:m_pLock(pLock),m_nCount(0)
	{
		if(bAutoLock)
			Lock();
	}
	~DXSingleLock()
	{
		while(m_nCount)
			UnLock();
	}
	void Lock()
	{
		EnterCriticalSection(&m_pLock.m_DeviceLock);
		m_nCount++;
	}
	void UnLock()
	{
		if(m_nCount)
		{
			LeaveCriticalSection(&m_pLock.m_DeviceLock);
			m_nCount--;
		}
	}
protected:
	CDeviceLock& m_pLock;
	size_t m_nCount;
};

struct Common_DeviceSoft;
class   SYDeviceContex
{
public:
	SYDeviceContex();
	~SYDeviceContex();
	//state
	void	BeginRenderTexture();
	void	EndRenderTexture();
	void	RenderMix(DWORD* pdwDesBuffer,		// 目标
			   IN int nDesX,					// 目标起点
			   IN int nDesY,
			   IN int nDesWidth,				// 目标操作区域大小
			   IN int nDesHeight,
			   IN int nDesBufferWidth,			// 目标缓冲区大小
			   IN int nDesBufferHeight,
			   IN DWORD* pdwSrcBuffer,			// 源
			   IN int nSrcX,					// 源起点
			   IN int nSrcY,
			   IN int nSrcWidth,				// 源操作区域大小
			   IN int nSrcHeight,
			   IN int nSrcBufferWidth,			// 源缓冲区大小
			   IN int nSrcBufferHeight,
			   IN BUFFER_MODE eMode,
			   RECT* pCull);	// 目标裁减矩形（可选项，为 NULL 时表示目标全部有效）)
	void SetRenderTarget(Common_DeviceSoft* pDevice,int hRT,int hDepth);
	void SetViewPort(int nLeft,int nTop,int nRight,int nBottom);
	void SetViewPort(const RECT& pRect){rtViewport = pRect;};
	void SetScissor(const RECT& pRect){rtScissor = pRect;};
	BOOL SetVertexBuffer(int hVertexBuffer);
	BOOL SetIndexBuffer(int hIndexBuffer);
	bool	SetRenderState(int nStateType,int nState);
	bool SetSamplerState(int nState,int nType,int nSampler);
	BOOL SetShaderTexture(int hTex,DXShaderType nShaderType,int nRegIndex);
	BOOL SetShader(int nShaderType,int hShader);
	int	SetConstBuffer(size_t nType,size_t  nIndex,int hBuffer){return pConstBuffer[nType][nIndex] = hBuffer;}

	const RECT& GetViewPort()const{return rtViewport;};
	const RECT& GetScissor()const{return rtScissor;};
	int	GetRenderState(int nStateType)const{return pNowState[nStateType];};
	int	GetDepth()const{return hNowDepth;}
	int	GetRenderTarget()const{return hNowRenderTarget;}
	int	GetConstBuffer(size_t nType,size_t  nIndex){return pConstBuffer[nType][nIndex];}
	int GetIndexBuffer(){return hNowIndexBuffer;};
	int GetVertexBuffer(){return hNowVertexBuffer;};
public:
	CDeviceLock				EngineLock;
	CDeviceLock				RenderLock;
	CDeviceLock				ContexLock;
protected:
	//state
	int						hNowRenderTarget;
	int						hNowDepth;
	int						pNowState[DXRenderState_Count];
	int						pSpritState[DXRenderState_Count];
	int						pNowSamplerState[3][16];//vs,ps,gs
	int						nNowPriType;
	int						pShader[3];
	int						pConstBuffer[3][14];//vs,ps,gs
	int						hNowIndexBuffer;
	int						hNowVertexBuffer;
	int						nShaderTexstate[3][128];

	RECT					rtViewport;
	RECT					rtScissor;

	//multi thread mix
	MixThreadParam  mix_threadparam[MULTI_THREAD_COUNT];
};

//data struct
struct Common_DeviceSoft
{
	matrix4f		spriteProj;
	htable			httexture;
	htable			htSwapChain;
	htable			htModel;
	htable			htShaderTable;
	htable			htContexTable;
	BaseHash*			ShaderHash;
	BaseHash*			TextureFromFileHash;
	BaseHash*			BumpTextureFromFileHash;
	htable			htBuffer;
	CDeviceLock		ResLock;
	std::map<DWORD,SYDeviceContex*> mapContext;
	SYDeviceContex  defaultContext;
////////////////////////////////////////////////////////////////////////////////
//render info
	DXRenderInfo			info;
////////////////////////////////////////////////////////////////////////////////
};

struct SYSoftTexture 
{
	TCHAR*						pTexFile;
	DXTextureType				nType;
	BOOL						bRecentRT;
	CCommonBuffer*					pTexture;
};

typedef SYSoftTexture SYSoftSwapChain ;
struct SYSoftBuffer
{
	DXBufferType nType;
	int			nVertexSize;
	int			nBufferSize;
	char*    	pBuffer;
};

//inline function
inline bool ModifyBuffer(Common_DeviceSoft* pDevice,int hBuffer,void* pMemBuffer)
{
	SYSoftBuffer* pBuffer = (SYSoftBuffer*) BseeHTable_GetAt(&pDevice->htBuffer,hBuffer);
	memcpy(pBuffer->pBuffer,pMemBuffer,pBuffer->nBufferSize);
	return true;
}
inline void ReleaseBuffer(Common_DeviceSoft* pDevice,int hBuffer)
{
	if (!hBuffer)
		return;
	SYSoftBuffer* pBuffer = (SYSoftBuffer*)BseeHTable_RemoveAt(&pDevice->htBuffer,hBuffer);
	switch(pBuffer->nType)
	{
	case DXVertexBuffer:
		pDevice->defaultContext.SetVertexBuffer((pDevice->defaultContext.GetVertexBuffer() == hBuffer) ? 0 : pDevice->defaultContext.GetVertexBuffer());
		break;
	case DXIndexBuffer:
		pDevice->defaultContext.SetIndexBuffer((pDevice->defaultContext.GetIndexBuffer() == hBuffer) ? 0 : pDevice->defaultContext.GetIndexBuffer());
		break;
	case DXConstBuffer:
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 14; j++)
			{
				if(pDevice->defaultContext.GetConstBuffer(i,j) != hBuffer)
					continue;
				pDevice->defaultContext.SetConstBuffer(i,j,0);
				goto END;
			}
		}
		break;
	}
END:
	SAFEDELETE_ARRAY(pBuffer->pBuffer);
	SAFE_DELETE(pBuffer);
}
inline CCommonBuffer* getTexture(Common_DeviceSoft* pDevice,int hTexture)
{
	SYSoftTexture* pSrcWDTexture = (SYSoftTexture*)BseeHTable_GetAt(&pDevice->httexture,hTexture);
	if (!pSrcWDTexture)
		return NULL;
	return pSrcWDTexture->pTexture;
}
inline void getTextureInfo(CCommonBuffer* pTexture,int* pWidth,int* pHeight)
{
	if(pWidth)
		*pWidth = pTexture->m_nWidth;
	if(pHeight)
		*pHeight = pTexture->m_nHeight;
}
#endif//softEngineEnd