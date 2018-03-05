#include "stdafx.h"
#include "SceneRender\DXRenderEngineDX10Api.h"
#include <string>
#include <iostream>
#include <fstream>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

htable g_deviceHT;
IDXGIFactory*	g_pDXFactory = NULL;
IDXGIAdapter* g_pAdapter = NULL;
TCHAR g_szDX10PathShader[MAX_PATH] = {0};	
TCHAR g_szDX10PathEffect[MAX_PATH] = {0};	

inline void cutStrechArea(int& srcOffsetX, int& srcOffsetY, int& srcValidWidth, int& srcValidHeight, int srcBufferWidth, int srcBufferHeight, int& destOffsetX, int& destOffsetY, int& destValidWidth, int& destValidHeight, int destBufferWidth, int destBufferHeight, float* fXScale, float* fYScale)
{
	float xscale = destValidWidth / (float)srcValidWidth;
	float yscale = destValidHeight / (float)srcValidHeight;
	if (fXScale)
		*fXScale = xscale;
	if (fYScale)
		*fYScale = yscale;


	if (srcOffsetX > srcBufferWidth
		|| srcOffsetY > srcBufferHeight
		|| destOffsetX > destBufferWidth
		|| destOffsetY > destBufferHeight
		|| srcOffsetX + srcValidWidth < 0
		|| srcOffsetY + srcValidHeight < 0
		|| destOffsetY + destValidHeight < 0
		|| destOffsetX + destValidWidth < 0)
	{
		srcValidHeight = 0;
		srcValidWidth = 0;
		destValidHeight = 0;
		destValidWidth = 0;
		return;
	}
	if (srcOffsetX < 0)
	{
		destOffsetX -= SY_FToI(srcOffsetX * xscale);
		srcValidWidth += srcOffsetX;
		destValidWidth += SY_FToI(srcOffsetX * xscale);
		srcOffsetX = 0;
	}
	if (destOffsetX < 0)
	{
		srcOffsetX -= SY_FToI(destOffsetX / xscale);
		destValidWidth += destOffsetX;
		srcValidWidth += SY_FToI(destOffsetX / xscale);
		destOffsetX = 0;
	}

	if (srcOffsetY < 0)
	{
		destOffsetY -= SY_FToI(srcOffsetY * yscale);
		srcValidHeight += srcOffsetY;
		destValidHeight += SY_FToI(srcOffsetY * yscale);
		srcOffsetY = 0;
	}
	if (destOffsetY < 0)
	{
		srcOffsetY -= SY_FToI(destOffsetY * yscale);
		destValidHeight += destOffsetY;
		srcValidHeight += SY_FToI(destOffsetY / yscale);
		destOffsetY = 0;
	}

	if (srcOffsetX + srcValidWidth > srcBufferWidth)
	{
		int OffsetX = srcOffsetX + srcValidWidth - srcBufferWidth;
		srcValidWidth -= OffsetX;
		destValidWidth -= SY_FToI(OffsetX * xscale);
	}
	if (srcOffsetY + srcValidHeight > srcBufferHeight)
	{
		int OffsetY = srcOffsetY + srcValidHeight - srcBufferHeight;
		srcValidHeight -= OffsetY;
		destValidHeight -= SY_FToI(OffsetY * yscale);
	}

	if (destOffsetX + destValidWidth > destBufferWidth)
	{
		int OffsetX = destOffsetX + destValidWidth - destBufferWidth;
		destValidWidth -= OffsetX;
		srcValidWidth -= SY_FToI(OffsetX / xscale);
	}
	if (destOffsetY + destValidHeight > destBufferHeight)
	{
		int OffsetY = destOffsetY + destValidHeight - destBufferHeight;
		destValidHeight -= OffsetY;
		srcValidHeight -= SY_FToI(OffsetY / yscale);
	}
}

CRITICAL_SECTION	g_dx10EngineLock;  // just for temp
static int g_nEngineLockCount = 0;
static BOOL g_bOptimal=FALSE;
class CDX10EngineLock
{
public:
	CDX10EngineLock(){ EnterCriticalSection(&g_dx10EngineLock); g_nEngineLockCount++;}
	~CDX10EngineLock(){ LeaveCriticalSection(&g_dx10EngineLock);g_nEngineLockCount--;}
};
void EngineThreadLock()
{
	EnterCriticalSection(&g_dx10EngineLock); g_nEngineLockCount++;
}
void EngineThreadUnLock()
{
	LeaveCriticalSection(&g_dx10EngineLock);g_nEngineLockCount--;
}

