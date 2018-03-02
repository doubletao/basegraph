#include "stdafx.h"
#include "SceneRender\DXRenderEngineSoftAPI.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

htable g_SoftDeviceHT;
TCHAR g_szPathEffect[MAX_PATH] = {0};	
CDeviceLock g_DeviceTableLock;
DWORD WINAPI soft_engine_mixthread(void* param)
{
	MixThreadParam* pparam = (MixThreadParam*)param;

	HANDLE hHandle[2];
	hHandle[1] = pparam->hStart;
	hHandle[0] = pparam->hStop;
	for(;;)
	{
		int nRet = WaitForMultipleObjects(2,hHandle,FALSE,-1);
		if(nRet == WAIT_FAILED ||nRet - WAIT_OBJECT_0 == 0)		//停止
		{
			SetEvent(pparam->hComplete);
			return 1;
		}

		BufferFunction::BufferBitblt
		(
			pparam->des, 
			pparam->desx, 
			pparam->desy, 
			pparam->srccx, 
			pparam->srccy, 
			pparam->des_buf_width,
			pparam->des_buf_height,
			pparam->src, 
			pparam->srcx, 
			pparam->srcy, 
			pparam->srccx, 
			pparam->srccy, 
			pparam->src_buf_width,
			pparam->src_buf_height,
			pparam->eMode,
			BUFFER_PARITY_N_K,
			pparam->pCull,
			MULTI_THREAD_COUNT,
			pparam->threadid);
		SetEvent(pparam->hComplete);
	}
	return 1;
}
//////////////////////////////////////////////////////////////////////////

void SYDeviceContex::SetViewPort(int nLeft,int nTop,int nRight,int nBottom)
{
	rtViewport.left = nLeft;
	rtViewport.right = nRight;
	rtViewport.top = nTop;
	rtViewport.bottom = nBottom;
}
void SYDeviceContex::SetRenderTarget(SY_DeviceSoft* pDevice,int hRT,int hDepth)
{
	if (hRT == hNowRenderTarget && hDepth == hNowDepth)
		return;

	int nWidth,nHeight;
	hNowRenderTarget = hRT;
	hNowDepth = hDepth;

	if(hRT == 0)
		return;
	CSYBuffer* pTexture = getTexture(pDevice,hRT);
	getTextureInfo(pTexture,&nWidth,&nHeight);
	SetViewPort(0,0,nWidth,nHeight);
}

void  SYDeviceContex::BeginRenderTexture()
{
	memcpy(pSpritState,pNowState,DXRenderState_Count * sizeof(int));
	SetRenderState(DXBlendState,DXBlend_Mix);
	SetRenderState(DXDepthState,DXDepthDisable);
	SetRenderState(DXRasterizerState,DXRas_Solid_NoCullBack_NoMultiSample);
	SetSamplerState(DXSample_Line_Clamp,DXShaderType_PixelShader,0);
}

void  SYDeviceContex::EndRenderTexture()
{
	for (int i = 0; i < DXRenderState_Count; i++)
		SetRenderState(i,pSpritState[i]);
}

bool SYDeviceContex::SetRenderState(int nStateType,int nState)
{
	pNowState[nStateType] = nState;
	return true;
}

bool SYDeviceContex::SetSamplerState(int nState,int nType,int nSampler)
{
	pNowSamplerState[nType][nSampler] = nState;
	return true;
}
BOOL SYDeviceContex::SetShader(int nShaderType,int hShader)
{
	pShader[nShaderType] = hShader;
	return TRUE;
}

BOOL SYDeviceContex::SetIndexBuffer(int hIndexBuffer)
{
	hNowIndexBuffer = hIndexBuffer;
	return TRUE;
}

BOOL SYDeviceContex::SetVertexBuffer(int hVertexBuffer)
{
	hNowVertexBuffer = hVertexBuffer;
	return TRUE;
}

BOOL SYDeviceContex::SetShaderTexture(int hTex,DXShaderType nShaderType,int nRegIndex)
{
	nShaderTexstate[nShaderType][nRegIndex] = hTex;
	return true;
}

void SYDeviceContex::RenderMix(DWORD* pdwDesBuffer,		// 目标
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
	RECT* pCull)	// 目标裁减矩形（可选项，为 NULL 时表示目标全部有效）)
{
	DXSingleLock lock(ContexLock);
	for(int i =0; i < MULTI_THREAD_COUNT;i ++)
	{
		mix_threadparam[i].des = pdwDesBuffer;
		mix_threadparam[i].desx = nDesX;
		mix_threadparam[i].desy = nDesY;
		mix_threadparam[i].des_buf_width = nDesBufferWidth;
		mix_threadparam[i].des_buf_height = nDesBufferHeight;
		mix_threadparam[i].src = pdwSrcBuffer;
		mix_threadparam[i].srcx = nSrcX;
		mix_threadparam[i].srcy = nSrcY;

		mix_threadparam[i].srccx = nSrcWidth;
		mix_threadparam[i].srccy = nSrcHeight;
		mix_threadparam[i].src_buf_width = nSrcBufferWidth;
		mix_threadparam[i].src_buf_height = nSrcBufferHeight;
		mix_threadparam[i].pCull = pCull;
		mix_threadparam[i].eMode = eMode;

		SetEvent(mix_threadparam[i].hStart);
	}
	for(int i = 0; i < MULTI_THREAD_COUNT;i ++)
	{
		WaitForSingleObject(mix_threadparam[i].hComplete,-1);
	}
}
//////////////////////////////////////////////////////////////////////////
//contex 实现
//////////////////////////////////////////////////////////////////////////
SYDeviceContex::SYDeviceContex()
{
	hNowRenderTarget = 0;
	hNowDepth = 0;
	nNowPriType = -1;
	hNowIndexBuffer = 0;
	hNowVertexBuffer = 0;
	memset(pNowState,0,sizeof(pNowState));
	memset(pSpritState,0,sizeof(pSpritState));
	memset(pNowSamplerState,0,sizeof(pNowSamplerState));
	memset(pShader,0,sizeof(pShader));
	memset(pConstBuffer,0,sizeof(pConstBuffer));
	memset(nShaderTexstate,0,sizeof(nShaderTexstate));
	memset(&rtViewport,0,sizeof(rtViewport));
	memset(&rtScissor,0,sizeof(rtScissor));

	//initialize multi thread mix
	for(int i = 0 ;i < MULTI_THREAD_COUNT;i ++)
	{
		mix_threadparam[i].threadid = i;
		mix_threadparam[i].hStart = CreateEvent(NULL,FALSE,FALSE,NULL);
		mix_threadparam[i].hStop = CreateEvent(NULL,FALSE,FALSE,NULL);
		mix_threadparam[i].hComplete = CreateEvent(NULL,FALSE,FALSE,NULL);
		mix_threadparam[i].hThread = CreateThread(NULL,0,soft_engine_mixthread,mix_threadparam + i,0,NULL);
	}
}

