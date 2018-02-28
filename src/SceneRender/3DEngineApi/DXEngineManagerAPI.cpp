#include "stdafx.h"
#include "SYEngineManagerAPI.h"
#include "SYRenderEngineDX10Api.h"
#include "SYRenderEngineSoftAPI.h"
#include "..\Algorithm\SYColorAPI.h"
#include "..\Algorithm\SYInterSection.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#ifdef _DEBUG
#define new SY_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
CSY3DLineManager* pLineManager = NULL;
TCHAR g_szPathShader[MAX_PATH] = {0};	

SYDeviceType g_dRenderType;
//////////////////////////////////////////////////////////////////////////
//CTextureSet封装类
CTextureSet::CTextureSet()
{
	InitializeCriticalSection(&m_Lock);
	m_hDevice = 0;
	m_nWidth  = 0;
	m_nHeight = 0;
	SYAR_Init(&m_arResource);
}
CTextureSet::~CTextureSet()
{
	int nCount = (int)SYAR_GetSize(&m_arResource);
	for (int i = 0; i < nCount; i++)
	{
		CSingleResource* pResource = (CSingleResource*)SYAR_GetAt(&m_arResource,i);
#ifdef _DEBUG
		assert(!pResource->bUsed);
#endif // _DEBUG
		SY3DEngine_ReleaseTexture(m_hDevice,pResource->hResource);
		SAFE_DELETE(pResource);
	}
	SYAR_RemoveAll(&m_arResource);
	DeleteCriticalSection(&m_Lock);
}
BOOL CTextureSet::Init( IN int hDevice, IN int nWidth, IN int nHeight,int nAAFactor )
{
	Lock();
	m_hDevice = hDevice;
	m_nWidth  = nWidth;
	m_nHeight = nHeight;
	m_nMaxFactor = nAAFactor;
	UnLock();
	return TRUE;
}
int CTextureSet::GetResource(IN int nIndex)
{
	CSingleResource* pResource = GetSingleResource(nIndex);
	if(pResource)
		return pResource->hResource;
	return 0;
}
int CTextureSet::GetResourceFactor(IN int nIndex)
{
	CSingleResource* pResource = GetSingleResource(nIndex);
	if(pResource)
		return pResource->nFactor;
	return -1;
}
bool CTextureSet::ResourceIsUsed(IN int nIndex)
{
	CSingleResource* pResource = GetSingleResource(nIndex);
	if(pResource)
		return pResource->bUsed;
	return TRUE;
}
void CTextureSet::SetResourceUsed( IN int nIndex, IN bool bUsed )
{
	Lock();
	if(nIndex >= 0 &&nIndex < GetCount())
	{
		GetSingleResource(nIndex)->bUsed = bUsed;
	}
	UnLock();
}
int CTextureSet::AddResource(int hResource,int nFactor)
{
	Lock();
	CSingleResource* newResoure = new CSingleResource;
	newResoure->bUsed = false;
	newResoure->hResource = hResource;
	newResoure->nFactor = nFactor;
	UnLock();
	return (int)SYAR_Add(&m_arResource,newResoure);
}
void CTextureSet::RemoveResource(int nIndex)
{
	CSingleResource* pResource = GetSingleResource(nIndex);
	SY3DEngine_ReleaseTexture(m_hDevice,pResource->hResource);
	SAFE_DELETE(pResource);
	SYAR_RemoveAt(&m_arResource,nIndex,1);
}
//////////////////////////////////////////////////////////////////////////
//SY3DLineManager封装类
CSY3DLineManager::CSY3DLineManager(int nMaxVertexSize/* = DEFAULT_LINE_NUM*/, int nMaxLineNum /*= DEFAULT_LINE_NUM*/)
{
	m_MaxVertexSize = max(10,nMaxVertexSize);
	m_pLineVertex = NULL;
	m_hLineVertex = 0;
	m_NumVertex = 0;
	m_dColor = 0xffffffff;
	m_pNowVertex = new SYEasyVertex;

	m_hMatrixBufferfog = 0;
	m_hVertexShaderfog = 0;
	m_hVertexShader = 0;
	m_hPixShader = 0;
	m_hPixShaderfog = 0;

}
CSY3DLineManager::~CSY3DLineManager()
{
	if (m_pLineVertex)
	{
		delete []m_pLineVertex;
		m_pLineVertex = NULL;
	}
	if(m_pNowVertex)
	{
		delete m_pNowVertex;
		m_pNowVertex = NULL;
	}
}
void CSY3DLineManager::ReleaseManager(int hDevice)
{
	if(m_hLineVertex)
	{
		SY3DEngine_ReleaseBuffer(hDevice,m_hLineVertex);
		m_hLineVertex = NULL;
	}
	if(m_hMatrixBuffer)
	{
		SY3DEngine_ReleaseBuffer(hDevice,m_hMatrixBuffer);
		m_hMatrixBuffer = NULL;
	}

	if(m_hVertexShader)
	{
		SY3DEngine_ReleaseShader(hDevice,m_hVertexShader);
		m_hVertexShader = NULL;
	}
	if(m_hPixShader)
	{
		SY3DEngine_ReleaseShader(hDevice,m_hPixShader);
		m_hPixShader = NULL;
	}

	if(m_hMatrixBufferfog)
	{
		SY3DEngine_ReleaseBuffer(hDevice,m_hMatrixBufferfog);
		m_hMatrixBufferfog = NULL;
	}

	if(m_hVertexShaderfog)
	{
		SY3DEngine_ReleaseShader(hDevice,m_hVertexShaderfog);
		m_hVertexShaderfog = NULL;
	}
	if(m_hPixShaderfog)
	{
		SY3DEngine_ReleaseShader(hDevice,m_hPixShaderfog);
		m_hPixShaderfog = NULL;
	}

}
void CSY3DLineManager::InitialManager(int hDevice)
{
	m_hDevice = hDevice;
	m_pLineVertex = new SYEasyVertex[m_MaxVertexSize];
	m_hLineVertex = SY3DEngine_CreateBuffer(hDevice,sizeof(SYEasyVertex),m_MaxVertexSize,m_pLineVertex,SYVertexBuffer,LINE_LIST);
	m_nVedioVertexNum = m_MaxVertexSize;
	{
		matrix4f temp;
		matrix4f_identity(&temp);

		m_hMatrixBuffer = SY3DEngine_CreateBuffer(hDevice,sizeof(matrix4f),1,&temp,SYConstBuffer);

		LPCTSTR strPath = g_szPathShader;
		size_t nCount = _tcsclen(VS_FILE) + _tcsclen(g_szPathShader) + 1;
		TCHAR* vsPath = new TCHAR[nCount];
		_tcscpy_s(vsPath,nCount,g_szPathShader);
		_tcscat_s(vsPath,nCount,VS_FILE);
		FILE* pVSFile;
		_wfopen_s(&pVSFile ,vsPath,_T("rb"));

		fseek(pVSFile, 0, SEEK_END);
		int len = ftell(pVSFile);
		fseek(pVSFile, 0, SEEK_SET);

		char* vsStr = new char[len + 1];
		fread(vsStr,len,1,pVSFile);
		fclose(pVSFile);
		vsStr[len] = 0;

		SYVertexType nType = SYVertex_POS_COLOR_TEX;
		m_hVertexShader = SY3DEngine_CreateShaderFromString(hDevice,FALSE,_T("Easy_VS"),vsStr,SYShaderType_VertexShader,&nType);
		SAFEDELETE_ARRAY(vsPath);
		SAFEDELETE_ARRAY(vsStr);

		nCount = _tcsclen(PS_FILE) + _tcsclen(strPath) + 1;
		TCHAR* psPath = new TCHAR[nCount];
		_tcscpy_s(psPath,nCount,strPath);
		_tcscat_s(psPath,nCount,PS_FILE);
		FILE* pPSFile;
		_wfopen_s(&pPSFile ,psPath,_T("rb"));

		fseek(pPSFile, 0, SEEK_END);
		len = ftell(pPSFile);
		fseek(pPSFile, 0, SEEK_SET);

		char* psStr = new char[len + 1];
		fread(psStr,len,1,pPSFile);
		fclose(pPSFile);
		psStr[len] = 0;

		m_hPixShader = SY3DEngine_CreateShaderFromString(hDevice,FALSE,_T("Easy_PS"),psStr,SYShaderType_PixelShader);
		SAFEDELETE_ARRAY(psPath);
		SAFEDELETE_ARRAY(psStr);
	}
	{
		matrix4f temp[2];
		matrix4f_identity(&temp[0]);
		matrix4f_identity(&temp[1]);

		m_hMatrixBufferfog = SY3DEngine_CreateBuffer(hDevice,sizeof(matrix4f)*2,1,temp,SYConstBuffer);

		LPCTSTR strPath = g_szPathShader;
		size_t nCount = _tcsclen(VS_FILE_FOG) + _tcsclen(g_szPathShader) + 1;
		TCHAR* vsPath = new TCHAR[nCount];
		_tcscpy_s(vsPath,nCount,g_szPathShader);
		_tcscat_s(vsPath,nCount,VS_FILE_FOG);
		FILE* pVSFile;
		_wfopen_s(&pVSFile ,vsPath,_T("rb"));

		fseek(pVSFile, 0, SEEK_END);
		int len = ftell(pVSFile);
		fseek(pVSFile, 0, SEEK_SET);

		char* vsStr = new char[len + 1];
		fread(vsStr,len,1,pVSFile);
		fclose(pVSFile);
		vsStr[len] = 0;

		SYVertexType nType = SYVertex_POS_COLOR_TEX;
		m_hVertexShaderfog = SY3DEngine_CreateShaderFromString(hDevice,FALSE,_T("Easy2_VS"),vsStr,SYShaderType_VertexShader,&nType);
		SAFEDELETE_ARRAY(vsPath);
		SAFEDELETE_ARRAY(vsStr);

		nCount = _tcsclen(PS_FILE_FOG) + _tcsclen(strPath) + 1;
		TCHAR* psPath = new TCHAR[nCount];
		_tcscpy_s(psPath,nCount,strPath);
		_tcscat_s(psPath,nCount,PS_FILE_FOG);
		FILE* pPSFile;
		_wfopen_s(&pPSFile ,psPath,_T("rb"));

		fseek(pPSFile, 0, SEEK_END);
		len = ftell(pPSFile);
		fseek(pPSFile, 0, SEEK_SET);

		char* psStr = new char[len + 1];
		fread(psStr,len,1,pPSFile);
		fclose(pPSFile);
		psStr[len] = 0;

		m_hPixShaderfog = SY3DEngine_CreateShaderFromString(hDevice,FALSE,_T("Easy2_PS"),psStr,SYShaderType_PixelShader);
		SAFEDELETE_ARRAY(psPath);
		SAFEDELETE_ARRAY(psStr);
	}
}

