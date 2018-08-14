//////////////////////////////////////////////////////////////////////////
// Copyright (c) Shineon Corporation. All rights reserved.
//
// Description:	仿CDC类，实现对32bit缓存的绘制功能。内部颜色为0xARGB格式。
//////////////////////////////////////////////////////////////////////////
#pragma once
// 此内存DC中存在两个参考坐标系：
//     一个是屏幕坐标系，以内存的起点为参考原点，即内存矩形的左上顶点，
// 绘制的内容将与最终显示在屏幕上的图像吻合；
//     一个是客户坐标系，可以设定屏幕坐标系的任意一点为参考原点（可以超出
// 内存操作区域），还可以设定缩放系数。
//     两个坐标系都以内存的宽高方向建立 X、Y 坐标轴，在使用客户坐标系进行
// 绘制时，需要先将坐标变换到屏幕坐标系下。严格定义，两个坐标系之间只存在
// 缩放与位移变换，不存在旋转变换。
//     缺省状态下，客户坐标系等于屏幕坐标系。

// 提供两套接口：
//     一套的输入参考屏幕坐标系，以下划线开头，适用于内部的代码重用与光栅
// 化过程，因为最终的绘制都将在这个坐标系下进行。
//     一套的输入参考客户坐标系，一般情况下用户使用的都是这套接口，在接口
// 的内部进行坐标系变换，然后调用以下划线开头的接口。

// 裁减矩形区必须以屏幕坐标系为参考，如果裁减区为不规则图形，则使用它最小外包
// 矩形，在绘制直线的过程中对端点进行减裁，减少光栅化的负担。

// 为用户提供绘制区域记录功能，即用户可以添加已绘制过的矩形区域，用于快速提取
// 绘制后的有效图像，此矩形区必须以屏幕坐标系为参考，且包含于裁减矩形。
// 目前只提供一个外包有效矩形，可扩展为有效矩形序列，以获得更高的裁减效率。
#include "Common\2DBufferAPI.h"
#include "Common\BezierDef.h"

class BASEGRAPH_COMMON_API CCommonBufferDC
{
public:
	CCommonBufferDC(void);
	virtual ~CCommonBufferDC(void);

	enum SYBORDER_STYLE		// 边框类型
	{
		SYBORDER_RAISED = 0,		// 凸起
		SYBORDER_SUCKEN,			// 凹陷
	};

	//////////////////////////////////////////////////////////////////////////
	// 通用属性访问接口
public:
	// 绑定 Buffer，如果已经有绑定返回 FALSE。
	BOOL	AttachBuffer(IN DWORD *pdwBuffer,	// 指定内存
						 IN int nWidth,			// 内存大小
						 IN int nHeight);
	// 解除绑定（返回绑定的内存）	
	DWORD*	DetachBuffer();
	// 获取绑定对象的属性
	DWORD*	GetBuffer() const;
	int		GetBufferWidth() const;
	int		GetBufferHeight() const;
	void	GetBufferSize(OUT int &nWidth, OUT int &nHeight);
	// 清空屏幕为某一种颜色
	void	ClearBuffer(IN DWORD dwColor);
	void	ClearRect(IN int x, IN int y, IN int nWidth, IN int nHeight, IN DWORD dwColor);
	// 设置获取绘制的裁剪区域，输入 NULL 时为全屏可见，裁剪区域为左上闭右下开。
	void		SetClipRect(IN const RECT *prcClip);
	void		GetClipRect(OUT RECT &rcClip);
	const RECT& GetClipRect() const;
	// 设置获取绘制后的有效图像区域，用于最后提取有效图像（左上闭右下开）。
	void		InitValidRect(IN const RECT *prcValid);		// 为 NULL 时表示空矩形
	void		AddValidRect(IN const RECT &rcValid);
	void		GetValidRect(OUT RECT &rcValid);
	const RECT& GetValidRect() const;
	// 设置获取客户坐标系的参考信息
	POINT	SetOffsetRef(IN int x, IN int y);
	POINT	SetOffsetRef(IN float x, IN float y);
	POINT	SetOffsetRef(IN POINT pt);
	POINT	GetOffsetRef() const;
	float	SetZoomRef(IN float fZoom);
	float	GetZoomRef() const;
	// 设置获取画笔的属性（返回旧属性）
	int 	SetPenStyle(IN int nPenStyle);		// 兼容 GDI
	int 	GetPenStyle() const;
	int 	SetPenWidth(IN int nWidth);
	int 	GetPenWidth() const;
	DWORD	SetPenColor(IN DWORD dwColor);
	DWORD	GetPenColor() const;
	void	SetPen(IN int nPenStyle, IN int nWidth, IN DWORD dwColor);
	// 设置获取背景属性（返回旧属性）
	DWORD	SetBkColor(IN DWORD dwColor);
	DWORD	GetBkColor() const;
	int 	SetBkMode(IN int nBkMode);		// 兼容 GDI
	int 	GetBkMode() const;
	// 设置获取填充属性（返回旧属性）（模式与背景相同）
	DWORD	SetFillColor(IN DWORD dwColor);
	DWORD	GetFillColor() const;
	int 	SetFillMode(IN int nFillMode);
	int 	GetFillMode() const;
	// 设置获取绘制模式（返回旧模式）
	int		SetROP2(IN int nDrawMode);
	int		GetROP2() const;
	// 设置自定义线型（一个像素宽的直线）{ 直线段、虚线段、直线段、…… }
	BOOL	SetLinePattern(IN const int *pPattern,	// 线型数组
						   IN int nPatCount);		// 数组大小
	// 文本绘制
	HFONT	SetFont(IN HFONT hNewFont);			// 新字体
	HFONT	GetFont() const;
	DWORD	SetTextColor(IN DWORD dwColor);		// 文本颜色
	DWORD	GetTextColor() const;
	SIZE		GetTextExtent(IN LPCTSTR lpszString,	// 文本
						  IN int nCount);
	// 使能GDI绘制帮助
	BOOL		EnableGDIHelp(IN BOOL bEnable,		// 使能标志
						  IN int nDCWidth,		// 缓存DC大小
						  IN int nDCHeight);