SYDeviceContex::~SYDeviceContex()
{
	for(int i = 0; i< MULTI_THREAD_COUNT;i ++)
	{
		SetEvent(mix_threadparam[i].hStop);
		WaitForSingleObject(mix_threadparam[i].hThread,-1);
		CloseHandle(mix_threadparam[i].hThread);
		CloseHandle(mix_threadparam[i].hStart);
		CloseHandle(mix_threadparam[i].hStop);
		mix_threadparam[i].hThread = NULL;
	}
}

const int g_pLayoutNum[] = 
{
	2,
	3,
	3,
	5,
	1,
	10,
};

//////////////////////////////////////////////////////////////////////////
inline int CreateBuffer(SY_DeviceSoft* pDevice,int nSize,int nNum,void* pBuffer,DXBufferType nType,int nPriType = -1)
{
	SYSoftBuffer* pVideoBuffer = new SYSoftBuffer;
	pVideoBuffer->nType = nType;
	pVideoBuffer->nVertexSize = nSize;
	pVideoBuffer->nBufferSize = nSize* nNum;
	pVideoBuffer->pBuffer = new char[pVideoBuffer->nBufferSize];
	if (!pVideoBuffer->pBuffer)
	{
		assert(0);
		return 0;
	}
	int hBuffer = (int)SYHTable_Insert(&pDevice->htBuffer,pVideoBuffer);

	return hBuffer;
}

void SoftShaderHashRelease(void* pValue)
{
	shaderhash* hShader = (shaderhash*)pValue;
	SAFE_DELETE(hShader);

	return;
}

struct screenVertex
{
	float3	pos;
	float	tu;
	float	tv;
};

int Create2DVertex(SY_DeviceSoft* pDevice)
{
	screenVertex pSrceenVertex[6]; 
	pSrceenVertex[0].pos.x = 0;
	pSrceenVertex[0].pos.y = -1.f;
	pSrceenVertex[0].pos.z = 0.1f;
	pSrceenVertex[0].tu    = 0;
	pSrceenVertex[0].tv    = 1;

	pSrceenVertex[1].pos.x = 1.f;
	pSrceenVertex[1].pos.y = -1.f;
	pSrceenVertex[1].pos.z = 0.1f;
	//pSrceenVertex[1].tu    = 0.8023;
	pSrceenVertex[1].tu    = 1;
	pSrceenVertex[1].tv    = 1;

	pSrceenVertex[2].pos.x = 0;
	pSrceenVertex[2].pos.y = 0;
	pSrceenVertex[2].pos.z = 0.1f;
	pSrceenVertex[2].tu    = 0;
	pSrceenVertex[2].tv    = 0;

	pSrceenVertex[3].pos.x = 0;
	pSrceenVertex[3].pos.y = 0;
	pSrceenVertex[3].pos.z = 0.1f;
	pSrceenVertex[3].tu    = 0;
	pSrceenVertex[3].tv    = 0;

	pSrceenVertex[4].pos.x = 1.f;
	pSrceenVertex[4].pos.y = -1.f;
	pSrceenVertex[4].pos.z = 0.1f;
	pSrceenVertex[4].tu    = 1;
	pSrceenVertex[4].tv    = 1;

	pSrceenVertex[5].pos.x = 1.f;
	pSrceenVertex[5].pos.y = 0;
	pSrceenVertex[5].pos.z = 0.1f;
	pSrceenVertex[5].tu    = 1;
	pSrceenVertex[5].tv    = 0;

	return CreateBuffer(pDevice,sizeof(screenVertex),6,pSrceenVertex,DXVertexBuffer,TRIANGLELIST);
}
int CreateScreenVertex(SY_DeviceSoft* pDevice)
{
	screenVertex pSrceenVertex[6]; 
	pSrceenVertex[0].pos.x = -1.f;
	pSrceenVertex[0].pos.y = 1.f;
	pSrceenVertex[0].pos.z = 0.1f;
	pSrceenVertex[0].tu    = 0;
	pSrceenVertex[0].tv    = 0;

	pSrceenVertex[1].pos.x = 1.f;
	pSrceenVertex[1].pos.y = 1.f;
	pSrceenVertex[1].pos.z = 0.1f;
	//pSrceenVertex[1].tu    = 0.8023;
	pSrceenVertex[1].tu    = 1;
	pSrceenVertex[1].tv    = 0;

	pSrceenVertex[2].pos.x = -1.f;
	pSrceenVertex[2].pos.y = -1.f;
	pSrceenVertex[2].pos.z = 0.1f;
	pSrceenVertex[2].tu    = 0;
	pSrceenVertex[2].tv    = 1;

	pSrceenVertex[3].pos.x = -1.f;
	pSrceenVertex[3].pos.y = -1.f;
	pSrceenVertex[3].pos.z = 0.1f;
	pSrceenVertex[3].tu    = 0;
	pSrceenVertex[3].tv    = 1;

	pSrceenVertex[4].pos.x = 1.f;
	pSrceenVertex[4].pos.y = 1.f;
	pSrceenVertex[4].pos.z = 0.1f;
	pSrceenVertex[4].tu    = 1;
	pSrceenVertex[4].tv    = 0;

	pSrceenVertex[5].pos.x = 1.f;
	pSrceenVertex[5].pos.y = -1.f;
	pSrceenVertex[5].pos.z = 0.1f;
	pSrceenVertex[5].tu    = 1;
	pSrceenVertex[5].tv    = 1;

	return CreateBuffer(pDevice,sizeof(screenVertex),6,pSrceenVertex,DXVertexBuffer,TRIANGLELIST);
}

