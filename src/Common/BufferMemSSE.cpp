#include "stdafx.h"
#include "BufferMemSSE.h"
#include "Common\BufferFunction.h"
#include "BufferSSE2.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
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
	if (!Buffer_Fix_CutRange(nDesX, nDesY, nDesWidth, nDesHeight, nDesBufferWidth, nDesBufferHeight, \
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
			//bgra32_line_mix(pDesLine, pSrcLine, na16, nm16, g_pdwMixGainTable);
			bgra32_line_alpha_blend(pDesLine, pSrcLine, na16, nm16);
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
	if (pSrc == NULL || pDes == NULL)
		return FALSE;

	float fScaleWidth(1.0f), fScaleHeight(1.0f), fSrcStartX(0.0f), fSrcStartY(0.0f);
	int nDesAddWidth(0), nLineIndex(0), nLineInc(1);

	// 获得有效的操作范围
	if (!Buffer_Stretch_CutRange(nDesX, nDesY, nDesWidth, nDesHeight, nDesBufferWidth, nDesBufferHeight, \
		nSrcX, nSrcY, nSrcWidth, nSrcHeight, nSrcBufferWidth, nSrcBufferHeight, 1, \
		fScaleWidth, fScaleHeight, fSrcStartX, fSrcStartY, nDesAddWidth, nLineIndex, nLineInc, \
		eParity, prcDesClip))
		return FALSE;

	// 申请权重表与索引表的内存空间
	__declspec(align(16)) DWORD* pPowerX = (DWORD*)_mm_malloc(sizeof(DWORD) * (nDesWidth), 16);		// X 方向上目标列映射到源列的权重表
	__declspec(align(16)) int* pMapX = (int*)_mm_malloc(sizeof(int) * (nDesWidth), 16);			// X 方向上目标列映射到源列的索引表
	__declspec(align(16)) DWORD* pPowerY = (DWORD*)_mm_malloc(sizeof(DWORD) * (nDesHeight), 16);	// Y 方向上目标行映射到源行的权重表
	__declspec(align(16)) int* pMapY = (int*)_mm_malloc(sizeof(int) * (nDesHeight), 16);			// Y 方向上目标行映射到源行的索引表
	__declspec(align(16)) DWORD* pTmpBuf = NULL;	// 临时缓存
											// Buffer 起始地址
	DWORD *pDesLine = pDes + nDesY * nDesBufferWidth + nDesX, *pSrcLine = NULL;
	int nSrcPitch = (nSrcBufferWidth << 2);
	int na16 = (nDesWidth >> 2), nm16 = ((nDesWidth & 0x03) << 2);

	// 计算权重与映射索引
	Buffer_Power_BLStretch(pPowerX, pMapX, pPowerY, pMapY, nDesWidth, nDesHeight, \
		nSrcX, nSrcY, nSrcWidth, nSrcHeight, nSrcBufferWidth, nSrcBufferHeight, \
		fScaleWidth, fScaleHeight, fSrcStartX, fSrcStartY, sizeof(DWORD));
	// 逐行处理
	switch (eMode)
	{
	case BUFFER_COPY:		// 拷贝
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			pSrcLine = pSrc + pMapY[nLineIndex] * nSrcBufferWidth;
			bgra32_bl_copy_line(pDesLine, nDesWidth, pSrcLine, nSrcPitch, pPowerX, pMapX, pPowerY[nLineIndex]);
			pDesLine += nDesAddWidth;
		}
		break;
	case BUFFER_MIX:			// Alpha 混叠
		pTmpBuf = (DWORD*)_mm_malloc(sizeof(DWORD) * (nDesWidth), 16);
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			pSrcLine = pSrc + pMapY[nLineIndex] * nSrcBufferWidth;
			bgra32_bl_copy_line(pTmpBuf, nDesWidth, pSrcLine, nSrcPitch, pPowerX, pMapX, pPowerY[nLineIndex]);
			bgra32_line_mix(pDesLine, pTmpBuf, na16, nm16, g_pdwMixGainTable);
			pDesLine += nDesAddWidth;
		}
		break;
	case BUFFER_MIX_NOTSRC:	// Alpha 反向源混叠
		pTmpBuf = (DWORD*)_mm_malloc(sizeof(DWORD) * (nDesWidth), 16);
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			pSrcLine = pSrc + pMapY[nLineIndex] * nSrcBufferWidth;
			bgra32_bl_copy_line(pTmpBuf, nDesWidth, pSrcLine, nSrcPitch, pPowerX, pMapX, pPowerY[nLineIndex]);
			bgra32_line_mix_notsrc(pDesLine, pTmpBuf, na16, nm16, g_pdwMixGainTable);
			pDesLine += nDesAddWidth;
		}
		break;
	case BUFFER_ALPHA_BLEND:		// 快速 Alpha 混合
		pTmpBuf = (DWORD*)_mm_malloc(sizeof(DWORD) * (nDesWidth), 16);
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			pSrcLine = pSrc + pMapY[nLineIndex] * nSrcBufferWidth;
			bgra32_bl_copy_line(pTmpBuf, nDesWidth, pSrcLine, nSrcPitch, pPowerX, pMapX, pPowerY[nLineIndex]);
			bgra32_line_alpha_blend(pDesLine, pTmpBuf, na16, nm16);
			pDesLine += nDesAddWidth;
		}
		break;
	case BUFFER_ALPHA_BLEND_NOT:	// 快速 Alpha 反向源混合
		pTmpBuf = (DWORD*)_mm_malloc(sizeof(DWORD) * (nDesWidth), 16);
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			pSrcLine = pSrc + pMapY[nLineIndex] * nSrcBufferWidth;
			bgra32_bl_copy_line(pTmpBuf, nDesWidth, pSrcLine, nSrcPitch, pPowerX, pMapX, pPowerY[nLineIndex]);
			bgra32_line_alpha_blend_not(pDesLine, pTmpBuf, na16, nm16);
			pDesLine += nDesAddWidth;
		}
		break;
	case BUFFER_LINEAR_DODGE:	// 线性减淡
		pTmpBuf = (DWORD*)_mm_malloc(sizeof(DWORD) * (nDesWidth), 16);
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			pSrcLine = pSrc + pMapY[nLineIndex] * nSrcBufferWidth;
			bgra32_bl_copy_line(pTmpBuf, nDesWidth, pSrcLine, nSrcPitch, pPowerX, pMapX, pPowerY[nLineIndex]);
			bgra32_line_linear_dodge(pDesLine, pTmpBuf, na16, nm16, g_pdwMixGainTable);
			pDesLine += nDesAddWidth;
		}
		break;
	case BUFFER_LINEAR_BURN:		// 线性加深
		pTmpBuf = (DWORD*)_mm_malloc(sizeof(DWORD) * (nDesWidth), 16);
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			pSrcLine = pSrc + pMapY[nLineIndex] * nSrcBufferWidth;
			bgra32_bl_copy_line(pTmpBuf, nDesWidth, pSrcLine, nSrcPitch, pPowerX, pMapX, pPowerY[nLineIndex]);
			bgra32_line_linear_burn(pDesLine, pTmpBuf, na16, nm16, g_pdwMixGainTable);
			pDesLine += nDesAddWidth;
		}
		break;
	case BUFFER_COLOR_DODGE:		// 颜色减淡
		break;
	case BUFFER_COLOR_BURN:		// 颜色加深
		break;
	case BUFFER_MULTIPLY:		// 正片叠底
		break;
	case BUFFER_SCREEN:			// 屏幕滤色
		break;
	case BUFFER_LIGHTEN:			// 变亮
		break;
	case BUFFER_DARKEN:			// 变暗
		break;
	case BUFFER_DIFFERENCE:		// 差值
		break;
	case BUFFER_EXCLUSION:		// 排除
		break;
	case BUFFER_HARD_LIGHT:		// 强光
		break;
	case BUFFER_SOFT_LIGHT:		// 柔光
		break;
	case BUFFER_PIN_LIGHT:		// 点光
		break;
	case BUFFER_OVERLAY:			// 叠加
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
	// 释放权重表与索引表
	_mm_free(pPowerX);
	_mm_free(pMapX);
	_mm_free(pPowerY);
	_mm_free(pMapY);
	if ((pTmpBuf) != NULL)
		_mm_free(pTmpBuf);

	return TRUE;
}