DWORD CSY3DLineManager::SetColor(DWORD color)
{
	DWORD oldColor = m_dColor;
	m_dColor = color;
	return oldColor;
}

void CSY3DLineManager::GetCurentPosition(float3* vNowPos)
{
	if (m_NumVertex == 0)
	{
		vNowPos->x = vNowPos->y = vNowPos->z =0;
	}
	*vNowPos = m_pLineVertex[m_NumVertex - 1].pos;
}

void CSY3DLineManager::MoveTo(int hDevice,const float3* vStartPos,DWORD* color /* = NULL */)
{
	if (color)
	{
		m_pNowVertex->color = *color;
	}
	else
		m_pNowVertex->color = m_dColor;

	//m_pLineVertex[m_NumVertex++].pos = *vStartPos;
	m_pNowVertex->pos = *vStartPos;
}

void CSY3DLineManager::LineTo(int hDevice,const float3* vEndPos,DWORD* color /* = NULL */)
{
	if (m_NumVertex + 2 >= m_MaxVertexSize)
	{
		m_MaxVertexSize = max(m_MaxVertexSize * 2,m_NumVertex + 2);
		SYEasyVertex* tmpVertex = new SYEasyVertex[m_MaxVertexSize];
		memcpy(tmpVertex,m_pLineVertex,m_NumVertex * sizeof(SYEasyVertex));
		delete []m_pLineVertex;
		m_pLineVertex = tmpVertex;
	}
	m_pLineVertex[m_NumVertex++] = *m_pNowVertex;
	if (color)
	{
		m_pLineVertex[m_NumVertex].color = *color;
	}
	else
		m_pLineVertex[m_NumVertex].color = m_dColor;

	m_pLineVertex[m_NumVertex++].pos = *vEndPos;
	*m_pNowVertex = m_pLineVertex[m_NumVertex - 1];
}

void CSY3DLineManager::Render(int hDevice,matrix4f* pViewPorjMat)
{
	if (m_NumVertex > 0)
	{
		if (m_NumVertex > m_nVedioVertexNum)
		{
			SY3DEngine_ReleaseBuffer(hDevice,m_hLineVertex);
			m_hLineVertex = SY3DEngine_CreateBuffer(hDevice,sizeof(SYEasyVertex),m_NumVertex,m_pLineVertex,SYVertexBuffer,LINE_LIST);
			m_nVedioVertexNum = m_NumVertex;
		}
		else
			SY3DEngine_ModifyBuffer(hDevice,m_hLineVertex,m_pLineVertex);
		SY3DEngine_ModifyBuffer(hDevice,m_hMatrixBuffer,pViewPorjMat);
		SY3DEngine_SetConstBuffer(hDevice,m_hMatrixBuffer,0,SYShaderType_VertexShader);
		SY3DEngine_SetRenderState(hDevice,SYRasterizerState,SYRas_Solid_NoCullBack_MultiSample);
		SY3DEngine_SetRenderState(hDevice,SYBlendState,SYBlend_Mix);
		SY3DEngine_Render(hDevice,m_hLineVertex,0,m_hVertexShader,0,m_hPixShader,0,m_NumVertex);
	}
}
void CSY3DLineManager::RenderFog(int hDevice,matrix4f* pViewMat,matrix4f* pProjMat)
{
	if (m_NumVertex > 0)
	{
		if (m_NumVertex > m_nVedioVertexNum)
		{
			SY3DEngine_ReleaseBuffer(hDevice,m_hLineVertex);
			m_hLineVertex = SY3DEngine_CreateBuffer(hDevice,sizeof(SYEasyVertex),m_NumVertex,m_pLineVertex,SYVertexBuffer,LINE_LIST);
			m_nVedioVertexNum = m_NumVertex;
		}
		else
			SY3DEngine_ModifyBuffer(hDevice,m_hLineVertex,m_pLineVertex);

		matrix4f mat[2];

		mat[0] = *pViewMat;
		mat[1] = *pProjMat;


		SY3DEngine_ModifyBuffer(hDevice,m_hMatrixBufferfog,mat);
		SY3DEngine_SetConstBuffer(hDevice,m_hMatrixBufferfog,0,SYShaderType_VertexShader);
		SY3DEngine_SetRenderState(hDevice,SYRasterizerState,SYRas_Solid_NoCullBack_MultiSample);
		SY3DEngine_SetRenderState(hDevice,SYBlendState,SYBlend_Mix);
		SY3DEngine_Render(hDevice,m_hLineVertex,0,m_hVertexShaderfog,0,m_hPixShaderfog,0,m_NumVertex);
	}
}
void CSY3DLineManager::Release(int hDevice)
{
	SY3DEngine_ReleaseBuffer(hDevice,m_hLineVertex);
	SY3DEngine_ReleaseBuffer(hDevice,m_hMatrixBuffer);
}
void CSY3DLineManager::DrawSquare(int hDevice,const float3* center, float length,const matrix4f* pMatrix)
{
	const int nVerNum = 8;
	if (m_NumVertex + nVerNum > m_MaxVertexSize)
	{
		m_MaxVertexSize = m_MaxVertexSize + 2 * nVerNum;
		SYEasyVertex* tmpVertex = new SYEasyVertex[m_MaxVertexSize];
		memcpy(tmpVertex,m_pLineVertex,m_NumVertex * sizeof(SYEasyVertex));
		delete []m_pLineVertex;
		m_pLineVertex = tmpVertex;
	}
	float3 pt[4];
	pt[0].x = center->x - length ; //0:x- y+ 
	pt[0].y = center->y + length ;
	pt[0].z = center->z ;

	pt[1].x = center->x - length ; //1:x- y- 
	pt[1].y = center->y - length ;
	pt[1].z = center->z ;

	pt[2].x = center->x + length ; //2:x+ y- 
	pt[2].y = center->y - length ;
	pt[2].z = center->z ;

	pt[3].x = center->x + length ; //3:x+ y+ 
	pt[3].y = center->y + length ;
	pt[3].z = center->z;
	for (int i = 0; i < nVerNum /2;i++)
	{
		m_pLineVertex[m_NumVertex].color = m_dColor;
		m_pLineVertex[m_NumVertex++].pos = pt[i] * *pMatrix;
		m_pLineVertex[m_NumVertex].color = m_dColor;
		m_pLineVertex[m_NumVertex++].pos = pt[(i + 1) % 4] * *pMatrix;
	}
}

void CSY3DLineManager::BeginRender()
{
	clearLine();
}
void CSY3DLineManager::clearLine()
{
	m_NumVertex = 0;
}
void CSY3DLineManager::DrawCircle( int hDevice,float fRad,const float3* pCenter,const matrix4f* pMatrix ,DWORD* pcolor /*= NULL*/)
{
	if(fRad < ABS_FLOAT_0)
		return;
	if ( pcolor == NULL)
	{
		pcolor = &m_dColor;
	}
	const int nCircleNum = 36;
	if (m_NumVertex + nCircleNum * 2 >= m_MaxVertexSize)
	{
		m_MaxVertexSize = m_MaxVertexSize + 2 * nCircleNum;
		SYEasyVertex* tmpVertex = new SYEasyVertex[m_MaxVertexSize];
		memcpy(tmpVertex,m_pLineVertex,m_NumVertex * sizeof(SYEasyVertex));
		delete []m_pLineVertex;
		m_pLineVertex = tmpVertex;
	}
	matrix4f mat; 
	if(pCenter)
	{
		matrix4f_from_pos(&mat,pCenter->x,pCenter->y,pCenter->z);
		mat = *pMatrix * mat;
	}
	else
		mat = *pMatrix;

	for(int i = 0; i < nCircleNum; i++)
	{
		float3 point;
		float fAngle = i * PAI_2F / (nCircleNum - 1);
		point.y = fRad * sinf(fAngle);
		point.x = fRad * cosf(fAngle);
		point.z = 0;
		m_pLineVertex[m_NumVertex].color = *pcolor;
		m_pLineVertex[m_NumVertex++].pos = point * mat;

		fAngle = (i + 1) * PAI_2F / (nCircleNum - 1);
		point.y = fRad * sinf(fAngle);
		point.x = fRad * cosf(fAngle);
		point.z = 0;
		m_pLineVertex[m_NumVertex].color = *pcolor;
		m_pLineVertex[m_NumVertex++].pos = point * mat;
	}
}

void CSY3DLineManager::DrawEllipse(int hDevice,float fRad_A,float fRad_B,const float3* pCenter,const matrix4f* pMatrix,DWORD* pcolor/* = NULL*/)
{
	if(fRad_A < ABS_FLOAT_0 || fRad_B < ABS_FLOAT_0)
		return;
	if ( pcolor == NULL)
	{
		pcolor = &m_dColor;
	}
	const int nCircleNum = 36;
	if (m_NumVertex + nCircleNum * 2 >= m_MaxVertexSize)
	{
		m_MaxVertexSize = m_MaxVertexSize + 2 * nCircleNum;
		SYEasyVertex* tmpVertex = new SYEasyVertex[m_MaxVertexSize];
		memcpy(tmpVertex,m_pLineVertex,m_NumVertex * sizeof(SYEasyVertex));
		delete []m_pLineVertex;
		m_pLineVertex = tmpVertex;
	}
	matrix4f mat; 
	if(pCenter)
	{
		matrix4f_from_pos(&mat,pCenter->x,pCenter->y,pCenter->z);
		mat = *pMatrix * mat;
	}
	else
		mat = *pMatrix;

	for(int i = 0; i < nCircleNum; i++)
	{
		float3 point;
		float fAngle = i * PAI_2F / (nCircleNum - 1);
		point.y = fRad_B * sinf(fAngle);
		point.x = fRad_A * cosf(fAngle);
		point.z = 0;
		m_pLineVertex[m_NumVertex].color = *pcolor;
		m_pLineVertex[m_NumVertex++].pos = point * mat;

		fAngle = (i + 1) * PAI_2F / (nCircleNum - 1);
		point.y = fRad_B * sinf(fAngle);
		point.x = fRad_A * cosf(fAngle);
		point.z = 0;
		m_pLineVertex[m_NumVertex].color = *pcolor;
		m_pLineVertex[m_NumVertex++].pos = point * mat;
	}
}
void CSY3DLineManager::DrawBox( int hDevice, const float3& vMin,const float3 vMax, const matrix4f& pMat ,DWORD* pColor)
{
	const int nAppendNums = 24;

	if (m_NumVertex + nAppendNums >= m_MaxVertexSize)
	{
		m_MaxVertexSize = max(m_MaxVertexSize * 2,m_NumVertex + nAppendNums);
		SYEasyVertex* tmpVertex = new SYEasyVertex[m_MaxVertexSize];
		memcpy(tmpVertex,m_pLineVertex,m_NumVertex * sizeof(SYEasyVertex));
		delete []m_pLineVertex;
		m_pLineVertex = tmpVertex;
	}
	float3 vPt[8];
	CreateBox(vPt,vMin,vMax, 0,0,0 );
	for(int i = 0;i < 8; i ++)
	{
		vPt[i] *= pMat;
	}
	for(int i = 0;i < nAppendNums; i++)
	{
		if(pColor)
			m_pLineVertex[m_NumVertex + i].color = *pColor;
		else
			m_pLineVertex[m_NumVertex + i].color = m_dColor;
	}

	m_pLineVertex[m_NumVertex++].pos = vPt[0];
	m_pLineVertex[m_NumVertex++].pos = vPt[1];

	m_pLineVertex[m_NumVertex++].pos = vPt[0];
	m_pLineVertex[m_NumVertex++].pos = vPt[3];

	m_pLineVertex[m_NumVertex++].pos = vPt[0];
	m_pLineVertex[m_NumVertex++].pos = vPt[4];

	m_pLineVertex[m_NumVertex++].pos = vPt[1];
	m_pLineVertex[m_NumVertex++].pos = vPt[2];

	m_pLineVertex[m_NumVertex++].pos = vPt[1];
	m_pLineVertex[m_NumVertex++].pos = vPt[5];

	m_pLineVertex[m_NumVertex++].pos = vPt[2];
	m_pLineVertex[m_NumVertex++].pos = vPt[6];

	m_pLineVertex[m_NumVertex++].pos = vPt[2];
	m_pLineVertex[m_NumVertex++].pos = vPt[3];

	m_pLineVertex[m_NumVertex++].pos = vPt[3];
	m_pLineVertex[m_NumVertex++].pos = vPt[7];

	m_pLineVertex[m_NumVertex++].pos = vPt[4];
	m_pLineVertex[m_NumVertex++].pos = vPt[5];

	m_pLineVertex[m_NumVertex++].pos = vPt[4];
	m_pLineVertex[m_NumVertex++].pos = vPt[7];

	m_pLineVertex[m_NumVertex++].pos = vPt[5];
	m_pLineVertex[m_NumVertex++].pos = vPt[6];

	m_pLineVertex[m_NumVertex++].pos = vPt[6];
	m_pLineVertex[m_NumVertex++].pos = vPt[7];

}

