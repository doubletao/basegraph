#ifndef BaseGraphRenderEngineDX10Api_h__
#define BaseGraphRenderEngineDX10Api_h__
#include "assert.h"
#include "DXEffectManager.h"
#include "Common\2DBufferAPI.h"
#include "3DSpaceConvert.h"
#include "SceneRender\DXEngineDataDef.h"
//////////////////////////////////////////////////////////////////////////
//宏定义
#define VSPROFILE "vs_4_0"
#define PSPROFILE "ps_4_0"
#define GSPROFILE "gs_4_0"

//data struct
struct Common_DeviceDX10
{
	ID3D10Device*	pDevice;
	ID3D10Device*	pSwapChainDevice;
	int				nAAFactor;
	int				nMutiSampleCount;
	int				nSampleQuality;
	matrix4f		spriteProj;
	htable			httexture;
	htable			htSwapChain;
	htable			htModel;
	htable			htShaderTable;
	BaseHash*			ShaderHash; 
	BaseHash*			TextureFromFileHash;
	BaseHash*			BumpTextureFromFileHash;
	htable			htBuffer;
	//state
	ID3D10DepthStencilState* pDepthState[DXDepthStateCount];
	ID3D10BlendState*		pBlendState[DXBlendStateCount];
	ID3D10RasterizerState*	pRasterizerState[DXRasStateCount];
	ID3D10SamplerState*		pSamplerState[DXSampleStateCount] ;

	//state
	int			hNowRenderTarget;
	int			hNowDepth;
	int			pNowState[DXRenderState_Count];
	int			pSpritState[DXRenderState_Count];
	int			pNowSamplerState[3][16];//vs,ps,gs
	int			nNowPriType;
	int			pShader[3];
	int			pConstBuffer[3][14];//vs,ps,gs
	int			hNowIndexBuffer;
	int			hNowVertexBuffer;
	int			nShaderTexstate[3][128];

	RECT		rtViewport;
	RECT		rtScissor;

	//引擎中使用的资源,需要释放
	int			hScreenVertex;	//全屏用顶点
	int			h2DVertex;		//全屏用顶点
	int			hScreenVS;		//全屏用VS
	int			h3DVisionPS;

	int			hScreenCB;
	int			hDynamicCB;
	int			hClearCB;
	int			hModiyCB;

	int			hConvertFormatPS;	//格式转换ps
	int			hOnlyKeyOutPS;//格式转换ps
	int			hDynamicPS;	//格式转换ps
	int			hBlur5PtPS;	//格式转换ps
	int			hBlur9PtPS;	//格式转换ps
	int			hClearScreenPS;
	int			hMaskRender;
	int			hScreenPS;	//格式转换ps
	int			hScreenAdjustPS;	
	int			hScreenMSPS;	//格式转换ps
	int			hUYVYBrightPS;
	int			hFieldAddKeyPS;
	int			hRGBA_Frame;
	int			hRGBA_YUAYVA4224PS;
	int			hUYVY_RGBA_PS0;
	int			hUYVY_RGBA_PS1;
	int			hUYVY_RGBA_KeyPS0;
	int			hUYVY_RGBA_KeyPS1;
	int			hUYVY_RGBA_Frame;
	int			hRGBA_UYVY_Frame;

	int			hRenderTexVS;
	int			hRenderTexPS;
	int			hRenderTexCB;
	int			hRenderTexCB5;

	htable				htEffectUnit;
	SYD3DEffectManager*	pEffectMan;

	CRITICAL_SECTION	EngineLock;
	CRITICAL_SECTION	RenderLock;
////////////////////////////////////////////////////////////////////////////////
//render info
////////////////////////////////////////////////////////////////////////////////
	DXRenderInfo		info;
	Common_DeviceDX10();
	void Reset();
};

struct BaseGraphGetMeshInfoIn
{
	int hMesh;
};

struct BaseGraphShader
{
	DXShaderType	nType;
	void*		pShader;
	ID3D10InputLayout* pLayout;//for vs
	TCHAR*		pShaderName;
};

