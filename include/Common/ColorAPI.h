//////////////////////////////////////////////////////////////////////////
// SYColorAPI.cpp - APIs for complex color processing.
//////////////////////////////////////////////////////////////////////////
#ifndef __SY_COLOR_API_INC__
#define __SY_COLOR_API_INC__

#include "Common\ColorDef.h"
#include "Common\VectorDef.h"
#include "Common\2DBufferAPI.h"
//////////////////////////////////////////////////////////////////////////
// constant definitions

typedef int SYCOLOR_RESULT;
// define the return values.
enum SYCOLOR_RET_VALUES
{
	SYCOLOR_RET_SUCCESS = 1,		// success
	SYCOLOR_RET_FAILED = 0,			// failed
	SYCOLOR_RET_INITED = -1,		// already initiated
	SYCOLOR_RET_NOT_INIT = -2,		// not initiate
	SYCOLOR_RET_INVALIDPARAM = -3,	// invalid parameters
	SYCOLOR_RET_FILEEXISTS = -4,	// file exists
};

// 防止越界的宏
#define IN_RANGE(x, start, end)		if((x)<(start))(x)=(start);	\
									else if((x)>(end))(x)=(end)
//////////////////////////////////////////////////////////////////////////
// General APIs for color processing.
// 初始化颜色路径
BASEGRAPH_COMMON_API SYCOLOR_RESULT SYColor_InitEnv(LPCTSTR szBinPath);

BASEGRAPH_COMMON_API SYCOLOR_RESULT SYColor_ReleaseEnv();
// 拷贝颜色
BASEGRAPH_COMMON_API void SYColor_CopyColor(OUT SYColor* pDes,	// 目标
								   IN SYColor* pSrc);	// 源

BASEGRAPH_COMMON_API DWORD SYColor_GetRGBColor(IN SYColor* pColor);

// 填充颜色
BASEGRAPH_COMMON_API void SYColor_FillColor(IN SYColor* pColor,		// 颜色
								   OUT DWORD* pdwBuf,		// 填充对象
								   IN int nBufWidth,		// 对象大小
								   IN int nBufHeight,
								   IN int nX,				// 填充区域
								   IN int nY,
								   IN int nWidth,
								   IN int nHeight,
								   IN float fScale,
								   IN int nFrame = 0,		// 颜色动画的帧号，于静态颜色无用
								   IN BYTE* pbyMask = NULL,	// 掩膜（8位缓存），大小与填充区域相同
								   IN int nMaskPitch = 0);	// 掩膜缓存的特殊性，pitch可能不等于宽度

BASEGRAPH_COMMON_API BOOL SYColor_CalcAniParam(OUT SYColor *pNewColor,	// 根据动画参数计算得到的静态颜色参数
									  IN SYColor* pSrcColor,	// 动画颜色参数
									  IN int nFrame);			// 动画帧号
//////////////////////////////////////////////////////////////////////////
// 单色

// 创建
BASEGRAPH_COMMON_API void SYColor_CreateHomo(OUT SYColor* pColor,		// 待创建颜色
									IN unsigned long color);	// 单色
// 填充
void SYColor_FillHomo(IN SYColor* pColor,		// 填充颜色
					  OUT DWORD* pdwBuf,		// 填充对象
					  IN int nBufWidth,			// 对象大小
					  IN int nBufHeight,
					  IN int nX,				// 填充区域
					  IN int nY,
					  IN int nWidth,
					  IN int nHeight,
					  IN int nFrame = 0,		// 颜色动画的帧号，于静态颜色无用
					  IN BYTE* pbyMask = NULL,	// 掩膜（8位缓存），大小与填充区域相同
					  IN int nMaskPitch = 0);	// 掩膜缓存的特殊性，pitch可能不等于宽度

//////////////////////////////////////////////////////////////////////////
// 填充
void SYColor_FillGradLinear(IN SYColor* pColor,		// 填充颜色
							OUT DWORD* pdwBuf,		// 填充对象
							IN int nBufWidth,		// 对象大小
							IN int nBufHeight,
							IN int nX,				// 填充区域
							IN int nY,
							IN int nWidth,
							IN int nHeight,
							IN int nFrame = 0,		// 颜色动画的帧号，于静态颜色无用
							IN BYTE* pbyMask = NULL,// 掩膜（8位缓存），大小与填充区域相同
							IN int nMaskPitch = 0);	// 掩膜缓存的特殊性，pitch可能不等于宽度
// 采用浮点颜色递归运算进行行填充
// （1）在中间点左边时（x < m）：0.5*(c1-c0)/(m-p0) * (x-p0) + c0 = A1 * (x - B1) + C1
// （2）在中间点右边时（y > m）：0.5*(c1-c0)/(p1-m) * (y-p1) + c1 = A2 * (y - B2) + C2
void SYColor_FillLineBaseAR_1_Recursive(OUT DWORD* pdwBuf,		// 填充对象
									IN int nBufWidth,
									IN DWORD *pdwColor,		// 单色组
									IN float *pfPos,		// 位置组
									IN float *pfMid,		// 中间点组
									IN int nCount,			// 节点个数
									IN DWORD dwRepeat,		// 重复频率
									IN DWORD dwStyle);		// 重复方式