void CSY3DLineManager::DrawCone( int hDevice,float fUpRad,float fDownRad,float fHeight,const float3* pCenter,const matrix4f* pMatrix ,DWORD* pcolor /*= NULL*/ )
{
	matrix4f matLast;
	matrix4f_identity(&matLast);
	if(pCenter)
		matrix4f_from_pos(&matLast,pCenter->x,pCenter->y,pCenter->z);
	if(pMatrix)
		matLast = *pMatrix * matLast;

	DrawCircle( hDevice,fUpRad,NULL,&matLast ,pcolor );
	matrix4f matEnd ;
	matrix4f_from_pos(&matEnd,0,0,fHeight);
	matEnd *= matLast;
	DrawCircle( hDevice,fDownRad,NULL,&matEnd ,pcolor );

	float3 vStart = {fUpRad,0,0};
	vStart *= matLast;
	MoveTo(hDevice,&vStart,pcolor);
	float3 vEnd = {fDownRad,0,fHeight};
	vEnd *= matLast;
	LineTo(hDevice,&vEnd);

	float3Set(&vStart,-fUpRad,0,0);
	vStart *= matLast;
	MoveTo(hDevice,&vStart,pcolor);
	float3Set(&vEnd,-fDownRad,0,fHeight);
	vEnd *= matLast;
	LineTo(hDevice,&vEnd);

	float3Set(&vStart,0,-fUpRad,0);
	vStart *= matLast;
	MoveTo(hDevice,&vStart,pcolor);
	float3Set(&vEnd,0,-fDownRad,fHeight);
	vEnd *= matLast;
	LineTo(hDevice,&vEnd);

	float3Set(&vStart,0,fUpRad,0);
	vStart *= matLast;
	MoveTo(hDevice,&vStart,pcolor);
	float3Set(&vEnd,0,fDownRad,fHeight);
	vEnd *= matLast;
	LineTo(hDevice,&vEnd);
}
CTextureSet* g_TexSet = NULL;

BOOL SYTextureManager_Init( IN int hDevice, IN int nBufferWidth, IN int nBufferHeight, IN int nAAFactor )
{
	if(!g_TexSet)
	{
		g_TexSet = new CTextureSet;
		g_TexSet->Init(hDevice,nBufferWidth,nBufferHeight,nAAFactor);
	}

	return true;
}

BOOL SYTextureManager_ResetBufferSize( int nBufferWidth,int nBufferHeight )
{
	int nWidth = g_TexSet->GetWidth();
	int nHeight = g_TexSet->GetHeight();
	if(nWidth < nBufferWidth || nHeight < nBufferHeight)
	{
		int hDevice = g_TexSet->GetDevice();
		SAFE_DELETE(g_TexSet);
		g_TexSet = new CTextureSet;
		g_TexSet->Init(hDevice,nBufferWidth,nBufferHeight,g_TexSet->GetMaxFactor());
	}
	return true;
}

BOOL SYTextureManager_Release()
{
	SAFE_DELETE(g_TexSet);
	return TRUE;
}

int SYTexture_GetUnUsedTexture(CTextureSet* pTexture,int nWidth,int nHeight, SYTextureType nTexType)
{
	pTexture->Lock();
	int nCount = pTexture->GetCount();
	int getType = -1;
	for (int i = 0; i < nCount; i++)
	{
		if(pTexture->ResourceIsUsed(i))
			continue;
		int hTex = pTexture->GetResource(i);
		int nTexWidth = 0,nTexHeight = 0;
		SY3DEngine_GetTextureInfo(pTexture->GetDevice(),hTex,&nTexWidth,&nTexHeight,&getType);
		if (getType == nTexType && nWidth <= nTexWidth && nHeight <= nTexHeight )
		{
			pTexture->UnLock();
			return i;
		}
	}
	//没有找着，重新创建一个
	int hNewTex = SY3DEngine_CreateTexture(pTexture->GetDevice(),nWidth,nHeight,nTexType);
	if (hNewTex != 0)
	{
		pTexture->UnLock();
		return pTexture->AddResource(hNewTex,-1);
	}
	pTexture->UnLock();
	return -1;
}

int SYTexture_GetUnUsedTexture(CTextureSet* pTexture,int nAAFactor, SYTextureType nTexType,BOOL bForce = TRUE)
{
	pTexture->Lock();
	int nCount = pTexture->GetCount();
	int getType = -1;
	for (int i = 0; i < nCount; i++)
	{
		if(pTexture->ResourceIsUsed(i) || pTexture->GetResourceFactor(i) != nAAFactor)
			continue;
		int hTex = pTexture->GetResource(i);
		SY3DEngine_GetTextureInfo(pTexture->GetDevice(),hTex,NULL,NULL,&getType);
		if (getType == nTexType)
		{
			pTexture->UnLock();
			return i;
		}
	}
	//没有找着，重新创建一个
	if(!bForce)
	{
		pTexture->UnLock();
		return -1;
	}
	int hNewTex = SY3DEngine_CreateTexture(pTexture->GetDevice(),pTexture->GetWidth() * nAAFactor,pTexture->GetHeight() * nAAFactor,nTexType);
	if (hNewTex != 0)
	{
		pTexture->UnLock();
		return pTexture->AddResource(hNewTex,nAAFactor);
	}
	pTexture->UnLock();
	return -1;
}

int SYTexture_GetTexture(CTextureSet* pTexture, int hTexture)
{
	pTexture->Lock();
	int nCount = pTexture->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		int hTex = pTexture->GetResource(i);
		if (hTex == hTexture)
		{
			pTexture->UnLock();
			return i;
		}
	}
	pTexture->UnLock();
	return -1;
}

int SYTextureManager_QueryTexture( IN SYTextureType nTexType,int nFactor)
{
	g_TexSet->Lock();
	int nIndex = SYTexture_GetUnUsedTexture(g_TexSet,nFactor,nTexType);
	g_TexSet->SetResourceUsed(nIndex,true);
	g_TexSet->UnLock();
	int nRes =  g_TexSet->GetResource(nIndex);
	return nRes;
}

int SYTextureManager_QueryTexture( IN SYTextureType nTexType,int nWidth,int nHeight )
{
	g_TexSet->Lock();
	int nBaseWidth = SYTMGetTexBufferWidth();
	int nBaseHeight = SYTMGetTexBufferHeight();
	int nScale = max((nWidth - 1) / SYTMGetTexBufferWidth() ,(nHeight - 1) / SYTMGetTexBufferHeight() ) + 1;
	int nIndex = SYTexture_GetUnUsedTexture(g_TexSet,nScale,nTexType,nScale < 3);
	if(nIndex < 0 )
		nIndex = SYTexture_GetUnUsedTexture(g_TexSet,nWidth,nHeight,nTexType);
	assert( nIndex != -1 );
	g_TexSet->SetResourceUsed(nIndex,true);
	int nRes =  g_TexSet->GetResource(nIndex);
	g_TexSet->UnLock();
	return nRes;
}

BOOL SYTextureManager_UnQueryTexture(IN int hTexture,BOOL bForceRelease /*= FALSE*/)
{
	if(hTexture <= 0)
		return TRUE;
	g_TexSet->Lock();
	int nIndex = SYTexture_GetTexture(g_TexSet,hTexture) ;
	if(nIndex >= 0)
	{
		if(bForceRelease && g_TexSet->GetResourceFactor(nIndex) < 0)
			g_TexSet->RemoveResource(nIndex);
		else
			g_TexSet->SetResourceUsed(nIndex,false);
		g_TexSet->UnLock();
		return TRUE;
	}
	g_TexSet->UnLock();
	return FALSE;
}

int SYTMGetTexBufferWidth()
{
	return g_TexSet->GetWidth();
}

int SYTMGetTexBufferHeight()
{
	return g_TexSet->GetHeight();
}
//////////////////////////////////////////////////////////////////////////
BOOL SY3DEngine_Init(SYDeviceType nType,LPCTSTR lpPathShader)
{
	_tcscpy_s(g_szPathShader,lpPathShader);
	g_dRenderType = nType;
	if (g_dRenderType==SY_DX10_Device)
	{
		if (RenderEngineDX10_Main(RENDERENGINE_INIT, 0, (void*)lpPathShader, NULL ) == FALSE )
		{
			return FALSE;
		}
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		if (RenderSoftEngine_Main(RENDERENGINE_INIT, 0, (void*)lpPathShader, NULL ) == FALSE )
		{
			return FALSE;
		}
	}

	return TRUE;
}
int SY3DEngine_GetType()
{
	return g_dRenderType;
}