struct BaseGraphTexture
{
	TCHAR*		pTexFile;
	void*		pTexture;
	DXTextureType	nType;
	void*		pRenderTarget;
	ID3D10ShaderResourceView*	pResourceView;

	BOOL		bRecentRT;
	void*		pMemBuffer;
	DWORD		nPitch;

	int			nNeedWidth;
	int			nNeedHeight;
	int			nRealWidth;
	int			nRealHeight;
};

struct BaseGraphSwapChain
{
	IDXGISwapChain*	pSwapChain;
	ID3D10Texture2D*	pShareTexture;
	int				hShareTexture;
	int				hBufferTexture;
};

struct CommonBuffer
{
	DXBufferType nType;
	int			nVertexSize;
	int			nBufferSize;
	ID3D10Buffer* pBuffer;
	int			nPriType;//for vb
};

struct screenCB
{
	float4 texCoord;
	matrix4f world_View_proj;
};

struct DynamicPixCB
{
	float4	texCoord;
	matrix4f	texWorld;
	float   fUseColorAlpha;
	float   fUnuse0;
	float   fUnuse1;
	float   fUnuse2;
};
//inline function
inline bool SetRenderState(Common_DeviceDX10* pDevice,int nStateType,int nState);
inline bool SetSamplerState(Common_DeviceDX10* pDevice,int nState,int nType,int nSampler);
inline BOOL SetVertexBuffer(Common_DeviceDX10* pDevice,int hVertexBuffer);
inline bool ModifyBuffer(Common_DeviceDX10* pDevice,int hBuffer,void* pMemBuffer)
{
	CommonBuffer* pBuffer = (CommonBuffer*) BseeHTable_GetAt(&pDevice->htBuffer,hBuffer);
	//从效率考虑,暂不支持indexbuffer的更新
	if(pBuffer->nType == DXIndexBuffer)
		return false;
	void* pMapBuffer = NULL;
	pBuffer->pBuffer->Map(D3D10_MAP_WRITE_DISCARD ,0,&pMapBuffer);
	if(!pMapBuffer)
		return false;
	memcpy(pMapBuffer,pMemBuffer,pBuffer->nBufferSize);
	pBuffer->pBuffer->Unmap();
	return true;
}
inline void CreateVertexDesc(D3D10_BUFFER_DESC* desc,int nSize)
{
    desc->ByteWidth = nSize;
    desc->Usage = D3D10_USAGE_DYNAMIC;
    desc->BindFlags = D3D10_BIND_VERTEX_BUFFER;
    desc->CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    desc->MiscFlags = 0;
}
inline void CreateIndexDesc(D3D10_BUFFER_DESC* desc,int nSize)
{
    desc->ByteWidth = nSize;
    desc->Usage = D3D10_USAGE_DEFAULT;
    desc->BindFlags = D3D10_BIND_INDEX_BUFFER;
    desc->CPUAccessFlags = 0;
    desc->MiscFlags = 0;
}
inline void CreateConstDesc(D3D10_BUFFER_DESC* desc,int nSize)
{
    desc->ByteWidth = nSize;
    desc->Usage = D3D10_USAGE_DYNAMIC;
    desc->BindFlags = D3D10_BIND_CONSTANT_BUFFER;
    desc->CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    desc->MiscFlags = 0;
}
inline void SetPrimitiveTopology(Common_DeviceDX10* pDevice,int nType)
{
	D3D10_PRIMITIVE_TOPOLOGY d3dPriType = D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED;
	switch (nType)
	{
	case LINE_LIST:
		d3dPriType = D3D10_PRIMITIVE_TOPOLOGY_LINELIST;
		break;
	case TRIANGLELIST:
		d3dPriType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		break;
	case TRIANGLESTRIP:
		d3dPriType = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		break;
	}
	pDevice->pDevice->IASetPrimitiveTopology(d3dPriType);
	pDevice->nNowPriType = (DXPrimitiveTypes)nType;
}
inline void ReleaseBuffer(Common_DeviceDX10* pDevice,int hBuffer)
{
	if (!hBuffer)
		return;
	CommonBuffer* pBuffer = (CommonBuffer*)BseeHTable_RemoveAt(&pDevice->htBuffer,hBuffer);
	if (!pBuffer)
		return;
	
	switch(pBuffer->nType)
	{
	case DXVertexBuffer:
		pDevice->hNowVertexBuffer = (pDevice->hNowVertexBuffer == hBuffer) ? 0 : pDevice->hNowVertexBuffer;
		break;
	case DXIndexBuffer:
		pDevice->hNowIndexBuffer = (pDevice->hNowIndexBuffer == hBuffer) ? 0 : pDevice->hNowIndexBuffer;
		break;
	case DXConstBuffer:
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 14; j++)
			{
				if(pDevice->pConstBuffer[i][j] == hBuffer)
					pDevice->pConstBuffer[i][j] = 0;
			}
		}
		break;
	}
	SAFE_RELEASE(pBuffer->pBuffer);
	SAFE_DELETE(pBuffer);
}
inline void ReleaseShader(Common_DeviceDX10* pDevice,int hShader)
{
	BaseGraphShader* pShader = (BaseGraphShader*)BseeHTable_GetAt(&pDevice->htShaderTable,hShader);
	if (pShader)
	{
		assert(pShader);
		shaderhash* pShaderHashValue = (shaderhash*)BaseHash_Search(pDevice->ShaderHash, (void *)pShader->pShaderName);
		--(pShaderHashValue->nCount);
		if (pShaderHashValue->nCount != 0)
			return;

		BaseHash_RemoveAt(pDevice->ShaderHash, (void *)pShader->pShaderName, TRUE);
		SAFEDELETE_ARRAY(pShader->pShaderName);
		switch (pShader->nType)
		{
		case DXShaderType_VertexShader:
		{
			ID3D10VertexShader * pPixShader = (ID3D10VertexShader *)pShader->pShader;
			SAFE_RELEASE(pPixShader);
		}
		break;
		case DXShaderType_GeometryShader:
		{
			ID3D10GeometryShader * pPixShader = (ID3D10GeometryShader*)pShader->pShader;
			SAFE_RELEASE(pPixShader);
		}
		break;
		case DXShaderType_PixelShader:
		{
			ID3D10PixelShader* pPixShader = (ID3D10PixelShader*)pShader->pShader;
			SAFE_RELEASE(pPixShader);
		}
		break;
		}
		SAFE_DELETE(pShader);
		BseeHTable_RemoveAt(&pDevice->htShaderTable, hShader);
	}
}

