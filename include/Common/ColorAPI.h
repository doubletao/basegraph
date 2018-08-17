//////////////////////////////////////////////////////////////////////////
// BaseGraphColorAPI.cpp - APIs for complex color processing.
//////////////////////////////////////////////////////////////////////////
#ifndef __Common_COLOR_API_INC__
#define __Common_COLOR_API_INC__

#include "Common\ColorDef.h"
#include "Common\VectorDef.h"
#include "Common\2DBufferAPI.h"
//////////////////////////////////////////////////////////////////////////
// constant definitions

typedef int BaseGraphCOLOR_RESULT;
// define the return values.
enum BaseGraphCOLOR_RET_VALUES
{
	BaseGraphCOLOR_RET_SUCCESS = 1,		// success
	BaseGraphCOLOR_RET_FAILED = 0,			// failed
	BaseGraphCOLOR_RET_INITED = -1,		// already initiated
	BaseGraphCOLOR_RET_NOT_INIT = -2,		// not initiate
	BaseGraphCOLOR_RET_INVALIDPARAM = -3,	// invalid parameters
	BaseGraphCOLOR_RET_FILEEXISTS = -4,	// file exists
};

// 防止越界的宏
#define IN_RANGE(x, start, end)		if((x)<(start))(x)=(start);	\
									else if((x)>(end))(x)=(end)
//////////////////////////////////////////////////////////////////////////
// General APIs for color processing.
// 初始化颜色路径
BASEGRAPH_COMMON_API BaseGraphCOLOR_RESULT BaseGraphColor_InitEnv(LPCTSTR szBinPath);

BASEGRAPH_COMMON_API BaseGraphCOLOR_RESULT BaseGraphColor_ReleaseEnv();
// 拷贝颜色
BASEGRAPH_COMMON_API void BaseGraphColor_CopyColor(OUT BaseGraphColor* pDes,	// 目标
								   IN BaseGraphColor* pSrc);	// 源

BASEGRAPH_COMMON_API DWORD BaseGraphColor_GetRGBColor(IN BaseGraphColor* pColor);

