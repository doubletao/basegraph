//////////////////////////////////////////////////////////////////////////
// 描述：	各个颜色空间之间的转换函数
// 创建：	
//////////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////////
// 根据制式获取转换矩阵
float*	BUFFERCS_GetMatYUVA2RGBA(IN UINT uDefinition);		// 制式
float	BUFFERCS_GetMatYUVA2RGBA_Cell(IN int nRow,			// 行
												IN int nCol,			// 列
												IN UINT uDefinition);	// 制式

float* BUFFERCS_GetMatRGBA2YUVA(IN UINT uDefinition);		// 制式
float	BUFFERCS_GetMatRGBA2YUVA_Cell(IN int nRow,			// 行
												IN int nCol,			// 列
												IN UINT uDefinition);	// 制式
//////////////////////////////////////////////////////////////////////////
// LUVA 与 RGBA 之间的转换
float* BUFFERCS_GetMatLUVA2RGBA();
float* BUFFERCS_GetMatRGBA2LUVA();
//////////////////////////////////////////////////////////////////////////
// 场与帧的转换，会丢掉奇数行的最后一行

void BUFFERCS_ComposeFields(OUT BYTE* pDes,			// 目标
											IN int nDesPitch,
											IN BYTE* pSrcTop,		// 上场源
											IN int nTopPitch,
											IN BYTE* pSrcBottom,	// 下场源
											IN int nBottomPitch,
											IN int nWidth,		// 目标大小
											IN int nHeight,
											IN int nPixelBytes);	// 一个像素的大小

void BUFFERCS_SplitFrame(OUT BYTE* pDesTop,			// 目标上场
										 IN int nTopPitch,
										 OUT BYTE* pDesBottom,		// 目标下场
										 IN int nBottomPitch,
										 IN BYTE* pSrc,				// 源帧
										 IN int nSrcPitch,
										 IN int nWidth,				// 源大小
										 IN int nHeight,
										 IN int nPixelBytes);		// 一个像素的大小
// YUV 内部格式转换 420 <-> 422 <-> 444
void BUFFERCS_420_422_YUVFrame(OUT BYTE* pDes,		// YUYV 输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											   IN int nDesPitch,
											   IN BYTE* pSrcY,	// Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
											   IN int nSrcYPitch,
											   IN BYTE* pSrcU,	// U 输入（存储方式 UU0 UU2 UU4 ……）（两行共用一个）
											   IN int nSrcUPitch,
											   IN BYTE* pSrcV,	// V 输入（存储方式 VV0 VV2 VV4 ……）（两行共用一个）
											   IN int nSrcVPitch,
											   IN int nWidth,		// 源图大小
											   IN int nHeight);
void BUFFERCS_420_422_YUVSplit(OUT BYTE* pDesFirst,	// YUYV 第一场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											   IN int nFirstPitch,
											   OUT BYTE* pDesSecond,	// YUYV 第二场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											   IN int nSecondPitch,
											   IN BYTE* pSrcY,		// Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
											   IN int nSrcYPitch,
											   IN BYTE* pSrcU,		// U 输入（存储方式 UU0 UU2 UU4 ……）（两行共用一个）
											   IN int nSrcUPitch,
											   IN BYTE* pSrcV,		// V 输入（存储方式 VV0 VV2 VV4 ……）（两行共用一个）
											   IN int nSrcVPitch,
											   IN int nWidth,			// 源图大小
											   IN int nHeight);

void BUFFERCS_ComposeYUV422_Frame(OUT BYTE* pDes,		// YUYV 输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
												  IN int nDesPitch,
												  IN BYTE* pSrcY,		// Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
												  IN int nSrcYPitch,
												  IN BYTE* pSrcU,		// U 输入（存储方式 U0 U2 U4 ……）
												  IN int nSrcUPitch,
												  IN BYTE* pSrcV,		// V 输入（存储方式 V0 V2 V4 ……）
												  IN int nSrcVPitch,
												  IN int nWidth,		// 源图大小
												  IN int nHeight);
void BUFFERCS_ComposeYUV422_Split(OUT BYTE* pDesFirst,		// YUYV 第一场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
												  IN int nFirstPitch,
												  OUT BYTE* pDesSecond,	// YUYV 第二场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
												  IN int nSecondPitch,
												  IN BYTE* pSrcY,			// Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
												  IN int nSrcYPitch,
												  IN BYTE* pSrcU,			// U 输入（存储方式 U0 U2 U4 ……）
												  IN int nSrcUPitch,
												  IN BYTE* pSrcV,			// V 输入（存储方式 V0 V2 V4 ……）
												  IN int nSrcVPitch,
												  IN int nWidth,			// 源图大小
												  IN int nHeight);