inline BOOL IsNeedScaleTexParam(Common_DeviceDX10* pDevice,int hTex,float& fScale)
{
	BaseGraphTexture* pTexture = (BaseGraphTexture*)BseeHTable_GetAt(&pDevice->httexture,hTex);
	if(!pTexture)
	{
		fScale = 1;
		return FALSE;
	}

	if(pTexture->nNeedWidth == pTexture->nRealWidth && 
		pTexture->nNeedHeight == pTexture->nRealHeight)
	{
		fScale = 1;
		return FALSE;
	}
	else
	{
		fScale = pTexture->nRealHeight / (float)pTexture->nNeedHeight;
		return TRUE;
	}
}

inline void SetViewPort(Common_DeviceDX10* pDevice,int nLeft,int nTop,int nRight,int nBottom)
{
	//进行一个正向判定，由于某些显卡不支持负值，所以此处遇到负值直接归零
	if (nLeft < 0)
	{
		nLeft = 0;
	}
	if (nTop < 0)
	{
		nTop = 0;
	}
	if (nRight < 0)
	{
		nRight = 0;
	}
	if (nBottom < 0)
	{
		nBottom = 0;
	}
	pDevice->rtViewport.left = nLeft;
	pDevice->rtViewport.right = nRight;
	pDevice->rtViewport.top = nTop;
	pDevice->rtViewport.bottom = nBottom;
	float fScale = 1;
	IsNeedScaleTexParam(pDevice,pDevice->hNowRenderTarget,fScale);
	D3D10_VIEWPORT viewPort = {nLeft * fScale,nTop * fScale,(nRight - nLeft) * fScale,(nBottom - nTop) * fScale,0,1};
	pDevice->pDevice->RSSetViewports(1,&viewPort);
}
inline void SetScissor(Common_DeviceDX10* pDevice,const RECT* pRect)
{
	pDevice->rtScissor = *pRect;
	float fScale = 1;
	IsNeedScaleTexParam(pDevice,pDevice->hNowRenderTarget,fScale);
	RECT rt; 
	rt.left = pRect->left * fScale; 
	rt.right = pRect->right * fScale; 
	rt.top = pRect->top * fScale; 
	rt.bottom = pRect->bottom * fScale; 

	pDevice->pDevice->RSSetScissorRects(1,&rt);
}

