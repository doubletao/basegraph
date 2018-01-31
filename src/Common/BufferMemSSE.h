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

	//////////////////////////////////////////////////////////////////////////
	// 不缩放时的范围剪切，可选目标裁减矩形。
	// 如果裁减矩形为空指针，则表示裁减区覆盖整个目标。
	// 范围无效时返回 FALSE 有效时返回 TRUE
	//////////////////////////////////////////////////////////////////////////
	static BOOL Buffer_Fix_CutRange(IN OUT int &nDesX,			// 目标起点
		IN OUT int &nDesY,
		IN OUT int &nDesWidth,			// 目标范围
		IN OUT int &nDesHeight,
		IN int nDesBufferWidth,		// 目标 Buffer 大小
		IN int nDesBufferHeight,
		IN OUT int &nSrcX,			// 源起点
		IN OUT int &nSrcY,
		IN OUT int &nSrcWidth,			// 源范围
		IN OUT int &nSrcHeight,
		IN int nSrcBufferWidth,		// 源 Buffer 大小
		IN int nSrcBufferHeight,
		IN RECT* prcDesClip = NULL);	// 目标裁减矩形（可选项）

	static BOOL Buffer_Stretch_CutRange(IN OUT int &nDesX,			// 目标起点
		IN OUT int &nDesY,
		IN OUT int &nDesWidth,			// 目标范围
		IN OUT int &nDesHeight,
		IN int nDesBufferWidth,		// 目标 Buffer 大小
		IN int nDesBufferHeight,
		IN OUT int &nSrcX,			// 源起点
		IN OUT int &nSrcY,
		IN OUT int &nSrcWidth,			// 源范围
		IN OUT int &nSrcHeight,
		IN int nSrcBufferWidth,		// 源 Buffer 大小
		IN int nSrcBufferHeight,
		IN int nEdgeOffset,			// 边缘偏移量，用于计算缩放比例
		OUT float &fScaleWidth,		// 缩放比例
		OUT float &fScaleHeight,
		OUT float &fSrcStartX,			// 源浮点起点坐标
		OUT float &fSrcStartY,
		OUT int &nDesAddWidth,			// 目标 Buffer 增量
		OUT int &nLineIndex,			// 行起始
		OUT int &nLineInc,			// 行增量
		IN BUFFER_PARITY_STYLE eParity,	// 奇偶场类型
		IN RECT* prcDesClip = NULL);	// 目标裁减矩形（可选项，为 NULL 时表示目标全部有效）

	//////////////////////////////////////////////////////////////////////////
	// 双线性插值权重与映射索引的计算
	//////////////////////////////////////////////////////////////////////////
	static void Buffer_Power_BLStretch(OUT DWORD *pPowerX,		// X 方向上的权重表与索引表
		OUT int *pMapX,
		OUT DWORD *pPowerY,		// Y 方向上的权重表与索引表
		OUT int *pMapY,
		IN int nDesWidth,		// 目标范围
		IN int nDesHeight,
		IN int nSrcX,			// 源起点
		IN int nSrcY,
		IN int nSrcWidth,		// 源范围
		IN int nSrcHeight,
		IN int nSrcBufferWidth,	// 源 Buffer 大小
		IN int nSrcBufferHeight,
		IN float fScaleWidth,	// 缩放比例
		IN float fScaleHeight,
		IN float fSrcStartX,		// 源浮点起点坐标
		IN float fSrcStartY,
		IN int nPixelBytes);		// 一个像素的字节宽度
};

