#include "stdafx.h"
#include "C3DViewBufferInfo.h"

C3DViewBufferInfo::C3DViewBufferInfo()
	: m_nEditBuffer(0)
	, m_hEditTexture(0)
	, m_nUIBuffer(0)
	, m_hUITexture(0)
	, m_nCtrlBuffer(0)
	, m_hCtrlTexture(0)
	, m_nWidth(0)
	, m_nHeight(0)
{
}

C3DViewBufferInfo::~C3DViewBufferInfo()
{
	Release();
}

bool C3DViewBufferInfo::Initialize(int nScreenWidth, int nScreenHeight)
{
	return false;
}

bool C3DViewBufferInfo::Release()
{
	return false;
}

int C3DViewBufferInfo::GetControlMemBuffer()
{
	return m_nCtrlBuffer;
}

int C3DViewBufferInfo::GetControlTexture()
{
	return m_hCtrlTexture;
}

int C3DViewBufferInfo::GetEditMemBuffer()
{
	return m_nEditBuffer;
}

int C3DViewBufferInfo::GetEditTexture()
{
	return m_hEditTexture;
}

int C3DViewBufferInfo::GetUIMemBuffer()
{
	return m_nUIBuffer;
}

int C3DViewBufferInfo::GetUITexture()
{
	return m_hUITexture;
}