const D3D10_INPUT_ELEMENT_DESC tex_layout[] = 
{
	{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT, 0, 0 ,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
};

const D3D10_INPUT_ELEMENT_DESC pos_tex_layout[] = 
{
	{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT, 0, 12 ,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
};

const D3D10_INPUT_ELEMENT_DESC pos_color_tex_layout[] =
{
	{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",  0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12 ,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT, 0, 16 ,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
};


const D3D10_INPUT_ELEMENT_DESC pos_normal_tex_layout[] = 
{
	{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12 ,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT, 0, 24 ,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
};
 
const D3D10_INPUT_ELEMENT_DESC pos_normal_tex_tang_bi_layout[] = 
{
	{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12 ,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT, 0, 24 ,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",  1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32 ,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",  2, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44 ,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
};

const D3D10_INPUT_ELEMENT_DESC VertexInstanceLayout[] = 
{
	{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12 ,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT, 0, 24 ,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",  1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32 ,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",  2, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44 ,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "mTransform",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0 ,  D3D10_INPUT_PER_INSTANCE_DATA, 1 },
	{ "mTransform",  1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16 ,  D3D10_INPUT_PER_INSTANCE_DATA, 1 },
	{ "mTransform",  2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32 ,  D3D10_INPUT_PER_INSTANCE_DATA, 1 },
	{ "mTransform",  3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48 ,  D3D10_INPUT_PER_INSTANCE_DATA, 1 },
	{ "TEXCOORD",  3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64 ,  D3D10_INPUT_PER_INSTANCE_DATA, 1 },
};


const D3D10_INPUT_ELEMENT_DESC effect_explore_layout[] = 
{
	{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0 ,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "POSITION",  1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0 ,  D3D10_INPUT_PER_INSTANCE_DATA, 1 },
	{ "POSITION",  2, DXGI_FORMAT_R32G32B32_FLOAT, 1, 12,  D3D10_INPUT_PER_INSTANCE_DATA, 1 },
	{ "POSITION",  3, DXGI_FORMAT_R32G32B32_FLOAT, 1, 24,  D3D10_INPUT_PER_INSTANCE_DATA, 1 },
	{ "POSITION",  4, DXGI_FORMAT_R32G32B32_FLOAT, 1, 36,  D3D10_INPUT_PER_INSTANCE_DATA, 1 },
	{ "POSITION",  5, DXGI_FORMAT_R32G32B32_FLOAT, 1, 48,  D3D10_INPUT_PER_INSTANCE_DATA, 1 },
	{ "POSITION",  6, DXGI_FORMAT_R32G32B32_FLOAT, 1, 60,  D3D10_INPUT_PER_INSTANCE_DATA, 1 },
	{ "POSITION",  7, DXGI_FORMAT_R32G32B32_FLOAT, 1, 72,  D3D10_INPUT_PER_INSTANCE_DATA, 1 },
};


const D3D10_INPUT_ELEMENT_DESC effect_tile_layout[] = 
{
	{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0 ,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "POSITION",  1, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0 ,  D3D10_INPUT_PER_INSTANCE_DATA, 1 },
	{ "POSITION",  2, DXGI_FORMAT_R32G32B32_FLOAT, 1, 12,  D3D10_INPUT_PER_INSTANCE_DATA, 1 },
	{ "POSITION",  3, DXGI_FORMAT_R32G32B32_FLOAT, 1, 24,  D3D10_INPUT_PER_INSTANCE_DATA, 1 },
};


const D3D10_INPUT_ELEMENT_DESC pos[] = 
{
	{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
};


const D3D10_INPUT_ELEMENT_DESC* g_pLayout[] =
{
	pos_tex_layout,
	pos_color_tex_layout,
	pos_normal_tex_layout,
	pos_normal_tex_tang_bi_layout,
	pos,
	VertexInstanceLayout,
	effect_explore_layout,
	tex_layout,
	effect_tile_layout,
};

const int g_pLayoutNum[] = 
{
	2,
	3,
	3,
	5,
	1,
	10,
	8,
	1,
	4,
};
//////////////////////////////////////////////////////////////////////////
BOOL SetConstBuffer(SY_DeviceDX10* pDevice,int nShaderType,int nRegIndex,int hConstBuffer)
{
	SYBuffer* pCB = (SYBuffer*)SYHTable_GetAt(&pDevice->htBuffer,hConstBuffer );
	if(pCB==NULL)
	{
		return FALSE;
	}
	switch(nShaderType)
	{
	case DXShaderType_VertexShader:
		pDevice->pDevice->VSSetConstantBuffers(nRegIndex,1,&pCB->pBuffer);
		break;
	case DXShaderType_GeometryShader:
		pDevice->pDevice->GSSetConstantBuffers(nRegIndex,1,&pCB->pBuffer);
		break;
	case DXShaderType_PixelShader:
		pDevice->pDevice->PSSetConstantBuffers(nRegIndex,1,&pCB->pBuffer);
		break;
	default:
		return false;
	}
	pDevice->pConstBuffer[nShaderType][nRegIndex] = hConstBuffer;
	return true;

}
void  RenderTexture(SY_DeviceDX10* pDevice,int hTex,float fAlpha, const RECT& SrcRect,const RECT& DestRect,
	int nSrcBufferWidth,int nSrcBufferHeight,int nDstBufferWidth,int nDstBufferHeight,BOOL bAdjustColor,DXTextureType nType,FIELD fieldState=BOTH_FIELD)
{
	screenCB screen;
	matrix4f matWorld;
	matrix4f_identity(&matWorld);

	float fXScale = (float)(DestRect.right - DestRect.left) ;
	float fYScale = (float)(DestRect.bottom - DestRect.top);
	matWorld._11 = fXScale / 2.f;
	matWorld._22 = fYScale / 2.f;

	matrix4f posMat;
	matrix4f_identity(&posMat);
	float posX = DestRect.left - (-matWorld._11 + nDstBufferWidth / 2.f) ;
	float posY = -(DestRect.top - (-matWorld._22 + nDstBufferHeight / 2.f));
	posMat._41 = posX;
	posMat._42 = posY;
	posMat._43 = 0.5;
	screen.world_View_proj = matWorld * posMat * pDevice->spriteProj;
	//float4 test = {-1,1,0,1};
	//test *= screen.world_View_proj;

	screen.texCoord.x = SrcRect.left / (float)nSrcBufferWidth;
	screen.texCoord.y = SrcRect.top / (float)nSrcBufferHeight;

	int nSrcWidth = (SrcRect.right - SrcRect.left);
	int nSrcHeight = (SrcRect.bottom - SrcRect.top);
	screen.texCoord.z = nSrcWidth / (float)nSrcBufferWidth;
	screen.texCoord.w = nSrcHeight / (float)nSrcBufferHeight;
	ModifyBuffer(pDevice,pDevice->hScreenCB,&screen);

	float4 vPSParam[3];
	vPSParam[0].x = fAlpha;
	if(fieldState==FIRST_FIELD)
		vPSParam[0].y = 1.f;
	else if(fieldState==SECOND_FIELD)
		vPSParam[0].y = -1.f;
	else
		vPSParam[0].y = 0.f;
	ModifyBuffer(pDevice,pDevice->hModiyCB,vPSParam);
	SetConstBuffer(pDevice,DXShaderType_PixelShader,0,pDevice->hModiyCB);

	SetVertexBuffer(pDevice,pDevice->hScreenVertex);
	SetPrimitiveTopology(pDevice,TRIANGLELIST);
	SetConstBuffer(pDevice,DXShaderType_VertexShader,0,pDevice->hScreenCB);
	SetShader(pDevice,DXShaderType_VertexShader,pDevice->hScreenVS);
	SetShader(pDevice,DXShaderType_GeometryShader,NULL);
	if(nType == DXTexture_MS2DTex)
		SetShader(pDevice,DXShaderType_PixelShader,pDevice->hScreenMSPS);
	else
		SetShader(pDevice,DXShaderType_PixelShader,bAdjustColor?pDevice->hScreenAdjustPS:pDevice->hScreenPS);
	//pDevice->pDevice->PSSetShaderResources(0,1,&pTexture);
	SetShaderTexture(pDevice,hTex,DXShaderType_PixelShader,0);

	pDevice->pDevice->Draw(6,0);	
}

//////////////////////////////////////////////////////////////////////////
inline int CreateBuffer(SY_DeviceDX10* pDevice,int nSize,int nNum,void* pBuffer,DXBufferType nType,int nPriType = -1)
{
	SYBuffer* pVideoBuffer = new SYBuffer;
	pVideoBuffer->nType = nType;
	pVideoBuffer->nVertexSize = nSize;
	pVideoBuffer->nBufferSize = nSize* nNum;
	pVideoBuffer->nPriType = -1;
	D3D10_BUFFER_DESC desc;
	switch (nType)
	{
	case DXVertexBuffer:
		CreateVertexDesc(&desc,nSize * nNum);
		pVideoBuffer->nPriType = nPriType;
		break;
	case DXIndexBuffer:
		CreateIndexDesc(&desc,nSize * nNum);
		break;
	case DXConstBuffer:
		CreateConstDesc(&desc,nSize * nNum);
		break;
	}
	if(pBuffer)
	{
		D3D10_SUBRESOURCE_DATA ibInitData;
		ZeroMemory( &ibInitData, sizeof( D3D10_SUBRESOURCE_DATA ) );
		ibInitData.pSysMem = pBuffer;

		pDevice->pDevice->CreateBuffer(&desc,&ibInitData,&pVideoBuffer->pBuffer);
	}
	else
		pDevice->pDevice->CreateBuffer(&desc,NULL,&pVideoBuffer->pBuffer);

	if (!pVideoBuffer->pBuffer)
	{
		assert(0);
		return 0;
	}
	int hBuffer = (int)SYHTable_Insert(&pDevice->htBuffer,pVideoBuffer);

	return hBuffer;
}

bool Render_CreateShader(int hDevice, void* paramin, void* paramout);
bool Render_CreateShader(SY_DeviceDX10* pDevice, void* paramin, void* paramout);

int CreateShaderFromCompiledFile(SY_DeviceDX10* pDevice, LPCTSTR shaderName, LPCTSTR fileName, DXShaderType nType, DXVertexType* pType)
{
	std::ifstream s_stream;
	size_t s_size;
	char* s_data;

	s_stream.open(fileName, std::ifstream::in | std::ifstream::binary);

	if (s_stream.good())
	{
		s_stream.seekg(0, std::ios::end);
		s_size = size_t(s_stream.tellg());
		s_data = new char[s_size];
		s_stream.seekg(0, std::ios::beg);
		s_stream.read(&s_data[0], s_size);
		s_stream.close();
	}
	else
	{
		return 0;
	}


	int hShader = 0;
	DXCreateShadeIn param;
	param.shaderFile = NULL;
	param.shaderCode = s_data;
	param.nType = nType;
	param.shaderName = shaderName;
	param.pType = pType;
	param.bLock = FALSE;
	param.bComplied = TRUE;
	param.compliedBinSize = s_size;

	Render_CreateShader(pDevice, &param, &hShader);
	//if (g_dRenderType == DX_DX10_Device)
	//{
	//RenderEngineDX10_Main(RENDERENGINE_CREATESHADER, hDevice, &param, &hShader);
	//}
	//else if (g_dRenderType == DX_Soft_Device)
	//{
	//	RenderSoftEngine_Main(RENDERENGINE_CREATESHADER, hDevice, &param, &hShader);
	//}

	delete[] s_data;
	s_data = NULL;

	return hShader;
}

int CreateShaderFromFile(SY_DeviceDX10* pDevice,LPCTSTR shaderName,LPCTSTR fileName,DXShaderType nType,DXVertexType* pType)
{
	shaderhash* pShaderHashValue = (shaderhash*)SYHash_Search(pDevice->ShaderHash,(void *)shaderName);
	if (pShaderHashValue)
	{
		++(pShaderHashValue->nCount) ;
		return pShaderHashValue->hHandle;
	}

	char* profile = NULL;
	switch (nType)
	{
	case DXShaderType_VertexShader:
		profile = VSPROFILE;
		break;
	case DXShaderType_GeometryShader:
		profile = GSPROFILE;
		break;
	case DXShaderType_PixelShader:
		profile = PSPROFILE;
		break;
	}
	ID3D10Blob* pShader;
	ID3D10Blob** ppError = NULL;
#ifdef _DEBUG
	ID3D10Blob* pError;
	ppError = &pError; 
#endif // _DEBUG
	HRESULT hr = D3DX10CompileFromFile( fileName, NULL, NULL, "main", profile, NULL, D3D10_SHADER_OPTIMIZATION_LEVEL3, NULL, &pShader, ppError, NULL );

#ifdef _DEBUG
	if (hr != S_OK)
	{
		char* msg = (char*)pError->GetBufferPointer();
		assert(hr == S_OK);
		int nSize = (int)pError->GetBufferSize();
	}
#endif // _DEBUG

	SYShader* pNewShader = new SYShader;
	pNewShader->pLayout = NULL;
	pNewShader->nType = nType;
	switch (nType)
	{
	case DXShaderType_VertexShader:
		pDevice->pDevice->CreateVertexShader(pShader->GetBufferPointer(),pShader->GetBufferSize(),(ID3D10VertexShader**)&pNewShader->pShader);
		if (pType)
			hr = pDevice->pDevice->CreateInputLayout( g_pLayout[*pType],g_pLayoutNum[*pType] , pShader->GetBufferPointer(), pShader->GetBufferSize(), &pNewShader->pLayout ) ;
		break;
	case DXShaderType_GeometryShader:
		pDevice->pDevice->CreateGeometryShader(pShader->GetBufferPointer(),pShader->GetBufferSize(),(ID3D10GeometryShader**)&pNewShader->pShader);
		break;
	case DXShaderType_PixelShader:
		pDevice->pDevice->CreatePixelShader(pShader->GetBufferPointer(),pShader->GetBufferSize(),(ID3D10PixelShader**)&pNewShader->pShader);
		break;
	}
	pShader->Release();

	int nNameLenth = (int)_tcslen(shaderName) + 1;
	pNewShader->pShaderName = new TCHAR[nNameLenth];
	_tcscpy_s(pNewShader->pShaderName,nNameLenth,shaderName);
	int hShader = (int)SYHTable_Insert(&pDevice->htShaderTable,pNewShader);
	//if(hShader == 21)
	//	int a = 0;
	pShaderHashValue = new shaderhash;
	pShaderHashValue->hHandle = hShader;
	pShaderHashValue->nCount = 1;
	SYHash_Insert(pDevice->ShaderHash,pNewShader->pShaderName,pShaderHashValue);
	return hShader;
}
void ShaderHashRelease(void* pValue)
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

int Create2DVertex(SY_DeviceDX10* pDevice)
{
	screenVertex pSrceenVertex[6]; 
	pSrceenVertex[0].pos.x = 0;
	pSrceenVertex[0].pos.y = -1.f;
	pSrceenVertex[0].pos.z = 0.f;
	pSrceenVertex[0].tu    = 0;
	pSrceenVertex[0].tv    = 1;

	pSrceenVertex[1].pos.x = 1.f;
	pSrceenVertex[1].pos.y = -1.f;
	pSrceenVertex[1].pos.z = 0.f;
	//pSrceenVertex[1].tu    = 0.8023;
	pSrceenVertex[1].tu    = 1;
	pSrceenVertex[1].tv    = 1;

	pSrceenVertex[2].pos.x = 0;
	pSrceenVertex[2].pos.y = 0;
	pSrceenVertex[2].pos.z = 0.f;
	pSrceenVertex[2].tu    = 0;
	pSrceenVertex[2].tv    = 0;

	pSrceenVertex[3].pos.x = 0;
	pSrceenVertex[3].pos.y = 0;
	pSrceenVertex[3].pos.z = 0.f;
	pSrceenVertex[3].tu    = 0;
	pSrceenVertex[3].tv    = 0;

	pSrceenVertex[4].pos.x = 1.f;
	pSrceenVertex[4].pos.y = -1.f;
	pSrceenVertex[4].pos.z = 0.f;
	pSrceenVertex[4].tu    = 1;
	pSrceenVertex[4].tv    = 1;

	pSrceenVertex[5].pos.x = 1.f;
	pSrceenVertex[5].pos.y = 0;
	pSrceenVertex[5].pos.z = 0.f;
	pSrceenVertex[5].tu    = 1;
	pSrceenVertex[5].tv    = 0;

	return CreateBuffer(pDevice,sizeof(screenVertex),6,pSrceenVertex,DXVertexBuffer,TRIANGLELIST);
}
int CreateScreenVertex(SY_DeviceDX10* pDevice)
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

void CreateBlendState(SY_DeviceDX10* pDevice)
{
	pDevice->pNowState[DXBlendState] = DXBlendState_Default;
	D3D10_BLEND_DESC blendDesc;
	memset(&blendDesc,0,sizeof(blendDesc));
	//////////////////////////////////////////////////////////////////////////
	//SYBlendDisable
	for (int i = 0; i < 8;i++)
	{
		blendDesc.BlendEnable[i] = 0;
		blendDesc.RenderTargetWriteMask[i] = D3D10_COLOR_WRITE_ENABLE_ALL;
	}
	pDevice->pDevice->CreateBlendState(&blendDesc,&pDevice->pBlendState[DXBlendDisable]);
	
	//////////////////////////////////////////////////////////////////////////
	//SYBlendEnable,
	for (int i = 0; i < 8;i++)
	{
		blendDesc.BlendEnable[i] = 1;
		blendDesc.RenderTargetWriteMask[i] = D3D10_COLOR_WRITE_ENABLE_ALL;
	}
//	blendDesc.AlphaToCoverageEnable = TRUE;
	blendDesc.SrcBlend =D3D10_BLEND_SRC_ALPHA ;
	blendDesc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA ;
	blendDesc.BlendOp = D3D10_BLEND_OP_ADD;
	blendDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD; 
	blendDesc.SrcBlendAlpha = D3D10_BLEND_ONE;
	blendDesc.DestBlendAlpha = D3D10_BLEND_INV_SRC_ALPHA;
	pDevice->pDevice->CreateBlendState(&blendDesc,&pDevice->pBlendState[DXBlend_Mix]);

	//////////////////////////////////////////////////////////////////////////
	//SYBlend_Trans_Mix,
	blendDesc.AlphaToCoverageEnable = FALSE;
	pDevice->pDevice->CreateBlendState(&blendDesc,&pDevice->pBlendState[DXBlend_Trans_Mix]);

	//////////////////////////////////////////////////////////////////////////
	//SYBlend_Enhance,
	blendDesc.SrcBlend =D3D10_BLEND_ONE ;
	blendDesc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA ;
	blendDesc.BlendOp = D3D10_BLEND_OP_ADD;
	blendDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD; 
	blendDesc.SrcBlendAlpha = D3D10_BLEND_ONE;
	blendDesc.DestBlendAlpha = D3D10_BLEND_INV_SRC_ALPHA;
	pDevice->pDevice->CreateBlendState(&blendDesc,&pDevice->pBlendState[DXBlend_Enhance]);
	//////////////////////////////////////////////////////////////////////////
	//SYBlend_Add
	blendDesc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
	blendDesc.DestBlend = D3D10_BLEND_DEST_ALPHA;
	blendDesc.BlendOp = D3D10_BLEND_OP_ADD;
	blendDesc.SrcBlendAlpha = D3D10_BLEND_ONE;
	blendDesc.DestBlendAlpha = D3D10_BLEND_INV_SRC_ALPHA;
	blendDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	pDevice->pDevice->CreateBlendState(&blendDesc,&pDevice->pBlendState[DXBlend_Add]);


	//SYBlend_Effect_Fire1
	blendDesc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
	blendDesc.DestBlend = D3D10_BLEND_ONE;
	blendDesc.BlendOp = D3D10_BLEND_OP_ADD;
	blendDesc.SrcBlendAlpha = D3D10_BLEND_ONE;
	blendDesc.DestBlendAlpha = D3D10_BLEND_ONE;
	blendDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	pDevice->pDevice->CreateBlendState(&blendDesc,&pDevice->pBlendState[DXBlend_Effect_Fire1]);

	//SYBlend_Effect_Fire2
	blendDesc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
	blendDesc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
	blendDesc.BlendOp = D3D10_BLEND_OP_ADD;
	blendDesc.SrcBlendAlpha = D3D10_BLEND_ONE;
	blendDesc.DestBlendAlpha = D3D10_BLEND_ONE;
	blendDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	pDevice->pDevice->CreateBlendState(&blendDesc,&pDevice->pBlendState[DXBlend_Effect_Fire2]);

	//Test
	blendDesc.SrcBlend = D3D10_BLEND_ONE;
	blendDesc.DestBlend = D3D10_BLEND_ZERO;
	blendDesc.BlendOp = D3D10_BLEND_OP_MAX;
	blendDesc.SrcBlendAlpha = D3D10_BLEND_ONE;
	blendDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
	blendDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	pDevice->pDevice->CreateBlendState(&blendDesc,&pDevice->pBlendState[DXBlend_Test]);
}

void CreateDepthState(SY_DeviceDX10* pDevice)
{
	pDevice->pNowState[DXDepthState] = DXDepthState_Default;
	D3D10_DEPTH_STENCIL_DESC desc;
	memset(&desc,0,sizeof(D3D10_DEPTH_STENCIL_DESC));
	desc.StencilEnable = FALSE;
	desc.StencilReadMask = 0xFF;
	desc.StencilWriteMask = 0xFF;
	desc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
	desc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	desc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
	desc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	desc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
	////////////////////////////////////////////////////////////////////////////
	////SYDepthEnable_LessPass,
	//desc.DepthEnable = TRUE;
	//desc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	//desc.DepthFunc = D3D10_COMPARISON_LESS;

	//pDevice->pDevice->CreateDepthStencilState(&desc,&pDevice->pDepthState[SYDepthEnable_LessPass]);

	//////////////////////////////////////////////////////////////////////////
	//SYDepthEnable_LessEqualPass,
	desc.DepthEnable = TRUE;
	desc.DepthFunc = D3D10_COMPARISON_LESS_EQUAL;
	desc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	pDevice->pDevice->CreateDepthStencilState(&desc,&pDevice->pDepthState[DXDepthEnable_LessEqualPass]);
	//////////////////////////////////////////////////////////////////////////
	//SYDepthEnable_LessEqualPass_DepthNoWrite,
	desc.DepthEnable = TRUE;
	desc.DepthFunc = D3D10_COMPARISON_LESS_EQUAL;
	desc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ZERO;
	pDevice->pDevice->CreateDepthStencilState(&desc,&pDevice->pDepthState[DXDepthEnable_LessEqualPass_DepthNoWrite]);
	
	//////////////////////////////////////////////////////////////////////////
	//SYDepthLessEqual_Mask_01 - SYDepthLessEqual_Mask_FF
	desc.DepthEnable = TRUE;
	desc.DepthFunc = D3D10_COMPARISON_LESS_EQUAL;
	desc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;

	desc.StencilEnable = TRUE;
	desc.StencilWriteMask = 0;

	desc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
	desc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFunc = D3D10_COMPARISON_EQUAL;

	desc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
	desc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	desc.BackFace.StencilFunc = D3D10_COMPARISON_EQUAL;

	for (int i = DXDepthLessEqual_Mask_01; i <= DXDepthLessEqual_Mask_FF; i++ )
	{
		desc.StencilReadMask = 0x01 + i - DXDepthLessEqual_Mask_01;
		pDevice->pDevice->CreateDepthStencilState(&desc,&pDevice->pDepthState[i]);
	}

	//////////////////////////////////////////////////////////////////////////
	//SYDepthLessEqual_NoWrite_Mask_01 - SYDepthLessEqual_NoWrite_Mask_FF
	desc.DepthEnable = TRUE;
	desc.DepthFunc = D3D10_COMPARISON_LESS_EQUAL;
	desc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ZERO;

	desc.StencilEnable = TRUE;
	desc.StencilWriteMask = 0;

	desc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
	desc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFunc = D3D10_COMPARISON_EQUAL;

	desc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
	desc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	desc.BackFace.StencilFunc = D3D10_COMPARISON_EQUAL;

	for (int i = DXDepthLessEqual_NoWrite_Mask_01; i <= DXDepthLessEqual_NoWrite_Mask_FF; i++ )
	{
		desc.StencilReadMask = 0x01 + i - DXDepthLessEqual_NoWrite_Mask_01;
		pDevice->pDevice->CreateDepthStencilState(&desc,&pDevice->pDepthState[i]);
	}

	//////////////////////////////////////////////////////////////////////////
	//SYDepthDisable_Mask_01 - SYDepthDisable_Mask_FF
	//desc.DepthEnable = FALSE;

	//desc.StencilEnable = TRUE;
	//desc.StencilWriteMask = 0;

	//desc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	//desc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
	//desc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	//desc.FrontFace.StencilFunc = D3D10_COMPARISON_EQUAL;

	//desc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	//desc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
	//desc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	//desc.BackFace.StencilFunc = D3D10_COMPARISON_EQUAL;

	//for (int i = SYDepthDisable_Mask_01; i <= SYDepthDisable_Mask_FF; i++ )
	//{
	//	desc.StencilReadMask = 0x01 + i - SYDepthDisable_Mask_01;
	//	pDevice->pDevice->CreateDepthStencilState(&desc,&pDevice->pDepthState[i]);
	//}
	//////////////////////////////////////////////////////////////////////////
	//SYDepthDisable_WriteMask_01 - SYDepthDisable_WriteMask_FF
	desc.DepthEnable = FALSE;

	desc.StencilEnable = TRUE;
	desc.StencilReadMask = 0xFF;

	desc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_REPLACE;
	desc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_REPLACE;
	desc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_REPLACE;
	desc.FrontFace.StencilFunc = D3D10_COMPARISON_NEVER;

	desc.BackFace.StencilFailOp = D3D10_STENCIL_OP_REPLACE;
	desc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_REPLACE;
	desc.BackFace.StencilPassOp = D3D10_STENCIL_OP_REPLACE;
	desc.BackFace.StencilFunc = D3D10_COMPARISON_NEVER;

	for (int i = DXDepthDisable_WriteMask_01; i <= DXDepthDisable_WriteMask_FF; i++ )
	{
		desc.StencilWriteMask = 0x01 + i - DXDepthDisable_WriteMask_01;
		pDevice->pDevice->CreateDepthStencilState(&desc,&pDevice->pDepthState[i]);
	}
	//SYDepthDisable,
	desc.DepthEnable = FALSE;
	desc.StencilEnable = FALSE;
	pDevice->pDevice->CreateDepthStencilState(&desc,&pDevice->pDepthState[DXDepthDisable]);
}

void CreateRasterizerState(SY_DeviceDX10* pDevice)
{
	pDevice->pNowState[DXRasterizerState] = DXRasterizer_Default;
	D3D10_RASTERIZER_DESC desc;
	memset(&desc,0,sizeof(D3D10_RASTERIZER_DESC));
	desc.DepthClipEnable = TRUE;
	desc.ScissorEnable = TRUE;
	//desc.FrontCounterClockwise = TRUE;
	//////////////////////////////////////////////////////////////////////////
	//SYRas_Line_MultiSample,
	desc.FillMode = D3D10_FILL_SOLID;
	desc.CullMode = D3D10_CULL_NONE;
	desc.MultisampleEnable = TRUE;
	desc.AntialiasedLineEnable = TRUE;
	pDevice->pDevice->CreateRasterizerState(&desc,&pDevice->pRasterizerState[DXRas_Line_MultiSample]);
	//////////////////////////////////////////////////////////////////////////
	//SYRas_Line_NoMultiSample,
	desc.FillMode = D3D10_FILL_SOLID;
	desc.CullMode = D3D10_CULL_NONE;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;
	pDevice->pDevice->CreateRasterizerState(&desc,&pDevice->pRasterizerState[DXRas_Line_NoMultiSample]);
	//////////////////////////////////////////////////////////////////////////
	//SYRas_Solid_CullBack_MultiSample,
	desc.FillMode = D3D10_FILL_SOLID;
	desc.CullMode = D3D10_CULL_BACK;
	desc.MultisampleEnable = TRUE;
	pDevice->pDevice->CreateRasterizerState(&desc,&pDevice->pRasterizerState[DXRas_Solid_CullBack_MultiSample]);
	//////////////////////////////////////////////////////////////////////////
	//SYRas_Solid_CullBack_NoMultiSample,
	desc.FillMode = D3D10_FILL_SOLID;
	desc.CullMode = D3D10_CULL_BACK;
	desc.MultisampleEnable = FALSE;
	pDevice->pDevice->CreateRasterizerState(&desc,&pDevice->pRasterizerState[DXRas_Solid_CullBack_NoMultiSample]);
	//////////////////////////////////////////////////////////////////////////
	//SYRas_Solid_CullFront_MultiSample,
	desc.FillMode = D3D10_FILL_SOLID;
	desc.CullMode = D3D10_CULL_FRONT;
	desc.MultisampleEnable = TRUE;
	pDevice->pDevice->CreateRasterizerState(&desc,&pDevice->pRasterizerState[DXRas_Solid_CullFront_MultiSample]);
	//////////////////////////////////////////////////////////////////////////
	//SYRas_Solid_CullFront_NoMultiSample,
	desc.FillMode = D3D10_FILL_SOLID;
	desc.CullMode = D3D10_CULL_FRONT;
	desc.MultisampleEnable = FALSE;
	pDevice->pDevice->CreateRasterizerState(&desc,&pDevice->pRasterizerState[DXRas_Solid_CullFront_NoMultiSample]);
	//////////////////////////////////////////////////////////////////////////
	//SYRas_Solid_NoCullBack_MultiSample,
	desc.FillMode = D3D10_FILL_SOLID;
	desc.CullMode = D3D10_CULL_NONE;
	desc.MultisampleEnable = TRUE;
	pDevice->pDevice->CreateRasterizerState(&desc,&pDevice->pRasterizerState[DXRas_Solid_NoCullBack_MultiSample]);
	//////////////////////////////////////////////////////////////////////////
	//SYRas_Solid_NoCullBack_NoMultiSample,
	desc.FillMode = D3D10_FILL_SOLID;
	desc.CullMode = D3D10_CULL_NONE;
	desc.MultisampleEnable = FALSE;
	pDevice->pDevice->CreateRasterizerState(&desc,&pDevice->pRasterizerState[DXRas_Solid_NoCullBack_NoMultiSample]);
	//////////////////////////////////////////////////////////////////////////
	//SYRas_WireFrame_CullBack_MultiSample,
	desc.FillMode = D3D10_FILL_WIREFRAME;
	desc.CullMode = D3D10_CULL_BACK;
	desc.MultisampleEnable = TRUE;
	pDevice->pDevice->CreateRasterizerState(&desc,&pDevice->pRasterizerState[DXRas_WireFrame_CullBack_MultiSample]);
	//////////////////////////////////////////////////////////////////////////
	//SYRas_WireFrame_CullBack_NoMultiSample, 
	desc.FillMode = D3D10_FILL_WIREFRAME;
	desc.CullMode = D3D10_CULL_BACK;
	desc.MultisampleEnable = FALSE;
	pDevice->pDevice->CreateRasterizerState(&desc,&pDevice->pRasterizerState[DXRas_WireFrame_CullBack_NoMultiSample]);
	//////////////////////////////////////////////////////////////////////////
	//SYRas_WireFrame_NoCullBack_MultiSample,
	desc.FillMode = D3D10_FILL_WIREFRAME;
	desc.CullMode = D3D10_CULL_NONE;
	desc.MultisampleEnable = TRUE;
	pDevice->pDevice->CreateRasterizerState(&desc,&pDevice->pRasterizerState[DXRas_WireFrame_NoCullBack_MultiSample]);
	//////////////////////////////////////////////////////////////////////////
	//SYRas_WireFrame_NoCullBack_NoMultiSample,
	desc.FillMode = D3D10_FILL_WIREFRAME;
	desc.CullMode = D3D10_CULL_NONE;
	desc.MultisampleEnable = FALSE;
	pDevice->pDevice->CreateRasterizerState(&desc,&pDevice->pRasterizerState[DXRas_WireFrame_NoCullBack_NoMultiSample]);

}

void CreateSampleState(SY_DeviceDX10* pDevice)
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			pDevice->pNowSamplerState[i][j] = DXSample_Default;
		}
	}
	D3D10_SAMPLER_DESC dsec;
	memset(&dsec,0,sizeof(dsec));
	dsec.MaxLOD = 100;
	dsec.ComparisonFunc = D3D10_COMPARISON_NEVER;
	//////////////////////////////////////////////////////////////////////////
	//SYSample_Point_Clamp,
	dsec.Filter = D3D10_FILTER_MIN_MAG_MIP_POINT ;
	dsec.AddressU = D3D10_TEXTURE_ADDRESS_CLAMP ;
	dsec.AddressV = D3D10_TEXTURE_ADDRESS_CLAMP ;
	dsec.AddressW = D3D10_TEXTURE_ADDRESS_CLAMP;
	pDevice->pDevice->CreateSamplerState(&dsec,&pDevice->pSamplerState[DXSample_Point_Clamp]);
	//////////////////////////////////////////////////////////////////////////
	//SYSample_Point_Wrap,
	dsec.Filter = D3D10_FILTER_MIN_MAG_MIP_POINT ;
	dsec.AddressU = D3D10_TEXTURE_ADDRESS_WRAP ;
	dsec.AddressV = D3D10_TEXTURE_ADDRESS_WRAP ;
	dsec.AddressW = D3D10_TEXTURE_ADDRESS_WRAP;
	pDevice->pDevice->CreateSamplerState(&dsec,&pDevice->pSamplerState[DXSample_Point_Wrap]);
	//////////////////////////////////////////////////////////////////////////
	//SYSample_Point_Mirror,
	dsec.Filter = D3D10_FILTER_MIN_MAG_MIP_POINT ;
	dsec.AddressU = D3D10_TEXTURE_ADDRESS_MIRROR ;
	dsec.AddressV = D3D10_TEXTURE_ADDRESS_MIRROR ;
	dsec.AddressW = D3D10_TEXTURE_ADDRESS_MIRROR;
	pDevice->pDevice->CreateSamplerState(&dsec,&pDevice->pSamplerState[DXSample_Point_Mirror]);
	//////////////////////////////////////////////////////////////////////////
	//SYSample_Point_Border,
	dsec.Filter = D3D10_FILTER_MIN_MAG_MIP_POINT ;
	dsec.AddressU = D3D10_TEXTURE_ADDRESS_BORDER ;
	dsec.AddressV = D3D10_TEXTURE_ADDRESS_BORDER ;
	dsec.AddressW = D3D10_TEXTURE_ADDRESS_BORDER;
	pDevice->pDevice->CreateSamplerState(&dsec,&pDevice->pSamplerState[DXSample_Point_Border]);
	//////////////////////////////////////////////////////////////////////////
	//SYSample_Line_Clamp,
	dsec.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR ;
	dsec.AddressU = D3D10_TEXTURE_ADDRESS_CLAMP ;
	dsec.AddressV = D3D10_TEXTURE_ADDRESS_CLAMP ;
	dsec.AddressW = D3D10_TEXTURE_ADDRESS_CLAMP;
	pDevice->pDevice->CreateSamplerState(&dsec,&pDevice->pSamplerState[DXSample_Line_Clamp]);
	//////////////////////////////////////////////////////////////////////////
	//SYSample_Line_Wrap,
	dsec.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR ;
	dsec.AddressU = D3D10_TEXTURE_ADDRESS_WRAP ;
	dsec.AddressV = D3D10_TEXTURE_ADDRESS_WRAP ;
	dsec.AddressW = D3D10_TEXTURE_ADDRESS_WRAP;
	pDevice->pDevice->CreateSamplerState(&dsec,&pDevice->pSamplerState[DXSample_Line_Wrap]);
	//////////////////////////////////////////////////////////////////////////
	//SYSample_Line_Mirror,
	dsec.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR ;
	dsec.AddressU = D3D10_TEXTURE_ADDRESS_MIRROR ;
	dsec.AddressV = D3D10_TEXTURE_ADDRESS_MIRROR ;
	dsec.AddressW = D3D10_TEXTURE_ADDRESS_MIRROR;
	pDevice->pDevice->CreateSamplerState(&dsec,&pDevice->pSamplerState[DXSample_Line_Mirror]);
	//////////////////////////////////////////////////////////////////////////
	//SYSample_Line_Border,
	dsec.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR ;
	dsec.AddressU = D3D10_TEXTURE_ADDRESS_BORDER ;
	dsec.AddressV = D3D10_TEXTURE_ADDRESS_BORDER ;
	dsec.AddressW = D3D10_TEXTURE_ADDRESS_BORDER;
	pDevice->pDevice->CreateSamplerState(&dsec,&pDevice->pSamplerState[DXSample_Line_Border]);
	//////////////////////////////////////////////////////////////////////////
	//SYSample_Anisotropic_Clamp,
	dsec.Filter = D3D10_FILTER_ANISOTROPIC ;
	dsec.MaxAnisotropy = 16;
	dsec.AddressU = D3D10_TEXTURE_ADDRESS_CLAMP ;
	dsec.AddressV = D3D10_TEXTURE_ADDRESS_CLAMP ;
	dsec.AddressW = D3D10_TEXTURE_ADDRESS_CLAMP;
	pDevice->pDevice->CreateSamplerState(&dsec,&pDevice->pSamplerState[DXSample_Anisotropic_Clamp]);
	//////////////////////////////////////////////////////////////////////////
	//SYSample_Anisotropic_Wrap,
	dsec.Filter = D3D10_FILTER_ANISOTROPIC ;
	dsec.MaxAnisotropy = 16;
	dsec.AddressU = D3D10_TEXTURE_ADDRESS_WRAP ;
	dsec.AddressV = D3D10_TEXTURE_ADDRESS_WRAP ;
	dsec.AddressW = D3D10_TEXTURE_ADDRESS_WRAP;
	pDevice->pDevice->CreateSamplerState(&dsec,&pDevice->pSamplerState[DXSample_Anisotropic_Wrap]);
	//////////////////////////////////////////////////////////////////////////
	//SYSample_Anisotropic_Mirror,
	dsec.Filter = D3D10_FILTER_ANISOTROPIC ;
	dsec.MaxAnisotropy = 16;
	dsec.AddressU = D3D10_TEXTURE_ADDRESS_MIRROR ;
	dsec.AddressV = D3D10_TEXTURE_ADDRESS_MIRROR ;
	dsec.AddressW = D3D10_TEXTURE_ADDRESS_MIRROR;
	pDevice->pDevice->CreateSamplerState(&dsec,&pDevice->pSamplerState[DXSample_Anisotropic_Mirror]);
	//////////////////////////////////////////////////////////////////////////
	//SYSample_Anisotropic_Border,
	dsec.Filter = D3D10_FILTER_ANISOTROPIC ;
	dsec.MaxAnisotropy = 16;
	dsec.AddressU = D3D10_TEXTURE_ADDRESS_BORDER ;
	dsec.AddressV = D3D10_TEXTURE_ADDRESS_BORDER ;
	dsec.AddressW = D3D10_TEXTURE_ADDRESS_BORDER;
	pDevice->pDevice->CreateSamplerState(&dsec,&pDevice->pSamplerState[DXSample_Anisotropic_Border]);
	//////////////////////////////////////////////////////////////////////////
	//SYSample_NoMipMap_Line_Clamp,
	dsec.Filter = D3D10_FILTER_ANISOTROPIC ;
	dsec.MaxAnisotropy = 16;
	dsec.MaxLOD = 0;
	dsec.AddressU = D3D10_TEXTURE_ADDRESS_CLAMP ;
	dsec.AddressV = D3D10_TEXTURE_ADDRESS_CLAMP ;
	dsec.AddressW = D3D10_TEXTURE_ADDRESS_CLAMP;
	pDevice->pDevice->CreateSamplerState(&dsec,&pDevice->pSamplerState[DXSample_NoMipMap_Anisotropic_Clamp]);
	//////////////////////////////////////////////////////////////////////////
	//SYSample_NoMipMap_Line_Wrap,
	dsec.Filter = D3D10_FILTER_ANISOTROPIC ;
	dsec.MaxAnisotropy = 16;
	dsec.MaxLOD = 0;
	dsec.AddressU = D3D10_TEXTURE_ADDRESS_WRAP ;
	dsec.AddressV = D3D10_TEXTURE_ADDRESS_WRAP ;
	dsec.AddressW = D3D10_TEXTURE_ADDRESS_WRAP;
	pDevice->pDevice->CreateSamplerState(&dsec,&pDevice->pSamplerState[DX_NoMipMap_Anisotropic_Wrap]);
	//////////////////////////////////////////////////////////////////////////
	//SYSample_NoMipMap_Line_Mirror,
	dsec.Filter = D3D10_FILTER_ANISOTROPIC ;
	dsec.MaxAnisotropy = 16;
	dsec.MaxLOD = 0;
	dsec.AddressU = D3D10_TEXTURE_ADDRESS_MIRROR ;
	dsec.AddressV = D3D10_TEXTURE_ADDRESS_MIRROR ;
	dsec.AddressW = D3D10_TEXTURE_ADDRESS_MIRROR;
	pDevice->pDevice->CreateSamplerState(&dsec,&pDevice->pSamplerState[DXSample_NoMipMap_Anisotropic_Mirror]);
	//////////////////////////////////////////////////////////////////////////
	//SYSample_NoMipMap_Line_Border,
	dsec.Filter = D3D10_FILTER_ANISOTROPIC ;
	dsec.MaxAnisotropy = 16;
	dsec.MaxLOD = 0;
	dsec.AddressU = D3D10_TEXTURE_ADDRESS_BORDER ;
	dsec.AddressV = D3D10_TEXTURE_ADDRESS_BORDER ;
	dsec.AddressW = D3D10_TEXTURE_ADDRESS_BORDER;
	pDevice->pDevice->CreateSamplerState(&dsec,&pDevice->pSamplerState[DXSample_NoMipMap_Anisotropic_Border]);
	//////////////////////////////////////////////////////////////////////////
}

int hTestPs = 0;
bool CreateDeviceResource(SY_DeviceDX10* pDevice)
{
	pDevice->hScreenVertex = CreateScreenVertex(pDevice);
	pDevice->h2DVertex = Create2DVertex(pDevice);
	screenCB screen;
	float4Set(&screen.texCoord,0,0,1,1);
	matrix4f_identity(&screen.world_View_proj);
	pDevice->hScreenCB = CreateBuffer(pDevice,sizeof(screenCB),1,&screen,DXConstBuffer);
	pDevice->hClearCB = CreateBuffer(pDevice,2 * sizeof(float4),1,NULL,DXConstBuffer);
	pDevice->hModiyCB = CreateBuffer(pDevice,3 * sizeof(float4),1,NULL,DXConstBuffer);
	pDevice->hDynamicCB = CreateBuffer(pDevice,sizeof(DynamicPixCB),1,NULL,DXConstBuffer);

	pDevice->hRenderTexCB = CreateBuffer(pDevice,sizeof(matrix4f),1,NULL,DXConstBuffer);
	pDevice->hRenderTexCB5 = CreateBuffer(pDevice,sizeof(matrix4f) + sizeof(float4),1,NULL,DXConstBuffer);

	CreateBlendState(pDevice);
	CreateDepthState(pDevice);
	CreateRasterizerState(pDevice);
	CreateSampleState(pDevice);

	DXVertexType nType = DXVertex_POS_TEX;

	//TODO djt: 这里边的shader用到哪个加载哪个，不再一次性加载了
	return true;

	TCHAR filePath[512];
	_tcscpy_s(filePath,g_szDX10PathShader);
	_tcscat_s(filePath,_T("screen.vso"));
	pDevice->hScreenVS = CreateShaderFromCompiledFile(pDevice, _T("screen_VS"), filePath, DXShaderType_VertexShader, NULL);

	_tcscpy_s(filePath, g_szDX10PathShader);
	_tcscat_s(filePath, _T("screen.pso"));
	pDevice->hScreenPS = CreateShaderFromCompiledFile(pDevice, _T("screen_PS"), filePath, DXShaderType_PixelShader, NULL);

	_tcscpy_s(filePath, g_szDX10PathShader);
	_tcscat_s(filePath, _T("screenAdjustColor.pso"));
	pDevice->hScreenAdjustPS = CreateShaderFromCompiledFile(pDevice, _T("screenAdjustColor_PS"), filePath, DXShaderType_PixelShader, NULL);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	_tcscpy_s(filePath,g_szDX10PathShader);
	_tcscat_s(filePath,_T("\\PS\\3DVision.PS"));
	pDevice->h3DVisionPS = CreateShaderFromFile(pDevice,_T("3DVison_PS"),filePath,DXShaderType_PixelShader,&nType);

	_tcscpy_s(filePath,g_szDX10PathShader);
	_tcscat_s(filePath,_T("\\PS\\color\\uyvy_bright.ps"));
	pDevice->hUYVYBrightPS = CreateShaderFromFile(pDevice,_T("uyvy_bright_ps"),filePath,DXShaderType_PixelShader,NULL);

	_tcscpy_s(filePath,g_szDX10PathShader);
	_tcscat_s(filePath,_T("\\PS\\color\\field_add_key.PS"));
	pDevice->hFieldAddKeyPS = CreateShaderFromFile(pDevice,_T("field_add_key_PS"),filePath,DXShaderType_PixelShader,NULL);

	_tcscpy_s(filePath,g_szDX10PathShader);
	_tcscat_s(filePath,_T("\\PS\\color\\makeFrame.PS"));
	pDevice->hRGBA_Frame = CreateShaderFromFile(pDevice,_T("makeFrame_PS"),filePath,DXShaderType_PixelShader,NULL);

	_tcscpy_s(filePath,g_szDX10PathShader);
	_tcscat_s(filePath,_T("\\PS\\color\\rgba_to_yuayva4224.PS"));
	pDevice->hRGBA_YUAYVA4224PS = CreateShaderFromFile(pDevice,_T("rgba_to_yuayva4224_PS"),filePath,DXShaderType_PixelShader,NULL);

	_tcscpy_s(filePath,g_szDX10PathShader);
	_tcscat_s(filePath,_T("\\PS\\color\\uyvy_to_rgba_with_key0.PS"));
	pDevice->hUYVY_RGBA_KeyPS0 = CreateShaderFromFile(pDevice,_T("uyvy_to_rgba_with_key0_PS"),filePath,DXShaderType_PixelShader,NULL);

	_tcscpy_s(filePath,g_szDX10PathShader);
	_tcscat_s(filePath,_T("\\PS\\color\\uyvy_to_rgba_with_key1.PS"));
	pDevice->hUYVY_RGBA_KeyPS1 = CreateShaderFromFile(pDevice,_T("uyvy_to_rgba_with_key1_PS"),filePath,DXShaderType_PixelShader,NULL);

	_tcscpy_s(filePath,g_szDX10PathShader);
	_tcscat_s(filePath,_T("\\PS\\color\\uyvy_to_rgba0.PS"));
	pDevice->hUYVY_RGBA_PS0 = CreateShaderFromFile(pDevice,_T("uyvy_to_rgba0_PS"),filePath,DXShaderType_PixelShader,NULL);

	_tcscpy_s(filePath,g_szDX10PathShader);
	_tcscat_s(filePath,_T("\\PS\\color\\uyvy_to_rgba1.PS"));
	pDevice->hUYVY_RGBA_PS1 = CreateShaderFromFile(pDevice,_T("uyvy_to_rgba1_PS"),filePath,DXShaderType_PixelShader,NULL);

	_tcscpy_s(filePath,g_szDX10PathShader);
	_tcscat_s(filePath,_T("\\PS\\color\\uyvyframe_to_rgba.ps"));
	pDevice->hUYVY_RGBA_Frame = CreateShaderFromFile(pDevice,_T("uyvy_to_rgba_frame_PS"),filePath,DXShaderType_PixelShader,NULL);

	_tcscpy_s(filePath,g_szDX10PathShader);
	_tcscat_s(filePath,_T("\\PS\\color\\rgbaframe_to_uyvy.ps"));
	pDevice->hRGBA_UYVY_Frame = CreateShaderFromFile(pDevice,_T("rgba_to_uyvy_frame_PS"),filePath,DXShaderType_PixelShader,NULL);
	
	_tcscpy_s(filePath,g_szDX10PathShader);
	_tcscat_s(filePath,_T("\\PS\\screenMS.PS"));
	pDevice->hScreenMSPS = CreateShaderFromFile(pDevice,_T("screenMS_PS"),filePath,DXShaderType_PixelShader,NULL);

	_tcscpy_s(filePath,g_szDX10PathShader);
	_tcscat_s(filePath,_T("\\PS\\convert.PS"));
	pDevice->hConvertFormatPS = CreateShaderFromFile(pDevice,_T("convert_PS"),filePath,DXShaderType_PixelShader,NULL);

	_tcscpy_s(filePath,g_szDX10PathShader);
	_tcscat_s(filePath,_T("\\PS\\color\\keyout.PS"));
	pDevice->hOnlyKeyOutPS = CreateShaderFromFile(pDevice,_T("keyout_PS"),filePath,DXShaderType_PixelShader,NULL);

	_tcscpy_s(filePath,g_szDX10PathShader);
	_tcscat_s(filePath,_T("\\PS\\DynamicTex.PS"));
	pDevice->hDynamicPS = CreateShaderFromFile(pDevice,_T("dynamic_PS"),filePath,DXShaderType_PixelShader,NULL);

	_tcscpy_s(filePath,g_szDX10PathShader);
	_tcscat_s(filePath,_T("\\PS\\blur.PS"));
	pDevice->hBlur5PtPS = CreateShaderFromFile(pDevice,_T("blur_PS"),filePath,DXShaderType_PixelShader,NULL);

	_tcscpy_s(filePath,g_szDX10PathShader);
	_tcscat_s(filePath,_T("\\PS\\blur9.PS"));
	pDevice->hBlur9PtPS = CreateShaderFromFile(pDevice,_T("blur9_PS"),filePath,DXShaderType_PixelShader,NULL);

	_tcscpy_s(filePath,g_szDX10PathShader);
	_tcscat_s(filePath,_T("\\PS\\clearscreen.PS"));
	pDevice->hClearScreenPS = CreateShaderFromFile(pDevice,_T("clearscreen_PS"),filePath,DXShaderType_PixelShader,NULL);

	_tcscpy_s(filePath,g_szDX10PathShader);
	_tcscat_s(filePath,_T("\\PS\\RenderMask.PS"));
	pDevice->hMaskRender = CreateShaderFromFile(pDevice,_T("RenderMask_PS"),filePath,DXShaderType_PixelShader,NULL);

	_tcscpy_s(filePath,g_szDX10PathShader);
	_tcscat_s(filePath,_T("\\VS\\RenderTex.vs"));
	 
	nType = DXVertex_POS;
	pDevice->hRenderTexVS = CreateShaderFromFile(pDevice,_T("RenderTex_VS"),filePath,DXShaderType_VertexShader,&nType);

	_tcscpy_s(filePath,g_szDX10PathShader);
	_tcscat_s(filePath,_T("\\PS\\RenderTex.ps"));
	pDevice->hRenderTexPS = CreateShaderFromFile(pDevice,_T("RenderTex_PS"),filePath,DXShaderType_PixelShader,NULL);

	return true;
}

bool ReleaseDeviceResource(SY_DeviceDX10* pDevice)
{
	//TODO djt: 这里边的shader用到哪个加载哪个，不再一次性加载了
	ReleaseBuffer(pDevice,pDevice->hScreenVertex);
	ReleaseBuffer(pDevice,pDevice->h2DVertex);
#if 0
	ReleaseBuffer(pDevice,pDevice->hScreenCB);
	ReleaseBuffer(pDevice,pDevice->hDynamicCB);
	ReleaseBuffer(pDevice,pDevice->hClearCB);
	ReleaseBuffer(pDevice,pDevice->hModiyCB);
	ReleaseBuffer(pDevice,pDevice->hRenderTexCB);
	ReleaseBuffer(pDevice,pDevice->hRenderTexCB5);

	ReleaseShader(pDevice,pDevice->hScreenVS);
	ReleaseShader(pDevice,pDevice->h3DVisionPS);
	ReleaseShader(pDevice,pDevice->hScreenPS);
	ReleaseShader(pDevice,pDevice->hScreenAdjustPS);
	ReleaseShader(pDevice,pDevice->hUYVYBrightPS);
	ReleaseShader(pDevice,pDevice->hFieldAddKeyPS);
	ReleaseShader(pDevice,pDevice->hRGBA_Frame);
	ReleaseShader(pDevice,pDevice->hRGBA_YUAYVA4224PS);
	ReleaseShader(pDevice,pDevice->hUYVY_RGBA_KeyPS0);
	ReleaseShader(pDevice,pDevice->hUYVY_RGBA_KeyPS1);
	ReleaseShader(pDevice,pDevice->hUYVY_RGBA_PS0);
	ReleaseShader(pDevice,pDevice->hUYVY_RGBA_PS1);
	ReleaseShader(pDevice,pDevice->hUYVY_RGBA_Frame);
	ReleaseShader(pDevice,pDevice->hRGBA_UYVY_Frame);
	ReleaseShader(pDevice,pDevice->hScreenMSPS);
	ReleaseShader(pDevice,pDevice->hConvertFormatPS);
	ReleaseShader(pDevice,pDevice->hOnlyKeyOutPS);
	ReleaseShader(pDevice,pDevice->hDynamicPS);
	ReleaseShader(pDevice,pDevice->hBlur5PtPS);
	ReleaseShader(pDevice,pDevice->hBlur9PtPS);
	ReleaseShader(pDevice,pDevice->hClearScreenPS);
	ReleaseShader(pDevice,pDevice->hMaskRender);

	ReleaseShader(pDevice,pDevice->hRenderTexVS);
	ReleaseShader(pDevice,pDevice->hRenderTexPS);
#endif

	//play effect support
	if(pDevice->pEffectMan)
	{
		int hVB = pDevice->pEffectMan->GetQuadVB();
		ReleaseBuffer(pDevice,hVB);
		
		SAFE_DELETE(pDevice->pEffectMan);
		SYHTable_RemoveAll(&(pDevice->htEffectUnit));
	}
	return true;
}

bool Render_init(int hDevice,void* paramin,void* paramout)
{
	LPCTSTR lpPathShader = (LPCTSTR)paramin;
	_tcscpy_s(g_szDX10PathShader,lpPathShader);

	_tcscpy_s(g_szDX10PathEffect,lpPathShader);
	_tcscat_s(g_szDX10PathEffect,_T("\\FX"));

	//g_pDXFactory = GetGpuRenderBase()->GetD3D10GIFactory();
	g_pDXFactory = NULL;
	if (!g_pDXFactory)
	{
		if(CreateDXGIFactory(__uuidof(IDXGIFactory),(void**)&g_pDXFactory) == S_OK)
			g_pDXFactory->EnumAdapters(0,&g_pAdapter);
	}
	else
	{
		g_pDXFactory->EnumAdapters(0,&g_pAdapter);
	}

	if (!g_pAdapter)
	{
		AfxMessageBox(_T("Find Proper Adapter Failure!"));
		return false;
	}
	SYHTable_Init(&g_deviceHT);

	InitializeCriticalSection(&g_dx10EngineLock);
	return true;
}

void Render_ReleaseTexture(SYTexture* pWDTexture)
{
	SAFEDELETE_ARRAY(pWDTexture->pTexFile);
	switch (pWDTexture->nType)
	{
	case DXTexture_Depth:
	case DXTexture_MSDepth:
		{
			ID3D10DepthStencilView* pRenderTarget = (ID3D10DepthStencilView*)(pWDTexture->pRenderTarget);
			SAFE_RELEASE(pRenderTarget);
			SAFE_RELEASE(pWDTexture->pResourceView);
			ID3D10Texture2D* pTexture = (ID3D10Texture2D*)(pWDTexture->pTexture);
			SAFE_RELEASE(pTexture);
			SAFE_DELETE(pWDTexture);
			return;
		}
		break;
	case DXTexture_MemTexture:
	case DXTexture_R16G16MemTexture: 
		{
			ID3D10Texture2D* pTexture = (ID3D10Texture2D*)(pWDTexture->pTexture);
			if(pWDTexture->pMemBuffer)
				pTexture->Unmap(0);
		}
		break;
	}

	ID3D10Texture2D* pTexture = (ID3D10Texture2D*)(pWDTexture->pTexture);
	SAFE_RELEASE(pTexture);

	ID3D10RenderTargetView* pRenderTarget = (ID3D10RenderTargetView*)(pWDTexture->pRenderTarget);
	SAFE_RELEASE(pRenderTarget);

	SAFE_RELEASE(pWDTexture->pResourceView);

	SAFE_DELETE(pWDTexture);
}

void ReleaseSwapChainBufferTex(SY_DeviceDX10* pDevice,SYSwapChain* pSwapChain)
{
	SAFE_RELEASE(pSwapChain->pShareTexture);
	if(pSwapChain->hShareTexture > 0)
	{
		SYTexture* pShareTex = (SYTexture*)SYHTable_RemoveAt(&pDevice->httexture,pSwapChain->hShareTexture);
		Render_ReleaseTexture(pShareTex);
	}
	if(pSwapChain->hBufferTexture > 0)
	{
		SYTexture* pBufferTex = (SYTexture*)SYHTable_RemoveAt(&pDevice->httexture,pSwapChain->hBufferTexture);
		Render_ReleaseTexture(pBufferTex);
	}
}
void Render_ReleaseDevice(SY_DeviceDX10* pDevice)
{
	if (!pDevice )
	{
		return;
	}
	ReleaseDeviceResource(pDevice);
	DeleteCriticalSection(&pDevice->EngineLock);
	DeleteCriticalSection(&pDevice->RenderLock);

	UINT firstSwapChain = SYHTable_GetFirstValid(&pDevice->htSwapChain);		
	SYSwapChain* pSwapTexture = (SYSwapChain*)SYHTable_GetAt(&pDevice->htSwapChain,firstSwapChain);
	while(pSwapTexture)
	{
		ReleaseSwapChainBufferTex(pDevice,pSwapTexture);
		SAFE_RELEASE(pSwapTexture->pSwapChain);
		SAFE_DELETE(pSwapTexture);
		pSwapTexture = (SYSwapChain*)SYHTable_GetNextValid(&pDevice->htSwapChain,&firstSwapChain);
	}

	UINT firstTexture = SYHTable_GetFirstValid(&pDevice->httexture);		
	SYTexture* pTexture = (SYTexture*)SYHTable_GetAt(&pDevice->httexture,firstTexture);
	while(pTexture)
	{
		Render_ReleaseTexture(pTexture);
		pTexture = (SYTexture*)SYHTable_GetNextValid(&pDevice->httexture,&firstTexture);
	}

	UINT nShdCount = SYHTable_GetCount(&pDevice->htShaderTable);
	UINT firstShade = SYHTable_GetFirstValid(&pDevice->htShaderTable);		
	while(firstShade)
	{
		int a = 0;
		ReleaseShader(pDevice,firstShade);
		SYHTable_GetNextValid(&pDevice->htShaderTable,&firstShade);
	}
	
	UINT nBufCount = SYHTable_GetCount(&pDevice->htBuffer);
	UINT firstBuf = SYHTable_GetFirstValid(&pDevice->htBuffer);		
	while(firstBuf)
	{
		ReleaseBuffer(pDevice,firstBuf);
		SYHTable_GetNextValid(&pDevice->htBuffer,&firstBuf);
	}

	//窗口销毁后的assert将导致debug错误
	//assert(nShdCount <= 0 && nBufCount <= 0);
	
	SYHash_Delete(pDevice->ShaderHash,TRUE);
	SYHash_Delete(pDevice->TextureFromFileHash,TRUE);
	SYHash_Delete(pDevice->BumpTextureFromFileHash,TRUE);
	
	SYHTable_Release(&pDevice->httexture);
	SYHTable_Release(&pDevice->htModel);
	SYHTable_Release(&pDevice->htShaderTable);
	SYHTable_Release(&pDevice->htBuffer);
	SYHTable_Release(&pDevice->htEffectUnit);
	SYHTable_Release(&pDevice->htSwapChain);

	if(pDevice->pSwapChainDevice != pDevice->pDevice)
		SAFE_RELEASE(pDevice->pSwapChainDevice);

	pDevice->pSwapChainDevice = NULL;
	SAFE_RELEASE(pDevice->pDevice);
	SAFE_DELETE(pDevice);
}

bool Render_Release(int hDevice,void* paramin,void* paramout)
{
	UINT firstDevice = SYHTable_GetFirstValid(&g_deviceHT);		
	SY_DeviceDX10* pNowDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,firstDevice);
	do 
	{
		Render_ReleaseDevice(pNowDevice);
		pNowDevice = (SY_DeviceDX10*)SYHTable_GetNextValid(&g_deviceHT,&firstDevice);
	} while (pNowDevice);

	SYHTable_Release(&g_deviceHT);
	DeleteCriticalSection(&g_dx10EngineLock);
	return true;
}

bool Render_CreateDevice(int hDevice,void* paramin,void* paramout)
{
	BOOL* pDoubleDevice = (BOOL*)paramin;

	SY_DeviceDX10* pNewDevice = NULL;
	pNewDevice = new SY_DeviceDX10;
	D3D10_DRIVER_TYPE driverType = D3D10_DRIVER_TYPE_HARDWARE;

	IDXGIAdapter* selectedAdapter = g_pAdapter;
#if 0
	// Look for 'NVIDIA PerfHUD' adapter
	// If it is present, override default settings
	// Search for a PerfHUD adapter.  
	UINT nAdapter = 0;
	IDXGIAdapter* adapter = NULL;

	while (g_pDXFactory->EnumAdapters(nAdapter, &adapter) != 
		DXGI_ERROR_NOT_FOUND)
	{
		if (adapter)
		{
			DXGI_ADAPTER_DESC adaptDesc;
			if (SUCCEEDED(adapter->GetDesc(&adaptDesc)))
			{
				const bool isPerfHUD = wcscmp(adaptDesc.Description, L"NVIDIA PerfHUD") == 0;
				// Select the first adapter in normal circumstances or the PerfHUD one if it exists.
					if(nAdapter == 0 || isPerfHUD)
						selectedAdapter = adapter;
				if(isPerfHUD)
					driverType = D3D10_DRIVER_TYPE_REFERENCE;
			}
		}
		++nAdapter;
	}
#endif
//创建调试层级
UINT createFlag=D3D10_CREATE_DEVICE_SINGLETHREADED;
#ifdef _DEBUG
	//createFlag |=D3D10_CREATE_DEVICE_DEBUG;
#endif
	HRESULT hr=D3D10CreateDevice(selectedAdapter,driverType,NULL,createFlag,D3D10_SDK_VERSION,&pNewDevice->pDevice);
	if(!pNewDevice->pDevice)
	{
		SAFE_DELETE(pNewDevice);
		CString strInfo;
		strInfo.Format(_T("D3D10CreateDevice Failure! ErrorCode=%0xd "),hr);
		AfxMessageBox(strInfo);
		return false;
	}
	if(*pDoubleDevice)
	{
		D3D10CreateDevice(selectedAdapter,driverType,NULL,createFlag ,D3D10_SDK_VERSION,&pNewDevice->pSwapChainDevice);
		if(!pNewDevice->pDevice)
		{
			SAFE_DELETE(pNewDevice);
			return false;
		}
	}
	else
		pNewDevice->pSwapChainDevice = pNewDevice->pDevice;
	pNewDevice->nMutiSampleCount = 4;
	//init device
	UINT nTexQuality = 0;
	hr = pNewDevice->pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM,pNewDevice->nMutiSampleCount,&nTexQuality);
	if (FAILED(hr))
	{
		SAFE_DELETE(pNewDevice);
		CString strInfo;
		strInfo.Format(_T("CheckMultisampleQualityLevels Failure! ErrorCode=%0xd "),hr);
		AfxMessageBox(strInfo);
		return false;
	}
	UINT nDepthQuality = 0;
	hr = pNewDevice->pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_D24_UNORM_S8_UINT,pNewDevice->nMutiSampleCount,&nDepthQuality);
	if (FAILED(hr))
	{
		SAFE_DELETE(pNewDevice);
		CString strInfo;
		strInfo.Format(_T("CheckMultisampleQualityLevels Failure! ErrorCode=%0xd "),hr);
		AfxMessageBox(strInfo);
		return false;
	}
	pNewDevice->nSampleQuality = min(nTexQuality,nDepthQuality);
	if (pNewDevice->nSampleQuality == 0)
	{
		pNewDevice->nMutiSampleCount = 1;
	}
	pNewDevice->nSampleQuality = max(0,pNewDevice->nSampleQuality - 1);

//	D3DX10CreateSprite(pNewDevice->pDevice,64,&pNewDevice->pSprite);

	SYHTable_Init(&pNewDevice->httexture);
	SYHTable_Init(&pNewDevice->htModel);
	SYHTable_Init(&pNewDevice->htShaderTable);
	SYHTable_Init(&pNewDevice->htSwapChain);

	pNewDevice->ShaderHash = SYHash_Create(1,NULL,NULL, NULL, NULL, ShaderHashRelease);
	pNewDevice->TextureFromFileHash = SYHash_Create(1,NULL,NULL, NULL, NULL, ShaderHashRelease);
	pNewDevice->BumpTextureFromFileHash = SYHash_Create(1,NULL,NULL, NULL, NULL, ShaderHashRelease);
	CreateDeviceResource(pNewDevice);
	InitializeCriticalSection(&pNewDevice->EngineLock);
	InitializeCriticalSection(&pNewDevice->RenderLock);


	//D3DX10CreateThreadPump(0,0,&pNewDevice->pThreadPump);
	int hNewDevice = (int)SYHTable_Insert(&g_deviceHT,pNewDevice);
	*(int*)paramout = hNewDevice;

	//play effect support
	pNewDevice->pEffectMan = new SYD3DEffectManager(pNewDevice->pDevice);
	SYHTable_Init(&pNewDevice->htEffectUnit);
	return true;
}