// YUV 420、422 片存储格式与 RGBA、BGRA 之间的转换

void BUFFERCS_YUV420_RGBA_Frame(OUT BYTE* pDesRGBA,		// RGBA 输出（存储方式 R G B A R G B A R G B A ……）（Alpha 直接填 0xff）
												IN int nDesPitch,
												IN BYTE* pSrcY,		// Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
												IN int nSrcYPitch,
												IN BYTE* pSrcU,		// U 输入（存储方式 UU0 UU2 UU4 ……）（两行共用一个）
												IN int nSrcUPitch,
												IN BYTE* pSrcV,		// V 输入（存储方式 VV0 VV2 VV4 ……）（两行共用一个）
												IN int nSrcVPitch,
												IN int nWidth,		// 目标大小
												IN int nHeight,
												IN BOOL bHD,			// 高标清标志
												IN BOOL bDesRevert);	// 图像是否行翻转标志
void BUFFERCS_YUV420_RGBA_Split(OUT BYTE* pDesFirst,		// RGBA 第一场输出（存储方式 R G B A R G B A R G B A ……）（Alpha 直接填 0xff）
												IN int nFirstPitch,
												OUT BYTE* pDesSecond,	// RGBA 第二场输出（存储方式 R G B A R G B A R G B A ……）（Alpha 直接填 0xff）
												IN int nSecondPitch,
												IN BYTE* pSrcY,		// Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
												IN int nSrcYPitch,
												IN BYTE* pSrcU,		// U 输入（存储方式 UU0 UU2 UU4 ……）（两行共用一个）
												IN int nSrcUPitch,
												IN BYTE* pSrcV,		// V 输入（存储方式 VV0 VV2 VV4 ……）（两行共用一个）
												IN int nSrcVPitch,
												IN int nWidth,		// 目标大小
												IN int nHeight,
												IN BOOL bHD,			// 高标清标志
												IN BOOL bDesRevert);	// 图像是否行翻转标志

void BUFFERCS_YUV422_RGBA_Frame(OUT BYTE* pDesRGBA,		// RGBA 输出（存储方式 R G B A R G B A R G B A ……）（Alpha 直接填 0xff）
												IN int nDesPitch,
												IN BYTE* pSrcY,		// Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
												IN int nSrcYPitch,
												IN BYTE* pSrcU,		// U 输入（存储方式 U0 U2 U4 ……）
												IN int nSrcUPitch,
												IN BYTE* pSrcV,		// V 输入（存储方式 V0 V2 V4 ……）
												IN int nSrcVPitch,
												IN int nWidth,		// 目标大小
												IN int nHeight,
												IN BOOL bHD,		// 高标清标志
												IN BOOL bDesRevert);	// 图像是否行翻转标志
void BUFFERCS_YUV422_RGBA_Split(OUT BYTE* pDesFirst,		// RGBA 第一场输出（存储方式 R G B A R G B A R G B A ……）（Alpha 直接填 0xff）
												IN int nFirstPitch,
												OUT BYTE* pDesSecond,	// RGBA 第二场输出（存储方式 R G B A R G B A R G B A ……）（Alpha 直接填 0xff）
												IN int nSecondPitch,
												IN BYTE* pSrcY,		// Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
												IN int nSrcYPitch,
												IN BYTE* pSrcU,		// U 输入（存储方式 U0 U2 U4 ……）
												IN int nSrcUPitch,
												IN BYTE* pSrcV,		// V 输入（存储方式 V0 V2 V4 ……）
												IN int nSrcVPitch,
												IN int nWidth,	// 目标大小
												IN int nHeight,
												IN BOOL bHD,		// 高标清标志
												IN BOOL bDesRevert);		// 图像是否行翻转标志

void BUFFERCS_YUV420_BGRA_Frame(OUT BYTE* pDesBGRA,		// BGRA 输出（存储方式 B G R A B G R A B G R A ……）（Alpha 直接填 0xff）
												IN int nDesPitch,
												IN BYTE* pSrcY,		// Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
												IN int nSrcYPitch,
												IN BYTE* pSrcU,		// U 输入（存储方式 UU0 UU2 UU4 ……）（两行共用一个）
												IN int nSrcUPitch,
												IN BYTE* pSrcV,		// V 输入（存储方式 VV0 VV2 VV4 ……）（两行共用一个）
												IN int nSrcVPitch,
												IN int nWidth,		// 目标大小
												IN int nHeight,
												IN BOOL bHD,		// 高标清标志
												IN BOOL bDesRevert);	// 图像是否行翻转标志
