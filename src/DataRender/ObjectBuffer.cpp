#include "stdafx.h"
#include "ObjectBuffer.h"
#include <assert.h>

static const int BUFFER_AREA_RATIO = 2;//面积系数，当前设为2，即reserve面积小于等于实际使用面积的2倍

ObjectBuffer::ObjectBuffer()
{
	ReleaseBuffer();
}


ObjectBuffer::~ObjectBuffer()
{
	ReleaseBuffer();
}

bool ObjectBuffer::InitializeBuffer(int nWidth, int nHeight)
{
	assert(nWidth);
	assert(nHeight);
	ReleaseBuffer();
	m_pBuffer = new byte[nWidth * nHeight];
	if (m_pBuffer)
	{
		m_nReserveWidth = nWidth;
		m_nReserveHeight = nHeight;
		return true;
	}
	return false;
}

void ObjectBuffer::ReleaseBuffer()
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_nReserveWidth = 0;
	m_nReserveHeight = 0;
	m_bUsed = false;
	delete[] m_pBuffer;
	m_pBuffer = nullptr;
}

bool ObjectBuffer::SetUsed(int nWidth, int nHeight)
{
	assert(nWidth);
	assert(nHeight);
	if (m_nReserveWidth >= nWidth
		&& m_nReserveHeight >= nHeight)
	{
		m_nWidth = nWidth;
		m_nHeight = nHeight;
		m_bUsed = true;
	}
	return false;
}

void ObjectBuffer::ReleaseUse()
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_bUsed = true;
}

bool ObjectBuffer::IsUsed()
{
	return m_bUsed;
}

bool ObjectBuffer::FitIn(int nReqWidth, int nReqHeight)
{
	assert(nReqWidth);
	assert(nReqHeight);
	//当前的fit标准为宽高合适且面积在2倍以内
	if (!m_bUsed
		&& m_nReserveWidth >= nReqWidth
		&& m_nReserveHeight >= nReqHeight
		&& m_nReserveWidth * m_nReserveHeight <= nReqWidth * nReqHeight * BUFFER_AREA_RATIO)
	{
		return true;
	}
	return false;
}

byte * ObjectBuffer::GetBuffer()
{
	assert(m_pBuffer);
	return m_pBuffer;
}

int ObjectBuffer::GetWidth()
{
	assert(m_nWidth);
	return m_nWidth;
}

int ObjectBuffer::GetHeight()
{
	assert(m_nHeight);
	return m_nHeight;
}