bool Render_GetSwapChainDesc(int hDevice,void* paramin,void* paramout)
{
	createSwapChainIn* pIn = (createSwapChainIn*)paramin;
	createSwapChainIn* pOut = (createSwapChainIn*)paramout;
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	SYSwapChain* pWDSwapChain = (SYSwapChain*)SYHTable_GetAt(&pDevice->htSwapChain,(int)pIn->hWnd);
	if(pWDSwapChain)
	{
		DXGI_SWAP_CHAIN_DESC Desc;
		pWDSwapChain->pSwapChain->GetDesc(&Desc);
		pOut->bWindowed = Desc.Windowed;
		pOut->nWidth = Desc.BufferDesc.Width;
		pOut->nHeight = Desc.BufferDesc.Height;
	
		return true;
	}
	return false;
}

bool Render_CreateTexture(int hDevice,void* paramin,void* paramout);
void CreateSwapChainBufferTex(int hDevice,int nWidth,int nHeight,SYSwapChain* pSwapChain)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	if(pDevice->pSwapChainDevice != pDevice->pDevice)
	{
		createTextureIn texIn;
		texIn.nWidth = nWidth;
		texIn.nHeight = nHeight;
		texIn.nType = DXTexture_ShareBuffer;
		texIn.pBuffer = NULL;
		Render_CreateTexture(hDevice,&texIn,&pSwapChain->hShareTexture);
		ID3D10Texture2D* pShareTex = getTexture(pDevice,pSwapChain->hShareTexture);

		IDXGIResource* pOtherResource(NULL);
		pShareTex->QueryInterface( __uuidof(IDXGIResource), (void**)&pOtherResource );
		HANDLE sharedHandle;
		pOtherResource->GetSharedHandle(&sharedHandle);
		pDevice->pSwapChainDevice->OpenSharedResource(sharedHandle,__uuidof(ID3D10Texture2D),(void**)&pSwapChain->pShareTexture);
	}

	SYTexture* pSwapChainBuffer = new SYTexture;
	memset(pSwapChainBuffer,0,sizeof(SYTexture));
	pSwapChainBuffer->nNeedWidth = pSwapChainBuffer->nRealWidth = nWidth;
	pSwapChainBuffer->nNeedHeight = pSwapChainBuffer->nRealHeight = nHeight;
	ID3D10Texture2D* m_pTexture = NULL;
	pSwapChain->pSwapChain->GetBuffer( 0, __uuidof( ID3D10Texture2D ), (LPVOID*)&m_pTexture );
	pSwapChainBuffer->pTexture = m_pTexture;
	ID3D10RenderTargetView* pRenderTarget = NULL;
	pDevice->pSwapChainDevice->CreateRenderTargetView(m_pTexture,NULL,&pRenderTarget);
	pSwapChainBuffer->pRenderTarget = pRenderTarget;
	pSwapChainBuffer->nType = DXTexture_Normal2DTex;

	pSwapChain->hBufferTexture = (int)SYHTable_Insert(&pDevice->httexture,pSwapChainBuffer);
}

bool Render_CreateSwapChain(int hDevice,void* paramin,void* paramout)
{
	createSwapChainIn* pIn = (createSwapChainIn*)paramin;
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	DXGI_SWAP_CHAIN_DESC Desc;
	memset(&Desc,0,sizeof(DXGI_SWAP_CHAIN_DESC));
	Desc.BufferCount = 3;
	Desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Desc.BufferDesc.Height = pIn->nHeight;
	Desc.BufferDesc.Width = pIn->nWidth;
	Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	Desc.OutputWindow = pIn->hWnd;
	Desc.BufferDesc.RefreshRate.Numerator = 0;
	Desc.BufferDesc.RefreshRate.Denominator = 0;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality =  0;
	Desc.Windowed = pIn->bWindowed; //modify by 
	Desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	Desc.Flags = 0;
	if(!pIn->bWindowed)
		Desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	IDXGISwapChain* pSwapChain = NULL;
	HRESULT hr = g_pDXFactory->CreateSwapChain(pDevice->pSwapChainDevice,&Desc,&pSwapChain);
	if (!pSwapChain)
		return false; 

	SYSwapChain* pWDSwapChain = new SYSwapChain;
	memset(pWDSwapChain,0,sizeof(SYSwapChain));
	pWDSwapChain->pSwapChain = pSwapChain;
	CreateSwapChainBufferTex(hDevice,pIn->nWidth,pIn->nHeight,pWDSwapChain);
	int hNewSwapChain = (int)SYHTable_Insert(&pDevice->htSwapChain,pWDSwapChain);
	*(int*)paramout = hNewSwapChain;
	if(!pIn->bWindowed)
		hr =pSwapChain->SetFullscreenState(TRUE,NULL);
	return true;
}

bool render_DumpTexture(int hDevice,void* paramin,void* paramout);
bool Render_UpdateSwapChain(int hDevice,int hSwapChain)
{
	CDX10EngineLock lock;
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	if(pDevice->pDevice == pDevice->pSwapChainDevice)
		return false;
	SYSwapChain* pTexture = (SYSwapChain*)SYHTable_GetAt(&pDevice->htSwapChain,hSwapChain);
	pDevice->pDevice->Flush();

	DXGI_SWAP_CHAIN_DESC Desc;
	pTexture->pSwapChain->GetDesc(&Desc);

	D3D10_BOX Box = {0,0,0, Desc.BufferDesc.Width,Desc.BufferDesc.Height,1};
	pDevice->pSwapChainDevice->CopySubresourceRegion(getTexture(pDevice,pTexture->hBufferTexture),0,0,0,0, pTexture->pShareTexture,0,&Box);
	if(0)
	{
		DXDumpIn dump;
		memset(&dump,0,sizeof(DXDumpIn));
		dump.hTexture = pTexture->hBufferTexture;
		dump.hTexture = pTexture->hShareTexture;
		render_DumpTexture(hDevice,&dump,0);
	}
	return true;
}

int Render_Present(int hDevice,void* paramin,void* paramout)
{
	if(!hDevice)
		return S_FALSE;
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	int* pHSwapChain = (int*)paramin;
	int* pTimeSyn = (int*)paramout;
	if(*pHSwapChain == 0)
	{
		EngineThreadLock();
		pDevice->pDevice->Flush();
		EngineThreadUnLock();
		return S_OK;
	}

	if(pDevice->pSwapChainDevice == pDevice->pDevice)
		EngineThreadLock();
	Render_UpdateSwapChain(hDevice,*pHSwapChain);
	SYSwapChain* pTexture = (SYSwapChain*)SYHTable_GetAt(&pDevice->htSwapChain,*pHSwapChain);
	//pDevice->pDevice->Flush();
	HRESULT hr = pTexture->pSwapChain->Present(*pTimeSyn,0);
	if(pDevice->pSwapChainDevice == pDevice->pDevice)
		EngineThreadUnLock();

	return hr;
}

bool Render_SetRenderTarget(int hDevice,void* paramin,void* paramout)
{
	setRenderTarget* param = (setRenderTarget*)paramin;
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	SetRenderTarget(pDevice,param->hRenderTarget,param->hDepth);
	return TRUE;
}

void GetTexScale(UINT& nWidth,UINT& nHeight,int nMultiSample,int nMipMap)
{
	//宽高不能超过8192;
	float nMaxWidth = 8192 / sqrt((float)nMultiSample );
	float fScale = min(1,(float)nMaxWidth / max(nWidth,nHeight));
	nWidth *= fScale;
	nHeight *= fScale;
}

inline D3D10_TEXTURE2D_DESC* createDepthDesc(int nWidth,int nHeight)
{
	D3D10_TEXTURE2D_DESC* pDesc = new D3D10_TEXTURE2D_DESC;
	memset(pDesc,0,sizeof(D3D10_TEXTURE2D_DESC));
	pDesc->MipLevels = 1;
	pDesc->Width = nWidth;
	pDesc->Height = nHeight;
	pDesc->ArraySize = 1;
	pDesc->Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	pDesc->SampleDesc.Count = 1;
	pDesc->Usage = D3D10_USAGE_DEFAULT;
	pDesc->BindFlags = D3D10_BIND_DEPTH_STENCIL;
	return pDesc;
}

inline D3D10_TEXTURE2D_DESC* createMemTexDesc(int nWidth,int nHeight)
{
	D3D10_TEXTURE2D_DESC* pDesc = new D3D10_TEXTURE2D_DESC;
	memset(pDesc,0,sizeof(D3D10_TEXTURE2D_DESC));
	pDesc->MipLevels = 1;
	pDesc->Width = nWidth;
	pDesc->Height = nHeight;
	pDesc->ArraySize = 1;
	pDesc->CPUAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;
	pDesc->Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	pDesc->SampleDesc.Count = 1;
	pDesc->Usage = D3D10_USAGE_STAGING;
	//经测试不支持D3D10_BIND_RENDER_TARGET;D3D10_BIND_SHADER_RESOURCE
	//pDesc->BindFlags = D3D10_BIND_SHADER_RESOURCE;
	return pDesc;
}