bool Render_init(void* paramin,void* paramout)
{
	LPCTSTR lpPathShader = (LPCTSTR)paramin;

	_tcscpy_s(g_szPathEffect,lpPathShader);
	_tcscat_s(g_szPathEffect,_T("\\Effect"));

	SYHTable_Init(&g_SoftDeviceHT);

	return true;
}

void Soft_ReleaseTexture(SYSoftTexture* pWDTexture)
{
	SAFEDELETE_ARRAY(pWDTexture->pTexFile);

	CSYBuffer* pTexture = pWDTexture->pTexture;
	SAFE_DELETE(pTexture);
	SAFE_DELETE(pWDTexture);
}

void Render_ReleaseDevice(SY_DeviceSoft* pDevice)
{
	if (!pDevice )
		return;
	UINT firstSwapChain = SYHTable_GetFirstValid(&pDevice->htSwapChain);		
	SYSoftSwapChain* pSwapTexture = (SYSoftSwapChain*)SYHTable_GetAt(&pDevice->htSwapChain,firstSwapChain);
	while(pSwapTexture)
	{
		SAFE_DELETE(pSwapTexture->pTexture);
		SAFE_DELETE(pSwapTexture);
		pSwapTexture = (SYSoftSwapChain*)SYHTable_GetNextValid(&pDevice->htSwapChain,&firstSwapChain);
	}

	UINT firstTexture = SYHTable_GetFirstValid(&pDevice->httexture);		
	SYSoftTexture* pTexture = (SYSoftTexture*)SYHTable_GetAt(&pDevice->httexture,firstTexture);
	while(pTexture)
	{
		Soft_ReleaseTexture(pTexture);
		pTexture = (SYSoftTexture*)SYHTable_GetNextValid(&pDevice->httexture,&firstTexture);
	}

	UINT nShdCount = SYHTable_GetCount(&pDevice->htShaderTable);
	UINT firstShade = SYHTable_GetFirstValid(&pDevice->htShaderTable);		
	while(firstShade)
	{
		//ReleaseShader(pDevice,firstShade);
		SYHTable_GetNextValid(&pDevice->htShaderTable,&firstShade);
	}
	
	UINT nBufCount = SYHTable_GetCount(&pDevice->htBuffer);
	UINT firstBuf = SYHTable_GetFirstValid(&pDevice->htBuffer);		
	while(firstBuf)
	{
		int a = 0;
		ReleaseBuffer(pDevice,firstBuf);
		SYHTable_GetNextValid(&pDevice->htBuffer,&firstBuf);
	}

	assert(nShdCount <= 0 && nBufCount <= 0);
	
	SYHash_Delete(pDevice->ShaderHash,TRUE);
	SYHash_Delete(pDevice->TextureFromFileHash,TRUE);
	SYHash_Delete(pDevice->BumpTextureFromFileHash,TRUE);
	
	SYHTable_Release(&pDevice->httexture);
	SYHTable_Release(&pDevice->htModel);
	SYHTable_Release(&pDevice->htShaderTable);
	SYHTable_Release(&pDevice->htBuffer);
	SYHTable_Release(&pDevice->htSwapChain);
	SAFE_DELETE(pDevice);
}

bool Render_Release(void* paramin,void* paramout)
{
	UINT firstDevice = SYHTable_GetFirstValid(&g_SoftDeviceHT);		
	SY_DeviceSoft* pNowDevice = (SY_DeviceSoft*)SYHTable_GetAt(&g_SoftDeviceHT,firstDevice);
	do 
	{
		Render_ReleaseDevice(pNowDevice);
		pNowDevice = (SY_DeviceSoft*)SYHTable_GetNextValid(&g_SoftDeviceHT,&firstDevice);
	} while (pNowDevice);

	SYHTable_Release(&g_SoftDeviceHT);
	return true;
}


bool Render_CreateDevice(void* paramin,void* paramout)
{
	BOOL* pDoubleDevice = (BOOL*)paramin;

	SY_DeviceSoft* pNewDevice = NULL;
	pNewDevice = new SY_DeviceSoft;

	SYHTable_Init(&pNewDevice->httexture);
	SYHTable_Init(&pNewDevice->htModel);
	SYHTable_Init(&pNewDevice->htShaderTable);
	SYHTable_Init(&pNewDevice->htSwapChain);
	SYHTable_Init(&pNewDevice->htContexTable);

	pNewDevice->ShaderHash = SYHash_Create(1,NULL,NULL, NULL, NULL, SoftShaderHashRelease);
	pNewDevice->TextureFromFileHash = SYHash_Create(1,NULL,NULL, NULL, NULL, SoftShaderHashRelease);
	pNewDevice->BumpTextureFromFileHash = SYHash_Create(1,NULL,NULL, NULL, NULL, SoftShaderHashRelease);

	int hNewDevice = (int)SYHTable_Insert(&g_SoftDeviceHT,pNewDevice);
	*(int*)paramout = hNewDevice;
	return true;
}