BOOL BufferMemSSE::Buffer_Parity_Range(IN OUT int &nDesY, OUT int & nDesAddWidth, IN int nDesBufferWidth, IN int nDesClipHeight, IN OUT int &nSrcY, OUT int & nSrcAddWidth, IN int nSrcBufferWidth, IN int nSrcBufferHeight, OUT int & nLineInc, IN BUFFER_PARITY_STYLE eParity, IN int n, IN int k)
{
	switch (eParity)		// 根据奇偶场类型获得增量
	{
	case BUFFER_PARITY_BOTH:		// 全场
		nDesAddWidth = nDesBufferWidth;
		nSrcAddWidth = nSrcBufferWidth;
		nLineInc = 1;
		break;
	case BUFFER_PARITY_EVEN:			// 偶场
		if ((nDesY % 2) == 1)		// 起点在奇场，则从下一行开始
		{
			nDesY++;
			nSrcY++;
			// 判断范围的有效性
			if (nDesY >= nDesClipHeight || nSrcY >= nSrcBufferHeight)
				return FALSE;
		}
		nDesAddWidth = (nDesBufferWidth << 1);
		nSrcAddWidth = (nSrcBufferWidth << 1);
		nLineInc = 2;
		break;
	case BUFFER_PARITY_ODD:				// 奇场
		if ((nDesY % 2) == 0)		// 起点在偶场，则从下一行开始
		{
			nDesY++;
			nSrcY++;
			// 判断范围的有效性
			if (nDesY >= nDesClipHeight || nSrcY >= nSrcBufferHeight)
				return FALSE;
		}
		nDesAddWidth = (nDesBufferWidth << 1);
		nSrcAddWidth = (nSrcBufferWidth << 1);
		nLineInc = 2;
		break;
	case BUFFER_PARITY_FOUR_0:
	case BUFFER_PARITY_FOUR_1:
	case BUFFER_PARITY_FOUR_2:
	case BUFFER_PARITY_FOUR_3:
	{
		int noline = eParity - BUFFER_PARITY_FOUR_0;
		int nline = nDesY % 4;
		nline = (4 + noline - nline) % 4;
		nDesY += nline;
		nSrcY += nline;
		if (nDesY >= nDesClipHeight || nSrcY >= nSrcBufferHeight)
			return FALSE;
		nDesAddWidth = (nDesBufferWidth << 2);
		nSrcAddWidth = (nSrcBufferWidth << 2);
		nLineInc = 4;
	}
	break;
	case BUFFER_PARITY_N_K:
	{
		int noline = k;
		int nline = nDesY % n;
		nline = (n + noline - nline) % n;
		nDesY += nline;
		nSrcY += nline;
		if (nDesY >= nDesClipHeight || nSrcY >= nSrcBufferHeight)
			return FALSE;
		nDesAddWidth = (nDesBufferWidth * n);
		nSrcAddWidth = (nSrcBufferWidth * n);
		nLineInc = n;
	}
	break;
	default:
		//assert(FALSE);
		return FALSE;
	}
	return TRUE;
}