inline D3D10_TEXTURE2D_DESC* createNormal2DTexDesc(int nWidth,int nHeight)
{
	D3D10_TEXTURE2D_DESC* pDesc = new D3D10_TEXTURE2D_DESC;
	memset(pDesc,0,sizeof(D3D10_TEXTURE2D_DESC));
	pDesc->MipLevels = 1;
	pDesc->Width = nWidth;
	pDesc->Height = nHeight;
	pDesc->ArraySize = 1;
	pDesc->Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	pDesc->BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE ;
	pDesc->SampleDesc.Count = 1;
	pDesc->Usage = D3D10_USAGE_DEFAULT;
	return pDesc;
}

inline D3D10_TEXTURE2D_DESC* createR16G16MemTexDesc(int nWidth,int nHeight)
{
	D3D10_TEXTURE2D_DESC* pDesc = new D3D10_TEXTURE2D_DESC;
	memset(pDesc,0,sizeof(D3D10_TEXTURE2D_DESC));
	pDesc->MipLevels = 1;
	pDesc->Width = nWidth;
	pDesc->Height = nHeight;
	pDesc->ArraySize = 1;
	pDesc->CPUAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;
	pDesc->Format = DXGI_FORMAT_R16G16_SNORM;
	pDesc->SampleDesc.Count = 1;
	pDesc->Usage = D3D10_USAGE_STAGING;
	//经测试不支持D3D10_BIND_RENDER_TARGET;D3D10_BIND_SHADER_RESOURCE
	//pDesc->BindFlags = D3D10_BIND_SHADER_RESOURCE;
	return pDesc;
}

inline D3D10_TEXTURE2D_DESC* createR16G16TexDesc(int nWidth,int nHeight)
{
	D3D10_TEXTURE2D_DESC* pDesc = new D3D10_TEXTURE2D_DESC;
	memset(pDesc,0,sizeof(D3D10_TEXTURE2D_DESC));
	pDesc->MipLevels = 1;
	pDesc->Width = nWidth;
	pDesc->Height = nHeight;
	pDesc->ArraySize = 1;
	pDesc->Format = DXGI_FORMAT_R16G16_SNORM;
	pDesc->BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE ;
	pDesc->SampleDesc.Count = 1;
	pDesc->Usage = D3D10_USAGE_DEFAULT;
	return pDesc;
}

inline D3D10_TEXTURE2D_DESC* createMS2DTexDesc(int nWidth,int nHeight,int nMutiSampleCount, int nQuality)
{
	D3D10_TEXTURE2D_DESC* pDesc = new D3D10_TEXTURE2D_DESC;
	memset(pDesc,0,sizeof(D3D10_TEXTURE2D_DESC));
	pDesc->MipLevels = 1;
	pDesc->Width = nWidth;
	pDesc->Height = nHeight;
	pDesc->ArraySize = 1;
	pDesc->Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	pDesc->BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
	pDesc->SampleDesc.Count = nMutiSampleCount;
	pDesc->SampleDesc.Quality = nQuality;
	pDesc->Usage = D3D10_USAGE_DEFAULT;
	return pDesc;
}
inline D3D10_TEXTURE2D_DESC* createMS2DDepthDesc(int nWidth,int nHeight,int nMutiSampleCount, int nQuality)
{
	D3D10_TEXTURE2D_DESC* pDesc = new D3D10_TEXTURE2D_DESC;
	memset(pDesc,0,sizeof(D3D10_TEXTURE2D_DESC));
	pDesc->Width = nWidth;
	pDesc->Height = nHeight;
	pDesc->MipLevels = 1;
	pDesc->ArraySize = 1;
	pDesc->Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// DXGI_FORMAT_R24G8_TYPELESS;
	pDesc->SampleDesc.Count = nMutiSampleCount;
	pDesc->SampleDesc.Quality = nQuality;
	pDesc->Usage = D3D10_USAGE_DEFAULT;
	pDesc->BindFlags = D3D10_BIND_DEPTH_STENCIL ;
	return pDesc;
}
inline D3D10_TEXTURE2D_DESC* createShareBufferDesc(int nWidth,int nHeight)
{
	D3D10_TEXTURE2D_DESC* pDesc = new D3D10_TEXTURE2D_DESC;
	memset(pDesc,0,sizeof(D3D10_TEXTURE2D_DESC));
	pDesc->MipLevels = 1;
	pDesc->Width = nWidth;
	pDesc->Height = nHeight;
	pDesc->ArraySize = 1;
	pDesc->Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	pDesc->BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE ;
	pDesc->MiscFlags = D3D10_RESOURCE_MISC_SHARED;
	pDesc->SampleDesc.Count = 1;
	pDesc->Usage = D3D10_USAGE_DEFAULT;
	return pDesc;
}

inline D3D10_TEXTURE2D_DESC* createMipMapDesc(int nWidth,int nHeight)
{
	D3D10_TEXTURE2D_DESC* pDesc = new D3D10_TEXTURE2D_DESC;
	memset(pDesc,0,sizeof(D3D10_TEXTURE2D_DESC));
	pDesc->MipLevels = 4;
	pDesc->Width = nWidth;
	pDesc->Height = nHeight;
	pDesc->ArraySize = 1;
	pDesc->Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	pDesc->BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE ;
	pDesc->MiscFlags = D3D10_RESOURCE_MISC_GENERATE_MIPS ;
	pDesc->SampleDesc.Count = 1;
	pDesc->Usage = D3D10_USAGE_DEFAULT;
	return pDesc;
}
inline D3D10_TEXTURE2D_DESC* createReadOnlyDesc(int nWidth,int nHeight,int nMipLevels)
{
	D3D10_TEXTURE2D_DESC* pDesc = new D3D10_TEXTURE2D_DESC;
	memset(pDesc,0,sizeof(D3D10_TEXTURE2D_DESC));
	pDesc->MipLevels = nMipLevels;
	pDesc->Width = nWidth;
	pDesc->Height = nHeight;
	pDesc->ArraySize = 1;
	pDesc->Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	pDesc->BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE ; //D3D10_RESOURCE_MISC_GENERATE_MIPS 需要D3D10_BIND_RENDER_TARGET
	pDesc->MiscFlags = D3D10_RESOURCE_MISC_GENERATE_MIPS ;
	pDesc->SampleDesc.Count = 1;
	pDesc->Usage = D3D10_USAGE_DEFAULT;
	return pDesc;
}

bool Render_CreateTexture(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	createTextureIn* param = (createTextureIn*)paramin;
	D3D10_TEXTURE2D_DESC* pDesc = NULL;
	switch (param->nType)
	{
	case DXTexture_Depth:
		pDesc = createDepthDesc(param->nWidth,param->nHeight);
		break;
	case DXTexture_MemTexture:
		pDesc = createMemTexDesc(param->nWidth,param->nHeight);
		break;
	case DXTexture_Normal2DTex:
		pDesc = createNormal2DTexDesc(param->nWidth,param->nHeight);
		break;
	case DXTexture_MS2DTex:
		pDesc = createMS2DTexDesc(param->nWidth,param->nHeight,pDevice->nMutiSampleCount, pDevice->nSampleQuality);
		break;
	case DXTexture_MSDepth:
		pDesc = createMS2DDepthDesc(param->nWidth,param->nHeight,pDevice->nMutiSampleCount, pDevice->nSampleQuality);
		break;
	case DXTexture_MipMap:
		pDesc = createMipMapDesc(param->nWidth,param->nHeight);
		break;
	case DXTexture_ShareBuffer:
		pDesc = createShareBufferDesc(param->nWidth,param->nHeight);
		break;
	case DXTexture_R16G16Tex:
		pDesc = createR16G16TexDesc(param->nWidth,param->nHeight);
		break;
	case DXTexture_R16G16MemTexture:
		pDesc = createR16G16MemTexDesc(param->nWidth,param->nHeight);
		break;
	}
	if(!pDesc)
		return false;

	GetTexScale(pDesc->Width,pDesc->Height,pDesc->SampleDesc.Count,pDesc->MipLevels);
	ID3D10Texture2D* pNewTexture = NULL;
	//pDesc->Width  = min(8192,pDesc->Width);
	//pDesc->Height  = min(8192,pDesc->Height);
	D3D10_SUBRESOURCE_DATA data;
	data.pSysMem = param->pBuffer;
	data.SysMemPitch = sizeof(DWORD) * param->nWidth;
	data.SysMemSlicePitch = 0;

	HRESULT hr = pDevice->pDevice->CreateTexture2D(pDesc,param->pBuffer?&data:NULL,&pNewTexture);
	if(!pNewTexture)
	{
		*(int*)paramout = 0;
		return false;
	}

	SYTexture* pNewTex = new SYTexture;
	memset(pNewTex,0,sizeof(SYTexture));

	pNewTex->pTexture = pNewTexture;
	pNewTex->nType = param->nType;
	pNewTex->nNeedWidth = param->nWidth;
	pNewTex->nNeedHeight = param->nHeight;
	pNewTex->nRealWidth = pDesc->Width;
	pNewTex->nRealHeight = pDesc->Height;

	//创建renderTarget
	switch (param->nType)
	{
	case DXTexture_Depth:
	case DXTexture_MSDepth:
		{
			ID3D10DepthStencilView* pRenderTarget = NULL;

			HRESULT hr = pDevice->pDevice->CreateDepthStencilView( pNewTexture, NULL, &pRenderTarget );
			pNewTex->pRenderTarget = pRenderTarget;
			pNewTex->pResourceView = NULL;
		}
		break;
	case DXTexture_MemTexture:
	case DXTexture_R16G16MemTexture:
		pNewTex->pRenderTarget = NULL;
		pNewTex->pResourceView = NULL;
		break;
	default:
		{
			ID3D10RenderTargetView* pRenderTarget = NULL;

			HRESULT hr = pDevice->pDevice->CreateRenderTargetView( pNewTexture, NULL, &pRenderTarget );
			pNewTex->pRenderTarget = pRenderTarget;
			pDevice->pDevice->CreateShaderResourceView(pNewTexture,NULL,&(pNewTex->pResourceView));
		}
		break;
	}

	int hNewTex = (int)SYHTable_Insert(&pDevice->httexture,pNewTex);
	*(int*)paramout = hNewTex;

	SAFE_DELETE(pDesc);
	return true;
}

bool render_ClearTexture(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	clearTextureIn* param = (clearTextureIn*)paramin;
	SYTexture* pTexture = (SYTexture*)SYHTable_GetAt(&pDevice->httexture,param->hTexture);
	if(!pTexture)
		return false;

	switch (pTexture->nType)
	{
	case DXTexture_Normal2DTex:
	case DXTexture_MS2DTex:
    case DXTexture_MipMap:
    case DXTexture_ShareBuffer:
		{
			ID3D10RenderTargetView* pCurTargetView10 = (ID3D10RenderTargetView*)(pTexture->pRenderTarget);
			D3DXCOLOR m_color(*(DWORD*)(param->pData)) ;
			pDevice->pDevice->ClearRenderTargetView(pCurTargetView10,m_color);
		}
		break;
	case DXTexture_MemTexture:
	case DXTexture_R16G16MemTexture:
		{
			ID3D10Texture2D* pD3DTexture = (ID3D10Texture2D*)pTexture->pTexture;
			D3D10_MAPPED_TEXTURE2D mapTex;
			pD3DTexture->Map(0,D3D10_MAP_WRITE,0,&mapTex );
			if(mapTex.pData) 
				memset(mapTex.pData,0,mapTex.RowPitch * pTexture->nRealHeight);
			pD3DTexture->Unmap(0);
			//ASSERT(0);

		}
		break;
	case DXTexture_Depth:
	case DXTexture_MSDepth:
		{
			ID3D10DepthStencilView* pCurDepth = (ID3D10DepthStencilView*)(pTexture->pRenderTarget);
			float fDepth = *(float*)(param->pData);
			pDevice->pDevice->ClearDepthStencilView(pCurDepth,D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL,fDepth , 0);
			if (0)
			{
				//ID3D10Texture2D* pHalTexture = pTexture->pTexture;
				//pHalTexture->Map(0)
			}
		}
		break;
	}
	return true;
}

bool Render_GetTextureView(int hDevice,void* paramin,void* paramout)
{
	void** pDXTexture = (void**)paramout;
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	SYTexture* pTexture = (SYTexture*)SYHTable_GetAt(&pDevice->httexture,*(int*)paramin);
	if(pTexture)
	{
		*pDXTexture = pTexture->pResourceView;
		return true;
	}
	*pDXTexture  = NULL;
	
	
	return false;
}

bool render_GetTexture(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	SYTexture* pTexture = (SYTexture*)SYHTable_GetAt(&pDevice->httexture,*(int*)paramin);
	void** pDXTexture = (void**)paramout;
	*pDXTexture = pTexture->pTexture;
	return true;
}



bool render_GetTextureInfo(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	getTextureInfoOut* pOut = (getTextureInfoOut*)paramout;
	getTextureInfo(pDevice,*(int*)paramin,pOut->nWidth,pOut->nHeight,pOut->nMipMap); 

	SYTexture* pSrcWDTexture = (SYTexture*)SYHTable_GetAt(&pDevice->httexture,*(int*)paramin);
	if (pOut->nType)
		*pOut->nType = pSrcWDTexture->nType;
	return true;
}


bool Render_CopyTexture(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	copyTexture* param = (copyTexture*)paramin;
	float xScale = 1,yScale = 1;
	
	int nSrcBufferWidth, nSrcBufferHeight ;
	getTextureInfo(pDevice,param->hSrcTex,&nSrcBufferWidth,&nSrcBufferHeight,NULL);

	int nDestBufferWidth,nDestBufferHeight;
	getTextureInfo(pDevice,param->hDesTex,&nDestBufferWidth,&nDestBufferHeight,NULL);

	cutStrechArea(param->nSrcOffsetX,param->nSrcOffsetY,param->nSrcWidth,param->nSrcHeight,nSrcBufferWidth,nSrcBufferHeight,
		param->nDesOffsetX,param->nDesOffsetY,param->nDesWidth,param->nDesHeight,nDestBufferWidth,nDestBufferHeight,
		&xScale,&yScale);

	if(param->nSrcWidth <= 0
		||param->nSrcHeight<= 0
		||param->nDesWidth<= 0
		||param->nDesHeight<= 0)
		return false;

	float fSrcScale = 1;
	float fDstScale = 1;
	IsNeedScaleTexParam(pDevice,param->hSrcTex,fSrcScale);
	IsNeedScaleTexParam(pDevice,param->hDesTex,fDstScale);
	xScale *= fDstScale / fSrcScale;
	yScale *= fDstScale / fSrcScale;
	SYTexture* pSrcWDTexture = (SYTexture*)SYHTable_GetAt(&pDevice->httexture,param->hSrcTex);
	SYTexture* pDstWDTexture = (SYTexture*)SYHTable_GetAt(&pDevice->httexture,param->hDesTex);
	if (pSrcWDTexture && pDstWDTexture)
	{
		BOOL bCanCopy = (xScale == 1 && yScale == 1);
		bCanCopy &= (pSrcWDTexture->nType != DXTexture_MS2DTex && pSrcWDTexture->nType != DXTexture_MSDepth);
		bCanCopy &= (pDstWDTexture->nType != DXTexture_MS2DTex && pDstWDTexture->nType != DXTexture_MSDepth);
		if (bCanCopy)
		{
			D3D10_BOX Box = {param->nSrcOffsetX*fSrcScale,param->nSrcOffsetY*fSrcScale,0,
				(param->nSrcOffsetX + param->nSrcWidth)*fSrcScale,(param->nSrcOffsetY + param->nSrcHeight)*fSrcScale,1};
			ID3D10Texture2D* pDestTexture = getTexture(pDevice,param->hDesTex);
			ID3D10Texture2D* pSrcTexture = getTexture(pDevice,param->hSrcTex);
			pDevice->pDevice->CopySubresourceRegion(pDestTexture,0,param->nDesOffsetX*fDstScale,param->nDesOffsetY * fDstScale,0,
				pSrcTexture,0,&Box);
			pDevice->info.nRenderPixCount += SY_FToI(param->nDesWidth * param->nDesHeight * fDstScale * fDstScale);
		}
		else
		{
			//出于效率考虑，并不支持SYTexture_MemTexture的放缩拷贝
			if ( !pSrcWDTexture->pResourceView || !pDstWDTexture->pRenderTarget)
			{
				return false;
			}
			int oldRt = pDevice->hNowRenderTarget;
			int oldDepth = pDevice->hNowDepth;
			RECT oldView = pDevice->rtViewport;
			RECT oldScissor = pDevice->rtScissor;
			SetRenderTarget(pDevice,param->hDesTex,0);
			SetScissor(pDevice,&pDevice->rtViewport);
			BeginRenderTexture(pDevice);
			SetRenderState(pDevice,DXBlendState,DXBlendDisable);

			RECT srcRt = {param->nSrcOffsetX,param->nSrcOffsetY,param->nSrcOffsetX + param->nSrcWidth,param->nSrcOffsetY + param->nSrcHeight};
			RECT destRt = {param->nDesOffsetX,param->nDesOffsetY,param->nDesOffsetX + param->nDesWidth,param->nDesOffsetY + param->nDesHeight};
			RenderTexture(pDevice,param->hSrcTex,1,srcRt,destRt,nSrcBufferWidth,nSrcBufferHeight,nDestBufferWidth,nDestBufferHeight,FALSE,pSrcWDTexture->nType);
			EndRenderTexture(pDevice);
			SetRenderTarget(pDevice,oldRt,oldDepth);
			SetViewPort(pDevice,oldView.left,oldView.top,oldView.right,oldView.bottom);
			SetScissor(pDevice,&oldScissor);
			pDevice->info.nRenderPixCount += param->nDesWidth * param->nDesHeight;
		}
		return true;
	}
	return false;
}

bool Render_ReleaseTexture(int hDevice,void* paramin,void* paramout);
bool render_DumpTexture(int hDevice,void* paramin,void* paramout)
{
	LPCTSTR szFile = ((DXDumpIn*)paramin)->szFile;
	int hTexture = ((DXDumpIn*)paramin)->hTexture;
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	ID3D10Texture2D* pTexture = getTexture(pDevice,hTexture);

	SYTexture* pSrcWDTexture = (SYTexture*)SYHTable_GetAt(&pDevice->httexture,*(int*)paramin);
	if(pSrcWDTexture->nType == DXTexture_MS2DTex )
	{
		int nSrcWidth = pSrcWDTexture->nNeedWidth;
		int nSrcHeight = pSrcWDTexture->nNeedHeight;
		createTextureIn param;
		param.nWidth = nSrcWidth;
		param.nHeight = nSrcHeight;
		param.nType = DXTexture_Normal2DTex;
		param.pBuffer = NULL;
		int hTestTex;
		Render_CreateTexture(hDevice,&param,&hTestTex);
		ID3D10Texture2D* pTestTex = getTexture(pDevice,hTestTex);
		pDevice->pDevice->ResolveSubresource(pTestTex,0,pTexture,0,DXGI_FORMAT_R8G8B8A8_UNORM);
		static const TCHAR* path = _T("e:\\testTex.bmp");
		HRESULT hr = D3DX10SaveTextureToFile(pTestTex,D3DX10_IFF_BMP,szFile==NULL?path:szFile);
		Render_ReleaseTexture(hDevice,&hTestTex,NULL);
		return true;
	}
	else if(pSrcWDTexture->nType == DXTexture_R16G16Tex)
	{
		static const TCHAR* path = _T("e:\\testTex.dds");
		HRESULT hr = D3DX10SaveTextureToFile(pTexture,D3DX10_IFF_DDS,szFile==NULL?path:szFile);
		int i =0;
	}

	//CString path(L"e:\\testTex.bmp");
	static const TCHAR* path = _T("e:\\testTex.bmp");
	HRESULT hr = D3DX10SaveTextureToFile(pTexture,D3DX10_IFF_BMP,szFile==NULL?path:szFile);
	if(0)
	{
		const TCHAR* pathTif = _T("e:\\testTex.tif");
		hr = D3DX10SaveTextureToFile(pTexture,D3DX10_IFF_TIFF,pathTif);
	}
	if(hr!=S_OK)
	{
		hr=pDevice->pDevice->GetDeviceRemovedReason();
		CString strInfo;
		strInfo.Format(_T("GetDeviceRemovedReason ErrorCode=%0xd "),hr);
		AfxMessageBox(strInfo);
	}
	return true;
}
bool Render_ReleaseSwapChain(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	SYSwapChain* pSrcWDTexture = (SYSwapChain*)SYHTable_RemoveAt(&pDevice->htSwapChain,*(int*)paramin);
	ReleaseSwapChainBufferTex(pDevice,pSrcWDTexture);

	SAFE_RELEASE(pSrcWDTexture->pSwapChain);
	SAFE_DELETE(pSrcWDTexture);

	return true;
}

void ClearTexState(SY_DeviceDX10* pDevice,int hTex) 
{
	if(pDevice->hNowDepth == hTex)
		pDevice->hNowDepth = 0;
	if(pDevice->hNowRenderTarget == hTex)
		pDevice->hNowRenderTarget = 0;
	for(int i  =0; i < 3; i++)
	{
		for(int j = 0; j < 128; j++)
		{
			if(pDevice->nShaderTexstate[i][j] == hTex)
				pDevice->nShaderTexstate[i][j] = 0;
		}
	}
}
bool Render_ReleaseTexture(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	SYTexture* pSrcWDTexture = (SYTexture*)SYHTable_GetAt(&pDevice->httexture,*(int*)paramin);
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
		Render_ReleaseTexture(pSrcWDTexture);
	}
	else if(pSrcWDTexture)
		Render_ReleaseTexture(pSrcWDTexture);
	
	int hTex = *(int*)paramin;
	ClearTexState(pDevice,hTex);
	//Render_ReleaseTexture(pSrcWDTexture);//有pSrcWDTexture == NULL的时候
	SYHTable_RemoveAt(&pDevice->httexture,*(int*)paramin);
	return true;

}

bool Render_ResizeSwapChain(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	ResizeSwapChainIn* pIn = (ResizeSwapChainIn*)paramin;
	SYSwapChain* pSrcWDTexture = (SYSwapChain*)SYHTable_GetAt(&pDevice->htSwapChain,pIn->hSwapChain);
	ReleaseSwapChainBufferTex(pDevice,pSrcWDTexture);

	HRESULT hr = pSrcWDTexture->pSwapChain->ResizeBuffers(2,pIn->nWidth,pIn->nHeight,DXGI_FORMAT_R8G8B8A8_UNORM,0);
	assert(hr == S_OK);

	CreateSwapChainBufferTex(hDevice,pIn->nWidth,pIn->nHeight,pSrcWDTexture);
	return true;
}
bool Render_DownLoad(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	downloadBufferIn* pIn = (downloadBufferIn*)paramin;

	copyTexture copyParam;
	copyParam.hDesTex = pIn->hTransTex;
	copyParam.nDesOffsetX = 0;
	copyParam.nDesOffsetY = 0;
	copyParam.nDesWidth = pIn->dstWidth;
	copyParam.nDesHeight = pIn->dstHeight;
	copyParam.hSrcTex = pIn->hSrcTex;
	copyParam.nSrcOffsetX = pIn->srcOffsetX;
	copyParam.nSrcOffsetY = pIn->srcOffsetY;
	copyParam.nSrcWidth = pIn->dstWidth;
	copyParam.nSrcHeight = pIn->dstHeight;
	copyParam.eMode = pIn->eMode;

	Render_CopyTexture(hDevice,&copyParam,NULL);
	
	SYTexture* pMemTex = (SYTexture*)SYHTable_GetAt(&pDevice->httexture,pIn->hTransTex);
	float fScale = 1;
	IsNeedScaleTexParam(pDevice,pIn->hTransTex,fScale);
	ID3D10Texture2D* pMem2DTex = (ID3D10Texture2D* )pMemTex->pTexture;

	D3D10_MAPPED_TEXTURE2D mapTex;
	pMem2DTex->Map(0,D3D10_MAP_READ,0,&mapTex );
	int nSrcWidth=(int)(pIn->dstWidth * fScale-1);
	int nSrcHeight=(int)(pIn->dstHeight * fScale-1);
	SY_BitBlt( pIn->dstBuffer,
		pIn->dstOffsetX, pIn->dstOffsetY,
		pIn->dstWidth, pIn->dstHeight,
		pIn->dstBufferWidth, pIn->dstBufferHeight, 
		(DWORD*)mapTex.pData,
		0,0,
		nSrcWidth,nSrcHeight ,
		mapTex.RowPitch / sizeof(DWORD), pMemTex->nRealWidth, 
		(BUFFER_MODE)pIn->eMode, BUFFER_PARITY_BOTH);
	pMem2DTex->Unmap(0);
	return true;
}

bool Render_BeginTex(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	BeginRenderTexture(pDevice);
	return true;
}

bool Render_CascadeTex(SY_DeviceDX10* pDevice,int hTex,float fAlpha, const RECT& SrcRect,const RECT& DestRect,
					 int nSrcBufferWidth,int nSrcBufferHeight,int nDstBufferWidth,int nDstBufferHeight,BOOL bAdjustColor,DXTextureType nType)
{
	screenCB screen;
	matrix4f matWorld;
	matrix4f_identity(&matWorld);
	int nSrcWidth = (SrcRect.right - SrcRect.left);
	int nSrcHeight = (SrcRect.bottom - SrcRect.top);
	float fXScale = (float)nSrcWidth;
	float fYScale = (float)nSrcHeight;
	matWorld._11 = fXScale / 2.f;
	matWorld._22 = fYScale / 2.f;

	matrix4f posMat;
	matrix4f_identity(&posMat);
	int nDstWidth = DestRect.right - DestRect.left;
	int nDstHeight = DestRect.bottom - DestRect.top;

	SetVertexBuffer(pDevice,pDevice->hScreenVertex);
	SetPrimitiveTopology(pDevice,TRIANGLELIST);
	SetShader(pDevice,DXShaderType_VertexShader,pDevice->hScreenVS);
	SetShader(pDevice,DXShaderType_GeometryShader,NULL);

	float4 vPSParam[3];
	vPSParam[0].x = fAlpha;
	ModifyBuffer(pDevice,pDevice->hModiyCB,vPSParam);
	SetConstBuffer(pDevice,DXShaderType_PixelShader,0,pDevice->hModiyCB);

	if(nType == DXTexture_MS2DTex)
		SetShader(pDevice,DXShaderType_PixelShader,pDevice->hScreenMSPS);
	else
		SetShader(pDevice,DXShaderType_PixelShader,bAdjustColor?pDevice->hScreenAdjustPS:pDevice->hScreenPS);
	SetShaderTexture(pDevice,hTex,DXShaderType_PixelShader,0);
	SetSamplerState(pDevice,DXSample_Point_Wrap,DXShaderType_PixelShader,0);
//这个效率还可以优化
	for(int nY = 0; nY < nDstHeight / nSrcHeight + 1; nY ++)
	{
		for(int nX = 0; nX < nDstWidth / nSrcWidth + 1; nX ++)
		{
			float posX = (nX + 0.5f) * nSrcWidth + (DestRect.left - nDstBufferWidth / 2.f) ;
			float posY = -(nY + 0.5f) * nSrcHeight + (nDstBufferHeight / 2.f - DestRect.top);
			posMat._41 = posX;
			posMat._42 = posY;
			posMat._43 = 0.5;
			screen.world_View_proj = matWorld * posMat * pDevice->spriteProj;

			screen.texCoord.x = SrcRect.left / (float)nSrcBufferWidth;
			screen.texCoord.y = SrcRect.top / (float)nSrcBufferHeight;

			screen.texCoord.z = nSrcWidth / (float)nSrcBufferWidth;
			screen.texCoord.w = nSrcHeight / (float)nSrcBufferHeight;
			ModifyBuffer(pDevice,pDevice->hScreenCB,&screen);
			SetConstBuffer(pDevice,DXShaderType_VertexShader,0,pDevice->hScreenCB);

			pDevice->pDevice->Draw(6,0);	
			pDevice->info.nRenderCount++ ;
			pDevice->info.nTriangleCount += 2;
		}
	}
	return true;
}