void SY3DEngine_Release()
{
	SYTextureManager_Release();
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_RELEASE,0,NULL,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_RELEASE,0,NULL,NULL);
	}	
}
BOOL SY3DEngine_Lock(int hDevice)
{
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_ENGINE_LOCK,hDevice,NULL,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_ENGINE_LOCK,hDevice,NULL,NULL);
	}	
	return TRUE;
}
BOOL SY3DEngine_UnLock(int hDevice)
{
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_ENGINE_UNLOCK,hDevice,NULL,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_ENGINE_UNLOCK,hDevice,NULL,NULL);
	}
	return TRUE;
}
BOOL SY3DEngine_ResLock(int hDevice)
{
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_ENGINE_RES_LOCK,hDevice,NULL,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_ENGINE_RES_LOCK,hDevice,NULL,NULL);
	}
	return TRUE;
}
BOOL SY3DEngine_ResUnLock(int hDevice)
{
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_ENGINE_RES_UNLOCK,hDevice,NULL,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_ENGINE_RES_UNLOCK,hDevice,NULL,NULL);
	}
	return TRUE;
}

BOOL SY3DEngine_RenderLock(int hDevice)
{
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_RENDER_LOCK,hDevice,NULL,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_RENDER_LOCK,hDevice,NULL,NULL);
	}
	return TRUE;
}
BOOL SY3DEngine_RenderUnLock(int hDevice)
{
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_RENDER_UNLOCK,hDevice,NULL,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_RENDER_UNLOCK,hDevice,NULL,NULL);
	}
	return TRUE;
}

int SY3DEngine_CreateSwapChain( int hDevice,HWND hWnd,int nWidth,int nHeight,BOOL bWindowed )
{
	createSwapChainIn In = {hWnd,nWidth,nHeight,bWindowed};
	int hSwapChain = 0;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_CREATESWAPCHAIN,hDevice,&In,&hSwapChain);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_CREATESWAPCHAIN,hDevice,&In,&hSwapChain);
	}
	return hSwapChain;
}

int SY3DEngine_GetSwapChainDesc(int hDevice,int nSwapchain,int& nWidth,int& nHeight,BOOL& bWindowed )
{
	createSwapChainIn In = {(HWND)nSwapchain,nWidth,nHeight,bWindowed};
	createSwapChainIn Out = {NULL,nWidth,nHeight,bWindowed};
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_GET_SWAPCHAINDESC,hDevice,&In,&Out);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_GET_SWAPCHAINDESC,hDevice,&In,&Out);
	}
	nWidth = Out.nWidth;
	nHeight = Out.nHeight;
	bWindowed = Out.bWindowed;
	return 1;
}


int SY3DEngine_CreateDevice(int nScreenWidth,int nScreenHeight,BOOL bDoubleDevice)
{
	int hNewDeivce = 0;
//	bDoubleDevice = FALSE;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_CREATEDEVICE,0,&bDoubleDevice,&hNewDeivce);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_CREATEDEVICE,0,&bDoubleDevice,&hNewDeivce);
	}
	if (hNewDeivce > 0)
	{
		SYTextureManager_Init(hNewDeivce,nScreenWidth,nScreenHeight,2);	
	}
	return hNewDeivce;
}
void SY3DEngine_ReleaseDevice(int hDevice)
{
	if (pLineManager)
	{
		pLineManager->ReleaseManager(hDevice);
		delete pLineManager;
		pLineManager=NULL;
	}
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_RELEASEDEVICE,hDevice,NULL,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_RELEASEDEVICE,hDevice,NULL,NULL);
	}
}
void SY3DEngine_MuliSampleTexToNormalTex(int hDevice,int hMultiSampleTex,int hNormalTex)
{
	MultiToNormal param;
	param.hDesTex = hNormalTex;
	param.hSrcTex = hMultiSampleTex;
	int nSrcWidth,nSrcHeight;
	SY3DEngine_GetTextureInfo(hDevice,hMultiSampleTex,&nSrcWidth,&nSrcHeight,NULL);
	int nDstWidth,nDstHeight;
	SY3DEngine_GetTextureInfo(hDevice,hNormalTex,&nDstWidth,&nDstHeight,NULL);
	if(nSrcWidth != nDstWidth || nSrcHeight != nDstHeight)
		SY3DEngine_CopyTexture(hDevice,hNormalTex,0,0,nSrcWidth,nSrcHeight,hMultiSampleTex,0,0,nSrcWidth,nSrcHeight);
	else
	{
		if (g_dRenderType==SY_DX10_Device)
		{
			RenderEngineDX10_Main(RENDERENGINE_MULTI_NORMAL,hDevice,&param,NULL);
		}
		else if(g_dRenderType==SY_Soft_Device)
		{
			RenderSoftEngine_Main(RENDERENGINE_MULTI_NORMAL,hDevice,&param,NULL);
		}
	}	
}
BOOL SY3DEngine_CopyTexture( int hDevice,int hDesTexture,int nOffsetX,int nOffsetY,int nDestWidth,int nDestHeight,int hSrcTexture,int nSrcOffsetX,int nSrcOffsetY,int nSrcWidth,int nSrcHeight )
{
	if(hDesTexture == hSrcTexture || !hDesTexture || !hSrcTexture)
		return FALSE;
	copyTexture param;
	param.hDesTex = hDesTexture;
	param.nDesOffsetX = nOffsetX;
	param.nDesOffsetY = nOffsetY;
	param.nDesWidth = nDestWidth;
	param.nDesHeight = nDestHeight;

	param.hSrcTex = hSrcTexture;
	param.nSrcWidth = nSrcWidth;
	param.nSrcHeight = nSrcHeight;
	param.nSrcOffsetX = nSrcOffsetX;
	param.nSrcOffsetY = nSrcOffsetY;
	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_COPY_TEXTURE,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_COPY_TEXTURE,hDevice,&param,NULL);
	}
	return FALSE;
}

BOOL SY3DEngine_SetRenderTarget( int hDevice,int hRenderTarget,int hDepth )
{
	setRenderTarget param;
	param.hDepth = hDepth;
	param.hRenderTarget = hRenderTarget;
	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_SET_RENDERTARGET,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_SET_RENDERTARGET,hDevice,&param,NULL);
	}
	return FALSE;
}

BOOL SY3DEngine_ClearDepth(int hDevice,int hTexture,float depth)
{ 
	if(hTexture <= 0)
		return FALSE;

	clearTextureIn param = {hTexture,&depth};
	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_CLEAR_TEXTURE,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_CLEAR_TEXTURE,hDevice,&param,NULL);
	}
	return FALSE;
}
BOOL SY3DEngine_ClearTexture(int hDevice,int hTexture,DWORD color)
{
	if(hTexture <= 0)
		return FALSE;

	clearTextureIn param = {hTexture,&color};
	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_CLEAR_TEXTURE,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_CLEAR_TEXTURE,hDevice,&param,NULL);
	}
	return FALSE;
}

int SY3DEngine_Present(int hDevice,int hSwapChain,int nTimeSyn /*= 0*/)
{
	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_PRESENT,hDevice,&hSwapChain,&nTimeSyn);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_PRESENT,hDevice,&hSwapChain,&nTimeSyn);
	}
	return FALSE;
}

int SY3DEngine_CreateTexture( int hDevice,int nWidth,int nHeight,SYTextureType nType )
{
	if(nWidth <= 0 ||nHeight <= 0)
		return NULL;
	createTextureIn param = {nWidth,nHeight,nType};
	param.pBuffer = NULL;
	int hTexture = 0;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_CREATE_TEXTURE,hDevice,&param,&hTexture);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_CREATE_TEXTURE,hDevice,&param,&hTexture);
	}
	return hTexture;
}

void* SY3DEngine_GetTexture( int hDevice,int hTexture )
{
	void* pTexture = NULL;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_GET_TEXTURE,hDevice,&hTexture,&pTexture);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_GET_TEXTURE,hDevice,&hTexture,&pTexture);
	}
	return pTexture;
}

void SY3DEngine_DumpTexture(int hDevice, int hTexture,LPCTSTR szFile )
{
	if(hTexture == 0)
		return;
	SYDumpIn paramIn;
	paramIn.hTexture = hTexture;
	paramIn.szFile = szFile;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_DUMP_TEXTURE,hDevice,&paramIn,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_DUMP_TEXTURE,hDevice,&paramIn,NULL);
	}
}

void SY3DEngine_GetTextureInfo( int hDevice,int hTexture,int* nWidth, int* nHeight,int* nType )
{
	if(hTexture == 0)
		return;
	getTextureInfoOut param = {nWidth,nHeight,nType,NULL};
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_GET_TEXTURE_INFO,hDevice,&hTexture,&param);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_GET_TEXTURE_INFO,hDevice,&hTexture,&param);
	}
}

bool SY3DEngine_ReleaseTexture( int hDevice,int hTexture )
{
	if(hTexture <= 0)
		return TRUE;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_RELEASE_TEXTURE,hDevice,&hTexture,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_RELEASE_TEXTURE,hDevice,&hTexture,NULL);
	}
	return TRUE;
}

BOOL SY3DEngine_ResizeSwapChain( int hDevice,int hSwapChain,int nWidth,int nHeight )
{
	ResizeSwapChainIn In = {hSwapChain,nWidth,nHeight};
	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_RESIZE_SWAPCHAIN,hDevice,&In,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_RESIZE_SWAPCHAIN,hDevice,&In,NULL);
	}
	return FALSE;
}