BOOL BufferMemSSE::Buffer_Fix_CutRange(IN OUT int &nDesX, IN OUT int &nDesY, IN OUT int &nDesWidth, IN OUT int &nDesHeight, IN int nDesBufferWidth, IN int nDesBufferHeight, IN OUT int &nSrcX, IN OUT int &nSrcY, IN OUT int &nSrcWidth, IN OUT int &nSrcHeight, IN int nSrcBufferWidth, IN int nSrcBufferHeight, IN RECT * prcDesClip)
{
	// 边界有效性检测
	if (nDesBufferWidth <= 0 || nDesBufferHeight <= 0
		|| nSrcBufferWidth <= 0 || nSrcBufferHeight <= 0
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

BOOL BufferMemSSE::Buffer_Stretch_CutRange(IN OUT int &nDesX, IN OUT int &nDesY, IN OUT int &nDesWidth, IN OUT int &nDesHeight, IN int nDesBufferWidth, IN int nDesBufferHeight, IN OUT int &nSrcX, IN OUT int &nSrcY, IN OUT int &nSrcWidth, IN OUT int &nSrcHeight, IN int nSrcBufferWidth, IN int nSrcBufferHeight, IN int nEdgeOffset, OUT float & fScaleWidth, OUT float & fScaleHeight, OUT float & fSrcStartX, OUT float & fSrcStartY, OUT int & nDesAddWidth, OUT int & nLineIndex, OUT int & nLineInc, IN BUFFER_PARITY_STYLE eParity, IN RECT * prcDesClip)
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

void BufferMemSSE::Buffer_Power_BLStretch(OUT DWORD * pPowerX, OUT int * pMapX, OUT DWORD * pPowerY, OUT int * pMapY, IN int nDesWidth, IN int nDesHeight, IN int nSrcX, IN int nSrcY, IN int nSrcWidth, IN int nSrcHeight, IN int nSrcBufferWidth, IN int nSrcBufferHeight, IN float fScaleWidth, IN float fScaleHeight, IN float fSrcStartX, IN float fSrcStartY, IN int nPixelBytes)
{
	int nMapTmp(0), nLastWidth(nSrcBufferWidth - 2), nLastHeight(nSrcBufferHeight - 2);
	DWORD dwPowerTmp1(0), dwPowerTmp2(0);

	nDesWidth--; nDesHeight--;
	// 计算 X 方向的权重表与索引表
	for (int i = 0; i<nDesWidth; i++, fSrcStartX += fScaleWidth)
	{
		nMapTmp = (int)fSrcStartX;
		dwPowerTmp1 = (DWORD)((nMapTmp + 1.0f - fSrcStartX) * 65535.0f + 0.5f);
		dwPowerTmp2 = 65535 - dwPowerTmp1;
		pPowerX[i] = (dwPowerTmp1 | (dwPowerTmp2 << 16));
		nMapTmp = (nMapTmp > nLastWidth) ? nLastWidth : nMapTmp;
		pMapX[i] = (nMapTmp * nPixelBytes);
	}
	if (nSrcX + nSrcWidth >= nSrcBufferWidth)
	{
		pMapX[nDesWidth] = ((nSrcBufferWidth - 2) * nPixelBytes);
		pPowerX[nDesWidth] = 0xffff0000;
	}
	else
	{
		nMapTmp = (int)fSrcStartX;
		dwPowerTmp1 = (DWORD)((nMapTmp + 1.0f - fSrcStartX) * 65535.0f + 0.5f);
		dwPowerTmp2 = 65535 - dwPowerTmp1;
		pPowerX[nDesWidth] = (dwPowerTmp1 | (dwPowerTmp2 << 16));
		pMapX[nDesWidth] = (nMapTmp * nPixelBytes);
	}
	// 计算 Y 方向的权重表与索引表
	for (int i = 0; i<nDesHeight; i++, fSrcStartY += fScaleHeight)
	{
		nMapTmp = (int)fSrcStartY;
		dwPowerTmp1 = (DWORD)((nMapTmp + 1.0f - fSrcStartY) * 65535.0f + 0.5f);
		pPowerY[i] = (dwPowerTmp1 | (dwPowerTmp1 << 16));
		nMapTmp = (nMapTmp > nLastHeight) ? nLastHeight : nMapTmp;
		pMapY[i] = nMapTmp;
	}
	if (nSrcY + nSrcHeight >= nSrcBufferHeight)
	{
		pMapY[nDesHeight] = nSrcBufferHeight - 2;
		pPowerY[nDesHeight] = 0x00000000;
	}
	else
	{
		pMapY[nDesHeight] = nMapTmp = (int)fSrcStartY;
		dwPowerTmp1 = (DWORD)((nMapTmp + 1.0f - fSrcStartY) * 65535.0f + 0.5f);
		pPowerY[nDesHeight] = (dwPowerTmp1 | (dwPowerTmp1 << 16));
	}
}

//////////////////////////////////////////////////////////////////////////
// 用于加权平均缩放时计算权重的接口
//////////////////////////////////////////////////////////////////////////
// 计算水平或垂直方向上某个像素的权重个数及其首尾权重
int calc_power_pixel(OUT float* pPower,		// 首尾权重表
	IN OUT float& fStart,	// 起始点坐标值
	IN float fStep)		// 缩放步长
{
	if (fStep < 1.0f)
	{
		if (1.0f - fStart > fStep)
		{
			pPower[0] = fStep;
			fStart += fStep;
			return 1;
		}
	}
	pPower[0] = 1.0f - fStart;

	// 上一次计算剩余占一个位置
	float fLeave = fStep - (1.0f - fStart);
	int n = (int)floorf(fLeave);

	fStart = pPower[1] = (fLeave - n);

	return n + 2;
}
// 计算水平或垂直方向上各像素的权重个数表与首尾权重表
void strech_calc_power(OUT float* pPower,		// 权重表
	OUT int* pMap,			// 每个点占用的权重个数表
	IN int nCount,			// 目标拷贝长度或高度
	IN float fStep,			// 缩放步长
	IN float fStart = 0.0f)	// 起始点坐标值
{
	nCount--;
	for (int i = 0; i<nCount; i++)
	{
		pMap[i] = calc_power_pixel(pPower, fStart, fStep);
		pPower += 2;
	}
	pMap[nCount] = calc_power_pixel(pPower, fStart, fStep) - 1;
	pPower[1] = 1.f;
}

BOOL BufferMemSSE::Buffer_BGRA_MeanStretch_SSE(OUT DWORD * pDes, IN int nDesX, IN int nDesY, IN int nDesWidth, IN int nDesHeight, IN int nDesBufferWidth, IN int nDesBufferHeight, IN DWORD * pSrc, IN int nSrcX, IN int nSrcY, IN int nSrcWidth, IN int nSrcHeight, IN int nSrcBufferWidth, IN int nSrcBufferHeight, IN BUFFER_MODE eMode, IN BUFFER_PARITY_STYLE eParity, IN RECT * prcDesClip)
{
	if (pSrc == NULL || pDes == NULL)
		return FALSE;

	float fScaleWidth(1.0f), fScaleHeight(1.0f), fSrcStartX(0.0f), fSrcStartY(0.0f);
	int nDesAddWidth(0), nLineIndex(0), nLineInc(1);

	// 获得有效的操作范围
	if (!Buffer_Stretch_CutRange(nDesX, nDesY, nDesWidth, nDesHeight, nDesBufferWidth, nDesBufferHeight, \
		nSrcX, nSrcY, nSrcWidth, nSrcHeight, nSrcBufferWidth, nSrcBufferHeight, 0, \
		fScaleWidth, fScaleHeight, fSrcStartX, fSrcStartY, nDesAddWidth, nLineIndex, nLineInc, \
		eParity, prcDesClip))
		return FALSE;

	// 申请权重表、权重数量表及缓存空间
	float fScaleW_I = 1.0f / fScaleWidth, fScaleH_I = 1.0f / fScaleHeight;
	SYSSE_DECLARE(int*) pMapX = SYSSE_NEW(nDesWidth, int);					// X 方向上的权重数量表
	SYSSE_DECLARE(int*) pMapY = SYSSE_NEW(nDesHeight, int);					// Y 方向上的权重数量表
	SYSSE_DECLARE(float*) pPowerX = SYSSE_NEW((nDesWidth << 1), float);		// X 方向上的权重表
	SYSSE_DECLARE(float*) pPowerY = SYSSE_NEW((nDesHeight << 1), float);	// Y 方向上的权重表
	DWORD *pDesLine = pDes + nDesX + nDesY * nDesBufferWidth;				// Buffer 起始地址
	DWORD *pSrcLine = pSrc + nSrcX + nSrcY * nSrcBufferWidth;
	float *pTmpPowerY = pPowerY;											// Y 方向权重表地址
	int nSrcPitch = (nSrcBufferWidth << 2), na16 = (nDesWidth >> 2), nm16 = ((nDesWidth & 0x03) << 2);
	SYSSE_DECLARE(float*) pTmpLine = SYSSE_NEW((nSrcWidth << 2), float);	// 源行的临时缓存
	SYSSE_DECLARE(DWORD*) pTmpBuf = NULL;			// 临时缓存
	SYSSE_DECLARE(float) fCoefW[4] = { fScaleW_I, fScaleW_I, fScaleW_I, fScaleW_I };
	SYSSE_DECLARE(float) fCoefH[4] = { fScaleH_I, fScaleH_I, fScaleH_I, fScaleH_I };

	// 计算权重
	strech_calc_power(pPowerX, pMapX, nDesWidth, fScaleWidth, fSrcStartX - (int)fSrcStartX);
	strech_calc_power(pPowerY, pMapY, nDesHeight, fScaleHeight, fSrcStartY - (int)fSrcStartY);
	// 逐行处理
	switch (eMode)
	{
	case BUFFER_COPY:		// 拷贝
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			bgra32_mean_copy_add_y_quick(pTmpLine, pSrcLine, nSrcWidth, nSrcPitch, pTmpPowerY, fCoefH, pMapY[nLineIndex]);
			bgra32_mean_copy_add_x(pDesLine, nDesWidth, pTmpLine, pPowerX, pMapX, fCoefW);
			pSrcLine += ((pMapY[nLineIndex] + ((nLineInc == 1) ? -1 : (pMapY[nLineIndex + 1] - 2))) * nSrcBufferWidth);
			pTmpPowerY += ((nLineInc == 1) ? 2 : 4);
			pDesLine += nDesAddWidth;
		}
		break;
	case BUFFER_MIX:			// Alpha 混叠
		pTmpBuf = SYSSE_NEW(nDesWidth, DWORD);
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			bgra32_mean_copy_add_y_quick(pTmpLine, pSrcLine, nSrcWidth, nSrcPitch, pTmpPowerY, fCoefH, pMapY[nLineIndex]);
			bgra32_mean_copy_add_x(pTmpBuf, nDesWidth, pTmpLine, pPowerX, pMapX, fCoefW);
			bgra32_line_mix(pDesLine, pTmpBuf, na16, nm16, g_pdwMixGainTable);
			if (nLineInc == 1)
			{
				pSrcLine += ((pMapY[nLineIndex] - 1) * nSrcBufferWidth);
				pTmpPowerY += 2;
			}
			else
			{
				pSrcLine += (pMapY[nLineIndex] - 2 + pMapY[nLineIndex + 1]) * nSrcBufferWidth;
				pTmpPowerY += 4;
			}
			pDesLine += nDesAddWidth;
		}
		break;
	case BUFFER_MIX_NOTSRC:	// Alpha 反向源混叠
		pTmpBuf = SYSSE_NEW(nDesWidth, DWORD);
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			bgra32_mean_copy_add_y_quick(pTmpLine, pSrcLine, nSrcWidth, nSrcPitch, pTmpPowerY, fCoefH, pMapY[nLineIndex]);
			bgra32_mean_copy_add_x(pTmpBuf, nDesWidth, pTmpLine, pPowerX, pMapX, fCoefW);
			bgra32_line_mix_notsrc(pDesLine, pTmpBuf, na16, nm16, g_pdwMixGainTable);
			pSrcLine += ((pMapY[nLineIndex] + (nLineInc == 1) ? -1 : (pMapY[nLineIndex + 1] - 2)) * nSrcBufferWidth);
			pTmpPowerY += ((nLineInc == 1) ? 2 : 4);
			pDesLine += nDesAddWidth;
		}
		break;
	case BUFFER_ALPHA_BLEND:		// 快速 Alpha 混合
		pTmpBuf = SYSSE_NEW(nDesWidth, DWORD);
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			bgra32_mean_copy_add_y_quick(pTmpLine, pSrcLine, nSrcWidth, nSrcPitch, pTmpPowerY, fCoefH, pMapY[nLineIndex]);
			bgra32_mean_copy_add_x(pTmpBuf, nDesWidth, pTmpLine, pPowerX, pMapX, fCoefW);
			bgra32_line_alpha_blend(pDesLine, pTmpBuf, na16, nm16);
			pSrcLine += ((pMapY[nLineIndex] + (nLineInc == 1) ? -1 : (pMapY[nLineIndex + 1] - 2)) * nSrcBufferWidth);
			pTmpPowerY += ((nLineInc == 1) ? 2 : 4);
			pDesLine += nDesAddWidth;
		}
		break;
	case BUFFER_ALPHA_BLEND_NOT:	// 快速 Alpha 反向源混合
		pTmpBuf = SYSSE_NEW(nDesWidth, DWORD);
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			bgra32_mean_copy_add_y_quick(pTmpLine, pSrcLine, nSrcWidth, nSrcPitch, pTmpPowerY, fCoefH, pMapY[nLineIndex]);
			bgra32_mean_copy_add_x(pTmpBuf, nDesWidth, pTmpLine, pPowerX, pMapX, fCoefW);
			bgra32_line_alpha_blend_not(pDesLine, pTmpBuf, na16, nm16);
			pSrcLine += ((pMapY[nLineIndex] + (nLineInc == 1) ? -1 : (pMapY[nLineIndex + 1] - 2)) * nSrcBufferWidth);
			pTmpPowerY += ((nLineInc == 1) ? 2 : 4);
			pDesLine += nDesAddWidth;
		}
		break;
	case BUFFER_LINEAR_DODGE:	// 线性减淡
		pTmpBuf = SYSSE_NEW(nDesWidth, DWORD);
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			bgra32_mean_copy_add_y_quick(pTmpLine, pSrcLine, nSrcWidth, nSrcPitch, pTmpPowerY, fCoefH, pMapY[nLineIndex]);
			bgra32_mean_copy_add_x(pTmpBuf, nDesWidth, pTmpLine, pPowerX, pMapX, fCoefW);
			bgra32_line_linear_dodge(pDesLine, pTmpBuf, na16, nm16, g_pdwMixGainTable);
			pSrcLine += ((pMapY[nLineIndex] + (nLineInc == 1) ? -1 : (pMapY[nLineIndex + 1] - 2)) * nSrcBufferWidth);
			pTmpPowerY += ((nLineInc == 1) ? 2 : 4);
			pDesLine += nDesAddWidth;
		}
		break;
	case BUFFER_LINEAR_BURN:		// 线性加深
		pTmpBuf = SYSSE_NEW(nDesWidth, DWORD);
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			bgra32_mean_copy_add_y_quick(pTmpLine, pSrcLine, nSrcWidth, nSrcPitch, pTmpPowerY, fCoefH, pMapY[nLineIndex]);
			bgra32_mean_copy_add_x(pTmpBuf, nDesWidth, pTmpLine, pPowerX, pMapX, fCoefW);
			bgra32_line_linear_burn(pDesLine, pTmpBuf, na16, nm16, g_pdwMixGainTable);
			pSrcLine += ((pMapY[nLineIndex] + (nLineInc == 1) ? -1 : (pMapY[nLineIndex + 1] - 2)) * nSrcBufferWidth);
			pTmpPowerY += ((nLineInc == 1) ? 2 : 4);
			pDesLine += nDesAddWidth;
		}
		break;
	case BUFFER_COLOR_DODGE:		// 颜色减淡
		break;
	case BUFFER_COLOR_BURN:		// 颜色加深
		break;
	case BUFFER_MULTIPLY:		// 正片叠底
		break;
	case BUFFER_SCREEN:			// 屏幕滤色
		break;
	case BUFFER_LIGHTEN:			// 变亮
		break;
	case BUFFER_DARKEN:			// 变暗
		break;
	case BUFFER_DIFFERENCE:		// 差值
		break;
	case BUFFER_EXCLUSION:		// 排除
		break;
	case BUFFER_HARD_LIGHT:		// 强光
		break;
	case BUFFER_SOFT_LIGHT:		// 柔光
		break;
	case BUFFER_PIN_LIGHT:		// 点光
		break;
	case BUFFER_OVERLAY:			// 叠加
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
	// 释放缓存资源
	SYSSE_FREE(pMapX);
	SYSSE_FREE(pMapY);
	SYSSE_FREE(pPowerX);
	SYSSE_FREE(pPowerY);
	SYSSE_FREE(pTmpLine);
	SYSSE_SAFEFREE(pTmpBuf);

	return TRUE;
}

