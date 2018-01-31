//////////////////////////////////////////////////////////////////////////
// 描述：	各个颜色空间之间的转换函数
// 创建：	
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "BufferColorSpaceConv.h"
#include "BufferSSE2.h"
#include "Common\BufferCommonDef.h"

//////////////////////////////////////////////////////////////////////////
// 常量定义

#define DEFAULT_ALPHA_VALUE			0xff000000		// 缺省的 Alpha 值

///////////////////////////////////////////////////////////////////////////////
// YUVA 与 RGBA 之间的转换

// 高清定义（HD）
static const float gc_matYUVA2RGBA_HD[][4] = 
{
    { 1.f,	    0.f,		    1.574722f,	    0.f },
    { 1.f,	    -0.187314f,	    -0.468207f, 	0.f },
    { 1.f,	    1.855615f,	    0.f,	    	0.f },
    { 0.f,	    0.f,	    	0.f,	    	1.f }
};

static const float gc_matRGBA2YUVA_HD[][4] = {
    { 0.212639f,	0.715169f,  	0.072192f,	    0.f },
    { -0.114592f,	-0.385409f,	    0.5f,		    0.f },
    { 0.5f,		    -0.454156f,	    -0.045844f,	    0.f },
    { 0.f,		    0.f,		    0.f,	    	1.f }
};

// 标清定义（SD）（索尼认为数字信号下 PAL 与 NTSC 相同）
static const float gc_matYUVA2RGBA_SD[][4] = {
    { 1.f,  	0.f,		    1.401978f,	    0.f },
    { 1.f,  	-0.344116f,	    -0.714111f, 	0.f },
    { 1.f,  	1.771973f,  	0.f,		    0.f },
    { 0.f,  	0.f,		    0.f,		    1.f }
};

static const float gc_matRGBA2YUVA_SD[][4] = {
    { 0.29895f, 	0.587036f,	    0.114014f,  	0.f },
    { -0.168701f,	-0.331299f,     0.5f,		    0.f },
    { 0.5f,	    	-0.418701f,	    -0.081299f, 	0.f },
    { 0.f,	    	0.f,		    0.f,		    1.f }
};

///////////////////////////////////////////////////////////////////////////////
// LUVA 与 RGBA 之间的转换

static const float gc_matRGBA2LUVA[][4] = {
    { 0.2988f,      0.5868f,        0.1144f,        0.f },
    { -0.1610824f,	-0.3162388f,	0.4773212f,		0.f },
    { 0.4773212f,	-0.3996969f,	-0.07762426f,	0.f },
    { 0.f,			0.f,			0.f,			1.f }
};

static const float gc_matLUVA2RGBA[][4] = {
    { 1.f,  	0.f,			1.468613f,		0.f },
    { 1.f,	    -0.3604871f,	-0.7480668f,	0.f },
    { 1.f,	    1.856193f,		0.f,			0.f },
    { 0.f,	    0.f,			0.f,			1.f }
};

//////////////////////////////////////////////////////////////////////////
// 根据制式获取转换矩阵

float* BUFFERCS_GetMatYUVA2RGBA(IN UINT uDefinition)     // 制式
{
    if (uDefinition == BUFFER_DEFINITION_HIGH)
        return (float*)gc_matYUVA2RGBA_HD;

    if (uDefinition == BUFFER_DEFINITION_STANDARD)
        return (float*)gc_matYUVA2RGBA_SD;

    return NULL;
}

float BUFFERCS_GetMatYUVA2RGBA_Cell(IN int nRow,			// 行
													IN int nCol,			// 列
													IN UINT uDefinition)	// 制式
{
	//assert((nRow >= 0) && (nRow < 4) && (nCol >= 0) && (nCol < 4));

    if (uDefinition == BUFFER_DEFINITION_HIGH)
        return gc_matYUVA2RGBA_HD[nRow][nCol];

    if (uDefinition == BUFFER_DEFINITION_STANDARD)
        return gc_matYUVA2RGBA_SD[nRow][nCol];

    return 0.f;
}

float* BUFFERCS_GetMatRGBA2YUVA(IN UINT uDefinition)     // 制式
{
    if (uDefinition == BUFFER_DEFINITION_HIGH)
        return (float*)gc_matRGBA2YUVA_HD;

    if (uDefinition == BUFFER_DEFINITION_STANDARD)
        return (float*)gc_matRGBA2YUVA_SD;

    return NULL;
}

float BUFFERCS_GetMatRGBA2YUVA_Cell(IN int nRow,			// 行
													IN int nCol,			// 列
													IN UINT uDefinition)	// 制式
{
	//assert((nRow >= 0) && (nRow < 4) && (nCol >= 0) && (nCol < 4));

    if (uDefinition == BUFFER_DEFINITION_HIGH)
        return gc_matRGBA2YUVA_HD[nRow][nCol];

    if (uDefinition == BUFFER_DEFINITION_STANDARD)
        return gc_matRGBA2YUVA_SD[nRow][nCol];

    return 0.f;
}

///////////////////////////////////////////////////////////////////////////////
// LUVA 与 RGBA 之间的转换

float* BUFFERCS_GetMatLUVA2RGBA()
{
    return (float*)gc_matLUVA2RGBA;
}

float* BUFFERCS_GetMatRGBA2LUVA()
{
    return (float*)gc_matRGBA2LUVA;
}

///////////////////////////////////////////////////////////////////////////////

// 场与帧的转换

void BUFFERCS_ComposeFields(OUT BYTE* pDes,			// 目标
											IN int nDesPitch,
											IN BYTE* pSrcTop,		// 上场源
											IN int nTopPitch,
											IN BYTE* pSrcBottom,	// 下场源
											IN int nBottomPitch,
											IN int nWidth,			// 目标大小
											IN int nHeight,
											IN int nPixelBytes)		// 一个像素的大小
{
	//assert((pDes != NULL) && (pSrcTop != NULL) && (pSrcBottom != NULL));

	int nRealLen = nWidth * nPixelBytes;
    int na16 = (nRealLen >> 4), nm16 = (nRealLen & 0x0f);
    int nHalfHeight = (nHeight >> 1);

	//assert((nTopPitch >= nRealLen) && (nBottomPitch >= nRealLen) && (nDesPitch >= nRealLen));
    for (int i=0; i<nHalfHeight; i++)
    {
        // 第一场填充
        memcpy_line(pDes, pSrcTop, na16, nm16);
        pDes += nDesPitch;
        pSrcTop += nTopPitch;
        // 第二场填充
        memcpy_line(pDes, pSrcBottom, na16, nm16);
        pDes += nDesPitch;
        pSrcBottom += nBottomPitch;
    }
}