	//////////////////////////////////////////////////////////////////////////
	// 客户坐标系下的绘制接口
public:
	// 贴图
	BOOL	BitBlt(IN int x,					// 目标位置
				   IN int y,
				   IN int nWidth,				// 目标区域大小
				   IN int nHeight,
				   IN DWORD* pSrc,				// 源
				   IN int xSrc,					// 源位置
				   IN int ySrc,
				   IN int nSrcWidth,			// 源区域大小
				   IN int nSrcHeight,
				   IN int nSrcBufWidth,			// 源大小
				   IN int nSrcBufHeight,
				   IN BUFFER_MODE eMode,		// 混合操作类型
				   IN BUFFER_PARITY_STYLE eParity);	// 奇偶场类型
	// 设置某一点（点在可见区域内时返回 TRUE）
	BOOL	SetPixel(IN float x,			// 点位置
					 IN float y,
					 IN DWORD dwColor);		// 颜色
	BOOL	SetPixel(IN const pointf &pt,	// 点位置
					 IN DWORD dwColor);		// 颜色
	BOOL	SetPixel(IN int x,				// 点位置
					 IN int y,
					 IN DWORD dwColor);		// 颜色
	BOOL	SetPixel(IN const POINT &pt,	// 点位置
					 IN DWORD dwColor);		// 颜色
	// 画线
	void	MoveTo(IN float x, IN float y);
	void	MoveTo(IN const pointf &pt);
	void	MoveTo(IN int x, IN int y);
	void	MoveTo(IN const POINT &pt);
	BOOL	LineTo(IN float x, IN float y);
	BOOL	LineTo(IN const pointf &pt);
	BOOL	LineTo(IN int x, IN int y);
	BOOL	LineTo(IN const POINT &pt);
	// 画矩形
	void	DrawRect(IN float l, IN float t, IN float r, IN float b);
	void	DrawRect(IN const pointf &ptLT, IN const pointf &ptRB);
	void	DrawRect(IN const rectf &rc);
	void	DrawRect(IN int l, IN int t, IN int r, IN int b);
	void	DrawRect(IN const POINT &ptLT, IN const POINT &ptRB);
	void	DrawRect(IN const RECT &rc);
	// 填充矩形，此处填充色与内部填充色变量m_dwFillColor无关
	void	FillRect(IN float l, IN float t, IN float r, IN float b, IN DWORD dwColor);
	void	FillRect(IN const pointf &ptLT, IN const pointf &ptRB, IN DWORD dwColor);
	void	FillRect(IN const rectf & rc, IN DWORD dwColor);
	void	FillRect(IN int l, IN int t, IN int r, IN int b, IN DWORD dwColor);
	void	FillRect(IN const POINT &ptLT, IN const POINT &ptRB, IN DWORD dwColor);
	void	FillRect(IN const RECT& rc, IN DWORD dwColor);
	// 画圆
	void	DrawCircle(IN float xc,					// 圆心
					   IN float yc,
					   IN int r,					// 半径
					   IN BOOL bOddDia = TRUE);		// 直径为奇数（即 d = 2r + 1）
	void	DrawCircle(IN const pointf &ptC,		// 圆心
					   IN int r,					// 半径
					   IN BOOL bOddDia = TRUE);		// 直径为奇数（即 d = 2r + 1）
	void	DrawCircle(IN int xc,					// 圆心
					   IN int yc,
					   IN int r,					// 半径
					   IN BOOL bOddDia = TRUE);		// 直径为奇数（即 d = 2r + 1）
	void	DrawCircle(IN const POINT &ptC,			// 圆心
					   IN int r,					// 半径
					   IN BOOL bOddDia = TRUE);		// 直径为奇数（即 d = 2r + 1）
	// 画椭圆
	void DrawEllipse(IN float xc,					// 圆心
						IN float yc,
						IN int a,							// 长轴
						IN int b,							// 短轴
						IN BOOL bOdda = TRUE,		// 长轴为奇数
						IN BOOL bOddb = TRUE);	// 短轴为奇数
	void DrawEllipse(IN const pointf &ptC,		// 圆心
		IN int a,											// 长轴
		IN int b,											// 短轴
		IN BOOL bOdda = TRUE,						// 长轴为奇数
		IN BOOL bOddb = TRUE);					// 短轴为奇数
	void DrawEllipse(IN int xc,						// 圆心
		IN int yc,
		IN int a,											// 长轴
		IN int b,											// 短轴
		IN BOOL bOdda = TRUE,						// 长轴为奇数
		IN BOOL bOddb = TRUE);					// 短轴为奇数
	void DrawEllipse(IN const POINT &ptC,		// 圆心
		IN int a,											// 长轴
		IN int b,											// 短轴
		IN BOOL bOdda = TRUE,						// 长轴为奇数
		IN BOOL bOddb = TRUE);					// 短轴为奇数

	// 画弧线
	void DrawArc(
		IN int nCenterX, 
		IN int nCenterY, 
		IN pointf ptStart, 
		IN pointf ptEnd);

	void	DrawSector(int nCenterX, int nCenterY, int nRadius, float fBeginAngle, float fEndAngle);	// 按角度来进行绘制
	void	DrawSector(int nCenterX, int nCenterY, int nRadius, POINT ptBegin, POINT ptEnd);			// 按起始终止点来绘制
	// 画三次贝塞尔曲线（如果数组大小为 3 * n，则自动闭合曲线)
	void	DrawBezier3(IN const pointf *pPtBezier,	// 曲线点与控制点的数组（P0 C1 C2 P1 C3 C4 P2 ……）
						IN int nPtCount);				// 数组大小（3 * n + 1）
	void	DrawBezier3(IN const POINT *pPtBezier,	// 曲线点与控制点的数组（P0 C1 C2 P1 C3 C4 P2 ……）
						IN int nPtCount);				// 数组大小（3 * n + 1）
	// 在绘制三次贝塞尔曲线时使用投影矩阵，如果"pMat == NULL"，则进入正常绘制流程。
	void	DrawBezier3(IN const pointf *pPtBezier,		// 曲线点与控制点的数组（P0 C1 C2 P1 C3 C4 P2 ……）
						IN int nPtCount,					// 数组大小（3 * n + 1）
						IN const matrix4f* pMat);		// 投影矩阵
	void	DrawBezier3(IN const POINT *pPtBezier,		// 曲线点与控制点的数组（P0 C1 C2 P1 C3 C4 P2 ……）
						IN int nPtCount,					// 数组大小（3 * n + 1）
						IN const matrix4f* pMat);		// 投影矩阵
	// 画边框
	void	DrawBorder(IN float l,					// 目标区域
					   IN float t,
					   IN float r,
					   IN float b,
					   IN DWORD dwShadowColor,		// 暗色
                       IN DWORD dwLightColor,		// 亮色
					   IN SYBORDER_STYLE eStyle,	// 风格
					   IN int nBorderWidth = 1);	// 边框宽度
	void	DrawBorder(IN const rectf &rcDest,	// 目标区域
					   IN DWORD dwShadowColor,		// 暗色
					   IN DWORD dwLightColor,		// 亮色
					   IN SYBORDER_STYLE eStyle,	// 风格
					   IN int nBorderWidth = 1);	// 边框宽度
	void	DrawBorder(IN int l,					// 目标区域
					   IN int t,
					   IN int r,
					   IN int b,
					   IN DWORD dwShadowColor,		// 暗色
					   IN DWORD dwLightColor,		// 亮色
					   IN SYBORDER_STYLE eStyle,	// 风格
					   IN int nBorderWidth = 1);	// 边框宽度
	void	DrawBorder(IN const RECT &rcDest,		// 目标区域
					   IN DWORD dwShadowColor,		// 暗色
					   IN DWORD dwLightColor,		// 亮色
					   IN SYBORDER_STYLE eStyle,	// 风格
					   IN int nBorderWidth = 1);	// 边框宽度
	// 如果成功则返回文本高度，否则返回0。
	int		DrawText(IN LPCTSTR lpszString,		// 文本
					 IN int nCount,
					 IN OUT LPRECT lpRect,		// 目标区域
					 IN UINT uFormat,			// 格式（兼容GDI）
					 IN BOOL bBKTrans = FALSE);	// 是否在透明背景上绘制文字

