#include "stdafx.h"
#include "ObjectCalculator.h"


ObjectCalculator::ObjectCalculator(CString strObjID)
	: m_strObjID(strObjID)
	, m_nObjBufferID(0)
	, m_emChgLevel(ChgLevel_NULL)
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

bool ObjectCalculator::GetObjBuffer(byte *& pBuffer)
{
	int nBufID = GetObjBufferID();
	ObjectBuffer * pObjBuf = DataBufferManager::Instance().GetBuf(nBufID);
	if (pObjBuf)
	{
		pBuffer = pObjBuf->GetBuffer();
		return true;
	}
	return false;
}

bool ObjectCalculator::NeedRenderBuffer()
{
	//如果连bufer都还没有呢，必然需要render
	ObjectBuffer * pBufer = DataBufferManager::Instance().GetBuf(GetObjBufferID());
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