inline ID3D10Texture2D* getTexture(Common_DeviceDX10* pDevice,int hTexture)
{
	BaseGraphTexture* pSrcWDTexture = (BaseGraphTexture*)BseeHTable_GetAt(&pDevice->httexture,hTexture);
	if (!pSrcWDTexture)
		return NULL;
	return (ID3D10Texture2D*)pSrcWDTexture->pTexture;
}

inline void getTextureInfo(Common_DeviceDX10* pDevice,int hTexture,int* pWidth,int* pHeight,int* pMipMap)
{
	if(hTexture <= 0)
		return;
	BaseGraphTexture* pSrcWDTexture = (BaseGraphTexture*)BseeHTable_GetAt(&pDevice->httexture,hTexture);
	ID3D10Texture2D* pTexture = (ID3D10Texture2D*)(pSrcWDTexture->pTexture);
	D3D10_TEXTURE2D_DESC desc;
	pTexture->GetDesc(&desc);
	if(pWidth)
		*pWidth = pSrcWDTexture->nNeedWidth;
	if(pHeight)
		*pHeight = pSrcWDTexture->nNeedHeight;
	if(pMipMap)
		*pMipMap = desc.MipLevels;
}

inline void SetRenderTarget(Common_DeviceDX10* pDevice,int hRT,int hDepth)
{
	ID3D10RenderTargetView* pRenderTarget = NULL;
	ID3D10DepthStencilView* pDepth = NULL;
	if (hRT == pDevice->hNowRenderTarget && hDepth == pDevice->hNowDepth)
	{
		return;
	}
	if (hRT > 0)
	{
		BaseGraphTexture* pTexture = (BaseGraphTexture*)BseeHTable_GetAt(&pDevice->httexture,hRT);
		pTexture->bRecentRT = TRUE;
		pRenderTarget = (ID3D10RenderTargetView*)pTexture->pRenderTarget;
	}

	if (hDepth > 0)
	{
		BaseGraphTexture* pTexture = (BaseGraphTexture*)BseeHTable_GetAt(&pDevice->httexture,hDepth);
		pDepth = (ID3D10DepthStencilView*)pTexture->pRenderTarget;
	}
	pDevice->pDevice->OMSetRenderTargets(1,&pRenderTarget,pDepth);
	int nWidth,nHeight;

	pDevice->hNowRenderTarget = hRT;
	//if(hDepth > 0)
	pDevice->hNowDepth = hDepth;

	if(hRT == 0)
		return;
	getTextureInfo(pDevice,hRT,&nWidth,&nHeight,NULL);
	SetViewPort(pDevice,0,0,nWidth,nHeight);
	//SetScissor(pDevice,&pDevice->rtViewport);
}

