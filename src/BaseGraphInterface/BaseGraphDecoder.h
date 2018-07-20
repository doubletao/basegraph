#pragma once

#ifdef BASEGRAPH_INTERFACE_EXPORTS
#define BASEGRAPH_INTERFACE_API __declspec(dllexport)
#else
#define BASEGRAPH_INTERFACE_API __declspec(dllimport)
#ifdef BASEGRAPH_AUTO_LOAD
#ifdef _DEBUG
#	pragma comment(lib, "BaseGraphInterfaceD.lib")
#else
#	pragma comment(lib, "BaseGraphInterface.lib")
#endif
#endif // BASEGRAPH_AUTO_LOAD
#endif // BASEGRAPH_INTERFACE_EXPORTS

BASEGRAPH_INTERFACE_API int DX3DEngine_Initialize3DDevice(int nWidth, int nHeight);
BASEGRAPH_INTERFACE_API int DX3DEngine_GetMainDevice();
BASEGRAPH_INTERFACE_API void DX3DEngine_ReleaseMainDevice();