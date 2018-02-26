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
	return nRet;
}

int ObjectCalculator::GetHeight()
{
	int nRet = 0;
	return nRet;
}

int ObjectCalculator::GetObjBufferID()
{
	return m_nObjBufferID;
}

bool ObjectCalculator::GetObjBuffer(StuSimpleBuffer & pBuf)
{
	int nBufID = GetObjBufferID();
	ObjectBuffer * pObjBuf = DataBufferManager::Instance().Get2DBuf(nBufID);
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