inline BOOL SetShader(Common_DeviceDX10* pDevice,int nShaderType,int hShader);
inline BOOL SetShaderTexture(Common_DeviceDX10* pDevice,int hTex,DXShaderType nShaderType,int nRegIndex);
inline void  BeginRenderTexture(Common_DeviceDX10* pDevice)
{
	int nWidth,nHeight;
	getTextureInfo(pDevice,pDevice->hNowRenderTarget,&nWidth,&nHeight,NULL);

	float halfWidth = nWidth / 2.f;
	float halfHeight = nHeight / 2.f;
	// Create the projection matrix using the values in the viewport
	CalProjectMat(&pDevice->spriteProj,
		pDevice->rtViewport.left - halfWidth,
		pDevice->rtViewport.right - halfWidth ,
		halfHeight - pDevice->rtViewport.bottom ,
		halfHeight - pDevice->rtViewport.top,0.1f,10,Common_ORTHO_PROJ);
	memcpy(pDevice->pSpritState,pDevice->pNowState,DXRenderState_Count * sizeof(int));
	SetRenderState(pDevice,DXBlendState,DXBlend_Mix);
	SetRenderState(pDevice,DXDepthState,DXDepthDisable);
	SetRenderState(pDevice,DXRasterizerState,DXRas_Solid_NoCullBack_NoMultiSample);
	SetSamplerState(pDevice,DXSample_Line_Clamp,DXShaderType_PixelShader,0);
}

inline void  EndRenderTexture(Common_DeviceDX10* pDevice)
{
	for (int i = 0; i < DXRenderState_Count; i++)
	{
		SetRenderState(pDevice,i,pDevice->pSpritState[i]);
	}
}

inline bool SetDepthState(Common_DeviceDX10* pDevice,int nState)
{
	if (nState == DXDepthState_Default)
		pDevice->pDevice->OMSetDepthStencilState(NULL,0);
	else if(nState >= DXDepthDisable_WriteMask_01 && nState <= DXDepthDisable_WriteMask_FF)
		pDevice->pDevice->OMSetDepthStencilState(pDevice->pDepthState[nState],nState - DXDepthDisable_WriteMask_01 + 1);
	else
		pDevice->pDevice->OMSetDepthStencilState(pDevice->pDepthState[nState],0);
	return true;
}
inline bool SetBlendState(Common_DeviceDX10* pDevice,int nState)
{
	static float blend[4];
	if(nState == DXBlendState_Default)
		pDevice->pDevice->OMSetBlendState(NULL,blend,0xffffffff);
	else
		pDevice->pDevice->OMSetBlendState(pDevice->pBlendState[nState],blend,0xffffffff);
	return true;
}
inline bool SetRasterizerState(Common_DeviceDX10* pDevice,int nState)
{
	if(nState == DXRasterizer_Default)
		pDevice->pDevice->RSSetState(NULL);
	else
		pDevice->pDevice->RSSetState(pDevice->pRasterizerState[nState]);
	return true;
}
inline bool SetRenderState(Common_DeviceDX10* pDevice,int nStateType,int nState)
{
	if (pDevice->pNowState[nStateType] == nState)
	{
		return true;
	}
	switch(nStateType)
	{
	case DXDepthState:
		SetDepthState(pDevice,nState);
		break;
	case DXBlendState:
		SetBlendState(pDevice,nState);
		break;
	case DXRasterizerState:
		SetRasterizerState(pDevice,nState);
		break;
	default:
		return false;
	}
	pDevice->pNowState[nStateType] = nState;
	return true;
}

inline bool SetSamplerState(Common_DeviceDX10* pDevice,int nState,int nType,int nSampler)
{
	if (pDevice->pNowSamplerState[nType][nSampler] == nState)
	{
		return true;
	}
	ID3D10SamplerState* pSampleState;
	if(nState == DXSample_Default)
		pSampleState = NULL;
	else
		pSampleState = pDevice->pSamplerState[nState];
	switch(nType)
	{
	case DXShaderType_VertexShader:
		pDevice->pDevice->VSSetSamplers(nSampler,1,&pSampleState);
		break;;
	case DXShaderType_GeometryShader:
		pDevice->pDevice->GSSetSamplers(nSampler,1,&pSampleState);
		break;;
	case DXShaderType_PixelShader:
		pDevice->pDevice->PSSetSamplers(nSampler,1,&pSampleState);
		break;
	default:
		return false;
	}
	pDevice->pNowSamplerState[nType][nSampler] = nState;
	return true;
}
inline BOOL SetShader(Common_DeviceDX10* pDevice,int nShaderType,int hShader)
{
	if (pDevice->pShader[nShaderType] == hShader)
	{
		return TRUE;
	}

	BaseGraphShader* pShader = (BaseGraphShader*)BseeHTable_GetAt(&pDevice->htShaderTable,hShader);
	switch(nShaderType)
	{
	case DXShaderType_VertexShader:
		pDevice->pDevice->VSSetShader(pShader ? (ID3D10VertexShader*)pShader->pShader : NULL);
		pDevice->pDevice->IASetInputLayout(pShader->pLayout);
		break;
	case DXShaderType_GeometryShader:
		pDevice->pDevice->GSSetShader(pShader ? (ID3D10GeometryShader*)pShader->pShader : NULL);
		break;
	case DXShaderType_PixelShader:
		pDevice->pDevice->PSSetShader(pShader ? (ID3D10PixelShader*)pShader->pShader : NULL);
		break;
	default:
		return FALSE;
	}
	pDevice->pShader[nShaderType] = hShader;
	return TRUE;
}