	//////////////////////////////////////////////////////////////////////////
	// 屏幕坐标系下的绘制接口（以下划线开头）
public:
	// 判断点或矩形是否可见
	BOOL	_PixelVisible(IN POINT pt);			// 像素位置
	BOOL	_PixelVisible(IN int x, IN int y);	// 像素位置
	BOOL	_RectVisible(IN const RECT& rc);	// 矩形区域
	// 贴图
	BOOL	_BitBlt(IN int x,						// 目标位置
					IN int y,
					IN int nWidth,					// 目标区域大小
					IN int nHeight,
					IN DWORD* pSrc,					// 源
					IN int xSrc,					// 源位置
					IN int ySrc,
					IN int nSrcWidth,				// 源区域大小
					IN int nSrcHeight,
					IN int nSrcBufWidth,			// 源大小
					IN int nSrcBufHeight,
					IN BUFFER_MODE eMode,		// 混合操作类型
					IN BUFFER_PARITY_STYLE eParity);	// 奇偶场类型
	// 画线
	void	_MoveTo(IN int x, IN int y);
	BOOL	_LineTo(IN int x, IN int y);
	// 设置某一点（点在可见区域内时返回 TRUE）
	BOOL	_SetPixel(IN int x,				// 点位置
					  IN int y,
					  IN DWORD dwColor);		// 颜色
	// 画矩形
	void	_DrawRect(IN int l, IN int t, IN int r, IN int b);
	// 填充矩形，此处填充色与内部填充色变量m_dwFillColor无关
	void	_FillRect(IN int l, IN int t, IN int r, IN int b, IN DWORD dwColor);
	// 画圆（由于采用对称算法，只会画出偶数直径的圆）
	void	_DrawCircle(IN int xc,			// 圆心
					   IN int yc,
					   IN int r,			// 半径
					   IN BOOL bOddDia);	// 直径为奇数（即 d = 2r + 1）
	// 画椭圆
	void _DrawEllipse(IN int xc,					// 圆心
						IN int yc,
						IN int a,							// 长径
						IN int b,							// 短轴
						IN BOOL bOdda,				// 长轴为奇数
						IN BOOL bOddb);				// 短轴为奇数
	// 画三次贝塞尔曲线（如果数组大小为 3 * n，则自动闭合曲线)
	void	_DrawBezier3(IN const POINT *pPtBezier,	// 曲线点与控制点的数组（P0 C1 C2 P1 C3 C4 P2 ……）
						IN int nPtCount);				// 数组大小（3 * n + 1）
	void	_DrawBezier3(IN const pointf *pPtBezier,	// 曲线点与控制点的数组（P0 C1 C2 P1 C3 C4 P2 ……）
						IN int nPtCount);				// 数组大小（3 * n + 1）
	// 在绘制三次贝塞尔曲线时使用投影矩阵，如果"pMat == NULL"，则进入正常绘制流程。
	void	_DrawBezier3(IN const POINT *pPtBezier,		// 曲线点与控制点的数组（P0 C1 C2 P1 C3 C4 P2 ……）
						IN int nPtCount,					// 数组大小（3 * n + 1）
						IN const matrix4f* pMat);		// 投影矩阵
	void	_DrawBezier3(IN const pointf *pPtBezier,		// 曲线点与控制点的数组（P0 C1 C2 P1 C3 C4 P2 ……）
						IN int nPtCount,					// 数组大小（3 * n + 1）
						IN const matrix4f* pMat);		// 投影矩阵
	// 画边框
	void	_DrawBorder(IN int l,					// 目标区域
						IN int t,
						IN int r,
						IN int b,
						IN DWORD dwShadowColor,		// 暗色
						IN DWORD dwLightColor,		// 亮色
						IN SYBORDER_STYLE eStyle,	// 风格
						IN int nBorderWidth);		// 边框宽度
	// 如果成功则返回文本高度，否则返回0。
	int		_DrawText(IN LPCTSTR lpszString,	// 文本
					  IN int nCount,
					  IN OUT LPRECT lpRect,			// 目标区域
					  IN UINT uFormat,				// 格式（兼容GDI）
					  IN BOOL bBKTrans = FALSE);	// 是否在透明背景上绘制文字

protected:
	// 拷贝线型（一个像素直线的线型）
	void	CopyLinePattern(IN const int *pPattern,	// 线型数组
							IN int nPatCount);		// 数组大小
	// 根据宽度及直线斜率缩放线型
	void	ScalePattern(OUT int *pPattern,	// 线型数组
						 IN int nPatCount,	// 数组大小
						 IN float fScale);	// 缩放比例
	// 画一个像素的直线（不包含终点）
	void	Bresenham(IN int x1,				// 起点
					  IN int y1,
					  IN int x2,				// 终点
					  IN int y2,
					  IN const int *pPattern,	// 线型
					  IN int nPatCount,
                      IN int &nPatIndex,		// 索引
					  IN int &nPatItem,			// 项的点数
					  IN DWORD dwPenColor,		// 笔的颜色
					  IN int nBkMode,			// 背景模式（在非实线的情况下使用）
					  IN DWORD dwBkColor);		// 背景颜色
	// 裁减线段并返回线段的斜率
	DWORD	ClipLine(IN OUT int &x1,			// 起点
					 IN OUT int &y1,
					 IN OUT int &x2,			// 终点
					 IN OUT int &y2,
					 OUT float &fPatScale,		// 线型缩放系数
					 OUT int &nNewWidth,		// 新的线宽（由于斜率的原因，需要进行线宽的变化）
					 OUT float &fDelta,			// 用于宽斜线绘制中的端点处理参数
					 IN int nWidth);			// 线宽
	// 裁减坐标的分量
	int		ClipPixelX(IN int x);
	int		ClipPixelY(IN int y);
	// 判断像素是否在裁减矩形内
	BOOL	PixelInClipRect(IN int x, IN int y);
	BOOL	PixelInClipRect(IN POINT pt);
	// 检查线型是否为空隙（如果是空隙则返回 TRUE）
	BOOL	IsSpace(IN const int *pPattern,		// 线型
					IN int nPatCount,			// 线型大小
					IN int &nPatIndex,			// 索引
					IN int &nPatItem);			// 项的点数
	// 画宽线
	void	FillWideLine(IN int x1,				// 起点
						 IN int y1,
						 IN int x2,				// 终点
						 IN int y2,
						 IN int nWidth,			// 线宽（宽度为 1 的直线请使用 Bresenham 算法）
						 IN int *pPattern,		// 单像素线型
						 IN int nPatCount,
						 IN DWORD dwPenColor,	// 笔的颜色
						 IN int nBkMode,		// 背景模式（在非实线的情况下使用）
						 IN DWORD dwBkColor);	// 背景颜色
	// 往栈内推点，如果栈满则返回 TRUE，默认栈存在且是安全的。
	BOOL	PushPtInStack(IN int x, IN int y);
	// 根据堆栈填充连续的宽线段（采用线刷子的方式填充）
	void	FillLineFromStack(IN float fDelta,		// 两个端点的特殊处理区域
							  IN int nSub,			// 宽度的两个边界
							  IN int nSuper,
							  IN int dFast2,		// 主轴（变化快的轴）上的端点差的双倍值
							  IN int dSlow,			// 副轴（变化慢的轴）上的端点差
							  IN int dSlow2,		// 副轴（变化慢的轴）上的端点差的双倍值
							  IN DWORD dwColor,		// 颜色
							  IN DWORD dwFlag);		// 根据直线的走势定义的标志
	// 病态的处理线刷子端点（实际意义不大）
	void	ProcessLineCaput(IN int min,			// 起点的特殊处理阈值
							 IN int max,			// 终点的特殊处理阈值
							 IN int nSub,			// 线刷子底限
							 IN int nSuper,			// 线刷子上限
							 IN int dFast2,			// 快轴端点差的双倍
							 IN int dSlow,			// 慢轴端点差
							 IN int dSlow2,			// 慢轴端点差的双倍
							 IN int x1,				// 起点坐标
							 IN int y1,
							 IN int x1_2,			// 起点坐标的双倍
							 IN int y1_2,
							 IN int x2,				// 终点坐标
							 IN int y2,
							 IN int x2_2,			// 终点坐标的双倍
							 IN int y2_2,
							 IN DWORD dwColor,		// 颜色
							 IN BOOL bNegative,		// 慢轴增长为负值的标志
							 IN DWORD dwFlag);		// 根据直线的走势定义的标志
	void	RecursionMin(IN int x,					// 当前点
						 IN int y,
						 IN int x1,					// 起点坐标
						 IN int y1,
						 IN int x1_2,				// 起点坐标的双倍
						 IN int y1_2,
						 IN int x2,					// 终点坐标
						 IN int y2,
						 IN int x2_2,				// 终点坐标的双倍
						 IN int y2_2,
						 IN int dFast2,				// 快轴端点差的双倍
						 IN int dSlow,				// 慢轴端点差
						 IN int dSlow2,				// 慢轴端点差的双倍
						 IN DWORD dwColor,			// 颜色
						 IN BOOL bNegative,			// 慢轴增长为负值的标志
						 IN DWORD dwFlag);			// 根据直线的走势定义的标志
	void	RecursionMax(IN int x,					// 当前点
						 IN int y,
						 IN int x1,					// 起点坐标
						 IN int y1,
						 IN int x1_2,				// 起点坐标的双倍
						 IN int y1_2,
						 IN int x2,					// 终点坐标
						 IN int y2,
						 IN int x2_2,				// 终点坐标的双倍
						 IN int y2_2,
						 IN int dFast2,				// 快轴端点差的双倍
						 IN int dSlow,				// 慢轴端点差
						 IN int dSlow2,				// 慢轴端点差的双倍
						 IN DWORD dwColor,			// 颜色
						 IN BOOL bNegative,			// 快轴增长为负值的标志
						 IN DWORD dwFlag);			// 根据直线的走势定义的标志
	// 画一个像素的圆（采用横向扫描填充）
	void	BreCircle(IN int xc,				// 圆心
					  IN int yc,
					  IN int r,					// 半径
					  IN BOOL bOddDia,			// 直径为奇数（即 d = 2r + 1）
					  IN int *pPattern,			// 线型
					  IN int nPatCount,
					  IN DWORD dwPenColor,		// 笔的颜色
					  IN int nBkMode,			// 背景模式（在非实线的情况下使用）
					  IN DWORD dwBkColor,		// 背景颜色
					  IN int nFillMode,			// 填充模式（在封闭图形中使用）
					  IN DWORD dwFillColor);	// 填充颜色

