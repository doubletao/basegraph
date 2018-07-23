#include "stdafx.h"
#include "DataRender\MaterialDef.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
SYMatBufferCallBack g_pObjectFun = NULL;
void*				g_pObjectParam = NULL;
SYMatBufferCallBack g_pChannelFun = NULL;
void*				g_pChannelParam = NULL;

TCHAR*	gShaderPath = NULL;
char*	g_strShaderMain = NULL;
char*	g_strShaderMainHead = NULL;
int		g_nMainSize = 0;
int		g_nMainHeadSize = 0;
BASEGRAPH_DATARENDER_API void SYMaterialModuleInit(LPCTSTR pShaderPath,SYMatBufferCallBack pObject /*= NULL*/,void* pObjParam /*= NULL*/,SYMatBufferCallBack pChannel /*= NULL*/,void* pChannelParam /*= NULL*/)
{
	const TCHAR* strMainFile = _T("\\PS\\materialPS\\main.txt");
	const TCHAR* strMainHeadFile = _T("\\PS\\materialPS\\mainHead.txt");
	size_t nLen = _tcsclen(pShaderPath) + 1;
	SAFEDELETE_ARRAY(gShaderPath);
	gShaderPath = new TCHAR[nLen];
	_tcscpy_s(gShaderPath,nLen,pShaderPath);
	g_pObjectFun = pObject;
	g_pObjectParam = pObjParam;
	g_pChannelFun = pChannel;
	g_pChannelParam = pChannelParam;

	TCHAR MainFilePath[512];
	_tcscpy_s(MainFilePath,gShaderPath);
	_tcscat_s(MainFilePath,strMainFile);
	HANDLE hFile = CreateFile(MainFilePath,GENERIC_READ ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	g_nMainSize = GetFileSize(hFile,NULL) + 1;
	g_strShaderMain = new char[g_nMainSize];
	g_strShaderMain[g_nMainSize - 1] = 0;
	DWORD nCount;
	ReadFile(hFile,g_strShaderMain,g_nMainSize,&nCount,NULL);
	CloseHandle(hFile);

	_tcscpy_s(MainFilePath,gShaderPath);
	_tcscat_s(MainFilePath,strMainHeadFile);
	hFile = CreateFile(MainFilePath,GENERIC_READ ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	g_nMainHeadSize = GetFileSize(hFile,NULL) + 1;
	g_strShaderMainHead = new char[g_nMainHeadSize ];
	g_strShaderMainHead[g_nMainHeadSize  - 1] = 0;
	ReadFile(hFile,g_strShaderMainHead,g_nMainHeadSize ,&nCount,NULL);
	CloseHandle(hFile);
}

BASEGRAPH_DATARENDER_API void SYMaterialModuleRelease()
{
	SAFEDELETE_ARRAY(gShaderPath);
	SAFEDELETE_ARRAY(g_strShaderMain);
	SAFEDELETE_ARRAY(g_strShaderMainHead);
}
void BASEGRAPH_DATARENDER_API SYMaterial_RemoveShader(SYMaterial* pMaterial,int nShaderType)
{
	SYJoinShader* pShader = pMaterial->m_ShaderArray.ppShader[nShaderType];
	if (pShader != NULL)
	{
		SAFEDELETE_ARRAY(pShader->strBody);
		SAFEDELETE_ARRAY(pShader->strHead);
	}
	SAFE_DELETE(pMaterial->m_ShaderArray.ppShader[nShaderType]);
}
void BASEGRAPH_DATARENDER_API SYMaterial_AddShader(SYJoinShader* pShader ,int nShaderType)
{
	static const TCHAR* strLightHead = _T("\\PS\\materialPS\\lighthead.txt");
	static const TCHAR* strLigthBody = _T("\\PS\\materialPS\\lightbody.txt");
	static const TCHAR* strDiffuseHead = _T("\\PS\\materialPS\\diffusehead.txt");
	static const TCHAR* strDiffuseBody = _T("\\PS\\materialPS\\diffusebody.txt");
	static const TCHAR* strSpecHead = _T("\\PS\\materialPS\\spechead.txt");
	static const TCHAR* strSpecBody = _T("\\PS\\materialPS\\specbody.txt");
	static const TCHAR* strEnvMapHead = _T("\\PS\\materialPS\\envmaphead.txt");
	static const TCHAR* strEnvMapBody = _T("\\PS\\materialPS\\envmapbody.txt");
	static const TCHAR* strEmissiveHead = _T("\\PS\\materialPS\\emissivehead.txt");
	static const TCHAR* strEmissiveBody = _T("\\PS\\materialPS\\emissivebody.txt");
	static const TCHAR* strBumpHead = _T("\\PS\\materialPS\\bumphead.txt");
	static const TCHAR* strBumpBody = _T("\\PS\\materialPS\\bumpbody.txt");

	TCHAR HeadFilePath[512],BodyFilePath[512];
	switch (nShaderType)
	{
		case SYMaterial_DiffuseShader:
			_tcscpy_s(HeadFilePath,gShaderPath);
			_tcscat_s(HeadFilePath,strDiffuseHead);

			_tcscpy_s(BodyFilePath,gShaderPath);
			_tcscat_s(BodyFilePath,strDiffuseBody);
			break;
		case SYMaterial_SpecShader:
			_tcscpy_s(HeadFilePath,gShaderPath);
			_tcscat_s(HeadFilePath,strSpecHead);

			_tcscpy_s(BodyFilePath,gShaderPath);
			_tcscat_s(BodyFilePath,strSpecBody);
			break;
		case SYMaterial_EnvMapShader:
			_tcscpy_s(HeadFilePath,gShaderPath);
			_tcscat_s(HeadFilePath,strEnvMapHead);

			_tcscpy_s(BodyFilePath,gShaderPath);
			_tcscat_s(BodyFilePath,strEnvMapBody);
			break;
		case SYMaterial_EmissiveShader:
			_tcscpy_s(HeadFilePath,gShaderPath);
			_tcscat_s(HeadFilePath,strEmissiveHead);

			_tcscpy_s(BodyFilePath,gShaderPath);
			_tcscat_s(BodyFilePath,strEmissiveBody);
			break;

		case SYMaterial_BumpShader:
			_tcscpy_s(HeadFilePath,gShaderPath);
			_tcscat_s(HeadFilePath,strBumpHead);

			_tcscpy_s(BodyFilePath,gShaderPath);
			_tcscat_s(BodyFilePath,strBumpBody);
			break;

		case SYMaterial_LightShader:
			_tcscpy_s(HeadFilePath,gShaderPath);
			_tcscat_s(HeadFilePath,strLightHead);

			_tcscpy_s(BodyFilePath,gShaderPath);
			_tcscat_s(BodyFilePath,strLigthBody);
			break;
		default:
			return;
	}
	HANDLE hFile = CreateFile(HeadFilePath,GENERIC_READ ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	assert(hFile != INVALID_HANDLE_VALUE);
	int nSize = GetFileSize(hFile,NULL) + 1;
	pShader->strHead = new char[nSize];
	pShader->strHead[nSize - 1] = 0;
	DWORD nCount;
	ReadFile(hFile,pShader->strHead,nSize,&nCount,NULL);
	CloseHandle(hFile);

	hFile = CreateFile(BodyFilePath,GENERIC_READ ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	assert(hFile != INVALID_HANDLE_VALUE);
	nSize = GetFileSize(hFile,NULL) + 1;
	pShader->strBody = new char[nSize];
	pShader->strBody[nSize - 1] = 0;
	ReadFile(hFile,pShader->strBody,nSize,&nCount,NULL);
	CloseHandle(hFile);
	pShader->nType = nShaderType;

}
void BASEGRAPH_DATARENDER_API SYMaterial_Init( SYMaterial* pMaterial,DWORD InitColor,BOOL bLight)
{
	//static const TCHAR* pPicFile = _T("\\test.bmp");
	pMaterial->m_nVersion = 0;
	pMaterial->nMaterialType = 0;
	pMaterial->m_PhyData.Diffuse = InitColor;
	pMaterial->m_PhyData.Ambient = 0xff101010;
	pMaterial->m_PhyData.Specular = 0xffffffff;
	//pMaterial->m_PhyData.Diffuse = InitColor;
	//pMaterial->m_PhyData.Specular = InitColor;
	//pMaterial->m_PhyData.Ambient = 0xffffe0e0;//white
	pMaterial->m_PhyData.Emissive = 0xff101010;//0xff101010; 物体本身颜色
	pMaterial->m_PhyData.Power = 200;
	pMaterial->m_PhyData.fAlpha = 1;
	pMaterial->m_PhyData.disable = FALSE;

	memset(&pMaterial->m_ShaderArray,0,sizeof(SYShaderArray));
	memset(&pMaterial->m_TexArray,0,sizeof(SYMatTexArray));
	//添加Shader
	if (bLight)
	{
		SYJoinShader* pShder = new SYJoinShader;
		SYMaterial_AddShader(pShder,SYMaterial_LightShader);
		ASSERT(pMaterial->m_ShaderArray.ppShader[SYMaterial_LightShader]==NULL);
		pMaterial->m_ShaderArray.ppShader[SYMaterial_LightShader] = pShder;
	}
}

void ReleaseTexPrivateData(void*& pData,int nType)
{
	if (nType == SYMaterial_SpecTex)
	{
		SYSpecTexPrivateData* pPrivate = (SYSpecTexPrivateData* )pData;
		SAFE_DELETE(pPrivate);
	}
	else if (nType == SYMaterial_EnvMapTex)
	{
		SYEnvMapTexPrivateData* pPrivate = (SYEnvMapTexPrivateData* )pData;
		SAFE_DELETE(pPrivate);
	}
	else if(nType == SYMaterial_EmissiveTex)
	{
		SYEmissiveTexPrivateData* pPrivate = (SYEmissiveTexPrivateData* )pData;
		SAFE_DELETE(pPrivate);
	}
	else if(nType == SYMaterial_BumpTex)
	{
		SYBumpTexPrivateData* pPrivate = (SYBumpTexPrivateData* )pData;
		SAFE_DELETE(pPrivate);
	}
	pData = NULL;
}

void CreateTexPrivateData(void** ppData,int nType,void* pSrcData = NULL)
{
	if (nType == SYMaterial_SpecTex)
	{
		if(!*ppData)
		{
			*ppData = new SYSpecTexPrivateData;
		}
		if(!pSrcData)
		{
			((SYSpecTexPrivateData*)*ppData)->fFactor = 1;
		}
		else
			memcpy(*ppData,pSrcData,sizeof(SYSpecTexPrivateData));
	}
	else if (nType == SYMaterial_EnvMapTex)
	{
		if(!*ppData)
		{
			*ppData = new SYEnvMapTexPrivateData;
		}
		if(!pSrcData)
		{
			((SYEnvMapTexPrivateData*)*ppData)->fFactor = 1;
		}
		else
			memcpy(*ppData,pSrcData,sizeof(SYEnvMapTexPrivateData));
	}
	else if(nType == SYMaterial_EmissiveTex)
	{
		if(!*ppData)
		{
			*ppData = new SYEmissiveTexPrivateData;
		}
		if(!pSrcData)
		{
			((SYEmissiveTexPrivateData*)*ppData)->fFactor = 1;
		}
		else
			memcpy(*ppData,pSrcData,sizeof(SYEmissiveTexPrivateData));
	}
	else if(nType == SYMaterial_BumpTex)
	{
		if(!*ppData)
		{
			*ppData = new SYBumpTexPrivateData;
		}
		if(!pSrcData)
		{	
			((SYBumpTexPrivateData*)*ppData)->fFactor = 1;
			((SYBumpTexPrivateData*)*ppData)->bReverse = 1;
		}
		else
			memcpy(*ppData,pSrcData,sizeof(SYBumpTexPrivateData));
	}
}
void SYMaterial_ReleaseTexSrc(SYMatTex* pTex)
{
	switch(pTex->nSrcType)
	{
	case SYMaterial_PicFile:
	case SYMaterial_MovFile:
		SAFEDELETE_ARRAY(pTex->pTexPath);
		break;
	case SYMaterial_Object:
		SAFEDELETE_ARRAY(pTex->pTexSrc);
		break;
	case SYMaterial_Buffer:
		if(pTex->pBufferDef)
		{
			SAFEDELETE_ARRAY(pTex->pBufferDef->m_dwBuffer);
			SAFE_DELETE(pTex->pBufferDef);
		}
		break;
	}
}
void SYMaterial_CreateTexSrc(SYMatTex* pTex,int nSrcType,void* pData)
{
	pTex->nSrcType = nSrcType;
	size_t nLen;
	switch(pTex->nSrcType)
	{
	case SYMaterial_PicFile:
	case SYMaterial_MovFile:
		{
			TCHAR* pPath = (TCHAR*)pData;
			nLen = _tcsclen(pPath) + 1;
			pTex->pTexPath = new TCHAR[nLen];
			_tcscpy_s(pTex->pTexPath,nLen,pPath);
		}
		break;
	case SYMaterial_Object:
		{
			TCHAR* pObject = (TCHAR*)pData;
			nLen = _tcsclen(pObject) + 1;
			pTex->pTexSrc = new TCHAR[nLen];
			_tcscpy_s(pTex->pTexSrc,nLen,pObject);
		}
		break;
	case SYMaterial_Buffer:
		{
			SYBufferDef32* pBuffer = (SYBufferDef32*)pData;
			if(pBuffer && pBuffer->m_dwBuffer)
			{
				pTex->pBufferDef = new SYBufferDef32;
				memcpy(pTex->pBufferDef,pBuffer,sizeof(SYBufferDef32));
				pTex->pBufferDef->m_dwBuffer = new DWORD[pBuffer->m_nHeight * pBuffer->m_nWidth];
				memcpy(pTex->pBufferDef->m_dwBuffer,pBuffer->m_dwBuffer,pBuffer->m_nWidth * pBuffer->m_nHeight * sizeof(DWORD));
			}
		}
		break;
	}
}

void SYMaterial_ReleaseTex(SYMatTex* pTex)
{
	SYMaterial_ReleaseTexSrc(pTex);
	ReleaseTexPrivateData(pTex->pData,pTex->nType);
}
void BASEGRAPH_DATARENDER_API SYMaterial_Release( SYMaterial* pMaterial )
{
	if (pMaterial == NULL)
		return;
	for (int i = 0; i < SYMaterial_TexTypeCount;i++)
	{
		SYMatTex* pTex = pMaterial->m_TexArray.ppTex[i];
		if(pTex)
		{
			SYMaterial_ReleaseTex(pTex); 
		}
		SAFE_DELETE(pMaterial->m_TexArray.ppTex[i]);
	}
	for (int i = 0; i < SYMaterial_ShaderTypeCount;i++)
	{
		SYJoinShader* pShader = pMaterial->m_ShaderArray.ppShader[i];
		if(pShader)
		{
			SAFEDELETE_ARRAY(pShader->strHead);
			SAFEDELETE_ARRAY(pShader->strBody);

			SAFE_DELETE(pMaterial->m_ShaderArray.ppShader[i]);
		}
	}
}

void SYMaterial_CopyTex(SYMatTex* pDesTex,SYMatTex* pSrcTex)
{
	ReleaseTexPrivateData(pDesTex->pData,pDesTex->nType);
	SYMaterial_ReleaseTexSrc(pDesTex);

	pDesTex->TexOffset = pSrcTex->TexOffset;
	pDesTex->TexScale = pSrcTex->TexScale;
	pDesTex->Center = pSrcTex->Center;
	pDesTex->TexRotate = pSrcTex->TexRotate;
	pDesTex->bAutoRun = pSrcTex->bAutoRun;
	pDesTex->dSpeed= pSrcTex->dSpeed;
	pDesTex->vRunDir= pSrcTex->vRunDir;
	pDesTex->bRepeat= pSrcTex->bRepeat;

	pDesTex->nType = pSrcTex->nType;
	pDesTex->disable = pSrcTex->disable;
	switch(pSrcTex->nSrcType)
	{
	case SYMaterial_PicFile:
	case SYMaterial_MovFile:
		{
			SYMaterial_CreateTexSrc(pDesTex,pSrcTex->nSrcType,pSrcTex->pTexPath);
		}
		break;
	case SYMaterial_Object:
		{
			SYMaterial_CreateTexSrc(pDesTex,pSrcTex->nSrcType,pSrcTex->pTexSrc);
		}
		break;
	case SYMaterial_Buffer:
		{
			SYMaterial_CreateTexSrc(pDesTex,pSrcTex->nSrcType,pSrcTex->pBufferDef);
		}
		break;
	}

	CreateTexPrivateData(&pDesTex->pData,pDesTex->nType,pSrcTex->pData);
}

BASEGRAPH_DATARENDER_API int SYMaterial_Copy( OUT SYMaterial* pDesMaterial, IN const SYMaterial* pSrcMaterial )
{
	pDesMaterial->m_nVersion = pSrcMaterial->m_nVersion;
	int nChangeType = SYMaterial_NoChange;
	memcpy(&pDesMaterial->m_PhyData,&pSrcMaterial->m_PhyData,sizeof(SYPhyData));
	nChangeType |= SYMaterial_PhyDataChange;

	for (int i = 0; i < SYMaterial_TexTypeCount;i++)
	{
		SYMatTex* pDesTex = pDesMaterial->m_TexArray.ppTex[i];
		SYMatTex* pSrcTex = pSrcMaterial->m_TexArray.ppTex[i];
		if(pDesTex == NULL && pSrcTex == NULL)
			continue;
		else if(pDesTex != NULL && pSrcTex != NULL)
		{
			int nTexParamChange = SYMaterial_DiffuseParamChange << i;
			nChangeType |= nTexParamChange;
			int nTexChange = SYMaterial_DiffTexChange << i;
			nChangeType |= nTexParamChange;
			SYMaterial_CopyTex(pDesTex,pSrcTex);
		}
		else
		{
			nChangeType |= SYMaterial_TexCountChange;
			if (pDesTex)
			{
				SYMaterial_ReleaseTex(pDesTex);
				SAFE_DELETE(pDesTex);
				pDesMaterial->m_TexArray.ppTex[i] = NULL;
			}

			if(pSrcTex)
			{
				pDesTex = new SYMatTex;
				pDesMaterial->m_TexArray.ppTex[i] = pDesTex;
				memset(pDesTex,0,sizeof(SYMatTex));
				SYMaterial_CopyTex(pDesTex,pSrcTex);
			}
		}
	}

	for (int i = 0; i < SYMaterial_TexTypeCount + 1;i++)
	{
		SYJoinShader* pDesShader = pDesMaterial->m_ShaderArray.ppShader[i];
		SYJoinShader* pSrcShader = pSrcMaterial->m_ShaderArray.ppShader[i];
		if (pDesShader == NULL && pSrcShader == NULL)
			continue;
		else
		{
			nChangeType |= SYMaterial_ShaderChange;
   			if (pDesShader)
			{
				SAFEDELETE_ARRAY(pDesShader->strHead);
				SAFEDELETE_ARRAY(pDesShader->strBody);
				SAFE_DELETE(pDesShader);
				pDesMaterial->m_ShaderArray.ppShader[i] = NULL;
			}

			if(pSrcShader)
			{
				pDesShader = new SYJoinShader;
				memset(pDesShader,0,sizeof(SYJoinShader));
				pDesShader->nType = i;
				pDesMaterial->m_ShaderArray.ppShader[i] = pDesShader;
				size_t nLen = strlen(pSrcShader->strHead) + 1;
				pDesShader->strHead = new char[nLen];
				strcpy_s(pDesShader->strHead,nLen,pSrcShader->strHead);

				nLen = strlen(pSrcShader->strBody) + 1;
				pDesShader->strBody = new char[nLen];
				strcpy_s(pDesShader->strBody,nLen,pSrcShader->strBody);
			}
		}
	}
	return nChangeType;
}

BASEGRAPH_DATARENDER_API BOOL SYMaterial_Compare( const SYMaterial* pMaterial1, const SYMaterial* pMaterial2 )
{
	if (pMaterial1 == NULL && pMaterial2 == NULL)
		return TRUE;

	if (pMaterial1 == NULL || pMaterial2 == NULL)
		return FALSE;

	if (pMaterial1->nMaterialType != pMaterial2->nMaterialType)
	{
		return FALSE;
	}
	if (memcmp(&pMaterial1->m_PhyData, &pMaterial2->m_PhyData, sizeof(SYPhyData)) != 0)
	{
		return FALSE;
	}
	for (int i = 0; i < SYMaterial_TexTypeCount;i++)
	{
		SYMatTex* pMatTex1 = pMaterial1->m_TexArray.ppTex[i];
		SYMatTex* pMatTex2 = pMaterial2->m_TexArray.ppTex[i];
		if (!SYMatTex_Compare(pMatTex1, pMatTex2))
		{
			return FALSE;
		}
	}
	for (int i = 0; i < SYMaterial_ShaderTypeCount;i++)
	{
		SYJoinShader* pShader1 = pMaterial1->m_ShaderArray.ppShader[i];
		SYJoinShader* pShader2 = pMaterial2->m_ShaderArray.ppShader[i];
		if (!SYJoinShader_Compare(pShader1, pShader2))
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

BASEGRAPH_DATARENDER_API BOOL	SYMatTex_Compare( const SYMatTex* pMatTex1, const SYMatTex* pMatTex2)
{
	if (pMatTex1 == NULL && pMatTex2 == NULL)
		return TRUE;

	if (pMatTex1 == NULL || pMatTex2 == NULL)
		return FALSE;

	if (pMatTex1->disable != pMatTex2->disable ||
		pMatTex1->nType != pMatTex2->nType ||
		pMatTex1->nSrcType != pMatTex2->nSrcType ||
		pMatTex1->TexOffset != pMatTex2->TexOffset ||
		pMatTex1->TexScale != pMatTex2->TexScale ||
		pMatTex1->Center != pMatTex2->Center ||
		pMatTex1->TexRotate != pMatTex2->TexRotate ||
		pMatTex1->bAutoRun != pMatTex2->bAutoRun ||
		pMatTex1->dSpeed!= pMatTex2->dSpeed ||
		pMatTex1->vRunDir!= pMatTex2->vRunDir ||
		pMatTex1->bRepeat!= pMatTex2->bRepeat)
	{
		return FALSE;
	}

	switch(pMatTex1->nSrcType)
	{
	case SYMaterial_PicFile:
	case SYMaterial_MovFile:
		{
			if (_tcscmp(pMatTex1->pTexPath, pMatTex2->pTexPath) != 0)
			{
				return FALSE;
			}
		}
		break;
	case SYMaterial_Object:
		{
			if (_tcscmp(pMatTex1->pTexSrc, pMatTex2->pTexSrc) != 0)
			{
				return FALSE;
			}
		}
		break;
	case SYMaterial_Buffer:
		{
			if (!SYBufferDef32_Compare(pMatTex1->pBufferDef, pMatTex2->pBufferDef))
			{
				return FALSE;
			}
		}
		break;
	}

	switch(pMatTex1->nSrcType)
	{
	case SYMaterial_SpecTex:
		{
			SYSpecTexPrivateData *pData1 = (SYSpecTexPrivateData*)pMatTex1->pData;
			SYSpecTexPrivateData *pData2 = (SYSpecTexPrivateData*)pMatTex2->pData;
			if (pData1->fFactor != pData2->fFactor)
			{
				return FALSE;
			}
		}
		break;
	case SYMaterial_EnvMapTex:
		{
			SYEnvMapTexPrivateData *pData1 = (SYEnvMapTexPrivateData*)pMatTex1->pData;
			SYEnvMapTexPrivateData *pData2 = (SYEnvMapTexPrivateData*)pMatTex2->pData;
			if (pData1->fFactor != pData2->fFactor)
			{
				return FALSE;
			}
		}
		break;
	case SYMaterial_EmissiveTex:
		{
			SYEmissiveTexPrivateData *pData1 = (SYEmissiveTexPrivateData*)pMatTex1->pData;
			SYEmissiveTexPrivateData *pData2 = (SYEmissiveTexPrivateData*)pMatTex2->pData;
			if (pData1->fFactor != pData2->fFactor)
			{
				return FALSE;
			}
		}
		break;
	case SYMaterial_BumpTex:
		{
			SYBumpTexPrivateData *pData1 = (SYBumpTexPrivateData*)pMatTex1->pData;
			SYBumpTexPrivateData *pData2 = (SYBumpTexPrivateData*)pMatTex2->pData;
			if (pData1->fFactor != pData2->fFactor ||
				pData1->bReverse != pData2->bReverse)
			{
				return FALSE;
			}

		}
		break;
	}
	return TRUE;
}

BASEGRAPH_DATARENDER_API BOOL	SYJoinShader_Compare( const SYJoinShader* pShader1, const SYJoinShader* pShader2)
{
	if (pShader1 == NULL && pShader2 == NULL)
		return TRUE;

	if (pShader1 == NULL || pShader2 == NULL)
		return FALSE;

	if (pShader1->nType != pShader2->nType ||
		strcmp(pShader1->strHead, pShader2->strHead) != 0 ||
		strcmp(pShader1->strBody, pShader2->strBody) != 0)
	{
		return FALSE;
	}

	return TRUE;
}

//void SYMaterial_AddTexPrivateKeyValue(double dFrame,param_def* param,SYMatTex* pTex,void* pValue)
//{
//	switch(pTex->nType)
//	{
//	case SYMaterial_SpecTex:
//	case SYMaterial_EnvMapTex:
//	case SYMaterial_EmissiveTex:
//	case SYMaterial_BumpTex:
//		{
//			float* pData = (float*)pValue;
//			float fValue = *pData - *(float*)pTex->pData;
//			AddClipKeyFrame(param,dFrame,(BYTE*)&fValue);
//		}
//		break;
//	}
//}

//void SYMaterial_AddTexKeyValue(double dFrame,SYArray* pArTexParam,SYMatTex* pTex,void* pValue,int nType)
//{
//	param_def* param = (param_def*)SYAR_GetAt(pArTexParam,nType);
//	switch(nType)
//	{
//	case SYTex_Offset_ParamDef:
//		{
//			float2* pData = (float2*)pValue;
//			float2 offset = *pData - pTex->TexOffset;
//			AddClipKeyFrame(param,dFrame,(BYTE*)&offset);
//		}
//		break;
//	case SYTex_Scale_ParamDef:
//		{
//			float2* pData = (float2*)pValue;
//			float2 offset = *pData;
//			offset.x /= pTex->TexScale.x;
//			offset.y /= pTex->TexScale.y;
//			AddClipKeyFrame(param,dFrame,(BYTE*)&offset);
//		}
//		break;
//	case SYTex_Center_ParamDef:
//		{
//			float2* pData = (float2*)pValue;
//			float2 offset = *pData - pTex->Center;
//			AddClipKeyFrame(param,dFrame,(BYTE*)&offset);
//		}
//		break;
//	case SYTex_Rotate_ParamDef:
//		{
//			float* pRotate = (float*)pValue;
//			float rotate =  *pRotate - pTex->TexRotate;
//			AddClipKeyFrame(param,dFrame,(BYTE*)&rotate);
//		}
//		break;
//	case SYTex_Private_ParamDef:
//		SYMaterial_AddTexPrivateKeyValue(dFrame,param,pTex,pValue);
//		break;
//	}
//}

//void SYMaterial_AddPrivateParamdef(SYArray* pArTexParam ,int nTexType)
//{
//	if(nTexType == SYMaterial_SpecTex)
//	{
//		param_def* pparamdefFactor = new param_def;
//		sy_init_paramdef(pparamdefFactor,sizeof(float),NULL);
//		pparamdefFactor->interpolate.nRange             = 1;
//		pparamdefFactor->interpolate.pRanges            = new PAIR[1];
//		pparamdefFactor->interpolate.pRanges[0].fFirst  = 0.f;
//		pparamdefFactor->interpolate.pRanges[0].fSecond = 1.f;
//		SYAR_Add( pArTexParam, pparamdefFactor );
//	}
//	else if(nTexType == SYMaterial_EnvMapTex)
//	{
//		param_def* pparamdefFactor = new param_def;
//		sy_init_paramdef(pparamdefFactor,sizeof(float),NULL);
//		pparamdefFactor->interpolate.nRange             = 1;
//		pparamdefFactor->interpolate.pRanges            = new PAIR[1];
//		pparamdefFactor->interpolate.pRanges[0].fFirst  = 0.f;
//		pparamdefFactor->interpolate.pRanges[0].fSecond = 1.f;
//		SYAR_Add( pArTexParam, pparamdefFactor );
//	}
//	else if(nTexType == SYMaterial_EmissiveTex)
//	{
//		param_def* pparamdefFactor = new param_def;
//		sy_init_paramdef(pparamdefFactor,sizeof(float),NULL);
//		pparamdefFactor->interpolate.nRange             = 1;
//		pparamdefFactor->interpolate.pRanges            = new PAIR[1];
//		pparamdefFactor->interpolate.pRanges[0].fFirst  = 0.f;
//		pparamdefFactor->interpolate.pRanges[0].fSecond = 1.f;
//		SYAR_Add( pArTexParam, pparamdefFactor );
//	}
//	else if(nTexType == SYMaterial_BumpTex)
//	{
//		param_def* pparamdefFactor = new param_def;
//		sy_init_paramdef(pparamdefFactor,sizeof(float),NULL);
//		pparamdefFactor->interpolate.nRange             = 1;
//		pparamdefFactor->interpolate.pRanges            = new PAIR[1];
//		pparamdefFactor->interpolate.pRanges[0].fFirst  = 0.f;
//		pparamdefFactor->interpolate.pRanges[0].fSecond = 1.f;
//		SYAR_Add( pArTexParam, pparamdefFactor );
//	}
//}
//void SYMaterial_AddParamdef(SYArray* pArTexParam ,int nTexType)
//{
//	static const float2 vec = {0,0},vec1 = {1,1};
//	param_def* pparamdef_offset = new param_def;
//	sy_init_paramdef(pparamdef_offset,sizeof(float2),NULL);
//	pparamdef_offset->interpolate.nRange             = 2;
//	pparamdef_offset->interpolate.pRanges            = new PAIR[2];
//	pparamdef_offset->interpolate.pRanges[0].fFirst  = 0.f;
//	pparamdef_offset->interpolate.pRanges[0].fSecond = 1.f;
//	pparamdef_offset->interpolate.pRanges[1].fFirst  = 0.f;
//	pparamdef_offset->interpolate.pRanges[1].fSecond = 1.f;
//	SYAR_Add( pArTexParam, pparamdef_offset );
//
//	param_def* pparamdef_scale = new param_def;
//	sy_init_paramdef(pparamdef_scale,sizeof(float2),(void*)&vec1);
//	pparamdef_scale->interpolate.nRange             = 2;
//	pparamdef_scale->interpolate.pRanges            = new PAIR[2];
//	pparamdef_scale->interpolate.pRanges[0].fFirst  = 0.f;
//	pparamdef_scale->interpolate.pRanges[0].fSecond = 1.f;
//	pparamdef_scale->interpolate.pRanges[1].fFirst  = 0.f;
//	pparamdef_scale->interpolate.pRanges[1].fSecond = 1.f;
//	SYAR_Add( pArTexParam, pparamdef_scale );
//
//	param_def* pparamdef_center = new param_def;
//	sy_init_paramdef(pparamdef_center,sizeof(float2),(void*)&vec);
//	pparamdef_center->interpolate.nRange             = 2;
//	pparamdef_center->interpolate.pRanges            = new PAIR[2];
//	pparamdef_center->interpolate.pRanges[0].fFirst  = 0.f;
//	pparamdef_center->interpolate.pRanges[0].fSecond = 1.f;
//	pparamdef_center->interpolate.pRanges[1].fFirst  = 0.f;
//	pparamdef_center->interpolate.pRanges[1].fSecond = 1.f;
//	SYAR_Add( pArTexParam, pparamdef_center );
//
//	param_def* pparamdefrotate = new param_def;
//	sy_init_paramdef(pparamdefrotate,sizeof(float),NULL);
//	pparamdefrotate->interpolate.nRange             = 1;
//	pparamdefrotate->interpolate.pRanges            = new PAIR[1];
//	pparamdefrotate->interpolate.pRanges[0].fFirst  = 0.f;
//	pparamdefrotate->interpolate.pRanges[0].fSecond = 1.f;
//	SYAR_Add( pArTexParam, pparamdefrotate );
//
//	SYMaterial_AddPrivateParamdef(pArTexParam ,nTexType);
//}

BASEGRAPH_DATARENDER_API int SYMaterial_AddTex(SYMaterial* pMaterial,int nTexType)
{
	if (pMaterial->m_TexArray.ppTex[nTexType])
	{
		//添加Shader
		if (pMaterial->m_ShaderArray.ppShader[nTexType] == NULL)
		{
			SYJoinShader* pShder = new SYJoinShader;
			SYMaterial_AddShader(pShder,nTexType);
			pMaterial->m_ShaderArray.ppShader[nTexType] = pShder;
			return SYMaterial_TexCountChange;
		}
		return SYMaterial_NoChange;
	}
	SYMatTex* pTex = new SYMatTex;
	memset(pTex,0,sizeof(SYMatTex));
	pTex->nType = nTexType;
	pTex->bRepeat = TRUE;
	pTex->TexScale.x = pTex->TexScale.y = 1.f;
	CreateTexPrivateData(&pTex->pData,nTexType);
	pMaterial->m_TexArray.ppTex[nTexType] = pTex;

	//添加Shader
	SYJoinShader* pShder = new SYJoinShader;
	SYMaterial_AddShader(pShder,nTexType);
	pMaterial->m_ShaderArray.ppShader[nTexType] = pShder;

	return SYMaterial_TexCountChange;
}

BASEGRAPH_DATARENDER_API int SYMaterial_RemoveTex( SYMaterial* pMaterial,int nTexType )
{
	if (!pMaterial->m_TexArray.ppTex[nTexType])
		return SYMaterial_NoChange;
	SYMaterial_ReleaseTex(pMaterial->m_TexArray.ppTex[nTexType]);
	SAFE_DELETE(pMaterial->m_TexArray.ppTex[nTexType]);

	//删除Shader
	SYMaterial_RemoveShader(pMaterial,nTexType);
	return SYMaterial_TexCountChange;
}

BASEGRAPH_DATARENDER_API void* SYMaterial_GetTexSrc( SYMaterial* pMaterial,int nTexType )
{
	if (!pMaterial->m_TexArray.ppTex[nTexType])
		return NULL;
	return pMaterial->m_TexArray.ppTex[nTexType]->pTexPath;
}

BASEGRAPH_DATARENDER_API int SYMaterial_GetTexSrcType( SYMaterial* pMaterial,int nIndex )
{
	if (!pMaterial->m_TexArray.ppTex[nIndex])
		return SYMaterial_Invid;
	return pMaterial->m_TexArray.ppTex[nIndex]->nSrcType;

}

BASEGRAPH_DATARENDER_API int SYMaterial_SetTexSrc( SYMaterial* pMat,int nTexIndex,int nSrcType,void* pData )
{
	SYMatTex* pTex = pMat->m_TexArray.ppTex[nTexIndex];
	if (!pTex)
		return SYMaterial_NoChange;
	SYMaterial_ReleaseTexSrc(pTex);
	SYMaterial_CreateTexSrc(pTex,nSrcType,pData);
	return SYMaterial_DiffTexChange << nTexIndex;
}

BASEGRAPH_DATARENDER_API int SYMaterial_SetTexOffset( SYMaterial* pMat,int nTexIndex,float2* pOffset )
{
	SYMatTex* pTex = pMat->m_TexArray.ppTex[nTexIndex];
	if (!pTex)
		return SYMaterial_NoChange;
	pTex->TexOffset = *pOffset;
	return SYMaterial_DiffuseParamChange << nTexIndex;
}

BASEGRAPH_DATARENDER_API int SYMaterial_SetTexScale( SYMaterial* pMat,int nTexIndex,float2* pScale )
{
	SYMatTex* pTex = pMat->m_TexArray.ppTex[nTexIndex];
	if (!pTex)
		return SYMaterial_NoChange;
	pTex->TexScale = *pScale;
	return SYMaterial_DiffuseParamChange << nTexIndex;

}

BASEGRAPH_DATARENDER_API int SYMaterial_SetTexCenter( SYMaterial* pMat,int nTexIndex,float2* pCenter )
{
	SYMatTex* pTex = pMat->m_TexArray.ppTex[nTexIndex];
	if (!pTex)
		return SYMaterial_NoChange;
	pTex->Center = *pCenter;
	return SYMaterial_DiffuseParamChange << nTexIndex;
}

BASEGRAPH_DATARENDER_API int SYMaterial_SetTexRotate( SYMaterial* pMat,int nTexIndex,float fRotate )
{
	SYMatTex* pTex = pMat->m_TexArray.ppTex[nTexIndex];
	if (!pTex)
		return SYMaterial_NoChange;
	pTex->TexRotate = fRotate;
	return SYMaterial_DiffuseParamChange << nTexIndex;

}

BASEGRAPH_DATARENDER_API int SYMaterial_SetTexPrivateData( SYMaterial* pMat,int nTexIndex,void* pData )
{
	SYMatTex* pTex = pMat->m_TexArray.ppTex[nTexIndex];
	if (!pTex)
		return SYMaterial_NoChange;
	switch(pTex->nType)
	{
	case SYMaterial_SpecTex:
	case SYMaterial_EnvMapTex:
	case SYMaterial_EmissiveTex:
		{
			SYSpecTexPrivateData* pValue = (SYSpecTexPrivateData*)pData;
			*(SYSpecTexPrivateData*)pTex->pData = *pValue;
		}
		break;
	case SYMaterial_BumpTex:
		{
			SYBumpTexPrivateData* pValue = (SYBumpTexPrivateData*)pData;
			*(SYBumpTexPrivateData*)pTex->pData = *pValue;
		}
		break;
	}
	return SYMaterial_DiffuseParamChange << nTexIndex;
}

BASEGRAPH_DATARENDER_API int SYMaterial_SetPhyColor( SYMaterial* pMat,DWORD color )
{
	pMat->m_PhyData.Diffuse = color;
	pMat->m_PhyData.Ambient = color;
	pMat->m_PhyData.Specular = 0xffffffff;
	return SYMaterial_PhyDataChange;
}

BASEGRAPH_DATARENDER_API int		SYMaterial_SetPhyParam(SYMaterial* pMat,const SYPhyRenderData* pData)
{
	Float4ToDWORDColor(&pMat->m_PhyData.Diffuse,pData->Diffuse);
	Float4ToDWORDColor(&pMat->m_PhyData.Ambient,pData->Ambient);
	Float4ToDWORDColor(&pMat->m_PhyData.Specular,pData->Specular);
	Float4ToDWORDColor(&pMat->m_PhyData.Emissive,pData->Emissive);
	return SYMaterial_PhyDataChange;
}
BASEGRAPH_DATARENDER_API int SYMaterial_SetPhyParam( SYMaterial* pMat,const SYPhyData* pData )
{
	memcpy(&pMat->m_PhyData,pData,sizeof(SYPhyData));
	return SYMaterial_PhyDataChange;
}

BASEGRAPH_DATARENDER_API void SYMaterial_SetChangeType(SYVideoMaterial* pVideoMat,int nChangeType)
{
	if(pVideoMat)
		pVideoMat->m_nChangeType |= nChangeType;
}
BASEGRAPH_DATARENDER_API void SYMaterial_ClearAllChangeType(SYVideoMaterial* pVideoMat,int nChangeType = 0)
{
	if(pVideoMat)
		pVideoMat->m_nChangeType = nChangeType;
}
BASEGRAPH_DATARENDER_API void SYMaterial_ClearChangeType(SYVideoMaterial* pVideoMat,int nChangeType)
{
	if(pVideoMat)
		pVideoMat->m_nChangeType &= ~nChangeType;
}

BASEGRAPH_DATARENDER_API void SYMaterial_CreateVideoMat( SYVideoMaterial* pVideoMat,SYMaterial* pMaterial ,int hDevice,BOOL bEdit)
{
	memset(pVideoMat,0,sizeof(SYVideoMaterial));
	pVideoMat->m_hDevice = hDevice;
	//SYMaterial_PhyToPhyRenderData(&pVideoMat->m_PhyData,&pMaterial->m_PhyData);
	pVideoMat->m_bEdit = bEdit;
	pVideoMat->m_hPhyCB = NULL;//DX3DEngine_CreateBuffer(pVideoMat->m_hDevice,sizeof(float4),5,NULL,DXConstBuffer);
	pVideoMat->m_dTime = 0;
	pVideoMat->m_dTotalTime = 0;
	pVideoMat->m_pShaderStr = NULL;
	pVideoMat->m_pMaterial = pMaterial;
	SYMaterial_ClearAllChangeType(pVideoMat,SYMaterial_ALLChange);

	memset(&pVideoMat->m_VideoTexAr,0,sizeof(SYMatVideoTexArray));
}

void SYMaterial_SetStaticPhyParam(SYVideoMaterial* pMat,SYPhyRenderData* pData,int nType)
{
	if(nType & SYMATERIAL_DIFFUSE_COLOR)
		pMat->m_PhyDataStatic.Diffuse = pData->Diffuse;
	if(nType & SYMATERIAL_SPECTURE_COLOR)
		pMat->m_PhyDataStatic.Specular = pData->Specular;
	if(nType & SYMATERIAL_AMBEINT_COLOR	)	
		pMat->m_PhyDataStatic.Ambient = pData->Ambient;
	if(nType & SYMATERIAL_EMISSIVE_COLOR)		
		pMat->m_PhyDataStatic.Emissive = pData->Emissive;
	if(nType & SYMATERIAL_FACTOR)	
		pMat->m_PhyDataStatic.Power = pData->Power;
	if(nType & SYMATERIAL_ALPHA	)
		pMat->m_PhyDataStatic.fAlpha = pData->fAlpha;
	if(nType & SYMATERIAL_DISABLE	)
		pMat->m_PhyDataStatic.disable = pData->disable;
	SYMaterial_SetChangeType(pMat, SYMaterial_PhyDataChange);
}
void SYMaterial_ReleaseVideoTex(int hDevice,SYMatVideoTex* pTex,BOOL bReleaseParam = TRUE)
{
	//if (pTex)
	//{
	//	if(bReleaseParam)
	//	{
	//		DX3DEngine_ReleaseBuffer(hDevice,pTex->hTexCB);
	//		pTex->hTexCB = NULL;
	//	}
	//	if(pTex->bTexNeedRelease)
	//		DX3DEngine_ReleaseTexture(hDevice,pTex->hTex);
	//	pTex->hTex = NULL;

	//	if(pTex->pResource)
	//	{
	//		pTex->pResource->pVideoSource->Stop();
	//		SAFE_DELETE(pTex->pResource->pVideoSource);
	//		DX3DEngine_ReleaseTexture(hDevice,pTex->pResource->hTex[0]);
	//		DX3DEngine_ReleaseTexture(hDevice,pTex->pResource->hTex[1]);
	//		SAFE_DELETE(pTex->pResource);
	//	}
	//	if(pTex->pSeekData)
	//	{
	//		SYMovieClose(pTex->pSeekData->pVideoSeekData);
	//		pTex->pSeekData->pVideoSeekData = NULL;
	//		SAFE_DELETE(pTex->pSeekData);
	//	}
	//	ReleaseTexPrivateData(pTex->pCurData,pTex->nType);
	//	pTex->pCurData = NULL;
	//}
}

BASEGRAPH_DATARENDER_API void SYMaterial_ReleaseVideoMat( SYVideoMaterial* pVideoMat )
{
	if(!pVideoMat)
		return;
	DX3DEngine_ReleaseBuffer(pVideoMat->m_hDevice,pVideoMat->m_hPhyCB);
	SAFEDELETE_ARRAY(pVideoMat->m_pShaderStr);
	delete[] pVideoMat->m_pShaderName;
	pVideoMat->m_pMaterial = NULL;
	
	for (int i = 0; i < SYMaterial_TexTypeCount;i++)
	{
		SYMatVideoTex* pTex = pVideoMat->m_VideoTexAr.ppVTex[i];
		SYMaterial_ReleaseVideoTex(pVideoMat->m_hDevice,pTex);
		SAFE_DELETE(pVideoMat->m_VideoTexAr.ppVTex[i]);
	}
}

const char* g_NOlightProcess = "fDiffContrib = DiffuseRef; //fSpecContrib = SpeculRef; fEmissiveContrib = Emissive;\n";
const int g_NoLightLen = 86;

int SYMaterial_GetSubShaderSize(SYMaterial* pMat)
{
	size_t nSize = 0;

	for (int i = 0; i <SYMaterial_ShaderTypeCount;i++)
	{
		if (!pMat->m_ShaderArray.ppShader[i])
			continue;
		nSize += strlen(pMat->m_ShaderArray.ppShader[i]->strHead);
		nSize += strlen(pMat->m_ShaderArray.ppShader[i]->strBody);
	}
	nSize += g_NoLightLen;
	return (int)nSize;
}

const TCHAR* gSubShaderName[SYMaterial_ShaderTypeCount] =
{
	_T("diffuse_"),		//材质
	_T("spec_"),		//高光
	_T("envmap_"),		//环境
	_T("emissive_"),	//光影
	_T("bump_"),		//凹凸
	_T("light_"),
};
void SYMaterial_JoinShader(SYVideoMaterial* pVideoMat,int nLightNum)
{
	nLightNum = max(0,nLightNum);

	int nShaderSize = g_nMainHeadSize + g_nMainSize + SYMaterial_GetSubShaderSize(pVideoMat->m_pMaterial) + 1;
	SAFEDELETE_ARRAY(pVideoMat->m_pShaderStr);
	pVideoMat->m_pShaderStr = new char[nShaderSize];
	pVideoMat->m_pShaderStr[0] = 0;
	TCHAR strShaderName[60];
	strShaderName[0] = 0;
	//插入shader的头部分
	strcpy_s(pVideoMat->m_pShaderStr,nShaderSize,g_strShaderMainHead);
	for (int i = 0;i < SYMaterial_ShaderTypeCount;i++)
	{
		if(!pVideoMat->m_pMaterial->m_ShaderArray.ppShader[i])
			continue;
		if((nLightNum == 0) && ( i > SYMaterial_DiffuseShader ) )
			continue;
		strcat_s(pVideoMat->m_pShaderStr,nShaderSize,pVideoMat->m_pMaterial->m_ShaderArray.ppShader[i]->strHead);
		_tcscat_s(strShaderName,gSubShaderName[i]);
	}
	//找到插入点
	static const char* strBegin = "//end add";
	char* pBeginIns = strstr(g_strShaderMain,strBegin);
	size_t nNowLen = strlen(pVideoMat->m_pShaderStr);
	memcpy(pVideoMat->m_pShaderStr + nNowLen,g_strShaderMain,pBeginIns - g_strShaderMain);
	pVideoMat->m_pShaderStr[nNowLen + pBeginIns-g_strShaderMain] = 0;

	//插入shader的主体部分
	//插入凹凸纹理
	if(pVideoMat->m_pMaterial->m_ShaderArray.pBumpTexShader && nLightNum > 0)
		strcat_s(pVideoMat->m_pShaderStr,nShaderSize,pVideoMat->m_pMaterial->m_ShaderArray.pBumpTexShader->strBody);
	//插入光照函数
	if(pVideoMat->m_pMaterial->m_ShaderArray.pLightShader && nLightNum > 0)

		strcat_s(pVideoMat->m_pShaderStr,nShaderSize,pVideoMat->m_pMaterial->m_ShaderArray.pLightShader->strBody);
	else
		strcat_s(pVideoMat->m_pShaderStr,nShaderSize,g_NOlightProcess);

	if(pVideoMat->m_pMaterial->m_ShaderArray.pDiffuseTexShader)
		strcat_s(pVideoMat->m_pShaderStr,nShaderSize,pVideoMat->m_pMaterial->m_ShaderArray.pDiffuseTexShader->strBody);
	if(pVideoMat->m_pMaterial->m_ShaderArray.pSpecTexShader&& nLightNum > 0)
		strcat_s(pVideoMat->m_pShaderStr,nShaderSize,pVideoMat->m_pMaterial->m_ShaderArray.pSpecTexShader->strBody);
	if(pVideoMat->m_pMaterial->m_ShaderArray.pEnvMapTexShader&& nLightNum > 0)
		strcat_s(pVideoMat->m_pShaderStr,nShaderSize,pVideoMat->m_pMaterial->m_ShaderArray.pEnvMapTexShader->strBody);
	if(pVideoMat->m_pMaterial->m_ShaderArray.pEmissiveTexShader&& nLightNum > 0)
		strcat_s(pVideoMat->m_pShaderStr,nShaderSize,pVideoMat->m_pMaterial->m_ShaderArray.pEmissiveTexShader->strBody);
	strcat_s(pVideoMat->m_pShaderStr,nShaderSize,pBeginIns);

	//替换灯光个数
	char* pSubStr = NULL;
	while(pSubStr = strstr(pVideoMat->m_pShaderStr,"#1"))
	{
		pSubStr[0] = '0' + nLightNum / 10;
		pSubStr[1] = '0' + nLightNum % 10;
	}

	size_t nShaderName = _tcsclen(strShaderName) + 4;
	SAFEDELETE_ARRAY(pVideoMat->m_pShaderName);
	pVideoMat->m_pShaderName = new TCHAR[nShaderName];
	pVideoMat->m_pShaderName[0] = nLightNum;
	pVideoMat->m_pShaderName[1] = 0;
	_tcscat_s(pVideoMat->m_pShaderName,nShaderName,strShaderName);
	_tcscat_s(pVideoMat->m_pShaderName,nShaderName,_T("PS"));
	//printf(pVideoMat->m_pShaderStr);
}

BOOL IsNeedUpdateTexParamBuffer(float2* pNewOffset,float2* pNewScale,float2* pNewCenter,float* pNewRotate,SYMatVideoTex* pVideoTex)
{
	if(*pNewOffset != pVideoTex->CurTexOffset)
		return TRUE;
	if(*pNewScale != pVideoTex->CurTexScale)
		return TRUE;
	if(*pNewCenter != pVideoTex->CurCenter)
		return TRUE;
	if(*pNewRotate != pVideoTex->CurTexRotate)
		return TRUE;
	return FALSE;
}

BOOL IsNeedUpdatePhyParamBuffer(float fAlpha,const float4& diffuse,const float4& ambient,const float4& specular,const float4& emissive,SYVideoMaterial* pVideoMat)
{
	if(fAlpha != pVideoMat->m_fAlpha)
		return TRUE;
	if(diffuse != pVideoMat->m_PhyData.Diffuse)
		return TRUE;
	if(ambient != pVideoMat->m_PhyData.Ambient)
		return TRUE;
	if(specular != pVideoMat->m_PhyData.Specular)
		return TRUE;
	if(emissive != pVideoMat->m_PhyData.Emissive)
		return TRUE;
	return FALSE;
}

#define PHY_DIFFUSE		0
#define PHY_AMBIENT		1
#define PHY_SPECULAR	2
#define PHY_EMISSIVE	3
int UpdatePhyParam(double dTime,SYMaterialParam* pMatParam,SYVideoMaterial* pVideoMat,float fAlpha)
{
	//assert(pVideoMat);
	//SYMaterial* pMaterial = pVideoMat->m_pMaterial;
	//float4 phy[5] = {0};
	//float fMatAlpha;

	//phy[4].x = pMaterial->m_PhyData.Power +		pVideoMat->m_PhyDataStatic.Power;
	//fMatAlpha = pMaterial->m_PhyData.fAlpha +	pVideoMat->m_PhyDataStatic.fAlpha;

	//DWORDColorToFloat4(phy + PHY_DIFFUSE,pMaterial->m_PhyData.Diffuse);
	//DWORDColorToFloat4(phy + PHY_AMBIENT,pMaterial->m_PhyData.Ambient);
	//DWORDColorToFloat4(phy + PHY_SPECULAR,pMaterial->m_PhyData.Specular);
	//DWORDColorToFloat4(phy + PHY_EMISSIVE,pMaterial->m_PhyData.Emissive);

	//phy[PHY_DIFFUSE] += pVideoMat->m_PhyDataStatic.Diffuse;
	//phy[PHY_AMBIENT] += pVideoMat->m_PhyDataStatic.Ambient;
	//phy[PHY_SPECULAR] +=  pVideoMat->m_PhyDataStatic.Specular;
	//phy[PHY_EMISSIVE]+=  pVideoMat->m_PhyDataStatic.Emissive;

	//if(pMatParam)
	//{
	//	float4 value;
	//	if(calc_get_value(&pMatParam->common.Diffuse,dTime,&value,NULL))
	//		phy[PHY_DIFFUSE] += value;
	//	if(calc_get_value(&pMatParam->common.Ambient,dTime,&value,NULL))
	//		phy[PHY_AMBIENT] += value;
	//	if(calc_get_value(&pMatParam->common.Specular,dTime,&value,NULL))
	//		phy[PHY_SPECULAR] += value;
	//	if(calc_get_value(&pMatParam->common.Emissive,dTime,&value,NULL))
	//		phy[PHY_EMISSIVE]+=  value;
	//	float fCalAlpha = 0;
	//	if(calc_get_value(&pMatParam->common.fAlpha,dTime,&fCalAlpha,NULL))
	//		fMatAlpha += fCalAlpha;
	//}

	//for(int i = PHY_DIFFUSE; i <= PHY_EMISSIVE ;i++)
	//	float4Clamp(phy + i,0,1);

	//fAlpha *= fMatAlpha;
	//pVideoMat->m_PhyData.fAlpha = fMatAlpha;
	//if(!pVideoMat->m_hPhyCB || pVideoMat->m_nChangeType & SYMaterial_PhyDataChange ||
	//	IsNeedUpdatePhyParamBuffer(fAlpha,phy[PHY_DIFFUSE],phy[PHY_AMBIENT],phy[PHY_SPECULAR],phy[PHY_EMISSIVE],pVideoMat))
	//{
	//	pVideoMat->m_PhyData.Diffuse = phy[PHY_DIFFUSE];
	//	pVideoMat->m_PhyData.Ambient = phy[PHY_AMBIENT];
	//	pVideoMat->m_PhyData.Specular = phy[PHY_SPECULAR];
	//	pVideoMat->m_PhyData.Emissive = phy[PHY_EMISSIVE];
	//	pVideoMat->m_PhyData.disable = pMaterial->m_PhyData.disable;
	//	pVideoMat->m_fAlpha = fAlpha;
	//	pVideoMat->m_PhyData.Power = phy[4].x;

	//	phy[4].y = fAlpha;

	//	if(!pVideoMat->m_hPhyCB)
	//		pVideoMat->m_hPhyCB = DX3DEngine_CreateBuffer(pVideoMat->m_hDevice,sizeof(float4),5,phy,DXConstBuffer);
	//	else
	//		DX3DEngine_ModifyBuffer(pVideoMat->m_hDevice,pVideoMat->m_hPhyCB,phy);
	//	return SYMaterial_PhyDataChange;
	//}
	//SYMaterial_ClearChangeType(pVideoMat,SYMaterial_PhyDataChange);
	return SYMaterial_NoChange;
}

BOOL CalTexMatrix(matrix4f* pTexMat,double dTotalTime,double dFrame,SYArray* pTexParam,SYMatVideoTex* pVideoTex,SYMatTex* pTex)
{
	float2	Offset = {0,0}, Scale = {1,1}, Center = {0,0}; 
	float	Rotate = 0;
	if(pTex->bAutoRun)
	{
		double dOffset = dTotalTime / 2 * pTex->dSpeed;
		Offset = pTex->vRunDir * (float)dOffset;
		Scale.x  = pTex->TexScale.x;
		Scale.y  = pTex->TexScale.y;
		Center = pTex->Center;
		Rotate = pTex->TexRotate;
	}
	else
	{
		if(pTexParam)
		{
			//param_def* pParam = (param_def*)SYAR_GetAt(pTexParam,SYTex_Offset_ParamDef);
			//calc_get_value(pParam,dFrame,&Offset,NULL);
			//pParam = (param_def*)SYAR_GetAt(pTexParam,SYTex_Scale_ParamDef);
			//calc_get_value(pParam,dFrame,&Scale,NULL);
			//pParam = (param_def*)SYAR_GetAt(pTexParam,SYTex_Center_ParamDef);
			//calc_get_value(pParam,dFrame,&Center,NULL);
			//pParam = (param_def*)SYAR_GetAt(pTexParam,SYTex_Rotate_ParamDef);
			//calc_get_value(pParam,dFrame,&Rotate,NULL);
		}

		Offset += pTex->TexOffset;
		Scale.x  *= pTex->TexScale.x;
		Scale.y  *= pTex->TexScale.y;
		Center += pTex->Center;
		Rotate += pTex->TexRotate;
	}

	//计算曲线插值
	if(pVideoTex->hTexCB && !IsNeedUpdateTexParamBuffer(&Offset,&Scale,&Center,&Rotate,pVideoTex))
	{
		*pTexMat = pVideoTex->TexMat ;
		return FALSE;
	}
	matrix4f TexMat,CenterMat,CenterMatInv,ScaleMat,RotateMat;
	matrix4f_from_scale(&ScaleMat,Scale.x,Scale.y,0);
	matrix4f_from_pos(&CenterMat,Center.x,Center.y,0);
	matrix4f_from_pos(&CenterMatInv,-Center.x,-Center.y,0);
	matrix4f_fromeuler_lyxz(&RotateMat,0,0,-Rotate);
	*pTexMat = CenterMat * ScaleMat * RotateMat * CenterMatInv ;
	if(pTex->bAutoRun)
	{
		float3 vOne = {1,1,0};
		vOne *= *pTexMat;
		vOne.x +=(vOne.x < 0 ? -1 : 1); vOne.y+=(vOne.y < 0 ? -1 : 1);
		int nInt = (int)(Offset.x/ vOne.x);
		Offset.x -= nInt * vOne.x;
		nInt = (int)(Offset.y/ vOne.y);
		Offset.y -= nInt * vOne.y;
		Offset += pTex->TexOffset ; 
	}
	matrix4f_from_pos(&TexMat,Offset.x,Offset.y,0);
	*pTexMat *= TexMat;

	pVideoTex->TexMat = *pTexMat;
	pVideoTex->CurTexOffset = Offset;
	pVideoTex->CurTexScale = Scale;
	pVideoTex->CurCenter = Center;
	pVideoTex->CurTexRotate = Rotate;
	return TRUE;
}
BOOL UpdateTexParam(double dTotalFrame,double dFrame,int hDevice,SYMaterialParam* pMatParam,SYMatVideoTex* pVideoTex,SYMatTex* pTex)
{
	if(!pTex || !pVideoTex )
		return FALSE;
	BOOL bChange = FALSE;

	matrix4f TexMat;
	float4 TexData1[5];
	SYArray* pTexParam = pMatParam ? pMatParam->texParam + pTex->nType : NULL;
	bChange |= CalTexMatrix(&TexMat,dTotalFrame,dFrame,pTexParam,pVideoTex,pTex);
	void* pMemBuffer = &TexMat;
	int nSize = sizeof(matrix4f);

	//param_def* pParam = NULL;
	//if(pMatParam)
	//	pParam = (param_def*)SYAR_GetAt(&pMatParam->texParam[pTex->nType],SYTex_Private_ParamDef);
	//if(pTex->nType == SYMaterial_SpecTex 
	//	|| pTex->nType == SYMaterial_EnvMapTex 
	//	|| pTex->nType == SYMaterial_EmissiveTex 
	//	)
	//{
	//	float fValue = 0;
	//	if(pParam)
	//		calc_get_value(pParam,dFrame,&fValue,NULL);
	//	if (!pTex->disable)
	//	{
	//		TexData1[4].x = fValue + *(float*)pTex->pData;
	//	}
	//	if (pVideoTex->pCurData == NULL)
	//	{
	//		CreateTexPrivateData(&pVideoTex->pCurData,pTex->nType);
	//	}
	//	if((TexData1[4].x != *(float*)pVideoTex->pCurData) || bChange)
	//	{
	//		bChange = TRUE;
	//		*(float*)pVideoTex->pCurData = TexData1[4].x ;
	//		memcpy(TexData1,&TexMat,sizeof(matrix4f));
	//		pMemBuffer = TexData1;
	//		nSize = 5 * sizeof(float4);
	//	}
	//}
	//else if(pTex->nType == SYMaterial_BumpTex)
	//{
	//	float fValue = 0;
	//	if(pParam)
	//		calc_get_value(pParam,dFrame,&fValue,NULL);
	//	SYBumpTexPrivateData* pValue = (SYBumpTexPrivateData*)pTex->pData;
	//	if (!pTex->disable)
	//	{
	//		TexData1[4].x = fValue + pValue->fFactor;
	//	}
	//	if(pValue->bReverse)
	//		TexData1[4].x = -TexData1[4].x;
	//	if (pVideoTex->pCurData == NULL)
	//	{
	//		CreateTexPrivateData(&pVideoTex->pCurData,pTex->nType);
	//	}
	//	if((TexData1[4].x != *(float*)pVideoTex->pCurData) || bChange)
	//	{
	//		bChange = TRUE;
	//		*(float*)pVideoTex->pCurData = TexData1[4].x ;
	//		memcpy(TexData1,&TexMat,sizeof(matrix4f));
	//		pMemBuffer = TexData1;
	//		nSize = 5 * sizeof(float4);
	//	}
	//}

	if(!pVideoTex->hTexCB)
	{
		pVideoTex->hTexCB = DX3DEngine_CreateBuffer(hDevice,nSize,1,pMemBuffer,DXConstBuffer);
		bChange = TRUE;
	}
	else if(bChange)
	{
		DX3DEngine_ModifyBuffer(hDevice,pVideoTex->hTexCB,pMemBuffer);
	}
	return bChange;
}

BASEGRAPH_DATARENDER_API int SYUpdateDynamicTex(double dTotalFrame,int hDevice,SYMatVideoTex* pVideoTex,SYMatTex* pTex)
{
	return UpdateTexParam(dTotalFrame,dTotalFrame,hDevice,NULL,pVideoTex,pTex);
}

BASEGRAPH_DATARENDER_API int SYMaterial_UpdateFrame( SYVideoMaterial* pVideoMat,double dFrame,float fFrameRate,float fAlpha,int nLightNum,SYMaterialParam* param,void* pEnvParam, LPRECT pBound/* = NULL*/)
{
	int nChange = 0;
	if(param || pVideoMat->m_nChangeType & SYMaterial_PhyDataChange)
		nChange |= UpdatePhyParam(dFrame,param,pVideoMat,fAlpha);
	if(pVideoMat->m_nChangeType & SYMaterial_TexCountChange)
	{
		for (int i = 0; i < SYMaterial_TexTypeCount;i++)
		{
			if (pVideoMat->m_pMaterial->m_TexArray.ppTex[i] &&
				pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->disable)
			{
				SYMaterial_ReleaseVideoTex(pVideoMat->m_hDevice,pVideoMat->m_VideoTexAr.ppVTex[i]);
				SAFE_DELETE(pVideoMat->m_VideoTexAr.ppVTex[i]);
			}
			else if (pVideoMat->m_VideoTexAr.ppVTex[i]&& !pVideoMat->m_pMaterial->m_TexArray.ppTex[i])
			{
				SYMaterial_ReleaseVideoTex(pVideoMat->m_hDevice,pVideoMat->m_VideoTexAr.ppVTex[i]);
				SAFE_DELETE(pVideoMat->m_VideoTexAr.ppVTex[i]);
			}
			else if(!pVideoMat->m_VideoTexAr.ppVTex[i]&& pVideoMat->m_pMaterial->m_TexArray.ppTex[i])
			{
				SYMatVideoTex* pTex = new SYMatVideoTex;
				memset(pTex,0,sizeof(SYMatVideoTex));
				pTex->CurTexScale.x = 1;
				pTex->CurTexScale.y = 1;
				pTex->bTexNeedRelease = TRUE;
				pTex->nType = i;
				CreateTexPrivateData(&pTex->pCurData,i);
				pVideoMat->m_VideoTexAr.ppVTex[i] = pTex;

				SYMaterial_SetChangeType(pVideoMat,SYMaterial_DiffTexChange << i | SYMaterial_DiffuseParamChange << i);
			}
		}
	}
	if((pVideoMat->m_nChangeType & SYMaterial_TexCountChange) || (pVideoMat->m_nChangeType & SYMaterial_ShaderChange) || nLightNum != pVideoMat->m_nLightNum)
	{
		SYMaterial_JoinShader(pVideoMat,nLightNum);
		SYMaterial_ClearChangeType(pVideoMat,SYMaterial_TexCountChange | SYMaterial_ShaderChange);
		pVideoMat->m_nLightNum = nLightNum;
		nChange |=  SYMaterial_ShaderChange;
	}
	if (pVideoMat->m_dTime != dFrame)
	{
		pVideoMat->m_dTotalTime += dFrame - pVideoMat->m_dTime;
		//if(UpdatePhyParam(nTime,param,pVideoMat,fAlpha ))
		//	nChange |= SYMaterial_PhyDataChange;
		for(int i = 0; i< SYMaterial_TexTypeCount;i ++)
		{
			if(SYMaterial_UpdateTex(NULL,NULL,NULL,SYMaterial_DiffuseTex + i,dFrame,fFrameRate, pVideoMat,pEnvParam, FALSE, pBound))
				nChange |=  SYMaterial_DiffTexChange << i;
			if(UpdateTexParam(pVideoMat->m_dTotalTime,dFrame,pVideoMat->m_hDevice,param,pVideoMat->m_VideoTexAr.ppVTex[i],pVideoMat->m_pMaterial->m_TexArray.ppTex[i]))
				nChange |=  SYMaterial_DiffuseParamChange << i;
			SYMaterial_ClearChangeType(pVideoMat,SYMaterial_DiffTexChange << i | SYMaterial_DiffuseParamChange << i);
		}
		pVideoMat->m_dTime = dFrame;
	}
	else
	{
		//pVideoMat->m_dTotalTime += 1;
		for(int i = 0; i < SYMaterial_TexTypeCount; i++)
		{
			if(!pVideoMat->m_pMaterial->m_TexArray.ppTex[i]
			|| pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->disable)
				continue;

			if (pVideoMat->m_nChangeType & SYMaterial_DiffTexChange << i || pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->nSrcType == SYMaterial_MovFile)
			{
				if(SYMaterial_UpdateTex(NULL,NULL,NULL,SYMaterial_DiffuseTex + i,dFrame,fFrameRate,pVideoMat,pEnvParam, FALSE, pBound))
					nChange |=  SYMaterial_DiffTexChange << i;
				SYMaterial_ClearChangeType(pVideoMat,SYMaterial_DiffTexChange << i);
			}
			if(pVideoMat->m_nChangeType & SYMaterial_DiffuseParamChange << i || pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->bAutoRun)
			{				
				if(UpdateTexParam(pVideoMat->m_dTotalTime,dFrame,pVideoMat->m_hDevice,param,pVideoMat->m_VideoTexAr.ppVTex[i],pVideoMat->m_pMaterial->m_TexArray.ppTex[i]))
					nChange |=  SYMaterial_DiffuseParamChange << i;
				SYMaterial_ClearChangeType(pVideoMat,SYMaterial_DiffuseParamChange << i);
			}
		}
	}
	return nChange;
}

BASEGRAPH_DATARENDER_API void SYMaterial_SetParamToDevice( SYVideoMaterial* pVideoMat, BOOL bAdd)
{
	bool bAlpha = (pVideoMat->m_fAlpha != 1) || (pVideoMat->m_PhyData.Diffuse.w < 1);

	DX3DEngine_SetConstBuffer(pVideoMat->m_hDevice,pVideoMat->m_hPhyCB,SYMaterial_PhyParam,DXShaderType_PixelShader);
	for(int i = 0; i < SYMaterial_TexTypeCount;i++)
	{
		SYMatVideoTex* pVideoTex = pVideoMat->m_VideoTexAr.ppVTex[i];
		if(pVideoTex)
		{
			if(pVideoMat==NULL || 
				pVideoMat->m_pMaterial==NULL || 
				pVideoMat->m_pMaterial->m_TexArray.ppTex[i]==NULL)// ||
// 				pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->disable)
				continue;
			if(pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->nSrcType == SYMaterial_MovFile)
			{
				int test = 0;
			}
			if (pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->pTexPath != NULL &&
				_tcscmp(pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->pTexPath, _T("NONE")) == 0)
			{
				SYMaterial_SetChangeType(pVideoMat,SYMaterial_ShaderChange);
				//SYMaterial_ReleaseVideoTex(pVideoMat->m_hDevice,pVideoMat->m_VideoTexAr.ppVTex[i], FALSE);
				//SAFE_DELETE(pVideoMat->m_VideoTexAr.ppVTex[i]);
			}
			bAlpha = true;
			DX3DEngine_SetConstBuffer(pVideoMat->m_hDevice,pVideoTex->hTexCB,SYMaterial_DiffuseShaderParam + i,DXShaderType_PixelShader);
			DXSampleStateType nState = DXSample_Anisotropic_Clamp;
			if(pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->bAutoRun )
				nState = pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->bRepeat ? DXSample_Anisotropic_Wrap : DXSample_Anisotropic_Border;
			else if(i == SYMaterial_SpecTex)
				nState = pVideoMat->m_bTexClamp ? DXSample_NoMipMap_Anisotropic_Clamp : DXSample_NoMipMap_Anisotropic_Wrap ;
			else
				nState = pVideoMat->m_bTexClamp ? DXSample_Line_Clamp : DXSample_Line_Wrap ;

			DX3DEngine_SetSampleState(pVideoMat->m_hDevice,SYMaterial_DiffuseShaderParam + i,nState,DXShaderType_PixelShader);
			DX3DEngine_SetShaderTexture(pVideoMat->m_hDevice,pVideoTex->hTex,SYMaterial_DiffuseShaderParam + i,DXShaderType_PixelShader);
			if(0)
				DX3DEngine_DumpTexture(pVideoMat->m_hDevice,pVideoTex->hTex);
		}
	}
	if (bAdd)
	{
		DX3DEngine_SetRenderState(pVideoMat->m_hDevice,DXBlendState,DXBlend_Add);
	}
	else
	{
		if(bAlpha)
			DX3DEngine_SetRenderState(pVideoMat->m_hDevice,DXBlendState,DXBlend_Trans_Mix);
		else
			DX3DEngine_SetRenderState(pVideoMat->m_hDevice,DXBlendState,DXBlendDisable );
	}
}

SYMovieResource* CreateMovieResource(TCHAR* pTexPath,double dTime,int hDevice)
{
	//SMovieInfo movieInfo;
	//if( !SYGetMovieInfo( pTexPath, movieInfo ) || movieInfo.dwFrameNum == 0 )
	//	return FALSE;
	//SYMovieResource* pResource = new SYMovieResource;
	//pResource->pVideoSource = new CFileVideoSource();

	//VideoSourceInfo vs_info;   
	//_tcscpy_s(vs_info.file_name, MAX_PATH, pTexPath );
	//vs_info.color_format =  movieInfo.dwColorFormat;
	//vs_info.width = movieInfo.nMotionWidth;
	//vs_info.height = movieInfo.nMotionHeight;
	//vs_info.scan_mode = movieInfo.ScanMode;
	//pResource->nMovieWidth = movieInfo.nMotionWidth;
	//pResource->nMovHeight = movieInfo.nMotionHeight;
	//pResource->nTotalFrame = movieInfo.dwFrameNum;

	//pResource->bIsProgress = (movieInfo.ScanMode == DYVIDEOSCANMODE_Progressive 
	//	|| movieInfo.ScanMode == DYVIDEOSCANMODE_ProgressiveSegmented);

	////vs_info.obj_in_frame = 0;
	////vs_info.obj_out_frame = movieInfo.dwFrameNum-1;

	//vs_info.trim_in_frame = 0;//pMovie->nTrimInFrame;
	//vs_info.trim_out_frame = movieInfo.dwFrameNum-1;//pMovie->nTrimOutFrame; //movieInfo.dwFrameNum-1;
	//vs_info.start_frame = 0;

	////vs_info.time_in_frame = 0;
	////vs_info.time_out_frame = movieInfo.dwFrameNum-1;

	//vs_info.dx_device = hDevice;

	//vs_info.audio_init_size = 0;

	//pResource->pVideoSource->Stop();
	//pResource->pVideoSource->SetVideoInfo(vs_info);
	//pResource->pVideoSource->Cue();

	//pResource->hTex[0] = DX3DEngine_CreateTexture(hDevice,movieInfo.nMotionWidth,movieInfo.nMotionHeight,DXTexture_Normal2DTex);
	//pResource->hTex[1] = DX3DEngine_CreateTexture(hDevice,movieInfo.nMotionWidth,movieInfo.nMotionHeight,DXTexture_Normal2DTex);

	//MovieProcessPara para;
	////para.maskBuf = 0;
	//pResource->nCur = 0;
	//para.hMovieTex = pResource->hTex[pResource->nCur];
	//para.field =  SECOND_FIELD;
	//para.nFrame = (int)dTime;  
	//pResource->pVideoSource->UpdateFrame(para);
	//return pResource;
	return nullptr;
}

int CalcMoviePlayTime( int nField, int nTotalFrame )
{
	int nRealFrame = (nField/2)%nTotalFrame;
	return nRealFrame;
}
BASEGRAPH_DATARENDER_API BOOL SYMaterial_UpdateTex( SYBufferDef32* pSrcBufferDef, DXHalDef32* pSrcTexDef,int hSrcTex, int nTexIndex,double dTime, float fFrameRate, SYVideoMaterial* pVideoMat,void* param,BOOL isForceUpdate /*= FALSE*/, LPRECT pBound/* = NULL*/ )
{
	SYMatTex* pTex = pVideoMat->m_pMaterial->m_TexArray.ppTex[nTexIndex];
	if(!pTex)
		return FALSE;
	SYMatVideoTex* pVideoTex = pVideoMat->m_VideoTexAr.ppVTex[nTexIndex];
	if(!pVideoTex)
		return FALSE;

	if(pTex->nSrcType == SYMaterial_Object)
	{
		RECT rtBuffer;
		g_pObjectFun(SYMaterial_GetBufferSize,&rtBuffer,NULL,g_pObjectParam,param);
		if(!pVideoTex->hTex)
		{
			pVideoTex->hTex = DX3DEngine_CreateTexture(pVideoMat->m_hDevice,rtBuffer.right,rtBuffer.bottom,DXTexture_Normal2DTex);
			pVideoTex->bTexNeedRelease = TRUE;
		}
		else
		{
			int nWidth,nHeight;
			DX3DEngine_GetTextureInfo(pVideoMat->m_hDevice,pVideoTex->hTex,&nWidth,&nHeight,NULL);
			if(nWidth < rtBuffer.right || nHeight < rtBuffer.bottom)
			{
				DX3DEngine_ReleaseTexture(pVideoMat->m_hDevice,pVideoTex->hTex);
				pVideoTex->hTex = DX3DEngine_CreateTexture(pVideoMat->m_hDevice,rtBuffer.right,rtBuffer.bottom,DXTexture_Normal2DTex);
				pVideoTex->bTexNeedRelease = TRUE;
			}
		}
		return g_pObjectFun(SYMaterial_GetBuffer,NULL,pVideoTex->hTex,g_pObjectParam,param);
	}
	else if(pTex->nSrcType == SYMaterial_Channel)
	{
		RECT rtBuffer;
		g_pChannelFun(SYMaterial_GetBufferSize,&rtBuffer,NULL,g_pChannelParam,param);
		if(!pVideoTex->hTex)
		{
			pVideoTex->hTex = DX3DEngine_CreateTexture(pVideoMat->m_hDevice,rtBuffer.right,rtBuffer.bottom,DXTexture_Normal2DTex);
			pVideoTex->bTexNeedRelease = TRUE;
		}
		else
		{
			int nWidth,nHeight;
			DX3DEngine_GetTextureInfo(pVideoMat->m_hDevice,pVideoTex->hTex,&nWidth,&nHeight,NULL);
			if(nWidth < rtBuffer.right || nHeight < rtBuffer.bottom)
			{
				DX3DEngine_ReleaseTexture(pVideoMat->m_hDevice,pVideoTex->hTex);
				pVideoTex->hTex = DX3DEngine_CreateTexture(pVideoMat->m_hDevice,rtBuffer.right,rtBuffer.bottom,DXTexture_Normal2DTex);
				pVideoTex->bTexNeedRelease = TRUE;
			}
		}
		return g_pChannelFun(SYMaterial_GetBuffer,NULL,pVideoTex->hTex,g_pChannelParam,param);
	}
	else if(pTex->nSrcType == SYMaterial_PicFile)
	{
		if(!(pVideoMat->m_nChangeType & SYMaterial_DiffTexChange << pTex->nType))
			return FALSE;
		//处理图片
		if(pVideoTex->hTex)
		{
			DX3DEngine_ReleaseTexture(pVideoMat->m_hDevice,pVideoTex->hTex);
			pVideoTex->hTex = 0;
		}

		TCHAR realpath[MAX_PATH];
		//GetFileManager()->TagPathTrans(pTex->pTexPath,realpath,GetFileManager()->lpParam);
		//路径无效时,返回
		if (/*pTex->disable || */!pTex->pTexPath || _taccess(realpath, 0) == -1)
			return FALSE;

		BOOL bMap = pTex->nType == SYMaterial_BumpTex;

		pVideoTex->hTex = DX3DEngine_CreateTextureFromFile(pVideoMat->m_hDevice,realpath,bMap);
#ifdef _DEBUG
		//assert(pVideoTex->hTex > 0);
#endif
		if( pVideoTex->hTex == 0 )
			return FALSE;

		if(0)
		{
			DX3DEngine_DumpTexture(pVideoMat->m_hDevice,pVideoTex->hTex);
		}
		return TRUE;
	}
	else if(pTex->nSrcType == SYMaterial_MovFile)
	{
		//if(!(pVideoMat->m_nChangeType & SYMaterial_DiffTexChange << pTex->nType) && pVideoMat->m_dTime == dTime)
		//	return FALSE;

		if (!pTex->pTexPath )
			return FALSE;
		TCHAR realpath[MAX_PATH];
		//GetFileManager()->TagPathTrans(pTex->pTexPath,realpath,GetFileManager()->lpParam);
		if( _taccess(realpath, 0) == -1)
			return FALSE;

		if((pVideoMat->m_nChangeType & SYMaterial_DiffTexChange << pTex->nType))
		{
			SYMaterial_ReleaseVideoTex(pVideoMat->m_hDevice,pVideoTex,FALSE);
		}

		int nField = (int)pVideoMat->m_dTotalTime;
		if( pVideoMat->m_bEdit)
		{
			//if(!pVideoTex->pSeekData)
			//{   
			//	pVideoTex->pSeekData = new SYMovieSeekData;
			//	SMovieInfo movieInfo;
			//	SYGetMovieInfo(realpath, movieInfo );
			//	pVideoTex->pSeekData->nWidth = movieInfo.nMotionWidth;
			//	pVideoTex->pSeekData->nHeight = movieInfo.nMotionHeight;
			//	if (pBound)
			//	{
			//		//这里对宽高做一个处理，以物件边界作为参考值，缩小视频的解码宽高
			//		CRect rtBound = *pBound;
			//		rtBound.NormalizeRect();
			//		int nBoundWidth = rtBound.Width();
			//		int nBoundHeight = rtBound.Height();
			//		//仅处理如果视频宽高都比物件边界大的情况
			//		if (nBoundWidth > 0 && nBoundHeight > 0 && movieInfo.nMotionWidth > nBoundWidth && movieInfo.nMotionHeight > nBoundHeight)
			//		{
			//			//变换原则是按视频比例缩小，但要确保宽高>=物件边界
			//			float fWRate = nBoundWidth * 1.0f / movieInfo.nMotionWidth;
			//			float fHRate = nBoundHeight * 1.0f /movieInfo.nMotionHeight;
			//			if (fWRate > fHRate)
			//			{
			//				pVideoTex->pSeekData->nWidth *= fWRate;
			//				pVideoTex->pSeekData->nHeight *= fWRate;
			//			}
			//			else
			//			{
			//				pVideoTex->pSeekData->nWidth *= fHRate;
			//				pVideoTex->pSeekData->nHeight *= fHRate;
			//			}
			//		}
			//	}
			//	pVideoTex->pSeekData->nTotalFrame = movieInfo.dwFrameNum;
			//	pVideoTex->pSeekData->pVideoSeekData = NULL;
			//	SYMoviePrePlay(realpath,pVideoTex->pSeekData->nWidth,pVideoTex->pSeekData->nHeight,fFrameRate,
			//		pVideoTex->pSeekData->pVideoSeekData, TRUE );
			//	pVideoTex->pSeekData->movie_buffer.Create(pVideoTex->pSeekData->nWidth , pVideoTex->pSeekData->nHeight);

			//}

			//int nMovieTime = CalcMoviePlayTime(nField,pVideoTex->pSeekData->nTotalFrame);    
			//SYMoviePlay(pVideoTex->pSeekData->pVideoSeekData, &pVideoTex->pSeekData->movie_buffer,nMovieTime , NULL, NULL );
			//if(!pVideoTex->hTex)
			//{
			//	pVideoTex->hTex = DX3DEngine_CreateTexture(pVideoMat->m_hDevice,pVideoTex->pSeekData->nWidth,pVideoTex->pSeekData->nHeight,DXTexture_Normal2DTex);
			//	pVideoTex->bTexNeedRelease = TRUE;
			//}

			//int hTransTex = DXTextureManager_QueryTexture(SYTexture_MemTexture,pVideoTex->pSeekData->nWidth,pVideoTex->pSeekData->nHeight);
			//DX3DEngine_UpLoad(pVideoTex->hTex,pVideoMat->m_hDevice,hTransTex,0,0,pVideoTex->pSeekData->nWidth,pVideoTex->pSeekData->nHeight, 
			//	pVideoTex->pSeekData->movie_buffer.m_dwBuffer,0,0,pVideoTex->pSeekData->nWidth,pVideoTex->pSeekData->nHeight,
			//	pVideoTex->pSeekData->nWidth,pVideoTex->pSeekData->nHeight);
			//DXTextureManager_UnQueryTexture(hTransTex);
		}
		else
		{
			if(!pVideoTex->pResource)
			{
				pVideoTex->pResource = CreateMovieResource(realpath,dTime,pVideoMat->m_hDevice);

				if(pVideoTex->hTex)
				{
					int nTexWidth,nTexHeight;
					DX3DEngine_GetTextureInfo(pVideoMat->m_hDevice,pVideoTex->hTex,&nTexWidth,&nTexHeight,NULL);
					if(nTexWidth != pVideoTex->pResource->nMovieWidth || nTexHeight != pVideoTex->pResource->nMovHeight)
					{
						DX3DEngine_ReleaseTexture(pVideoMat->m_hDevice,pVideoTex->hTex);
						pVideoTex->hTex = 0;
					}
				}
			}
			if(!pVideoTex->hTex)
			{
				pVideoTex->hTex = DX3DEngine_CreateTexture(pVideoMat->m_hDevice,pVideoTex->pResource->nMovieWidth,pVideoTex->pResource->nMovHeight,DXTexture_Normal2DTex);
				pVideoTex->bTexNeedRelease = TRUE;
			}

			DX3DEngine_CopyTexture(pVideoMat->m_hDevice,pVideoTex->hTex,0,0,pVideoTex->pResource->nMovieWidth,pVideoTex->pResource->nMovHeight, 
				pVideoTex->pResource->hTex[pVideoTex->pResource->nCur],0,0,pVideoTex->pResource->nMovieWidth,pVideoTex->pResource->nMovHeight);
			BUFFER_PARITY_STYLE parity = (BUFFER_PARITY_STYLE)((nField % 2 == 0) ?PARITY_EVEN : PARITY_ODD);
			//if(pVideoTex->pResource->bIsProgress && parity == SY_PARITY_EVEN) //
			//	return TRUE;
			//if(!pVideoTex->pSeekData)
			//{   
			//	pVideoTex->pSeekData = new SYMovieSeekData;
			//	SMovieInfo movieInfo;
			//	SYGetMovieInfo(realpath, movieInfo );
			//	pVideoTex->pSeekData->nWidth = movieInfo.nMotionWidth;
			//	pVideoTex->pSeekData->nHeight = movieInfo.nMotionHeight;
			//	pVideoTex->pSeekData->nTotalFrame = movieInfo.dwFrameNum;
			//	pVideoTex->pSeekData->pVideoSeekData = NULL;
			//	SYMoviePrePlay(realpath,pVideoTex->pSeekData->nWidth,pVideoTex->pSeekData->nHeight,fFrameRate,
			//		pVideoTex->pSeekData->pVideoSeekData, TRUE );
			//	pVideoTex->pSeekData->movie_buffer.Create(pVideoTex->pSeekData->nWidth , pVideoTex->pSeekData->nHeight);

			//}
			int nMovieTime = CalcMoviePlayTime(nField,pVideoTex->pResource->nTotalFrame);    
			//SYMoviePlay(pVideoTex->pSeekData->pVideoSeekData, &pVideoTex->pSeekData->movie_buffer,nMovieTime , NULL, NULL );

			pVideoTex->pResource->nCur = (pVideoTex->pResource->nCur + 1)%2;

			//MovieProcessPara para;
			////para.maskBuf = 0;
			//para.hMovieTex = pVideoTex->pResource->hTex[pVideoTex->pResource->nCur];
			////这块进行融合时，因为前边已经将场号限定在只渲偶场，所以顶场先及帧模式需要变动
			//if(parity == SY_PARITY_EVEN)
			//{
			//	//para.field =  FIRST_FIELD;
			//	//para.nFrame = nMovieTime;
			//	para.field =  SECOND_FIELD;
			//	para.nFrame = nMovieTime;
			//}
			//else if(pVideoTex->pResource->bIsProgress)
			//{
			//	para.field =  SECOND_FIELD;
			//	para.nFrame = nMovieTime + 1;
			//}
			//else
			//{
			//	//para.field =  SECOND_FIELD;
			//	//para.nFrame = nMovieTime + 1;
			//	para.field =  SECOND_FIELD;
			//	para.nFrame = nMovieTime;
			//}
			//pVideoTex->pResource->pVideoSource->UpdateFrame(para);

		}
		if(0)
		{
			DX3DEngine_DumpTexture(pVideoMat->m_hDevice,pVideoTex->hTex);
		}
		return TRUE;
	}
	else if(pTex->nSrcType == SYMaterial_Buffer)
	{
		int nWidth,nHeight;
		int nSrcWidth , nSrcHeight;
		RECT* pBound;

		if(pSrcTexDef)
		{
			nSrcWidth = pSrcTexDef->m_rtBound.right - pSrcTexDef->m_rtBound.left;
			nSrcHeight = pSrcTexDef->m_rtBound.bottom - pSrcTexDef->m_rtBound.top;
			pBound = &pSrcTexDef->m_rtBound;
			if(pVideoTex->hTex)
			{
				DX3DEngine_GetTextureInfo(pVideoMat->m_hDevice,pVideoTex->hTex,&nWidth,&nHeight,NULL);
				if(nWidth < nSrcWidth || nHeight < nSrcHeight)
				{
					DX3DEngine_ReleaseTexture(pVideoMat->m_hDevice,pVideoTex->hTex);
					pVideoTex->hTex = 0;
				}
			}
			if(!pVideoTex->hTex)
			{
				nWidth = nSrcWidth;
				nHeight = nSrcHeight;
				pVideoTex->hTex = DX3DEngine_CreateTexture(pVideoMat->m_hDevice,nWidth,nHeight,DXTexture_MipMap);
				pVideoTex->bTexNeedRelease = TRUE;
			}
			DX3DEngine_CopyTexture(pVideoMat->m_hDevice,pVideoTex->hTex,0,0,nSrcWidth,nSrcHeight,pSrcTexDef->m_hTex,0,0,nSrcWidth,nSrcHeight);
			DX3DEngine_GenerateMip(pVideoMat->m_hDevice,pVideoTex->hTex);
			if (0)
			{
				DX3DEngine_DumpTexture(pVideoMat->m_hDevice,pVideoTex->hTex);
			}
		}
		else
		{
			if(!(pVideoMat->m_nChangeType & SYMaterial_DiffTexChange << pTex->nType))
				return FALSE;
			//add by zhenghang  柱图文字需要修改纹理
			pSrcBufferDef = pTex->pBufferDef;
			if(!pSrcBufferDef)
				return FALSE;
			nSrcWidth = pSrcBufferDef->m_nWidth;
			nSrcHeight = pSrcBufferDef->m_nHeight;
			pBound = &pSrcBufferDef->m_rtBound;
			if(pVideoTex->hTex)
			{
				DX3DEngine_GetTextureInfo(pVideoMat->m_hDevice,pVideoTex->hTex,&nWidth,&nHeight,NULL);
				if(nWidth < pSrcBufferDef->m_nWidth || nHeight < pSrcBufferDef->m_nHeight)
				{
					DX3DEngine_ReleaseTexture(pVideoMat->m_hDevice,pVideoTex->hTex);
					pVideoTex->hTex = 0;
				}
			}
			if(!pVideoTex->hTex)
			{
				nWidth = pSrcBufferDef->m_nWidth;
				nHeight = pSrcBufferDef->m_nHeight;
				pVideoTex->hTex = DX3DEngine_CreateTexture(pVideoMat->m_hDevice,pSrcBufferDef->m_nWidth,pSrcBufferDef->m_nHeight,DXTexture_MipMap);				
			}
			int hTrans = DXTextureManager_QueryTexture(DXTexture_MemTexture,nWidth,nHeight);
			DX3DEngine_UpLoad(pVideoTex->hTex,pVideoMat->m_hDevice,hTrans,0,0,pSrcBufferDef->m_nWidth,pSrcBufferDef->m_nHeight,
				pSrcBufferDef->m_dwBuffer,0,0,pSrcBufferDef->m_nWidth,pSrcBufferDef->m_nHeight,pSrcBufferDef->m_nWidth,pSrcBufferDef->m_nHeight);
			DX3DEngine_GenerateMip(pVideoMat->m_hDevice,pVideoTex->hTex);
			if (0)
			{
				DX3DEngine_DumpTexture(pVideoMat->m_hDevice,pVideoTex->hTex);
			}
			DXTextureManager_UnQueryTexture(hTrans);
		}
		SYMaterial_SetTex(pVideoTex->hTex,*pBound,pSrcBufferDef->m_nOriginX,pSrcBufferDef->m_nOriginY, pVideoMat,nTexIndex);
		pVideoTex->bTexNeedRelease = TRUE;
	}
	else
	{
		//处理视频,缺少解码库
	}
	return FALSE;
}

//void InitColorParamdef(param_def* pParam)
//{
//	sy_init_paramdef(pParam,sizeof(float4),NULL);
//	pParam->interpolate.nRange             = 4;
//	pParam->interpolate.pRanges            = new PAIR[4];
//	for(int i = 0; i < 4; i++)
//	{
//		pParam->interpolate.pRanges[i].fFirst  = 0.f;
//		pParam->interpolate.pRanges[i].fSecond = 1.f;
//	}
//}

BASEGRAPH_DATARENDER_API void SYMaterial_CreateParam(SYMaterialParam* pMatParam ,SYMaterial*  pMaterial )
{
	//for (int i = 0; i < SYMATERIAL_TEX_COUNT; i++)
	//{
	//	SYAR_Init(&pMatParam->texParam[i]);
	//	SYMaterial_AddParamdef(&pMatParam->texParam[i],i);
	//}
	////pMatParam->diffuse
	//InitColorParamdef(&pMatParam->common.Diffuse);
	//InitColorParamdef(&pMatParam->common.Ambient);
	//InitColorParamdef(&pMatParam->common.Specular);
	//InitColorParamdef(&pMatParam->common.Emissive);

	//sy_init_paramdef(&pMatParam->common.fAlpha,sizeof(float),NULL);
	//pMatParam->common.fAlpha.interpolate.nRange   = 1;
	//pMatParam->common.fAlpha.interpolate.pRanges  = new PAIR[1];
	//pMatParam->common.fAlpha.interpolate.pRanges[0].fFirst  = 0.f;
	//pMatParam->common.fAlpha.interpolate.pRanges[0].fSecond = 1.f;
}

BASEGRAPH_DATARENDER_API void SYMaterial_ReleaseParam( SYMaterialParam* pMatParam ,SYMaterial* pMaterial )
{
	//for (int i = 0; i < SYMATERIAL_TEX_COUNT; i++)
	//{
	//	int nParamCount = SYAR_GetCount(&pMatParam->texParam[i]);
	//	for(int j = 0; j < nParamCount; j++)
	//	{
	//		param_def* param = (param_def*)SYAR_GetAt(&pMatParam->texParam[i],j);
	//		sy_release_paramdef(param);
	//		SAFE_DELETE(param);
	//	}
	//}
	////pMatParam->diffuse
	//sy_release_paramdef(&pMatParam->common.Diffuse);
	//sy_release_paramdef(&pMatParam->common.Ambient);
	//sy_release_paramdef(&pMatParam->common.Specular);
	//sy_release_paramdef(&pMatParam->common.Emissive);
	//sy_release_paramdef(&pMatParam->common.fAlpha);
}

BASEGRAPH_DATARENDER_API int SYMaterial_AddKeyValue( double dFrame,SYMaterialParam* pMatParam, SYMaterial* pMaterial,void* pValue,int nParamType )
{
	//if (nParamType == SYMaterial_Phy_ParamDef)
	//{
	//	SYPhyData* pData = (SYPhyData*)pValue;
	//	float4 fColor;
	//	DWORDColorToFloat4(&fColor,pData->Diffuse - pMaterial->m_PhyData.Diffuse);
	//	AddClipKeyFrame(&pMatParam->common.Diffuse,dFrame,(BYTE*)&fColor);

	//	DWORDColorToFloat4(&fColor,pData->Ambient - pMaterial->m_PhyData.Ambient);
	//	AddClipKeyFrame(&pMatParam->common.Ambient,dFrame,(BYTE*)&fColor);

	//	DWORDColorToFloat4(&fColor,pData->Specular - pMaterial->m_PhyData.Specular);
	//	AddClipKeyFrame(&pMatParam->common.Specular,dFrame,(BYTE*)&fColor);

	//	DWORDColorToFloat4(&fColor,pData->Emissive - pMaterial->m_PhyData.Emissive);
	//	AddClipKeyFrame(&pMatParam->common.Emissive,dFrame,(BYTE*)&fColor);

	//	float fValue = pData->fAlpha - pMaterial->m_PhyData.fAlpha;
	//	AddClipKeyFrame(&pMatParam->common.fAlpha,dFrame,(BYTE*)&fValue);
	//	pMaterial->m_PhyData.Power = pData->Power;
	//	return SYMaterial_PhyDataChange;
	//}
	//else if(nParamType == SYMaterial_Phy_Diffuse_ParamDef)
	//{
	//	float4* pColor = (float4*)pValue;
	//	AddClipKeyFrame(&pMatParam->common.Diffuse,dFrame,(BYTE*)pColor);
	//	return SYMaterial_PhyDataChange;
	//}
	//else if(nParamType == SYMaterial_Phy_Ambient_ParamDef)
	//{
	//	float4* pColor = (float4*)pValue;
	//	AddClipKeyFrame(&pMatParam->common.Ambient,dFrame,(BYTE*)pColor);
	//	return SYMaterial_PhyDataChange;
	//}
	//else if(nParamType == SYMaterial_Phy_Specular_ParamDef)
	//{
	//	float4* pColor = (float4*)pValue;
	//	AddClipKeyFrame(&pMatParam->common.Specular,dFrame,(BYTE*)pColor);
	//	return SYMaterial_PhyDataChange;
	//}
	//else if(nParamType == SYMaterial_Phy_Emissive_ParamDef)
	//{
	//	float4* pColor = (float4*)pValue;
	//	AddClipKeyFrame(&pMatParam->common.Emissive,dFrame,(BYTE*)pColor);
	//	return SYMaterial_PhyDataChange;
	//}
	//else if(nParamType == SYMaterial_Phy_Alpha_ParamDef)
	//{
	//	float* pData = (float*)pValue;
	//	float fValue = *pData- pMaterial->m_PhyData.fAlpha;
	//	AddClipKeyFrame(&pMatParam->common.fAlpha,dFrame,(BYTE*)&fValue);
	//	return SYMaterial_PhyDataChange;
	//}
	//else if(nParamType <= SYBumpTex_Private_ParamDef && nParamType >= SYDiffuseTex_Offset_ParamDef)
	//{
	//	int nTexType = (nParamType - SYDiffuseTex_Offset_ParamDef) / SYTex_ParamDefCount;
	//	int paramBegin = (nParamType - SYDiffuseTex_Offset_ParamDef) % SYTex_ParamDefCount;
	//	if(!pMaterial->m_TexArray.ppTex[nTexType])
	//		return SYMaterial_NoChange;
	//	SYMaterial_AddTexKeyValue(dFrame,&pMatParam->texParam[nTexType],pMaterial->m_TexArray.ppTex[nTexType],pValue, paramBegin);
	//	return SYMaterial_DiffuseParamChange << nTexType;
	//}
	return SYMaterial_NoChange;
}

BASEGRAPH_DATARENDER_API BOOL		SYMaterial_SetTex(int hTex,const rectf& pValidRt,int nOriginX, int nOriginY, SYVideoMaterial* pVideoMat, int nTexIndex,BOOL bNeedRelease )
{
	if(!hTex)
		return FALSE;

	float nSrcWidth , nSrcHeight;
	nSrcWidth = pValidRt.right - pValidRt.left;
	nSrcHeight = pValidRt.bottom - pValidRt.top;

	int nWidth,nHeight;
	DX3DEngine_GetTextureInfo(pVideoMat->m_hDevice,hTex,&nWidth,&nHeight,NULL);

	SYMatVideoTex* pVideoTex = pVideoMat->m_VideoTexAr.ppVTex[nTexIndex];
	SYMatTex* pTex = pVideoMat->m_pMaterial->m_TexArray.ppTex[nTexIndex];
	float fXScale = (float)nSrcWidth / nWidth ;
	pTex->TexScale.x = fXScale / (pVideoTex->CurTexScale.x / pTex->TexScale.x);
	pVideoTex->CurTexScale.x = fXScale;
	float fYScale = (float)nSrcHeight / nHeight;
	pTex->TexScale.y = fYScale / (pVideoTex->CurTexScale.y / pTex->TexScale.y);
	pVideoTex->CurTexScale.y = fYScale;
	//pVideoTex->CurTexScale.y = (float)nSrcHeight / nHeight ;
	float fXOffset = (float)(pValidRt.left + nOriginX) / nWidth ;
	pTex->TexOffset.x = fXOffset - (pVideoTex->CurTexOffset.x - pTex->TexOffset.x);
	pVideoTex->CurTexOffset.x = fXOffset;
	float fYOffset = (float)(pValidRt.top + nOriginY)/ nHeight ;
	pTex->TexOffset.y = fYOffset - (pVideoTex->CurTexOffset.y - pTex->TexOffset.y);
	pVideoTex->CurTexOffset.y = fYOffset;
	//pVideoTex->CurTexOffset.y = (float)(pValidRt.top + nOriginY)/ nHeight;

	matrix4f TexMat,CenterMat,CenterMatInv,ScaleMat,RotateMat;
	matrix4f_from_scale(&ScaleMat,pVideoTex->CurTexScale.x,pVideoTex->CurTexScale.y,0);
	matrix4f_from_pos(&CenterMat,pVideoTex->CurCenter.x,pVideoTex->CurCenter.y,0);
	matrix4f_from_pos(&CenterMatInv,-pVideoTex->CurCenter.x,-pVideoTex->CurCenter.y,0);
	matrix4f_fromeuler_lyxz(&RotateMat,0,0,-pVideoTex->CurTexRotate);
	matrix4f_from_pos(&TexMat,pVideoTex->CurTexOffset.x,pVideoTex->CurTexOffset.y,0);
	pVideoTex->TexMat = CenterMat * ScaleMat * RotateMat * CenterMatInv * TexMat;
	if(!pVideoTex->hTexCB)
		pVideoTex->hTexCB = DX3DEngine_CreateBuffer(pVideoMat->m_hDevice,sizeof(matrix4f),1,&pVideoTex->TexMat,DXConstBuffer);
	else
		DX3DEngine_ModifyBuffer(pVideoMat->m_hDevice,pVideoTex->hTexCB,&pVideoTex->TexMat);

	if(0)
	{
		DX3DEngine_DumpTexture(pVideoMat->m_hDevice,hTex);
	}
	pVideoTex->bTexNeedRelease = FALSE; 
	pVideoTex->hTex = hTex; 
	pVideoMat->m_bTexClamp = TRUE;
	return TRUE;
}

BASEGRAPH_DATARENDER_API BOOL		SYMaterial_SetTex(int hTex,const RECT& pValidRt, int nOriginX, int nOriginY,SYVideoMaterial* pVideoMat, int nTexIndex,BOOL bNeedRelease)
{
	int nWidth,nHeight;
	int nSrcWidth , nSrcHeight;

	nSrcWidth = pValidRt.right - pValidRt.left;
	nSrcHeight = pValidRt.bottom - pValidRt.top;

	if(!hTex)
	{
		return FALSE;
	}
	DX3DEngine_GetTextureInfo(pVideoMat->m_hDevice,hTex,&nWidth,&nHeight,NULL);
	SYMatVideoTex* pVideoTex = pVideoMat->m_VideoTexAr.ppVTex[nTexIndex];
	SYMatTex* pTex = pVideoMat->m_pMaterial->m_TexArray.ppTex[nTexIndex];
	if (pVideoTex == NULL || pVideoMat == NULL)
	{
		return FALSE;
	}
	float fXScale = (float)nSrcWidth / nWidth ;
	pTex->TexScale.x = fXScale / (pVideoTex->CurTexScale.x / pTex->TexScale.x);
	pVideoTex->CurTexScale.x = fXScale;
	float fYScale = (float)nSrcHeight / nHeight;
	pTex->TexScale.y = fYScale / (pVideoTex->CurTexScale.y / pTex->TexScale.y);
	pVideoTex->CurTexScale.y = fYScale;
	//pVideoTex->CurTexScale.y = (float)nSrcHeight / nHeight ;
	float fXOffset = (float)(pValidRt.left + nOriginX) / nWidth ;
	pTex->TexOffset.x = fXOffset - (pVideoTex->CurTexOffset.x - pTex->TexOffset.x);
	pVideoTex->CurTexOffset.x = fXOffset;
	float fYOffset = (float)(pValidRt.top + nOriginY)/ nHeight ;
	pTex->TexOffset.y = fYOffset - (pVideoTex->CurTexOffset.y - pTex->TexOffset.y);
	pVideoTex->CurTexOffset.y = fYOffset;
	//pVideoTex->CurTexOffset.y = (float)(pValidRt.top + nOriginY)/ nHeight;

	matrix4f TexMat,CenterMat,CenterMatInv,ScaleMat,RotateMat;
	matrix4f_from_scale(&ScaleMat,pVideoTex->CurTexScale.x,pVideoTex->CurTexScale.y,0);
	matrix4f_from_pos(&CenterMat,pVideoTex->CurCenter.x,pVideoTex->CurCenter.y,0);
	matrix4f_from_pos(&CenterMatInv,-pVideoTex->CurCenter.x,-pVideoTex->CurCenter.y,0);
	matrix4f_fromeuler_lyxz(&RotateMat,0,0,-pVideoTex->CurTexRotate);
	matrix4f_from_pos(&TexMat,pVideoTex->CurTexOffset.x,pVideoTex->CurTexOffset.y,0);
	pVideoTex->TexMat = CenterMat * ScaleMat * RotateMat * CenterMatInv * TexMat;
	if(!pVideoTex->hTexCB)
		pVideoTex->hTexCB = DX3DEngine_CreateBuffer(pVideoMat->m_hDevice,sizeof(matrix4f),1,&pVideoTex->TexMat,DXConstBuffer);
	else
		DX3DEngine_ModifyBuffer(pVideoMat->m_hDevice,pVideoTex->hTexCB,&pVideoTex->TexMat);

	if(0)
	{
		DX3DEngine_DumpTexture(pVideoMat->m_hDevice,hTex);
	}
	pVideoTex->bTexNeedRelease = FALSE; 
	pVideoTex->hTex = hTex; 
	pVideoMat->m_bTexClamp = TRUE;
	return TRUE;
}

BASEGRAPH_DATARENDER_API int SYMaterial_SetTexRepeat( SYMaterial* pMat,int nTexIndex,BOOL bRepeat )
{
	SYMatTex* pTex = pMat->m_TexArray.ppTex[nTexIndex];
	if (!pTex)
		return SYMaterial_NoChange;
	pTex->bRepeat = bRepeat;
	return SYMaterial_DiffuseParamChange << nTexIndex;
}

BASEGRAPH_DATARENDER_API void		SYMaterial_CopyParam(SYMaterialParam* pDestMatParam,const SYMaterialParam* pSrcMatParam   )
{
	//SYMaterial_ReleaseParam(pDestMatParam,NULL);

	//for (int i = 0; i < SYMATERIAL_TEX_COUNT; i++)
	//{
	//	int nCount = SYAR_GetCount(&pSrcMatParam->texParam[i]);
	//	for(int j = 0; j < nCount; j++)
	//	{
	//		param_def* pSrc = (param_def*)SYAR_GetAt(&pSrcMatParam->texParam[i],j);
	//		param_def* pDst = new param_def;
	//		memset(pDst,0,sizeof(param_def));
	//		sy_copy_paramdef(pDst,pSrc);
	//		SYAR_Add(&pDestMatParam->texParam[i],pDst);
	//	}
	//}
	////pMatParam->diffuse
	//sy_copy_paramdef(&pDestMatParam->common.Diffuse,&pSrcMatParam->common.Diffuse);
	//sy_copy_paramdef(&pDestMatParam->common.Ambient,&pSrcMatParam->common.Ambient);
	//sy_copy_paramdef(&pDestMatParam->common.Specular,&pSrcMatParam->common.Specular);
	//sy_copy_paramdef(&pDestMatParam->common.Emissive,&pSrcMatParam->common.Emissive);
	//sy_copy_paramdef(&pDestMatParam->common.fAlpha,&pSrcMatParam->common.fAlpha);
}

BASEGRAPH_DATARENDER_API size_t SYMaterial_GetVideoMemSize( SYVideoMaterial* pVideoMat )
{
	size_t size = 0;
	size += 5 * sizeof(float4);

	for (int i = 0; i < SYMATERIAL_TEX_COUNT; i++)
	{
		if(pVideoMat->m_VideoTexAr.ppVTex[i])
		{
			size += DX3DEngine_GetTextureMemSize(pVideoMat->m_hDevice,pVideoMat->m_VideoTexAr.ppVTex[i]->hTex);
			size += 5 * sizeof(float4);
		}
	}
	return size;
}

BASEGRAPH_DATARENDER_API int SYMaterial_SetTexParam( SYMaterial* pMat,int nTexIndex,SYMatTex* pTexParam,int nType )
{
	if(nType & SYMATERIAL_TEXPARAM_TEXOFFSETX)
		pMat->m_TexArray.ppTex[nTexIndex]->TexOffset.x = pTexParam->TexOffset.x;
	if(nType & SYMATERIAL_TEXPARAM_TEXOFFSETY)
		pMat->m_TexArray.ppTex[nTexIndex]->TexOffset.y = pTexParam->TexOffset.y;
	if(nType & SYMATERIAL_TEXPARAM_TEXSCALEX	)
		pMat->m_TexArray.ppTex[nTexIndex]->TexScale.x = pTexParam->TexScale.x;
	if(nType & SYMATERIAL_TEXPARAM_TEXSCALEY	)
		pMat->m_TexArray.ppTex[nTexIndex]->TexScale.y = pTexParam->TexScale.y;
	if(nType & SYMATERIAL_TEXPARAM_CENTERX	)
		pMat->m_TexArray.ppTex[nTexIndex]->Center.x = pTexParam->Center.x;
	if(nType & SYMATERIAL_TEXPARAM_CENTERY	)
		pMat->m_TexArray.ppTex[nTexIndex]->Center.y = pTexParam->Center.y;
	if(nType & SYMATERIAL_TEXPARAM_TEXROTATE	)
		pMat->m_TexArray.ppTex[nTexIndex]->TexRotate = pTexParam->TexRotate;
	if(nType & SYMATERIAL_TEXPARAM_AUTORUN	)
		pMat->m_TexArray.ppTex[nTexIndex]->bAutoRun = pTexParam->bAutoRun;
	if(nType & SYMATERIAL_TEXPARAM_SPEED	)
		pMat->m_TexArray.ppTex[nTexIndex]->dSpeed = pTexParam->dSpeed;
	if(nType & SYMATERIAL_TEXPARAM_RUNDIRX	)
		pMat->m_TexArray.ppTex[nTexIndex]->vRunDir.x = pTexParam->vRunDir.x;
	if(nType & SYMATERIAL_TEXPARAM_RUNDIRY	)
		pMat->m_TexArray.ppTex[nTexIndex]->vRunDir.y = pTexParam->vRunDir.y;
	if(nType & SYMATERIAL_TEXPARAM_REPEAT	)
		pMat->m_TexArray.ppTex[nTexIndex]->bRepeat = pTexParam->bRepeat;
	if(nType & SYMATERIAL_TEXPARAM_DISABLE	)
		pMat->m_TexArray.ppTex[nTexIndex]->disable = pTexParam->disable;

	return SYMaterial_DiffuseParamChange << nTexIndex;
}

BASEGRAPH_DATARENDER_API void SYMaterial_PhyRenderDataToPhy( SYPhyData* pData,SYPhyRenderData* pSrcData )
{
	Float4ToDWORDColor(&pData->Diffuse,pSrcData->Diffuse);
	Float4ToDWORDColor(&pData->Ambient,pSrcData->Ambient);
	Float4ToDWORDColor(&pData->Specular,pSrcData->Specular);
	Float4ToDWORDColor(&pData->Emissive,pSrcData->Emissive);
	pData->fAlpha = pSrcData->fAlpha;
	pData->Power = pSrcData->Power;
	pData->disable = pSrcData->disable;
}

BASEGRAPH_DATARENDER_API void		SYMaterial_PhyToPhyRenderData(SYPhyRenderData* pData,SYPhyData* pSrcData)
{
	DWORDColorToFloat4(&pData->Diffuse,pSrcData->Diffuse);
	DWORDColorToFloat4(&pData->Ambient,pSrcData->Ambient);
	DWORDColorToFloat4(&pData->Specular,pSrcData->Specular);
	DWORDColorToFloat4(&pData->Emissive,pSrcData->Emissive);
	pData->fAlpha = pSrcData->fAlpha;
	pData->Power = pSrcData->Power;
	pData->disable = pSrcData->disable;
}

BASEGRAPH_DATARENDER_API int SYMaterial_VideoMatUpdateCopyVideo(SYVideoMaterial* pVideoMat, double dFrame,float fFrameRate,float fAlpha,int nLightNum,SYMaterialParam* param,void* pEnvParam,SYVideoMaterial* pVideoMatSrc)
{
	int nChange = 0;
	if(param || pVideoMat->m_nChangeType & SYMaterial_PhyDataChange)
		nChange |= UpdatePhyParam(dFrame,param,pVideoMat,fAlpha);
	if(pVideoMat->m_nChangeType & SYMaterial_TexCountChange)
	{
		for (int i = 0; i < SYMaterial_TexTypeCount;i++)
		{
			if (pVideoMat->m_pMaterial->m_TexArray.ppTex[i] &&
				pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->disable)
			{
				SYMaterial_ReleaseVideoTex(pVideoMat->m_hDevice,pVideoMat->m_VideoTexAr.ppVTex[i]);
				SAFE_DELETE(pVideoMat->m_VideoTexAr.ppVTex[i]);
			}
			else if (pVideoMat->m_VideoTexAr.ppVTex[i]&& !pVideoMat->m_pMaterial->m_TexArray.ppTex[i])
			{
				SYMaterial_ReleaseVideoTex(pVideoMat->m_hDevice,pVideoMat->m_VideoTexAr.ppVTex[i]);
				SAFE_DELETE(pVideoMat->m_VideoTexAr.ppVTex[i]);
			}
			else if(!pVideoMat->m_VideoTexAr.ppVTex[i]&& pVideoMat->m_pMaterial->m_TexArray.ppTex[i])
			{
				SYMatVideoTex* pTex = new SYMatVideoTex;
				memset(pTex,0,sizeof(SYMatVideoTex));
				pTex->CurTexScale.x = 1;
				pTex->CurTexScale.y = 1;
				pTex->bTexNeedRelease = TRUE;
				pTex->nType = i;
				CreateTexPrivateData(&pTex->pCurData,i);
				pVideoMat->m_VideoTexAr.ppVTex[i] = pTex;

				SYMaterial_SetChangeType(pVideoMat,SYMaterial_DiffTexChange << i | SYMaterial_DiffuseParamChange << i);
			}
		}
	}
	if((pVideoMat->m_nChangeType & SYMaterial_TexCountChange) || (pVideoMat->m_nChangeType & SYMaterial_ShaderChange) || nLightNum != pVideoMat->m_nLightNum)
	{
		SYMaterial_JoinShader(pVideoMat,nLightNum);
		SYMaterial_ClearChangeType(pVideoMat,SYMaterial_TexCountChange | SYMaterial_ShaderChange);
		pVideoMat->m_nLightNum = nLightNum;
		nChange |=  SYMaterial_ShaderChange;
	}
	if (pVideoMat->m_dTime != dFrame)
	{
		pVideoMat->m_dTotalTime += dFrame - pVideoMat->m_dTime;
		//if(UpdatePhyParam(nTime,param,pVideoMat,fAlpha ))
		//	nChange |= SYMaterial_PhyDataChange;
		for(int i = 0; i< SYMaterial_TexTypeCount;i ++)
		{
			if(SYMaterial_UpdateTexCopyVideo(NULL,NULL,NULL,SYMaterial_DiffuseTex + i,dFrame,fFrameRate, pVideoMat,pVideoMatSrc,pEnvParam))
				nChange |=  SYMaterial_DiffTexChange << i;
			if(UpdateTexParam(pVideoMat->m_dTotalTime,dFrame,pVideoMat->m_hDevice,param,pVideoMat->m_VideoTexAr.ppVTex[i],pVideoMat->m_pMaterial->m_TexArray.ppTex[i]))
				nChange |=  SYMaterial_DiffuseParamChange << i;
			SYMaterial_ClearChangeType(pVideoMat,SYMaterial_DiffTexChange << i | SYMaterial_DiffuseParamChange << i);
		}
		pVideoMat->m_dTime = dFrame;
	}
	else
	{
		//pVideoMat->m_dTotalTime += 1;
		for(int i = 0; i < SYMaterial_TexTypeCount; i++)
		{
			if(!pVideoMat->m_pMaterial->m_TexArray.ppTex[i]
			|| pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->disable)
				continue;

			if (pVideoMat->m_nChangeType & SYMaterial_DiffTexChange << i || pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->nSrcType == SYMaterial_MovFile)
			{
				if(SYMaterial_UpdateTexCopyVideo(NULL,NULL,NULL,SYMaterial_DiffuseTex + i,dFrame,fFrameRate,pVideoMat,pVideoMatSrc,pEnvParam))
					nChange |=  SYMaterial_DiffTexChange << i;
				SYMaterial_ClearChangeType(pVideoMat,SYMaterial_DiffTexChange << i);
			}
			if(pVideoMat->m_nChangeType & SYMaterial_DiffuseParamChange << i || pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->bAutoRun)
			{				
				if(UpdateTexParam(pVideoMat->m_dTotalTime,dFrame,pVideoMat->m_hDevice,param,pVideoMat->m_VideoTexAr.ppVTex[i],pVideoMat->m_pMaterial->m_TexArray.ppTex[i]))
					nChange |=  SYMaterial_DiffuseParamChange << i;
				SYMaterial_ClearChangeType(pVideoMat,SYMaterial_DiffuseParamChange << i);
			}
		}
	}
	return nChange;
}

BASEGRAPH_DATARENDER_API BOOL SYMaterial_UpdateTexCopyVideo(SYBufferDef32* pSrcBufferDef, DXHalDef32* pSrcTexDef,int hSrcTex, int nTexIndex,double dTime, float fFrameRate,SYVideoMaterial* pVideoMat,SYVideoMaterial* pVideoMatSrc,void* param,BOOL isForceUpdate /*= FALSE*/)
{
	SYMatTex* pTex = pVideoMat->m_pMaterial->m_TexArray.ppTex[nTexIndex];
	if(!pTex)
		return FALSE;
	SYMatVideoTex* pVideoTex = pVideoMat->m_VideoTexAr.ppVTex[nTexIndex];
	if(!pVideoTex)
		return FALSE;
	if (!pVideoMatSrc)
		return FALSE;
	SYMatVideoTex* pVideoTexSrc = pVideoMatSrc->m_VideoTexAr.ppVTex[nTexIndex];
	if(!pVideoTexSrc)
		return FALSE;

	if(pTex->nSrcType == SYMaterial_Object)
	{
		RECT rtBuffer;
		g_pObjectFun(SYMaterial_GetBufferSize,&rtBuffer,NULL,g_pObjectParam,param);
		if(!pVideoTex->hTex)
		{
			pVideoTex->hTex = DX3DEngine_CreateTexture(pVideoMat->m_hDevice,rtBuffer.right,rtBuffer.bottom,DXTexture_Normal2DTex);
			pVideoTex->bTexNeedRelease = TRUE;
		}
		else
		{
			int nWidth,nHeight;
			DX3DEngine_GetTextureInfo(pVideoMat->m_hDevice,pVideoTex->hTex,&nWidth,&nHeight,NULL);
			if(nWidth < rtBuffer.right || nHeight < rtBuffer.bottom)
			{
				DX3DEngine_ReleaseTexture(pVideoMat->m_hDevice,pVideoTex->hTex);
				pVideoTex->hTex = DX3DEngine_CreateTexture(pVideoMat->m_hDevice,rtBuffer.right,rtBuffer.bottom,DXTexture_Normal2DTex);
				pVideoTex->bTexNeedRelease = TRUE;
			}
		}
		return g_pObjectFun(SYMaterial_GetBuffer,NULL,pVideoTex->hTex,g_pObjectParam,param);
	}
	else if(pTex->nSrcType == SYMaterial_Channel)
	{
		RECT rtBuffer;
		g_pChannelFun(SYMaterial_GetBufferSize,&rtBuffer,NULL,g_pChannelParam,param);
		if(!pVideoTex->hTex)
		{
			pVideoTex->hTex = DX3DEngine_CreateTexture(pVideoMat->m_hDevice,rtBuffer.right,rtBuffer.bottom,DXTexture_Normal2DTex);
			pVideoTex->bTexNeedRelease = TRUE;
		}
		else
		{
			int nWidth,nHeight;
			DX3DEngine_GetTextureInfo(pVideoMat->m_hDevice,pVideoTex->hTex,&nWidth,&nHeight,NULL);
			if(nWidth < rtBuffer.right || nHeight < rtBuffer.bottom)
			{
				DX3DEngine_ReleaseTexture(pVideoMat->m_hDevice,pVideoTex->hTex);
				pVideoTex->hTex = DX3DEngine_CreateTexture(pVideoMat->m_hDevice,rtBuffer.right,rtBuffer.bottom,DXTexture_Normal2DTex);
				pVideoTex->bTexNeedRelease = TRUE;
			}
		}
		return g_pChannelFun(SYMaterial_GetBuffer,NULL,pVideoTex->hTex,g_pChannelParam,param);
	}
	else if(pTex->nSrcType == SYMaterial_PicFile)
	{
		if(!(pVideoMat->m_nChangeType & SYMaterial_DiffTexChange << pTex->nType))
			return FALSE;
		//处理图片
		if(pVideoTex->hTex)
		{
			DX3DEngine_ReleaseTexture(pVideoMat->m_hDevice,pVideoTex->hTex);
			pVideoTex->hTex = 0;
		}

		TCHAR realpath[MAX_PATH];
		//GetFileManager()->TagPathTrans(pTex->pTexPath,realpath,GetFileManager()->lpParam);
		//路径无效时,返回
		if (/*pTex->disable || */!pTex->pTexPath || _taccess(realpath, 0) == -1)
			return FALSE;

		BOOL bMap = pTex->nType == SYMaterial_BumpTex;

		pVideoTex->hTex = DX3DEngine_CreateTextureFromFile(pVideoMat->m_hDevice,realpath,bMap);
#ifdef _DEBUG
		//assert(pVideoTex->hTex > 0);
#endif
		if( pVideoTex->hTex == 0 )
			return FALSE;

		if(0)
		{
			DX3DEngine_DumpTexture(pVideoMat->m_hDevice,pVideoTex->hTex);
		}
		return TRUE;
	}
	else if(pTex->nSrcType == SYMaterial_MovFile)
	{
		if (pVideoTexSrc && pVideoTexSrc->pSeekData)
		{
			if(!pVideoTex->hTex)
			{
				pVideoTex->hTex = DX3DEngine_CreateTexture(pVideoMat->m_hDevice,pVideoTexSrc->pSeekData->nWidth,pVideoTexSrc->pSeekData->nHeight,DXTexture_Normal2DTex);
				pVideoTex->bTexNeedRelease = TRUE;
			}

			if (pVideoTexSrc->hTex)
			{
				DX3DEngine_CopyTexture(pVideoMat->m_hDevice, pVideoTex->hTex, 0, 0, pVideoTexSrc->pSeekData->nWidth, pVideoTexSrc->pSeekData->nHeight, pVideoTexSrc->hTex,
					0, 0, pVideoTexSrc->pSeekData->nWidth, pVideoTexSrc->pSeekData->nHeight);

				if(0)
				{
					DX3DEngine_DumpTexture(pVideoMat->m_hDevice,pVideoTex->hTex);
				}
				return TRUE;
			}
		}
	}
	else if(pTex->nSrcType == SYMaterial_Buffer)
	{
		int nWidth,nHeight;
		int nSrcWidth , nSrcHeight;
		RECT* pBound;

		if(pSrcTexDef)
		{
			nSrcWidth = pSrcTexDef->m_rtBound.right - pSrcTexDef->m_rtBound.left;
			nSrcHeight = pSrcTexDef->m_rtBound.bottom - pSrcTexDef->m_rtBound.top;
			pBound = &pSrcTexDef->m_rtBound;
			if(pVideoTex->hTex)
			{
				DX3DEngine_GetTextureInfo(pVideoMat->m_hDevice,pVideoTex->hTex,&nWidth,&nHeight,NULL);
				if(nWidth < nSrcWidth || nHeight < nSrcHeight)
				{
					DX3DEngine_ReleaseTexture(pVideoMat->m_hDevice,pVideoTex->hTex);
					pVideoTex->hTex = 0;
				}
			}
			if(!pVideoTex->hTex)
			{
				nWidth = nSrcWidth;
				nHeight = nSrcHeight;
				pVideoTex->hTex = DX3DEngine_CreateTexture(pVideoMat->m_hDevice,nWidth,nHeight,DXTexture_MipMap);
				pVideoTex->bTexNeedRelease = TRUE;
			}
			DX3DEngine_CopyTexture(pVideoMat->m_hDevice,pVideoTex->hTex,0,0,nSrcWidth,nSrcHeight,pSrcTexDef->m_hTex,0,0,nSrcWidth,nSrcHeight);
			DX3DEngine_GenerateMip(pVideoMat->m_hDevice,pVideoTex->hTex);
			if (0)
			{
				DX3DEngine_DumpTexture(pVideoMat->m_hDevice,pVideoTex->hTex);
			}
		}
		else
		{
			if(!(pVideoMat->m_nChangeType & SYMaterial_DiffTexChange << pTex->nType))
				return FALSE;
			//add by zhenghang  柱图文字需要修改纹理
			pSrcBufferDef = pTex->pBufferDef;
			if(!pSrcBufferDef)
				return FALSE;
			nSrcWidth = pSrcBufferDef->m_nWidth;
			nSrcHeight = pSrcBufferDef->m_nHeight;
			pBound = &pSrcBufferDef->m_rtBound;
			if(pVideoTex->hTex)
			{
				DX3DEngine_GetTextureInfo(pVideoMat->m_hDevice,pVideoTex->hTex,&nWidth,&nHeight,NULL);
				if(nWidth < pSrcBufferDef->m_nWidth || nHeight < pSrcBufferDef->m_nHeight)
				{
					DX3DEngine_ReleaseTexture(pVideoMat->m_hDevice,pVideoTex->hTex);
					pVideoTex->hTex = 0;
				}
			}
			if(!pVideoTex->hTex)
			{
				nWidth = pSrcBufferDef->m_nWidth;
				nHeight = pSrcBufferDef->m_nHeight;
				pVideoTex->hTex = DX3DEngine_CreateTexture(pVideoMat->m_hDevice,pSrcBufferDef->m_nWidth,pSrcBufferDef->m_nHeight,DXTexture_MipMap);				
			}
			int hTrans = DXTextureManager_QueryTexture(DXTexture_MemTexture,nWidth,nHeight);
			DX3DEngine_UpLoad(pVideoTex->hTex,pVideoMat->m_hDevice,hTrans,0,0,pSrcBufferDef->m_nWidth,pSrcBufferDef->m_nHeight,
				pSrcBufferDef->m_dwBuffer,0,0,pSrcBufferDef->m_nWidth,pSrcBufferDef->m_nHeight,pSrcBufferDef->m_nWidth,pSrcBufferDef->m_nHeight);
			DX3DEngine_GenerateMip(pVideoMat->m_hDevice,pVideoTex->hTex);
			if (0)
			{
				DX3DEngine_DumpTexture(pVideoMat->m_hDevice,pVideoTex->hTex);
			}
			DXTextureManager_UnQueryTexture(hTrans);
		}
		SYMaterial_SetTex(pVideoTex->hTex,*pBound,pSrcBufferDef->m_nOriginX,pSrcBufferDef->m_nOriginY, pVideoMat,nTexIndex);
		pVideoTex->bTexNeedRelease = TRUE;
	}
	else
	{
		//处理视频,缺少解码库
	}
	return FALSE;
}

BASEGRAPH_DATARENDER_API void SYMatTex::Reset()
{
	memset(this, 0, sizeof(SYMatTex));
}

BASEGRAPH_DATARENDER_API void SYPhyData::Reset()
{
	disable = 0;
	Diffuse = 0;
	Ambient = 0;
	Specular = 0;
	Emissive = 0;
	Power = 0;
	fAlpha = 0;
}

BASEGRAPH_DATARENDER_API void SYJoinShader::Reset()
{
	memset(this, 0, sizeof(SYJoinShader));
}

BASEGRAPH_DATARENDER_API void SYMaterial::Reset()
{
	memset(this, 0, sizeof(SYMaterial));
}

BASEGRAPH_DATARENDER_API void SYSpecTexPrivateData::Reset()
{
	fFactor = 1;
}

BASEGRAPH_DATARENDER_API void SYBumpTexPrivateData::Reset()
{
	fFactor = 1;
	bReverse = FALSE;
}
