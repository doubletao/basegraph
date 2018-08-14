#pragma once
#include "Common\BufferCommonDef.h"
#include "Common\2DBuffer.h"

//////////////////////////////////////////////////////////////////////////
// 缓存变换时的裁减与边界计算、双线性权重与索引计算、双线性插值计算
//////////////////////////////////////////////////////////////////////////
typedef struct tag_tpinnervertex {
	float	x, y, z;// w;
} SYInnerVertex, *LPSYINNERVERTEX;

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

	//////////////////////////////////////////////////////////////////////////
	// 两块 BGRA 32 位内存的平均插值算法操作
	//////////////////////////////////////////////////////////////////////////
	static BOOL Buffer_BGRA_MeanStretch_SSE(OUT DWORD* pDes,				// 目标 Buffer
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
		IN RECT* prcDesClip = NULL);	// 目标裁减矩形（可选项，为 NULL 时表示目标全部有效）

		//////////////////////////////////////////////////////////////////////////
		// 对一块 BGRA 32 位内存进行固定大小的键值操作
		//////////////////////////////////////////////////////////////////////////
	static BOOL Buffer_BGRA_Key_Fix_SSE(IN OUT DWORD* pdwBuf,			// 目标
		IN int nDesX,					// 目标操作区域
		IN int nDesY,
		IN int nDesWidth,
		IN int nDesHeight,
		IN int nDesBufWidth,			// 目标缓存大小
		IN int nDesBufHeight,
		IN LPBYTE pbyKey,				// 键缓存
		IN int nKeyX,					// 键操作区域
		IN int nKeyY,
		IN int nKeyWidth,
		IN int nKeyHeight,
		IN int nKeyBufWidth,			// 键幅面大小
		IN int nKeyBufHeight,
		IN int nKeyPitch,				// 键缓存一行的字节宽度
		IN Common_KEY_MODE eMode,			// 键操作模式
		IN BUFFER_PARITY_STYLE eParity,		// 奇偶场类型
		IN RECT* prcDesClip = NULL,
		IN int n = 1,
		IN int k = 1);	// 裁剪区域（可选项）

		//////////////////////////////////////////////////////////////////////////
		// 对一块 BGRA 32 位内存进行双线性插值算法的键值操作
		//////////////////////////////////////////////////////////////////////////
	static BOOL Buffer_BGRA_Key_BLStretch_SSE(IN OUT DWORD* pdwBuf,				// 目标
		IN int nDesX,						// 目标操作区域
		IN int nDesY,
		IN int nDesWidth,
		IN int nDesHeight,
		IN int nDesBufWidth,				// 目标缓存大小
		IN int nDesBufHeight,
		IN LPBYTE pbyKey,					// 键缓存
		IN int nKeyX,						// 键操作区域
		IN int nKeyY,
		IN int nKeyWidth,
		IN int nKeyHeight,
		IN int nKeyBufWidth,				// 键幅面大小
		IN int nKeyBufHeight,
		IN int nKeyPitch,					// 键缓存一行的字节宽度
		IN Common_KEY_MODE eMode,				// 键操作模式
		IN BUFFER_PARITY_STYLE eParity,		// 奇偶场类型
		IN RECT* prcDesClip = NULL);		// 裁剪区域（可选项）

		//////////////////////////////////////////////////////////////////////////
		// 两块 8 位键内存的固定大小操作
		//////////////////////////////////////////////////////////////////////////
	static BOOL Buffer_Key_Fix_SSE(IN OUT LPBYTE pbyDes,			// 目标键
		IN int nDesX,					// 目标操作区域
		IN int nDesY,
		IN int nDesWidth,
		IN int nDesHeight,
		IN int nDesBufWidth,				// 目标缓存大小
		IN int nDesBufHeight,
		IN int nDesKeyPitch,				// 目标键缓存一行的字节宽度
		IN LPBYTE pbySrc,				// 源键
		IN int nSrcX,					// 源操作区域
		IN int nSrcY,
		IN int nSrcWidth,
		IN int nSrcHeight,
		IN int nSrcBufWidth,				// 键幅面大小
		IN int nSrcBufHeight,
		IN int nSrcKeyPitch,				// 源键缓存一行的字节宽度
		IN Common_KEY_MODE eMode,			// 键操作模式
		IN BUFFER_PARITY_STYLE eParity,		// 奇偶场类型
		IN RECT* prcDesClip = NULL,
		IN int n = 1,
		IN int k = 1);		// 裁剪区域（可选项）

		//////////////////////////////////////////////////////////////////////////
		// 两块 8 位键内存的双线性插值算法操作
		//////////////////////////////////////////////////////////////////////////
	static BOOL Buffer_Key_BLStretch_SSE(IN OUT LPBYTE pbyDes,			// 目标键
		IN int nDesX,					// 目标操作区域
		IN int nDesY,
		IN int nDesWidth,
		IN int nDesHeight,
		IN int nDesBufWidth,			// 目标缓存大小
		IN int nDesBufHeight,
		IN int nDesKeyPitch,			// 目标键缓存一行的字节宽度
		IN LPBYTE pbySrc,				// 源键
		IN int nSrcX,					// 源操作区域
		IN int nSrcY,
		IN int nSrcWidth,
		IN int nSrcHeight,
		IN int nSrcBufWidth,			// 键幅面大小
		IN int nSrcBufHeight,
		IN int nSrcKeyPitch,			// 源键缓存一行的字节宽度
		IN Common_KEY_MODE eMode,			// 键操作模式
		IN BUFFER_PARITY_STYLE eParity,	// 奇偶场类型
		IN RECT* prcDesClip = NULL);	// 裁剪区域（可选项）

		//////////////////////////////////////////////////////////////////////////
		// 缓存变换时的裁减与边界计算、双线性权重与索引计算、双线性插值计算
		//////////////////////////////////////////////////////////////////////////

	static BOOL Buffer_Trans_CalcBorder(IN CommonBufferDef32* pDesBuf,			// 目标缓存
		IN int nOffsetLeft,					// 目标缓存在投影平面坐标系下的位置
		IN int nOffsetTop,
		IN LPSYINNERVERTEX pvVertexes,		// 投影顶点组（顺时针：(0,0),(1,0),(1,1),(0,1)）
		OUT LPSYINNERVERTEX pvLeftEdgeUVW,	// 边界左边缘的 UVW 值
		OUT LPSYINNERVERTEX pvDeltaUVW,		// 每行内的 UVW 增量
		OUT DWORD* *ppdwDesLeftEdge,		// 目标缓存中左边界的起始位置
		OUT int* pnLinePixels,				// 每行拥有的像素个数
		OUT int* pnMaxLinePixels);			// 最长行的像素个数

	static void Buffer_Trans_CalcPowerAndIndex_BL(IN LPSYINNERVERTEX pvLeftEdgeUVW,	// 行左边缘的 UVW 值
		IN LPSYINNERVERTEX pvDeltaUVW,	// 行内的 UVW 增量
		IN int nLinePixels,				// 一行要处理的像素个数
		IN int nSrcWidth,					// 源的大小
		IN int nSrcHeight,
		IN int nSrcBufWidth,				// 源缓存的真实宽度
		OUT DWORD* pdwSrcIndex,			// 双线性纹理映射时源的四个索引
		OUT float* pfPowerX,				// 水平方向的权重
		OUT float* pfPowerY,				// 垂直方向的权重
		OUT float* pfPowerXY);			// 权重之积

	static void Buffer_Trans_Interpolate_BL(OUT DWORD* pdwDesLeftEdge,		// 目标行的起始位置
		IN int nLinePixels,				// 一行要处理的像素个数
		IN DWORD* *ppdwSrcStart,		// 源缓存的起始位置
		IN DWORD* pdwSrcIndex,			// 双线性纹理映射时源的四个索引
		IN float* pfPowerX,				// 水平方向的权重
		IN float* pfPowerY,				// 垂直方向的权重
		IN float* pfPowerXY);			// 权重之积
};

