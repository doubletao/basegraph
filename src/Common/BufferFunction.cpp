#include "stdafx.h"
#include "Common\BufferFunction.h"
#include "BufferMemSSE.h"

BufferFunction::BufferFunction()
{
}


BufferFunction::~BufferFunction()
{
}

BOOL BufferFunction::BufferBitblt(DWORD * pdwDesBuffer, int nDesX, int nDesY, int nDesWidth, int nDesHeight, int nDesBufferWidth, int nDesBufferHeight, DWORD * pdwSrcBuffer, int nSrcX, int nSrcY, int nSrcWidth, int nSrcHeight, int nSrcBufferWidth, int nSrcBufferHeight, BUFFER_MODE eMode, BUFFER_PARITY_STYLE eParity, RECT * prcDesClip, int n, int k)
{
	if (nSrcWidth == nDesWidth && nSrcHeight == nDesHeight)		// 固定大小
	{
		return BufferMemSSE::Buffer_BGRA_Fix_SSE(pdwDesBuffer,
			nDesX,
			nDesY,
			nDesWidth,
			nDesHeight,
			nDesBufferWidth,
			nDesBufferHeight,
			pdwSrcBuffer,
			nSrcX,
			nSrcY,
			nSrcWidth,
			nSrcHeight,
			nSrcBufferWidth,
			nSrcBufferHeight,
			eMode,
			eParity,
			prcDesClip, n, k);
	}
	else		// 缩放
	{
		return BufferMemSSE::Buffer_BGRA_BLStretch_SSE(pdwDesBuffer,
			nDesX,
			nDesY,
			nDesWidth,
			nDesHeight,
			nDesBufferWidth,
			nDesBufferHeight,
			pdwSrcBuffer,
			nSrcX,
			nSrcY,
			nSrcWidth,
			nSrcHeight,
			nSrcBufferWidth,
			nSrcBufferHeight,
			eMode,
			eParity,
			prcDesClip);
	}
	return FALSE;
}

BOOL BufferFunction::WriteBitmap(LPCTSTR ptszFileName, int iWidth, int iHeight, int iDepth, BYTE * pData)
{
	BITMAPFILEHEADER stFileHdr;
	stFileHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	stFileHdr.bfSize = sizeof(BITMAPFILEHEADER);
	stFileHdr.bfType = 0x4d42;
	BITMAPINFOHEADER stInfoHdr;
	memset(&stInfoHdr, 0, sizeof(BITMAPINFOHEADER));
	stInfoHdr.biSize = sizeof(BITMAPINFOHEADER);
	stInfoHdr.biPlanes = 1;
	stInfoHdr.biWidth = iWidth;
	stInfoHdr.biHeight = iHeight;
	stInfoHdr.biBitCount = iDepth;
	stInfoHdr.biSizeImage = stInfoHdr.biWidth * stInfoHdr.biHeight * stInfoHdr.biBitCount / 8;
	FILE* fp = NULL;
	_tfopen_s(&fp, ptszFileName, _T("wb"));
	if (!fp)
		return FALSE;
	fwrite(&stFileHdr, 1, sizeof(BITMAPFILEHEADER), fp);
	fwrite(&stInfoHdr, 1, sizeof(BITMAPINFOHEADER), fp);
	size_t nSize = fwrite(pData, 1, stInfoHdr.biSizeImage, fp);
	fclose(fp);
	return TRUE;
}