	// 画一个像素的椭圆（采用横向扫描填充）
	void	BreEllipse(IN int xc,					// 圆心
						IN int yc,
						IN int a,						// 长轴
						IN int b,						// 短轴
						IN BOOL bOdda,			// 长轴为奇数
						IN BOOL bOddb,			// 短轴为奇数
						IN int *pPattern,			// 线型
						IN int nPatCount,
						IN DWORD dwPenColor,	// 笔的颜色
						IN int nBkMode,			// 背景模式（在非实线的情况下使用）
						IN DWORD dwBkColor,	// 背景颜色
						IN int nFillMode,			// 填充模式（在封闭图形中使用）
						IN DWORD dwFillColor);	// 填充颜色


	// 使用BreCircle的方法，通过限制包括弧形的矩形区域来限定绘制的弧形的范围
	void	BreSector(IN int xc,				// 圆心
					IN int yc,
					IN int r,					// 半径
					IN BOOL bOddDia,			// 直径为奇数（即 d = 2r + 1）
					IN int left,
					IN int right,
					IN int top,
					IN int bottom,
					IN int *pPattern,			// 线型
					IN int nPatCount,
					IN DWORD dwPenColor,		// 笔的颜色
					IN int nBkMode,				// 背景模式（在非实线的情况下使用）
					IN DWORD dwBkColor,			// 背景颜色
					IN int nFillMode,			// 填充模式（在封闭图形中使用）
					IN DWORD dwFillColor);		// 填充颜色


	// 画一个像素的圆（采用横向扫描填充）
	// 按角度来进行绘制
	void	_DrawSector(IN int xc,				// 圆心
					IN int yc,
					IN int r,					// 半径
					IN BOOL bOddDia,			// 直径为奇数（即 d = 2r + 1）
					IN float fStartAngle,		//起始角度
					IN float fEndAngle,			//终止角度
					IN int *pPattern,			// 线型
					IN int nPatCount,
					IN DWORD dwPenColor,		// 笔的颜色
					IN int nBkMode,				// 背景模式（在非实线的情况下使用）
					IN DWORD dwBkColor,			// 背景颜色
					IN int nFillMode,			// 填充模式（在封闭图形中使用）
					IN DWORD dwFillColor);		// 填充颜色

	// 画一个像素的圆（采用横向扫描填充）
	// 按起始终止点来绘制
	void	_DrawSector(IN int xc,				// 圆心
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
					IN DWORD dwFillColor);		// 填充颜色

	void _DrawArc(IN int xc,					// 圆心
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
					IN DWORD dwFillColor);		// 填充颜色

	// 利用八对称特性通过圆弧上的一个点画出八个对称点（并横向扫描填充）
	// 由于 BreCirle 中的光栅化算法使用了圆弧的右下半段，而这段圆弧只有在奇数直径
	// 的情况下才属于圆弧，而其中心对称的圆弧段恒定在圆弧上，所以此接口的输入 x、y
	// 必定为正值，在内部对直径的奇偶性进行特殊处理。
	void	PlotCirclePoints(IN int x,				// 输入正的光栅化点，则（-x, -y）一定在圆弧上
							 IN int y,
							 IN int xc,				// 圆心
							 IN int yc,
							 IN BOOL bOddDia,		// 直径为奇数（即 d = 2r + 1）
							 IN int nPenMode,		// 笔的模式（透明与不透明的区别）
							 IN DWORD dwPenColor,	// 笔的颜色
							 IN int nFillMode,		// 填充模式（在封闭图形中使用）
							 IN DWORD dwFillColor);	// 填充颜色

	void	PlotEllipsePoints(IN int x,		// 输入正的光栅化点，则（-x, -y）一定在圆弧上
		IN int y,
		IN int xc,								// 圆心
		IN int yc,
		IN BOOL bOdda,						// 长轴为奇数（即 d = 2r + 1）
		IN BOOL bOddb,						// 短轴为奇数（即 d = 2r + 1）
		IN int nPenMode,						// 笔的模式（透明与不透明的区别）
		IN DWORD dwPenColor,				// 笔的颜色
		IN int nFillMode,						// 填充模式（在封闭图形中使用）
		IN DWORD dwFillColor);				// 填充颜色

	void	PlotSectorPoints(IN int x,					// 输入正的光栅化点，则（-x, -y）一定在圆弧上
							IN int y,
							IN int xc,					// 圆心
							IN int yc,
							IN BOOL bOddDia,			// 直径为奇数（即 d = 2r + 1）
							IN int left,
							IN int right,
							IN int top,
							IN int bottom,
							IN int nPenMode,			// 笔的模式（透明与不透明的区别）
							IN DWORD dwPenColor,		// 笔的颜色
							IN int nFillMode,			// 填充模式（在封闭图形中使用）
							IN DWORD dwFillColor);		// 填充颜色
	// 递归画出一条贝塞尔曲线
	void	RecursionBezier3(IN pointf *pPtBezier,	// 四个点
							 IN int *pPattern,			// 线型
							 IN int nPatCount,
							 IN int &nPatIndex,			// 索引
							 IN int &nPatItem,			// 项的点数
							 IN DWORD dwPenColor,		// 笔的颜色
							 IN int nBkMode,			// 背景模式（在非实线的情况下使用）
							 IN DWORD dwBkColor);		// 背景颜色
	// 判断一段贝塞尔曲线的是否已经接近直线
	BOOL	IsBezier3NearLine(IN pointf *pPtBezier);	// 四个点
	// 二分法拆分一条贝塞尔曲线为两段
	void	SplitBezier3(IN pointf *pPtOrigin,		// 原始贝塞尔
						 OUT pointf *pPtFirst,		// 第一段（前段）
						 OUT pointf *pPtSecond);		// 第二段（后段）
	// 递归画出一条贝塞尔曲线，在画每一段前，对该段的端点进行投影变换
	void	RecursionBezier3(IN pointf *pPtBezier,	// 四个点
							 IN matrix4f* pMat,		// 投影矩阵
							 IN int *pPattern,			// 线型
							 IN int nPatCount,
							 IN int &nPatIndex,			// 当前使用线型中某个索引
							 IN int &nPatItem,			// 当前索引项的点数
							 IN DWORD dwPenColor,		// 笔色
							 IN int nBkMode,			// 背景模式（在非实线的情况下使用）
							 IN DWORD dwBkColor);		// 背景颜色
	// 释放DIB缓存操作
	void	ReleaseDIB();
	// 计算文本在DIB缓存中的绘制区域，始终做到文本都将顶齐DIB缓存的左上角点被画出，而不管文本的方向如何。
	void	CalcTextDrawParam(OUT RECT &rcDraw,				// 文本的绘制区域
							  OUT int &nDesX,				// 实际拷贝到m_pdwBuffer上的位置
							  OUT int &nDesY,
							  OUT int &nFillWidth,			// 需要填充Alpha通道的大小，也是实际需要拷贝的大小
							  OUT int &nFillHeight,
							  IN const LPRECT lpRect);		// 调用者指定的区域
	// 坐标系转换（先缩放后平移）
	void	ClientToScreen(IN OUT float &x, IN OUT float &y);
	void	ClientToScreen(IN OUT pointf &pt);
	void	ClientToScreen(IN OUT rectf &rc);
	void	ClientToScreen(IN OUT int &x, IN OUT int &y);
	void	ClientToScreen(IN OUT POINT &pt);
	void	ClientToScreen(IN OUT RECT &rc);
	float	DoZoomRef(IN float x);

private:
	// Buffer 相关参数
	BOOL	m_bAttached;		// 绑定标志
	DWORD	*m_pdwBuffer;		// 绑定的内存
	int		m_nWidth;			// 内存大小
	int		m_nHeight;
	RECT	m_rcClip;			// 可见的绘制区域（以屏幕坐标系为参考）
	RECT	m_rcValid;			// 绘制后的有效图像区域（以屏幕坐标系为参考）
	POINT	m_ptOffsetRef;		// 客户坐标系的原点（以屏幕坐标系为参考）
	float	m_fZoomRef;			// 客户坐标系的缩放系数（以屏幕坐标系为参考）
	// 画笔的属性
	int		m_nPenStyle;		// 画笔风格
	int		m_nPenWidth;		// 线条宽度
	DWORD	m_dwPenColor;		// 画笔颜色
	int		m_nDrawMode;		// 绘制模式
	// 背景属性
	int		m_nBkMode;			// 背景模式
	DWORD	m_dwBkColor;		// 背景颜色
	// 填充属性
	int		m_nFillMode;		// 填充模式
	DWORD	m_dwFillColor;		// 填充颜色
	// 线的属性
	int		*m_pPattern;		// 线型数组
	int		m_nPatCount;		// 数组大小（多数情况为偶数）
	POINT	m_ptStart;			// 画线时记录起点
	// 文本属性
	HFONT	m_hFont;			// 字体
	DWORD	m_dwTextColor;		// 文本颜色
	int		m_nTextDirection;	// 文本方向
	float	m_fTextAngleSin;	// 字体中文本角度的正弦值
	float	m_fTextAngleCos;	// 字体中文本角度的余弦值
	// 特殊应用参数，当出现无法使用非DC手段进行绘制时，求助GDI。
	HDC		m_hMemDC;			// DC缓存
	HBITMAP	m_hMemBM;			// 缓存对象（DIB）
	int		m_nMemWidth;
	int		m_nMemHeight;
	DWORD	*m_pdwMemBM;
	HBITMAP m_hOldBM;			// 用以还原DC环境的对象变量
	// 堆栈记录宽线经过的连续的填充点
	POINT	*m_pPtStack;		// 栈
	int		m_nPtStackSize;		// 栈的大小
	int		m_nTop;				// 栈顶（与主轴的值相关）
};

