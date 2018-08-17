//////////////////////////////////////////////////////////////////////////
// Copyright (c) Shineon Corporation. All rights reserved.
//
// Description:	仿CDC类，实现对32bit缓存的绘制功能。内部颜色为0xARGB格式。
//
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Common\2DBufferDC.h"
#include "Common\ColorDef.h"
#include "Common\BufferFunction.h"
#include <math.h>
#include <wingdi.h>
#include <winuser.h>
#include <assert.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
#pragma comment (lib, "Gdi32.lib")
#pragma comment (lib, "User32.lib")

//////////////////////////////////////////////////////////////////////////
// 宏、常量与枚举定义

// 线型（表达线型有两种方式：采用位比较、采用数组点比较。此处采用数组点比较）
static const int PATTERN_SOLID[] = { 100 };
static const int PATTERN_DASH[] = { 8, 2 };
static const int PATTERN_DOT[] = { 2, 2 };
static const int PATTERN_DASHDOT[] = { 8, 2, 2, 2 };
static const int PATTERN_DASHDOTDOT[] = { 8, 2, 2, 2, 2, 2 };
static const int PATTERN_NULL[] = { 0, 1000 };
static const int* PATTERN_PTR[] = {PATTERN_SOLID, PATTERN_DASH, PATTERN_DOT, PATTERN_DASHDOT, PATTERN_DASHDOTDOT, PATTERN_NULL,};
static const int PATTERN_COUNT[] = { 1, 2, 2, 4, 6, 2, };

#define LINE_PATTERN_MASK		0x07		// 线型掩码
#define LINE_PATTERN_MAX		PS_NULL		// 当前支持的线型
// 光栅化相关定义
enum RASTER_FLAG_DEF
{
	// 标志位的 0-3 位数据表示直线的斜率
	SLOPE_UNDER_TAN = 0x00000001,	// 斜率小于 1
	SLOPE_ABOVE_TAN,				// 斜率大于 1
	SLOPE_EQUAL_TAN,				// 斜率等于 1
	SLOPE_EQUAL_ZERO,				// 斜率等于 0（水平）
	SLOPE_EQUAL_INFINITE,			// 斜率无穷大（垂直）
	SLOPE_TYPE_MASK = 0x0000000f,	// 斜率掩码
	// 标志位的 4-7 位数据表示直线的增长方向
	X_INCREASE_NEGATIVE = 0x00000080,	// X 轴负增长
	Y_INCREASE_NEGATIVE = 0x00000040,	// Y 轴负增长

	LINE_IS_INVISIBLE = 0xffffffff,		// 线段不可见
};
// 点栈的相关定义
#define DEFAULT_POINT_STACK_SIZE	1024	// 缺省的栈大小
// 贝塞尔曲线相关定义
#define POINT_BEZIER_COUNT		4			// 一条贝塞尔曲线的表示点个数
#define BEZIER_DIST_LIMIT		0.25f		// 误差极限的平方（半个像素）

enum SYTEXT_DIRECTION		// 文本方向
{
	SYTEXT_DIRECTION_ANGLE_0 = 0,	// 0度
	SYTEXT_DIRECTION_QUADRANT_1,	// 第一象限
	SYTEXT_DIRECTION_ANGLE_90,		// 90度
	SYTEXT_DIRECTION_QUADRANT_2,	// 第二象限
	SYTEXT_DIRECTION_ANGLE_180,		// 180度
	SYTEXT_DIRECTION_QUADRANT_3,	// 第三象限
	SYTEXT_DIRECTION_ANGLE_270,		// 270度
	SYTEXT_DIRECTION_QUADRANT_4,	// 第四象限
};

//////////////////////////////////////////////////////////////////////////
// CCommonBufferDC

CCommonBufferDC::CCommonBufferDC(void)
	: m_bAttached(FALSE)			// 绑定参数
	, m_pdwBuffer(NULL)
	, m_nWidth(0)
	, m_nHeight(0)
	, m_fZoomRef(1.f)
	, m_nPenStyle(PS_SOLID)			// 缺省画笔
	, m_nPenWidth(1)
	, m_dwPenColor(0xff000000)
	, m_nDrawMode(R2_COPYPEN)		// 缺省背景
	, m_nBkMode(OPAQUE)
	, m_dwBkColor(0xff000000)
	, m_nFillMode(TRANSPARENT)		// 缺省填充
	, m_dwFillColor(0xff000000)
	, m_pPattern(NULL)				// 缺省线型（实线）
	, m_nPatCount(0)
	, m_hFont(NULL)					// 文本属性
	, m_dwTextColor(0xff000000)
	, m_nTextDirection(SYTEXT_DIRECTION_ANGLE_0)
	, m_fTextAngleSin(0.f)
	, m_fTextAngleCos(1.f)
	, m_hMemDC(NULL)				// GDI相关参数
	, m_hMemBM(NULL)
	, m_nMemWidth(0)
	, m_nMemHeight(0)
	, m_pdwMemBM(NULL)
	, m_hOldBM(NULL)
	, m_pPtStack(NULL)				// 点栈
	, m_nPtStackSize(0)
	, m_nTop(0)
{
	// 清零可见区域
	memset(&m_rcClip, 0, sizeof(RECT));
	// 清零绘制后的有效图像区域
	memset(&m_rcValid, 0, sizeof(RECT));
	// 缺省绘制参考
	m_ptOffsetRef.x = m_ptOffsetRef.y = 0L;
	m_nBkMode = TRANSPARENT;
	// 缺省起点
	m_ptStart.x = m_ptStart.y = 0L;
	// 缺省线型（实线）
	CopyLinePattern(PATTERN_PTR[m_nPenStyle], PATTERN_COUNT[m_nPenStyle]);
	// 缺省点栈
	m_nPtStackSize = DEFAULT_POINT_STACK_SIZE;
	m_pPtStack = new POINT[m_nPtStackSize];
}

CCommonBufferDC::~CCommonBufferDC(void)
{
	DetachBuffer();
	// 清空内存
	delete[] m_pPattern;
	delete[] m_pPtStack;
	m_pPattern = NULL;
	m_pPtStack = NULL;
}

//////////////////////////////////////////////////////////////////////////
// Public

//////////////////////////////////////////////////////////////////////////
// 通用属性访问接口

// 绑定 Buffer，如果已经有绑定返回 FALSE
BOOL CCommonBufferDC::AttachBuffer(IN DWORD *pdwBuffer,		// 指定内存
							   IN int nWidth,			// 内存大小
							   IN int nHeight)
{
	//assert(pdwBuffer != NULL && nWidth >= 0 && nHeight >= 0);

	if (m_bAttached)		// 必须先 detach
		return FALSE;

	// 先设定标志
	m_bAttached = TRUE;
	// 初始化相关变量
	m_pdwBuffer = pdwBuffer;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	SetRect(&m_rcClip, 0, 0, nWidth, nHeight);
	InitValidRect(NULL);		// 未绘制，则无有效图像区域
	m_ptOffsetRef.x = m_ptOffsetRef.y = 0L;
	m_fZoomRef = 1.f;

	return TRUE;
}
// 解除绑定（返回绑定的内存）
DWORD* CCommonBufferDC::DetachBuffer()
{
	if (!m_bAttached)
		return NULL;

	DWORD *pTmp = m_pdwBuffer;

	// 清空变量
	m_pdwBuffer = NULL;
	m_nWidth = m_nHeight = 0;
	memset(&m_rcClip, 0, sizeof(RECT));
	InitValidRect(NULL);
	m_ptOffsetRef.x = m_ptOffsetRef.y = 0L;
	m_fZoomRef = 1.f;
	// 解除GDI帮助
	EnableGDIHelp(FALSE, 0, 0);
	// 最后清除标志
	m_bAttached = FALSE;

	return pTmp;
}

// 设置绘制的裁剪区域，输入 NULL 时为全屏可见，裁剪区域为左上闭右下开。
void CCommonBufferDC::SetClipRect(IN const RECT* prcClip)
{
	if (prcClip == NULL)	// 设置 NULL 时为全屏可见
	{
		m_rcClip.left = 0L;
		m_rcClip.right = m_nWidth;
		m_rcClip.top = 0L;
		m_rcClip.bottom = m_nHeight;
	}
	else	// 否则规则化可见区域
	{
		memcpy(&m_rcClip, prcClip, sizeof(RECT));

		//DSRectL_NormalizeRect((LPDSRECTL)&m_rcClip);
		if (m_rcClip.left > m_rcClip.right)
		{
			m_rcClip.left += m_rcClip.right;
			m_rcClip.right = m_rcClip.left - m_rcClip.right;
			m_rcClip.left -= m_rcClip.right;
		}
		if (m_rcClip.top > m_rcClip.bottom)
		{
			m_rcClip.top += m_rcClip.bottom;
			m_rcClip.bottom = m_rcClip.top - m_rcClip.right;
			m_rcClip.top -= m_rcClip.bottom;
		}

		m_rcClip.left = (m_rcClip.left > 0L) ? m_rcClip.left : 0L;
		m_rcClip.right = (m_rcClip.right > m_nWidth) ? m_nWidth : m_rcClip.right;
		m_rcClip.top = (m_rcClip.top > 0L) ? m_rcClip.top : 0L;
		m_rcClip.bottom = (m_rcClip.bottom > m_nHeight) ? m_nHeight : m_rcClip.bottom;
	}
}

// 设置获取绘制后的有效图像区域，用于最后提取有效图像（左上闭右下开）。
void CCommonBufferDC::InitValidRect(IN const RECT *prcValid)	// 为 NULL 时表示空矩形
{
	if (prcValid == NULL)
	{
		memset(&m_rcValid, 0, sizeof(RECT));
	}
	else	// 裁减至可见区域内
	{
		m_rcValid.left = (LONG)ClipPixelX((int)prcValid->left);
		m_rcValid.right = (LONG)ClipPixelX((int)prcValid->right);
		m_rcValid.top = (LONG)ClipPixelY((int)prcValid->top);
		m_rcValid.bottom = (LONG)ClipPixelY((int)prcValid->bottom);
	}
}
void CCommonBufferDC::AddValidRect(IN const RECT &rcValid)
{
	m_rcValid.left = (LONG)ClipPixelX((int)Common_Min(m_rcValid.left, rcValid.left));
	m_rcValid.right = (LONG)ClipPixelX((int)Common_Max(m_rcValid.right, rcValid.right));
	m_rcValid.top = (LONG)ClipPixelY((int)Common_Min(m_rcValid.top, rcValid.top));
	m_rcValid.bottom = (LONG)ClipPixelY((int)Common_Max(m_rcValid.bottom, rcValid.bottom));
}

// 设置画笔的属性（返回旧属性）
int CCommonBufferDC::SetPenStyle(IN int nPenStyle)		// 兼容 GDI
{
	nPenStyle &= PS_STYLE_MASK;
	if (m_nPenStyle == nPenStyle)
		return m_nPenStyle;

	int nTmp = m_nPenStyle;

	m_nPenStyle = nPenStyle;
	if (nPenStyle <= LINE_PATTERN_MAX)		// 切换线型
	{
		CopyLinePattern(PATTERN_PTR[nPenStyle], PATTERN_COUNT[nPenStyle]);
	}
	return nTmp;
}
void CCommonBufferDC::SetPen(IN int nPenStyle, IN int nWidth, IN DWORD dwColor)
{
	nPenStyle &= LINE_PATTERN_MASK;
	if (nPenStyle <= LINE_PATTERN_MAX && m_nPenStyle != nPenStyle)		// 切换线型
	{
		CopyLinePattern(PATTERN_PTR[nPenStyle], PATTERN_COUNT[nPenStyle]);
	}
	m_nPenStyle = nPenStyle;
	m_nPenWidth = nWidth;
	m_dwPenColor = dwColor;
}

// 文本属性
HFONT CCommonBufferDC::SetFont(IN HFONT hNewFont)		// 新字体
{
	HFONT hOldFont = m_hFont;
	LOGFONT stuLogFont;
	LONG lfValidAngle(0L);
	float fAngle(0.f);

	m_hFont = hNewFont;
	// 获取新字体的文本方向
	memset(&stuLogFont, 0, sizeof(LOGFONT));
	::GetObject((HGDIOBJ)m_hFont, sizeof(LOGFONT), &stuLogFont);
	// 规则化文本方向
	lfValidAngle = (stuLogFont.lfEscapement % 3600L);
	lfValidAngle = (lfValidAngle < 0L) ? (lfValidAngle + 3600L) : lfValidAngle;
	// 根据角度值计算文本的方向，字体的角度值被放大十倍。
	fAngle = lfValidAngle * 0.1f * ANGLE_TO_RADIAN;
	m_fTextAngleSin = sinf(fAngle);
	m_fTextAngleCos = cosf(fAngle);
	if (lfValidAngle == 0)		// 水平向右
	{
		m_nTextDirection = SYTEXT_DIRECTION_ANGLE_0;
	}
	else if (lfValidAngle < 900)	// 第一象限
	{
		m_nTextDirection = SYTEXT_DIRECTION_QUADRANT_1;
	}
	else if (lfValidAngle == 900)	// 垂直向上
	{
		m_nTextDirection = SYTEXT_DIRECTION_ANGLE_90;
	}
	else if (lfValidAngle < 1800)	// 第二角限
	{
		m_nTextDirection = SYTEXT_DIRECTION_QUADRANT_2;
	}
	else if (lfValidAngle == 1800)		// 水平向左
	{
		m_nTextDirection = SYTEXT_DIRECTION_ANGLE_180;
	}
	else if (lfValidAngle < 2700)		// 第三象限
	{
		m_nTextDirection = SYTEXT_DIRECTION_QUADRANT_3;
	}
	else if (lfValidAngle ==  2700)		// 垂直向下
	{
		m_nTextDirection = SYTEXT_DIRECTION_ANGLE_270;
	}
	else		// 第四象限
	{
		m_nTextDirection = SYTEXT_DIRECTION_QUADRANT_4;
	}
	return hOldFont;
}
SIZE CCommonBufferDC::GetTextExtent(IN LPCTSTR lpszString,		// 文本
								IN int nCount)
{
	SIZE szTmp;

	szTmp.cx = szTmp.cy = 0L;
	if (EnableGDIHelp(TRUE, 0, 0))	// 使能GDI绘制帮助
	{
		HFONT hOldFont = (HFONT)::SelectObject(m_hMemDC, (HGDIOBJ)m_hFont);

		::GetTextExtentPoint32(m_hMemDC, lpszString, nCount, &szTmp);
		::SelectObject(m_hMemDC, (HGDIOBJ)hOldFont);
	}
	return szTmp;
}