bool Render_GetSwapChainDesc(SY_DeviceSoft* pDevice,void* paramin,void* paramout)
{
	createSwapChainIn* pIn = (createSwapChainIn*)paramin;
	createSwapChainIn* pOut = (createSwapChainIn*)paramout;
	SYSoftSwapChain* pWDSwapChain = (SYSoftSwapChain*)SYHTable_GetAt(&pDevice->htSwapChain,(int)pIn->hWnd);
	if(pWDSwapChain)
	{
		pOut->bWindowed = FALSE;
		pOut->nWidth = pWDSwapChain->pTexture->m_nWidth;
		pOut->nHeight = pWDSwapChain->pTexture->m_nHeight;
	
		return true;
	}
	return false;
}

bool Render_CreateSwapChain(SY_DeviceSoft* pDevice,void* paramin,void* paramout)
{
	createSwapChainIn* pIn = (createSwapChainIn*)paramin;

	SYSoftSwapChain* pWDSwapChain = new SYSoftSwapChain;
	memset(pWDSwapChain,0,sizeof(SYSoftSwapChain));
	pWDSwapChain->pTexture = new CSYBuffer(pIn->nWidth,pIn->nHeight);
	int hNewSwapChain = (int)SYHTable_Insert(&pDevice->htSwapChain,pWDSwapChain);
	*(int*)paramout = hNewSwapChain;
	return true;
}

bool render_DumpTexture(int hDevice,void* paramin,void* paramout);

bool Render_SetRenderTarget(SY_DeviceSoft* pDevice, SYDeviceContex* pDeviceContext,void* paramin,void* paramout)
{
	setRenderTarget* param = (setRenderTarget*)paramin;
	pDeviceContext->SetRenderTarget(pDevice,param->hRenderTarget,param->hDepth);
	return TRUE;
}

bool Render_CreateTexture(SY_DeviceSoft* pDevice,void* paramin,void* paramout)
{
	createTextureIn* param = (createTextureIn*)paramin;
	CSYBuffer* pNewTexture = new CSYBuffer(param->nWidth,param->nHeight);
	if(!pNewTexture)
	{
		*(int*)paramout = 0;
		return false;
	}
	SYSoftTexture* pWDSwapChain = new SYSoftTexture;
	memset(pWDSwapChain,0,sizeof(SYSoftTexture));
	pWDSwapChain->pTexture = pNewTexture;
	pWDSwapChain->nType = param->nType;

	int hNewSwapChain = (int)SYHTable_Insert(&pDevice->httexture,pWDSwapChain);
	*(int*)paramout = hNewSwapChain;
	return true;
}

bool render_ClearTexture(SY_DeviceSoft* pDevice,void* paramin,void* paramout)
{
	clearTextureIn* param = (clearTextureIn*)paramin;
	CSYBuffer* pBuffer = getTexture(pDevice,param->hTexture);
	pBuffer->Clear(*(DWORD*)param->pData,NULL);
	return true;
}

bool render_GetTextureInfo(SY_DeviceSoft* pDevice,void* paramin,void* paramout)
{
	SYSoftTexture* pSrcWDTexture = (SYSoftTexture*)SYHTable_GetAt(&pDevice->httexture,*(int*)paramin);
	getTextureInfoOut* pOut = (getTextureInfoOut*)paramout;
	
	CSYBuffer* pDesTexture = (CSYBuffer*)pSrcWDTexture->pTexture;
	getTextureInfo(pDesTexture,pOut->nWidth,pOut->nHeight); 
	if (pOut->nType)
		*pOut->nType = pSrcWDTexture->nType;
	if(pOut->nMipMap)
		*pOut->nMipMap = 1;
	return true;
}


bool Render_CopyTexture(SY_DeviceSoft* pDevice,SYDeviceContex* pDeviceContext,void* paramin,void* paramout)
{
	copyTexture* param = (copyTexture*)paramin;
	DXSingleLock resLock(pDevice->ResLock,false);
	resLock.Lock();
	CSYBuffer* pSrcTexture = getTexture(pDevice,param->hSrcTex);
	CSYBuffer* pDestTexture = getTexture(pDevice,param->hDesTex);
	resLock.UnLock();

	pDeviceContext->RenderMix(pDestTexture->m_dwBuffer,param->nDesOffsetX,param->nDesOffsetY,param->nDesWidth,param->nDesHeight,pDestTexture->m_nWidth,pDestTexture->m_nHeight,
			pSrcTexture->m_dwBuffer,param->nSrcOffsetX,param->nSrcOffsetY,param->nSrcWidth,param->nSrcHeight,pSrcTexture->m_nWidth,pSrcTexture->m_nHeight,BUFFER_COPY,NULL);
	return true;
}
bool render_DumpTexture(SY_DeviceSoft* pDevice,void* paramin,void* paramout)
{
	LPCTSTR szFile = ((DXDumpIn*)paramin)->szFile;
	int hTexture = ((DXDumpIn*)paramin)->hTexture;
	CSYBuffer* pTexture = getTexture(pDevice,hTexture);
	//CString path(L"e:\\testTex.bmp");
	static const TCHAR* path = _T("e:\\testTex.TGA");
	BufferFunction::WriteBitmap(szFile == NULL ? path : szFile, pTexture->m_nWidth, pTexture->m_nHeight, pTexture->m_nPitch, (BYTE *)pTexture->m_dwBuffer);
	return true;
}
bool Render_ReleaseSwapChain(SY_DeviceSoft* pDevice,void* paramin,void* paramout)
{
	SYSoftSwapChain* pSrcWDTexture = (SYSoftSwapChain*)SYHTable_RemoveAt(&pDevice->htSwapChain,*(int*)paramin);

	SAFE_DELETE(pSrcWDTexture->pTexture);
	SAFE_DELETE(pSrcWDTexture);

	return true;
}