BOOL BufferMemSSE::Buffer_BGRA_Key_Fix_SSE(IN OUT DWORD * pdwBuf, IN int nDesX, IN int nDesY, IN int nDesWidth, IN int nDesHeight, IN int nDesBufWidth, IN int nDesBufHeight, IN LPBYTE pbyKey, IN int nKeyX, IN int nKeyY, IN int nKeyWidth, IN int nKeyHeight, IN int nKeyBufWidth, IN int nKeyBufHeight, IN int nKeyPitch, IN Common_KEY_MODE eMode, IN BUFFER_PARITY_STYLE eParity, IN RECT * prcDesClip, IN int n, IN int k)
{
	if (pdwBuf == NULL || pbyKey == NULL)
		return FALSE;

	// 获得有效的操作范围
	if (!Buffer_Fix_CutRange(nDesX, nDesY, nDesWidth, nDesHeight, nDesBufWidth, nDesBufHeight, \
		nKeyX, nKeyY, nKeyWidth, nKeyHeight, nKeyBufWidth, nKeyBufHeight, prcDesClip))
		return FALSE;

	int nDesAddWidth(0), nKeyAddPitch(0), nLineInc(0);
	int nDesClipHeight = (prcDesClip == NULL) ? nDesBufHeight	\
		: ((nDesBufHeight > prcDesClip->bottom) ? prcDesClip->bottom : nDesBufHeight);

	// 获得有效的奇偶场操作范围
	if (!Buffer_Parity_Range(nDesY, nDesAddWidth, nDesBufWidth, nDesClipHeight, \
		nKeyY, nKeyAddPitch, nKeyPitch, nKeyBufHeight, nLineInc, eParity, n, k))
		return FALSE;

	//	printf("DesX = %d,DesY = %d\n",nDesX,nDesY);
	DWORD* pdwDesLine = pdwBuf + nDesY * nDesBufWidth + nDesX;		// 目标 Buffer 起始地址
	BYTE* pbyKeyLine = pbyKey + nKeyY * nKeyPitch + nKeyX;			// 键起始地址
	int na16 = (nDesWidth >> 2), nm16 = ((nDesWidth & 0x03) << 2);	// 一行的对齐参数

																	// 逐行处理
	switch (eMode)
	{
	case Common_KEY_MULTIPLY:			// 相乘
		for (int i = 0; i<nDesHeight; i += nLineInc)
		{
			bgra32_key_mul_line(pdwDesLine, pbyKeyLine, na16, nm16);
			pdwDesLine += nDesAddWidth;
			pbyKeyLine += nKeyAddPitch;
		}
		break;
	case Common_KEY_MULTIPLY_NOTSRC:	// 反向源键相乘
		for (int i = 0; i<nDesHeight; i += nLineInc)
		{
			bgra32_key_mul_notsrc_line(pdwDesLine, pbyKeyLine, na16, nm16);
			pdwDesLine += nDesAddWidth;
			pbyKeyLine += nKeyAddPitch;
		}
		break;
	case Common_KEY_MULTIPLY_NOTDES:	// 反向目标键相乘
		for (int i = 0; i<nDesHeight; i += nLineInc)
		{
			bgra32_key_mul_notdes_line(pdwDesLine, pbyKeyLine, na16, nm16);
			pdwDesLine += nDesAddWidth;
			pbyKeyLine += nKeyAddPitch;
		}
		break;
	case Common_KEY_REPLACE:			// 替换
		for (int i = 0; i<nDesHeight; i += nLineInc)
		{
			//			printf("i = %d/n",i);
			bgra32_replace_alpha_line(pdwDesLine, pbyKeyLine, na16, nm16);
			pdwDesLine += nDesAddWidth;
			pbyKeyLine += nKeyAddPitch;
		}
		break;
	case Common_KEY_MIX:				// 混叠
		for (int i = 0; i<nDesHeight; i += nLineInc)
		{
			bgra32_key_mix_line(pdwDesLine, pbyKeyLine, na16, nm16);
			pdwDesLine += nDesAddWidth;
			pbyKeyLine += nKeyAddPitch;
		}
		break;
	case Common_KEY_MIX_NOTSRC:			// 反向源键混叠
		for (int i = 0; i<nDesHeight; i += nLineInc)
		{
			bgra32_key_mix_notsrc_line(pdwDesLine, pbyKeyLine, na16, nm16);
			pdwDesLine += nDesAddWidth;
			pbyKeyLine += nKeyAddPitch;
		}
		break;
	case Common_KEY_SAT_SUB:		// 饱和差值
		for (int i = 0; i<nDesHeight; i += nLineInc)
		{
			bgra32_key_sat_sub_line(pdwDesLine, pbyKeyLine, na16, nm16);
			pdwDesLine += nDesAddWidth;
			pbyKeyLine += nKeyAddPitch;
		}
		break;
	default:
		break;
		//assert(FALSE);
	}
	return TRUE;
}

