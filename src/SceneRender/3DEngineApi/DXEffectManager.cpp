#include "StdAfx.h"
#include "SceneRender\DXEffectManager.h"

////////////////////////////////////////////////////////////
CTechUnit::~CTechUnit()
{
	Release();
}

void CTechUnit::Release()
{
	SAFE_DELETE(m_resource_ptr);
}

BOOL CTechUnit::Create(LPCTSTR strResName,SYTechCreateParam* pCreateParam)
{
	if(!pCreateParam)
		return FALSE;
	ID3D10Effect* pd3dEffect = pCreateParam->pd3deffect;
	m_resource_ptr = new SYTechUnit;
	_tcscpy_s(m_resource_name,strResName);
	size_t nLen =    _tcslen(m_resource_name); 
#ifdef UNICODE
	char    *name = (char *)malloc(255);
	size_t ConvertedChars;
	wcstombs_s(&ConvertedChars,name,255,m_resource_name,255);
#else
	char* name = new char[(nLen+1)];
	_tcscpy_s(name,m_resource_name);
#endif
	m_resource_ptr->m_pTech = pd3dEffect->GetTechniqueByName(name);
	if (name)
	{
		free(name);
	}
	if(!(m_resource_ptr->m_pTech ->IsValid()))
	{
		SAFE_DELETE(m_resource_ptr);
		return FALSE;
	}
	D3D10_PASS_DESC PassDesc;
	(m_resource_ptr->m_pTech)->GetPassByIndex(0)->GetDesc( &PassDesc );
	(pCreateParam->pDevice)->CreateInputLayout( pCreateParam->pInputDesc,
								pCreateParam->nInputElements, 
								PassDesc.pIAInputSignature,
								PassDesc.IAInputSignatureSize, 
								&(m_resource_ptr->m_pLayout));
	return TRUE;
}

////////////////////////////////////////////////////////////
//CEffectUnit
////////////////////////////////////////////////////////////
CEffectUnit::~CEffectUnit()
{
	Release();
}

void CEffectUnit::Release()
{
	m_TechPool.ReleaseAll();

	if(m_resource_ptr)
		m_resource_ptr->Release();
	m_resource_ptr = 0;
}

BOOL CEffectUnit::Create(LPCTSTR strResName,SYEffectCreateParam* pCreateParam)
{
	_tcscpy_s(m_resource_name,strResName);

	TCHAR filename[MAX_PATH];
	_tcscpy_s(filename,pCreateParam->pPath);
	_tcscat_s(filename,_T("\\Fx"));
	_tcscat_s(filename,m_resource_name);
	_tcscat_s(filename,_T(".syx"));
	UINT FXFlags = 0,HLSLFlags = 0;
#ifdef _DEBUG
	HLSLFlags = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG;
#endif
	if(FAILED(D3DX10CreateEffectFromFile(filename,pCreateParam->pDefines,NULL,"fx_4_0",HLSLFlags,FXFlags,pCreateParam->pDevice,NULL,NULL,&m_resource_ptr,NULL, NULL )))
		return FALSE;
	for(int i = 0 ;i<pCreateParam->nTechs; i++)
	{
		pCreateParam->pTechParam[i].pd3deffect = m_resource_ptr;
		m_TechPool.CreateResource((pCreateParam->pTechParam[i]).pTechName,&(pCreateParam->pTechParam[i]));
	}
	return TRUE;
}

CTechUnit*	CEffectUnit::GetTechnique(LPCTSTR strTechName)
{
	return m_TechPool.GetResource(strTechName);
}

SYD3DEffectManager::SYD3DEffectManager(void)
{
}

SYD3DEffectManager::~SYD3DEffectManager(void)
{
	m_EffectPool.ReleaseAll();
}

SYD3DEffectManager::SYD3DEffectManager(ID3D10Device* device):m_d3d_device(device),m_hQuadVB(0)
{
	
}

CEffectUnit*	SYD3DEffectManager::CreateEffectUnit(SYEffectCreateParam* pParam)
{
	return m_EffectPool.CreateResource(pParam->pEffectName,pParam);
}

void		SYD3DEffectManager::ReleaseEffectUnit(LPCTSTR strEffectName)
{
	return m_EffectPool.ReleaseResource(strEffectName);
}

CTechUnit*	SYD3DEffectManager::GetTechUnit(LPCTSTR strEffectName,LPCTSTR strTechName)
{
	CEffectUnit* pEu = m_EffectPool.GetResource(strEffectName);
	if(!pEu)
		return NULL;
	else
		return pEu->GetTechnique(strTechName);
}