bool Render_RenderTex(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	renderTextureIn* pIn = (renderTextureIn*)paramin;

	int nSrcWidth,nSrcHeight;
	getTextureInfo(pDevice,pIn->hTex,&nSrcWidth,&nSrcHeight,NULL);

 	int nRTWidth,nRTHeight;
	getTextureInfo(pDevice,pDevice->hNowRenderTarget,&nRTWidth,&nRTHeight,NULL);

	SYTexture* pTex = (SYTexture*)SYHTable_GetAt(&pDevice->httexture,pIn->hTex);
	SetRenderState(pDevice,DXDepthState,DXDepthDisable);
	if(pIn->bStretch)
	{
		RenderTexture(pDevice,pIn->hTex,pIn->fAlpha,*pIn->pSrcRt,*pIn->pDstRt,
				nSrcWidth,nSrcHeight,nRTWidth,nRTHeight,pIn->bAdjustColor,pTex->nType,pIn->nField);
	}
	else
	{
		Render_CascadeTex(pDevice,pIn->hTex,pIn->fAlpha,*pIn->pSrcRt,*pIn->pDstRt,
				nSrcWidth,nSrcHeight,nRTWidth,nRTHeight,pIn->bAdjustColor,pTex->nType);
	}
	pDevice->info.nRenderPixCount += (pIn->pDstRt->right - pIn->pDstRt->left) * (pIn->pDstRt->bottom - pIn->pDstRt->top);
	return true;
}
bool Render_GetRenderTarget(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	getRenderTargetIn* pIn = (getRenderTargetIn*)paramin;

	if(pIn->phRT)
		*pIn->phRT = pDevice->hNowRenderTarget;
	if (pIn->phDepth)
		*pIn->phDepth = pDevice->hNowDepth;
	return true;
}
BOOL Render_EngineLock(int hDevice,void* paramin,void* paramout);
BOOL Render_EngineUnLock(int hDevice,void* paramin,void* paramout);
BOOL Render_EngineLock(SY_DeviceDX10* pDevice, void* paramin, void* paramout);
BOOL Render_EngineUnLock(SY_DeviceDX10* pDevice, void* paramin, void* paramout);
bool Render_UpLoad(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	uploadBufferIn* pIn = (uploadBufferIn*)paramin;

	if(pIn->bLock)
		Render_EngineLock(hDevice,NULL,NULL);
	EngineThreadLock();
	SYTexture* pMemTex = (SYTexture*)SYHTable_GetAt(&pDevice->httexture,pIn->hTransTex);
	ID3D10Texture2D* pMem2DTex = (ID3D10Texture2D*)pMemTex->pTexture;

	D3D10_MAPPED_TEXTURE2D mapTex;
//	if(!pMemTex->pMemBuffer)
	{
		HRESULT hr = pMem2DTex->Map(0,D3D10_MAP_WRITE,0,&mapTex );
		pMemTex->pMemBuffer = mapTex.pData;
		pMemTex->nPitch = mapTex.RowPitch;
//		pMem2DTex->Unmap(0);
	}
	//else
	//	pDevice->pDevice->Flush();

	EngineThreadUnLock();
	if(pIn->bLock)
		Render_EngineUnLock(hDevice,NULL,NULL);
	
	float fScale = 1;
	IsNeedScaleTexParam(pDevice,pIn->hTransTex,fScale);
	SY_BitBlt( (DWORD*)pMemTex->pMemBuffer ,
		0,0,
		pIn->srcWidth * fScale, pIn->srcHeight * fScale,
		pMemTex->nPitch / sizeof(DWORD), pMemTex->nRealHeight, 
		pIn->srcBuffer,
		pIn->srcOffsetX, pIn->srcOffsetY,
		pIn->srcWidth, pIn->srcHeight,
		pIn->srcBufferWidth, pIn->srcBufferHeight, 
		BUFFER_COPY, BUFFER_PARITY_BOTH  );

	if(pIn->bLock)
		Render_EngineLock(hDevice,NULL,NULL);
	EngineThreadLock();

	pMem2DTex->Unmap(0);
	pMemTex->pMemBuffer = NULL;

	
	copyTexture copyParam;
	copyParam.hDesTex = pIn->hDestTex;
	copyParam.nDesOffsetX = pIn->destOffsetX;
	copyParam.nDesOffsetY = pIn->destOffsetY;
	copyParam.nDesWidth = pIn->srcWidth;
	copyParam.nDesHeight = pIn->srcHeight;
	copyParam.hSrcTex = pIn->hTransTex;
	copyParam.nSrcOffsetX = 0;
	copyParam.nSrcOffsetY = 0;
	copyParam.nSrcWidth = pIn->srcWidth;
	copyParam.nSrcHeight = pIn->srcHeight;

	Render_CopyTexture(hDevice,&copyParam,NULL);

	EngineThreadUnLock();
	if(pIn->bLock)
		Render_EngineUnLock(hDevice,NULL,NULL);
	
	return true;
}
bool Render_EndTex(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	EndRenderTexture(pDevice);
	return true;
}

struct SYMeshVertex
{
	float3 pos;
	float3 normal;
	float2 tex;
};
BOOL TransMesh9to10Ex(ID3DXMesh* pMesh9,ID3D10Device* pDevice10,ID3DX10Mesh** ppMesh10)
{
	static D3D10_INPUT_ELEMENT_DESC ModelLayout[3] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },  
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },  
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },  
	};
	int nVertices = pMesh9->GetNumVertices();
	int nNumFaces = pMesh9->GetNumFaces();
	HRESULT hr = D3DX10CreateMesh(pDevice10,ModelLayout,
		sizeof( ModelLayout ) / sizeof( ModelLayout[0] ),ModelLayout[0].SemanticName,
		pMesh9->GetNumVertices(),pMesh9->GetNumFaces(),D3DX10_MESH_32_BIT,ppMesh10);

	D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE];
	pMesh9->GetDeclaration(Declaration);

	void* pVertices = NULL;
	hr = pMesh9->LockVertexBuffer(0,&pVertices);
	SYMeshVertex* test = (SYMeshVertex*)pVertices;
	hr = (*ppMesh10)->SetVertexData( 0, pVertices );
	pMesh9->UnlockVertexBuffer();

	DWORD* pIndices = NULL;
	hr = pMesh9->LockIndexBuffer(D3DLOCK_READONLY,(void**)&pIndices);
	hr = (*ppMesh10)->SetIndexData( pIndices, pMesh9->GetNumFaces() * 3 );
	pMesh9->UnlockIndexBuffer();

	DWORD dwTableSize = 0;
	hr = pMesh9->GetAttributeTable(NULL,&dwTableSize);
	D3DXATTRIBUTERANGE* pTable = new D3DXATTRIBUTERANGE[dwTableSize];
	hr = pMesh9->GetAttributeTable(pTable,&dwTableSize);
	D3DX10_ATTRIBUTE_RANGE* pAttr = new D3DX10_ATTRIBUTE_RANGE[dwTableSize];
	for(int i=0;i<(int)dwTableSize;i++)
	{
		pAttr[i].AttribId = pTable[i].AttribId;
		pAttr[i].FaceStart = pTable[i].FaceStart;
		pAttr[i].FaceCount = pTable[i].FaceCount;
		pAttr[i].VertexStart = pTable[i].VertexStart;
		pAttr[i].VertexCount = pTable[i].VertexCount;
	}
	hr = (*ppMesh10)->SetAttributeTable( pAttr, dwTableSize );
	delete []pTable;
	delete []pAttr;
	pTable = NULL;
	pAttr = NULL;
	hr = (*ppMesh10)->CommitToDevice();
	return TRUE;
}

LPDIRECT3D9			m_pD3D9;
LPDIRECT3DDEVICE9		m_pD3DDevice9;
BOOL Render_CreateModelFromXFile(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	LPCTSTR lpszFileName = (LPCTSTR )paramin;


	if (!m_pD3D9)
		m_pD3D9 = Direct3DCreate9( D3D_SDK_VERSION );
	if(!m_pD3DDevice9)
	{
		D3DCAPS9 Param;
		m_pD3D9->GetDeviceCaps( 0, D3DDEVTYPE_HAL, &Param );

		HRESULT hr;
		D3DPRESENT_PARAMETERS* d3dpp = new D3DPRESENT_PARAMETERS;
		ZeroMemory( d3dpp, sizeof(D3DPRESENT_PARAMETERS) );

		d3dpp->Windowed               = true;
		d3dpp->BackBufferCount        = 0;
		d3dpp->BackBufferFormat       = D3DFMT_A8R8G8B8;
		d3dpp->EnableAutoDepthStencil = TRUE;
		d3dpp->AutoDepthStencilFormat = D3DFMT_D16;
		d3dpp->hDeviceWindow          = NULL;
		d3dpp->PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
		D3DMULTISAMPLE_TYPE dwMultiSamplerType = D3DMULTISAMPLE_NONE;
		d3dpp->BackBufferWidth  = 10;
		d3dpp->BackBufferHeight = 10;
		d3dpp->SwapEffect = D3DSWAPEFFECT_COPY;
		if( FAILED( hr = m_pD3D9->CreateDevice( 0, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED|D3DCREATE_PUREDEVICE, 
			d3dpp, &m_pD3DDevice9 ) ) )
		{
			return false;
		}
		SAFE_DELETE(d3dpp);
	}
	//ID3DXBuffer* pMaterialBuffer = NULL;
	DWORD dwCount = 0;
	 
	ID3DXMesh* pMesh = NULL,*pOldMesh;
	//D3DXLoadMeshFromX(lpszFileName,D3DXMESH_MANAGED ,m_pD3DDevice9,NULL, &pMaterialBuffer, NULL, &dwCount,&pMesh);
	LPD3DXBUFFER pMaterialBuffer = NULL, pAdjacencyBuffer = NULL;
	D3DXLoadMeshFromX( lpszFileName, D3DXMESH_32BIT | D3DXMESH_MANAGED, m_pD3DDevice9, &pAdjacencyBuffer, &pMaterialBuffer, NULL, &dwCount, &pOldMesh );

	DWORD* pAdjacency = (DWORD*)pAdjacencyBuffer->GetBufferPointer();
	pOldMesh->Optimize( D3DXMESHOPT_ATTRSORT|D3DXMESHOPT_DONOTSPLIT, pAdjacency, NULL, NULL, NULL, &pMesh );
	pOldMesh->Release();

	//D3DXATTRIBUTERANGE* pAttribTable = new D3DXATTRIBUTERANGE[dwCount];
	//pMesh->GetAttributeTable( pAttribTable, NULL );

	ID3DX10Mesh* pMesh10 = NULL;
	TransMesh9to10Ex(pMesh,pDevice->pDevice,&pMesh10);
	if (!pMesh10)
	{
		return false;
	}
	
	*(int*)paramout = (int)SYHTable_Insert(&pDevice->htModel,pMesh10);
	return true;
}

bool Render_CreateShader(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	DXCreateShadeIn* param = (DXCreateShadeIn*)paramin;

	if(param->bLock)
		Render_EngineLock(hDevice,NULL,NULL);
	EngineThreadLock();

	shaderhash* pShaderHashValue = (shaderhash*)SYHash_Search(pDevice->ShaderHash,(void *)param->shaderName);

	if (pShaderHashValue)
	{
		++(pShaderHashValue->nCount) ;
		*(int*)paramout = pShaderHashValue->hHandle;
		EngineThreadUnLock();
		if(param->bLock)
			Render_EngineUnLock(hDevice,NULL,NULL);
		return true;
	}

	if (param->shaderFile)
	{
		*(int*)paramout = CreateShaderFromFile(pDevice,param->shaderName,param->shaderFile,param->nType,param->pType);
		EngineThreadUnLock();
		if(param->bLock)
			Render_EngineUnLock(hDevice,NULL,NULL);
		return true;
	}
	else if(param->shaderCode)
	{
		int strLen = (int)strlen(param->shaderCode);
		
		ID3D10Blob* pShader = 0;
		LPVOID	pCodeBin = 0;
		size_t  nBinSize = 0;

		if(param->bComplied)
		{
			pCodeBin = (void*)param->shaderCode;
			nBinSize = param->compliedBinSize;
		}
		else
		{
			char* profile = NULL;
			switch (param->nType)
			{
			case DXShaderType_VertexShader:
				profile = VSPROFILE;
				break;
			case DXShaderType_GeometryShader:
				profile = GSPROFILE;
				break;
			case DXShaderType_PixelShader:
				profile = PSPROFILE;
				break;
			}

			EngineThreadUnLock();
			if(param->bLock)
				Render_EngineUnLock(hDevice,NULL,NULL);

			ID3D10Blob** ppError = NULL;
#ifdef _DEBUG
			ID3D10Blob* pError;
			ppError = &pError; 
#endif // _DEBUG
			HRESULT hr = D3DX10CompileFromMemory(param->shaderCode,strLen,NULL,NULL,NULL,"main",profile,D3D10_SHADER_OPTIMIZATION_LEVEL3,0,NULL,&pShader,ppError,NULL);
#ifdef _DEBUG
			if (hr != S_OK)
			{
				char* msg = (char*)pError->GetBufferPointer();
				int nSize = (int)pError->GetBufferSize();
				assert(0);
			}
#endif // _DEBUG

			if(param->bLock)
				Render_EngineLock(hDevice,NULL,NULL);
			EngineThreadLock();

			pCodeBin = pShader->GetBufferPointer();
			nBinSize = pShader->GetBufferSize();
		}
					
		shaderhash* pShaderHashValue = (shaderhash*)SYHash_Search(pDevice->ShaderHash,(void *)param->shaderName);
		if (pShaderHashValue)
		{
			++(pShaderHashValue->nCount) ;
			*(int*)paramout = pShaderHashValue->hHandle;
		}
		else
		{
			SYShader* pNewShader = new SYShader;
			pNewShader->pLayout = NULL;
			pNewShader->nType = param->nType;
			switch (param->nType)
			{
			case DXShaderType_VertexShader:
				pDevice->pDevice->CreateVertexShader(pCodeBin,nBinSize,(ID3D10VertexShader**)&pNewShader->pShader);
				if (param->pType)
					pDevice->pDevice->CreateInputLayout( g_pLayout[*param->pType],g_pLayoutNum[*param->pType] ,pCodeBin,nBinSize, &pNewShader->pLayout ) ;
				break;
			case DXShaderType_GeometryShader:
				pDevice->pDevice->CreateGeometryShader(pCodeBin,nBinSize,(ID3D10GeometryShader**)&pNewShader->pShader);
				break;
			case DXShaderType_PixelShader:
				pDevice->pDevice->CreatePixelShader(pCodeBin,nBinSize,(ID3D10PixelShader**)&pNewShader->pShader);
				break;
			}

			int nNameLenth = (int)_tcsclen(param->shaderName) + 1;
			pNewShader->pShaderName = new TCHAR[nNameLenth];
			wcscpy_s(pNewShader->pShaderName,nNameLenth,param->shaderName);
			int hShader = (int)SYHTable_Insert(&pDevice->htShaderTable,pNewShader);
			pShaderHashValue = new shaderhash;
			pShaderHashValue->hHandle = hShader;
			pShaderHashValue->nCount = 1;
			SYHash_Insert(pDevice->ShaderHash,pNewShader->pShaderName,pShaderHashValue);
			*(int*)paramout = hShader;
		}

		if(pShader)
			pShader->Release();
		EngineThreadUnLock();
		if(param->bLock)
			Render_EngineUnLock(hDevice,NULL,NULL);
		return true;
	}
	EngineThreadUnLock();
	if(param->bLock)
		Render_EngineUnLock(hDevice,NULL,NULL);
	return false;

}

bool Render_CreateShader(SY_DeviceDX10* pDevice, void* paramin, void* paramout)
{
	if (!pDevice)
	{
		return false;
	}
	DXCreateShadeIn* param = (DXCreateShadeIn*)paramin;

	if (param->bLock)
		Render_EngineLock(pDevice, NULL, NULL);
	EngineThreadLock();

	shaderhash* pShaderHashValue = (shaderhash*)SYHash_Search(pDevice->ShaderHash, (void *)param->shaderName);

	if (pShaderHashValue)
	{
		++(pShaderHashValue->nCount);
		*(int*)paramout = pShaderHashValue->hHandle;
		EngineThreadUnLock();
		if (param->bLock)
			Render_EngineUnLock(pDevice, NULL, NULL);
		return true;
	}

	if (param->shaderFile)
	{
		*(int*)paramout = CreateShaderFromFile(pDevice, param->shaderName, param->shaderFile, param->nType, param->pType);
		EngineThreadUnLock();
		if (param->bLock)
			Render_EngineUnLock(pDevice, NULL, NULL);
		return true;
	}
	else if (param->shaderCode)
	{
		int strLen = (int)strlen(param->shaderCode);

		ID3D10Blob* pShader = 0;
		LPVOID	pCodeBin = 0;
		size_t  nBinSize = 0;

		if (param->bComplied)
		{
			pCodeBin = (void*)param->shaderCode;
			nBinSize = param->compliedBinSize;
		}
		else
		{
			char* profile = NULL;
			switch (param->nType)
			{
			case DXShaderType_VertexShader:
				profile = VSPROFILE;
				break;
			case DXShaderType_GeometryShader:
				profile = GSPROFILE;
				break;
			case DXShaderType_PixelShader:
				profile = PSPROFILE;
				break;
			}

			EngineThreadUnLock();
			if (param->bLock)
				Render_EngineUnLock(pDevice, NULL, NULL);

			ID3D10Blob** ppError = NULL;
#ifdef _DEBUG
			ID3D10Blob* pError;
			ppError = &pError;
#endif // _DEBUG
			HRESULT hr = D3DX10CompileFromMemory(param->shaderCode, strLen, NULL, NULL, NULL, "main", profile, D3D10_SHADER_OPTIMIZATION_LEVEL3, 0, NULL, &pShader, ppError, NULL);
#ifdef _DEBUG
			if (hr != S_OK)
			{
				char* msg = (char*)pError->GetBufferPointer();
				int nSize = (int)pError->GetBufferSize();
				assert(0);
			}
#endif // _DEBUG

			if (param->bLock)
				Render_EngineLock(pDevice, NULL, NULL);
			EngineThreadLock();

			pCodeBin = pShader->GetBufferPointer();
			nBinSize = pShader->GetBufferSize();
		}

		shaderhash* pShaderHashValue = (shaderhash*)SYHash_Search(pDevice->ShaderHash, (void *)param->shaderName);
		if (pShaderHashValue)
		{
			++(pShaderHashValue->nCount);
			*(int*)paramout = pShaderHashValue->hHandle;
		}
		else
		{
			SYShader* pNewShader = new SYShader;
			pNewShader->pLayout = NULL;
			pNewShader->nType = param->nType;
			switch (param->nType)
			{
			case DXShaderType_VertexShader:
				pDevice->pDevice->CreateVertexShader(pCodeBin, nBinSize, (ID3D10VertexShader**)&pNewShader->pShader);
				if (param->pType)
					pDevice->pDevice->CreateInputLayout(g_pLayout[*param->pType], g_pLayoutNum[*param->pType], pCodeBin, nBinSize, &pNewShader->pLayout);
				break;
			case DXShaderType_GeometryShader:
				pDevice->pDevice->CreateGeometryShader(pCodeBin, nBinSize, (ID3D10GeometryShader**)&pNewShader->pShader);
				break;
			case DXShaderType_PixelShader:
				pDevice->pDevice->CreatePixelShader(pCodeBin, nBinSize, (ID3D10PixelShader**)&pNewShader->pShader);
				break;
			}

			int nNameLenth = (int)_tcsclen(param->shaderName) + 1;
			pNewShader->pShaderName = new TCHAR[nNameLenth];
			wcscpy_s(pNewShader->pShaderName, nNameLenth, param->shaderName);
			int hShader = (int)SYHTable_Insert(&pDevice->htShaderTable, pNewShader);
			pShaderHashValue = new shaderhash;
			pShaderHashValue->hHandle = hShader;
			pShaderHashValue->nCount = 1;
			SYHash_Insert(pDevice->ShaderHash, pNewShader->pShaderName, pShaderHashValue);
			*(int*)paramout = hShader;
		}

		if (pShader)
			pShader->Release();
		EngineThreadUnLock();
		if (param->bLock)
			Render_EngineUnLock(pDevice, NULL, NULL);
		return true;
	}
	EngineThreadUnLock();
	if (param->bLock)
		Render_EngineUnLock(pDevice, NULL, NULL);
	return false;

}

bool Render_ReleaseShader(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	ReleaseShader(pDevice,*(int*)paramin);
	return true;
}

matrix4f __mYUVRGB601 = {	
    1.164383f	,	1.164383f		,	1.164383f		,0.0f	,
    0.0f		,	-0.391762f		,	2.017232f		,0.0f	,
    1.596027f	,	-0.812968f		,	0.0f			,0.0f	,
    0.0f		,	0.0f			,	0.0f			,1.0f	
    };

matrix4f __mYUVRGB709= {	
    1.164f		,	1.164f			,	1.164f			,0.0f	,
    0.000f		,	-0.213f			,	2.115f			,0.0f	,
    1.793f		,	-0.534f			,	0.000f			,0.0f	,
    0.0f		,	0.0f			,	0.0f			,1.0f	
    };

matrix4f __mRGBYUV709= {	
	0.183f		,	-0.101f/2		,	0.439f/2	,0.0f	,
	0.614f		,	-0.338f/2		,	-0.399f/2	,0.0f	,
	0.062f		,	0.439f/2		,	-0.040f/2	,0.0f	,
	0.0627450f  ,	0.5019600f/2	,	0.5019600f/2,0.0f
	};

matrix4f __mRGBYUV601= {	0.2567880f	,	-0.1482230f/2	,	0.4392160f/2	,0.0f	,
						0.5041290f	,	-0.2909930f/2	,	-0.367788f/2	,0.0f	,
						0.0979060f	,	 0.4392160f/2	,	-0.071427f/2	,0.0f	,
						0.0627450f  ,	 0.5019600f/2	,	0.5019600f/2	,0.0f	
						};

