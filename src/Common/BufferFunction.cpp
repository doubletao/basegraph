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

BOOL BufferFunction::Buffer_Fix_CutRange(IN OUT int &nDesX, IN OUT int &nDesY, IN OUT int &nDesWidth, IN OUT int &nDesHeight, IN int nDesBufferWidth, IN int nDesBufferHeight, IN OUT int &nSrcX, IN OUT int &nSrcY, IN OUT int &nSrcWidth, IN OUT int &nSrcHeight, IN int nSrcBufferWidth, IN int nSrcBufferHeight, IN RECT * prcDesClip)
{
	return 0;
}