// Inline Implementation.

//////////////////////////////////////////////////////////////////////////
// 通用属性访问接口

// 获取绑定对象的属性
inline DWORD* CCommonBufferDC::GetBuffer() const
{
	return m_pdwBuffer;
}
inline int CCommonBufferDC::GetBufferWidth() const
{
	return m_nWidth;
}
inline int CCommonBufferDC::GetBufferHeight() const
{
	return m_nHeight;
}
inline void CCommonBufferDC::GetBufferSize(OUT int &nWidth, OUT int &nHeight)
{
	nWidth = m_nWidth;
	nHeight = m_nHeight;
}

// 将屏幕设为某一种颜色
inline void CCommonBufferDC::ClearBuffer(IN DWORD dwColor)
{
	//SYBGRA32_FillRect(m_pdwBuffer, 0, 0, m_nWidth, m_nHeight, m_nWidth, m_nHeight, NULL, dwColor);
}
inline void CCommonBufferDC::ClearRect(IN int x, IN int y, IN int nWidth, IN int nHeight, IN DWORD dwColor)
{
	//SYBGRA32_FillRect(m_pdwBuffer, x, y, nWidth, nHeight, m_nWidth, m_nHeight, NULL, dwColor);
}

// 获取绘制的裁剪区域，裁剪区域为左上闭右下开。
inline void CCommonBufferDC::GetClipRect(OUT RECT& rcClip)
{
	rcClip = m_rcClip;
}
inline const RECT& CCommonBufferDC::GetClipRect() const
{
	return m_rcClip;
}

// 设置获取绘制后的有效图像区域，用于最后提取有效图像（左上闭右下开）。
inline void CCommonBufferDC::GetValidRect(OUT RECT &rcValid)
{
	rcValid = m_rcValid;
}
inline const RECT& CCommonBufferDC::GetValidRect() const
{
	return m_rcValid;
}

// 设置获取绘制坐标系的参考信息
inline POINT CCommonBufferDC::SetOffsetRef(IN int x, IN int y)
{
	POINT ptOld = m_ptOffsetRef;

	m_ptOffsetRef.x = (LONG)x;
	m_ptOffsetRef.y = (LONG)y;

	return ptOld;
}

inline POINT CCommonBufferDC::SetOffsetRef(IN float x, IN float y)
{
	POINT ptOld = m_ptOffsetRef;

	m_ptOffsetRef.x = (LONG)x;
	m_ptOffsetRef.y = (LONG)y;

	return ptOld;
}

inline POINT CCommonBufferDC::SetOffsetRef(IN POINT pt)
{
	POINT ptOld = m_ptOffsetRef;

	m_ptOffsetRef = pt;

	return ptOld;
}
inline POINT CCommonBufferDC::GetOffsetRef() const
{
	return m_ptOffsetRef;
}
inline float CCommonBufferDC::SetZoomRef(IN float fZoom)
{
	float fOld = m_fZoomRef;

	m_fZoomRef = fZoom;

	return fOld;
}
inline float CCommonBufferDC::GetZoomRef() const
{
	return m_fZoomRef;
}

// 设置获取画笔的属性（返回旧属性）
inline int CCommonBufferDC::GetPenStyle() const
{
	return m_nPenStyle;
}
inline int CCommonBufferDC::SetPenWidth(IN int nWidth)
{
	int nTmp = m_nPenWidth;

	m_nPenWidth = nWidth;

	return nTmp;
}
inline int CCommonBufferDC::GetPenWidth() const
{
	return m_nPenWidth;
}
inline DWORD CCommonBufferDC::SetPenColor(IN DWORD dwColor)
{
	DWORD dwTmp = m_dwPenColor;

	m_dwPenColor = dwColor;

	return dwTmp;
}
inline DWORD CCommonBufferDC::GetPenColor() const
{
	return m_dwPenColor;
}

// 设置获取背景属性（返回旧属性）
inline DWORD CCommonBufferDC::SetBkColor(IN DWORD dwColor)
{
	DWORD dwTmp = m_dwBkColor;

	m_dwBkColor = dwColor;

	return dwTmp;
}
inline DWORD CCommonBufferDC::GetBkColor() const
{
	return m_dwBkColor;
}
inline int CCommonBufferDC::SetBkMode(IN int nBkMode)
{
	int nTmp = m_nBkMode;

	m_nBkMode = nBkMode;

	return nTmp;
}
inline int CCommonBufferDC::GetBkMode() const
{
	return m_nBkMode;
}

// 设置获取填充属性（返回旧属性）（模式与背景相同）
inline DWORD CCommonBufferDC::SetFillColor(IN DWORD dwColor)
{
	DWORD dwTmp = m_dwFillColor;

	m_dwFillColor = dwColor;

	return dwTmp;
}
inline DWORD CCommonBufferDC::GetFillColor() const
{
	return m_dwFillColor;
}
inline int CCommonBufferDC::SetFillMode(IN int nFillMode)
{
	int nTmp = m_nFillMode;

	m_nFillMode = nFillMode;

	return nTmp;
}
inline int CCommonBufferDC::GetFillMode() const
{
	return m_nFillMode;
}

// 设置获取绘制模式（返回旧模式）
inline int CCommonBufferDC::SetROP2(IN int nDrawMode)
{
	//assert(nDrawMode > 0 && nDrawMode <= R2_LAST);

	int nTmp = m_nDrawMode;

	m_nDrawMode = nDrawMode;

	return nTmp;
}
inline int CCommonBufferDC::GetROP2() const
{
	return m_nDrawMode;
}

// 设置自定义线型（一个像素宽的直线）{ 直线段、虚线段、直线段、…… }
inline BOOL CCommonBufferDC::SetLinePattern(IN const int *pPattern,		// 线型数组
										IN int nPatCount)			// 数组大小
{
	if (pPattern == NULL || nPatCount <= 0)
		return FALSE;

	m_nPenStyle = PS_USERSTYLE;
	CopyLinePattern(pPattern, nPatCount);

	return TRUE;
}

// 文本绘制
inline HFONT CCommonBufferDC::GetFont() const
{
	return m_hFont;
}
inline DWORD CCommonBufferDC::SetTextColor(IN DWORD dwColor)		// 文本颜色
{
	DWORD dwOldColor = m_dwTextColor;

	m_dwTextColor = dwColor;

	return dwOldColor;
}
inline DWORD CCommonBufferDC::GetTextColor() const
{
	return m_dwTextColor;
}

//////////////////////////////////////////////////////////////////////////
// 客户坐标系下的绘制接口

