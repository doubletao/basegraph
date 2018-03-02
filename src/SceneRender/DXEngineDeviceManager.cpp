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
	ReleaseMainDevice();
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

void DXEngineDeviceManager::ReleaseMainDevice()
{
	if (m_nDevice > 0)
	{
		DX3DEngine_ReleaseDevice(m_nDevice);
		m_nDevice = 0;
		m_nSceneWidth = 0;
		m_nSceneHeight = 0;
	}
}