BOOL BufferMemSSE::Buffer_BGRA_Key_BLStretch_SSE(IN OUT DWORD * pdwBuf, IN int nDesX, IN int nDesY, IN int nDesWidth, IN int nDesHeight, IN int nDesBufWidth, IN int nDesBufHeight, IN LPBYTE pbyKey, IN int nKeyX, IN int nKeyY, IN int nKeyWidth, IN int nKeyHeight, IN int nKeyBufWidth, IN int nKeyBufHeight, IN int nKeyPitch, IN Common_KEY_MODE eMode, IN BUFFER_PARITY_STYLE eParity, IN RECT * prcDesClip)
{
	if (pdwBuf == NULL || pbyKey == NULL)
		return FALSE;

	float fScaleWidth(1.0f), fScaleHeight(1.0f), fSrcStartX(0.0f), fSrcStartY(0.0f);
	int nDesAddWidth(0), nLineIndex(0), nLineInc(1);

	// 获得有效的操作范围
	if (!Buffer_Stretch_CutRange(nDesX, nDesY, nDesWidth, nDesHeight, nDesBufWidth, nDesBufHeight, \
		nKeyX, nKeyY, nKeyWidth, nKeyHeight, nKeyBufWidth, nKeyBufHeight, 1, \
		fScaleWidth, fScaleHeight, fSrcStartX, fSrcStartY, nDesAddWidth, nLineIndex, nLineInc, \
		eParity, prcDesClip))
		return FALSE;

	// 申请权重表与索引表的内存空间
	SYSSE_DECLARE(DWORD*) pPowerX = SYSSE_NEW(nDesWidth, DWORD);	// X 方向上目标列映射到源列的权重表
	SYSSE_DECLARE(int*) pMapX = SYSSE_NEW(nDesWidth, int);			// X 方向上目标列映射到源列的索引表
	SYSSE_DECLARE(DWORD*) pPowerY = SYSSE_NEW(nDesHeight, DWORD);	// Y 方向上目标行映射到源行的权重表
	SYSSE_DECLARE(int*) pMapY = SYSSE_NEW(nDesHeight, int);			// Y 方向上目标行映射到源行的索引表
	SYSSE_DECLARE(BYTE*) pTmpBuf = SYSSE_NEW(nDesWidth, BYTE);		// 临时缓存
																	// Buffer 起始地址
	DWORD* pdwDesLine = pdwBuf + nDesY * nDesBufWidth + nDesX;		// 目标 Buffer 起始地址
	BYTE* pbyKeyLine = NULL;
	int na4 = (nDesWidth >> 2), nm4 = (nDesWidth & 0x03);
	int na16 = (nDesWidth >> 2), nm16 = ((nDesWidth & 0x03) << 2);	// 一行的对齐参数

																	// 计算权重与映射索引
	Buffer_Power_BLStretch(pPowerX, pMapX, pPowerY, pMapY, nDesWidth, nDesHeight, \
		nKeyX, nKeyY, nKeyWidth, nKeyHeight, nKeyBufWidth, nKeyBufHeight, \
		fScaleWidth, fScaleHeight, fSrcStartX, fSrcStartY, sizeof(BYTE));
	// 逐行处理
	switch (eMode)
	{
	case Common_KEY_MULTIPLY:		// 相乘
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			pbyKeyLine = pbyKey + pMapY[nLineIndex] * nKeyPitch;
			keybuf_bl_copy_line(pTmpBuf, na4, nm4, pbyKeyLine, nKeyPitch, pPowerX, pMapX, pPowerY[nLineIndex]);
			bgra32_key_mul_line(pdwDesLine, pTmpBuf, na16, nm16);
			pdwDesLine += nDesAddWidth;
		}
		break;
	case Common_KEY_MULTIPLY_NOTSRC:		// 反向源键相乘
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			pbyKeyLine = pbyKey + pMapY[nLineIndex] * nKeyPitch;
			keybuf_bl_copy_line(pTmpBuf, na4, nm4, pbyKeyLine, nKeyPitch, pPowerX, pMapX, pPowerY[nLineIndex]);
			bgra32_key_mul_notsrc_line(pdwDesLine, pTmpBuf, na16, nm16);
			pdwDesLine += nDesAddWidth;
		}
		break;
	case Common_KEY_MULTIPLY_NOTDES:		// 反向目标键相乘
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			pbyKeyLine = pbyKey + pMapY[nLineIndex] * nKeyPitch;
			keybuf_bl_copy_line(pTmpBuf, na4, nm4, pbyKeyLine, nKeyPitch, pPowerX, pMapX, pPowerY[nLineIndex]);
			bgra32_key_mul_notdes_line(pdwDesLine, pTmpBuf, na16, nm16);
			pdwDesLine += nDesAddWidth;
		}
		break;
	case Common_KEY_REPLACE:		// 替换
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			pbyKeyLine = pbyKey + pMapY[nLineIndex] * nKeyPitch;
			keybuf_bl_copy_line(pTmpBuf, na4, nm4, pbyKeyLine, nKeyPitch, pPowerX, pMapX, pPowerY[nLineIndex]);
			bgra32_replace_alpha_line(pdwDesLine, pTmpBuf, na16, nm16);
			pdwDesLine += nDesAddWidth;
		}
		break;
	case Common_KEY_MIX:		// 混叠
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			pbyKeyLine = pbyKey + pMapY[nLineIndex] * nKeyPitch;
			keybuf_bl_copy_line(pTmpBuf, na4, nm4, pbyKeyLine, nKeyPitch, pPowerX, pMapX, pPowerY[nLineIndex]);
			bgra32_key_mix_line(pdwDesLine, pTmpBuf, na16, nm16);
			pdwDesLine += nDesAddWidth;
		}
		break;
	case Common_KEY_MIX_NOTSRC:		// 反向源键混叠
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			pbyKeyLine = pbyKey + pMapY[nLineIndex] * nKeyPitch;
			keybuf_bl_copy_line(pTmpBuf, na4, nm4, pbyKeyLine, nKeyPitch, pPowerX, pMapX, pPowerY[nLineIndex]);
			bgra32_key_mix_notsrc_line(pdwDesLine, pTmpBuf, na16, nm16);
			pdwDesLine += nDesAddWidth;
		}
		break;
	case Common_KEY_SAT_SUB:		// 饱和差值
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			pbyKeyLine = pbyKey + pMapY[nLineIndex] * nKeyPitch;
			keybuf_bl_copy_line(pTmpBuf, na4, nm4, pbyKeyLine, nKeyPitch, pPowerX, pMapX, pPowerY[nLineIndex]);
			bgra32_key_sat_sub_line(pdwDesLine, pTmpBuf, na16, nm16);
			pdwDesLine += nDesAddWidth;
		}
		break;
	default:
		break;
		//assert(FALSE);
	}
	// 释放权重表与索引表
	SYSSE_FREE(pPowerX);
	SYSSE_FREE(pMapX);
	SYSSE_FREE(pPowerY);
	SYSSE_FREE(pMapY);
	SYSSE_FREE(pTmpBuf);

	return TRUE;
}

