//////////////////////////////////////////////////////////////////////////
// 描述：	涉及大片内存运算处理的接口，采用 SSE2 进行算法优化
//
//////////////////////////////////////////////////////////////////////////
#ifndef __SY_MEMAPI_INC__
#define __SY_MEMAPI_INC__
#include "Common\2DBuffer.h"
#include "Common\VectorDef.h"
//////////////////////////////////////////////////////////////////////////
// 不缩放时的范围剪切，可选目标裁减矩形。
// 如果裁减矩形为空指针，则表示裁减区覆盖整个目标。
// 范围无效时返回 FALSE 有效时返回 TRUE
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL SYFix_CutRange(IN OUT int &nDesX,			// 目标起点
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
//////////////////////////////////////////////////////////////////////////
// 缩放时的范围剪切
// 范围无效时返回 FALSE 有效时返回 TRUE
//////////////////////////////////////////////////////////////////////////
BOOL SYStretch_CutRange(IN OUT int &nDesX,			// 目标起点
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
// 填充时对目标范围的裁剪
// 范围无效时返回 FALSE 有效时返回 TRUE
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL SYFill_ClipRange(IN OUT int &nX,	// 填充起点
									IN OUT int &nY,
									IN OUT int &nWidth,	// 填充大小
									IN OUT int &nHeight,
									IN int nBufWidth,		// 缓存大小
									IN int nBufHeight,
									IN RECT* prcClip);	// 裁剪区域（可选项）

//////////////////////////////////////////////////////////////////////////
// 使用给定的颜色值替换32位灰度图的颜色buffer中RGB三个通道的值
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void SY_GrayscaleColrReplace(IN OUT DWORD* pBuffer,	//RGBA
												 IN int len,			//DWORD count
												 IN DWORD color);		//RGBA

//////////////////////////////////////////////////////////////////////////
// 使用给定的权值对32位图的颜色buffer中RGBA通道进行加权
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void SY_ColrWeightMix(IN OUT DWORD* pBuffer,	//RGBA
										  IN int len,				//DWORD count
										  IN DWORD weight);			//Rw Gw Bw Aw

//////////////////////////////////////////////////////////////////////////
// 算法分类：根据运算的目的、方式对接口进行分类，以便统一管理
//		（1）类似于系统 API 的一行 memory 简单操作
//		（2）对大片 32 位内存的拷贝、混叠、变换等操作
//		（3）对大片内存进行带掩码的操作
//		（4）利用上述的简单操作合成的特殊操作，如生成特殊的内存
//		（5）用于支持算法优化的一些系统操作接口
//		（6）特殊的运算接口
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//		（1）类似于系统 API 的一行 memory 简单操作
//////////////////////////////////////////////////////////////////////////

// 一行 Buffer 的拷贝操作
BASEGRAPH_COMMON_API void SYMemcpy_Line(OUT void *des,		// 目标
									   IN void *src,		// 源
									   IN int nlen);		// 字节长度
// 一行 Buffer 的反向拷贝操作
BASEGRAPH_COMMON_API void SYMemcpy_RevChar(OUT void *des,			// 目标
										  IN void *src,		// 源
										  IN int nlen);		// 字节长度
BASEGRAPH_COMMON_API void SYMemcpy_RevDWORD(OUT DWORD *pDes,		// 目标
										   IN DWORD *pSrc,	// 源
										   IN int nsize);		// 双字的个数
// 一列 Buffer 的拷贝
// 如果 bUpDown 为 TRUE 则从 pDes 开始填充下面的列，否则填充上面的列。
// 内部不做越界的安全检测，要求调用者自己注意。
BASEGRAPH_COMMON_API void SYMemcpy_ColumnDWORD(OUT DWORD* pDes,		// 目标起点
											  IN int nHeight,		// 拷贝的列高（所指向的内存必须有效）
											  IN int nPitch,		// 目标一行字节宽度
											  IN DWORD* pSrc,		// 源一行缓存（长度必须不小于nHeight）
											  IN BOOL bUpDown);	// 从上往下拷贝顺序的标志，否则反向

// 一行 Buffer 的设值操作
BASEGRAPH_COMMON_API void SYMemset_LineChar(OUT void *des,		// 目标
										   IN char value,		// 字节值
										   IN int nlen);		// 字节长度
BASEGRAPH_COMMON_API void SYMemset_LineDWORD(OUT DWORD *pDes,		// 目标
											IN DWORD value,	// 双字值
											IN int nsize);	// 双字的个数
// 兼容旧接口
#define SY_DMemSet		SYMemset_LineDWORD
// 一行 Buffer 的Alpha值替换操作，包含掩码颜色的使用。
BASEGRAPH_COMMON_API void SYMemset_LineAlpha(IN OUT DWORD *pDes,		// 目标
											IN char alpha,		// alpha 值
											IN int nsize,			// 双字长度
											IN SY_FILL_MODE eMode,	// 填充模式
											IN DWORD pdwMaskCr);	// 掩码颜色值（依赖模式的可选项）

// 一行 Buffer 的翻转操作
BASEGRAPH_COMMON_API void SYMemrev_LineChar(IN OUT void *buf,		// 缓存
										   IN int nlen);		// 字节长度
BASEGRAPH_COMMON_API void SYMemrev_LineDWORD(IN OUT DWORD *pBuf,	// 缓存
											IN int nsize);	// 双字的个数


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//		（2）对大片 32 位内存的拷贝、混叠、变换等操作
//////////////////////////////////////////////////////////////////////////

// 一行 Buffer 的 MIX 操作
BASEGRAPH_COMMON_API void SYBGRA32_LineMix(IN OUT void *des,			// 目标
										  IN void *src,			// 源
										  IN int na16,			// 宽度除以 16 的商
										  IN int nm16);			// 宽度除以 16 的余数
BASEGRAPH_COMMON_API void SYBGRA32_LineMix_NotSrc(IN OUT void *des,	// 目标
												 IN void *src,	// 源
												 IN int na16,		// 宽度除以 16 的商
												 IN int nm16);	// 宽度除以 16 的余数

// 第三方权重的填充操作：des = c0 * f + c1 * (1 - f) = (c0 - c1) * f + c1
BASEGRAPH_COMMON_API void SYBGRA32_PowerFill(OUT DWORD* pDes,	// 目标
											IN DWORD dwC0,		// 起点颜色
											IN DWORD dwC1,		// 终点颜色
											IN short* pPower,		// 权重被放大 14 位
											IN int nWidth);		// 像素个数
// 更快速的权重填充，不需要计算权重表，权重仍然被放大 14 位
BASEGRAPH_COMMON_API void SYBGRA32_PowerFill_Q(OUT DWORD* pDes,		// 目标
											  IN DWORD dwC0,		// 起点颜色
                                              IN DWORD dwC1,		// 终点颜色
											  IN float fPrev,		// 起点前一个权重
											  IN float fDelta,	// 像素间的权重增量
											  IN int nWidth);		// 像素个数
// 采用递归法对浮点颜色进行插值
// 公式 f(x) = A * (c1 - c0) * (x - B) + C
BASEGRAPH_COMMON_API void SYBGRA32_LineSYAR_Recursive(OUT DWORD* pDes,		// 目标
												   IN float* pfCr0,	// 起点颜色
												   IN float* pfCr1,	// 终点颜色
												   IN float fX,		// X 的起始点（像素索引）
												   IN float fCoefA,	// 系数 A
												   IN float fCoefB,	// 系数 B
												   IN float* pfCoefC,	// 系数 C
												   IN int nWidth);	// 像素个数

// 新接口（ Buffer 信息存在结构里）

// 采用双线性拉伸
BASEGRAPH_COMMON_API BOOL SY_BitBlt_N(IN OUT SYBufferDef32* pDesBuf,	// 目标 Buffer
									 IN int nDesWidth,			// 目标操作大小
									 IN int nDesHeight,
									 IN SYBufferDef32* pSrcBuf,	// 源 Buffer
									 IN int nSrcWidth,			// 源操作大小
									 IN int nSrcHeight,
									 IN BUFFER_MODE eMode,		// 混合操作类型
									 IN BUFFER_PARITY_STYLE eParity);	// 奇偶场类型

// 采用双线性拉伸
BASEGRAPH_COMMON_API BOOL SY_StretchBlt_N(IN OUT SYBufferDef32* pDesBuf,	// 目标 Buffer
										 IN int nDesWidth,			// 目标操作大小
										 IN int nDesHeight,
										 IN SYBufferDef32* pSrcBuf,	// 源 Buffer
										 IN int nSrcWidth,			// 源操作大小
										 IN int nSrcHeight,
										 IN BUFFER_MODE eMode,		// 混合操作类型
										 IN BUFFER_PARITY_STYLE eParity);	// 奇偶场类型

// 采用权重总和的均值拉伸
BASEGRAPH_COMMON_API BOOL SY_StretchBlt_Mean(IN OUT SYBufferDef32* pDesBuf,	// 目标 Buffer
											IN int nDesX,
											IN int nDesY,
											IN int nDesWidth,			// 目标操作大小
											IN int nDesHeight,
											IN SYBufferDef32* pSrcBuf,	// 源 Buffer
											IN int nSrcX,
											IN int nSrcY,
											IN int nSrcWidth,			// 源操作大小
											IN int nSrcHeight,
											IN BUFFER_MODE eMode,	// 混合操作类型
											IN BUFFER_PARITY_STYLE eParity);// 奇偶场类型

// 采用双线性拉伸
BASEGRAPH_COMMON_API BOOL SY_BitBlt(IN OUT DWORD* pdwDesBuffer,		// 目标缓存
								   IN int nDesX,					// 目标操作起点
								   IN int nDesY,
								   IN int nDesWidth,				// 目标操作大小
								   IN int nDesHeight,
								   IN int nDesBufferWidth,			// 目标缓存大小
								   IN int nDesBufferHeight,
								   IN DWORD* pdwSrcBuffer,			// 源缓存
								   IN int nSrcX,					// 源操作起点
								   IN int nSrcY,
								   IN int nSrcWidth,				// 源操作大小
								   IN int nSrcHeight,
								   IN int nSrcBufferWidth,			// 源缓存大小
								   IN int nSrcBufferHeight,
								   IN BUFFER_MODE eMode,		// 混合操作类型
								   IN BUFFER_PARITY_STYLE eParity,		// 奇偶场类型
								   IN RECT* prcDesClip = NULL,
								   IN int n = 1,
								   IN int k = 1);		// 目标裁减矩形（可选项，为 NULL 时表示目标全部有效）

// 采用双线性拉伸
BASEGRAPH_COMMON_API BOOL SY_StretchBlt(IN OUT DWORD* pdwDesBuffer,		// 目标缓存
									   IN int nDesX,					// 目标操作起点
									   IN int nDesY,
									   IN int nDesWidth,				// 目标操作大小
									   IN int nDesHeight,
									   IN int nDesBufferWidth,			// 目标缓存大小
									   IN int nDesBufferHeight,
									   IN DWORD* pdwSrcBuffer,			// 源缓存
									   IN int nSrcX,					// 源操作起点
									   IN int nSrcY,
									   IN int nSrcWidth,				// 源操作大小
									   IN int nSrcHeight,
									   IN int nSrcBufferWidth,			// 源缓存大小
									   IN int nSrcBufferHeight,
									   IN BUFFER_MODE eMode,		// 混合操作类型
									   IN BUFFER_PARITY_STYLE eParity,		// 奇偶场类型
									   IN RECT* prcDesClip = NULL);	// 目标裁减矩形（可选项，为 NULL 时表示目标全部有效）

// 扫描方式定义
#define	SK_PARITY_BOTH 0
#define 	SK_PARITY_ODD  1
#define 	SK_PARITY_EVEN 2
BASEGRAPH_COMMON_API BOOL rgba_mean_copy(OUT DWORD* pDest,
								   IN int nDestX,
								   IN int nDestY,
								   IN int nDestWidth,
								   IN int nDestHeight,
								   IN int nDestBufferWidth,
								   IN int nDestBufferHeight,
								   IN DWORD* pSrc,
								   IN int nSrcX,
								   IN int nSrcY,
								   IN int nSrcWidth,
								   IN int nSrcHeight,
								   IN int nSrcBufferWidth,
								   IN int nSrcBufferHeight,
								   IN int eParity);

BASEGRAPH_COMMON_API BOOL SY_BufferTransform(IN OUT SYBufferDef32* pDesBuf,			// 目标缓存
											IN int nOffsetLeft,				// 目标缓存在投影平面坐标系下的位置
											IN int nOffsetTop,
											IN SYBufferDef32* pSrcBuf,			// 源缓存
											IN int nSrcWidth,					// 源操作区域大小（相对于缓存起点而言）
											IN int nSrcHeight,
											IN void* pvfXYZW,					// 源操作区域的四个角点经过一系列变换后的顶点数据（必须有四个，顺序为(0,0),(w,0),(w,h),(h,0)）
											IN SYBufferDef32* pMaskBuf = NULL);	// 掩码缓存
BASEGRAPH_COMMON_API BOOL SY_BufferTransformLinear(IN OUT SYBufferDef32* pDesBuf,	// 目标缓存
												  IN int nOffsetLeft,			// 目标缓存在投影平面坐标系下的位置
												  IN int nOffsetTop,
												  IN SYBufferDef32* pSrcBuf,	// 源缓存
												  IN int nSrcWidth,			// 源操作区域大小（相对于缓存起点而言）
												  IN int nSrcHeight,
												  IN vector4f* pvfXYZW,		// 源操作区域的四个角点经过一系列变换后的顶点数据（必须有四个，顺序为(0,0),(w,0),(w,h),(h,0)）
												  IN SYBufferDef32* pMaskBuf);	// 掩码缓存

// 使用某个值填充内存中的某块区域，可指定裁剪区域。
BASEGRAPH_COMMON_API void SYBGRA32_FillRect(IN OUT DWORD* pdwBuf,	// 操作缓存
										   IN int nX,		// 操作区域
										   IN int nY,
										   IN int nWidth,
										   IN int nHeight,
										   IN int nBufWidth,	// 缓存大小
										   IN int nBufHeight,
										   IN RECT* prcClip,	// 裁剪区域（可选项）
										   IN DWORD dwValue);	// 填充值

BASEGRAPH_COMMON_API void SYBGRA32_ReplaceA_MulA(IN OUT DWORD* pdwBuf,	// 操作缓存
											 IN int nX,			// 操作区域
											 IN int nY,
											 IN int nWidth,
											 IN int nHeight,
											 IN int nBufWidth,	// 缓存大小
											 IN int nBufHeight,
											 IN RECT* prcClip,	// 裁剪区域（可选项）
											 DWORD* psrcBuf,
											 IN int nSrcX,
											 IN int nSrcY,
											 IN int nSrcBufWidth,
											 IN int nSrcBufHeight,
											 IN BOOL bInvert);	// 填充值

BASEGRAPH_COMMON_API void SYBGRA32_ReplaceRGB_MulA(IN OUT DWORD* pdwBuf,	// 操作缓存
												  IN int nX,			// 操作区域
												  IN int nY,
												  IN int nWidth,
												  IN int nHeight,
												  IN int nBufWidth,	// 缓存大小
												  IN int nBufHeight,
												  IN RECT* prcClip,	// 裁剪区域（可选项）
												  IN DWORD dwValue);	// 填充值

BASEGRAPH_COMMON_API void SYBGRA32_ReplaceRGB_MulA(IN OUT DWORD* pdwBuf,	// 操作缓存
												  IN int nX,			// 操作区域
												  IN int nY,
												  IN int nWidth,
												  IN int nHeight,
												  IN int nBufWidth,	// 缓存大小
												  IN int nBufHeight,
												  IN RECT* prcClip,	// 裁剪区域（可选项）
												  DWORD* psrcBuf);	// 填充值

BASEGRAPH_COMMON_API void SYBGRA32_ReplaceRGB_MulA_Width_HArray(IN OUT DWORD* pdwBuf,	// 操作缓存
															   IN int nX,		// 操作区域
															   IN int nY,
															   IN int nWidth,
															   IN int nHeight,
															   IN int nBufWidth,	// 缓存大小
															   IN int nBufHeight,
															   IN RECT* prcClip,	// 裁剪区域（可选项）
															   IN DWORD* dwValue);// 填充值

BASEGRAPH_COMMON_API void SYBGRA32_ReplaceRGB(IN OUT DWORD* pdwBuf,	// 操作缓存
										   IN int nX,			// 操作区域
										   IN int nY,
										   IN int nWidth,
										   IN int nHeight,
										   IN int nBufWidth,		// 缓存大小
										   IN int nBufHeight,
										   IN RECT* prcClip,		// 裁剪区域（可选项）
										   IN DWORD dwValue);		// 填充值

// 填充某块区域中的 Alpha 值，可使用掩码颜色，可指定裁剪区域。
// 填充模式中有支持缩放 Alpha 的模式，在这些模式下，cAlpha / 255 表示一个比例。
BASEGRAPH_COMMON_API void SYBGRA32_FillAlpha(IN OUT DWORD* pdwBuf,		// 目标
											IN int nX,			// 填充区域
											IN int nY,
											IN int nWidth,
											IN int nHeight,
											IN int nBufWidth,		// 缓存大小
											IN int nBufHeight,
											IN RECT* prcClip,		// 裁剪区域（可选项）
											IN char cAlpha,		// Alpha值
											IN SY_FILL_MODE eMode,	// 填充模式
											IN DWORD pdwMaskCr);	// 掩码颜色值（依赖模式的可选项）

BASEGRAPH_COMMON_API void SYBGRA32_FillAlpha_With_H(IN OUT DWORD* pdwBuf,		// 目标
												   IN int nX,			// 填充区域
												   IN int nY,
												   IN int nWidth,
												   IN int nHeight,
												   IN int nBufWidth,		// 缓存大小
												   IN int nBufHeight,
												   IN RECT* prcClip,		// 裁剪区域（可选项）
												   IN DWORD* cAlpha,		// Alpha值
												   IN SY_FILL_MODE eMode,	// 填充模式
												   IN DWORD pdwMaskCr);	// 掩码颜色值（依赖模式的可选项）

#if 0
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//		（3）对大片内存进行带掩码的操作
//////////////////////////////////////////////////////////////////////////

// 对 32 位内存进行键值操作
BASEGRAPH_COMMON_API BOOL SYBGRA32_ProcKey(IN OUT DWORD* pdwBuf,			// 目标
										  IN int nDesX,				// 目标操作区域
										  IN int nDesY,
										  IN int nDesWidth,
										  IN int nDesHeight,
										  IN int nDesBufWidth,		// 目标缓存大小
										  IN int nDesBufHeight,
										  IN LPBYTE pbyKey,			// 键缓存
										  IN int nKeyX,				// 键操作区域
										  IN int nKeyY,
										  IN int nKeyWidth,
										  IN int nKeyHeight,
										  IN int nKeyBufWidth,		// 键幅面大小
										  IN int nKeyBufHeight,
										  IN int nKeyPitch,			// 键缓存一行的字节宽度
										  IN SY_KEY_MODE eMode,		// 键操作模式
										  IN BUFFER_PARITY_STYLE eParity,	// 奇偶场类型
										  IN RECT* prcDesClip = NULL);	// 裁剪区域（可选项）

// 实现一块 8 位键值内存与一个标量间的操作
BASEGRAPH_COMMON_API BOOL SYKey_ProcScalar(IN OUT LPBYTE pbyKey,		// 键目标
										  IN int nX,				// 填充区域
										  IN int nY,
										  IN int nWidth,
										  IN int nHeight,
										  IN int nKeyBufWidth,	// 键幅面大小
										  IN int nKeyBufHeight,
										  IN int nKeyPitch,		// 键缓存一行的字节宽度
										  IN RECT* prcClip,		// 裁剪区域（可选项）
										  IN char cAlpha,			// Alpha值
										  IN SY_KEY_MODE eMode);	// 键操作模式
// 实现两块 8 位键值内存间的操作
BASEGRAPH_COMMON_API BOOL SYKey_ProcBuffer(IN OUT LPBYTE pbyDes,			// 目标键
										  IN int nDesX,				// 目标操作区域
										  IN int nDesY,
										  IN int nDesWidth,
										  IN int nDesHeight,
										  IN int nDesBufWidth,		// 目标缓存大小
										  IN int nDesBufHeight,
										  IN int nDesKeyPitch,		// 目标键缓存一行的字节宽度
										  IN LPBYTE pbySrc,			// 源键
										  IN int nSrcX,				// 源操作区域
										  IN int nSrcY,
										  IN int nSrcWidth,
										  IN int nSrcHeight,
										  IN int nSrcBufWidth,		// 键幅面大小
										  IN int nSrcBufHeight,
										  IN int nSrcKeyPitch,		// 源键缓存一行的字节宽度
										  IN SY_KEY_MODE eMode,		// 键操作模式
										  IN BUFFER_PARITY_STYLE eParity,	// 奇偶场类型
										  IN RECT* prcDesClip = NULL);	// 裁剪区域（可选项）


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//		（4）利用上述的简单操作合成的特殊操作，如生成特殊的内存
//////////////////////////////////////////////////////////////////////////

// 生成交叉颜色块的透明色背景
BASEGRAPH_COMMON_API void SY_MakeTranslucentBack(OUT DWORD* pBuf,			// 背景 Buffer
												IN int nBufWidth,		// Buffer 大小
												IN int nBufHeight,
												IN int nBlockSize,	// 相间颜色块大小
												IN DWORD crOddBlock,	// 奇数列颜色块颜色
												IN DWORD crEvenBlock);	// 偶数列颜色块颜色
//////////////////////////////////////////////////////////////////////////
//		（5）用于支持算法优化的一些系统操作接口
//////////////////////////////////////////////////////////////////////////
// 获取 CPU 时钟周期计数器
BASEGRAPH_COMMON_API unsigned __int64 SY_GetCycleCount();

//////////////////////////////////////////////////////////////////////////
//		（6）特殊的运算接口
//////////////////////////////////////////////////////////////////////////

// 计算一等差序列数与某一特定数的完全平方和（特定数为负则计算完全平方差）
BASEGRAPH_COMMON_API void SY_SerialPerfectSquare(OUT float* pResult,		// 计算结果
												IN int nSerialCount,	// 序列长度
												IN float fStart,		// 序列起始数
												IN float fStep,		// 等差
												IN float fGiven);		// 特定的数

// 计算一等差序列数与某一特定数的完全平方和（特定数为负则计算完全平方差）
// 增加额外功能，对和进行线性缩放，并计算四次方结果
BASEGRAPH_COMMON_API void SY_SerialPerfectSquareEx(OUT float* pResSqr,			// 平方和结果
												  OUT float* pResQuartic,	// 四次方结果
												  OUT float* pResSum,		// 缩放和结果
												  IN int nSerialCount,	// 序列长度
												  IN float fStart,		// 序列起始数
												  IN float fStep,			// 等差
												  IN float fGiven,		// 特定的数
												  IN float fScaling);		// 和的缩放系数

// 计算一非负序列进行特定偏移后的平方根，根据表被放大的系数计算出索引，并查表
BASEGRAPH_COMMON_API void SY_OffsetSqrtIndexLookup(OUT void* pBuf,				// 查表结果（32 位表，要求与表的类型相同）
												  IN int nSerialCount,	// 序列长度
												  IN float* pSerial,		// 序列
												  IN float fOffset,		// 特定偏移
												  IN void* pTable,		// 表（32 位表）
												  IN float fScaling);		// 表的大小相对于序列长度的放大倍数

// 使用泰勒级数展开求弧长，取前三项，以其整型值为索引进行查表
// 弧长公式 S = PI/2 * R - R * arcsin(X / R)
// 泰勒展开级数：arcsin(t) = t + 1/6 * t^3 + 3/40 * t^5 + ……	(-1 < t < 1)
// 采用级数展开求解，会带来精度的损失，因此将圆弧从左中点起按角度进行分段（顺时针角度）
// (45°,135°) 范围内使用公式
//
//					  R * (x - xc)[ 149/120 * (x - xc)^4 + 13/6 * (y - yc)^2 * (x - xc)^2 + (y - yc)^4 ]
// Sx = PI/2 * R  +  ------------------------------------------------------------------------------------
//						                     [(x - xc)^2 + (y - yc)^2]^(5/2)
//    = Cx + R * (x - xc) * Mx
//
// 而与之相对称的下半圆弧段 (225°,315°) 则需要替换常数项 Cx = 3 * PI/2 * R 并使用 (xc - x)
// (0°,45°) 范围内使用公式
//
//		R * (yc - y)[ 149/120 * (y - yc)^4 + 13/6 * (y - yc)^2 * (x - xc)^2 + (x - xc)^4 ]
// Sy = ------------------------------------------------------------------------------------
//						           [(x - xc)^2 + (y - yc)^2]^(5/2)
//    = Cy + R * (yc - y) * My
//
// (135°,225°) 范围内则令 Cy = PI * R 并使用 (y - yc)
// (315°,360°) 范围内则令 Cy = 2 * PI * R 并使用 (y - yc)
// 为了最大化加速对 (x - xc)^4、(x - xc)^2、(x - xc) * R、(y - yc)^4、(y - yc)^2、(y - yc) * R 进行缓存
BASEGRAPH_COMMON_API void SY_TylerArcIndexLookupX(OUT void* pBuf,				// 查表结果（32 位表，要求与表的类型相同）
												 IN int nSerialCount,		// 序列长度
												 IN float* pQuartic,		// 四次方序列
												 IN float* pSquare,		// 完全平方和序列
												 IN float* pSum,			// 和序列
												 IN float fOffsetQ,		// (y - yc)^4
												 IN float fOffsetSq,		// (y - yc)^2
												 IN void* pTable,			// 表（32 位表）
												 IN float fConst);		// 计算式最后添加的常量（定积分的起点值）
BASEGRAPH_COMMON_API void SY_TylerArcIndexLookupY(OUT void* pBuf,				// 查表结果（32 位表，要求与表的类型相同）
												 IN int nSerialCount,		// 序列长度
												 IN float* pQuartic,		// 四次方序列
												 IN float* pSquare,		// 完全平方和序列
												 IN float fOffsetQ,		// (y - yc)^4
												 IN float fOffsetSq,		// (y - yc)^2
												 IN float fSum,			// R * (y - yc)
												 IN void* pTable,			// 表（32 位表）
												 IN float fConst);		// 计算式最后添加的常量（定积分的起点值）

// 翻转符号，将负值变正值或反过来（当输入与输出指针相同时，认为直接翻转）
BASEGRAPH_COMMON_API void SY_NegtiveFloat(OUT float* pDes,		// 目标
										 IN float* pSrc,		// 源
										 IN int nlen);		// 数组长度

// 求解一整型等差序列经过两种偏移后的新序列 offset_1 + a 与 offset_2 - a
// 特殊情况：通过指定圆心的直线 y = x 与 y = -x 在指定大小区域内的点的 X 序列值，序列以 y 为索引
BASEGRAPH_COMMON_API void SY_SerialOffset2(OUT int* pDes1,		// offset_1 + a
										  OUT int* pDes2,		// offset_2 - a
										  IN int nSerialCount,// 序列长度
										  IN int nMin,		// 最小值
										  IN int nMax,		// 最大值
										  IN float fOffset1,	// 对于 y = x 为 xc - yc
										  IN float fOffset2,	// 对于 y = -x 为 xc + yc
										  IN int nStart,		// 序列起点
										  IN int nStep);		// 序列步长
//////////////////////////////////////////////////////////////////////////
// 旧接口：速度较慢，需要重新优化
//////////////////////////////////////////////////////////////////////////
void CALLBACK SY_CalMask(IN OUT BYTE *pbyMask,
				IN int nStartX, 
				IN int nStartY, 
				IN int nMaskWidth,
				IN int nMaskHeight,
				IN BYTE *pnOperator,
				IN int nOperatorWidth, 
				IN int nOperatorHeight,
				IN int nMainOperatorX,
				IN int nMainOperatorY);

// 源Buffer平铺拷贝到目标Buffer中
LRESULT CALLBACK SY_TileCopy(IN OUT DWORD* pdwDesBuf,
			     IN int nDesX,
			     IN int nDesY,
			     IN int nDesWidth,
			     IN int nDesHeight,
			     IN int nDesBufWidth,
			     IN int nDesBufHeight,
			     IN DWORD* pdwSrcBuf,
			     IN int nSrcX,
			     IN int nSrcY,
			     IN int nSrcWidth,
			     IN int nSrcHeight,
			     IN int nSrcBufWidth,
			     IN int nSrcBufHeight);

void CALLBACK SY_CreateTableFor_H1920To1440();
void CALLBACK SY_DeleteTableFor_H1920To1440();
void CALLBACK SY_LinearScale_H1920To1440(OUT BYTE* pDes,
									IN int nDesWidth,
                                    IN int nDesPitch,
                                    IN BYTE* pSrc,
                                    IN int nSrcWidth,
                                    IN int nSrcPitch,
                                    IN int nHeight);
// YUYV拉伸拷贝
LRESULT CALLBACK SY_StretchBlt_YUYV(IN OUT BYTE* pbyDesBuffer, 
                                IN int nDesPitch,
                                IN int nDesBufferWidth,
                                IN int nDesBufferHeight,
                                IN BYTE* pbySrcBuffer,
                                IN int nSrcPitch,
                                IN int nSrcBufferWidth,		// 像素点数
                                IN int nSrcBufferHeight);
BOOL yuv_yyuuvv2yuv(BYTE* pDesBuffer,BYTE* pSrcField,int nA16,int nMod16);
BOOL yuv_yuv2yyuuvv(BYTE* pDesBuffer,BYTE* pSrcField,int nA8,int nMod8);
void yuv_uyvy2yuv8(BYTE* puyvy,BYTE* py,BYTE* pu,BYTE* pv,int nA16,int nM16);
void yuv_yuyv2yuv8(BYTE* puyvy,BYTE* py,BYTE* pu,BYTE* pv,int nA16,int nM16);
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API LRESULT Gauss_RLE(IN OUT void* pInOutBuf,	// 目标缓冲区:包含输入缓冲区、输出缓冲区
									  IN int nInPlane,	// 象素位数
									  IN int nInX,		// 输入缓冲区起点坐标 
									  IN int nInY, 
									  IN int nInWidth,	// 输入缓冲区宽、高
									  IN int nInHeight,
									  IN int nInOutWidth,	// 目标缓冲区宽、高
									  IN int nInOutHeight,						
									  IN int nOutPlane,
									  IN int& nOutX,		// 输出缓冲区起点坐标
									  IN int& nOutY, 
									  IN int& nOutWidth,	// 输出缓冲区宽、高
									  IN int& nOutHeight,
									  IN float fHorizon,
									  IN float fVertical,
									  IN int nRefPlane);
BASEGRAPH_COMMON_API void SY_GuassianBlur(LPBYTE lpDes,LPSIZE pSize,int degrees );
#endif
//////////////////////////////////////////////////////////////////////////
#endif