// 贴图
inline BOOL CCommonBufferDC::BitBlt(IN int x,					// 目标位置
								IN int y,
								IN int nWidth,				// 目标区域大小
								IN int nHeight,
								IN DWORD* pSrc,				// 源
								IN int xSrc,				// 源位置
								IN int ySrc,
								IN int nSrcWidth,			// 源区域大小
								IN int nSrcHeight,
								IN int nSrcBufWidth,		// 源大小
								IN int nSrcBufHeight,
								IN BUFFER_MODE eMode,	// 混合操作类型
								IN BUFFER_PARITY_STYLE eParity)	// 奇偶场类型
{
	float nHalfW = nWidth * 0.5f, nHalfH = nHeight * 0.5f;
	float xc = (float)x + nHalfW, yc = (float)y + nHalfH;

	ClientToScreen(xc, yc);
	x = SY_FToI(xc - nHalfW);
	y = SY_FToI(yc - nHalfH);

	return _BitBlt(x, y, nWidth, nHeight, pSrc, xSrc, ySrc, nSrcWidth, nSrcHeight,	\
				nSrcBufWidth, nSrcBufHeight, eMode, eParity);
}

// 设置某一点（点在有效区域内时返回 TRUE）
inline BOOL CCommonBufferDC::SetPixel(IN float x,			// 点位置
								  IN float y,
								  IN DWORD dwColor)		// 颜色
{
	ClientToScreen(x, y);

	return _SetPixel(SY_FToI(x), SY_FToI(y), dwColor);
}
inline BOOL CCommonBufferDC::SetPixel(IN const pointf &pt,	// 点位置
								  IN DWORD dwColor)			// 颜色
{
	return SetPixel(pt.x, pt.y, dwColor);
}
inline BOOL CCommonBufferDC::SetPixel(IN int x,				// 点位置
                                  IN int y,
								  IN DWORD dwColor)		// 颜色
{
	ClientToScreen(x, y);

	return _SetPixel(x, y, dwColor);
}
inline BOOL CCommonBufferDC::SetPixel(IN const POINT &pt,	// 点位置
								  IN DWORD dwColor)		// 颜色
{
	return SetPixel(pt.x, pt.y, dwColor);
}

// 画线
inline void CCommonBufferDC::MoveTo(IN float x, IN float y)
{
	ClientToScreen(x, y);
	_MoveTo(SY_FToI(x), SY_FToI(y));
}
inline void CCommonBufferDC::MoveTo(IN const pointf &pt)
{
	MoveTo(pt.x, pt.y);
}
inline void CCommonBufferDC::MoveTo(IN int x, IN int y)
{
	ClientToScreen(x, y);
	_MoveTo(x, y);
}
inline void CCommonBufferDC::MoveTo(IN const POINT &pt)
{
	MoveTo((int)pt.x, (int)pt.y);
}
inline BOOL CCommonBufferDC::LineTo(IN float x, IN float y)
{
	ClientToScreen(x, y);

	return _LineTo(SY_FToI(x), SY_FToI(y));
}
inline BOOL CCommonBufferDC::LineTo(IN const pointf &pt)
{
	return LineTo(pt.x, pt.y);
}
inline BOOL CCommonBufferDC::LineTo(IN int x, IN int y)
{
	ClientToScreen(x, y);

	return _LineTo(x, y);
}
inline BOOL CCommonBufferDC::LineTo(IN const POINT &pt)
{
	return LineTo((int)pt.x, (int)pt.y);
}

// 画矩形
inline void CCommonBufferDC::DrawRect(IN float l, IN float t, IN float r, IN float b)
{
	ClientToScreen(l, t);
	ClientToScreen(r, b);
	_DrawRect(SY_FToI(l), SY_FToI(t), SY_FToI(r), SY_FToI(b));
}
inline void CCommonBufferDC::DrawRect(IN const pointf &ptLT, IN const pointf &ptRB)
{
	DrawRect(ptLT.x, ptLT.y, ptRB.x, ptRB.y);
}
inline void CCommonBufferDC::DrawRect(IN const rectf &rc)
{
	DrawRect(rc.left, rc.top, rc.right, rc.bottom);
}
inline void CCommonBufferDC::DrawRect(IN int l, IN int t, IN int r, IN int b)
{
	ClientToScreen(l, t);
	ClientToScreen(r, b);
	_DrawRect(l, t, r, b);
}
inline void CCommonBufferDC::DrawRect(IN const POINT &ptLT, IN const POINT &ptRB)
{
	DrawRect((int)ptLT.x, (int)ptLT.y, (int)ptRB.x, (int)ptRB.y);
}
inline void CCommonBufferDC::DrawRect(IN const RECT &rc)
{
	DrawRect((int)rc.left, (int)rc.top, (int)rc.right, (int)rc.bottom);
}

// 填充矩形，此处填充色与内部填充色变量m_dwFillColor无关
inline void CCommonBufferDC::FillRect(IN float l, IN float t, IN float r, IN float b, IN DWORD dwColor)
{
	ClientToScreen(l, t);
	ClientToScreen(r, b);
	_FillRect(SY_FToI(l), SY_FToI(t), SY_FToI(r), SY_FToI(b), dwColor);
}
inline void CCommonBufferDC::FillRect(IN const pointf &ptLT, IN const pointf &ptRB, IN DWORD dwColor)
{
	FillRect(ptLT.x, ptLT.y, ptRB.x, ptRB.y, dwColor);
}
inline void CCommonBufferDC::FillRect(IN const rectf& rc, IN DWORD dwColor)
{
	FillRect(rc.left, rc.top, rc.right, rc.bottom, dwColor);
}
inline void CCommonBufferDC::FillRect(IN int l, IN int t, IN int r, IN int b, IN DWORD dwColor)
{
	ClientToScreen(l, t);
	ClientToScreen(r, b);
	_FillRect(l, t, r, b, dwColor);
}
inline void CCommonBufferDC::FillRect(IN const POINT &ptLT, IN const POINT &ptRB, IN DWORD dwColor)
{
	FillRect((int)ptLT.x, (int)ptLT.y, (int)ptRB.x, (int)ptRB.y, dwColor);
}
inline void CCommonBufferDC::FillRect(IN const RECT& rc, IN DWORD dwColor)
{
	FillRect((int)rc.left, (int)rc.top, (int)rc.right, (int)rc.bottom, dwColor);
}

// 画圆
inline void CCommonBufferDC::DrawCircle(IN float xc,					// 圆心
									IN float yc,
									IN int r,						// 半径
									IN BOOL bOddDia/* = TRUE*/)		// 直径为奇数（即 d = 2r + 1）
{
	int d = SY_FToI(DoZoomRef((float)((r << 1) + (bOddDia ? 1 : 0))));

	ClientToScreen(xc, yc);
	_DrawCircle(SY_FToI(xc), SY_FToI(yc), (d >> 1), ((d & 1) != 0));
}
inline void CCommonBufferDC::DrawCircle(IN const pointf &ptC,			// 圆心
									IN int r,						// 半径
									IN BOOL bOddDia/* = TRUE*/)		// 直径为奇数（即 d = 2r + 1）
{
	DrawCircle(ptC.x, ptC.y, r, bOddDia);
}
inline void CCommonBufferDC::DrawCircle(IN int xc,						// 圆心
									IN int yc,
									IN int r,						// 半径
									IN BOOL bOddDia/* = TRUE*/)		// 直径为奇数（即 d = 2r + 1）
{
	int d = SY_FToI(DoZoomRef((float)((r << 1) + (bOddDia ? 1 : 0))));

	ClientToScreen(xc, yc);
	_DrawCircle(xc, yc, (d >> 1), ((d & 1) != 0));
}
inline void CCommonBufferDC::DrawCircle(IN const POINT &ptC,		// 圆心
									IN int r,					// 半径
									IN BOOL bOddDia/* = TRUE*/)	// 直径为奇数（即 d = 2r + 1）
{
	DrawCircle((int)ptC.x, (int)ptC.y, r, bOddDia);
}