BOOL Render_SetShaderTexture(int hDevice,void* paramin,void* paramout);
void TexProcessSetPixShader(SY_DeviceDX10* pDevice,DXTexProcessIn* param,int nSrcBufferWidth,int nSrcBufferHeight)
{
	if(param->nType == DXTextureBlur_5Pt || param->nType == DXTextureBlur_9Pt)
	{
		DXDistortionParam* pDistortion = (DXDistortionParam*)param->pData;
		float4 buffer[3]; 
		buffer[0].x = 1.0f / nSrcBufferWidth;
		buffer[0].y = 1.0f / nSrcBufferHeight;
		buffer[0].z = 0.5f + pDistortion->fCenterX;
		buffer[0].w = 0.5f + pDistortion->fCenterY;

		buffer[1].x = pDistortion->fRadial;

		buffer[2].x = pDistortion->fCentrifugal1;
		buffer[2].y = pDistortion->fCentrifugal2;
		buffer[2].z = pDistortion->fPrism1;
		buffer[2].w = pDistortion->fPrism2;
		ModifyBuffer(pDevice,pDevice->hModiyCB,buffer);
		SetConstBuffer(pDevice,DXShaderType_PixelShader,0,pDevice->hModiyCB);
		SetShader(pDevice,DXShaderType_PixelShader,param->nType == DXTextureBlur_5Pt?pDevice->hBlur5PtPS : pDevice->hBlur9PtPS);
		SetSamplerState(pDevice,DXSample_Anisotropic_Clamp,DXShaderType_PixelShader,0);
	}
	else if(param->nType == DXTextureConvertFormat)
	{
		ColorConvertType* pType = (ColorConvertType*)(param->pData);
		if(*pType == Color_RGBA_To_BGRA)
		{
			SetShader(pDevice,DXShaderType_PixelShader,pDevice->hConvertFormatPS);
			SetSamplerState(pDevice,DXSample_Line_Border,DXShaderType_PixelShader,0);
		}
		else if(*pType == ColorAdjustAlpha)
		{
			float4 vPSParam[3];
			vPSParam[0].x = 1;
			ModifyBuffer(pDevice,pDevice->hModiyCB,vPSParam);
			SetConstBuffer(pDevice,DXShaderType_PixelShader,0,pDevice->hModiyCB);
			SetShader(pDevice,DXShaderType_PixelShader,pDevice->hScreenAdjustPS);
			SetSamplerState(pDevice,DXSample_Line_Border,DXShaderType_PixelShader,0);
		}
		else if(*pType == Color_UYVU_RGBA_Key)
		{
			BOOL bEnableKey = (param->hSrcKey != NULL);
			int hShader = NULL;
			if(bEnableKey)
				hShader = (param->nDstField == SECOND_FIELD ? pDevice->hUYVY_RGBA_KeyPS0 : pDevice->hUYVY_RGBA_KeyPS1);
			else
				hShader = (param->nDstField == SECOND_FIELD ? pDevice->hUYVY_RGBA_PS0 : pDevice->hUYVY_RGBA_PS1);
			SetShader(pDevice,DXShaderType_PixelShader,hShader);

			int nWidth = param->pSrcRt->right - param->pSrcRt->left;
			matrix4f* pPsParam = nWidth > 360 ? &__mYUVRGB709 : &__mYUVRGB601 ;
			ModifyBuffer(pDevice,pDevice->hRenderTexCB,pPsParam);
			SetConstBuffer(pDevice,DXShaderType_PixelShader,0,pDevice->hRenderTexCB); 

		}
		else if (*pType == Color_UYVYFrame_RGBA)
		{
			int hShader = NULL;
			hShader = pDevice->hUYVY_RGBA_Frame;
			SetShader(pDevice,DXShaderType_PixelShader,hShader);

			int nWidth = param->pSrcRt->right - param->pSrcRt->left;
			matrix4f* pPsParam = nWidth > 360 ? &__mYUVRGB709 : &__mYUVRGB601 ;
			ModifyBuffer(pDevice,pDevice->hRenderTexCB,pPsParam);
			SetConstBuffer(pDevice,DXShaderType_PixelShader,0,pDevice->hRenderTexCB);
		}
		else if(*pType == Color_RGBA_YUAYVA4224)
		{
			int nWidth = param->pSrcRt->right - param->pSrcRt->left;
			float4 ShaderParam[5];
			matrix4f* pPsParam = nWidth > 720 ? &__mRGBYUV709 : &__mRGBYUV601;
			memcpy(ShaderParam,pPsParam,sizeof(matrix4f));
			ShaderParam[4].x = param->nDstField == SECOND_FIELD ? 0.f : 1.0f;
			ModifyBuffer(pDevice,pDevice->hRenderTexCB5,ShaderParam);
			SetConstBuffer(pDevice,DXShaderType_PixelShader,0,pDevice->hRenderTexCB5);
			SetShader(pDevice,DXShaderType_PixelShader,pDevice->hRGBA_YUAYVA4224PS);
		}
		else if (*pType == Color_RGBA_UYVY)     
		{
			SetShader(pDevice,DXShaderType_PixelShader,pDevice->hRGBA_UYVY_Frame);
			int nWidth = param->pSrcRt->right - param->pSrcRt->left;
			float4 ShaderParam[5];
			matrix4f* pPsParam = nWidth > 720 ? &__mRGBYUV709 : &__mRGBYUV601;
			ShaderParam[4].x = float(nWidth);
			memcpy(ShaderParam,pPsParam,sizeof(matrix4f));
			ModifyBuffer(pDevice,pDevice->hRenderTexCB5,ShaderParam);
			SetConstBuffer(pDevice,DXShaderType_PixelShader,0,pDevice->hRenderTexCB5);
		}
		else if(*pType == Color_RGBA_To_FRAME)
		{
			SetShader(pDevice,DXShaderType_PixelShader,pDevice->hRGBA_Frame);

			int nWidth = param->pSrcRt->right - param->pSrcRt->left;
			matrix4f* pPsParam = nWidth > 360 ? &__mYUVRGB709 : &__mYUVRGB601 ;
			ModifyBuffer(pDevice,pDevice->hRenderTexCB,pPsParam);
			SetConstBuffer(pDevice,DXShaderType_PixelShader,0,pDevice->hRenderTexCB);
		}
		else if(*pType == Color_RGBA_TO_Alpha)
		{
			SetShader(pDevice,DXShaderType_PixelShader,pDevice->hOnlyKeyOutPS);
			SetSamplerState(pDevice,DXSample_Point_Border,DXShaderType_PixelShader,0);
		}
		else if(*pType == Color_RGB_Alpha_To_Key)
		{
			SetShader(pDevice,DXShaderType_PixelShader,pDevice->hFieldAddKeyPS);
			SetSamplerState(pDevice,DXSample_Point_Border,DXShaderType_PixelShader,0);
		}
	}
	else if(param->nType == DX_UYVY_BRIGHT)
	{
		float* pGain = (float*)param->pData;
		matrix4f PsParam;
		PsParam._11 = *pGain;
		ModifyBuffer(pDevice,pDevice->hRenderTexCB,&PsParam);
		SetConstBuffer(pDevice,DXShaderType_PixelShader,0,pDevice->hRenderTexCB);
		SetShader(pDevice,DXShaderType_PixelShader,pDevice->hUYVYBrightPS);
		SetSamplerState(pDevice,DXSample_Point_Clamp,DXShaderType_PixelShader,0);
		//SetBlendState(pDevice,SYBlendDisable);
	}
}
bool Render_TexProcess(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	DXTexProcessIn* param = (DXTexProcessIn*)paramin;
	int oldRT = pDevice->hNowRenderTarget;
	int oldDepth = pDevice->hNowDepth;
	RECT oldRect = pDevice->rtViewport;
	RECT oldScissor = pDevice->rtScissor;
	SetRenderTarget(pDevice,param->hDestTex,0);
	SetScissor(pDevice,param->pDstRt);
	int nDstBufferWidth = pDevice->rtViewport.right;
	int nDstBufferHeight = pDevice->rtViewport.bottom;
	//ID3D10Texture2D* pTexture = getTexture();
	int nSrcBufferWidth,nSrcBufferHeight;
	getTextureInfo(pDevice,param->hSrcTex,&nSrcBufferWidth,&nSrcBufferHeight,NULL);

	screenCB screen;
	matrix4f matWorld;
	matrix4f_identity(&matWorld);
	int nSrcWidth = (param->pSrcRt->right - param->pSrcRt->left);
	int nSrcHeight = (param->pSrcRt->bottom - param->pSrcRt->top);
	float fXScale = (float)(param->pDstRt->right - param->pDstRt->left) ;
	float fYScale = (float)(param->pDstRt->bottom - param->pDstRt->top);
	matWorld._11 = fXScale / 2.f;
	matWorld._22 = fYScale / 2.f;

	matrix4f posMat;
	matrix4f_identity(&posMat);
	float posX = param->pDstRt->left - (-matWorld._11 + nDstBufferWidth / 2.f) ;
	float posY = -(param->pDstRt->top - (-matWorld._22 + nDstBufferHeight / 2.f));
	posMat._41 = posX;
	posMat._42 = posY;
	posMat._43 = 0.5;

	float halfWidth = nDstBufferWidth / 2.f;
	float halfHeight = nDstBufferHeight / 2.f;
	//D3DXMATRIX matProjection;
	// Create the projection matrix using the values in the viewport
	matrix4f matProj;
	CalProjectMat(&matProj,-halfWidth,halfWidth,-halfHeight,halfHeight,0.1f,10,SY_ORTHO_PROJ);
	screen.world_View_proj = matWorld * posMat * matProj;
	float4 test = {-1,1,0,1};
	test *= screen.world_View_proj;

	screen.texCoord.x = param->pSrcRt->left / (float)nSrcBufferWidth;
	screen.texCoord.y = param->pSrcRt->top / (float)nSrcBufferHeight;

	screen.texCoord.z = nSrcWidth / (float)nSrcBufferWidth;
	screen.texCoord.w = nSrcHeight / (float)nSrcBufferHeight;

	ModifyBuffer(pDevice,pDevice->hScreenCB,&screen);
	SetConstBuffer(pDevice,DXShaderType_VertexShader,0,pDevice->hScreenCB);

	SetVertexBuffer(pDevice,pDevice->hScreenVertex);
	SetPrimitiveTopology(pDevice,TRIANGLELIST);

	SetShader(pDevice,DXShaderType_VertexShader,pDevice->hScreenVS);
	SetShader(pDevice,DXShaderType_GeometryShader,NULL);
	TexProcessSetPixShader(pDevice,param,nSrcBufferWidth,nSrcBufferHeight);

	SetRenderState(pDevice,DXDepthState,DXDepthDisable);
	SetRenderState(pDevice,DXBlendState,DXBlendDisable);
	SetRenderState(pDevice,DXRasterizerState,DXRas_Solid_NoCullBack_MultiSample);

	SetShaderTexture(pDevice,param->hSrcTex,DXShaderType_PixelShader,0);
	if(param->hSrcKey)
		SetShaderTexture(pDevice,param->hSrcKey,DXShaderType_PixelShader,1);

	pDevice->pDevice->Draw(6,0);	
	pDevice->info.nRenderCount++ ;
	pDevice->info.nTriangleCount += 2;
	pDevice->info.nRenderPixCount += SY_FToI(fXScale * fYScale);

	SetRenderTarget(pDevice,oldRT,oldDepth);
	SetViewPort(pDevice,oldRect.left,oldRect.top,oldRect.right,oldRect.bottom);
	SetScissor(pDevice,&oldScissor);

	if(0)
		render_DumpTexture(hDevice,&param->hSrcTex,0);
	return true;
}

bool Render_SetViewPort(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	RECT* pRect = (RECT*)paramin;
	SetViewPort(pDevice,pRect->left ,pRect->top ,pRect->right ,pRect->bottom );
	return true;
}

bool Render_GetViewPort(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	RECT* pRect = (RECT*)paramout;
	*pRect = pDevice->rtViewport;
	return true;
}
bool Render_CreateBuffer(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	DXCreateBufferIn* param = (DXCreateBufferIn*)paramin;
	*(int*)paramout = CreateBuffer(pDevice,param->nVertexSize,param->nVertexNum,param->pBuffer,param->nType,param->nPriType );
	return true;
}

bool Render_ReleaseBuffer(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	ReleaseBuffer(pDevice,*(int*)paramin);
	return true;
}

bool Render_ModifyBuffer(int hDevice,void* paramin,void* paramout)
{
	DXModifyBufferIn* param = (DXModifyBufferIn*)paramin;
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	ModifyBuffer(pDevice,param->hBuffer,param->pBuffer);
	return true;
}
ID3D10BlendState* g_pNoBlendState = NULL;

int CalVertexCountByPrimitiveCount(int nPrimitiveCount,int nType)
{
	switch(nType)
	{
	case LINE_LIST:
		return nPrimitiveCount * 2;
	case TRIANGLELIST:
		return nPrimitiveCount * 3;
	case TRIANGLESTRIP:
		return nPrimitiveCount + 2;
	}
	return 0;
}

bool Render_Render(int hDevice,void* paramin,void* paramout)
{
	DXRenderIn* param = (DXRenderIn*)paramin;
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);

	if (param->hVertexBuffer)
	{
		SetVertexBuffer(pDevice,param->hVertexBuffer);
		SetPrimitiveTopology(pDevice,param->nVertexType);
	}
	if(param->hIndexBuffer)
	{
		SetIndexBuffer(pDevice,param->hIndexBuffer);
	}

	if (0)
	{
		ID3D10RasterizerState *pRasterizerState = NULL;

		pDevice->pDevice->RSGetState(&pRasterizerState);
		D3D10_RASTERIZER_DESC desc;
		pRasterizerState->GetDesc(&desc);
		SAFE_RELEASE(pRasterizerState);
	}

	if(param->hIndexBuffer)
	{
		int nIndexCount = CalVertexCountByPrimitiveCount(param->PrimitiveCount,param->nVertexType);
		pDevice->pDevice->DrawIndexed(nIndexCount,param->nStart,0);	
		pDevice->info.nRenderCount++ ;
		pDevice->info.nTriangleCount += param->PrimitiveCount;
	}
	else
	{
		pDevice->pDevice->Draw(param->nVertexCount,param->nStart);
		pDevice->info.nRenderCount++ ;
		pDevice->info.nTriangleCount += param->nVertexCount / 3;
	}
	return true;
}
bool Render_InstanceRender(int hDevice,void* paramin,void* paramout)
{
	DXRenderInstanceIn* param = (DXRenderInstanceIn*)paramin;
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);

	SYBuffer* pVB = (SYBuffer*)SYHTable_GetAt(&pDevice->htBuffer,param->RenderData.hVertexBuffer);
	SYBuffer* pInstance = (SYBuffer*)SYHTable_GetAt(&pDevice->htBuffer,param->hInstanceBuffer);
	UINT offsets[2] = {0,0};
	UINT strides[2] = { pVB->nVertexSize,pInstance->nVertexSize };
	ID3D10Buffer* pBufferAr[2] = {pVB->pBuffer,pInstance->pBuffer};
	pDevice->pDevice->IASetVertexBuffers(0,2,pBufferAr,strides,offsets);
	pDevice->hNowVertexBuffer = param->RenderData.hVertexBuffer;

	SetPrimitiveTopology(pDevice,param->RenderData.nVertexType);
	if(param->RenderData.hIndexBuffer)
	{
		SetIndexBuffer(pDevice,param->RenderData.hIndexBuffer);
	}

	if(param->RenderData.hIndexBuffer)
	{
		int nIndexCount = CalVertexCountByPrimitiveCount(param->RenderData.PrimitiveCount,param->RenderData.nVertexType);
		pDevice->pDevice->DrawIndexedInstanced(nIndexCount,param->nInstanceCount,param->RenderData.nStart,0,0);	
		pDevice->info.nRenderCount++ ;
		pDevice->info.nTriangleCount += param->RenderData.PrimitiveCount * param->nInstanceCount;
	}
	else
	{
		pDevice->pDevice->DrawInstanced(param->RenderData.nVertexCount,param->nInstanceCount,param->RenderData.nStart,0);
		pDevice->info.nRenderCount++ ;
		pDevice->info.nTriangleCount += param->RenderData.nVertexCount * param->nInstanceCount / 3;
	}
	return true;
}

bool Render_EasyRender(int hDevice,void* paramin,void* paramout)
{
	DXEasyRenderIn* param = (DXEasyRenderIn*)paramin;
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);

	if (param->hVertexBuffer)
	{
		SYBuffer* pVB = (SYBuffer*)SYHTable_GetAt(&pDevice->htBuffer,param->hVertexBuffer);
		SetVertexBuffer(pDevice,param->hVertexBuffer);
		SetPrimitiveTopology(pDevice,pVB->nPriType);
	}
	if(param->hIndexBuffer)
	{
		SetIndexBuffer(pDevice,param->hIndexBuffer);
	}
	assert(param->hVertexShader);
	assert(param->hPixShader);
	SetShader(pDevice,DXShaderType_VertexShader,param->hVertexShader);
	SetShader(pDevice,DXShaderType_GeometryShader,param->hGeometryShader);
	SetShader(pDevice,DXShaderType_PixelShader,param->hPixShader);

	if(param->hIndexBuffer)
	{
		pDevice->pDevice->DrawIndexed(param->nVertexCount,param->nStart,0);	
		pDevice->info.nRenderCount++ ;
		pDevice->info.nTriangleCount += param->nVertexCount ;
	}
	else
	{
		pDevice->pDevice->Draw(param->nVertexCount,param->nStart);
		pDevice->info.nRenderCount++ ;
		pDevice->info.nTriangleCount += param->nVertexCount ;
	}
	return true;
}

BOOL Render_SetConstBuffer(int hDevice,void* paramin,void* paramout)
{
	DXSetConstBufferIn* param = (DXSetConstBufferIn*)paramin;
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	return SetConstBuffer(pDevice,param->nShaderType,param->hRegIndex,param->hConstBuffer);
}

bool Render_CreateTexFromFile(int hDevice,void* paramin,void* paramout)
{
	createTextureFromFileIn* param = (createTextureFromFileIn*)paramin;
	TCHAR* pPath = param->pPath;
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);

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

	ID3D10Resource* pNewTexture = NULL;
	HRESULT hr = S_FALSE ;
	BOOL bMipMap = FALSE;
	int nX = 0,nY = 0;
	BaseCodec_GetImgInfo(pPath,nX,nY,nImgWidth,nImgHeight);
	if(nImgWidth * nImgHeight == 0)
	{
		if(D3DX10CreateTextureFromFile(pDevice->pDevice,pPath,NULL,NULL,&pNewTexture,NULL) )
		{
			*(int*)paramout = 0;
			return false;
		}
		hr = S_OK;
	}
	else
	{
		int nMipMap = 4;
		GetTexScale((UINT&)nImgWidth,(UINT&)nImgHeight,1,nMipMap);

		DWORD* pBuffer = new DWORD[nImgWidth * nImgHeight];
		BaseCodec_ReadImage(pPath, nImgWidth, nImgHeight, pBuffer);
		D3D10_SUBRESOURCE_DATA initData = {pBuffer,nImgWidth * sizeof(DWORD),0};
		D3D10_TEXTURE2D_DESC* pMemDesc = createMemTexDesc(nImgWidth,nImgHeight);
		ID3D10Texture2D* pMemTex = NULL;
		hr = pDevice->pDevice->CreateTexture2D(pMemDesc,&initData,(ID3D10Texture2D**)&pMemTex);

		D3D10_TEXTURE2D_DESC* pDesc = createReadOnlyDesc(nImgWidth,nImgHeight,nMipMap);
		bMipMap = (pDesc->MipLevels != 1);
		hr = pDevice->pDevice->CreateTexture2D(pDesc,NULL,(ID3D10Texture2D**)&pNewTexture);
		
		D3D10_BOX Box = {0,0,0,nImgWidth,nImgHeight,1};

		pDevice->pDevice->CopySubresourceRegion(pNewTexture,0,0,0,0,
			pMemTex,0,&Box);
		if(0)
			D3DX10SaveTextureToFile(pNewTexture,D3DX10_IFF_BMP,_T("e:\\loadTex.bmp"));

		if(param->bNormalMap)
			D3DX10ComputeNormalMap((ID3D10Texture2D*)pNewTexture,0,D3DX_CHANNEL_RED,1.f,(ID3D10Texture2D*)pNewTexture);
		SAFEDELETE_ARRAY(pBuffer);
		SAFE_DELETE(pDesc);
		SAFE_DELETE(pMemDesc);
		SAFE_RELEASE(pMemTex);
	}

	if(hr != S_OK)
	{
		*(int*)paramout = 0;
		return false;
	}

	SYTexture* pFileTex = new SYTexture;
	memset(pFileTex,0,sizeof(SYTexture));
	pFileTex->pTexture = pNewTexture;
	pFileTex->nType = nType;

	D3D10_TEXTURE2D_DESC desc;
	((ID3D10Texture2D*)pNewTexture)->GetDesc(&desc);
	pFileTex->nRealWidth = pFileTex->nNeedWidth = desc.Width;
	pFileTex->nRealHeight = pFileTex->nNeedHeight = desc.Height;

	size_t nLength = _tcslen(pPath) + 1;
	pFileTex->pTexFile = new TCHAR[nLength];
	_tcscpy_s(pFileTex->pTexFile,nLength,pPath);

	pDevice->pDevice->CreateShaderResourceView(pNewTexture,NULL,&(pFileTex->pResourceView));
	if(bMipMap)
		pDevice->pDevice->GenerateMips(pFileTex->pResourceView);
	int hNewSwapChain = (int)SYHTable_Insert(&pDevice->httexture,pFileTex);
	*(int*)paramout = hNewSwapChain;

	pTexHashValue = new HandleHash;
	pTexHashValue->hHandle = hNewSwapChain;
	pTexHashValue->nCount = 1;
	SYHash_Insert(pHash,pFileTex->pTexFile,pTexHashValue);
	return true;
}

bool Render_SetRenderState(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	DXRenderStateIn* param = (DXRenderStateIn*)paramin;
	SetRenderState(pDevice,param->nType,param->nValue);
	return true;
}

bool Render_CreateMipMap(int hDevice,void* paramin,void* paramout)
{
	*(int*)paramout = 0;

	return false;
}
bool Render_SetSamplerState(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	DXSampleStateIn* param = (DXSampleStateIn*)paramin;
	return SetSamplerState(pDevice,param->nState,param->nShaderType,param->nSampler);
}
BOOL Render_SetShaderTexture(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	DXSetShaderTexIn* param = (DXSetShaderTexIn*)paramin;
	return SetShaderTexture(pDevice,param->hConstBuffer,param->nShaderType,param->hRegIndex);
}

BOOL Render_SetShader(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	DXSetShaderIn* param = (DXSetShaderIn*)paramin;
	SetShader(pDevice,param->nShaderType,param->hShader);
	return TRUE;

}

BOOL Render_SetDevCaps(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	int* nFlag = (int*)paramin;
	if(*nFlag == DXDevice_AAFactor)
	{
		pDevice->nAAFactor = *((int*)paramout);
		return TRUE;
	}
	return FALSE;
}

BOOL Render_GetDevCaps(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	int* nFlag = (int*)paramin;
	if(*nFlag == DXDevice_AAFactor)
	{
		*((int*)paramout) = pDevice->nAAFactor;
		return TRUE;
	}
	return FALSE;
}

BOOL Render_ClearScreen(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	clearScreenIn* pParam = (clearScreenIn*)paramin;
	if(pParam->nType == DX_CLEAR_RENDERTARGET_ONLY || pParam->nType == DX_CLEAR_ALL)
	{
		float4 clearBuffer[2];
		clearBuffer[0].x = GetBGRA_R(pParam->nColor) / 255.f;
		clearBuffer[0].y = GetBGRA_G(pParam->nColor) / 255.f;
		clearBuffer[0].z = GetBGRA_B(pParam->nColor) / 255.f;
		clearBuffer[0].w = GetAValue(pParam->nColor) / 255.f;

		clearBuffer[1].x = pParam->fZValue;
		ModifyBuffer(pDevice,pDevice->hClearCB,clearBuffer);

		screenCB buffer;
		matrix4f_identity(&buffer.world_View_proj);
		buffer.texCoord.x = buffer.texCoord.y = 0;
		buffer.texCoord.w = buffer.texCoord.z = 1;
		ModifyBuffer(pDevice,pDevice->hScreenCB,&buffer);

		SetShader(pDevice,DXShaderType_VertexShader,pDevice->hScreenVS);
		SetShader(pDevice,DXShaderType_GeometryShader,NULL);
		SetShader(pDevice,DXShaderType_PixelShader,pDevice->hClearScreenPS);
		SetVertexBuffer(pDevice,pDevice->hScreenVertex);
		SetPrimitiveTopology(pDevice,TRIANGLELIST);
		SetConstBuffer(pDevice,DXShaderType_PixelShader,0,pDevice->hClearCB);
		SetConstBuffer(pDevice,DXShaderType_VertexShader,0,pDevice->hScreenCB);
		int nRasterState = pDevice->pNowState[DXRasterizerState];
		int nDepth = pDevice->pNowState[DXDepthState];
		SetRenderState(pDevice,DXRasterizerState,DXRas_Solid_NoCullBack_MultiSample);
		SetRenderState(pDevice,DXDepthState,DXDepthDisable);
		SetRenderState(pDevice,DXBlendState,DXBlendDisable);
		pDevice->pDevice->Draw(6,0);	

		pDevice->info.nRenderCount++ ;
		pDevice->info.nTriangleCount += 2 ;

		SetRenderState(pDevice,DXRasterizerState,nRasterState);
		SetRenderState(pDevice,DXDepthState,nDepth);
	}
	if(pDevice->hNowDepth && (pParam->nType == DX_CLEAR_DEPTH_ONLY || pParam->nType == DX_CLEAR_ALL))
	{
		clearTextureIn param = {pDevice->hNowDepth,&pParam->fZValue};
		render_ClearTexture(hDevice,&param,NULL);
	}
	return TRUE;
}

BOOL Render_HitTestMesh(int hDevice,void* paramin,void* paramout)
{
	DXHitTestMeshIn* paramIn = (DXHitTestMeshIn*)paramin;
	DXHitTestMeshOut* paramOut = (DXHitTestMeshOut*)paramout;
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);

	ID3DX10Mesh* pMesh10 = (ID3DX10Mesh*)SYHTable_GetAt(&pDevice->htModel,paramIn->hMesh);
	UINT nSubSetCount = 0;
	pMesh10->GetAttributeTable(NULL,&nSubSetCount);
	paramOut->nHitPart = -1;
	float fNearDist = 9e10f;
	for(UINT i = 0; i < nSubSetCount;i++)
	{
		float fDist = 0;
		UINT nHitCount;
		HRESULT hr = pMesh10->IntersectSubset(i,(D3DXVECTOR3*)paramIn->pStart,(D3DXVECTOR3*)paramIn->pDir,&nHitCount,NULL,NULL,NULL,&fDist,NULL);
		if (nHitCount > 0 && fNearDist > fDist)
		{
			paramOut->nHitPart = i;
			paramOut->fDist = fDist;
			fNearDist = fDist;
		}
	}
	return TRUE;
}


BOOL Render_RenderMesh(int hDevice,void* paramin,void* paramout)
{
	DXRenderMeshIn* paramIn = (DXRenderMeshIn*)paramin;
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	ID3DX10Mesh* pMesh10 = (ID3DX10Mesh*)SYHTable_GetAt(&pDevice->htModel,paramIn->hMesh);
	pDevice->hNowVertexBuffer = -1;
	pDevice->hNowIndexBuffer = -1;
	UINT nSubSetCount = 0;
	pMesh10->GetAttributeTable(NULL,&nSubSetCount);
	HRESULT hr ;
	if( paramIn->nDrawCount < 0 )
	{
		for(UINT i = 0; i < nSubSetCount;i++)
			hr =pMesh10->DrawSubset(i);
	}
	else 
	{
		for( int i = 0; i<paramIn->nDrawCount; i++ )
		{
			if( paramIn->pnSubset!= NULL && paramIn->pnSubset[i] < nSubSetCount )
				pMesh10->DrawSubset(paramIn->pnSubset[i]);
		}
	}
	return TRUE;
}
BOOL Render_GetMeshInfo( int hDevice, void* paramin,void* paramout )
{
	SYGetMeshInfoIn* paramIn = (SYGetMeshInfoIn*)paramin;
	DXGetMeshInfoOut* paramOut = (DXGetMeshInfoOut*)paramout;
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	ID3DX10Mesh* pMesh10 = (ID3DX10Mesh*)SYHTable_GetAt(&pDevice->htModel,paramIn->hMesh);
	UINT nSubSetCount = 0;
	pMesh10->GetAttributeTable(NULL,&nSubSetCount);
	paramOut->nSubSetCount = nSubSetCount;
	return TRUE;
}
BOOL Render_GetRenderState(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	*(int*)paramout = pDevice->pNowState[*(int*)paramin];
	return TRUE;
}

BOOL Render_GetMeshBox(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	DXGetMeshBoxIn* pParam = (DXGetMeshBoxIn*)paramin;
	ID3DX10Mesh* pMesh10 = (ID3DX10Mesh*)SYHTable_GetAt(&pDevice->htModel,*(int*)paramin);
	ID3DX10MeshBuffer* pBuffer;
	pMesh10->GetVertexBuffer(0,&pBuffer);
	SYMeshVertex* pVertex = NULL;
	SIZE_T nSize = 0;
	pBuffer->Map((void**)&pVertex,&nSize);
	size_t nVertexSize = nSize / sizeof(SYMeshVertex);
	*(pParam->pMin) = pVertex[0].pos;
	*(pParam->pMax) = pVertex[0].pos;
	for (size_t i = 1; i < nVertexSize;i++)
	{
		pParam->pMin->x = min(pParam->pMin->x,pVertex[i].pos.x);
		pParam->pMin->y = min(pParam->pMin->y,pVertex[i].pos.y);
		pParam->pMin->z = min(pParam->pMin->z,pVertex[i].pos.z);

		pParam->pMax->x = max(pParam->pMax->x,pVertex[i].pos.x);
		pParam->pMax->y = max(pParam->pMax->y,pVertex[i].pos.y);
		pParam->pMax->z = max(pParam->pMax->z,pVertex[i].pos.z);
	}
	pBuffer->Unmap();
	SAFE_RELEASE(pBuffer);
	return TRUE;
}