BOOL BufferMemSSE::Buffer_Key_Fix_SSE(IN OUT LPBYTE pbyDes, IN int nDesX, IN int nDesY, IN int nDesWidth, IN int nDesHeight, IN int nDesBufWidth, IN int nDesBufHeight, IN int nDesKeyPitch, IN LPBYTE pbySrc, IN int nSrcX, IN int nSrcY, IN int nSrcWidth, IN int nSrcHeight, IN int nSrcBufWidth, IN int nSrcBufHeight, IN int nSrcKeyPitch, IN Common_KEY_MODE eMode, IN BUFFER_PARITY_STYLE eParity, IN RECT * prcDesClip, IN int n, IN int k)
{
	if (pbyDes == NULL || pbySrc == NULL)
		return FALSE;

	// 获得有效的操作范围
	if (!Buffer_Fix_CutRange(nDesX, nDesY, nDesWidth, nDesHeight, nDesBufWidth, nDesBufHeight, \
		nSrcX, nSrcY, nSrcWidth, nSrcHeight, nSrcBufWidth, nSrcBufHeight, prcDesClip))
		return FALSE;

	int nDesAddPitch(0), nSrcAddPitch(0), nLineInc(0);
	int nDesClipHeight = (prcDesClip == NULL) ? nDesBufHeight	\
		: ((nDesBufHeight > prcDesClip->bottom) ? prcDesClip->bottom : nDesBufHeight);

	// 获得有效的奇偶场操作范围
	if (!Buffer_Parity_Range(nDesY, nDesAddPitch, nDesKeyPitch, nDesClipHeight, \
		nSrcY, nSrcAddPitch, nSrcKeyPitch, nSrcBufHeight, nLineInc, eParity, n, k))
		return FALSE;

	LPBYTE pbyDesLine = pbyDes + nDesY * nDesKeyPitch + nDesX;		// 目标键起始地址
	LPBYTE pbySrcLine = pbySrc + nSrcY * nSrcKeyPitch + nSrcX;		// 目标键起始地址
	int na16 = (nDesWidth >> 4), nm16 = (nDesWidth & 0x0f);			// 一行的对齐参数

																	// 逐行处理
	switch (eMode)
	{
	case Common_KEY_MULTIPLY:		// 相乘
		for (int i = 0; i<nDesHeight; i += nLineInc)
		{
			keybuf_mul_buf_line(pbyDesLine, pbySrcLine, na16, nm16);
			pbyDesLine += nDesAddPitch;
			pbySrcLine += nSrcAddPitch;
		}
		break;
	case Common_KEY_MULTIPLY_NOTSRC:		// 反向源键相乘
		for (int i = 0; i<nDesHeight; i += nLineInc)
		{
			keybuf_mul_buf_line_notsrc(pbyDesLine, pbySrcLine, na16, nm16);
			pbyDesLine += nDesAddPitch;
			pbySrcLine += nSrcAddPitch;
		}
		break;
	case Common_KEY_MULTIPLY_NOTDES:		// 反向目标键相乘
		for (int i = 0; i<nDesHeight; i += nLineInc)
		{
			keybuf_mul_buf_line_notdes(pbyDesLine, pbySrcLine, na16, nm16);
			pbyDesLine += nDesAddPitch;
			pbySrcLine += nSrcAddPitch;
		}
		break;
	case Common_KEY_REPLACE:		// 替换
		for (int i = 0; i<nDesHeight; i += nLineInc)
		{
			memcpy_line(pbyDesLine, pbySrcLine, na16, nm16);
			pbyDesLine += nDesAddPitch;
			pbySrcLine += nSrcAddPitch;
		}
		break;
	case Common_KEY_MIX:		// 混叠
		for (int i = 0; i<nDesHeight; i += nLineInc)
		{
			keybuf_mix_buf_line(pbyDesLine, pbySrcLine, na16, nm16);
			pbyDesLine += nDesAddPitch;
			pbySrcLine += nSrcAddPitch;
		}
		break;
	case Common_KEY_MIX_NOTSRC:		// 反向源键混叠
		for (int i = 0; i<nDesHeight; i += nLineInc)
		{
			keybuf_mix_buf_line_notsrc(pbyDesLine, pbySrcLine, na16, nm16);
			pbyDesLine += nDesAddPitch;
			pbySrcLine += nSrcAddPitch;
		}
		break;
	case Common_KEY_SAT_SUB:		// 饱和差值
		for (int i = 0; i<nDesHeight; i += nLineInc)
		{
			keybuf_sat_sub_buf_line(pbyDesLine, pbySrcLine, na16, nm16);
			pbyDesLine += nDesAddPitch;
			pbySrcLine += nSrcAddPitch;
		}
		break;
	default:
		break;
		//assert(FALSE);
	}
	return TRUE;
}

