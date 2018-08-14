#include "StdAfx.h"
#include "Common\ColorAPI.h"
#include "Common\BufferFunction.h"
#include <xmmintrin.h>		// supports SSE instructions
#include <tchar.h>
#include <assert.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// 缺省颜色初始化定义
// 单色
#define DEFAULT_HOMO_COLOR				0x7f7f7f7f
// 渐变色
#define DEFAULT_GRAD_PEG_COUNT_2		2				// 渐变块个数
#define DEFAULT_GRAD_PEG_COUNT_4		4
#define DEFAULT_GRAD_RAPEAT			1				// 频率
#define DEFAULT_GRAD_START_COLOR		0x7f000000		// 起点颜色
#define DEFAULT_GRAD_END_COLOR			0xffffffff		// 终点颜色
#define DEFAULT_GRAD_START_POS			0.f				// 起点位置
#define DEFAULT_GRAD_POS_1_4			0.25f			// 1/4 位置
#define DEFAULT_GRAD_POS_3_4			0.75f			// 3/4 位置
#define DEFAULT_GRAD_END_POS			1.f				// 终点位置
#define DEFAULT_GRAD_MID				0.5f				// 中点位置
#define DFFAULT_GRAD_LINEAR_RADIAN		0.f				// 角度
#define DEFAULT_GRAD_CENTER_XY			0.5f				// 中心位置
#define DEFAULT_GRAD_XY_MIN			0.f				// xy 最小值
#define DEFAULT_GRAD_XY_MAX			1.f				// xy 最大值

// 线性渐变色权重放大系数
#define DEFAULT_POWER_SCALE			16384.f
#define DEFAULT_POWER_SCALE_HALF		8192.f
// 渐变色一行渲染长度的缩放倍数
#define CENTER_ROUND_LINEAR_SCALING		2.f
#define OUTLINE_CIRCLE_LINEAR_SCALING	1.f
#define OUTLINE_POLYGON_LINEAR_SCALING	2.f

// Define the relative path.
#define DEFAULT_TEMPLET_NAME			_T("varycolor")
#define DEFAULT_IMG_TEX_NAME			_T("ImgTexFiles")
#define DEFAULT_LIB_EXT_NAME			_T(".dat")

// some common operations
#define SAFEDELETE_ARRAY(p)			if(p!=NULL){delete[] p; p=NULL;}


HANDLE		g_hModule = NULL;			// the handle of the module
BOOL		g_bInit = FALSE;			// the flag of initialization
LPTSTR		g_szTempletPath = NULL;	// the working directory of the templet files
LPTSTR		g_szImagePath = NULL;		// the working directory of the image texture files


// Add file for templet gradual color or image texture.
SYCOLOR_RESULT SYColor_AddFile(IN LPCTSTR szFilePath,		// file path
							   IN int nIconWidth,			// icon size
							   IN int nIconHeight,
							   OUT int *pnIndex,			// index of the new file
							   IN int nType);				// type of color
// Delete file for templet gradual color or image texture.
SYCOLOR_RESULT SYColor_DeleteFile(IN int nIndex,		// index of the file in the library
								  IN int nType);		// type of color

// 颜色动画处理
void SYColor_ProcAniColor(IN OUT DWORD* pdwLine,
						  IN int nWidth,
						  IN int nFrame,
						  IN float fSpeed,
						  IN DWORD dwStyle);

// 初始化颜色路径
BASEGRAPH_COMMON_API SYCOLOR_RESULT SYColor_InitEnv(LPCTSTR szBinPath)
{
	//if (g_bInit)
	//	return SYCOLOR_RET_INITED;

	//TCHAR szColorPath[MAX_PATH];
	//int nColorPathLen(0), nRetaivePathLen(0);

	//szColorPath[0] = _T('\0');
	//if (szBinPath == NULL)
	//{
	//	LPCTSTR szBinPath = SYPath_GetPath(SY_PATH_BIN);
	//	wcscpy_s(szColorPath,MAX_PATH,szBinPath);
	//}
	//else
	//{
	//	_tcscpy(szColorPath, szBinPath);
	//}
	//_tcscat(szColorPath, _T("\\CGSys\\"));
	//CreateDirectory(szColorPath,NULL);

	//nColorPathLen = _tcslen(szColorPath);
	//nRetaivePathLen = _tcslen(DEFAULT_TEMPLET_NAME) + 1;
	//g_szTempletPath = new TCHAR[nColorPathLen + nRetaivePathLen + 1];
	//g_szTempletPath[0] = _T('\0');
	//_tcscpy(g_szTempletPath, szColorPath);
	//_tcscat(g_szTempletPath, DEFAULT_TEMPLET_NAME);
	//_tcscat(g_szTempletPath, _T("\\"));
	//CreateDirectory(g_szTempletPath,NULL);
	//g_bInit = TRUE;

	return SYCOLOR_RET_SUCCESS;
}

BASEGRAPH_COMMON_API SYCOLOR_RESULT SYColor_ReleaseEnv()
{
	if (!g_bInit)
		return SYCOLOR_RET_NOT_INIT;

	SAFEDELETE_ARRAY(g_szTempletPath);
	//	g_libTemplet.CloseLib();
	SAFEDELETE_ARRAY(g_szImagePath);
	//	g_libImage.CloseLib();
	g_bInit = FALSE;

	return SYCOLOR_RET_SUCCESS;
}

// 拷贝颜色
BASEGRAPH_COMMON_API void SYColor_CopyColor(OUT SYColor* pDes,	// 目标
								   IN SYColor* pSrc)	// 源
{
	SYMemcpy_Line(pDes, pSrc, sizeof(SYColor));
}