BOOL SY3DEngine_Download( OUT DWORD* pDesBuffer, 
						IN int hDevice,int hTransTex,/*用于下行的中间buffer,buffer类型必须为mem */ 
						IN int nDesOffestX, IN int nDesOffestY, IN int nDesWidth, IN int nDesHeight, IN int nDestBufferWidth, IN int nDestBufferHeight, 
						int hSrcTex,
						IN int nSrcOffestX, IN int nSrcOffestY, IN int nSrcWidth, IN int nSrcHeight, BUFFER_MODE eMode/* = BUFFER_COPY*/)
{
	bool isUnquery = false;
	if(nSrcWidth != nDesWidth || nSrcHeight != nDesHeight)
	{
		int hScaleTex = SYTextureManager_QueryTexture(SYTexture_Normal2DTex,nDesWidth,nDesHeight);
		SY3DEngine_CopyTexture(hDevice,hScaleTex,nSrcOffestX,nSrcOffestY,nDesWidth,nDesHeight,
				hSrcTex,nSrcOffestX,nSrcOffestY,nSrcWidth,nSrcHeight);
		hSrcTex = hScaleTex;
		isUnquery = true;
	}
	downloadBufferIn param;
	param.dstBuffer = pDesBuffer;
	param.dstOffsetX = nDesOffestX;
	param.dstOffsetY = nDesOffestY;
	param.dstWidth = nDesWidth;
	param.dstHeight = nDesHeight;
	param.dstBufferWidth = nDestBufferWidth;
	param.dstBufferHeight = nDestBufferHeight;

	param.hSrcTex = hSrcTex;
	param.srcOffsetX = nSrcOffestX;
	param.srcOffsetY = nSrcOffestY;
	param.hTransTex = hTransTex;
	param.eMode = eMode;
	BOOL rt=FALSE;
	if (g_dRenderType==SY_DX10_Device)
	{
		rt=RenderEngineDX10_Main(RENDERENGINE_DOWNLOAD,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		rt=RenderSoftEngine_Main(RENDERENGINE_DOWNLOAD,hDevice,&param,NULL);
	}
	if (isUnquery)
	{
		SYTextureManager_UnQueryTexture(hSrcTex,TRUE);
	}
	return rt;
}

BOOL SY3DEngine_UpLoad(OUT int hTex, IN int hDevice,int hTransTex,/*用于下行的中间buffer,buffer类型必须为mem */ 
					IN int nDesOffestX, IN int nDesOffestY, IN int nDesWidth, IN int nDesHeight, IN DWORD* pBuffer, 
					IN int nSrcOffestX, IN int nSrcOffestY, IN int nSrcWidth, IN int nSrcHeight, IN int nSrcBufferWidth, IN int nSrcBufferHeight ,
					BOOL bEngineLock)
{
	TIME_DEBUG_RECORD_FILTER("Render")
	assert(pBuffer);
	if(nDesWidth == 0 || nDesHeight == 0|| nSrcWidth == 0|| nSrcHeight == 0)
 		return FALSE;
	uploadBufferIn param;
	param.destOffsetX = nDesOffestX;
	param.destOffsetY = nDesOffestY;

	param.srcBuffer = pBuffer;
	param.srcOffsetX = nSrcOffestX;
	param.srcOffsetY = nSrcOffestY;
	param.srcWidth = nSrcWidth;
	param.srcHeight = nSrcHeight;
	param.srcBufferWidth = nSrcBufferWidth;
	param.srcBufferHeight = nSrcBufferHeight;

	param.hTransTex = hTransTex;
	param.bLock = bEngineLock;

	if(nSrcWidth != nDesWidth || nSrcHeight != nDesHeight)
	{
		int hScaleTex = SYTextureManager_QueryTexture(SYTexture_Normal2DTex,nSrcWidth,nSrcHeight);
		param.hDestTex = hScaleTex;
		BOOL rt=FALSE;
		if (g_dRenderType==SY_DX10_Device)
		{
			rt=RenderEngineDX10_Main(RENDERENGINE_UPLOAD,hDevice,&param,NULL);
		}
		else if(g_dRenderType==SY_Soft_Device)
		{
			rt=RenderSoftEngine_Main(RENDERENGINE_UPLOAD,hDevice,&param,NULL);
		}
		SY3DEngine_CopyTexture(hDevice,hScaleTex,nSrcOffestX,nSrcOffestY,nSrcWidth,nSrcHeight,
			hTex,nDesOffestX,nDesOffestY,nDesWidth,nDesHeight);
		SYTextureManager_UnQueryTexture(hScaleTex,TRUE);
		return rt;
	}
	else
	{
		param.hDestTex = hTex;
		if (g_dRenderType==SY_DX10_Device)
		{
			return RenderEngineDX10_Main(RENDERENGINE_UPLOAD,hDevice,&param,NULL);
		}
		else if(g_dRenderType==SY_Soft_Device)
		{
			return RenderSoftEngine_Main(RENDERENGINE_UPLOAD,hDevice,&param,NULL);
		}
		return FALSE;
	}
	return FALSE;
}

BOOL SY3DEngine_ComputeNormalMap(int hDevice,int hDestTex,int hSrcTex)
{
	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_NORMAL_MAP,hDevice,&hSrcTex,&hDestTex);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_NORMAL_MAP,hDevice,&hSrcTex,&hDestTex);
	}
	return FALSE;
}
void SY3DEngine_BeginRenderTexture(int hDevice)
{
	SY3DEngine_RenderLock(hDevice);
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_BEGIN_RENDER_TEXTURE,hDevice,NULL,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_BEGIN_RENDER_TEXTURE,hDevice,NULL,NULL);
	}
}

void SY3DEngine_RenderTexture( int hDevice,int hTex,float fAlpha,const RECT* pSrcRt,const RECT* pDesRt ,BOOL bAdjustColor,FIELD field,BOOL bStretch)
{
	renderTextureIn param = {hTex,pSrcRt,pDesRt,bAdjustColor,bStretch,fAlpha,field};
	int nWidth,nHeight,nType = 0;
	SY3DEngine_GetTextureInfo(hDevice,hTex,&nWidth,&nHeight,&nType);
	int hTransTex = 0;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_RENDER_TEXTURE,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_RENDER_TEXTURE,hDevice,&param,NULL);
	}
	if(hTransTex)
		SYTextureManager_UnQueryTexture(hTransTex);
}

void SY3DEngine_EndRenderTexture(int hDevice)
{
	SY3DEngine_RenderUnLock(hDevice);
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_END_RENDER_TEXTURE,hDevice,NULL,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_END_RENDER_TEXTURE,hDevice,NULL,NULL);
	}
}

BOOL SY3DEngine_ClearScreen( int hDevice, DWORD nColor, float fZValue, SYClearRTType nClearTarget )
{
	clearScreenIn param = {nColor,fZValue,nClearTarget};
	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_CLEAR_SCREEN,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_CLEAR_SCREEN,hDevice,&param,NULL);
	}
	return FALSE;
}

void SY3DEngine_GetRenderTarget( int hDevice,int* phRenderTarget,int* phDepth )
{
	getRenderTargetIn param = {phRenderTarget,phDepth};
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_GET_RENDERTARGET,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_GET_RENDERTARGET,hDevice,&param,NULL);
	}
}

int SY3DEngine_CreateMeshFromXFile( int hDevice,LPCTSTR lpszFileName )
{
	int hModel = 0;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_CREATE_MODEL_FROM_X_FILE,hDevice,(void*)lpszFileName,&hModel);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_CREATE_MODEL_FROM_X_FILE,hDevice,(void*)lpszFileName,&hModel);
	}
	return hModel;
}

int SY3DEngine_CreateShaderFromString( int hDevice,BOOL bEngineLock,LPCTSTR shaderName,LPCSTR shaderCode,SYShaderType nType ,SYVertexType* pVertexType)
{
	int hShader = 0;
	SYCreateShadeIn param;
	param.shaderFile = NULL;
	param.shaderCode = shaderCode;
	param.nType = nType;
	param.shaderName = shaderName;
	param.pType = pVertexType;
	param.bLock = bEngineLock;
	param.bComplied = FALSE;

	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_CREATESHADER,hDevice,&param,&hShader);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_CREATESHADER,hDevice,&param,&hShader);
	}
	return hShader;
}

int SY3DEngine_GetShader(int hDevice,LPCTSTR shaderName)//会增加引用计数，需要release;
{
	return SY3DEngine_CreateShaderFromString(hDevice,FALSE,shaderName,NULL,SYShaderType_PixelShader,NULL);
}

void SY3DEngine_ReleaseShader( int hDevice,int hShader )
{
	if (hShader == 0)
	{
		return;
	}
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_RELEASESHADER,hDevice,&hShader,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_RELEASESHADER,hDevice,&hShader,NULL);
	}
}

BOOL CheckForFieldAndRect(FIELD srcField,FIELD dstField,SYConvertType nType,const RECT& srcRt,RECT& dstRt)
{
	switch(nType)
	{
	case SY_RGBA_To_BGRA:
	case SY_RGBA_TO_Alpha:
	case SY_RGB_Alpha_To_Key:
	case SYAdjustAlpha:
		if(srcField != dstField)
			return FALSE;
		dstRt = srcRt;
		break;
	case SY_UYVYFrame_RGBA:
	case SY_UYVU_RGBA_Key:
		if(dstField != BOTH_FIELD)
			return FALSE;
		dstRt.left = srcRt.left * 2;
		dstRt.right = srcRt.right * 2;
		dstRt.top = srcRt.top * 2;
		dstRt.bottom = srcRt.bottom * 2;
		break;
	case SY_RGBA_YUAYVA4224:
		if(srcField != BOTH_FIELD)
			return FALSE;
		dstRt.left = srcRt.left * 3 / 4;
		dstRt.right = srcRt.right * 3 / 4;
		dstRt.top = srcRt.top / 2;
		dstRt.bottom = srcRt.bottom / 2;
		break;
	case SY_RGBA_UYVY:
		dstRt.left = srcRt.left ;
		dstRt.right = srcRt.right ;
		dstRt.top = srcRt.top / 2;
		dstRt.bottom = srcRt.bottom / 2;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

void SY3DEngine_ConvertFormatWithKey(int hDevice,int hSrcTex,int hSrcKey,FIELD srcField,const RECT* pSrcRt,int hDestTex,FIELD dstField,SYConvertType nType)
{
	RECT dstRT;
	if(!CheckForFieldAndRect(srcField,dstField,nType,*pSrcRt,dstRT))
	{
		ASSERT(0);
		return;
	}
	SYTexProcessIn param;
	param.hDestTex = hDestTex;
	param.hSrcTex = hSrcTex;
	param.pDstRt = &dstRT;
	param.pSrcRt = pSrcRt;
	param.nType = SYTextureConvertFormat;
	param.pData = &nType;
	param.hSrcKey = hSrcKey;
	param.nSrcField = srcField;
	param.nDstField = dstField;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_TEXTURE_PROCESS,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_TEXTURE_PROCESS,hDevice,&param,NULL);
	}
}
void SY3DEngine_ConvertFormat(int hDevice,int hSrcTex,FIELD srcField,const RECT* pSrcRt,int hDestTex,FIELD dstField,SYConvertType nType)
{
	RECT dstRT;
	if(!CheckForFieldAndRect(srcField,dstField,nType,*pSrcRt,dstRT))
	{
		ASSERT(0);
		return;
	}
	SYTexProcessIn param;
	param.hDestTex = hDestTex;
	param.hSrcTex = hSrcTex;
	param.pDstRt = &dstRT;
	param.pSrcRt = pSrcRt;
	param.nType = SYTextureConvertFormat;
	param.pData = &nType;
	param.hSrcKey = 0;
	param.nDstField = dstField;
	param.nSrcField = srcField;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_TEXTURE_PROCESS,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_TEXTURE_PROCESS,hDevice,&param,NULL);
	}
}

void SY3DEngine_SetViewPort( int hDevice, const RECT* pRect )
{
	RECT rt = *pRect;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_SETVIEWPORT,hDevice,(void*)&rt,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_SETVIEWPORT,hDevice,(void*)&rt,NULL);
	}
}

