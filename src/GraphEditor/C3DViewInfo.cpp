#include "stdafx.h"
#include "C3DViewInfo.h"

C3DViewInfo::C3DViewInfo()
	: m_nSceneWidth(0)
	, m_nSceneHeight(0)
	, m_nWndWidth(0)
	, m_nWndHeight(0)
	, m_nSceneOffsetX(0)
	, m_nSceneOffsetY(0)
	, m_nScrollX(0)
	, m_nScrollY(0)
	, m_fScale(0.0f)
	, m_hSwapChain(0)
{
}

C3DViewInfo::~C3DViewInfo()
{
}