void BUFFERCS_YUV420_BGRA_Split(OUT BYTE* pDesFirst,		// BGRA 第一场输出（存储方式 B G R A B G R A B G R A ……）（Alpha 直接填 0xff）
												IN int nFirstPitch,
												OUT BYTE* pDesSecond,	// RGBA 第二场输出（存储方式 B G R A B G R A B G R A ……）（Alpha 直接填 0xff）
												IN int nSecondPitch,
												IN BYTE* pSrcY,		// Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
												IN int nSrcYPitch,
												IN BYTE* pSrcU,		// U 输入（存储方式 UU0 UU2 UU4 ……）（两行共用一个）
												IN int nSrcUPitch,
												IN BYTE* pSrcV,		// V 输入（存储方式 VV0 VV2 VV4 ……）（两行共用一个）
												IN int nSrcVPitch,
												IN int nWidth,		// 目标大小
												IN int nHeight,
												IN BOOL bHD,			// 高标清标志
												IN BOOL bDesRevert);	// 图像是否行翻转标志

void BUFFERCS_YUV422_BGRA_Frame(OUT BYTE* pDesBGRA,		// BGRA 输出（存储方式 B G R A B G R A B G R A ……）（Alpha 直接填 0xff）
												IN int nDesPitch,
												IN BYTE* pSrcY,		// Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
												IN int nSrcYPitch,
												IN BYTE* pSrcU,		// U 输入（存储方式 U0 U2 U4 ……）
												IN int nSrcUPitch,
												IN BYTE* pSrcV,		// V 输入（存储方式 V0 V2 V4 ……）
												IN int nSrcVPitch,
												IN int nWidth,		// 目标大小
												IN int nHeight,
												IN BOOL bHD,			// 高标清标志
												IN BOOL bDesRevert);	// 图像是否行翻转标志
void BUFFERCS_YUV422_BGRA_Split(OUT BYTE* pDesFirst,		// BGRA 第一场输出（存储方式 B G R A B G R A B G R A ……）（Alpha 直接填 0xff）
												IN int nFirstPitch,
												OUT BYTE* pDesSecond,	// RGBA 第二场输出（存储方式 B G R A B G R A B G R A ……）（Alpha 直接填 0xff）
												IN int nSecondPitch,
												IN BYTE* pSrcY,		// Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
												IN int nSrcYPitch,
												IN BYTE* pSrcU,		// U 输入（存储方式 U0 U2 U4 ……）
												IN int nSrcUPitch,
												IN BYTE* pSrcV,		// V 输入（存储方式 V0 V2 V4 ……）
												IN int nSrcVPitch,
												IN int nWidth,		// 目标大小
												IN int nHeight,
												IN BOOL bHD,			// 高标清标志
												IN BOOL bDesRevert);	// 图像是否行翻转标志

// YUYVA 与 RGBA、BGRA 之间的转换
void BUFFERCS_RGBA2YUYVA_Frame(OUT BYTE* pDesYUYV,		// YUYV 输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											   IN int nYUYVPitch,
											   OUT BYTE* pDesAlpha,	// Alpha 输出（存储方式 A0 A1 A2 A3 A4 ……）
											   IN int nAlphaPitch,
											   IN BYTE* pSrcRGBA,		// RGBA 输入（存储方式 R G B A R G B A R G B A ……）
											   IN int nSrcPitch,		// 源一行的字节宽度
											   IN int nWidth,			// 源图大小
											   IN int nHeight,
											   IN BOOL bHD,			// 高标清标志
											   IN BOOL bSrcRevert);	// 图像是否行翻转标志
void BUFFERCS_RGBA2YUYVA_Split(OUT BYTE* pDesYUYVF,		// YUYV 第一场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											   IN int nYUYVPitchF,
											   OUT BYTE* pDesAlphaF,	// Alpha 第一场输出（存储方式 A0 A1 A2 A3 A4 ……）
											   IN int nAlphaPitchF,
											   OUT BYTE* pDesYUYVS,	// YUYV 第二场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											   IN int nYUYVPitchS,
											   OUT BYTE* pDesAlphaS,	// Alpha 第二场输出（存储方式 A0 A1 A2 A3 A4 ……）
											   IN int nAlphaPitchS,
											   IN BYTE* pSrcRGBA,		// RGBA 输入（存储方式 R G B A R G B A R G B A ……）
											   IN int nSrcPitch,		// 源一行的字节宽度
											   IN int nWidth,			// 源图大小
											   IN int nHeight,
											   IN BOOL bHD,			// 高标清标志
											   IN BOOL bSrcRevert);	// 图像是否行翻转标志
