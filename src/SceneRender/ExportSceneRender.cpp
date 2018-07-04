#include "stdafx.h"
#include "ExportSceneRender.h"
#include "DXEngineDeviceManager.h"

BASEGRAPH_SCENE_RENDER_API int DX3DEngine_Initialize3DDevice(int nWidth, int nHeight)
{
	return DXEngineDeviceManager::Instance().Initialize3D(nWidth, nHeight);
}

BASEGRAPH_SCENE_RENDER_API int DX3DEngine_GetMainDevice()
{
	return DXEngineDeviceManager::Instance().GetMainDevice();
}

BASEGRAPH_SCENE_RENDER_API void DX3DEngine_ReleaseMainDevice()
{
	DXEngineDeviceManager::Instance().ReleaseMainDevice();
}
