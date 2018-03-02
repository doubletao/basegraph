#include "stdafx.h"
#include "ExportSceneRender.h"
#include "DXEngineDeviceManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

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
