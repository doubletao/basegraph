#include "stdafx.h"
#include "ExportBaseCodec.h"
#include "ImageDecoder.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
bool BASEGRAPH_BASECODEC_API BaseCodec_QueryImageSupport(IN LPCTSTR strExt, IN bool bDecode)
{
	return ImageDecoder::QueryImageSupport(strExt, bDecode);
}

LPCTSTR BASEGRAPH_BASECODEC_API BaseCodec_EnumSuportImageType(bool bAddAllFile, bool bTerminated)
{
	return ImageDecoder::EnumSuportImageType(bAddAllFile, bTerminated);
}

bool BASEGRAPH_BASECODEC_API BaseCodec_GetImgInfo(IN LPCTSTR szFileName, OUT int & xStart, OUT int & yStart, OUT int & Width, OUT int & Height, int nLayer)
{
	ImageDecoder imgDec;
	if (imgDec.Open(szFileName))
	{
		if (imgDec.GetImgInfo(xStart, yStart, Width, Height, nLayer))
		{
			return true;
		}
	}
	return false;
}

bool BASEGRAPH_BASECODEC_API BaseCodec_ReadImage(IN LPCTSTR szFileName, IN int nWidth, IN int nHeight, OUT byte * pBuffer, IN int nPixSize)
{
	ImageDecoder imgDec;
	if (imgDec.Open(szFileName))
	{
		return imgDec.ReadImage(nWidth, nHeight, pBuffer, nPixSize);
	}
	return false;
}

bool BASEGRAPH_BASECODEC_API BaseCodec_ReadImage(IN LPCTSTR szFileName, IN int nWidth, IN int nHeight, OUT DWORD * pBuffer, IN int nPixSize)
{
	return BaseCodec_ReadImage(szFileName, nWidth, nHeight, (byte *)pBuffer, nPixSize);
}
