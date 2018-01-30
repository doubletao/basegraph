#pragma once
#ifdef BASEGRAPH_DATARENDER_EXPORTS
#define BASEGRAPH_DATARENDER_API __declspec(dllexport)
#else
#define BASEGRAPH_DATARENDER_API __declspec(dllimport)
#endif // BASEGRAPH_DATARENDER_EXPORTS

#ifdef BASEGRAPH_AUTO_LOAD
#ifdef _DEBUG
#	pragma comment(lib, "DataRenderD.lib")
#else
#	pragma comment(lib, "DataRender.lib")
#endif
#endif // BASEGRAPH_AUTO_LOAD
