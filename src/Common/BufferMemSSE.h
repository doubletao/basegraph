#pragma once
#include "Common\BufferCommonDef.h"

class BufferMemSSE
{
private:
	BufferMemSSE();
	~BufferMemSSE();
public:
	// 全局二维增益表
	static __declspec(align(16)) DWORD*	g_pdwMixGainTable;		// SA / (SA + DA - SA * DA / 255) 缩放 2^15 - 1（需要补偿）

	static BOOL Buffer_CreateGainTable();
	static BOOL Buffer_ReleaseGainTable();

	// 两块 BGRA 32 位内存的固定大小操作
	static BOOL Buffer_BGRA_Fix_SSE(OUT DWORD* pDes,				// 目标 Buffer
		IN int nDesX,					// 目标起始点
		IN int nDesY,
		IN int nDesWidth,				// 目标拷贝大小
		IN int nDesHeight,
		IN int nDesBufferWidth,		// 目标 Buffer 大小
		IN int nDesBufferHeight,
		IN DWORD* pSrc,				// 源 Buffer
		IN int nSrcX,					// 源起始点
		IN int nSrcY,
		IN int nSrcWidth,				// 源拷贝大小
		IN int nSrcHeight,
		IN int nSrcBufferWidth,		// 源 Buffer 大小
		IN int nSrcBufferHeight,
		IN BUFFER_MODE eMode,		// 操作类型
		IN BUFFER_PARITY_STYLE eParity,	// 奇偶场类型
		IN RECT* prcDesClip = NULL,
		IN int n = 1,
		IN int k = 1);	// 目标裁减矩形（可选项，为 NULL 时表示目标全部有效）

	// 两块 BGRA 32 位内存的双线性插值算法操作
	static BOOL Buffer_BGRA_BLStretch_SSE(OUT DWORD* pDes,				// 目标 Buffer
		IN int nDesX,				// 目标起始点
		IN int nDesY,
		IN int nDesWidth,			// 目标拷贝大小
		IN int nDesHeight,
		IN int nDesBufferWidth,		// 目标 Buffer 大小
		IN int nDesBufferHeight,
		IN DWORD* pSrc,				// 源 Buffer
		IN int nSrcX,				// 源起始点
		IN int nSrcY,
		IN int nSrcWidth,			// 源拷贝大小
		IN int nSrcHeight,
		IN int nSrcBufferWidth,		// 源 Buffer 大小
		IN int nSrcBufferHeight,
		IN BUFFER_MODE eMode,		// 操作类型
		IN BUFFER_PARITY_STYLE eParity,	// 奇偶场类型
		IN RECT* prcDesClip = NULL);	// 目标裁减矩形（可选项，为 NULL 时表示目标全部有效）

	static BOOL Buffer_Parity_Range(IN OUT int &nDesY,			// 目标起点
		OUT int &nDesAddWidth,		// 目标 Buffer 增量
		IN int nDesBufferWidth,		// 目标 Buffer 大小
		IN int nDesClipHeight,
		IN OUT int &nSrcY,			// 源起点
		OUT int &nSrcAddWidth,		// 源 Buffer 增量
		IN int nSrcBufferWidth,		// 源 Buffer 大小
		IN int nSrcBufferHeight,
		OUT int &nLineInc,			// 行增量
		IN BUFFER_PARITY_STYLE eParity,
		IN int n = 1,
		IN int k = 1);	// 奇偶场类型
};