// 填充颜色
BASEGRAPH_COMMON_API void BaseGraphColor_FillColor(IN BaseGraphColor* pColor,		// 颜色
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

BASEGRAPH_COMMON_API BOOL BaseGraphColor_CalcAniParam(OUT BaseGraphColor *pNewColor,	// 根据动画参数计算得到的静态颜色参数
									  IN BaseGraphColor* pSrcColor,	// 动画颜色参数
									  IN int nFrame);			// 动画帧号
//////////////////////////////////////////////////////////////////////////
// 单色

// 创建
BASEGRAPH_COMMON_API void BaseGraphColor_CreateHomo(OUT BaseGraphColor* pColor,		// 待创建颜色
									IN unsigned long color);	// 单色
// 填充
void BaseGraphColor_FillHomo(IN BaseGraphColor* pColor,		// 填充颜色
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
void BaseGraphColor_FillGradLinear(IN BaseGraphColor* pColor,		// 填充颜色
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
void BaseGraphColor_FillLineBaseAR_1_Recursive(OUT DWORD* pdwBuf,		// 填充对象
									IN int nBufWidth,
									IN DWORD *pdwColor,		// 单色组
									IN float *pfPos,		// 位置组
									IN float *pfMid,		// 中间点组
									IN int nCount,			// 节点个数
									IN DWORD dwRepeat,		// 重复频率
									IN DWORD dwStyle);		// 重复方式
// 填充
// 圆形中心渐变色
void BaseGraphColor_FillGradCenterRound(IN BaseGraphColor* pColor,		// 填充颜色
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
void BaseGraphColor_FillGradCenterPolygon(IN BaseGraphColor* pColor,		// 填充颜色
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
void BaseGraphColor_FillGradOutlineCircle(IN BaseGraphColor* pColor,		// 填充颜色
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
void BaseGraphColor_FillGradOutlinePolygon(IN BaseGraphColor* pColor,		// 填充颜色
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
void BaseGraphColor_FillGradPtContour(IN BaseGraphColor* pColor,		// 填充颜色
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
void BaseGraphColor_FillGradPtBlend(IN BaseGraphColor* pColor,		// 填充颜色
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
void BaseGraphColor_FillGradTemplet(IN BaseGraphColor* pColor,		// 填充颜色
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
BASEGRAPH_COMMON_API BaseGraphCOLOR_RESULT BaseGraphColor_AddTemplet(IN LPCTSTR szFilePath,		// 模板原始路径
											  IN int nIconWidth,
											  IN int nIconHeight,
											  OUT int *pnIndex);
BASEGRAPH_COMMON_API BaseGraphCOLOR_RESULT BaseGraphColor_DelTemplet(IN int nIndex);
// 获取模板数据
BASEGRAPH_COMMON_API DWORD* BaseGraphColor_GetTempletIconData(IN int nIndex);
BASEGRAPH_COMMON_API int BaseGraphColor_GetTempletIconWidth(IN int nIndex);
BASEGRAPH_COMMON_API int BaseGraphColor_GetTempletIconHeight(IN int nIndex);
BASEGRAPH_COMMON_API BaseGraphCOLOR_RESULT BaseGraphColor_GetTempletIconSize(OUT int *pnWidth,OUT int *pnHeight,IN int nIndex);
BASEGRAPH_COMMON_API LPCTSTR BaseGraphColor_GetTempletName(IN int nIndex);
BASEGRAPH_COMMON_API int BaseGraphColor_GetTempletCount();

// 填充
void BaseGraphColor_FillTexImg(IN BaseGraphColor* pColor,			// 填充颜色
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
BASEGRAPH_COMMON_API BaseGraphCOLOR_RESULT BaseGraphColor_AddTexImg(IN LPCTSTR szFilePath,		// 图片原始路径
											 IN int nIconWidth,
											 IN int nIconHeight,
											 OUT int *pnIndex);
BASEGRAPH_COMMON_API BaseGraphCOLOR_RESULT BaseGraphColor_DelTexImg(IN int nIndex);
// 获取纹理数据
BASEGRAPH_COMMON_API DWORD* BaseGraphColor_GetTexImgIconData(IN int nIndex);
BASEGRAPH_COMMON_API int BaseGraphColor_GetTexImgIconWidth(IN int nIndex);
BASEGRAPH_COMMON_API int BaseGraphColor_GetTexImgIconHeight(IN int nIndex);
BASEGRAPH_COMMON_API BaseGraphCOLOR_RESULT BaseGraphColor_GetTexImgIconSize(OUT int *pnWidth,
													 OUT int *pnHeight,
													 IN int nIndex);
BASEGRAPH_COMMON_API LPCTSTR BaseGraphColor_GetTexImgName(IN int nIndex);
BASEGRAPH_COMMON_API int BaseGraphColor_GetTexImgCount();

BASEGRAPH_COMMON_API void DWORDColorToFloat4(float4* pOutColor,DWORD colorIn);	
BASEGRAPH_COMMON_API void Float4ToDWORDColor(DWORD* pColorOut , const float4& ColorIn);

BASEGRAPH_COMMON_API int BaseGraphColor_SaveColor(IN BaseGraphColor* pColor,void* pBuffer,int nCount);
BASEGRAPH_COMMON_API int BaseGraphColor_LoadColor(IN BaseGraphColor* pColor,void* pBuffer,int nCount);

// 创建缺省色
void BaseGraphColor_CreateLinearDefault(OUT BaseGraphColor* pColor);
void BaseGraphColor_CreateCenterRoundDefault(OUT BaseGraphColor* pColor)	;
void BaseGraphColor_CreateCenterPolygonDefault(OUT BaseGraphColor* pColor);
void BaseGraphColor_CreateOutlineCircleDefault(OUT BaseGraphColor* pColor);
void BaseGraphColor_CreateOutlinePolygonDefault(OUT BaseGraphColor* pColor);
void BaseGraphColor_CreateMultiPtDefault(OUT BaseGraphColor* pColor,	IN DWORD dwType);
void BaseGraphColor_CreateTempletDefault(OUT BaseGraphColor* pColor);
void BaseGraphColor_CreateTexImgDefault(OUT BaseGraphColor* pColor);
BASEGRAPH_COMMON_API void BaseGraphColor_CreateDefault(OUT BaseGraphColor* pColor,IN DWORD dwType);		// 颜色种类

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
