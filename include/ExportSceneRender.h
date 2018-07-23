#pragma once

#ifdef BASEGRAPH_SCENE_RENDER_EXPORTS
#define BASEGRAPH_SCENE_RENDER_API __declspec(dllexport)
#else
#define BASEGRAPH_SCENE_RENDER_API __declspec(dllimport)
#ifdef BASEGRAPH_AUTO_LOAD
#ifdef _DEBUG
#	pragma comment(lib, "SceneRenderD.lib")
#else
#	pragma comment(lib, "SceneRender.lib")
#endif
#endif // BASEGRAPH_AUTO_LOAD
#endif // BASEGRAPH_SCENE_RENDER_EXPORTS

#include "SceneRender\3DInterSection.h"
#include "SceneRender\3DSpaceConvert.h"
#include "SceneRender\DXEffectManager.h"
#include "SceneRender\DXEngineDataDef.h"
#include "SceneRender\DXEngineManagerAPI.h"
#include "SceneRender\DXRenderEngineDX10Api.h"
#include "SceneRender\DXRenderEngineSoftAPI.h"

BASEGRAPH_SCENE_RENDER_API int DX3DEngine_Initialize3DDevice(int nWidth, int nHeight);
BASEGRAPH_SCENE_RENDER_API int DX3DEngine_GetMainDevice();
BASEGRAPH_SCENE_RENDER_API void DX3DEngine_ReleaseMainDevice();