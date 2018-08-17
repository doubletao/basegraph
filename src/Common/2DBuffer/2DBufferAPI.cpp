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

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////////
// 不缩放时的范围剪切，可选目标裁减矩形。
// 如果裁减矩形为空指针，则表示裁减区覆盖整个目标。
// 范围无效时返回 FALSE 有效时返回 TRUE
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL BaseGraphFix_CutRange(IN OUT int &nDesX,				// 目标起点
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
BOOL BaseGraphStretch_CutRange(IN OUT int &nDesX,				// 目标起点
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
BASEGRAPH_COMMON_API BOOL BaseGraphFill_ClipRange(IN OUT int &nX,			// 填充起点
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
BASEGRAPH_COMMON_API void Common_GrayscaleColrReplace(IN OUT DWORD* pBuffer, //RGBA
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
BASEGRAPH_COMMON_API void Common_ColrWeightMix(IN OUT DWORD* pBuffer,	//RGBA
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
BASEGRAPH_COMMON_API void BaseGraphMemcpy_Line(OUT void *des,	// 目标
									   IN void *src,	// 源
									   IN int nlen)		// 字节长度
{
	memcpy_line(des, src, (nlen >> 4), (nlen & 0x0f));
}

// 一行 Buffer 的反向拷贝操作
BASEGRAPH_COMMON_API void BaseGraphMemcpy_RevChar(OUT void *des,	// 目标
										  IN void *src,		// 源
										  IN int nlen)		// 字节长度
{
	memcpy_rev_char(des, src, nlen, (nlen >> 4), (nlen & 0x0f));
}
BASEGRAPH_COMMON_API void BaseGraphMemcpy_RevDWORD(OUT DWORD *pDes,	// 目标
										   IN DWORD *pSrc,	// 源
										   IN int nsize)	// 双字的个数
{
	memcpy_rev_dword(pDes, pSrc, (nsize << 2), (nsize >> 2), ((nsize & 0x03) << 2));
}

// 一列 Buffer 的拷贝
// 如果 bUpDown 为 TRUE 则从 pDes 开始填充下面的列，否则填充上面的列。
// 内部不做越界的安全检测，要求调用者自己注意。
BASEGRAPH_COMMON_API void BaseGraphMemcpy_ColumnDWORD(OUT DWORD* pDes,		// 目标起点
											  IN int nHeight,		// 拷贝的列高（所指向的内存必须有效）
											  IN int nPitch,		// 目标一行字节宽度
											  IN DWORD* pSrc,		// 源一行缓存（长度必须不小于nHeight）
											  IN BOOL bUpDown)		// 从上往下拷贝顺序的标志，否则反向
{
	memcpy_dword_col(pDes, pSrc, (bUpDown) ? nPitch : -nPitch, (nHeight >> 2), ((nHeight & 0x03) << 2));
}

// 一行 Buffer 的设值操作
BASEGRAPH_COMMON_API void BaseGraphMemset_LineChar(OUT void *des,		// 目标
										   IN char value,		// 字节值
										   IN int nlen)			// 字节长度
{
	memset_char_line(des, value, (nlen >> 4), (nlen & 0x0f));
}
BASEGRAPH_COMMON_API void BaseGraphMemset_LineDWORD(OUT DWORD *pDes,	// 目标
											IN DWORD value,		// 双字值
											IN int nsize)		// 双字的个数
{
	memset_dword_line(pDes, value, (nsize >> 2), ((nsize & 0x03) << 2));
}

// 一行Buffer的Alpha值替换操作，包含掩码颜色的使用。
BASEGRAPH_COMMON_API void BaseGraphMemset_LineAlpha(IN OUT DWORD *pDes,		// 目标
											IN char alpha,			// alpha 值
											IN int nsize,			// 双字长度
											IN Common_FILL_MODE eMode,	// 填充模式
											IN DWORD pdwMaskCr)		// 掩码颜色值（依赖模式的可选项）
{
	switch (eMode)
	{
	case Common_FILL_MASK:		// 只填充颜色为 pdwMaskCr 的像素
		memset_alpha_line_mask(pDes, alpha, pdwMaskCr, (nsize >> 2), ((nsize & 0x03) << 2));
		break;
	case Common_FILL_NOT_MASK:	// 不填充颜色为 pdwMaskCr 的像素
		memset_alpha_line_notmask(pDes, alpha, pdwMaskCr, (nsize >> 2), ((nsize & 0x03) << 2));
		break;
	default:		// 全部填充
		memset_alpha_line_all(pDes, alpha, (nsize >> 2), ((nsize & 0x03) << 2));
		break;
	}
}

// 一行 Buffer 的翻转操作
BASEGRAPH_COMMON_API void BaseGraphMemrev_LineChar(IN OUT void *buf,	// 缓存
										   IN int nlen)			// 字节长度
{
	memrev_char_line(buf, ((BYTE*)buf + nlen), (nlen >> 5), ((nlen >> 1) & 0x0f));
}
BASEGRAPH_COMMON_API void BaseGraphMemrev_LineDWORD(IN OUT DWORD *pBuf,	// 缓存
											IN int nsize)		// 双字的个数
{
	memrev_dword_line(pBuf, (pBuf + nsize), (nsize >> 3), (((nsize >> 1) & 0x03) << 2));
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//		（2）对大片 32 位内存的拷贝、混叠、变换等操作
//////////////////////////////////////////////////////////////////////////

// 一行 Buffer 的 MIX 操作
BASEGRAPH_COMMON_API void BGRA32_LineMix(IN OUT void *des,		// 目标
										  IN void *src,			// 源
										  IN int na16,			// 宽度除以 16 的商
										  IN int nm16)			// 宽度除以 16 的余数
{
	bgra32_line_mix(des, src, na16, nm16, BufferMemSSE::g_pdwMixGainTable);
}
BASEGRAPH_COMMON_API void BGRA32_LineMix_NotSrc(IN OUT void *des,		// 目标
												 IN void *src,			// 源
												 IN int na16,			// 宽度除以 16 的商
												 IN int nm16)			// 宽度除以 16 的余数
{
	bgra32_line_mix_notsrc(des, src, na16, nm16, BufferMemSSE::g_pdwMixGainTable);
}

// 第三方权重的填充操作：des = c0 * f + c1 * (1 - f) = (c0 - c1) * f + c1
BASEGRAPH_COMMON_API void BGRA32_PowerFill(OUT DWORD* pDes,	// 目标
											IN DWORD dwC0,		// 起点颜色
											IN DWORD dwC1,		// 终点颜色
											IN short* pPower,	// 权重被放大 14 位
											IN int nWidth)		// 像素个数
{
	bgra32_powerfill(pDes, dwC0, dwC1, pPower, (nWidth >> 3), ((nWidth & 0x07) << 2));
}
// 更快速的权重填充，不需要计算权重表，权重仍然被放大 14 位
BASEGRAPH_COMMON_API void BGRA32_PowerFill_Q(OUT DWORD* pDes,		// 目标
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
BASEGRAPH_COMMON_API void BGRA32_LineBaseAR_Recursive(OUT DWORD* pDes,		// 目标
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
BASEGRAPH_COMMON_API BOOL Common_BitBlt_N(IN OUT CommonBufferDef32* pDesBuf,	// 目标 Buffer
								     IN int nDesX,
									 IN int nDesY,
									 IN int nDesWidth,					// 目标操作大小
									 IN int nDesHeight,
									 IN CommonBufferDef32* pSrcBuf,			// 源 Buffer
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
BASEGRAPH_COMMON_API BOOL Common_StretchBlt_N(IN OUT CommonBufferDef32* pDesBuf,	// 目标 Buffer
									     IN int nDesX,
										 IN int nDesY,
										 IN int nDesWidth,					// 目标操作大小
										 IN int nDesHeight,
										 IN CommonBufferDef32* pSrcBuf,			// 源 Buffer
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
BASEGRAPH_COMMON_API BOOL Common_StretchBlt_Mean(IN OUT CommonBufferDef32* pDesBuf,		// 目标 Buffer
											IN int nDesX,
											IN int nDesY,
											IN int nDesWidth,					// 目标操作大小
											IN int nDesHeight,
											IN CommonBufferDef32* pSrcBuf,			// 源 Buffer
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
BASEGRAPH_COMMON_API BOOL Common_BitBlt(IN OUT DWORD* pdwDesBuffer,		// 目标
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
BASEGRAPH_COMMON_API BOOL Common_StretchBlt(IN OUT DWORD* pdwDesBuffer,		// 目标缓存
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
// it's compitable with "vector4f" declared in the project "BaseGraphCommon\TPVector".
// To get low coupling between "BaseGraphCommon" and "CommonBufferOperate", this declaration
// is very important, and it makes "CommonBufferOperate" independent.
BASEGRAPH_COMMON_API BOOL Common_BufferTransform(IN OUT CommonBufferDef32* pDesBuf,			// 目标缓存
											IN int nOffsetLeft,						// 目标缓存在投影平面坐标系下的位置
											IN int nOffsetTop,
											IN CommonBufferDef32* pSrcBuf,				// 源缓存
											IN int nSrcWidth,						// 源操作区域大小（相对于缓存起点而言）
											IN int nSrcHeight,
											IN void* pvfXYZW,						// 源操作区域的四个角点经过一系列变换后的顶点数据（必须有四个，顺序为(0,0),(w,0),(w,h),(h,0)）
											IN CommonBufferDef32* pMaskBuf)				// 掩码缓存
{
	if ((pDesBuf == NULL) || (pDesBuf->m_dwBuffer == NULL)
		|| (pSrcBuf == NULL) || (pSrcBuf->m_dwBuffer == NULL)
		|| (pvfXYZW == NULL))
		return FALSE;

	//检测是否没有做斜切形变，如果没有则使用快速的线性变换算法
	LPCommonINNERVERTEX t_pvfXYZW =  (LPCommonINNERVERTEX)pvfXYZW;
	//if (t_pvfXYZW[0].w == 1 && t_pvfXYZW[1].w == 1 && t_pvfXYZW[2].w == 1 && t_pvfXYZW[3].w == 1)
	{
		BOOL bResult = Common_BufferTransformLinear(pDesBuf,nOffsetLeft,nOffsetTop,pSrcBuf,nSrcWidth,nSrcHeight,(vector4f*)pvfXYZW,pMaskBuf);
		return bResult;
	}


	BaseGraphInnerVertex*  pvLeftEdgeUVW = BaseGraphSSE_NEW(pDesBuf->m_nHeight, BaseGraphInnerVertex);		// @  1.f/w  v/w  u/w
	BaseGraphInnerVertex*  pvDeltaUVW = BaseGraphSSE_NEW(pDesBuf->m_nHeight, BaseGraphInnerVertex);		// @  dw  dv  du
	DWORD* *ppdwDesLeftEdge = BaseGraphSSE_NEW(pDesBuf->m_nHeight, DWORD*);				// 目标中左边缘的起始位置（要考虑拖出去的情况）
	int *pnLinePixels = BaseGraphSSE_NEW(pDesBuf->m_nHeight, int);						// 每行要处理的像素个数
	int nMaxLinePixels(0);					// 最长行的像素个数
	BaseGraphSSE_DECLARE(DWORD*) ppdwSrcStart[4] = { 0 };		// 源的起点
	float *pfPowerX = NULL, *pfPowerY = NULL, *pfPowerXY = NULL;
	DWORD* pdwSrcIndex = NULL;				// 双线性插值所需要的索引（四个像素 00  01  10  11）
	// 临时指针
	LPCommonINNERVERTEX pvTmpLeftEdgeUVW = pvLeftEdgeUVW, pvTmpDeltaUVW = pvDeltaUVW;
	DWORD* *ppdwTmpDesLeftEdge = ppdwDesLeftEdge;
	int *pnTmpLinePixels = pnLinePixels;

	memset_dword_line(ppdwDesLeftEdge, 0, (pDesBuf->m_nHeight >> 2), ((pDesBuf->m_nHeight & 0x03) << 2));
	memset_dword_line(pnLinePixels, 0, (pDesBuf->m_nHeight >> 2), ((pDesBuf->m_nHeight & 0x03) << 2));
	memset_dword_line(ppdwSrcStart, (DWORD)(pSrcBuf->m_dwBuffer + pSrcBuf->m_nOriginX + pSrcBuf->m_rtBound.left + (pSrcBuf->m_nOriginY + pSrcBuf->m_rtBound.top) * pSrcBuf->m_nWidth), 1, 0);
	// 求解按行扫描的左右边界，以及边界上的 u/w、v/w、1/w、delta等。
	//if (!SYTrans_CalcBorder(pDesBuf, nOffsetLeft, nOffsetTop, (LPCommonINNERVERTEX)pvfXYZW, pvLeftEdgeUVW,
	//						pvDeltaUVW, ppdwDesLeftEdge, pnLinePixels, &nMaxLinePixels))
	//	return FALSE;
	// 申请临时缓存
	pfPowerX = BaseGraphSSE_NEW(nMaxLinePixels, float);
	pfPowerY = BaseGraphSSE_NEW(nMaxLinePixels, float);
	pfPowerXY = BaseGraphSSE_NEW(nMaxLinePixels, float);
	pdwSrcIndex = BaseGraphSSE_NEW((nMaxLinePixels << 2), DWORD);
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
	BaseGraphSSE_FREE(pvLeftEdgeUVW);
	BaseGraphSSE_FREE(pvDeltaUVW);
	BaseGraphSSE_FREE(ppdwDesLeftEdge);
	BaseGraphSSE_FREE(pnLinePixels);
	BaseGraphSSE_FREE(pfPowerX);
	BaseGraphSSE_FREE(pfPowerY);
	BaseGraphSSE_FREE(pfPowerXY);
	BaseGraphSSE_FREE(pdwSrcIndex);

	return TRUE;
}

struct STU_Rect
{
	int left;
	int top;
	int width;
	int height;
};

struct STU_deltaUV
{
	float dxu; //目标buffer中x方向单位长度增量引起的源buffer中位置的x方向增量
	float dxv; //目标buffer中x方向单位长度增量引起的源buffer中位置的y方向增量
	float dyu; //目标buffer中y方向单位长度增量引起的源buffer中位置的x方向增量
	float dyv; //目标buffer中y方向单位长度增量引起的源buffer中位置的y方向增量
};

inline int CommonBufferDef32_AreaW(const CommonBufferDef32* p)
{
	return (int)(p->m_rtBound.right - p->m_rtBound.left);
}

inline int CommonBufferDef32_AreaH(const CommonBufferDef32* p)
{
	return (int)(p->m_rtBound.bottom - p->m_rtBound.top);
}

inline int CommonBufferDef32_AreaX(const CommonBufferDef32* p)
{
	return (int)(p->m_nOriginX + p->m_rtBound.left);
}

inline int CommonBufferDef32_AreaY(const CommonBufferDef32* p)
{
	return (int)(p->m_nOriginY + p->m_rtBound.top);
}

//把浮点数向上取整转换为整数
//#define CEIL_F2I(f) ( (int)f+((f) > 0) )
#define CEIL_F2I(f) ((int)ceilf(f))

//把浮点数向下取整转换为整数
//#define FLOOR_F2I(f) ( (int)f - ((f) < 0) )
#define FLOOR_F2I(f) ((int)floorf(f))

//把bool值转换为正负1，真对应+1，假对应-1
#define BOOL_TO_SIGN_ONE(b) ((((int)(b))<<1) - 1)

//交换两个整数的值，对于int,char,long等整数类型都适用
#define SWAP_INT(a,b) \
	do{					\
	(a) ^= (b);			\
	(b) ^= (a);			\
	(a) ^= (b);			\
	}while(false)

void RefillTransparentPixel(DWORD& left_top, DWORD& right_top, DWORD& left_bottom, DWORD& right_bottom);
BOOL Common_BufferTransformLineBaseAR_cuda(IN OUT CommonBufferDef32* pDesBuf,			// 目标缓存
	IN int nOffsetLeft,						// 目标缓存在投影平面坐标系下的位置
	IN int nOffsetTop,
	IN CommonBufferDef32* pSrcBuf,				// 源缓存
	IN int nSrcWidth,							// 源操作区域大小（相对于缓存起点而言）
	IN int nSrcHeight,
	IN vector4f* pvfXYZW,					// 源操作区域的四个角点经过一系列变换后的顶点数据（必须有四个，顺序为(0,0),(w,0),(w,h),(h,0)）
	IN CommonBufferDef32* pMaskBuf);				// 掩码缓存

DWORD Bilinear_interpolate(DWORD left_top,			//左上角插值点颜色，RGBA
	DWORD right_top,			//左上角插值点颜色，RGBA
	DWORD left_bottom,		//左上角插值点颜色，RGBA
	DWORD right_bottom,		//左上角插值点颜色，RGBA
	float x_weight,			//左上角插值点的x方向权重
	float y_weight)			//左上角插值点的y方向权重
{
	//直接使用浮点数计算
	const float weight_lt = (x_weight*y_weight);
	const float weight_rt = y_weight - weight_lt;
	const float weight_lb = x_weight - weight_lt;
	const float weight_rb = 1 - y_weight - x_weight + weight_lt;
	//------------------------------------------------------------------------
	// 先做alpha预乘，插值完毕再做alpha除法(需要排除分母为0的情况)
	DWORD dst = 0;
	//取出alpha并存储
	int alpha_lt = GetAValue(left_top);
	int alpha_lb = GetAValue(left_bottom);
	int alpha_rt = GetAValue(right_top);
	int alpha_rb = GetAValue(right_bottom);

	const float alpha_dst = alpha_lt*weight_lt + alpha_lb*weight_lb + alpha_rt*weight_rt + alpha_rb*weight_rb;
	if (alpha_dst == 0)
		return dst;

	int byte_lt = alpha_lt*GetRValue(left_top);
	int byte_lb = alpha_lb*GetRValue(left_bottom);
	int byte_rt = alpha_rt*GetRValue(right_top);
	int byte_rb = alpha_rb*GetRValue(right_bottom);
	dst |= ((int)((byte_lt*weight_lt + byte_lb*weight_lb + byte_rt*weight_rt + byte_rb*weight_rb) / alpha_dst)) & 0xff;

	byte_lt = alpha_lt*GetGValue(left_top);
	byte_lb = alpha_lb*GetGValue(left_bottom);
	byte_rt = alpha_rt*GetGValue(right_top);
	byte_rb = alpha_rb*GetGValue(right_bottom);
	dst |= (((int)((byte_lt*weight_lt + byte_lb*weight_lb + byte_rt*weight_rt + byte_rb*weight_rb) / alpha_dst)) & 0xff) << 8;

	byte_lt = alpha_lt*GetBValue(left_top);
	byte_lb = alpha_lb*GetBValue(left_bottom);
	byte_rt = alpha_rt*GetBValue(right_top);
	byte_rb = alpha_rb*GetBValue(right_bottom);
	dst |= (((int)((byte_lt*weight_lt + byte_lb*weight_lb + byte_rt*weight_rt + byte_rb*weight_rb) / alpha_dst)) & 0xff) << 16;

	dst |= (((int)alpha_dst) & 0xff) << 24;
	return dst;
}

//定义存储边界的结构并分配空间。此处也许可以采用目标buffer的空间来存储边界信息，而不另外开辟空间
struct Edge_info
{
	int		type;	//标识该行当前已经处理了几个点，可取值：0，1，2，另外，取-1表示该行没有有效的完全点
	union
	{
		float	fv;
		int		iv;
	}pos[2];		//该行第一个和最后一个完全点的相对坐标（相对于box的左边界）
	float	u;		//该行第一个完全点的U
	float	v;		//该行第一个完全点的V
};

void Fill_inner_pixel_simle(DWORD* t_pDst, DWORD* t_pSrc, Edge_info* edge_info, int src_buff_width,
	int dst_buff_width, int box_height, float dxu, float dxv)
{
	//每行要记录的信息：行首的UV，行首的目标buffer指针
	DWORD* dst_line_head = t_pDst;
	DWORD* cur_dst = NULL;
	//每行要记录的信息：行首的UV，行首的目标buffer指针
	//每行中每个点扫描时要记录的信息：目标buffer指针，行的起始结束点位置，当前点的UV
	int line_start = 0; //行起始结束位置都是相对于行首的
	int line_end = 0;	//行内有效位置应该小于等于line_end
	Edge_info* p_cur_edge_info = edge_info;
	float cur_u = 0;
	float cur_v = 0;
	int column_loop_max = box_height - 1;
	for (int i = column_loop_max; i >= 0; --i) //仅仅是循环计数用
	{
		if (p_cur_edge_info->type == 2)
		{
			line_start = p_cur_edge_info->pos[0].iv + 1;
			line_end = p_cur_edge_info->pos[1].iv - 1;

			//------------------------------------------------------------------------
			//计算扫描起始点的UV和目标buffer的指针
			cur_u = p_cur_edge_info->u + dxu;
			cur_v = p_cur_edge_info->v + dxv;
			cur_dst = dst_line_head + line_start;

			//------------------------------------------------------------------------
			//循环扫描当前行
			for (int j = line_end - line_start; j >= 0; --j) //仅仅是循环计数用
			{
				//取整映射到源坐标，读取四个源点颜色
				int left = FLOOR_F2I(cur_u);
				int top = FLOOR_F2I(cur_v);
				//读取四个插值点的颜色
				*cur_dst = *(t_pSrc + src_buff_width*top + left);

				//行内扫描的信息更新
				cur_u += dxu;
				cur_v += dxv;
				++cur_dst;
			}
		}
		//------------------------------------------------------------------------
		//处理行扫描的信息更新
		dst_line_head += dst_buff_width;
		++p_cur_edge_info;
	}
}

//intel优化后的SSE2版本
void Fill_inner_pixel_intel_opt_SSE2(DWORD* t_pDst, DWORD* t_pSrc, Edge_info* edge_info,
	int src_buff_width, int dst_buff_width, int box_height, float dxu, float dxv)
{
	__declspec (align(16)) float cur_v_u[4];
	__declspec (align(16)) float dxv_dxu[4] = { 2.0f*dxu, 2.0f*dxv, 2.0f*dxu, 2.0f*dxv };
	__declspec (align(16)) float ones[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//__declspec (align(16)) int one = 1;
	__declspec (align(16)) short src_offest[8] = { 1,src_buff_width, 1,src_buff_width, 0,0,0,0 };
	__declspec (align(16)) int	zero[4] = { 0, 0, 0, 0 };
	__declspec (align(16)) int	temp[4] = { 0, 0, 0, 0 };
	__declspec (align(16)) int	tempA[4] = { 0, 0, 0, 0 };

	__declspec (align(16)) int	alpha_mask[4] = { 0, 0xff0000, 0, 0xff0000 };
	__declspec (align(16)) int	alpha_one_mask[4] = { 1, 0, 1, 0 };
	__declspec (align(16)) int	alpha_ps_mask[4] = { 0, 0, 0, 0xffffffff };
	__declspec (align(16)) float one_ps_mask[4] = { 1.f, 0, 0, 0 };
	__declspec (align(16)) float one_ps_mask2[4] = { 0, 0, 0, 1.f };

	//每行要记录的信息：行首的UV，行首的目标buffer指针
	DWORD* dst_line_head = t_pDst;
	DWORD* cur_dst = NULL;
	//每行要记录的信息：行首的UV，行首的目标buffer指针
	//每行中每个点扫描时要记录的信息：目标buffer指针，行的起始结束点位置，当前点的UV
	int line_start = 0; //行起始结束位置都是相对于行首的
	int line_end = 0;	//行内有效位置应该小于等于line_end
	Edge_info* p_cur_edge_info = edge_info;
	//float cur_u = 0;
	//float cur_v = 0;

	int column_loop_max = box_height - 1;
	//提前判断来去掉循环中的条件分支
	{
		p_cur_edge_info = edge_info;
		int begin_pos = 0;
		for (begin_pos = column_loop_max; begin_pos >= 0; --begin_pos)
		{
			if (p_cur_edge_info->type == 2)
				break;
			++p_cur_edge_info;
		}
		begin_pos = column_loop_max - begin_pos;
		p_cur_edge_info = edge_info + column_loop_max;
		int end_pos = 0;
		for (end_pos = column_loop_max; end_pos >= 0; --end_pos)
		{
			if (p_cur_edge_info->type == 2)
				break;
			--p_cur_edge_info;
		}
		column_loop_max = end_pos - begin_pos;
		p_cur_edge_info = edge_info + begin_pos;
		dst_line_head += begin_pos*dst_buff_width;
	}

	for (int i = column_loop_max; i >= 0; --i) //仅仅是循环计数用
	{
		//两边向中间收缩1个像素，因为这个像素已经在绘制边界时绘制过了，并且可能不是完全点
		line_start = p_cur_edge_info->pos[0].iv + 1;
		line_end = p_cur_edge_info->pos[1].iv - 1;

		//------------------------------------------------------------------------
		//计算扫描起始点的UV和目标buffer的指针
		//cur_u = p_cur_edge_info->u + dxu;
		//cur_v = p_cur_edge_info->v + dxv;
		cur_v_u[0] = p_cur_edge_info->u + dxu;
		cur_v_u[1] = p_cur_edge_info->v + dxv;
		cur_v_u[2] = cur_v_u[0] + dxu;		// for processing 2 pixels per iteration
		cur_v_u[3] = cur_v_u[1] + dxv;
		cur_dst = dst_line_head + line_start;

		//@keycode 1 : 关键汇编代码：扫描目标中一行的某个连续区域的像素点，计算其在源中的映射位置并进行双线性插值
		//------------------------------------------------------------------------
		//循环扫描当前行
		/* 原始buffer中像素排列序号
		像素1  像素2  像素5  像素6
		像素3  像素4  像素7  像素8
		*/
		int num_pix = line_end - line_start + 1;
		_Fill_inner_pixel(t_pSrc, src_buff_width, cur_dst, num_pix, cur_v_u, dxv_dxu, src_offest, alpha_mask, alpha_one_mask, alpha_ps_mask, one_ps_mask2, one_ps_mask);
		 //------------------------------------------------------------------------
		 //处理行扫描的信息更新
		dst_line_head += dst_buff_width;
		++p_cur_edge_info;
	}
	return;
}

void Fill_inner_pixel(DWORD* t_pDst, DWORD* t_pSrc, Edge_info* edge_info,
	int src_buff_width, int dst_buff_width,
	int box_height, float dxu, float dxv)
{
	Fill_inner_pixel_intel_opt_SSE2(t_pDst, t_pSrc, edge_info,
		src_buff_width, dst_buff_width,
		box_height,
		dxu, dxv);
	return;

	const bool using_asm_version = false;
	if (using_asm_version)
	{
		//每行要记录的信息：行首的UV，行首的目标buffer指针
		DWORD* dst_line_head = t_pDst;
		DWORD* cur_dst = NULL;
		//每行要记录的信息：行首的UV，行首的目标buffer指针
		//每行中每个点扫描时要记录的信息：目标buffer指针，行的起始结束点位置，当前点的UV
		int line_start = 0; //行起始结束位置都是相对于行首的
		int line_end = 0;	//行内有效位置应该小于等于line_end
		Edge_info* p_cur_edge_info = edge_info;
		float cur_u = 0;
		float cur_v = 0;
		int column_loop_max = box_height - 1;
		//提前判断来去掉循环中的条件分支
		{
			p_cur_edge_info = edge_info;
			int begin_pos = 0;
			for (begin_pos = column_loop_max; begin_pos >= 0; --begin_pos)
			{
				if (p_cur_edge_info->type == 2)
					break;
				++p_cur_edge_info;
			}
			begin_pos = column_loop_max - begin_pos;
			p_cur_edge_info = edge_info + column_loop_max;
			int end_pos = 0;
			for (end_pos = column_loop_max; end_pos >= 0; --end_pos)
			{
				if (p_cur_edge_info->type == 2)
					break;
				--p_cur_edge_info;
			}
			column_loop_max = end_pos - begin_pos;
			p_cur_edge_info = edge_info + begin_pos;
			dst_line_head += begin_pos*dst_buff_width;
		}

		for (int i = column_loop_max; i >= 0; --i) //仅仅是循环计数用
		{
			//两边向中间收缩1个像素，因为这个像素已经在绘制边界时绘制过了，并且可能不是完全点
			line_start = p_cur_edge_info->pos[0].iv + 1;
			line_end = p_cur_edge_info->pos[1].iv - 1;

			//------------------------------------------------------------------------
			//计算扫描起始点的UV和目标buffer的指针
			cur_u = p_cur_edge_info->u + dxu;
			cur_v = p_cur_edge_info->v + dxv;
			cur_dst = dst_line_head + line_start;

			//------------------------------------------------------------------------
			//循环扫描当前行
			assert(0);
			//------------------------------------------------------------------------
			//处理行扫描的信息更新
			dst_line_head += dst_buff_width;
			++p_cur_edge_info;
		}

		return;
	}

	//每行要记录的信息：行首的UV，行首的目标buffer指针
	DWORD* dst_line_head = t_pDst;
	DWORD* cur_dst = NULL;
	//每行要记录的信息：行首的UV，行首的目标buffer指针
	//每行中每个点扫描时要记录的信息：目标buffer指针，行的起始结束点位置，当前点的UV
	int line_start = 0; //行起始结束位置都是相对于行首的
	int line_end = 0;	//行内有效位置应该小于等于line_end
	Edge_info* p_cur_edge_info = edge_info;
	float cur_u = 0;
	float cur_v = 0;
	int column_loop_max = box_height - 1;
	//提前判断来去掉循环中的条件分支
	{
		p_cur_edge_info = edge_info;
		int begin_pos = 0;
		for (begin_pos = column_loop_max; begin_pos >= 0; --begin_pos)
		{
			if (p_cur_edge_info->type == 2)
				break;
			++p_cur_edge_info;
		}
		begin_pos = column_loop_max - begin_pos;
		p_cur_edge_info = edge_info + column_loop_max;
		int end_pos = 0;
		for (end_pos = column_loop_max; end_pos >= 0; --end_pos)
		{
			if (p_cur_edge_info->type == 2)
				break;
			--p_cur_edge_info;
		}
		column_loop_max = end_pos - begin_pos;
		p_cur_edge_info = edge_info + begin_pos;
		dst_line_head += begin_pos*dst_buff_width;
	}
	assert(column_loop_max < box_height);
	for (int i = column_loop_max; i >= 0; --i) //仅仅是循环计数用
	{
		//if (p_cur_edge_info->type == 2)
		{
			line_start = p_cur_edge_info->pos[0].iv + 1;
			line_end = p_cur_edge_info->pos[1].iv - 1;

			//------------------------------------------------------------------------
			//计算扫描起始点的UV和目标buffer的指针
			cur_u = p_cur_edge_info->u + dxu;
			cur_v = p_cur_edge_info->v + dxv;
			cur_dst = dst_line_head + line_start;

			//------------------------------------------------------------------------
			//循环扫描当前行
			for (int j = line_end - line_start; j >= 0; --j) //仅仅是循环计数用
			{
				//取整映射到源坐标，读取四个源点颜色
				int left = FLOOR_F2I(cur_u);
				int top = FLOOR_F2I(cur_v);
				const float x_weight = 1 - (cur_u - left);
				const float y_weight = 1 - (cur_v - top);
				//读取四个插值点的颜色
				DWORD* const src_line_above = t_pSrc + src_buff_width*top + left;
				DWORD* const src_line_below = src_line_above + src_buff_width;
				DWORD left_top = *src_line_above;
				DWORD right_top = *(src_line_above + 1);
				DWORD left_bottom = *src_line_below;
				DWORD right_bottom = *(src_line_below + 1);
				//assert(left_top == 0x55555555 
				//	&& right_top == 0x55555555 
				//	&& left_bottom == 0x55555555 
				//	&& right_bottom == 0x55555555);
				RefillTransparentPixel(left_top, right_top, left_bottom, right_bottom);
				*cur_dst = Bilinear_interpolate(left_top, right_top, left_bottom, right_bottom, x_weight, y_weight);

				//行内扫描的信息更新
				cur_u += dxu;
				cur_v += dxv;
				++cur_dst;
			}
		}
		//------------------------------------------------------------------------
		//处理行扫描的信息更新
		dst_line_head += dst_buff_width;
		++p_cur_edge_info;
	}
}

//------------------------------------------------------------------------
//测试：填充目标区域的边界方便观察
void Test_FillDstBound(CommonBufferDef32* pDesBuf)
{
	DWORD* dst_head = pDesBuf->m_dwBuffer;
	int width = pDesBuf->m_nWidth;
	int height = pDesBuf->m_nHeight;
	const int dst_buff_width = pDesBuf->m_nPitch >> 2; //目标buffer的原始区域宽度
	memset(dst_head, -1, sizeof(DWORD)*width);
	dst_head += dst_buff_width;
	for (int i = 1; i < height - 1; ++i)
	{
		*dst_head = -1;
		*(dst_head + width - 1) = -1;
		dst_head += dst_buff_width;
	}
	memset(dst_head, -1, sizeof(DWORD)*width);
}

void CalcMinBoundingBox(LPCommonINNERVERTEX &t_pvfXYZW, STU_Rect &box, CommonBufferDef32* pSrcBuf, CommonBufferDef32* pDesBuf, int nOffsetLeft, int nOffsetTop)
{
	//------------------------------------------------------------------------
	// 计算四个目标点的最小包围矩形box的左上角点坐标和长宽
	{
		float max_value = max(max(t_pvfXYZW[0].x, t_pvfXYZW[1].x), max(t_pvfXYZW[2].x, t_pvfXYZW[3].x));
		float min_value = min(min(t_pvfXYZW[0].x, t_pvfXYZW[1].x), min(t_pvfXYZW[2].x, t_pvfXYZW[3].x));
		box.left = FLOOR_F2I(min_value);
		//box.width = CEIL_F2I(max_value) - box.left;
		box.width = (int)(max_value)+1 - box.left; //此处的行为不是ceil，例如：当刚好max_value是128.000时128应该是一个可取的点，所以宽度应该是129
		max_value = max(max(t_pvfXYZW[0].y, t_pvfXYZW[1].y), max(t_pvfXYZW[2].y, t_pvfXYZW[3].y));
		min_value = min(min(t_pvfXYZW[0].y, t_pvfXYZW[1].y), min(t_pvfXYZW[2].y, t_pvfXYZW[3].y));
		box.top = FLOOR_F2I(min_value);
		//box.height = CEIL_F2I(max_value) - box.top;
		box.height = (int)(max_value)+1 - box.top;
	}

	//------------------------------------------------------------------------
	//为羽化边缘对box做适当扩大：按照最大可能的放大比例来扩充一个源边缘像素带来的影响
	{
		float dx = fabs(t_pvfXYZW[0].x - t_pvfXYZW[1].x);
		float dy = fabs(t_pvfXYZW[0].y - t_pvfXYZW[1].y);
		int dlen = CEIL_F2I(sqrtf(dx*dx + dy*dy) / CommonBufferDef32_AreaW(pSrcBuf));
		box.left -= dlen;
		box.width += dlen << 1;
		dx = fabs(t_pvfXYZW[2].x - t_pvfXYZW[1].x);
		dy = fabs(t_pvfXYZW[2].y - t_pvfXYZW[1].y);
		dlen = CEIL_F2I(sqrtf(dx*dx + dy*dy) / CommonBufferDef32_AreaH(pSrcBuf));
		box.top -= dlen;
		box.height += dlen << 1;
	}

	//------------------------------------------------------------------------
	//通过目标buffer的大小和偏移对box进行剪裁
	{
		//目前不清楚pDesBuf中的rtBound的含义，忽略之
		//先处理宽度方向
		int min_pos = pDesBuf->m_nOriginX + nOffsetLeft;
		int max_pos = min_pos + pDesBuf->m_nWidth;
		if (max_pos < box.left + box.width) //右边界裁剪
			box.width = max_pos - box.left;
		if (min_pos > box.left) //左边界裁剪
		{
			box.width -= min_pos - box.left;
			box.left = min_pos;
		}
		//再处理高度方向
		min_pos = pDesBuf->m_nOriginY + nOffsetTop;
		max_pos = min_pos + pDesBuf->m_nHeight;
		if (max_pos < box.top + box.height) //下边界裁剪
			box.height = max_pos - box.top;
		if (min_pos > box.top) //上边界裁剪
		{
			box.height -= min_pos - box.top;
			box.top = min_pos;
		}
	}
}

//------------------------------------------------------------------------
//测试：把源图像直接填充到目标区域看看
void Test_FillDstWithSrc(DWORD* t_pSrc, DWORD* t_pDst, CommonBufferDef32* pSrcBuf, CommonBufferDef32* pDesBuf)
{
	DWORD* pSrc = NULL;
	DWORD* pDst = NULL;
	DWORD* src_head = t_pSrc;
	DWORD* dst_head = t_pDst;
	int width = pSrcBuf->m_nWidth + 2;
	int height = pSrcBuf->m_nHeight + 2;
	const int src_buff_width = pSrcBuf->m_nPitch >> 2; //源buffer的原始区域宽度
	const int dst_buff_width = pDesBuf->m_nPitch >> 2; //目标buffer的原始区域宽度
	memset(dst_head, -1, sizeof(DWORD)*width);
	src_head += src_buff_width;
	dst_head += dst_buff_width;
	for (int i = 1; i < height - 1; ++i)
	{
		*dst_head = -1;
		*(dst_head + width - 1) = -1;
		memcpy(dst_head + 1, src_head, sizeof(DWORD)*(width - 2));
		src_head += src_buff_width;
		dst_head += dst_buff_width;
	}
	memset(dst_head, -1, sizeof(DWORD)*width);
}

//------------------------------------------------------------------------
//计算最小包围矩形box的左上角点的U,V和全局的dU,dV（因为是线性变换，所以全局dUdV是相同的）
void CalcDeltaUV(CommonBufferDef32* pSrcBuf, LPCommonINNERVERTEX &t_pvfXYZW, STU_Rect &box, STU_deltaUV &duv)
{
	/* mathematica中求解得到的符号解
	x0\x1\x3\y0\y1\y3是变换后四个角点的坐标
	xlt == box.left; ylt == box.top; w == box.width; h == box.height; ult == u_left_top; vlt == v_left_top;

	Solve[{	(x0-xlt)*dxu+(y0-ylt)*dyu+ult==0,
	(x0-xlt)*dxv+(y0-ylt)*dyv+vlt==0,
	(x1-xlt)*dxu+(y1-ylt)*dyu+ult==w-1,
	(x1-xlt)*dxv+(y1-ylt)*dyv+vlt==0,
	(x3-xlt)*dxu+(y3-ylt)*dyu+ult==0,
	(x3-xlt)*dxv+(y3-ylt)*dyv+vlt==h-1},{dxu,dxv,dyu,dyv,ult,vlt}]//FullSimplify

	dxu->((-1+w) (y0-y3))/(x3 (-y0+y1)+x1 (y0-y3)+x0 (-y1+y3)),
	dyu->((-1+w) (x0-x3))/(x3 (y0-y1)+x0 (y1-y3)+x1 (-y0+y3)),
	dxv->((-1+h) (y0-y1))/(x3 (y0-y1)+x0 (y1-y3)+x1 (-y0+y3)),
	dyv->((-1+h) (x0-x1))/(x3 (-y0+y1)+x1 (y0-y3)+x0 (-y1+y3)),
	ult->((-1+w) (xlt (-y0+y3)+x3 (y0-ylt)+x0 (-y3+ylt)))/(x3 (y0-y1)+x0 (y1-y3)+x1 (-y0+y3)),
	vlt->((-1+h) (xlt (y0-y1)+x0 (y1-ylt)+x1 (-y0+ylt)))/(x3 (y0-y1)+x0 (y1-y3)+x1 (-y0+y3))
	分解：
	mx = w - 1;
	my = h - 1;
	dy03 = y0-y3;
	dx03 = x0-x3;
	dy01 = y0-y1;
	dx01 = x0-x1;
	denominator = (x3*dy01 + x0*(y1-y3) - x1*dy03;
	dxu-> -	mx * dy03 / denominator,
	dyu->	mx * dx03 / denominator,
	dxv->	my * dy01 / denominator,
	dyv-> -	my * dx01 / denominator,
	ult->	mx * (xlt (-y0+y3)+x3 (y0-ylt)+x0 (-y3+ylt))/denominator,
	vlt->	my * (xlt (y0-y1)+x0 (y1-ylt)+x1 (-y0+ylt)))/denominator
	*/
	const int max_x = CommonBufferDef32_AreaW(pSrcBuf) - 1;
	const int max_y = CommonBufferDef32_AreaH(pSrcBuf) - 1;
	const float& x0 = t_pvfXYZW[0].x;
	const float& y0 = t_pvfXYZW[0].y;
	const float& x1 = t_pvfXYZW[1].x;
	const float& y1 = t_pvfXYZW[1].y;
	const float& x3 = t_pvfXYZW[3].x;
	const float& y3 = t_pvfXYZW[3].y;
	const int& xlt = box.left;
	const int& ylt = box.top;
	const float dy03 = y0 - y3;
	const float dx03 = x0 - x3;
	const float dy01 = y0 - y1;
	const float dx01 = x0 - x1;
	const float denominator = x3*dy01 + x0*(y1 - y3) - x1*dy03;
	duv.dxu = -max_x * dy03 / denominator;
	duv.dyu = max_x * dx03 / denominator;
	duv.dxv = max_y * dy01 / denominator;
	duv.dyv = -max_y * dx01 / denominator;
}

//------------------------------------------------------------------------
//计算最小包围矩形box的左上角点的U,V和全局的dU,dV（因为是线性变换，所以全局dUdV是相同的）,采用[0,1)区间规范化的结果
void CalcDeltaUVandLTUV(CommonBufferDef32* pSrcBuf, LPCommonINNERVERTEX &t_pvfXYZW, STU_Rect &box, STU_deltaUV &duv, float& ult, float& vlt)
{
	const int max_x = pSrcBuf->m_nWidth - 1;
	const int max_y = pSrcBuf->m_nHeight - 1;
	const float& x0 = t_pvfXYZW[0].x;
	const float& y0 = t_pvfXYZW[0].y;
	const float& x1 = t_pvfXYZW[1].x;
	const float& y1 = t_pvfXYZW[1].y;
	const float& x3 = t_pvfXYZW[3].x;
	const float& y3 = t_pvfXYZW[3].y;
	const int& xlt = box.left;
	const int& ylt = box.top;
	const float dy03 = y0 - y3;
	const float dx03 = x0 - x3;
	const float dy01 = y0 - y1;
	const float dx01 = x0 - x1;
	const float denominator = x3*dy01 + x0*(y1 - y3) - x1*dy03;
	duv.dxu = -(max_x * dy03 / denominator) / pSrcBuf->m_nWidth;
	duv.dyu = (max_x * dx03 / denominator) / pSrcBuf->m_nWidth;
	duv.dxv = (max_y * dy01 / denominator) / pSrcBuf->m_nHeight;
	duv.dyv = -(max_y * dx01 / denominator) / pSrcBuf->m_nHeight;
	ult = (max_x * (xlt*(-y0 + y3) + x3*(y0 - ylt) + x0*(-y3 + ylt)) / denominator) / (pSrcBuf->m_nWidth - 1);
	vlt = (max_y * (xlt*(y0 - y1) + x0*(y1 - ylt) + x1*(-y0 + ylt)) / denominator) / (pSrcBuf->m_nHeight - 1);
}

//------------------------------------------------------------------------
//先计算边界，并填充边缘像素，之后就可以只做完整的定长循环像素渲染
//完全点：指其UV映射的坐标在源buffer中都是有效的坐标，不会超出边界
//之所以没有使用之前的行扫描并根据u和v的取值范围反算出完全点区间，是由于
//这个反算过程中有除法，相当影响效率的
void CalcEdge(Edge_info* edge_info, STU_Rect &box, CommonBufferDef32* pSrcBuf, LPCommonINNERVERTEX &t_pvfXYZW, STU_deltaUV &duv)
{
	memset(edge_info, 0, sizeof(Edge_info)*box.height);

	const int src_width = CommonBufferDef32_AreaW(pSrcBuf);
	const int src_height = CommonBufferDef32_AreaH(pSrcBuf);
	const int src_x = CommonBufferDef32_AreaX(pSrcBuf);
	const int src_y = CommonBufferDef32_AreaY(pSrcBuf);
	//遍历四条边，计算每行的完全点坐标范围。每条边只处理起点和中间部分，不处理终点
	const struct { int idx; float u; float v; } vertex_index[5] =
	{
		{ 0,		0,							0 },
		{ 1,		(float)src_width - 1,			0 },
		{ 2,		(float)src_width - 1,			(float)src_height - 1 },
		{ 3,		0,							(float)src_height - 1 },
		{ 0,		0,							0 }
	};
	Edge_info* p_cur_edge_info = NULL;
	for (int i = 0; i < 4; ++i)
	{
		const float sxf = t_pvfXYZW[vertex_index[i].idx].x; //边的起点x坐标
		const float syf = t_pvfXYZW[vertex_index[i].idx].y; //边的起点y坐标
		const float exf = t_pvfXYZW[vertex_index[i + 1].idx].x; //边的终点x坐标
		const float eyf = t_pvfXYZW[vertex_index[i + 1].idx].y; //边的终点y坐标
		const int syi = FLOOR_F2I(syf);
		const int eyi = FLOOR_F2I(eyf);
		//计算x对y(目标buffer中的坐标)增长率
		//const float dx = ( (eyi == syi) ? 0 : (exf - sxf)/(eyf - syf) );
		const float dx = (fabs(eyf - syf) < 0.0001 ? 0 : (exf - sxf) / (eyf - syf));
		//计算该边上UV对y的增长率
		const float du = duv.dxu*dx + duv.dyu;
		const float dv = duv.dxv*dx + duv.dyv;
		//计算有效的起点和终点(相对于box顶边的坐标)，ey是最后一个点的坐标(不与起点重合时比终点小1，重合时就是起点的坐标)
		int sy = syi - box.top;
		int ey = (eyi == syi) ? sy : (eyi - BOOL_TO_SIGN_ONE(eyi > syi) - box.top); //加或减1是因为不处理边的终点
		if (sy > ey)
			SWAP_INT(sy, ey);
		sy = max(sy, 0);
		ey = min(ey, box.height - 1); //不能超过box底部
									  //计算取整后的起点的UV
		const float dy_start = sy + box.top - syf; //y向取整的起点距离边的起点的y向增量
		const float sx = sxf + dy_start*dx - box.left; //y向取整的起点的精确x坐标（相对于box的左边）
		const float su = vertex_index[i].u + dy_start * du; //边上的整数起点的U
		const float sv = vertex_index[i].v + dy_start * dv; //边上的整数起点的V
															//遍历当前边的有效行并记录交点的x及其UV
		p_cur_edge_info = edge_info + sy;
		float cur_u = su;
		float cur_v = sv;
		float cur_x = sx;
		assert(sy >= 0 && ey < box.height);
		const int min_u = src_x;
		const int max_u = src_x + src_width - 2; //width-1点可以取，但是双线性插值还要去右边一点，所以完全点最大只能取到width-2
		const int min_v = src_y;
		const int max_v = src_y + src_height - 2; //-2原因同max_u
		for (int j = sy; j <= ey; ++j)
		{
			if (p_cur_edge_info->type == 0)
			{
				p_cur_edge_info->pos[0].fv = cur_x;
				p_cur_edge_info->u = cur_u;
				p_cur_edge_info->v = cur_v;
			}
			else if (p_cur_edge_info->type == 1)
			{
				if (p_cur_edge_info->pos[0].fv > cur_x)
				{
					p_cur_edge_info->pos[1].iv = FLOOR_F2I(p_cur_edge_info->pos[0].fv);
					p_cur_edge_info->pos[0].iv = CEIL_F2I(cur_x);
					//可能两个浮点值很接近并且没有跨越整数，从而导致取整运算后pos1 > pos2，为解决之需要进行必要的交换
					if (p_cur_edge_info->pos[0].iv > p_cur_edge_info->pos[1].iv)
						SWAP_INT(p_cur_edge_info->pos[0].iv, p_cur_edge_info->pos[1].iv);
					//校正由于x取整导致的UV误差
					const float dx_error = p_cur_edge_info->pos[0].iv - cur_x;
					p_cur_edge_info->u = dx_error*duv.dxu + cur_u;
					p_cur_edge_info->v = dx_error*duv.dxv + cur_v;
					//assert(p_cur_edge_info->u >= min_u - 1 && p_cur_edge_info->u <= max_u + 2
					//	&& p_cur_edge_info->v >= min_v - 1 && p_cur_edge_info->v <= max_v + 2);
				}
				else
				{
					p_cur_edge_info->pos[1].iv = FLOOR_F2I(cur_x);
					float dx_error = p_cur_edge_info->pos[0].fv;
					p_cur_edge_info->pos[0].iv = CEIL_F2I(p_cur_edge_info->pos[0].fv);
					//可能两个浮点值很接近并且没有跨越整数，从而导致取整运算后pos1 > pos2，为解决之需要进行必要的交换
					if (p_cur_edge_info->pos[0].iv > p_cur_edge_info->pos[1].iv)
						SWAP_INT(p_cur_edge_info->pos[0].iv, p_cur_edge_info->pos[1].iv);
					//校正由于x取整导致的UV误差
					dx_error = p_cur_edge_info->pos[0].iv - dx_error;
					p_cur_edge_info->u += dx_error*duv.dxu;
					p_cur_edge_info->v += dx_error*duv.dxv;
					//assert(p_cur_edge_info->u >= min_u - 1 && p_cur_edge_info->u <= max_u + 2
					//	&& p_cur_edge_info->v >= min_v - 1 && p_cur_edge_info->v <= max_v + 2);
				}
				//注意：调整后的边界点的x坐标可能超出目标buffer的区域(例如几乎水平的边做了y向取整偏移后可能远远超出给定目标边界)
				//所以这里要对始末点的坐标进行超出截断处理
				if (p_cur_edge_info->pos[0].iv < 0)
				{
					float dx_error = 0.0f - p_cur_edge_info->pos[0].iv;
					p_cur_edge_info->u += dx_error*duv.dxu;
					p_cur_edge_info->v += dx_error*duv.dxv;
					p_cur_edge_info->pos[0].iv = 0;
				}
				if (p_cur_edge_info->pos[1].iv >= box.width - 1)
				{
					p_cur_edge_info->pos[1].iv = box.width - 1;
				}
				//调整左边界
				const int t_max_left = p_cur_edge_info->pos[1].iv - 1;
				float t_next_u = p_cur_edge_info->u;
				float t_next_v = p_cur_edge_info->v;
				for (int k = p_cur_edge_info->pos[0].iv; k <= t_max_left; ++k)
				{
					t_next_u += duv.dxu;
					t_next_v += duv.dxv;
					if ((t_next_u >= min_u && t_next_u <= max_u
						&& t_next_v >= min_v && t_next_v <= max_v) || k == t_max_left)
					{
						if (p_cur_edge_info->pos[0].iv != k)
						{
							p_cur_edge_info->pos[0].iv = k;
							p_cur_edge_info->u = t_next_u - duv.dxu;
							p_cur_edge_info->v = t_next_v - duv.dxv;
						}
						break;
					}
				}
				//调整右边界
				const int t_min_right = p_cur_edge_info->pos[0].iv + 1;
				const int t_row_len = p_cur_edge_info->pos[1].iv - p_cur_edge_info->pos[0].iv;
				t_next_u = p_cur_edge_info->u + t_row_len*duv.dxu;
				t_next_v = p_cur_edge_info->v + t_row_len*duv.dxv;
				for (int k = p_cur_edge_info->pos[1].iv; k >= t_min_right; --k)
				{
					t_next_u -= duv.dxu;
					t_next_v -= duv.dxv;
					if ((t_next_u >= min_u && t_next_u <= max_u
						&& t_next_v >= min_v && t_next_v <= max_v) || k == t_min_right)
					{
						if (p_cur_edge_info->pos[1].iv != k)
						{
							p_cur_edge_info->pos[1].iv = k;
						}
						break;
					}
				}
				//目前仍然有某些情形下导致下面的断言失败，待查
				//assert(p_cur_edge_info->u >= min_u - duv.dxu && p_cur_edge_info->u <= max_u + duv.dxu
				//	&& p_cur_edge_info->v >= min_v - duv.dxv && p_cur_edge_info->v <= max_v + duv.dxv);
			}
			else
			{
				//在很特殊的情况下会进入这个分支：
				// 1、目标四边形高度方向被挤压到只有一个像素
				// 2、目标四边形高度方向被挤压到只有两个像素并且其中三个都处在同一行
				// 处理方案是取所有可能值中最大和最小的作为边界
				// 可见这里分成type为0、1、2或者更多的情况分别处理不合适，应该改成一开始就
				//		初始化为[box.width,-1]这个无效区间，然后每次比较新值的floor及ceil取整值
				//		和两端，从而得到完全点区间。另外，在接下来的渲染边界部分要先判断in_bound，
				//		而不能假定第一个点永远都in bound.
				//assert(0);
			}
			++p_cur_edge_info->type;

			++p_cur_edge_info;
			cur_u += du;
			cur_v += dv;
			cur_x += dx;
		}
	}
}

//处理透明度为0的像素
void RefillTransparentPixel(DWORD& left_top, DWORD& right_top, DWORD& left_bottom, DWORD& right_bottom)
{
	//优先行结合
	if (GetAValue(left_top) == 0)
	{
		if (GetAValue(right_top) != 0)
			left_top = right_top;
		else if (GetAValue(left_bottom) != 0)
			left_top = left_bottom;
		else
			left_top = right_bottom;
		left_top &= 0xffffff;
	}
	if (GetAValue(right_top) == 0)
	{
		if (GetAValue(left_top) != 0)
			right_top = left_top;
		else if (GetAValue(right_bottom) != 0)
			right_top = right_bottom;
		else
			right_top = left_bottom;
		right_top &= 0xffffff;
	}
	if (GetAValue(left_bottom) == 0)
	{
		if (GetAValue(right_bottom) != 0)
			left_bottom = right_bottom;
		else if (GetAValue(left_top) != 0)
			left_bottom = left_top;
		else
			left_bottom = right_top;
		left_bottom &= 0xffffff;
	}
	if (GetAValue(right_bottom) == 0)
	{
		if (GetAValue(left_bottom) != 0)
			right_bottom = left_bottom;
		else if (GetAValue(right_top) != 0)
			right_bottom = right_top;
		else
			right_bottom = left_top;
		right_bottom &= 0xffffff;
	}
}

//------------------------------------------------------------------------
//遍历边界并渲染边缘
void RenderEdge(CommonBufferDef32* pSrcBuf, CommonBufferDef32* pDesBuf, Edge_info* edge_info, DWORD* t_pDst, STU_Rect &box, DWORD* t_pSrc, STU_deltaUV &duv)
{
	const int src_buff_width = pSrcBuf->m_nPitch >> 2; //源buffer的原始区域宽度
	const int dst_buff_width = pDesBuf->m_nPitch >> 2; //目标buffer的原始区域宽度

	const int src_width = CommonBufferDef32_AreaW(pSrcBuf);
	const int src_height = CommonBufferDef32_AreaH(pSrcBuf);
	const int src_x = CommonBufferDef32_AreaX(pSrcBuf);
	const int src_y = CommonBufferDef32_AreaY(pSrcBuf);

	Edge_info* p_cur_edge_info = edge_info;
	DWORD* dst_line_head = t_pDst;
	DWORD* cur_dst = NULL;
	int cur_rx = 0; //当前点相对于box左边界的坐标
	const bool test_render_edge = true;
	float cur_u = 0;
	float cur_v = 0;
	int cur_dst_pos = 0;	//记录当前扫描的目标点位置
	for (int i = 0; test_render_edge && i < box.height; ++i)
	{
		if (p_cur_edge_info->type == 2)
		{
			//填充左边缘
			cur_dst_pos = p_cur_edge_info->pos[0].iv;
			cur_dst = dst_line_head + cur_dst_pos;
			cur_u = p_cur_edge_info->u;
			cur_v = p_cur_edge_info->v;
			bool in_bound = cur_u >= -1 && cur_u <= src_width - 1
				&& cur_v >= -1 && cur_v <= src_height - 1
				&& cur_dst_pos >= 0 && cur_dst_pos < box.width;
			while (in_bound)
			{
				//取整映射到源坐标，读取四个源点颜色
				int left = FLOOR_F2I(cur_u);
				int top = FLOOR_F2I(cur_v);
				//计算权值
				float x_weight = 1 - (cur_u - left);
				float y_weight = 1 - (cur_v - top);
				//读取四个插值点的颜色
				DWORD* src_line_above = t_pSrc + src_buff_width*top + left;
				DWORD* src_line_below = src_line_above + src_buff_width;
				DWORD left_top = 0;
				DWORD right_top = 0;
				DWORD left_bottom = 0;
				DWORD right_bottom = 0;
				if (left >= 0 && top >= 0)
				{
					left_top = *src_line_above;
					//assert(left_top == 0x55555555);
				}
				if (left < src_width - 1 && top >= 0)
				{
					right_top = *(src_line_above + 1);
					//assert(right_top == 0x55555555);
				}
				if (left >= 0 && top < src_height - 1)
				{
					left_bottom = *src_line_below;
					//assert(left_bottom == 0x55555555);
				}
				if (left < src_width - 1 && top < src_height - 1)
				{
					right_bottom = *(src_line_below + 1);
					//assert(right_bottom == 0x55555555);
				}

				//*cur_dst = RGBA(255,255,255,255);
				//进行双线性插值并写目标缓存
				//*cur_dst = left_top;
				*cur_dst = Bilinear_interpolate(left_top, right_top, left_bottom, right_bottom, x_weight, y_weight);
				//*cur_dst = RGBA(255,0,0,255);

				cur_u -= duv.dxu;
				cur_v -= duv.dxv;
				--cur_dst_pos;
				in_bound = cur_u >= -1 && cur_u <= src_width - 1
					&& cur_v >= -1 && cur_v <= src_height - 1
					&& cur_dst_pos >= 0 && cur_dst_pos < box.width;
				--cur_dst;
			}
			//填充右边缘
			cur_dst_pos = p_cur_edge_info->pos[1].iv;
			cur_dst = dst_line_head + cur_dst_pos;
			const int ling_len = p_cur_edge_info->pos[1].iv - p_cur_edge_info->pos[0].iv;
			cur_u = p_cur_edge_info->u + ling_len*duv.dxu;
			cur_v = p_cur_edge_info->v + ling_len*duv.dxv;
			in_bound = cur_u >= -1 && cur_u <= src_width - 1
				&& cur_v >= -1 && cur_v <= src_height - 1
				&& cur_dst_pos >= 0 && cur_dst_pos < box.width;
			while (in_bound)
			{
				//取整映射到源坐标，读取四个源点颜色
				int left = FLOOR_F2I(cur_u);
				int top = FLOOR_F2I(cur_v);
				//计算权值
				float x_weight = 1 - (cur_u - left);
				float y_weight = 1 - (cur_v - top);
				//读取四个插值点的颜色
				DWORD* src_line_above = t_pSrc + src_buff_width*top + left;
				DWORD* src_line_below = src_line_above + src_buff_width;
				DWORD left_top = 0;
				DWORD right_top = 0;
				DWORD left_bottom = 0;
				DWORD right_bottom = 0;
				if (left >= 0 && top >= 0)
				{
					left_top = *src_line_above;
					//assert(left_top == 0x55555555);
				}
				if (left < src_width - 1 && top >= 0)
				{
					right_top = *(src_line_above + 1);
					//assert(right_top == 0x55555555);
				}
				if (left >= 0 && top < src_height - 1)
				{
					left_bottom = *src_line_below;
					//assert(left_bottom == 0x55555555);
				}
				if (left < src_width - 1 && top < src_height - 1)
				{
					right_bottom = *(src_line_below + 1);
					//assert(right_bottom == 0x55555555);
				}

				//*cur_dst = RGBA(255,255,255,255);
				//进行双线性插值并写目标缓存
				//*cur_dst = left_top;
				*cur_dst = Bilinear_interpolate(left_top, right_top, left_bottom, right_bottom, x_weight, y_weight);
				//*cur_dst = RGBA(255,0,0,255);

				cur_u += duv.dxu;
				cur_v += duv.dxv;
				++cur_dst_pos;
				in_bound = cur_u >= -1 && cur_u <= src_width - 1
					&& cur_v >= -1 && cur_v <= src_height - 1
					&& cur_dst_pos >= 0 && cur_dst_pos < box.width;
				++cur_dst;
			}
		}

		//更新循环状态
		dst_line_head += dst_buff_width;
		++p_cur_edge_info;
	}
}

//------------------------------------------------------------------------
//计算源区域buffer的起始点指针
DWORD* CalcSrcHead(CommonBufferDef32* pSrcBuf)
{
	return pSrcBuf->m_dwBuffer + (CommonBufferDef32_AreaY(pSrcBuf)*pSrcBuf->m_nPitch / sizeof(DWORD) + CommonBufferDef32_AreaX(pSrcBuf));
}

//------------------------------------------------------------------------
//计算目标区域buffer的起始点指针
DWORD* CalcDstHead(CommonBufferDef32* pDesBuf, STU_Rect &box, int nOffsetTop, int nOffsetLeft)
{
	DWORD* t_pDst = pDesBuf->m_dwBuffer;
	{
		const int dy = box.top - nOffsetTop - pDesBuf->m_nOriginY;
		const int dx = box.left - nOffsetLeft - pDesBuf->m_nOriginX;
		t_pDst += (dy*pDesBuf->m_nPitch / sizeof(DWORD) + dx);
	}
	return t_pDst;
}

//------------------------------------------------------------------------
//计算是否是不放大并且足够小的buffer，如果是则可以不使用双线性插值并且不渲染边界
bool JudgeSimpleTrans(STU_Rect &box, LPCommonINNERVERTEX &t_pvfXYZW, CommonBufferDef32* pSrcBuf)
{
	bool bSimpleTrans = false;
	if (box.width < 55 && box.height < 55)
	{
		float y_len = t_pvfXYZW[1].y - t_pvfXYZW[0].y;
		float x_len = t_pvfXYZW[1].x - t_pvfXYZW[0].x;
		if (sqrt(x_len*x_len + y_len*y_len) <= pSrcBuf->m_nWidth)
		{
			y_len = t_pvfXYZW[3].y - t_pvfXYZW[0].y;
			x_len = t_pvfXYZW[3].x - t_pvfXYZW[0].x;
			if (sqrt(x_len*x_len + y_len*y_len) <= pSrcBuf->m_nHeight)
				bSimpleTrans = true;
		}
	}
	return bSimpleTrans;
}

BASEGRAPH_COMMON_API BOOL Common_BufferTransformLinear(IN OUT CommonBufferDef32 * pDesBuf, IN int nOffsetLeft, IN int nOffsetTop, IN CommonBufferDef32 * pSrcBuf, IN int nSrcWidth, IN int nSrcHeight, IN vector4f * pvfXYZW, IN CommonBufferDef32 * pMaskBuf)
{
	if (CommonBufferDef32_AreaW(pDesBuf) <= 0 || CommonBufferDef32_AreaH(pDesBuf) <= 0
		|| CommonBufferDef32_AreaW(pSrcBuf) <= 0 || CommonBufferDef32_AreaH(pSrcBuf) <= 0)
	{
		//assert(0 && "输入buffer或者输出buffer的裁剪后区域高或者宽不大于0");
		return FALSE;
	}

	//Test_FillDstBound(pDesBuf); //测试：填充目标区域的边界方便观察

	LPCommonINNERVERTEX t_pvfXYZW = (LPCommonINNERVERTEX)pvfXYZW;

	// 计算需要渲染的最小包围盒
	STU_Rect box = { 0,0,0,0 };
	CalcMinBoundingBox(t_pvfXYZW, box, pSrcBuf, pDesBuf, nOffsetLeft, nOffsetTop);

	//计算是否是不放大并且足够小的buffer，如果是则可以不使用双线性插值并且不渲染边界
	bool bSimpleTrans = FALSE;//JudgeSimpleTrans(box, t_pvfXYZW, pSrcBuf);
							  //计算源区域和目标区域buffer的起始点指针
	DWORD* t_pSrc = CalcSrcHead(pSrcBuf);
	DWORD* t_pDst = CalcDstHead(pDesBuf, box, nOffsetTop, nOffsetLeft);

	//测试：把源图像直接填充到目标区域看看
	//if(pSrcBuf->m_nWidth <= box.width -2 && pSrcBuf->m_nHeight <= box.height - 2) return Test_FillDstWithSrc(t_pSrc, t_pDst, pSrcBuf, pDesBuf);

	//计算最小包围矩形box的左上角点的U,V和全局的dU,dV（因为是线性变换，所以全局dUdV是相同的）
	STU_deltaUV duv = { 0,0,0,0 };
	CalcDeltaUV(pSrcBuf, t_pvfXYZW, box, duv);

	//先计算边界，并填充边缘像素，之后就可以只做完整的定长循环像素渲染
	//完全点：指其UV映射的坐标在源buffer中都是有效的坐标，不会超出边界
	Edge_info* edge_info = new Edge_info[box.height];
	CalcEdge(edge_info, box, pSrcBuf, t_pvfXYZW, duv);

	//遍历边界并渲染边缘
	if (!bSimpleTrans)
		RenderEdge(pSrcBuf, pDesBuf, edge_info, t_pDst, box, t_pSrc, duv);

	//循环处理box每一行,填充内部完全点
	const int src_buff_width = pSrcBuf->m_nPitch >> 2; //源buffer的原始区域宽度
	const int dst_buff_width = pDesBuf->m_nPitch >> 2; //目标buffer的原始区域宽度
	if (bSimpleTrans)
		Fill_inner_pixel_simle(t_pDst, t_pSrc, edge_info,
			src_buff_width, dst_buff_width,
			box.height,
			duv.dxu, duv.dxv);
	else
		Fill_inner_pixel(t_pDst, t_pSrc, edge_info,
			src_buff_width, dst_buff_width,
			box.height,
			duv.dxu, duv.dxv);
	delete[] edge_info;
	edge_info = NULL;
	return TRUE;
}

// 使用某个值填充内存中的某块区域，可指定裁剪区域。
BASEGRAPH_COMMON_API void BGRA32_FillRect(IN OUT DWORD* pdwBuf,	// 操作缓存
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
	if (!BaseGraphFill_ClipRange(nX, nY, nWidth, nHeight, nBufWidth, nBufHeight, prcClip))
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
BASEGRAPH_COMMON_API void BGRA32_ReplaceRGB_MulA(IN OUT DWORD* pdwBuf,	// 操作缓存
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
	if (!BaseGraphFill_ClipRange(nX, nY, nWidth, nHeight, nBufWidth, nBufHeight, prcClip))
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


BASEGRAPH_COMMON_API void BGRA32_ReplaceRGB_MulA(IN OUT DWORD* pdwBuf,	// 操作缓存
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
	if (!BaseGraphFix_CutRange(nX, nY, nWidth, nHeight, nBufWidth, nBufHeight,
		nSrcX, nSrcY, nSrcWidth, nSrcHeight, nSrcBufWidth, nSrcBufHeight, prcClip))
			return;

	//// 有效区域裁剪
	//if (!BaseGraphFill_ClipRange(nX, nY, nWidth, nHeight, nBufWidth, nBufHeight, prcClip))
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

BASEGRAPH_COMMON_API void BGRA32_ReplaceA_MulA(IN OUT DWORD* pdwBuf,	// 操作缓存
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
	if (!BaseGraphFix_CutRange(nX, nY, nWidth, nHeight, nBufWidth, nBufHeight,
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

BASEGRAPH_COMMON_API void BGRA32_ReplaceRGB_MulA(IN OUT DWORD* pdwBuf,	// 操作缓存
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
	if (!BaseGraphFill_ClipRange(nX, nY, nWidth, nHeight, nBufWidth, nBufHeight, prcClip))
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

BASEGRAPH_COMMON_API void BGRA32_ReplaceRGB_MulA_Width_HArray(IN OUT DWORD* pdwBuf,	// 操作缓存
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
	if (!BaseGraphFill_ClipRange(nX, nY, nWidth, nHeight, nBufWidth, nBufHeight, prcClip))
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

BASEGRAPH_COMMON_API void BGRA32_ReplaceRGB(IN OUT DWORD* pdwBuf,	// 操作缓存
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
	if (!BaseGraphFill_ClipRange(nX, nY, nWidth, nHeight, nBufWidth, nBufHeight, prcClip))
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
BASEGRAPH_COMMON_API void BGRA32_FillAlpha(IN OUT DWORD* pdwBuf,		// 目标
											IN int nX,					// 填充区域
											IN int nY,
											IN int nWidth,
											IN int nHeight,
											IN int nBufWidth,			// 缓存大小
											IN int nBufHeight,
											IN RECT* prcClip,			// 裁剪区域（可选项）
											IN char cAlpha,				// Alpha值
											IN Common_FILL_MODE eMode,		// 填充模式
											IN DWORD pdwMaskCr)			// 掩码颜色值（依赖模式的可选项）
{
	if (pdwBuf == NULL)
		return;

	// 有效区域裁剪
	if (!BaseGraphFill_ClipRange(nX, nY, nWidth, nHeight, nBufWidth, nBufHeight, prcClip))
		return;

	// 计算必要参数
	DWORD* pdwLineStart = pdwBuf + nY * nBufWidth + nX;
	int na16 = (nWidth >> 2), nm16 = ((nWidth & 0x03) << 2);

	switch (eMode)
	{
	case Common_FILL_ALL:		// 全部填充
		for (int i=0; i<nHeight; i++)
		{
			memset_alpha_line_all(pdwLineStart, cAlpha, na16, nm16);
			pdwLineStart += nBufWidth;
		}
		break;
	case Common_FILL_MASK:		// 只填充颜色为 pdwMaskCr 的像素
		for (int i=0; i<nHeight; i++)
		{
			memset_alpha_line_mask(pdwLineStart, cAlpha, pdwMaskCr, na16, nm16);
			pdwLineStart += nBufWidth;
		}
		break;
	case Common_FILL_NOT_MASK:	// 不填充颜色为 pdwMaskCr 的像素
		for (int i=0; i<nHeight; i++)
		{
			memset_alpha_line_notmask(pdwLineStart, cAlpha, pdwMaskCr, na16, nm16);
			pdwLineStart += nBufWidth;
		}
		break;
	case Common_FILL_SCALE_ALPHA_ALL:
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


BASEGRAPH_COMMON_API void BGRA32_FillAlpha_With_H(IN OUT DWORD* pdwBuf,		// 目标
											IN int nX,					// 填充区域
											IN int nY,
											IN int nWidth,
											IN int nHeight,
											IN int nBufWidth,			// 缓存大小
											IN int nBufHeight,
											IN RECT* prcClip,			// 裁剪区域（可选项）
											IN DWORD* cAlpha,				// Alpha值
											IN Common_FILL_MODE eMode,		// 填充模式
											IN DWORD pdwMaskCr)			// 掩码颜色值（依赖模式的可选项）
{
	if (pdwBuf == NULL)
		return;

	// 有效区域裁剪
	if (!BaseGraphFill_ClipRange(nX, nY, nWidth, nHeight, nBufWidth, nBufHeight, prcClip))
		return;

	// 计算必要参数
	DWORD* pdwLineStart = pdwBuf + nY * nBufWidth + nX;
	int na16 = (nWidth >> 2), nm16 = ((nWidth & 0x03) << 2);

	switch (eMode)
	{
	case Common_FILL_ALL:		// 全部填充
		for (int i=0; i<nHeight; i++)
		{
			memset_alpha_line_all(pdwLineStart, (char)cAlpha[i], na16, nm16);
			pdwLineStart += nBufWidth;
		}
		break;
	case Common_FILL_MASK:		// 只填充颜色为 pdwMaskCr 的像素
		for (int i=0; i<nHeight; i++)
		{
			memset_alpha_line_mask(pdwLineStart, (char)cAlpha[i], pdwMaskCr, na16, nm16);
			pdwLineStart += nBufWidth;
		}
		break;
	case Common_FILL_NOT_MASK:	// 不填充颜色为 pdwMaskCr 的像素
		for (int i=0; i<nHeight; i++)
		{
			memset_alpha_line_notmask(pdwLineStart, (char)cAlpha[i], pdwMaskCr, na16, nm16);
			pdwLineStart += nBufWidth;
		}
		break;
	case Common_FILL_SCALE_ALPHA_ALL:
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

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//		（3）对大片内存进行带掩码的操作
//////////////////////////////////////////////////////////////////////////

// 对 32 位内存进行键值操作
BASEGRAPH_COMMON_API BOOL BGRA32_ProcKey(IN OUT DWORD* pdwBuf,				// 目标
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
										  IN RECT* prcDesClip/* = NULL */)	// 裁剪区域（可选项）
{
	//assert(nKeyPitch >= nKeyBufWidth);

	if (nKeyWidth == nDesWidth && nKeyHeight == nDesHeight)		// 固定大小
	{
		return BufferMemSSE::Buffer_BGRA_Key_Fix_SSE(pdwBuf,
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
		return BufferMemSSE::Buffer_BGRA_Key_BLStretch_SSE(pdwBuf,
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
BASEGRAPH_COMMON_API BOOL BaseGraphKey_ProcScalar(IN OUT LPBYTE pbyKey,		// 键目标
										  IN int nX,				// 填充区域
										  IN int nY,
										  IN int nWidth,
										  IN int nHeight,
										  IN int nKeyBufWidth,		// 键幅面大小
										  IN int nKeyBufHeight,
										  IN int nKeyPitch,			// 键缓存一行的字节宽度
										  IN RECT* prcClip,			// 裁剪区域（可选项）
										  IN char cAlpha,			// Alpha值
										  IN Common_KEY_MODE eMode)		// 键操作模式
{
	//assert(nKeyPitch >= nKeyBufWidth);

	if (pbyKey == NULL)
		return FALSE;

	// 有效区域裁剪
	if (!BaseGraphFill_ClipRange(nX, nY, nWidth, nHeight, nKeyBufWidth, nKeyBufHeight, prcClip))
		return FALSE;

	// 计算必要参数
	BYTE* pbyKeyLine = pbyKey + nY * nKeyPitch + nX;			// 键起始地址
	int na16 = (nWidth >> 2), nm16 = ((nWidth & 0x03) << 2);

	// 逐行处理
	switch (eMode)
	{
	case Common_KEY_MULTIPLY:			// 相乘
		for (int i=0; i<nHeight; i++)
		{
			keybuf_mul_char_line(pbyKeyLine, cAlpha, na16, nm16);
			pbyKeyLine += nKeyPitch;
		}
		break;
	case Common_KEY_MULTIPLY_NOTSRC:	// 反向源键相乘
		cAlpha = 0xff - cAlpha;
		for (int i=0; i<nHeight; i++)
		{
			keybuf_mul_char_line(pbyKeyLine, cAlpha, na16, nm16);
			pbyKeyLine += nKeyPitch;
		}
		break;
	case Common_KEY_MULTIPLY_NOTDES:	// 反向目标键相乘
		for (int i=0; i<nHeight; i++)
		{
			keybuf_mul_char_line_notdes(pbyKeyLine, cAlpha, na16, nm16);
			pbyKeyLine += nKeyPitch;
		}
		break;
	case Common_KEY_REPLACE:			// 替换
		for (int i=0; i<nHeight; i++)
		{
			memset_char_line(pbyKeyLine, cAlpha, na16, nm16);
			pbyKeyLine += nKeyPitch;
		}
		break;
	case Common_KEY_MIX:				// 混叠
		for (int i=0; i<nHeight; i++)
		{
			keybuf_mix_char_line(pbyKeyLine, cAlpha, na16, nm16);
			pbyKeyLine += nKeyPitch;
		}
		break;
	case Common_KEY_MIX_NOTSRC:			// 反向源键混叠
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
BASEGRAPH_COMMON_API BOOL BaseGraphKey_ProcBuffer(IN OUT LPBYTE pbyDes,				// 目标键
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
										  IN Common_KEY_MODE eMode,				// 键操作模式
										  IN BUFFER_PARITY_STYLE eParity,		// 奇偶场类型
										  IN RECT* prcDesClip/* = NULL*/)	// 裁剪区域（可选项）
{
	//assert(nDesKeyPitch >= nDesBufWidth && nSrcKeyPitch >= nSrcBufWidth);

	if (nSrcWidth == nDesWidth && nSrcHeight == nDesHeight)		// 固定大小
	{
		return BufferMemSSE::Buffer_Key_Fix_SSE(pbyDes,
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
		return BufferMemSSE::Buffer_Key_BLStretch_SSE(pbyDes,
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
BASEGRAPH_COMMON_API void Common_MakeTranslucentBack(OUT DWORD* pBuf,			// 背景 Buffer
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
	DWORD* pBlockLine = BaseGraphSSE_NEW(nLineSize, DWORD);
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
	BaseGraphSSE_FREE(pBlockLine);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//		（5）用于支持算法优化的一些系统操作接口
//////////////////////////////////////////////////////////////////////////

// 获取计数器值（高精度）：时钟周期——（纳秒级）/主频HZ数
BASEGRAPH_COMMON_API unsigned __int64 Common_GetCycleCount()
{
	assert(0);
	return 0;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//		（6）特殊的运算接口
//////////////////////////////////////////////////////////////////////////

// 计算一等差序列数与某一特定数的完全平方和（特定数为负则计算完全平方差）
BASEGRAPH_COMMON_API void Common_SerialPerfectSquare(OUT float* pResult,		// 计算结果
												IN int nSerialCount,	// 序列长度
												IN float fStart,		// 序列起始数
												IN float fStep,			// 等差
												IN float fGiven)		// 特定的数
{
	math_serial_perfect_square(pResult, fStart, fStep, fGiven, (nSerialCount >> 2), ((nSerialCount & 0x03) << 2));
}

// 计算一等差序列数与某一特定数的完全平方和（特定数为负则计算完全平方差）
// 增加额外功能，对和进行线性缩放，并计算四次方结果
BASEGRAPH_COMMON_API void Common_SerialPerfectSquareEx(OUT float* pResSqr,		// 平方和结果
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
BASEGRAPH_COMMON_API void Common_OffsetSqrtIndexLookup(OUT void* pBuf,		// 查表结果（32 位表，要求与表的类型相同）
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
BASEGRAPH_COMMON_API void Common_TylerArcIndexLookupX(OUT void* pBuf,			// 查表结果（32 位表，要求与表的类型相同）
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
BASEGRAPH_COMMON_API void Common_TylerArcIndexLookupY(OUT void* pBuf,			// 查表结果（32 位表，要求与表的类型相同）
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
BASEGRAPH_COMMON_API void Common_NegtiveFloat(OUT float* pDes,		// 目标
										 IN float* pSrc,		// 源
										 IN int nlen)			// 数组长度
{
	math_negtive_float(pDes, pSrc, (nlen >> 2), ((nlen & 0x03) << 2));
}

// 求解一整型等差序列经过两种偏移后的新序列 offset_1 + a 与 offset_2 - a
// 特殊情况：通过指定圆心的直线 y = x 与 y = -x 在指定大小区域内的点的 X 序列值，序列以 y 为索引
BASEGRAPH_COMMON_API void Common_SerialOffset2(OUT int* pDes1,		// offset_1 + a
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
void CALLBACK Common_CalMask(IN OUT BYTE *pbyMask,
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

	// 添加了对Mask边缘的计算
	// 其汇编实现
	int nCurHeight = 0;					// 当前坐标点
	int nCurWidth = 0;
	int nCurX = 0;
	int nCurY = 0;
	int nCurOperateHeight = 0;
	int nCurOperateWidth = 0;
	// 一次算Mask四个点
	delete[] pTempBuf;
}

// 源像素索引表
_declspec(align(16)) DWORD *g_pnSrcIndexTable = NULL;
// 权重表
_declspec(align(8)) BYTE *g_pnSrcWeightTable = NULL;

void CALLBACK Common_CreateTableFor_H1920To1440()
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

void CALLBACK Common_DeleteTableFor_H1920To1440()
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

void CALLBACK Common_LinearScale_H1920To1440(OUT BYTE* pDes,
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
		pSrcTemp += nSrcPitch;
		pDesTemp += nDesPitch;
	}
}

// YUYV拉伸拷贝(对于多余的YUV，则变成YU0V)
LRESULT CALLBACK Common_StretchBlt_YUYV(
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
		}
		if(nY+1 >= nSrcBufferHeight)
			memset(pbyTempSrc2, 0, nSrcBytes);
		else
		{
			pTemp += nSrcPitch;
		}
		//fX = 0.0f;
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
	return 1;
}
_declspec(align(16)) DWORD ymask[] = {0x00ff00ff,0x00ff00ff,0x00ff00ff,0x00ff00ff,};
BOOL yuv_yyuuvv2yuv(BYTE* pDesBuffer,BYTE* pSrcField,int nA16,int nMod16)
{
	_yuv_yyuuvv2yuv_x64(pDesBuffer,pSrcField,nA16,nMod16,ymask);
	return TRUE;
}

BOOL yuv_yuv2yyuuvv(BYTE* pDesBuffer,BYTE* pSrcField,int nA8,int nMod8)
{
	_yuv_yuv2yyuuvv_x64(pDesBuffer,pSrcField,nA8,nMod8);
	return 1;
}

void yuv_uyvy2yuv8(BYTE* puyvy,BYTE* py,BYTE* pu,BYTE* pv,int nA16,int nM16)
{
	_yuv_uyvy2yuv8_x64(puyvy,py,pu,pv,nA16,nM16);
}

void yuv_yuyv2yuv8(BYTE* puyvy,BYTE* py,BYTE* pu,BYTE* pv,int nA16,int nM16)
{
	_yuv_yuyv2yuv8_x64(puyvy,py,pu,pv,nA16,nM16);
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
	_bgra32_mean_copy_add_y_quick_field_x64(des,src,scw,src_pitch,pPowerY,pCoefH,nMapY,nFirst);
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
	BaseGraphSSE_DECLARE(int*) pMapX = BaseGraphSSE_NEW(nDestWidth, int);		// X 方向上的权重数量表
	BaseGraphSSE_DECLARE(int*) pMapY = BaseGraphSSE_NEW(nDestHeight, int);	// Y 方向上的权重数量表
	BaseGraphSSE_DECLARE(float*) pPowerX = BaseGraphSSE_NEW((nDestWidth << 1), float);	// X 方向上的权重表
	BaseGraphSSE_DECLARE(float*) pPowerY = BaseGraphSSE_NEW((nDestHeight << 1), float);	// Y 方向上的权重表
	DWORD *pDesLine = pDest + nDestX + nDestY * nDestBufferWidth;		// Buffer 起始地址
	float *pTmpPowerY = pPowerY;		// Y 方向权重表地址
	int nSrcPitch = (nSrcBufferWidth << 2);
	BaseGraphSSE_DECLARE(float*) pTmpLine = BaseGraphSSE_NEW((nSrcWidth << 2), float);	// 源行的临时缓存
	BaseGraphSSE_DECLARE(float) fCoefW[4] = { fScaleW_I, fScaleW_I, fScaleW_I, fScaleW_I };
	BaseGraphSSE_DECLARE(float) fCoefH[4] = { fScaleH_I, fScaleH_I, fScaleH_I, fScaleH_I };

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
	BaseGraphSSE_FREE(pMapX);
	BaseGraphSSE_FREE(pMapY);
	BaseGraphSSE_FREE(pPowerX);
	BaseGraphSSE_FREE(pPowerY);
	BaseGraphSSE_FREE(pTmpLine);

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
BASEGRAPH_COMMON_API void Common_GuassianBlur(LPBYTE lpDes,LPSIZE pSize,int degrees )
{
	int nSide = degrees*2+1;
	int nGuass[33];

	int  nSign = CreateGaussMap( nGuass,degrees );
	GauseFilter( lpDes,pSize,nGuass,nSign );
}