// 填充
// 圆形中心渐变色
void SYColor_FillGradCenterRound(IN SYColor* pColor,		// 填充颜色
								 OUT DWORD* pdwBuf,			// 填充对象
								 IN int nBufWidth,			// 对象大小
								 IN int nBufHeight,
								 IN int nX,					// 填充区域
								 IN int nY,
								 IN int nWidth,
								 IN int nHeight,
								 IN int nFrame = 0,			// 颜色动画的帧号，于静态颜色无用
								 IN BYTE* pbyMask = NULL,	// 掩膜（8位缓存），大小与填充区域相同
								 IN int nMaskPitch = 0);	// 掩膜缓存的特殊性，pitch可能不等于宽度
// 凸多边形中心渐变色
void SYColor_FillGradCenterPolygon(IN SYColor* pColor,		// 填充颜色
								   OUT DWORD* pdwBuf,		// 填充对象
								   IN int nBufWidth,		// 对象大小
								   IN int nBufHeight,
								   IN int nX,				// 填充区域
								   IN int nY,
								   IN int nWidth,
								   IN int nHeight,
								   IN int nFrame = 0,		// 颜色动画的帧号，于静态颜色无用
								   IN BYTE* pbyMask = NULL,	// 掩膜（8位缓存），大小与填充区域相同
								   IN int nMaskPitch = 0);	// 掩膜缓存的特殊性，pitch可能不等于宽度
// 填充
// 圆周轮廓渐变色
void SYColor_FillGradOutlineCircle(IN SYColor* pColor,		// 填充颜色
								   OUT DWORD* pdwBuf,		// 填充对象
								   IN int nBufWidth,		// 对象大小
								   IN int nBufHeight,
								   IN int nX,				// 填充区域
								   IN int nY,
								   IN int nWidth,
								   IN int nHeight,
								   IN int nFrame = 0,		// 颜色动画的帧号，于静态颜色无用
								   IN BYTE* pbyMask = NULL,	// 掩膜（8位缓存），大小与填充区域相同
								   IN int nMaskPitch = 0);	// 掩膜缓存的特殊性，pitch可能不等于宽度
// 多边形轮廓渐变色
void SYColor_FillGradOutlinePolygon(IN SYColor* pColor,		// 填充颜色
									OUT DWORD* pdwBuf,		// 填充对象
									IN int nBufWidth,		// 对象大小
									IN int nBufHeight,
									IN int nX,				// 填充区域
									IN int nY,
									IN int nWidth,
									IN int nHeight,
									IN int nFrame = 0,		// 颜色动画的帧号，于静态颜色无用
									IN BYTE* pbyMask = NULL,// 掩膜（8位缓存），大小与填充区域相同
									IN int nMaskPitch = 0);	// 掩膜缓存的特殊性，pitch可能不等于宽度
// 填充
// 多点等高渐变色
// 令 sn = [(x - xn)^2 + (y - yn)^2]^1/2 表示目标点到第 n 个顶点的距离
// 以其倒数为权重，目标颜色为 c_des = (c1/s1 + c2/s2 + ……) / (1/s1 + 1/s2 + ……)
void SYColor_FillGradPtContour(IN SYColor* pColor,		// 填充颜色
							   OUT DWORD* pdwBuf,		// 填充对象
							   IN int nBufWidth,		// 对象大小
							   IN int nBufHeight,
							   IN int nX,				// 填充区域
							   IN int nY,
							   IN int nWidth,
							   IN int nHeight,
							   IN int nFrame = 0,		// 颜色动画的帧号，于静态颜色无用
							   IN BYTE* pbyMask = NULL,	// 掩膜（8位缓存），大小与填充区域相同
							   IN int nMaskPitch = 0);	// 掩膜缓存的特殊性，pitch可能不等于宽度
// 多点混成渐变色
// 令 sn = [(x - xn)^2 + (y - yn)^2]^1/2 表示目标点到第 n 个顶点的距离
// 以其为权重，目标颜色为 c_des = (c1 * s1 + c2 * s2 + ……) / (s1 + s2 + ……)
void SYColor_FillGradPtBlend(IN SYColor* pColor,		// 填充颜色
							 OUT DWORD* pdwBuf,			// 填充对象
							 IN int nBufWidth,			// 对象大小
							 IN int nBufHeight,
							 IN int nX,					// 填充区域
							 IN int nY,
							 IN int nWidth,
							 IN int nHeight,
							 IN int nFrame = 0,			// 颜色动画的帧号，于静态颜色无用
							 IN BYTE* pbyMask = NULL,	// 掩膜（8位缓存），大小与填充区域相同
							 IN int nMaskPitch = 0);	// 掩膜缓存的特殊性，pitch可能不等于宽度