BOOL BufferMemSSE::Buffer_Key_BLStretch_SSE(IN OUT LPBYTE pbyDes, IN int nDesX, IN int nDesY, IN int nDesWidth, IN int nDesHeight, IN int nDesBufWidth, IN int nDesBufHeight, IN int nDesKeyPitch, IN LPBYTE pbySrc, IN int nSrcX, IN int nSrcY, IN int nSrcWidth, IN int nSrcHeight, IN int nSrcBufWidth, IN int nSrcBufHeight, IN int nSrcKeyPitch, IN Common_KEY_MODE eMode, IN BUFFER_PARITY_STYLE eParity, IN RECT * prcDesClip)
{
	if (pbyDes == NULL || pbySrc == NULL)
		return FALSE;

	float fScaleWidth(1.0f), fScaleHeight(1.0f), fSrcStartX(0.0f), fSrcStartY(0.0f);
	int nDesAddPitch(0), nLineIndex(0), nLineInc(1);

	// 获得有效的操作范围
	if (!Buffer_Stretch_CutRange(nDesX, nDesY, nDesWidth, nDesHeight, nDesBufWidth, nDesBufHeight, \
		nSrcX, nSrcY, nSrcWidth, nSrcHeight, nSrcBufWidth, nSrcBufHeight, 1, \
		fScaleWidth, fScaleHeight, fSrcStartX, fSrcStartY, nDesAddPitch, nLineIndex, nLineInc, \
		eParity, prcDesClip))
		return FALSE;

	// 重新定位目标的增长
	nDesAddPitch = (nDesKeyPitch << (nLineInc - 1));

	// 申请权重表与索引表的内存空间
	SYSSE_DECLARE(DWORD*) pPowerX = SYSSE_NEW(nDesWidth, DWORD);	// X 方向上目标列映射到源列的权重表
	SYSSE_DECLARE(int*) pMapX = SYSSE_NEW(nDesWidth, int);			// X 方向上目标列映射到源列的索引表
	SYSSE_DECLARE(DWORD*) pPowerY = SYSSE_NEW(nDesHeight, DWORD);	// Y 方向上目标行映射到源行的权重表
	SYSSE_DECLARE(int*) pMapY = SYSSE_NEW(nDesHeight, int);			// Y 方向上目标行映射到源行的索引表
	SYSSE_DECLARE(BYTE*) pTmpBuf = NULL;				// 临时缓存
														// Buffer 起始地址
	LPBYTE pbyDesLine = pbyDes + nDesY * nDesKeyPitch + nDesX, pbySrcLine = NULL;
	int na4 = (nDesWidth >> 2), nm4 = (nDesWidth & 0x03);
	int na16 = (nDesWidth >> 4), nm16 = (nDesWidth & 0x0f);

	// 计算权重与映射索引
	Buffer_Power_BLStretch(pPowerX, pMapX, pPowerY, pMapY, nDesWidth, nDesHeight, \
		nSrcX, nSrcY, nSrcWidth, nSrcHeight, nSrcBufWidth, nSrcBufHeight, \
		fScaleWidth, fScaleHeight, fSrcStartX, fSrcStartY, sizeof(BYTE));
	// 逐行处理
	switch (eMode)
	{
	case Common_KEY_MULTIPLY:		// 相乘
		pTmpBuf = SYSSE_NEW(nDesWidth, BYTE);
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			pbySrcLine = pbySrc + pMapY[nLineIndex] * nSrcKeyPitch;
			keybuf_bl_copy_line(pTmpBuf, na4, nm4, pbySrcLine, nSrcKeyPitch, pPowerX, pMapX, pPowerY[nLineIndex]);
			keybuf_mul_buf_line(pbyDesLine, pTmpBuf, na16, nm16);
			pbyDesLine += nDesAddPitch;
		}
		break;
	case Common_KEY_MULTIPLY_NOTSRC:		// 反向源键相乘
		pTmpBuf = SYSSE_NEW(nDesWidth, BYTE);
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			pbySrcLine = pbySrc + pMapY[nLineIndex] * nSrcKeyPitch;
			keybuf_bl_copy_line(pTmpBuf, na4, nm4, pbySrcLine, nSrcKeyPitch, pPowerX, pMapX, pPowerY[nLineIndex]);
			keybuf_mul_buf_line_notsrc(pbyDesLine, pTmpBuf, na16, nm16);
			pbyDesLine += nDesAddPitch;
		}
		break;
	case Common_KEY_MULTIPLY_NOTDES:		// 反向目标键相乘
		pTmpBuf = SYSSE_NEW(nDesWidth, BYTE);
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			pbySrcLine = pbySrc + pMapY[nLineIndex] * nSrcKeyPitch;
			keybuf_bl_copy_line(pTmpBuf, na4, nm4, pbySrcLine, nSrcKeyPitch, pPowerX, pMapX, pPowerY[nLineIndex]);
			keybuf_mul_buf_line_notdes(pbyDesLine, pTmpBuf, na16, nm16);
			pbyDesLine += nDesAddPitch;
		}
		break;
	case Common_KEY_REPLACE:		// 替换
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			pbySrcLine = pbySrc + pMapY[nLineIndex] * nSrcKeyPitch;
			keybuf_bl_copy_line(pbyDesLine, na4, nm4, pbySrcLine, nSrcKeyPitch, pPowerX, pMapX, pPowerY[nLineIndex]);
			pbyDesLine += nDesAddPitch;
		}
		break;
	case Common_KEY_MIX:		// 混叠
		pTmpBuf = SYSSE_NEW(nDesWidth, BYTE);
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			pbySrcLine = pbySrc + pMapY[nLineIndex] * nSrcKeyPitch;
			keybuf_bl_copy_line(pTmpBuf, na4, nm4, pbySrcLine, nSrcKeyPitch, pPowerX, pMapX, pPowerY[nLineIndex]);
			keybuf_mix_buf_line(pbyDesLine, pTmpBuf, na16, nm16);
			pbyDesLine += nDesAddPitch;
		}
		break;
	case Common_KEY_MIX_NOTSRC:		// 反向源键混叠
		pTmpBuf = SYSSE_NEW(nDesWidth, BYTE);
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			pbySrcLine = pbySrc + pMapY[nLineIndex] * nSrcKeyPitch;
			keybuf_bl_copy_line(pTmpBuf, na4, nm4, pbySrcLine, nSrcKeyPitch, pPowerX, pMapX, pPowerY[nLineIndex]);
			keybuf_mix_buf_line_notsrc(pbyDesLine, pTmpBuf, na16, nm16);
			pbyDesLine += nDesAddPitch;
		}
		break;
	case Common_KEY_SAT_SUB:		// 饱和差值
		pTmpBuf = SYSSE_NEW(nDesWidth, BYTE);
		for (; nLineIndex<nDesHeight; nLineIndex += nLineInc)
		{
			pbySrcLine = pbySrc + pMapY[nLineIndex] * nSrcKeyPitch;
			keybuf_bl_copy_line(pTmpBuf, na4, nm4, pbySrcLine, nSrcKeyPitch, pPowerX, pMapX, pPowerY[nLineIndex]);
			keybuf_sat_sub_buf_line(pbyDesLine, pTmpBuf, na16, nm16);
			pbyDesLine += nDesAddPitch;
		}
		break;
	default:
		break;
		//assert(FALSE);
	}
	// 释放权重表与索引表
	SYSSE_FREE(pPowerX);
	SYSSE_FREE(pMapX);
	SYSSE_FREE(pPowerY);
	SYSSE_FREE(pMapY);
	SYSSE_SAFEFREE(pTmpBuf);

	return TRUE;
}

BOOL BufferMemSSE::Buffer_Trans_CalcBorder(IN CommonBufferDef32 * pDesBuf, IN int nOffsetLeft, IN int nOffsetTop, IN LPCommonINNERVERTEX pvVertexes, OUT LPCommonINNERVERTEX pvLeftEdgeUVW, OUT LPCommonINNERVERTEX pvDeltaUVW, OUT DWORD ** ppdwDesLeftEdge, OUT int * pnLinePixels, OUT int * pnMaxLinePixels)
{
	return FALSE;
}

void BufferMemSSE::Buffer_Trans_CalcPowerAndIndex_BL(IN LPCommonINNERVERTEX pvLeftEdgeUVW, IN LPCommonINNERVERTEX pvDeltaUVW, IN int nLinePixels, IN int nSrcWidth, IN int nSrcHeight, IN int nSrcBufWidth, OUT DWORD * pdwSrcIndex, OUT float * pfPowerX, OUT float * pfPowerY, OUT float * pfPowerXY)
{
	float fU = pvLeftEdgeUVW->x, fV = pvLeftEdgeUVW->y, fW = pvLeftEdgeUVW->z;
	float fDU = pvDeltaUVW->x, fDV = pvDeltaUVW->y, fDW = pvDeltaUVW->z;
	float fGainW(0.f), fTmp(0.f), fSrcX(0.f), fSrcY(0.f);
	int nSrcX(0), nSrcY(0), nScaleWidth(nSrcWidth - 1), nScaleHeight(nSrcHeight - 1);
	float *pfTmpPowerX = pfPowerX, *pfTmpPowerY = pfPowerY, *pfTmpPowerXY = pfPowerXY;
	DWORD* pdwTmpSrcIndex = pdwSrcIndex;

	fU *= nScaleWidth;
	fV *= nScaleHeight;
	fDU *= nScaleWidth;
	fDV *= nScaleHeight;
	for (int i = 0; i<nLinePixels; i++)
	{
		fGainW = 1.f / fW;
		//fSrcX = fU * nScaleWidth * fGainW;
		//fSrcY = fV * nScaleHeight * fGainW;
		fSrcX = fU * fGainW;
		fSrcY = fV * fGainW;
		nSrcX = (int)fSrcX;
		nSrcY = (int)fSrcY;
		*pfTmpPowerX = nSrcX + 1.f - fSrcX;
		*pfTmpPowerY = nSrcY + 1.f - fSrcY;
		*pfTmpPowerXY = *pfTmpPowerX * *pfTmpPowerY;
		nSrcX = (nSrcX <= 0) ? 0 : nSrcX;
		nSrcX = (nSrcX >= nScaleWidth) ? nScaleWidth : nSrcX;
		nSrcY = (nSrcY <= 0) ? 0 : nSrcY;
		nSrcY = (nSrcY >= nScaleHeight) ? nScaleHeight : nSrcY;
		pdwTmpSrcIndex[0] = nSrcX + nSrcY * nSrcBufWidth;
		pdwTmpSrcIndex[1] = (nSrcX >= nScaleWidth) ? pdwTmpSrcIndex[0] : (pdwTmpSrcIndex[0] + 1);
		pdwTmpSrcIndex[2] = (nSrcY >= nScaleHeight) ? pdwTmpSrcIndex[0] : (pdwTmpSrcIndex[0] + nSrcBufWidth);
		pdwTmpSrcIndex[3] = (nSrcY >= nScaleHeight) ? pdwTmpSrcIndex[1] : (pdwTmpSrcIndex[1] + nSrcBufWidth);
		pdwTmpSrcIndex += 4;
		pfTmpPowerX++;
		pfTmpPowerY++;
		pfTmpPowerXY++;
		fU += fDU;
		fV += fDV;
		fW += fDW;
	}
}