int SY3DEngine_CreateBuffer( int hDevice,int nVertexSize,int nVertexNum,void* pBuffer,SYBufferType nType,int nPriType /*= -1*/ )
{
	SYCreateBufferIn param;
	param.nVertexNum = nVertexNum;
	param.nPriType = nPriType;
	param.nVertexSize = nVertexSize;
	param.pBuffer = pBuffer;
	param.nType = nType;

	int hBuffer = 0;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_CREATE_BUFFER,hDevice,&param,&hBuffer);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_CREATE_BUFFER,hDevice,&param,&hBuffer);
	}
	return hBuffer;
}

BOOL SY3DEngine_ReleaseBuffer( int hDevice,int hBuffer )
{
	if (hBuffer <= 0)
		return FALSE;
	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_RELEASE_BUFFER,hDevice,&hBuffer,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_RELEASE_BUFFER,hDevice,&hBuffer,NULL);
	}
	return FALSE;
}

BOOL SY3DEngine_ModifyBuffer( int hDevice,int hBuffer,void* pBuffer )
{
	SYModifyBufferIn param;
	param.hBuffer = hBuffer;
	param.pBuffer = pBuffer;
	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_MODIFY_BUFFER,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_MODIFY_BUFFER,hDevice,&param,NULL);
	}
	return FALSE;
}
BOOL SY3DEngine_BeginRender( IN int hDevice, IN BOOL bAntiAlias/* = FALSE*/ )
{
	SY3DEngine_RenderLock(hDevice);
	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_BEGIN_RENDER,hDevice,NULL,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_BEGIN_RENDER,hDevice,NULL,NULL);
	}
	return FALSE;
}

BOOL SY3DEngine_EndRender(IN int hDevice)
{
	SY3DEngine_RenderUnLock(hDevice);
	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_END_RENDER,hDevice,NULL,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_END_RENDER,hDevice,NULL,NULL);
	}
	return FALSE;
}
BOOL SY3DEngine_SetVertexShader(int hDevice,int hVertexShader)
{
	SYSetShaderIn param = {SYShaderType_VertexShader,hVertexShader};
	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_SET_SHADER,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_SET_SHADER,hDevice,&param,NULL);
	}
	return FALSE;
}
BOOL SY3DEngine_SetGeometreShader(int hDevice,int hVertexShader)
{
	SYSetShaderIn param = {SYShaderType_GeometryShader,hVertexShader};
	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_SET_SHADER,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_SET_SHADER,hDevice,&param,NULL);
	}
	return FALSE;
}
BOOL SY3DEngine_SetPixShader(int hDevice,int hVertexShader)
{
	SYSetShaderIn param = {SYShaderType_PixelShader,hVertexShader};
	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_SET_SHADER,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_SET_SHADER,hDevice,&param,NULL);
	}
	return FALSE;
}
BOOL SY3DEngine_Render(int hDevice,int hVertexBuffer,int hIndexBuffer,int nVertexType ,int nStart,int PrimitiveCount,int nVertexCount)
{
	SYRenderIn param = {hVertexBuffer,hIndexBuffer,nVertexType,nStart,PrimitiveCount,nVertexCount};
	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_RENDER,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_RENDER,hDevice,&param,NULL);
	}
	return FALSE;
}
BOOL SY3DEngine_Render(int hDevice,int hVertexBuffer,int hIndexBuffer,int hVertexShader,int hGeometryShader,int hPixShader,int nStart,int nVertexCount)
{
	SYEasyRenderIn param;
	param.hVertexBuffer = hVertexBuffer;
	param.hIndexBuffer = hIndexBuffer;

	param.hVertexShader = hVertexShader;
	param.hGeometryShader = hGeometryShader;
	param.hPixShader = hPixShader;

	param.nStart = nStart;
	param.nVertexCount = nVertexCount;

	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_EASY_RENDER,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_EASY_RENDER,hDevice,&param,NULL);
	}
	return FALSE;
}
BOOL SY3DEngine_RenderInstance(int hDevice,int hVertexBuffer,int hInstanceBuffer,int hIndexBuffer,int nVertexType ,int nStart,int PrimitiveCount,int nVertexCount,int nInstanceCount)//nVertexCount兼容dx9
{
	SYRenderInstanceIn param;
	param.RenderData.hVertexBuffer = hVertexBuffer;
	param.RenderData.hIndexBuffer = hIndexBuffer;

	param.RenderData.nStart = nStart;
	param.RenderData.nVertexCount = nVertexCount;
	param.hInstanceBuffer = hInstanceBuffer;
	param.nInstanceCount = nInstanceCount;
	param.RenderData.nVertexType = nVertexType;
	param.RenderData.PrimitiveCount = PrimitiveCount;

	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_RENDER_INSTANCE,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_RENDER_INSTANCE,hDevice,&param,NULL);
	}
	return FALSE;
}

BOOL SY3DEngine_SetShaderTexture(int hDevice,int hTexture,int nRegisterIndex,SYShaderType nShaderType)
{
	SYSetShaderTexIn param = {hTexture,nRegisterIndex,nShaderType};
	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_SET_SHADER_TEXTURE,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_SET_SHADER_TEXTURE,hDevice,&param,NULL);
	}
	return FALSE;
}
BOOL SY3DEngine_SetConstBuffer(int hDevice,int hBuffer,int nRegisterIndex,SYShaderType nShaderType)
{
	SYSetConstBufferIn param = {hBuffer,nRegisterIndex,nShaderType};
	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_SET_CONST_BUFFER,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_SET_CONST_BUFFER,hDevice,&param,NULL);
	}
	return FALSE;
}

void SY3DEngine_MoveTo( int hDevice,const float3* vPos,DWORD* pColor /*= NULL*/ )
{
	pLineManager->MoveTo(hDevice,vPos,pColor);
}

void SY3DEngine_LineTo( int hDevice,const float3* vPos,DWORD* pColor /*= NULL*/ )
{
	pLineManager->LineTo(hDevice,vPos,pColor);
}

void SY3DEngine_RenderLine( int hDevice,matrix4f* pViewProjMat )
{
	SY3DEngine_RenderLock(hDevice);
	pLineManager->Render(hDevice,pViewProjMat);
	pLineManager->clearLine();
	SY3DEngine_RenderUnLock(hDevice);
}

void SY3DEngine_RenderLineFog(int hDevice,matrix4f* pViewMat,matrix4f* pProjMat)
{
	SY3DEngine_RenderLock(hDevice);
	pLineManager->RenderFog(hDevice,pViewMat,pProjMat);
	pLineManager->clearLine();
	SY3DEngine_RenderUnLock(hDevice);
}

int SY3DEngine_CreateTextureFromFile(int hDevice,TCHAR* pPath,BOOL bNormalMap)
{
	int hTexture;
	createTextureFromFileIn param = {pPath,bNormalMap};
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_CREATE_TEXTURE_FROM_FILE,hDevice,&param,&hTexture);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_CREATE_TEXTURE_FROM_FILE,hDevice,&param,&hTexture);
	}
	return hTexture;
}

void SY3DEngine_GetViewPort( int hDevice, RECT* pRect )
{
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_GETVIEWPORT,hDevice,NULL,pRect);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_GETVIEWPORT,hDevice,NULL,pRect);
	}
}

void SY3DEngine_CreateLineManage(int hDevice)
{
	pLineManager = new CSY3DLineManager;
	pLineManager->InitialManager(hDevice);
}
void SY3DEngine_ReleaseLineManage(int hDevice)
{
	if(pLineManager)
	{
		pLineManager->ReleaseManager(hDevice);
		delete pLineManager;
		pLineManager = NULL;
	}
}

int	SY3DEngine_GetRenderState(int hDevice,int nType)
{
	int nValue = -1;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_GET_RENDER_STATE,hDevice,&nType,&nValue);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_GET_RENDER_STATE,hDevice,&nType,&nValue);
	}
	return nValue;
}

BOOL SY3DEngine_SetRenderState(int hDevice,int nType,int nValue)
{
	SYRenderStateIn param = {nType,nValue};
	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_SET_RENDER_STATE,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_SET_RENDER_STATE,hDevice,&param,NULL);
	}
	return FALSE;
}

BOOL SY3DEngine_SetSampleState(int hDevice,int nSampler,int nState,int nShaderType)
{
	SYSampleStateIn param = {nSampler,nState,nShaderType};
	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_SET_SAMPLER_STATE,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_SET_SAMPLER_STATE,hDevice,&param,NULL);
	}
	return FALSE;
}

void* SY3DEngine_GetTextureView( int hDevice,int hTexture )
{
	void* pTextureView = NULL;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_GET_TEXTURE_VIEW,hDevice,&hTexture,&pTextureView);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_GET_TEXTURE_VIEW,hDevice,&hTexture,&pTextureView);
	}
	return pTextureView;
}

void SY3DEngine_GenerateMip(int hDevice,int hTexture)
{
	if(hTexture <= 0)
		return;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_GENERATE_MIPMAP,hDevice,&hTexture,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_GENERATE_MIPMAP,hDevice,&hTexture,NULL);
	}
}

int SY3DEngine_CreateShaderFromFile( int hDevice,LPCTSTR shaderName,LPCTSTR shaderFilePath,SYShaderType nType,SYVertexType* pVertexType /*= NULL/*FOR VS*/ )
{
	if (!SYCGlobalFunction::IsFileExists(shaderFilePath))
	{
		return 0;
	}
	int hShader = 0;
	SYCreateShadeIn param;
	param.shaderFile = shaderFilePath;
	param.shaderCode = NULL;
	param.nType = nType;
	param.shaderName = shaderName;
	param.pType = pVertexType;
	param.bLock = FALSE;
	param.bComplied = FALSE;

	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_CREATESHADER,hDevice,&param,&hShader);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_CREATESHADER,hDevice,&param,&hShader);
	}
	return hShader;
}

int	SY3DEngine_CreateShaderFromCompliedFile(int hDevice,LPCTSTR shaderName,LPCTSTR shaderFilePath,SYShaderType nType,SYVertexType* pVertexType)
{
	std::ifstream s_stream;
	size_t s_size;
	char* s_data;

	std::wstring path(g_szPathShader);	
	path += _T("\\FX\\");
	path += shaderFilePath;

	s_stream.open(path, std::ifstream::in | std::ifstream::binary);

	if(s_stream.good())
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
	SYCreateShadeIn param;
	param.shaderFile = NULL;
	param.shaderCode = s_data;
	param.nType = nType;
	param.shaderName = shaderName;
	param.pType = pVertexType;
	param.bLock = FALSE;
	param.bComplied = TRUE;
	param.compliedBinSize = s_size;

	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_CREATESHADER,hDevice,&param,&hShader);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_CREATESHADER,hDevice,&param,&hShader);
	}

	delete[] s_data;
	s_data = NULL;

	return hShader;
}