void BUFFERCS_RGBA2YUYV_Frame(OUT BYTE* pDesYUYV,		// YUYV 输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											  IN int nYUYVPitch,
											  IN BYTE* pSrcRGBA,		// RGBA 输入（存储方式 R G B A R G B A R G B A ……）
											  IN int nSrcPitch,		// 源一行的字节宽度
											  IN int nWidth,			// 源图大小
											  IN int nHeight,
											  IN BOOL bHD,			// 高标清标志
											  IN BOOL bSrcRevert);	// 图像是否行翻转标志
void BUFFERCS_RGBA2YUYV_Split(OUT BYTE* pDesYUYVF,		// YUYV 第一场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											  IN int nYUYVPitchF,
											  OUT BYTE* pDesYUYVS,	// YUYV 第二场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											  IN int nYUYVPitchS,
											  IN BYTE* pSrcRGBA,		// RGBA 输入（存储方式 R G B A R G B A R G B A ……）
											  IN int nSrcPitch,		// 源一行的字节宽度
											  IN int nWidth,			// 源图大小
											  IN int nHeight,
											  IN BOOL bHD,			// 高标清标志
											  IN BOOL bSrcRevert);	// 图像是否行翻转标志

void BUFFERCS_YUYV2RGBA_Compose(OUT BYTE* pDesRGBA,		// RGBA 输出（存储方式 R G B A R G B A R G B A ……）（Alpha 直接填 0xff）
												IN int nDesPitch,
												IN BYTE* pSrcYUYVF,	// YUYV 第一场输入（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
												IN int nFirstPitch,
												IN BYTE* pSrcYUYVS,	// YUYV 第二场输入（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
												IN int nSecondPitch,
												IN int nWidth,		// 目标大小
												IN int nHeight,
												IN BOOL bHD);			// 高标清标志

void BUFFERCS_BGRA2YUYVA_Frame(OUT BYTE* pDesYUYV,		// YUYV 输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											   IN int nYUYVPitch,
											   OUT BYTE* pDesAlpha,	// Alpha 输出（存储方式 A0 A1 A2 A3 A4 ……）
											   IN int nAlphaPitch,
											   IN BYTE* pSrcBGRA,		// BGRA 输入（存储方式 B G R A B G R A B G R A ……）
											   IN int nSrcPitch,		// 源一行的字节宽度
											   IN int nWidth,			// 源图大小
											   IN int nHeight,
											   IN BOOL bHD,				// 高标清标志
											   IN BOOL bSrcRevert);		// 图像是否行翻转标志
void BUFFERCS_BGRA2YUYVA_Split(OUT BYTE* pDesYUYVF,		// YUYV 第一场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											   IN int nYUYVPitchF,
											   OUT BYTE* pDesAlphaF,	// Alpha 第一场输出（存储方式 A0 A1 A2 A3 A4 ……）
											   IN int nAlphaPitchF,
											   OUT BYTE* pDesYUYVS,	// YUYV 第二场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											   IN int nYUYVPitchS,
											   OUT BYTE* pDesAlphaS,	// Alpha 第二场输出（存储方式 A0 A1 A2 A3 A4 ……）
											   IN int nAlphaPitchS,
											   IN BYTE* pSrcBGRA,		// BGRA 输入（存储方式 B G R A B G R A B G R A ……）
											   IN int nSrcPitch,		// 源一行的字节宽度
											   IN int nWidth,			// 源图大小
											   IN int nHeight,
											   IN BOOL bHD,				// 高标清标志
											   IN BOOL bSrcRevert);		// 图像是否行翻转标志
void BUFFERCS_BGRA2YUYV_Frame(OUT BYTE* pDesYUYV,		// YUYV 输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											  IN int nYUYVPitch,
											  IN BYTE* pSrcBGRA,		// BGRA 输入（存储方式 B G R A B G R A B G R A ……）
											  IN int nSrcPitch,		// 源一行的字节宽度
											  IN int nWidth,			// 源图大小
											  IN int nHeight,
											  IN BOOL bHD,			// 高标清标志
											  IN BOOL bSrcRevert);	// 图像是否行翻转标志
