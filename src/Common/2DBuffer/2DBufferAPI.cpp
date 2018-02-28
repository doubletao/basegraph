/********************************************************************
	摘要:	对缓冲区各种操作的加速算法的实现
*********************************************************************/
#include "stdafx.h"
#include "Common\2DBuffer.h"
#include "Common\2DBufferAPI.h"
#include "..\BufferSSE2.h"
#include "..\BufferMemSSE.h"
#include "stdio.h"
#include "TCHAR.h"
#include "..\BufferRipRender.h"
#include <assert.h>
#include "Common\ColorDef.h"
//////////////////////////////////////////////////////////////////////////
// 不缩放时的范围剪切，可选目标裁减矩形。
// 如果裁减矩形为空指针，则表示裁减区覆盖整个目标。
// 范围无效时返回 FALSE 有效时返回 TRUE
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL SYFix_CutRange(IN OUT int &nDesX,				// 目标起点
										IN OUT int &nDesY,
										IN OUT int &nDesWidth,			// 目标范围
										IN OUT int &nDesHeight,
										IN int nDesBufferWidth,			// 目标 Buffer 大小
										IN int nDesBufferHeight,
										IN OUT int &nSrcX,				// 源起点
										IN OUT int &nSrcY,
										IN OUT int &nSrcWidth,			// 源范围
										IN OUT int &nSrcHeight,
										IN int nSrcBufferWidth,			// 源 Buffer 大小
										IN int nSrcBufferHeight,
										IN RECT* prcDesClip/* = NULL*/)	// 目标裁减矩形（可选项）
{
	// 边界有效性检测
	if (nDesBufferWidth <= 0 || nDesBufferHeight <= 0
		|| nSrcBufferWidth <= 0	|| nSrcBufferHeight <= 0
		|| nDesX >= nDesBufferWidth || nDesY >= nDesBufferHeight
		|| nSrcX >= nSrcBufferWidth || nSrcY >= nSrcBufferHeight
		|| nDesWidth <= 0 || nDesHeight <= 0)
		return FALSE;

	int nLeft(0), nTop(0), nRight(nDesBufferWidth), nBottom(nDesBufferHeight);

	if (prcDesClip != NULL)		// 如果有裁剪区域
	{
		nLeft = (nLeft > prcDesClip->left) ? nLeft : prcDesClip->left;
		nRight = (nRight < prcDesClip->right) ? nRight : prcDesClip->right;
		nTop = (nTop > prcDesClip->top) ? nTop : prcDesClip->top;
		nBottom = (nBottom < prcDesClip->bottom) ? nBottom : prcDesClip->bottom;
		// 判断区域是否有效
		if (nLeft >= nRight || nTop >= nBottom || nDesX >= nRight || nDesY >= nBottom)
			return FALSE;
	}
	// 剪切超出范围
	if (nDesX < nLeft)		// 目标左边界超出，拷贝范围进行右缩进
	{
		int nDelta = nLeft - nDesX;

		nDesWidth -= nDelta; nSrcX += nDelta; nDesX = nLeft;
	}
	if (nDesY < nTop)		// 目标上边界超出，拷贝范围进行下缩进
	{
		int nDelta = nTop - nDesY;

		nDesHeight -= nDelta; nSrcY += nDelta; nDesY = nTop;
	}
	if (nSrcX < 0)		// 源左边界超出，拷贝范围进行右缩进
	{
		nDesWidth += nSrcX; nDesX -= nSrcX; nSrcX = 0;
	}
	if (nSrcY < 0)		// 源上边界超出，拷贝范围进行下缩进
	{
		nDesHeight += nSrcY; nDesY -= nSrcY; nSrcY = 0;
	}

	int nTmpWidth = nDesWidth + nSrcX;		// 考虑源的右边界
	int nTmpHeight = nDesHeight + nSrcY;	// 考虑源的下边界

	if (nTmpWidth > nSrcBufferWidth)	// 源右边界超出，拷贝范围进行左缩进
	{
		nDesWidth -= (nTmpWidth - nSrcBufferWidth);
	}
	if (nTmpHeight > nSrcBufferHeight)	// 源下边界超出，拷贝范围进行上缩进
	{
		nDesHeight -= (nTmpHeight - nSrcBufferHeight);
	}
	nTmpWidth = nDesWidth + nDesX;		// 考虑目标的右边界
	nTmpHeight = nDesHeight + nDesY;	// 考虑目标的下边界
	if (nTmpWidth > nRight)		// 目标右边界超出，拷贝范围进行左缩进
	{
		nDesWidth -= (nTmpWidth - nRight);
	}
	if (nTmpHeight > nBottom)	// 目标下边界超出，拷贝范围进行上缩进
	{
		nDesHeight -= (nTmpHeight - nBottom);
	}
	if (nDesWidth <= 0 || nDesHeight <= 0)	// 拷贝范围无效
		return FALSE;

	nSrcWidth = nDesWidth;
	nSrcHeight = nDesHeight;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 缩放时的范围剪切
// 范围无效时返回 FALSE 有效时返回 TRUE
//////////////////////////////////////////////////////////////////////////
BOOL SYStretch_CutRange(IN OUT int &nDesX,				// 目标起点
						IN OUT int &nDesY,
						IN OUT int &nDesWidth,			// 目标范围
						IN OUT int &nDesHeight,
						IN int nDesBufferWidth,			// 目标 Buffer 大小
						IN int nDesBufferHeight,
						IN OUT int &nSrcX,				// 源起点
						IN OUT int &nSrcY,
						IN OUT int &nSrcWidth,			// 源范围
						IN OUT int &nSrcHeight,
						IN int nSrcBufferWidth,			// 源 Buffer 大小
						IN int nSrcBufferHeight,
						IN int nEdgeOffset,				// 边缘偏移量，用于计算缩放比例
						OUT float &fScaleWidth,			// 缩放比例
						OUT float &fScaleHeight,
						OUT float &fSrcStartX,			// 源浮点起点坐标
						OUT float &fSrcStartY,
						OUT int &nDesAddWidth,			// 目标 Buffer 增量
						OUT int &nLineIndex,			// 行起始
						OUT int &nLineInc,				// 行增量
						IN BUFFER_PARITY_STYLE eParity,		// 奇偶场类型
						IN RECT* prcDesClip/* = NULL*/)	// 目标裁减矩形（可选项，为 NULL 时表示目标全部有效）
{
	// 边界有效性检测
	if (nDesBufferWidth <= 0 || nDesBufferHeight <= 0
		|| nSrcBufferWidth <= 0 || nSrcBufferHeight <= 0
		|| nDesX >= nDesBufferWidth || nDesY >= nDesBufferHeight
		|| nSrcX >= nSrcBufferWidth || nSrcY >= nSrcBufferHeight)
		return FALSE;

	if (nDesWidth <= nEdgeOffset || nDesHeight <= nEdgeOffset
		|| nSrcWidth <= nEdgeOffset || nSrcHeight <= nEdgeOffset)
		return FALSE;

	int nLeft(0), nTop(0), nRight(nDesBufferWidth), nBottom(nDesBufferHeight);

	if (prcDesClip != NULL)		// 如果有裁剪区域
	{
		nLeft = (nLeft > prcDesClip->left) ? nLeft : prcDesClip->left;
		nRight = (nRight < prcDesClip->right) ? nRight : prcDesClip->right;
		nTop = (nTop > prcDesClip->top) ? nTop : prcDesClip->top;
		nBottom = (nBottom < prcDesClip->bottom) ? nBottom : prcDesClip->bottom;
		// 判断区域是否有效
		if (nLeft >= nRight || nTop >= nBottom || nDesX >= nRight || nDesY >= nBottom)
			return FALSE;
	}
	if ((nDesX + nDesWidth) <= nLeft || (nDesY + nDesHeight) <= nTop
		|| (nSrcX + nSrcWidth) <= 0 || (nSrcY + nSrcHeight) <= 0)
		return FALSE;

	int nOldDesX(nDesX), nOldDesY(nDesY), nOldSrcX(nSrcX), nOldSrcY(nSrcY);
	int nTmpOffset(0), nTmpWidth(0), nTmpHeight(0);

	fScaleWidth = (float)(nSrcWidth - nEdgeOffset) / (nDesWidth - nEdgeOffset);	// 缩放比例
	fScaleHeight = (float)(nSrcHeight - nEdgeOffset) / (nDesHeight - nEdgeOffset);
	// 剪切超出范围
	if (nDesX < nLeft)		// 目标左边界超出，缩放范围进行右缩进
	{
		nTmpOffset = nLeft - nDesX;
		nDesWidth -= nTmpOffset; nDesX = nLeft;
		nTmpOffset = (int)((float)nTmpOffset * fScaleWidth + 0.5f);	// 源的右缩进量
		nSrcWidth -= nTmpOffset; nSrcX += nTmpOffset;
	}
	if (nDesY < nTop)		// 目标上边界超出，缩放范围进行下缩进
	{
		nTmpOffset = nTop - nDesY;
		nDesHeight -= nTmpOffset; nDesY = nTop;
		nTmpOffset = (int)((float)nTmpOffset * fScaleHeight + 0.5f);	// 源的下缩进量
		nSrcHeight -= nTmpOffset; nSrcY += nTmpOffset;
	}
	if (nSrcX < 0)		// 源左边界超出，缩放范围进行右缩进
	{
		nTmpOffset = (int)ceilf((-nSrcX) / fScaleWidth);		// 目标的右缩进量
		nSrcWidth += nSrcX; nSrcX = 0;
		nDesWidth -= nTmpOffset; nDesX += nTmpOffset;
	}
	if (nSrcY < 0)		// 源上边界超出，缩放范围进行下缩进
	{
		nTmpOffset = (int)ceilf((-nSrcY) / fScaleHeight);	// 目标的下缩进量
		nSrcHeight += nSrcY; nSrcY = 0;
		nDesHeight -= nTmpOffset; nDesY += nTmpOffset;
	}
	nTmpWidth = nDesWidth + nDesX;		// 考虑目标的右边界
	nTmpHeight = nDesHeight + nDesY;	// 考虑目标的下边界
	if (nTmpWidth > nRight)		// 目标右边界超出，缩放范围进行左缩进
	{
		nTmpOffset = (nTmpWidth - nRight);	// 目标的左缩进量
		nDesWidth -= nTmpOffset;
		nTmpOffset = (int)(nTmpOffset * fScaleWidth);	// 源的左缩进量
		nSrcWidth -= nTmpOffset;
		if (nSrcWidth <= 0)
			return FALSE;
	}
	if (nTmpHeight > nBottom)	// 目标下边界超出，缩放范围进行上缩进
	{
		nTmpOffset = (nTmpHeight - nBottom);	// 目标的上缩进量
		nDesHeight -= nTmpOffset;
		nTmpOffset = (int)(nTmpOffset * fScaleHeight);	// 源的上缩进量
		nSrcHeight -= nTmpOffset;
		if (nSrcHeight <= 0)
			return FALSE;
	}
	nTmpWidth = nSrcWidth + nSrcX;		// 考虑源的右边界
	nTmpHeight = nSrcHeight + nSrcY;	// 考虑源的下边界
	if (nTmpWidth > nSrcBufferWidth)	// 源右边界超出，缩放范围进行左缩进
	{
		nTmpOffset = (nTmpWidth - nSrcBufferWidth);		// 源的左缩进量
		nSrcWidth -= nTmpOffset;
		nTmpOffset = (int)ceilf(nTmpOffset / fScaleWidth);	// 目标的左缩进量
		nDesWidth -= nTmpOffset;
		if (nDesWidth <= 0)
			return FALSE;
	}
	if (nTmpHeight > nSrcBufferHeight)	// 源下边界超出，拷贝范围进行上缩进
	{
		nTmpOffset = (nTmpHeight - nSrcBufferHeight);	// 源的上缩进量
		nSrcHeight -= nTmpOffset;
		nTmpOffset = (int)ceilf(nTmpOffset / fScaleHeight);	// 目标的上缩进量
		nDesHeight -= nTmpOffset;
		if (nDesHeight <= 0)
			return FALSE;
	}
	// 源起点坐标
	fSrcStartX = fScaleWidth * (nDesX - nOldDesX) + nOldSrcX;
	fSrcStartY = fScaleHeight * (nDesY - nOldDesY) + nOldSrcY;
	// 根据奇偶场类型获得增量
	switch (eParity)
	{
	case BUFFER_PARITY_BOTH:		// 全场
		nDesAddWidth = nDesBufferWidth;
		nLineIndex = 0;
		nLineInc = 1;
		break;
	case BUFFER_PARITY_ODD:			// 奇场
		nLineIndex = 0;
		if ((nDesY % 2) == 1)		// 起点在偶场，则从下一行开始
		{
			nDesY++;
			nLineIndex = 1;
			// 判断范围的有效性
			if (nDesY >= nBottom)
				return FALSE;
		}
		nDesAddWidth = (nDesBufferWidth << 1);
		nLineInc = 2;
		break;
	case BUFFER_PARITY_EVEN:		// 偶场
		nLineIndex = 0;
		if ((nDesY % 2) == 0)		// 起点在奇场，则从下一行开始
		{
			nDesY++;
			nLineIndex = 1;
			// 判断范围的有效性
			if (nDesY >= nBottom)
				return FALSE;
		}
		nDesAddWidth = (nDesBufferWidth << 1);
		nLineInc = 2;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 填充时对目标范围的裁剪
// 范围无效时返回 FALSE 有效时返回 TRUE
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL SYFill_ClipRange(IN OUT int &nX,			// 填充起点
										  IN OUT int &nY,
										  IN OUT int &nWidth,		// 填充大小
										  IN OUT int &nHeight,
										  IN int nBufWidth,			// 缓存大小
										  IN int nBufHeight,
										  IN RECT* prcClip)			// 裁剪区域（可选项）
{
	if (nBufWidth <= 0 || nBufHeight <= 0)
		return FALSE;

	if (nWidth <= 0 || nHeight <= 0 || nX >= nBufWidth || nY >= nBufHeight)
		return FALSE;

	int nLeft(0), nTop(0), nRight(nBufWidth), nBottom(nBufHeight);

	if (prcClip != NULL)		// 如果有裁剪区域
	{
		nLeft = (nLeft > prcClip->left) ? nLeft : prcClip->left;
		nRight = (nRight < prcClip->right) ? nRight : prcClip->right;
		nTop = (nTop > prcClip->top) ? nTop : prcClip->top;
		nBottom = (nBottom < prcClip->bottom) ? nBottom : prcClip->bottom;
		// 判断区域是否有效
		if (nLeft >= nRight || nTop >= nBottom || nX >= nRight || nY >= nBottom)
			return FALSE;
	}
	// 规则化填充区域
	if (nX + nWidth < nLeft || nY + nHeight < nTop)
	{
		return FALSE;
	}
	/*
	purpose:nx改变会带来nwidth的改变，ny同理
	old:
	nX = (nX < nLeft) ? nLeft : nX;
	nY = (nY < nTop) ? nTop : nY;

	End old*/
	//new
	if (nX < nLeft)
	{
		nWidth -= nLeft - nX;
		nX = nLeft;
	}
	if (nY < nTop)
	{
		nHeight -= nTop - nY;
		nY = nTop;
	}
	//End new
	nWidth = ((nX + nWidth) > nRight) ? (nRight - nX) : nWidth;
	nHeight = ((nY + nHeight) > nBottom) ? (nBottom - nY) : nHeight;
	// 再次判断区域的有效性
	if (nWidth <= 0 || nHeight <= 0)
		return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 使用给定的颜色值替换32位灰度图的颜色buffer中RGB三个通道的值
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void SY_GrayscaleColrReplace(IN OUT DWORD* pBuffer, //RGBA
												 IN int len,			//DWORD count
												 IN DWORD color)		//RGBA
{
	const bool using_asm_version = true;
	if(using_asm_version)
	{
		assert(0);
	}
	else
	{
		DWORD* pPixel = pBuffer;
		const DWORD alpha_mask = 0xff << 24;
		const DWORD color_mask = color & (~alpha_mask);
		DWORD cur_pixel = 0;
		for (int i = 0; i < len; ++i)
		{
			*pPixel &= alpha_mask;
			*pPixel |= color_mask;
			++pPixel;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 使用给定的权值对32位图的颜色buffer中RGBA通道进行加权
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void SY_ColrWeightMix(IN OUT DWORD* pBuffer,	//RGBA
										  IN int len,				//DWORD count
										  IN DWORD weight)			//Rw Gw Bw Aw
{
	const bool using_asm_version = true;
	if(using_asm_version)
	{
		assert(0);
	}
	else
	{
		DWORD* pPixel = pBuffer;
		DWORD cur_pixel = 0;
		int r,g,b,a;
		for (int i = 0; i < len; ++i)
		{
			r = GetRValue(*pPixel)*GetRValue(weight)/255;
			g = GetGValue(*pPixel)*GetGValue(weight)/255;
			b = GetBValue(*pPixel)*GetBValue(weight)/255;
			a = GetAValue(*pPixel)*GetAValue(weight)/255;
			*pPixel = RGBA(r,g,b,a);
			++pPixel;
		}
	}
}

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
BASEGRAPH_COMMON_API void SYMemcpy_Line(OUT void *des,	// 目标
									   IN void *src,	// 源
									   IN int nlen)		// 字节长度
{
	memcpy_line(des, src, (nlen >> 4), (nlen & 0x0f));
}

// 一行 Buffer 的反向拷贝操作
BASEGRAPH_COMMON_API void SYMemcpy_RevChar(OUT void *des,	// 目标
										  IN void *src,		// 源
										  IN int nlen)		// 字节长度
{
	memcpy_rev_char(des, src, nlen, (nlen >> 4), (nlen & 0x0f));
}
BASEGRAPH_COMMON_API void SYMemcpy_RevDWORD(OUT DWORD *pDes,	// 目标
										   IN DWORD *pSrc,	// 源
										   IN int nsize)	// 双字的个数
{
	memcpy_rev_dword(pDes, pSrc, (nsize << 2), (nsize >> 2), ((nsize & 0x03) << 2));
}

// 一列 Buffer 的拷贝
// 如果 bUpDown 为 TRUE 则从 pDes 开始填充下面的列，否则填充上面的列。
// 内部不做越界的安全检测，要求调用者自己注意。
BASEGRAPH_COMMON_API void SYMemcpy_ColumnDWORD(OUT DWORD* pDes,		// 目标起点
											  IN int nHeight,		// 拷贝的列高（所指向的内存必须有效）
											  IN int nPitch,		// 目标一行字节宽度
											  IN DWORD* pSrc,		// 源一行缓存（长度必须不小于nHeight）
											  IN BOOL bUpDown)		// 从上往下拷贝顺序的标志，否则反向
{
	memcpy_dword_col(pDes, pSrc, (bUpDown) ? nPitch : -nPitch, (nHeight >> 2), ((nHeight & 0x03) << 2));
}

// 一行 Buffer 的设值操作
BASEGRAPH_COMMON_API void SYMemset_LineChar(OUT void *des,		// 目标
										   IN char value,		// 字节值
										   IN int nlen)			// 字节长度
{
	memset_char_line(des, value, (nlen >> 4), (nlen & 0x0f));
}
BASEGRAPH_COMMON_API void SYMemset_LineDWORD(OUT DWORD *pDes,	// 目标
											IN DWORD value,		// 双字值
											IN int nsize)		// 双字的个数
{
	memset_dword_line(pDes, value, (nsize >> 2), ((nsize & 0x03) << 2));
}

// 一行Buffer的Alpha值替换操作，包含掩码颜色的使用。
BASEGRAPH_COMMON_API void SYMemset_LineAlpha(IN OUT DWORD *pDes,		// 目标
											IN char alpha,			// alpha 值
											IN int nsize,			// 双字长度
											IN SY_FILL_MODE eMode,	// 填充模式
											IN DWORD pdwMaskCr)		// 掩码颜色值（依赖模式的可选项）
{
	switch (eMode)
	{
	case SY_FILL_MASK:		// 只填充颜色为 pdwMaskCr 的像素
		memset_alpha_line_mask(pDes, alpha, pdwMaskCr, (nsize >> 2), ((nsize & 0x03) << 2));
		break;
	case SY_FILL_NOT_MASK:	// 不填充颜色为 pdwMaskCr 的像素
		memset_alpha_line_notmask(pDes, alpha, pdwMaskCr, (nsize >> 2), ((nsize & 0x03) << 2));
		break;
	default:		// 全部填充
		memset_alpha_line_all(pDes, alpha, (nsize >> 2), ((nsize & 0x03) << 2));
		break;
	}
}

// 一行 Buffer 的翻转操作
BASEGRAPH_COMMON_API void SYMemrev_LineChar(IN OUT void *buf,	// 缓存
										   IN int nlen)			// 字节长度
{
	memrev_char_line(buf, ((BYTE*)buf + nlen), (nlen >> 5), ((nlen >> 1) & 0x0f));
}
BASEGRAPH_COMMON_API void SYMemrev_LineDWORD(IN OUT DWORD *pBuf,	// 缓存
											IN int nsize)		// 双字的个数
{
	memrev_dword_line(pBuf, (pBuf + nsize), (nsize >> 3), (((nsize >> 1) & 0x03) << 2));
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//		（2）对大片 32 位内存的拷贝、混叠、变换等操作
//////////////////////////////////////////////////////////////////////////

// 一行 Buffer 的 MIX 操作
BASEGRAPH_COMMON_API void SYBGRA32_LineMix(IN OUT void *des,		// 目标
										  IN void *src,			// 源
										  IN int na16,			// 宽度除以 16 的商
										  IN int nm16)			// 宽度除以 16 的余数
{
	bgra32_line_mix(des, src, na16, nm16, BufferMemSSE::g_pdwMixGainTable);
}
BASEGRAPH_COMMON_API void SYBGRA32_LineMix_NotSrc(IN OUT void *des,		// 目标
												 IN void *src,			// 源
												 IN int na16,			// 宽度除以 16 的商
												 IN int nm16)			// 宽度除以 16 的余数
{
	bgra32_line_mix_notsrc(des, src, na16, nm16, BufferMemSSE::g_pdwMixGainTable);
}

// 第三方权重的填充操作：des = c0 * f + c1 * (1 - f) = (c0 - c1) * f + c1
BASEGRAPH_COMMON_API void SYBGRA32_PowerFill(OUT DWORD* pDes,	// 目标
											IN DWORD dwC0,		// 起点颜色
											IN DWORD dwC1,		// 终点颜色
											IN short* pPower,	// 权重被放大 14 位
											IN int nWidth)		// 像素个数
{
	bgra32_powerfill(pDes, dwC0, dwC1, pPower, (nWidth >> 3), ((nWidth & 0x07) << 2));
}
// 更快速的权重填充，不需要计算权重表，权重仍然被放大 14 位
BASEGRAPH_COMMON_API void SYBGRA32_PowerFill_Q(OUT DWORD* pDes,		// 目标
											  IN DWORD dwC0,		// 起点颜色
											  IN DWORD dwC1,		// 终点颜色
											  IN float fPrev,		// 起点前一个权重
											  IN float fDelta,		// 像素间的权重增量
											  IN int nWidth)		// 像素个数
{
	bgra32_powerfill_q(pDes, dwC0, dwC1, fPrev, fDelta, (nWidth >> 2), ((nWidth & 0x03) << 2));
}

// 采用递归法对浮点颜色进行插值
// 公式 f(x) = A * (x - B) + C		A = 0.5 * (c1 - c0) / (fPEnd - fPStart)
BASEGRAPH_COMMON_API void SYBGRA32_LineSYAR_Recursive(OUT DWORD* pDes,		// 目标
												   IN float* pfCr0,		// 起点颜色
												   IN float* pfCr1,		// 终点颜色
												   IN float fX,			// X 的起始点（像素索引）
												   IN float fCoefA,		// 系数 A
												   IN float fCoefB,		// 系数 B
												   IN float* pfCoefC,	// 系数 C
												   IN int nWidth)		// 像素个数
{
	bgra32_linear_recursive(pDes, pfCr0, pfCr1, fX, fCoefA, fCoefB, pfCoefC, (nWidth >> 2), ((nWidth & 0x03) << 2));
}

// 新接口（ Buffer 信息存在结构里）

// 采用双线性拉伸
BASEGRAPH_COMMON_API BOOL SY_BitBlt_N(IN OUT SYBufferDef32* pDesBuf,	// 目标 Buffer
								     IN int nDesX,
									 IN int nDesY,
									 IN int nDesWidth,					// 目标操作大小
									 IN int nDesHeight,
									 IN SYBufferDef32* pSrcBuf,			// 源 Buffer
									 IN int nSrcX,
									 IN int nSrcY,
									 IN int nSrcWidth,					// 源操作大小
									 IN int nSrcHeight,
									 IN BUFFER_MODE eMode,			// 混合操作类型
									 IN BUFFER_PARITY_STYLE eParity)		// 奇偶场类型
{
	if (nSrcWidth == nDesWidth && nSrcHeight == nDesHeight)		// 固定大小
	{
		return BufferMemSSE::Buffer_BGRA_Fix_SSE(pDesBuf->m_dwBuffer,
							  nDesX,
							  nDesY,
							  nDesWidth,
							  nDesHeight,
							  pDesBuf->m_nWidth,
							  pDesBuf->m_nHeight,
							  pSrcBuf->m_dwBuffer,
							  nSrcX,
							  nSrcY,
							  nSrcWidth,
							  nSrcHeight,
							  pSrcBuf->m_nWidth,
							  pSrcBuf->m_nHeight,
							  eMode,
							  eParity);
	}
	else	// 缩放
	{
		return BufferMemSSE::Buffer_BGRA_BLStretch_SSE(pDesBuf->m_dwBuffer,
									nDesX,
									nDesY,
									nDesWidth,
									nDesHeight,
									pDesBuf->m_nWidth,
									pDesBuf->m_nHeight,
									pSrcBuf->m_dwBuffer,
									nSrcX,
									nSrcY,
									nSrcWidth,
									nSrcHeight,
									pSrcBuf->m_nWidth,
									pSrcBuf->m_nHeight,
									eMode,
									eParity);
	}
	return FALSE;
}

// 采用双线性拉伸
BASEGRAPH_COMMON_API BOOL SY_StretchBlt_N(IN OUT SYBufferDef32* pDesBuf,	// 目标 Buffer
									     IN int nDesX,
										 IN int nDesY,
										 IN int nDesWidth,					// 目标操作大小
										 IN int nDesHeight,
										 IN SYBufferDef32* pSrcBuf,			// 源 Buffer
										 IN int nSrcX,
										 IN int nSrcY,
										 IN int nSrcWidth,					// 源操作大小
										 IN int nSrcHeight,
										 IN BUFFER_MODE eMode,			// 混合操作类型
										 IN BUFFER_PARITY_STYLE eParity)		// 奇偶场类型
{
	if (nSrcWidth != nDesWidth || nSrcHeight != nDesHeight)	// 缩放
	{
		return BufferMemSSE::Buffer_BGRA_BLStretch_SSE(pDesBuf->m_dwBuffer,
									nDesX,
									nDesY,
									nDesWidth,
									nDesHeight,
									pDesBuf->m_nWidth,
									pDesBuf->m_nHeight,
									pSrcBuf->m_dwBuffer,
									nSrcX,
									nSrcY,
									nSrcWidth,
									nSrcHeight,
									pSrcBuf->m_nWidth,
									pSrcBuf->m_nHeight,
									eMode,
									eParity);
	}
	else		// 固定大小
	{
		return BufferMemSSE::Buffer_BGRA_Fix_SSE(pDesBuf->m_dwBuffer,
							 nDesX,
							 nDesY,
							  nDesWidth,
							  nDesHeight,
							  pDesBuf->m_nWidth,
							  pDesBuf->m_nHeight,
							  pSrcBuf->m_dwBuffer,
							  nSrcX,
							  nSrcY,
							  nSrcWidth,
							  nSrcHeight,
							  pSrcBuf->m_nWidth,
							  pSrcBuf->m_nHeight,
							  eMode,
							  eParity);
	}
	return FALSE;
}

// 采用权重总和的均值拉伸
BASEGRAPH_COMMON_API BOOL SY_StretchBlt_Mean(IN OUT SYBufferDef32* pDesBuf,		// 目标 Buffer
											IN int nDesX,
											IN int nDesY,
											IN int nDesWidth,					// 目标操作大小
											IN int nDesHeight,
											IN SYBufferDef32* pSrcBuf,			// 源 Buffer
											IN int nSrcX,
											IN int nSrcY,
											IN int nSrcWidth,					// 源操作大小
											IN int nSrcHeight,
											IN BUFFER_MODE eMode,			// 混合操作类型
											IN BUFFER_PARITY_STYLE eParity)			// 奇偶场类型
{
	if (nSrcWidth != nDesWidth || nSrcHeight != nDesHeight)		// 缩放
	{
		return BufferMemSSE::Buffer_BGRA_MeanStretch_SSE(pDesBuf->m_dwBuffer,
										nDesX,
										nDesY,
									  nDesWidth,
									  nDesHeight,
									  pDesBuf->m_nWidth,
									  pDesBuf->m_nHeight,
									  pSrcBuf->m_dwBuffer,
									  nSrcX,
									  nSrcY,
									  nSrcWidth,
									  nSrcHeight,
									  pSrcBuf->m_nWidth,
									  pSrcBuf->m_nHeight,
									  eMode,
									  eParity);
	}
	else		// 固定大小
	{
		return BufferMemSSE::Buffer_BGRA_Fix_SSE(pDesBuf->m_dwBuffer,
								nDesX,
								nDesY,
							  nDesWidth,
							  nDesHeight,
							  pDesBuf->m_nWidth,
							  pDesBuf->m_nHeight,
							  pSrcBuf->m_dwBuffer,
							  nSrcX,
							  nSrcY,
							  nSrcWidth,
							  nSrcHeight,
							  pSrcBuf->m_nWidth,
							  pSrcBuf->m_nHeight,
							  eMode,
							  eParity);
	}
	return FALSE;
}

// 采用双线性拉伸
BASEGRAPH_COMMON_API BOOL SY_BitBlt(IN OUT DWORD* pdwDesBuffer,		// 目标
								   IN int nDesX,					// 目标起点
								   IN int nDesY,
								   IN int nDesWidth,				// 目标操作区域大小
								   IN int nDesHeight,
								   IN int nDesBufferWidth,			// 目标缓冲区大小
								   IN int nDesBufferHeight,
								   IN DWORD* pdwSrcBuffer,			// 源
								   IN int nSrcX,					// 源起点
								   IN int nSrcY,
								   IN int nSrcWidth,				// 源操作区域大小
								   IN int nSrcHeight,
								   IN int nSrcBufferWidth,			// 源缓冲区大小
								   IN int nSrcBufferHeight,
								   IN BUFFER_MODE eMode,			// 混合操作类型
								   IN BUFFER_PARITY_STYLE eParity,		// 奇偶场类型
								   IN RECT* prcDesClip,
								   IN int n,
								   IN int k/* = NULL*/)	// 目标裁减矩形（可选项，为 NULL 时表示目标全部有效）

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
							  prcDesClip,n,k);
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
									   IN BUFFER_MODE eMode,			// 混合操作类型
									   IN BUFFER_PARITY_STYLE eParity,		// 奇偶场类型
									   IN RECT* prcDesClip/* = NULL*/)	// 目标裁减矩形（可选项，为 NULL 时表示目标全部有效）
{
	if (nSrcWidth != nDesWidth || nSrcHeight != nDesHeight)		// 缩放
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
	else		// 固定大小
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
							  prcDesClip);
	}
	return FALSE;
}

// Implement texture coordinates mapping of perspective projection, the algorithm is
// same as GPU, named Hyperbolic Interpolation or Rational Linear Interpolation.
// Use bilinear interpolation for color rendering.
// The parameter "pvfXYZW" is an array pointer, it indicates the coordinates of
// the four vertexes of the surface after a series of tranformations such as
// rotation, transfer, zoom and so on, the order of the vertexes must be LeftTop(0),
// RightTop(1), RightBottom(2), LeftBottom(3).
// The coordinates are formed by 4 floats cell, like this: struct vc{ float x, y, z, w; },
// it's compitable with "vector4f" declared in the project "SYCommon\TPVector".
// To get low coupling between "SYCommon" and "SYBufferOperate", this declaration
// is very important, and it makes "SYBufferOperate" independent.
BASEGRAPH_COMMON_API BOOL SY_BufferTransform(IN OUT SYBufferDef32* pDesBuf,			// 目标缓存
											IN int nOffsetLeft,						// 目标缓存在投影平面坐标系下的位置
											IN int nOffsetTop,
											IN SYBufferDef32* pSrcBuf,				// 源缓存
											IN int nSrcWidth,						// 源操作区域大小（相对于缓存起点而言）
											IN int nSrcHeight,
											IN void* pvfXYZW,						// 源操作区域的四个角点经过一系列变换后的顶点数据（必须有四个，顺序为(0,0),(w,0),(w,h),(h,0)）
											IN SYBufferDef32* pMaskBuf)				// 掩码缓存
{
	if ((pDesBuf == NULL) || (pDesBuf->m_dwBuffer == NULL)
		|| (pSrcBuf == NULL) || (pSrcBuf->m_dwBuffer == NULL)
		|| (pvfXYZW == NULL))
		return FALSE;

	//检测是否没有做斜切形变，如果没有则使用快速的线性变换算法
	LPSYINNERVERTEX t_pvfXYZW =  (LPSYINNERVERTEX)pvfXYZW;
	//if (t_pvfXYZW[0].w == 1 && t_pvfXYZW[1].w == 1 && t_pvfXYZW[2].w == 1 && t_pvfXYZW[3].w == 1)
	{
		BOOL bResult = SY_BufferTransformLinear(pDesBuf,nOffsetLeft,nOffsetTop,pSrcBuf,nSrcWidth,nSrcHeight,(vector4f*)pvfXYZW,pMaskBuf);
		return bResult;
	}


	SYInnerVertex*  pvLeftEdgeUVW = SYSSE_NEW(pDesBuf->m_nHeight, SYInnerVertex);		// @  1.f/w  v/w  u/w
	SYInnerVertex*  pvDeltaUVW = SYSSE_NEW(pDesBuf->m_nHeight, SYInnerVertex);		// @  dw  dv  du
	DWORD* *ppdwDesLeftEdge = SYSSE_NEW(pDesBuf->m_nHeight, DWORD*);				// 目标中左边缘的起始位置（要考虑拖出去的情况）
	int *pnLinePixels = SYSSE_NEW(pDesBuf->m_nHeight, int);						// 每行要处理的像素个数
	int nMaxLinePixels(0);					// 最长行的像素个数
	SYSSE_DECLARE(DWORD*) ppdwSrcStart[4] = { 0 };		// 源的起点
	float *pfPowerX = NULL, *pfPowerY = NULL, *pfPowerXY = NULL;
	DWORD* pdwSrcIndex = NULL;				// 双线性插值所需要的索引（四个像素 00  01  10  11）
	// 临时指针
	LPSYINNERVERTEX pvTmpLeftEdgeUVW = pvLeftEdgeUVW, pvTmpDeltaUVW = pvDeltaUVW;
	DWORD* *ppdwTmpDesLeftEdge = ppdwDesLeftEdge;
	int *pnTmpLinePixels = pnLinePixels;

	memset_dword_line(ppdwDesLeftEdge, 0, (pDesBuf->m_nHeight >> 2), ((pDesBuf->m_nHeight & 0x03) << 2));
	memset_dword_line(pnLinePixels, 0, (pDesBuf->m_nHeight >> 2), ((pDesBuf->m_nHeight & 0x03) << 2));
	memset_dword_line(ppdwSrcStart, (DWORD)(pSrcBuf->m_dwBuffer + pSrcBuf->m_nOriginX + pSrcBuf->m_rtBound.left + (pSrcBuf->m_nOriginY + pSrcBuf->m_rtBound.top) * pSrcBuf->m_nWidth), 1, 0);
	// 求解按行扫描的左右边界，以及边界上的 u/w、v/w、1/w、delta等。
	//if (!SYTrans_CalcBorder(pDesBuf, nOffsetLeft, nOffsetTop, (LPSYINNERVERTEX)pvfXYZW, pvLeftEdgeUVW,
	//						pvDeltaUVW, ppdwDesLeftEdge, pnLinePixels, &nMaxLinePixels))
	//	return FALSE;
	// 申请临时缓存
	pfPowerX = SYSSE_NEW(nMaxLinePixels, float);
	pfPowerY = SYSSE_NEW(nMaxLinePixels, float);
	pfPowerXY = SYSSE_NEW(nMaxLinePixels, float);
	pdwSrcIndex = SYSSE_NEW((nMaxLinePixels << 2), DWORD);
	// 循环对每行进行单独处理
	for (int i=0; i<pDesBuf->m_nHeight; i++)
	{
		if (*pnTmpLinePixels > 0)
		{
			// 求解出某行上的索引映射，以及权重表。
			//TPTrans_CalcPowerAndIndex_BL(pvTmpLeftEdgeUVW, pvTmpDeltaUVW, *pnTmpLinePixels,
			//	nSrcWidth, nSrcHeight, pSrcBuf->m_nWidth, pdwSrcIndex, pfPowerX, pfPowerY, pfPowerXY);
			trans_power_index_bl(pdwSrcIndex, pfPowerX, pfPowerY, pfPowerXY, pvTmpLeftEdgeUVW,
				pvTmpDeltaUVW, nSrcWidth, nSrcHeight, pSrcBuf->m_nWidth, *pnTmpLinePixels);
			// 查表并双线性插值。
			// 计算公式		S00 * x * y				S10 * (1 - x) * y
			//				S01 * x * (1 - y)		S11 * (1 - x) * (1 - y)
			// D = S11 + (S00 - S10 - S01 + S11) * x * y + (S01 - S11) * x + (S10 - S11) * y
			// 要特殊处理边缘问题，防止透明像素影响边缘的颜色，透明像素只能影响边缘的透明度。
			//TPTrans_Interpolate_BL(*ppdwTmpDesLeftEdge, *pnTmpLinePixels, ppdwSrcStart,
			//	pdwSrcIndex, pfPowerX, pfPowerY, pfPowerXY);
			trans_interpo_bl(*ppdwTmpDesLeftEdge, ppdwSrcStart, pdwSrcIndex, pfPowerX,
				pfPowerY, pfPowerXY, *pnTmpLinePixels);
		}
		pnTmpLinePixels++;
		pvTmpLeftEdgeUVW++;
		pvTmpDeltaUVW++;
		ppdwTmpDesLeftEdge++;
	}
	// 释放缓存
	SYSSE_FREE(pvLeftEdgeUVW);
	SYSSE_FREE(pvDeltaUVW);
	SYSSE_FREE(ppdwDesLeftEdge);
	SYSSE_FREE(pnLinePixels);
	SYSSE_FREE(pfPowerX);
	SYSSE_FREE(pfPowerY);
	SYSSE_FREE(pfPowerXY);
	SYSSE_FREE(pdwSrcIndex);

	return TRUE;
}

// 使用某个值填充内存中的某块区域，可指定裁剪区域。
BASEGRAPH_COMMON_API void SYBGRA32_FillRect(IN OUT DWORD* pdwBuf,	// 操作缓存
										   IN int nX,				// 操作区域
										   IN int nY,
										   IN int nWidth,
										   IN int nHeight,
										   IN int nBufWidth,		// 缓存大小
										   IN int nBufHeight,
										   IN RECT* prcClip,		// 裁剪区域（可选项）
										   IN DWORD dwValue)		// 填充值
{
	if (pdwBuf == NULL)
		return;

	// 有效区域裁剪
	if (!SYFill_ClipRange(nX, nY, nWidth, nHeight, nBufWidth, nBufHeight, prcClip))
		return;

	// 计算必要参数
	DWORD* pdwLineStart = pdwBuf + nY * nBufWidth + nX;
	int na16 = (nWidth >> 2), nm16 = ((nWidth & 0x03) << 2);

	// 逐行填充
	for (int i=0; i<nHeight; i++)
	{
		memset_dword_line(pdwLineStart, dwValue, na16, nm16);
		pdwLineStart += nBufWidth;
	}
}
BASEGRAPH_COMMON_API void SYBGRA32_ReplaceRGB_MulA(IN OUT DWORD* pdwBuf,	// 操作缓存
												  IN int nX,				// 操作区域
												  IN int nY,
												  IN int nWidth,
												  IN int nHeight,
												  IN int nBufWidth,		// 缓存大小
												  IN int nBufHeight,
												  IN RECT* prcClip,		// 裁剪区域（可选项）
												  DWORD* psrcBuf)		// 填充值
{
	if (pdwBuf == NULL)
		return;

	// 有效区域裁剪
	if (!SYFill_ClipRange(nX, nY, nWidth, nHeight, nBufWidth, nBufHeight, prcClip))
		return;

	// 计算必要参数
	DWORD* pdwLineStart = pdwBuf + nY * nBufWidth + nX;
	DWORD* pdwSrcLine = psrcBuf + nY * nBufWidth + nX;
	int na16 = (nWidth >> 2), nm16 = ((nWidth & 0x03) << 2);

	// 逐行填充
	for (int i=0; i<nHeight; i++)
	{
		copy_dword_line_rgb_mula(pdwLineStart,pdwSrcLine,na16, nm16);
		pdwLineStart += nBufWidth;
		pdwSrcLine += nBufWidth;
	}
}


BASEGRAPH_COMMON_API void SYBGRA32_ReplaceRGB_MulA(IN OUT DWORD* pdwBuf,	// 操作缓存
											   IN int nX,				// 操作区域
											   IN int nY,
											   IN int nWidth,
											   IN int nHeight,
											   IN int nBufWidth,		// 缓存大小
											   IN int nBufHeight,
											   IN RECT* prcClip,		// 裁剪区域（可选项）
											   DWORD* psrcBuf,
											   IN int nSrcX,
											   IN int nSrcY,
											   IN int nSrcBufWidth,
											   IN int nSrcBufHeight)		// 填充值
{
	if (pdwBuf == NULL)
		return;

	// 获得有效的操作范围
	int nSrcWidth = nWidth;
	int nSrcHeight = nHeight;
	if (!SYFix_CutRange(nX, nY, nWidth, nHeight, nBufWidth, nBufHeight,
		nSrcX, nSrcY, nSrcWidth, nSrcHeight, nSrcBufWidth, nSrcBufHeight, prcClip))
			return;

	//// 有效区域裁剪
	//if (!SYFill_ClipRange(nX, nY, nWidth, nHeight, nBufWidth, nBufHeight, prcClip))
	//	return;

	// 计算必要参数
	DWORD* pdwLineStart = pdwBuf + nY * nBufWidth + nX;
	DWORD* pdwSrcLine = psrcBuf + nSrcY * nBufWidth + nSrcX;
	int na16 = (nWidth >> 2), nm16 = ((nWidth & 0x03) << 2);

	// 逐行填充
	for (int i=0; i<nHeight; i++)
	{
		copy_dword_line_rgb_mula(pdwLineStart,pdwSrcLine,na16, nm16);
		pdwLineStart += nBufWidth;
		pdwSrcLine += nSrcBufWidth;
	}
}

BASEGRAPH_COMMON_API void SYBGRA32_ReplaceA_MulA(IN OUT DWORD* pdwBuf,	// 操作缓存
											   IN int nX,				// 操作区域
											   IN int nY,
											   IN int nWidth,
											   IN int nHeight,
											   IN int nBufWidth,		// 缓存大小
											   IN int nBufHeight,
											   IN RECT* prcClip,		// 裁剪区域（可选项）
											   DWORD* psrcBuf,
											   IN int nSrcX,
											   IN int nSrcY,
											   IN int nSrcBufWidth,
											   IN int nSrcBufHeight,
											   IN BOOL bInvert)		// 填充值
{
	if (pdwBuf == NULL)
		return;

	// 获得有效的操作范围
	int nSrcWidth = nWidth;
	int nSrcHeight = nHeight;
	if (!SYFix_CutRange(nX, nY, nWidth, nHeight, nBufWidth, nBufHeight,
		nSrcX, nSrcY, nSrcWidth, nSrcHeight, nSrcBufWidth, nSrcBufHeight, prcClip))
		return;

	// 计算必要参数
	DWORD* pdwLineStart = pdwBuf + nY * nBufWidth + nX;
	DWORD* pdwSrcLine = psrcBuf + nSrcY * nBufWidth + nSrcX;
	int na16 = (nWidth >> 2), nm16 = ((nWidth & 0x03) << 2);

	// 逐行填充
	for (int i=0; i<nHeight; i++)
	{
		if(bInvert)
			copy_dword_line_mula_invert(pdwLineStart,pdwSrcLine,na16, nm16);
		else
			copy_dword_line_mula(pdwLineStart,pdwSrcLine,na16, nm16);
		pdwLineStart += nBufWidth;
		pdwSrcLine += nSrcBufWidth;
	}
}

BASEGRAPH_COMMON_API void SYBGRA32_ReplaceRGB_MulA(IN OUT DWORD* pdwBuf,	// 操作缓存
											 IN int nX,				// 操作区域
											 IN int nY,
											 IN int nWidth,
											 IN int nHeight,
											 IN int nBufWidth,		// 缓存大小
											 IN int nBufHeight,
											 IN RECT* prcClip,		// 裁剪区域（可选项）
											 IN DWORD dwValue)		// 填充值
{
	if (pdwBuf == NULL)
		return;

	// 有效区域裁剪
	if (!SYFill_ClipRange(nX, nY, nWidth, nHeight, nBufWidth, nBufHeight, prcClip))
		return;

	// 计算必要参数
	DWORD* pdwLineStart = pdwBuf + nY * nBufWidth + nX;
	int na16 = (nWidth >> 2), nm16 = ((nWidth & 0x03) << 2);

	// 逐行填充
	for (int i=0; i<nHeight; i++)
	{
		memset_dword_line_rgb_mula(pdwLineStart,dwValue, na16, nm16);
		pdwLineStart += nBufWidth;
	}
}

BASEGRAPH_COMMON_API void SYBGRA32_ReplaceRGB_MulA_Width_HArray(IN OUT DWORD* pdwBuf,	// 操作缓存
												  IN int nX,				// 操作区域
												  IN int nY,
												  IN int nWidth,
												  IN int nHeight,
												  IN int nBufWidth,		// 缓存大小
												  IN int nBufHeight,
												  IN RECT* prcClip,		// 裁剪区域（可选项）
												  IN DWORD* dwValue)		// 填充值
{
	if (pdwBuf == NULL)
		return;

	// 有效区域裁剪
	if (!SYFill_ClipRange(nX, nY, nWidth, nHeight, nBufWidth, nBufHeight, prcClip))
		return;

	// 计算必要参数
	DWORD* pdwLineStart = pdwBuf + nY * nBufWidth + nX;
	int na16 = (nWidth >> 2), nm16 = ((nWidth & 0x03) << 2);

	// 逐行填充
	for (int i=0; i<nHeight; i++)
	{
		memset_dword_line_rgb_mula(pdwLineStart,dwValue[i], na16, nm16);
		pdwLineStart += nBufWidth;
	}
}

BASEGRAPH_COMMON_API void SYBGRA32_ReplaceRGB(IN OUT DWORD* pdwBuf,	// 操作缓存
											   IN int nX,				// 操作区域
											   IN int nY,
											   IN int nWidth,
											   IN int nHeight,
											   IN int nBufWidth,		// 缓存大小
											   IN int nBufHeight,
											   IN RECT* prcClip,		// 裁剪区域（可选项）
											   IN DWORD dwValue)		// 填充值
{
	if (pdwBuf == NULL)
		return;

	// 有效区域裁剪
	if (!SYFill_ClipRange(nX, nY, nWidth, nHeight, nBufWidth, nBufHeight, prcClip))
		return;

	// 计算必要参数
	DWORD* pdwLineStart = pdwBuf + nY * nBufWidth + nX;
	int na16 = (nWidth >> 2), nm16 = ((nWidth & 0x03) << 2);

	// 逐行填充
	for (int i=0; i<nHeight; i++)
	{
		memset_dword_line_rgb(pdwLineStart,dwValue, na16, nm16);
		pdwLineStart += nBufWidth;
	}
}
// 填充某块区域中的Alpha值，可使用掩码颜色，可指定裁剪区域。
// 填充模式中有支持缩放 Alpha 的模式，在这些模式下，cAlpha / 255  表示一个比例。
BASEGRAPH_COMMON_API void SYBGRA32_FillAlpha(IN OUT DWORD* pdwBuf,		// 目标
											IN int nX,					// 填充区域
											IN int nY,
											IN int nWidth,
											IN int nHeight,
											IN int nBufWidth,			// 缓存大小
											IN int nBufHeight,
											IN RECT* prcClip,			// 裁剪区域（可选项）
											IN char cAlpha,				// Alpha值
											IN SY_FILL_MODE eMode,		// 填充模式
											IN DWORD pdwMaskCr)			// 掩码颜色值（依赖模式的可选项）
{
	if (pdwBuf == NULL)
		return;

	// 有效区域裁剪
	if (!SYFill_ClipRange(nX, nY, nWidth, nHeight, nBufWidth, nBufHeight, prcClip))
		return;

	// 计算必要参数
	DWORD* pdwLineStart = pdwBuf + nY * nBufWidth + nX;
	int na16 = (nWidth >> 2), nm16 = ((nWidth & 0x03) << 2);

	switch (eMode)
	{
	case SY_FILL_ALL:		// 全部填充
		for (int i=0; i<nHeight; i++)
		{
			memset_alpha_line_all(pdwLineStart, cAlpha, na16, nm16);
			pdwLineStart += nBufWidth;
		}
		break;
	case SY_FILL_MASK:		// 只填充颜色为 pdwMaskCr 的像素
		for (int i=0; i<nHeight; i++)
		{
			memset_alpha_line_mask(pdwLineStart, cAlpha, pdwMaskCr, na16, nm16);
			pdwLineStart += nBufWidth;
		}
		break;
	case SY_FILL_NOT_MASK:	// 不填充颜色为 pdwMaskCr 的像素
		for (int i=0; i<nHeight; i++)
		{
			memset_alpha_line_notmask(pdwLineStart, cAlpha, pdwMaskCr, na16, nm16);
			pdwLineStart += nBufWidth;
		}
		break;
	case SY_FILL_SCALE_ALPHA_ALL:
		for (int i=0; i<nHeight; i++)
		{
			bgra32_scale_alpha_line(pdwLineStart, cAlpha, na16, nm16);
			pdwLineStart += nBufWidth;
		}
		break;
	default:
		break;
		//assert(FALSE);
	}
}


BASEGRAPH_COMMON_API void SYBGRA32_FillAlpha_With_H(IN OUT DWORD* pdwBuf,		// 目标
											IN int nX,					// 填充区域
											IN int nY,
											IN int nWidth,
											IN int nHeight,
											IN int nBufWidth,			// 缓存大小
											IN int nBufHeight,
											IN RECT* prcClip,			// 裁剪区域（可选项）
											IN DWORD* cAlpha,				// Alpha值
											IN SY_FILL_MODE eMode,		// 填充模式
											IN DWORD pdwMaskCr)			// 掩码颜色值（依赖模式的可选项）
{
	if (pdwBuf == NULL)
		return;

	// 有效区域裁剪
	if (!SYFill_ClipRange(nX, nY, nWidth, nHeight, nBufWidth, nBufHeight, prcClip))
		return;

	// 计算必要参数
	DWORD* pdwLineStart = pdwBuf + nY * nBufWidth + nX;
	int na16 = (nWidth >> 2), nm16 = ((nWidth & 0x03) << 2);

	switch (eMode)
	{
	case SY_FILL_ALL:		// 全部填充
		for (int i=0; i<nHeight; i++)
		{
			memset_alpha_line_all(pdwLineStart, (char)cAlpha[i], na16, nm16);
			pdwLineStart += nBufWidth;
		}
		break;
	case SY_FILL_MASK:		// 只填充颜色为 pdwMaskCr 的像素
		for (int i=0; i<nHeight; i++)
		{
			memset_alpha_line_mask(pdwLineStart, (char)cAlpha[i], pdwMaskCr, na16, nm16);
			pdwLineStart += nBufWidth;
		}
		break;
	case SY_FILL_NOT_MASK:	// 不填充颜色为 pdwMaskCr 的像素
		for (int i=0; i<nHeight; i++)
		{
			memset_alpha_line_notmask(pdwLineStart, (char)cAlpha[i], pdwMaskCr, na16, nm16);
			pdwLineStart += nBufWidth;
		}
		break;
	case SY_FILL_SCALE_ALPHA_ALL:
		for (int i=0; i<nHeight; i++)
		{
			bgra32_scale_alpha_line(pdwLineStart, (char)cAlpha[i], na16, nm16);
			pdwLineStart += nBufWidth;
		}
		break;
	default:
		break;
		//assert(FALSE);
	}
}

#if 0
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//		（3）对大片内存进行带掩码的操作
//////////////////////////////////////////////////////////////////////////

// 对 32 位内存进行键值操作
BASEGRAPH_COMMON_API BOOL SYBGRA32_ProcKey(IN OUT DWORD* pdwBuf,				// 目标
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
										  IN SY_KEY_MODE eMode,				// 键操作模式
										  IN BUFFER_PARITY_STYLE eParity,		// 奇偶场类型
										  IN RECT* prcDesClip/* = NULL */)	// 裁剪区域（可选项）
{
	//assert(nKeyPitch >= nKeyBufWidth);

	if (nKeyWidth == nDesWidth && nKeyHeight == nDesHeight)		// 固定大小
	{
		return SYBGRA_Key_Fix_SSE(pdwBuf,
						  		  nDesX,
								  nDesY,
								  nDesWidth,
								  nDesHeight,
								  nDesBufWidth,
								  nDesBufHeight,
								  pbyKey,
								  nKeyX,
								  nKeyY,
								  nKeyWidth,
								  nKeyHeight,
								  nKeyBufWidth,
								  nKeyBufHeight,
								  nKeyPitch,
								  eMode,
								  eParity,
								  prcDesClip);
	}
	else		// 缩放源
	{
		return SYBGRA_Key_BLStretch_SSE(pdwBuf,
										nDesX,
										nDesY,
										nDesWidth,
										nDesHeight,
										nDesBufWidth,
										nDesBufHeight,
										pbyKey,
										nKeyX,
										nKeyY,
										nKeyWidth,
										nKeyHeight,
										nKeyBufWidth,
										nKeyBufHeight,
										nKeyPitch,
										eMode,
										eParity,
										prcDesClip);
	}
	return FALSE;
}

// 实现一块 8 位键值内存与一个标量间的操作
BASEGRAPH_COMMON_API BOOL SYKey_ProcScalar(IN OUT LPBYTE pbyKey,		// 键目标
										  IN int nX,				// 填充区域
										  IN int nY,
										  IN int nWidth,
										  IN int nHeight,
										  IN int nKeyBufWidth,		// 键幅面大小
										  IN int nKeyBufHeight,
										  IN int nKeyPitch,			// 键缓存一行的字节宽度
										  IN RECT* prcClip,			// 裁剪区域（可选项）
										  IN char cAlpha,			// Alpha值
										  IN SY_KEY_MODE eMode)		// 键操作模式
{
	//assert(nKeyPitch >= nKeyBufWidth);

	if (pbyKey == NULL)
		return FALSE;

	// 有效区域裁剪
	if (!SYFill_ClipRange(nX, nY, nWidth, nHeight, nKeyBufWidth, nKeyBufHeight, prcClip))
		return FALSE;

	// 计算必要参数
	BYTE* pbyKeyLine = pbyKey + nY * nKeyPitch + nX;			// 键起始地址
	int na16 = (nWidth >> 2), nm16 = ((nWidth & 0x03) << 2);

	// 逐行处理
	switch (eMode)
	{
	case SY_KEY_MULTIPLY:			// 相乘
		for (int i=0; i<nHeight; i++)
		{
			keybuf_mul_char_line(pbyKeyLine, cAlpha, na16, nm16);
			pbyKeyLine += nKeyPitch;
		}
		break;
	case SY_KEY_MULTIPLY_NOTSRC:	// 反向源键相乘
		cAlpha = 0xff - cAlpha;
		for (int i=0; i<nHeight; i++)
		{
			keybuf_mul_char_line(pbyKeyLine, cAlpha, na16, nm16);
			pbyKeyLine += nKeyPitch;
		}
		break;
	case SY_KEY_MULTIPLY_NOTDES:	// 反向目标键相乘
		for (int i=0; i<nHeight; i++)
		{
			keybuf_mul_char_line_notdes(pbyKeyLine, cAlpha, na16, nm16);
			pbyKeyLine += nKeyPitch;
		}
		break;
	case SY_KEY_REPLACE:			// 替换
		for (int i=0; i<nHeight; i++)
		{
			memset_char_line(pbyKeyLine, cAlpha, na16, nm16);
			pbyKeyLine += nKeyPitch;
		}
		break;
	case SY_KEY_MIX:				// 混叠
		for (int i=0; i<nHeight; i++)
		{
			keybuf_mix_char_line(pbyKeyLine, cAlpha, na16, nm16);
			pbyKeyLine += nKeyPitch;
		}
		break;
	case SY_KEY_MIX_NOTSRC:			// 反向源键混叠
		cAlpha = 0xff - cAlpha;
		for (int i=0; i<nHeight; i++)
		{
			keybuf_mix_char_line(pbyKeyLine, cAlpha, na16, nm16);
			pbyKeyLine += nKeyPitch;
		}
		break;
	default:
		break;
		//assert(FALSE);
	}
	return TRUE;
}
// 实现两块 8 位键值内存间的操作
BASEGRAPH_COMMON_API BOOL SYKey_ProcBuffer(IN OUT LPBYTE pbyDes,				// 目标键
										  IN int nDesX,						// 目标操作区域
										  IN int nDesY,
										  IN int nDesWidth,
										  IN int nDesHeight,
										  IN int nDesBufWidth,				// 目标缓存大小
										  IN int nDesBufHeight,
										  IN int nDesKeyPitch,				// 目标键缓存一行的字节宽度
										  IN LPBYTE pbySrc,					// 源键
										  IN int nSrcX,						// 源操作区域
										  IN int nSrcY,
										  IN int nSrcWidth,
										  IN int nSrcHeight,
										  IN int nSrcBufWidth,				// 键幅面大小
										  IN int nSrcBufHeight,
										  IN int nSrcKeyPitch,				// 源键缓存一行的字节宽度
										  IN SY_KEY_MODE eMode,				// 键操作模式
										  IN BUFFER_PARITY_STYLE eParity,		// 奇偶场类型
										  IN RECT* prcDesClip/* = NULL*/)	// 裁剪区域（可选项）
{
	//assert(nDesKeyPitch >= nDesBufWidth && nSrcKeyPitch >= nSrcBufWidth);

	if (nSrcWidth == nDesWidth && nSrcHeight == nDesHeight)		// 固定大小
	{
		return SYKey_Fix_SSE(pbyDes,
							 nDesX,
							 nDesY,
							 nDesWidth,
							 nDesHeight,
							 nDesBufWidth,
							 nDesBufHeight,
							 nDesKeyPitch,
							 pbySrc,
							 nSrcX,
							 nSrcY,
							 nSrcWidth,
							 nSrcHeight,
							 nSrcBufWidth,
							 nSrcBufHeight,
							 nSrcKeyPitch,
							 eMode,
							 eParity,
							 prcDesClip);
	}
	else		// 缩放源
	{
		return SYKey_BLStretch_SSE(pbyDes,
								   nDesX,
								   nDesY,
								   nDesWidth,
								   nDesHeight,
								   nDesBufWidth,
								   nDesBufHeight,
								   nDesKeyPitch,
								   pbySrc,
								   nSrcX,
								   nSrcY,
								   nSrcWidth,
								   nSrcHeight,
								   nSrcBufWidth,
								   nSrcBufHeight,
								   nSrcKeyPitch,
								   eMode,
								   eParity,
								   prcDesClip);
	}
	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//		（4）利用上述的简单操作合成的特殊操作，如生成特殊的内存
//////////////////////////////////////////////////////////////////////////

// 生成交叉颜色块的透明色背景
BASEGRAPH_COMMON_API void SY_MakeTranslucentBack(OUT DWORD* pBuf,			// 背景 Buffer
												IN int nBufWidth,			// Buffer 大小
												IN int nBufHeight,
												IN int nBlockSize,			// 相间颜色块大小
												IN DWORD crOddBlock,		// 奇数列颜色块颜色
												IN DWORD crEvenBlock)		// 偶数列颜色块颜色
{
	if (pBuf == NULL)
		return;

	int nBlockCountH = nBufWidth / nBlockSize + 2;	// 确定一行最多需要的颜色块个数
	int nBlockCountV = nBufHeight / nBlockSize;		// 确定一列拥有的完整颜色块个数
	int nLineSize = nBlockCountH * nBlockSize;
	int na16 = (nBlockSize >> 2), nm16 = ((nBlockSize & 0x03) << 2);
	DWORD* pBlockLine = SYSSE_NEW(nLineSize, DWORD);
	DWORD* pTmpLine = pBlockLine;

	// 填充出相间图片
	for (int i=0; i<nBlockCountH; i++)
	{
		memset_dword_line(pTmpLine, (i & 0x01) ? crEvenBlock : crOddBlock, na16, nm16);
		pTmpLine += nBlockSize;
	}
	// 逐行填充背景
	na16 = (nBufWidth >> 2), nm16 = ((nBufWidth & 0x03) << 2);
	for (int i=0; i<nBlockCountV; i++)	// 先填充完整的颜色块
	{
		pTmpLine = (i & 0x01) ? (pBlockLine + nBlockSize) : pBlockLine;
		for (int k=0; k<nBlockSize; k++)
		{
			memcpy_line(pBuf, pTmpLine, na16, nm16);
			pBuf += nBufWidth;
		}
		nBufHeight -= nBlockSize;
	}
	pTmpLine = (nBlockCountV & 0x01) ? (pBlockLine + nBlockSize) : pBlockLine;
	for (int k=0; k<nBufHeight; k++)	// 填充最后一个不完整的颜色块
	{
		memcpy_line(pBuf, pTmpLine, na16, nm16);
		pBuf += nBufWidth;
	}
	SYSSE_FREE(pBlockLine);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//		（5）用于支持算法优化的一些系统操作接口
//////////////////////////////////////////////////////////////////////////

// 获取计数器值（高精度）：时钟周期——（纳秒级）/主频HZ数
BASEGRAPH_COMMON_API unsigned __int64 SY_GetCycleCount()
{
#ifdef WIN64
	assert(0);
	return 0;
#else
	__asm _emit 0x0F;		// RDTSC 指令
	__asm _emit 0x31;

#endif
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//		（6）特殊的运算接口
//////////////////////////////////////////////////////////////////////////

// 计算一等差序列数与某一特定数的完全平方和（特定数为负则计算完全平方差）
BASEGRAPH_COMMON_API void SY_SerialPerfectSquare(OUT float* pResult,		// 计算结果
												IN int nSerialCount,	// 序列长度
												IN float fStart,		// 序列起始数
												IN float fStep,			// 等差
												IN float fGiven)		// 特定的数
{
	math_serial_perfect_square(pResult, fStart, fStep, fGiven, (nSerialCount >> 2), ((nSerialCount & 0x03) << 2));
}

// 计算一等差序列数与某一特定数的完全平方和（特定数为负则计算完全平方差）
// 增加额外功能，对和进行线性缩放，并计算四次方结果
BASEGRAPH_COMMON_API void SY_SerialPerfectSquareEx(OUT float* pResSqr,		// 平方和结果
												  OUT float* pResQuartic,	// 四次方结果
												  OUT float* pResSum,		// 缩放和结果
												  IN int nSerialCount,		// 序列长度
												  IN float fStart,			// 序列起始数
												  IN float fStep,			// 等差
												  IN float fGiven,			// 特定的数
												  IN float fScaling)		// 和的缩放系数
{
	math_serial_perfect_square_ex(pResSqr, pResQuartic, pResSum, fStart, fStep, fGiven, fScaling, (nSerialCount >> 2), ((nSerialCount & 0x03) << 2));
}

// 计算一非负序列进行特定偏移后的平方根，根据表被放大的系数计算出索引，并查表
BASEGRAPH_COMMON_API void SY_OffsetSqrtIndexLookup(OUT void* pBuf,		// 查表结果（32 位表，要求与表的类型相同）
												  IN int nSerialCount,	// 序列长度
												  IN float* pSerial,	// 序列
												  IN float fOffset,		// 特定偏移
												  IN void* pTable,		// 表（32 位表）
												  IN float fScaling)	// 表的大小相对于序列长度的放大倍数
{
	math_offset_sqrt_index_lookup(pBuf, pSerial, fOffset, pTable, fScaling, (nSerialCount >> 2), ((nSerialCount & 0x03) << 2));
}

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
BASEGRAPH_COMMON_API void SY_TylerArcIndexLookupX(OUT void* pBuf,			// 查表结果（32 位表，要求与表的类型相同）
												 IN int nSerialCount,		// 序列长度
												 IN float* pQuartic,		// 四次方序列
												 IN float* pSquare,			// 完全平方和序列
												 IN float* pSum,			// 和序列
												 IN float fOffsetQ,			// (y - yc)^4
												 IN float fOffsetSq,		// (y - yc)^2
												 IN void* pTable,			// 表（32 位表）
												 IN float fConst)			// 计算式最后添加的常量（定积分的起点值）
{
	math_tyler_arc_index_lookup_x(pBuf, pQuartic, pSquare, pSum, fOffsetQ, fOffsetSq, pTable, fConst, (nSerialCount >> 2), ((nSerialCount & 0x03) << 2));
}
BASEGRAPH_COMMON_API void SY_TylerArcIndexLookupY(OUT void* pBuf,			// 查表结果（32 位表，要求与表的类型相同）
												 IN int nSerialCount,		// 序列长度
												 IN float* pQuartic,		// 四次方序列
												 IN float* pSquare,			// 完全平方和序列
												 IN float fOffsetQ,			// (y - yc)^4
												 IN float fOffsetSq,		// (y - yc)^2
												 IN float fSum,				// R * (y - yc)
												 IN void* pTable,			// 表（32 位表）
												 IN float fConst)			// 计算式最后添加的常量（定积分的起点值）
{
	math_tyler_arc_index_lookup_y(pBuf, pQuartic, pSquare, fOffsetQ, fOffsetSq, fSum, pTable, fConst, (nSerialCount >> 2), ((nSerialCount & 0x03) << 2));
}

// 翻转符号，将负值变正值或反过来（当输入与输出指针相同时，认为直接翻转）
BASEGRAPH_COMMON_API void SY_NegtiveFloat(OUT float* pDes,		// 目标
										 IN float* pSrc,		// 源
										 IN int nlen)			// 数组长度
{
	math_negtive_float(pDes, pSrc, (nlen >> 2), ((nlen & 0x03) << 2));
}

// 求解一整型等差序列经过两种偏移后的新序列 offset_1 + a 与 offset_2 - a
// 特殊情况：通过指定圆心的直线 y = x 与 y = -x 在指定大小区域内的点的 X 序列值，序列以 y 为索引
BASEGRAPH_COMMON_API void SY_SerialOffset2(OUT int* pDes1,		// offset_1 + a
										  OUT int* pDes2,		// offset_2 - a
										  IN int nSerialCount,	// 序列长度
										  IN int nMin,			// 最小值
										  IN int nMax,			// 最大值
										  IN float fOffset1,	// 对于 y = x 为 xc - yc
										  IN float fOffset2,	// 对于 y = -x 为 xc + yc
										  IN int nStart,		// 序列起点
										  IN int nStep)			// 序列步长
{
	math_serial_offset_2(pDes1, pDes2, nMin, nMax, fOffset1, fOffset2, nStart, nStep, (nSerialCount >> 2), ((nSerialCount & 0x03) << 2));
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// 旧接口：速度较慢，需要重新优化
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// 功能：根据加权算子计算Mask值:blur掩模，反走样操作
// 参数：pbyMask：要计算的掩模缓冲区指针
//		 nStartX：掩模起点X坐标
//		 nStartY：掩模起点Y坐标
//		 nMaskWidth：掩模宽度
//		 nMaskHeight：掩模高度
//		 pnOperator：加权算子数组指针
//		 nOperatorWidth：数组宽度
//		 nOperatorHeight：数组高度
//		 nMainOperatorX：主象素点X坐标
//		 nMainOperatorY：主象素点Y坐标
//////////////////////////////////////////////////////////////////////////
void CALLBACK SY_CalMask(IN OUT BYTE *pbyMask,
				IN int nStartX, 
				IN int nStartY, 
				IN int nMaskWidth,
				IN int nMaskHeight,
				IN BYTE *pbyOperator,
				IN int nOperatorWidth, 
				IN int nOperatorHeight,
				IN int nMainOperatorX,
				IN int nMainOperatorY)
{
	return ;
	//ASSERT(pbyMask != NULL);
	if(pbyOperator == NULL)
		return;
	//if(nMaskWidth<=64&&nMaskHeight<=64)
	//	return;
	if(nOperatorWidth <= 0 || nOperatorHeight <= 0)
		return;
	if(nStartX >= nMaskWidth || nStartY >= nMaskHeight)
		return;
	if(nStartX < 0)
		nStartX = 0;
	if(nStartY < 0)
		nStartY = 0;
	int nWidth = nMaskWidth - nStartX;			// 要计算的Mask宽度与高度
	int nHeight = nMaskHeight - nStartY;
	if(nWidth <= 0 || nHeight <= 0)
		return;
	if(nMainOperatorX < 0 || nMainOperatorY < 0 || nMainOperatorX >= nOperatorWidth || nMainOperatorY >= nOperatorHeight)
		return;
	//	nWidth = nWidth - (nOperatorWidth - 1);	// 要计算的宽度与高度
	//	nHeight = nHeight - (nOperatorHeight - 1);
	BYTE *pSrcMask = pbyMask + nStartY*nMaskWidth + nStartX;	// 起点指针
	int nCalSum = 0;
	for(int i=0; i<nOperatorHeight; i++)
	{
		for(int j=0; j<nOperatorWidth; j++)
			nCalSum += *(pbyOperator+i*nOperatorWidth+j);
	}
	float fCalSum = 1.0f/nCalSum;
	BYTE *pTempBuf = new BYTE[nHeight*nWidth];
	if(pTempBuf == NULL)
		return;
	memset(pTempBuf, 0, sizeof(BYTE)*(nHeight*nWidth));
	BYTE *pTempBuf1 = pTempBuf;
	BYTE *pDes = pSrcMask;

#if 0
	// 一次算Mask四个点
	__asm
	{
		XOR ECX, ECX
			MOVSS XMM7, fCalSum
			PSHUFD XMM7, XMM7, 0		// 存放4个nCalSum
			MOV EDX, pTempBuf1			// 临时缓冲区指针
LOOPX:
		CMP ECX, nHeight				// x<nHeight
			JGE LOOPXEND

			PUSH ECX
			XOR ECX, ECX
			MOV EAX, pDes					// 临时象素点指针
LOOPY:
		ADD ECX, 4
			CMP ECX, nWidth				// y<nWidth
			JG LOOPY2

			PUSH ECX
			PUSH EAX
			PUSH EDX
		{
			XOR ECX, ECX
				PXOR XMM0, XMM0		// 存放计算的结果
				MOV EDX, pbyOperator
LOOPI:
			CMP ECX, nOperatorHeight
				JGE LOOPIEND
				PUSH ECX
				XOR ECX, ECX
LOOPJ:
			CMP ECX, nOperatorWidth
				JGE LOOPJEND

				PUSH EAX
				XOR EAX, EAX
				MOV  AL,  [ECX+EDX]			// 读出pbyTempOperator[j]值到AX的低8位
				MOVD XMM1,EAX 
					POP EAX	
					PSHUFD XMM1, XMM1, 0		// 打包，XMM1存四个相同的值
					MOVD XMM2, [ECX+EAX]		// 读出pbyTempDes[j]后面的四个BYTE值
					PXOR XMM3, XMM3				// 转换成4个DWORD值
						PUNPCKLBW XMM2, XMM3
						PUNPCKLWD XMM2, XMM3
						PMULLW XMM1, XMM2			// pbyTempOperator[j]*pbyTempDes[j]
						INC ECX
						PADDD XMM0, XMM1			// nSum += pbyTempOperator[j]*pbyTempDes[j]

						JMP LOOPJ
LOOPJEND:
					POP ECX
						INC ECX
						ADD EDX, nOperatorWidth			// pbyTempOperator += nOperatorWidth;
						ADD EAX, nMaskWidth				// pbyTempDes += nMaskWidth;
						JMP LOOPI
LOOPIEND:
					CVTDQ2PS XMM0, XMM0				// 转换成float
						MULPS XMM0,	XMM7				// nSum*1.0f/nCalSum
						POP EDX
						POP EAX
						POP ECX
						ADD EAX, 4							// pDes+4
		}
		CVTPS2DQ XMM0, XMM0					// 转换成DWORD
			PACKSSDW XMM0, XMM0					// 解包成4个Byte放在最后4字节
			PACKUSWB XMM0, XMM0						
			MOVD     [EDX], XMM0				// *pTempBuf1 = nSum/nCalSum;
			ADD EDX, 4							// pTempBuf1+4,后移4

			JMP LOOPY
LOOPY2:
		SUB ECX, 4
LOOPY3:
		CMP ECX, nWidth
			JGE LOOPYEND

			PUSH ECX
			PUSH EAX
			PUSH EDX
		{
			XOR ECX, ECX
				PXOR XMM0, XMM0		// 存放计算的结果
				MOV EDX, pbyOperator
LOOPI2:
			CMP ECX, nOperatorHeight
				JGE LOOPIEND2
				PUSH ECX
				XOR ECX, ECX
LOOPJ2:
			CMP ECX, nOperatorWidth
				JGE LOOPJEND2

				PUSH EAX
				XOR EAX, EAX
				MOV  AL,  [ECX+EDX]			// 读出pbyTempOperator[j]值
				MOVD XMM1,EAX 
					POP EAX	
					PUSH EDX					// 读出pbyTempDes[j]
					XOR EDX, EDX
					MOV DL, [ECX+EAX]
					MOVD XMM2, EDX
						POP EDX 		
						PXOR XMM3, XMM3				// 转换成4个DWORD值
						PUNPCKLBW XMM2, XMM3
						PUNPCKLWD XMM2, XMM3
						PMULLW XMM1, XMM2			// pbyTempOperator[j]*pbyTempDes[j]
						INC ECX
						PADDD XMM0, XMM1			// nSum += pbyTempOperator[j]*pbyTempDes[j]

						JMP LOOPJ2
LOOPJEND2:
					POP ECX
						INC ECX
						ADD EDX, nOperatorWidth			// pbyTempOperator += nOperatorWidth;
						ADD EAX, nMaskWidth				// pbyTempDes += nMaskWidth;
						JMP LOOPI2
LOOPIEND2:
					CVTDQ2PS XMM0, XMM0				// 转换成float
						MULPS XMM0,	XMM7				// nSum*1.0f/nCalSum
						POP EDX
						POP EAX
						POP ECX
						ADD EAX, 1							// pDes+1
		}
		CVTPS2DQ XMM0, XMM0					// 转换成DWORD
			PACKSSDW XMM0, XMM0					// 解包成4个Byte放在最后4字节
			PACKUSWB XMM0, XMM0		
			PUSH EAX
			MOVD  EAX, XMM0
			MOV     [EDX], AL					// *pTempBuf1 = nSum/nCalSum;
			POP EAX
			ADD EDX, 1							// pTempBuf1+1,后移1

			INC ECX
			JMP LOOPY3
LOOPYEND:
		MOV ECX, nMaskWidth
			ADD [pDes], ECX
			POP ECX
			INC ECX
			JMP LOOPX
LOOPXEND:
		EMMS
	}
	pTempBuf1 = pTempBuf;
	pSrcMask = pbyMask +(nStartY+nMainOperatorY)*nMaskWidth + (nStartX+nMainOperatorX);
	pDes = pSrcMask;
	__asm
	{
		XOR ECX, ECX
			MOV EAX, pTempBuf1
			MOV EDX, pDes
LOOPY1:
		CMP ECX, nHeight
			JGE LOOPY1END
			PUSH ECX
			XOR ECX, ECX
LOOPX1:
		Add ECX, 4
			CMP ECX, nWidth
			JG LOOPX2
			SUB ECX, 4
			MOVD MM0, [EAX]
			MOVD [EDX+ECX], MM0
				ADD EAX, 4
				ADD ECX, 4
				JMP LOOPX1
LOOPX2:
			SUB ECX, 4
LOOPX3:
			CMP ECX, nWidth
				JGE LOOPX1END
				PUSH EAX
				PUSH EDX
				MOV DL, [EAX]
				MOV AL, DL
					POP EDX
					MOV [EDX+ECX], AL
					POP EAX
					INC EAX
					INC ECX
					JMP LOOPX3
LOOPX1END:
				POP ECX
					INC ECX
					ADD EDX, nMaskWidth
					JMP LOOPY1
LOOPY1END:
				EMMS
	}

	delete[] pTempBuf;
#endif
#if 0
	for(int y=0; y<nHeight; y++)
	{
		for(int x=0; x<nWidth/4; x++)
		{
			for(int k=0; k<4; k++)
			{
				int nSum = 0;
				BYTE *pbyTempDes = pDes;// - nMainOperatorY*nMaskWidth - nMainOperatorX;
				BYTE *pbyTempOperator = pbyOperator;
				for(int i=0; i<nOperatorHeight; i++)
				{
					for(int j=0; j<nOperatorWidth; j++)
					{
						nSum += pbyTempOperator[j]*pbyTempDes[j];
					}
					pbyTempOperator += nOperatorWidth;
					pbyTempDes += nMaskWidth;
				}
				*pTempBuf1 = nSum/nCalSum;
				pTempBuf1++;
				pDes++;
			}
		}
		for(x=0; x<nWidth%4; x++)
		{
			int nSum = 0;
			BYTE *pbyTempDes = pDes;// - nMainOperatorY*nMaskWidth - nMainOperatorX;
			BYTE *pbyTempOperator = pbyOperator;
			for(int i=0; i<nOperatorHeight; i++)
			{
				for(int j=0; j<nOperatorWidth; j++)
				{
					nSum += pbyTempOperator[j]*pbyTempDes[j];
				}
				pbyTempOperator += nOperatorWidth;
				pbyTempDes += nMaskWidth;
			}
			*pTempBuf1 = nSum/nCalSum;
			pTempBuf1++;
			pDes++;
		}
		pSrcMask += nMaskWidth;
		pDes = pSrcMask;
	}

	pTempBuf1 = pTempBuf;
	pSrcMask = pbyMask +(nStartY+nMainOperatorY)*nMaskWidth + (nStartX+nMainOperatorX);
	pDes = pSrcMask;
	for(int y=0; y<nHeight; y++)
	{
		for(int x=0; x<nWidth; x++)
		{
			*pDes = *pTempBuf1;
			pTempBuf1++;
			pDes++;
		}
		pSrcMask += nMaskWidth;
		pDes = pSrcMask;
	}

	delete[] pTempBuf;
#endif

	// 添加于2004.2.1,添加者：陈玉峰
	// 添加了对Mask边缘的计算
#if 1								// 其汇编实现
	int nCurHeight = 0;					// 当前坐标点
	int nCurWidth = 0;
	int nCurX = 0;
	int nCurY = 0;
	int nCurOperateHeight = 0;
	int nCurOperateWidth = 0;
	// 一次算Mask四个点
#ifdef WIN64
	assert(0);
#else
	__asm
	{
		MOVSS XMM7, fCalSum
			PSHUFD XMM7, XMM7, 0		// 存放4个fCalSum在XMM7中
			MOV EDX, pTempBuf1			// 临时缓冲区指针
LOOPX:
		MOV ECX, nCurHeight
			CMP ECX, nHeight			// nCurHeight<nHeight
			JGE LOOPXEND
LOOPY:
		MOV ECX, nCurWidth
			ADD ECX, 4
			CMP ECX, nWidth				// nCurWidth<nWidth
			JG LOOPY2

			PUSH EDX
		{
			PXOR XMM0, XMM0		// 存放计算的结果SUM
				MOV EDX, pbyOperator// 算子指针
LOOPI:
			MOV ECX, nCurOperateHeight
				CMP ECX, nOperatorHeight	// nCurOperateHeight<nOperatorHeight
				JGE LOOPIEND

				MOV EAX, nCurHeight			// 计算相对的CurY,存入nCurY
				ADD EAX, ECX
				SUB EAX, nMainOperatorY
				CMP EAX, 0					// nCurY>=0且nCurY<nHeight
				JL  LOOPJEND
				CMP EAX, nHeight
				JGE LOOPJEND
				MOV nCurY, EAX
LOOPJ:
			MOV ECX, nCurOperateWidth
				CMP ECX, nOperatorWidth		// nCurOperateWidth<nOperatorWidth
				JGE LOOPJEND

				XOR EAX, EAX
				MOV  AL,  [ECX+EDX]			// 读出pbyOperator[nCurOperateWidth]值
				MOVD XMM1,EAX 
					PSHUFD XMM1, XMM1, 0		// 打包，XMM1存四个相同的算子值

					PXOR XMM2, XMM2				// 存放pDes的值
					PXOR XMM3, XMM3
					PXOR XMM4, XMM4
					PXOR XMM5, XMM5
					PUSH EDX
					MOV EAX, nCurY				// pDes+nCurY*nMaskWidth
					MUL nMaskWidth
					MOV EDX, pDes
					ADD EDX, EAX

					MOV ECX, nCurWidth			// 计算相对的CurX
					ADD ECX, nCurOperateWidth
					SUB ECX, nMainOperatorX
					CMP ECX, 0					// CurX>=0且nCurX<nWidth
					JL  CurX1
					CMP ECX, nWidth
					JGE CurX1
					XOR EAX, EAX
					MOV AL,  [EDX+ECX]			// 取出pDes[nCurX][nCurY]的值
					MOVD XMM2, EAX
CurX1:
					INC ECX						// CurX+1
						CMP ECX, 0					// CurX>=0且nCurX<nWidth
						JL  CurX2
						CMP ECX, nWidth
						JGE CurX2
						XOR EAX, EAX
						MOV AL,  [EDX+ECX]			// 取出pDes[nCurX][nCurY]的值
						MOVD XMM3, EAX
							PUNPCKLDQ XMM2, XMM3
CurX2:
						INC ECX						// CurX+1
							CMP ECX, 0					// CurX>=0且nCurX<nWidth
							JL  CurX3
							CMP ECX, nWidth
							JGE CurX3
							XOR EAX, EAX
							MOV AL,  [EDX+ECX]			// 取出pDes[nCurX][nCurY]的值
							MOVD XMM4, EAX
CurX3:
							INC ECX						// CurX+1
								CMP ECX, 0					// CurX>=0且nCurX<nWidth
								JL  CurXEnd			
								CMP ECX, nWidth
								JGE CurXEnd
								XOR EAX, EAX
								MOV AL,  [EDX+ECX]			// 取出pDes[nCurX][nCurY]的值
								MOVD XMM5, EAX
									PUNPCKLDQ XMM4, XMM5
CurXEnd:
								PUNPCKLQDQ XMM2, XMM4		// 组合成4个DWORD放入XMM2
									POP EDX

									PMULLW XMM1, XMM2			// pbyOperator[j]*pDes[j]
									PADDD XMM0, XMM1			// nSum += pOperator[j]*pDes[j]

									MOV ECX, nCurOperateWidth
									INC ECX
									MOV nCurOperateWidth, ECX
									JMP LOOPJ
LOOPJEND:
								MOV nCurOperateWidth, 0		// nCurOperateWidth 清0
									MOV ECX, nCurOperateHeight
									INC ECX
									MOV nCurOperateHeight, ECX
									ADD EDX, nOperatorWidth			// pbyTempOperator += nOperatorWidth;
									JMP LOOPI
LOOPIEND:
								MOV nCurOperateHeight, 0		// nCurOperateHeight 清0
									CVTDQ2PS XMM0, XMM0				// 转换成float
									MULPS XMM0,	XMM7				// nSum*1.0f/nCalSum
									POP EDX							// 弹出pTempBuf指针
		}
		CVTPS2DQ XMM0, XMM0					// 转换成DWORD
			PACKSSDW XMM0, XMM0					// 解包成4个Byte放在最后4字节
			PACKUSWB XMM0, XMM0						
			MOVD     [EDX], XMM0				// *pTempBuf1 = nSum/nCalSum;
			ADD EDX, 4							// pTempBuf1+4,后移4

			MOV ECX, nCurWidth
			ADD ECX, 4
			MOV nCurWidth, ECX
			JMP LOOPY
LOOPY2:
		MOV ECX, nCurWidth
LOOPY3:
		CMP ECX, nWidth
			JGE LOOPYEND

			PUSH EDX
		{
			PXOR XMM0, XMM0		// 存放计算的结果SUM
				MOV EDX, pbyOperator// 算子指针
LOOPI1:
			MOV ECX, nCurOperateHeight
				CMP ECX, nOperatorHeight	// nCurOperateHeight<nOperatorHeight
				JGE LOOPIEND1

				MOV EAX, nCurHeight			// 计算相对的CurY,存入nCurY
				ADD EAX, ECX
				SUB EAX, nMainOperatorY
				CMP EAX, 0					// nCurY>=0且nCurY<nHeight
				JL  LOOPJEND1
				CMP EAX, nHeight
				JGE LOOPJEND1
				MOV nCurY, EAX
LOOPJ1:
			MOV ECX, nCurOperateWidth
				CMP ECX, nOperatorWidth		// nCurOperateWidth<nOperatorWidth
				JGE LOOPJEND1

				XOR EAX, EAX
				MOV  AL,  [ECX+EDX]			// 读出pbyOperator[nCurOperateWidth]值
				MOVD XMM1,EAX 

					PXOR XMM2, XMM2				// 存放pDes的值
					PUSH EDX
					MOV EAX, nCurY				// pDes+nCurY*nMaskWidth
					MUL nMaskWidth
					MOV EDX, pDes
					ADD EDX, EAX

					MOV ECX, nCurWidth			// 计算相对的CurX
					ADD ECX, nCurOperateWidth
					SUB ECX, nMainOperatorX
					CMP ECX, 0					// CurX>=0且nCurX<nWidth
					JL  CurXEnd1
					CMP ECX, nWidth
					JGE CurXEnd1
					XOR EAX, EAX
					MOV AL,  [EDX+ECX]			// 取出pDes[nCurX][nCurY]的值
					MOVD XMM2, EAX
CurXEnd1:
					POP EDX

						PMULLW XMM1, XMM2			// pbyOperator[j]*pDes[j]
						PADDD XMM0, XMM1			// nSum += pOperator[j]*pDes[j]

						MOV ECX, nCurOperateWidth
						INC ECX
						MOV nCurOperateWidth, ECX
						JMP LOOPJ1
LOOPJEND1:
					MOV nCurOperateWidth, 0		// nCurOperateWidth 清0
						MOV ECX, nCurOperateHeight
						INC ECX
						MOV nCurOperateHeight, ECX
						ADD EDX, nOperatorWidth			// pbyTempOperator += nOperatorWidth;
						JMP LOOPI1
LOOPIEND1:
					MOV nCurOperateHeight, 0		// nCurOperateHeight 清0
						CVTDQ2PS XMM0, XMM0				// 转换成float
						MULPS XMM0,	XMM7				// nSum*1.0f/nCalSum
						POP EDX							// 弹出pTempBuf指针
		}
		CVTPS2DQ XMM0, XMM0					// 转换成DWORD
			PACKSSDW XMM0, XMM0					// 解包成4个Byte放在最后4字节
			PACKUSWB XMM0, XMM0						
			MOVD     EAX, XMM0				// *pTempBuf1 = nSum/nCalSum;
			MOV      [EDX], AL
			ADD EDX, 1							// pTempBuf1+1,后移1

			MOV ECX, nCurWidth
			INC ECX
			MOV nCurWidth, ECX
			JMP LOOPY3
LOOPYEND:
		MOV nCurWidth, 0				// nCurWidth 清0
			MOV ECX, nCurHeight
			INC ECX
			MOV nCurHeight, ECX
			JMP LOOPX
LOOPXEND:
		EMMS
	}
	pTempBuf1 = pTempBuf;
	__asm
	{
		XOR ECX, ECX
			MOV EAX, pTempBuf1
			MOV EDX, pDes
LOOPY1:
		CMP ECX, nHeight
			JGE LOOPY1END
			PUSH ECX
			XOR ECX, ECX
LOOPX1:
		Add ECX, 4
			CMP ECX, nWidth
			JG LOOPX2
			SUB ECX, 4
			MOVD MM0, [EAX]
			MOVD [EDX+ECX], MM0
				ADD EAX, 4
				ADD ECX, 4
				JMP LOOPX1
LOOPX2:
			SUB ECX, 4
LOOPX3:
			CMP ECX, nWidth
				JGE LOOPX1END
				PUSH EAX
				PUSH EDX
				MOV DL, [EAX]
				MOV AL, DL
					POP EDX
					MOV [EDX+ECX], AL
					POP EAX
					INC EAX
					INC ECX
					JMP LOOPX3
LOOPX1END:
				POP ECX
					INC ECX
					ADD EDX, nMaskWidth
					JMP LOOPY1
LOOPY1END:
				EMMS
	}

#endif // WIN64
	delete[] pTempBuf;
#endif
#if 0
	int nSum = 0;
	for(int y=0; y<nHeight; y++)
	{
		for(int x=0; x<nWidth/4; x++)
		{
			for(int k=0; k<4; k++)
			{
				nSum = 0;
				BYTE *pbyTempOperator = pbyOperator;
				for(int i=0; i<nOperatorHeight; i++)
				{
					for(int j=0; j<nOperatorWidth; j++)
					{
						// 当前点坐标
						int nCurX = x*4 + k - nMainOperatorX + j;
						int nCurY = y - nMainOperatorY + i;
						if(nCurX>=0 && nCurX<nWidth && nCurY>=0 && nCurY<nHeight)
							nSum += pbyTempOperator[j]*(*(pDes+nCurY*nWidth+nCurX));
					}
					pbyTempOperator += nOperatorWidth;
				}
				*pTempBuf1 = nSum/nCalSum;
				pTempBuf1++;
			}
		}
		for(x=x*4; x<nWidth; x++)
		{
			nSum = 0;
			BYTE *pbyTempOperator = pbyOperator;
			for(int i=0; i<nOperatorHeight; i++)
			{
				for(int j=0; j<nOperatorWidth; j++)
				{
					// 当前点坐标
					int nCurX = x - nMainOperatorX + j;
					int nCurY = y - nMainOperatorY + i;
					if(nCurX>=0 && nCurX<nWidth && nCurY>=0 && nCurY<nHeight)
						nSum += pbyTempOperator[j]*(*(pDes+nCurY*nWidth+nCurX));
				}
				pbyTempOperator += nOperatorWidth;
			}
			*pTempBuf1 = nSum/nCalSum;
			pTempBuf1++;
		}
	}

	pTempBuf1 = pTempBuf;
	pSrcMask = pbyMask +nStartY*nMaskWidth + nStartX;
	pDes = pSrcMask;
	for(int y=0; y<nHeight; y++)
	{
		for(int x=0; x<nWidth; x++)
		{
			*pDes = *pTempBuf1;
			pTempBuf1++;
			pDes++;
		}
		pSrcMask += nMaskWidth;
		pDes = pSrcMask;
	}

	delete[] pTempBuf;
#endif
}

// 源像素索引表
_declspec(align(16)) DWORD *g_pnSrcIndexTable = NULL;
// 权重表
_declspec(align(8)) BYTE *g_pnSrcWeightTable = NULL;

void CALLBACK SY_CreateTableFor_H1920To1440()
{
	if(!g_pnSrcIndexTable)
		g_pnSrcIndexTable = (DWORD*)_mm_malloc(sizeof(DWORD)*4096, 16);
	if(!g_pnSrcWeightTable)
		g_pnSrcWeightTable = (BYTE*)_mm_malloc(sizeof(BYTE)*4096, 8);
	float fTemp = 0.0f;
	float fAdd = 1920/1440.0f;
	for(int i=0; i<4096; i++)
	{
		fTemp = i*fAdd;
		g_pnSrcIndexTable[i] = (int)fTemp;
		g_pnSrcWeightTable[i] = (BYTE)(fmodf(fTemp, 1.0f) * 256);
	}
}

void CALLBACK SY_DeleteTableFor_H1920To1440()
{
	if(g_pnSrcIndexTable)
	{
		_mm_free(g_pnSrcIndexTable);
		g_pnSrcIndexTable = NULL;
	}
	if(g_pnSrcWeightTable)
	{
		_mm_free(g_pnSrcWeightTable);
		g_pnSrcWeightTable = NULL;
	}
}

void CALLBACK SY_LinearScale_H1920To1440(OUT BYTE* pDes,
									   IN int nDesWidth,
									   IN int nDesPitch,
									   IN BYTE* pSrc,
									   IN int nSrcWidth,
									   IN int nSrcPitch,
									   IN int nHeight
									   )
{
	BYTE* pSrcTemp = pSrc;
	BYTE* pDesTemp = pDes;
	int nTemp = nSrcWidth - 1;
	__declspec(align(8)) static const __int64 nMax		=		0x0100010001000100;
	for(int i=0; i<nHeight; i++)
	{
		/*for(int j=0; j<nDesWidth; j++)
		{
			BYTE* pSrc1 = pSrcTemp + g_pnSrcIndexTable[j] * 4;
			if(g_pnSrcWeightTable[j] == 0)
			{
				pDesTemp[j*4] = (*pSrc1);
				pDesTemp[j*4+1] = (*(pSrc1+1));
				pDesTemp[j*4+2] = (*(pSrc1+2));
				pDesTemp[j*4+3] = (*(pSrc1+3));
			}
			else if(g_pnSrcIndexTable[j] >= nTemp)
			{
				pDesTemp[j*4] = (*pSrc1) * (256-g_pnSrcWeightTable[j]) /256;
				pDesTemp[j*4+1] = (*(pSrc1+1)) * (256-g_pnSrcWeightTable[j]) /256;
				pDesTemp[j*4+2] = (*(pSrc1+2)) * (256-g_pnSrcWeightTable[j]) /256;
				pDesTemp[j*4+3] = (*(pSrc1+3)) * (256-g_pnSrcWeightTable[j]) /256;
			}
			else
			{
				pDesTemp[j*4] = ((*pSrc1) * (256-g_pnSrcWeightTable[j]) + (*(pSrc1+4)) * g_pnSrcWeightTable[j])/256;
				pDesTemp[j*4+1] = ((*(pSrc1+1)) * (256-g_pnSrcWeightTable[j]) + (*(pSrc1+5)) * g_pnSrcWeightTable[j])/256;
				pDesTemp[j*4+2] = ((*(pSrc1+2)) * (256-g_pnSrcWeightTable[j]) + (*(pSrc1+6)) * g_pnSrcWeightTable[j])/256;
				pDesTemp[j*4+3] = ((*(pSrc1+3)) * (256-g_pnSrcWeightTable[j]) + (*(pSrc1+7)) * g_pnSrcWeightTable[j])/256;
			}
		}*/
#ifdef WIN64
		assert(0);
#else
		__asm
		{
			mov eax, pDesTemp
			xor ecx, ecx
LOOPWIDTH:
			cmp ecx, nDesWidth
			jge LOOPWIDTH_END

			mov esi, ecx
			shl esi, 2		// *4
			add esi, g_pnSrcIndexTable
			mov edx, [esi]	// g_pnSrcIndexTable[j] 
			cmp edx, nTemp
			jl  LOOPWIDTH_NORMAL

			// g_pnSrcIndexTable[j] >= nTemp
			shl edx, 2		// g_pnSrcIndexTable[j] * 4
			add edx, pSrcTemp
			mov edi, [edx]	// src颜色放在edi中
			mov esi, ecx
			add esi, g_pnSrcWeightTable
			xor edx, edx
			mov dl, [esi]	// 取出权重值放在edx中
			cmp dl, 0
			jnz LOOPWIDTH_WEIGHT0  
			// 权重 == 0 时
			mov [eax], edi	// 直接附值
			inc ecx
			add eax, 4
			jmp LOOPWIDTH
LOOPWIDTH_WEIGHT0:
			// 权重非0时
			movq mm0, nMax
			movd mm1, edx
			punpcklwd mm1, mm1
			punpckldq mm1, mm1
			psubw mm0, mm1		// 256 - g_pnSrcWeightTable[j]
			movd mm1, edi
			pxor mm2, mm2
			punpcklbw mm1, mm2	// 0A0R0G0B
			pmullw mm0, mm1
			psraw mm0, 8		// /256
			packsswb mm0, mm0
			movd [eax], mm0	// 直接附值
			inc ecx
			add eax, 4
			jmp LOOPWIDTH

LOOPWIDTH_NORMAL:
			// g_pnSrcIndexTable[j] < nTemp
			shl edx, 2		// g_pnSrcIndexTable[j] * 4
			add edx, pSrcTemp
			mov edi, [edx]	// src颜色放在edi中
			movd mm6, [edx+4] // src后一像素
			mov esi, ecx
			add esi, g_pnSrcWeightTable
			xor edx, edx
			mov dl, [esi]	// 取出权重值放在edx中
			cmp dl, 0 
			jnz LOOPWIDTH_NORMAL_WEIGHT0  
			// 权重 == 0 时
			mov [eax], edi	// 直接附值 
			inc ecx
			add eax, 4
			jmp LOOPWIDTH
LOOPWIDTH_NORMAL_WEIGHT0:
			// 权重非0时
			movq mm0, nMax
			movd mm1, edx
			punpcklwd mm1, mm1
			punpckldq mm1, mm1
			
			psubw mm0, mm1		// mm0 = 256 - g_pnSrcWeightTable[j]
			movd mm3, edi
			pxor mm2, mm2
			punpcklbw mm3, mm2	// 0A0R0G0B
			pmullw mm0, mm3
			punpcklbw mm6, mm2	// mm6 = 后面一像素
			pmullw mm6, mm1
			paddusw mm0, mm6 
			psraw mm0, 8		// /256
			packsswb mm0, mm0
			movd [eax], mm0	// 直接附值

			inc ecx
			add eax, 4
			jmp LOOPWIDTH
LOOPWIDTH_END:

		}
#endif // WIN64
		pSrcTemp += nSrcPitch;
		pDesTemp += nDesPitch;
	}
#ifdef WIN64
	assert(0);
#else
	__asm emms
#endif // WIN64
}

// YUYV拉伸拷贝(对于多余的YUV，则变成YU0V)
LRESULT CALLBACK SY_StretchBlt_YUYV(
									IN OUT BYTE* pbyDesBuffer, 
									IN int nDesPitch,
									IN int nDesBufferWidth,
									IN int nDesBufferHeight,
									IN BYTE* pbySrcBuffer,
									IN int nSrcPitch,
									IN int nSrcBufferWidth,		// 像素点数
									IN int nSrcBufferHeight)
{
	float fScaleWidth	= nSrcBufferWidth/(float)nDesBufferWidth;
	float fScaleHeight	= nSrcBufferHeight/(float)nDesBufferHeight;
 
#if 1
	// 临时存储Y和Y+1的YUVXYUVX串
	// 每行字节数
	int nSrcBytes = nSrcBufferWidth<<2; // *3
	BYTE* pbyTempSrc1 = (BYTE*)_mm_malloc(sizeof(BYTE)*nSrcBytes, 16);
	BYTE* pbyTempSrc2 =  (BYTE*)_mm_malloc(sizeof(BYTE)*nSrcBytes, 16);
	// 临时存储目标YUVYUV串
	int nDstBytes = nDesBufferWidth<<2; // *3
	BYTE* pbyTempDes =  (BYTE*)_mm_malloc(sizeof(BYTE)*nDstBytes, 16);
	float fY = 0.0f, fModY = 0.0f, fSubModY=0.0f;
	int nY = 0;
	BYTE* pDes = pbyDesBuffer;
//	BYTE pixel[3][4];
//	int nHalfWidth = nSrcBufferWidth>>1;
	int nHalfDstWidth = nDesBufferWidth>>1;
	_declspec(align(16)) float arfTemp1[4] = {fScaleWidth, fScaleWidth, fScaleWidth, fScaleWidth};
	_declspec(align(16)) float arfTemp2[4] = {1.0f, 1.0f, 1.0f, 1.0f};
  __declspec(align(16)) static DWORD mask[]		=	{0x00FF00FF, 0x00FF00FF, 0x00FF00FF, 0x00FF00FF};
	BYTE *pTemp;
	for(int i=0; i<nDesBufferHeight; i++)
	{
		nY = (int)fY;
		fModY = fmodf(fY, 1.0f);
		fSubModY = 1.0f - fModY;
		if(nY >= nSrcBufferHeight)
		{
			memset(pbyTempDes, 0, nDstBytes);
			fY += fScaleHeight;
			pDes += nDesPitch;
			continue;
		}
		else
		{
			pTemp = pbySrcBuffer + nY*nSrcPitch;
#ifdef WIN64
			assert(0);
#else
			_asm
			{
				mov eax, pTemp
				mov edx, pbyTempSrc1
				mov ecx, nSrcBufferWidth
				neg ecx
				add ecx, 7					// 每次8
				jbe loopY1_4
loopY1_8:
				
				movdqa xmm0, [eax]
				movdqa xmm7, xmm0
				punpckhqdq xmm7, xmm0		// y1Uy2Vy3Uy4V
				movdqa xmm1, xmm0
				psrlw xmm1, 8			// U0V0U0V0
				punpcklbw xmm1, xmm1	// UU100VV00 UU200VV00
				movdqa xmm2, xmm1
				punpckhqdq xmm2, xmm1	// UU200VV00 UU100VV00

				punpcklwd xmm1, xmm2
				movdqa xmm2, xmm1
				punpckhqdq xmm2, xmm2
				punpckldq xmm1, xmm2

			//	packssdw xmm1, xmm1		// UU1VVUUVV 
			//	packssdw xmm2, xmm2		// UU2VVUUVV
			//	punpcklwd xmm1, xmm2	// UU1UU2VV1VV2

      pand xmm0, [mask]
      packuswb xmm0, xmm0
				punpcklbw xmm0, xmm1	// y1U1y2U1y3U2y4U2
				punpcklbw xmm1, xmm0	// 
				punpckhqdq xmm1, xmm1	// V1AV1AV2AV2A
				punpcklwd xmm0, xmm1	// 最终结果
				movdqa [edx], xmm0
				add edx, 16

				movdqa xmm1, xmm7
				psrlw xmm1, 8			// U0V0U0V0
				punpcklbw xmm1, xmm1	// UU100VV00 UU200VV00
				movdqa xmm2, xmm1
				punpckhqdq xmm2, xmm1	// UU200VV00 UU100VV00

				punpcklwd xmm1, xmm2
				movdqa xmm2, xmm1
				punpckhqdq xmm2, xmm2
				punpckldq xmm1, xmm2

			//	packssdw xmm1, xmm1		// UU1VVUUVV
			//	packssdw xmm2, xmm2		// UU2VVUUVV
			//	punpcklwd xmm1, xmm2	// UU1UU2VV1VV2

      pand xmm7, [mask]
      packuswb xmm7, xmm7
				punpcklbw xmm7, xmm1	// y1U1y2U1y3U2y4U2
				punpcklbw xmm1, xmm7	// 
				punpckhqdq xmm1, xmm1	// V1AV1AV2AV2A
				punpcklwd xmm7, xmm1	// 最终结果
				movdqa [edx], xmm7

				add eax, 16
				add edx, 16
				add ecx, 8
				jnc loopY1_8
loopY1_4:
				sub ecx, 4
				jns loopY1_2

				movq mm0, [eax]
				movq2dq xmm0, mm0		// y1Uy2Vy3Uy4V
				movdqa xmm1, xmm0
				psrlw xmm1, 8			// U0V0U0V0
				punpcklbw xmm1, xmm1	// UU100VV00 UU200VV00
				movdqa xmm2, xmm1
				punpckhqdq xmm2, xmm1	// UU200VV00 UU100VV00

				punpcklwd xmm1, xmm2
				movdqa xmm2, xmm1
				punpckhqdq xmm2, xmm2
				punpckldq xmm1, xmm2

			//	packssdw xmm1, xmm1		// UU1VVUUVV
			//	packssdw xmm2, xmm2		// UU2VVUUVV
			//	punpcklwd xmm1, xmm2	// UU1UU2VV1VV2

				punpcklbw xmm0, xmm1	// y1U1y2U1y3U2y4U2
				punpcklbw xmm1, xmm0	// 
				punpckhqdq xmm1, xmm1	// V1AV1AV2AV2A
				punpcklwd xmm0, xmm1	// 最终结果
				movdqa [edx], xmm0

				add eax, 8
				add edx, 16
				add ecx, 4
loopY1_2:
				sub ecx, 2
				jns skipLoopY1_2

				movd mm0, [eax]			// y1Uy2V
				movq mm1, mm0
				psrlw mm1, 8			// U0V0
				punpcklbw mm1, mm1		// UU100VV00
				movq mm2, mm2
				punpckhbw mm2, mm2
				punpcklwd mm1, mm2
			//	packssdw mm1, mm1		// UU1VV

				punpcklbw mm0, mm1		// y1U1y2U1
				psrlw mm1, mm1			// VUVU
				punpcklwd mm0, mm1	// 最终结果
				movq [edx], mm0

skipLoopY1_2:
				emms	
			}
#endif // WIN64
		}
		if(nY+1 >= nSrcBufferHeight)
			memset(pbyTempSrc2, 0, nSrcBytes);
		else
		{
			pTemp += nSrcPitch;
#ifdef WIN64
			assert(0);
#else
			_asm
			{
				mov eax, pTemp
				mov edx, pbyTempSrc2
				mov ecx, nSrcBufferWidth
				neg ecx
				add ecx, 7					// 每次8
				jbe loopY2_4
loopY2_8:
				
        movdqa xmm0, [eax]
        movdqa xmm7, xmm0
          punpckhqdq xmm7, xmm0		// y1Uy2Vy3Uy4V
          movdqa xmm1, xmm0
          psrlw xmm1, 8			// U0V0U0V0
          punpcklbw xmm1, xmm1	// UU100VV00 UU200VV00
          movdqa xmm2, xmm1
          punpckhqdq xmm2, xmm1	// UU200VV00 UU100VV00

          punpcklwd xmm1, xmm2
          movdqa xmm2, xmm1
          punpckhqdq xmm2, xmm2
          punpckldq xmm1, xmm2

          //	packssdw xmm1, xmm1		// UU1VVUUVV 
          //	packssdw xmm2, xmm2		// UU2VVUUVV
          //	punpcklwd xmm1, xmm2	// UU1UU2VV1VV2

          pand xmm0, [mask]
          packuswb xmm0, xmm0
          punpcklbw xmm0, xmm1	// y1U1y2U1y3U2y4U2
          punpcklbw xmm1, xmm0	// 
          punpckhqdq xmm1, xmm1	// V1AV1AV2AV2A
          punpcklwd xmm0, xmm1	// 最终结果
          movdqa [edx], xmm0
          add edx, 16

          movdqa xmm1, xmm7
          psrlw xmm1, 8			// U0V0U0V0
          punpcklbw xmm1, xmm1	// UU100VV00 UU200VV00
          movdqa xmm2, xmm1
          punpckhqdq xmm2, xmm1	// UU200VV00 UU100VV00

          punpcklwd xmm1, xmm2
          movdqa xmm2, xmm1
          punpckhqdq xmm2, xmm2
          punpckldq xmm1, xmm2

          //	packssdw xmm1, xmm1		// UU1VVUUVV
          //	packssdw xmm2, xmm2		// UU2VVUUVV
          //	punpcklwd xmm1, xmm2	// UU1UU2VV1VV2
         
          pand xmm7, [mask]
          packuswb xmm7, xmm7
          punpcklbw xmm7, xmm1	// y1U1y2U1y3U2y4U2
          punpcklbw xmm1, xmm7	// 
          punpckhqdq xmm1, xmm1	// V1AV1AV2AV2A
          punpcklwd xmm7, xmm1	// 最终结果
          movdqa [edx], xmm7

				add eax, 16
				add edx, 16
				add ecx, 8
				jnc loopY2_8
loopY2_4:
				sub ecx, 4
				jns loopY2_2

				movq mm0, [eax]
				movq2dq xmm0, mm0		// y1Uy2Vy3Uy4V
				movdqa xmm1, xmm0
				psrlw xmm1, 8			// U0V0U0V0
				punpcklbw xmm1, xmm1	// UU100VV00 UU200VV00
				movdqa xmm2, xmm1
				punpckhqdq xmm2, xmm1	// UU200VV00 UU100VV00

				punpcklwd xmm1, xmm2
				movdqa xmm2, xmm1
				punpckhqdq xmm2, xmm2
				punpckldq xmm1, xmm2

			//	packssdw xmm1, xmm1		// UU1VVUUVV
			//	packssdw xmm2, xmm2		// UU2VVUUVV
			//	punpcklwd xmm1, xmm2	// UU1UU2VV1VV2

				punpcklbw xmm0, xmm1	// y1U1y2U1y3U2y4U2
				punpcklbw xmm1, xmm0	// 
				punpckhqdq xmm1, xmm1	// V1AV1AV2AV2A
				punpcklwd xmm0, xmm1	// 最终结果
				movdqa [edx], xmm0

				add eax, 8
				add edx, 16
				add ecx, 4
loopY2_2:
				sub ecx, 2
				jns skipLoopY2_2

				movd mm0, [eax]			// y1Uy2V
				movq mm1, mm0
				psrlw mm1, 8			// U0V0
				punpcklbw mm1, mm1		// UU100VV00
				movq mm2, mm2
				punpckhbw mm2, mm2
				punpcklwd mm1, mm2
			//	packssdw mm1, mm1		// UU1VV

				punpcklbw mm0, mm1		// y1U1y2U1
				psrlw mm1, mm1			// VUVU
				punpcklwd mm0, mm1	// 最终结果
				movq [edx], mm0
				
skipLoopY2_2:
				emms	
			}
#endif // WIN64
		}
		//fX = 0.0f;
#ifdef WIN64
		assert(0);
#else
		_asm
		{
			MOVSS	XMM0, fModY
			MOVSS	XMM1, fSubModY
			PUNPCKLDQ XMM0, XMM0
			PUNPCKLQDQ XMM0, XMM0
			PUNPCKLDQ XMM1, XMM1
			PUNPCKLQDQ XMM1, XMM1			// 四个相同值:XMM0存4个fModY, XMM1存4个fSubModY
			pxor XMM7, XMM7
			
			mov edx, pbyTempDes
			xor eax, eax
blastloop:
			cmp eax, nDesBufferWidth
			jnl		skipblastloop			// 跳转
				
			movaps xmm2, xmm7				// fScaleWidth
			CVTPS2DQ XMM5, XMM2				// 取fScaleWidth的整数nScaleWidth
			CVTDQ2PS XMM3, XMM5				// 整数转为Float,存在xmm3
			SUBPS XMM2, XMM3				// 相减，取出其小数部分存入XMM2,fModx
			MOVAPS XMM4, [arfTemp2]
			SUBPS XMM4, XMM2				// 相减：1.0f - 小数,存入Xmm4, fSubModx
			
			movd ecx, xmm5
			cmp ecx, nSrcBufferWidth		// 如果nX >= nSrcBufferWidth
			jl blastloop1
			mov [edx], 0
			add edx, 4
			inc eax
			addps xmm7, [arfTemp1]
			jmp blastloop
blastloop1:
			
			mov esi, ecx
			shl esi, 2
			mov edi, esi
			add esi, pbyTempSrc1
			add edi, pbyTempSrc2
			movd xmm5, [esi]			// pixel0
			movd xmm6, [edi]			// pixel2
			pxor xmm3, xmm3
			punpcklbw xmm5, xmm3
			punpcklbw xmm6, xmm3
			punpcklwd xmm5, xmm3
			punpcklwd xmm6, xmm3
			cvtdq2ps xmm5, xmm5			// 转成float
			cvtdq2ps xmm6, xmm6
			movaps xmm3, xmm4
			mulps xmm3, xmm1			// fSubModY*fSubModX
			mulps xmm3, xmm5			// fSubModY*fSubModX*p0
			mulps xmm4,	xmm0			// fModY*fSubModX
			mulps xmm4, xmm6			// fModY*fSubModX*p2
			addps xmm3, xmm4

			inc ecx
			cmp ecx, nSrcBufferWidth	// 如果nX+1 >= nSrcBufferWidth
			jl blastloop2
			
			cvtps2dq xmm3, xmm3			// 转成整数
			packssdw xmm3, xmm3
			PACKUSWB xmm3, xmm3
			movd [edx], xmm3

			add edx, 4
			inc eax
			addps xmm7, [arfTemp1]
			jmp blastloop
blastloop2:
			mov esi, ecx
			shl esi, 2
			mov edi, esi
			add esi, pbyTempSrc1
			add edi, pbyTempSrc2
			movd xmm5, [esi]			// pixel1
			movd xmm6, [edi]			// pixel3
			pxor xmm4, xmm4
			punpcklbw xmm5, xmm4
			punpcklbw xmm6, xmm4
			punpcklwd xmm5, xmm4
			punpcklwd xmm6, xmm4
			cvtdq2ps xmm5, xmm5			// 转成float
			cvtdq2ps xmm6, xmm6
			movaps xmm4, xmm2
			mulps xmm2, xmm1			// fSubModY*fModX
			mulps xmm2, xmm5			// fSubModY*fModX*p1
			mulps xmm4,	xmm0			// fModY*fModX
			mulps xmm4, xmm6			// fModY*fModX*p3
			addps xmm2, xmm4
			addps xmm2, xmm3			//
			cvtps2dq xmm2,xmm2			// 转成整数
			packssdw xmm2, xmm2
			PACKUSWB xmm2, xmm2
			movd [edx], xmm2
			
			add edx, 4
			inc eax
			addps xmm7, [arfTemp1]
			jmp blastloop
skipblastloop:
			emms
		}
#endif // WIN64
		// 拆成YUYV
		for(int k=0, p=0, q=0; k<nHalfDstWidth; k++)
		{
			*(pDes+p) = *(pbyTempDes + q);
			*(pDes+p+1) = (*(pbyTempDes + q+1) + *(pbyTempDes + q+5)) >> 1;
			*(pDes+p+2) = *(pbyTempDes + q+4);
			*(pDes+p+3) = (*(pbyTempDes + q+2) + *(pbyTempDes + q+6)) >> 1;
			p += 4;
			q += 8;
		}
		fY += fScaleHeight;
		pDes += nDesPitch;
	}
	_mm_free(pbyTempSrc1);
	_mm_free(pbyTempSrc2);
	_mm_free(pbyTempDes);
#endif
	return 1;
}
_declspec(align(16)) DWORD ymask[] = {0x00ff00ff,0x00ff00ff,0x00ff00ff,0x00ff00ff,};
BOOL yuv_yyuuvv2yuv(BYTE* pDesBuffer,BYTE* pSrcField,int nA16,int nMod16)
{
#ifdef WIN64
	_yuv_yyuuvv2yuv_x64(pDesBuffer,pSrcField,nA16,nMod16,ymask);
	return TRUE;
#else
	_asm{
		PXOR	XMM0,XMM0
		MOVDQA	XMM1,ymask
		MOV		EDI,pDesBuffer
		MOV		ESI,pSrcField
		MOV		ECX,nA16
	loop_16:
		TEST	 ECX,ECX
		JZ		 loop_8
		MOVDQU	 XMM2,[ESI]
		PAND	 XMM2,XMM1
	    ADD		 ESI,16
		SUB		 ECX,1
		JNZ		 loop_16_1
		PACKUSWB XMM2,XMM2
		MOVSD	 [EDI],XMM2
		ADD		 EDI,8
		JMP		 loop_8
loop_16_1:
		MOVDQU   XMM3,[ESI]
		PAND	 XMM3,XMM1
		PACKUSWB XMM2,XMM3
		MOVDQU	 [EDI],XMM2
		ADD		 EDI,16
		ADD		 ESI,16
		SUB		 ECX,1
		JMP		 loop_16
loop_8:
		MOV		 ECX,nMod16
		TEST	 ECX,0x00000008
		JZ		 loop_4
		MOVSD	 XMM2,[ESI]
		PAND	 XMM2,XMM1
		PACKUSWB XMM2,XMM2
		MOVD	 [EDI],XMM2
		ADD		 EDI,4
		ADD		 ESI,8
loop_4: 
		TEST	 ECX,0x00000004
		JZ		 loop_2
		MOVD	 XMM2,[ESI]
		PAND	 XMM2,XMM1
		PACKUSWB XMM2,XMM2
		MOVD	 EAX,XMM2
		MOV		 [EDI],AX
		ADD		 EDI,2
		ADD		 ESI,4
loop_2:
		TEST	 ECX,0x00000002
		JZ		 loop_end
		MOV		 AX,[ESI]
		MOVD	 XMM2,EAX
		PAND	 XMM2,XMM1
		PACKUSWB XMM2,XMM2
		MOVD	 EAX,XMM2
		MOV		 [EDI],AL
		ADD		 EDI,1
		ADD		 ESI,2
loop_end:
	}

#endif
}

BOOL yuv_yuv2yyuuvv(BYTE* pDesBuffer,BYTE* pSrcField,int nA8,int nMod8)
{
#ifdef WIN64
	_yuv_yuv2yyuuvv_x64(pDesBuffer,pSrcField,nA8,nMod8);
#else
	_asm{
		PXOR	XMM0,XMM0
		MOV		EDI,pDesBuffer
		MOV		ESI,pSrcField
		MOV		ECX,nA8
	loop_8:
		TEST	 ECX,ECX
		JZ		 loop_4
		MOVSD	 XMM1,[ESI]
		PUNPCKLBW XMM1,XMM0
		MOVDQA	 XMM2,XMM1
		PSLLW	 XMM2,8
		POR		 XMM1,XMM2
		MOVDQU	 [EDI],XMM1
		SUB		 ECX,1
		ADD		 ESI,8
		ADD	     EDI,16
		JMP		 loop_8
loop_4: 
		MOV		 ECX,nMod8
		TEST	 ECX,0x00000004
		JZ		 loop_2
		MOVD	 XMM1,[ESI]
		PUNPCKLBW XMM1,XMM0
		MOVDQA	 XMM2,XMM1
		PSLLW	 XMM2,8
		POR		 XMM1,XMM2
		MOVSD	 [EDI],XMM1
		ADD		 ESI,4
		ADD	     EDI,8
loop_2:
		TEST	 ECX,0x00000002
		JZ		 loop_1
		MOV		 AX,[ESI]
		MOVD	 XMM1,EAX
		PUNPCKLBW XMM1,XMM0
		MOVDQA	 XMM2,XMM1
		PSLLW	 XMM2,8
		POR		 XMM1,XMM2
		MOVD	 [EDI],XMM1
		ADD		 EDI,4
		ADD		 ESI,2
loop_1:
		TEST	 ECX,0x00000001
		JZ		 loop_end
		MOV		 AL,[ESI]
		MOVD	 XMM1,EAX
		PUNPCKLBW XMM1,XMM0
		MOVDQA	 XMM2,XMM1
		PSLLW	 XMM2,8
		POR		 XMM1,XMM2
		MOVD	 EAX,XMM1
		MOV		 [EDI],AX
		ADD		 EDI,2
		ADD		 ESI,1
loop_end:
	}

#endif
	return 1;
}

void yuv_uyvy2yuv8(BYTE* puyvy,BYTE* py,BYTE* pu,BYTE* pv,int nA16,int nM16)
{
#ifdef WIN64
	_yuv_uyvy2yuv8_x64(puyvy,py,pu,pv,nA16,nM16);
#else
	__asm{
		MOV			ECX,nA16
		MOV			ESI,puyvy
		MOV			EAX,py
		MOV			EDI,pu
		MOV			EDX,pv
loop_16:		
		TEST		ECX,ECX
		JZ			loop_8
		MOVDQU		XMM1,[ESI]
		MOVDQA		XMM2,XMM1
		PSRLW		XMM1,8
		PACKUSWB	XMM1,XMM1
		PSLLW		XMM2,8
		PSRLW		XMM2,8
		PACKUSWB	XMM2,XMM2

		MOVDQA		XMM3,XMM2
		PSLLW		XMM2,8
		PSRLW		XMM2,8
		PACKUSWB	XMM2,XMM2
		PSRLW		XMM3,8
		PACKUSWB	XMM3,XMM3

		
		

		MOVSD		[EAX],XMM1
		MOVD		[EDI],XMM2
		MOVD		[EDX],XMM3

		ADD			ESI,16
		ADD			EAX,8
		ADD			EDX,4
		ADD			EDI,4
		SUB			ECX,1
		JMP			loop_16
loop_8:
		MOV			ECX,nM16
		TEST		ECX,0x00000008
		JZ			loop_4
		MOVSD		XMM1,[ESI]
		MOVSD		XMM2,XMM1
		PSRLW		XMM1,8
		PSLLW		XMM2,8
		PACKUSWB	XMM1,XMM1
		PSRLW		XMM2,8
		PACKUSWB	XMM2,XMM2

		MOVDQA		XMM3,XMM2
		PSLLW		XMM2,8
		PSRLW		XMM2,8
		PACKUSWB	XMM2,XMM2
		PSRLW		XMM3,8
		PACKUSWB	XMM3,XMM3

		MOVD		[EAX],XMM1
		PUSH		EAX
		MOVD		EAX,XMM2
		MOV			[EDI],AX
		MOVD		EAX,XMM3
		MOV			[EDX],AX
		POP			EAX
		ADD			ESI,8
		ADD			EAX,4
		ADD			EDX,2
		ADD			EDI,2
loop_4:
		TEST		ECX,0x00000004
		JZ			loop_end
		MOVD		XMM1,[ESI]
		MOVDQA		XMM2,XMM1
		PSRLW		XMM1,8
		PSLLW		XMM2,8
		PACKUSWB	XMM1,XMM1
		PSRLW		XMM2,8
		PACKUSWB	XMM2,XMM2

		MOVDQA		XMM3,XMM2
		PSLLW		XMM2,8
		PSRLW		XMM2,8
		PACKUSWB	XMM2,XMM2
		PSRLW		XMM3,8
		PACKUSWB	XMM3,XMM3

		PUSH		EDX
		MOVD		EDX,XMM1
		MOV			[EAX],DX
		POP			EDX

		PUSH		EAX
		MOVD		EAX,XMM2
		MOV			[EDI],AL
		MOVD		EAX,XMM3
		MOV			[EDX],AL
		POP			EAX

		ADD			ESI,8
		ADD			EAX,4
		ADD			EDX,2
		ADD			EDI,2
loop_end:
		
	};

#endif
}

void yuv_yuyv2yuv8(BYTE* puyvy,BYTE* py,BYTE* pu,BYTE* pv,int nA16,int nM16)
{
#ifdef WIN64
	_yuv_yuyv2yuv8_x64(puyvy,py,pu,pv,nA16,nM16);
#else
	__asm{
		MOV			ECX,nA16
		MOV			ESI,puyvy
		MOV			EAX,py
		MOV			EDI,pu
		MOV			EDX,pv
loop_16:		
		TEST		ECX,ECX
		JZ			loop_8
		MOVDQU		XMM1,[ESI]
		MOVDQA		XMM2,XMM1
		PSLLW		XMM1,8
		PSRLW		XMM1,8
		PACKUSWB	XMM1,XMM1
		PSRLW		XMM2,8
		PACKUSWB	XMM2,XMM2

		MOVDQA		XMM3,XMM2
		PSLLW		XMM2,8
		PSRLW		XMM2,8
		PACKUSWB	XMM2,XMM2
		PSRLW		XMM3,8
		PACKUSWB	XMM3,XMM3

		
		

		MOVSD		[EAX],XMM1
		MOVD		[EDI],XMM2
		MOVD		[EDX],XMM3

		ADD			ESI,16
		ADD			EAX,8
		ADD			EDX,4
		ADD			EDI,4
		SUB			ECX,1
		JMP			loop_16
loop_8:
		MOV			ECX,nM16
		TEST		ECX,0x00000008
		JZ			loop_4
		MOVSD		XMM1,[ESI]
		MOVSD		XMM2,XMM1
		PSLLW		XMM1,8
		PSRLW		XMM1,8
		PACKUSWB	XMM1,XMM1
		PSRLW		XMM2,8
		PACKUSWB	XMM2,XMM2

		MOVDQA		XMM3,XMM2
		PSLLW		XMM2,8
		PSRLW		XMM2,8
		PACKUSWB	XMM2,XMM2
		PSRLW		XMM3,8
		PACKUSWB	XMM3,XMM3

		MOVD		[EAX],XMM1
		PUSH		EAX
		MOVD		EAX,XMM2
		MOV			[EDI],AX
		MOVD		EAX,XMM3
		MOV			[EDX],AX
		POP			EAX
		ADD			ESI,8
		ADD			EAX,4
		ADD			EDX,2
		ADD			EDI,2
loop_4:
		TEST		ECX,0x00000004
		JZ			loop_end
		MOVD		XMM1,[ESI]
		MOVDQA		XMM2,XMM1
		PSLLW		XMM1,8
		PSRLW		XMM1,8
		PACKUSWB	XMM1,XMM1
		PSRLW		XMM2,8
		PACKUSWB	XMM2,XMM2

		MOVDQA		XMM3,XMM2
		PSLLW		XMM2,8
		PSRLW		XMM2,8
		PACKUSWB	XMM2,XMM2
		PSRLW		XMM3,8
		PACKUSWB	XMM3,XMM3

		PUSH		EDX
		MOVD		EDX,XMM1
		MOV			[EAX],DX
		POP			EDX

		PUSH		EAX
		MOVD		EAX,XMM2
		MOV			[EDI],AL
		MOVD		EAX,XMM3
		MOV			[EDX],AL
		POP			EAX

		ADD			ESI,8
		ADD			EAX,4
		ADD			EDX,2
		ADD			EDI,2
loop_end:
		
	};

#endif
}

//////////////////////////////////////////////////////////////////////////

int* MakeCurve (float  sigma, int*  length,int *&pCurveHead)
{
	int *curve;
	float sigma2;
	float l;
	int temp;
	int i, n;

	sigma2 = 2 * sigma * sigma;
	l = (float)sqrt (-sigma2 * log (1.0 / 255.0));

	n = (int)(ceil (l) * 2);
	if ((n % 2) == 0)
		n += 1;

	curve = new int[n];
	pCurveHead = curve;

	*length = n / 2;
	curve += *length;
	curve[0] = 255;

	for (i = 1; i <= *length; i++)
	{
		temp = (int) (exp (- (i * i) / sigma2) * 255);
		curve[-i] = temp;
		curve[i] = temp;
	}

	return curve;
}
BOOL GetCol(BYTE* pSrc, int nWidth, int nHeight, BYTE* src, int nCol, int nInY, int nInHeight, int nPlane)
{
	int i=0, j=0;
	int nRowBytes = nWidth*nPlane;
	BYTE* pTemp = pSrc + nInY*nRowBytes + nCol*nPlane;
	for(i=0; i<nInHeight; i++)
	{
		for(j=0; j<nPlane; j++)
			src[j] = pTemp[j];
		src += nPlane;
		pTemp += nRowBytes;
	}
	return TRUE;
}
// 获取指定行的数据
BOOL GetRow(BYTE* pSrc, int nWidth, int nHeight, BYTE* src, int nRow, int nInX, int nInWidth, int nPlane)
{
	BYTE* pTemp = pSrc + nRow*nWidth*nPlane + nInX*nPlane;
	int i=0, j=0;
	for(i=0; i<nInWidth; i++)
	{
		for(j=0; j<nPlane; j++)
			src[j] = pTemp[j];
		src += nPlane;
		pTemp += nPlane;
	}
	return TRUE;
}
// 设置指定列数据
BOOL SetCol(BYTE* pSrc, int nWidth, int nHeight, BYTE* src, int nCol, int nInY, int nInHeight, int nPlane)
{
	int i=0, j=0;
	int nRowBytes = nWidth*nPlane;
	BYTE* pTemp = pSrc + nInY*nRowBytes + nCol*nPlane;
	for(i=0; i<nInHeight; i++)
	{
		for(j=0; j<nPlane; j++)
			pTemp[j] = src[j];
		src += nPlane;
		pTemp += nRowBytes;
	}
	return TRUE;
}
// 设置指定行数据
BOOL SetRow(BYTE* pSrc, int nWidth, int nHeight, BYTE* pDesTemp, int nCurRow, int nInX, int nInWidth, int nPlane)
{
	BYTE* pTemp = pSrc + nCurRow*nWidth*nPlane + nInX*nPlane;
	int i=0, j=0;
	for(i=0; i<nInWidth; i++)
	{
		for(j=0; j<nPlane; j++)
			pTemp[j] = pDesTemp[j];
		pDesTemp += nPlane;
		pTemp += nPlane;
	}
	return TRUE;
}
// Alpha混叠
void MultiplyAlpha(BYTE *buf,
				   int    nWidth,
				   int    nBytes)
{
	int i, j;
	float alpha;
	float fTemp = 1.0f/255.0f;
	int nRowBytes = nWidth*nBytes;

	for(i=0; i<nRowBytes; i+= nBytes)
	{
		alpha = buf[i + nBytes - 1] * fTemp;
		for(j = 0; j<nBytes - 1; j++)
			buf[i + j] = (BYTE)(buf[i + j]*alpha);
	}
}
// 计算象素值
void TransferPixels(float *src1,
					float *src2,
					BYTE  *Des,
					int     width,
					int     bytes
					)
{
	int b;
	int bend = bytes * width;
	float sum;

	for(b = 0; b < bend; b++)
	{
		sum = *src1++ + *src2++;
		if (sum > 255)
			sum = 255;
		else if(sum < 0) 
			sum = 0;
		*Des++ = (BYTE) sum;
	}
}
// 分离Alpha值
void SeparateAlpha(BYTE *buf,
				   int    width,
				   int    bytes)
{
	int i, j;
	BYTE alpha;
	float recip_alpha;
	int new_val;

	for(i = 0; i<width*bytes; i+=bytes)
	{
		alpha = buf[i + bytes - 1];
		if (alpha != 0 && alpha != 255)
		{
			recip_alpha = 255.0f / alpha;
			for (j = 0; j < bytes - 1; j++)
			{
				new_val = (int)(buf[i + j] * recip_alpha);
				buf[i + j] = (BYTE)(min(255, new_val));
			}
		}
	}
}
void RunLengthEncode (BYTE *src,
					  int   *Des,
					  int    bytes,
					  int    width)
{
	int start;
	int i;
	int j;
	BYTE last;

	last = *src;
	src += bytes;
	start = 0;

	for (i = 1; i < width; i++)
	{
		if (*src != last)
		{
			for (j = start; j < i; j++)
			{
				*Des++ = (i - j);
				*Des++ = last;
			}
			start = i;
			last = *src;
		}
		src += bytes;
	}

	for (j = start; j < i; j++)
	{
		*Des++ = (i - j);
		*Des++ = last;
	}
}
BASEGRAPH_COMMON_API LRESULT Gauss_RLE(IN OUT void* pInOutBuf,	// 目标缓冲区:包含输入缓冲区、输出缓冲区
									  IN int nInPlane,		// 象素位数
									  IN int nInX,		// 输入缓冲区起点坐标 
									  IN int nInY, 
									  IN int nInWidth,		// 输入缓冲区宽、高
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
									  IN int nRefPlane)
{
	// 进行高斯操作
	BYTE *pInBuffer = (BYTE*)pInOutBuf;

	int nMax = max(nInWidth, nInHeight);
	int* pnBuf = new int[nMax*2];
	BYTE* pSrcTemp =  new BYTE[nMax*nInPlane];	// 源和目标的临时缓冲区	
	BYTE* pDesTemp =  new BYTE[nMax*nInPlane];

	int nRowBytes = nInWidth*nInPlane;		// 行字节数
	int nColBytes = nInHeight*nInPlane;		// 列字节数
	int nCurRow = 0, nCurCol = 0;			// 当前行、当前列
	int i=0, j=0;
	float fDev = 0.0f;
	int nLength = 0;
	int initial_p, initial_m;
	int nTotal = 0;
	int* pCurveHead = NULL;
	int* pnCurve = NULL;
	int* pnSumHead = NULL;
	int* pnSum = NULL;
	int pixels = 0;

	nOutWidth = nInWidth;
	nOutHeight = nInHeight;
	nOutX = nInX;
	nOutY = nInY;
	nOutPlane = nInPlane;
	if(fVertical > 0.0f)
	{
		fVertical = fabs(fVertical) + 1.0f;	// 取最大整数
		fDev = sqrt(-(fVertical*fVertical)/(2*log(1.0f/255.0f)));
		// 通过Dev值获取高斯常量
		pnCurve = MakeCurve(fDev, &nLength,pCurveHead);
		pnSum = new int[2*nLength+1];
		pnSumHead = pnSum;
		pnSum[0] = 0;
		for (i = 1; i <= nLength*2; i++)
			pnSum[i] = pnCurve[i-nLength-1] + pnSum[i-1];
		pnSum += nLength;
		nTotal = pnSum[nLength] - pnSum[-nLength];
		for(nCurCol=nInX; nCurCol<nInOutWidth; nCurCol++)
		{
			// 从InOutBuf中取得第nCol列数据
			GetCol(pInBuffer, nInOutWidth, nInOutHeight, pSrcTemp, nCurCol, nInY, nInHeight, nInPlane);
			// 每个象素与自己的Alpha值混合
			if(nInPlane == 4 && nRefPlane < 0)
				MultiplyAlpha(pSrcTemp, nInHeight, nInPlane);

			BYTE* sp_p = pSrcTemp;				// 起点
			BYTE* dp_p = pDesTemp;
			for(i=0; i<nInPlane; i++)
			{
				if(nRefPlane > 0 &&i != nRefPlane)
					continue;

				initial_p = sp_p[i];
				initial_m = sp_p[(nInHeight-1)*nInPlane+i];
				RunLengthEncode(sp_p + i, pnBuf, nInPlane, nInHeight);

				// 一象素一象素进行操作
				for(nCurRow=0; nCurRow<nInHeight; nCurRow++)
				{
					int start = (nCurRow < nLength) ? -nCurRow : -nLength;
					int end = (nInHeight <= (nCurRow + nLength) ?
						(nInHeight - nCurRow - 1) : nLength);

					int val = 0;
					j = start;
					int* bb = pnBuf + (nCurRow + j) * 2;

					if (start != -nLength)
						val += initial_p * (pnSum[start] - pnSum[-nLength]);

					while (j < end)
					{
						pixels = bb[0];
						j += pixels;
						if (j > end)
							j = end;
						val += bb[1] * (pnSum[j] - pnSum[start]);
						bb += (pixels * 2);
						start = j;
					}

					if (end != nLength)
						val += initial_m * (pnSum[nLength] - pnSum[end]);

					dp_p[nCurRow * nInPlane + i] = val / nTotal;
				}
			}
			if(nInPlane == 4)
				SeparateAlpha(pSrcTemp, nInHeight, nInPlane);
			SetCol(pInBuffer, nInOutWidth, nInOutHeight, pDesTemp, nCurCol, nInY, nInHeight, nInPlane);
		}
	}
	if(fHorizon > 0.0f)
	{
		fHorizon = fabs(fHorizon) + 1.0f;	// 取最大整数
		if(fHorizon != fVertical)
		{
			if(pCurveHead != NULL)
				delete[] pCurveHead;
			if(pnSumHead != NULL)
				delete[] pnSumHead;
			fDev = sqrt(-(fHorizon*fHorizon)/(2*log(1.0f/255.0f)));
			// 通过Dev值获取高斯常量
			pnCurve = MakeCurve(fDev, &nLength,pCurveHead);
			pnSum = new int[2*nLength+1];
			pnSumHead = pnSum;
			pnSum[0] = 0;
			for (i = 1; i <= nLength*2; i++)
				pnSum[i] = pnCurve[i-nLength-1] + pnSum[i-1];
			pnSum += nLength;
			nTotal = pnSum[nLength] - pnSum[-nLength];
		}
		for(nCurRow=nInY; nCurRow<nInOutHeight; nCurRow++)
		{
			// 从InOutBuf中取得第nCol列数据
			GetRow(pInBuffer, nInOutWidth, nInOutHeight, pSrcTemp, nCurRow, nInX, nInWidth, nInPlane);
			// 每个象素与自己的Alpha值混合
			if(nRefPlane < 0)
				MultiplyAlpha(pDesTemp, nInWidth, nInPlane);

			BYTE* sp_p = pSrcTemp;				// 起点
			BYTE* dp_p = pDesTemp;
			for(i=0; i<nInPlane; i++)
			{
				if(nRefPlane > 0 &&i != nRefPlane)
					continue;

				initial_p = sp_p[i];
				initial_m = sp_p[(nInWidth-1)*nInPlane+i];
				RunLengthEncode(sp_p + i, pnBuf, nInPlane, nInWidth);

				// 一象素一象素进行操作
				for(nCurCol=0; nCurCol<nInWidth; nCurCol++)
				{
					int start = (nCurCol < nLength) ? -nCurCol : -nLength;
					int end = (nInWidth <= (nCurCol + nLength) ?
						(nInWidth - nCurCol - 1) : nLength);

					int val = 0;
					j = start;
					int* bb = pnBuf + (nCurCol + j) * 2;

					//for(int k = -nLength;k<start;k ++)
					//{
					//	val += initial_p * (pnSum[k] - pnSum[-nLength]);
					//}
					if (start != -nLength)
						val += initial_p * (pnSum[start] - pnSum[-nLength]);

					while(j < end)
					{
						pixels = bb[0];
						j += pixels;
						if (j > end)
							j = end;
						val += bb[1] * (pnSum[j] - pnSum[start]);
						bb += (pixels * 2);
						start = j;
					}

					if (end != nLength)
						val += initial_m * (pnSum[nLength] - pnSum[end]);

					dp_p[nCurCol * nInPlane + i] = val / nTotal;
				}
			}
			SeparateAlpha(pDesTemp, nInWidth, nInPlane);
			SetRow(pInBuffer, nInOutWidth, nInOutHeight, pDesTemp, nCurRow, nInX, nInWidth, nInPlane);
		}
	}

	if(pCurveHead != NULL)
	{
		delete[] pCurveHead;
		pCurveHead = NULL;
	}
	if(pnSumHead != NULL)
	{
		delete[] pnSumHead;
		pnSumHead = NULL;
	}
	delete[] pnBuf;
	delete[] pSrcTemp;
	delete[] pDesTemp;
	pnBuf = NULL;
	pSrcTemp = NULL;
	pDesTemp = NULL;
	return 1;
}
//////////////////////////////////////////////////////////////////////////
// 双线性插值缩放时的范围剪切
// 范围有效时返回 TRUE 无效时返回 FALSE
//////////////////////////////////////////////////////////////////////////
BOOL meanstretch_currange(IN OUT int &nDestX,			// 目标起点
						  IN OUT int &nDestY,
						  IN OUT int &nDestWidth,		// 目标范围
						  IN OUT int &nDestHeight,
						  IN int nDestBufferWidth,	// 目标 Buffer 大小
						  IN int nDestBufferHeight,
						  IN OUT int &nSrcX,			// 源起点
						  IN OUT int &nSrcY,
						  IN OUT int &nSrcWidth,		// 源范围
						  IN OUT int &nSrcHeight,
						  IN int nSrcBufferWidth,		// 源 Buffer 大小
						  IN int nSrcBufferHeight,
						  OUT float &fScaleWidth,		// 缩放比例
						  OUT float &fScaleHeight,
						  OUT float &fSrcStartX,		// 源浮点起点坐标
						  OUT float &fSrcStartY,
						  OUT int &nDestAddWidth,		// 目标 Buffer 增量
						  OUT int &nLineIndex,		// 行起始
						  OUT int &nLineInc,			// 行增量
						  IN BUFFER_PARITY_STYLE eParity)	// 奇偶场类型
{
	// 边界有效性检测
	if (nDestBufferWidth <= 0 || nDestBufferHeight <= 0
		|| nSrcBufferWidth <= 0	|| nSrcBufferHeight <= 0)
		return FALSE;

	if (nDestX >= nDestBufferWidth || nDestY >= nDestBufferHeight
		|| nSrcX >= nSrcBufferWidth || nSrcY >= nSrcBufferHeight)
		return FALSE;

	if (nDestWidth <= 0 || nDestHeight <= 0 || nSrcWidth <= 0 || nSrcHeight <= 0)
		return FALSE;

	if ((nDestX + nDestWidth) <= 0 || (nDestY + nDestHeight) <= 0
		|| (nSrcX + nSrcWidth) <= 0 || (nSrcY + nSrcHeight) <= 0)
		return FALSE;

	int nTmpDestX(nDestX), nTmpDestY(nDestY), nTmpSrcX(nSrcX), nTmpSrcY(nSrcY);
	int nTmpOffset(0), nTmpWidth(0), nTmpHeight(0);

	fScaleWidth = (float)nSrcWidth / nDestWidth;	// 缩放比例
	fScaleHeight = (float)nSrcHeight / nDestHeight;
	// 剪切超出范围
	if (nDestX < 0)		// 目标左边界超出，缩放范围进行右缩进
	{
		nTmpOffset = (int)((-nDestX) * fScaleWidth + 0.5f);	// 源的右缩进量
		nDestWidth += nDestX; nDestX = 0;
		nSrcX += nTmpOffset; nSrcWidth -= nTmpOffset;
	}
	if (nDestY < 0)		// 目标上边界超出，缩放范围进行下缩进
	{
		nTmpOffset = (int)((-nDestY) * fScaleHeight + 0.5f);	// 源的下缩进量
		nDestHeight += nDestY; nDestY = 0;
		nSrcY += nTmpOffset; nSrcHeight -= nTmpOffset;
	}
	if (nSrcX < 0)		// 源左边界超出，缩放范围进行右缩进
	{
		nTmpOffset = (int)ceilf((-nSrcX) / fScaleWidth);		// 目标的右缩进量
		nSrcWidth += nSrcY; nSrcX = 0;
		nDestWidth -= nTmpOffset; nDestX += nTmpOffset;
	}
	if (nSrcY < 0)		// 源上边界超出，缩放范围进行下缩进
	{
		nTmpOffset = (int)ceilf((-nSrcY) / fScaleHeight);	// 目标的下缩进量
		nSrcHeight += nSrcY; nSrcY = 0;
		nDestHeight -= nTmpOffset; nDestY += nTmpOffset;
	}
	nTmpWidth = nDestWidth + nDestX;		// 考虑目标的右边界
	nTmpHeight = nDestHeight + nDestY;		// 考虑目标的下边界
	if (nTmpWidth > nDestBufferWidth)	// 目标右边界超出，缩放范围进行左缩进
	{
		nTmpOffset = (nTmpWidth - nDestBufferWidth);	// 目标的左缩进量
		nDestWidth -= nTmpOffset;
		nTmpOffset = (int)(nTmpOffset * fScaleWidth);	// 源的左缩进量
		nSrcWidth -= nTmpOffset;
		if (nSrcWidth <= 0)
			return FALSE;
	}
	if (nTmpHeight > nDestBufferHeight)	// 目标下边界超出，缩放范围进行上缩进
	{
		nTmpOffset = (nTmpHeight - nDestBufferHeight);	// 目标的上缩进量
		nDestHeight -= nTmpOffset;
		nTmpOffset = (int)(nTmpOffset * fScaleHeight);	// 源的上缩进量
		nSrcHeight -= nTmpOffset;
		if (nSrcHeight <= 0)
			return FALSE;
	}
	nTmpWidth = nSrcWidth + nSrcX;		// 考虑源的右边界
	nTmpHeight = nSrcHeight + nSrcY;	// 考虑源的下边界
	if (nTmpWidth > nSrcBufferWidth)	// 源右边界超出，缩放范围进行左缩进
	{
		nTmpOffset = (nTmpWidth - nSrcBufferWidth);		// 源的左缩进量
		nSrcWidth -= nTmpOffset;
		nTmpOffset = (int)ceilf(nTmpOffset / fScaleWidth);	// 目标的左缩进量
		nDestWidth -= nTmpOffset;
		if (nDestWidth <= 0)
			return FALSE;
	}
	if (nTmpHeight > nSrcBufferHeight)	// 源下边界超出，拷贝范围进行上缩进
	{
		nTmpOffset = (nTmpHeight - nSrcBufferHeight);	// 源的上缩进量
		nSrcHeight -= nTmpOffset;
		nTmpOffset = (int)ceilf(nTmpOffset / fScaleHeight);	// 目标的上缩进量
		nDestHeight -= nTmpOffset;
		if (nDestHeight <= 0)
			return FALSE;
	}
	// 源起点坐标
	fSrcStartX = fScaleWidth * (nDestX - nTmpDestX) + nTmpSrcX;
	fSrcStartY = fScaleHeight * (nDestY - nTmpDestY) + nTmpSrcY;
	// 根据奇偶场类型获得增量
	switch (eParity)
	{
	case SK_PARITY_BOTH:		// 全场
		nDestAddWidth = nDestBufferWidth;
		nLineIndex = 0;
		nLineInc = 1;
		break;

	case SK_PARITY_ODD:			// 奇场
		nLineIndex = 0;
		if ((nDestY % 2) == 1)		// 起点在偶场，则从下一行开始
		{
			nDestY++;
			nLineIndex = 1;
			// 判断范围的有效性
			if (nDestY >= nDestBufferHeight)
				return FALSE;
		}
		nDestAddWidth = (nDestBufferWidth << 1);
		nLineInc = 2;
		break;

	case SK_PARITY_EVEN:		// 偶场
		nLineIndex = 0;
		if ((nDestY % 2) == 0)		// 起点在奇场，则从下一行开始
		{
			nDestY++;
			nLineIndex = 1;
			// 判断范围的有效性
			if (nDestY >= nDestBufferHeight)
				return FALSE;
		}
		nDestAddWidth = (nDestBufferWidth << 1);
		nLineInc = 2;
		break;

	default:
		return FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 用于加权平均缩放时计算权重的接口
//////////////////////////////////////////////////////////////////////////
// 计算水平或垂直方向上某个像素的权重个数及其首尾权重
int calc_power_pixel(OUT float* pPower,		// 首尾权重表
					 IN OUT float& fStart,	// 起始点坐标值
					 IN float fStep);		// 缩放步长

// 计算水平或垂直方向上各像素的权重个数表与首尾权重表
void strech_calc_power(OUT float* pPower,		// 权重表
					   OUT int* pMap,			// 每个点占用的权重个数表
					   IN int nCount,			// 目标拷贝长度或高度
					   IN float fStep,			// 缩放步长
					   IN float fStart = 0.0f);	// 起始点坐标值

//////////////////////////////////////////////////////////////////////////
// 采用加权均值滤波法对 BGRA32 Buffer 进行缩放，使用权重与索引映射表进行逐行逐个像素处理
// 先根据垂直方向上的映射对源进行行加权平均运算，将结果存放于临时缓存中
// 而后根据水平方向上的映射对临时缓存进行列加权平均运算，得到每个像素的值
//////////////////////////////////////////////////////////////////////////
void bgra32_mean_copy_add_y(OUT void* des,			// 临时缓存行（是源行拷贝宽度的四倍）
							IN void* src,			// 源拷贝起始行
							IN int scw,				// 源拷贝宽度
							IN int src_pitch,		// 源行字节数
							IN void* pPowerY,		// 目标某行映射的垂直方向上的权重表
							IN void* pCoefH,		// 垂直方向上的缩放系数
							IN int nMapY);			// 目标一行映射到源的行数

// （缩放倍数小于255的高效算法）
void bgra32_mean_copy_add_y_quick(OUT void* des,		// 临时缓存行（是源行拷贝宽度的四倍）
								  IN void* src,			// 源拷贝起始行
								  IN int scw,			// 源拷贝宽度
								  IN int src_pitch,		// 源行字节数
								  IN void* pPowerY,		// 目标某行映射的垂直方向上的权重表
								  IN void* pCoefH,		// 垂直方向上的缩放系数
								  IN int nMapY);			// 目标一行映射到源的行数
// （缩放倍数小于255的高效算法）
void bgra32_mean_copy_add_y_quick_field(OUT void* des,		// 临时缓存行（是源行拷贝宽度的四倍）
								  IN void* src,			// 源拷贝起始行
								  IN int scw,			// 源拷贝宽度
								  IN int src_pitch,		// 源行字节数
								  IN void* pPowerY,		// 目标某行映射的垂直方向上的权重表
								  IN void* pCoefH,		// 垂直方向上的缩放系数
								  IN int nMapY,
								  IN int nFirst)			// 目标一行映射到源的行数
{
#ifdef WIN64
	_bgra32_mean_copy_add_y_quick_field_x64(des,src,scw,src_pitch,pPowerY,pCoefH,nMapY,nFirst);
#else
	_asm{
		mov			edi, des;
		mov			esi, src;
		mov			ecx, scw;
		mov			edx, pPowerY;
		movss		xmm2, [edx];		
		movss		xmm3, [edx + 0x04];	
		shufps		xmm2, xmm2, 0x00;	
		shufps		xmm3, xmm3, 0x00;
		mov			eax, pCoefH;
		pxor		xmm0, xmm0;			
		movaps		xmm7, [eax];	
		
loop_pixel:
		mov			edx, esi;
		pxor		xmm6, xmm6;
		pxor		xmm5, xmm5;
		mov			eax,nFirst;
		test		eax,eax
		mov			eax, nMapY;
		jz			loop_skip;
seg_first_row:
		// 取得第一行的像素
		movss		xmm1, [edx];		
		punpcklbw	xmm1, xmm0;			
		punpcklwd	xmm1, xmm0;			
		cvtdq2ps	xmm1, xmm1;
		mulps		xmm1, xmm2;
		add			edx, src_pitch;
		addps		xmm6, xmm1;
		add			edx, src_pitch;
		// 判断占用的像素情况
		sub			eax, 0x03;
		jb			map_end;			
		jnz			loop_mid_row;		
		jmp			seg_last_row;		
loop_skip:
		add			edx, src_pitch;
		// 判断占用的像素情况
		sub			eax, 0x02;
		jb			map_end;			
		jnz			loop_mid_row;		
		jmp			seg_last_row;
loop_mid_row:
		// 取得中间行的像素
		movd		xmm1, [edx];		
		punpcklbw	xmm1, xmm0;			
		add			edx, src_pitch;
		paddusw		xmm5, xmm1;
		add			edx, src_pitch;
		sub			eax, 0x02;
		jb			loop_mid_toend;			// 只占一个像素
		jnz			loop_mid_row;
		jz			seg_last_row;
loop_mid_toend:
		punpcklwd	xmm5, xmm0;			// 000a 000r 000g 000b
		cvtdq2ps	xmm5, xmm5;
		addps		xmm6, xmm5;			// 加权求和
		jmp         map_end;
seg_last_row:
		// 取得最后一行的像素
		movss		xmm1, [edx];		// 源像素 0 0 0 argb
		punpcklbw	xmm1, xmm0;			// 0000 0000 0a0r 0g0b
		punpcklwd	xmm1, xmm0;			// 000a 000r 000g 000b
		cvtdq2ps	xmm1, xmm1;
		punpcklwd	xmm5, xmm0;			// 000a 000r 000g 000b
		mulps		xmm1, xmm3;
		cvtdq2ps	xmm5, xmm5;
		addps		xmm6, xmm5;
		addps		xmm6, xmm1;			// 加权求和
		
map_end:
		// 加权结束
		mulps		xmm6, xmm7;			// 平均
		movaps		[edi], xmm6;
		add			esi, 0x04;
		add			edi, 0x10;
		sub			ecx, 0x01;
		jnz			loop_pixel;
	}

#endif
}

void bgra32_mean_copy_add_x(OUT void* des,
							IN int dcw,
							IN void* src,
							IN void* pPowerX,
							IN void* pMapX,
							IN void* pCoefW);



//////////////////////////////////////////////////////////////////////////
// BGRA 32 位色 Buffer 加权平均插值缩放拷贝
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL rgba_mean_copy(OUT DWORD* pDest,			// 目标 Buffer
								   IN int nDestX,				// 目标起点
								   IN int nDestY,
								   IN int nDestWidth,			// 目标缩放区域
								   IN int nDestHeight,
								   IN int nDestBufferWidth,	// 目标 Buffer 大小
								   IN int nDestBufferHeight,
								   IN DWORD* pSrc,				// 源 Buffer
								   IN int nSrcX,				// 源起点
								   IN int nSrcY,
								   IN int nSrcWidth,			// 源缩放区域
								   IN int nSrcHeight,
								   IN int nSrcBufferWidth,		// 源 Buffer 大小
								   IN int nSrcBufferHeight,
								   IN int eParity)	// 奇偶场类型
{
	if (pSrc == NULL || pDest == NULL)
		return FALSE;

	float fScaleWidth(1.0f), fScaleHeight(1.0f), fSrcStartX(0.0f), fSrcStartY(0.0f);
	int nDestAddWidth(0), nLineIndex(0), nLineInc(1);

	// 获得有效的操作范围
	if (!meanstretch_currange(nDestX, nDestY, nDestWidth, nDestHeight, nDestBufferWidth, nDestBufferHeight,
		nSrcX, nSrcY, nSrcWidth, nSrcHeight, nSrcBufferWidth, nSrcBufferHeight, fScaleWidth, fScaleHeight,
		fSrcStartX, fSrcStartY, nDestAddWidth, nLineIndex, nLineInc, (BUFFER_PARITY_STYLE)eParity))
		return FALSE;

	// 申请权重表、权重数量表及缓存空间
	float fScaleW_I = 1.0f / fScaleWidth, fScaleH_I = 1.0f / fScaleHeight;
	SYSSE_DECLARE(int*) pMapX = SYSSE_NEW(nDestWidth, int);		// X 方向上的权重数量表
	SYSSE_DECLARE(int*) pMapY = SYSSE_NEW(nDestHeight, int);	// Y 方向上的权重数量表
	SYSSE_DECLARE(float*) pPowerX = SYSSE_NEW((nDestWidth << 1), float);	// X 方向上的权重表
	SYSSE_DECLARE(float*) pPowerY = SYSSE_NEW((nDestHeight << 1), float);	// Y 方向上的权重表
	DWORD *pDesLine = pDest + nDestX + nDestY * nDestBufferWidth;		// Buffer 起始地址
	float *pTmpPowerY = pPowerY;		// Y 方向权重表地址
	int nSrcPitch = (nSrcBufferWidth << 2);
	SYSSE_DECLARE(float*) pTmpLine = SYSSE_NEW((nSrcWidth << 2), float);	// 源行的临时缓存
	SYSSE_DECLARE(float) fCoefW[4] = { fScaleW_I, fScaleW_I, fScaleW_I, fScaleW_I };
	SYSSE_DECLARE(float) fCoefH[4] = { fScaleH_I, fScaleH_I, fScaleH_I, fScaleH_I };

	// 计算权重
	strech_calc_power(pPowerX, pMapX, nDestWidth, fScaleWidth, fSrcStartX - (int)fSrcStartX);
	strech_calc_power(pPowerY, pMapY, nDestHeight, fScaleHeight, fSrcStartY - (int)fSrcStartY);
	if(nLineIndex != 0)
		nSrcY = pMapY[0] - 1;
	DWORD *pSrcLine = pSrc + nSrcX + nSrcY * nSrcBufferWidth;

	if(nLineIndex!=0)
		pTmpPowerY += 2;
	int nSrcLineIndex = nSrcY;
	int nlineodd = 0;
	// 逐行处理
	for (; nLineIndex<nDestHeight; nLineIndex+=nLineInc)
	{
		if(nLineInc == 1)
		{
			bgra32_mean_copy_add_y_quick(pTmpLine, pSrcLine, nSrcWidth, nSrcPitch, pTmpPowerY, fCoefH, pMapY[nLineIndex]);
		}
		else
		{
			nlineodd = (~(nLineIndex^nSrcLineIndex))&0x00000001;
			fCoefH[0] = 0.0f;
			if(pMapY[nLineIndex] == 1)
			{
				if(nlineodd)
				{
					fCoefH[0] += pTmpPowerY[0];
				}
				else
					fCoefH[0] += pTmpPowerY[1];
			}
			else
			{
				if(nlineodd)
				{
					fCoefH[0] += pTmpPowerY[0];
					fCoefH[0] += (pMapY[nLineIndex] - 2) >> 1; 
					if(pMapY[nLineIndex]&0x01)
						fCoefH[0] += pTmpPowerY[1];
				}
				else
				{
					fCoefH[0] += (pMapY[nLineIndex] - 2 + 1)>>1; 
					if(!(pMapY[nLineIndex]&0x01))
						fCoefH[0] += pTmpPowerY[1];
				}
			}
			//int a = 0x42FF0000;
			//float b = *((float*)(&a));
			fCoefH[0] = 1.0f/fCoefH[0];
			fCoefH[3] = fCoefH[2] = fCoefH[1] = fCoefH[0];
			//			bgra32_mean_copy_add_y_quick(pTmpLine, pSrcLine, nSrcWidth, nSrcPitch, pTmpPowerY, fCoefH, pMapY[nLineIndex]);
			bgra32_mean_copy_add_y_quick_field(pTmpLine, pSrcLine, nSrcWidth, nSrcPitch, pTmpPowerY, fCoefH, pMapY[nLineIndex],nlineodd);

		}
		bgra32_mean_copy_add_x(pDesLine, nDestWidth, pTmpLine, pPowerX, pMapX, fCoefW);
		if (nLineInc == 1)
		{
			nSrcLineIndex += (pMapY[nLineIndex] - 1);
			pSrcLine += ((pMapY[nLineIndex] - 1) * nSrcBufferWidth);
			pTmpPowerY += 2;
		}
		else
		{
			nSrcLineIndex += (pMapY[nLineIndex] - 2 + pMapY[nLineIndex + 1]);
			pSrcLine += (pMapY[nLineIndex] - 2 + pMapY[nLineIndex + 1]) * nSrcBufferWidth;
			pTmpPowerY += 4;
		}
		pDesLine += nDestAddWidth;
	}
	SYSSE_FREE(pMapX);
	SYSSE_FREE(pMapY);
	SYSSE_FREE(pPowerX);
	SYSSE_FREE(pPowerY);
	SYSSE_FREE(pTmpLine);

	return TRUE;
}
int   CreateGaussMap( int* pGuass,int degrees )
{
	//	1 - 32 ==> 4 - 256
	{
		degrees = 14*degrees+48;
		degrees*= degrees;
		degrees /=31*31;
	}
	//	高斯分布函数表达式
	//	
	//	f(d) = (1/sqrt(2*PI*t)) * exp( -d2 / 2*t )
	//	f( d1 + d2 ) = (sqrt(2*PI*t))( f(d1) + f(d2) )

	double dGuass[33];
	//	 计算高斯分布值
	{
		double dt = degrees / 2.0;		//	t
		double dm = 1/sqrt(2*3.1415926f*dt);	//	1/sqrt(2*PI*t)
		for( int d = 0; d <= 32; d++ )
		{
			dGuass[d] = dm * exp( -d*d/(2*dt) );
		}
	}
	//	 对高斯分布值求和
	double dTotal = 0;
	{
		for( int x = 1; x <= 32; x++ )
			for( int y = 0; y <= 32; y++ )
			{
				dTotal += dGuass[x] * dGuass[y];
			}
			dTotal = 1 / ( dTotal*4+dGuass[0]*dGuass[0]);	//	(x+1)*(x+1) = 4*(x+1)*x+1
	}
	{
		double dT =0.0; 
		int i = 0;
		for(i = 0; i <= 32; i++ )
		{
			dT+=dGuass[i];
			pGuass[i] = (int)(dGuass[i]*0x100 / dTotal);
			if( pGuass[i] == 0 )break;
		}
		int nSign =i;
		dT =1024.0*256.0/(dT*2.0-dGuass[0]); 
		for(i = 0; i <nSign; i++ )
			pGuass[i] =(int)(dGuass[i]*dT);
		return nSign;
	}
}
void GauseFilter( LPBYTE lpDes,LPSIZE pSize,int* pGuass,int nSign )
{
	if( nSign > 2*pSize->cx )nSign = pSize->cx/2;
	if( nSign == 0 )return;

	int nDataLength = pSize->cx*pSize->cy;
	int* pTemp = new int[nDataLength];
	//	水平方向BLUR
	{
		LPBYTE	pSrc = lpDes;
		LPINT	pDes = pTemp;
		int		nExt = nSign-1;
		int	x,y;
		for( y = 0; y < pSize->cy; y++,pSrc += pSize->cx,pDes += pSize->cx )
		{			
			register LPINT pLDes=pDes;
			for( x = 0; x < pSize->cx; x++,pLDes++)
			{
				*pLDes =0;
				for(int nG=-nExt;nG<=nExt;nG++)
				{
					int nSrc = nG+x;
					if(nSrc<0)
						nSrc=0;
					else if(nSrc>=pSize->cx)
						nSrc=pSize->cx-1;
					if(nG>0)
						*pLDes+= pSrc[nSrc]*pGuass[nG];
					else
						*pLDes+= pSrc[nSrc]*pGuass[-nG];
				}
				*pLDes>>=18;
			}
		}
	}

	//	垂直方向BLUR
	{
		LPINT	pSrc = pTemp;
		LPBYTE	pDes = lpDes;
		int		nExt = nSign-1;
		int	x,y;
		for( x = 0; x < pSize->cx; x++,pSrc++,pDes++ )
		{
			register LPBYTE pLDes=pDes;
			for( y = 0; y < pSize->cy; y++,pLDes+=pSize->cx )
			{
				int nRes =0;
				for(int nG=-nExt;nG<=nExt;nG++)
				{
					int nSrc = nG+y;
					if(nSrc<0)
						nSrc=0;
					else if(nSrc>=pSize->cy)
						nSrc=pSize->cy-1;
					if(nG>0)
						nRes+= pSrc[nSrc*pSize->cx]*pGuass[nG];
					else
						nRes+= pSrc[nSrc*pSize->cx]*pGuass[-nG];
				}
				nRes>>=18;
				if(nRes>255)
					*pLDes=255;
				else
					*pLDes =(BYTE)nRes;

			}
		}
	}
	delete pTemp;
}
BASEGRAPH_COMMON_API void SY_GuassianBlur(LPBYTE lpDes,LPSIZE pSize,int degrees )
{
	int nSide = degrees*2+1;
	int nGuass[33];

	int  nSign = CreateGaussMap( nGuass,degrees );
	GauseFilter( lpDes,pSize,nGuass,nSign );
}
#endif

