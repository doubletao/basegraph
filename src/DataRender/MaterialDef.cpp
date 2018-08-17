#include "stdafx.h"
#include "DataRender\MaterialDef.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
BaseGraphMatBufferCallBack g_pObjectFun = NULL;
void*				g_pObjectParam = NULL;
BaseGraphMatBufferCallBack g_pChannelFun = NULL;
void*				g_pChannelParam = NULL;

TCHAR*	gShaderPath = NULL;
char*	g_strShaderMain = NULL;
char*	g_strShaderMainHead = NULL;
int		g_nMainSize = 0;
int		g_nMainHeadSize = 0;
BASEGRAPH_DATARENDER_API void BaseGraphMaterialModuleInit(LPCTSTR pShaderPath,BaseGraphMatBufferCallBack pObject /*= NULL*/,void* pObjParam /*= NULL*/,BaseGraphMatBufferCallBack pChannel /*= NULL*/,void* pChannelParam /*= NULL*/)
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

BASEGRAPH_DATARENDER_API void BaseGraphMaterialModuleRelease()
{
	SAFEDELETE_ARRAY(gShaderPath);
	SAFEDELETE_ARRAY(g_strShaderMain);
	SAFEDELETE_ARRAY(g_strShaderMainHead);
}
void BASEGRAPH_DATARENDER_API BaseGraphMaterial_RemoveShader(BaseGraphMaterial* pMaterial,int nShaderType)
{
	BaseGraphJoinShader* pShader = pMaterial->m_ShaderArray.ppShader[nShaderType];
	if (pShader != NULL)
	{
		SAFEDELETE_ARRAY(pShader->strBody);
		SAFEDELETE_ARRAY(pShader->strHead);
	}
	SAFE_DELETE(pMaterial->m_ShaderArray.ppShader[nShaderType]);
}
void BASEGRAPH_DATARENDER_API BaseGraphMaterial_AddShader(BaseGraphJoinShader* pShader ,int nShaderType)
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
		case BaseGraphMaterial_DiffuseShader:
			_tcscpy_s(HeadFilePath,gShaderPath);
			_tcscat_s(HeadFilePath,strDiffuseHead);

			_tcscpy_s(BodyFilePath,gShaderPath);
			_tcscat_s(BodyFilePath,strDiffuseBody);
			break;
		case BaseGraphMaterial_SpecShader:
			_tcscpy_s(HeadFilePath,gShaderPath);
			_tcscat_s(HeadFilePath,strSpecHead);

			_tcscpy_s(BodyFilePath,gShaderPath);
			_tcscat_s(BodyFilePath,strSpecBody);
			break;
		case BaseGraphMaterial_EnvMapShader:
			_tcscpy_s(HeadFilePath,gShaderPath);
			_tcscat_s(HeadFilePath,strEnvMapHead);

			_tcscpy_s(BodyFilePath,gShaderPath);
			_tcscat_s(BodyFilePath,strEnvMapBody);
			break;
		case BaseGraphMaterial_EmissiveShader:
			_tcscpy_s(HeadFilePath,gShaderPath);
			_tcscat_s(HeadFilePath,strEmissiveHead);

			_tcscpy_s(BodyFilePath,gShaderPath);
			_tcscat_s(BodyFilePath,strEmissiveBody);
			break;

		case BaseGraphMaterial_BumpShader:
			_tcscpy_s(HeadFilePath,gShaderPath);
			_tcscat_s(HeadFilePath,strBumpHead);

			_tcscpy_s(BodyFilePath,gShaderPath);
			_tcscat_s(BodyFilePath,strBumpBody);
			break;

		case BaseGraphMaterial_LightShader:
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
void BASEGRAPH_DATARENDER_API BaseGraphMaterial_Init( BaseGraphMaterial* pMaterial,DWORD InitColor,BOOL bLight)
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

	memset(&pMaterial->m_ShaderArray,0,sizeof(BaseGraphShaderArray));
	memset(&pMaterial->m_TexArray,0,sizeof(BaseGraphMatTexArray));
	//添加Shader
	if (bLight)
	{
		BaseGraphJoinShader* pShder = new BaseGraphJoinShader;
		BaseGraphMaterial_AddShader(pShder,BaseGraphMaterial_LightShader);
		ASSERT(pMaterial->m_ShaderArray.ppShader[BaseGraphMaterial_LightShader]==NULL);
		pMaterial->m_ShaderArray.ppShader[BaseGraphMaterial_LightShader] = pShder;
	}
}

void ReleaseTexPrivateData(void*& pData,int nType)
{
	if (nType == BaseGraphMaterial_SpecTex)
	{
		BaseGraphSpecTexPrivateData* pPrivate = (BaseGraphSpecTexPrivateData* )pData;
		SAFE_DELETE(pPrivate);
	}
	else if (nType == BaseGraphMaterial_EnvMapTex)
	{
		BaseGraphEnvMapTexPrivateData* pPrivate = (BaseGraphEnvMapTexPrivateData* )pData;
		SAFE_DELETE(pPrivate);
	}
	else if(nType == BaseGraphMaterial_EmissiveTex)
	{
		BaseGraphEmissiveTexPrivateData* pPrivate = (BaseGraphEmissiveTexPrivateData* )pData;
		SAFE_DELETE(pPrivate);
	}
	else if(nType == BaseGraphMaterial_BumpTex)
	{
		BumpTexPrivateData* pPrivate = (BumpTexPrivateData* )pData;
		SAFE_DELETE(pPrivate);
	}
	pData = NULL;
}

void CreateTexPrivateData(void** ppData,int nType,void* pSrcData = NULL)
{
	if (nType == BaseGraphMaterial_SpecTex)
	{
		if(!*ppData)
		{
			*ppData = new BaseGraphSpecTexPrivateData;
		}
		if(!pSrcData)
		{
			((BaseGraphSpecTexPrivateData*)*ppData)->fFactor = 1;
		}
		else
			memcpy(*ppData,pSrcData,sizeof(BaseGraphSpecTexPrivateData));
	}
	else if (nType == BaseGraphMaterial_EnvMapTex)
	{
		if(!*ppData)
		{
			*ppData = new BaseGraphEnvMapTexPrivateData;
		}
		if(!pSrcData)
		{
			((BaseGraphEnvMapTexPrivateData*)*ppData)->fFactor = 1;
		}
		else
			memcpy(*ppData,pSrcData,sizeof(BaseGraphEnvMapTexPrivateData));
	}
	else if(nType == BaseGraphMaterial_EmissiveTex)
	{
		if(!*ppData)
		{
			*ppData = new BaseGraphEmissiveTexPrivateData;
		}
		if(!pSrcData)
		{
			((BaseGraphEmissiveTexPrivateData*)*ppData)->fFactor = 1;
		}
		else
			memcpy(*ppData,pSrcData,sizeof(BaseGraphEmissiveTexPrivateData));
	}
	else if(nType == BaseGraphMaterial_BumpTex)
	{
		if(!*ppData)
		{
			*ppData = new BumpTexPrivateData;
		}
		if(!pSrcData)
		{	
			((BumpTexPrivateData*)*ppData)->fFactor = 1;
			((BumpTexPrivateData*)*ppData)->bReverse = 1;
		}
		else
			memcpy(*ppData,pSrcData,sizeof(BumpTexPrivateData));
	}
}
void BaseGraphMaterial_ReleaseTexSrc(BaseGraphMatTex* pTex)
{
	switch(pTex->nSrcType)
	{
	case BaseGraphMaterial_PicFile:
	case BaseGraphMaterial_MovFile:
		SAFEDELETE_ARRAY(pTex->pTexPath);
		break;
	case BaseGraphMaterial_Object:
		SAFEDELETE_ARRAY(pTex->pTexSrc);
		break;
	case BaseGraphMaterial_Buffer:
		if(pTex->pBufferDef)
		{
			SAFEDELETE_ARRAY(pTex->pBufferDef->m_dwBuffer);
			SAFE_DELETE(pTex->pBufferDef);
		}
		break;
	}
}
void BaseGraphMaterial_CreateTexSrc(BaseGraphMatTex* pTex,int nSrcType,void* pData)
{
	pTex->nSrcType = nSrcType;
	size_t nLen;
	switch(pTex->nSrcType)
	{
	case BaseGraphMaterial_PicFile:
	case BaseGraphMaterial_MovFile:
		{
			TCHAR* pPath = (TCHAR*)pData;
			nLen = _tcsclen(pPath) + 1;
			pTex->pTexPath = new TCHAR[nLen];
			_tcscpy_s(pTex->pTexPath,nLen,pPath);
		}
		break;
	case BaseGraphMaterial_Object:
		{
			TCHAR* pObject = (TCHAR*)pData;
			nLen = _tcsclen(pObject) + 1;
			pTex->pTexSrc = new TCHAR[nLen];
			_tcscpy_s(pTex->pTexSrc,nLen,pObject);
		}
		break;
	case BaseGraphMaterial_Buffer:
		{
			CommonBufferDef32* pBuffer = (CommonBufferDef32*)pData;
			if(pBuffer && pBuffer->m_dwBuffer)
			{
				pTex->pBufferDef = new CommonBufferDef32;
				memcpy(pTex->pBufferDef,pBuffer,sizeof(CommonBufferDef32));
				pTex->pBufferDef->m_dwBuffer = new DWORD[pBuffer->m_nHeight * pBuffer->m_nWidth];
				memcpy(pTex->pBufferDef->m_dwBuffer,pBuffer->m_dwBuffer,pBuffer->m_nWidth * pBuffer->m_nHeight * sizeof(DWORD));
			}
		}
		break;
	}
}

void BaseGraphMaterial_ReleaseTex(BaseGraphMatTex* pTex)
{
	BaseGraphMaterial_ReleaseTexSrc(pTex);
	ReleaseTexPrivateData(pTex->pData,pTex->nType);
}
void BASEGRAPH_DATARENDER_API BaseGraphMaterial_Release( BaseGraphMaterial* pMaterial )
{
	if (pMaterial == NULL)
		return;
	for (int i = 0; i < BaseGraphMaterial_TexTypeCount;i++)
	{
		BaseGraphMatTex* pTex = pMaterial->m_TexArray.ppTex[i];
		if(pTex)
		{
			BaseGraphMaterial_ReleaseTex(pTex); 
		}
		SAFE_DELETE(pMaterial->m_TexArray.ppTex[i]);
	}
	for (int i = 0; i < BaseGraphMaterial_ShaderTypeCount;i++)
	{
		BaseGraphJoinShader* pShader = pMaterial->m_ShaderArray.ppShader[i];
		if(pShader)
		{
			SAFEDELETE_ARRAY(pShader->strHead);
			SAFEDELETE_ARRAY(pShader->strBody);

			SAFE_DELETE(pMaterial->m_ShaderArray.ppShader[i]);
		}
	}
}