inline BOOL SetIndexBuffer(Common_DeviceDX10* pDevice,int hIndexBuffer)
{
	if (pDevice->hNowIndexBuffer == hIndexBuffer)
	{
		return TRUE;
	}
	CommonBuffer* pIB = (CommonBuffer*)BseeHTable_GetAt(&pDevice->htBuffer,hIndexBuffer);
	pDevice->pDevice->IASetIndexBuffer(pIB->pBuffer,DXGI_FORMAT_R32_UINT,0);
	pDevice->hNowIndexBuffer = hIndexBuffer;
	return TRUE;
}

inline BOOL SetVertexBuffer(Common_DeviceDX10* pDevice,int hVertexBuffer)
{
	if (pDevice->hNowVertexBuffer == hVertexBuffer)
	{
		return TRUE;
	}
	CommonBuffer* pVB = (CommonBuffer*)BseeHTable_GetAt(&pDevice->htBuffer,hVertexBuffer);
	UINT offsets[1] = {0};
	UINT strides[1] = { pVB->nVertexSize };
	pDevice->pDevice->IASetVertexBuffers(0,1,&pVB->pBuffer,strides,offsets);
	pDevice->hNowVertexBuffer = hVertexBuffer;
	return TRUE;
}

inline BOOL SetVertexInstanceBuffer(Common_DeviceDX10* pDevice,int hVertexBuffer,int hInstanceBuffer)
{
	CommonBuffer* pVB0 = (CommonBuffer*)BseeHTable_GetAt(&pDevice->htBuffer,hVertexBuffer);
	CommonBuffer* pVB1 = (CommonBuffer*)BseeHTable_GetAt(&pDevice->htBuffer,hInstanceBuffer);

	ID3D10Buffer* vb[] = {pVB0->pBuffer,pVB1->pBuffer};
	UINT strides[] = {pVB0->nVertexSize, pVB1->nVertexSize};
	UINT offsets[] = {0,0};

	pDevice->pDevice->IASetVertexBuffers(0,2,vb,strides,offsets);
	pDevice->hNowVertexBuffer = hVertexBuffer;
	return TRUE;
}

inline BOOL SetShaderTexture(Common_DeviceDX10* pDevice,int hTex,DXShaderType nShaderType,int nRegIndex)
{
	BaseGraphTexture* pTexture = (BaseGraphTexture*)BseeHTable_GetAt(&pDevice->httexture,hTex);
	ID3D10ShaderResourceView* pView = NULL;
	if(pTexture)
	{
		pView = pTexture->pResourceView;
		pTexture->bRecentRT = FALSE;
	}

	switch(nShaderType)
	{
	case DXShaderType_VertexShader:
		pDevice->pDevice->VSSetShaderResources(nRegIndex,1,&pView);
		break;
	case DXShaderType_GeometryShader:
		pDevice->pDevice->GSSetShaderResources(nRegIndex,1,&pView);
		break;  
	case DXShaderType_PixelShader:
		pDevice->pDevice->PSSetShaderResources(nRegIndex,1,&pView);
		break;
	}
	pDevice->nShaderTexstate[nShaderType][nRegIndex] = hTex;
	return true;
}
#endif // BaseGraphRenderEngineDX10Api_h__
