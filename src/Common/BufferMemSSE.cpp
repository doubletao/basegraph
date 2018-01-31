#include "stdafx.h"
#include "BufferMemSSE.h"
#include "Common\BufferFunction.h"
#include "BufferSSE2.h"

__declspec(align(16)) DWORD*	BufferMemSSE::g_pdwMixGainTable = nullptr;
static const double MIX_TABLE_SCALE_COEF = ((1 << 15) - 1) * 255.0;

BufferMemSSE::BufferMemSSE()
{
}


BufferMemSSE::~BufferMemSSE()
{
}

BOOL BufferMemSSE::Buffer_CreateGainTable()
{
	int nIndex(0), nTmpSrc(0), nTmpDes(0), nTmpDelta(0);
	double dTmpSrc(0.0), dTmpScale(0.0);

	g_pdwMixGainTable = (DWORD*)_mm_malloc(sizeof(DWORD) * (65536), 16);
	for (int i = 0; i<256; i++)
	{
		nTmpDes = nTmpSrc;
		nTmpDelta = 255 - i;
		for (int j = 0; j<256; j++)
		{
			if (nTmpDes == 0)
			{
				g_pdwMixGainTable[nIndex] = 0;
			}
			else
			{
				dTmpScale = 1.0 / (double)nTmpDes;
				g_pdwMixGainTable[nIndex] = (DWORD)(dTmpSrc * dTmpScale);
			}
			nIndex++;
			nTmpDes += nTmpDelta;
		}
		nTmpSrc += 255;
		dTmpSrc += MIX_TABLE_SCALE_COEF;
	}
	return TRUE;
}

BOOL BufferMemSSE::Buffer_ReleaseGainTable()
{
	if ((g_pdwMixGainTable) != NULL)
	{
		_mm_free(g_pdwMixGainTable);
	}
	return TRUE;
}