void BUFFERCS_BGRA2YUYV_Split(OUT BYTE* pDesYUYVF,		// YUYV 第一场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											  IN int nYUYVPitchF,
											  OUT BYTE* pDesYUYVS,	// YUYV 第二场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											  IN int nYUYVPitchS,
											  IN BYTE* pSrcBGRA,		// BGRA 输入（存储方式 B G R A B G R A B G R A ……）
											  IN int nSrcPitch,			// 源一行的字节宽度
											  IN int nWidth,			// 源图大小
											  IN int nHeight,
											  IN BOOL bHD,			// 高标清标志
											  IN BOOL bSrcRevert);	// 图像是否行翻转标志

void BUFFERCS_YUYV2BGRA_Compose(OUT BYTE* pDesBGRA,		// BGRA 输出（存储方式 B G R A B G R A B G R A ……）（Alpha 直接填 0xff）
												IN int nDesPitch,
												IN BYTE* pSrcYUYVF,	// YUYV 第一场输入（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
												IN int nFirstPitch,
												IN BYTE* pSrcYUYVS,	// YUYV 第二场输入（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
												IN int nSecondPitch,
												IN int nWidth,		// 目标大小
												IN int nHeight,
												IN BOOL bHD);			// 高标清标志

// YUYV 与 RGB、BGR 之间的转换

void BUFFERCS_RGB2YUYV_Frame(OUT BYTE* pDesYUYV,		// YUYV 输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											 IN int nYUYVPitch,
											 IN BYTE* pSrcRGB,	// RGB 输入（存储方式 R G B R G B R G B ……）
											 IN int nSrcPitch,	// 源一行的字节宽度（必须为 4 的倍数）
											 IN int nWidth,		// 源图大小
											 IN int nHeight,
											 IN BOOL bHD,			// 高标清标志
											 IN BOOL bSrcRevert);	// 图像是否行翻转标志
void BUFFERCS_RGB2YUYV_Split(OUT BYTE* pDesFirst,		// 第一场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											 IN int nFirstPitch,
											 OUT BYTE* pDesSecond,	// 第二场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											 IN int nSecondPitch,
											 IN BYTE* pSrcRGB,		// RGB 输入（存储方式 R G B R G B R G B ……）
											 IN int nSrcPitch,	// 源一行的字节宽度（必须为 4 的倍数）
											 IN int nWidth,		// 源图大小
											 IN int nHeight,
											 IN BOOL bHD,			// 高标清标志
											 IN BOOL bSrcRevert);	// 图像是否行翻转标志

void BUFFERCS_BGR2YUYV_Frame(OUT BYTE* pDesYUYV,		// YUYV 输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											 IN int nYUYVPitch,
											 IN BYTE* pSrcBGR,	// BGR 输入（存储方式 B G R B G R B G R ……）
											 IN int nSrcPitch,	// 源一行的字节宽度（必须为 4 的倍数）
											 IN int nWidth,		// 源图大小
											 IN int nHeight,
											 IN BOOL bHD,			// 高标清标志
											 IN BOOL bSrcRevert);	// 图像是否行翻转标志
void BUFFERCS_BGR2YUYV_Split(OUT BYTE* pDesFirst,		// 第一场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											 IN int nFirstPitch,
											 OUT BYTE* pDesSecond,	// 第二场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
											 IN int nSecondPitch,
											 IN BYTE* pSrcBGR,	// BGR 输入（存储方式 B G R B G R B G R ……）
											 IN int nSrcPitch,	// 源一行的字节宽度（必须为 4 的倍数）
											 IN int nWidth,		// 源图大小
											 IN int nHeight,
											 IN BOOL bHD,		// 高标清标志
											 IN BOOL bSrcRevert);	// 图像是否行翻转标志

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
											 IN BOOL bSrcAlphaRevert);	// Alpha 是否行翻转标志
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
											 IN BOOL bSrcAlphaRevert);	// Alpha 是否行翻转标志
// 一行 24 位缓存到 32 位缓存的转换，填充统一的 Alpha 通道
void BaseGraphColor24_To_Color32(OUT void* color32,	// *G@A 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……
											IN void* color24,		// *G@ 输入，存放方式 *0 G0 @0 *1 G1 @1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
											IN DWORD dwAlpha,		// Alpha 通道值，要求以这种方式提供：0xAA000000
											IN int nap5,			// 源像素宽度除以 5 的商与余数（正好满足十六字节对齐）
											IN int nmp5);

//////////////////////////////////////////////////////////////////////////
// 文件结束