void BUFFERCS_SplitFrame(OUT BYTE* pDesTop,			// 目标上场
										 IN int nTopPitch,
										 OUT BYTE* pDesBottom,		// 目标下场
										 IN int nBottomPitch,
										 IN BYTE* pSrc,				// 源帧
										 IN int nSrcPitch,
										 IN int nWidth,				// 源大小
										 IN int nHeight,
										 IN int nPixelBytes)		// 一个像素的大小
{
	//assert((pDesTop != NULL) && (pSrc != NULL) && (pDesBottom != NULL));

    int nRealLen = nWidth * nPixelBytes;
    int na16 = (nRealLen >> 4), nm16 = (nRealLen & 0x0f);
    int nHalfHeight = (nHeight >> 1);

	//assert((nTopPitch >= nRealLen) && (nBottomPitch >= nRealLen) && (nSrcPitch >= nRealLen));
    for (int i=0; i<nHalfHeight; i++)
    {
        // 第一场填充
        memcpy_line(pDesTop, pSrc, na16, nm16);
        pSrc += nSrcPitch;
        pDesTop += nTopPitch;
        // 第二场填充
        memcpy_line(pDesBottom, pSrc, na16, nm16);
        pDesBottom += nBottomPitch;
        pSrc += nSrcPitch;
    }
}

// YUV 内部格式转换 420 <-> 422 <-> 444

