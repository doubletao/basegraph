#include "stdafx.h"
#include "ObjectCalculator.h"


ObjectCalculator::ObjectCalculator(CString strObjID)
	: m_strObjID(strObjID)
	, m_nObjBufferID(0)
	, m_emChgLevel(ChgLevel_NULL)
	, m_bSelected(false)
{
}


ObjectCalculator::~ObjectCalculator()
{
}

int ObjectCalculator::GetWidth()
{
	int nRet = 0;
	BaseGraph_Common::Value stuValue;
	if (DataBase_GetItemProperty(m_strObjID, BaseGraph_DataBase::Obj_Width_Int, stuValue))
	{
		nRet = stuValue.ToInt();
	}
	return nRet;
}

int ObjectCalculator::GetHeight()
{
	int nRet = 0;
	BaseGraph_Common::Value stuValue;
	if (DataBase_GetItemProperty(m_strObjID, BaseGraph_DataBase::Obj_Height_Int, stuValue))
	{
		nRet = stuValue.ToInt();
	}
	return nRet;
}

ObjectBuffer * ObjectCalculator::GetObjectBuffer()
{
	int nBufID = GetObjBufferID();
	ObjectBuffer * pObjBuf = DataBufferManager::Instance().Get2DBuf(nBufID);
	return pObjBuf;
}

int ObjectCalculator::GetObjBufferID()
{
	return m_nObjBufferID;
}

bool ObjectCalculator::GetObjSimpBuffer(StuSimpleBuffer & pBuf)
{
	ObjectBuffer * pObjBuf = GetObjectBuffer();
	if (pObjBuf)
	{
		pBuf.pBuffer = pObjBuf->GetBuffer();
		pBuf.nWidth = pObjBuf->GetWidth();
		pBuf.nHeight = pObjBuf->GetHeight();
		return true;
	}
	return false;
}

bool ObjectCalculator::SetSelected(bool bSel)
{
	bool bSelOri = m_bSelected;
	m_bSelected = bSel;
	return bSelOri;
}

bool ObjectCalculator::GetSelected()
{
	return m_bSelected;
}

DXHalDef32 * ObjectCalculator::GetHalDes()
{
	return &m_HalDes;
}

bool ObjectCalculator::NeedRenderBuffer()
{
	//如果连bufer都还没有呢，必然需要render
	ObjectBuffer * pBufer = DataBufferManager::Instance().Get2DBuf(GetObjBufferID());
	if (!pBufer)
	{
		return true;
	}
	//如果level大于等于buf，需要render
	if (m_emChgLevel >= ChgLevel_Buffer)
	{
		return true;
	}
	return false;
}

bool ObjectCalculator::UpdateRenderBuffer()
{
	return false;
}

bool ObjectCalculator::PrepareItemHalBufer(int nDevice)
{
	int nTexWidth = 0;
	int nTexHeight = 0;
	int nWidth = GetWidth();
	int nHeight = GetHeight();
	DXTextureType emTexType = DXTexture_Normal2DTex;
	if (m_HalDes.m_hTex)
	{
		DX3DEngine_GetTextureInfo(nDevice, m_HalDes.m_hTex, &nTexWidth, &nTexHeight, (int *)&emTexType);
		BOOL bRelease = nTexWidth < nWidth || nTexHeight < nHeight
			|| nWidth >= 8192 || nHeight >= 8192
			|| ((nTexWidth > nWidth || nTexHeight > nHeight) && (nTexHeight > 8192 || nTexWidth > 8192));
		// 8192以内，新的tex超过旧的，重新生成，新的tex超过8192，重新生成，新的tex正常旧的tex超过8192，重新生成
		if (bRelease)
		{
			if (m_HalDes.m_bRelease)
				DX3DEngine_ReleaseTexture(nDevice, m_HalDes.m_hTex);
			m_HalDes.m_hTex = 0;
		}
	}
	if (!m_HalDes.m_hTex)
	{
		m_HalDes.m_hTex = DX3DEngine_CreateTexture(nDevice, nWidth, nHeight, emTexType);
		m_HalDes.m_bRelease = TRUE;
	}
	m_HalDes.m_nOriginX = nWidth / 2;
	m_HalDes.m_nOriginY = nHeight / 2;
	m_HalDes.m_rtBound.left = -nWidth / 2;
	m_HalDes.m_rtBound.right = nWidth / 2;
	m_HalDes.m_rtBound.top = -nHeight / 2;
	m_HalDes.m_rtBound.bottom = nHeight / 2;
	DX3DEngine_ClearTexture(nDevice, m_HalDes.m_hTex);
	return true;
}