inline void CCommonBufferDC::DrawEllipse(IN float xc,					// 圆心
									IN float yc,
									IN int a,										// 长轴
									IN int b,										// 短轴
									IN BOOL bOdda/* = TRUE*/,			// 长轴为奇数
									IN BOOL bOddb/* = TRUE*/)			// 短轴为奇数
{
	int d_a = SY_FToI(DoZoomRef((float)((a << 1) + (bOdda ? 1 : 0))));
	int d_b = SY_FToI(DoZoomRef((float)((b << 1) + (bOddb ? 1 : 0))));

	ClientToScreen(xc, yc);
	_DrawEllipse(SY_FToI(xc), SY_FToI(yc), (d_a >> 1), (d_b >> 1), ((d_a & 1) != 0), ((d_b & 1) != 0));
}
inline void CCommonBufferDC::DrawEllipse(IN const pointf &ptC,		// 圆心
									IN int a,										// 长轴
									IN int b,										// 短轴
									IN BOOL bOdda/* = TRUE*/,			// 长轴为奇数
									IN BOOL bOddb/* = TRUE*/)			// 短轴为奇数
{
	DrawEllipse(ptC.x, ptC.y, a, b, bOdda, bOddb);
}
inline void CCommonBufferDC::DrawEllipse(IN int xc,						// 圆心
									IN int yc,
									IN int a,										// 长轴
									IN int b,										// 短轴
									IN BOOL bOdda/* = TRUE*/,			// 长轴为奇数
									IN BOOL bOddb/* = TRUE*/)			// 短轴为奇数
{
	int d_a = SY_FToI(DoZoomRef((float)((a << 1) + (bOdda ? 1 : 0))));
	int d_b = SY_FToI(DoZoomRef((float)((b << 1) + (bOddb ? 1 : 0))));

	ClientToScreen(xc, yc);
	_DrawEllipse(xc, yc, (d_a >> 1), (d_b >> 1), ((d_a & 1) != 0), ((d_b & 1) != 0));
}
inline void CCommonBufferDC::DrawEllipse(IN const POINT &ptC,		// 圆心
									IN int a,										// 长轴
									IN int b,										// 短轴
									IN BOOL bOdda/* = TRUE*/,			// 长轴为奇数
									IN BOOL bOddb/* = TRUE*/)			// 短轴为奇数
{
	DrawEllipse((int)ptC.x, (int)ptC.y, a, b, bOdda, bOddb);
}

inline void CCommonBufferDC::DrawArc(IN int nCenterX, 
								 IN int nCenterY, 
								 IN pointf ptStart, 
								 IN pointf ptEnd)
{
	ClientToScreen(nCenterX, nCenterY);
	int ptBegin_x, ptBegin_y;
	int ptEnd_x, ptEnd_y;
	ptBegin_x = (int)ptStart.x;
	ptBegin_y = (int)ptStart.y;
	ptEnd_x = (int)ptEnd.x;
	ptEnd_y = (int)ptEnd.y;

	ClientToScreen( ptBegin_x, ptBegin_y );
	ClientToScreen( ptEnd_x, ptEnd_y );

	POINT ptBeginTemp, ptEndTemp;
	ptBeginTemp.x = (LONG)ptBegin_x;
	ptBeginTemp.y = (LONG)ptBegin_y;
	ptEndTemp.x = (LONG)ptEnd_x;
	ptEndTemp.y = (LONG)ptEnd_y;

	// 半径也需要按比例缩放
	double n_2 = (ptBegin_x - nCenterX)*(ptBegin_x - nCenterX) + (ptBegin_y - nCenterY)*(ptBegin_y - nCenterY);
	int nRadius = (int)(sqrt(n_2)+0.5);

	if (!m_bAttached)
		return;

	_DrawArc(nCenterX, nCenterY, nRadius, FALSE, ptBeginTemp, ptEndTemp, m_pPattern, m_nPatCount, m_dwPenColor, m_nBkMode, m_dwBkColor, m_nFillMode, m_dwFillColor);
}

//画扇形
// 按角度来进行绘制
inline void CCommonBufferDC::DrawSector(int nCenterX, int nCenterY, int nRadius, float fBeginAngle, float fEndAngle)
{
	ClientToScreen(nCenterX, nCenterY);

	if (!m_bAttached)
		return;
	_DrawSector(nCenterX, nCenterY, nRadius, FALSE,fBeginAngle, fEndAngle, m_pPattern, m_nPatCount, m_dwPenColor, m_nBkMode, m_dwBkColor, m_nFillMode, m_dwFillColor);
}

// 按起始终止点来绘制
inline void CCommonBufferDC::DrawSector(int nCenterX, int nCenterY, int nRadius, POINT ptBegin, POINT ptEnd)
{
	ClientToScreen(nCenterX, nCenterY);
	int ptBegin_x, ptBegin_y;
	int ptEnd_x, ptEnd_y;
	ptBegin_x = (int)ptBegin.x;
	ptBegin_y = (int)ptBegin.y;
	ptEnd_x = (int)ptEnd.x;
	ptEnd_y = (int)ptEnd.y;

	ClientToScreen( ptBegin_x, ptBegin_y );
	ClientToScreen( ptEnd_x, ptEnd_y );

	ptBegin.x = (LONG)ptBegin_x;
	ptBegin.y = (LONG)ptBegin_y;
	ptEnd.x = (LONG)ptEnd_x;
	ptEnd.y = (LONG)ptEnd_y;

	// 半径也需要按比例缩放
	double n_2 = (ptBegin_x - nCenterX)*(ptBegin_x - nCenterX) + (ptBegin_y - nCenterY)*(ptBegin_y - nCenterY);
	nRadius = (int)(sqrt(n_2)+0.5);

	if (!m_bAttached)
		return;

	_DrawSector(nCenterX, nCenterY, nRadius, FALSE,ptBegin, ptEnd, m_pPattern, m_nPatCount, m_dwPenColor, m_nBkMode, m_dwBkColor, m_nFillMode, m_dwFillColor);
}

// 画边框
inline void CCommonBufferDC::DrawBorder(IN float l,						// 目标区域
									IN float t,
									IN float r,
									IN float b,
									IN DWORD dwShadowColor,			// 暗色
									IN DWORD dwLightColor,			// 亮色
									IN SYBORDER_STYLE eStyle,		// 风格
									IN int nBorderWidth/* = 1*/)	// 边框宽度
{
	ClientToScreen(l, t);
	ClientToScreen(r, b);
	_DrawBorder(SY_FToI(l), SY_FToI(t), SY_FToI(r), SY_FToI(b), dwShadowColor, dwLightColor, eStyle, nBorderWidth);
}
inline void CCommonBufferDC::DrawBorder(IN const rectf &rcDest,		// 目标区域
									IN DWORD dwShadowColor,			// 暗色
									IN DWORD dwLightColor,			// 亮色
									IN SYBORDER_STYLE eStyle,		// 风格
									IN int nBorderWidth/* = 1*/)	// 边框宽度
{
	DrawBorder(rcDest.left, rcDest.top, rcDest.top, rcDest.bottom, dwShadowColor, dwLightColor, eStyle, nBorderWidth);
}
inline void CCommonBufferDC::DrawBorder(IN int l,					// 目标区域
									IN int t,
									IN int r,
									IN int b,
									IN DWORD dwShadowColor,			// 暗色
									IN DWORD dwLightColor,			// 亮色
									IN SYBORDER_STYLE eStyle,		// 风格
									IN int nBorderWidth/* = 1*/)	// 边框宽度
{
	ClientToScreen(l, t);
	ClientToScreen(r, b);
	_DrawBorder(l, t, r, b, dwShadowColor, dwLightColor, eStyle, nBorderWidth);
}
inline void CCommonBufferDC::DrawBorder(IN const RECT &rcDest,			// 目标区域
									IN DWORD dwShadowColor,			// 暗色
									IN DWORD dwLightColor,			// 亮色
									IN SYBORDER_STYLE eStyle,		// 风格
									IN int nBorderWidth/* = 1*/)	// 边框宽度
{
	DrawBorder((int)rcDest.left, (int)rcDest.top, (int)rcDest.right, (int)rcDest.bottom,	\
		dwShadowColor, dwLightColor, eStyle, nBorderWidth);
}

