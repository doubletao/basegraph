#include "stdafx.h"
#include "DXEngineDeviceManager.h"
#include "SceneRender\DXEngineManagerAPI.h"

DXEngineDeviceManager::DXEngineDeviceManager()
	: m_nDevice(0)
	, m_nSceneWidth(0)
	, m_nSceneHeight(0)
{
}


DXEngineDeviceManager::~DXEngineDeviceManager()
{
}

DXEngineDeviceManager & DXEngineDeviceManager::Instance()
{
	static DXEngineDeviceManager inst;
	return inst;
}

int DXEngineDeviceManager::Initialize3D(int nWidth, int nHeight)
{
	//保证只创建一遍
	if (m_nDevice == 0)
	{
		m_nDevice = DX3DEngine_CreateDevice(nWidth, nHeight);
		m_nSceneWidth = nWidth;
		m_nSceneHeight = nHeight;
	}
	return m_nDevice;
}

int DXEngineDeviceManager::GetMainDevice()
{
	return m_nDevice;
}
