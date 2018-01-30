#include "stdafx.h"
#include "ExportBaseCodec.h"

bool BASEGRAPH_BASECODEC_API BaseCodec_QueryImageSupport(IN LPCTSTR strExt, IN bool bDecode)
{
	return false;
}

LPCTSTR BASEGRAPH_BASECODEC_API BaseCodec_EnumSuportImageType(bool bAddAllFile, bool bTerminated)
{
	return _T("");
}

bool BASEGRAPH_BASECODEC_API BaseCodec_GetImgInfo(IN LPCTSTR szFileName, OUT int & xStart, OUT int & yStart, OUT int & Width, OUT int & Height, int nLayer)
{
	return false;
}

bool BASEGRAPH_BASECODEC_API BaseCodec_ReadImage(IN LPCTSTR szFileName, IN int nWidth, IN int nHeight, OUT byte * pBuffer, IN int nPixSize)
{
	return false;
}