bool RenderSoft_ReleaseTexture(SY_DeviceSoft* pDevice,void* paramin,void* paramout)
{
	SYSoftTexture* pSrcWDTexture = (SYSoftTexture*)SYHTable_GetAt(&pDevice->httexture,*(int*)paramin);
	assert(pSrcWDTexture);
	if (pSrcWDTexture&&pSrcWDTexture->pTexFile)//有pSrcWDTexture == NULL的时候
	{
		SYHash*	pHash = NULL;		
		if(pSrcWDTexture->nType == DXTexture_2DDumpReadOnly)
			pHash = pDevice->BumpTextureFromFileHash;
		else if(pSrcWDTexture->nType == DXTexture_2DReadOnly)
			pHash = pDevice->TextureFromFileHash;

		HandleHash* pTexHashValue = (HandleHash*)SYHash_Search(pHash,(void *)pSrcWDTexture->pTexFile);
		--(pTexHashValue->nCount);
		if (pTexHashValue->nCount != 0)
		{
			return true;
		}
		SYHash_RemoveAt(pHash,(void *)pSrcWDTexture->pTexFile,TRUE);
		Soft_ReleaseTexture(pSrcWDTexture);
	}
	else if(pSrcWDTexture)
	{
		Soft_ReleaseTexture(pSrcWDTexture);
	}
	
	//Soft_ReleaseTexture(pSrcWDTexture);//有pSrcWDTexture == NULL的时候
	SYHTable_RemoveAt(&pDevice->httexture,*(int*)paramin);
	return true;

}

bool Render_BeginTex(SYDeviceContex* pDeviceContext,void* paramin,void* paramout)
{
	pDeviceContext->BeginRenderTexture();
	return true;
}


bool Render_RenderTex(SY_DeviceSoft* pDevice,SYDeviceContex* pDeviceContext,void* paramin,void* paramout)
{
	renderTextureIn* pIn = (renderTextureIn*)paramin;

	CSYBuffer* pSrcTex = getTexture(pDevice,pIn->hTex);
	int nSrcWidth,nSrcHeight;
	getTextureInfo(pSrcTex,&nSrcWidth,&nSrcHeight);

 	CSYBuffer* pRTTex = getTexture(pDevice,pDeviceContext->GetRenderTarget());
 	int nRTWidth,nRTHeight;
	getTextureInfo(pRTTex,&nRTWidth,&nRTHeight);
	
	BUFFER_MODE mode = BUFFER_COPY; 
	if(pDeviceContext->GetRenderState(DXBlendState) == DXBlend_Mix)
		mode = BUFFER_MIX;
	else if(pDeviceContext->GetRenderState(DXBlendState) == DXBlend_Mix)
		mode = BUFFER_MIX;
	else if(pDeviceContext->GetRenderState(DXBlendState) == DXBlend_Add)
		mode = BUFFER_LINEAR_DODGE;

	RECT clearRT;
	IntersectRect(&clearRT,&pDeviceContext->GetViewPort(),&pDeviceContext->GetScissor());
	BUFFER_PARITY_STYLE style;
	switch(pIn->nField)
	{
	case BOTH_FIELD:
		style = BUFFER_PARITY_BOTH;
		break;
	case FIRST_FIELD:
		style = BUFFER_PARITY_ODD;
		break;
	case SECOND_FIELD:
		style = BUFFER_PARITY_EVEN;
		break;
	}
	//pRTTex->BitBlt((RECT*)pIn->pDstRt,pSrcTex,(RECT*)pIn->pSrcRt,mode,style,&clearRT);
	if(pIn->nField == BOTH_FIELD)
	pDeviceContext->RenderMix(pRTTex->m_dwBuffer,pIn->pDstRt->left,pIn->pDstRt->top,pIn->pDstRt->right - pIn->pDstRt->left,pIn->pDstRt->bottom - pIn->pDstRt->top,pRTTex->m_nWidth,pRTTex->m_nHeight,
			pSrcTex->m_dwBuffer,pIn->pSrcRt->left,pIn->pSrcRt->top,pIn->pSrcRt->right - pIn->pSrcRt->left,pIn->pSrcRt->bottom - pIn->pSrcRt->top,pSrcTex->m_nWidth,pSrcTex->m_nHeight,mode,&clearRT);
	else
	{
		BOOL bOddStart = pIn->pDstRt->top %2 ;
		BOOL bOddHeight = (pIn->pDstRt->bottom - pIn->pDstRt->top) % 2;
		assert(bOddStart == pIn->pSrcRt->top %2);
		assert((pIn->pDstRt->bottom - pIn->pDstRt->top) == (pIn->pDstRt->bottom - pIn->pSrcRt->top ));
		int nDesXOffSet = (pIn->nField == SECOND_FIELD) ? 0 : pRTTex->m_nWidth;
		int nSrcXOffSet = (pIn->nField == SECOND_FIELD) ? 0 : pSrcTex->m_nWidth;
		int nDesYOffset = pIn->pDstRt->top / 2;
		int nSrcYOffset = pIn->pSrcRt->top / 2;
		if(pIn->nField == SECOND_FIELD)
		{
			clearRT.top /= 2;
			clearRT.bottom = clearRT.bottom/2 + 1;
		}
		else
		{
			clearRT.top /= 2;
			clearRT.bottom = clearRT.bottom/2 + 1;
			clearRT.left += pRTTex->m_nWidth;
			clearRT.right += pRTTex->m_nWidth;
		}
		
		
		if(bOddStart && (pIn->nField == SECOND_FIELD))
		{
			nDesYOffset ++;
			nSrcYOffset ++;
		}
		int nDesHeight = (pIn->pDstRt->bottom - pIn->pDstRt->top) / 2;
		int nSrcHeight = (pIn->pSrcRt->bottom - pIn->pSrcRt->top) / 2;
		if((pIn->nField == FIRST_FIELD && bOddStart && bOddHeight)
			||	(pIn->nField == SECOND_FIELD && !bOddStart && bOddHeight)
			)
		{
			nDesHeight ++;
			nSrcHeight ++;
		}
		pDeviceContext->RenderMix(pRTTex->m_dwBuffer,nDesXOffSet + pIn->pDstRt->left,nDesYOffset,
			pIn->pDstRt->right - pIn->pDstRt->left,nDesHeight,pRTTex->m_nWidth * 2,pRTTex->m_nHeight /2,
			pSrcTex->m_dwBuffer,nSrcXOffSet + pIn->pSrcRt->left,nSrcYOffset,
			pIn->pSrcRt->right - pIn->pSrcRt->left,nSrcHeight,pSrcTex->m_nWidth * 2,pRTTex->m_nHeight / 2,
			mode,&clearRT);
	}
	return true;
}
bool Render_GetRenderTarget(SYDeviceContex* pDeviceContext,void* paramin,void* paramout)
{
	getRenderTargetIn* pIn = (getRenderTargetIn*)paramin;
	if(pIn->phRT)
		*pIn->phRT = pDeviceContext->GetRenderTarget();
	if (pIn->phDepth)
		*pIn->phDepth = pDeviceContext->GetDepth();
	return true;
}