void BUFFERCS_420_422_YUVFrame(OUT BYTE* pDes,		// YUYV 输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											   IN int nDesPitch,
											   IN BYTE* pSrcY,		// Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
											   IN int nSrcYPitch,
											   IN BYTE* pSrcU,		// U 输入（存储方式 UU0 UU2 UU4 ……）（两行共用一个）
											   IN int nSrcUPitch,
											   IN BYTE* pSrcV,		// V 输入（存储方式 VV0 VV2 VV4 ……）（两行共用一个）
											   IN int nSrcVPitch,
											   IN int nWidth,		// 源图大小
											   IN int nHeight)
{
	//assert((pDes != NULL) && (pSrcY != NULL) && (pSrcU != NULL) && (pSrcV != NULL));
	//assert((nDesPitch >= (nWidth << 1)) && (nSrcYPitch >= nWidth));
	//assert((nSrcUPitch >= (nWidth >> 1)) && (nSrcVPitch >= (nWidth >> 1)));

    int nHalfHeight = (nHeight >> 1), nHalfHalf = ((nHalfHeight + 1) >> 1);
    int na16 = (nWidth >> 4), nm16 = (nWidth & 0x0f);
    int nYP2 = (nSrcYPitch << 1), nUP2 = (nSrcUPitch << 1), nVP2 = (nSrcVPitch << 1);
    int nDP2 = (nDesPitch << 1), nDP3 = (nDP2 + nDesPitch), nYP3 = (nYP2 + nSrcYPitch);

    // 前两行
    // 第一场
    yuv420_to_yuyv_line_2_F(pDes, nDP2, pSrcY, nYP2, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
    pDes += nDesPitch; pSrcY += nSrcYPitch; pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
    // 第二场
    yuv420_to_yuyv_line_2_F(pDes, nDP2, pSrcY, nYP2, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
    pDes += nDP3; pSrcY += nYP3; pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
    // 中间行
    for (int i=2; i<nHalfHalf; i++)
    {
        // 第一场
        yuv420_to_yuyv_line_2(pDes, nDP2, pSrcY, nYP2, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
        pDes += nDesPitch; pSrcY += nSrcYPitch; pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
        // 第二场
        yuv420_to_yuyv_line_2(pDes, nDP2, pSrcY, nYP2, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
        pDes += nDP3; pSrcY += nYP3; pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
    }
    // 最后两行或一行（场为奇数行时）
    if (nHalfHalf & 0x01)       // 一行
    {
        // 第一场
        yuv420_to_yuyv_line_1_L(pDes, pSrcY, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
        pDes += nDesPitch; pSrcY += nSrcYPitch; pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
        // 第二场
        yuv420_to_yuyv_line_1_L(pDes, pSrcY, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
    }
    else    // 两行
    {
        // 第一场
        yuv420_to_yuyv_line_2_L(pDes, nDP2, pSrcY, nYP2, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
        pDes += nDesPitch; pSrcY += nSrcYPitch; pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
        // 第二场
        yuv420_to_yuyv_line_2_L(pDes, nDP2, pSrcY, nYP2, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
    }
}
void BUFFERCS_420_422_YUVSplit(OUT BYTE* pDesFirst,		// YUYV 第一场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											   IN int nFirstPitch,
											   OUT BYTE* pDesSecond,	// YUYV 第二场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											   IN int nSecondPitch,
											   IN BYTE* pSrcY,			// Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
											   IN int nSrcYPitch,
											   IN BYTE* pSrcU,			// U 输入（存储方式 UU0 UU2 UU4 ……）（两行共用一个）
											   IN int nSrcUPitch,
											   IN BYTE* pSrcV,			// V 输入（存储方式 VV0 VV2 VV4 ……）（两行共用一个）
											   IN int nSrcVPitch,
											   IN int nWidth,			// 源图大小
											   IN int nHeight)
{
	//assert((pDesFirst != NULL) && (pDesSecond != NULL) && (pSrcY != NULL) && (pSrcU != NULL) && (pSrcV != NULL));
	//assert((nFirstPitch >= (nWidth << 1)) && (nSecondPitch >= (nWidth << 1)) && (nSrcYPitch >= nWidth));
	//assert((nSrcUPitch >= (nWidth >> 1)) && (nSrcVPitch >= (nWidth >> 1)));

    int nHalfHeight = (nHeight >> 1), nHalfHalf = ((nHalfHeight + 1) >> 1);
    int na16 = (nWidth >> 4), nm16 = (nWidth & 0x0f);
    BYTE* pYF = pSrcY, *pYS = pSrcY + nSrcYPitch;
    BYTE* pUF = pSrcU, *pUS = pSrcU + nSrcUPitch;
    BYTE* pVF = pSrcV, *pVS = pSrcV + nSrcVPitch;
    int nYP2 = (nSrcYPitch << 1), nUP2 = (nSrcUPitch << 1), nVP2 = (nSrcVPitch << 1);
    int nDFP2 = (nFirstPitch << 1), nDSP2 = (nSecondPitch << 1), nYP4 = (nYP2 << 1);

    // 前两行
    // 第一场
    yuv420_to_yuyv_line_2_F(pDesFirst, nFirstPitch, pYF, nYP2, pUF, nUP2, pVF, nVP2, na16, nm16);
    pDesFirst += nDFP2; pYF += nYP4; pUF += nUP2; pVF += nVP2;
    // 第二场
    yuv420_to_yuyv_line_2_F(pDesSecond, nSecondPitch, pYS, nYP2, pUS, nUP2, pVS, nVP2, na16, nm16);
    pDesSecond += nDSP2; pYS += nYP4; pUS += nUP2; pVS += nVP2;
    // 中间行
    for (int i=2; i<nHalfHalf; i++)
    {
        // 第一场
        yuv420_to_yuyv_line_2(pDesFirst, nFirstPitch, pYF, nYP2, pUF, nUP2, pVF, nVP2, na16, nm16);
        pDesFirst += nDFP2; pYF += nYP4; pUF += nUP2; pVF += nVP2;
        // 第二场
        yuv420_to_yuyv_line_2(pDesSecond, nSecondPitch, pYS, nYP2, pUS, nUP2, pVS, nVP2, na16, nm16);
        pDesSecond += nDSP2; pYS += nYP4; pUS += nUP2; pVS += nVP2;
    }
    // 最后两行或一行（场为奇数行时）
    if (nHalfHalf & 0x01)       // 一行
    {
        // 第一场
        yuv420_to_yuyv_line_1_L(pDesFirst, pYF, pUF, nUP2, pVF, nVP2, na16, nm16);
        // 第二场
        yuv420_to_yuyv_line_1_L(pDesSecond, pYS, pUS, nUP2, pVS, nVP2, na16, nm16);
    }
    else    // 两行
    {
        // 第一场
        yuv420_to_yuyv_line_2_L(pDesFirst, nFirstPitch, pYF, nYP2, pUF, nUP2, pVF, nVP2, na16, nm16);
        // 第二场
        yuv420_to_yuyv_line_2_L(pDesSecond, nSecondPitch, pYS, nYP2, pUS, nUP2, pVS, nVP2, na16, nm16);
    }
}

void BUFFERCS_ComposeYUV422_Frame(OUT BYTE* pDes,		// YUYV 输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
												  IN int nDesPitch,
												  IN BYTE* pSrcY,		// Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
												  IN int nSrcYPitch,
												  IN BYTE* pSrcU,		// U 输入（存储方式 U0 U2 U4 ……）
												  IN int nSrcUPitch,
												  IN BYTE* pSrcV,		// V 输入（存储方式 V0 V2 V4 ……）
												  IN int nSrcVPitch,
												  IN int nWidth,		// 源图大小
												  IN int nHeight)
{
	//assert((pDes != NULL) && (pSrcY != NULL) && (pSrcU != NULL) && (pSrcV != NULL));
	//assert((nDesPitch >= (nWidth << 1)) && (nSrcYPitch >= nWidth));
	//assert((nSrcUPitch >= (nWidth >> 1)) && (nSrcVPitch >= (nWidth >> 1)));
    
    int na16 = (nWidth >> 4), nm16 = (nWidth & 0x0f);

    for (int i=0; i<nHeight; i++)
    {
        yuv422_to_yuyv_line(pDes, pSrcY, pSrcU, pSrcV, na16, nm16);
        pDes += nDesPitch;
        pSrcY += nSrcYPitch;
        pSrcU += nSrcUPitch;
        pSrcV += nSrcVPitch;
    }
}
void BUFFERCS_ComposeYUV422_Split(OUT BYTE* pDesFirst,		// YUYV 第一场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
												  IN int nFirstPitch,
												  OUT BYTE* pDesSecond,		// YUYV 第二场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
												  IN int nSecondPitch,
												  IN BYTE* pSrcY,			// Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
												  IN int nSrcYPitch,
												  IN BYTE* pSrcU,			// U 输入（存储方式 U0 U2 U4 ……）
												  IN int nSrcUPitch,
												  IN BYTE* pSrcV,			// V 输入（存储方式 V0 V2 V4 ……）
												  IN int nSrcVPitch,
												  IN int nWidth,			// 源图大小
												  IN int nHeight)
{
	//assert((pDesFirst != NULL) && (pDesSecond != NULL) && (pSrcY != NULL) && (pSrcU != NULL) && (pSrcV != NULL));
	//assert((nFirstPitch >= (nWidth << 1)) && (nSecondPitch >= (nWidth << 1)) && (nSrcYPitch >= nWidth));
	//assert((nSrcUPitch >= (nWidth >> 1)) && (nSrcVPitch >= (nWidth >> 1)));

    int nHalfHeight = (nHeight >> 1);
    int na16 = (nWidth >> 4), nm16 = (nWidth & 0x0f);

    for (int i=0; i<nHalfHeight; i++)
    {
        yuv422_to_yuyv_line(pDesFirst, pSrcY, pSrcU, pSrcV, na16, nm16);
        pDesFirst += nFirstPitch;
        pSrcY += nSrcYPitch;
        pSrcU += nSrcUPitch;
        pSrcV += nSrcVPitch;
        yuv422_to_yuyv_line(pDesSecond, pSrcY, pSrcU, pSrcV, na16, nm16);
        pDesSecond += nSecondPitch;
        pSrcY += nSrcYPitch;
        pSrcU += nSrcUPitch;
        pSrcV += nSrcVPitch;
    }
    if (nHeight & 0x01)     // 奇数行则第二场拷贝
    {
        yuv422_to_yuyv_line(pDesFirst, pSrcY, pSrcU, pSrcV, na16, nm16);
        memcpy_line(pDesSecond, pDesFirst, (nWidth >> 3), ((nWidth & 0x07) << 1));
    }
}

// YUV 420、422 片存储格式与 RGBA、BGRA 之间的转换

void BUFFERCS_YUV420_RGBA_Frame(OUT BYTE* pDesRGBA,			// RGBA 输出（存储方式 R G B A R G B A R G B A ……）（Alpha 直接填 0xff）
												IN int nDesPitch,
												IN BYTE* pSrcY,				// Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
												IN int nSrcYPitch,
												IN BYTE* pSrcU,				// U 输入（存储方式 UU0 UU2 UU4 ……）（两行共用一个）
												IN int nSrcUPitch,
												IN BYTE* pSrcV,				// V 输入（存储方式 VV0 VV2 VV4 ……）（两行共用一个）
												IN int nSrcVPitch,
												IN int nWidth,				// 目标大小
												IN int nHeight,
												IN BOOL bHD,				// 高标清标志
												IN BOOL bDesRevert)			// 图像是否行翻转标志
{
	//assert((pDesRGBA != NULL) && (pSrcY != NULL) && (pSrcU != NULL) && (pSrcV != NULL));
	//assert((nSrcYPitch >= nWidth) && ((nSrcUPitch << 1) >= nWidth) && ((nSrcVPitch << 1) >= nWidth) && (nDesPitch >= (nWidth << 2)));

    yuv420_to_color32_frame(pDesRGBA, nDesPitch, pSrcY, nSrcYPitch, pSrcU, nSrcUPitch, pSrcV, nSrcVPitch, nWidth, nHeight, bHD, bDesRevert, FALSE);
}
void BUFFERCS_YUV420_RGBA_Split(OUT BYTE* pDesFirst,		// RGBA 第一场输出（存储方式 R G B A R G B A R G B A ……）（Alpha 直接填 0xff）
												IN int nFirstPitch,
												OUT BYTE* pDesSecond,		// RGBA 第二场输出（存储方式 R G B A R G B A R G B A ……）（Alpha 直接填 0xff）
												IN int nSecondPitch,
												IN BYTE* pSrcY,				// Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
												IN int nSrcYPitch,
												IN BYTE* pSrcU,				// U 输入（存储方式 UU0 UU2 UU4 ……）（两行共用一个）
												IN int nSrcUPitch,
												IN BYTE* pSrcV,				// V 输入（存储方式 VV0 VV2 VV4 ……）（两行共用一个）
												IN int nSrcVPitch,
												IN int nWidth,				// 目标大小
												IN int nHeight,
												IN BOOL bHD,				// 高标清标志
												IN BOOL bDesRevert)			// 图像是否行翻转标志
{
	//assert((pDesFirst != NULL) && (pDesSecond != NULL) && (pSrcY != NULL) && (pSrcU != NULL) && (pSrcV != NULL));
	//assert((nFirstPitch >= (nWidth << 2)) && (nSecondPitch >= (nWidth << 2)));
	//assert((nSrcYPitch >= nWidth) && ((nSrcUPitch << 1) >= nWidth) && ((nSrcVPitch << 1) >= nWidth));

    yuv420_to_color32_split(pDesFirst, nFirstPitch, pDesSecond, nSecondPitch,
        pSrcY, nSrcYPitch, pSrcU, nSrcUPitch, pSrcV, nSrcVPitch, nWidth, nHeight, bHD, bDesRevert, FALSE);
}

void BUFFERCS_YUV422_RGBA_Frame(OUT BYTE* pDesRGBA,			// RGBA 输出（存储方式 R G B A R G B A R G B A ……）（Alpha 直接填 0xff）
												IN int nDesPitch,
												IN BYTE* pSrcY,				// Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
												IN int nSrcYPitch,
												IN BYTE* pSrcU,				// U 输入（存储方式 U0 U2 U4 ……）
												IN int nSrcUPitch,
												IN BYTE* pSrcV,				// V 输入（存储方式 V0 V2 V4 ……）
												IN int nSrcVPitch,
												IN int nWidth,				// 目标大小
												IN int nHeight,
												IN BOOL bHD,				// 高标清标志
												IN BOOL bDesRevert)			// 图像是否行翻转标志
{
	//assert((pDesRGBA != NULL) && (pSrcY != NULL) && (pSrcU != NULL) && (pSrcV != NULL));
	//assert((nSrcYPitch >= nWidth) && ((nSrcUPitch << 1) >= nWidth) && ((nSrcVPitch << 1) >= nWidth) && (nDesPitch >= (nWidth << 2)));

    yuv422_to_color32_frame(pDesRGBA, nDesPitch, pSrcY, nSrcYPitch, pSrcU, nSrcUPitch, pSrcV, nSrcVPitch, nWidth, nHeight, bHD, bDesRevert, FALSE);
}
void BUFFERCS_YUV422_RGBA_Split(OUT BYTE* pDesFirst,		// RGBA 第一场输出（存储方式 R G B A R G B A R G B A ……）（Alpha 直接填 0xff）
												IN int nFirstPitch,
												OUT BYTE* pDesSecond,		// RGBA 第二场输出（存储方式 R G B A R G B A R G B A ……）（Alpha 直接填 0xff）
												IN int nSecondPitch,
												IN BYTE* pSrcY,				// Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
												IN int nSrcYPitch,
												IN BYTE* pSrcU,				// U 输入（存储方式 U0 U2 U4 ……）
												IN int nSrcUPitch,
												IN BYTE* pSrcV,				// V 输入（存储方式 V0 V2 V4 ……）
												IN int nSrcVPitch,
												IN int nWidth,				// 目标大小
												IN int nHeight,
												IN BOOL bHD,				// 高标清标志
												IN BOOL bDesRevert)			// 图像是否行翻转标志
{
	//assert((pDesFirst != NULL) && (pDesSecond != NULL) && (pSrcY != NULL) && (pSrcU != NULL) && (pSrcV != NULL));
	//assert((nFirstPitch >= (nWidth << 2)) && (nSecondPitch >= (nWidth << 2)));
	//assert((nSrcYPitch >= nWidth) && ((nSrcUPitch << 1) >= nWidth) && ((nSrcVPitch << 1) >= nWidth));

    yuv422_to_color32_split(pDesFirst, nFirstPitch, pDesSecond, nSecondPitch,
        pSrcY, nSrcYPitch, pSrcU, nSrcUPitch, pSrcV, nSrcVPitch, nWidth, nHeight, bHD, bDesRevert, FALSE);
}

void BUFFERCS_YUV420_BGRA_Frame(OUT BYTE* pDesBGRA,			// BGRA 输出（存储方式 B G R A B G R A B G R A ……）（Alpha 直接填 0xff）
												IN int nDesPitch,
												IN BYTE* pSrcY,				// Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
												IN int nSrcYPitch,
												IN BYTE* pSrcU,				// U 输入（存储方式 UU0 UU2 UU4 ……）（两行共用一个）
												IN int nSrcUPitch,
												IN BYTE* pSrcV,				// V 输入（存储方式 VV0 VV2 VV4 ……）（两行共用一个）
												IN int nSrcVPitch,
												IN int nWidth,				// 目标大小
												IN int nHeight,
												IN BOOL bHD,				// 高标清标志
												IN BOOL bDesRevert)			// 图像是否行翻转标志
{
	//assert((pDesBGRA != NULL) && (pSrcY != NULL) && (pSrcU != NULL) && (pSrcV != NULL));
	//assert((nSrcYPitch >= nWidth) && ((nSrcUPitch << 1) >= nWidth) && ((nSrcVPitch << 1) >= nWidth) && (nDesPitch >= (nWidth << 2)));

    yuv420_to_color32_frame(pDesBGRA, nDesPitch, pSrcY, nSrcYPitch, pSrcU, nSrcUPitch, pSrcV, nSrcVPitch, nWidth, nHeight, bHD, bDesRevert, TRUE);
}
void BUFFERCS_YUV420_BGRA_Split(OUT BYTE* pDesFirst,		// BGRA 第一场输出（存储方式 B G R A B G R A B G R A ……）（Alpha 直接填 0xff）
												IN int nFirstPitch,
												OUT BYTE* pDesSecond,		// RGBA 第二场输出（存储方式 B G R A B G R A B G R A ……）（Alpha 直接填 0xff）
												IN int nSecondPitch,
												IN BYTE* pSrcY,				// Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
												IN int nSrcYPitch,
												IN BYTE* pSrcU,				// U 输入（存储方式 UU0 UU2 UU4 ……）（两行共用一个）
												IN int nSrcUPitch,
												IN BYTE* pSrcV,				// V 输入（存储方式 VV0 VV2 VV4 ……）（两行共用一个）
												IN int nSrcVPitch,
												IN int nWidth,				// 目标大小
												IN int nHeight,
												IN BOOL bHD,				// 高标清标志
												IN BOOL bDesRevert)			// 图像是否行翻转标志
{
	//assert((pDesFirst != NULL) && (pDesSecond != NULL) && (pSrcY != NULL) && (pSrcU != NULL) && (pSrcV != NULL));
	//assert((nFirstPitch >= (nWidth << 2)) && (nSecondPitch >= (nWidth << 2)));
	//assert((nSrcYPitch >= nWidth) && ((nSrcUPitch << 1) >= nWidth) && ((nSrcVPitch << 1) >= nWidth));

    yuv420_to_color32_split(pDesFirst, nFirstPitch, pDesSecond, nSecondPitch,
        pSrcY, nSrcYPitch, pSrcU, nSrcUPitch, pSrcV, nSrcVPitch, nWidth, nHeight, bHD, bDesRevert, TRUE);
}

void BUFFERCS_YUV422_BGRA_Frame(OUT BYTE* pDesBGRA,			// BGRA 输出（存储方式 B G R A B G R A B G R A ……）（Alpha 直接填 0xff）
												IN int nDesPitch,
												IN BYTE* pSrcY,				// Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
												IN int nSrcYPitch,
												IN BYTE* pSrcU,				// U 输入（存储方式 U0 U2 U4 ……）
												IN int nSrcUPitch,
												IN BYTE* pSrcV,				// V 输入（存储方式 V0 V2 V4 ……）
												IN int nSrcVPitch,
												IN int nWidth,				// 目标大小
												IN int nHeight,
												IN BOOL bHD,				// 高标清标志
												IN BOOL bDesRevert)			// 图像是否行翻转标志
{
	//assert((pDesBGRA != NULL) && (pSrcY != NULL) && (pSrcU != NULL) && (pSrcV != NULL));
	//assert((nSrcYPitch >= nWidth) && ((nSrcUPitch << 1) >= nWidth) && ((nSrcVPitch << 1) >= nWidth) && (nDesPitch >= (nWidth << 2)));

    yuv422_to_color32_frame(pDesBGRA, nDesPitch, pSrcY, nSrcYPitch, pSrcU, nSrcUPitch, pSrcV, nSrcVPitch, nWidth, nHeight, bHD, bDesRevert, TRUE);
}
void BUFFERCS_YUV422_BGRA_Split(OUT BYTE* pDesFirst,		// BGRA 第一场输出（存储方式 B G R A B G R A B G R A ……）（Alpha 直接填 0xff）
												IN int nFirstPitch,
												OUT BYTE* pDesSecond,		// RGBA 第二场输出（存储方式 B G R A B G R A B G R A ……）（Alpha 直接填 0xff）
												IN int nSecondPitch,
												IN BYTE* pSrcY,				// Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
												IN int nSrcYPitch,
												IN BYTE* pSrcU,				// U 输入（存储方式 U0 U2 U4 ……）
												IN int nSrcUPitch,
												IN BYTE* pSrcV,				// V 输入（存储方式 V0 V2 V4 ……）
												IN int nSrcVPitch,
												IN int nWidth,				// 目标大小
												IN int nHeight,
												IN BOOL bHD,				// 高标清标志
												IN BOOL bDesRevert)			// 图像是否行翻转标志
{
	//assert((pDesFirst != NULL) && (pDesSecond != NULL) && (pSrcY != NULL) && (pSrcU != NULL) && (pSrcV != NULL));
	//assert((nFirstPitch >= (nWidth << 2)) && (nSecondPitch >= (nWidth << 2)));
	//assert((nSrcYPitch >= nWidth) && ((nSrcUPitch << 1) >= nWidth) && ((nSrcVPitch << 1) >= nWidth));

    yuv422_to_color32_split(pDesFirst, nFirstPitch, pDesSecond, nSecondPitch,
        pSrcY, nSrcYPitch, pSrcU, nSrcUPitch, pSrcV, nSrcVPitch, nWidth, nHeight, bHD, bDesRevert, TRUE);
}

// YUYVA 与 RGBA、BGRA 之间的转换

void BUFFERCS_RGBA2YUYVA_Frame(OUT BYTE* pDesYUYV,		// YUYV 输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											   IN int nYUYVPitch,
											   OUT BYTE* pDesAlpha,		// Alpha 输出（存储方式 A0 A1 A2 A3 A4 ……）
											   IN int nAlphaPitch,
											   IN BYTE* pSrcRGBA,		// RGBA 输入（存储方式 R G B A R G B A R G B A ……）
											   IN int nSrcPitch,		// 源一行的字节宽度
											   IN int nWidth,			// 源图大小
											   IN int nHeight,
											   IN BOOL bHD,				// 高标清标志
											   IN BOOL bSrcRevert)		// 图像是否行翻转标志
{
	//assert((pDesYUYV != NULL) && (pDesAlpha != NULL) && (pSrcRGBA != NULL));
	//assert((nYUYVPitch >= (nWidth << 1)) && (nAlphaPitch >= nWidth) && (nSrcPitch >= (nWidth << 2)));

    color32_to_yuyva_frame(pDesYUYV, nYUYVPitch, pDesAlpha, nAlphaPitch, pSrcRGBA, nSrcPitch, nWidth, nHeight, bHD, bSrcRevert, FALSE);
}
void BUFFERCS_RGBA2YUYVA_Split(OUT BYTE* pDesYUYVF,		// YUYV 第一场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											   IN int nYUYVPitchF,
											   OUT BYTE* pDesAlphaF,	// Alpha 第一场输出（存储方式 A0 A1 A2 A3 A4 ……）
											   IN int nAlphaPitchF,
											   OUT BYTE* pDesYUYVS,		// YUYV 第二场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											   IN int nYUYVPitchS,
											   OUT BYTE* pDesAlphaS,	// Alpha 第二场输出（存储方式 A0 A1 A2 A3 A4 ……）
											   IN int nAlphaPitchS,
											   IN BYTE* pSrcRGBA,		// RGBA 输入（存储方式 R G B A R G B A R G B A ……）
											   IN int nSrcPitch,		// 源一行的字节宽度
											   IN int nWidth,			// 源图大小
											   IN int nHeight,
											   IN BOOL bHD,				// 高标清标志
											   IN BOOL bSrcRevert)		// 图像是否行翻转标志
{
	//assert((pDesYUYVF != NULL) && (pDesAlphaF != NULL) && (pDesAlphaS != NULL) && (pDesYUYVS != NULL) && (pSrcRGBA != NULL));
	//assert((nYUYVPitchF >= (nWidth << 1)) && (nAlphaPitchF >= nWidth) && (nYUYVPitchS >= (nWidth << 1)) && (nAlphaPitchS >= nWidth));
	//assert(nSrcPitch >= (nWidth << 2));

    color32_to_yuyva_split(pDesYUYVF, nYUYVPitchF, pDesAlphaF, nAlphaPitchF, pDesYUYVS, nYUYVPitchS, pDesAlphaS, nAlphaPitchS,
        pSrcRGBA, nSrcPitch, nWidth, nHeight, bHD, bSrcRevert, FALSE);
}
void BUFFERCS_RGBA2YUYV_Frame(OUT BYTE* pDesYUYV,		// YUYV 输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											  IN int nYUYVPitch,
											  IN BYTE* pSrcRGBA,		// RGBA 输入（存储方式 R G B A R G B A R G B A ……）
											  IN int nSrcPitch,			// 源一行的字节宽度
											  IN int nWidth,			// 源图大小
											  IN int nHeight,
											  IN BOOL bHD,				// 高标清标志
											  IN BOOL bSrcRevert)		// 图像是否行翻转标志
{
	//assert((pDesYUYV != NULL) && (pSrcRGBA != NULL));
	//assert((nYUYVPitch >= (nWidth << 1)) && (nSrcPitch >= (nWidth << 2)));

    color32_to_yuyv_frame(pDesYUYV, nYUYVPitch, pSrcRGBA, nSrcPitch, nWidth, nHeight, bHD, bSrcRevert, FALSE);
}
void BUFFERCS_RGBA2YUYV_Split(OUT BYTE* pDesYUYVF,		// YUYV 第一场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											  IN int nYUYVPitchF,
											  OUT BYTE* pDesYUYVS,		// YUYV 第二场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											  IN int nYUYVPitchS,
											  IN BYTE* pSrcRGBA,		// RGBA 输入（存储方式 R G B A R G B A R G B A ……）
											  IN int nSrcPitch,			// 源一行的字节宽度
											  IN int nWidth,			// 源图大小
											  IN int nHeight,
											  IN BOOL bHD,				// 高标清标志
											  IN BOOL bSrcRevert)		// 图像是否行翻转标志
{
	//assert((pDesYUYVF != NULL) && (pDesYUYVS != NULL) && (pSrcRGBA != NULL));
	//assert((nYUYVPitchF >= (nWidth << 1)) && (nYUYVPitchS >= (nWidth << 1)) && (nSrcPitch >= (nWidth << 2)));

    color32_to_yuyv_split(pDesYUYVF, nYUYVPitchF, pDesYUYVS, nYUYVPitchS, pSrcRGBA, nSrcPitch, nWidth, nHeight, bHD, bSrcRevert, FALSE);
}

void BUFFERCS_YUYV2RGBA_Compose(OUT BYTE* pDesRGBA,		// RGBA 输出（存储方式 R G B A R G B A R G B A ……）（Alpha 直接填 0xff）
												IN int nDesPitch,
												IN BYTE* pSrcYUYVF,		// YUYV 第一场输入（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
												IN int nFirstPitch,
												IN BYTE* pSrcYUYVS,		// YUYV 第二场输入（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
												IN int nSecondPitch,
												IN int nWidth,			// 目标大小
												IN int nHeight,
												IN BOOL bHD)			// 高标清标志
{
	//assert((pDesRGBA != NULL) && (pSrcYUYVF != NULL) && (pSrcYUYVS != NULL));
	//assert((nDesPitch >= (nWidth << 2)) && (nFirstPitch >= (nWidth << 1)) && (nSecondPitch >= (nWidth << 1)));

    yuyv_to_color32_compose(pDesRGBA, nDesPitch, pSrcYUYVF, nFirstPitch, pSrcYUYVS, nSecondPitch, nWidth, nHeight, bHD, FALSE);
}

void BUFFERCS_BGRA2YUYVA_Frame(OUT BYTE* pDesYUYV,		// YUYV 输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											   IN int nYUYVPitch,
											   OUT BYTE* pDesAlpha,		// Alpha 输出（存储方式 A0 A1 A2 A3 A4 ……）
											   IN int nAlphaPitch,
											   IN BYTE* pSrcBGRA,		// BGRA 输入（存储方式 B G R A B G R A B G R A ……）
											   IN int nSrcPitch,		// 源一行的字节宽度
											   IN int nWidth,			// 源图大小
											   IN int nHeight,
											   IN BOOL bHD,				// 高标清标志
											   IN BOOL bSrcRevert)		// 图像是否行翻转标志
{
	//assert((pDesYUYV != NULL) && (pDesAlpha != NULL) && (pSrcBGRA != NULL));
	//assert((nYUYVPitch >= (nWidth << 1)) && (nAlphaPitch >= nWidth) && (nSrcPitch >= (nWidth << 2)));

    color32_to_yuyva_frame(pDesYUYV, nYUYVPitch, pDesAlpha, nAlphaPitch, pSrcBGRA, nSrcPitch, nWidth, nHeight, bHD, bSrcRevert, TRUE);
}
void BUFFERCS_BGRA2YUYVA_Split(OUT BYTE* pDesYUYVF,		// YUYV 第一场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											   IN int nYUYVPitchF,
											   OUT BYTE* pDesAlphaF,	// Alpha 第一场输出（存储方式 A0 A1 A2 A3 A4 ……）
											   IN int nAlphaPitchF,
											   OUT BYTE* pDesYUYVS,		// YUYV 第二场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											   IN int nYUYVPitchS,
											   OUT BYTE* pDesAlphaS,	// Alpha 第二场输出（存储方式 A0 A1 A2 A3 A4 ……）
											   IN int nAlphaPitchS,
											   IN BYTE* pSrcBGRA,		// BGRA 输入（存储方式 B G R A B G R A B G R A ……）
											   IN int nSrcPitch,		// 源一行的字节宽度
											   IN int nWidth,			// 源图大小
											   IN int nHeight,
											   IN BOOL bHD,				// 高标清标志
											   IN BOOL bSrcRevert)		// 图像是否行翻转标志
{
	//assert((pDesYUYVF != NULL) && (pDesAlphaF != NULL) && (pDesAlphaS != NULL) && (pDesYUYVS != NULL) && (pSrcBGRA != NULL));
	//assert((nYUYVPitchF >= (nWidth << 1)) && (nAlphaPitchF >= nWidth) && (nYUYVPitchS >= (nWidth << 1)) && (nAlphaPitchS >= nWidth));
	//assert(nSrcPitch >= (nWidth << 2));

    color32_to_yuyva_split(pDesYUYVF, nYUYVPitchF, pDesAlphaF, nAlphaPitchF, pDesYUYVS, nYUYVPitchS, pDesAlphaS, nAlphaPitchS,
        pSrcBGRA, nSrcPitch, nWidth, nHeight, bHD, bSrcRevert, TRUE);
}
void BUFFERCS_BGRA2YUYV_Frame(OUT BYTE* pDesYUYV,		// YUYV 输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											  IN int nYUYVPitch,
											  IN BYTE* pSrcBGRA,		// BGRA 输入（存储方式 B G R A B G R A B G R A ……）
											  IN int nSrcPitch,			// 源一行的字节宽度
											  IN int nWidth,			// 源图大小
											  IN int nHeight,
											  IN BOOL bHD,				// 高标清标志
											  IN BOOL bSrcRevert)		// 图像是否行翻转标志
{
	//assert((pDesYUYV != NULL) && (pSrcBGRA != NULL));
	//assert((nYUYVPitch >= (nWidth << 1)) && (nSrcPitch >= (nWidth << 2)));

    color32_to_yuyv_frame(pDesYUYV, nYUYVPitch, pSrcBGRA, nSrcPitch, nWidth, nHeight, bHD, bSrcRevert, TRUE);
}
void BUFFERCS_BGRA2YUYV_Split(OUT BYTE* pDesYUYVF,		// YUYV 第一场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											  IN int nYUYVPitchF,
											  OUT BYTE* pDesYUYVS,		// YUYV 第二场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											  IN int nYUYVPitchS,
											  IN BYTE* pSrcBGRA,		// BGRA 输入（存储方式 B G R A B G R A B G R A ……）
											  IN int nSrcPitch,			// 源一行的字节宽度
											  IN int nWidth,			// 源图大小
											  IN int nHeight,
											  IN BOOL bHD,				// 高标清标志
											  IN BOOL bSrcRevert)		// 图像是否行翻转标志
{
	//assert((pDesYUYVF != NULL) && (pDesYUYVS != NULL) && (pSrcBGRA != NULL));
	//assert((nYUYVPitchF >= (nWidth << 1)) && (nYUYVPitchS >= (nWidth << 1)) && (nSrcPitch >= (nWidth << 2)));

    color32_to_yuyv_split(pDesYUYVF, nYUYVPitchF, pDesYUYVS, nYUYVPitchS, pSrcBGRA, nSrcPitch, nWidth, nHeight, bHD, bSrcRevert, TRUE);
}

void BUFFERCS_YUYV2BGRA_Compose(OUT BYTE* pDesBGRA,		// BGRA 输出（存储方式 B G R A B G R A B G R A ……）（Alpha 直接填 0xff）
												IN int nDesPitch,
												IN BYTE* pSrcYUYVF,		// YUYV 第一场输入（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
												IN int nFirstPitch,
												IN BYTE* pSrcYUYVS,		// YUYV 第二场输入（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
												IN int nSecondPitch,
												IN int nWidth,			// 目标大小
												IN int nHeight,
												IN BOOL bHD)			// 高标清标志
{
	//assert((pDesBGRA != NULL) && (pSrcYUYVF != NULL) && (pSrcYUYVS != NULL));
	//assert((nDesPitch >= (nWidth << 2)) && (nFirstPitch >= (nWidth << 1)) && (nSecondPitch >= (nWidth << 1)));

    yuyv_to_color32_compose(pDesBGRA, nDesPitch, pSrcYUYVF, nFirstPitch, pSrcYUYVS, nSecondPitch, nWidth, nHeight, bHD, TRUE);
}

// YUYV 与 RGB、BGR 之间的转换

void BUFFERCS_RGB2YUYV_Frame(OUT BYTE* pDesYUYV,		// YUYV 输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											 IN int nYUYVPitch,
											 IN BYTE* pSrcRGB,			// RGB 输入（存储方式 R G B R G B R G B ……）
											 IN int nSrcPitch,			// 源一行的字节宽度（必须为 4 的倍数）
											 IN int nWidth,				// 源图大小
											 IN int nHeight,
											 IN BOOL bHD,				// 高标清标志
											 IN BOOL bSrcRevert)		// 图像是否行翻转标志
{
	//assert((pDesYUYV != NULL) && (pSrcRGB != NULL));
	//assert(nYUYVPitch >= ((nWidth + (nWidth & 0x01)) << 1));
	//assert(nSrcPitch >= (nWidth * 3));

    color24_to_yuyv_frame(pDesYUYV, nYUYVPitch, pSrcRGB, nSrcPitch, nWidth, nHeight, bHD, bSrcRevert, FALSE);
}
void BUFFERCS_RGB2YUYV_Split(OUT BYTE* pDesFirst,		// 第一场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											 IN int nFirstPitch,
											 OUT BYTE* pDesSecond,		// 第二场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											 IN int nSecondPitch,
											 IN BYTE* pSrcRGB,			// RGB 输入（存储方式 R G B R G B R G B ……）
											 IN int nSrcPitch,			// 源一行的字节宽度（必须为 4 的倍数）
											 IN int nWidth,				// 源图大小
											 IN int nHeight,
											 IN BOOL bHD,				// 高标清标志
											 IN BOOL bSrcRevert)		// 图像是否行翻转标志
{
	//assert((pDesFirst != NULL) && (pDesSecond != NULL) && (pSrcRGB != NULL));
	//assert(nFirstPitch >= ((nWidth + (nWidth & 0x01)) << 1));
	//assert(nSecondPitch >= ((nWidth + (nWidth & 0x01)) << 1));
	//assert(nSrcPitch >= (nWidth * 3));

    color24_to_yuyv_split(pDesFirst, nFirstPitch, pDesSecond, nSecondPitch, pSrcRGB, nSrcPitch, nWidth, nHeight, bHD, bSrcRevert, FALSE);
}

void BUFFERCS_BGR2YUYV_Frame(OUT BYTE* pDesYUYV,		// YUYV 输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											 IN int nYUYVPitch,
											 IN BYTE* pSrcBGR,			// BGR 输入（存储方式 B G R B G R B G R ……）
											 IN int nSrcPitch,			// 源一行的字节宽度（必须为 4 的倍数）
											 IN int nWidth,				// 源图大小
											 IN int nHeight,
											 IN BOOL bHD,				// 高标清标志
											 IN BOOL bSrcRevert)		// 图像是否行翻转标志
{
    //assert((pDesYUYV != NULL) && (pSrcBGR != NULL));
    //assert(nYUYVPitch >= ((nWidth + (nWidth & 0x01)) << 1));
	//assert(nSrcPitch >= (nWidth * 3));

    color24_to_yuyv_frame(pDesYUYV, nYUYVPitch, pSrcBGR, nSrcPitch, nWidth, nHeight, bHD, bSrcRevert, TRUE);
}
void BUFFERCS_BGR2YUYV_Split(OUT BYTE* pDesFirst,		// 第一场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											 IN int nFirstPitch,
											 OUT BYTE* pDesSecond,		// 第二场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											 IN int nSecondPitch,
											 IN BYTE* pSrcBGR,			// BGR 输入（存储方式 B G R B G R B G R ……）
											 IN int nSrcPitch,			// 源一行的字节宽度（必须为 4 的倍数）
											 IN int nWidth,				// 源图大小
											 IN int nHeight,
											 IN BOOL bHD,				// 高标清标志
											 IN BOOL bSrcRevert)		// 图像是否行翻转标志
{
    //assert((pDesFirst != NULL) && (pDesSecond != NULL) && (pSrcBGR != NULL));
    //assert(nFirstPitch >= ((nWidth + (nWidth & 0x01)) << 1));
    //assert(nSecondPitch >= ((nWidth + (nWidth & 0x01)) << 1));
	//assert(nSrcPitch >= (nWidth * 3));

    color24_to_yuyv_split(pDesFirst, nFirstPitch, pDesSecond, nSecondPitch, pSrcBGR, nSrcPitch, nWidth, nHeight, bHD, bSrcRevert, TRUE);
}

// RGB Alpha 与 RGBA、BGR Alpha 与 BGRA 之间的转换

void BUFFERCS_24_to_32_Frame(OUT BYTE* pDes32,			// 输出（存储方式 X0 X1 X2 A X0 X1 X2 A ……）
											 IN int nDesPitch,
											 IN BYTE* pSrc24,			// 24 位源输入（存储方式 X0 X1 X2 X0 X1 X2 ……）
											 IN int nSrc24Pitch,		// 源一行的字节宽度（必须为 4 的倍数）
											 IN BYTE* pSrcAlpha,		// 源 Alpha 输入（存储方式 A0 A1 A2 A3 ……）（如果输入为 NULL 则直接填 0xff）
											 IN int nSrcAlphaPitch,
											 IN int nWidth,				// 源图大小
											 IN int nHeight,
											 IN BOOL bSrc24Revert,		// 图像是否行翻转标志
											 IN BOOL bSrcAlphaRevert)	// Alpha 是否行翻转标志
{
	//assert((pDes32 != NULL) && (pSrc24 != NULL));
	//assert(nDesPitch >= ((nWidth + (nWidth & 0x01)) << 1));
	//assert(nSrc24Pitch >= (nWidth * 3));
	//assert((pSrcAlpha == NULL) || (nSrcAlphaPitch >= nWidth));

	int nap5 = (nWidth / 5), nmp5 = (nWidth - 5 * nap5);
	if(nmp5  == 0)
	{
		nmp5 = 5;
		if(nap5 > 1)
			nap5 -= 1;
	}
	BYTE* pSrc24Line = (bSrc24Revert) ? (pSrc24 + (nHeight - 1) * nSrc24Pitch) : pSrc24;
	int nRealSrc24Pitch = (bSrc24Revert) ? -nSrc24Pitch : nSrc24Pitch;

	if (pSrcAlpha)      // 有 Alpha 输入
	{
		BYTE* pSrcAlphaLine = (bSrcAlphaRevert) ? (pSrcAlpha + (nHeight - 1) * nSrcAlphaPitch) : pSrcAlpha;
		int nRealSrcAlphaPitch = (bSrcAlphaRevert) ? -nSrcAlphaPitch : nSrcAlphaPitch;
	}
	else    // 无 Alpha 输入则直接填充 0xff
	{
		
		for (int i=0; i<nHeight; i++)
		{
			color24_to_color32(pDes32, pSrc24Line, DEFAULT_ALPHA_VALUE, nap5, nmp5);
			pSrc24Line += nRealSrc24Pitch;
			pDes32 += nDesPitch;
		}
	}
}
void BUFFERCS_24_to_32_Split(OUT BYTE* pDes32First,		// 第一场输出（存储方式 X0 X1 X2 A X0 X1 X2 A ……）
											 IN int nFirstPitch,
											 OUT BYTE* pDes32Second,	// 第二场输出（存储方式 X0 X1 X2 A X0 X1 X2 A ……）
											 IN int nSecondPitch,
											 IN BYTE* pSrc24,			// 24 位源输入（存储方式 X0 X1 X2 X0 X1 X2 ……）
											 IN int nSrc24Pitch,		// 源一行的字节宽度（必须为 4 的倍数）
											 IN BYTE* pSrcAlpha,		// 源 Alpha 输入（存储方式 A0 A1 A2 A3 ……）（如果输入为 NULL 则直接填 0xff）
											 IN int nSrcAlphaPitch,
											 IN int nWidth,				// 源图大小
											 IN int nHeight,
											 IN BOOL bSrc24Revert,		// 图像是否行翻转标志
											 IN BOOL bSrcAlphaRevert)	// Alpha 是否行翻转标志
{
	//assert((pDes32First != NULL) && (pDes32Second != NULL) && (pSrc24 != NULL));
	//assert(nFirstPitch >= ((nWidth + (nWidth & 0x01)) << 1));
	//assert(nSecondPitch >= ((nWidth + (nWidth & 0x01)) << 1));
	//assert(nSrc24Pitch >= (nWidth * 3));
	//assert((pSrcAlpha == NULL) || (nSrcAlphaPitch >= nWidth));

    int nap5 = (nWidth / 5), nmp5 = (nWidth - 5 * nap5);
	if(nmp5  == 0)
	{
		nmp5 = 5;
		if(nap5 > 1)
			nap5 -= 1;
	}
    int nHalfHeight = (nHeight >> 1);
    BYTE* pSrc24Line = (bSrc24Revert) ? (pSrc24 + (nHeight - 1) * nSrc24Pitch) : pSrc24;
    int nRealSrc24Pitch = (bSrc24Revert) ? -nSrc24Pitch : nSrc24Pitch;

    if (pSrcAlpha)      // 有 Alpha 输入
    {
        BYTE* pSrcAlphaLine = (bSrcAlphaRevert) ? (pSrcAlpha + (nHeight - 1) * nSrcAlphaPitch) : pSrcAlpha;
        int nRealSrcAlphaPitch = (bSrcAlphaRevert) ? -nSrcAlphaPitch : nSrcAlphaPitch;
    }
    else    // 无 Alpha 输入则直接填充 0xff
    {
        for (int i=0; i<nHalfHeight; i++)
        {
            // 第一场
            color24_to_color32(pDes32First, pSrc24Line, DEFAULT_ALPHA_VALUE, nap5, nmp5);
            pSrc24Line += nRealSrc24Pitch;
            pDes32First += nFirstPitch;
            // 第二场
            color24_to_color32(pDes32Second, pSrc24Line, DEFAULT_ALPHA_VALUE, nap5, nmp5);
            pSrc24Line += nRealSrc24Pitch;
            pDes32Second += nSecondPitch;
        }
        if (nHeight & 0x01)     // 如果为奇数行，需要特殊处理，第一场照旧，第二场清黑或拷贝
        {
            // 第一场
            color24_to_color32(pDes32First, pSrc24Line, DEFAULT_ALPHA_VALUE, nap5, nmp5);
            // 第二场
            nSecondPitch = min(nFirstPitch, nSecondPitch);      // 拷贝用
            nap5 = (nSecondPitch >> 4), nmp5 = (nSecondPitch & 0x0f);
            //memset_dword_line(pDes32Second, CS_RGBA_BLACK_VALUE, na12, nmp);
            memcpy_line(pDes32Second, pDes32First, nap5, nmp5);
        }
    }
}
// 一行 24 位缓存到 32 位缓存的转换，填充统一的 Alpha 通道
void SYColor24_To_Color32(OUT void* color32,	// *G@A 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……
											  IN void* color24,		// *G@ 输入，存放方式 *0 G0 @0 *1 G1 @1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
											  IN DWORD dwAlpha,		// Alpha 通道值，要求以这种方式提供：0xAA000000
											  IN int nap5,			// 源像素宽度除以 5 的商与余数（正好满足十六字节对齐）
											  IN int nmp5)
{
	if(nmp5  == 0)
	{
		nmp5 = 5;
		if(nap5 > 1)
			nap5 -= 1;
	}
	color24_to_color32(color32, color24, dwAlpha, nap5, nmp5);
}

//////////////////////////////////////////////////////////////////////////
// 文件结束