// 填充
void SYColor_FillGradTemplet(IN SYColor* pColor,		// 填充颜色
							 OUT DWORD* pdwBuf,			// 填充对象
							 IN int nBufWidth,			// 对象大小
							 IN int nBufHeight,
							 IN int nX,					// 填充区域
							 IN int nY,
							 IN int nWidth,
							 IN int nHeight,
							 IN int nFrame = 0,			// 颜色动画的帧号，于静态颜色无用
							 IN BYTE* pbyMask = NULL,	// 掩膜（8位缓存），大小与填充区域相同
							 IN int nMaskPitch = 0);	// 掩膜缓存的特殊性，pitch可能不等于宽度
// 添加模板
BASEGRAPH_COMMON_API SYCOLOR_RESULT SYColor_AddTemplet(IN LPCTSTR szFilePath,		// 模板原始路径
											  IN int nIconWidth,
											  IN int nIconHeight,
											  OUT int *pnIndex);
BASEGRAPH_COMMON_API SYCOLOR_RESULT SYColor_DelTemplet(IN int nIndex);
// 获取模板数据
BASEGRAPH_COMMON_API DWORD* SYColor_GetTempletIconData(IN int nIndex);
BASEGRAPH_COMMON_API int SYColor_GetTempletIconWidth(IN int nIndex);
BASEGRAPH_COMMON_API int SYColor_GetTempletIconHeight(IN int nIndex);
BASEGRAPH_COMMON_API SYCOLOR_RESULT SYColor_GetTempletIconSize(OUT int *pnWidth,OUT int *pnHeight,IN int nIndex);
BASEGRAPH_COMMON_API LPCTSTR SYColor_GetTempletName(IN int nIndex);
BASEGRAPH_COMMON_API int SYColor_GetTempletCount();

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
						IN int nFrame = 0,			// 颜色动画的帧号，于静态颜色无用
						IN BYTE* pbyMask = NULL,	// 掩膜（8位缓存），大小与填充区域相同
						IN int nMaskPitch = 0);		// 掩膜缓存的特殊性，pitch可能不等于宽度
 //添加删除纹理
BASEGRAPH_COMMON_API SYCOLOR_RESULT SYColor_AddTexImg(IN LPCTSTR szFilePath,		// 图片原始路径
											 IN int nIconWidth,
											 IN int nIconHeight,
											 OUT int *pnIndex);
BASEGRAPH_COMMON_API SYCOLOR_RESULT SYColor_DelTexImg(IN int nIndex);
// 获取纹理数据
BASEGRAPH_COMMON_API DWORD* SYColor_GetTexImgIconData(IN int nIndex);
BASEGRAPH_COMMON_API int SYColor_GetTexImgIconWidth(IN int nIndex);
BASEGRAPH_COMMON_API int SYColor_GetTexImgIconHeight(IN int nIndex);
BASEGRAPH_COMMON_API SYCOLOR_RESULT SYColor_GetTexImgIconSize(OUT int *pnWidth,
													 OUT int *pnHeight,
													 IN int nIndex);
BASEGRAPH_COMMON_API LPCTSTR SYColor_GetTexImgName(IN int nIndex);
BASEGRAPH_COMMON_API int SYColor_GetTexImgCount();

BASEGRAPH_COMMON_API void DWORDColorToFloat4(float4* pOutColor,DWORD colorIn);	
BASEGRAPH_COMMON_API void Float4ToDWORDColor(DWORD* pColorOut , const float4& ColorIn);

BASEGRAPH_COMMON_API int SYColor_SaveColor(IN SYColor* pColor,void* pBuffer,int nCount);
BASEGRAPH_COMMON_API int SYColor_LoadColor(IN SYColor* pColor,void* pBuffer,int nCount);

// 创建缺省色
void SYColor_CreateLinearDefault(OUT SYColor* pColor);
void SYColor_CreateCenterRoundDefault(OUT SYColor* pColor)	;
void SYColor_CreateCenterPolygonDefault(OUT SYColor* pColor);
void SYColor_CreateOutlineCircleDefault(OUT SYColor* pColor);
void SYColor_CreateOutlinePolygonDefault(OUT SYColor* pColor);
void SYColor_CreateMultiPtDefault(OUT SYColor* pColor,	IN DWORD dwType);
void SYColor_CreateTempletDefault(OUT SYColor* pColor);
void SYColor_CreateTexImgDefault(OUT SYColor* pColor);
BASEGRAPH_COMMON_API void SYColor_CreateDefault(OUT SYColor* pColor,IN DWORD dwType);		// 颜色种类

typedef struct   
{    
	BYTE   red;              // [0,255]    
	BYTE   green;            // [0,255]    
	BYTE   blue;             // [0,255]    
}COLOR_RGB;    

typedef struct   
{    
	float hue;              // [0,360]    
	float saturation;       // [0,100]    
	float luminance;        // [0,100]    
}COLOR_HSL;   
#define min3v(v1, v2, v3)    ((v1)>(v2)? ((v2)>(v3)?(v3):(v2)):((v1)>(v3)?(v3):(v2))) 
#define max3v(v1, v2, v3)    ((v1)<(v2)? ((v2)<(v3)?(v3):(v2)):((v1)<(v3)?(v3):(v1))) 
#endif
//////////////////////////////////////////////////////////////////////////
// End of file.