int SY3DEngine_HitMesh( int hDevice,int hMesh,const float3* pStart,const float3* pDir,float3* pHitPoint ,float* pfDist)
{
	SYHitTestMeshIn param = {hMesh,pStart,pDir};
	SYHitTestMeshOut paramOut;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_HITTEST_MESH,hDevice,&param,&paramOut);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_HITTEST_MESH,hDevice,&param,&paramOut);
	}
	if(pHitPoint)
		*pHitPoint = *pStart + *pDir * paramOut.fDist;
	if (pfDist)
		*pfDist = paramOut.fDist;
	return paramOut.nHitPart;
}

BOOL SY3DEngine_RenderMesh(int hDevice,int hMesh, int nDrawCount/* = -1*/, unsigned int* nSubset /*= NULL*/)
{
	SYRenderMeshIn param = {hMesh, nDrawCount,nSubset};
	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_RENDER_MESH,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_RENDER_MESH,hDevice,&param,NULL);
	}
	return FALSE;
}

#define PS_FILE _T("\\PS\\pos_normal_tex_tang_bi.PS")
int SY3DEngine_RenderEasyMesh( int hDevice,DWORD color,int hVertex,int hIndex, int nVertexType ,int nStrart,int PrimitiveCount,int nVertexCount )
{
	static int hPixShader = 0;
	static int hPSParam = SY3DEngine_CreateBuffer(hDevice,sizeof(float4),1,NULL,SYConstBuffer);
	if(!hPixShader)
	{
		size_t nCount = _tcsclen(PS_FILE) + _tcsclen(g_szPathShader) + 1;
		TCHAR psPath[1024];
		_tcscpy_s(psPath,g_szPathShader);
		_tcscat_s(psPath,PS_FILE);
		hPixShader = SY3DEngine_CreateShaderFromFile(hDevice,_T("pos_normal_tex_tang_bi_PS"),psPath,SYShaderType_PixelShader);
	}
	float4 vColor;
	DWORDColorToFloat4(&vColor,color);
	SY3DEngine_ModifyBuffer(hDevice,hPSParam,&vColor);
	SY3DEngine_SetConstBuffer(hDevice,hPSParam,0,SYShaderType_PixelShader);
	SY3DEngine_SetPixShader(hDevice,hPixShader);
	SY3DEngine_Render(hDevice,hVertex,hIndex,nVertexType,nStrart,PrimitiveCount,nVertexCount);
	return TRUE;
}

BOOL SY3DEngine_GetMeshInfo(int hDevice,int hMesh, int* nSubsetCount )
{
	SYRenderMeshIn param = {hMesh};
	SYGetMeshInfoOut paramOut; 
	BOOL bRet=FALSE;
	if (g_dRenderType==SY_DX10_Device)
	{
		bRet=RenderEngineDX10_Main(RENDERENGINE_GET_MESH_INFO, hDevice, &param, &paramOut);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		bRet=RenderSoftEngine_Main(RENDERENGINE_GET_MESH_INFO, hDevice, &param, &paramOut);
	}
	*nSubsetCount = paramOut.nSubSetCount;
	return bRet;
}
int SY3DEngine_ReleaseMesh( int hDevice,int hMesh )
{
	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_RELEASE_MESH,hDevice,&hMesh,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_RELEASE_MESH,hDevice,&hMesh,NULL);
	}
	return FALSE;
}
int SY3DEngine_GetMeshBox(int hDevice,int hMesh,float3* pMin,float3* pMax)
{
	SYGetMeshBoxIn param = {hMesh,pMin,pMax};
	if (g_dRenderType==SY_DX10_Device)
	{
		return RenderEngineDX10_Main(RENDERENGINE_GET_MESH_BOX,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		return RenderSoftEngine_Main(RENDERENGINE_GET_MESH_BOX,hDevice,&param,NULL);
	}
	return FALSE;
}

void SY3DEngine_SetScissor(int hDevice, const RECT* pRect)
{
	RECT rt = *pRect;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_SET_SCISSOR,hDevice,(void*)&rt,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_SET_SCISSOR,hDevice,(void*)&rt,NULL);
	}
}
void	SY3DEngine_GetScissor(int hDevice, RECT* pRect)
{
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_GET_SCISSOR,hDevice,NULL,pRect);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_GET_SCISSOR,hDevice,NULL,pRect);
	}
}

void SY3DEngine_DrawCircle( int hDevice,float fRad,const float3* pCenter,const matrix4f* pMatrix ,DWORD* pcolor /*= NULL*/)
{
	pLineManager->DrawCircle(hDevice,fRad,pCenter,pMatrix,pcolor);
}

void SY3DEngine_DrawEllipse(int hDevice,float fRad_A,float fRad_B,const float3* pCenter,const matrix4f* pMatrix ,DWORD* pcolor /*= NULL*/)
{
	pLineManager->DrawEllipse(hDevice,fRad_A,fRad_B,pCenter,pMatrix,pcolor);
}

void SY3DEngine_SetColor( int hDevice,DWORD Color )
{
	pLineManager->SetColor(Color);
}

void SY3DEngine_Square( int hDevice, const vector3f* center, float length,const matrix4f* pMatrix )
{
	pLineManager->DrawSquare(hDevice,center,length,pMatrix);
}

//PlayEffectSupport
int SY3DEngine_CreateEffectUnit(int hDevice,LPCTSTR EffectName,SYEffectTechDesc* techDesc,int nTechs)
{	
	int hEffectUnit = 0;
	SYCreateEffectUnitIn in_param;
	in_param.pEffectName = EffectName;
	in_param.nTechs = nTechs;
	in_param.pTechDesc = techDesc;

	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_CREATE_EFFECTUNIT,hDevice,&in_param,&hEffectUnit);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_CREATE_EFFECTUNIT,hDevice,&in_param,&hEffectUnit);
	}
	return hEffectUnit;
}
void SY3DEngine_ReleaseEffectUnit(int hDevice,int hEffect)
{
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_RELEASE_EFFECTUNIT,hDevice,&hEffect,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_RELEASE_EFFECTUNIT,hDevice,&hEffect,NULL);
	}
}
int SY3DEngine_GetQuadVB(int hDevice)
{
	int hVB = 0;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_EFFECT_GET_QUADVB,hDevice,NULL,&hVB);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_EFFECT_GET_QUADVB,hDevice,NULL,&hVB);
	}
	return hVB;
}	

void SY3DEngine_EffectSetTexture(int hDevice,int hEffect,int hTex,const char* texName)
{
	SYEffectSetTextureIn parmin = {hEffect, hTex, texName};
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_EFFECT_SET_TEXTURE,hDevice,&parmin, NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_EFFECT_SET_TEXTURE,hDevice,&parmin, NULL);
	}
}

void SY3DEngine_EffectSetConstBuffer(int hDevice,int hEffect,int hCB,const char* bufName)
{
	SYEffectSetConstBufferIn parmin = {hEffect, hCB, bufName};
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_EFFECT_SET_CONST_BUFFER,hDevice,&parmin,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_EFFECT_SET_CONST_BUFFER,hDevice,&parmin,NULL);
	}
}

void SY3DEngine_EffectSetVariable(int hDevice,int hEffect,void* pVariable, int nCount, SYEffectVariableType nType,const char* varName)
{
	SYEffectSetVariableIn parmin = {hEffect, pVariable, nCount, nType,varName};
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_EFFECT_SET_VARIABLE,hDevice,&parmin, NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_EFFECT_SET_VARIABLE,hDevice,&parmin, NULL);
	}
}


BOOL SY3DEngine_EffectRender(int hDevice,
							int hEffect,
							LPCTSTR TechniqueName,
							int hVertexBuffer,
							int hIndexBuffer,
							int nVertexType,
							int nPrimitiveCount,
							int nStart,
							int nVertexCount,
							int nIndexCount)
{
	SYEffectRenderIn in_param =
	{
		hEffect,
		TechniqueName,
		hVertexBuffer,
		hIndexBuffer,
		nVertexType,
		nPrimitiveCount,
		nStart,
		nVertexCount,
		nIndexCount,
	};
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_EFFECT_RENDER,hDevice,&in_param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_EFFECT_RENDER,hDevice,&in_param,NULL);
	}
	return TRUE;
}


BOOL SY3DEngine_EffectRenderInstance(int hDevice,
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
									int nInstanceCount)	
{
	SYEffectRenderInstanceIn in_param =
	{
		hEffect,
		TechniqueName,
		hVertexBuffer,
		hIndexBuffer,
		nVertexType,
		nPrimitiveCount,
		nStart,
		nVertexCount,
		nIndexCount,
		hInstanceBuffer,
		nInstanceCount,
	};
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_EFFECT_RENDER_INSTANCE,hDevice,&in_param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_EFFECT_RENDER_INSTANCE,hDevice,&in_param,NULL);
	}
	return TRUE;
}
int SY3DEngine_CreateTexFromBuffer(int hDevice,DWORD* pBuffer,int nWidth,int nHeight,SYTextureType nType)
{

	if(nWidth <= 0 ||nHeight <= 0 || pBuffer == NULL)
		return NULL;
	createTextureIn param = {nWidth,nHeight,nType,pBuffer}; 
	int hTexture = 0;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_CREATE_TEXTURE,hDevice,&param,&hTexture);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_CREATE_TEXTURE,hDevice,&param,&hTexture);
	}
	return hTexture;
}

void* SY3DEngine_GetMemTexBufferPtr( int hDevice, int hTex ,DWORD* pPitch)
{
	SYGetMemTexBufferOut out;
	if(hTex == 0)
		return NULL;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_GET_MESYEX_BUFFER,hDevice,&hTex,&out);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_GET_MESYEX_BUFFER,hDevice,&hTex,&out);
	}
	if(pPitch)
		*pPitch = out.pitch;
	return out.ptr;
}


void SY3DEngine_RenderTexture( int hDevice,SYHalDef32* pTex,const matrix4f* pWorldMatrix,const RECT* prtClip ,BOOL bAdjustColor,float fAlpha)
{
	SYRender2D in;
	in.pDstRt = prtClip;
	in.pTex = pTex;
	in.pWorldMatrix = pWorldMatrix;
	in.bAdustColor = bAdjustColor;
	in.fAlpha=fAlpha;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_RENDER_2D,hDevice,&in,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_RENDER_2D,hDevice,&in,NULL);
	}
}

void SY3DEngine_TextOut(int hDevice,LPCTSTR strText,RECT* rtText,DWORD dwColor)
{
    SYTextOutIn in;
    in.strText = strText;
    in.rtText = rtText;
    in.dwColor = dwColor;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_TEXTOUT,hDevice,&in,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_TEXTOUT,hDevice,&in,NULL);
	}
}   

void SY3DEngine_RenderMask(int hDevice,int hSrcTex,rectf& cutRt,int hMaskTex,SYHalDef32* pTargetTex)
{
	RECT bound = pTargetTex->m_rtBound;
	OffsetRect(&bound,pTargetTex->m_nOriginX,pTargetTex->m_nOriginY);
	SYMaskRenderIn in;
	in.hMask = hMaskTex;
	in.hSrc = hSrcTex;
	in.hTarget = pTargetTex->m_hTex;
	in.pCutRt = &cutRt;
	in.pDstRt = &bound;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_RENDER_MASK,hDevice,&in,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_RENDER_MASK,hDevice,&in,NULL);
	}
}