BOOL Render_EngineLock(int hDevice,void* paramin,void* paramout);
bool Render_UpLoad(SY_DeviceSoft* pDevice,SYDeviceContex* pDeviceContext,void* paramin,void* paramout)
{
	uploadBufferIn* pIn = (uploadBufferIn*)paramin;

	DXSingleLock resLock(pDevice->ResLock,false);
	resLock.Lock();
	CSYBuffer* pDstTexture = getTexture(pDevice,pIn->hDestTex);
	resLock.UnLock();
	pDeviceContext->RenderMix(pDstTexture->m_dwBuffer,pIn->destOffsetX,pIn->destOffsetY,pIn->srcWidth,pIn->srcHeight,pIn->srcWidth,pDstTexture->m_nHeight,
			pIn->srcBuffer,pIn->srcOffsetX,pIn->srcOffsetY,pIn->srcWidth,pIn->srcHeight,pIn->srcBufferWidth,pIn->srcBufferHeight,BUFFER_COPY,NULL);
	return true;
}
bool Render_EndTex(SYDeviceContex* pDeviceContext,void* paramin,void* paramout)
{
	pDeviceContext->EndRenderTexture();
	return true;
}

bool Render_SetViewPort(SYDeviceContex* pDeviceContext,void* paramin,void* paramout)
{
	RECT* pRect = (RECT*)paramin;
	pDeviceContext->SetViewPort(*pRect);
	return true;
}

bool Render_GetViewPort(SYDeviceContex* pDeviceContext,void* paramin,void* paramout)
{
	RECT* pRect = (RECT*)paramout;
	*pRect = pDeviceContext->GetViewPort();
	return true;
}

bool Render_SetRenderState(SYDeviceContex* pDeviceContext,void* paramin,void* paramout)
{
	DXRenderStateIn* param = (DXRenderStateIn*)paramin;
	pDeviceContext->SetRenderState(param->nType,param->nValue);
	return true;
}

BOOL Render_ClearScreen(SY_DeviceSoft* pDevice,SYDeviceContex* pDeviceContext,void* paramin,void* paramout)
{
	clearScreenIn* pParam = (clearScreenIn*)paramin;
	if(pParam->nType == DX_CLEAR_RENDERTARGET_ONLY || pParam->nType == DX_CLEAR_ALL)
	{
		RECT clearRT;
		IntersectRect(&clearRT,&pDeviceContext->GetViewPort(),&pDeviceContext->GetScissor());
		CSYBuffer* pBuffer = getTexture(pDevice,pDeviceContext->GetRenderTarget());
		pBuffer->Clear(pParam->nColor,&clearRT);
	}
	if(pDeviceContext->GetDepth() && (pParam->nType == DX_CLEAR_DEPTH_ONLY || pParam->nType == DX_CLEAR_ALL))
	{
		//clearTextureIn param = {pDevice->hNowDepth,&pParam->fZValue};
		//render_ClearTexture(hDevice,&param,NULL);
	}
	return TRUE;
}

BOOL Render_GetRenderState(SYDeviceContex* pDeviceContext,void* paramin,void* paramout)
{
	*(int*)paramout = pDeviceContext->GetRenderState(*(int*)paramin);
	return TRUE;
}


BOOL Render_SetScissor(SYDeviceContex* pDeviceContext,void* paramin,void* paramout)
{
	pDeviceContext->SetScissor(*(RECT*)paramin);
	return TRUE;
}

BOOL Render_GetScissor(SYDeviceContex* pDeviceContext,void* paramin,void* paramout)
{
	RECT* pRect = (RECT*)paramout;
	*pRect = pDeviceContext->GetScissor();
	return TRUE;
}

BOOL Render_EngineLock(SYDeviceContex* pDeviceContext)
{
	pDeviceContext->EngineLock.Lock();
	return TRUE;
}
BOOL Render_EngineUnLock(SYDeviceContex* pDeviceContext)
{
	pDeviceContext->EngineLock.UnLock();
	return TRUE;
}
BOOL Render_RenderLock(SYDeviceContex* pDeviceContext)
{
	pDeviceContext->RenderLock.Lock();
	return TRUE;
}