BASEGRAPH_COMMON_API DWORD SYColor_GetRGBColor(IN SYColor* pColor)
{
	switch(pColor->type)
	{
	case SYCOLOR_NONE:
	case SYCOLOR_HOMO:
		return (DWORD)pColor->color;
    case SYCOLOR_GRAD_LINEAR:
    case SYCOLOR_GRAD_CENTER_ROUND:
    case SYCOLOR_GRAD_CENTER_POLYGON:
    case SYCOLOR_GRAD_OUTLINE_CIRCLE:
    case SYCOLOR_GRAD_OUTLINE_POLYGON:
    case SYCOLOR_GRAD_PT_CONTOUR:
    case SYCOLOR_GRAD_PT_BLEND:
    //case SYCOLOR_GRAD_TEMPLET:
        return (DWORD)pColor->gradual.color[0];
	default:
		return 0xffffffff;
	}
}
// 填充颜色
BASEGRAPH_COMMON_API void SYColor_FillColor(IN SYColor* pColor,			// 颜色
								   OUT DWORD* pdwBuf,			// 填充对象
								   IN int nBufWidth,			// 对象大小
								   IN int nBufHeight,
								   IN int nX,					// 填充区域
								   IN int nY,
								   IN int nWidth,
								   IN int nHeight,
								   IN float fScale,
								   IN int nFrame/* = 0*/,		// 颜色动画的帧号，于静态颜色无用
								   IN BYTE* pbyMask/* = NULL*/,	// 掩膜（8位缓存），大小与填充区域相同
								   IN int nMaskPitch/* = 0*/)	// 掩膜缓存的特殊性，pitch可能不等于宽度
{
	switch(pColor->type)
	{
	case SYCOLOR_NONE:
	case SYCOLOR_HOMO:
		SYColor_FillHomo(pColor, pdwBuf, nBufWidth, nBufHeight, nX, nY, nWidth, nHeight, nFrame, pbyMask, nMaskPitch);
		break;
	case SYCOLOR_GRAD_LINEAR:
		SYColor_FillGradLinear(pColor, pdwBuf, nBufWidth, nBufHeight, nX, nY, nWidth, nHeight, nFrame, pbyMask, nMaskPitch);
		break;
	case SYCOLOR_GRAD_CENTER_ROUND:
		SYColor_FillGradCenterRound(pColor, pdwBuf, nBufWidth, nBufHeight, nX, nY, nWidth, nHeight, nFrame, pbyMask, nMaskPitch);
		break;
	case SYCOLOR_GRAD_CENTER_POLYGON:
		SYColor_FillGradCenterPolygon(pColor, pdwBuf, nBufWidth, nBufHeight, nX, nY, nWidth, nHeight, nFrame, pbyMask, nMaskPitch);
		break;
	case SYCOLOR_GRAD_OUTLINE_CIRCLE:
		SYColor_FillGradOutlineCircle(pColor, pdwBuf, nBufWidth, nBufHeight, nX, nY, nWidth, nHeight, nFrame, pbyMask, nMaskPitch);
		break;
	case SYCOLOR_GRAD_OUTLINE_POLYGON:
		SYColor_FillGradOutlinePolygon(pColor, pdwBuf, nBufWidth, nBufHeight, nX, nY, nWidth, nHeight, nFrame, pbyMask, nMaskPitch);
		break;
	case SYCOLOR_GRAD_PT_CONTOUR:
		SYColor_FillGradPtContour(pColor, pdwBuf, nBufWidth, nBufHeight, nX, nY, nWidth, nHeight, nFrame, pbyMask, nMaskPitch);
		break;
	case SYCOLOR_GRAD_PT_BLEND:
		SYColor_FillGradPtBlend(pColor, pdwBuf, nBufWidth, nBufHeight, nX, nY, nWidth, nHeight, nFrame, pbyMask, nMaskPitch);
		break;
	case SYCOLOR_GRAD_TEMPLET:
		SYColor_FillGradTemplet(pColor, pdwBuf, nBufWidth, nBufHeight, nX, nY, nWidth, nHeight, nFrame, pbyMask, nMaskPitch);
		break;
	case SYCOLOR_TEX_IMG:
		SYColor_FillTexImg(pColor, pdwBuf, nBufWidth, nBufHeight, nX, nY, nWidth, nHeight, fScale,nFrame, pbyMask, nMaskPitch);
		break;
	default:
		break;
		//assert(FALSE);
	}
}
// 计算渐变色的动画参数，如果有动画则返回TRUE。
BASEGRAPH_COMMON_API BOOL SYColor_CalcAniParam(OUT SYColor *pNewColor,	// 根据动画参数计算得到的静态颜色参数
									  IN SYColor* pSrcColor,	// 动画颜色参数
									  IN int nFrame)			// 动画帧号
{
	//assert(pNewColor != NULL && pSrcColor != NULL);

	DWORD dwValidType = GRAD_ANI_NONE;

	switch (pSrcColor->type)
	{
	case SYCOLOR_GRAD_LINEAR:
		// 有效的动画类型
		dwValidType = (GRAD_ANI_COLOR | GRAD_ANI_LINEAR_RAD | GRAD_ANI_REPEAT);
		if ((pSrcColor->gradual.animation & dwValidType)== GRAD_ANI_NONE)
			break;

		SYMemcpy_Line(pNewColor, pSrcColor, sizeof(SYColor));
		pNewColor->gradual.animation = GRAD_ANI_NONE;
		if ((pSrcColor->gradual.animation & GRAD_ANI_LINEAR_RAD) != GRAD_ANI_NONE)
		{
			pNewColor->gradual.linear.frad += nFrame * pSrcColor->gradual.aniparam[GRAD_ANI_PARAM_LINEAR_RAD];
		}
		return TRUE;
	default:
		break;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
// 单色

// 创建
BASEGRAPH_COMMON_API void SYColor_CreateHomo(OUT SYColor* pColor,
									IN unsigned long color)
{
	//assert(pColor != NULL);

	SYMemset_LineChar(pColor, 0, sizeof(SYColor));
	pColor->type = SYCOLOR_HOMO;
	pColor->color = color;
}

// 填充
void SYColor_FillHomo(IN SYColor* pColor,			// 填充颜色
					  OUT DWORD* pdwBuf,			// 填充对象
					  IN int nBufWidth,				// 对象大小
					  IN int nBufHeight,
					  IN int nX,					// 填充区域
					  IN int nY,
					  IN int nWidth,
					  IN int nHeight,
					  IN int nFrame/* = 0*/,		// 颜色动画的帧号，于静态颜色无用
					  IN BYTE* pbyMask/* = NULL*/,	// 掩膜（8位缓存），大小与填充区域相同
					  IN int nMaskPitch/* = 0*/)	// 掩膜缓存的特殊性，pitch可能不等于宽度
{
	//assert((pColor != NULL) && (pdwBuf != NULL));

	int nminw = min(nBufWidth - nX, nWidth);
	int nminh = min(nBufHeight - nY, nHeight);
	DWORD* pdwFillBuf = pdwBuf + nY * nBufWidth + nX;

	for(int i=0; i<nminh; i++)
	{
		SYMemset_LineDWORD(pdwFillBuf, pColor->color, nminw);
		pdwFillBuf += nBufWidth;
	}
}

// 填充
void SYColor_FillGradLinear(IN SYColor* pColor,				// 填充颜色
							OUT DWORD* pdwBuf,				// 填充对象
							IN int nBufWidth,				// 对象大小
							IN int nBufHeight,
							IN int nX,						// 填充区域
							IN int nY,
							IN int nWidth,
							IN int nHeight,
							IN int nFrame/* = 0*/,			// 颜色动画的帧号，于静态颜色无用
							IN BYTE* pbyMask/* = NULL*/,	// 掩膜（8位缓存），大小与填充区域相同
							IN int nMaskPitch/* = 0*/)		// 掩膜缓存的特殊性，pitch可能不等于宽度
{
	//assert((pColor != NULL) && (pdwBuf != NULL));
	printf("pColor->gradual.color = 0x%02x\n\
		   pColor->gradual.pos = %f\n\
		   pColor->gradual.mid = %f\n\
		   pColor->gradual.ncount =%d\n\
		   pColor->gradual.linear.repeat = %d\n\
		   pColor->gradual.linear.style = %d\n\
		   pColor->gradual.linear.frad = %f\n",
		   pColor->gradual.color,
		   pColor->gradual.pos,
		   pColor->gradual.mid,
		   pColor->gradual.ncount,
		   pColor->gradual.linear.repeat,
		   pColor->gradual.linear.style,
		   pColor->gradual.linear.frad);
	SYColor crStatic, *pcrFill = pColor;		// 用于动画颜色
	int nminw = min(nBufWidth - nX, nWidth);
	int nminh = min(nBufHeight - nY, nHeight);
	DWORD* pdwFillBuf = pdwBuf + nY * nBufWidth + nX;
	int nAngle(0);

	// 如果有动画，则使用计算得到的静态颜色参数进行填充。
	if (SYColor_CalcAniParam(&crStatic, pColor, nFrame))
	{
		pcrFill = &crStatic;
	}
	// 计算角度并规则化到值域 [0, 360)
	nAngle = SY_ClampAngle_PI_2(SY_RadianToAngle(pcrFill->gradual.linear.frad));
	if (nAngle == 0 || nAngle == 180)	// 水平
	{
		DWORD* pdwTmpLine = pdwFillBuf;
		int nPitch = (nminw << 2);

		// 填充一行
		SYColor_FillLineBaseAR_1_Recursive(pdwTmpLine, nminw, pcrFill->gradual.color,
			pcrFill->gradual.pos, pcrFill->gradual.mid, pcrFill->gradual.ncount,
			pcrFill->gradual.linear.repeat, pcrFill->gradual.linear.style);
		// 节点颜色动画的测试版
		if ((pColor->gradual.animation & GRAD_ANI_COLOR) != GRAD_ANI_NONE)
		{
			SYColor_ProcAniColor(pdwTmpLine, nminw, nFrame, pColor->gradual.aniparam[GRAD_ANI_PARAM_COLOR], pcrFill->gradual.linear.style);
		}
		// 反向则翻转内存
		if (nAngle == 180)
		{
			SYMemrev_LineDWORD(pdwTmpLine, nminw);
		}
		// 填充其它行
		for(int i=1; i<nminh; i++)
		{
			pdwTmpLine += nBufWidth;
			SYMemcpy_Line(pdwTmpLine, pdwFillBuf, nPitch);
		}
	}
	else if (nAngle == 90 || nAngle == 270)		// 垂直
	{
		DWORD *pdwTmpVert = SYSSE_NEW(nminh, DWORD), *pdwTmpPixel = NULL;
		int nStep = (nAngle == 90) ? -1 : 1;

		// 填充一列
		SYColor_FillLineBaseAR_1_Recursive(pdwTmpVert, nminh, pcrFill->gradual.color,
			pcrFill->gradual.pos, pcrFill->gradual.mid, pcrFill->gradual.ncount,
			pcrFill->gradual.linear.repeat, pcrFill->gradual.linear.style);
		// 节点颜色动画的测试版
		if ((pColor->gradual.animation & GRAD_ANI_COLOR) != GRAD_ANI_NONE)
		{
			SYColor_ProcAniColor(pdwTmpVert, nminh, nFrame, pColor->gradual.aniparam[GRAD_ANI_PARAM_COLOR], pcrFill->gradual.linear.style);
		}
		// 根据方向进行水平填充
		pdwTmpPixel = (nAngle == 90) ? (pdwTmpVert + nminh - 1) : pdwTmpVert;
		for (int i=0; i<nminh; i++)
		{
			SYMemset_LineDWORD(pdwFillBuf, *pdwTmpPixel, nminw);
			pdwFillBuf += nBufWidth;
			pdwTmpPixel += nStep;
		}
		SYSSE_FREE(pdwTmpVert);
	}
	else	// 倾斜，则需要根据倾斜度来决定是宽度主打还是高度主打
	{
		float fSlope = tanf(pcrFill->gradual.linear.frad);
		float fAbsSlope = fabsf(fSlope), fStart(0.f);
		int nRealLen(0), nPitch(0);
		DWORD *pdwRealLine = NULL;

		if (fAbsSlope > 1.f)	// 与水平轴的夹角超过 45 度，高度主打
		{
			BOOL bUpDown = TRUE;		// 列从上往下拷贝的标志
			int nStep(1);				// 列在行内前进的方向

			nPitch = (nBufWidth << 2);		// 缓存的长度
			fAbsSlope = 1.f / fAbsSlope;
			fStart = nminw * fAbsSlope;
			nRealLen = nminh + (int)ceilf(fStart);
			pdwRealLine = SYSSE_NEW(nRealLen, DWORD);
			// 填充一行

			SYColor_FillLineBaseAR_1_Recursive(pdwRealLine, nRealLen, pcrFill->gradual.color,
				pcrFill->gradual.pos, pcrFill->gradual.mid, pcrFill->gradual.ncount,
				pcrFill->gradual.linear.repeat, pcrFill->gradual.linear.style);
			// 节点颜色动画的测试版
			if ((pColor->gradual.animation & GRAD_ANI_COLOR) != GRAD_ANI_NONE)
			{
				SYColor_ProcAniColor(pdwRealLine, nRealLen, nFrame, pColor->gradual.aniparam[GRAD_ANI_PARAM_COLOR], pcrFill->gradual.linear.style);
			}
			// 根据角度所在的位置判断是否需要进行翻转
			if (nAngle < 90)	// 一象限从最后一行的第一列开始
			{
				pdwFillBuf += ((nminh - 1) * nBufWidth);
				bUpDown = FALSE;
			}
			else if (nAngle < 180)	// 二象限从最后一行的最后一列开始
			{
				pdwFillBuf += ((nminh - 1) * nBufWidth + nminw - 1);
				nStep = -1;
				bUpDown = FALSE;
			}
			else if (nAngle < 270)	// 三象限从第一行的最后一列开始
			{
				pdwFillBuf += (nminw - 1);
				nStep = -1;
			}
			// 四象限则从第一行的第一列开始
			fStart = 0.f;
			fSlope = fAbsSlope;
			// 计算每一列的起点位置
			for (int i=0; i<nminw; i++)
			{
				SYMemcpy_ColumnDWORD(pdwFillBuf, nminh, nPitch, (pdwRealLine + (int)fStart), bUpDown);
				pdwFillBuf += nStep;
				fStart += fSlope;
			}
		}
		else	// 否则贴近水平轴，宽度主打
		{
			nPitch = (nminw << 2);		// 填充的长度
			fStart = nminh * fAbsSlope;
			nRealLen = nminw + (int)ceilf(fStart);
			pdwRealLine = SYSSE_NEW(nRealLen, DWORD);
			// 填充一行
			SYColor_FillLineBaseAR_1_Recursive(pdwRealLine, nRealLen, pcrFill->gradual.color,
				pcrFill->gradual.pos, pcrFill->gradual.mid, pcrFill->gradual.ncount,
				pcrFill->gradual.linear.repeat, pcrFill->gradual.linear.style);
			// 节点颜色动画的测试版
			if ((pColor->gradual.animation & GRAD_ANI_COLOR) != GRAD_ANI_NONE)
			{
				SYColor_ProcAniColor(pdwRealLine, nRealLen, nFrame, pColor->gradual.aniparam[GRAD_ANI_PARAM_COLOR], pcrFill->gradual.linear.style);
			}
			// 根据角度所在的位置判断是否需要进行翻转
			if (nAngle > 90 && nAngle < 270)	// 在二三象限内
			{
				SYMemrev_LineDWORD(pdwRealLine, nRealLen);
			}
			// 计算出渐变色移动的方向，一三象限从右往左移，二四象限从左往右移
			if (fSlope < 0.f)
			{
				fStart = 0.f;
			}
			fSlope = -fSlope;
			// 计算每一行的起点位置
			for (int i=0; i<nminh; i++)
			{
				SYMemcpy_Line(pdwFillBuf, (pdwRealLine + (int)fStart), nPitch);
				pdwFillBuf += nBufWidth;
				fStart += fSlope;
			}
		}
		SYSSE_SAFEFREE(pdwRealLine);
	}
}
// 采用浮点颜色递归运算进行填充
// （1）在中间点左边时（x < m）：0.5*(c1-c0)/(m-p0) * (x-p0) + c0 = A1 * (x - B1) + C1
// （2）在中间点右边时（y > m）：0.5*(c1-c0)/(p1-m) * (y-p1) + c1 = A2 * (y - B2) + C2
void SYColor_FillLineBaseAR_1_Recursive(OUT DWORD* pdwBuf,		// 填充对象
									IN int nBufWidth,
									IN DWORD *pdwColor,		// 单色组
									IN float *pfPos,		// 位置组
									IN float *pfMid,		// 中间点组
									IN int nCount,			// 节点个数
									IN DWORD dwRepeat,		// 重复频率
									IN DWORD dwStyle)		// 重复方式
{
	//assert((pdwBuf != NULL) && (pdwColor != NULL) && (pfPos != NULL) && (pfMid != NULL));
	//assert(nCount > 0);

	int nP0(0), nP1(0), nPm(0);
	float fP0(0.f), fP1(0.f), fPm(0.f), fCoefA(0.f);
	float fSectLen = (float)nBufWidth / (float)dwRepeat;	// 一段的长度
	float fCrP0[4] = { 0.f }, fCrP1[4] = { 0.f };
	DWORD dwCrP0(0), dwCrP1(0);		// 节点颜色
	DWORD* pdwTmpBuf = pdwBuf;

	// 第一段的内存填充，如果重复，则后面的段直接进行拷贝，并特殊处理两段之间的渐变填充
	// 第一个节点的左边完全填充为第一个节点的颜色
	fP0 = pfPos[0] * fSectLen;
	nP0 = (int)ceilf(fP0);		// 浮点位置所在像素的下一个索引
	dwCrP0 = pdwColor[0];
	SYMemset_LineDWORD(pdwTmpBuf, dwCrP0, nP0);
	pdwTmpBuf += nP0;
	// 起点颜色
	fCrP0[0] = (float)GetBGRA_B(dwCrP0);
	fCrP0[1] = (float)GetBGRA_G(dwCrP0);
	fCrP0[2] = (float)GetBGRA_R(dwCrP0);
	fCrP0[3] = (float)GetAValue(dwCrP0);
	// 节点之间按照中间点的位置进行插值
	for (int i=1; i<nCount; i++)
	{
		fP1 = pfPos[i] * fSectLen;
		nP1 = (int)ceilf(fP1);		// 浮点位置所在像素的下一个索引
		dwCrP1 = pdwColor[i];
		if (dwCrP1 == dwCrP0)		// 两个节点的颜色相同，则不需要进行插值
		{
			// 防止出现两个位置颠倒情况
			int nLen = max(0, nP1 - nP0);
			SYMemset_LineDWORD(pdwTmpBuf, dwCrP0, nLen);
		}
		else	// 按中间点将段分成前后两部分进行插值
		{
			// 终点颜色
			fCrP1[0] = (float)GetBGRA_B(dwCrP1);
			fCrP1[1] = (float)GetBGRA_G(dwCrP1);
			fCrP1[2] = (float)GetBGRA_R(dwCrP1);
			fCrP1[3] = (float)GetAValue(dwCrP1);
			if (nP1 > nP0)		// 起点与终点在像素位置上不重合
			{
				if (pfMid[i-1] == 0.5f)		// 中间点绝对没有移动时，则可直接进行插值
				{
					fCoefA = 1.f / (fP1 - fP0);
					SYBGRA32_LineBaseAR_Recursive(pdwTmpBuf, fCrP0, fCrP1, (float)nP0, fCoefA, fP0, fCrP0, nP1 - nP0);
				}
				else	// 否则要分成两段
				{
					// 计算中间点的绝对位置
					fPm = pfMid[i - 1] * (fP1 - fP0) + fP0;
					nPm = (int)ceilf(fPm);		// 浮点位置所在像素的下一个索引
					if (nPm > nP0)	// 中间点不与起点重合
					{
						fCoefA = 0.5f / (fPm - fP0);
						SYBGRA32_LineBaseAR_Recursive(pdwTmpBuf, fCrP0, fCrP1, (float)nP0, fCoefA, fP0, fCrP0, nPm - nP0);
					}
					if (nP1 > nPm)	// 中间点不与终点重合
					{
						fCoefA = 0.5f / (fP1 - fPm);
						SYBGRA32_LineBaseAR_Recursive((pdwTmpBuf + nPm - nP0), fCrP0, fCrP1, (float)nPm, fCoefA, fP1, fCrP1, nP1 - nPm);
					}
				}
			}
			// 需要交换颜色数据
			dwCrP0 = dwCrP1;
			SYMemcpy_Line(fCrP0, fCrP1, (sizeof(float) << 2));
		}
		// 交换位置，进行下一次的递归
		pdwTmpBuf += (nP1 - nP0);
		nP0 = nP1;
		fP0 = fP1;
	}
	if (dwRepeat > 1)	// 重复第一段，需要考虑重复的方式
	{
		float fStartPos = pfPos[0] * fSectLen;		// 第一段的起点
		int nStartPos = (int)ceilf(fStartPos);		// 浮点位置所在像素的下一个索引
		float fEndPos = fP0;			// 第一段的最后一个节点的绝对位置
		int nLastEndPos = nP0;			// 浮点位置所在像素的下一个索引
		float fFirstLen = fEndPos - fStartPos;		// 第一段浮点长度
		int nFirstLen = nLastEndPos - nStartPos;		// 第一段像素长度
		DWORD* pdwFirst = pdwBuf + nStartPos;			// 第一段起点
		int nCurrEndPos(0), nCurrLen(0);
		BOOL bRevCopy = FALSE;
		float *pfCrLastEnd = fCrP1, *pfCrCurrStart = fCrP0;		// 上一段终点与当前段起点的颜色
		DWORD *pdwCrLastEnd = &dwCrP1, *pdwCrCurrStart = &dwCrP0;

		// 当前 fCrP1 里肯定存放着第一段终点的颜色，只需填充 fCrP0 为第一段的起点颜色即可
		dwCrP0 = pdwColor[0];
		fCrP0[0] = (float)GetBGRA_B(dwCrP0);
		fCrP0[1] = (float)GetBGRA_G(dwCrP0);
		fCrP0[2] = (float)GetBGRA_R(dwCrP0);
		fCrP0[3] = (float)GetAValue(dwCrP0);
		// 根据排序方式进行重复
		for (DWORD i=1; i<dwRepeat; i++)
		{
			fStartPos += fSectLen;		// 顺序排列的起点与终点
			fEndPos += fSectLen;
			if ((dwStyle == SYCOLOR_STYLE_SYMMETRY) && (i & 0x01))	// 对称排列且处于对称状态
			{
				// 翻转起点与终点，这一段的起点也即空隙的终点取最后一个节点
				fP1 = fEndPos - fFirstLen;
				nStartPos = (int)ceilf(fEndPos - fFirstLen);
				nCurrEndPos = (int)ceilf(fStartPos + fFirstLen);
				pdwCrCurrStart = &dwCrP1;
				pfCrCurrStart = fCrP1;
				// 设置反向拷贝的标志
				bRevCopy = TRUE;
			}
			else	// 其它的都按照顺序排列来处理
			{
				// 这一段的起点也即空隙的终点取第一个节点
				fP1 = fStartPos;
				nStartPos = (int)ceilf(fStartPos);
				nCurrEndPos = (int)ceilf(fEndPos);
				pdwCrCurrStart = &dwCrP0;
				pfCrCurrStart = fCrP0;
			}
			// 防止内存越界
			nCurrEndPos = (nCurrEndPos > nBufWidth) ? nBufWidth : nCurrEndPos;
			// 判断当前段的起点是否在上一段的终点之后（只会相差一个像素）
			nStartPos = (nStartPos < nLastEndPos) ? nLastEndPos : nStartPos;
			nCurrLen = (nCurrEndPos > nStartPos) ? (nCurrEndPos - nStartPos) : 0;
			if (nCurrLen > nFirstLen)	// 如果当前段比第一段长，则往后挪起点
			{
				nStartPos += (nCurrLen - nFirstLen);	// 只会相差一个像素
				nCurrLen = nFirstLen;
			}
			if (nStartPos > nLastEndPos)	// 当前段的起点与上一段的终点之间存在空隙，进行渐变填充
			{
				pdwTmpBuf = pdwBuf + nLastEndPos;			// 空隙缓存起点
				if (*pdwCrLastEnd == *pdwCrCurrStart)	// 前后点颜色相同则直接填充
				{
					SYMemset_LineDWORD(pdwTmpBuf, *pdwCrCurrStart, nStartPos - nLastEndPos);
				}
				else	// 颜色不同则插值
				{
					fCoefA = 1.f / (fP1 - fP0);
					SYBGRA32_LineBaseAR_Recursive(pdwTmpBuf, pfCrLastEnd, pfCrCurrStart, (float)nLastEndPos, fCoefA, fP0, pfCrLastEnd, nStartPos - nLastEndPos);
				}
			}
			// 重复第一段
			pdwTmpBuf = pdwBuf + nStartPos;
			if (bRevCopy)		// 对称
			{
				SYMemcpy_RevDWORD(pdwTmpBuf, pdwFirst, nCurrLen);
				pdwCrLastEnd = &dwCrP0;
				pfCrLastEnd = fCrP0;
				bRevCopy = FALSE;
			}
			else	// 顺序
			{
				SYMemcpy_Line(pdwTmpBuf, pdwFirst, (nCurrLen << 2));
				pdwCrLastEnd = &dwCrP1;
				pfCrLastEnd = fCrP1;
			}
			// 交换数据
			fP0 = fP1;
			nLastEndPos = nCurrEndPos;
		}
		// 记录最后一个节点的位置以确保与不重复的情况保持兼容
		nP0 = nLastEndPos;
		pdwTmpBuf = pdwBuf + nP0;
		dwCrP0 = *pdwCrLastEnd;
	}
	// 最后一个节点的右边完全填充为最后一个节点的颜色
	SYMemset_LineDWORD(pdwTmpBuf, dwCrP0, nBufWidth - nP0);
}

// 填充
// 圆形中心渐变色
void SYColor_FillGradCenterRound(IN SYColor* pColor,			// 填充颜色
								 OUT DWORD* pdwBuf,				// 填充对象
								 IN int nBufWidth,				// 对象大小
								 IN int nBufHeight,
								 IN int nX,						// 填充区域
								 IN int nY,
								 IN int nWidth,
								 IN int nHeight,
								 IN int nFrame/* = 0*/,			// 颜色动画的帧号，于静态颜色无用
								 IN BYTE* pbyMask/* = NULL*/,	// 掩膜（8位缓存），大小与填充区域相同
								 IN int nMaskPitch/* = 0*/)		// 掩膜缓存的特殊性，pitch可能不等于宽度
{
	//assert((pColor != NULL) && (pdwBuf != NULL));

	int nminw = min(nBufWidth - nX, nWidth);
	int nminh = min(nBufHeight - nY, nHeight);
	DWORD* pdwFillBuf = pdwBuf + nY * nBufWidth + nX;
	// 外接圆半径，实际需要做线性渐变的多倍半径，通过细分量化减少浮点运算负载
	int nRadius_Z = (int)ceilf(sqrtf((float)(nminw * nminw + nminh * nminh)) * 0.5f * CENTER_ROUND_LINEAR_SCALING);
	// 离中心位置最远的距离
	float fCenterX = pColor->gradual.center.centerx * nminw;	// 以左上角为原点
	float fCenterY = pColor->gradual.center.centery * nminh;
	float fMaxDx = ((pColor->gradual.center.centerx < 0.5f) ? (nminw - fCenterX) : fCenterX);
	float fMaxDy = ((pColor->gradual.center.centery < 0.5f) ? (nminh - fCenterY) : fCenterY);
	// 线性表的最大长度，放大多倍以得到更好的精度（多加一个像素防止访问越界）
	int nMaxLen_Z = (int)ceilf(sqrtf(fMaxDx * fMaxDx + fMaxDy * fMaxDy) * CENTER_ROUND_LINEAR_SCALING + CENTER_ROUND_LINEAR_SCALING);
	DWORD* pdwMaxLineTable = SYSSE_NEW(nMaxLen_Z, DWORD);		// 半径方向上的表
	float* pfRadiusX = SYSSE_NEW(nminw, float);		// 求点到圆心的距离时的 X 分量：(x - xc)^2
	float* pfRadiusY = SYSSE_NEW(nminh, float);		// 求点到圆心的距离时的 Y 分量：(y - yc)^2
	float* pfTmpRadiusY = pfRadiusY;

	// 沿半径方向进行线性渐变生成一行表
	SYColor_FillLineBaseAR_1_Recursive(pdwMaxLineTable, nRadius_Z, pColor->gradual.color,
		pColor->gradual.pos, pColor->gradual.mid, pColor->gradual.ncount,
		pColor->gradual.center.repeat, pColor->gradual.center.style);
	// 节点颜色动画的测试版
	if ((pColor->gradual.animation & GRAD_ANI_COLOR) != GRAD_ANI_NONE)
	{
		SYColor_ProcAniColor(pdwMaxLineTable, nRadius_Z, nFrame, pColor->gradual.aniparam[GRAD_ANI_PARAM_COLOR], pColor->gradual.center.style);
	}
	// 使用最后一个渐变色填充渐变区域之外的缓存
	SYMemset_LineDWORD((pdwMaxLineTable + nRadius_Z), *(pdwMaxLineTable + nRadius_Z - 1), (nMaxLen_Z - nRadius_Z));
	// 生成求点到圆心的距离的分量表
	SY_SerialPerfectSquare(pfRadiusX, nminw, 0.f, 1.f, -fCenterX);
	SY_SerialPerfectSquare(pfRadiusY, nminh, 0.f, 1.f, -fCenterY);
	// 根据分量表计算出像素点距中心的位置，查表进行填充
	for (int i=0; i<nminh; i++)
	{
		SY_OffsetSqrtIndexLookup(pdwFillBuf, nminw, pfRadiusX, *pfTmpRadiusY, pdwMaxLineTable, CENTER_ROUND_LINEAR_SCALING);
		pdwFillBuf += nBufWidth;
		pfTmpRadiusY++;
	}
	// 释放缓存表
	SYSSE_FREE(pdwMaxLineTable);
	SYSSE_FREE(pfRadiusX);
	SYSSE_FREE(pfRadiusY);
}
// 凸多边形中心渐变色
void SYColor_FillGradCenterPolygon(IN SYColor* pColor,			// 填充颜色
								   OUT DWORD* pdwBuf,			// 填充对象
								   IN int nBufWidth,			// 对象大小
								   IN int nBufHeight,
								   IN int nX,					// 填充区域
								   IN int nY,
								   IN int nWidth,
								   IN int nHeight,
								   IN int nFrame/* = 0*/,		// 颜色动画的帧号，于静态颜色无用
								   IN BYTE* pbyMask/* = NULL*/,	// 掩膜（8位缓存），大小与填充区域相同
								   IN int nMaskPitch/* = 0*/)	// 掩膜缓存的特殊性，pitch可能不等于宽度
{
	//assert((pColor != NULL) && (pdwBuf != NULL));

	int nminw = min(nBufWidth - nX, nWidth);
	int nminh = min(nBufHeight - nY, nHeight);
	DWORD* pdwFillBuf = pdwBuf + nY * nBufWidth + nX, *pdwTmpLine = NULL;
	// 外接圆半径，实际需要做线性渐变的多倍半径，通过细分量化减少浮点运算负载
	float fRadius_Z = sqrtf((float)(nminw * nminw + nminh * nminh)) * 0.5f * CENTER_ROUND_LINEAR_SCALING;
	int nRadius_Z = (int)ceilf(fRadius_Z);
	// 离中心位置最远的距离
	float fCenterX = pColor->gradual.center.centerx * nminw;	// 以左上角为原点
	float fCenterY = pColor->gradual.center.centery * nminh;
	float fMaxDx = ((pColor->gradual.center.centerx < 0.5f) ? (nminw - fCenterX) : fCenterX);
	float fMaxDy = ((pColor->gradual.center.centery < 0.5f) ? (nminh - fCenterY) : fCenterY);
	// 线性表的最大长度，放大多倍以得到更好的精度（多加一个像素防止访问越界）
	int nMaxLen_Z = (int)ceilf(sqrtf(fMaxDx * fMaxDx + fMaxDy * fMaxDy) * CENTER_ROUND_LINEAR_SCALING + CENTER_ROUND_LINEAR_SCALING);
	DWORD* pdwMaxLineTable = SYSSE_NEW(nMaxLen_Z, DWORD);		// 半径方向上的表
	// 要求凸多边形按顺时针或逆时针顺序给出顶点，要求中心点要在凸多边形内部
	float fDist_Inv[8] = { 0.f };			// 中心点到边的距离的倒数（不考虑 (a^2 + b^2)^1/2）
	float fCoefLine[8][9] = { 0.f };		// 顺序三角形顺序边的直线系数 (x2-x1)*y+(y1-y2)*x+(y2x1-y1x2)=0
	float fBaryCenter[8][3] = { 0.f };		// 三角形重心代入直线的值，说明了三角形重心相对于直线的位置
	float fx[3] = { 0.f }, fy[3] = { 0.f }, fTmpX(0.f), fTmpY(0.f);
	int nNext(0), nStart(0);
	BOOL bInTriangle = FALSE;

	// 沿半径方向进行线性渐变生成一行表
	SYColor_FillLineBaseAR_1_Recursive(pdwMaxLineTable, nRadius_Z, pColor->gradual.color,
		pColor->gradual.pos, pColor->gradual.mid, pColor->gradual.ncount,
		pColor->gradual.center.repeat, pColor->gradual.center.style);
	// 节点颜色动画的测试版
	if ((pColor->gradual.animation & GRAD_ANI_COLOR) != GRAD_ANI_NONE)
	{
		SYColor_ProcAniColor(pdwMaxLineTable, nRadius_Z, nFrame, pColor->gradual.aniparam[GRAD_ANI_PARAM_COLOR], pColor->gradual.center.style);
	}
	// 使用最后一个渐变色填充渐变区域之外的缓存
	SYMemset_LineDWORD((pdwMaxLineTable + nRadius_Z), *(pdwMaxLineTable + nRadius_Z - 1), (nMaxLen_Z - nRadius_Z));
	// 生成凸多边形的系数
	fx[2] = fCenterX, fy[2] = fCenterY;
	for (int i=0; i<pColor->gradual.center.nvertex; i++)
	{
		// 三角形顶点
		nNext = (i == pColor->gradual.center.nvertex - 1) ? 0 : (i + 1);
		fx[0] = pColor->gradual.center.xpt[i] * nminw;
		fy[0] = pColor->gradual.center.ypt[i] * nminh;
		fx[1] = pColor->gradual.center.xpt[nNext] * nminw;
		fy[1] = pColor->gradual.center.ypt[nNext] * nminh;
		// 三角形重心
		fTmpX = (fx[0] + fx[1] + fx[2]) / 3.f;
		fTmpY = (fy[0] + fy[1] + fy[2]) / 3.f;
		// 三角形相关系数
		nStart = 0;
		for (int j=0; j<3; j++)		// 三条边
		{
			nNext = (j == 2) ? 0 : (j + 1);
			// 直线系数 ay + bx + c = 0
			fCoefLine[i][nStart + 0] = fx[nNext] - fx[j];	// a
			fCoefLine[i][nStart + 1] = fy[j] - fy[nNext];	// b
			fCoefLine[i][nStart + 2] = fy[nNext] * fx[j] - fy[j] * fx[nNext];	// c
			// 三角形重心位置
			fBaryCenter[i][j] = fCoefLine[i][nStart + 0] * fTmpY + fCoefLine[i][nStart + 1] * fTmpX + fCoefLine[i][nStart + 2];
			nStart += 3;
		}
		// 中心点到边的距离（三角形边组的第一个肯定是凸多边形的边）
		fDist_Inv[i] = 1.f / (fabsf(fCoefLine[i][0] * fCenterY + fCoefLine[i][1] * fCenterX + fCoefLine[i][2]) + 0.000001f);
	}
	// 判断点在哪个三角形内，然后求出点到凸多边形边的距离，以其反向距离进行查表
	for (int i=0; i<nminh; i++)
	{
		pdwTmpLine = pdwFillBuf;
		for (int j=0; j<nminw; j++)
		{
			nNext = nRadius_Z;		// 一开始认为点在凸多边形外
			for (int k=0; k<pColor->gradual.center.nvertex; k++)	// n 个三角形
			{
				nStart = 0;
				bInTriangle = TRUE;
				for (int l=0; l<3; l++)		// 三条边
				{
					fTmpX = fCoefLine[k][nStart + 0] * i + fCoefLine[k][nStart + 1] * j + fCoefLine[k][nStart + 2];
					if (fTmpX * fBaryCenter[k][l] < 0.f)	// 点在边的方向与重心不同，则说明点不在三角形内
					{
						bInTriangle = FALSE;
						break;
					}
					nStart += 3;
				}
				if (bInTriangle)	// 如果在三角形内，计算表索引
				{
					fTmpX = fabsf(fCoefLine[k][0] * i + fCoefLine[k][1] * j + fCoefLine[k][2]) * fDist_Inv[k];
					IN_RANGE(fTmpX, 0.f, 1.f);
					nNext = (int)(fRadius_Z * (1.f - fTmpX) + 0.5f);
					break;
				}
			}
			*pdwTmpLine++ = pdwMaxLineTable[nNext];
		}
		pdwFillBuf += nBufWidth;
	}
	// 释放缓存表
	SYSSE_FREE(pdwMaxLineTable);
}
// 填充
// 圆周轮廓渐变色
void SYColor_FillGradOutlineCircle(IN SYColor* pColor,			// 填充颜色
								   OUT DWORD* pdwBuf,			// 填充对象
								   IN int nBufWidth,			// 对象大小
								   IN int nBufHeight,
								   IN int nX,					// 填充区域
								   IN int nY,
								   IN int nWidth,
								   IN int nHeight,
								   IN int nFrame/* = 0*/,		// 颜色动画的帧号，于静态颜色无用
								   IN BYTE* pbyMask/* = NULL*/,	// 掩膜（8位缓存），大小与填充区域相同
								   IN int nMaskPitch/* = 0*/)	// 掩膜缓存的特殊性，pitch可能不等于宽度
{
	//assert((pColor != NULL) && (pdwBuf != NULL));

	int nminw = min(nBufWidth - nX, nWidth);
	int nminh = min(nBufHeight - nY, nHeight);
	DWORD *pdwFillBuf = pdwBuf + nY * nBufWidth + nX;
	// 外接圆圆心与最大外接半径（离中心位置最远的距离）
	float fCenterX = pColor->gradual.outline.centerx * nminw;	// 以左上角为原点
	float fCenterY = pColor->gradual.outline.centery * nminh;
	int nCenterX = (int)ceilf(fCenterX);
	int nCenterY = (int)fCenterY, nCenterYWidth = (int)ceilf(fCenterY - (float)nCenterY);
	float fMaxDx = ((pColor->gradual.outline.centerx < 0.5f) ? (nminw - fCenterX) : fCenterX);
	float fMaxDy = ((pColor->gradual.outline.centery < 0.5f) ? (nminh - fCenterY) : fCenterY);
	float fMaxRadius_Z = sqrtf(fMaxDx * fMaxDx + fMaxDy * fMaxDy) * OUTLINE_CIRCLE_LINEAR_SCALING;	// 放大半径以放大圆周长
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
	//						          [(x - xc)^2 + (y - yc)^2]^(5/2)
	//    = Cy + R * (yc - y) * My
	//
	// (135°,225°) 范围内则令 Cy = PI * R 并使用 (y - yc)
	// (315°,360°) 范围内则令 Cy = 2 * PI * R 并使用 (yc - y)
	// 为了最大化加速对 (x - xc)^4、(x - xc)^2、(x - xc) * R、(y - yc)^4、(y - yc)^2、(y - yc) * R 进行缓存
	// 参数定义，通过放大半径对圆周长进行线性放大以提高精度（通过细分量化减少浮点运算负载）
	float fGirth_Z = fMaxRadius_Z * RADIAN_2_PI, fHalfGirth_Z = fGirth_Z * 0.5f;
	float fGirth_1_4_Z = fGirth_Z * 0.25f, fGirth_3_4_Z = fGirth_Z * 0.75f;
	// 圆周线性表的最大长度，放大多倍以得到更好的精度（多加一个像素防止访问越界）
	int nGirth_Z = (int)ceilf(fGirth_Z);
	int nMaxLen_Z = (int)ceilf(fGirth_Z + OUTLINE_CIRCLE_LINEAR_SCALING);
	// 缓存
	DWORD* pdwMaxLineTable = SYSSE_NEW(nMaxLen_Z, DWORD);		// 圆周上的表
	float* pfQuarticX = SYSSE_NEW(nminw, float);				// (x - xc)^4
	float* pfRadiusX = SYSSE_NEW(nminw, float);					// (x - xc)^2
	float* pfSubX = SYSSE_NEW(nminw, float);					// (x - xc) * R
	float* pfQuarticY = SYSSE_NEW(nminh, float);				// (y - yc)^4
	float* pfRadiusY = SYSSE_NEW(nminh, float);					// (y - yc)^2
	float* pfSubY = SYSSE_NEW(nminh, float);					// (y - yc) * R
	int* pnLine45_X = SYSSE_NEW(nminh, int);					// 直线 y = x 与目标区域的交点 xc - yc + y
	int* pnLine135_X = SYSSE_NEW(nminh, int);					// 直线 y = -x 与目标区域的交点 xc + yc - y
	float *pfTmpQuarticY = pfQuarticY, *pfTmpRadiusY = pfRadiusY, *pfTmpSubY = pfSubY;
	int *pnTmpLine45_X = pnLine45_X, *pnTmpLine135_X = pnLine135_X;
	int nLeft(0), nRight(0), nTmpLoop(0);

	// 沿圆周方向进行线性渐变生成一行表，设起点为左边中点
	SYColor_FillLineBaseAR_1_Recursive(pdwMaxLineTable, nGirth_Z, pColor->gradual.color,
		pColor->gradual.pos, pColor->gradual.mid, pColor->gradual.ncount,
		pColor->gradual.outline.repeat, pColor->gradual.outline.style);
	// 节点颜色动画的测试版
	if ((pColor->gradual.animation & GRAD_ANI_COLOR) != GRAD_ANI_NONE)
	{
		SYColor_ProcAniColor(pdwMaxLineTable, nGirth_Z, nFrame, pColor->gradual.aniparam[GRAD_ANI_PARAM_COLOR], pColor->gradual.outline.style);
	}
	// 使用最后一个渐变色填充安全区缓存
	SYMemset_LineDWORD((pdwMaxLineTable + nGirth_Z), *(pdwMaxLineTable + nGirth_Z - 1), (nMaxLen_Z - nGirth_Z));
	// 生成缓存表
	SY_SerialPerfectSquareEx(pfRadiusX, pfQuarticX, pfSubX, nminw, 0.f, 1.f, -fCenterX, fMaxRadius_Z);
	SY_SerialPerfectSquareEx(pfRadiusY, pfQuarticY, pfSubY, nminh, 0.f, 1.f, -fCenterY, fMaxRadius_Z);
	SY_SerialOffset2(pnLine45_X, pnLine135_X, nminh, 0, nminw, (fCenterX - fCenterY), (fCenterX + fCenterY), 0, 1);
	// 按照圆心 Y 坐标将圆弧分为上下两部分，分别进行求解
	nTmpLoop = (nCenterY > nminh) ? nminh : nCenterY;
	for (int i=0; i<nTmpLoop; i++)
	{
		nLeft = *pnTmpLine45_X++;
		nRight = *pnTmpLine135_X++;
		// 45 度直线左边
		if (nLeft)
		{
			SY_TylerArcIndexLookupY(pdwFillBuf, nLeft, pfQuarticX, pfRadiusX, *pfTmpQuarticY, *pfTmpRadiusY, -*pfTmpSubY, pdwMaxLineTable, 0.f);
		}
		// 45 度直线上
		//nLeft++;
		// 45 度至 135 度之间
		if (nLeft < nRight)
		{
			SY_TylerArcIndexLookupX((pdwFillBuf + nLeft), (nRight - nLeft), (pfQuarticX + nLeft), (pfRadiusX + nLeft),
				(pfSubX + nLeft), *pfTmpQuarticY, *pfTmpRadiusY, pdwMaxLineTable, fGirth_1_4_Z);
		}
		// 135 度直线上
		//nRight++;
		// 135 度直线右边
		if (nRight < nminw)
		{
			SY_TylerArcIndexLookupY((pdwFillBuf + nRight), (nminw - nRight), (pfQuarticX + nRight),
				(pfRadiusX + nRight), *pfTmpQuarticY, *pfTmpRadiusY, *pfTmpSubY, pdwMaxLineTable, fHalfGirth_Z);
		}
		// 缓存移动
		pdwFillBuf += nBufWidth;
		pfTmpQuarticY++;
		pfTmpRadiusY++;
		pfTmpSubY++;
	}
	if (nCenterYWidth && nCenterY < nminh)	// 如果圆心正好在一个像素的中间，则特殊处理这一行
	{
		IN_RANGE(nCenterX, 0, nminw);
		// 圆心左边填充起点颜色，右边填充中间颜色
		SYMemset_LineDWORD(pdwFillBuf, *pdwMaxLineTable, nCenterX);
		SYMemset_LineDWORD((pdwFillBuf + nCenterX), *(pdwMaxLineTable + (nGirth_Z >> 1)), (nminw - nCenterX));
		pdwFillBuf += nBufWidth;
		pfTmpQuarticY++;
		pfTmpRadiusY++;
		pfTmpSubY++;
		pnTmpLine45_X++;
		pnTmpLine135_X++;
	}
	nTmpLoop = nCenterYWidth + nCenterY;
	if (nTmpLoop < nminh)		// 有下半圆弧
	{
		// 翻转数值的符号
		SY_NegtiveFloat(pfSubX, pfSubX, nminw);
		nTmpLoop = (nTmpLoop < 0) ? 0 : nTmpLoop;
		for (int i=nTmpLoop; i<nminh; i++)
		{
			nLeft = *pnTmpLine135_X++;
			nRight = *pnTmpLine45_X++;
			// 315 度直线左边
			if (nLeft)
			{
				SY_TylerArcIndexLookupY(pdwFillBuf, nLeft, pfQuarticX, pfRadiusX, *pfTmpQuarticY, *pfTmpRadiusY, -*pfTmpSubY, pdwMaxLineTable, fGirth_Z);
			}
			// 315 度直线上
			//nLeft++;
			// 315 度至 225 度之间
			if (nLeft < nRight)
			{
				SY_TylerArcIndexLookupX((pdwFillBuf + nLeft), (nRight - nLeft), (pfQuarticX + nLeft), (pfRadiusX + nLeft),
					(pfSubX + nLeft), *pfTmpQuarticY, *pfTmpRadiusY, pdwMaxLineTable, fGirth_3_4_Z);
			}
			// 225 度直线上
			//nRight++;
			// 225 度直线右边
			if (nRight < nminw)
			{
				SY_TylerArcIndexLookupY((pdwFillBuf + nRight), (nminw - nRight), (pfQuarticX + nRight),
					(pfRadiusX + nRight), *pfTmpQuarticY, *pfTmpRadiusY, *pfTmpSubY, pdwMaxLineTable, fHalfGirth_Z);
			}
			// 缓存移动
			pdwFillBuf += nBufWidth;
			pfTmpQuarticY++;
			pfTmpRadiusY++;
			pfTmpSubY++;
		}
	}
	// 释放缓存表
	SYSSE_FREE(pdwMaxLineTable);
	SYSSE_FREE(pfQuarticX);
	SYSSE_FREE(pfRadiusX);
	SYSSE_FREE(pfSubX);
	SYSSE_FREE(pfQuarticY);
	SYSSE_FREE(pfRadiusY);
	SYSSE_FREE(pfSubY);
	SYSSE_FREE(pnLine45_X);
	SYSSE_FREE(pnLine135_X);
}
// 多边形轮廓渐变色
void SYColor_FillGradOutlinePolygon(IN SYColor* pColor,			// 填充颜色
									OUT DWORD* pdwBuf,			// 填充对象
									IN int nBufWidth,			// 对象大小
									IN int nBufHeight,
									IN int nX,					// 填充区域
									IN int nY,
									IN int nWidth,
									IN int nHeight,
									IN int nFrame/* = 0*/,		// 颜色动画的帧号，于静态颜色无用
									IN BYTE* pbyMask/* = NULL*/,// 掩膜（8位缓存），大小与填充区域相同
									IN int nMaskPitch/* = 0*/)	// 掩膜缓存的特殊性，pitch可能不等于宽度
{
	//assert((pColor != NULL) && (pdwBuf != NULL));
	//assert(pColor->gradual.ncount >= SYCOLOR_GRAD_VERTEX_MIN);

	int nminw = min(nBufWidth - nX, nWidth);
	int nminh = min(nBufHeight - nY, nHeight);
	DWORD* pdwFillBuf = pdwBuf + nY * nBufWidth + nX, *pdwTmpLine = NULL;
	// 中心位置
	float fCenterX = pColor->gradual.outline.centerx * nminw;	// 以左上角为原点
	float fCenterY = pColor->gradual.outline.centery * nminh;
	// 边的线性表，放大多倍以得到更好的精度
	int nSideLen[8] = { 0 };
	DWORD* pdwSideLineTable[8] = { NULL };		// 半径方向上的表
	// 要求凸多边形按顺时针或逆时针顺序给出顶点，要求中心点要在凸多边形内部
	float fCoefLine[8][9] = { 0.f };		// 顺序三角形顺序边的直线系数 (x2-x1)*y+(y1-y2)*x+(y2x1-y1x2)=0
	float fBaryCenter[8][3] = { 0.f };		// 三角形重心代入直线的值，说明了三角形重心相对于直线的位置
	float fStartPtX[8] = { 0.f }, fStartPtY[8] = { 0.f };		// 边渐变色的起点坐标
	float fx[3] = { 0.f }, fy[3] = { 0.f }, fTmpX(0.f), fTmpY(0.f), fDx(0.f), fDy(0.f);
	int nNext(0), nStart(0), nSideIndex(0);
	BOOL bInTriangle = FALSE;
	DWORD dwCr[2] = { 0 };			// 边上的线性渐变色参数
	float fPos[2] = { 0.f, 1.f };
	float fMid[1] = { 0.f };

	fx[2] = fCenterX, fy[2] = fCenterY;
	// 生成凸多边形的系数，求解边长并生成边的线性渐变表
	for (int i=0; i<pColor->gradual.ncount; i++)
	{
		// 三角形顶点
		nNext = (i == pColor->gradual.ncount - 1) ? 0 : (i + 1);
		fStartPtX[i] = fx[0] = pColor->gradual.outline.xpt[i] * nminw;
		fStartPtY[i] = fy[0] = pColor->gradual.outline.ypt[i] * nminh;
		fx[1] = pColor->gradual.outline.xpt[nNext] * nminw;
		fy[1] = pColor->gradual.outline.ypt[nNext] * nminh;
		// 最外边边长（放大数倍，以得到更大的精度），以及边上的渐变色
		fDx = fx[0] - fx[1];
		fDy = fy[0] - fy[1];
		nSideLen[i] = (int)ceilf(sqrtf(fDx * fDx + fDy * fDy) * OUTLINE_POLYGON_LINEAR_SCALING);
		pdwSideLineTable[i] = SYSSE_NEW(nSideLen[i], DWORD);
		dwCr[0] = pColor->gradual.color[i];
		dwCr[1] = pColor->gradual.color[nNext];
		fMid[0] = (nNext == 0) ? 0.5f : pColor->gradual.mid[i];
		SYColor_FillLineBaseAR_1_Recursive(pdwSideLineTable[i], nSideLen[i], dwCr, fPos, fMid, 2, 1, 0);
		// 三角形重心
		fTmpX = (fx[0] + fx[1] + fx[2]) / 3.f;
		fTmpY = (fy[0] + fy[1] + fy[2]) / 3.f;
		// 三角形相关系数
		nStart = 0;
		for (int j=0; j<3; j++)		// 三条边
		{
			nNext = (j == 2) ? 0 : (j + 1);
			// 直线系数 ay + bx + c = 0
			fCoefLine[i][nStart + 0] = fx[nNext] - fx[j];	// a
			fCoefLine[i][nStart + 1] = fy[j] - fy[nNext];	// b
			fCoefLine[i][nStart + 2] = fy[nNext] * fx[j] - fy[j] * fx[nNext];	// c
			// 三角形重心位置
			fBaryCenter[i][j] = fCoefLine[i][nStart + 0] * fTmpY + fCoefLine[i][nStart + 1] * fTmpX + fCoefLine[i][nStart + 2];
			nStart += 3;
		}
	}
	// 判断点在哪个三角形内，然后求解通过点与中心点的直线与边的交点，以该点到边起点的距离进行查表
	for (int i=0; i<nminh; i++)
	{
		pdwTmpLine = pdwFillBuf;
		for (int j=0; j<nminw; j++)
		{
			nNext = 0;		// 一开始认为点在三角形边上
			nSideIndex = 0;
			for (int k=0; k<pColor->gradual.ncount; k++)	// n 个三角形
			{
				nStart = 0;
				bInTriangle = TRUE;
				for (int l=0; l<3; l++)		// 三条边
				{
					fTmpX = fCoefLine[k][nStart + 0] * i + fCoefLine[k][nStart + 1] * j + fCoefLine[k][nStart + 2];
					if (fTmpX * fBaryCenter[k][l] < 0.f)	// 点在边的方向与重心不同，则说明点不在三角形内
					{
						bInTriangle = FALSE;
						break;
					}
					nStart += 3;
				}
				if (bInTriangle)	// 如果在三角形内，计算表索引
				{
					// 直线系数 ay + bx + c = 0
					nSideIndex = k;
					fx[0] = j - fCenterX;		// a (x2-x1)
					fx[1] = fCenterY - i;		// b (y1-y2)
					fx[2] = i * fCenterX - j * fCenterY;	// c (y2x1-y1x2)
					fDx = 1.f / (fx[0] * fCoefLine[k][1] - fCoefLine[k][0] * fx[1] + 0.000001f);	// a2*b1-a1*b2
					fTmpX = fDx * (fCoefLine[k][0] * fx[2] - fx[0] * fCoefLine[k][2]);	// a1*c2-a2*c1
					fTmpY = fDx * (fx[1] * fCoefLine[k][2] - fCoefLine[k][1] * fx[2]);	// b2*c1-b1*c2
					fDx = fTmpX - fStartPtX[nSideIndex];
					fDy = fTmpY - fStartPtY[nSideIndex];
					nNext = (int)(sqrtf(fDx * fDx + fDy * fDy) * OUTLINE_POLYGON_LINEAR_SCALING + 0.5f);
					IN_RANGE(nNext, 0, nSideLen[nSideIndex] - 1);
					break;
				}
			}
			*pdwTmpLine++ = *(pdwSideLineTable[nSideIndex] + nNext);
		}
		pdwFillBuf += nBufWidth;
	}
	// 释放缓存表
	for (int i=0; i<pColor->gradual.ncount; i++)
	{
		SYSSE_FREE(pdwSideLineTable[i]);
	}
}
// 填充
// 多点等高渐变色
// 令 sn = [(x - xn)^2 + (y - yn)^2]^1/2 表示目标点到第 n 个顶点的距离
// 以其倒数为权重，目标颜色为 c_des = (c1/s1 + c2/s2 + ……) / (1/s1 + 1/s2 + ……)
void SYColor_FillGradPtContour(IN SYColor* pColor,			// 填充颜色
							   OUT DWORD* pdwBuf,			// 填充对象
							   IN int nBufWidth,			// 对象大小
							   IN int nBufHeight,
							   IN int nX,					// 填充区域
							   IN int nY,
							   IN int nWidth,
							   IN int nHeight,
							   IN int nFrame/* = 0*/,		// 颜色动画的帧号，于静态颜色无用
							   IN BYTE* pbyMask/* = NULL*/,	// 掩膜（8位缓存），大小与填充区域相同
							   IN int nMaskPitch/* = 0*/)	// 掩膜缓存的特殊性，pitch可能不等于宽度
{
	//assert((pColor != NULL) && (pdwBuf != NULL));
	//assert(pColor->gradual.ncount >= SYCOLOR_GRAD_VERTEX_MIN);

	int nminw = min(nBufWidth - nX, nWidth);
	int nminh = min(nBufHeight - nY, nHeight);
	DWORD *pdwFillBuf = pdwBuf + nY * nBufWidth + nX;
	float *ppfDistX[8] = { NULL };		// 水平索引到各个顶点的距离分量
	float *ppfDistY[8] = { NULL };		// 垂直索引到各个顶点的距离分量
	float ppfCrCell[8][4] = { 0.f };	// 各个顶点颜色的分量，0 存储最低位字节，3 存储最高位字节

	// 申请缓存并生成数据
	for (int i=0; i<pColor->gradual.ncount; i++)
	{
		ppfDistX[i] = SYSSE_NEW(nminw, float);
		ppfDistY[i] = SYSSE_NEW(nminh, float);
		SY_SerialPerfectSquare(ppfDistX[i], nminw, 0.f, 1.f, -(pColor->gradual.multipt.xpt[i] * nminw));
		SY_SerialPerfectSquare(ppfDistY[i], nminh, 0.f, 1.f, -(pColor->gradual.multipt.ypt[i] * nminh));
		ppfCrCell[i][0] = (float)GetBGRA_B(pColor->gradual.color[i]);
		ppfCrCell[i][1] = (float)GetBGRA_G(pColor->gradual.color[i]);
		ppfCrCell[i][2] = (float)GetBGRA_R(pColor->gradual.color[i]);
		ppfCrCell[i][3] = (float)GetAValue(pColor->gradual.color[i]);
	}
	// 令 sn = [(x - xn)^2 + (y - yn)^2]^1/2 表示目标点到第 n 个顶点的距离
	// 以其倒数为权重，目标颜色为 c_des = (c1/s1 + c2/s2 + ……) / (1/s1 + 1/s2 + ……)
	for (int i=0; i<nminh; i++)
	{
		DWORD* pdwTmpLine = pdwFillBuf;
		float fWeightSum(0.f), fCurWeight(0.f), fDistY(0.f), fCurDist(0.f), fDistX(0.f);
		float fA(0.f), fR(0.f), fG(0.f), fB(0.f);
		DWORD dwA(0), dwR(0), dwG(0), dwB(0);

		for (int j=0; j<nminw; j++)
		{
			fWeightSum = 0.000001f;
			fA = fR = fG = fB = 0.f;
			for (int k=0; k<pColor->gradual.ncount; k++)
			{
				fDistX = *(ppfDistX[k] + j);
				fDistY = *(ppfDistY[k] + i);
				fCurDist = sqrtf(fDistX + fDistY) + 0.000001f;
				fCurWeight = 1.f / fCurDist;
				fWeightSum += fCurWeight;
				fA += (ppfCrCell[k][3] * fCurWeight);
				fR += (ppfCrCell[k][2] * fCurWeight);
				fG += (ppfCrCell[k][1] * fCurWeight);
				fB += (ppfCrCell[k][0] * fCurWeight);
			}
			fWeightSum = 1.f / fWeightSum;
			dwA = (DWORD)(fA * fWeightSum + 0.5f); if (dwA > 255)	dwA = 255;
			dwR = (DWORD)(fR * fWeightSum + 0.5f); if (dwR > 255)	dwR = 255;
			dwG = (DWORD)(fG * fWeightSum + 0.5f); if (dwG > 255)	dwG = 255;
			dwB = (DWORD)(fB * fWeightSum + 0.5f); if (dwB > 255)	dwB = 255;
			*pdwTmpLine = BGRA(dwB, dwG, dwR, dwA);
			pdwTmpLine++;
		}
		pdwFillBuf += nBufWidth;
	}
	// 释放缓存
	for (int i=0; i<pColor->gradual.ncount; i++)
	{
		SYSSE_FREE(ppfDistX[i]);
		SYSSE_FREE(ppfDistY[i]);
	}
}
// 多点混成渐变色
// 令 sn = [(x - xn)^2 + (y - yn)^2]^1/2 表示目标点到第 n 个顶点的距离
// 以其为权重，目标颜色为 c_des = (c1 * s1 + c2 * s2 + ……) / (s1 + s2 + ……)
void SYColor_FillGradPtBlend(IN SYColor* pColor,			// 填充颜色
							 OUT DWORD* pdwBuf,				// 填充对象
							 IN int nBufWidth,				// 对象大小
							 IN int nBufHeight,
							 IN int nX,						// 填充区域
							 IN int nY,
							 IN int nWidth,
							 IN int nHeight,
							 IN int nFrame/* = 0*/,			// 颜色动画的帧号，于静态颜色无用
							 IN BYTE* pbyMask/* = NULL*/,	// 掩膜（8位缓存），大小与填充区域相同
							 IN int nMaskPitch/* = 0*/)		// 掩膜缓存的特殊性，pitch可能不等于宽度
{
	int nminw = min(nBufWidth - nX, nWidth);
	int nminh = min(nBufHeight - nY, nHeight);
	DWORD *pdwFillBuf = pdwBuf + nY * nBufWidth + nX;
	float *ppfDistX[8] = { NULL };		// 水平索引到各个顶点的距离分量
	float *ppfDistY[8] = { NULL };		// 垂直索引到各个顶点的距离分量
	float ppfCrCell[8][4] = { 0.f };		// 各个顶点颜色的分量，0 存储最低位字节，3 存储最高位字节
	float fMaxDist_Inv[8] = { 0.f };		// 各个顶点离目标区域四个顶点最远距离的倒数
	float fCoefRadius = 10.f / (sqrtf((float)(nminw * nminw + nminh * nminh)) + 0.000001f);
	//float fCoefRadius = (1.f / pColor->gradual.ncount) / sqrtf((float)(nminw * nminw + nminh * nminh));
	//float fCoefRadius = sqrtf((float)(nminw * nminw + nminh * nminh)) * 0.5f;
	float fVertexX(0.f), fVertexY(0.f);

	// 申请缓存并生成数据
	for (int i=0; i<pColor->gradual.ncount; i++)
	{
		ppfDistX[i] = SYSSE_NEW(nminw, float);
		ppfDistY[i] = SYSSE_NEW(nminh, float);
		SY_SerialPerfectSquare(ppfDistX[i], nminw, 0.f, 1.f, -(pColor->gradual.multipt.xpt[i] * nminw));
		SY_SerialPerfectSquare(ppfDistY[i], nminh, 0.f, 1.f, -(pColor->gradual.multipt.ypt[i] * nminh));
		ppfCrCell[i][0] = (float)GetBGRA_B(pColor->gradual.color[i]);
		ppfCrCell[i][1] = (float)GetBGRA_G(pColor->gradual.color[i]);
		ppfCrCell[i][2] = (float)GetBGRA_R(pColor->gradual.color[i]);
		ppfCrCell[i][3] = (float)GetAValue(pColor->gradual.color[i]);
	}
	// 令 sn = [(x - xn)^2 + (y - yn)^2]^1/2 表示目标点到第 n 个顶点的距离
	// 以其为权重，目标颜色为 c_des = (c1 * s1 + c2 * s2 + ……) / (s1 + s2 + ……)
	for (int i=0; i<nminh; i++)
	{
		DWORD* pdwTmpLine = pdwFillBuf;
		float fWeightSum(0.f), fCurWeight(0.f), fDistY(0.f), fDistX(0.f);
		float fA(0.f), fR(0.f), fG(0.f), fB(0.f);
		DWORD dwA(0), dwR(0), dwG(0), dwB(0);

		for (int j=0; j<nminw; j++)
		{
			fWeightSum = 0.000001f;
			fA = fR = fG = fB = 0.f;
			for (int k=0; k<pColor->gradual.ncount; k++)
			{
				fDistX = *(ppfDistX[k] + j);
				fDistY = *(ppfDistY[k] + i);
				//fCurWeight = sqrtf(fDistX + fDistY) * fMaxDist_Inv[k];
				fCurWeight = sqrtf(fDistX + fDistY) * fCoefRadius;
				//fCurWeight = 1.f - sqrtf(fDistX + fDistY) * fMaxDist_Inv[k];
				//fCurWeight = (fDistX + fDistY) * fCoefRadius;
				fCurWeight = powf(0.5f, fCurWeight);
				fWeightSum += fCurWeight;
				fA += (ppfCrCell[k][3] * fCurWeight);
				fR += (ppfCrCell[k][2] * fCurWeight);
				fG += (ppfCrCell[k][1] * fCurWeight);
				fB += (ppfCrCell[k][0] * fCurWeight);
			}
			fWeightSum = 1.f / fWeightSum;
			dwA = (DWORD)(fA * fWeightSum + 0.5f); if (dwA > 255)	dwA = 255;
			dwR = (DWORD)(fR * fWeightSum + 0.5f); if (dwR > 255)	dwR = 255;
			dwG = (DWORD)(fG * fWeightSum + 0.5f); if (dwG > 255)	dwG = 255;
			dwB = (DWORD)(fB * fWeightSum + 0.5f); if (dwB > 255)	dwB = 255;
			*pdwTmpLine = BGRA(dwB, dwG, dwR, dwA);
			pdwTmpLine++;
		}
		pdwFillBuf += nBufWidth;
	}
	// 释放缓存
	for (int i=0; i<pColor->gradual.ncount; i++)
	{
		SYSSE_FREE(ppfDistX[i]);
		SYSSE_FREE(ppfDistY[i]);
	}
}
// 填充
void SYColor_FillGradTemplet(IN SYColor* pColor,			// 填充颜色
							 OUT DWORD* pdwBuf,				// 填充对象
							 IN int nBufWidth,				// 对象大小
							 IN int nBufHeight,
							 IN int nX,						// 填充区域
							 IN int nY,
							 IN int nWidth,
							 IN int nHeight,
							 IN int nFrame/* = 0*/,			// 颜色动画的帧号，于静态颜色无用
							 IN BYTE* pbyMask/* = NULL*/,	// 掩膜（8位缓存），大小与填充区域相同
							 IN int nMaskPitch/* = 0*/)		// 掩膜缓存的特殊性，pitch可能不等于宽度
{
	// 获取模板绝对目录（模板会自动添加到系统缺省的一个位置，增强管理）
	TCHAR szFilePath[MAX_PATH];
	assert(false);
#if 0
	szFilePath[0] = _T('\0');
	if (g_szTempletPath)
	{
		_tcscpy_s(szFilePath, g_szTempletPath);
	}
	_tcscat_s(szFilePath, pColor->gradual.templet.name);

	int nStartx,nStarty,nImgWidth,nImgHeight;
	if(!BaseCodec_GetImgInfo(szFilePath,nStartx,nStarty,nImgWidth,nImgHeight))
		return;

	int nminw = min(nBufWidth - nX, nWidth);
	int nminh = min(nBufHeight - nY, nHeight);

	
	int nmod = nminw%4;
	if(nmod != 0)
		nmod = 1;

	nminw = (nminw/4 + nmod) * 4;

	int nSize = (int)nminh * (int)nminw;
	BYTE *pbyTemplet = SYSSE_NEW(nSize, BYTE), *pbyTmpTemplet = pbyTemplet;
	
	if(!SYReadMaskImage(szFilePath,pbyTemplet,nminw,nminh))
	{
		SYSSE_FREE(pbyTemplet);
		return;
	}

	
	DWORD *pdwFillBuf = pdwBuf + nY * nBufWidth + nX;
	
	

	// 一行 256 的渐变条
	DWORD dwLineSize = 256;
	DWORD *pdwLine = SYSSE_NEW(dwLineSize, DWORD);

	// 填充渐变色
	SYColor_FillLineBaseAR_1_Recursive(pdwLine, dwLineSize, pColor->gradual.color,
		pColor->gradual.pos, pColor->gradual.mid, pColor->gradual.ncount, 1, 0);
	// 节点颜色动画的测试版
	if ((pColor->gradual.animation & GRAD_ANI_COLOR) != GRAD_ANI_NONE)
	{
		SYColor_ProcAniColor(pdwLine, dwLineSize, nFrame, pColor->gradual.aniparam[GRAD_ANI_PARAM_COLOR], 0);
	}
	// 填充内存
	int nmin = min(nminw,nBufWidth);
	for (int i=0; i<nminh; i++)
	{
		for (int j=0; j<nmin; j++)
		{
			*(pdwFillBuf + j) = *(pdwLine + pbyTmpTemplet[j]);
		}
		pbyTmpTemplet += nminw;
		pdwFillBuf += nBufWidth;
	}
	SYSSE_FREE(pbyTemplet);
	SYSSE_FREE(pdwLine);
#endif
}

// 填充
void SYColor_FillTexImg(IN SYColor* pColor,			// 填充颜色
						OUT DWORD* pdwBuf,			// 填充对象
						IN int nBufWidth,			// 对象大小
						IN int nBufHeight,
						IN int nX,					// 填充区域
						IN int nY,
						IN int nWidth,
						IN int nHeight,
						IN float fScale,
						IN int nFrame/* = 0*/,		// 颜色动画的帧号，于静态颜色无用
						IN BYTE* pbyMask/* = NULL*/,// 掩膜（8位缓存），大小与填充区域相同
						IN int nMaskPitch/* = 0*/)	// 掩膜缓存的特殊性，pitch可能不等于宽度
{
	// 获取图片纹理的绝对目录（图片会自动添加到系统缺省的一个位置，增强管理）
	TCHAR szFilePath[MAX_PATH];
//	SYImageRes tmpRes;
//	SYImageInfo tmpInfo;
	assert(false);
#if 0
	szFilePath[0] = _T('\0');
	GetFileManager()->TagPathTrans(pColor->imgtex.name,szFilePath,GetFileManager()->lpParam);
//	_tcscpy(szFilePath, g_szImagePath);
	int xStart,yStart,nImgWidth,nImgHeight;
	if(!SYGetImgInfo(szFilePath,xStart,yStart,nImgWidth,nImgHeight))
		return;
	nImgWidth = nImgWidth * fScale;
	nImgHeight = nImgHeight * fScale;
	DWORD *pdwSrc = SYSSE_NEW((int)nImgWidth * (int)nImgHeight, DWORD);		// 直接读出 32 位数据
	int nSrcX(0), nSrcY(0), nSrcWidth(nImgWidth), nSrcHeight(nImgHeight);
	int nTmpX(0), nTmpY(0), nStepWidth(nImgWidth), nStepHeight(nImgHeight);
	int nLeftEdge(0), nTopEdge(0), nRightEdge(0), nBottomEdge(0);
	int nHorCount(0), nVerCount(0);

	if(!SYReadImage(szFilePath,pdwSrc,nImgWidth, nImgHeight))
	{
		SYSSE_FREE(pdwSrc);
		return;
	}
	// 根据填充方式填充目标
	if (pColor->imgtex.fill == SYCOLOR_IMG_ZOOM)	// 拉伸
	{
		switch (pColor->imgtex.align)
		{
		case SYCOLOR_ALIGN_ZOOM_WIDTH:		// 按宽度比
			nSrcHeight = (int)(nSrcWidth * nHeight / (float)nWidth + 0.5f);
			nSrcY += (((int)nImgHeight - nSrcHeight) >> 1);
			break;
		case SYCOLOR_ALIGN_ZOOM_HEIGHT:		// 按高度比
			nSrcWidth = (int)(nSrcHeight * nWidth / (float)nHeight + 0.5f);
			nSrcX += (((int)nImgWidth - nSrcWidth) >> 1);
			break;
		case SYCOLOR_ALIGN_ZOOM_FULL:		// 满屏
		default:
			break;
		}
		SY_BitBlt(pdwBuf, nX, nY, nWidth, nHeight, nBufWidth, nBufHeight,
			pdwSrc, nSrcX, nSrcY, nSrcWidth, nSrcHeight, (int)nImgWidth, (int)nImgHeight, SY_BUFFER_COPY, SY_PARITY_BOTH);
	}
	else if (pColor->imgtex.fill == SYCOLOR_IMG_COPY)		// 原始大小拷贝
	{
		switch (pColor->imgtex.align)
		{
		case SYCOLOR_ALIGN_COPY_CENTER:		// 居中
			nSrcX += ((nSrcWidth - nWidth) >> 1);
			nSrcY += ((nSrcHeight - nHeight) >> 1);
			break;
		case SYCOLOR_ALIGN_COPY_LEFTBOTTOM:	// 左下角
			nSrcY += (nSrcHeight - nHeight);
			break;
		case SYCOLOR_ALIGN_COPY_RIGHTTOP:	// 右上角
			nSrcX += (nSrcWidth - nWidth);
			break;
		case SYCOLOR_ALIGN_COPY_RIGHTBOTTOM:	// 右下角
			nSrcX += (nSrcWidth - nWidth);
			nSrcY += (nSrcHeight - nHeight);
			break;
		case SYCOLOR_ALIGN_COPY_LEFTTOP:	// 左上角
		default:
			break;
		}
		// 确保不会越出指定的目标范围，因此采用目标大小为基准
		SY_BitBlt(pdwBuf, nX, nY, nWidth, nHeight, nBufWidth, nBufHeight,
			pdwSrc, nSrcX, nSrcY, nWidth, nHeight, (int)nImgWidth, (int)nImgHeight, SY_BUFFER_COPY, SY_PARITY_BOTH);
	}
	else if (pColor->imgtex.fill == SYCOLOR_IMG_TILES)		// 原始大小排列
	{
		// 要求不能越出指定的目标范围，因此采用目标大小为基准，并对边缘进行特殊的处理
		switch (pColor->imgtex.align)
		{
		case SYCOLOR_ALIGN_COPY_CENTER:			// 居中（会出现四个边缘）
			if (nWidth < nSrcWidth)		// 目标比源小，则让其中心对称
			{
				nSrcX += ((nSrcWidth - nWidth) >> 1);
				nHorCount = 1;
			}
			else	// 否则计算边缘，至少有一个宽
			{
				// 先计算左边
				nTmpX = (nWidth >> 1) - (nSrcWidth >> 1);
				nHorCount = (int)(nTmpX / nSrcWidth);
				nLeftEdge = nTmpX - nHorCount * nSrcWidth;
				// 再计算右边
				nTmpX = (nWidth - nTmpX - nSrcWidth);
				nTmpY = (int)(nTmpX / nSrcWidth);
				nRightEdge = nTmpX - nTmpY * nSrcWidth;
				// 合并平铺的个数
				nHorCount += (nTmpY + 1);
			}
			if (nHeight < nSrcHeight)		// 目标比源小，则让其中心对称
			{
				nSrcY += ((nSrcHeight - nHeight) >> 1);
				nVerCount = 1;
			}
			else	// 否则计算边缘，至少有一个高
			{
				// 先计算顶端
				nTmpY = (nHeight >> 1) - (nSrcHeight >> 1);
				nVerCount = (int)(nTmpY / nSrcHeight);
				nTopEdge = nTmpY - nVerCount * nSrcHeight;
				// 再计算底端
				nTmpY = (nHeight - nTmpY - nSrcHeight);
				nTmpX = (int)(nTmpY / nSrcHeight);
				nBottomEdge = nTmpY - nTmpX * nSrcHeight;
				// 合并平铺的个数
				nVerCount += (nTmpX + 1);
			}
			break;
		case SYCOLOR_ALIGN_COPY_LEFTBOTTOM:		// 左下角（会出现右边缘与顶边缘）
			nHorCount = (int)(nWidth / nSrcWidth);
			nVerCount = (int)(nHeight / nSrcHeight);
			nRightEdge = nWidth - nSrcWidth * nHorCount;
			nTopEdge = nHeight - nSrcHeight * nVerCount;
			break;
		case SYCOLOR_ALIGN_COPY_RIGHTTOP:		// 右上角（会出现左边缘与底边缘）
			nHorCount = (int)(nWidth / nSrcWidth);
			nVerCount = (int)(nHeight / nSrcHeight);
			nLeftEdge = nWidth - nSrcWidth * nHorCount;
			nBottomEdge = nHeight - nSrcHeight * nVerCount;
			break;
		case SYCOLOR_ALIGN_COPY_RIGHTBOTTOM:	// 右下角（会出现左边缘与顶边缘）
			nHorCount = (int)(nWidth / nSrcWidth);
			nVerCount = (int)(nHeight / nSrcHeight);
			nLeftEdge = nWidth - nSrcWidth * nHorCount;
			nTopEdge = nHeight - nSrcHeight * nVerCount;
			break;
		case SYCOLOR_ALIGN_COPY_LEFTTOP:	// 左上角（会出现右边缘与底边缘）
		default:
			nHorCount = (int)(nWidth / nSrcWidth);
			nVerCount = (int)(nHeight / nSrcHeight);
			nRightEdge = nWidth - nSrcWidth * nHorCount;
			nBottomEdge = nHeight - nSrcHeight * nVerCount;
			break;
		}
		// 整体平铺，平铺时要以目标大小为准，因为是原始大小拷贝
		nTmpY = nY + nTopEdge;
		for (int i=0; i<nVerCount; i++)
		{
			nTmpX = nX + nLeftEdge;
			for (int i=0; i<nHorCount; i++)
			{
				SY_BitBlt(pdwBuf, nTmpX, nTmpY, nWidth, nHeight, nBufWidth, nBufHeight,
					pdwSrc, nSrcX, nSrcY, nWidth, nHeight, (int)nImgWidth, (int)nImgHeight, SY_BUFFER_COPY, SY_PARITY_BOTH);
				nTmpX += nStepWidth;
			}
			nTmpY += nStepHeight;
		}
		// 处理边缘及角落
		if (nTopEdge > 0)		// 处理顶边缘，忽略左右边角不完整区域
		{
			nTmpX = nX + nLeftEdge;
			nTmpY = nSrcY + nSrcHeight - nTopEdge;
			for (int i=0; i<nHorCount; i++)
			{
				SY_BitBlt(pdwBuf, nTmpX, nY, nWidth, nTopEdge, nBufWidth, nBufHeight,
					pdwSrc, nSrcX, nTmpY, nWidth, nTopEdge, (int)nImgWidth, (int)nImgHeight, SY_BUFFER_COPY, SY_PARITY_BOTH);
				nTmpX += nStepWidth;
			}
		}
		if (nLeftEdge > 0)		// 处理左边缘，忽略上下边角不完整区域
		{
			nTmpX = nSrcX + nSrcWidth - nLeftEdge;
			nTmpY = nY + nTopEdge;
			for (int i=0; i<nVerCount; i++)
			{
				SY_BitBlt(pdwBuf, nX, nTmpY, nLeftEdge, nHeight, nBufWidth, nBufHeight,
					pdwSrc, nTmpX, nSrcY, nLeftEdge, nHeight, (int)nImgWidth, (int)nImgHeight, SY_BUFFER_COPY, SY_PARITY_BOTH);
				nTmpY += nStepHeight;
			}
		}
		if (nRightEdge > 0)		// 处理右边缘，忽略上下边角不完整区域
		{
			nTmpX = nX + nWidth - nRightEdge;
			nTmpY = nY + nTopEdge;
			for (int i=0; i<nVerCount; i++)
			{
				SY_BitBlt(pdwBuf, nTmpX, nTmpY, nRightEdge, nHeight, nBufWidth, nBufHeight,
					pdwSrc, nSrcX, nSrcY, nRightEdge, nHeight, (int)nImgWidth, (int)nImgHeight, SY_BUFFER_COPY, SY_PARITY_BOTH);
				nTmpY += nStepHeight;
			}
		}
		if (nBottomEdge > 0)		// 处理底边缘，忽略左右边角不完整区域
		{
			nTmpX = nX + nLeftEdge;
			nTmpY = nY + nHeight - nBottomEdge;
			for (int i=0; i<nHorCount; i++)
			{
				SY_BitBlt(pdwBuf, nTmpX, nTmpY, nWidth, nBottomEdge, nBufWidth, nBufHeight,
					pdwSrc, nSrcX, nSrcY, nWidth, nBottomEdge, (int)nImgWidth, (int)nImgHeight, SY_BUFFER_COPY, SY_PARITY_BOTH);
				nTmpX += nStepWidth;
			}
		}
		if (nLeftEdge > 0 && nTopEdge > 0)	// 处理左上角不完整区域
		{
			nTmpX = nSrcX + nSrcWidth - nLeftEdge;
			nTmpY = nSrcY + nSrcHeight - nTopEdge;
			SY_BitBlt(pdwBuf, nX, nY, nLeftEdge, nTopEdge, nBufWidth, nBufHeight,
				pdwSrc, nTmpX, nTmpY, nLeftEdge, nTopEdge, (int)nImgWidth, (int)nImgHeight, SY_BUFFER_COPY, SY_PARITY_BOTH);
		}
		if (nLeftEdge > 0 && nBottomEdge > 0)	// 处理左下角不完整区域
		{
			nTmpX = nSrcX + nSrcWidth - nLeftEdge;
			nTmpY = nY + nHeight - nBottomEdge;
			SY_BitBlt(pdwBuf, nX, nTmpY, nLeftEdge, nBottomEdge, nBufWidth, nBufHeight,
				pdwSrc, nTmpX, nSrcY, nLeftEdge, nBottomEdge, (int)nImgWidth, (int)nImgHeight, SY_BUFFER_COPY, SY_PARITY_BOTH);
		}
		if (nRightEdge > 0 && nTopEdge > 0)	// 处理右上角不完整区域
		{
			nTmpX = nX + nWidth - nRightEdge;
			nTmpY = nSrcY + nSrcHeight - nTopEdge;
			SY_BitBlt(pdwBuf, nTmpX, nY, nRightEdge, nTopEdge, nBufWidth, nBufHeight,
				pdwSrc, nSrcX, nTmpY, nRightEdge, nTopEdge, (int)nImgWidth, (int)nImgHeight, SY_BUFFER_COPY, SY_PARITY_BOTH);
		}
		if (nRightEdge > 0 && nBottomEdge > 0)	// 处理右下角不完整区域
		{
			nTmpX = nX + nWidth - nRightEdge;
			nTmpY = nY + nHeight - nBottomEdge;
			SY_BitBlt(pdwBuf, nTmpX, nTmpY, nRightEdge, nBottomEdge, nBufWidth, nBufHeight,
				pdwSrc, nSrcX, nSrcY, nRightEdge, nBottomEdge, (int)nImgWidth, (int)nImgHeight, SY_BUFFER_COPY, SY_PARITY_BOTH);
		}
	}
	SYSSE_FREE(pdwSrc);
#endif
}
// 颜色动画处理
void SYColor_ProcAniColor(IN OUT DWORD* pdwLine,
						  IN int nWidth,
						  IN int nFrame,
						  IN float fSpeed,
						  IN DWORD dwStyle)
{
	float fOffset = nFrame * fSpeed;
	DWORD* pdwTestBuf = SYSSE_NEW(nWidth, DWORD);
	BOOL bReverse = FALSE;
	int nOffset(0), nOffLen(0);

	fOffset = fOffset - floorf(fOffset * 0.5f) * 2.f;		// [0, 2.f)
	bReverse = ((dwStyle == SYCOLOR_STYLE_SYMMETRY) && (fOffset >= 1.f));
	nOffset = (int)((fOffset - floorf(fOffset)) * nWidth + 0.5f);
	nOffLen = nWidth - nOffset;
	if (bReverse)
	{
		SYMemcpy_RevDWORD(pdwTestBuf + nOffset, pdwLine + nOffset, nOffLen);
		SYMemcpy_Line(pdwTestBuf, pdwLine + nOffLen, (nOffset << 2));
	}
	else
	{
		SYMemcpy_Line(pdwTestBuf + nOffset, pdwLine, (nOffLen << 2));
		SYMemcpy_RevDWORD(pdwTestBuf, pdwLine, nOffset);
	}
	SYMemcpy_Line(pdwLine, pdwTestBuf, (nWidth << 2));
	SYSSE_FREE(pdwTestBuf);
}
  
BASEGRAPH_COMMON_API void DWORDColorToFloat4( float4* pOutColor,DWORD colorIn )
{
	pOutColor->z = GetBGRA_R(colorIn) / 255.f;
	pOutColor->y = GetBGRA_G(colorIn) / 255.f;
	pOutColor->x = GetBGRA_B(colorIn) / 255.f;
	pOutColor->w = GetAValue(colorIn) / 255.f;
}
BASEGRAPH_COMMON_API void Float4ToDWORDColor(DWORD* pColorOut , const float4& ColorIn)
{
	int r = (int)(ColorIn.z * 255) & 0xff;
	int g = (int)(ColorIn.y * 255) & 0xff;
	int b = (int)(ColorIn.x * 255) & 0xff;
	int a = (int)(ColorIn.w * 255) & 0xff;
	*pColorOut = BGRA(b,g,r,a);
}

BASEGRAPH_COMMON_API int SYColor_SaveColor(IN SYColor* pColor,void* pBuffer,int nCount)
{
	int factsize = 0;
	if(pColor->type == SYCOLOR_TEX_IMG)
	{
		TCHAR szNewName[MAX_PATH];
		TCHAR szFilePath[MAX_PATH];
		TCHAR realpath[MAX_PATH];
		//if(PathFile_SaveToStg(pColor->imgtex.name,_T("color"),szFilePath,szNewName,pFileInterface,TRUE))
		//	_tcscpy(pColor->imgtex.name,szFilePath);
	}
	if(pBuffer)
		memcpy(pBuffer,pColor,sizeof(SYColor));
	factsize = sizeof(SYColor);
	return factsize;
}

BASEGRAPH_COMMON_API int SYColor_LoadColor(IN SYColor* pColor,void* pBuffer,int nCount)
{
	int factsize = 0;
	memcpy(pColor,pBuffer,sizeof(SYColor));
	if(pColor->type == SYCOLOR_TEX_IMG)
	{
		//PathFile_LoadFromStg(pColor->imgtex.name,pFileInterface);
	}
	factsize = sizeof(SYColor);
	return factsize;
}

void SYColor_CreateLinearDefault(OUT SYColor* pColor)	// 目标
{
	//_SYASSERT(pColor != NULL);
	SYMemset_LineChar(pColor, 0, sizeof(SYColor));
	pColor->type = SYCOLOR_GRAD_LINEAR;
	pColor->gradual.color[0] = DEFAULT_GRAD_START_COLOR;
	pColor->gradual.color[1] = DEFAULT_GRAD_END_COLOR;
	pColor->gradual.ncount = DEFAULT_GRAD_PEG_COUNT_2;
	pColor->gradual.pos[0] = DEFAULT_GRAD_START_POS;
	pColor->gradual.pos[1] = DEFAULT_GRAD_END_POS;
	pColor->gradual.mid[0] = DEFAULT_GRAD_MID;
	pColor->gradual.linear.frad = DFFAULT_GRAD_LINEAR_RADIAN;
	pColor->gradual.linear.repeat = DEFAULT_GRAD_RAPEAT;
	pColor->gradual.linear.style = SYCOLOR_STYLE_SYMMETRY;
}
void SYColor_CreateCenterRoundDefault(OUT SYColor* pColor)	// 目标
{
	//_SYASSERT(pColor != NULL);
	SYMemset_LineChar(pColor, 0, sizeof(SYColor));
	pColor->type = SYCOLOR_GRAD_CENTER_ROUND;
	pColor->gradual.color[0] = DEFAULT_GRAD_START_COLOR;
	pColor->gradual.color[1] = DEFAULT_GRAD_END_COLOR;
	pColor->gradual.ncount = DEFAULT_GRAD_PEG_COUNT_2;
	pColor->gradual.pos[0] = DEFAULT_GRAD_START_POS;
	pColor->gradual.pos[1] = DEFAULT_GRAD_END_POS;
	pColor->gradual.mid[0] = DEFAULT_GRAD_MID;
	pColor->gradual.center.centerx = DEFAULT_GRAD_CENTER_XY;
	pColor->gradual.center.centery = DEFAULT_GRAD_CENTER_XY;
	pColor->gradual.center.repeat = DEFAULT_GRAD_RAPEAT;
	pColor->gradual.center.style = SYCOLOR_STYLE_SYMMETRY;
	pColor->gradual.center.nvertex = 0;
}
void SYColor_CreateCenterPolygonDefault(OUT SYColor* pColor)
{
	//_SYASSERT(pColor != NULL);
	SYMemset_LineChar(pColor, 0, sizeof(SYColor));
	pColor->type = SYCOLOR_GRAD_CENTER_POLYGON;
	pColor->gradual.color[0] = DEFAULT_GRAD_START_COLOR;
	pColor->gradual.color[1] = DEFAULT_GRAD_END_COLOR;
	pColor->gradual.ncount = DEFAULT_GRAD_PEG_COUNT_2;
	pColor->gradual.pos[0] = DEFAULT_GRAD_START_POS;
	pColor->gradual.pos[1] = DEFAULT_GRAD_END_POS;
	pColor->gradual.mid[0] = DEFAULT_GRAD_MID;
	pColor->gradual.center.centerx = DEFAULT_GRAD_CENTER_XY;
	pColor->gradual.center.centery = DEFAULT_GRAD_CENTER_XY;
	pColor->gradual.center.repeat = DEFAULT_GRAD_RAPEAT;
	pColor->gradual.center.style = SYCOLOR_STYLE_SYMMETRY;
	pColor->gradual.center.nvertex = DEFAULT_GRAD_PEG_COUNT_4;
	pColor->gradual.center.xpt[0] = DEFAULT_GRAD_XY_MIN;
	pColor->gradual.center.ypt[0] = DEFAULT_GRAD_XY_MIN;
	pColor->gradual.center.xpt[1] = DEFAULT_GRAD_XY_MAX;
	pColor->gradual.center.ypt[1] = DEFAULT_GRAD_XY_MIN;
	pColor->gradual.center.xpt[2] = DEFAULT_GRAD_XY_MAX;
	pColor->gradual.center.ypt[2] = DEFAULT_GRAD_XY_MAX;
	pColor->gradual.center.xpt[3] = DEFAULT_GRAD_XY_MIN;
	pColor->gradual.center.ypt[3] = DEFAULT_GRAD_XY_MAX;
}
void SYColor_CreateOutlineCircleDefault(OUT SYColor* pColor)
{
	//_SYASSERT(pColor != NULL);
	SYMemset_LineChar(pColor, 0, sizeof(SYColor));
	pColor->type = SYCOLOR_GRAD_OUTLINE_CIRCLE;
	pColor->gradual.color[0] = DEFAULT_GRAD_START_COLOR;
	pColor->gradual.color[1] = DEFAULT_GRAD_END_COLOR;
	pColor->gradual.ncount = DEFAULT_GRAD_PEG_COUNT_2;
	pColor->gradual.pos[0] = DEFAULT_GRAD_START_POS;
	pColor->gradual.pos[1] = DEFAULT_GRAD_END_POS;
	pColor->gradual.mid[0] = DEFAULT_GRAD_MID;
	pColor->gradual.outline.centerx = DEFAULT_GRAD_CENTER_XY;
	pColor->gradual.outline.centery = DEFAULT_GRAD_CENTER_XY;
	pColor->gradual.outline.repeat = DEFAULT_GRAD_RAPEAT;
	pColor->gradual.outline.style = SYCOLOR_STYLE_SYMMETRY;
}
void SYColor_CreateOutlinePolygonDefault(OUT SYColor* pColor)
{
	//_SYASSERT(pColor != NULL);
	SYMemset_LineChar(pColor, 0, sizeof(SYColor));
	pColor->type = SYCOLOR_GRAD_OUTLINE_POLYGON;
	pColor->gradual.color[0] = DEFAULT_GRAD_START_COLOR;
	pColor->gradual.color[1] = DEFAULT_GRAD_END_COLOR;
	pColor->gradual.color[2] = DEFAULT_GRAD_START_COLOR;
	pColor->gradual.color[3] = DEFAULT_GRAD_END_COLOR;
	pColor->gradual.ncount = DEFAULT_GRAD_PEG_COUNT_4;
	pColor->gradual.pos[0] = DEFAULT_GRAD_START_POS;
	pColor->gradual.pos[1] = DEFAULT_GRAD_POS_1_4;
	pColor->gradual.pos[2] = DEFAULT_GRAD_POS_3_4;
	pColor->gradual.pos[3] = DEFAULT_GRAD_END_POS;
	pColor->gradual.mid[0] = DEFAULT_GRAD_MID;
	pColor->gradual.mid[1] = DEFAULT_GRAD_MID;
	pColor->gradual.mid[2] = DEFAULT_GRAD_MID;
	pColor->gradual.outline.centerx = DEFAULT_GRAD_CENTER_XY;
	pColor->gradual.outline.centery = DEFAULT_GRAD_CENTER_XY;
	pColor->gradual.outline.xpt[0] = DEFAULT_GRAD_XY_MIN;
	pColor->gradual.outline.ypt[0] = DEFAULT_GRAD_XY_MIN;
	pColor->gradual.outline.xpt[1] = DEFAULT_GRAD_XY_MAX;
	pColor->gradual.outline.ypt[1] = DEFAULT_GRAD_XY_MIN;
	pColor->gradual.outline.xpt[2] = DEFAULT_GRAD_XY_MAX;
	pColor->gradual.outline.ypt[2] = DEFAULT_GRAD_XY_MAX;
	pColor->gradual.outline.xpt[3] = DEFAULT_GRAD_XY_MIN;
	pColor->gradual.outline.ypt[3] = DEFAULT_GRAD_XY_MAX;
}
void SYColor_CreateMultiPtDefault(OUT SYColor* pColor, IN DWORD dwType)			// 多点渐变色的那几个种类
{
	//_SYASSERT(pColor != NULL);
	SYMemset_LineChar(pColor, 0, sizeof(SYColor));
	pColor->type = dwType;
	pColor->gradual.color[0] = DEFAULT_GRAD_START_COLOR;
	pColor->gradual.color[1] = DEFAULT_GRAD_END_COLOR;
	pColor->gradual.color[2] = DEFAULT_GRAD_START_COLOR;
	pColor->gradual.color[3] = DEFAULT_GRAD_END_COLOR;
	pColor->gradual.ncount = DEFAULT_GRAD_PEG_COUNT_4;
	pColor->gradual.pos[0] = DEFAULT_GRAD_START_POS;
	pColor->gradual.pos[1] = DEFAULT_GRAD_POS_1_4;
	pColor->gradual.pos[2] = DEFAULT_GRAD_POS_3_4;
	pColor->gradual.pos[3] = DEFAULT_GRAD_END_POS;
	pColor->gradual.mid[0] = DEFAULT_GRAD_MID;
	pColor->gradual.mid[1] = DEFAULT_GRAD_MID;
	pColor->gradual.mid[2] = DEFAULT_GRAD_MID;
	pColor->gradual.multipt.xpt[0] = DEFAULT_GRAD_XY_MIN;
	pColor->gradual.multipt.ypt[0] = DEFAULT_GRAD_XY_MIN;
	pColor->gradual.multipt.xpt[1] = DEFAULT_GRAD_XY_MAX;
	pColor->gradual.multipt.ypt[1] = DEFAULT_GRAD_XY_MIN;
	pColor->gradual.multipt.xpt[2] = DEFAULT_GRAD_XY_MAX;
	pColor->gradual.multipt.ypt[2] = DEFAULT_GRAD_XY_MAX;
	pColor->gradual.multipt.xpt[3] = DEFAULT_GRAD_XY_MIN;
	pColor->gradual.multipt.ypt[3] = DEFAULT_GRAD_XY_MAX;
}
void SYColor_CreateTempletDefault(OUT SYColor* pColor)
{
	//_SYASSERT(pColor != NULL);
	SYMemset_LineChar(pColor, 0, sizeof(SYColor));
	pColor->type = SYCOLOR_GRAD_TEMPLET;
	pColor->gradual.color[0] = DEFAULT_GRAD_START_COLOR;
	pColor->gradual.color[1] = DEFAULT_GRAD_END_COLOR;
	pColor->gradual.ncount = DEFAULT_GRAD_PEG_COUNT_2;
	pColor->gradual.pos[0] = DEFAULT_GRAD_START_POS;
	pColor->gradual.pos[1] = DEFAULT_GRAD_END_POS;
	pColor->gradual.mid[0] = DEFAULT_GRAD_MID;
	pColor->gradual.templet.name[0] = 0;
}
void SYColor_CreateTexImgDefault(OUT SYColor* pColor)
{
	//_SYASSERT(pColor != NULL);
	SYMemset_LineChar(pColor, 0, sizeof(SYColor));
	pColor->type = SYCOLOR_TEX_IMG;
	pColor->imgtex.name[0] = 0;
	pColor->imgtex.fill = SYCOLOR_IMG_ZOOM;
	pColor->imgtex.align = SYCOLOR_ALIGN_ZOOM_FULL;
}
BASEGRAPH_COMMON_API void SYColor_CreateDefault(OUT SYColor* pColor, IN DWORD dwType)	// 颜色种类
{
	switch(dwType)
	{
	case SYCOLOR_NONE:
	case SYCOLOR_HOMO:
		SYColor_CreateHomo(pColor, DEFAULT_HOMO_COLOR);
		break;
	case SYCOLOR_GRAD_LINEAR:
		SYColor_CreateLinearDefault(pColor);
		break;
	case SYCOLOR_GRAD_CENTER_ROUND:
		SYColor_CreateCenterRoundDefault(pColor);
		break;
	case SYCOLOR_GRAD_CENTER_POLYGON:
		SYColor_CreateCenterPolygonDefault(pColor);
		break;
	case SYCOLOR_GRAD_OUTLINE_CIRCLE:
		SYColor_CreateOutlineCircleDefault(pColor);
		break;
	case SYCOLOR_GRAD_OUTLINE_POLYGON:
		SYColor_CreateOutlinePolygonDefault(pColor);
		break;
	case SYCOLOR_GRAD_PT_CONTOUR:
	case SYCOLOR_GRAD_PT_BLEND:
		SYColor_CreateMultiPtDefault(pColor, dwType);
		break;
	case SYCOLOR_GRAD_TEMPLET:
		SYColor_CreateTempletDefault(pColor);
		break;
	case SYCOLOR_TEX_IMG:
		SYColor_CreateTexImgDefault(pColor);
		break;
	default:
		break;
		//_SYASSERT(FALSE);
	}
}
