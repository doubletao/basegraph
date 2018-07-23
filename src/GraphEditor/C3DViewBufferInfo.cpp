#include "stdafx.h"
#include "C3DViewBufferInfo.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
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
	m_nWidth = nScreenWidth;
	m_nHeight = nScreenHeight;

	assert(DataRender_Query2DBuffer(m_nWidth, m_nHeight, m_nEditBuffer));
	DataRender_Clear2DBuf(m_nEditBuffer, 0);
	assert(DataRender_Query2DBuffer(m_nWidth, m_nHeight, m_nUIBuffer));
	DataRender_Clear2DBuf(m_nUIBuffer, 0);
	assert(DataRender_Query2DBuffer(m_nWidth, m_nHeight, m_nCtrlBuffer));
	DataRender_Clear2DBuf(m_nCtrlBuffer, 0);

	m_hEditTexture = DXTextureManager_QueryTexture(DXTexture_Normal2DTex, 1);
	m_hUITexture = DXTextureManager_QueryTexture(DXTexture_Normal2DTex, 1);
	m_hCtrlTexture = DXTextureManager_QueryTexture(DXTexture_Normal2DTex, 1);

	return true;
}

bool C3DViewBufferInfo::Release()
{
	if (m_nEditBuffer)
	{
		assert(DataRender_UnQuery2DBuffer(m_nEditBuffer));
	}
	if (m_nUIBuffer)
	{
		DataRender_UnQuery2DBuffer(m_nUIBuffer);
	}
	if (m_nCtrlBuffer)
	{
		DataRender_UnQuery2DBuffer(m_nCtrlBuffer);
	}

	DXTextureManager_UnQueryTexture(m_hEditTexture);
	DXTextureManager_UnQueryTexture(m_hUITexture);
	DXTextureManager_UnQueryTexture(m_hCtrlTexture);

	return true;
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