BOOL Render_RenderUnLock(SYDeviceContex* pDeviceContext)
{
	pDeviceContext->RenderLock.UnLock();
	return TRUE;
}

BOOL Render_GetMemTexBuffer(SY_DeviceSoft* pDevice,void* paramin,void* paramout)
{
	SYSoftTexture* pTexture = (SYSoftTexture*)SYHTable_GetAt(&pDevice->httexture,*(int*)paramin);
	DXGetMemTexBufferOut* pBuffer = (DXGetMemTexBufferOut*)paramout;
	pBuffer->ptr = pTexture->pTexture->m_dwBuffer;
	pBuffer->pitch = pTexture->pTexture->m_nPitch ;
	return true;
}

void GetTexInfo(SY_DeviceSoft* pDevice,int hTex,int* pWidth,int* pHeight)
{
	CSYBuffer* pSrcTex = getTexture(pDevice,hTex);
	getTextureInfo(pSrcTex,pWidth,pHeight);
}


BOOL Render_GetSwapChainBufferTex(SY_DeviceSoft* pDevice,void* paramin,void* paramout)
{
	int* hSwapChain = (int*)paramin;
	SYSoftSwapChain* pSwapChain = (SYSoftSwapChain*)SYHTable_GetAt(&pDevice->htSwapChain,*hSwapChain);
	int* pOut = (int*)paramout;
	*pOut = *hSwapChain;
	return TRUE;
}

BOOL Render_ClearRenderInfo(SY_DeviceSoft* pDevice,void* paramin,void* paramout)
{
	pDevice->info.nRenderCount = 0;
	pDevice->info.nTriangleCount = 0;
	return TRUE;
}

bool Render_CreateTexFromFile(SY_DeviceSoft* pDevice,void* paramin,void* paramout)
{
	createTextureFromFileIn* param = (createTextureFromFileIn*)paramin;
	TCHAR* pPath = param->pPath;

	SYHash*	pHash = NULL;		
	DXTextureType nType = param->bNormalMap ? DXTexture_2DDumpReadOnly : DXTexture_2DReadOnly;
	if(nType == DXTexture_2DDumpReadOnly)
		pHash = pDevice->BumpTextureFromFileHash;
	else if(nType == DXTexture_2DReadOnly)
		pHash = pDevice->TextureFromFileHash;

	HandleHash* pTexHashValue = (HandleHash*)SYHash_Search(pHash,(void *)pPath);
	if (pTexHashValue)
	{
		++(pTexHashValue->nCount) ;
		*(int*)paramout = pTexHashValue->hHandle;
		return true;
	}

	int nImgWidth=0,nImgHeight=0;

	HRESULT hr = S_FALSE ;
	BOOL bMipMap = FALSE;
	int nX = 0,nY = 0;
	BaseCodec_GetImgInfo(pPath,nX,nY,nImgWidth,nImgHeight);
	if(nImgWidth * nImgHeight == 0)
	{
		*(int*)paramout = 0;
		return false;
	}
	nImgWidth = min(nImgWidth,4096);
	nImgHeight = min(nImgHeight,4096);

	SYSoftTexture* pFileTex = new SYSoftTexture;
	memset(pFileTex,0,sizeof(SYSoftTexture));
	pFileTex->pTexture = new CSYBuffer(nImgWidth,nImgHeight);
	BaseCodec_ReadImage(pPath, nImgWidth, nImgHeight, pFileTex->pTexture->m_dwBuffer);
	pFileTex->nType = nType;

	size_t nLength = _tcslen(pPath) + 1;
	pFileTex->pTexFile = new TCHAR[nLength];
	_tcscpy_s(pFileTex->pTexFile,nLength,pPath);

	int hNewSwapChain = (int)SYHTable_Insert(&pDevice->httexture,pFileTex);
	*(int*)paramout = hNewSwapChain;

	pTexHashValue = new HandleHash;
	pTexHashValue->hHandle = hNewSwapChain;
	pTexHashValue->nCount = 1;
	SYHash_Insert(pHash,pFileTex->pTexFile,pTexHashValue);
	return true;
}

BOOL Render_CreateContext(SY_DeviceSoft* pDevice,void* paramout)
{
	SYDeviceContex* pDeviceContext = new SYDeviceContex;
	UINT hContext = SYHTable_Insert(&pDevice->htContexTable,pDeviceContext);
	*(int*)paramout = hContext;
	return TRUE;
}

BOOL Render_ReleaseContext(SY_DeviceSoft* pDevice,void* paramin)
{
	UINT hContext = *(UINT*)paramin;
	SYDeviceContex* pDeviceContext = (SYDeviceContex*)SYHTable_RemoveAt(&pDevice->htContexTable,hContext);
	for(auto it = pDevice->mapContext.begin(); it != pDevice->mapContext.end();)
	{
		if(it->second == pDeviceContext)
			pDevice->mapContext.erase(it++);
		else
			it++;
	}

	SAFE_DELETE(pDeviceContext);
	return TRUE;
}

BOOL Render_SetContext(SY_DeviceSoft* pDevice,void* paramin)
{
	UINT hContext = *(UINT*)paramin;
	SYDeviceContex* pDeviceContext = (SYDeviceContex*)SYHTable_GetAt(&pDevice->htContexTable,hContext);
	DWORD id = GetCurrentThreadId(); 
	pDevice->mapContext[id] = pDeviceContext;
	return TRUE;
}