BOOL Render_ComputeNormalMap(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	SYTexture* pSrcTexture = (SYTexture*)SYHTable_GetAt(&pDevice->httexture,*(int*)paramin);
	SYTexture* pDestTexture = (SYTexture*)SYHTable_GetAt(&pDevice->httexture,*(int*)paramout);
	D3DX10ComputeNormalMap((ID3D10Texture2D*)pSrcTexture->pTexture,0,D3DX_CHANNEL_RED,1.f,(ID3D10Texture2D*)pDestTexture->pTexture);
	if(0)
	{
		D3DX10SaveTextureToFile((ID3D10Texture2D*)pSrcTexture->pTexture,D3DX10_IFF_BMP,_T("e:\\maptex.bmp"));
	}
	return TRUE;
}

BOOL Render_SetScissor(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	SetScissor(pDevice,(RECT*)paramin);
	return TRUE;
}

BOOL Render_GetScissor(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	RECT* pRect = (RECT*)paramout;
	*pRect = pDevice->rtScissor;
	return TRUE;
}

BOOL Render_GenerateMipMap(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	SYTexture* pTexture = (SYTexture*)SYHTable_GetAt(&pDevice->httexture,*(int*)paramin);
	if(pTexture->nType != DXTexture_MipMap)
		return FALSE;
	pDevice->pDevice->GenerateMips(pTexture->pResourceView);
	return TRUE;
}

BOOL Render_EngineLock(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	EnterCriticalSection(&pDevice->EngineLock);
	return TRUE;
}
BOOL Render_EngineUnLock(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	LeaveCriticalSection(&pDevice->EngineLock);
	return TRUE;
}

BOOL Render_EngineLock(SY_DeviceDX10* pDevice, void* paramin, void* paramout)
{
	EnterCriticalSection(&pDevice->EngineLock);
	return TRUE;
}
BOOL Render_EngineUnLock(SY_DeviceDX10* pDevice, void* paramin, void* paramout)
{
	LeaveCriticalSection(&pDevice->EngineLock);
	return TRUE;
}

BOOL Render_RenderLock(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	EnterCriticalSection(&pDevice->RenderLock);
	return TRUE;
}

BOOL Render_RenderUnLock(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	LeaveCriticalSection(&pDevice->RenderLock);
	return TRUE;
}

BOOL Render_CreatEffectUnit(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	DXCreateEffectUnitIn* param = (DXCreateEffectUnitIn*)paramin;
	if(param)
	{	
		SYTechCreateParam* techs = new  SYTechCreateParam[param->nTechs];
		for(int i = 0; i<param->nTechs ;i++)
		{
			techs[i].pDevice = pDevice->pDevice;
			techs[i].pd3deffect = 0;
			techs[i].pInputDesc = g_pLayout[param->pTechDesc[i].vertexType];
			techs[i].nInputElements = g_pLayoutNum[param->pTechDesc[i].vertexType];
			techs[i].pTechName	= param->pTechDesc[i].pstrName;
		}
		SYEffectCreateParam ecparam;
		ecparam.nTechs = param->nTechs;
		ecparam.pTechParam = techs;
		ecparam.pPath = g_szDX10PathEffect;
		ecparam.pDefines = 0;
		ecparam.pEffectName = param->pEffectName;
		ecparam.pDevice = pDevice->pDevice;
		CEffectUnit* pEU = pDevice->pEffectMan->CreateEffectUnit(&ecparam);
		delete [] techs;
		techs = NULL;
		if(pEU)
		{
			(*(int*)paramout) =  (int)SYHTable_Insert(&(pDevice->htEffectUnit),pEU);
			return TRUE ; 
		}
		else
		{
			return FALSE ;
		}
	}
	return FALSE ;
}

BOOL Render_ReleaseEffectUnit(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	int hEffect = *((int*)paramin);

	CEffectUnit* pEU =  (CEffectUnit*)SYHTable_GetAt(&(pDevice->htEffectUnit),hEffect);
	if(pEU)
	{
		pDevice->pEffectMan->ReleaseEffectUnit(pEU->GetResourceName());
		SYHTable_RemoveAt(&(pDevice->htEffectUnit),hEffect);
	}
	return TRUE ;
}

BOOL Render_GetQuadVB(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	int hVB =  pDevice->pEffectMan->GetQuadVB();
	
	if(0 != hVB)
	{
		(*(int*)paramout) = hVB;
		return TRUE;
	}
	else
	{
		float3	vb[] = 
		{
			{-0.5,-0.5,0.5},
			{0.5,0.5,0.5},
			{0.5,-0.5,0.5},
			
			{-0.5,-0.5,0.5},
			{-0.5,0.5,0.5},
			{0.5,0.5,0.5},
		};
		hVB = CreateBuffer(pDevice,sizeof(float3),6,vb,DXVertexBuffer,TRIANGLELIST);
		if(0 == hVB)
			return FALSE;
		pDevice->pEffectMan->SetQuadVB(hVB);
		*(int*)paramout =  pDevice->pEffectMan->GetQuadVB();
		return TRUE;
	}
}

BOOL Render_EffectRender(int hDevice,void* paramin,void* paramout)
{
	DXEffectRenderIn* in_param = (DXEffectRenderIn*)paramin;
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);

	if (in_param->hVertexBuffer)
	{
		SetVertexBuffer(pDevice,in_param->hVertexBuffer);
		SetPrimitiveTopology(pDevice,in_param->nVertexType);
	}
	if(in_param->hIndexBuffer)
		SetIndexBuffer(pDevice,in_param->hIndexBuffer);
	else
		pDevice->hNowIndexBuffer = 0;
	CEffectUnit* pEU = (CEffectUnit*)SYHTable_GetAt(&(pDevice->htEffectUnit),in_param->hEffect);
	CTechUnit*  pTU = pEU->GetTechnique(in_param->techName);
	ID3D10EffectTechnique* pTech = (ID3D10EffectTechnique*)(pTU->GetResourcePtr()->m_pTech);
	pDevice->pDevice->IASetInputLayout(pTU->GetResourcePtr()->m_pLayout);
	pTech->GetPassByIndex(0)->Apply(0);
	if(in_param->hIndexBuffer)
	{
		int nIndexCount = CalVertexCountByPrimitiveCount(in_param->nPrimitiveCount,in_param->nVertexType);
		pDevice->pDevice->DrawIndexed(nIndexCount,in_param->nStart,0);	
	}
	else
		pDevice->pDevice->Draw(in_param->nVertexCount,in_param->nStart);
	//for temp
	pDevice->pShader[0] = -1;
	pDevice->pShader[1] = -1;
	pDevice->pShader[2] = -1;
	//pDevice->nShaderTexstate[nShaderType];
	memset(pDevice->nShaderTexstate,0,sizeof(pDevice->nShaderTexstate));
    memset(pDevice->pConstBuffer,0,sizeof(int)*14*3);
	memset(pDevice->pNowSamplerState,0,sizeof(pDevice->pNowSamplerState));
	return true;
}

BOOL Render_EffectRenderInstance(int hDevice,void* paramin,void* paramout)
{
	DXEffectRenderInstanceIn* in_param = (DXEffectRenderInstanceIn*)paramin;
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	if (in_param->hVertexBuffer)
	{
		SetVertexInstanceBuffer(pDevice,in_param->hVertexBuffer,in_param->hInstanceBuf);
		SetPrimitiveTopology(pDevice,in_param->nVertexType);
	}
	if(in_param->hIndexBuffer)
		SetIndexBuffer(pDevice,in_param->hIndexBuffer);
	else
		pDevice->hNowIndexBuffer = 0;
	CEffectUnit* pEU = (CEffectUnit*)SYHTable_GetAt(&(pDevice->htEffectUnit),in_param->hEffect);
	CTechUnit*  pTU = pEU->GetTechnique(in_param->techName);
	ID3D10EffectTechnique* pTech = (ID3D10EffectTechnique*)(pTU->GetResourcePtr()->m_pTech);
	pDevice->pDevice->IASetInputLayout(pTU->GetResourcePtr()->m_pLayout);
	pTech->GetPassByIndex(0)->Apply(0);
	if(in_param->hIndexBuffer)
	{
		int nIndexCount = CalVertexCountByPrimitiveCount(in_param->nPrimitiveCount,in_param->nVertexType);
		pDevice->pDevice->DrawIndexedInstanced(nIndexCount,in_param->nInstanceCount,in_param->nStart, 0, 0 );
	}
	else
		pDevice->pDevice->DrawInstanced(in_param->nVertexCount,in_param->nInstanceCount,in_param->nStart,0);
	//for temp
	pDevice->pShader[0] = -1;
	pDevice->pShader[1] = -1;
	pDevice->pShader[2] = -1;
	//pDevice->nShaderTexstate[nShaderType];
	memset(pDevice->nShaderTexstate,0,sizeof(pDevice->nShaderTexstate));
	memset(pDevice->pConstBuffer,0,sizeof(int)*14*3);
	return true;
}

BOOL Render_EffectSetTexture(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	DXEffectSetTextureIn* in_param = (DXEffectSetTextureIn*)paramin;
	CEffectUnit* pEU = (CEffectUnit*)SYHTable_GetAt(&(pDevice->htEffectUnit),in_param->hEffect);
	ID3D10EffectShaderResourceVariable*	pTexVar = pEU->GetResourcePtr()->GetVariableByName(in_param->szTexName)->AsShaderResource();
	assert(pTexVar);
	SYTexture* pTex = (SYTexture*)SYHTable_GetAt(&pDevice->httexture,in_param->hTex);
	pTexVar->SetResource(pTex->pResourceView);
	memset(pDevice->nShaderTexstate,0,sizeof(pDevice->nShaderTexstate));
	return TRUE;
}

BOOL Render_GetMemTexBuffer(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	SYTexture* pTexture = (SYTexture*)SYHTable_GetAt(&pDevice->httexture,*(int*)paramin);
	DXGetMemTexBufferOut* pBuffer = (DXGetMemTexBufferOut*)paramout;
	if (pTexture->nType != DXTexture_MemTexture)
	{
		pBuffer->ptr = NULL;
		pBuffer->pitch = 0;
		return FALSE;
	}
	HRESULT hr ;
	if (!pTexture->pMemBuffer)
	{
		ID3D10Texture2D* pMem2DTex = (ID3D10Texture2D*)pTexture->pTexture;
		D3D10_MAPPED_TEXTURE2D mapTex;
		hr = pMem2DTex->Map(0,D3D10_MAP_READ,0,&mapTex );
		pTexture->pMemBuffer = mapTex.pData;
		pTexture->nPitch = mapTex.RowPitch;
		pBuffer->ptr = pTexture->pMemBuffer;
		pBuffer->pitch = pTexture->nPitch;
		pMem2DTex->Unmap(0);
		if(FALSE==g_bOptimal)
			pTexture->pMemBuffer = 0;
	}
	else
	{
		pBuffer->ptr = pTexture->pMemBuffer;
		pBuffer->pitch = pTexture->nPitch;
		hr = S_OK;
	}
	return hr == S_OK;
}
BOOL Render_EffectSetConstBuffer(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	DXEffectSetConstBufferIn* in_param = (DXEffectSetConstBufferIn*)paramin;
	CEffectUnit* pEU = (CEffectUnit*)SYHTable_GetAt(&(pDevice->htEffectUnit),in_param->hEffect);
	ID3D10EffectConstantBuffer*	pCBVar = pEU->GetResourcePtr()->GetVariableByName(in_param->szCBName)->AsConstantBuffer();
	assert(pCBVar);
	BOOL bVaild = 1;
	bVaild = pCBVar->IsValid();
	SYBuffer* pCB = (SYBuffer*)SYHTable_GetAt(&pDevice->htBuffer,in_param->hConstBuffer );
	pCBVar->SetConstantBuffer(pCB->pBuffer);
	D3D10_EFFECT_VARIABLE_DESC desc;
	pCBVar->GetDesc(&desc);
	pDevice->pConstBuffer[0][desc.ExplicitBindPoint] = -1;
	pDevice->pConstBuffer[1][desc.ExplicitBindPoint] = -1;
	pDevice->pConstBuffer[2][desc.ExplicitBindPoint] = -1;
	return TRUE;
}

BOOL Render_EffectSetVariable(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	DXEffectSetVariableIn* in_param = (DXEffectSetVariableIn*)paramin;
	CEffectUnit* pEU = (CEffectUnit*)SYHTable_GetAt(&(pDevice->htEffectUnit),in_param->hEffect);
	HRESULT hr = S_OK;
	D3D10_EFFECT_VARIABLE_DESC desc;
	switch(in_param->nType)
	{
	case DXVar_BOOL:
		{
			ID3D10EffectScalarVariable* pVar =   pEU->GetResourcePtr()->GetVariableByName(in_param->szVarName)->AsScalar();
			pVar->SetBool(*((BOOL*)in_param->pVariable));
			hr  = pVar->GetDesc(&desc);
			break;
		}
	case DXVar_INT:
		{
			ID3D10EffectScalarVariable* pVar =   pEU->GetResourcePtr()->GetVariableByName(in_param->szVarName)->AsScalar();
			pVar->SetInt(*((int*)in_param->pVariable));
			hr  = pVar->GetDesc(&desc);
			break;
		}
	case DXVar_FLOAT:
		{
			ID3D10EffectScalarVariable* pVar =   pEU->GetResourcePtr()->GetVariableByName(in_param->szVarName)->AsScalar();
			pVar->SetFloat(*((float*)in_param->pVariable));
			hr  = pVar->GetDesc(&desc);
			break;
		}
	case DXVar_BOOL_Array:
		{
			ID3D10EffectScalarVariable* pVar =   pEU->GetResourcePtr()->GetVariableByName(in_param->szVarName)->AsScalar();
			pVar->SetBoolArray((BOOL*)in_param->pVariable,0,in_param->nCount);
			hr  = pVar->GetDesc(&desc);
			break;
		}
	case DX_INT_Array:
		{
			ID3D10EffectScalarVariable* pVar =   pEU->GetResourcePtr()->GetVariableByName(in_param->szVarName)->AsScalar();
			pVar->SetIntArray((int*)in_param->pVariable,0,in_param->nCount);
			hr  = pVar->GetDesc(&desc);
			break;
		}
	case DXVar_FLOAT_Array:
		{
			ID3D10EffectScalarVariable* pVar =   pEU->GetResourcePtr()->GetVariableByName(in_param->szVarName)->AsScalar();
			pVar->SetFloatArray((float*)in_param->pVariable,0,in_param->nCount);
			hr  = pVar->GetDesc(&desc);
			break;
		}
	case DXVar_BOOL_Vector:
		{
			ID3D10EffectVectorVariable* pVar =   pEU->GetResourcePtr()->GetVariableByName(in_param->szVarName)->AsVector();
			pVar->SetBoolVector((BOOL*)in_param->pVariable);
			hr  = pVar->GetDesc(&desc);
			break;
		}
	case DXVar_INT_Vector:
		{
			ID3D10EffectVectorVariable* pVar =   pEU->GetResourcePtr()->GetVariableByName(in_param->szVarName)->AsVector();
			pVar->SetIntVector((int*)in_param->pVariable);
			hr  = pVar->GetDesc(&desc);
			break;
		}
	case DXVar_FLOAT_Vector:
		{
			ID3D10EffectVectorVariable* pVar =   pEU->GetResourcePtr()->GetVariableByName(in_param->szVarName)->AsVector();
			pVar->SetFloatVector((float*)in_param->pVariable);
			hr  = pVar->GetDesc(&desc);
			break;
		}
	case DXVar_BOOL_Vector_Array:
		{
			ID3D10EffectVectorVariable* pVar =   pEU->GetResourcePtr()->GetVariableByName(in_param->szVarName)->AsVector();
			pVar->SetBoolVectorArray((BOOL*)in_param->pVariable,0,in_param->nCount);
			hr  = pVar->GetDesc(&desc);
			break;
		}
	case DXVar_INT_Vector_Array:
		{
			ID3D10EffectVectorVariable* pVar =   pEU->GetResourcePtr()->GetVariableByName(in_param->szVarName)->AsVector();
			pVar->SetIntVectorArray((int*)in_param->pVariable,0,in_param->nCount);
			hr  = pVar->GetDesc(&desc);
			break;
		}
	case DXVar_FLOAT_Vector_Array:
		{
			ID3D10EffectVectorVariable* pVar =   pEU->GetResourcePtr()->GetVariableByName(in_param->szVarName)->AsVector();
			pVar->SetFloatVectorArray((float*)in_param->pVariable,0,in_param->nCount);
			pVar->GetDesc(&desc);
			break;
		}
	case DXVar_Matrix:
		{
			ID3D10EffectMatrixVariable* pVar =   pEU->GetResourcePtr()->GetVariableByName(in_param->szVarName)->AsMatrix();
			pVar->SetMatrix((float*)in_param->pVariable);
			hr  = pVar->GetDesc(&desc);
			break;
		}
	case DXVar_Matrix_Array:
		{
			ID3D10EffectMatrixVariable* pVar =   pEU->GetResourcePtr()->GetVariableByName(in_param->szVarName)->AsMatrix();
			pVar->SetMatrixArray((float*)in_param->pVariable,0,in_param->nCount);
			hr  = pVar->GetDesc(&desc);
			break;
		}
	}
	if(S_OK == hr)
	{
		pDevice->pConstBuffer[0][desc.ExplicitBindPoint] = -1;
		pDevice->pConstBuffer[1][desc.ExplicitBindPoint] = -1;
		pDevice->pConstBuffer[2][desc.ExplicitBindPoint] = -1;
	}
	return TRUE;
}

BOOL Render_Render2D(int hDevice,void* paramin,void* paramout)
{
	DXRender2D* pIn = (DXRender2D*)paramin;
	//计算proj
	float fLeft =	(float)pIn->pDstRt->left;
	float fRight =	(float)pIn->pDstRt->right;
	float fTop =	(float)pIn->pDstRt->top;
	float fBottom = (float)pIn->pDstRt->bottom;

	int nWidth = pIn->pTex->m_rtBound.right - pIn->pTex->m_rtBound.left;
	int nHeight = pIn->pTex->m_rtBound.bottom - pIn->pTex->m_rtBound.top;

	float3 vScale = {0},vAngle = {0};
	matrix4f_decompose(pIn->pWorldMatrix,&vScale,&vAngle,NULL);
	float fFar = (float)max(nWidth * vScale.x,nHeight * vScale.y);

	matrix4f projMat;
	CalProjectMat(&projMat,fLeft,fRight,-fBottom,-fTop,0.01f,fFar * 3,SY_ORTHO_PROJ);
	//计算纹理放缩mat
	matrix4f scaleMat;
	matrix4f_from_scale(&scaleMat,(float)nWidth , (float)nHeight ,1);
	scaleMat._41 = -(float)(pIn->pTex->m_nOriginX );
	scaleMat._42 = (float)(pIn->pTex->m_nOriginY );

	screenCB screen;
	matrix4f viewMat;
	matrix4f_from_pos(&viewMat,0,0,fFar - pIn->pWorldMatrix->_43);
	screen.world_View_proj = scaleMat * *pIn->pWorldMatrix * viewMat * projMat;

	float fX, fY, fZ;
	matrix4f_toeuler_lxyz(pIn->pWorldMatrix, &fX, &fY, &fZ);

	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	int nSrcWidth,nSrcHeight;
	getTextureInfo(pDevice,pIn->pTex->m_hTex,&nSrcWidth,&nSrcHeight,NULL);
	screen.texCoord.x = (pIn->pTex->m_rtBound.left + pIn->pTex->m_nOriginX) / (float)nSrcWidth;
	screen.texCoord.y = (pIn->pTex->m_rtBound.top + pIn->pTex->m_nOriginY) / (float)nSrcHeight;
	screen.texCoord.z = nWidth / (float)nSrcWidth;
	screen.texCoord.w = nHeight / (float)nSrcHeight;

	ModifyBuffer(pDevice,pDevice->hScreenCB,&screen);

	SetRenderState(pDevice,DXBlendState,DXBlend_Mix);
	SetRenderState(pDevice,DXDepthState,DXDepthDisable);
	SetRenderState(pDevice,DXRasterizerState,DXRas_Solid_NoCullBack_NoMultiSample);
	SetVertexBuffer(pDevice,pDevice->h2DVertex);
	SetPrimitiveTopology(pDevice,TRIANGLELIST);
	SetConstBuffer(pDevice,DXShaderType_VertexShader,0,pDevice->hScreenCB);
	float4 vPSParam[3];
	vPSParam[0].x = pIn->fAlpha;
	if (0)
	{
		//这个参数对应的是透明度
		vPSParam[0].x = 0.2;
	}
	ModifyBuffer(pDevice,pDevice->hModiyCB,vPSParam);
	SetConstBuffer(pDevice,DXShaderType_PixelShader,0,pDevice->hModiyCB);
	SetShader(pDevice,DXShaderType_VertexShader,pDevice->hScreenVS);
	SetShader(pDevice,DXShaderType_GeometryShader,NULL);
	SetShader(pDevice,DXShaderType_PixelShader,pIn->bAdustColor?pDevice->hScreenAdjustPS:pDevice->hScreenPS);
	SetSamplerState(pDevice,DXSample_Line_Clamp,DXShaderType_PixelShader,0);
	SetShaderTexture(pDevice,pIn->pTex->m_hTex,DXShaderType_PixelShader,0);

	pDevice->pDevice->Draw(6,0);	
	pDevice->info.nRenderCount++ ;
	pDevice->info.nTriangleCount += 2;
	pDevice->info.nRenderPixCount += nWidth * nHeight;
	return TRUE;
}
BOOL Render_MaskRender(int hDevice,void* paramin,void* paramout)
{
    SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
    DXMaskRenderIn* pIn = (DXMaskRenderIn*)paramin;

	int oldRT = pDevice->hNowRenderTarget;
	int oldDepth = pDevice->hNowDepth;
	RECT viewPort = pDevice->rtViewport;
	RECT rtScissor = pDevice->rtScissor;

	SetRenderTarget(pDevice,pIn->hTarget,0);
	SetViewPort(pDevice,pIn->pDstRt->left,pIn->pDstRt->top,pIn->pDstRt->right,pIn->pDstRt->bottom);
	SetScissor(pDevice,&pDevice->rtViewport);
	screenCB buffer;
	matrix4f_identity(&buffer.world_View_proj);

	float fSrcWidth = pIn->pCutRt->right - pIn->pCutRt->left;
	float fSrcHeight = pIn->pCutRt->bottom - pIn->pCutRt->top;

	buffer.texCoord.z =  1/ fSrcWidth;
	buffer.texCoord.w =  1/ fSrcHeight;

	buffer.texCoord.x = -pIn->pCutRt->left  * buffer.texCoord.z;
	buffer.texCoord.y = -pIn->pCutRt->top  * buffer.texCoord.w;

	ModifyBuffer(pDevice,pDevice->hScreenCB,&buffer);

	SetShader(pDevice,DXShaderType_VertexShader,pDevice->hScreenVS);
	SetShader(pDevice,DXShaderType_GeometryShader,NULL);
	SetShader(pDevice,DXShaderType_PixelShader,pDevice->hMaskRender);
	SetVertexBuffer(pDevice,pDevice->hScreenVertex);
	SetPrimitiveTopology(pDevice,TRIANGLELIST);
	SetConstBuffer(pDevice,DXShaderType_VertexShader,0,pDevice->hScreenCB);
	int nRasterState = pDevice->pNowState[DXRasterizerState];
	int nDepth = pDevice->pNowState[DXDepthState];
	SetRenderState(pDevice,DXRasterizerState,DXRas_Solid_NoCullBack_MultiSample);
	SetRenderState(pDevice,DXDepthState,DXDepthDisable);
	SetRenderState(pDevice,DXBlendState,DXBlendDisable);

	SetSamplerState(pDevice,DXSample_Anisotropic_Border,DXShaderType_PixelShader,0);

	SetShaderTexture(pDevice,pIn->hSrc,DXShaderType_PixelShader,0);
	SetShaderTexture(pDevice,pIn->hMask,DXShaderType_PixelShader,1);
	pDevice->pDevice->Draw(6,0);	
	pDevice->info.nRenderCount++ ;
	pDevice->info.nTriangleCount += 2;

	SetRenderState(pDevice,DXRasterizerState,nRasterState);
	SetRenderState(pDevice,DXDepthState,nDepth);

	SetRenderTarget(pDevice,oldRT,oldDepth);
	SetViewPort(pDevice,viewPort.left,viewPort.top,viewPort.right,viewPort.bottom);
	SetScissor(pDevice,&rtScissor);
	return TRUE;
}
BOOL Render_RenderDynamicTex(int hDevice,void* paramin,void* paramout)
{
    SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
    DXDynamicTextureIn* pIn = (DXDynamicTextureIn*)paramin;

	float fLeft =	(float)pIn->pTarget->m_rtBound.left;
	float fRight =	(float)pIn->pTarget->m_rtBound.right;
	float fTop =	(float)pIn->pTarget->m_rtBound.top;
	float fBottom = (float)pIn->pTarget->m_rtBound.bottom;

	int nWidth = pIn->pMask->m_rtBound.right - pIn->pMask->m_rtBound.left;
	int nHeight = pIn->pMask->m_rtBound.bottom - pIn->pMask->m_rtBound.top;
	float3 vScale = {0},vAngle = {0};
	matrix4f worldMat;
	matrix4f_identity(&worldMat);
	matrix4f_from_pos(&worldMat,-nWidth/2,nHeight/2,0);
	matrix4f_decompose(&worldMat,&vScale,&vAngle,NULL);
	float fFar = (float)max(nWidth * vScale.x,nHeight * vScale.y);
	matrix4f projMat;
	CalProjectMat(&projMat,-nWidth/2,nWidth/2,-nHeight/2,nHeight/2,0.f,fFar * 3,SY_ORTHO_PROJ);
	//计算纹理放缩mat
	matrix4f scaleMat;
	matrix4f_from_scale(&scaleMat,(float)nWidth , (float)nHeight ,1);
	//scaleMat._41 = -(float)(pIn->pMask->m_nOriginX );
	//scaleMat._42 = -(float)(pIn->pMask->m_nOriginY );

	screenCB screen;
	matrix4f viewMat;
	matrix4f_from_pos(&viewMat,0,0,0);
	screen.world_View_proj = scaleMat * worldMat * viewMat * projMat;

	int nSrcWidth,nSrcHeight;
	getTextureInfo(pDevice,pIn->pMask->m_hTex,&nSrcWidth,&nSrcHeight,NULL);
	screen.texCoord.x = (pIn->pMask->m_rtBound.left + pIn->pMask->m_nOriginX) / (float)nSrcWidth;
	screen.texCoord.y = (pIn->pMask->m_rtBound.top + pIn->pMask->m_nOriginY) / (float)nSrcHeight;
	screen.texCoord.z = nWidth / (float)nSrcWidth;
	screen.texCoord.w = nHeight / (float)nSrcHeight;

	getTextureInfo(pDevice,pIn->pSrc->m_hTex,&nSrcWidth,&nSrcHeight,NULL);
	DynamicPixCB dynamic;
	//matrix4f_identity(pIn->pSrcMat);
	dynamic.texCoord.x = (pIn->pSrc->m_rtBound.left + pIn->pSrc->m_nOriginX) / (float)nSrcWidth;
	dynamic.texCoord.y = (pIn->pSrc->m_rtBound.top + pIn->pSrc->m_nOriginY) / (float)nSrcHeight;
	dynamic.texCoord.z = (pIn->pSrc->m_rtBound.right - pIn->pSrc->m_rtBound.left) / (float)nSrcWidth;
	dynamic.texCoord.w = (pIn->pSrc->m_rtBound.bottom - pIn->pSrc->m_rtBound.top) / (float)nSrcHeight;
	dynamic.texWorld = *pIn->pSrcMat; 
	if (pIn->bUseColorAlpha)
	{
		dynamic.fUseColorAlpha = 1.0f;
	}
	else
	{
		dynamic.fUseColorAlpha = 0.0f;
	}

	int hOldTex = pDevice->hNowRenderTarget,hOldDepth = pDevice->hNowDepth;
	RECT viewPort = pDevice->rtViewport;
	RECT rtScissor = pDevice->rtScissor;
	RECT nowViewPort ;
	nowViewPort.left =	pIn->pTarget->m_rtBound.left + pIn->pTarget->m_nOriginX;
	nowViewPort.right =	pIn->pTarget->m_rtBound.right + pIn->pTarget->m_nOriginX;
	nowViewPort.top =	pIn->pTarget->m_rtBound.top + pIn->pTarget->m_nOriginY;
	nowViewPort.bottom =	pIn->pTarget->m_rtBound.bottom + pIn->pTarget->m_nOriginY;

	SetRenderTarget(pDevice,pIn->pTarget->m_hTex,0);
	SetViewPort(pDevice,nowViewPort.left,nowViewPort.top,nowViewPort.right,nowViewPort.bottom);
	SetScissor(pDevice,&nowViewPort);
	BeginRenderTexture(pDevice);

	//screen.world_View_proj._11=0.45;
	//screen.world_View_proj._22=1;
	//screen.world_View_proj._33=0.001;
	//screen.world_View_proj._41=-0.22;
	//screen.world_View_proj._42=0.51;
	//screen.world_View_proj._43=0.33;
	//screen.world_View_proj._44=1.0;

	ModifyBuffer(pDevice,pDevice->hScreenCB,&screen);
	ModifyBuffer(pDevice,pDevice->hDynamicCB,&dynamic);

	if (pIn->bUseColorAlpha)
	{
		SetRenderState(pDevice,DXBlendState,DXBlend_Mix);
	}
	else
	{
		SetRenderState(pDevice,DXBlendState,DXBlendDisable);
	}
	SetRenderState(pDevice,DXDepthState,DXDepthDisable);
	SetRenderState(pDevice,DXRasterizerState,DXRas_Solid_NoCullBack_NoMultiSample);
	SetVertexBuffer(pDevice,pDevice->h2DVertex);
	SetPrimitiveTopology(pDevice,TRIANGLELIST);
	SetConstBuffer(pDevice,DXShaderType_VertexShader,0,pDevice->hScreenCB);
	SetConstBuffer(pDevice,DXShaderType_PixelShader,0,pDevice->hDynamicCB);

	SetShader(pDevice,DXShaderType_VertexShader,pDevice->hScreenVS);
	SetShader(pDevice,DXShaderType_GeometryShader,NULL);
	SetShader(pDevice,DXShaderType_PixelShader,pDevice->hDynamicPS);
	SetSamplerState(pDevice,pIn->bRepeat?DXSample_Line_Wrap:DXSample_Line_Border,DXShaderType_PixelShader,0);

	SetShaderTexture(pDevice,pIn->pMask->m_hTex,DXShaderType_PixelShader,1);
	SetShaderTexture(pDevice,pIn->pSrc->m_hTex,DXShaderType_PixelShader,0);

	pDevice->pDevice->Draw(6,0);	 
	pDevice->info.nRenderCount++ ;
	pDevice->info.nTriangleCount += 2;
	EndRenderTexture(pDevice);

	SetRenderTarget(pDevice,hOldTex,hOldDepth);
	SetViewPort(pDevice,viewPort.left,viewPort.top,viewPort.right,viewPort.bottom);
	SetScissor(pDevice,&rtScissor);

	if(0)
	{
		DXDumpIn in;
		in.hTexture = pIn->pTarget->m_hTex;//合成目标纹理
		in.szFile = NULL;
		render_DumpTexture(hDevice,&in,NULL);
		in.hTexture = pIn->pMask->m_hTex;//源纹理
		render_DumpTexture(hDevice,&in,NULL);
		in.hTexture = pIn->pSrc->m_hTex;//动态纹理文件
		render_DumpTexture(hDevice,&in,NULL);
		
	}
	return TRUE;
}