void BaseGraphMaterial_CopyTex(BaseGraphMatTex* pDesTex,BaseGraphMatTex* pSrcTex)
{
	ReleaseTexPrivateData(pDesTex->pData,pDesTex->nType);
	BaseGraphMaterial_ReleaseTexSrc(pDesTex);

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
	case BaseGraphMaterial_PicFile:
	case BaseGraphMaterial_MovFile:
		{
			BaseGraphMaterial_CreateTexSrc(pDesTex,pSrcTex->nSrcType,pSrcTex->pTexPath);
		}
		break;
	case BaseGraphMaterial_Object:
		{
			BaseGraphMaterial_CreateTexSrc(pDesTex,pSrcTex->nSrcType,pSrcTex->pTexSrc);
		}
		break;
	case BaseGraphMaterial_Buffer:
		{
			BaseGraphMaterial_CreateTexSrc(pDesTex,pSrcTex->nSrcType,pSrcTex->pBufferDef);
		}
		break;
	}

	CreateTexPrivateData(&pDesTex->pData,pDesTex->nType,pSrcTex->pData);
}

BASEGRAPH_DATARENDER_API int BaseGraphMaterial_Copy( OUT BaseGraphMaterial* pDesMaterial, IN const BaseGraphMaterial* pSrcMaterial )
{
	pDesMaterial->m_nVersion = pSrcMaterial->m_nVersion;
	int nChangeType = BaseGraphMaterial_NoChange;
	memcpy(&pDesMaterial->m_PhyData,&pSrcMaterial->m_PhyData,sizeof(BaseGraphPhyData));
	nChangeType |= BaseGraphMaterial_PhyDataChange;

	for (int i = 0; i < BaseGraphMaterial_TexTypeCount;i++)
	{
		BaseGraphMatTex* pDesTex = pDesMaterial->m_TexArray.ppTex[i];
		BaseGraphMatTex* pSrcTex = pSrcMaterial->m_TexArray.ppTex[i];
		if(pDesTex == NULL && pSrcTex == NULL)
			continue;
		else if(pDesTex != NULL && pSrcTex != NULL)
		{
			int nTexParamChange = BaseGraphMaterial_DiffuseParamChange << i;
			nChangeType |= nTexParamChange;
			int nTexChange = BaseGraphMaterial_DiffTexChange << i;
			nChangeType |= nTexParamChange;
			BaseGraphMaterial_CopyTex(pDesTex,pSrcTex);
		}
		else
		{
			nChangeType |= BaseGraphMaterial_TexCountChange;
			if (pDesTex)
			{
				BaseGraphMaterial_ReleaseTex(pDesTex);
				SAFE_DELETE(pDesTex);
				pDesMaterial->m_TexArray.ppTex[i] = NULL;
			}

			if(pSrcTex)
			{
				pDesTex = new BaseGraphMatTex;
				pDesMaterial->m_TexArray.ppTex[i] = pDesTex;
				memset(pDesTex,0,sizeof(BaseGraphMatTex));
				BaseGraphMaterial_CopyTex(pDesTex,pSrcTex);
			}
		}
	}

	for (int i = 0; i < BaseGraphMaterial_TexTypeCount + 1;i++)
	{
		BaseGraphJoinShader* pDesShader = pDesMaterial->m_ShaderArray.ppShader[i];
		BaseGraphJoinShader* pSrcShader = pSrcMaterial->m_ShaderArray.ppShader[i];
		if (pDesShader == NULL && pSrcShader == NULL)
			continue;
		else
		{
			nChangeType |= BaseGraphMaterial_ShaderChange;
   			if (pDesShader)
			{
				SAFEDELETE_ARRAY(pDesShader->strHead);
				SAFEDELETE_ARRAY(pDesShader->strBody);
				SAFE_DELETE(pDesShader);
				pDesMaterial->m_ShaderArray.ppShader[i] = NULL;
			}

			if(pSrcShader)
			{
				pDesShader = new BaseGraphJoinShader;
				memset(pDesShader,0,sizeof(BaseGraphJoinShader));
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

BASEGRAPH_DATARENDER_API BOOL BaseGraphMaterial_Compare( const BaseGraphMaterial* pMaterial1, const BaseGraphMaterial* pMaterial2 )
{
	if (pMaterial1 == NULL && pMaterial2 == NULL)
		return TRUE;

	if (pMaterial1 == NULL || pMaterial2 == NULL)
		return FALSE;

	if (pMaterial1->nMaterialType != pMaterial2->nMaterialType)
	{
		return FALSE;
	}
	if (memcmp(&pMaterial1->m_PhyData, &pMaterial2->m_PhyData, sizeof(BaseGraphPhyData)) != 0)
	{
		return FALSE;
	}
	for (int i = 0; i < BaseGraphMaterial_TexTypeCount;i++)
	{
		BaseGraphMatTex* pMatTex1 = pMaterial1->m_TexArray.ppTex[i];
		BaseGraphMatTex* pMatTex2 = pMaterial2->m_TexArray.ppTex[i];
		if (!BaseGraphMatTex_Compare(pMatTex1, pMatTex2))
		{
			return FALSE;
		}
	}
	for (int i = 0; i < BaseGraphMaterial_ShaderTypeCount;i++)
	{
		BaseGraphJoinShader* pShader1 = pMaterial1->m_ShaderArray.ppShader[i];
		BaseGraphJoinShader* pShader2 = pMaterial2->m_ShaderArray.ppShader[i];
		if (!BaseGraphJoinShader_Compare(pShader1, pShader2))
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

BASEGRAPH_DATARENDER_API BOOL	BaseGraphMatTex_Compare( const BaseGraphMatTex* pMatTex1, const BaseGraphMatTex* pMatTex2)
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
	case BaseGraphMaterial_PicFile:
	case BaseGraphMaterial_MovFile:
		{
			if (_tcscmp(pMatTex1->pTexPath, pMatTex2->pTexPath) != 0)
			{
				return FALSE;
			}
		}
		break;
	case BaseGraphMaterial_Object:
		{
			if (_tcscmp(pMatTex1->pTexSrc, pMatTex2->pTexSrc) != 0)
			{
				return FALSE;
			}
		}
		break;
	case BaseGraphMaterial_Buffer:
		{
			if (!CommonBufferDef32_Compare(pMatTex1->pBufferDef, pMatTex2->pBufferDef))
			{
				return FALSE;
			}
		}
		break;
	}

	switch(pMatTex1->nSrcType)
	{
	case BaseGraphMaterial_SpecTex:
		{
			BaseGraphSpecTexPrivateData *pData1 = (BaseGraphSpecTexPrivateData*)pMatTex1->pData;
			BaseGraphSpecTexPrivateData *pData2 = (BaseGraphSpecTexPrivateData*)pMatTex2->pData;
			if (pData1->fFactor != pData2->fFactor)
			{
				return FALSE;
			}
		}
		break;
	case BaseGraphMaterial_EnvMapTex:
		{
			BaseGraphEnvMapTexPrivateData *pData1 = (BaseGraphEnvMapTexPrivateData*)pMatTex1->pData;
			BaseGraphEnvMapTexPrivateData *pData2 = (BaseGraphEnvMapTexPrivateData*)pMatTex2->pData;
			if (pData1->fFactor != pData2->fFactor)
			{
				return FALSE;
			}
		}
		break;
	case BaseGraphMaterial_EmissiveTex:
		{
			BaseGraphEmissiveTexPrivateData *pData1 = (BaseGraphEmissiveTexPrivateData*)pMatTex1->pData;
			BaseGraphEmissiveTexPrivateData *pData2 = (BaseGraphEmissiveTexPrivateData*)pMatTex2->pData;
			if (pData1->fFactor != pData2->fFactor)
			{
				return FALSE;
			}
		}
		break;
	case BaseGraphMaterial_BumpTex:
		{
			BumpTexPrivateData *pData1 = (BumpTexPrivateData*)pMatTex1->pData;
			BumpTexPrivateData *pData2 = (BumpTexPrivateData*)pMatTex2->pData;
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

BASEGRAPH_DATARENDER_API BOOL	BaseGraphJoinShader_Compare( const BaseGraphJoinShader* pShader1, const BaseGraphJoinShader* pShader2)
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

//void BaseGraphMaterial_AddTexPrivateKeyValue(double dFrame,param_def* param,BaseGraphMatTex* pTex,void* pValue)
//{
//	switch(pTex->nType)
//	{
//	case BaseGraphMaterial_SpecTex:
//	case BaseGraphMaterial_EnvMapTex:
//	case BaseGraphMaterial_EmissiveTex:
//	case BaseGraphMaterial_BumpTex:
//		{
//			float* pData = (float*)pValue;
//			float fValue = *pData - *(float*)pTex->pData;
//			AddClipKeyFrame(param,dFrame,(BYTE*)&fValue);
//		}
//		break;
//	}
//}

//void BaseGraphMaterial_AddTexKeyValue(double dFrame,BaseARray* pArTexParam,BaseGraphMatTex* pTex,void* pValue,int nType)
//{
//	param_def* param = (param_def*)BaseAR_GetAt(pArTexParam,nType);
//	switch(nType)
//	{
//	case BaseGraphTex_Offset_ParamDef:
//		{
//			float2* pData = (float2*)pValue;
//			float2 offset = *pData - pTex->TexOffset;
//			AddClipKeyFrame(param,dFrame,(BYTE*)&offset);
//		}
//		break;
//	case BaseGraphTex_Scale_ParamDef:
//		{
//			float2* pData = (float2*)pValue;
//			float2 offset = *pData;
//			offset.x /= pTex->TexScale.x;
//			offset.y /= pTex->TexScale.y;
//			AddClipKeyFrame(param,dFrame,(BYTE*)&offset);
//		}
//		break;
//	case BaseGraphTex_Center_ParamDef:
//		{
//			float2* pData = (float2*)pValue;
//			float2 offset = *pData - pTex->Center;
//			AddClipKeyFrame(param,dFrame,(BYTE*)&offset);
//		}
//		break;
//	case BaseGraphTex_Rotate_ParamDef:
//		{
//			float* pRotate = (float*)pValue;
//			float rotate =  *pRotate - pTex->TexRotate;
//			AddClipKeyFrame(param,dFrame,(BYTE*)&rotate);
//		}
//		break;
//	case BaseGraphTex_Private_ParamDef:
//		BaseGraphMaterial_AddTexPrivateKeyValue(dFrame,param,pTex,pValue);
//		break;
//	}
//}

//void BaseGraphMaterial_AddPrivateParamdef(BaseARray* pArTexParam ,int nTexType)
//{
//	if(nTexType == BaseGraphMaterial_SpecTex)
//	{
//		param_def* pparamdefFactor = new param_def;
//		sy_init_paramdef(pparamdefFactor,sizeof(float),NULL);
//		pparamdefFactor->interpolate.nRange             = 1;
//		pparamdefFactor->interpolate.pRanges            = new PAIR[1];
//		pparamdefFactor->interpolate.pRanges[0].fFirst  = 0.f;
//		pparamdefFactor->interpolate.pRanges[0].fSecond = 1.f;
//		BaseAR_Add( pArTexParam, pparamdefFactor );
//	}
//	else if(nTexType == BaseGraphMaterial_EnvMapTex)
//	{
//		param_def* pparamdefFactor = new param_def;
//		sy_init_paramdef(pparamdefFactor,sizeof(float),NULL);
//		pparamdefFactor->interpolate.nRange             = 1;
//		pparamdefFactor->interpolate.pRanges            = new PAIR[1];
//		pparamdefFactor->interpolate.pRanges[0].fFirst  = 0.f;
//		pparamdefFactor->interpolate.pRanges[0].fSecond = 1.f;
//		BaseAR_Add( pArTexParam, pparamdefFactor );
//	}
//	else if(nTexType == BaseGraphMaterial_EmissiveTex)
//	{
//		param_def* pparamdefFactor = new param_def;
//		sy_init_paramdef(pparamdefFactor,sizeof(float),NULL);
//		pparamdefFactor->interpolate.nRange             = 1;
//		pparamdefFactor->interpolate.pRanges            = new PAIR[1];
//		pparamdefFactor->interpolate.pRanges[0].fFirst  = 0.f;
//		pparamdefFactor->interpolate.pRanges[0].fSecond = 1.f;
//		BaseAR_Add( pArTexParam, pparamdefFactor );
//	}
//	else if(nTexType == BaseGraphMaterial_BumpTex)
//	{
//		param_def* pparamdefFactor = new param_def;
//		sy_init_paramdef(pparamdefFactor,sizeof(float),NULL);
//		pparamdefFactor->interpolate.nRange             = 1;
//		pparamdefFactor->interpolate.pRanges            = new PAIR[1];
//		pparamdefFactor->interpolate.pRanges[0].fFirst  = 0.f;
//		pparamdefFactor->interpolate.pRanges[0].fSecond = 1.f;
//		BaseAR_Add( pArTexParam, pparamdefFactor );
//	}
//}
//void BaseGraphMaterial_AddParamdef(BaseARray* pArTexParam ,int nTexType)
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
//	BaseAR_Add( pArTexParam, pparamdef_offset );
//
//	param_def* pparamdef_scale = new param_def;
//	sy_init_paramdef(pparamdef_scale,sizeof(float2),(void*)&vec1);
//	pparamdef_scale->interpolate.nRange             = 2;
//	pparamdef_scale->interpolate.pRanges            = new PAIR[2];
//	pparamdef_scale->interpolate.pRanges[0].fFirst  = 0.f;
//	pparamdef_scale->interpolate.pRanges[0].fSecond = 1.f;
//	pparamdef_scale->interpolate.pRanges[1].fFirst  = 0.f;
//	pparamdef_scale->interpolate.pRanges[1].fSecond = 1.f;
//	BaseAR_Add( pArTexParam, pparamdef_scale );
//
//	param_def* pparamdef_center = new param_def;
//	sy_init_paramdef(pparamdef_center,sizeof(float2),(void*)&vec);
//	pparamdef_center->interpolate.nRange             = 2;
//	pparamdef_center->interpolate.pRanges            = new PAIR[2];
//	pparamdef_center->interpolate.pRanges[0].fFirst  = 0.f;
//	pparamdef_center->interpolate.pRanges[0].fSecond = 1.f;
//	pparamdef_center->interpolate.pRanges[1].fFirst  = 0.f;
//	pparamdef_center->interpolate.pRanges[1].fSecond = 1.f;
//	BaseAR_Add( pArTexParam, pparamdef_center );
//
//	param_def* pparamdefrotate = new param_def;
//	sy_init_paramdef(pparamdefrotate,sizeof(float),NULL);
//	pparamdefrotate->interpolate.nRange             = 1;
//	pparamdefrotate->interpolate.pRanges            = new PAIR[1];
//	pparamdefrotate->interpolate.pRanges[0].fFirst  = 0.f;
//	pparamdefrotate->interpolate.pRanges[0].fSecond = 1.f;
//	BaseAR_Add( pArTexParam, pparamdefrotate );
//
//	BaseGraphMaterial_AddPrivateParamdef(pArTexParam ,nTexType);
//}

BASEGRAPH_DATARENDER_API int BaseGraphMaterial_AddTex(BaseGraphMaterial* pMaterial,int nTexType)
{
	if (pMaterial->m_TexArray.ppTex[nTexType])
	{
		//添加Shader
		if (pMaterial->m_ShaderArray.ppShader[nTexType] == NULL)
		{
			BaseGraphJoinShader* pShder = new BaseGraphJoinShader;
			BaseGraphMaterial_AddShader(pShder,nTexType);
			pMaterial->m_ShaderArray.ppShader[nTexType] = pShder;
			return BaseGraphMaterial_TexCountChange;
		}
		return BaseGraphMaterial_NoChange;
	}
	BaseGraphMatTex* pTex = new BaseGraphMatTex;
	memset(pTex,0,sizeof(BaseGraphMatTex));
	pTex->nType = nTexType;
	pTex->bRepeat = TRUE;
	pTex->TexScale.x = pTex->TexScale.y = 1.f;
	CreateTexPrivateData(&pTex->pData,nTexType);
	pMaterial->m_TexArray.ppTex[nTexType] = pTex;

	//添加Shader
	BaseGraphJoinShader* pShder = new BaseGraphJoinShader;
	BaseGraphMaterial_AddShader(pShder,nTexType);
	pMaterial->m_ShaderArray.ppShader[nTexType] = pShder;

	return BaseGraphMaterial_TexCountChange;
}

BASEGRAPH_DATARENDER_API int BaseGraphMaterial_RemoveTex( BaseGraphMaterial* pMaterial,int nTexType )
{
	if (!pMaterial->m_TexArray.ppTex[nTexType])
		return BaseGraphMaterial_NoChange;
	BaseGraphMaterial_ReleaseTex(pMaterial->m_TexArray.ppTex[nTexType]);
	SAFE_DELETE(pMaterial->m_TexArray.ppTex[nTexType]);

	//删除Shader
	BaseGraphMaterial_RemoveShader(pMaterial,nTexType);
	return BaseGraphMaterial_TexCountChange;
}

BASEGRAPH_DATARENDER_API void* BaseGraphMaterial_GetTexSrc( BaseGraphMaterial* pMaterial,int nTexType )
{
	if (!pMaterial->m_TexArray.ppTex[nTexType])
		return NULL;
	return pMaterial->m_TexArray.ppTex[nTexType]->pTexPath;
}

BASEGRAPH_DATARENDER_API int BaseGraphMaterial_GetTexSrcType( BaseGraphMaterial* pMaterial,int nIndex )
{
	if (!pMaterial->m_TexArray.ppTex[nIndex])
		return BaseGraphMaterial_Invid;
	return pMaterial->m_TexArray.ppTex[nIndex]->nSrcType;

}

BASEGRAPH_DATARENDER_API int BaseGraphMaterial_SetTexSrc( BaseGraphMaterial* pMat,int nTexIndex,int nSrcType,void* pData )
{
	BaseGraphMatTex* pTex = pMat->m_TexArray.ppTex[nTexIndex];
	if (!pTex)
		return BaseGraphMaterial_NoChange;
	BaseGraphMaterial_ReleaseTexSrc(pTex);
	BaseGraphMaterial_CreateTexSrc(pTex,nSrcType,pData);
	return BaseGraphMaterial_DiffTexChange << nTexIndex;
}

BASEGRAPH_DATARENDER_API int BaseGraphMaterial_SetTexOffset( BaseGraphMaterial* pMat,int nTexIndex,float2* pOffset )
{
	BaseGraphMatTex* pTex = pMat->m_TexArray.ppTex[nTexIndex];
	if (!pTex)
		return BaseGraphMaterial_NoChange;
	pTex->TexOffset = *pOffset;
	return BaseGraphMaterial_DiffuseParamChange << nTexIndex;
}

BASEGRAPH_DATARENDER_API int BaseGraphMaterial_SetTexScale( BaseGraphMaterial* pMat,int nTexIndex,float2* pScale )
{
	BaseGraphMatTex* pTex = pMat->m_TexArray.ppTex[nTexIndex];
	if (!pTex)
		return BaseGraphMaterial_NoChange;
	pTex->TexScale = *pScale;
	return BaseGraphMaterial_DiffuseParamChange << nTexIndex;

}

BASEGRAPH_DATARENDER_API int BaseGraphMaterial_SetTexCenter( BaseGraphMaterial* pMat,int nTexIndex,float2* pCenter )
{
	BaseGraphMatTex* pTex = pMat->m_TexArray.ppTex[nTexIndex];
	if (!pTex)
		return BaseGraphMaterial_NoChange;
	pTex->Center = *pCenter;
	return BaseGraphMaterial_DiffuseParamChange << nTexIndex;
}

BASEGRAPH_DATARENDER_API int BaseGraphMaterial_SetTexRotate( BaseGraphMaterial* pMat,int nTexIndex,float fRotate )
{
	BaseGraphMatTex* pTex = pMat->m_TexArray.ppTex[nTexIndex];
	if (!pTex)
		return BaseGraphMaterial_NoChange;
	pTex->TexRotate = fRotate;
	return BaseGraphMaterial_DiffuseParamChange << nTexIndex;

}

BASEGRAPH_DATARENDER_API int BaseGraphMaterial_SetTexPrivateData( BaseGraphMaterial* pMat,int nTexIndex,void* pData )
{
	BaseGraphMatTex* pTex = pMat->m_TexArray.ppTex[nTexIndex];
	if (!pTex)
		return BaseGraphMaterial_NoChange;
	switch(pTex->nType)
	{
	case BaseGraphMaterial_SpecTex:
	case BaseGraphMaterial_EnvMapTex:
	case BaseGraphMaterial_EmissiveTex:
		{
			BaseGraphSpecTexPrivateData* pValue = (BaseGraphSpecTexPrivateData*)pData;
			*(BaseGraphSpecTexPrivateData*)pTex->pData = *pValue;
		}
		break;
	case BaseGraphMaterial_BumpTex:
		{
			BumpTexPrivateData* pValue = (BumpTexPrivateData*)pData;
			*(BumpTexPrivateData*)pTex->pData = *pValue;
		}
		break;
	}
	return BaseGraphMaterial_DiffuseParamChange << nTexIndex;
}

BASEGRAPH_DATARENDER_API int BaseGraphMaterial_SetPhyColor( BaseGraphMaterial* pMat,DWORD color )
{
	pMat->m_PhyData.Diffuse = color;
	pMat->m_PhyData.Ambient = color;
	pMat->m_PhyData.Specular = 0xffffffff;
	return BaseGraphMaterial_PhyDataChange;
}

BASEGRAPH_DATARENDER_API int		BaseGraphMaterial_SetPhyParam(BaseGraphMaterial* pMat,const BaseGraphPhyRenderData* pData)
{
	Float4ToDWORDColor(&pMat->m_PhyData.Diffuse,pData->Diffuse);
	Float4ToDWORDColor(&pMat->m_PhyData.Ambient,pData->Ambient);
	Float4ToDWORDColor(&pMat->m_PhyData.Specular,pData->Specular);
	Float4ToDWORDColor(&pMat->m_PhyData.Emissive,pData->Emissive);
	return BaseGraphMaterial_PhyDataChange;
}
BASEGRAPH_DATARENDER_API int BaseGraphMaterial_SetPhyParam( BaseGraphMaterial* pMat,const BaseGraphPhyData* pData )
{
	memcpy(&pMat->m_PhyData,pData,sizeof(BaseGraphPhyData));
	return BaseGraphMaterial_PhyDataChange;
}

BASEGRAPH_DATARENDER_API void BaseGraphMaterial_SetChangeType(BaseGraphVideoMaterial* pVideoMat,int nChangeType)
{
	if(pVideoMat)
		pVideoMat->m_nChangeType |= nChangeType;
}
BASEGRAPH_DATARENDER_API void BaseGraphMaterial_ClearAllChangeType(BaseGraphVideoMaterial* pVideoMat,int nChangeType = 0)
{
	if(pVideoMat)
		pVideoMat->m_nChangeType = nChangeType;
}
BASEGRAPH_DATARENDER_API void BaseGraphMaterial_ClearChangeType(BaseGraphVideoMaterial* pVideoMat,int nChangeType)
{
	if(pVideoMat)
		pVideoMat->m_nChangeType &= ~nChangeType;
}

BASEGRAPH_DATARENDER_API void BaseGraphMaterial_CreateVideoMat( BaseGraphVideoMaterial* pVideoMat,BaseGraphMaterial* pMaterial ,int hDevice,BOOL bEdit)
{
	memset(pVideoMat,0,sizeof(BaseGraphVideoMaterial));
	pVideoMat->m_hDevice = hDevice;
	//BaseGraphMaterial_PhyToPhyRenderData(&pVideoMat->m_PhyData,&pMaterial->m_PhyData);
	pVideoMat->m_bEdit = bEdit;
	pVideoMat->m_hPhyCB = NULL;//DX3DEngine_CreateBuffer(pVideoMat->m_hDevice,sizeof(float4),5,NULL,DXConstBuffer);
	pVideoMat->m_dTime = 0;
	pVideoMat->m_dTotalTime = 0;
	pVideoMat->m_pShaderStr = NULL;
	pVideoMat->m_pMaterial = pMaterial;
	BaseGraphMaterial_ClearAllChangeType(pVideoMat,BaseGraphMaterial_ALLChange);

	memset(&pVideoMat->m_VideoTexAr,0,sizeof(BaseGraphMatVideoTexArray));
}

void BaseGraphMaterial_SetStaticPhyParam(BaseGraphVideoMaterial* pMat,BaseGraphPhyRenderData* pData,int nType)
{
	if(nType & BaseGraphMATERIAL_DIFFUSE_COLOR)
		pMat->m_PhyDataStatic.Diffuse = pData->Diffuse;
	if(nType & BaseGraphMATERIAL_SPECTURE_COLOR)
		pMat->m_PhyDataStatic.Specular = pData->Specular;
	if(nType & BaseGraphMATERIAL_AMBEINT_COLOR	)	
		pMat->m_PhyDataStatic.Ambient = pData->Ambient;
	if(nType & BaseGraphMATERIAL_EMISSIVE_COLOR)		
		pMat->m_PhyDataStatic.Emissive = pData->Emissive;
	if(nType & BaseGraphMATERIAL_FACTOR)	
		pMat->m_PhyDataStatic.Power = pData->Power;
	if(nType & BaseGraphMATERIAL_ALPHA	)
		pMat->m_PhyDataStatic.fAlpha = pData->fAlpha;
	if(nType & BaseGraphMATERIAL_DISABLE	)
		pMat->m_PhyDataStatic.disable = pData->disable;
	BaseGraphMaterial_SetChangeType(pMat, BaseGraphMaterial_PhyDataChange);
}
void BaseGraphMaterial_ReleaseVideoTex(int hDevice,BaseGraphMatVideoTex* pTex,BOOL bReleaseParam = TRUE)
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
	//		BaseGraphMovieClose(pTex->pSeekData->pVideoSeekData);
	//		pTex->pSeekData->pVideoSeekData = NULL;
	//		SAFE_DELETE(pTex->pSeekData);
	//	}
	//	ReleaseTexPrivateData(pTex->pCurData,pTex->nType);
	//	pTex->pCurData = NULL;
	//}
}

BASEGRAPH_DATARENDER_API void BaseGraphMaterial_ReleaseVideoMat( BaseGraphVideoMaterial* pVideoMat )
{
	if(!pVideoMat)
		return;
	DX3DEngine_ReleaseBuffer(pVideoMat->m_hDevice,pVideoMat->m_hPhyCB);
	SAFEDELETE_ARRAY(pVideoMat->m_pShaderStr);
	delete[] pVideoMat->m_pShaderName;
	pVideoMat->m_pMaterial = NULL;
	
	for (int i = 0; i < BaseGraphMaterial_TexTypeCount;i++)
	{
		BaseGraphMatVideoTex* pTex = pVideoMat->m_VideoTexAr.ppVTex[i];
		BaseGraphMaterial_ReleaseVideoTex(pVideoMat->m_hDevice,pTex);
		SAFE_DELETE(pVideoMat->m_VideoTexAr.ppVTex[i]);
	}
}

const char* g_NOlightProcess = "fDiffContrib = DiffuseRef; //fSpecContrib = SpeculRef; fEmissiveContrib = Emissive;\n";
const int g_NoLightLen = 86;

int BaseGraphMaterial_GetSubShaderSize(BaseGraphMaterial* pMat)
{
	size_t nSize = 0;

	for (int i = 0; i <BaseGraphMaterial_ShaderTypeCount;i++)
	{
		if (!pMat->m_ShaderArray.ppShader[i])
			continue;
		nSize += strlen(pMat->m_ShaderArray.ppShader[i]->strHead);
		nSize += strlen(pMat->m_ShaderArray.ppShader[i]->strBody);
	}
	nSize += g_NoLightLen;
	return (int)nSize;
}

const TCHAR* gSubShaderName[BaseGraphMaterial_ShaderTypeCount] =
{
	_T("diffuse_"),		//材质
	_T("spec_"),		//高光
	_T("envmap_"),		//环境
	_T("emissive_"),	//光影
	_T("bump_"),		//凹凸
	_T("light_"),
};
void BaseGraphMaterial_JoinShader(BaseGraphVideoMaterial* pVideoMat,int nLightNum)
{
	nLightNum = max(0,nLightNum);

	int nShaderSize = g_nMainHeadSize + g_nMainSize + BaseGraphMaterial_GetSubShaderSize(pVideoMat->m_pMaterial) + 1;
	SAFEDELETE_ARRAY(pVideoMat->m_pShaderStr);
	pVideoMat->m_pShaderStr = new char[nShaderSize];
	pVideoMat->m_pShaderStr[0] = 0;
	TCHAR strShaderName[60];
	strShaderName[0] = 0;
	//插入shader的头部分
	strcpy_s(pVideoMat->m_pShaderStr,nShaderSize,g_strShaderMainHead);
	for (int i = 0;i < BaseGraphMaterial_ShaderTypeCount;i++)
	{
		if(!pVideoMat->m_pMaterial->m_ShaderArray.ppShader[i])
			continue;
		if((nLightNum == 0) && ( i > BaseGraphMaterial_DiffuseShader ) )
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

BOOL IsNeedUpdateTexParamBuffer(float2* pNewOffset,float2* pNewScale,float2* pNewCenter,float* pNewRotate,BaseGraphMatVideoTex* pVideoTex)
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

BOOL IsNeedUpdatePhyParamBuffer(float fAlpha,const float4& diffuse,const float4& ambient,const float4& specular,const float4& emissive,BaseGraphVideoMaterial* pVideoMat)
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
int UpdatePhyParam(double dTime,BaseGraphMaterialParam* pMatParam,BaseGraphVideoMaterial* pVideoMat,float fAlpha)
{
	//assert(pVideoMat);
	//BaseGraphMaterial* pMaterial = pVideoMat->m_pMaterial;
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
	//if(!pVideoMat->m_hPhyCB || pVideoMat->m_nChangeType & BaseGraphMaterial_PhyDataChange ||
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
	//	return BaseGraphMaterial_PhyDataChange;
	//}
	//BaseGraphMaterial_ClearChangeType(pVideoMat,BaseGraphMaterial_PhyDataChange);
	return BaseGraphMaterial_NoChange;
}

BOOL CalTexMatrix(matrix4f* pTexMat,double dTotalTime,double dFrame,BaseARray* pTexParam,BaseGraphMatVideoTex* pVideoTex,BaseGraphMatTex* pTex)
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
			//param_def* pParam = (param_def*)BaseAR_GetAt(pTexParam,BaseGraphTex_Offset_ParamDef);
			//calc_get_value(pParam,dFrame,&Offset,NULL);
			//pParam = (param_def*)BaseAR_GetAt(pTexParam,BaseGraphTex_Scale_ParamDef);
			//calc_get_value(pParam,dFrame,&Scale,NULL);
			//pParam = (param_def*)BaseAR_GetAt(pTexParam,BaseGraphTex_Center_ParamDef);
			//calc_get_value(pParam,dFrame,&Center,NULL);
			//pParam = (param_def*)BaseAR_GetAt(pTexParam,BaseGraphTex_Rotate_ParamDef);
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
BOOL UpdateTexParam(double dTotalFrame,double dFrame,int hDevice,BaseGraphMaterialParam* pMatParam,BaseGraphMatVideoTex* pVideoTex,BaseGraphMatTex* pTex)
{
	if(!pTex || !pVideoTex )
		return FALSE;
	BOOL bChange = FALSE;

	matrix4f TexMat;
	float4 TexData1[5];
	BaseARray* pTexParam = pMatParam ? pMatParam->texParam + pTex->nType : NULL;
	bChange |= CalTexMatrix(&TexMat,dTotalFrame,dFrame,pTexParam,pVideoTex,pTex);
	void* pMemBuffer = &TexMat;
	int nSize = sizeof(matrix4f);

	//param_def* pParam = NULL;
	//if(pMatParam)
	//	pParam = (param_def*)BaseAR_GetAt(&pMatParam->texParam[pTex->nType],BaseGraphTex_Private_ParamDef);
	//if(pTex->nType == BaseGraphMaterial_SpecTex 
	//	|| pTex->nType == BaseGraphMaterial_EnvMapTex 
	//	|| pTex->nType == BaseGraphMaterial_EmissiveTex 
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
	//else if(pTex->nType == BaseGraphMaterial_BumpTex)
	//{
	//	float fValue = 0;
	//	if(pParam)
	//		calc_get_value(pParam,dFrame,&fValue,NULL);
	//	BumpTexPrivateData* pValue = (BumpTexPrivateData*)pTex->pData;
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

BASEGRAPH_DATARENDER_API int SYUpdateDynamicTex(double dTotalFrame,int hDevice,BaseGraphMatVideoTex* pVideoTex,BaseGraphMatTex* pTex)
{
	return UpdateTexParam(dTotalFrame,dTotalFrame,hDevice,NULL,pVideoTex,pTex);
}

BASEGRAPH_DATARENDER_API int BaseGraphMaterial_UpdateFrame( BaseGraphVideoMaterial* pVideoMat,double dFrame,float fFrameRate,float fAlpha,int nLightNum,BaseGraphMaterialParam* param,void* pEnvParam, LPRECT pBound/* = NULL*/)
{
	int nChange = 0;
	if(param || pVideoMat->m_nChangeType & BaseGraphMaterial_PhyDataChange)
		nChange |= UpdatePhyParam(dFrame,param,pVideoMat,fAlpha);
	if(pVideoMat->m_nChangeType & BaseGraphMaterial_TexCountChange)
	{
		for (int i = 0; i < BaseGraphMaterial_TexTypeCount;i++)
		{
			if (pVideoMat->m_pMaterial->m_TexArray.ppTex[i] &&
				pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->disable)
			{
				BaseGraphMaterial_ReleaseVideoTex(pVideoMat->m_hDevice,pVideoMat->m_VideoTexAr.ppVTex[i]);
				SAFE_DELETE(pVideoMat->m_VideoTexAr.ppVTex[i]);
			}
			else if (pVideoMat->m_VideoTexAr.ppVTex[i]&& !pVideoMat->m_pMaterial->m_TexArray.ppTex[i])
			{
				BaseGraphMaterial_ReleaseVideoTex(pVideoMat->m_hDevice,pVideoMat->m_VideoTexAr.ppVTex[i]);
				SAFE_DELETE(pVideoMat->m_VideoTexAr.ppVTex[i]);
			}
			else if(!pVideoMat->m_VideoTexAr.ppVTex[i]&& pVideoMat->m_pMaterial->m_TexArray.ppTex[i])
			{
				BaseGraphMatVideoTex* pTex = new BaseGraphMatVideoTex;
				memset(pTex,0,sizeof(BaseGraphMatVideoTex));
				pTex->CurTexScale.x = 1;
				pTex->CurTexScale.y = 1;
				pTex->bTexNeedRelease = TRUE;
				pTex->nType = i;
				CreateTexPrivateData(&pTex->pCurData,i);
				pVideoMat->m_VideoTexAr.ppVTex[i] = pTex;

				BaseGraphMaterial_SetChangeType(pVideoMat,BaseGraphMaterial_DiffTexChange << i | BaseGraphMaterial_DiffuseParamChange << i);
			}
		}
	}
	if((pVideoMat->m_nChangeType & BaseGraphMaterial_TexCountChange) || (pVideoMat->m_nChangeType & BaseGraphMaterial_ShaderChange) || nLightNum != pVideoMat->m_nLightNum)
	{
		BaseGraphMaterial_JoinShader(pVideoMat,nLightNum);
		BaseGraphMaterial_ClearChangeType(pVideoMat,BaseGraphMaterial_TexCountChange | BaseGraphMaterial_ShaderChange);
		pVideoMat->m_nLightNum = nLightNum;
		nChange |=  BaseGraphMaterial_ShaderChange;
	}
	if (pVideoMat->m_dTime != dFrame)
	{
		pVideoMat->m_dTotalTime += dFrame - pVideoMat->m_dTime;
		//if(UpdatePhyParam(nTime,param,pVideoMat,fAlpha ))
		//	nChange |= BaseGraphMaterial_PhyDataChange;
		for(int i = 0; i< BaseGraphMaterial_TexTypeCount;i ++)
		{
			if(BaseGraphMaterial_UpdateTex(NULL,NULL,NULL,BaseGraphMaterial_DiffuseTex + i,dFrame,fFrameRate, pVideoMat,pEnvParam, FALSE, pBound))
				nChange |=  BaseGraphMaterial_DiffTexChange << i;
			if(UpdateTexParam(pVideoMat->m_dTotalTime,dFrame,pVideoMat->m_hDevice,param,pVideoMat->m_VideoTexAr.ppVTex[i],pVideoMat->m_pMaterial->m_TexArray.ppTex[i]))
				nChange |=  BaseGraphMaterial_DiffuseParamChange << i;
			BaseGraphMaterial_ClearChangeType(pVideoMat,BaseGraphMaterial_DiffTexChange << i | BaseGraphMaterial_DiffuseParamChange << i);
		}
		pVideoMat->m_dTime = dFrame;
	}
	else
	{
		//pVideoMat->m_dTotalTime += 1;
		for(int i = 0; i < BaseGraphMaterial_TexTypeCount; i++)
		{
			if(!pVideoMat->m_pMaterial->m_TexArray.ppTex[i]
			|| pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->disable)
				continue;

			if (pVideoMat->m_nChangeType & BaseGraphMaterial_DiffTexChange << i || pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->nSrcType == BaseGraphMaterial_MovFile)
			{
				if(BaseGraphMaterial_UpdateTex(NULL,NULL,NULL,BaseGraphMaterial_DiffuseTex + i,dFrame,fFrameRate,pVideoMat,pEnvParam, FALSE, pBound))
					nChange |=  BaseGraphMaterial_DiffTexChange << i;
				BaseGraphMaterial_ClearChangeType(pVideoMat,BaseGraphMaterial_DiffTexChange << i);
			}
			if(pVideoMat->m_nChangeType & BaseGraphMaterial_DiffuseParamChange << i || pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->bAutoRun)
			{				
				if(UpdateTexParam(pVideoMat->m_dTotalTime,dFrame,pVideoMat->m_hDevice,param,pVideoMat->m_VideoTexAr.ppVTex[i],pVideoMat->m_pMaterial->m_TexArray.ppTex[i]))
					nChange |=  BaseGraphMaterial_DiffuseParamChange << i;
				BaseGraphMaterial_ClearChangeType(pVideoMat,BaseGraphMaterial_DiffuseParamChange << i);
			}
		}
	}
	return nChange;
}

BASEGRAPH_DATARENDER_API void BaseGraphMaterial_SetParamToDevice( BaseGraphVideoMaterial* pVideoMat, BOOL bAdd)
{
	bool bAlpha = (pVideoMat->m_fAlpha != 1) || (pVideoMat->m_PhyData.Diffuse.w < 1);

	DX3DEngine_SetConstBuffer(pVideoMat->m_hDevice,pVideoMat->m_hPhyCB,BaseGraphMaterial_PhyParam,DXShaderType_PixelShader);
	for(int i = 0; i < BaseGraphMaterial_TexTypeCount;i++)
	{
		BaseGraphMatVideoTex* pVideoTex = pVideoMat->m_VideoTexAr.ppVTex[i];
		if(pVideoTex)
		{
			if(pVideoMat==NULL || 
				pVideoMat->m_pMaterial==NULL || 
				pVideoMat->m_pMaterial->m_TexArray.ppTex[i]==NULL)// ||
// 				pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->disable)
				continue;
			if(pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->nSrcType == BaseGraphMaterial_MovFile)
			{
				int test = 0;
			}
			if (pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->pTexPath != NULL &&
				_tcscmp(pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->pTexPath, _T("NONE")) == 0)
			{
				BaseGraphMaterial_SetChangeType(pVideoMat,BaseGraphMaterial_ShaderChange);
				//BaseGraphMaterial_ReleaseVideoTex(pVideoMat->m_hDevice,pVideoMat->m_VideoTexAr.ppVTex[i], FALSE);
				//SAFE_DELETE(pVideoMat->m_VideoTexAr.ppVTex[i]);
			}
			bAlpha = true;
			DX3DEngine_SetConstBuffer(pVideoMat->m_hDevice,pVideoTex->hTexCB,BaseGraphMaterial_DiffuseShaderParam + i,DXShaderType_PixelShader);
			DXSampleStateType nState = DXSample_Anisotropic_Clamp;
			if(pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->bAutoRun )
				nState = pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->bRepeat ? DXSample_Anisotropic_Wrap : DXSample_Anisotropic_Border;
			else if(i == BaseGraphMaterial_SpecTex)
				nState = pVideoMat->m_bTexClamp ? DXSample_NoMipMap_Anisotropic_Clamp : DXSample_NoMipMap_Anisotropic_Wrap ;
			else
				nState = pVideoMat->m_bTexClamp ? DXSample_Line_Clamp : DXSample_Line_Wrap ;

			DX3DEngine_SetSampleState(pVideoMat->m_hDevice,BaseGraphMaterial_DiffuseShaderParam + i,nState,DXShaderType_PixelShader);
			DX3DEngine_SetShaderTexture(pVideoMat->m_hDevice,pVideoTex->hTex,BaseGraphMaterial_DiffuseShaderParam + i,DXShaderType_PixelShader);
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

BaseGraphMovieResource* CreateMovieResource(TCHAR* pTexPath,double dTime,int hDevice)
{
	//SMovieInfo movieInfo;
	//if( !BaseGraphGetMovieInfo( pTexPath, movieInfo ) || movieInfo.dwFrameNum == 0 )
	//	return FALSE;
	//BaseGraphMovieResource* pResource = new BaseGraphMovieResource;
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
BASEGRAPH_DATARENDER_API BOOL BaseGraphMaterial_UpdateTex( CommonBufferDef32* pSrcBufferDef, DXHalDef32* pSrcTexDef,int hSrcTex, int nTexIndex,double dTime, float fFrameRate, BaseGraphVideoMaterial* pVideoMat,void* param,BOOL isForceUpdate /*= FALSE*/, LPRECT pBound/* = NULL*/ )
{
	BaseGraphMatTex* pTex = pVideoMat->m_pMaterial->m_TexArray.ppTex[nTexIndex];
	if(!pTex)
		return FALSE;
	BaseGraphMatVideoTex* pVideoTex = pVideoMat->m_VideoTexAr.ppVTex[nTexIndex];
	if(!pVideoTex)
		return FALSE;

	if(pTex->nSrcType == BaseGraphMaterial_Object)
	{
		RECT rtBuffer;
		g_pObjectFun(BaseGraphMaterial_GetBufferSize,&rtBuffer,NULL,g_pObjectParam,param);
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
		return g_pObjectFun(BaseGraphMaterial_GetBuffer,NULL,pVideoTex->hTex,g_pObjectParam,param);
	}
	else if(pTex->nSrcType == BaseGraphMaterial_Channel)
	{
		RECT rtBuffer;
		g_pChannelFun(BaseGraphMaterial_GetBufferSize,&rtBuffer,NULL,g_pChannelParam,param);
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
		return g_pChannelFun(BaseGraphMaterial_GetBuffer,NULL,pVideoTex->hTex,g_pChannelParam,param);
	}
	else if(pTex->nSrcType == BaseGraphMaterial_PicFile)
	{
		if(!(pVideoMat->m_nChangeType & BaseGraphMaterial_DiffTexChange << pTex->nType))
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

		BOOL bMap = pTex->nType == BaseGraphMaterial_BumpTex;

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
	else if(pTex->nSrcType == BaseGraphMaterial_MovFile)
	{
		//if(!(pVideoMat->m_nChangeType & BaseGraphMaterial_DiffTexChange << pTex->nType) && pVideoMat->m_dTime == dTime)
		//	return FALSE;

		if (!pTex->pTexPath )
			return FALSE;
		TCHAR realpath[MAX_PATH];
		//GetFileManager()->TagPathTrans(pTex->pTexPath,realpath,GetFileManager()->lpParam);
		if( _taccess(realpath, 0) == -1)
			return FALSE;

		if((pVideoMat->m_nChangeType & BaseGraphMaterial_DiffTexChange << pTex->nType))
		{
			BaseGraphMaterial_ReleaseVideoTex(pVideoMat->m_hDevice,pVideoTex,FALSE);
		}

		int nField = (int)pVideoMat->m_dTotalTime;
		if( pVideoMat->m_bEdit)
		{
			//if(!pVideoTex->pSeekData)
			//{   
			//	pVideoTex->pSeekData = new BaseGraphMovieSeekData;
			//	SMovieInfo movieInfo;
			//	BaseGraphGetMovieInfo(realpath, movieInfo );
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
			//	BaseGraphMoviePrePlay(realpath,pVideoTex->pSeekData->nWidth,pVideoTex->pSeekData->nHeight,fFrameRate,
			//		pVideoTex->pSeekData->pVideoSeekData, TRUE );
			//	pVideoTex->pSeekData->movie_buffer.Create(pVideoTex->pSeekData->nWidth , pVideoTex->pSeekData->nHeight);

			//}

			//int nMovieTime = CalcMoviePlayTime(nField,pVideoTex->pSeekData->nTotalFrame);    
			//BaseGraphMoviePlay(pVideoTex->pSeekData->pVideoSeekData, &pVideoTex->pSeekData->movie_buffer,nMovieTime , NULL, NULL );
			//if(!pVideoTex->hTex)
			//{
			//	pVideoTex->hTex = DX3DEngine_CreateTexture(pVideoMat->m_hDevice,pVideoTex->pSeekData->nWidth,pVideoTex->pSeekData->nHeight,DXTexture_Normal2DTex);
			//	pVideoTex->bTexNeedRelease = TRUE;
			//}

			//int hTransTex = DXTextureManager_QueryTexture(BaseGraphTexture_MemTexture,pVideoTex->pSeekData->nWidth,pVideoTex->pSeekData->nHeight);
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
			//if(pVideoTex->pResource->bIsProgress && parity == Common_PARITY_EVEN) //
			//	return TRUE;
			//if(!pVideoTex->pSeekData)
			//{   
			//	pVideoTex->pSeekData = new BaseGraphMovieSeekData;
			//	SMovieInfo movieInfo;
			//	BaseGraphGetMovieInfo(realpath, movieInfo );
			//	pVideoTex->pSeekData->nWidth = movieInfo.nMotionWidth;
			//	pVideoTex->pSeekData->nHeight = movieInfo.nMotionHeight;
			//	pVideoTex->pSeekData->nTotalFrame = movieInfo.dwFrameNum;
			//	pVideoTex->pSeekData->pVideoSeekData = NULL;
			//	BaseGraphMoviePrePlay(realpath,pVideoTex->pSeekData->nWidth,pVideoTex->pSeekData->nHeight,fFrameRate,
			//		pVideoTex->pSeekData->pVideoSeekData, TRUE );
			//	pVideoTex->pSeekData->movie_buffer.Create(pVideoTex->pSeekData->nWidth , pVideoTex->pSeekData->nHeight);

			//}
			int nMovieTime = CalcMoviePlayTime(nField,pVideoTex->pResource->nTotalFrame);    
			//BaseGraphMoviePlay(pVideoTex->pSeekData->pVideoSeekData, &pVideoTex->pSeekData->movie_buffer,nMovieTime , NULL, NULL );

			pVideoTex->pResource->nCur = (pVideoTex->pResource->nCur + 1)%2;

			//MovieProcessPara para;
			////para.maskBuf = 0;
			//para.hMovieTex = pVideoTex->pResource->hTex[pVideoTex->pResource->nCur];
			////这块进行融合时，因为前边已经将场号限定在只渲偶场，所以顶场先及帧模式需要变动
			//if(parity == Common_PARITY_EVEN)
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
	else if(pTex->nSrcType == BaseGraphMaterial_Buffer)
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
			if(!(pVideoMat->m_nChangeType & BaseGraphMaterial_DiffTexChange << pTex->nType))
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
		BaseGraphMaterial_SetTex(pVideoTex->hTex,*pBound,pSrcBufferDef->m_nOriginX,pSrcBufferDef->m_nOriginY, pVideoMat,nTexIndex);
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

BASEGRAPH_DATARENDER_API void BaseGraphMaterial_CreateParam(BaseGraphMaterialParam* pMatParam ,BaseGraphMaterial*  pMaterial )
{
	//for (int i = 0; i < BaseGraphMATERIAL_TEX_COUNT; i++)
	//{
	//	BaseAR_Init(&pMatParam->texParam[i]);
	//	BaseGraphMaterial_AddParamdef(&pMatParam->texParam[i],i);
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

BASEGRAPH_DATARENDER_API void BaseGraphMaterial_ReleaseParam( BaseGraphMaterialParam* pMatParam ,BaseGraphMaterial* pMaterial )
{
	//for (int i = 0; i < BaseGraphMATERIAL_TEX_COUNT; i++)
	//{
	//	int nParamCount = BaseAR_GetCount(&pMatParam->texParam[i]);
	//	for(int j = 0; j < nParamCount; j++)
	//	{
	//		param_def* param = (param_def*)BaseAR_GetAt(&pMatParam->texParam[i],j);
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

BASEGRAPH_DATARENDER_API int BaseGraphMaterial_AddKeyValue( double dFrame,BaseGraphMaterialParam* pMatParam, BaseGraphMaterial* pMaterial,void* pValue,int nParamType )
{
	//if (nParamType == BaseGraphMaterial_Phy_ParamDef)
	//{
	//	BaseGraphPhyData* pData = (BaseGraphPhyData*)pValue;
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
	//	return BaseGraphMaterial_PhyDataChange;
	//}
	//else if(nParamType == BaseGraphMaterial_Phy_Diffuse_ParamDef)
	//{
	//	float4* pColor = (float4*)pValue;
	//	AddClipKeyFrame(&pMatParam->common.Diffuse,dFrame,(BYTE*)pColor);
	//	return BaseGraphMaterial_PhyDataChange;
	//}
	//else if(nParamType == BaseGraphMaterial_Phy_Ambient_ParamDef)
	//{
	//	float4* pColor = (float4*)pValue;
	//	AddClipKeyFrame(&pMatParam->common.Ambient,dFrame,(BYTE*)pColor);
	//	return BaseGraphMaterial_PhyDataChange;
	//}
	//else if(nParamType == BaseGraphMaterial_Phy_Specular_ParamDef)
	//{
	//	float4* pColor = (float4*)pValue;
	//	AddClipKeyFrame(&pMatParam->common.Specular,dFrame,(BYTE*)pColor);
	//	return BaseGraphMaterial_PhyDataChange;
	//}
	//else if(nParamType == BaseGraphMaterial_Phy_Emissive_ParamDef)
	//{
	//	float4* pColor = (float4*)pValue;
	//	AddClipKeyFrame(&pMatParam->common.Emissive,dFrame,(BYTE*)pColor);
	//	return BaseGraphMaterial_PhyDataChange;
	//}
	//else if(nParamType == BaseGraphMaterial_Phy_Alpha_ParamDef)
	//{
	//	float* pData = (float*)pValue;
	//	float fValue = *pData- pMaterial->m_PhyData.fAlpha;
	//	AddClipKeyFrame(&pMatParam->common.fAlpha,dFrame,(BYTE*)&fValue);
	//	return BaseGraphMaterial_PhyDataChange;
	//}
	//else if(nParamType <= BumpTex_Private_ParamDef && nParamType >= BaseGraphDiffuseTex_Offset_ParamDef)
	//{
	//	int nTexType = (nParamType - BaseGraphDiffuseTex_Offset_ParamDef) / BaseGraphTex_ParamDefCount;
	//	int paramBegin = (nParamType - BaseGraphDiffuseTex_Offset_ParamDef) % BaseGraphTex_ParamDefCount;
	//	if(!pMaterial->m_TexArray.ppTex[nTexType])
	//		return BaseGraphMaterial_NoChange;
	//	BaseGraphMaterial_AddTexKeyValue(dFrame,&pMatParam->texParam[nTexType],pMaterial->m_TexArray.ppTex[nTexType],pValue, paramBegin);
	//	return BaseGraphMaterial_DiffuseParamChange << nTexType;
	//}
	return BaseGraphMaterial_NoChange;
}

BASEGRAPH_DATARENDER_API BOOL		BaseGraphMaterial_SetTex(int hTex,const rectf& pValidRt,int nOriginX, int nOriginY, BaseGraphVideoMaterial* pVideoMat, int nTexIndex,BOOL bNeedRelease )
{
	if(!hTex)
		return FALSE;

	float nSrcWidth , nSrcHeight;
	nSrcWidth = pValidRt.right - pValidRt.left;
	nSrcHeight = pValidRt.bottom - pValidRt.top;

	int nWidth,nHeight;
	DX3DEngine_GetTextureInfo(pVideoMat->m_hDevice,hTex,&nWidth,&nHeight,NULL);

	BaseGraphMatVideoTex* pVideoTex = pVideoMat->m_VideoTexAr.ppVTex[nTexIndex];
	BaseGraphMatTex* pTex = pVideoMat->m_pMaterial->m_TexArray.ppTex[nTexIndex];
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

BASEGRAPH_DATARENDER_API BOOL		BaseGraphMaterial_SetTex(int hTex,const RECT& pValidRt, int nOriginX, int nOriginY,BaseGraphVideoMaterial* pVideoMat, int nTexIndex,BOOL bNeedRelease)
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
	BaseGraphMatVideoTex* pVideoTex = pVideoMat->m_VideoTexAr.ppVTex[nTexIndex];
	BaseGraphMatTex* pTex = pVideoMat->m_pMaterial->m_TexArray.ppTex[nTexIndex];
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

BASEGRAPH_DATARENDER_API int BaseGraphMaterial_SetTexRepeat( BaseGraphMaterial* pMat,int nTexIndex,BOOL bRepeat )
{
	BaseGraphMatTex* pTex = pMat->m_TexArray.ppTex[nTexIndex];
	if (!pTex)
		return BaseGraphMaterial_NoChange;
	pTex->bRepeat = bRepeat;
	return BaseGraphMaterial_DiffuseParamChange << nTexIndex;
}

BASEGRAPH_DATARENDER_API void		BaseGraphMaterial_CopyParam(BaseGraphMaterialParam* pDestMatParam,const BaseGraphMaterialParam* pSrcMatParam   )
{
	//BaseGraphMaterial_ReleaseParam(pDestMatParam,NULL);

	//for (int i = 0; i < BaseGraphMATERIAL_TEX_COUNT; i++)
	//{
	//	int nCount = BaseAR_GetCount(&pSrcMatParam->texParam[i]);
	//	for(int j = 0; j < nCount; j++)
	//	{
	//		param_def* pSrc = (param_def*)BaseAR_GetAt(&pSrcMatParam->texParam[i],j);
	//		param_def* pDst = new param_def;
	//		memset(pDst,0,sizeof(param_def));
	//		sy_copy_paramdef(pDst,pSrc);
	//		BaseAR_Add(&pDestMatParam->texParam[i],pDst);
	//	}
	//}
	////pMatParam->diffuse
	//sy_copy_paramdef(&pDestMatParam->common.Diffuse,&pSrcMatParam->common.Diffuse);
	//sy_copy_paramdef(&pDestMatParam->common.Ambient,&pSrcMatParam->common.Ambient);
	//sy_copy_paramdef(&pDestMatParam->common.Specular,&pSrcMatParam->common.Specular);
	//sy_copy_paramdef(&pDestMatParam->common.Emissive,&pSrcMatParam->common.Emissive);
	//sy_copy_paramdef(&pDestMatParam->common.fAlpha,&pSrcMatParam->common.fAlpha);
}

BASEGRAPH_DATARENDER_API size_t BaseGraphMaterial_GetVideoMemSize( BaseGraphVideoMaterial* pVideoMat )
{
	size_t size = 0;
	size += 5 * sizeof(float4);

	for (int i = 0; i < BaseGraphMATERIAL_TEX_COUNT; i++)
	{
		if(pVideoMat->m_VideoTexAr.ppVTex[i])
		{
			size += DX3DEngine_GetTextureMemSize(pVideoMat->m_hDevice,pVideoMat->m_VideoTexAr.ppVTex[i]->hTex);
			size += 5 * sizeof(float4);
		}
	}
	return size;
}

BASEGRAPH_DATARENDER_API int BaseGraphMaterial_SetTexParam( BaseGraphMaterial* pMat,int nTexIndex,BaseGraphMatTex* pTexParam,int nType )
{
	if(nType & BaseGraphMATERIAL_TEXPARAM_TEXOFFSETX)
		pMat->m_TexArray.ppTex[nTexIndex]->TexOffset.x = pTexParam->TexOffset.x;
	if(nType & BaseGraphMATERIAL_TEXPARAM_TEXOFFSETY)
		pMat->m_TexArray.ppTex[nTexIndex]->TexOffset.y = pTexParam->TexOffset.y;
	if(nType & BaseGraphMATERIAL_TEXPARAM_TEXSCALEX	)
		pMat->m_TexArray.ppTex[nTexIndex]->TexScale.x = pTexParam->TexScale.x;
	if(nType & BaseGraphMATERIAL_TEXPARAM_TEXSCALEY	)
		pMat->m_TexArray.ppTex[nTexIndex]->TexScale.y = pTexParam->TexScale.y;
	if(nType & BaseGraphMATERIAL_TEXPARAM_CENTERX	)
		pMat->m_TexArray.ppTex[nTexIndex]->Center.x = pTexParam->Center.x;
	if(nType & BaseGraphMATERIAL_TEXPARAM_CENTERY	)
		pMat->m_TexArray.ppTex[nTexIndex]->Center.y = pTexParam->Center.y;
	if(nType & BaseGraphMATERIAL_TEXPARAM_TEXROTATE	)
		pMat->m_TexArray.ppTex[nTexIndex]->TexRotate = pTexParam->TexRotate;
	if(nType & BaseGraphMATERIAL_TEXPARAM_AUTORUN	)
		pMat->m_TexArray.ppTex[nTexIndex]->bAutoRun = pTexParam->bAutoRun;
	if(nType & BaseGraphMATERIAL_TEXPARAM_SPEED	)
		pMat->m_TexArray.ppTex[nTexIndex]->dSpeed = pTexParam->dSpeed;
	if(nType & BaseGraphMATERIAL_TEXPARAM_RUNDIRX	)
		pMat->m_TexArray.ppTex[nTexIndex]->vRunDir.x = pTexParam->vRunDir.x;
	if(nType & BaseGraphMATERIAL_TEXPARAM_RUNDIRY	)
		pMat->m_TexArray.ppTex[nTexIndex]->vRunDir.y = pTexParam->vRunDir.y;
	if(nType & BaseGraphMATERIAL_TEXPARAM_REPEAT	)
		pMat->m_TexArray.ppTex[nTexIndex]->bRepeat = pTexParam->bRepeat;
	if(nType & BaseGraphMATERIAL_TEXPARAM_DISABLE	)
		pMat->m_TexArray.ppTex[nTexIndex]->disable = pTexParam->disable;

	return BaseGraphMaterial_DiffuseParamChange << nTexIndex;
}

BASEGRAPH_DATARENDER_API void BaseGraphMaterial_PhyRenderDataToPhy( BaseGraphPhyData* pData,BaseGraphPhyRenderData* pSrcData )
{
	Float4ToDWORDColor(&pData->Diffuse,pSrcData->Diffuse);
	Float4ToDWORDColor(&pData->Ambient,pSrcData->Ambient);
	Float4ToDWORDColor(&pData->Specular,pSrcData->Specular);
	Float4ToDWORDColor(&pData->Emissive,pSrcData->Emissive);
	pData->fAlpha = pSrcData->fAlpha;
	pData->Power = pSrcData->Power;
	pData->disable = pSrcData->disable;
}

BASEGRAPH_DATARENDER_API void		BaseGraphMaterial_PhyToPhyRenderData(BaseGraphPhyRenderData* pData,BaseGraphPhyData* pSrcData)
{
	DWORDColorToFloat4(&pData->Diffuse,pSrcData->Diffuse);
	DWORDColorToFloat4(&pData->Ambient,pSrcData->Ambient);
	DWORDColorToFloat4(&pData->Specular,pSrcData->Specular);
	DWORDColorToFloat4(&pData->Emissive,pSrcData->Emissive);
	pData->fAlpha = pSrcData->fAlpha;
	pData->Power = pSrcData->Power;
	pData->disable = pSrcData->disable;
}

BASEGRAPH_DATARENDER_API int BaseGraphMaterial_VideoMatUpdateCopyVideo(BaseGraphVideoMaterial* pVideoMat, double dFrame,float fFrameRate,float fAlpha,int nLightNum,BaseGraphMaterialParam* param,void* pEnvParam,BaseGraphVideoMaterial* pVideoMatSrc)
{
	int nChange = 0;
	if(param || pVideoMat->m_nChangeType & BaseGraphMaterial_PhyDataChange)
		nChange |= UpdatePhyParam(dFrame,param,pVideoMat,fAlpha);
	if(pVideoMat->m_nChangeType & BaseGraphMaterial_TexCountChange)
	{
		for (int i = 0; i < BaseGraphMaterial_TexTypeCount;i++)
		{
			if (pVideoMat->m_pMaterial->m_TexArray.ppTex[i] &&
				pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->disable)
			{
				BaseGraphMaterial_ReleaseVideoTex(pVideoMat->m_hDevice,pVideoMat->m_VideoTexAr.ppVTex[i]);
				SAFE_DELETE(pVideoMat->m_VideoTexAr.ppVTex[i]);
			}
			else if (pVideoMat->m_VideoTexAr.ppVTex[i]&& !pVideoMat->m_pMaterial->m_TexArray.ppTex[i])
			{
				BaseGraphMaterial_ReleaseVideoTex(pVideoMat->m_hDevice,pVideoMat->m_VideoTexAr.ppVTex[i]);
				SAFE_DELETE(pVideoMat->m_VideoTexAr.ppVTex[i]);
			}
			else if(!pVideoMat->m_VideoTexAr.ppVTex[i]&& pVideoMat->m_pMaterial->m_TexArray.ppTex[i])
			{
				BaseGraphMatVideoTex* pTex = new BaseGraphMatVideoTex;
				memset(pTex,0,sizeof(BaseGraphMatVideoTex));
				pTex->CurTexScale.x = 1;
				pTex->CurTexScale.y = 1;
				pTex->bTexNeedRelease = TRUE;
				pTex->nType = i;
				CreateTexPrivateData(&pTex->pCurData,i);
				pVideoMat->m_VideoTexAr.ppVTex[i] = pTex;

				BaseGraphMaterial_SetChangeType(pVideoMat,BaseGraphMaterial_DiffTexChange << i | BaseGraphMaterial_DiffuseParamChange << i);
			}
		}
	}
	if((pVideoMat->m_nChangeType & BaseGraphMaterial_TexCountChange) || (pVideoMat->m_nChangeType & BaseGraphMaterial_ShaderChange) || nLightNum != pVideoMat->m_nLightNum)
	{
		BaseGraphMaterial_JoinShader(pVideoMat,nLightNum);
		BaseGraphMaterial_ClearChangeType(pVideoMat,BaseGraphMaterial_TexCountChange | BaseGraphMaterial_ShaderChange);
		pVideoMat->m_nLightNum = nLightNum;
		nChange |=  BaseGraphMaterial_ShaderChange;
	}
	if (pVideoMat->m_dTime != dFrame)
	{
		pVideoMat->m_dTotalTime += dFrame - pVideoMat->m_dTime;
		//if(UpdatePhyParam(nTime,param,pVideoMat,fAlpha ))
		//	nChange |= BaseGraphMaterial_PhyDataChange;
		for(int i = 0; i< BaseGraphMaterial_TexTypeCount;i ++)
		{
			if(BaseGraphMaterial_UpdateTexCopyVideo(NULL,NULL,NULL,BaseGraphMaterial_DiffuseTex + i,dFrame,fFrameRate, pVideoMat,pVideoMatSrc,pEnvParam))
				nChange |=  BaseGraphMaterial_DiffTexChange << i;
			if(UpdateTexParam(pVideoMat->m_dTotalTime,dFrame,pVideoMat->m_hDevice,param,pVideoMat->m_VideoTexAr.ppVTex[i],pVideoMat->m_pMaterial->m_TexArray.ppTex[i]))
				nChange |=  BaseGraphMaterial_DiffuseParamChange << i;
			BaseGraphMaterial_ClearChangeType(pVideoMat,BaseGraphMaterial_DiffTexChange << i | BaseGraphMaterial_DiffuseParamChange << i);
		}
		pVideoMat->m_dTime = dFrame;
	}
	else
	{
		//pVideoMat->m_dTotalTime += 1;
		for(int i = 0; i < BaseGraphMaterial_TexTypeCount; i++)
		{
			if(!pVideoMat->m_pMaterial->m_TexArray.ppTex[i]
			|| pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->disable)
				continue;

			if (pVideoMat->m_nChangeType & BaseGraphMaterial_DiffTexChange << i || pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->nSrcType == BaseGraphMaterial_MovFile)
			{
				if(BaseGraphMaterial_UpdateTexCopyVideo(NULL,NULL,NULL,BaseGraphMaterial_DiffuseTex + i,dFrame,fFrameRate,pVideoMat,pVideoMatSrc,pEnvParam))
					nChange |=  BaseGraphMaterial_DiffTexChange << i;
				BaseGraphMaterial_ClearChangeType(pVideoMat,BaseGraphMaterial_DiffTexChange << i);
			}
			if(pVideoMat->m_nChangeType & BaseGraphMaterial_DiffuseParamChange << i || pVideoMat->m_pMaterial->m_TexArray.ppTex[i]->bAutoRun)
			{				
				if(UpdateTexParam(pVideoMat->m_dTotalTime,dFrame,pVideoMat->m_hDevice,param,pVideoMat->m_VideoTexAr.ppVTex[i],pVideoMat->m_pMaterial->m_TexArray.ppTex[i]))
					nChange |=  BaseGraphMaterial_DiffuseParamChange << i;
				BaseGraphMaterial_ClearChangeType(pVideoMat,BaseGraphMaterial_DiffuseParamChange << i);
			}
		}
	}
	return nChange;
}

BASEGRAPH_DATARENDER_API BOOL BaseGraphMaterial_UpdateTexCopyVideo(CommonBufferDef32* pSrcBufferDef, DXHalDef32* pSrcTexDef,int hSrcTex, int nTexIndex,double dTime, float fFrameRate,BaseGraphVideoMaterial* pVideoMat,BaseGraphVideoMaterial* pVideoMatSrc,void* param,BOOL isForceUpdate /*= FALSE*/)
{
	BaseGraphMatTex* pTex = pVideoMat->m_pMaterial->m_TexArray.ppTex[nTexIndex];
	if(!pTex)
		return FALSE;
	BaseGraphMatVideoTex* pVideoTex = pVideoMat->m_VideoTexAr.ppVTex[nTexIndex];
	if(!pVideoTex)
		return FALSE;
	if (!pVideoMatSrc)
		return FALSE;
	BaseGraphMatVideoTex* pVideoTexSrc = pVideoMatSrc->m_VideoTexAr.ppVTex[nTexIndex];
	if(!pVideoTexSrc)
		return FALSE;

	if(pTex->nSrcType == BaseGraphMaterial_Object)
	{
		RECT rtBuffer;
		g_pObjectFun(BaseGraphMaterial_GetBufferSize,&rtBuffer,NULL,g_pObjectParam,param);
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
		return g_pObjectFun(BaseGraphMaterial_GetBuffer,NULL,pVideoTex->hTex,g_pObjectParam,param);
	}
	else if(pTex->nSrcType == BaseGraphMaterial_Channel)
	{
		RECT rtBuffer;
		g_pChannelFun(BaseGraphMaterial_GetBufferSize,&rtBuffer,NULL,g_pChannelParam,param);
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
		return g_pChannelFun(BaseGraphMaterial_GetBuffer,NULL,pVideoTex->hTex,g_pChannelParam,param);
	}
	else if(pTex->nSrcType == BaseGraphMaterial_PicFile)
	{
		if(!(pVideoMat->m_nChangeType & BaseGraphMaterial_DiffTexChange << pTex->nType))
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

		BOOL bMap = pTex->nType == BaseGraphMaterial_BumpTex;

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
	else if(pTex->nSrcType == BaseGraphMaterial_MovFile)
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
	else if(pTex->nSrcType == BaseGraphMaterial_Buffer)
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
			if(!(pVideoMat->m_nChangeType & BaseGraphMaterial_DiffTexChange << pTex->nType))
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
		BaseGraphMaterial_SetTex(pVideoTex->hTex,*pBound,pSrcBufferDef->m_nOriginX,pSrcBufferDef->m_nOriginY, pVideoMat,nTexIndex);
		pVideoTex->bTexNeedRelease = TRUE;
	}
	else
	{
		//处理视频,缺少解码库
	}
	return FALSE;
}

BASEGRAPH_DATARENDER_API void BaseGraphMatTex::Reset()
{
	memset(this, 0, sizeof(BaseGraphMatTex));
}

BASEGRAPH_DATARENDER_API void BaseGraphPhyData::Reset()
{
	disable = 0;
	Diffuse = 0;
	Ambient = 0;
	Specular = 0;
	Emissive = 0;
	Power = 0;
	fAlpha = 0;
}

BASEGRAPH_DATARENDER_API void BaseGraphJoinShader::Reset()
{
	memset(this, 0, sizeof(BaseGraphJoinShader));
}

BASEGRAPH_DATARENDER_API void BaseGraphMaterial::Reset()
{
	memset(this, 0, sizeof(BaseGraphMaterial));
}

BASEGRAPH_DATARENDER_API void BaseGraphSpecTexPrivateData::Reset()
{
	fFactor = 1;
}

BASEGRAPH_DATARENDER_API void BumpTexPrivateData::Reset()
{
	fFactor = 1;
	bReverse = FALSE;
}