// 如果成功则返回文本高度，否则返回0。
inline int CCommonBufferDC::DrawText(IN LPCTSTR lpszString,		// 文本
								 IN int nCount,
								 IN OUT LPRECT lpRect,		// 目标区域
								 IN UINT uFormat,			// 格式（兼容GDI）
								 IN BOOL bBKTrans)	// 是否在透明背景上绘制文字
{
	if ((uFormat & DT_CALCRECT) != 0)		// 如果调用者指定计算文本的区域大小
		return _DrawText(lpszString, nCount, lpRect, uFormat,bBKTrans);

	int nRet(0);

	if (lpRect != NULL)
	{
		int x = lpRect->left, y = lpRect->top;
		RECT rcTmp;

		ClientToScreen(x, y);
		rcTmp.left = x;
		rcTmp.right = rcTmp.left + lpRect->right - lpRect->left;
		rcTmp.top = y;
		rcTmp.bottom = rcTmp.top + lpRect->bottom - lpRect->top;
		nRet = _DrawText(lpszString, nCount, &rcTmp, uFormat,bBKTrans);
	}
	return nRet;
}

//////////////////////////////////////////////////////////////////////////
// 屏幕坐标系下的绘制接口（以下划线开头）


// 判断点或矩形是否可见
inline BOOL CCommonBufferDC::_PixelVisible(IN POINT pt)			// 像素位置
{
	return ((pt.x >= m_rcClip.left) && (pt.x < m_rcClip.right)
			&& (pt.y >= m_rcClip.top) && (pt.y < m_rcClip.bottom));
}
inline BOOL CCommonBufferDC::_PixelVisible(IN int x, IN int y)	// 像素位置
{
	return ((x >= m_rcClip.left) && (x < m_rcClip.right)
			&& (y >= m_rcClip.top) && (y < m_rcClip.bottom));
}
inline BOOL CCommonBufferDC::_RectVisible(IN const RECT& rc)		// 矩形区域
{
	return !((rc.left >= m_rcClip.right) || (rc.right <= m_rcClip.left)
			|| (rc.top >= m_rcClip.bottom) || (rc.bottom <= m_rcClip.top));
}

// 贴图
inline BOOL CCommonBufferDC::_BitBlt(IN int x,						// 目标位置
								 IN int y,
								 IN int nWidth,				// 目标区域大小
								 IN int nHeight,
								 IN DWORD* pSrc,				// 源
								 IN int xSrc,					// 源位置
								 IN int ySrc,
								 IN int nSrcWidth,				// 源区域大小
								 IN int nSrcHeight,
								 IN int nSrcBufWidth,			// 源大小
								 IN int nSrcBufHeight,
								 IN BUFFER_MODE eMode,		// 混合操作类型
								 IN BUFFER_PARITY_STYLE eParity)	// 奇偶场类型
{
	if (!m_bAttached)
		return FALSE;

	return		SY_BitBlt(m_pdwBuffer, x, y, nWidth, nHeight, m_nWidth, m_nHeight,	\
				pSrc, xSrc, ySrc, nSrcWidth, nSrcHeight, nSrcBufWidth, nSrcBufHeight,	\
				eMode, eParity, &m_rcClip);
}


// 画线
inline void CCommonBufferDC::_MoveTo(IN int x, IN int y)
{
	m_ptStart.x = (LONG)x;
	m_ptStart.y = (LONG)y;
}

// 填充矩形，此处填充色与内部填充色变量m_dwFillColor无关
inline void CCommonBufferDC::_FillRect(IN int l, IN int t, IN int r, IN int b, IN DWORD dwColor)
{
	SYBGRA32_FillRect(m_pdwBuffer, l, t, (r - l), (b - t), m_nWidth, m_nHeight, &m_rcClip, dwColor);
}

// 画圆（由于采用对称算法，只会画出偶数直径的圆）
inline void CCommonBufferDC::_DrawCircle(IN int xc,			// 圆心
									 IN int yc,
									 IN int r,			// 半径
									 IN BOOL bOddDia)	// 直径为奇数（即 d = 2r + 1）
{
	if (!m_bAttached)
		return;

	BreCircle(xc, yc, r, bOddDia, m_pPattern, m_nPatCount, m_dwPenColor, m_nBkMode, m_dwBkColor, m_nFillMode, m_dwFillColor);
}

// 画椭圆（由于采用四象限对称算法，只会画出偶数长短轴的椭圆）
inline void CCommonBufferDC::_DrawEllipse(IN int xc,			// 圆心
									 IN int yc,
									 IN int a,							// 长径
									 IN int b,							// 短轴
									 IN BOOL bOdda,				// 长轴为奇数
									 IN BOOL bOddb)				// 短轴为奇数
{
	if (!m_bAttached)
		return;

	BreEllipse(xc, yc, a, b, bOdda, bOddb, m_pPattern, m_nPatCount, m_dwPenColor, m_nBkMode, m_dwBkColor, m_nFillMode, m_dwFillColor);
}
//////////////////////////////////////////////////////////////////////////
// Protected

// 裁减坐标的分量
inline int CCommonBufferDC::ClipPixelX(IN int x)
{
	return (x < m_rcClip.left) ? m_rcClip.left : ((x > m_rcClip.right) ? m_rcClip.right : x);
}
inline int CCommonBufferDC::ClipPixelY(IN int y)
{
	return (y < m_rcClip.top) ? m_rcClip.top : ((y > m_rcClip.bottom) ? m_rcClip.bottom : y);
}

// 判断点是否在裁减矩形内
inline BOOL CCommonBufferDC::PixelInClipRect(IN POINT pt)			// 像素位置
{
	return ((pt.x >= m_rcClip.left) && (pt.x < m_rcClip.right)
			&& (pt.y >= m_rcClip.top) && (pt.y < m_rcClip.bottom));
}
inline BOOL CCommonBufferDC::PixelInClipRect(IN int x, IN int y)	// 像素位置
{
	return ((x >= m_rcClip.left) && (x < m_rcClip.right)
			&& (y >= m_rcClip.top) && (y < m_rcClip.bottom));
}

// 往栈内推点，如果栈满则返回 TRUE，默认栈存在且是安全的。
inline BOOL CCommonBufferDC::PushPtInStack(IN int x, IN int y)
{
	// 先不画线而将点推入栈，如果栈满则画线
	m_pPtStack[m_nTop].x = x;
	m_pPtStack[m_nTop].y = y;

	return ((++m_nTop) >= m_nPtStackSize);
}

// 坐标系转换（先缩放后平移）
inline void CCommonBufferDC::ClientToScreen(IN OUT float &x, IN OUT float &y)
{
	x = DoZoomRef(x) + (float)m_ptOffsetRef.x;
	y = DoZoomRef(y) + (float)m_ptOffsetRef.y;
}
inline void CCommonBufferDC::ClientToScreen(IN OUT pointf &pt)
{
	ClientToScreen(pt.x, pt.y);
}
inline void CCommonBufferDC::ClientToScreen(IN OUT rectf &rc)
{
	// 将矩形视为左上点与右下点的组合
	ClientToScreen(rc.left, rc.top);
	ClientToScreen(rc.right, rc.bottom);
}
inline void CCommonBufferDC::ClientToScreen(IN OUT int &x, IN OUT int &y)
{
	x = SY_FToI(DoZoomRef((float)x) + (float)m_ptOffsetRef.x);
	y = SY_FToI(DoZoomRef((float)y) + (float)m_ptOffsetRef.y);
}
inline void CCommonBufferDC::ClientToScreen(IN OUT POINT &pt)
{
	int x = (int)pt.x, y = (int)pt.y;

	ClientToScreen(x, y);
	pt.x = (LONG)x;
	pt.y = (LONG)y;
}
inline void CCommonBufferDC::ClientToScreen(IN OUT RECT &rc)
{
	int x = (int)rc.left, y = (int)rc.top;

	// 将矩形视为左上点与右下点的组合
	ClientToScreen(x, y);
	rc.left = (LONG)x;
	rc.top = (LONG)y;
	x = (int)rc.right;
	y = (int)rc.bottom;
	ClientToScreen(x, y);
	rc.right = (LONG)x;
	rc.bottom = (LONG)y;
}
inline float CCommonBufferDC::DoZoomRef(IN float x)
{
	return (SY_IsEqual_F(m_fZoomRef, 1.f) ? x : (x * m_fZoomRef));
}

//////////////////////////////////////////////////////////////////////////
// 文件结束