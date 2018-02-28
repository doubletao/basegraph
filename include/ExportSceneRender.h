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

//#include "SceneRender\SkinDlgWnd.h"
