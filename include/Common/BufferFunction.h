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
};

