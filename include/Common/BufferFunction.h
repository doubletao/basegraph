#pragma once
#include "Common\BufferCommonDef.h"

class BASEGRAPH_COMMON_API BufferFunction
{
private:
	BufferFunction();
	~BufferFunction();
public:
	static BOOL BufferBitblt(DWORD* pdwDesBuffer,		// 目标缓存
		int nDesX,					// 目标操作起点
		int nDesY,
		int nDesWidth,				// 目标操作大小
		int nDesHeight,
		int nDesBufferWidth,			// 目标缓存大小
		int nDesBufferHeight,
		DWORD* pdwSrcBuffer,			// 源缓存
		int nSrcX,					// 源操作起点
		int nSrcY,
		int nSrcWidth,				// 源操作大小
		int nSrcHeight,
		int nSrcBufferWidth,			// 源缓存大小
		int nSrcBufferHeight,
		BUFFER_MODE eMode,		// 混合操作类型
		BUFFER_PARITY_STYLE eParity,		// 奇偶场类型
		RECT* prcDesClip = NULL,
		int n = 1,
		int k = 1);

	static BOOL WriteBitmap(LPCTSTR ptszFileName, int iWidth, int iHeight, int iDepth, BYTE* pData);
};