void BufferMemSSE::Buffer_Trans_Interpolate_BL(OUT DWORD * pdwDesLeftEdge, IN int nLinePixels, IN DWORD ** ppdwSrcStart, IN DWORD * pdwSrcIndex, IN float * pfPowerX, IN float * pfPowerY, IN float * pfPowerXY)
{
	float *pfTmpPowerX = pfPowerX, *pfTmpPowerY = pfPowerY, *pfTmpPowerXY = pfPowerXY;
	DWORD* pdwTmpSrcIndex = pdwSrcIndex, *pdwSrcStart = ppdwSrcStart[0], *pdwTmpDes = pdwDesLeftEdge;
	DWORD *pdwSrc00 = NULL, *pdwSrc01 = NULL, *pdwSrc10 = NULL, *pdwSrc11 = NULL;
	int nTmp00(0), nTmp01(0), nTmp10(0), nTmp11(0), nR(0), nG(0), nB(0);
	int nA00(0), nA01(0), nA10(0), nA11(0), nA(0);

	// 计算公式		S00 * x * y				S01 * (1 - x) * y
	//				S10 * x * (1 - y)		S11 * (1 - x) * (1 - y)
	// D = S11 + (S00 - S01 - S10 + S11) * x * y + (S10 - S11) * x + (S01 - S11) * y
	// 要特殊处理边缘问题，防止透明像素影响边缘的颜色，透明像素只能影响边缘的透明度。
	for (int i = 0; i<nLinePixels; i++)
	{
		pdwSrc00 = pdwTmpSrcIndex[0] + pdwSrcStart;
		pdwSrc01 = pdwTmpSrcIndex[1] + pdwSrcStart;
		pdwSrc10 = pdwTmpSrcIndex[2] + pdwSrcStart;
		pdwSrc11 = pdwTmpSrcIndex[3] + pdwSrcStart;
		// A
		nA00 = (int)(((*pdwSrc00) & 0xff000000) >> 24);
		nA01 = (int)(((*pdwSrc01) & 0xff000000) >> 24);
		nA10 = (int)(((*pdwSrc10) & 0xff000000) >> 24);
		nA11 = (int)(((*pdwSrc11) & 0xff000000) >> 24);
		nA = (int)(nA11 + (nA00 - nA01 - nA10 + nA11) * *pfTmpPowerXY
			+ (nA10 - nA11) * *pfTmpPowerX + (nA01 - nA11) * *pfTmpPowerY);
		nA = (nA < 0) ? 0 : nA;
		// B
		nTmp00 = (int)((*pdwSrc00) & 0x000000ff);
		nTmp01 = (int)((*pdwSrc01) & 0x000000ff);
		nTmp10 = (int)((*pdwSrc10) & 0x000000ff);
		nTmp11 = (int)((*pdwSrc11) & 0x000000ff);
		//////////////////////////////////////////////////////////////////////////
		// 先进行列交换
		if (nA00 == 0 && nA01 == 0)
		{
			nTmp00 = nTmp10;
			nTmp01 = nTmp11;
		}
		if (nA10 == 0 && nA11 == 0)
		{
			nTmp10 = nTmp00;
			nTmp11 = nTmp01;
		}
		nTmp00 = (nA00 == 0) ? nTmp01 : nTmp00;
		nTmp01 = (nA01 == 0) ? nTmp00 : nTmp01;
		nTmp10 = (nA10 == 0) ? nTmp11 : nTmp10;
		nTmp11 = (nA11 == 0) ? nTmp10 : nTmp11;
		//////////////////////////////////////////////////////////////////////////
		nB = (int)(nTmp11 + (nTmp00 - nTmp01 - nTmp10 + nTmp11) * *pfTmpPowerXY
			+ (nTmp10 - nTmp11) * *pfTmpPowerX + (nTmp01 - nTmp11) * *pfTmpPowerY);
		nB = (nB < 0) ? 0 : nB;
		// G
		nTmp00 = (int)(((*pdwSrc00) & 0x0000ff00) >> 8);
		nTmp01 = (int)(((*pdwSrc01) & 0x0000ff00) >> 8);
		nTmp10 = (int)(((*pdwSrc10) & 0x0000ff00) >> 8);
		nTmp11 = (int)(((*pdwSrc11) & 0x0000ff00) >> 8);
		//////////////////////////////////////////////////////////////////////////
		// 先进行列交换
		if (nA00 == 0 && nA01 == 0)
		{
			nTmp00 = nTmp10;
			nTmp01 = nTmp11;
		}
		if (nA10 == 0 && nA11 == 0)
		{
			nTmp10 = nTmp00;
			nTmp11 = nTmp01;
		}
		nTmp00 = (nA00 == 0) ? nTmp01 : nTmp00;
		nTmp01 = (nA01 == 0) ? nTmp00 : nTmp01;
		nTmp10 = (nA10 == 0) ? nTmp11 : nTmp10;
		nTmp11 = (nA11 == 0) ? nTmp10 : nTmp11;
		//////////////////////////////////////////////////////////////////////////
		nG = (int)(nTmp11 + (nTmp00 - nTmp01 - nTmp10 + nTmp11) * *pfTmpPowerXY
			+ (nTmp10 - nTmp11) * *pfTmpPowerX + (nTmp01 - nTmp11) * *pfTmpPowerY);
		nG = (nG < 0) ? 0 : nG;
		// R
		nTmp00 = (int)(((*pdwSrc00) & 0x00ff0000) >> 16);
		nTmp01 = (int)(((*pdwSrc01) & 0x00ff0000) >> 16);
		nTmp10 = (int)(((*pdwSrc10) & 0x00ff0000) >> 16);
		nTmp11 = (int)(((*pdwSrc11) & 0x00ff0000) >> 16);
		//////////////////////////////////////////////////////////////////////////
		// 先进行列交换
		if (nA00 == 0 && nA01 == 0)
		{
			nTmp00 = nTmp10;
			nTmp01 = nTmp11;
		}
		if (nA10 == 0 && nA11 == 0)
		{
			nTmp10 = nTmp00;
			nTmp11 = nTmp01;
		}
		nTmp00 = (nA00 == 0) ? nTmp01 : nTmp00;
		nTmp01 = (nA01 == 0) ? nTmp00 : nTmp01;
		nTmp10 = (nA10 == 0) ? nTmp11 : nTmp10;
		nTmp11 = (nA11 == 0) ? nTmp10 : nTmp11;
		//////////////////////////////////////////////////////////////////////////
		nR = (int)(nTmp11 + (nTmp00 - nTmp01 - nTmp10 + nTmp11) * *pfTmpPowerXY
			+ (nTmp10 - nTmp11) * *pfTmpPowerX + (nTmp01 - nTmp11) * *pfTmpPowerY);
		nR = (nR < 0) ? 0 : nR;
		// 组合
		*pdwTmpDes = ((nA << 24) | (nR << 16) | (nG << 8) | nB);
		pdwTmpSrcIndex += 4;
		pfTmpPowerX++;
		pfTmpPowerY++;
		pfTmpPowerXY++;
		pdwTmpDes++;
	}
}