// 使能GDI绘制帮助
BOOL CCommonBufferDC::EnableGDIHelp(IN BOOL bEnable,		// 使能标志
								IN int nDCWidth,		// 缓存DC大小
								IN int nDCHeight)
{
	if (!m_bAttached)		// 未绑定无法创建
		return FALSE;

	if (bEnable)		// 使能GDI绘制帮助
	{
		if (m_hMemDC == NULL)		// 已经创建DC则不需创建DC
		{
			HDC hTopDC = ::GetDC(NULL);		// 获取屏幕DC，创建该DC的兼容缓存。

			if (hTopDC == NULL)		// 无法获取屏幕DC则无法使用GDI绘制帮助
				return FALSE;

			m_hMemDC = ::CreateCompatibleDC(hTopDC);
			::ReleaseDC(NULL, hTopDC);
		}
		// 当前缓存大小如果不够，则重新创建，否则使用以前的缓存。
		if (nDCHeight > m_nMemHeight || nDCWidth > m_nMemWidth)
		{
			BITMAPINFO stuInfo;

			// 填充DIB缓存信息
			memset(&stuInfo, 0, sizeof(BITMAPINFO));
			stuInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			stuInfo.bmiHeader.biWidth = nDCWidth;
			stuInfo.bmiHeader.biHeight = -nDCHeight;		// 为了适应屏幕坐标系
			stuInfo.bmiHeader.biPlanes = 1;
			stuInfo.bmiHeader.biBitCount = 32;
			stuInfo.bmiHeader.biCompression = BI_RGB;
			// 重新创建DIB缓存
			ReleaseDIB();
			m_hMemBM = ::CreateDIBSection(m_hMemDC, &stuInfo, DIB_RGB_COLORS, (void**)&m_pdwMemBM, NULL, 0);
			if (m_hMemBM == NULL)
				return FALSE;

			m_nMemWidth = nDCWidth;
			m_nMemHeight = nDCHeight;
			m_hOldBM = (HBITMAP)::SelectObject(m_hMemDC, (HGDIOBJ)m_hMemBM);
		}
	}
	else		// 禁止GDI绘制帮助
	{
		if (m_hMemDC == NULL)	// 未创建则不需释放
			return TRUE;

		ReleaseDIB();
		::DeleteDC(m_hMemDC);
		m_hMemDC = NULL;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 客户坐标系下的绘制接口

// 画三次贝塞尔曲线（如果数组大小为 3 * n，则自动闭合曲线)
void CCommonBufferDC::DrawBezier3(IN const pointf *pPtBezier,		// 曲线点与控制点的数组（P0 C1 C2 P1 C3 C4 P2 ……）
							  IN int nPtCount)					// 数组大小（3 * n + 1）
{
	if (!m_bAttached || (pPtBezier == NULL) || (nPtCount < POINT_BEZIER_COUNT))
		return;

	pointf pPtOrigin[POINT_BEZIER_COUNT];
	int nPatIndex(0), nPatItem(0), nPtStart(0);
	int nLen = sizeof(pointf) * POINT_BEZIER_COUNT;

	nPtCount -= 3;
	for (nPtStart=0; nPtStart<nPtCount; nPtStart+=3)
	{
		// 读取一段贝塞尔曲线并进行坐标转换
		SYMemcpy_Line(pPtOrigin, ((pointf*)pPtBezier + nPtStart), nLen);
		for (int i=0; i<POINT_BEZIER_COUNT; i++)
		{
			ClientToScreen(pPtOrigin[i]);
		}
		// 递归画出该曲线
		RecursionBezier3(pPtOrigin, m_pPattern, m_nPatCount, nPatIndex, nPatItem, m_dwPenColor, m_nBkMode, m_dwBkColor);
	}
	if (nPtStart == nPtCount)	// 数组大小为 3 * n，则自动闭合曲线
	{
		// 读取一段贝塞尔曲线并进行坐标转换
		SYMemcpy_Line(pPtOrigin, ((pointf*)pPtBezier + nPtStart), sizeof(pointf) * 3);
		pPtOrigin[3].x = pPtBezier[0].x;		// 闭合时使用第一个点
		pPtOrigin[3].y = pPtBezier[0].y;
		for (int i=0; i<POINT_BEZIER_COUNT; i++)
		{
			ClientToScreen(pPtOrigin[i]);
		}
		// 递归画出该曲线
		RecursionBezier3(pPtOrigin, m_pPattern, m_nPatCount, nPatIndex, nPatItem, m_dwPenColor, m_nBkMode, m_dwBkColor);
	}
}
void CCommonBufferDC::DrawBezier3(IN const POINT *pPtBezier,		// 曲线点与控制点的数组（P0 C1 C2 P1 C3 C4 P2 ……）
							  IN int nPtCount)					// 数组大小（3 * n + 1）
{
	if (!m_bAttached || (pPtBezier == NULL) || (nPtCount < POINT_BEZIER_COUNT))
		return;

	pointf pPtOrigin[POINT_BEZIER_COUNT];
	POINT *pPtTmp = NULL;
	int nPatIndex(0), nPatItem(0), nPtStart(0);

	nPtCount -= 3;
	for (nPtStart=0; nPtStart<nPtCount; nPtStart+=3)
	{
		// 读取一段贝塞尔曲线并进行坐标转换
		pPtTmp = (POINT*)pPtBezier + nPtStart;
		pPtOrigin[0].x = (float)pPtTmp[0].x;
		pPtOrigin[0].y = (float)pPtTmp[0].y;
		pPtOrigin[1].x = (float)pPtTmp[1].x;
		pPtOrigin[1].y = (float)pPtTmp[1].y;
		pPtOrigin[2].x = (float)pPtTmp[2].x;
		pPtOrigin[2].y = (float)pPtTmp[2].y;
		pPtOrigin[3].x = (float)pPtTmp[3].x;
		pPtOrigin[3].y = (float)pPtTmp[3].y;
		for (int i=0; i<POINT_BEZIER_COUNT; i++)
		{
			ClientToScreen(pPtOrigin[i]);
		}
		// 递归画出该曲线
		RecursionBezier3(pPtOrigin, m_pPattern, m_nPatCount, nPatIndex, nPatItem, m_dwPenColor, m_nBkMode, m_dwBkColor);
	}
	if (nPtStart == nPtCount)	// 数组大小为 3 * n，则自动闭合曲线
	{
		// 读取一段贝塞尔曲线并进行坐标转换
		pPtTmp = (POINT*)pPtBezier + nPtStart;
		pPtOrigin[0].x = (float)pPtTmp[0].x;
		pPtOrigin[0].y = (float)pPtTmp[0].y;
		pPtOrigin[1].x = (float)pPtTmp[1].x;
		pPtOrigin[1].y = (float)pPtTmp[1].y;
		pPtOrigin[2].x = (float)pPtTmp[2].x;
		pPtOrigin[2].y = (float)pPtTmp[2].y;
		pPtOrigin[3].x = (float)pPtBezier[0].x;		// 闭合时使用第一个点
		pPtOrigin[3].y = (float)pPtBezier[0].y;
		for (int i=0; i<POINT_BEZIER_COUNT; i++)
		{
			ClientToScreen(pPtOrigin[i]);
		}
		// 递归画出该曲线
		RecursionBezier3(pPtOrigin, m_pPattern, m_nPatCount, nPatIndex, nPatItem, m_dwPenColor, m_nBkMode, m_dwBkColor);
	}
}
// 在绘制三次贝塞尔曲线时使用投影矩阵，如果"pMat == NULL"，则进入正常绘制流程。
void CCommonBufferDC::DrawBezier3(IN const pointf *pPtBezier,		// 曲线点与控制点的数组（P0 C1 C2 P1 C3 C4 P2 ……）
							  IN int nPtCount,					// 数组大小（3 * n + 1）
							  IN const matrix4f* pMat)		// 投影矩阵
{
	if (!m_bAttached || (pPtBezier == NULL) || (nPtCount < POINT_BEZIER_COUNT))
		return;

	if (pMat == NULL)
	{
		DrawBezier3(pPtBezier, nPtCount);
	}
	else
	{
		pointf pPtOrigin[POINT_BEZIER_COUNT];
		int nPatIndex(0), nPatItem(0), nPtStart(0);
		int nLen = sizeof(pointf) * POINT_BEZIER_COUNT;

		nPtCount -= 3;
		for (nPtStart=0; nPtStart<nPtCount; nPtStart+=3)
		{
			// 读取一段贝塞尔曲线并进行坐标转换
			SYMemcpy_Line(pPtOrigin, ((pointf*)pPtBezier + nPtStart), nLen);
			for (int i=0; i<POINT_BEZIER_COUNT; i++)
			{
				ClientToScreen(pPtOrigin[i]);
			}
			// 递归画出该曲线
			RecursionBezier3(pPtOrigin, (matrix4f*)pMat, m_pPattern, m_nPatCount, nPatIndex, nPatItem, m_dwPenColor, m_nBkMode, m_dwBkColor);
		}
		if (nPtStart == nPtCount)	// 数组大小为 3 * n，则自动闭合曲线
		{
			// 读取一段贝塞尔曲线并进行坐标转换
			SYMemcpy_Line(pPtOrigin, ((pointf*)pPtBezier + nPtStart), sizeof(pointf) * 3);
			pPtOrigin[3].x = pPtBezier[0].x;		// 闭合时使用第一个点
			pPtOrigin[3].y = pPtBezier[0].y;
			for (int i=0; i<POINT_BEZIER_COUNT; i++)
			{
				ClientToScreen(pPtOrigin[i]);
			}
			// 递归画出该曲线
			RecursionBezier3(pPtOrigin, (matrix4f*)pMat, m_pPattern, m_nPatCount, nPatIndex, nPatItem, m_dwPenColor, m_nBkMode, m_dwBkColor);
		}
	}
}
void CCommonBufferDC::DrawBezier3(IN const POINT *pPtBezier,		// 曲线点与控制点的数组（P0 C1 C2 P1 C3 C4 P2 ……）
							  IN int nPtCount,					// 数组大小（3 * n + 1）
							  IN const matrix4f* pMat)		// 投影矩阵
{
	if (!m_bAttached || (pPtBezier == NULL) || (nPtCount < POINT_BEZIER_COUNT))
		return;

	if (pMat == NULL)
	{
		DrawBezier3(pPtBezier, nPtCount);
	}
	else
	{
		pointf pPtOrigin[POINT_BEZIER_COUNT];
		POINT *pPtTmp = NULL;
		int nPatIndex(0), nPatItem(0), nPtStart(0);

		nPtCount -= 3;
		for (nPtStart=0; nPtStart<nPtCount; nPtStart+=3)
		{
			// 读取一段贝塞尔曲线并进行坐标转换
			pPtTmp = (POINT*)pPtBezier + nPtStart;
			pPtOrigin[0].x = (float)pPtTmp[0].x;
			pPtOrigin[0].y = (float)pPtTmp[0].y;
			pPtOrigin[1].x = (float)pPtTmp[1].x;
			pPtOrigin[1].y = (float)pPtTmp[1].y;
			pPtOrigin[2].x = (float)pPtTmp[2].x;
			pPtOrigin[2].y = (float)pPtTmp[2].y;
			pPtOrigin[3].x = (float)pPtTmp[3].x;
			pPtOrigin[3].y = (float)pPtTmp[3].y;
			for (int i=0; i<POINT_BEZIER_COUNT; i++)
			{
				ClientToScreen(pPtOrigin[i]);
			}
			// 递归画出该曲线
			RecursionBezier3(pPtOrigin, (matrix4f*)pMat, m_pPattern, m_nPatCount, nPatIndex, nPatItem, m_dwPenColor, m_nBkMode, m_dwBkColor);
		}
		if (nPtStart == nPtCount)	// 数组大小为 3 * n，则自动闭合曲线
		{
			// 读取一段贝塞尔曲线并进行坐标转换
			pPtTmp = (POINT*)pPtBezier + nPtStart;
			pPtOrigin[0].x = (float)pPtTmp[0].x;
			pPtOrigin[0].y = (float)pPtTmp[0].y;
			pPtOrigin[1].x = (float)pPtTmp[1].x;
			pPtOrigin[1].y = (float)pPtTmp[1].y;
			pPtOrigin[2].x = (float)pPtTmp[2].x;
			pPtOrigin[2].y = (float)pPtTmp[2].y;
			pPtOrigin[3].x = (float)pPtBezier[0].x;		// 闭合时使用第一个点
			pPtOrigin[3].y = (float)pPtBezier[0].y;
			for (int i=0; i<POINT_BEZIER_COUNT; i++)
			{
				ClientToScreen(pPtOrigin[i]);
			}
			// 递归画出该曲线
			RecursionBezier3(pPtOrigin, (matrix4f*)pMat, m_pPattern, m_nPatCount, nPatIndex, nPatItem, m_dwPenColor, m_nBkMode, m_dwBkColor);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 屏幕坐标系下的绘制接口（以下划线开头）

// 画线
BOOL CCommonBufferDC::_LineTo(IN int x, IN int y)
{
	if (!m_bAttached)
		return FALSE;

	if (m_nPenWidth == 1)		// 画一个像素宽的直线
	{
		int nPatIndex(0), nPatItem(0);		// 必须初始化为 0

		Bresenham(m_ptStart.x, m_ptStart.y, x, y, m_pPattern, m_nPatCount,	\
			nPatIndex, nPatItem, m_dwPenColor, m_nBkMode, m_dwBkColor);
	}
	else		// 画宽线时需要特殊的填充算法（尤其是在与原始图颜色相关的填充方式下，必须防止重复填充）
	{
		FillWideLine(m_ptStart.x, m_ptStart.y, x, y, m_nPenWidth,	\
			m_pPattern, m_nPatCount, m_dwPenColor, m_nBkMode, m_dwBkColor);
	}
	// 设置新起点
	_MoveTo(x, y);

	return TRUE;
}

// 设置某一点（点在有效区域内时返回 TRUE）
BOOL CCommonBufferDC::_SetPixel(IN int x,				// 点位置
							IN int y,
							IN DWORD dwColor)		// 颜色
{
	if (!_PixelVisible(x, y))
		return FALSE;

	DWORD *pTmp = m_pdwBuffer + x + y * m_nWidth;

	switch (m_nDrawMode)
	{
	case R2_BLACK:			*pTmp = 0xff000000;		break;
	case R2_COPYPEN:		*pTmp = dwColor;		break;
	case R2_MASKNOTPEN:		*pTmp = (0xff000000 | (*pTmp & (~dwColor)));	break;
	case R2_MASKPEN:		*pTmp = (0xff000000 | (*pTmp & dwColor));		break;
	case R2_MASKPENNOT:		*pTmp = (0xff000000 | ((~(*pTmp)) & dwColor));	break;
	case R2_MERGENOTPEN:	*pTmp = (0xff000000 | (*pTmp | (~dwColor)));	break;
	case R2_MERGEPEN:		*pTmp = (0xff000000 | (*pTmp | dwColor));		break;
	case R2_MERGEPENNOT:	*pTmp = (0xff000000 | ((~(*pTmp)) | dwColor));	break;
	case R2_NOP:			break;
	case R2_NOT:			*pTmp = (0xff000000 | (~(*pTmp)));	break;
	case R2_NOTCOPYPEN:		*pTmp = (0xff000000 | (~dwColor));	break;
	case R2_NOTMASKPEN:		*pTmp = (0xff000000 | (~(*pTmp & dwColor)));	break;
	case R2_NOTMERGEPEN:	*pTmp = (0xff000000 | (~(*pTmp | dwColor)));	break;
	case R2_NOTXORPEN:		*pTmp = (0xff000000 | (~((*pTmp) ^ dwColor)));	break;
	case R2_WHITE:			*pTmp = 0xffffffff;		break;
	case R2_XORPEN:			*pTmp = (0xff000000 | ((*pTmp) ^ dwColor));		break;
	default:				return FALSE;
	}
	return TRUE;
}

// 画矩形
void CCommonBufferDC::_DrawRect(IN int l, IN int t, IN int r, IN int b)
{
	if (!m_bAttached)
		return;

	if (l == r || t == b)
		return;

	// 规则化矩形（buffer 中的坐标与屏幕坐标相同）
	if (l > r)		// 交换左右边
	{
		l += r; r = l - r; l = l - r;
	}
	if (t > b)		// 交换上下边
	{
		t += b; b = t - b; t = t - b;
	}
	if (m_nPenWidth == 1)		// 线宽为 1 则直接使用画线的方式
	{
		_MoveTo(l, t);
		_LineTo(r - 1, t);
		_LineTo(r - 1, b - 1);
		_LineTo(l, b - 1);
		_LineTo(l, t);
		if (m_nFillMode == OPAQUE)		// 不透明填充
		{
			int y_t_i = ClipPixelY(t + 1);
			int y_b_i = ClipPixelY(b - 1);
			int x_l_i = ClipPixelX(l + 1);
			int x_r_i = ClipPixelX(r - 1);

			for (int y_tmp=y_t_i; y_tmp<y_b_i; y_tmp++)
			{
				for (int x_tmp=x_l_i; x_tmp<x_r_i; x_tmp++)
				{
					_SetPixel(x_tmp, y_tmp, m_dwFillColor);
				}
			}
		}
	}
	else		// 线宽大于 1 时如果内部相交则需要采用扫描线的方式
	{
		int half_w = (m_nPenWidth >> 1);
		int l_o = l - half_w, l_i = l_o + m_nPenWidth - 1;
		int r_i = r - half_w, r_o = r_i + m_nPenWidth - 1;
		int t_o = t - half_w, t_i = t_o + m_nPenWidth - 1;
		int b_i = b - half_w, b_o = b_i + m_nPenWidth - 1;

		if (l_i >= (r_i - 1) || (t_i + 1) >= b_i)	// 内部相交则没有填充区
		{
			int y_t_o = ClipPixelY(t_o);
			int y_b_o = ClipPixelY(b_o);
			int x_l_o = ClipPixelX(l_o);
			int x_r_o = ClipPixelX(r_o);

			for (int x_tmp=x_l_o; x_tmp<x_r_o; x_tmp++)
			{
				for (int y_tmp=y_t_o; y_tmp<y_b_o; y_tmp++)
				{
					_SetPixel(x_tmp, y_tmp, m_dwPenColor);
				}
			}
		}
		else		// 具有填充区
		{
			_MoveTo(l_o, t);					_LineTo(r_o, t);
			_MoveTo(r - 1, t_i + 1);			_LineTo(r - 1, b_i - 1);
			_MoveTo(r_o - 1, b - 1);			_LineTo(l_o - 1, b - 1);
			_MoveTo(l, b_i - 2);				_LineTo(l, t_i);
			if (m_nFillMode == OPAQUE)		// 不透明填充
			{
				int y_t_i = ClipPixelY(t_i + 1);
				int y_b_i = ClipPixelY(b_i - 1);
				int x_l_i = ClipPixelX(l_i + 1);
				int x_r_i = ClipPixelX(r_i - 1);

				for (int y_tmp=y_t_i; y_tmp<y_b_i; y_tmp++)
				{
					for (int x_tmp=x_l_i; x_tmp<x_r_i; x_tmp++)
					{
						_SetPixel(x_tmp, y_tmp, m_dwFillColor);
					}
				}
			}
		}
	}
}

// 画三次贝塞尔曲线（如果数组大小为 3 * n，则自动闭合曲线)
void CCommonBufferDC::_DrawBezier3(IN const POINT *pPtBezier,	// 曲线点与控制点的数组（P0 C1 C2 P1 C3 C4 P2 ……）
							   IN int nPtCount)				// 数组大小（3 * n + 1）
{
	if (!m_bAttached || (pPtBezier == NULL) || (nPtCount < POINT_BEZIER_COUNT))
		return;

	pointf pPtOrigin[POINT_BEZIER_COUNT];
	POINT *pPtTmp = NULL;
	int nPatIndex(0), nPatItem(0), nPtStart(0);

	nPtCount -= 3;
	for (nPtStart=0; nPtStart<nPtCount; nPtStart+=3)
	{
		// 读取一段贝塞尔曲线
		pPtTmp = (POINT*)pPtBezier + nPtStart;
		pPtOrigin[0].x = (float)pPtTmp[0].x;
		pPtOrigin[0].y = (float)pPtTmp[0].y;
		pPtOrigin[1].x = (float)pPtTmp[1].x;
		pPtOrigin[1].y = (float)pPtTmp[1].y;
		pPtOrigin[2].x = (float)pPtTmp[2].x;
		pPtOrigin[2].y = (float)pPtTmp[2].y;
		pPtOrigin[3].x = (float)pPtTmp[3].x;
		pPtOrigin[3].y = (float)pPtTmp[3].y;
		// 递归画出该曲线
		RecursionBezier3(pPtOrigin, m_pPattern, m_nPatCount, nPatIndex, nPatItem, m_dwPenColor, m_nBkMode, m_dwBkColor);
	}
	if (nPtStart == nPtCount)	// 数组大小为 3 * n，则自动闭合曲线
	{
		// 读取一段贝塞尔曲线
		pPtTmp = (POINT*)pPtBezier + nPtStart;
		pPtOrigin[0].x = (float)pPtTmp[0].x;
		pPtOrigin[0].y = (float)pPtTmp[0].y;
		pPtOrigin[1].x = (float)pPtTmp[1].x;
		pPtOrigin[1].y = (float)pPtTmp[1].y;
		pPtOrigin[2].x = (float)pPtTmp[2].x;
		pPtOrigin[2].y = (float)pPtTmp[2].y;
		pPtOrigin[3].x = (float)pPtBezier[0].x;		// 闭合时使用第一个点
		pPtOrigin[3].y = (float)pPtBezier[0].y;
		// 递归画出该曲线
		RecursionBezier3(pPtOrigin, m_pPattern, m_nPatCount, nPatIndex, nPatItem, m_dwPenColor, m_nBkMode, m_dwBkColor);
	}
}
void CCommonBufferDC::_DrawBezier3(IN const pointf *pPtBezier,	// 曲线点与控制点的数组（P0 C1 C2 P1 C3 C4 P2 ……）
							   IN int nPtCount)				// 数组大小（3 * n + 1）
{
	if (!m_bAttached || (pPtBezier == NULL) || (nPtCount < POINT_BEZIER_COUNT))
		return;

	pointf *pPtTmp = NULL;
	int nPatIndex(0), nPatItem(0), nPtStart(0);

	nPtCount -= 3;
	for (nPtStart=0; nPtStart<nPtCount; nPtStart+=3)
	{
		// 读取一段贝塞尔曲线
		pPtTmp = (pointf*)pPtBezier + nPtStart;
		// 递归画出该曲线
		RecursionBezier3(pPtTmp, m_pPattern, m_nPatCount, nPatIndex, nPatItem, m_dwPenColor, m_nBkMode, m_dwBkColor);
	}
	if (nPtStart == nPtCount)	// 数组大小为 3 * n，则自动闭合曲线
	{
		pointf pPtOrigin[POINT_BEZIER_COUNT];

		// 读取一段贝塞尔曲线
		SYMemcpy_Line(pPtOrigin, ((pointf*)pPtBezier + nPtStart), sizeof(pointf) * 3);
		pPtOrigin[3].x = pPtBezier[0].x;		// 闭合时使用第一个点
		pPtOrigin[3].y = pPtBezier[0].y;
		// 递归画出该曲线
		RecursionBezier3(pPtOrigin, m_pPattern, m_nPatCount, nPatIndex, nPatItem, m_dwPenColor, m_nBkMode, m_dwBkColor);
	}
}

// 在绘制三次贝塞尔曲线时使用投影矩阵，如果"pMat == NULL"，则进入正常绘制流程。
void CCommonBufferDC::_DrawBezier3(IN const POINT *pPtBezier,	// 曲线点与控制点的数组（P0 C1 C2 P1 C3 C4 P2 ……）
							   IN int nPtCount,					// 数组大小（3 * n + 1）
							   IN const matrix4f* pMat)		// 投影矩阵
{
	if (!m_bAttached || (pPtBezier == NULL) || (nPtCount < POINT_BEZIER_COUNT))		// 无效参数输入
		return;

	if (pMat == NULL)
	{
		DrawBezier3(pPtBezier, nPtCount);
	}
	else
	{
		pointf pPtOrigin[POINT_BEZIER_COUNT];
		POINT *pPtTmp = NULL;
		int nPatIndex(0), nPatItem(0), nPtStart(0);

		nPtCount -= 3;
		for (nPtStart=0; nPtStart<nPtCount; nPtStart+=3)
		{
			// 读取一段贝塞尔曲线
			pPtTmp = (POINT*)pPtBezier + nPtStart;
			pPtOrigin[0].x = (float)pPtTmp[0].x;
			pPtOrigin[0].y = (float)pPtTmp[0].y;
			pPtOrigin[1].x = (float)pPtTmp[1].x;
			pPtOrigin[1].y = (float)pPtTmp[1].y;
			pPtOrigin[2].x = (float)pPtTmp[2].x;
			pPtOrigin[2].y = (float)pPtTmp[2].y;
			pPtOrigin[3].x = (float)pPtTmp[3].x;
			pPtOrigin[3].y = (float)pPtTmp[3].y;
			// 递归画出该曲线
			RecursionBezier3(pPtOrigin, (matrix4f*)pMat, m_pPattern, m_nPatCount, nPatIndex, nPatItem, m_dwPenColor, m_nBkMode, m_dwBkColor);
		}
		if (nPtStart == nPtCount)	// 数组大小为 3 * n，则自动闭合曲线
		{
			// 读取一段贝塞尔曲线
			pPtTmp = (POINT*)pPtBezier + nPtStart;
			pPtOrigin[0].x = (float)pPtTmp[0].x;
			pPtOrigin[0].y = (float)pPtTmp[0].y;
			pPtOrigin[1].x = (float)pPtTmp[1].x;
			pPtOrigin[1].y = (float)pPtTmp[1].y;
			pPtOrigin[2].x = (float)pPtTmp[2].x;
			pPtOrigin[2].y = (float)pPtTmp[2].y;
			pPtOrigin[3].x = (float)pPtBezier[0].x;		// 闭合时使用第一个点
			pPtOrigin[3].y = (float)pPtBezier[0].y;
			// 递归画出该曲线
			RecursionBezier3(pPtOrigin, (matrix4f*)pMat, m_pPattern, m_nPatCount, nPatIndex, nPatItem, m_dwPenColor, m_nBkMode, m_dwBkColor);
		}
	}
}
void CCommonBufferDC::_DrawBezier3(IN const pointf *pPtBezier,	// 曲线点与控制点的数组（P0 C1 C2 P1 C3 C4 P2 ……）
							   IN int nPtCount,					// 数组大小（3 * n + 1）
							   IN const matrix4f* pMat)		// 投影矩阵
{
	if (!m_bAttached || (pPtBezier == NULL) || (nPtCount < POINT_BEZIER_COUNT))		// 无效参数输入
		return;

	if (pMat == NULL)
	{
		_DrawBezier3(pPtBezier, nPtCount);
	}
	else
	{
		pointf *pPtTmp = NULL;
		int nPatIndex(0), nPatItem(0), nPtStart(0);

		nPtCount -= 3;
		for (nPtStart=0; nPtStart<nPtCount; nPtStart+=3)
		{
			// 读取一段贝塞尔曲线
			pPtTmp = (pointf*)pPtBezier + nPtStart;
			// 递归画出该曲线
			RecursionBezier3(pPtTmp, (matrix4f*)pMat, m_pPattern, m_nPatCount, nPatIndex, nPatItem, m_dwPenColor, m_nBkMode, m_dwBkColor);
		}
		if (nPtStart == nPtCount)	// 数组大小为 3 * n，则自动闭合曲线
		{
			pointf pPtOrigin[POINT_BEZIER_COUNT];

			// 读取一段贝塞尔曲线
			SYMemcpy_Line(pPtOrigin, ((pointf*)pPtBezier + nPtStart), sizeof(pointf) * 3);
			pPtOrigin[3].x = pPtBezier[0].x;		// 闭合时使用第一个点
			pPtOrigin[3].y = pPtBezier[0].y;
			// 递归画出该曲线
			RecursionBezier3(pPtOrigin, (matrix4f*)pMat, m_pPattern, m_nPatCount, nPatIndex, nPatItem, m_dwPenColor, m_nBkMode, m_dwBkColor);
		}
	}
}

// 画边框
void CCommonBufferDC::_DrawBorder(IN int l,					// 目标区域
							  IN int t,
							  IN int r,
							  IN int b,
							  IN DWORD dwShadowColor,	// 暗色
							  IN DWORD dwLightColor,	// 亮色
							  IN CommonBORDER_STYLE eStyle,	// 风格
							  IN int nBorderWidth)		// 边框宽度
{
	if (!m_bAttached || nBorderWidth <= 0 || l == r || t == b)
		return;

	DWORD crLT(0), crRB(0), crOldPen = GetPenColor();
	int nOldPenWidth = SetPenWidth(nBorderWidth);

	// 根据风格确定左上边与右下边的颜色
	switch (eStyle)
	{
	case CommonBORDER_RAISED:	crLT = dwLightColor; crRB = dwShadowColor; break;
	case CommonBORDER_SUCKEN:	crLT = dwShadowColor; crRB = dwLightColor; break;
	default:				break;//assert(FALSE);
	}
	// 规则化矩形（buffer 中的坐标与屏幕坐标相同）
	if (l > r)		// 交换左右边
	{
		l += r; r = l - r; l = l - r;
	}
	if (t > b)		// 交换上下边
	{
		t += b; b = t - b; t = t - b;
	}
	// 绘制（矩形被分成左上部分与右下部分）
	if (nBorderWidth == 1)		// 一个像素宽
	{
		// 画左上边
		SetPenColor(crLT);
		_MoveTo(l, b - 1);
		_LineTo(l, t);
		_LineTo(r, t);
		// 画右下边
		SetPenColor(crRB);
		_MoveTo(r - 1, t);
		_LineTo(r - 1, b - 1);
		_LineTo(l - 1, b - 1);
	}
	else
	{
		int half_w = (nBorderWidth >> 1);
		int l_o = l - half_w, l_i = l_o + nBorderWidth - 1;
		int r_i = r - half_w, r_o = r_i + nBorderWidth - 1;
		int t_o = t - half_w, t_i = t_o + nBorderWidth - 1;
		int b_i = b - half_w, b_o = b_i + nBorderWidth - 1;

		if (l_i >= (r_i - 1) || (t_i + 1) >= b_i)	// 内部相交
		{
			int y_t_o = ClipPixelY(t_o);
			int y_t_i = ClipPixelY(t_i);	// 左上与右下的 Y 边界
			int y_b_o = ClipPixelY(b_o);
			int x_l_o = ClipPixelX(l_o);
			int x_l_i = ClipPixelX(l_i);	// 左上与右下的 X 边界
			int x_r_o = ClipPixelX(r_o);

			for (int y_tmp=y_t_o; y_tmp<y_t_i; y_tmp++)		// 填充上部
			{
				for (int x_tmp=x_l_o; x_tmp<x_r_o; x_tmp++)
				{
					_SetPixel(x_tmp, y_tmp, crLT);
				}
			}
			for (int y_tmp=y_t_i; y_tmp<y_b_o; y_tmp++)
			{
				for (int x_tmp=x_l_o; x_tmp<x_l_i; x_tmp++)	// 填充左部
				{
					_SetPixel(x_tmp, y_tmp, crLT);
				}
				for (int x_tmp=x_l_i; x_tmp<x_r_o; x_tmp++)	// 填充右下部
				{
					_SetPixel(x_tmp, y_tmp, crRB);
				}
			}
		}
		else
		{
			// 先画左上边
			SetPenColor(crLT);
			_MoveTo(l_o, t);					_LineTo(r_o, t);
			_MoveTo(l, b_o - 1);				_LineTo(l, t_i);
			// 再画右下边
			SetPenColor(crRB);
			_MoveTo(r - 1, t_i + 1);			_LineTo(r - 1, b_i - 1);
			_MoveTo(r_o - 1, b - 1);			_LineTo(l_i, b - 1);
		}
	}
	// 恢复环境
	SetPenWidth(nOldPenWidth);
	SetPenColor(crOldPen);
}

// 如果成功则返回文本高度，否则返回0。
int CCommonBufferDC::_DrawText(IN LPCTSTR lpszString,	// 文本
						   IN int nCount,
						   IN LPRECT lpRect,			// 目标区域
						   IN UINT uFormat,			// 格式（兼容GDI）
						   IN BOOL bBKTrans)		// 是否在透明背景上绘制文字
{
	int nRealStringLen = (int)_tcslen(lpszString);

	if (lpRect == NULL || nRealStringLen <= 0)
		return 0;

	if (!EnableGDIHelp(TRUE, 0, 0))		// 使能GDI绘制帮助
		return 0;

	HFONT hOldFont = (HFONT)::SelectObject(m_hMemDC, (HGDIOBJ)m_hFont);

	if ((uFormat & DT_CALCRECT) != 0)		// 如果调用者指定计算文本的区域大小
	{
		int nRet = ::DrawText(m_hMemDC, lpszString, nCount, lpRect, uFormat|DT_CALCRECT);

		SelectObject(m_hMemDC, (HGDIOBJ)hOldFont);

		return nRet;
	}
	// 如果有裁减属性，且目标区域大小不存在，则直接返回。
	if ((uFormat & DT_NOCLIP) == 0
		&& (lpRect->right <= lpRect->left || lpRect->bottom <= lpRect->top))
		return 0;

	// 计算各个位置的参数，并使用填充大小来生成新的DIB缓存，填充大小大于等于指定文本区域大小。
	int nDesX(0), nDesY(0), nFillWidth(0), nFillHeight(0);
	RECT rcDrawText;

	memset(&rcDrawText, 0, sizeof(RECT));
	CalcTextDrawParam(rcDrawText, nDesX, nDesY, nFillWidth, nFillHeight, lpRect);
	if (!EnableGDIHelp(TRUE, nFillWidth, nFillHeight))
		return 0;

	// 绘制文本，DIB的输入颜色为0x0BGR，输出颜色为0x0RGB，所以在输入与输出过程要翻转颜色。
	COLORREF crOldText = ::SetTextColor(m_hMemDC, GetBGRA_Cr(m_dwTextColor));
	COLORREF crOldBk = ::SetBkColor(m_hMemDC, GetBGRA_Cr(m_dwBkColor));
	int nOldBkMode = ::SetBkMode(m_hMemDC, m_nBkMode);
	int nRet(0);

	if (m_nBkMode == OPAQUE)
	{
		// 清空缓存，注意颜色的转换，对内存直接操作时，颜色得使用0x0RGB。
		BGRA32_FillRect(m_pdwMemBM, 0, 0, nFillWidth, nFillHeight, m_nMemWidth, m_nMemHeight, NULL, GetRGBA_Cr(m_dwBkColor));
	}
	else
	{
		if( bBKTrans)
		{
			// 清空缓存，注意颜色的转换，对内存直接操作时，颜色得使用0x0RGB。
			BGRA32_FillRect(m_pdwMemBM, 0, 0, nFillWidth, nFillHeight, m_nMemWidth, m_nMemHeight, NULL, GetRGBA_Cr(m_dwBkColor));
		}
		else
		{
			// 取出背景buffer，在其上绘制文字
			Common_BitBlt(m_pdwMemBM,0,0,nFillWidth, nFillHeight,m_nMemWidth,m_nMemHeight,
				GetBuffer(),nDesX, nDesY, nFillWidth, nFillHeight, m_nWidth,m_nHeight,BUFFER_COPY, BUFFER_PARITY_BOTH);
		}
	}

	nRet = ::DrawText(m_hMemDC, lpszString, nCount, &rcDrawText, uFormat);

	if (0)
	{
		BufferFunction::WriteBitmap(_T("e:\\2.bmp"), 0, 0, nFillWidth, nFillHeight, m_nMemWidth, m_nMemHeight, m_pdwMemBM);
	}

	if (nRet != 0)		// 如果成功则将文本缓存贴到目标区域
	{
		BUFFER_MODE eMode = BUFFER_COPY;
		if (m_nBkMode == OPAQUE)	// 不透明情况下还需要填充背景色的Alpha通道
		{
			// 不管什么情况下都需要填充前景色的Alpha通道（等同于只不填充背景，防止反走样时填充不完整）
			BGRA32_FillAlpha(m_pdwMemBM, 0, 0, nFillWidth, nFillHeight, m_nMemWidth, m_nMemHeight,	
				NULL, GetAValue(m_dwTextColor), Common_FILL_NOT_MASK, GetRGBA_Cr(m_dwBkColor));

			BGRA32_FillAlpha(m_pdwMemBM, 0, 0, nFillWidth, nFillHeight, m_nMemWidth, m_nMemHeight,	
				NULL, GetAValue(m_dwBkColor), Common_FILL_MASK, GetRGBA_Cr(m_dwBkColor));
		}
		else	// 如果要画透明背景的文本，则需要做特殊处理。
		{
 			//if( bBKTrans)		//在CDSSymbolListCtrl中绘制文字是造成文字alpha通道错误，先注掉此判断
			{
				// 不管什么情况下都需要填充前景色的Alpha通道（等同于只不填充背景，防止反走样时填充不完整）
				BGRA32_FillAlpha(m_pdwMemBM, 0, 0, nFillWidth, nFillHeight, m_nMemWidth, m_nMemHeight,	
					NULL, GetAValue(m_dwTextColor), Common_FILL_NOT_MASK, GetRGBA_Cr(m_dwBkColor));
				eMode = BUFFER_MIX;
			}			
		}

	if(0)
		BufferFunction::WriteBitmap(_T("c:\\2.tga"),0,0, nFillWidth,nFillHeight,m_nMemWidth, m_nMemHeight, m_pdwMemBM);

		_BitBlt(nDesX, nDesY, nFillWidth, nFillHeight, m_pdwMemBM, 0, 0,	
			nFillWidth, nFillHeight, m_nMemWidth, m_nMemHeight, eMode, BUFFER_PARITY_BOTH);
	}
	::SetBkMode(m_hMemDC, nOldBkMode);
	::SetBkColor(m_hMemDC, crOldBk);
	::SetTextColor(m_hMemDC, crOldText);
	::SelectObject(m_hMemDC, (HGDIOBJ)hOldFont);

	return nRet;
}

//////////////////////////////////////////////////////////////////////////
// Protected

void CCommonBufferDC::CopyLinePattern(IN const int *pPattern,	// 线型数组
								  IN int nPatCount)			// 数组大小
{
	// 释放已有线型
	delete[] m_pPattern;
	m_pPattern = NULL;
	// 创建新线型
	m_nPatCount = nPatCount;
	m_pPattern = new int[nPatCount];
	memcpy(m_pPattern, pPattern, nPatCount * sizeof(int));
}

// 根据宽度及直线斜率缩放线型
void CCommonBufferDC::ScalePattern(OUT int *pPattern,	// 线型数组
							   IN int nPatCount,	// 数组大小
							   IN float fScale)		// 缩放比例
{
	for (int i=0; i<nPatCount; i++)
	{
		//pPattern[i] = (int)(pPattern[i] * fScale + 0.5f);
		pPattern[i] = (int)(pPattern[i] * fScale);
	}
}

// 画一个像素的直线（不包含终点）
void CCommonBufferDC::Bresenham(IN int x1,				// 起点
							IN int y1,
							IN int x2,				// 终点
							IN int y2,
							IN const int *pPattern,	// 线型
							IN int nPatCount,
							IN int &nPatIndex,		// 索引
							IN int &nPatItem,		// 项的点数
							IN DWORD dwPenColor,	// 笔的颜色
							IN int nBkMode,			// 背景模式（在非实线的情况下使用）
							IN DWORD dwBkColor)		// 背景颜色
{
	float fPatScale(1.f), fDelta(0.f);
	int nNewWidth(0);
	DWORD dwFlag = ClipLine(x1, y1, x2, y2, fPatScale, nNewWidth, fDelta, 1);

	if (dwFlag == LINE_IS_INVISIBLE)	// 线段不可见
		return;

	switch (dwFlag & SLOPE_TYPE_MASK)		// 根据线段的斜率分类光栅化
	{
	case SLOPE_EQUAL_ZERO:		// 水平线
		{
			BOOL bNegativeX = ((dwFlag & X_INCREASE_NEGATIVE) != 0);
			int dx = bNegativeX ? (x1 - x2) : (x2 - x1);
			int nDeltaX = bNegativeX ? -1 : 1;

			// 当两端点重合时最少画一点
			dx = (dx == 0) ? 1 : dx;
			// 获取用于实现虚线的项的点数
			nPatItem = (nPatItem == 0) ? pPattern[nPatIndex] : nPatItem;
			// 光栅化直线，从起点开始，而不包含终点，但起点与终点重合除外。
			for (int i=0; i<dx; i++, x1+=nDeltaX)
			{
				// 虚线判断，将虚线中的空段按照背景的标准进行填充。
				if (IsSpace(pPattern, nPatCount, nPatIndex, nPatItem))	// 背景
				{
					if (nBkMode == TRANSPARENT)		// 透明
						continue;
					_SetPixel(x1, y1, dwBkColor);
				}
				else	// 前景
				{
					_SetPixel(x1, y1, dwPenColor);
				}
			}
		}
		break;
	case SLOPE_EQUAL_INFINITE:		// 垂直线
		{
			BOOL bNegativeY = ((dwFlag & Y_INCREASE_NEGATIVE) != 0);
			int dy = bNegativeY ? (y1 - y2) : (y2 - y1);
			int nDeltaY = bNegativeY ? -1 : 1;

			// 当两端点重合时最少画一点
			dy = (dy == 0) ? 1 : dy;
			// 获取用于实现虚线的项的点数
			nPatItem = (nPatItem == 0) ? pPattern[nPatIndex] : nPatItem;
			// 光栅化直线，从起点开始，而不包含终点，但起点与终点重合除外。
			for (int i=0; i<dy; i++, y1+=nDeltaY)
			{
				// 虚线判断，将虚线中的空段按照背景的标准进行填充。
				if (IsSpace(pPattern, nPatCount, nPatIndex, nPatItem))	// 背景
				{
					if (nBkMode == TRANSPARENT)		// 透明
						continue;
					_SetPixel(x1, y1, dwBkColor);
				}
				else	// 前景
				{
					_SetPixel(x1, y1, dwPenColor);
				}
			}
		}
		break;
	case SLOPE_EQUAL_TAN:		// 斜率为 1 或 -1 的直线
		{
			int *pTmpPat = new int[nPatCount];		// 申请临时线型
			BOOL bNegativeX = ((dwFlag & X_INCREASE_NEGATIVE) != 0);
			int dx = bNegativeX ? (x1 - x2) : (x2 - x1);
			int nDeltaX = bNegativeX ? -1 : 1;
			int nDeltaY = ((dwFlag & Y_INCREASE_NEGATIVE) != 0) ? -1 : 1;

			// 当两端点重合时最少画一点
			dx = (dx == 0) ? 1 : dx;
			// 缩放线型以保证线段的比例
			memcpy(pTmpPat, pPattern, nPatCount * sizeof(int));
			ScalePattern(pTmpPat, nPatCount, fPatScale);
			// 获取用于实现虚线的项的点数
			nPatItem = (nPatItem == 0) ? pTmpPat[nPatIndex] : nPatItem;
			// 光栅化直线，从起点开始，而不包含终点。
			for (int i=0; i<dx; i++, x1+=nDeltaX, y1+=nDeltaY)
			{
				// 虚线判断，将虚线中的空段按照背景的标准进行填充。
				if (IsSpace(pTmpPat, nPatCount, nPatIndex, nPatItem))	// 背景
				{
					if (nBkMode == TRANSPARENT)		// 透明
						continue;
					_SetPixel(x1, y1, dwBkColor);
				}
				else	// 前景
				{
					_SetPixel(x1, y1, dwPenColor);
				}
			}
			delete[] pTmpPat;
			pTmpPat = NULL;
		}
		break;
	case SLOPE_UNDER_TAN:		// 斜率小于 1 的直线（主轴为 x）
	case SLOPE_ABOVE_TAN:		// 斜率大于 1 的直线（主轴为 y）
		{
			int *pTmpPat = new int[nPatCount];		// 申请临时线型
			BOOL bNegativeX = ((dwFlag & X_INCREASE_NEGATIVE) != 0);
			BOOL bNegativeY = ((dwFlag & Y_INCREASE_NEGATIVE) != 0);
			int dx = bNegativeX ? (x1 - x2) : (x2 - x1);
			int dy = bNegativeY ? (y1 - y2) : (y2 - y1);
			int nDeltaX = bNegativeX ? -1 : 1;
			int nDeltaY = bNegativeY ? -1 : 1;
			// 选取主导轴（变化快的）与辅助轴（变化慢的）变量，进行 Bresenham。
			BOOL bDominatedX = ((dwFlag & SLOPE_TYPE_MASK) == SLOPE_UNDER_TAN);
			int d_F = bDominatedX ? dx : dy;			// 主导轴
			int *pnFast = bDominatedX ? &x1 : &y1;
			int nDeltaFast = bDominatedX ? nDeltaX : nDeltaY;
			int d_S = bDominatedX ? dy : dx;			// 辅助轴
			int d2_S = (d_S << 1);
			int *pnSlow = bDominatedX ? &y1 : &x1;
			int nDeltaSlow = bDominatedX ? nDeltaY : nDeltaX;
			int d2yx = d2_S - (d_F << 1);
			int p = -d_F;
			unsigned int alpha = 0;
			unsigned int color = 0;
			unsigned int tempalpha = 0;

			float d_Step = d_S/(float)d_F;
			float fdistance = 0;

			int half = -d2yx/2;
			int all = - d2yx;
			// 当两端点重合时最少画一点
			d_F = (d_F == 0) ? 1 : d_F;
			float fd = 0.0f;
			// 缩放线型以保证线段的比例
			memcpy(pTmpPat, pPattern, nPatCount * sizeof(int));
			ScalePattern(pTmpPat, nPatCount, fPatScale);
			// 获取用于实现虚线的项的点数
			nPatItem = (nPatItem == 0) ? pTmpPat[nPatIndex] : nPatItem;
			int nD = nDeltaSlow/abs(nDeltaSlow);
			DWORD dwColor = dwPenColor;
			// 光栅化直线，从起点开始，而不包含终点。
			for (int i=0; i<d_F; i++, *pnFast+=nDeltaFast)
			{
				if(fd < 1.0f)
				{
					fd += d_Step;
				}
				if(fd >= 1.0f)
				{
					fd -= 1.0f;
					*pnSlow += nDeltaSlow;
				}
				//if (p < 0)
				//{
				//	p += d2_S;
				//}
				//else
				//{
				//	p += d2yx;
				//	*pnSlow += nDeltaSlow;
				//}
				// 虚线判断，将虚线中的空段按照背景的标准进行填充。
				if (IsSpace(pTmpPat, nPatCount, nPatIndex, nPatItem))	// 背景
				{
					if (nBkMode == TRANSPARENT)		// 透明
					{
						continue;
					}
					dwColor = dwBkColor;
//					_SetPixel(x1, y1, dwBkColor);
				}
				else	// 前景
				{
					dwColor = dwPenColor;
				}
				color = dwColor;
				alpha = (dwPenColor & 0xff000000) >> 24;
				tempalpha = (unsigned long)(alpha * (1-fd));
				
				if((dwFlag & SLOPE_TYPE_MASK) == SLOPE_UNDER_TAN)
				{
					color = ((255 - tempalpha)<<24 )|(dwPenColor&0x00ffffff);
					_SetPixel(x1, y1 + nD, color);
					color = (tempalpha<<24 )|(dwPenColor&0x00ffffff);
					_SetPixel(x1, y1, color);
				}
				else
				{
					color = ((255 - tempalpha)<<24 )|(dwPenColor&0x00ffffff);
					_SetPixel(x1 + nD, y1, color);
					color = (tempalpha<<24 )|(dwPenColor&0x00ffffff);
					_SetPixel(x1, y1, color);
				}
				
			}
			delete[] pTmpPat;
			pTmpPat = NULL;
		}
		break;
	default:
		break;
		//assert(FALSE);
	}
}

// 裁减线段并返回线段的斜率
DWORD CCommonBufferDC::ClipLine(IN OUT int &x1,			// 起点
							IN OUT int &y1,
							IN OUT int &x2,			// 终点
							IN OUT int &y2,
							OUT float &fPatScale,	// 线型缩放系数
							OUT int &nNewWidth,		// 新的线宽（由于斜率的原因，需要进行线宽的变化）
							OUT float &fDelta,		// 用于宽斜线绘制中的端点处理参数
							IN int nWidth)			// 线宽
{
	int dx = x2 - x1;
	int dy = y2 - y1;

	if (dy == 0)		// 水平线
	{
		// 判断直线是否可见，减少光栅化的负载。
		if ((y1 + ((nWidth - 1) >> 1)) < m_rcClip.top		// 直线的下边缘在裁减矩形的上方
			|| (y1 - (nWidth >> 1)) >= m_rcClip.bottom		// 直线的上边缘在裁减矩形的下方
			|| (dx >= 0 && (x2 < m_rcClip.left || x1 >= m_rcClip.right))
			|| (dx < 0 && (x1 < m_rcClip.left || x2 >= m_rcClip.right)))
			return LINE_IS_INVISIBLE;

		// 计算新的线型参数
		fPatScale = (float)nWidth;
		nNewWidth = nWidth;
		// 裁减直线段
		x1 = ClipPixelX(x1);
		x2 = ClipPixelX(x2);

		return (SLOPE_EQUAL_ZERO|((dx < 0) ? X_INCREASE_NEGATIVE : 0));
	}
	else if (dx == 0)	// 垂直线
	{
		// 判断直线是否可见，减少光栅化的负载。
		if ((x1 + ((nWidth - 1) >> 1)) < m_rcClip.left		// 直线的右边缘在裁减矩形的左方
			|| (x1 - (nWidth >> 1)) >= m_rcClip.right		// 直线的左边缘在裁减矩形的右方
			|| (dy >= 0 && (y2 < m_rcClip.top || y1 >= m_rcClip.bottom))
			|| (dy < 0 && (y1 < m_rcClip.top || y2 >= m_rcClip.bottom)))
			return LINE_IS_INVISIBLE;

		// 计算新的线型参数
		fPatScale = (float)nWidth;
		nNewWidth = nWidth;
		// 裁减直线段
		y1 = ClipPixelY(y1);
		y2 = ClipPixelY(y2);

		return (SLOPE_EQUAL_INFINITE|((dy < 0) ? Y_INCREASE_NEGATIVE : 0));
	}
	// 斜线段与裁减矩形的四种关系：
	//     一、与四条边没有交点，但线段的两个端点都在裁减矩形内；
	//     二、与四条边只有一个交点，且交点为裁减矩形的某个顶点，此时线段褪化为一个点；
	//     三、与四条边只有一个交点，但线段的一个端点在裁减矩形内，此时裁减线段的一个端点；
	//     四、与四条边有两个交点，此时裁减线段的两个端点。
	// 首先判断第一种情况，如果第一种不成立，则需要计算出每条裁减边与直线的交点。
	// 如果交点不在边内，或者在边内但不在线段内，则认为这个交点无效。
	// 按照 LTRB（0、1、2、3）的顺序依次检测裁减边，记录交点值及交点边的索引（最多两个交点）。
	// 在线段的裁减过程中，需要检测线段的方向，对新旧端点做出正确的映射。
	// 当线段的宽度不为 1 时，相交点即使不在裁减矩形的边上，也有可能与裁减区域发生交集，
	// 因此将裁减矩形区向外扩张一半宽度的大小，可近似地检测宽线段与裁减矩形的交集。

	BOOL bStartPtValid = PixelInClipRect(x1, y1);		// 起点在裁减区内标志
	BOOL bEndPtValid = PixelInClipRect(x2, y2);			// 终点在裁减区内标志

	if (/*!bStartPtValid || !bEndPtValid*/ FALSE)		// 两个端点不全在裁减矩形区内，则需要进行裁减。
	{
		// 四条边的索引定义（交错定义可以避免直线穿越顶点时，重复考虑交点）
		const int nPtCount(4), nLeftSide(0), nRightSide(1), nTopSide(2), nBottomSide(3);
		int nHalfWidth = (nWidth >> 1);
		int pnIndex[nPtCount] = { -1, -1, -1, -1 };		// 交点的边索引（定义四个是为了防止奇异的情况）
		int nIndexCount(0);								// 交点的个数
		int pnDominated[nPtCount] = { 0, 0, 0, 0 };		// 交点的主导坐标轴值
		// 交点的另一个坐标轴值，也即四条边的主导值。
		int pnOther[nPtCount] = { (m_rcClip.left - nHalfWidth), (m_rcClip.right + nHalfWidth),
								  (m_rcClip.top - nHalfWidth), (m_rcClip.bottom + nHalfWidth) };

		// 求解四条边的交点的坐标值
		if (dx == dy)		// 斜率为 1
		{
			pnDominated[nLeftSide] = pnOther[nLeftSide];
			pnDominated[nRightSide] = pnOther[nRightSide];
			pnDominated[nTopSide] = pnOther[nTopSide];
			pnDominated[nBottomSide] = pnOther[nBottomSide];
		}
		else if (dx == -dy)		// 斜率为 -1
		{
			pnDominated[nLeftSide] = -pnOther[nLeftSide];
			pnDominated[nRightSide] = -pnOther[nRightSide];
			pnDominated[nTopSide] = -pnOther[nTopSide];
			pnDominated[nBottomSide] = -pnOther[nBottomSide];
		}
		else		// 斜率不为 1 或 -1
		{
			float fK = (float)dy / (float)dx, fK_Inv = 1.f / fK;	// 斜率

			pnDominated[nLeftSide] = (int)((pnOther[nLeftSide] - x1) * fK + y1);
			pnDominated[nRightSide] = (int)((pnOther[nRightSide] - x1) * fK + y1);
			pnDominated[nTopSide] = (int)((pnOther[nTopSide] - y1) * fK_Inv + x1);
			pnDominated[nBottomSide] = (int)((pnOther[nBottomSide] - y1) * fK_Inv + x1);
		}
		// 对各个交点进行检测，其中 L 与 R 的主导轴为 Y，T 与 B 的主导轴为 X。
		for (int i=0; i<nPtCount; i++)
		{
			if (i == nLeftSide || i == nRightSide)		// Y 为主导轴
			{
				if ((pnDominated[i] >= pnOther[nTopSide] && pnDominated[i] <= pnOther[nBottomSide])	// 在边内
					&& ((dy > 0 && pnDominated[i] >= y1 && pnDominated[i] <= y2)
					|| (dy < 0 && pnDominated[i] <= y1 && pnDominated[i] >= y2)))		// 在线段内
				{
					pnIndex[nIndexCount++] = i;
				}
			}
			else		// X 为主导轴
			{
				if ((pnDominated[i] >= pnOther[nLeftSide] && pnDominated[i] <= pnOther[nRightSide])	// 在边内
					&& ((dx > 0 && pnDominated[i] >= x1 && pnDominated[i] <= x2)
					|| (dx < 0 && pnDominated[i] <= x1 && pnDominated[i] >= x2)))		// 在线段内
				{
					if (nIndexCount < 2)		// 最多只会有两交点
					{
						pnIndex[nIndexCount++] = i;
					}
				}
			}
		}
		// 判断线段与裁减矩形的关系，并在相交的情况下进行正确的端点裁减。
		if (nIndexCount <= 0)			// 没有交点则线段不可见
			return LINE_IS_INVISIBLE;

		BOOL bFirstIndexY = (pnIndex[0] == nLeftSide || pnIndex[0] == nRightSide);	// y 主导
		int nFirstPtX = bFirstIndexY ? pnOther[pnIndex[0]] : pnDominated[pnIndex[0]];
		int nFirstPtY = bFirstIndexY ? pnDominated[pnIndex[0]] : pnOther[pnIndex[0]];

		// 根据交点的个数以及它们相对于端点的位置，对端点进行正确的裁减。
		if (nIndexCount == 1)		// 只有一个交点，只要端点不在裁减矩形区内就进行裁减。
		{
			// 如果两个端点都不在裁减矩形区内，则线段褪化为一个点。
			if (!bStartPtValid)		// 起点不在裁减矩形区内
			{
				x1 = nFirstPtX;
				y1 = nFirstPtY;
			}
			if (!bEndPtValid)		// 终点不在裁减矩形区内
			{
				x2 = nFirstPtX;
				y2 = nFirstPtY;
			}
		}
		else		// 有两个交点，使用就近原则对两个端点进行裁减。
		{
			BOOL bSecondIndexY = (pnIndex[1] == nLeftSide || pnIndex[1] == nRightSide);	// y 主导
			int nSecondPtX = bSecondIndexY ? pnOther[pnIndex[1]] : pnDominated[pnIndex[1]];
			int nSecondPtY = bSecondIndexY ? pnDominated[pnIndex[1]] : pnOther[pnIndex[1]];
			// 计算起点与两个交点的距离（使用 X 轴向的即可），并以此来判断裁减对应的索引。
			int nDis_Start_First = (x1 > nFirstPtX) ? (x1 - nFirstPtX) : (nFirstPtX - x1);
			int nDis_Start_Second = (x1 > nSecondPtX) ? (x1 - nSecondPtX) : (nSecondPtX - x1);

			// 使用离端点近的交点对端点进行裁减，找出一个对应关系，则另一个端点只能匹配另一个交点。
			if (nDis_Start_Second > nDis_Start_First)		// 起点离第一个交点更近
			{
				x1 = nFirstPtX;
				y1 = nFirstPtY;
				x2 = nSecondPtX;
				y2 = nSecondPtY;
			}
			else		// 起点离第二个交点更近
			{
				x1 = nSecondPtX;
				y1 = nSecondPtY;
				x2 = nFirstPtX;
				y2 = nFirstPtY;
			}
		}
	}

	// 填充标志
	DWORD dwFlag(0);

	dwFlag |= ((dx < 0) ? X_INCREASE_NEGATIVE : 0);
	dwFlag |= ((dy < 0) ? Y_INCREASE_NEGATIVE : 0);
	if (dx == dy || dx == -dy)		// 45度或135度线
	{
		dwFlag |= SLOPE_EQUAL_TAN;
		fDelta = fPatScale = 0.7071068f * nWidth;
		nNewWidth = (int)(nWidth * 1.4142136f + 0.5f);
	}
	else		// 斜率不为 1 或 -1 的线
	{
		dx = (dx < 0) ? -dx : dx;
		dy = (dy < 0) ? -dy : dy;

		// 计算斜率角的三角函数值
		float fR_I = 1.f / sqrtf((float)(dx * dx + dy * dy));
		float fSin = dy * fR_I;
		float fCos = dx * fR_I;

		// 计算填充增量及端点裕量、设定标志
		if (dx < dy)	// 斜率大于 1（主轴为 y）
		{
			nNewWidth = (int)(nWidth / fSin + 0.5f);
			fPatScale = nWidth * fSin;
			fDelta = fCos * nWidth * 0.5f;
			dwFlag |= SLOPE_ABOVE_TAN;
		}
		else		// 斜率小于 1（主轴为 x）
		{
			nNewWidth = (int)(nWidth / fCos + 0.5f);
			fPatScale = nWidth * fCos;
			fDelta = fSin * nWidth * 0.5f;
			dwFlag |= SLOPE_UNDER_TAN;
		}
	}
	return dwFlag;
}

// 检查线型是否为空隙（如果是空隙则返回 TRUE）
BOOL CCommonBufferDC::IsSpace(IN const int *pPattern,	// 线型
						  IN int nPatCount,			// 线型大小
						  IN int &nPatIndex,		// 索引
						  IN int &nPatItem)			// 项的点数
{
	nPatItem--;
	if (nPatItem <= 0)
	{
		nPatIndex = (nPatIndex + 1) % nPatCount;
		if ((nPatItem = pPattern[nPatIndex]) == 0)
			return TRUE;
	}
	return ((nPatIndex & 1) == 1);
}

// 画宽线
void CCommonBufferDC::FillWideLine(IN int x1,				// 起点
							   IN int y1,
							   IN int x2,				// 终点
							   IN int y2,
							   IN int nWidth,			// 线宽（宽度为 1 的直线请使用 Bresenham 算法）
							   IN int *pPattern,		// 单像素线型
							   IN int nPatCount,
							   IN DWORD dwPenColor,		// 笔的颜色
							   IN int nBkMode,			// 背景模式（在非实线的情况下使用）
							   IN DWORD dwBkColor)		// 背景颜色
{
	float fPatScale(1.f), fDelta(0.f);
	int nNewWidth(0);
	DWORD dwFlag = ClipLine(x1, y1, x2, y2, fPatScale, nNewWidth, fDelta, nWidth);

	if (dwFlag == LINE_IS_INVISIBLE)
		return;

	int *pTmpPat = new int[nPatCount];
	int nPatIndex(0), nPatItem(0);

	// 拷贝线型，并缩放线型。
	memcpy(pTmpPat, pPattern, nPatCount * sizeof(int));
	ScalePattern(pTmpPat, nPatCount, fPatScale);
	nPatItem = pTmpPat[nPatIndex];
	// 按直线斜率分类
	switch(dwFlag & SLOPE_TYPE_MASK)
	{
	case SLOPE_EQUAL_ZERO:			// 水平线
		{
			BOOL bNegativeX = ((dwFlag & X_INCREASE_NEGATIVE) != 0);
			int dx = bNegativeX ? (x1 - x2) : (x2 - x1);
			int nDeltaX = bNegativeX ? -1 : 1;
			int nTopEdge = y1 - (nNewWidth >> 1);				// 直线的上边缘
			int nBottomEdge = y1 + ((nNewWidth - 1) >> 1);		// 直线的下边缘

			// 当两端点重合时最少画一点
			dx = (dx == 0) ? 1 : dx;
			// 裁减直线的上下边缘
			nTopEdge = ClipPixelY(nTopEdge);
			nBottomEdge = ClipPixelY(nBottomEdge);
			// 光栅化直线，从起点开始，而不包含终点，但起点与终点重合除外。
			for (int i=0; i<dx; i++, x1+=nDeltaX)
			{
				// 虚线判断，将虚线中的空段按照背景的标准进行填充。
				if (IsSpace(pTmpPat, nPatCount, nPatIndex, nPatItem))	// 背景
				{
					if (nBkMode == TRANSPARENT)		// 透明
						continue;
					for (int nTmpY=nTopEdge; nTmpY<=nBottomEdge; nTmpY++)
					{
						_SetPixel(x1, nTmpY, dwBkColor);
					}
				}
				else	// 前景
				{
					for (int nTmpY=nTopEdge; nTmpY<=nBottomEdge; nTmpY++)
					{
						_SetPixel(x1, nTmpY, dwPenColor);
					}
				}
			}
		}
		break;
	case SLOPE_EQUAL_INFINITE:		// 垂直线
		{
			BOOL bNegativeY = ((dwFlag & Y_INCREASE_NEGATIVE) != 0);
			int dy = bNegativeY ? (y1 - y2) : (y2 - y1);
			int nDeltaY = bNegativeY ? -1 : 1;
			int nLeftEdge = x1 - (nNewWidth >> 1);			// 直线的左边缘
			int nRightEdge = x1 + ((nNewWidth - 1) >> 1);	// 直线的右边缘

			// 当两端点重合时最少画一点
			dy = (dy == 0) ? 1 : dy;
			// 裁减直线的左右边缘
			nLeftEdge = ClipPixelX(nLeftEdge);
			nRightEdge = ClipPixelX(nRightEdge);
			// 光栅化直线，从起点开始，而不包含终点，但起点与终点重合除外。
			for (int i=0; i<dy; i++, y1+=nDeltaY)
			{
				// 虚线判断，将虚线中的空段按照背景的标准进行填充。
				if (IsSpace(pTmpPat, nPatCount, nPatIndex, nPatItem))	// 背景
				{
					if (nBkMode == TRANSPARENT)		// 透明
						continue;
					for (int nTmpX=nLeftEdge; nTmpX<=nRightEdge; nTmpX++)
					{
						_SetPixel(nTmpX, y1, dwBkColor);
					}
				}
				else	// 前景
				{
					for (int nTmpX=nLeftEdge; nTmpX<=nRightEdge; nTmpX++)
					{
						_SetPixel(nTmpX, y1, dwPenColor);
					}
				}
			}
		}
		break;
	case SLOPE_EQUAL_TAN:		// 斜率等于 1
		{
			BOOL bSpace = FALSE;
			int nSub = -(nNewWidth >> 1);
			int nSupper = ((nNewWidth - 1) >> 1);
			BOOL bNegativeX = ((dwFlag & X_INCREASE_NEGATIVE) != 0);
			int dx = bNegativeX ? (x1 - x2) : (x2 - x1);
			int nDeltaX = bNegativeX ? -1 : 1;
			int nDeltaY = ((dwFlag & Y_INCREASE_NEGATIVE) != 0) ? -1 : 1;

			// 当两端点重合时最少画一点
			dx = (dx == 0) ? 1 : dx;
			// 光栅化直线，从起点开始，而不包含终点，但起点与终点重合除外。
			for (int i=0; i<dx; i++, x1+=nDeltaX, y1+=nDeltaY)
			{
				if (IsSpace(pTmpPat, nPatCount, nPatIndex, nPatItem))	// 背景
				{
					if (!bSpace)	// 刚结束一段前景线时画出该线（采用滞后一步控制的方式画线）
					{
						bSpace = TRUE;
						FillLineFromStack(fDelta, nSub, nSupper, 0, 0, 0, dwPenColor, dwFlag);
					}
					if (nBkMode == TRANSPARENT)		// 透明
						continue;
					// 先不画线而将点推入栈，如果栈满则画线。
					if (PushPtInStack(x1, y1))
					{
						FillLineFromStack(fDelta, nSub, nSupper, 0, 0, 0, dwBkColor, dwFlag);
					}
				}
				else	// 前景
				{
					if (bSpace)		// 刚结束一段背景线时画出该线
					{
						bSpace = FALSE;
						// 如果背景透明的话，栈必定为空。
						FillLineFromStack(fDelta, nSub, nSupper, 0, 0, 0, dwBkColor, dwFlag);
					}
					// 先不画线而将点推入栈，如果栈满则画线。
					if (PushPtInStack(x1, y1))
					{
						FillLineFromStack(fDelta, nSub, nSupper, 0, 0, 0, dwPenColor, dwFlag);
					}
				}
			}
			// 填充线段（填充最后一段可能存在的线段）
			FillLineFromStack(fDelta, nSub, nSupper, 0, 0, 0, ((bSpace) ? dwBkColor : dwPenColor), dwFlag);
		}
		break;
	case SLOPE_UNDER_TAN:		// 斜率小于 1 的直线（主轴为 x）
	case SLOPE_ABOVE_TAN:		// 斜率大于 1 的直线（主轴为 y）
		{
			BOOL bSpace = FALSE;
			int nSub = -(nNewWidth >> 1);
			int nSupper = ((nNewWidth - 1) >> 1);
			BOOL bNegativeX = ((dwFlag & X_INCREASE_NEGATIVE) != 0);
			BOOL bNegativeY = ((dwFlag & Y_INCREASE_NEGATIVE) != 0);
			int dx = bNegativeX ? (x1 - x2) : (x2 - x1);
			int dy = bNegativeY ? (y1 - y2) : (y2 - y1);
			int nDeltaX = bNegativeX ? -1 : 1;
			int nDeltaY = bNegativeY ? -1 : 1;
			// 选取主导轴（变化快的）与辅助轴（变化慢的）变量，进行 Bresenham。
			BOOL bDominatedX = ((dwFlag & SLOPE_TYPE_MASK) == SLOPE_UNDER_TAN);
			int d_F = bDominatedX ? dx : dy;			// 主导轴
			int d2_F = (d_F << 1);
			int *pnFast = bDominatedX ? &x1 : &y1;
			int nDeltaFast = bDominatedX ? nDeltaX : nDeltaY;
			int d_S = bDominatedX ? dy : dx;			// 辅助轴
			int d2_S = (d_S << 1);
			int *pnSlow = bDominatedX ? &y1 : &x1;
			int nDeltaSlow = bDominatedX ? nDeltaY : nDeltaX;
			int d2yx = d2_S - d2_F;
			int p = -d_F;

			// 当两端点重合时最少画一点
			d_F = (d_F == 0) ? 1 : d_F;
			// Bresenham 方法
			for (int i=0; i<d_F; i++, *pnFast+=nDeltaFast)
			{
				if (p < 0)
				{
					p += d2_S;
				}
				else
				{
					p += d2yx;
					*pnSlow += nDeltaSlow;
				}
				if (IsSpace(pTmpPat, nPatCount, nPatIndex, nPatItem))	// 背景
				{
					if (!bSpace)	// 刚结束一段前景线时画出该线（采用滞后一步控制的方式画线）
					{
						bSpace = TRUE;
						FillLineFromStack(fDelta, nSub, nSupper, d2_F, d_S, d2_S, dwPenColor, dwFlag);
					}
					if (nBkMode == TRANSPARENT)		// 透明
						continue;
					// 先不画线而将点推入栈，如果栈满则画线。
					if (PushPtInStack(x1, y1))
					{
						FillLineFromStack(fDelta, nSub, nSupper, d2_F, d_S, d2_S, dwBkColor, dwFlag);
					}
				}
				else	// 前景
				{
					if (bSpace)		// 刚结束一段背景线时画出该线
					{
						bSpace = FALSE;
						// 如果背景透明的话，栈必定为空。
						FillLineFromStack(fDelta, nSub, nSupper, d2_F, d_S, d2_S, dwBkColor, dwFlag);
					}
					// 先不画线而将点推入栈，如果栈满则画线。
					if (PushPtInStack(x1, y1))
					{
						FillLineFromStack(fDelta, nSub, nSupper, d2_F, d_S, d2_S, dwPenColor, dwFlag);
					}
				}
			}
			// 填充线段（填充最后一段可能存在的线段）
			FillLineFromStack(fDelta, nSub, nSupper, d2_F, d_S, d2_S, ((bSpace) ? dwBkColor : dwPenColor), dwFlag);
		}
		break;
	default:
		break;
		//assert(FALSE);
	}
	delete[] pTmpPat;
	pTmpPat = NULL;
}

// 根据堆栈填充连续的宽线段（采用线刷子的方式填充）
void CCommonBufferDC::FillLineFromStack(IN float fDelta,		// 两个端点的特殊处理区域
									IN int nSub,			// 宽度的两个边界
									IN int nSuper,
									IN int dFast2,			// 主轴（变化快的轴）上的端点差的双倍值
									IN int dSlow,			// 副轴（变化慢的轴）上的端点差
									IN int dSlow2,			// 副轴（变化慢的轴）上的端点差的双倍值
									IN DWORD dwColor,		// 颜色
									IN DWORD dwFlag)		// 根据直线的走势定义的标志
{
	if (m_nTop <= 0)
		return;

	m_nTop--;		// 得到线段上最后一个点的索引

	int min = (int)(fDelta + 0.5f);			// 起点的裕量
	int max = (int)(m_nTop - fDelta);		// 终点的裕量
	int x1 = m_pPtStack[0].x, y1 = m_pPtStack[0].y;	// 起点
	int x1_2 = (x1 << 1), y1_2 = (y1 << 1);
	int x2 = m_pPtStack[m_nTop].x, y2 = m_pPtStack[m_nTop].y;		// 终点
	int x2_2 = (x2 << 1), y2_2 = (y2 << 1);
	int threshold(0);			// 阈值
	BOOL bNegative = FALSE;		// 主轴增长为负

	switch (dwFlag & SLOPE_TYPE_MASK)
	{
	case SLOPE_UNDER_TAN:		// 斜率小于 1
		// 判断增长的符号
		bNegative = (dwFlag & Y_INCREASE_NEGATIVE);
		if (min > max)		// 病态的直线
		{
			// 病态的处理线头
			ProcessLineCaput(min, max, nSub, nSuper, dFast2, dSlow, dSlow2, x1, y1,	\
				x1_2, y1_2, x2, y2, x2_2, y2_2, dwColor, bNegative, dwFlag);
			break;
		}
		// 顺序扫描堆栈填充线段（特殊处理线头与线尾成矩形形状）
		for (int i=0; i<=m_nTop; i++)
		{
			if (i < min)		// 线头
			{
				threshold = (nSub + m_pPtStack[i].y - y1) * dSlow2 + ((bNegative) ? -i : i) * dFast2 + dSlow;
				for (int j=nSub; j<=nSuper; j++, threshold+=dSlow2)
				{
					if ((bNegative && threshold < 0) || (!bNegative && threshold > 0))		// 正常填充
					{
						_SetPixel(m_pPtStack[i].x, m_pPtStack[i].y + j, dwColor);
					}
					else if (i != 0)		// 中心对称填充
					{
						_SetPixel(x1_2 - m_pPtStack[i].x, y1_2 - m_pPtStack[i].y - j, dwColor);
					}
				}
			}
			else if (i > max)		// 线尾
			{
				threshold = (nSub + m_pPtStack[i].y - y2) * dSlow2 + ((bNegative) ? (m_nTop - i) : (i - m_nTop)) * dFast2 + dSlow;
				for (int j=nSub; j<=nSuper; j++, threshold+=dSlow2)
				{
					if ((bNegative && threshold >= 0) || (!bNegative && threshold <= 0))
					{
						_SetPixel(m_pPtStack[i].x, m_pPtStack[i].y + j, dwColor);
					}
					else if (i != m_nTop)
					{
						_SetPixel(x2_2 - m_pPtStack[i].x, y2_2 - m_pPtStack[i].y - j, dwColor);
					}
				}
			}
			else		// 中间段
			{
				for (int j=nSub; j<=nSuper; j++)
				{
					_SetPixel(m_pPtStack[i].x, m_pPtStack[i].y + j, dwColor);
				}
			}
		}
		break;
	case SLOPE_ABOVE_TAN:		// 斜率大于 1
		// 判断增长的符号
		bNegative = (dwFlag & X_INCREASE_NEGATIVE);
		if (min > max)		// 病态的直线
		{
			// 病态的处理线头
			ProcessLineCaput(min, max, nSub, nSuper, dFast2, dSlow, dSlow2, x1, y1,	\
				x1_2, y1_2, x2, y2, x2_2, y2_2, dwColor, bNegative, dwFlag);
			break;
		}
		// 顺序扫描堆栈填充线段（特殊处理线头与线尾成矩形形状）
		for (int i=0; i<=m_nTop; i++)
		{
			if (i < min)		// 线头
			{
				threshold = (nSub + m_pPtStack[i].x - x1) * dSlow2 + ((bNegative) ? -i : i) * dFast2 + dSlow;
				for (int j=nSub; j<=nSuper; j++, threshold+=dSlow2)
				{
					if ((bNegative && threshold < 0) || (!bNegative && threshold > 0))		// 正常填充
					{
						_SetPixel(m_pPtStack[i].x + j, m_pPtStack[i].y, dwColor);
					}
					else if (i != 0)		// 中心对称填充
					{
						_SetPixel(x1_2 - m_pPtStack[i].x - j, y1_2 - m_pPtStack[i].y, dwColor);
					}
				}
			}
			else if (i > max)		// 线尾
			{
				threshold = (nSub + m_pPtStack[i].x - x2) * dSlow2 + ((bNegative) ? (m_nTop - i) : (i - m_nTop)) * dFast2 + dSlow;
				for (int j=nSub; j<=nSuper; j++, threshold+=dSlow2)
				{
					if ((bNegative && threshold >= 0) || (!bNegative && threshold <= 0))
					{
						_SetPixel(m_pPtStack[i].x + j, m_pPtStack[i].y, dwColor);
					}
					else if (i != m_nTop)
					{
						_SetPixel(x2_2 - m_pPtStack[i].x - j, y2_2 - m_pPtStack[i].y, dwColor);
					}
				}
			}
			else		// 中间段
			{
				for (int j=nSub; j<=nSuper; j++)
				{
					_SetPixel(m_pPtStack[i].x + j, m_pPtStack[i].y, dwColor);
				}
			}
		}
		break;
	case SLOPE_EQUAL_TAN:		// 斜率等于 1
		// 判断增长的符号
		bNegative = (dwFlag & Y_INCREASE_NEGATIVE);
		if (min > max)		// 病态的直线
		{
			// 病态的处理线头
			ProcessLineCaput(min, max, nSub, nSuper, dFast2, dSlow, dSlow2, x1, y1,	\
				x1_2, y1_2, x2, y2, x2_2, y2_2, dwColor, bNegative, dwFlag);
			break;
		}
		// 顺序扫描堆栈填充线段（特殊处理线头与线尾成矩形形状）
		for (int i=0; i<=m_nTop; i++)
		{
			if (i < min)		// 线头
			{
				threshold = nSub + nSub + ((bNegative) ? -(i << 2) : (i << 2)) + 1;
				for (int j=nSub; j<=nSuper; j++, threshold+=2)
				{
					if ((bNegative && threshold < 0) || (!bNegative && threshold > 0))		// 正常填充
					{
						_SetPixel(m_pPtStack[i].x, m_pPtStack[i].y + j, dwColor);
					}
					else if (i != 0)		// 中心对称填充
					{
						_SetPixel(x1_2 - m_pPtStack[i].x, y1_2 - m_pPtStack[i].y - j, dwColor);
					}
				}
			}
			else if (i > max)		// 线尾
			{
				threshold = nSub + nSub + ((bNegative) ? ((m_nTop - i) << 2) : -((m_nTop - i) << 2)) + 1;
				for (int j=nSub; j<=nSuper; j++, threshold+=2)
				{
					if ((bNegative && threshold >= 0) || (!bNegative && threshold <= 0))
					{
						_SetPixel(m_pPtStack[i].x, m_pPtStack[i].y + j, dwColor);
					}
					else if (i != m_nTop)
					{
						_SetPixel(x2_2 - m_pPtStack[i].x, y2_2 - m_pPtStack[i].y - j, dwColor);
					}
				}
			}
			else		// 中间段
			{
				for (int j=nSub; j<=nSuper; j++)
				{
					_SetPixel(m_pPtStack[i].x, m_pPtStack[i].y + j, dwColor);
				}
			}
		}
		break;
	}
	// 清空栈
	m_nTop = 0;
}

// 病态的处理线刷子端点（实际意义不大）
void CCommonBufferDC::ProcessLineCaput(IN int min,			// 起点的特殊处理阈值
								   IN int max,			// 终点的特殊处理阈值
								   IN int nSub,			// 线刷子底限
								   IN int nSuper,		// 线刷子上限
								   IN int dFast2,		// 快轴端点差的双倍
								   IN int dSlow,		// 慢轴端点差
								   IN int dSlow2,		// 慢轴端点差的双倍
								   IN int x1,			// 起点坐标
								   IN int y1,
								   IN int x1_2,			// 起点坐标的双倍
								   IN int y1_2,
								   IN int x2,			// 终点坐标
								   IN int y2,
								   IN int x2_2,			// 终点坐标的双倍
								   IN int y2_2,
								   IN DWORD dwColor,	// 颜色
								   IN BOOL bNegative,	// 慢轴增长为负值的标志
								   IN DWORD dwFlag)		// 根据直线的走势定义的标志
{
	int th_start(0), th_end(0), i(0);

	switch (dwFlag & SLOPE_TYPE_MASK)
	{
	case SLOPE_UNDER_TAN:
		if (min <= m_nTop)
		{
			for (i=0; i<max; i++)
			{
				th_start = (nSub + m_pPtStack[i].y - y1) * dSlow2 + ((bNegative) ? -i : i) * dFast2 + dSlow;
				for (int j=nSub; j<=nSuper; j++, th_start+=dSlow2)
				{
					if ((bNegative && th_start < 0) || (!bNegative && th_start > 0))		// 正常填充
					{
						_SetPixel(m_pPtStack[i].x, m_pPtStack[i].y + j, dwColor);
					}
					else if (i != 0)		// 中心对称填充
					{
						_SetPixel(x1_2 - m_pPtStack[i].x, y1_2 - m_pPtStack[i].y - j, dwColor);
					}
				}
			}
			for (; i<min; i++)
			{
				th_start = (nSub + m_pPtStack[i].y - y1) * dSlow2 + ((bNegative) ? -i : i) * dFast2 + dSlow;
				th_end = (nSub + m_pPtStack[i].y - y2) * dSlow2 + ((bNegative) ? (m_nTop - i) : (i - m_nTop)) * dFast2 + dSlow;
				for (int j=nSub; j<=nSuper; j++, th_start+=dSlow2, th_end+=dSlow2)
				{
					if ((bNegative && th_start < 0) || (!bNegative && th_start > 0))		// 正常填充
					{
						if ((bNegative && th_end >= 0) || (!bNegative && th_end <= 0))
						{
							_SetPixel(m_pPtStack[i].x, m_pPtStack[i].y + j, dwColor);
						}
						else
						{
							_SetPixel(x2_2 - m_pPtStack[i].x, y2_2 - m_pPtStack[i].y - j, dwColor);
						}
					}
					else if (i != 0)		// 中心对称填充
					{
						_SetPixel(x1_2 - m_pPtStack[i].x, y1_2 - m_pPtStack[i].y - j, dwColor);
					}
				}
			}
			for (; i<=m_nTop; i++)
			{
				th_end = (nSub + m_pPtStack[i].y - y2) * dSlow2 + ((bNegative) ? (m_nTop - i) : (i - m_nTop)) * dFast2 + dSlow;
				for (int j=nSub; j<=nSuper; j++, th_end+=dSlow2)
				{
					if ((bNegative && th_end >= 0) || (!bNegative && th_end <= 0))
					{
						_SetPixel(m_pPtStack[i].x, m_pPtStack[i].y + j, dwColor);
					}
					else if (i != m_nTop)
					{
						_SetPixel(x2_2 - m_pPtStack[i].x, y2_2 - m_pPtStack[i].y - j, dwColor);
					}
				}
			}
		}
		else
		{
			for (i=0; i<=m_nTop; i++)
			{
				th_start = (nSub + m_pPtStack[i].y - y1) * dSlow2 + ((bNegative) ? -i : i) * dFast2 + dSlow;
				th_end = (nSub + m_pPtStack[i].y - y2) * dSlow2 + ((bNegative) ? (m_nTop - i) : (i - m_nTop)) * dFast2 + dSlow;
				for (int j=nSub; j<=nSuper; j++, th_start+=dSlow2, th_end+=dSlow2)
				{
					if ((bNegative && th_start < 0) || (!bNegative && th_start > 0))
					{
						if ((bNegative && th_end >= 0) || (!bNegative && th_end <= 0))		// 正常填充
						{
							_SetPixel(m_pPtStack[i].x, m_pPtStack[i].y + j, dwColor);
						}
						else if (i != m_nTop)	// 递归
						{
							RecursionMin(x2_2 - m_pPtStack[i].x, y2_2 - m_pPtStack[i].y - j, x1, y1,	\
								x1_2, y1_2, x2, y2, x2_2, y2_2, dFast2, dSlow, dSlow2, dwColor, bNegative, dwFlag);
						}
					}
					else if (i != 0)		// 中心对称递归
					{
						RecursionMax(x1_2 - m_pPtStack[i].x, y1_2 - m_pPtStack[i].y - j, x1, y1,	\
							x1_2, y1_2, x2, y2, x2_2, y2_2, dFast2, dSlow, dSlow2, dwColor, bNegative, dwFlag);
					}
				}
			}
		}
		break;
	case SLOPE_ABOVE_TAN:
		if (min <= m_nTop)
		{
			for (i=0; i<max; i++)
			{
				th_start = (nSub + m_pPtStack[i].x - x1) * dSlow2 + ((bNegative) ? -i : i) * dFast2 + dSlow;
				for (int j=nSub; j<=nSuper; j++, th_start+=dSlow2)
				{
					if ((bNegative && th_start < 0) || (!bNegative && th_start > 0))		// 正常填充
					{
						_SetPixel(m_pPtStack[i].x + j, m_pPtStack[i].y, dwColor);
					}
					else if (i != 0)		// 中心对称填充
					{
						_SetPixel(x1_2 - m_pPtStack[i].x - j, y1_2 - m_pPtStack[i].y, dwColor);
					}
				}
			}
			for (; i<min; i++)
			{
				th_start = (nSub + m_pPtStack[i].x - x1) * dSlow2 + ((bNegative) ? -i : i) * dFast2 + dSlow;
				th_end = (nSub + m_pPtStack[i].x - x2) * dSlow2 + ((bNegative) ? (m_nTop - i) : (i - m_nTop)) * dFast2 + dSlow;
				for (int j=nSub; j<=nSuper; j++, th_start+=dSlow2, th_end+=dSlow2)
				{
					if ((bNegative && th_start < 0) || (!bNegative && th_start > 0))		// 正常填充
					{
						if ((bNegative && th_end >= 0) || (!bNegative && th_end <= 0))
						{
							_SetPixel(m_pPtStack[i].x + j, m_pPtStack[i].y, dwColor);
						}
						else
						{
							_SetPixel(x2_2 - m_pPtStack[i].x - j, y2_2 - m_pPtStack[i].y, dwColor);
						}
					}
					else if (i != 0)		// 中心对称填充
					{
						_SetPixel(x1_2 - m_pPtStack[i].x - j, y1_2 - m_pPtStack[i].y, dwColor);
					}
				}
			}
			for (; i<=m_nTop; i++)
			{
				th_end = (nSub + m_pPtStack[i].x - x2) * dSlow2 + ((bNegative) ? (m_nTop - i) : (i - m_nTop)) * dFast2 + dSlow;
				for (int j=nSub; j<=nSuper; j++, th_end+=dSlow2)
				{
					if ((bNegative && th_end >= 0) || (!bNegative && th_end <= 0))
					{
						_SetPixel(m_pPtStack[i].x + j, m_pPtStack[i].y, dwColor);
					}
					else if (i != m_nTop)
					{
						_SetPixel(x2_2 - m_pPtStack[i].x - j, y2_2 - m_pPtStack[i].y, dwColor);
					}
				}
			}
		}
		else
		{
			for (i=0; i<=m_nTop; i++)
			{
				th_start = (nSub + m_pPtStack[i].x - x1) * dSlow2 + ((bNegative) ? -i : i) * dFast2 + dSlow;
				th_end = (nSub + m_pPtStack[i].x - x2) * dSlow2 + ((bNegative) ? (m_nTop - i) : (i - m_nTop)) * dFast2 + dSlow;
				for (int j=nSub; j<=nSuper; j++, th_start+=dSlow2, th_end+=dSlow2)
				{
					if ((bNegative && th_start < 0) || (!bNegative && th_start > 0))
					{
						if ((bNegative && th_end >= 0) || (!bNegative && th_end <= 0))		// 正常填充
						{
							_SetPixel(m_pPtStack[i].x + j, m_pPtStack[i].y, dwColor);
						}
						else if (i != m_nTop)	// 递归
						{
							RecursionMin(x2_2 - m_pPtStack[i].x - j, y2_2 - m_pPtStack[i].y, x1, y1,	\
								x1_2, y1_2, x2, y2, x2_2, y2_2, dFast2, dSlow, dSlow2, dwColor, bNegative, dwFlag);
						}
					}
					else if (i != 0)		// 中心对称递归
					{
						RecursionMax(x1_2 - m_pPtStack[i].x - j, y1_2 - m_pPtStack[i].y, x1, y1,	\
							x1_2, y1_2, x2, y2, x2_2, y2_2, dFast2, dSlow, dSlow2, dwColor, bNegative, dwFlag);
					}
				}
			}
		}
		break;
	case SLOPE_EQUAL_TAN:
		if (min <= m_nTop)
		{
			for (i=0; i<max; i++)
			{
				th_start = nSub + nSub + ((bNegative) ? -(i << 2) : (i << 2)) + 1;
				for (int j=nSub; j<=nSuper; j++, th_start+=2)
				{
					if ((bNegative && th_start < 0) || (!bNegative && th_start > 0))		// 正常填充
					{
						_SetPixel(m_pPtStack[i].x, m_pPtStack[i].y + j, dwColor);
					}
					else if (i != 0)		// 中心对称填充
					{
						_SetPixel(x1_2 - m_pPtStack[i].x, y1_2 - m_pPtStack[i].y - j, dwColor);
					}
				}
			}
			for (; i<min; i++)
			{
				th_start = nSub + nSub + ((bNegative) ? -(i << 2) : (i << 2)) + 1;
				th_end = nSub + nSub + ((bNegative) ? ((m_nTop - i) << 2) : -((m_nTop - i) << 2)) + 1;
				for (int j=nSub; j<=nSuper; j++, th_start+=2, th_end+=2)
				{
					if ((bNegative && th_start < 0) || (!bNegative && th_start > 0))		// 正常填充
					{
						if ((bNegative && th_end >= 0) || (!bNegative && th_end <= 0))
						{
							_SetPixel(m_pPtStack[i].x, m_pPtStack[i].y + j, dwColor);
						}
						else
						{
							_SetPixel(x2_2 - m_pPtStack[i].x, y2_2 - m_pPtStack[i].y - j, dwColor);
						}
					}
					else if (i != 0)		// 中心对称填充
					{
						_SetPixel(x1_2 - m_pPtStack[i].x, y1_2 - m_pPtStack[i].y - j, dwColor);
					}
				}
			}
			for (; i<=m_nTop; i++)
			{
				th_end = nSub + nSub + ((bNegative) ? ((m_nTop - i) << 2) : -((m_nTop - i) << 2)) + 1;
				for (int j=nSub; j<=nSuper; j++, th_end+=2)
				{
					if ((bNegative && th_end >= 0) || (!bNegative && th_end <= 0))
					{
						_SetPixel(m_pPtStack[i].x, m_pPtStack[i].y + j, dwColor);
					}
					else if (i != m_nTop)
					{
						_SetPixel(x2_2 - m_pPtStack[i].x, y2_2 - m_pPtStack[i].y - j, dwColor);
					}
				}
			}
		}
		else
		{
			for (i=0; i<=m_nTop; i++)
			{
				th_start = nSub + nSub + ((bNegative) ? -(i << 2) : (i << 2)) + 1;
				th_end = nSub + nSub + ((bNegative) ? ((m_nTop - i) << 2) : -((m_nTop - i) << 2)) + 1;
				for (int j=nSub; j<=nSuper; j++, th_start+=2, th_end+=2)
				{
					if ((bNegative && th_start < 0) || (!bNegative && th_start > 0))
					{
						if ((bNegative && th_end >= 0) || (!bNegative && th_end <= 0))		// 正常填充
						{
							_SetPixel(m_pPtStack[i].x, m_pPtStack[i].y + j, dwColor);
						}
						else if (i != m_nTop)	// 递归
						{
							RecursionMin(x2_2 - m_pPtStack[i].x, y2_2 - m_pPtStack[i].y - j, x1, y1,	\
								x1_2, y1_2, x2, y2, x2_2, y2_2, dFast2, dSlow, dSlow2, dwColor, bNegative, dwFlag);
						}
					}
					else if (i != 0)		// 中心对称递归
					{
						RecursionMax(x1_2 - m_pPtStack[i].x, y1_2 - m_pPtStack[i].y - j, x1, y1,	\
							x1_2, y1_2, x2, y2, x2_2, y2_2, dFast2, dSlow, dSlow2, dwColor, bNegative, dwFlag);
					}
				}
			}
		}
		break;
	}
}
void CCommonBufferDC::RecursionMin(IN int x,				// 当前点
							   IN int y,
							   IN int x1,				// 起点坐标
							   IN int y1,
							   IN int x1_2,				// 起点坐标的双倍
							   IN int y1_2,
							   IN int x2,				// 终点坐标
							   IN int y2,
							   IN int x2_2,				// 终点坐标的双倍
							   IN int y2_2,
							   IN int dFast2,			// 快轴端点差的双倍
							   IN int dSlow,			// 慢轴端点差
							   IN int dSlow2,			// 慢轴端点差的双倍
							   IN DWORD dwColor,		// 颜色
							   IN BOOL bNegative,		// 慢轴增长为负值的标志
							   IN DWORD dwFlag)			// 根据直线的走势定义的标志
{
	int th_start(0), nDelta(0);

	switch (dwFlag & SLOPE_TYPE_MASK)
	{
	case SLOPE_UNDER_TAN:
		nDelta = (x1 > x) ? (x1 - x) : (x - x1);
		th_start = (y - y1) * dSlow2 + ((bNegative) ? -nDelta : nDelta) * dFast2 + dSlow;
		if ((bNegative && th_start <= 0) || (!bNegative && th_start >= 0))		// 停止递归
		{
			_SetPixel(x, y, dwColor);
		}
		else	// 递归
		{
			RecursionMax(x1_2 - x, y1_2 - y, x1, y1, x1_2, y1_2, x2, y2, x2_2, y2_2,	\
				dFast2, dSlow, dSlow2, dwColor, bNegative, dwFlag);
		}
		break;
	case SLOPE_ABOVE_TAN:
		nDelta = (y1 > y) ? (y1 - y) : (y - y1);
		th_start = (x - x1) * dSlow2 + ((bNegative) ? -nDelta : nDelta) * dFast2 + dSlow;
		if ((bNegative && th_start <= 0) || (!bNegative && th_start >= 0))		// 停止递归
		{
			_SetPixel(x, y, dwColor);
		}
		else	// 递归
		{
			RecursionMax(x1_2 - x, y1_2 - y, x1, y1, x1_2, y1_2, x2, y2, x2_2, y2_2,	\
				dFast2, dSlow, dSlow2, dwColor, bNegative, dwFlag);
		}
		break;
	case SLOPE_EQUAL_TAN:
		nDelta = ((x1 > x) ? (x1 - x) : (x - x1)) << 1;
		th_start = (y - y1) * 2 + ((bNegative) ? -nDelta : nDelta) + 1;
		if ((bNegative && th_start <= 0) || (!bNegative && th_start >= 0))		// 停止递归
		{
			_SetPixel(x, y, dwColor);
		}
		else	// 递归
		{
			RecursionMax(x1_2 - x, y1_2 - y, x1, y1, x1_2, y1_2, x2, y2, x2_2, y2_2,	\
				dFast2, dSlow, dSlow2, dwColor, bNegative, dwFlag);
		}
		break;
	}
}
void CCommonBufferDC::RecursionMax(IN int x,				// 当前点
							   IN int y,
							   IN int x1,				// 起点坐标
							   IN int y1,
							   IN int x1_2,				// 起点坐标的双倍
							   IN int y1_2,
							   IN int x2,				// 终点坐标
							   IN int y2,
							   IN int x2_2,				// 终点坐标的双倍
							   IN int y2_2,
							   IN int dFast2,			// 快轴端点差的双倍
							   IN int dSlow,			// 慢轴端点差
							   IN int dSlow2,			// 慢轴端点差的双倍
							   IN DWORD dwColor,		// 颜色
							   IN BOOL bNegative,		// 慢轴增长为负值的标志
							   IN DWORD dwFlag)			// 根据直线的走势定义的标志
{
	int th_end(0), nDelta(0);

	switch (dwFlag & SLOPE_TYPE_MASK)
	{
	case SLOPE_UNDER_TAN:
		nDelta = (x2 > x) ? (x2 - x) : (x - x2);
		th_end = (y - y2) * dSlow2 + ((bNegative) ? nDelta : -nDelta) * dFast2 + dSlow;
		if ((bNegative && th_end >= 0) || (!bNegative && th_end <= 0))		// 停止递归
		{
			_SetPixel(x, y, dwColor);
		}
		else	// 递归
		{
			RecursionMin(x2_2 - x, y2_2 - y, x1, y1, x1_2, y1_2, x2, y2, x2_2, y2_2,	\
				dFast2, dSlow, dSlow2, dwColor, bNegative, dwFlag);
		}
		break;
	case SLOPE_ABOVE_TAN:
		nDelta = (y2 > y) ? (y2 - y) : (y - y2);
		th_end = (x - x2) * dSlow2 + ((bNegative) ? nDelta : -nDelta) * dFast2 + dSlow;
		if ((bNegative && th_end >= 0) || (!bNegative && th_end <= 0))		// 停止递归
		{
			_SetPixel(x, y, dwColor);
		}
		else	// 递归
		{
			RecursionMin(x2_2 - x, y2_2 - y, x1, y1, x1_2, y1_2, x2, y2, x2_2, y2_2,	\
				dFast2, dSlow, dSlow2, dwColor, bNegative, dwFlag);
		}
		break;
	case SLOPE_EQUAL_TAN:
		nDelta = ((x2 > x) ? (x2 - x) : (x - x2)) << 1;
		th_end = (y - y2) * 2 + ((bNegative) ? nDelta : -nDelta) + 1;
		if ((bNegative && th_end >= 0) || (!bNegative && th_end <= 0))		// 停止递归
		{
			_SetPixel(x, y, dwColor);
		}
		else	// 递归
		{
			RecursionMin(x2_2 - x, y2_2 - y, x1, y1, x1_2, y1_2, x2, y2, x2_2, y2_2,	\
				dFast2, dSlow, dSlow2, dwColor, bNegative, dwFlag);
		}
		break;
	}
}

// 画一个像素的圆（采用横向扫描填充）
void CCommonBufferDC::BreCircle(IN int xc,				// 圆心
							IN int yc,
							IN int r,				// 半径
							IN BOOL bOddDia,		// 直径为奇数（即 d = 2r + 1）
							IN int *pPattern,		// 线型
							IN int nPatCount,
							IN DWORD dwPenColor,	// 笔的颜色
							IN int nBkMode,			// 背景模式（在非实线的情况下使用）
							IN DWORD dwBkColor,		// 背景颜色
							IN int nFillMode,		// 填充模式（在封闭图形中使用）
							IN DWORD dwFillColor)	// 填充颜色
{
	if (r < 0)		// 不处理直径小于 0 的情况
		return;

	if (r < 1)		// 直径小于 2 时只画圆心点
	{
		_SetPixel(xc, yc, dwPenColor);
		return;
	}
	// 对圆进行裁减，如果圆（包括圆弧与填充区）与裁减区域没有任何交集则不进行光栅化。
	// 如果需要进行填充，则在填充前对填充区进行第二次裁减。
	// 由于像素的特殊性与圆的对称性，当直径为偶数时，对称中心为圆心像素的左上边缘，
	// 当直径为奇数时，对称中心为圆心像素的中心点。这两种情况，都将以偶数值进行光栅化，
	// 奇数则取小于它的最大偶数，然后在对称时进行不同的处理。
	// 本函数采用圆的右下半圆弧进行光栅化，所得到的光栅点只有在直径为奇数时，才真实地
	// 位于圆弧之上，而这些光栅点的中心对称点则恒定在圆弧上，根据这一特性，在对称的过
	// 程中，使用偶数直径的坐标补偿量，对在圆心右下的坐标分量进行补偿。

	BOOL bCenterInClip = PixelInClipRect(xc, yc);	// 圆心在裁减矩形内标志
	int nEvenDiaComp = bOddDia ? 0 : -1;			// 偶数直径的坐标补偿量，只对坐标值大于 0 的分量进行补偿。

	if (nFillMode == TRANSPARENT)	// 如果圆内部是透明的，则要判断圆弧与裁减区域是否有交集。
	{
		int nDX_L = Common_Abs(xc, (int)m_rcClip.left);
		int nDX_R = Common_Abs(xc, (int)m_rcClip.right);
		int nDY_T = Common_Abs(yc, (int)m_rcClip.top);
		int nDY_B = Common_Abs(yc, (int)m_rcClip.bottom);
		int nMinDX = Common_Min(nDX_L, nDX_R), nMinDY = Common_Min(nDY_T, nDY_B);
		int nMaxDX = Common_Max(nDX_L, nDX_R), nMaxDY = Common_Max(nDY_T, nDY_B);
		int nRR = r * r;

		// 圆心在裁减区域内，但离顶点的最远距离仍小于半径，则无交集。
		// 圆心不在裁减区域内，而离顶点的最近距离小于半径，也无交集。
		if ((bCenterInClip && (nMaxDX * nMaxDX + nMaxDY * nMaxDY) <= nRR)
			|| (!bCenterInClip && (nMinDX * nMinDX + nMinDY * nMinDY) >= nRR))
			return;
	}
	else if (!bCenterInClip)	// 否则，如果圆心不在裁减区域内，还需要判断圆是不是位于区域外。
	{
		if ((xc - r) >= m_rcClip.right						// 圆的最左点在矩形的右边缘外
			|| (xc + r + nEvenDiaComp) < m_rcClip.left		// 圆的最右点在矩形的左边缘外
			|| (yc - r) >= m_rcClip.bottom					// 圆的最上点在矩形的底边缘外
			|| (yc + r + nEvenDiaComp) < m_rcClip.top)		// 圆的最下点在矩形的顶边缘外
			return;
	}

	// 光栅化圆
	int x_tmp(0), y_tmp(r), p(0);		// 起点从（0，r）开始，实际上是模拟从（0，-r）开始。
	int nPatIndex(0), nPatItem = pPattern[nPatIndex];

	p = 3 - (r << 1);
	// 当直径为奇数时，坐标轴穿越像素中心，需要特殊处理坐标轴上的点。
	if (bOddDia)
	{
		_SetPixel(xc, yc + r, dwPenColor);		// 点（0, r）
		_SetPixel(xc, yc - r, dwPenColor);		// 点（0, -r）
		_SetPixel(xc + r, yc, dwPenColor);		// 点（r, 0）
		_SetPixel(xc - r, yc, dwPenColor);		// 点（-r, 0）
		// 由于采用横向扫描填充，则横轴穿越圆心的直线会被漏掉。
		if (nFillMode == OPAQUE)
		{
			for (int i=(xc-r+1); i<(xc+r); i++)
			{
				_SetPixel(i, yc, dwFillColor);
			}
		}
	}
	// 根据像素的特点，需要将光栅化的按中心对称，因为（-0，-r）一定是圆弧上的点，
	// 而（0，r）在直径为偶数时，不是圆弧上的点，此时（0，r - 1）才是。
	while (x_tmp < y_tmp)		// 只处理数学坐标系下的第一象限的上四分之一圆弧，其它部分采用八对称特性求得。
	{
		if (IsSpace(pPattern, nPatCount, nPatIndex, nPatItem))	// 背景
		{
			PlotCirclePoints(x_tmp, y_tmp, xc, yc, bOddDia, nBkMode, dwBkColor, nFillMode, dwFillColor);
		}
		else	// 前景
		{
			PlotCirclePoints(x_tmp, y_tmp, xc, yc, bOddDia, OPAQUE, dwPenColor, nFillMode, dwFillColor);
		}
		if (p <= 0)
		{
			p += ((x_tmp << 2) + 6);
		}
		else
		{
			p += ((x_tmp << 2) - (y_tmp << 2) + 10);
			y_tmp--;
		}
		x_tmp++;
	}
	// 特殊处理对称线上的点
	if (x_tmp == y_tmp)
	{
		int nTopY = yc - y_tmp, nBottomY = yc + y_tmp + nEvenDiaComp;
		int nLeftX = xc - x_tmp, nRightX = xc + x_tmp + nEvenDiaComp;

		// 点（-x, -x）一定是圆弧上的点，而其它三个分别要对奇数直径进行判断。
		if (IsSpace(pPattern, nPatCount, nPatIndex, nPatItem))	// 背景
		{
			if (nBkMode == OPAQUE)		// 不透明
			{
				_SetPixel(nLeftX, nTopY, dwBkColor);		// 点（-x, -x）
				_SetPixel(nLeftX, nBottomY, dwBkColor);		// 点（-x, x）
				_SetPixel(nRightX, nTopY, dwBkColor);		// 点（x, -x）
				_SetPixel(nRightX, nBottomY, dwBkColor);	// 点（x, x）
			}
		}
		else	// 前景
		{
			_SetPixel(nLeftX, nTopY, dwPenColor);		// 点（-x, -x）
			_SetPixel(nLeftX, nBottomY, dwPenColor);	// 点（-x, x）
			_SetPixel(nRightX, nTopY, dwPenColor);		// 点（x, -x）
			_SetPixel(nRightX, nBottomY, dwPenColor);	// 点（x, x）
		}
		// 填充所在的水平直线
		if (nFillMode == OPAQUE)	// 不透明
		{
			// 二次裁减
			nLeftX = ClipPixelX(nLeftX + 1);
			nRightX = ClipPixelX(nRightX - 1);
			if (nTopY >= m_rcClip.top && nTopY < m_rcClip.bottom)
			{
				for (int i=nLeftX; i<=nRightX; i++)
				{
					_SetPixel(i, nTopY, dwFillColor);
				}
			}
			if (nBottomY >= m_rcClip.top && nBottomY < m_rcClip.bottom)
			{
				for (int i=nLeftX; i<=nRightX; i++)
				{
					_SetPixel(i, nBottomY, dwFillColor);
				}
			}
		}
	}
}

// 画一个像素的椭圆（采用横向扫描填充）
void	CCommonBufferDC::BreEllipse(IN int xc,		// 圆心
				   IN int yc,
				   IN int a,								// 长轴
				   IN int b,								// 短轴
				   IN BOOL bOdda,					// 长轴为奇数
				   IN BOOL bOddb,					// 短轴为奇数
				   IN int *pPattern,					// 线型
				   IN int nPatCount,
				   IN DWORD dwPenColor,			// 笔的颜色
				   IN int nBkMode,						// 背景模式（在非实线的情况下使用）
				   IN DWORD dwBkColor,			// 背景颜色
				   IN int nFillMode,					// 填充模式（在封闭图形中使用）
				   IN DWORD dwFillColor)			// 填充颜色
{
	if (a < 0 || b< 0)		// 不处理长短轴小于 0 的情况
		return;

	if (a < 1 || b < 1)		// 长短轴小于 1 时只画圆心点
	{
		_SetPixel(xc, yc, dwPenColor);
		return;
	}
	// 对圆进行裁减，如果圆（包括圆弧与填充区）与裁减区域没有任何交集则不进行光栅化。
	// 如果需要进行填充，则在填充前对填充区进行第二次裁减。
	// 由于像素的特殊性与圆的对称性，当直径为偶数时，对称中心为圆心像素的左上边缘，
	// 当直径为奇数时，对称中心为圆心像素的中心点。这两种情况，都将以偶数值进行光栅化，
	// 奇数则取小于它的最大偶数，然后在对称时进行不同的处理。
	// 本函数采用圆的右下半圆弧进行光栅化，所得到的光栅点只有在直径为奇数时，才真实地
	// 位于圆弧之上，而这些光栅点的中心对称点则恒定在圆弧上，根据这一特性，在对称的过
	// 程中，使用偶数直径的坐标补偿量，对在圆心右下的坐标分量进行补偿。

	BOOL bCenterInClip = PixelInClipRect(xc, yc);	// 圆心在裁减矩形内标志
	int nEvenDiaComp_a = bOdda ? 0 : -1;			// 偶数直径的坐标补偿量，只对坐标值大于 0 的分量进行补偿。
	int nEvenDiaComp_b = bOddb ? 0 : -1;

	if (nFillMode == TRANSPARENT)	// 如果圆内部是透明的，则要判断圆弧与裁减区域是否有交集。
	{
		int nDX_L = Common_Abs(xc, (int)m_rcClip.left);
		int nDX_R = Common_Abs(xc, (int)m_rcClip.right);
		int nDY_T = Common_Abs(yc, (int)m_rcClip.top);
		int nDY_B = Common_Abs(yc, (int)m_rcClip.bottom);
		int nMinDX = Common_Min(nDX_L, nDX_R), nMinDY = Common_Min(nDY_T, nDY_B);
		int nMaxDX = Common_Max(nDX_L, nDX_R), nMaxDY = Common_Max(nDY_T, nDY_B);
		int nRR = Common_Max(a, b);

		// 圆心在裁减区域内，但离顶点的最远距离仍小于半径，则无交集。
		// 圆心不在裁减区域内，而离顶点的最近距离小于半径，也无交集。
		if ((bCenterInClip && (nMaxDX * nMaxDX + nMaxDY * nMaxDY) <= nRR)
			|| (!bCenterInClip && (nMinDX * nMinDX + nMinDY * nMinDY) >= nRR))
			return;
	}
	else if (!bCenterInClip)	// 否则，如果圆心不在裁减区域内，还需要判断圆是不是位于区域外。
	{
		if ((xc - a) >= m_rcClip.right						// 圆的最左点在矩形的右边缘外
			|| (xc + a + nEvenDiaComp_a) < m_rcClip.left		// 圆的最右点在矩形的左边缘外
			|| (yc - b) >= m_rcClip.bottom					// 圆的最上点在矩形的底边缘外
			|| (yc + b + nEvenDiaComp_b) < m_rcClip.top)		// 圆的最下点在矩形的顶边缘外
			return;
	}

	// 光栅化圆
	int x_tmp(0), y_tmp(b), nMaxX(0);		// 起点从（0，r）开始，实际上是模拟从（0，-r）开始。
	int nPatIndex(0), nPatItem = pPattern[nPatIndex];

	// 当直径为奇数时，坐标轴穿越像素中心，需要特殊处理坐标轴上的点。
	if (bOdda)
	{
		_SetPixel(xc + b, yc, dwPenColor);		// 点（r, 0）
		_SetPixel(xc - b, yc, dwPenColor);		// 点（-r, 0）
		// 由于采用横向扫描填充，则横轴穿越圆心的直线会被漏掉。
		if (nFillMode == OPAQUE)
		{
			for (int i=(xc-a+1); i<(xc+a); i++)
			{
				_SetPixel(i, yc, dwFillColor);
			}
		}
	}
	if (bOddb)
	{
		_SetPixel(xc, yc + a, dwPenColor);		// 点（0, r）
		_SetPixel(xc, yc - a, dwPenColor);		// 点（0, -r）
	}

	while (y_tmp > 0)		// 只处理数学坐标系下的第一象限圆弧，其它部分采用四对称特性求得。
	{
		if (IsSpace(pPattern, nPatCount, nPatIndex, nPatItem))	// 背景
		{
			PlotEllipsePoints(x_tmp, y_tmp, xc, yc, bOdda, bOddb,  nBkMode, dwBkColor, nFillMode, dwFillColor);
		}
		else	// 前景
		{
			PlotEllipsePoints(x_tmp, y_tmp, xc, yc, bOdda, bOddb, OPAQUE, dwPenColor, nFillMode, dwFillColor);
		}
		if (x_tmp >= nMaxX)
		{
			y_tmp--;
			float f = 1 - pow((float)y_tmp/(float)b, 2);
			int t = (1 - pow((float)y_tmp/(float)b, 2)) *  pow((float)a, 2);
			nMaxX = (int)sqrt((1 - pow((float)y_tmp/(float)b, 2)) * pow((float)a, 2));
		}
		x_tmp = (x_tmp > nMaxX) ? nMaxX : ++x_tmp;
	}
}

// 使用BreCircle的方法，通过限制包括弧形的矩形区域来限定绘制的弧形的范围
void CCommonBufferDC::BreSector(IN int xc,				// 圆心
							IN int yc,
							IN int r,				// 半径
							IN BOOL bOddDia,		// 直径为奇数（即 d = 2r + 1）
							IN int left,
							IN int right,
							IN int top,
							IN int bottom,
							IN int *pPattern,		// 线型
							IN int nPatCount,
							IN DWORD dwPenColor,	// 笔的颜色
							IN int nBkMode,			// 背景模式（在非实线的情况下使用）
							IN DWORD dwBkColor,		// 背景颜色
							IN int nFillMode,		// 填充模式（在封闭图形中使用）
							IN DWORD dwFillColor)	// 填充颜色
{
	if (r < 0)		// 不处理直径小于 0 的情况
		return;

	if (r < 1)		// 直径小于 2 时只画圆心点
	{
		_SetPixel(xc, yc, dwPenColor);
		return;
	}
	// 对圆进行裁减，如果圆（包括圆弧与填充区）与裁减区域没有任何交集则不进行光栅化。
	// 如果需要进行填充，则在填充前对填充区进行第二次裁减。
	// 由于像素的特殊性与圆的对称性，当直径为偶数时，对称中心为圆心像素的左上边缘，
	// 当直径为奇数时，对称中心为圆心像素的中心点。这两种情况，都将以偶数值进行光栅化，
	// 奇数则取小于它的最大偶数，然后在对称时进行不同的处理。
	// 本函数采用圆的右下半圆弧进行光栅化，所得到的光栅点只有在直径为奇数时，才真实地
	// 位于圆弧之上，而这些光栅点的中心对称点则恒定在圆弧上，根据这一特性，在对称的过
	// 程中，使用偶数直径的坐标补偿量，对在圆心右下的坐标分量进行补偿。

	BOOL bCenterInClip = PixelInClipRect(xc, yc);	// 圆心在裁减矩形内标志
	int nEvenDiaComp = bOddDia ? 0 : -1;			// 偶数直径的坐标补偿量，只对坐标值大于 0 的分量进行补偿。

	if (nFillMode == TRANSPARENT)	// 如果圆内部是透明的，则要判断圆弧与裁减区域是否有交集。
	{
		int nDX_L = Common_Abs(xc, (int)m_rcClip.left);
		int nDX_R = Common_Abs(xc, (int)m_rcClip.right);
		int nDY_T = Common_Abs(yc, (int)m_rcClip.top);
		int nDY_B = Common_Abs(yc, (int)m_rcClip.bottom);
		int nMinDX = Common_Min(nDX_L, nDX_R), nMinDY = Common_Min(nDY_T, nDY_B);
		int nMaxDX = Common_Max(nDX_L, nDX_R), nMaxDY = Common_Max(nDY_T, nDY_B);
		int nRR = r * r;

		// 圆心在裁减区域内，但离顶点的最远距离仍小于半径，则无交集。
		// 圆心不在裁减区域内，而离顶点的最近距离小于半径，也无交集。
		if ((bCenterInClip && (nMaxDX * nMaxDX + nMaxDY * nMaxDY) <= nRR)
			|| (!bCenterInClip && (nMinDX * nMinDX + nMinDY * nMinDY) >= nRR))
			return;
	}
	else if (!bCenterInClip)	// 否则，如果圆心不在裁减区域内，还需要判断圆是不是位于区域外。
	{
		if ((xc - r) >= m_rcClip.right						// 圆的最左点在矩形的右边缘外
			|| (xc + r + nEvenDiaComp) < m_rcClip.left		// 圆的最右点在矩形的左边缘外
			|| (yc - r) >= m_rcClip.bottom					// 圆的最上点在矩形的底边缘外
			|| (yc + r + nEvenDiaComp) < m_rcClip.top)		// 圆的最下点在矩形的顶边缘外
			return;
	}

	// 光栅化圆
	int x_tmp(0), y_tmp(r), p(0);		// 起点从（0，r）开始，实际上是模拟从（0，-r）开始。
	int nPatIndex(0), nPatItem = pPattern[nPatIndex];

	p = 3 - (r << 1);
	// 当直径为奇数时，坐标轴穿越像素中心，需要特殊处理坐标轴上的点。
	if (bOddDia)
	{
		_SetPixel(xc, yc + r, dwPenColor);		// 点（0, r）
		_SetPixel(xc, yc - r, dwPenColor);		// 点（0, -r）
		_SetPixel(xc + r, yc, dwPenColor);		// 点（r, 0）
		_SetPixel(xc - r, yc, dwPenColor);		// 点（-r, 0）
		// 由于采用横向扫描填充，则横轴穿越圆心的直线会被漏掉。
		if (nFillMode == OPAQUE)
		{
			for (int i=(xc-r+1); i<(xc+r); i++)
			{
				_SetPixel(i, yc, dwFillColor);
			}
		}
	}
	// 根据像素的特点，需要将光栅化的按中心对称，因为（-0，-r）一定是圆弧上的点，
	// 而（0，r）在直径为偶数时，不是圆弧上的点，此时（0，r - 1）才是。
	while (x_tmp < y_tmp)		// 只处理数学坐标系下的第一象限的上四分之一圆弧，其它部分采用八对称特性求得。
	{
		if (IsSpace(pPattern, nPatCount, nPatIndex, nPatItem))	// 背景
		{
			PlotSectorPoints(x_tmp, y_tmp, xc, yc, bOddDia, left, right, top, bottom, nBkMode, dwBkColor, nFillMode, dwFillColor);
		}
		else	// 前景
		{
			PlotSectorPoints(x_tmp, y_tmp, xc, yc, bOddDia, left, right, top, bottom, OPAQUE, dwPenColor, nFillMode, dwFillColor);
		}
		if (p <= 0)
		{
			p += ((x_tmp << 2) + 6);
		}
		else
		{
			p += ((x_tmp << 2) - (y_tmp << 2) + 10);
			y_tmp--;
		}
		x_tmp++;
	}
	//// 特殊处理对称线上的点
	//if (x_tmp == y_tmp)
	//{
	//	int nTopY = yc - y_tmp, nBottomY = yc + y_tmp + nEvenDiaComp;
	//	int nLeftX = xc - x_tmp, nRightX = xc + x_tmp + nEvenDiaComp;

	//	// 点（-x, -x）一定是圆弧上的点，而其它三个分别要对奇数直径进行判断。
	//	if (IsSpace(pPattern, nPatCount, nPatIndex, nPatItem))	// 背景
	//	{
	//		if (nBkMode == OPAQUE)		// 不透明
	//		{
	//			_SetPixel(nLeftX, nTopY, dwBkColor);		// 点（-x, -x）
	//			_SetPixel(nLeftX, nBottomY, dwBkColor);		// 点（-x, x）
	//			_SetPixel(nRightX, nTopY, dwBkColor);		// 点（x, -x）
	//			_SetPixel(nRightX, nBottomY, dwBkColor);	// 点（x, x）
	//		}
	//	}
	//	else	// 前景
	//	{
	//		_SetPixel(nLeftX, nTopY, dwPenColor);		// 点（-x, -x）
	//		_SetPixel(nLeftX, nBottomY, dwPenColor);	// 点（-x, x）
	//		_SetPixel(nRightX, nTopY, dwPenColor);		// 点（x, -x）
	//		_SetPixel(nRightX, nBottomY, dwPenColor);	// 点（x, x）
	//	}
	//	// 填充所在的水平直线
	//	if (nFillMode == OPAQUE)	// 不透明
	//	{
	//		// 二次裁减
	//		nLeftX = ClipPixelX(nLeftX + 1);
	//		nRightX = ClipPixelX(nRightX - 1);
	//		if (nTopY >= m_rcClip.top && nTopY < m_rcClip.bottom)
	//		{
	//			for (int i=nLeftX; i<=nRightX; i++)
	//			{
	//				_SetPixel(i, nTopY, dwFillColor);
	//			}
	//		}
	//		if (nBottomY >= m_rcClip.top && nBottomY < m_rcClip.bottom)
	//		{
	//			for (int i=nLeftX; i<=nRightX; i++)
	//			{
	//				_SetPixel(i, nBottomY, dwFillColor);
	//			}
	//		}
	//	}
	//}
}

// 画一个像素的圆（采用横向扫描填充）
// 按角度来进行绘制
void CCommonBufferDC::_DrawSector(IN int xc,				// 圆心
							IN int yc,
							IN int r,				// 半径
							IN BOOL bOddDia,		// 直径为奇数（即 d = 2r + 1）
							IN float fStartAngle,	//起始角度
							IN float fEndAngle,		//终止角度
							IN int *pPattern,		// 线型
							IN int nPatCount,
							IN DWORD dwPenColor,	// 笔的颜色
							IN int nBkMode,			// 背景模式（在非实线的情况下使用）
							IN DWORD dwBkColor,		// 背景颜色
							IN int nFillMode,		// 填充模式（在封闭图形中使用）
							IN DWORD dwFillColor)	// 填充颜色
{
	if (r < 0)		// 不处理直径小于 0 的情况
		return;

	if (r < 1)		// 直径小于 2 时只画圆心点
	{
		_SetPixel(xc, yc, dwPenColor);
		return;
	}

	//计算出圆弧上每一个点的坐标，用直线绘制

	// 设置新起点
	int StartX = Common_FToI(r*cos(fStartAngle));;
	int StartY = Common_FToI(r*(-sin(fStartAngle)));
	int EndX = Common_FToI(r*cos(fEndAngle));
	int EndY = Common_FToI(r*(-sin(fEndAngle)));

	int left = (fStartAngle<=PAI && fEndAngle>=PAI)?-r:(StartX<EndX?StartX:EndX);
	int right = (fStartAngle<=0 && fEndAngle>=0)?r:StartX>EndX?StartX:EndX;
	int top = (fStartAngle<=PAI/2 && fEndAngle>=PAI/2)?-r:(StartY<EndY?StartY:EndY);
	int bottom = (fStartAngle<=PAI*3/2 && fEndAngle>=PAI*3/2)?r:(StartY>EndY?StartY:EndY);

	left -= m_nPenWidth+1;
	right += m_nPenWidth+1;
	top -= m_nPenWidth+1;
	bottom += m_nPenWidth+1;

	_MoveTo(xc, yc);
	_LineTo(xc+StartX, yc+StartY);

	//绘制圆弧上的点
	{
		//TODO:...
		BreSector(xc, yc, r, bOddDia, left, right, top, bottom, pPattern, nPatCount, dwPenColor, nBkMode, dwPenColor, nFillMode,dwFillColor);
	}

	_MoveTo(xc+EndX, yc+EndY);
	_LineTo(xc, yc);
/*
		// 对圆进行裁减，如果圆（包括圆弧与填充区）与裁减区域没有任何交集则不进行光栅化。
		// 如果需要进行填充，则在填充前对填充区进行第二次裁减。
		// 由于像素的特殊性与圆的对称性，当直径为偶数时，对称中心为圆心像素的左上边缘，
		// 当直径为奇数时，对称中心为圆心像素的中心点。这两种情况，都将以偶数值进行光栅化，
		// 奇数则取小于它的最大偶数，然后在对称时进行不同的处理。
		// 本函数采用圆的右下半圆弧进行光栅化，所得到的光栅点只有在直径为奇数时，才真实地
		// 位于圆弧之上，而这些光栅点的中心对称点则恒定在圆弧上，根据这一特性，在对称的过
		// 程中，使用偶数直径的坐标补偿量，对在圆心右下的坐标分量进行补偿。
	
		BOOL bCenterInClip = PixelInClipRect(xc, yc);	// 圆心在裁减矩形内标志
		int nEvenDiaComp = bOddDia ? 0 : -1;			// 偶数直径的坐标补偿量，只对坐标值大于 0 的分量进行补偿。
	
		if (nFillMode == TRANSPARENT)	// 如果圆内部是透明的，则要判断圆弧与裁减区域是否有交集。
		{
			int nDX_L = Common_Abs(xc, (int)m_rcClip.left);
			int nDX_R = Common_Abs(xc, (int)m_rcClip.right);
			int nDY_T = Common_Abs(yc, (int)m_rcClip.top);
			int nDY_B = Common_Abs(yc, (int)m_rcClip.bottom);
			int nMinDX = Common_Min(nDX_L, nDX_R), nMinDY = Common_Min(nDY_T, nDY_B);
			int nMaxDX = Common_Max(nDX_L, nDX_R), nMaxDY = Common_Max(nDY_T, nDY_B);
			int nRR = r * r;
	
			// 圆心在裁减区域内，但离顶点的最远距离仍小于半径，则无交集。
			// 圆心不在裁减区域内，而离顶点的最近距离小于半径，也无交集。
			if ((bCenterInClip && (nMaxDX * nMaxDX + nMaxDY * nMaxDY) <= nRR)
				|| (!bCenterInClip && (nMinDX * nMinDX + nMinDY * nMinDY) >= nRR))
				return;
		}
		else if (!bCenterInClip)	// 否则，如果圆心不在裁减区域内，还需要判断圆是不是位于区域外。
		{
			if ((xc - r) >= m_rcClip.right						// 圆的最左点在矩形的右边缘外
				|| (xc + r + nEvenDiaComp) < m_rcClip.left		// 圆的最右点在矩形的左边缘外
				|| (yc - r) >= m_rcClip.bottom					// 圆的最上点在矩形的底边缘外
				|| (yc + r + nEvenDiaComp) < m_rcClip.top)		// 圆的最下点在矩形的顶边缘外
				return;
		}
	
		// 光栅化圆
		int x_tmp(0), y_tmp(r), p(0);		// 起点从（0，r）开始，实际上是模拟从（0，-r）开始。
		int nPatIndex(0), nPatItem = pPattern[nPatIndex];
	
		p = 3 - (r << 1);
		// 当直径为奇数时，坐标轴穿越像素中心，需要特殊处理坐标轴上的点。
		if (bOddDia)
		{
			_SetPixel(xc, yc + r, dwPenColor);		// 点（0, r）
			_SetPixel(xc, yc - r, dwPenColor);		// 点（0, -r）
			_SetPixel(xc + r, yc, dwPenColor);		// 点（r, 0）
			_SetPixel(xc - r, yc, dwPenColor);		// 点（-r, 0）
			// 由于采用横向扫描填充，则横轴穿越圆心的直线会被漏掉。
			if (nFillMode == OPAQUE)
			{
				for (int i=(xc-r+1); i<(xc+r); i++)
				{
					_SetPixel(i, yc, dwFillColor);
				}
			}
		}
		// 根据像素的特点，需要将光栅化的按中心对称，因为（-0，-r）一定是圆弧上的点，
		// 而（0，r）在直径为偶数时，不是圆弧上的点，此时（0，r - 1）才是。
		while (x_tmp < y_tmp)		// 只处理数学坐标系下的第一象限的上四分之一圆弧，其它部分采用八对称特性求得。
		{
			if (IsSpace(pPattern, nPatCount, nPatIndex, nPatItem))	// 背景
			{
				PlotCirclePoints(x_tmp, y_tmp, xc, yc, bOddDia, nBkMode, dwBkColor, nFillMode, dwFillColor);
			}
			else	// 前景
			{
				PlotCirclePoints(x_tmp, y_tmp, xc, yc, bOddDia, OPAQUE, dwPenColor, nFillMode, dwFillColor);
			}
			if (p <= 0)
			{
				p += ((x_tmp << 2) + 6);
			}
			else
			{
				p += ((x_tmp << 2) - (y_tmp << 2) + 10);
				y_tmp--;
			}
			x_tmp++;
		}*/
	
/*
	// 特殊处理对称线上的点
	if (x_tmp == y_tmp)
	{
		int nTopY = yc - y_tmp, nBottomY = yc + y_tmp + nEvenDiaComp;
		int nLeftX = xc - x_tmp, nRightX = xc + x_tmp + nEvenDiaComp;

		// 点（-x, -x）一定是圆弧上的点，而其它三个分别要对奇数直径进行判断。
		if (IsSpace(pPattern, nPatCount, nPatIndex, nPatItem))	// 背景
		{
			if (nBkMode == OPAQUE)		// 不透明
			{
				_SetPixel(nLeftX, nTopY, dwBkColor);		// 点（-x, -x）
				_SetPixel(nLeftX, nBottomY, dwBkColor);		// 点（-x, x）
				_SetPixel(nRightX, nTopY, dwBkColor);		// 点（x, -x）
				_SetPixel(nRightX, nBottomY, dwBkColor);	// 点（x, x）
			}
		}
		else	// 前景
		{
			_SetPixel(nLeftX, nTopY, dwPenColor);		// 点（-x, -x）
			_SetPixel(nLeftX, nBottomY, dwPenColor);	// 点（-x, x）
			_SetPixel(nRightX, nTopY, dwPenColor);		// 点（x, -x）
			_SetPixel(nRightX, nBottomY, dwPenColor);	// 点（x, x）
		}
		// 填充所在的水平直线
		if (nFillMode == OPAQUE)	// 不透明
		{
			// 二次裁减
			nLeftX = ClipPixelX(nLeftX + 1);
			nRightX = ClipPixelX(nRightX - 1);
			if (nTopY >= m_rcClip.top && nTopY < m_rcClip.bottom)
			{
				for (int i=nLeftX; i<=nRightX; i++)
				{
					_SetPixel(i, nTopY, dwFillColor);
				}
			}
			if (nBottomY >= m_rcClip.top && nBottomY < m_rcClip.bottom)
			{
				for (int i=nLeftX; i<=nRightX; i++)
				{
					_SetPixel(i, nBottomY, dwFillColor);
				}
			}
		}
	}*/

}

// 画一个像素的圆（采用横向扫描填充）
// 按起始终止点来绘制
void CCommonBufferDC::_DrawSector(IN int xc,				// 圆心
							IN int yc,
							IN int r,					// 半径
							IN BOOL bOddDia,			// 直径为奇数（即 d = 2r + 1）
							IN POINT ptBegin,			//起始坐标
							IN POINT ptEnd,				//终止坐标
							IN int *pPattern,			// 线型
							IN int nPatCount,
							IN DWORD dwPenColor,		// 笔的颜色
							IN int nBkMode,				// 背景模式（在非实线的情况下使用）
							IN DWORD dwBkColor,			// 背景颜色
							IN int nFillMode,			// 填充模式（在封闭图形中使用）
							IN DWORD dwFillColor)		// 填充颜色
{
	if (r < 0)		// 不处理直径小于 0 的情况
		return;

	if (r < 1)		// 直径小于 2 时只画圆心点
	{
		_SetPixel(xc, yc, dwPenColor);
		return;
	}

	//计算出圆弧上每一个点的坐标，用直线绘制

	// 设置新起点
	int StartX = ptBegin.x - xc;
	int StartY = ptBegin.y - yc;
	int EndX = ptEnd.x - xc;
	int EndY = ptEnd.y - yc;

	int left = ( StartY<=0 && EndY>=0 )?-r:(StartX<EndX?StartX:EndX);
	int right = ( StartY>=0 && EndY<=0 )?r:(StartX>EndX?StartX:EndX);
	int top = ( StartX>=0 && EndX<=0 && StartY<=0 )?-r:(StartY<EndY?StartY:EndY);	
	int bottom = ( StartX<=0 && EndX>=0 && EndY>=0 )?r:(StartY>EndY?StartY:EndY);

	//int left = (fStartAngle<=PAI && fEndAngle>=PAI)?-r:(StartX<EndX?StartX:EndX);
	//int right = StartX>EndX?StartX:EndX;
	//int top = (fStartAngle<=PAI/2 && fEndAngle>=PAI/2)?-r:(StartY<EndY?StartY:EndY);
	//int bottom = (fStartAngle<=PAI*3/2 && fEndAngle>=PAI*3/2)?r:(StartY>EndY?StartY:EndY);

	left -= m_nPenWidth+1;
	right += m_nPenWidth+1;
	top -= m_nPenWidth+1;
	bottom += m_nPenWidth+1;

	_MoveTo(xc, yc);
	_LineTo(xc+StartX, yc+StartY);

	//绘制圆弧上的点
	{
		//TODO:...
		BreSector(xc, yc, r, bOddDia, left, right, top, bottom, pPattern, nPatCount, dwPenColor, nBkMode, dwPenColor, nFillMode,dwFillColor);
	}

	_MoveTo(xc+EndX, yc+EndY);
	_LineTo(xc, yc);
}

// 画一个像素的圆（采用横向扫描填充）
// 按起始终止点来绘制
void CCommonBufferDC::_DrawArc(IN int xc,				// 圆心
						   IN int yc,
						   IN int r,					// 半径
						   IN BOOL bOddDia,			// 直径为奇数（即 d = 2r + 1）
						   IN POINT ptBegin,			//起始坐标
						   IN POINT ptEnd,				//终止坐标
						   IN int *pPattern,			// 线型
						   IN int nPatCount,
						   IN DWORD dwPenColor,		// 笔的颜色
						   IN int nBkMode,				// 背景模式（在非实线的情况下使用）
						   IN DWORD dwBkColor,			// 背景颜色
						   IN int nFillMode,			// 填充模式（在封闭图形中使用）
						   IN DWORD dwFillColor)		// 填充颜色
{
	if (r < 0)		// 不处理直径小于 0 的情况
		return;

	if (r < 1)		// 直径小于 2 时只画圆心点
	{
		_SetPixel(xc, yc, dwPenColor);
		return;
	}

	//计算出圆弧上每一个点的坐标，用直线绘制

	// 设置新起点
	int StartX = ptBegin.x - xc;
	int StartY = ptBegin.y - yc;
	int EndX = ptEnd.x - xc;
	int EndY = ptEnd.y - yc;

	int left = ( StartY<=0 && EndY>=0 )?-r:(StartX<EndX?StartX:EndX);
	int right = ( StartY>=0 && EndY<=0 )?r:(StartX>EndX?StartX:EndX);
	int top = ( StartX>=0 && EndX<=0 && StartY<=0 )?-r:(StartY<EndY?StartY:EndY);	
	int bottom = ( StartX<=0 && EndX>=0 && EndY>=0 )?r:(StartY>EndY?StartY:EndY);

	//int left = (fStartAngle<=PAI && fEndAngle>=PAI)?-r:(StartX<EndX?StartX:EndX);
	//int right = StartX>EndX?StartX:EndX;
	//int top = (fStartAngle<=PAI/2 && fEndAngle>=PAI/2)?-r:(StartY<EndY?StartY:EndY);
	//int bottom = (fStartAngle<=PAI*3/2 && fEndAngle>=PAI*3/2)?r:(StartY>EndY?StartY:EndY);

	left -= m_nPenWidth+1;
	right += m_nPenWidth+1;
	top -= m_nPenWidth+1;
	bottom += m_nPenWidth+1;

	//绘制圆弧上的点
	{
		//TODO:...
		BreSector(xc, yc, r, bOddDia, left, right, top, bottom, pPattern, nPatCount, dwPenColor, nBkMode, dwPenColor, nFillMode,dwFillColor);
	}
}

// 利用八对称特性通过圆弧上的一个点画出八个对称点（并横向扫描填充）
// 由于 BreCirle 中的光栅化算法使用了圆弧的右下半段，而这段圆弧只有在奇数直径
// 的情况下才属于圆弧，而其中心对称的圆弧段恒定在圆弧上，所以此接口的输入 x、y
// 必定为正值，在内部对直径的奇偶性进行特殊处理。
void CCommonBufferDC::PlotCirclePoints(IN int x,				// 输入正的光栅化点，则（-x, -y）一定在圆弧上
								   IN int y,
								   IN int xc,				// 圆心
								   IN int yc,
								   IN BOOL bOddDia,			// 直径为奇数（即 d = 2r + 1）
								   IN int nPenMode,			// 笔的模式（透明与不透明的区别）
								   IN DWORD dwPenColor,		// 笔的颜色
								   IN int nFillMode,		// 填充模式（在封闭图形中使用）
								   IN DWORD dwFillColor)	// 填充颜色
{
	if (x == 0 || y == 0 || x == y || x == -y)		// 不处理对称线上的点
		return;

	int nEvenDiaComp = bOddDia ? 0 : -1;
	int x_l_far(0), x_l_near(0), x_r_near(0), x_r_far(0);
	int y_t_far(0), y_t_near(0), y_b_near(0), y_b_far(0);

	if (nPenMode == OPAQUE)		// 不透明
	{
		// 求解各个顶点的坐标分量
		x_l_far = xc - y;
		x_l_near = xc - x;
		x_r_near = xc + x + nEvenDiaComp;
		x_r_far = xc + y + nEvenDiaComp;
		y_t_far = yc - y;
		y_t_near = yc - x;
		y_b_near = yc + x + nEvenDiaComp;
		y_b_far = yc + y + nEvenDiaComp;
		// 填充点
		_SetPixel(x_l_near, y_t_far, dwPenColor);		// 已知点 0
		_SetPixel(x_l_far, y_t_near, dwPenColor);		// 点 0 关于直线 y = x 对称的点 1
		_SetPixel(x_l_near, y_b_far, dwPenColor);		// 点 0 关于直线 x = 0 对称的点 2
		_SetPixel(x_r_near, y_t_far, dwPenColor);		// 点 0 关于直线 y = 0 对称的点 3
		_SetPixel(x_r_near, y_b_far, dwPenColor);		// 点 0 关于原点中心对称的点 4
		_SetPixel(x_l_far, y_b_near, dwPenColor);		// 点 1 关于直线 x = 0 对称的点 5
		_SetPixel(x_r_far, y_t_near, dwPenColor);		// 点 1 关于直线 y = 0 对称的点 6
		_SetPixel(x_r_far, y_b_near, dwPenColor);		// 点 1 关于原点中心对称的点 7
	}
	if (nFillMode == OPAQUE)	// 不透明（横向扫描填充）
	{
		if (nPenMode != OPAQUE)		// 说明未计算坐标分量
		{
			// 求解各个顶点的坐标分量
			x_l_far = xc - y;
			x_l_near = xc - x;
			x_r_near = xc + x + nEvenDiaComp;
			x_r_far = xc + y + nEvenDiaComp;
			y_t_far = yc - y;
			y_t_near = yc - x;
			y_b_near = yc + x + nEvenDiaComp;
			y_b_far = yc + y + nEvenDiaComp;
		}
		// 二次裁减
		x_l_far = ClipPixelX(x_l_far + 1);
		x_l_near = ClipPixelX(x_l_near + 1);
		x_r_near = ClipPixelX(x_r_near - 1);
		x_r_far = ClipPixelX(x_r_far - 1);
		if (y_t_far >= m_rcClip.top && y_t_far < m_rcClip.bottom)
		{
			for (int i=x_l_near; i<=x_r_near; i++)
			{
				_SetPixel(i, y_t_far, dwFillColor);
			}
		}
		if (y_t_near >= m_rcClip.top && y_t_near < m_rcClip.bottom)
		{
			for (int i=x_l_far; i<=x_r_far; i++)
			{
				_SetPixel(i, y_t_near, dwFillColor);
			}
		}
		if (y_b_near >= m_rcClip.top && y_b_near < m_rcClip.bottom)
		{
			for (int i=x_l_far; i<=x_r_far; i++)
			{
				_SetPixel(i, y_b_near, dwFillColor);
			}
		}
		if (y_b_far >= m_rcClip.top && y_b_far < m_rcClip.bottom)
		{
			for (int i=x_l_near; i<=x_r_near; i++)
			{
				_SetPixel(i, y_b_far, dwFillColor);
			}
		}
	}
}

void	CCommonBufferDC::PlotEllipsePoints(IN int x,		// 输入正的光栅化点，则（-x, -y）一定在圆弧上
						  IN int y,
						  IN int xc,								// 圆心
						  IN int yc,
						  IN BOOL bOdda,						// 长轴为奇数（即 d = 2r + 1）
						  IN BOOL bOddb,						// 短轴为奇数（即 d = 2r + 1）
						  IN int nPenMode,						// 笔的模式（透明与不透明的区别）
						  IN DWORD dwPenColor,				// 笔的颜色
						  IN int nFillMode,						// 填充模式（在封闭图形中使用）
						  IN DWORD dwFillColor)				// 填充颜色
{
	if (x == 0 || y == 0 || x == y || x == -y)		// 不处理对称线上的点
		return;

	int nEvenDiaComp_a = bOdda ? 0 : -1;
	int nEvenDiaComp_b = bOddb ? 0 : -1;
	int x_l_far(0), x_l_near(0), x_r_near(0), x_r_far(0);
	int y_t_far(0), y_t_near(0), y_b_near(0), y_b_far(0);

	if (nPenMode == OPAQUE)		// 不透明
	{
		// 求解各个顶点的坐标分量
		//x_l_far = xc - y;
		x_l_near = xc - x;
		x_r_near = xc + x + nEvenDiaComp_a;
		//x_r_far = xc + y + nEvenDiaComp_a;
		y_t_far = yc - y;
		//y_t_near = yc - x;
		//y_b_near = yc + x + nEvenDiaComp_b;
		y_b_far = yc + y + nEvenDiaComp_b;
		// 填充点
		_SetPixel(x_l_near, y_t_far, dwPenColor);		// 已知点 0
		//_SetPixel(x_l_far, y_t_near, dwPenColor);		// 点 0 关于直线 y = x 对称的点 1
		_SetPixel(x_l_near, y_b_far, dwPenColor);		// 点 0 关于直线 x = 0 对称的点 2
		_SetPixel(x_r_near, y_t_far, dwPenColor);		// 点 0 关于直线 y = 0 对称的点 3
		_SetPixel(x_r_near, y_b_far, dwPenColor);		// 点 0 关于原点中心对称的点 4
		//_SetPixel(x_l_far, y_b_near, dwPenColor);		// 点 1 关于直线 x = 0 对称的点 5
		//_SetPixel(x_r_far, y_t_near, dwPenColor);		// 点 1 关于直线 y = 0 对称的点 6
		//_SetPixel(x_r_far, y_b_near, dwPenColor);		// 点 1 关于原点中心对称的点 7
	}
	if (nFillMode == OPAQUE)	// 不透明（横向扫描填充）
	{
		if (nPenMode != OPAQUE)		// 说明未计算坐标分量
		{
			// 求解各个顶点的坐标分量
			//x_l_far = xc - y;
			x_l_near = xc - x;
			x_r_near = xc + x + nEvenDiaComp_a;
			//x_r_far = xc + y + nEvenDiaComp_a;
			y_t_far = yc - y;
			//y_t_near = yc - x;
			//y_b_near = yc + x + nEvenDiaComp_b;
			y_b_far = yc + y + nEvenDiaComp_b;
		}
		// 二次裁减
		//x_l_far = ClipPixelX(x_l_far + 1);
		x_l_near = ClipPixelX(x_l_near + 1);
		x_r_near = ClipPixelX(x_r_near - 1);
		//x_r_far = ClipPixelX(x_r_far - 1);
		if (y_t_far >= m_rcClip.top && y_t_far < m_rcClip.bottom)
		{
			for (int i=x_l_near; i<=x_r_near; i++)
			{
				_SetPixel(i, y_t_far, dwFillColor);
			}
		}
// 		if (y_t_near >= m_rcClip.top && y_t_near < m_rcClip.bottom)
// 		{
// 			for (int i=x_l_far; i<=x_r_far; i++)
// 			{
// 				_SetPixel(i, y_t_near, dwFillColor);
// 			}
// 		}
// 		if (y_b_near >= m_rcClip.top && y_b_near < m_rcClip.bottom)
// 		{
// 			for (int i=x_l_far; i<=x_r_far; i++)
// 			{
// 				_SetPixel(i, y_b_near, dwFillColor);
// 			}
// 		}
		if (y_b_far >= m_rcClip.top && y_b_far < m_rcClip.bottom)
		{
			for (int i=x_l_near; i<=x_r_near; i++)
			{
				_SetPixel(i, y_b_far, dwFillColor);
			}
		}
	}
}

void CCommonBufferDC::PlotSectorPoints(IN int x,				// 输入正的光栅化点，则（-x, -y）一定在圆弧上
								   IN int y,
								   IN int xc,				// 圆心
								   IN int yc,
								   IN BOOL bOddDia,			// 直径为奇数（即 d = 2r + 1）
								   IN int left,
								   IN int right,
								   IN int top,
								   IN int bottom,
								   IN int nPenMode,			// 笔的模式（透明与不透明的区别）
								   IN DWORD dwPenColor,		// 笔的颜色
								   IN int nFillMode,		// 填充模式（在封闭图形中使用）
								   IN DWORD dwFillColor)	// 填充颜色
{
	if (x == 0 || y == 0 || x == y || x == -y)		// 不处理对称线上的点
		return;

	int nEvenDiaComp = bOddDia ? 0 : -1;
	int x_l_far(0), x_l_near(0), x_r_near(0), x_r_far(0);
	int y_t_far(0), y_t_near(0), y_b_near(0), y_b_far(0);

	if (nPenMode == OPAQUE)		// 不透明
	{
		// 求解各个顶点的坐标分量
		x_l_far = xc - y;
		x_l_near = xc - x;
		x_r_near = xc + x + nEvenDiaComp;
		x_r_far = xc + y + nEvenDiaComp;
		y_t_far = yc - y;
		y_t_near = yc - x;
		y_b_near = yc + x + nEvenDiaComp;
		y_b_far = yc + y + nEvenDiaComp;

		left += xc;
		right += xc;
		top += yc;
		bottom += yc;

		// 填充点
		if ((left<x_l_near&&x_l_near<right) && (top<y_t_far&&y_t_far<bottom))
		{
			_SetPixel(x_l_near, y_t_far, dwPenColor);		// 已知点 0
		}
		if ((left<x_l_far&&x_l_far<right) && (top<y_t_near&&y_t_near<bottom))
		{
			_SetPixel(x_l_far, y_t_near, dwPenColor);		// 点 0 关于直线 y = x 对称的点 1
		}
		if ((left<x_l_near&&x_l_near<right) && (top<y_b_far&&y_b_far<bottom))
		{
			_SetPixel(x_l_near, y_b_far, dwPenColor);		// 点 0 关于直线 x = 0 对称的点 2
		}
		if ((left<x_r_near&&x_r_near<right) && (top<y_t_far&&y_t_far<bottom))
		{
			_SetPixel(x_r_near, y_t_far, dwPenColor);		// 点 0 关于直线 y = 0 对称的点 3
		}
		if ((left<x_r_near&&x_r_near<right) && (top<y_b_far&&y_b_far<bottom))
		{
			_SetPixel(x_r_near, y_b_far, dwPenColor);		// 点 0 关于原点中心对称的点 4
		}
		if ((left<x_l_far&&x_l_far<right) && (top<y_b_near&&y_b_near<bottom))
		{
			_SetPixel(x_l_far, y_b_near, dwPenColor);		// 点 1 关于直线 x = 0 对称的点 5
		}
		if ((left<x_r_far&&x_r_far<right) && (top<y_t_near&&y_t_near<bottom))
		{
			_SetPixel(x_r_far, y_t_near, dwPenColor);		// 点 1 关于直线 y = 0 对称的点 6
		}
		if ((left<x_r_far&&x_r_far<right) && (top<y_b_near&&y_b_near<bottom))
		{
			_SetPixel(x_r_far, y_b_near, dwPenColor);		// 点 1 关于原点中心对称的点 7
		}
	}
	if (nFillMode == OPAQUE)	// 不透明（横向扫描填充）
	{
		if (nPenMode != OPAQUE)		// 说明未计算坐标分量
		{
			// 求解各个顶点的坐标分量
			x_l_far = xc - y;
			x_l_near = xc - x;
			x_r_near = xc + x + nEvenDiaComp;
			x_r_far = xc + y + nEvenDiaComp;
			y_t_far = yc - y;
			y_t_near = yc - x;
			y_b_near = yc + x + nEvenDiaComp;
			y_b_far = yc + y + nEvenDiaComp;
		}
		// 二次裁减
		x_l_far = ClipPixelX(x_l_far + 1);
		x_l_near = ClipPixelX(x_l_near + 1);
		x_r_near = ClipPixelX(x_r_near - 1);
		x_r_far = ClipPixelX(x_r_far - 1);
		if (y_t_far >= m_rcClip.top && y_t_far < m_rcClip.bottom)
		{
			for (int i=x_l_near; i<=x_r_near; i++)
			{
				_SetPixel(i, y_t_far, dwFillColor);
			}
		}
		if (y_t_near >= m_rcClip.top && y_t_near < m_rcClip.bottom)
		{
			for (int i=x_l_far; i<=x_r_far; i++)
			{
				_SetPixel(i, y_t_near, dwFillColor);
			}
		}
		if (y_b_near >= m_rcClip.top && y_b_near < m_rcClip.bottom)
		{
			for (int i=x_l_far; i<=x_r_far; i++)
			{
				_SetPixel(i, y_b_near, dwFillColor);
			}
		}
		if (y_b_far >= m_rcClip.top && y_b_far < m_rcClip.bottom)
		{
			for (int i=x_l_near; i<=x_r_near; i++)
			{
				_SetPixel(i, y_b_far, dwFillColor);
			}
		}
	}
}

void CCommonBufferDC::RecursionBezier3(IN pointf *pPtBezier,	// 四个点
								   IN int *pPattern,		// 线型
								   IN int nPatCount,
								   IN int &nPatIndex,		// 索引
								   IN int &nPatItem,		// 项的点数
								   IN DWORD dwPenColor,		// 笔的颜色
								   IN int nBkMode,			// 背景模式（在非实线的情况下使用）
								   IN DWORD dwBkColor)		// 背景颜色
{
	if (IsBezier3NearLine(pPtBezier))	// 接近直线则画出
	{
		// 利用 nPatIndex 与 nPatItem 在直线或曲线的绘制过程中的作用完全相同的特性，实现直线与曲线共用同一种算法实现虚线模式
		Bresenham(Common_FToI(pPtBezier[0].x), Common_FToI(pPtBezier[0].y),	\
			Common_FToI(pPtBezier[3].x), Common_FToI(pPtBezier[3].y),	\
			pPattern, nPatCount, nPatIndex, nPatItem, dwPenColor, nBkMode, dwBkColor);
	}
	else		// 二分法拆分曲线
	{
		pointf pPtFirst[POINT_BEZIER_COUNT], pPtSecond[POINT_BEZIER_COUNT];

		SplitBezier3(pPtBezier, pPtFirst, pPtSecond);
		RecursionBezier3(pPtFirst, pPattern, nPatCount, nPatIndex, nPatItem, dwPenColor, nBkMode, dwBkColor);
		RecursionBezier3(pPtSecond, pPattern, nPatCount, nPatIndex, nPatItem, dwPenColor, nBkMode, dwBkColor);
	}
}

// 递归画出一条贝塞尔曲线，在画每一段前，对该段的端点进行投影变换
void CCommonBufferDC::RecursionBezier3(IN pointf *pPtBezier,		// 四个点
								   IN matrix4f* pMat,		// 投影矩阵
								   IN int *pPattern,			// 线型
								   IN int nPatCount,
								   IN int &nPatIndex,			// 当前使用线型中某个索引
								   IN int &nPatItem,			// 当前索引项的点数
								   IN DWORD dwPenColor,			// 笔色
								   IN int nBkMode,				// 背景模式（在非实线的情况下使用）
								   IN DWORD dwBkColor)			// 背景颜色
{
	if (IsBezier3NearLine(pPtBezier))	// 接近直线则画出
	{
		// 投影变换
		vector4f v1, v2;
		float fTmp1(0.f), fTmp2(0.f);

		float4Set(&v1, pPtBezier[0].x, pPtBezier[0].y, 0.f, 1.f);
		float4Set(&v2, pPtBezier[3].x, pPtBezier[3].y, 0.f, 1.f);
		float4MulMatQ(&v1, &v1, pMat);
		float4MulMatQ(&v2, &v2, pMat);
		v1.w += 0.000001f;		// 避免除0
		v2.w += 0.000001f;
#ifdef WIN64
		assert(0);
#else
		_asm {		// 使用SSE优化齐次化过程
			movss		xmm0, v1.w;
			movss		xmm1, v2.w;
			rcpss		xmm0, xmm0;
			rcpss		xmm1, xmm1;
			movss		fTmp1, xmm0;
			movss		fTmp2, xmm1;
		}
#endif // WIN64
		float4Scale(&v1, fTmp1);
		float4Scale(&v2, fTmp2);
		// 利用 nPatIndex 与 nPatItem 在直线或曲线的绘制过程中的作用完全相同的特性，实现直线与曲线共用同一种算法实现虚线模式
		Bresenham(Common_FToI(v1.x), Common_FToI(v1.y), Common_FToI(v2.x), Common_FToI(v2.y),	\
			pPattern, nPatCount, nPatIndex, nPatItem, dwPenColor, nBkMode, dwBkColor);
	}
	else		// 二分法拆分曲线
	{
		pointf pPtFirst[POINT_BEZIER_COUNT], pPtSecond[POINT_BEZIER_COUNT];

		SplitBezier3(pPtBezier, pPtFirst, pPtSecond);
		RecursionBezier3(pPtFirst, pMat, pPattern, nPatCount, nPatIndex, nPatItem, dwPenColor, nBkMode, dwBkColor);
		RecursionBezier3(pPtSecond, pMat, pPattern, nPatCount, nPatIndex, nPatItem, dwPenColor, nBkMode, dwBkColor);
	}
}
BOOL	Common_Beizer3IsNearLine(pointf* pt0,pointf* pt1,pointf* pt2,pointf* pt3,float fscale)
{
	//判断是否接近一条直线,如果接近一条直线,那么就用直线代替否则分割该曲线
	//直线的参数方程
	//x = x0+t*(x1-x0)
	//y = y0+t*(y1-y0)
	//t = (x-x0)/(x1-x0)
	//直线的一般方程
	//y = y0+x*((y1-y0)/(x1-x0))

	//判断控制点是否在两个基本点之间。只要t>=0&&t<=1
	float f30X  = (pt3->x-pt0->x) * fscale;
	float f10X  = (pt1->x-pt0->x) * fscale;
	float f20X  = (pt2->x-pt0->x) * fscale;
	if(f30X == 0.0f)
	{
		if(fabs(f10X)<0.5f&&fabs(f20X)<=0.5f)
			return TRUE;
	}
	/*	if(f10X>f30X||(f10X>0&&f30X<0)||(f10X<0&&f30X>0)||
	f20X>f30X||(f20X>0&&f30X<0)||(f20X<0&&f30X>0))
	{
	//t<0||t>1
	return FALSE;

	}
	else*/
	{
		float t1 = f10X/f30X;
		float fdy = (pt3->y-pt0->y) * fscale;
		float fy1 = pt0->y * fscale + t1*fdy;
		if(fabs(pt1->y * fscale - fy1)<0.1f)//Y相差
		{
			float t2 = f20X/f30X;
			float fy2 = pt0->y * fscale + t2*fdy;
			if(fabs(pt2->y * fscale-fy2) < 0.1f)//Y相差
			{
				return TRUE;
				//接近直线
			}
		}
		return FALSE;
		//需要分割

	}
}
BOOL CCommonBufferDC::IsBezier3NearLine(IN pointf *pPtBezier)		// 四个点
{
	pointf* ptemp = (pointf*)pPtBezier;
	return Common_Beizer3IsNearLine(&ptemp[0],&ptemp[1],&ptemp[2],&ptemp[3],1.0f);

	//float dx = pPtBezier[3].x - pPtBezier[0].x;
	//float dy = pPtBezier[3].y - pPtBezier[0].y;

	//if(dx == 0 && dy == 0)
	//	return TRUE;

	//
	//float d1 = fabsf(((pPtBezier[1].x - pPtBezier[3].x) * dy - (pPtBezier[1].y - pPtBezier[3].y) * dx));
	//float d2 = fabsf(((pPtBezier[2].x - pPtBezier[3].x) * dy - (pPtBezier[2].y - pPtBezier[3].y) * dx));

	//

	//if (((d1 + d2) * (d1 + d2)) < (BEZIER_DIST_LIMIT * (dx * dx + dy * dy)))
	//	return TRUE;

	//return FALSE;
}

void CCommonBufferDC::SplitBezier3(IN pointf *pPtOrigin,		// 原始贝塞尔
							   OUT pointf *pPtFirst,		// 第一段（前段）
							   OUT pointf *pPtSecond)		// 第二段（后段）
{
	pointf pt12;

	pt12.x = (pPtOrigin[1].x + pPtOrigin[2].x) * 0.5f;		// P12 = (P1 + P2) / 2
	pt12.y = (pPtOrigin[1].y + pPtOrigin[2].y) * 0.5f;

	pPtFirst[0].x = pPtOrigin[0].x;		// P0
	pPtFirst[0].y = pPtOrigin[0].y;
	pPtFirst[1].x = (pPtOrigin[0].x + pPtOrigin[1].x) * 0.5f;	// P01 = (P0 + P1) / 2
	pPtFirst[1].y = (pPtOrigin[0].y + pPtOrigin[1].y) * 0.5f;
	pPtFirst[2].x = (pPtFirst[1].x + pt12.x) * 0.5f;		// P012 = (P01 + P12) / 2
	pPtFirst[2].y = (pPtFirst[1].y + pt12.y) * 0.5f;

	pPtSecond[3].x = pPtOrigin[3].x;	// P3
	pPtSecond[3].y = pPtOrigin[3].y;
	pPtSecond[2].x = (pPtOrigin[2].x + pPtOrigin[3].x) * 0.5f;	// P23 = (P2 + P3) / 2
	pPtSecond[2].y = (pPtOrigin[2].y + pPtOrigin[3].y) * 0.5f;
	pPtSecond[1].x = (pt12.x + pPtSecond[2].x) * 0.5f;		// P123 = (P12 + P23) / 2
	pPtSecond[1].y = (pt12.y + pPtSecond[2].y) * 0.5f;

	pPtFirst[3].x = pPtSecond[0].x = (pPtFirst[2].x + pPtSecond[1].x) * 0.5f;	// P0123 = (P012 + P123) / 2
	pPtFirst[3].y = pPtSecond[0].y = (pPtFirst[2].y + pPtSecond[1].y) * 0.5f;
}

// 释放DIB缓存操作
void CCommonBufferDC::ReleaseDIB()
{
	if (m_hMemDC == NULL)
		return;

	if (m_hOldBM != NULL)		// 如果设置过缓存则需要恢复现场
	{
		::SelectObject(m_hMemDC, (HGDIOBJ)m_hOldBM);
		m_hOldBM = NULL;
	}
	if (m_hMemBM != NULL)
	{
		::DeleteObject((HGDIOBJ)m_hMemBM);
		m_hMemBM = NULL;
		m_nMemWidth = m_nMemHeight = 0;
		m_pdwMemBM = NULL;
	}
}

// 计算文本在DIB缓存中的绘制区域，始终做到文本都将顶齐DIB缓存的左上角点被画出，而不管文本的方向如何。
void CCommonBufferDC::CalcTextDrawParam(OUT RECT &rcDraw,			// 文本的绘制区域
									OUT int& nDesX,				// 实际拷贝到m_pdwBuffer上的位置
									OUT int& nDesY,
									OUT int& nFillWidth,		// 需要填充Alpha通道的大小，也是实际需要拷贝的大小
									OUT int& nFillHeight,
									IN const LPRECT lpRect)		// 调用者指定的区域
{
	LONG lWidth = lpRect->right - lpRect->left;
	LONG lHeight = lpRect->bottom - lpRect->top;

	// 根据文本绘制的方向计算需要填充Alpha通道的区域，始终认为填充起点为DIB缓存的左上角点。
	// 计算在DIB缓存中文本绘制的原点，不管方向如何，原点坐标都为DrawText函数输入矩形的左上角点。
	// 文本的角度是以原点坐标进行逆时针旋转的，以水平状况下的顶端水平线为X轴。
	switch (m_nTextDirection)
	{
	case SYTEXT_DIRECTION_ANGLE_0:		// 0度
	case SYTEXT_DIRECTION_ANGLE_270:	// 270度//纵向排布有问题，先不做处理
		rcDraw.left = 0L;
		rcDraw.right = rcDraw.left + lWidth;
		rcDraw.top = 0L;
		rcDraw.bottom = rcDraw.top + lHeight;
		nDesX = (int)lpRect->left;
		nDesY = (int)lpRect->top;
		nFillWidth = (int)lWidth;
		nFillHeight = (int)lHeight;
		break;
	case SYTEXT_DIRECTION_ANGLE_90:		// 90度
		rcDraw.left = 0L;
		rcDraw.right = rcDraw.left + lWidth;
		rcDraw.top = lWidth;
		rcDraw.bottom = rcDraw.top + lHeight;
		nDesX = (int)lpRect->left;
		nDesY = (int)(lpRect->top - lWidth);
		nFillWidth = (int)lHeight;
		nFillHeight = (int)lWidth;
		break;
	case SYTEXT_DIRECTION_ANGLE_180:	// 180度
		rcDraw.left = 0L;
		rcDraw.right = rcDraw.left + lWidth;
		rcDraw.top = 0L;
		rcDraw.bottom = rcDraw.top + lHeight;
		nDesX = (int)(lpRect->left - lWidth);
		nDesY = (int)(lpRect->top - lHeight);
		nFillWidth = (int)lWidth;
		nFillHeight = (int)lHeight;
		break;
	//case SYTEXT_DIRECTION_ANGLE_270:	// 270度
	//	rcDraw.left = lHeight;
	//	rcDraw.right = rcDraw.left + lWidth;
	//	rcDraw.top = 0L;
	//	rcDraw.bottom = rcDraw.top + lHeight;
	//	nDesX = (int)(lpRect->left - lHeight);
	//	nDesY = (int)lpRect->top;
	//	nFillWidth = (int)lHeight;
	//	nFillHeight = (int)lWidth;
	//	break;
	case SYTEXT_DIRECTION_QUADRANT_1:	// 第一象限
		rcDraw.left = 0L;
		rcDraw.right = rcDraw.left + lWidth;
		rcDraw.top = (LONG)ceilf(lWidth * m_fTextAngleSin);
		rcDraw.bottom = rcDraw.top + lHeight;
		nDesX = (int)lpRect->left;
		nDesY = (int)(lpRect->top - rcDraw.top);
		nFillWidth = (int)ceilf(lWidth * m_fTextAngleCos + lHeight * m_fTextAngleSin);
		nFillHeight = (int)(lpRect->top + (LONG)ceilf(lHeight * m_fTextAngleCos)) - nDesY;
		break;
	case SYTEXT_DIRECTION_QUADRANT_2:	// 第二象限
		rcDraw.left = (LONG)ceilf(-lWidth * m_fTextAngleCos);
		rcDraw.right = rcDraw.left + lWidth;
		rcDraw.top = (LONG)ceilf(lWidth * m_fTextAngleSin - lHeight * m_fTextAngleCos);
		rcDraw.bottom = rcDraw.top + lHeight;
		nDesX = (int)(lpRect->left - rcDraw.left);
		nDesY = (int)(lpRect->top - rcDraw.top);
		nFillWidth = (int)(lpRect->left + (LONG)ceilf(lHeight * m_fTextAngleSin)) - nDesX;
		nFillHeight = (int)rcDraw.top;
		break;
	case SYTEXT_DIRECTION_QUADRANT_3:	// 第三象限
		rcDraw.left = (LONG)ceilf(-lHeight * m_fTextAngleSin - lWidth * m_fTextAngleCos);
		rcDraw.right = rcDraw.left + lWidth;
		rcDraw.top = (LONG)ceilf(-lHeight * m_fTextAngleCos);
		rcDraw.bottom = rcDraw.top + lHeight;
		nDesX = (int)(lpRect->left - rcDraw.left);
		nDesY = (int)(lpRect->top - rcDraw.top);
		nFillWidth = (int)rcDraw.left;
		nFillHeight = (int)(lpRect->top - (LONG)floorf(lWidth * m_fTextAngleSin)) - nDesY;
		break;
	case SYTEXT_DIRECTION_QUADRANT_4:	// 第四象限
		rcDraw.left = (LONG)ceilf(-lHeight * m_fTextAngleSin);
		rcDraw.right = rcDraw.left + lWidth;
		rcDraw.top = 0L;
		rcDraw.bottom = rcDraw.top + lHeight;
		nDesX = (int)(lpRect->left - rcDraw.left);
		nDesY = (int)lpRect->top;
		nFillWidth = (int)(lpRect->left + (LONG)ceilf(lWidth * m_fTextAngleCos)) - nDesX;
		nFillHeight = (int)(lHeight * m_fTextAngleCos - lWidth * m_fTextAngleSin);
		break;
	default:
		//assert(FALSE);
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
// 文件结束