BOOL RenderSoftEngine_Main( int ncmd, int hDevice, void* paramin, void* paramout )
{
	DXSingleLock lock(g_DeviceTableLock,false);
	lock.Lock();

	if(ncmd == RENDERENGINE_INIT)
		return Render_init(paramin,paramout);
	else if(ncmd == RENDERENGINE_RELEASE)
		return Render_Release(paramin,paramout);
	else if(ncmd == RENDERENGINE_CREATEDEVICE)
		return Render_CreateDevice(paramin,paramout);

	SY_DeviceSoft* pDevice = (SY_DeviceSoft*)SYHTable_GetAt(&g_SoftDeviceHT,hDevice);
	lock.UnLock();
	//////////////////////////////////////////////////////////////////////////
	if(!pDevice)
		return FALSE;
	//资源相关

	DXSingleLock resLock(pDevice->ResLock,false);
	resLock.Lock();
	switch(ncmd)
	{
	case RENDERENGINE_CREATE_TEXTURE:
		return Render_CreateTexture(pDevice,paramin,paramout);
	case RENDERENGINE_RELEASE_SWAPCHAIN:
		return Render_ReleaseSwapChain(pDevice,paramin,paramout);
	case RENDERENGINE_CREATESWAPCHAIN:
		return Render_CreateSwapChain(pDevice,paramin,paramout);
	case RENDERENGINE_GET_SWAPCHAINDESC:
		return Render_GetSwapChainDesc(pDevice,paramin,paramout);
	case RENDERENGINE_GET_SWAPCHAIN_BUFFER_TEX:
		return Render_GetSwapChainBufferTex(pDevice,paramin,paramout);
	case RENDERENGINE_CLEAR_TEXTURE:
		return render_ClearTexture(pDevice,paramin,paramout);
	case RENDERENGINE_CLEAR_RENDER_INFO:
		return Render_ClearRenderInfo(pDevice,paramin,paramout);
	case RENDERENGINE_GET_MESYEX_BUFFER:
		return Render_GetMemTexBuffer(pDevice,paramin,paramout);
	case RENDERENGINE_GET_TEXTURE_INFO:
		return render_GetTextureInfo(pDevice,paramin,paramout);
	case RENDERENGINE_RELEASE_TEXTURE:
		return RenderSoft_ReleaseTexture(pDevice,paramin,paramout);
	case RENDERENGINE_CREATE_TEXTURE_FROM_FILE:
		return Render_CreateTexFromFile(pDevice,paramin,paramout);
	case RENDERENGINE_DUMP_TEXTURE:
		return render_DumpTexture(pDevice,paramin,paramout);
	case RENDERENGINE_CREATE_CONTEXT:
		return Render_CreateContext(pDevice,paramout);
	case RENDERENGINE_RELEASE_CONTEXT:
		return Render_ReleaseContext(pDevice,paramin);
	case RENDERENGINE_SET_CONTEXT:
		return Render_SetContext(pDevice,paramin);
	}
	resLock.UnLock();
	//////////////////////////////////////////////////////////////////////////
	DWORD id = GetCurrentThreadId(); 
	auto it = pDevice->mapContext.find(id);
	SYDeviceContex* pDeviceContext = &pDevice->defaultContext;
	if(it != pDevice->mapContext.end())
		pDeviceContext = it->second;
	//渲染相关
	//////////////////////////////////////////////////////////////////////////
	//无需api lock
	switch(ncmd)
	{
	case RENDERENGINE_ENGINE_LOCK:
	case RENDERENGINE_ENGINE_RES_LOCK:
		return Render_EngineLock(pDeviceContext);
	case RENDERENGINE_ENGINE_UNLOCK:
	case RENDERENGINE_ENGINE_RES_UNLOCK:
		return Render_EngineUnLock(pDeviceContext);
	case RENDERENGINE_RENDER_LOCK:
		return Render_RenderLock(pDeviceContext);
	case RENDERENGINE_RENDER_UNLOCK:
		return Render_RenderUnLock(pDeviceContext);
	case RENDERENGINE_UPLOAD: 
		return Render_UpLoad(pDevice,pDeviceContext,paramin,paramout);
	case RENDERENGINE_COPY_TEXTURE:
		return Render_CopyTexture(pDevice,pDeviceContext,paramin,paramout);
	}
	//////////////////////////////////////////////////////////////////////////
	DXSingleLock contextApiLock(pDeviceContext->ContexLock);
	switch(ncmd)
	{
		case RENDERENGINE_CLEAR_SCREEN:
			return Render_ClearScreen(pDevice,pDeviceContext,paramin,paramout);
		case RENDERENGINE_GET_RENDERTARGET:
			return Render_GetRenderTarget(pDeviceContext,paramin,paramout);
		case RENDERENGINE_SET_RENDERTARGET:
			return Render_SetRenderTarget(pDevice,pDeviceContext,paramin,paramout);
		case RENDERENGINE_SETVIEWPORT:
			return Render_SetViewPort(pDeviceContext,paramin,paramout);
		case RENDERENGINE_GETVIEWPORT:
			return Render_GetViewPort(pDeviceContext,paramin,paramout);
		case RENDERENGINE_GET_SCISSOR:
			return Render_GetScissor(pDeviceContext,paramin,paramout);
		case RENDERENGINE_SET_SCISSOR:
			return Render_SetScissor(pDeviceContext,paramin,paramout);
		case RENDERENGINE_RENDER_TEXTURE:
			return Render_RenderTex(pDevice,pDeviceContext,paramin,paramout);
		case RENDERENGINE_BEGIN_RENDER_TEXTURE:
			return Render_BeginTex(pDeviceContext,paramin,paramout);
		case RENDERENGINE_END_RENDER_TEXTURE:
			return Render_EndTex(pDeviceContext,paramin,paramout);
		case RENDERENGINE_SET_RENDER_STATE:
			return Render_SetRenderState(pDeviceContext,paramin,paramout);
	}
	return false;
}