BOOL BufferMemSSE::Buffer_BGRA_Fix_SSE(OUT DWORD * pDes, IN int nDesX, IN int nDesY, IN int nDesWidth, IN int nDesHeight, IN int nDesBufferWidth, IN int nDesBufferHeight, IN DWORD * pSrc, IN int nSrcX, IN int nSrcY, IN int nSrcWidth, IN int nSrcHeight, IN int nSrcBufferWidth, IN int nSrcBufferHeight, IN BUFFER_MODE eMode, IN BUFFER_PARITY_STYLE eParity, IN RECT * prcDesClip, IN int n, IN int k)
{
	if (pSrc == NULL || pDes == NULL)
		return FALSE;

	// 获得有效的操作范围
	if (!BufferFunction::Buffer_Fix_CutRange(nDesX, nDesY, nDesWidth, nDesHeight, nDesBufferWidth, nDesBufferHeight, \
		nSrcX, nSrcY, nSrcWidth, nSrcHeight, nSrcBufferWidth, nSrcBufferHeight, prcDesClip))
		return FALSE;

	int nDesAddWidth(0), nSrcAddWidth(0), nLineInc(0);
	int nDesClipHeight = (prcDesClip == NULL) ? nDesBufferHeight	\
		: ((nDesBufferHeight > prcDesClip->bottom) ? prcDesClip->bottom : nDesBufferHeight);

	int srcDesY = nDesY;
	// 获得有效的奇偶场操作范围
	if (!Buffer_Parity_Range(nDesY, nDesAddWidth, nDesBufferWidth, nDesClipHeight, \
		nSrcY, nSrcAddWidth, nSrcBufferWidth, nSrcBufferHeight, nLineInc, eParity, n, k))
		return FALSE;


	DWORD* pDesLine = pDes + nDesY * nDesBufferWidth + nDesX;		// 目标 Buffer 起始地址
	DWORD* pSrcLine = pSrc + nSrcY * nSrcBufferWidth + nSrcX;		// 源 Buffer 起始地址
	int na16 = (nDesWidth >> 2), nm16 = ((nDesWidth & 0x03) << 2);	// 一行的对齐参数

																	// 逐行处理
	switch (eMode)
	{
	case BUFFER_COPY:		// 拷贝 
		for (int i = nDesY - srcDesY; i<nDesHeight; i += nLineInc)
		{
			memcpy_line(pDesLine, pSrcLine, na16, nm16);
			pDesLine += nDesAddWidth;
			pSrcLine += nSrcAddWidth;
		}
		break;
	case BUFFER_MIX:			// Alpha 混叠
		for (int i = nDesY - srcDesY; i<nDesHeight; i += nLineInc)
		{
			bgra32_line_mix(pDesLine, pSrcLine, na16, nm16, g_pdwMixGainTable);
			pDesLine += nDesAddWidth;
			pSrcLine += nSrcAddWidth;
		}
		break;
	case BUFFER_MIX_NOTSRC:	// Alpha 反向源混叠
		for (int i = nDesY - srcDesY; i<nDesHeight; i += nLineInc)
		{
			bgra32_line_mix_notsrc(pDesLine, pSrcLine, na16, nm16, g_pdwMixGainTable);
			pDesLine += nDesAddWidth;
			pSrcLine += nSrcAddWidth;
		}
		break;
	case BUFFER_ALPHA_BLEND:		// 快速 Alpha 混合
		for (int i = nDesY; i<nDesHeight; i += nLineInc)
		{
			bgra32_line_alpha_blend(pDesLine, pSrcLine, na16, nm16);
			pDesLine += nDesAddWidth;
			pSrcLine += nSrcAddWidth;
		}
		break;
	case BUFFER_ALPHA_BLEND_NOT:	// 快速 Alpha 反向源混合
		for (int i = nDesY - srcDesY; i<nDesHeight; i += nLineInc)
		{
			bgra32_line_alpha_blend_not(pDesLine, pSrcLine, na16, nm16);
			pDesLine += nDesAddWidth;
			pSrcLine += nSrcAddWidth;
		}
		break;
	case BUFFER_LINEAR_DODGE:	// 线性减淡
		for (int i = nDesY - srcDesY; i<nDesHeight; i += nLineInc)
		{
			bgra32_line_linear_dodge(pDesLine, pSrcLine, na16, nm16, g_pdwMixGainTable);
			pDesLine += nDesAddWidth;
			pSrcLine += nSrcAddWidth;
		}
		break;
	case BUFFER_LINEAR_BURN:		// 线性加深
		for (int i = nDesY - srcDesY; i<nDesHeight; i += nLineInc)
		{
			bgra32_line_linear_burn(pDesLine, pSrcLine, na16, nm16, g_pdwMixGainTable);
			pDesLine += nDesAddWidth;
			pSrcLine += nSrcAddWidth;
		}
		break;
	case BUFFER_COLOR_DODGE:		// 颜色减淡
		for (int i = nDesY - srcDesY; i<nDesHeight; i += nLineInc)
		{
			DWORD *pTmpDes = pDesLine, *pTmpSrc = pSrcLine;
			for (int j = 0; j<nDesWidth; j++, pTmpDes++, pTmpSrc++)
			{
				BYTE *pbyDes = (BYTE*)pTmpDes, *pbySrc = (BYTE*)pTmpSrc;
				int nTmpA = ((int)pbyDes[3] + (int)pbySrc[3]) * 255 - (int)pbyDes[3] * (int)pbySrc[3];
				float fTmpScale = 16581375.f / ((float)nTmpA + 0.000001f);
				for (int k = 0; k<3; k++)
				{
					int nTmpDes = (int)pbyDes[k] * (int)pbyDes[3];
					int nTmp = (int)((float)nTmpDes * fTmpScale / (65025.000001f - (float)((int)pbySrc[k] * (int)pbySrc[3])) + 0.5f);
					pbyDes[k] = (BYTE)((nTmp > 255) ? 255 : nTmp);
				}
				pbyDes[3] = (BYTE)((nTmpA + 255) >> 8);
			}
			pDesLine += nDesAddWidth;
			pSrcLine += nSrcAddWidth;
		}
		break;
	case BUFFER_COLOR_BURN:		// 颜色加深
		for (int i = nDesY - srcDesY; i<nDesHeight; i += nLineInc)
		{
			DWORD *pTmpDes = pDesLine, *pTmpSrc = pSrcLine;
			for (int j = 0; j<nDesWidth; j++, pTmpDes++, pTmpSrc++)
			{
				BYTE *pbyDes = (BYTE*)pTmpDes, *pbySrc = (BYTE*)pTmpSrc;
				int nTmpA = ((int)pbyDes[3] + (int)pbySrc[3]) * 255 - (int)pbyDes[3] * (int)pbySrc[3];
				float fTmpScale = 65025.f / ((float)nTmpA + 0.000001f);
				for (int k = 0; k<3; k++)
				{
					int nTmpSrc = (int)pbySrc[3] * (int)pbySrc[k];
					int nTmp = nTmpSrc + (int)pbyDes[3] * (int)pbyDes[k] - 65025;
					nTmp = (nTmp <= 0) ? 0 : (int)((float)nTmp * fTmpScale / ((float)nTmpSrc + 0.000001f));
					pbyDes[k] = (BYTE)((nTmp > 255) ? 255 : nTmp);
				}
				pbyDes[3] = (BYTE)((nTmpA + 255) >> 8);
			}
			pDesLine += nDesAddWidth;
			pSrcLine += nSrcAddWidth;
		}
		break;
	case BUFFER_MULTIPLY:		// 正片叠底
		for (int i = nDesY - srcDesY; i<nDesHeight; i += nLineInc)
		{
			DWORD *pTmpDes = pDesLine, *pTmpSrc = pSrcLine;
			for (int j = 0; j<nDesWidth; j++, pTmpDes++, pTmpSrc++)
			{
				BYTE *pbyDes = (BYTE*)pTmpDes, *pbySrc = (BYTE*)pTmpSrc;
				int nTmpA = ((int)pbyDes[3] + (int)pbySrc[3]) * 255 - (int)pbyDes[3] * (int)pbySrc[3];
				double dTmpScale = 1.0 / ((double)(nTmpA * 255) + 0.000000000001);
				for (int k = 0; k<3; k++)
				{
					int nTmp = (int)((double)((int)pbyDes[k] * (int)pbyDes[3] * (int)pbySrc[k] * (int)pbySrc[3]) * dTmpScale + 0.5);
					pbyDes[k] = (BYTE)((nTmp > 255) ? 255 : nTmp);
				}
				pbyDes[3] = (BYTE)((nTmpA + 255) >> 8);
			}
			pDesLine += nDesAddWidth;
			pSrcLine += nSrcAddWidth;
		}
		break;
	case BUFFER_SCREEN:			// 屏幕滤色
		for (int i = nDesY - srcDesY; i<nDesHeight; i += nLineInc)
		{
			DWORD *pTmpDes = pDesLine, *pTmpSrc = pSrcLine;
			for (int j = 0; j<nDesWidth; j++, pTmpDes++, pTmpSrc++)
			{
				BYTE *pbyDes = (BYTE*)pTmpDes, *pbySrc = (BYTE*)pTmpSrc;
				int nTmpA = ((int)pbyDes[3] + (int)pbySrc[3]) * 255 - (int)pbyDes[3] * (int)pbySrc[3];
				float fTmpScale = 255.f / ((float)nTmpA + 0.000001f);
				for (int k = 0; k<3; k++)
				{
					int nTmpDes = (int)pbyDes[k] * (int)pbyDes[3];
					int nTmpSrc = (int)pbySrc[k] * (int)pbySrc[3];
					int nTmp = (int)((float)(nTmpDes + nTmpSrc - (((nTmpDes + 255) * (nTmpSrc + 255)) >> 16)) * fTmpScale + 0.5f);
					pbyDes[k] = (BYTE)((nTmp > 255) ? 255 : nTmp);
				}
				pbyDes[3] = (BYTE)((nTmpA + 255) >> 8);
			}
			pDesLine += nDesAddWidth;
			pSrcLine += nSrcAddWidth;
		}
		break;
	case BUFFER_LIGHTEN:			// 变亮
		for (int i = nDesY - srcDesY; i<nDesHeight; i += nLineInc)
		{
			DWORD *pTmpDes = pDesLine, *pTmpSrc = pSrcLine;
			for (int j = 0; j<nDesWidth; j++, pTmpDes++, pTmpSrc++)
			{
				BYTE *pbyDes = (BYTE*)pTmpDes, *pbySrc = (BYTE*)pTmpSrc;
				int nTmpA = (pbyDes[3] * pbySrc[3] + 0xff) >> 8;
				//int nTmpA = ((int)pbyDes[3] + (int)pbySrc[3]) * 255 - (int)pbyDes[3] * (int)pbySrc[3];
				float fTmpScale = 255.f / ((float)nTmpA + 0.000001f);
				for (int k = 0; k<3; k++)
				{
					int nTmpDes = (int)pbyDes[k] * (int)pbyDes[3];
					int nTmpSrc = (int)pbySrc[k] * (int)pbySrc[3];
					int nTmp = (int)((float)((nTmpDes > nTmpSrc) ? nTmpDes : nTmpSrc) * fTmpScale + 0.5f);
					pbyDes[k] = (BYTE)((nTmp > 255) ? 255 : nTmp);
				}
				pbyDes[3] = (BYTE)((nTmpA + 255) >> 8);
			}
			pDesLine += nDesAddWidth;
			pSrcLine += nSrcAddWidth;
		}
		break;
	case BUFFER_DARKEN:			// 变暗
		for (int i = nDesY - srcDesY; i<nDesHeight; i += nLineInc)
		{
			DWORD *pTmpDes = pDesLine, *pTmpSrc = pSrcLine;
			for (int j = 0; j<nDesWidth; j++, pTmpDes++, pTmpSrc++)
			{
				BYTE *pbyDes = (BYTE*)pTmpDes, *pbySrc = (BYTE*)pTmpSrc;
				int nTmpA = ((int)pbyDes[3] + (int)pbySrc[3]) * 255 - (int)pbyDes[3] * (int)pbySrc[3];
				float fTmpScale = 255.f / ((float)nTmpA + 0.000001f);
				for (int k = 0; k<3; k++)
				{
					int nTmpDes = (int)pbyDes[k] * (int)pbyDes[3];
					int nTmpSrc = (int)pbySrc[k] * (int)pbySrc[3];
					int nTmp = (int)((float)((nTmpDes < nTmpSrc) ? nTmpDes : nTmpSrc) * fTmpScale + 0.5f);
					pbyDes[k] = (BYTE)((nTmp > 255) ? 255 : nTmp);
				}
				pbyDes[3] = (BYTE)((nTmpA + 255) >> 8);
			}
			pDesLine += nDesAddWidth;
			pSrcLine += nSrcAddWidth;
		}
		break;
	case BUFFER_SUB:		// 差值
		for (int i = nDesY - srcDesY; i<nDesHeight; i += nLineInc)
		{
			DWORD *pTmpDes = pDesLine, *pTmpSrc = pSrcLine;
			for (int j = 0; j<nDesWidth; j++, pTmpDes++, pTmpSrc++)
			{
				BYTE *pbyDes = (BYTE*)pTmpDes, *pbySrc = (BYTE*)pTmpSrc;
				int nTempA = (int)(pbyDes[3] - pbySrc[3]);
				nTempA = nTempA < 0 ? 0 : nTempA;
				pbyDes[3] = nTempA;
			}
			pDesLine += nDesAddWidth;
			pSrcLine += nSrcAddWidth;
		}
		break;
	case BUFFER_DIFFERENCE:		// 差值
		for (int i = nDesY - srcDesY; i<nDesHeight; i += nLineInc)
		{
			DWORD *pTmpDes = pDesLine, *pTmpSrc = pSrcLine;
			for (int j = 0; j<nDesWidth; j++, pTmpDes++, pTmpSrc++)
			{
				BYTE *pbyDes = (BYTE*)pTmpDes, *pbySrc = (BYTE*)pTmpSrc;
				int nTmpA = ((int)pbyDes[3] + (int)pbySrc[3]) * 255 - (int)pbyDes[3] * (int)pbySrc[3];
				float fTmpScale = 255.f / ((float)nTmpA + 0.000001f);
				for (int k = 0; k<3; k++)
				{
					int nTmp = (int)pbyDes[k] * (int)pbyDes[3] - (int)pbySrc[k] * (int)pbySrc[3];
					nTmp = (int)((float)((nTmp < 0) ? -nTmp : nTmp) * fTmpScale + 0.5f);
					pbyDes[k] = (BYTE)((nTmp > 255) ? 255 : nTmp);
				}
				pbyDes[3] = (BYTE)((nTmpA + 255) >> 8);
			}
			pDesLine += nDesAddWidth;
			pSrcLine += nSrcAddWidth;
		}
		break;
	case BUFFER_EXCLUSION:		// 排除
		for (int i = nDesY - srcDesY; i<nDesHeight; i += nLineInc)
		{
			DWORD *pTmpDes = pDesLine, *pTmpSrc = pSrcLine;
			for (int j = 0; j<nDesWidth; j++, pTmpDes++, pTmpSrc++)
			{
				BYTE *pbyDes = (BYTE*)pTmpDes, *pbySrc = (BYTE*)pTmpSrc;
				int nTmpA = ((int)pbyDes[3] + (int)pbySrc[3]) * 255 - (int)pbyDes[3] * (int)pbySrc[3];
				float fTmpScale = 255.f / ((float)nTmpA + 0.000001f);
				for (int k = 0; k<3; k++)
				{
					int nTmpDes = (int)pbyDes[k] * (int)pbyDes[3];
					int nTmpSrc = (int)pbySrc[k] * (int)pbySrc[3];
					int nTmp = nTmpDes + nTmpSrc - (((nTmpDes + 255) * (nTmpSrc + 255)) >> 15);
					nTmp = (nTmp <= 0) ? 0 : (int)((float)nTmp * fTmpScale + 0.5f);
					pbyDes[k] = (BYTE)((nTmp > 255) ? 255 : nTmp);
				}
				pbyDes[3] = (BYTE)((nTmpA + 255) >> 8);
			}
			pDesLine += nDesAddWidth;
			pSrcLine += nSrcAddWidth;
		}
		break;
	case BUFFER_HARD_LIGHT:		// 强光
		for (int i = nDesY - srcDesY; i<nDesHeight; i += nLineInc)
		{
			DWORD *pTmpDes = pDesLine, *pTmpSrc = pSrcLine;
			for (int j = 0; j<nDesWidth; j++, pTmpDes++, pTmpSrc++)
			{
				BYTE *pbyDes = (BYTE*)pTmpDes, *pbySrc = (BYTE*)pTmpSrc;
				int nTmpA = (pbyDes[3] * 255);
				//int nTmpA = ((int)pbyDes[3] + (int)pbySrc[3]) * 255 - (int)pbyDes[3] * (int)pbySrc[3];
				double dTmpScale = 0.0078431372549 / ((double)nTmpA + 0.00000000000001);
				for (int k = 0; k<3; k++)
				{
					int nTmpDes = (int)pbyDes[k] * (int)pbyDes[3];
					int nTmpSrc = (int)pbySrc[k] * (int)pbySrc[3];
					int nTmp = nTmpDes * nTmpSrc;
					nTmp = (nTmpSrc < 32513) ? (int)((double)nTmp * dTmpScale + 0.5) : \
						(int)((double)(65025 * (nTmpSrc + nTmpDes) - nTmp) * dTmpScale - 255.0);
					nTmp = (nTmp <= 0) ? 0 : nTmp;
					pbyDes[k] = (BYTE)((nTmp > 255) ? 255 : nTmp);
				}
				pbyDes[3] = (BYTE)((nTmpA + 255) >> 8);
			}
			pDesLine += nDesAddWidth;
			pSrcLine += nSrcAddWidth;
		}
		break;
	case BUFFER_SOFT_LIGHT:		// 柔光
		for (int i = nDesY - srcDesY; i<nDesHeight; i += nLineInc)
		{
			DWORD *pTmpDes = pDesLine, *pTmpSrc = pSrcLine;
			for (int j = 0; j<nDesWidth; j++, pTmpDes++, pTmpSrc++)
			{
				BYTE *pbyDes = (BYTE*)pTmpDes, *pbySrc = (BYTE*)pTmpSrc;
				int nTmpA = (pbyDes[3] * 255);
				//int nTmpA = ((int)pbyDes[3] + (int)pbySrc[3]) * 255 - (int)pbyDes[3] * (int)pbySrc[3];
				double dTmpScale = 1.0 / ((double)nTmpA * 6.03086294e-8 + 0.00000000000001);
				for (int k = 0; k<3; k++)
				{
					int nTmpDes = (int)pbyDes[k] * (int)pbyDes[3];
					int nTmpSrc = (((int)pbySrc[k] * (int)pbySrc[3]));

					int nTmp = (nTmpDes + nTmpSrc + 0xff) >> 8;

					//int nTmp = (65025 * (nTmpSrc + nTmpDes) - nTmpSrc * nTmpDes) /65025;//(int)((double)(nTmpDes * (65025 * (nTmpSrc + nTmpDes) - nTmpSrc * nTmpDes)) * dTmpScale + 0.5);
					pbyDes[k] = (BYTE)((nTmp > 255) ? 255 : nTmp);
				}
				pbyDes[3] = (BYTE)((nTmpA + 255) >> 8);
			}
			pDesLine += nDesAddWidth;
			pSrcLine += nSrcAddWidth;
		}
		break;
	case BUFFER_PIN_LIGHT:		// 点光
		for (int i = nDesY - srcDesY; i<nDesHeight; i += nLineInc)
		{
			DWORD *pTmpDes = pDesLine, *pTmpSrc = pSrcLine;
			for (int j = 0; j<nDesWidth; j++, pTmpDes++, pTmpSrc++)
			{
				BYTE *pbyDes = (BYTE*)pTmpDes, *pbySrc = (BYTE*)pTmpSrc;
				int nTmpA = (pbyDes[3] * 255);
				//int nTmpA = ((int)pbyDes[3] + (int)pbySrc[3]) * 255 - (int)pbyDes[3] * (int)pbySrc[3];
				float fTmpScale = 255.f / ((float)nTmpA + 0.000001f);
				for (int k = 0; k<3; k++)
				{
					int nTmpDes = (int)pbyDes[k] * (int)pbyDes[3];
					int nTmpSrc = (int)pbySrc[k] * (int)pbySrc[3];
					int nTmp = (nTmpSrc < 32513) ? ((nTmpDes > nTmpSrc) ? nTmpSrc : nTmpDes) : \
						((nTmpDes < nTmpSrc) ? nTmpSrc : nTmpDes);
					nTmp = (int)((float)nTmp * fTmpScale + 0.5f);
					pbyDes[k] = (BYTE)((nTmp > 255) ? 255 : nTmp);
				}
				pbyDes[3] = (BYTE)((nTmpA + 255) >> 8);
			}
			pDesLine += nDesAddWidth;
			pSrcLine += nSrcAddWidth;
		}
		break;
	case BUFFER_OVERLAY:			// 叠加
		for (int i = nDesY - srcDesY; i<nDesHeight; i += nLineInc)
		{
			DWORD *pTmpDes = pDesLine, *pTmpSrc = pSrcLine;
			for (int j = 0; j<nDesWidth; j++, pTmpDes++, pTmpSrc++)
			{
				BYTE *pbyDes = (BYTE*)pTmpDes, *pbySrc = (BYTE*)pTmpSrc;
				int nTmpA = ((int)pbyDes[3] + (int)pbySrc[3]) * 255 - (int)pbyDes[3] * (int)pbySrc[3];
				double dTmpScale = 0.0078431372549 / ((double)nTmpA + 0.00000000000001);
				for (int k = 0; k<3; k++)
				{
					int nTmpDes = (int)pbyDes[k] * (int)pbyDes[3];
					int nTmpSrc = (int)pbySrc[k] * (int)pbySrc[3];
					int nTmp = nTmpDes * nTmpSrc;
					nTmp = (nTmpDes < 32513) ? (int)((double)nTmp * dTmpScale + 0.5) : \
						(int)((double)(65025 * (nTmpSrc + nTmpDes) - nTmp) * dTmpScale - 255.0);
					nTmp = (nTmp <= 0) ? 0 : nTmp;
					pbyDes[k] = (BYTE)((nTmp > 255) ? 255 : nTmp);
				}
				pbyDes[3] = (BYTE)((nTmpA + 255) >> 8);
			}
			pDesLine += nDesAddWidth;
			pSrcLine += nSrcAddWidth;
		}
		break;
	case BUFFER_HUE:				// 相位替换
		break;
	case BUFFER_LUM:				// 亮度替换
		break;
	case BUFFER_SAT:				// 饱和度替换
		break;
	case BUFFER_CHROMA:			// 色度替换
		break;
	default:
		break;
		//assert(FALSE);
	}
	return TRUE;
}

BOOL BufferMemSSE::Buffer_BGRA_BLStretch_SSE(OUT DWORD * pDes, IN int nDesX, IN int nDesY, IN int nDesWidth, IN int nDesHeight, IN int nDesBufferWidth, IN int nDesBufferHeight, IN DWORD * pSrc, IN int nSrcX, IN int nSrcY, IN int nSrcWidth, IN int nSrcHeight, IN int nSrcBufferWidth, IN int nSrcBufferHeight, IN BUFFER_MODE eMode, IN BUFFER_PARITY_STYLE eParity, IN RECT * prcDesClip)
{
	return 0;
}

BOOL BufferMemSSE::Buffer_Parity_Range(IN OUT int &nDesY, OUT int & nDesAddWidth, IN int nDesBufferWidth, IN int nDesClipHeight, IN OUT int &nSrcY, OUT int & nSrcAddWidth, IN int nSrcBufferWidth, IN int nSrcBufferHeight, OUT int & nLineInc, IN BUFFER_PARITY_STYLE eParity, IN int n, IN int k)
{
	return 0;
}