BOOL Render_GetSwapChainBufferTex(int hDevice,void* paramin,void* paramout)
{
	int* hSwapChain = (int*)paramin;
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	SYSwapChain* pSwapChain = (SYSwapChain*)SYHTable_GetAt(&pDevice->htSwapChain,*hSwapChain);
	int* pOut = (int*)paramout;
	*pOut = (pDevice->pDevice == pDevice->pSwapChainDevice) ? pSwapChain->hBufferTexture : pSwapChain->hShareTexture;
	return TRUE;
}

BOOL Render_MultiToNormal(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	MultiToNormal* param = (MultiToNormal*)paramin;
	ID3D10Texture2D* pSrcTexture = getTexture(pDevice,param->hSrcTex);
	ID3D10Texture2D* pDstTexture = getTexture(pDevice,param->hDesTex);
	pDevice->pDevice->ResolveSubresource(pDstTexture,0,pSrcTexture,0,DXGI_FORMAT_R8G8B8A8_UNORM);
	return TRUE;
}

BOOL Render_Render3DVision(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	DX3DVisionTex* param = (DX3DVisionTex*)paramin;
	int oldRT = pDevice->hNowRenderTarget;
	int oldDepth = pDevice->hNowDepth;
	RECT oldRect = pDevice->rtViewport;
	SetRenderTarget(pDevice,param->hDstTex,0);
	int nDstBufferWidth = pDevice->rtViewport.right;
	int nDstBufferHeight = pDevice->rtViewport.bottom;
	int nSrcBufferWidth,nSrcBufferHeight;
	//ID3D10Texture2D* pTexture = getTexture(pDevice,param->hLeftTex);
	getTextureInfo(pDevice,param->hLeftTex,&nSrcBufferWidth,&nSrcBufferHeight,NULL);

	screenCB screen;
	matrix4f matWorld;
	matrix4f_identity(&matWorld);
	int nSrcWidth = (param->pSrcRt->right - param->pSrcRt->left);
	int nSrcHeight = (param->pSrcRt->bottom - param->pSrcRt->top);
	float fXScale = (float)(param->pDstRt->right - param->pDstRt->left) ;
	float fYScale = (float)(param->pDstRt->bottom - param->pDstRt->top);
	matWorld._11 = fXScale / 2.f;
	matWorld._22 = fYScale / 2.f;

	matrix4f posMat;
	matrix4f_identity(&posMat);
	float posX = param->pDstRt->left - (-matWorld._11 + nDstBufferWidth / 2.f) ;
	float posY = -(param->pDstRt->top - (-matWorld._22 + nDstBufferHeight / 2.f));
	posMat._41 = posX;
	posMat._42 = posY;
	posMat._43 = 0.5;

	float halfWidth = nDstBufferWidth / 2.f;
	float halfHeight = nDstBufferHeight / 2.f;
	//D3DXMATRIX matProjection;
	// Create the projection matrix using the values in the viewport
	matrix4f matProj;
	CalProjectMat(&matProj,-halfWidth,halfWidth,-halfHeight,halfHeight,0.1f,10,SY_ORTHO_PROJ);
	screen.world_View_proj = matWorld * posMat * matProj;
	float4 test = {-1,1,0,1};
	test *= screen.world_View_proj;

	screen.texCoord.x = param->pSrcRt->left / (float)nSrcBufferWidth;
	screen.texCoord.y = param->pSrcRt->top / (float)nSrcBufferHeight;

	screen.texCoord.z = nSrcWidth / (float)nSrcBufferWidth;
	screen.texCoord.w = nSrcHeight / (float)nSrcBufferHeight;

	ModifyBuffer(pDevice,pDevice->hScreenCB,&screen);
	SetConstBuffer(pDevice,DXShaderType_VertexShader,0,pDevice->hScreenCB);

	SetVertexBuffer(pDevice,pDevice->hScreenVertex);
	SetPrimitiveTopology(pDevice,TRIANGLELIST);

	SetShader(pDevice,DXShaderType_VertexShader,pDevice->hScreenVS);
	SetShader(pDevice,DXShaderType_GeometryShader,NULL);
	SetShader(pDevice,DXShaderType_PixelShader,pDevice->h3DVisionPS);

	SetRenderState(pDevice,DXDepthState,DXDepthDisable);
	SetRenderState(pDevice,DXBlendState,DXBlendDisable);
	SetRenderState(pDevice,DXRasterizerState,DXRas_Solid_NoCullBack_MultiSample);

	SetShaderTexture(pDevice,param->hLeftTex,DXShaderType_PixelShader,0);
	SetShaderTexture(pDevice,param->hRightTex,DXShaderType_PixelShader,1);

	pDevice->pDevice->Draw(6,0);	
	pDevice->info.nRenderCount++ ;
	pDevice->info.nTriangleCount += 2;

	SetRenderTarget(pDevice,oldRT,oldDepth);
	SetViewPort(pDevice,oldRect.left,oldRect.top,oldRect.right,oldRect.bottom);

	if(0)
	{
		render_DumpTexture(hDevice,&param->hLeftTex,0);
		render_DumpTexture(hDevice,&param->hRightTex,0);
	}
	return true;
}

BOOL Render_RenderTex3D(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);

	DXRenderTex* param = (DXRenderTex*)paramin;

	if (pDevice->pEffectMan->GetQuadVB() == 0)
	{
		float3	vb[] = 
		{
			{-0.5,-0.5,0.5},
			{0.5,0.5,0.5},
			{0.5,-0.5,0.5},

			{-0.5,-0.5,0.5},
			{-0.5,0.5,0.5},
			{0.5,0.5,0.5},
		};
		int hVB = CreateBuffer(pDevice,sizeof(float3),6,vb,DXVertexBuffer,TRIANGLELIST);
		if(0 == hVB)
			return FALSE;
		pDevice->pEffectMan->SetQuadVB(hVB);
	}

	SetVertexBuffer(pDevice,pDevice->pEffectMan->GetQuadVB());
	SetPrimitiveTopology(pDevice,TRIANGLELIST);

	SetRenderState(pDevice,DXBlendState,DXBlend_Mix);
	SetShader(pDevice,DXShaderType_VertexShader,pDevice->hRenderTexVS);
	SetShader(pDevice,DXShaderType_GeometryShader,NULL);
	SetShader(pDevice,DXShaderType_PixelShader,pDevice->hRenderTexPS);

	ModifyBuffer(pDevice,pDevice->hRenderTexCB,(void*)param->pMatrix);
	SetConstBuffer(pDevice,DXShaderType_VertexShader,0,pDevice->hRenderTexCB);
	SetShaderTexture(pDevice,param->hSrcTex,DXShaderType_PixelShader,0);
	SetSamplerState(pDevice,DXSample_Line_Border,DXShaderType_PixelShader,0);
	
	pDevice->pDevice->Draw(6,0);
	pDevice->info.nRenderCount++ ;
	pDevice->info.nTriangleCount += 2;

	return true;
}

BOOL Render_GetRenderInfo(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	DXRenderInfo* pOut = (DXRenderInfo*)paramout;
	*pOut = pDevice->info;
	return TRUE;
}

BOOL Render_ClearRenderInfo(int hDevice,void* paramin,void* paramout)
{
	SY_DeviceDX10* pDevice = (SY_DeviceDX10*)SYHTable_GetAt(&g_deviceHT,hDevice);
	pDevice->info.nRenderCount = 0;
	pDevice->info.nTriangleCount = 0;
	pDevice->info.nRenderPixCount = 0;
	return TRUE;
}

BOOL RenderEngineDX10_Main( int ncmd, int hDevice, void* paramin, void* paramout )
{
	//no need lock;
	switch(ncmd)
	{
	case RENDERENGINE_INIT:
		return Render_init(hDevice,paramin,paramout);
	case RENDERENGINE_ENGINE_LOCK:
	case RENDERENGINE_ENGINE_RES_LOCK:
		return Render_EngineLock(hDevice,paramin,paramout);
	case RENDERENGINE_ENGINE_UNLOCK:
	case RENDERENGINE_ENGINE_RES_UNLOCK:
		return Render_EngineUnLock(hDevice,paramin,paramout);
	case RENDERENGINE_RENDER_LOCK:
		return Render_RenderLock(hDevice,paramin,paramout);
	case RENDERENGINE_RENDER_UNLOCK:
		return Render_RenderUnLock(hDevice,paramin,paramout);
	case RENDERENGINE_UPLOAD:
		return Render_UpLoad(hDevice,paramin,paramout);
	case RENDERENGINE_CREATESHADER:
		return Render_CreateShader(hDevice,paramin,paramout);
	case RENDERENGINE_PRESENT:
		return Render_Present(hDevice,paramin,paramout);
	}
	//need to lock;
	CDX10EngineLock lock;
	switch (ncmd)
	{
	case RENDERENGINE_INIT:
		return Render_init(hDevice,paramin,paramout);
	case RENDERENGINE_RELEASE:
		return Render_Release(hDevice,paramin,paramout);
	case RENDERENGINE_CREATEDEVICE:
		return Render_CreateDevice(hDevice,paramin,paramout);
	case RENDERENGINE_CREATESWAPCHAIN:
		return Render_CreateSwapChain(hDevice,paramin,paramout);
	case RENDERENGINE_GET_SWAPCHAINDESC:
		return Render_GetSwapChainDesc(hDevice,paramin,paramout);
	case RENDERENGINE_COPY_TEXTURE:
		return Render_CopyTexture(hDevice,paramin,paramout);
	case RENDERENGINE_SET_RENDERTARGET:
		return Render_SetRenderTarget(hDevice,paramin,paramout);
	case RENDERENGINE_CREATE_TEXTURE:
		return Render_CreateTexture(hDevice,paramin,paramout);
	case RENDERENGINE_CLEAR_TEXTURE:
		return render_ClearTexture(hDevice,paramin,paramout);
	case RENDERENGINE_GET_TEXTURE:
		return render_GetTexture(hDevice,paramin,paramout);
	case RENDERENGINE_GET_TEXTURE_VIEW:
		return Render_GetTextureView(hDevice,paramin,paramout);
	case RENDERENGINE_DUMP_TEXTURE:
		return render_DumpTexture(hDevice,paramin,paramout);
	case RENDERENGINE_GET_TEXTURE_INFO:
		return render_GetTextureInfo(hDevice,paramin,paramout);
	case RENDERENGINE_RELEASE_TEXTURE:
		return Render_ReleaseTexture(hDevice,paramin,paramout);
	case RENDERENGINE_RELEASE_SWAPCHAIN:
		return Render_ReleaseSwapChain(hDevice,paramin,paramout);
	case RENDERENGINE_RESIZE_SWAPCHAIN:
		return Render_ResizeSwapChain(hDevice,paramin,paramout);
	case RENDERENGINE_DOWNLOAD:
		return Render_DownLoad(hDevice,paramin,paramout);
	case RENDERENGINE_BEGIN_RENDER_TEXTURE:
		return Render_BeginTex(hDevice,paramin,paramout);
	case RENDERENGINE_END_RENDER_TEXTURE:
		return Render_EndTex(hDevice,paramin,paramout);
	case RENDERENGINE_RENDER_TEXTURE:
		return Render_RenderTex(hDevice,paramin,paramout);
	case RENDERENGINE_GET_RENDERTARGET:
		return Render_GetRenderTarget(hDevice,paramin,paramout);
	case RENDERENGINE_CREATE_MODEL_FROM_X_FILE:
		return Render_CreateModelFromXFile(hDevice,paramin,paramout);
	case RENDERENGINE_RELEASESHADER:
		return Render_ReleaseShader(hDevice,paramin,paramout);
	case RENDERENGINE_TEXTURE_PROCESS:
		return Render_TexProcess(hDevice,paramin,paramout);
	case RENDERENGINE_SETVIEWPORT:
		return Render_SetViewPort(hDevice,paramin,paramout);
	case RENDERENGINE_GETVIEWPORT:
		return Render_GetViewPort(hDevice,paramin,paramout);
	case RENDERENGINE_CREATE_BUFFER:
		return Render_CreateBuffer(hDevice,paramin,paramout);
	case RENDERENGINE_RELEASE_BUFFER:
		return Render_ReleaseBuffer(hDevice,paramin,paramout);
	case RENDERENGINE_MODIFY_BUFFER:
		return Render_ModifyBuffer(hDevice,paramin,paramout);
	case RENDERENGINE_RENDER:
		return Render_Render(hDevice,paramin,paramout);
	case RENDERENGINE_EASY_RENDER:
		return Render_EasyRender(hDevice,paramin,paramout);
	case RENDERENGINE_RENDER_INSTANCE:
		return Render_InstanceRender(hDevice,paramin,paramout);
	case RENDERENGINE_SET_CONST_BUFFER:
		return Render_SetConstBuffer(hDevice,paramin,paramout);
	case RENDERENGINE_CREATE_TEXTURE_FROM_FILE:
		return Render_CreateTexFromFile(hDevice,paramin,paramout);
	case RENDERENGINE_SET_RENDER_STATE:
		return Render_SetRenderState(hDevice,paramin,paramout);
	case RENDERENGINE_SET_SAMPLER_STATE:
		return Render_SetSamplerState(hDevice,paramin,paramout);
	case RENDERENGINE_SET_SHADER_TEXTURE:
		return Render_SetShaderTexture(hDevice,paramin,paramout);
	case RENDERENGINE_SET_SHADER:
		return Render_SetShader(hDevice,paramin,paramout);
	case RENDERENGINE_CLEAR_SCREEN:
		return Render_ClearScreen(hDevice,paramin,paramout);
	case RENDERENGINE_HITTEST_MESH:
		return Render_HitTestMesh(hDevice,paramin,paramout);
	case RENDERENGINE_RENDER_MESH:
		return Render_RenderMesh(hDevice,paramin,paramout);
	case RENDERENGINE_GET_RENDER_STATE:
		return Render_GetRenderState(hDevice,paramin,paramout);
	case RENDERENGINE_GET_MESH_BOX:
		return Render_GetMeshBox(hDevice,paramin,paramout);
	case RENDERENGINE_GET_MESH_INFO:
		return Render_GetMeshInfo(hDevice,paramin,paramout);
	case RENDERENGINE_NORMAL_MAP:
		return Render_ComputeNormalMap(hDevice,paramin,paramout);
	case RENDERENGINE_GET_SCISSOR:
		return Render_GetScissor(hDevice,paramin,paramout);
	case RENDERENGINE_SET_SCISSOR:
		return Render_SetScissor(hDevice,paramin,paramout);
	case RENDERENGINE_GENERATE_MIPMAP:
		return Render_GenerateMipMap(hDevice,paramin,paramout);
	case RENDERENGINE_ENGINE_LOCK:
	case RENDERENGINE_ENGINE_RES_LOCK:
		return Render_EngineLock(hDevice,paramin,paramout);
	case RENDERENGINE_ENGINE_UNLOCK:
	case RENDERENGINE_ENGINE_RES_UNLOCK:
		return Render_EngineUnLock(hDevice,paramin,paramout);
	case RENDERENGINE_RENDER_LOCK:
		return Render_RenderLock(hDevice,paramin,paramout);
	case RENDERENGINE_RENDER_UNLOCK:
		return Render_RenderUnLock(hDevice,paramin,paramout);
	case RENDERENGINE_CREATE_EFFECTUNIT:
		return Render_CreatEffectUnit(hDevice,paramin,paramout);
	case RENDERENGINE_RELEASE_EFFECTUNIT:
		return Render_ReleaseEffectUnit(hDevice,paramin,paramout);
	case RENDERENGINE_EFFECT_GET_QUADVB:
		return Render_GetQuadVB(hDevice,paramin,paramout);
	case RENDERENGINE_EFFECT_SET_TEXTURE:
		return Render_EffectSetTexture(hDevice,paramin,paramout);
	case RENDERENGINE_GET_MESYEX_BUFFER:
		return Render_GetMemTexBuffer(hDevice,paramin,paramout);
	case RENDERENGINE_EFFECT_SET_CONST_BUFFER:
		return Render_EffectSetConstBuffer(hDevice,paramin,paramout);
	case RENDERENGINE_EFFECT_SET_VARIABLE:
		return Render_EffectSetVariable(hDevice,paramin,paramout);
	case RENDERENGINE_EFFECT_RENDER:
		return Render_EffectRender(hDevice,paramin,paramout);
	case RENDERENGINE_EFFECT_RENDER_INSTANCE:
		return Render_EffectRenderInstance(hDevice,paramin,paramout);
	case RENDERENGINE_RENDER_2D:
		return Render_Render2D(hDevice,paramin,paramout);
	case RENDERENGINE_RENDER_MASK:
		return Render_MaskRender(hDevice,paramin,paramout);
	case RENDERENGINE_RENDER_DYNAMIC_TEX:
		return Render_RenderDynamicTex(hDevice,paramin,paramout);
	case RENDERENGINE_RENDER_3DVISION:
		return Render_Render3DVision(hDevice,paramin,paramout);
	case RENDERENGINE_GET_SWAPCHAIN_BUFFER_TEX:
		return Render_GetSwapChainBufferTex(hDevice,paramin,paramout);
	case RENDERENGINE_MULTI_NORMAL:
		return Render_MultiToNormal(hDevice,paramin,paramout);
	case RENDERENGINE_RENDER_TEXTURE_3D:
		return Render_RenderTex3D(hDevice,paramin,paramout);
	case RENDERENGINE_GET_RENDER_INFO:
		return Render_GetRenderInfo(hDevice,paramin,paramout);
	case RENDERENGINE_CLEAR_RENDER_INFO:
		return Render_ClearRenderInfo(hDevice,paramin,paramout);
	}
	return true;
}

void RenderEngineDX10SetOptimal(BOOL bOptimal/* =TRUE */)
{
	g_bOptimal=bOptimal;
}

SY_DeviceDX10::SY_DeviceDX10()
{
	Reset();
}

void SY_DeviceDX10::Reset()
{
	pDevice = 0;
	pSwapChainDevice = 0;
	nAAFactor = 0;
	nMutiSampleCount = 0;
	nSampleQuality = 0;
	memset(&spriteProj, 0, sizeof(matrix4f));
	ShaderHash = 0;
	TextureFromFileHash = nullptr;
	BumpTextureFromFileHash = nullptr;
	memset(pDepthState, 0, DXDepthStateCount * sizeof(ID3D10DepthStencilState*));
	memset(pBlendState, 0, DXBlendStateCount * sizeof(ID3D10BlendState*));
	memset(pRasterizerState, 0, DXRasStateCount * sizeof(ID3D10RasterizerState*));
	memset(pSamplerState, 0, DXSampleStateCount * sizeof(ID3D10SamplerState*));
	hNowRenderTarget = 0;
	hNowDepth = 0;
	nNowPriType = 0;
	hNowIndexBuffer = 0;
	hNowVertexBuffer = 0;
	memset(pNowState, 0, DXRenderState_Count * sizeof(int));
	memset(pSpritState, 0, DXRenderState_Count * sizeof(int));
	memset(pNowSamplerState, 0, 3 * 16 * sizeof(int));
	memset(pShader, 0, 3 * sizeof(int));
	memset(pConstBuffer, 0, 3 * 14 * sizeof(int));
	memset(nShaderTexstate, 0, 3 * 128 * sizeof(int));
	memset(&rtViewport, 0, sizeof(RECT));
	memset(&rtScissor, 0, sizeof(RECT));
	hScreenVertex = 0;
	h2DVertex = 0;
	hScreenVS = 0;
	h3DVisionPS = 0;
	hScreenCB = 0;
	hDynamicCB = 0;
	hClearCB = 0;
	hModiyCB = 0;
	hConvertFormatPS = 0;
	hOnlyKeyOutPS = 0;
	hDynamicPS = 0;
	hBlur5PtPS = 0;
	hBlur9PtPS = 0;
	hClearScreenPS = 0;
	hMaskRender = 0;
	hScreenPS = 0;
	hScreenAdjustPS = 0;
	hScreenMSPS = 0;
	hUYVYBrightPS = 0;
	hFieldAddKeyPS = 0;
	hRGBA_Frame = 0;
	hRGBA_YUAYVA4224PS = 0;
	hUYVY_RGBA_PS0 = 0;
	hUYVY_RGBA_PS1 = 0;
	hUYVY_RGBA_KeyPS0 = 0;
	hUYVY_RGBA_KeyPS1 = 0;
	hUYVY_RGBA_Frame = 0;
	hRGBA_UYVY_Frame = 0;
	hRenderTexVS = 0;
	hRenderTexPS = 0;
	hRenderTexCB = 0;
	hRenderTexCB5 = 0;
	pEffectMan = nullptr;
	memset(&EngineLock, 0, sizeof(CRITICAL_SECTION));
	memset(&RenderLock, 0, sizeof(CRITICAL_SECTION));
	memset(&info, 0, sizeof(DXRenderInfo));
}
