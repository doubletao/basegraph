#pragma once
#ifdef BASEGRAPH_DATARENDER_EXPORTS
#define BASEGRAPH_DATARENDER_API __declspec(dllexport)
#else
#define BASEGRAPH_DATARENDER_API __declspec(dllimport)
#ifdef BASEGRAPH_AUTO_LOAD
#ifdef _DEBUG
#	pragma comment(lib, "DataRenderD.lib")
#else
#	pragma comment(lib, "DataRender.lib")
#endif
#endif // BASEGRAPH_AUTO_LOAD
#endif // BASEGRAPH_DATARENDER_EXPORTS

