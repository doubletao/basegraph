#pragma once

#ifdef BASEGRAPH_BASECODEC_EXPORTS
#define BASEGRAPH_BASECODEC_API __declspec(dllexport)
#else
#define BASEGRAPH_BASECODEC_API __declspec(dllimport)
#ifdef BASEGRAPH_AUTO_LOAD
#ifdef _DEBUG
#	pragma comment(lib, "BaseCodecD.lib")
#else
#	pragma comment(lib, "BaseCodec.lib")
#endif
#endif // BASEGRAPH_AUTO_LOAD
#endif // BASEGRAPH_BASECODEC_EXPORTS

bool BASEGRAPH_BASECODEC_API BaseCodec_QueryImageSupport(IN LPCTSTR strExt, IN bool bDecode = true);
LPCTSTR BASEGRAPH_BASECODEC_API BaseCodec_EnumSuportImageType(bool bAddAllFile = true, bool bTerminated = true);
bool BASEGRAPH_BASECODEC_API BaseCodec_GetImgInfo(IN LPCTSTR szFileName, OUT int& xStart, OUT int& yStart, OUT int& Width, OUT int& Height, int nLayer = -1);
bool BASEGRAPH_BASECODEC_API BaseCodec_ReadImage(IN LPCTSTR szFileName, IN int nWidth, IN int nHeight, OUT byte * pBuffer, IN int nPixSize = 4);