void SY3DEngine_Blur_5Pt(int hDevice,int hSrcTex,const RECT* pSrcRt,int hDestTex,const RECT* pDesRt,const SYDistortionParam* pParam)
{
	SYDistortionParam DistortionParam = {0};
	SYTexProcessIn param;
	param.hDestTex = hDestTex;
	param.hSrcTex = hSrcTex;
	param.hSrcKey = 0;
	param.pDstRt = pDesRt;
	param.pSrcRt = pSrcRt;
	param.nType = SYTextureBlur_5Pt;
	if(pParam)
		DistortionParam = *pParam;
	param.pData = &DistortionParam;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_TEXTURE_PROCESS,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_TEXTURE_PROCESS,hDevice,&param,NULL);
	}
}
void SY3DEngine_Blur_9Pt(int hDevice,int hSrcTex,const RECT* pSrcRt,int hDestTex,const RECT* pDesRt,const SYDistortionParam* pParam)
{
	SYDistortionParam DistortionParam = {0};
	SYTexProcessIn param;
	param.hDestTex = hDestTex;
	param.hSrcTex = hSrcTex;
	param.pDstRt = pDesRt;
	param.pSrcRt = pSrcRt;
	param.nType = SYTextureBlur_9Pt;
	if(pParam)
		DistortionParam = *pParam;
	param.hSrcKey = 0;
	param.pData = &DistortionParam;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_TEXTURE_PROCESS,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_TEXTURE_PROCESS,hDevice,&param,NULL);
	}
}
void SY3DEngine_FrontKeyOut(int hDevice,int hFrontTex,int hBackTex,const RECT* pSrcRt,int hDestTex,const RECT* pDestRt)
{
	SYTexProcessIn param;
	param.hDestTex = hDestTex;
	param.hSrcKey = hFrontTex;
	param.hSrcTex = hBackTex;
	param.pDstRt = pDestRt;
	param.nSrcField = BOTH_FIELD;
	param.nDstField = BOTH_FIELD;
	param.nType = SY_FIRST_KEY_MIX;
	param.pData = NULL;
	param.pSrcRt = pSrcRt;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_TEXTURE_PROCESS,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_TEXTURE_PROCESS,hDevice,&param,NULL);
	}
}

void SY3DEngine_DrawBox( int hDevice, const float3& vMin,const float3& vMax,const matrix4f& pMatrix ,DWORD* pcolor)
{
	pLineManager->DrawBox(hDevice,vMin,vMax,pMatrix,pcolor);
}

void SY3DEngine_RenderDynamicTex( int hDevice,matrix4f* pMat,BOOL bRepeat,BOOL bUseColorAlpha,SYHalDef32* pSrcTex,SYHalDef32* pMaskTex,SYHalDef32* pTargetTex )
{
	SYDynamicTextureIn in;
	in.pMask = pMaskTex;
	in.pSrc = pSrcTex;
	in.pTarget = pTargetTex;
	in.pSrcMat = pMat;
	in.bRepeat = bRepeat;
	in.bUseColorAlpha = bUseColorAlpha;
	if (g_dRenderType==SY_DX10_Device)
	{
	    RenderEngineDX10_Main(RENDERENGINE_RENDER_DYNAMIC_TEX,hDevice,&in,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_RENDER_DYNAMIC_TEX,hDevice,&in,NULL);
	}
}

void SY3DEngine_DrawCone( int hDevice,float fUpRad,float fDownRad,float fHeight,const float3* pCenter,const matrix4f* pMatrix ,DWORD* pcolor /*= NULL*/ )
{
	pLineManager->DrawCone( hDevice,fUpRad,fDownRad,fHeight,pCenter,pMatrix ,pcolor );
}

void SY3DEngine_Render3DVisionTex(int hDevice,int hDstTex,const RECT* pDstRt,int hLeftTex,int hRightTex,const RECT* pSrcRt)
{
	SY3DVisionTex param;
	param.hDstTex = hDstTex;
	param.hLeftTex = hLeftTex;
	param.hRightTex = hRightTex;
	param.pDstRt = pDstRt;
	param.pSrcRt = pSrcRt;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_RENDER_3DVISION,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_RENDER_3DVISION,hDevice,&param,NULL);
	}
}

void SY3DEngine_ReleaseSwapChain( int hDevice,int hSwapChain )
{
	if(!hSwapChain)
		return;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_RELEASE_SWAPCHAIN,hDevice,&hSwapChain,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_RELEASE_SWAPCHAIN,hDevice,&hSwapChain,NULL);
	}
}

int SY3DEngine_GetSwapChainBuffer( int hDevice,int hSwapChain )
{
	if(!hSwapChain)
		return 0;
	int hBuffer = 0;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_GET_SWAPCHAIN_BUFFER_TEX,hDevice,&hSwapChain,&hBuffer);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_GET_SWAPCHAIN_BUFFER_TEX,hDevice,&hSwapChain,&hBuffer);
	}
	return hBuffer;
}

void SY3DEngine_RenderTexture( int hDevice, int hTex, matrix4f* pMatrix)										 
{
	SYRenderTex in;
	in.hSrcTex = hTex;
	in.pMatrix = pMatrix;
	in.hMaskTex = 0;

	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_RENDER_TEXTURE_3D,hDevice,&in,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_RENDER_TEXTURE_3D,hDevice,&in,NULL);
	}
}
 
void SY3DEngine_GetRenderInfo( int hDevice, SYRenderInfo* pInfo)
{
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_GET_RENDER_INFO,hDevice,NULL,pInfo);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_GET_RENDER_INFO,hDevice,NULL,pInfo);
	}
}	

void SY3DEngine_ClearRenderInfo( int hDevice)
{
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_CLEAR_RENDER_INFO,hDevice,NULL,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_CLEAR_RENDER_INFO,hDevice,NULL,NULL);
	}
}	

void SY3DEngine_DumpRenderTarget( int hDevice,LPCTSTR szFile /*= NULL*/ )
{
	int hRT;
	SY3DEngine_GetRenderTarget(hDevice,&hRT,NULL);
	SY3DEngine_DumpTexture(hDevice,hRT,szFile);
}

int SY3DEngine_GetTextureMipMap( int hDevice,int hTexture )
{
	int nMipMap = 1;
	getTextureInfoOut param = {NULL,NULL,NULL,&nMipMap};
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_GET_TEXTURE_INFO,hDevice,&hTexture,&param);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_GET_TEXTURE_INFO,hDevice,&hTexture,&param);
	}
	return nMipMap;
}

size_t SY3DEngine_GetTextureMemSize( int hDevice,int hTexture )
{
	if(hTexture <= 0)
		return 0;
	int nWidth,nHeight,nType;
	SY3DEngine_GetTextureInfo(hDevice,hTexture,&nWidth,&nHeight,&nType);
	int nScale = 1;
	if(nType == SYTexture_MS2DTex || nType == SYTexture_MSDepth)
		nScale = 4;
	int nMipMap = SY3DEngine_GetTextureMipMap(hDevice,hTexture);

	size_t BufferSize = nWidth * nHeight * sizeof(DWORD);
	BufferSize = SY_DToI( BufferSize * 4 /3. - (nMipMap > 0 ?(BufferSize >> ((nMipMap - 1) * 2)) : 0) / 3.);
	return BufferSize * nScale;
}

void SY3DEngine_ProcessBright( int hDevice,int eTexType,int hTex,const RECT* pSrcRt,int hDestTex,const RECT* pDestRt,float fBrightGain )
{
	SYTexProcessIn param;
	param.hDestTex = hDestTex;
	param.hSrcKey = NULL;
	param.hSrcTex = hTex;
	param.pDstRt = pDestRt;
	param.nSrcField = BOTH_FIELD;
	param.nDstField = BOTH_FIELD;
	param.nType = SY_UYVY_BRIGHT;
	param.pData = &fBrightGain;
	param.pSrcRt = pSrcRt;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_TEXTURE_PROCESS,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_TEXTURE_PROCESS,hDevice,&param,NULL);
	}
}

BOOL		SY3DEngine_GetDeviceRemoveReason(int hDevice)
{
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_GET_REMOVE_REASON,hDevice,NULL,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_GET_REMOVE_REASON,hDevice,NULL,NULL);
	}
	return FALSE;
}

int SY3DEngine_CreateContext( int hDevice )
{
	int hContext = 0;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_CREATE_CONTEXT,hDevice,NULL,&hContext);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_CREATE_CONTEXT,hDevice,NULL,&hContext);
	}
	return hContext;
}

void SY3DEngine_ReleaseContext( int hDevice,int hContext )
{
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_RELEASE_CONTEXT,hDevice,&hContext,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_RELEASE_CONTEXT,hDevice,&hContext,NULL);
	}
}

void SY3DEngine_SetContext( int hDevice,int hContext )
{
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_SET_CONTEXT,hDevice,&hContext,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_SET_CONTEXT,hDevice,&hContext,NULL);
	}
}

void SY3DEngine_RGBA_To_BGRA( int hDevice,int hSrcTex,const RECT* pSrcRt,int hDestTex,const RECT* pDesRt )
{
	int nType = SY_RGBA_To_BGRA;
	SYTexProcessIn param;
	param.hDestTex = hDestTex;
	param.hSrcTex = hSrcTex;
	param.pDstRt = pDesRt;
	param.pSrcRt = pSrcRt;
	param.nType = SYTextureConvertFormat;
	param.pData = &nType;
	param.hSrcKey = 0;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_TEXTURE_PROCESS,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_TEXTURE_PROCESS,hDevice,&param,NULL);
	}
}

void SY3DEngine_MakeFrame( int hDevice,int hFirstTex,int hSecondTex,const RECT* pRect,int hDestTex )
{
	int nType = SY_RGBA_To_FRAME;
	SYTexProcessIn param;
	param.hDestTex = hDestTex;
	param.hSrcTex = hFirstTex;
	param.hSrcKey = hSecondTex;
	param.pDstRt = pRect;
	param.pSrcRt = pRect;
	param.nType = SYTextureConvertFormat;
	param.pData = &nType;
	if (g_dRenderType==SY_DX10_Device)
	{
		RenderEngineDX10_Main(RENDERENGINE_TEXTURE_PROCESS,hDevice,&param,NULL);
	}
	else if(g_dRenderType==SY_Soft_Device)
	{
		RenderSoftEngine_Main(RENDERENGINE_TEXTURE_PROCESS,hDevice,&param,NULL);
	}
}

void EngineManagerSetOptimal(BOOL bOptimal/* =TRUE */)
{
	RenderEngineDX10SetOptimal(bOptimal);
}