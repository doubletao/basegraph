//////////////////////////////////////////////////////////////////////////
// 描述：采用 SSE2 优化算法
// 创建：
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "BufferSSE2.h"
#include "BufferSSE2Static.h"
#include "BufferColorSpaceConv.h"
#include "BufferRipRender.h"
#include "Common\BufferCommonDef.h"
#include <assert.h>

void InitSSEStaticCoef()
{
	DWORD dwTmpR(0), dwTmpG(0), dwTmpB(0), dwTmpU(0), dwTmpV(0);

	// HD RGB or BGR to YUV
	dwTmpR = (DWORD)(BUFFERCS_GetMatRGBA2YUVA_Cell(0, 0, BUFFER_DEFINITION_HIGH) * COEF_SCALE_16384 + 0.5f);
	dwTmpG = (DWORD)(BUFFERCS_GetMatRGBA2YUVA_Cell(0, 1, BUFFER_DEFINITION_HIGH) * COEF_SCALE_16384 + 0.5f);
	dwTmpB = (DWORD)(BUFFERCS_GetMatRGBA2YUVA_Cell(0, 2, BUFFER_DEFINITION_HIGH) * COEF_SCALE_16384 + 0.5f);
	g_nY32_BR_HD[0] = g_nY32_BR_HD[1] = g_nY32_BR_HD[2] = g_nY32_BR_HD[3] = g_nY24_RGB_HD[2] = (dwTmpR | (dwTmpB << 16));
	g_nY32_G_HD[0] = g_nY32_G_HD[1] = g_nY32_G_HD[2] = g_nY32_G_HD[3] = dwTmpG;
	g_nY32_RB_HD[0] = g_nY32_RB_HD[1] = g_nY32_RB_HD[2] = g_nY32_RB_HD[3] = g_nY24_BGR_HD[2] = (dwTmpB | (dwTmpR << 16));
	g_nY32_BGR_HD[0] = g_nY32_BGR_HD[2] = g_nY24_BGR_HD[1] = (dwTmpR | (dwTmpG << 16));
	g_nY32_BGR_HD[1] = g_nY32_BGR_HD[3] = dwTmpB;
	g_nY32_RGB_HD[1] = g_nY32_RGB_HD[3] = dwTmpR;
	g_nY24_RGB_HD[0] = g_nY24_RGB_HD[3] = ((dwTmpR << 16) | dwTmpG);
	g_nY24_BGR_HD[0] = g_nY24_BGR_HD[3] = ((dwTmpB << 16) | dwTmpG);
	g_nY32_RGB_HD[0] = g_nY32_RGB_HD[2] = g_nY24_RGB_HD[1] = ((dwTmpG << 16) | dwTmpB);

	dwTmpR = ((DWORD)(BUFFERCS_GetMatRGBA2YUVA_Cell(1, 0, BUFFER_DEFINITION_HIGH) * COEF_SCALE_16384 - 0.5f) & 0x0000ffff);
	dwTmpG = (((DWORD)(BUFFERCS_GetMatRGBA2YUVA_Cell(1, 1, BUFFER_DEFINITION_HIGH) * COEF_SCALE_16384 - 0.5f) & 0x0000ffff) << 16);
	dwTmpB = ((DWORD)(BUFFERCS_GetMatRGBA2YUVA_Cell(1, 2, BUFFER_DEFINITION_HIGH) * COEF_SCALE_16384 + 0.5f) & 0x0000ffff);
	g_nU_BGR_HD[0] = g_nU_BGR_HD[2] = g_nUV_BGR_HD[0] = (dwTmpR | dwTmpG);
	g_nU_BGR_HD[1] = g_nU_BGR_HD[3] = g_nUV_BGR_HD[1] = dwTmpB;
	g_nU_RGB_HD[0] = g_nU_RGB_HD[2] = g_nUV_RGB_HD[0] = (dwTmpB | dwTmpG);
	g_nU_RGB_HD[1] = g_nU_RGB_HD[3] = g_nUV_RGB_HD[1] = dwTmpR;

	dwTmpR = ((DWORD)(BUFFERCS_GetMatRGBA2YUVA_Cell(2, 0, BUFFER_DEFINITION_HIGH) * COEF_SCALE_16384 + 0.5f) & 0x0000ffff);
	dwTmpG = (((DWORD)(BUFFERCS_GetMatRGBA2YUVA_Cell(2, 1, BUFFER_DEFINITION_HIGH) * COEF_SCALE_16384 - 0.5f) & 0x0000ffff) << 16);
	dwTmpB = ((DWORD)(BUFFERCS_GetMatRGBA2YUVA_Cell(2, 2, BUFFER_DEFINITION_HIGH) * COEF_SCALE_16384 - 0.5f) & 0x0000ffff);
	g_nV_BGR_HD[0] = g_nV_BGR_HD[2] = g_nUV_BGR_HD[2] = (dwTmpR | dwTmpG);
	g_nV_BGR_HD[1] = g_nV_BGR_HD[3] = g_nUV_BGR_HD[3] = dwTmpB;
	g_nV_RGB_HD[0] = g_nV_RGB_HD[2] = g_nUV_RGB_HD[2] = (dwTmpB | dwTmpG);
	g_nV_RGB_HD[1] = g_nV_RGB_HD[3] = g_nUV_RGB_HD[3] = dwTmpR;

	dwTmpV = ((DWORD)(BUFFERCS_GetMatYUVA2RGBA_Cell(0, 2, BUFFER_DEFINITION_HIGH) * COEF_SCALE_16384 + 0.5f) & 0x0000ffff);
	g_nR_V_HD[0] = g_nR_V_HD[1] = g_nR_V_HD[2] = g_nR_V_HD[3] = (dwTmpV | (dwTmpV << 16));

	dwTmpU = ((DWORD)(BUFFERCS_GetMatYUVA2RGBA_Cell(1, 1, BUFFER_DEFINITION_HIGH) * COEF_SCALE_16384 - 0.5f) & 0x0000ffff);
	dwTmpV = ((DWORD)(BUFFERCS_GetMatYUVA2RGBA_Cell(1, 2, BUFFER_DEFINITION_HIGH) * COEF_SCALE_16384 - 0.5f) & 0x0000ffff);
	g_nG_U_HD[0] = g_nG_U_HD[1] = g_nG_U_HD[2] = g_nG_U_HD[3] = (dwTmpU | (dwTmpU << 16));
	g_nG_V_HD[0] = g_nG_V_HD[1] = g_nG_V_HD[2] = g_nG_V_HD[3] = (dwTmpV | (dwTmpV << 16));

	dwTmpU = ((DWORD)(BUFFERCS_GetMatYUVA2RGBA_Cell(2, 1, BUFFER_DEFINITION_HIGH) * COEF_SCALE_16384 + 0.5f) & 0x0000ffff);
	g_nB_U_HD[0] = g_nB_U_HD[1] = g_nB_U_HD[2] = g_nB_U_HD[3] = (dwTmpU | (dwTmpU << 16));

	//////////////////////////////////////////////////////////////////////////
	// SD RGB or BGR to YUV
	dwTmpR = (DWORD)(BUFFERCS_GetMatRGBA2YUVA_Cell(0, 0, BUFFER_DEFINITION_STANDARD) * COEF_SCALE_16384 + 0.5f);
	dwTmpG = (DWORD)(BUFFERCS_GetMatRGBA2YUVA_Cell(0, 1, BUFFER_DEFINITION_STANDARD) * COEF_SCALE_16384 + 0.5f);
	dwTmpB = (DWORD)(BUFFERCS_GetMatRGBA2YUVA_Cell(0, 2, BUFFER_DEFINITION_STANDARD) * COEF_SCALE_16384 + 0.5f);
	g_nY32_BR_SD[0] = g_nY32_BR_SD[1] = g_nY32_BR_SD[2] = g_nY32_BR_SD[3] = g_nY24_RGB_SD[2] = (dwTmpR | (dwTmpB << 16));
	g_nY32_G_SD[0] = g_nY32_G_SD[1] = g_nY32_G_SD[2] = g_nY32_G_SD[3] = dwTmpG;
	g_nY32_RB_SD[0] = g_nY32_RB_SD[1] = g_nY32_RB_SD[2] = g_nY32_RB_SD[3] = g_nY24_BGR_SD[2] = (dwTmpB | (dwTmpR << 16));
	g_nY32_BGR_SD[0] = g_nY32_BGR_SD[2] = g_nY24_BGR_SD[1] = (dwTmpR | (dwTmpG << 16));
	g_nY32_BGR_SD[1] = g_nY32_BGR_SD[3] = dwTmpB;
	g_nY32_RGB_SD[1] = g_nY32_RGB_SD[3] = dwTmpR;
	g_nY24_RGB_SD[0] = g_nY24_RGB_SD[3] = ((dwTmpR << 16) | dwTmpG);
	g_nY24_BGR_SD[0] = g_nY24_BGR_SD[3] = ((dwTmpB << 16) | dwTmpG);
	g_nY32_RGB_SD[0] = g_nY32_RGB_SD[2] = g_nY24_RGB_SD[1] = ((dwTmpG << 16) | dwTmpB);

	dwTmpR = ((DWORD)(BUFFERCS_GetMatRGBA2YUVA_Cell(1, 0, BUFFER_DEFINITION_STANDARD) * COEF_SCALE_16384 - 0.5f) & 0x0000ffff);
	dwTmpG = (((DWORD)(BUFFERCS_GetMatRGBA2YUVA_Cell(1, 1, BUFFER_DEFINITION_STANDARD) * COEF_SCALE_16384 - 0.5f) & 0x0000ffff) << 16);
	dwTmpB = ((DWORD)(BUFFERCS_GetMatRGBA2YUVA_Cell(1, 2, BUFFER_DEFINITION_STANDARD) * COEF_SCALE_16384 + 0.5f) & 0x0000ffff);
	g_nU_BGR_SD[0] = g_nU_BGR_SD[2] = g_nUV_BGR_SD[0] = (dwTmpR | dwTmpG);
	g_nU_BGR_SD[1] = g_nU_BGR_SD[3] = g_nUV_BGR_SD[1] = dwTmpB;
	g_nU_RGB_SD[0] = g_nU_RGB_SD[2] = g_nUV_RGB_SD[2] = (dwTmpB | dwTmpG);
	g_nU_RGB_SD[1] = g_nU_RGB_SD[3] = g_nUV_RGB_SD[3] = dwTmpR;

	dwTmpR = ((DWORD)(BUFFERCS_GetMatRGBA2YUVA_Cell(2, 0, BUFFER_DEFINITION_STANDARD) * COEF_SCALE_16384 + 0.5f) & 0x0000ffff);
	dwTmpG = (((DWORD)(BUFFERCS_GetMatRGBA2YUVA_Cell(2, 1, BUFFER_DEFINITION_STANDARD) * COEF_SCALE_16384 - 0.5f) & 0x0000ffff) << 16);
	dwTmpB = ((DWORD)(BUFFERCS_GetMatRGBA2YUVA_Cell(2, 2, BUFFER_DEFINITION_STANDARD) * COEF_SCALE_16384 - 0.5f) & 0x0000ffff);
	g_nV_BGR_SD[0] = g_nV_BGR_SD[2] = g_nUV_BGR_SD[2] = (dwTmpR | dwTmpG);
	g_nV_BGR_SD[1] = g_nV_BGR_SD[3] = g_nUV_BGR_SD[3] = dwTmpB;
	g_nV_RGB_SD[0] = g_nV_RGB_SD[2] = g_nUV_RGB_SD[2] = (dwTmpB | dwTmpG);
	g_nV_RGB_SD[1] = g_nV_RGB_SD[3] = g_nUV_RGB_SD[3] = dwTmpR;

	dwTmpV = ((DWORD)(BUFFERCS_GetMatYUVA2RGBA_Cell(0, 2, BUFFER_DEFINITION_STANDARD) * COEF_SCALE_16384 + 0.5f) & 0x0000ffff);
	g_nR_V_SD[0] = g_nR_V_SD[1] = g_nR_V_SD[2] = g_nR_V_SD[3] = (dwTmpV | (dwTmpV << 16));

	dwTmpU = ((DWORD)(BUFFERCS_GetMatYUVA2RGBA_Cell(1, 1, BUFFER_DEFINITION_STANDARD) * COEF_SCALE_16384 - 0.5f) & 0x0000ffff);
	dwTmpV = ((DWORD)(BUFFERCS_GetMatYUVA2RGBA_Cell(1, 2, BUFFER_DEFINITION_STANDARD) * COEF_SCALE_16384 - 0.5f) & 0x0000ffff);
	g_nG_U_SD[0] = g_nG_U_SD[1] = g_nG_U_SD[2] = g_nG_U_SD[3] = (dwTmpU | (dwTmpU << 16));
	g_nG_V_SD[0] = g_nG_V_SD[1] = g_nG_V_SD[2] = g_nG_V_SD[3] = (dwTmpV | (dwTmpV << 16));

	dwTmpU = ((DWORD)(BUFFERCS_GetMatYUVA2RGBA_Cell(2, 1, BUFFER_DEFINITION_STANDARD) * COEF_SCALE_16384 + 0.5f) & 0x0000ffff);
	g_nB_U_SD[0] = g_nB_U_SD[1] = g_nB_U_SD[2] = g_nB_U_SD[3] = (dwTmpU | (dwTmpU << 16));
}

//////////////////////////////////////////////////////////////////////////
// 拷贝一行 Buffer
//////////////////////////////////////////////////////////////////////////
void _memcpy_line_align(OUT void *des,		// 目标
				 IN const void *src,		// 源
				 IN int na16,		// 宽度除以 16 的商
				 IN int nm16)		// 宽度除以 16 的余数
{
	_memcpy_line_align_x64(des,src,na16,nm16);		
}
void memcpy_line(OUT void *des,		// 目标
				 IN void *src,		// 源
				 IN int na16,		// 宽度除以 16 的商
				 IN int nm16)		// 宽度除以 16 的余数
{
	LONGLONG desAdd = (LONGLONG )des;
	LONGLONG srcAdd = (LONGLONG )src;
	LONGLONG isAlign = (desAdd &0xf )|| (srcAdd & 0xf);
	if (!isAlign)
	{
		_memcpy_line_align(des,src,na16,nm16);
	}

	_memcpy_line_x64( des,src,na16,nm16);
}

//////////////////////////////////////////////////////////////////////////
// 反向拷贝一行 Buffer
//////////////////////////////////////////////////////////////////////////
// 按字节翻转
void memcpy_rev_char(OUT void* des,		// 目标
					 IN void* src,		// 源
					 IN int nlen,		// 宽度（用以 seek 到最后一段起始点）
					 IN int na16,		// 宽度除以 16 的商
					 IN int nm16)		// 宽度除以 16 的余数
{
	_memcpy_rev_char_x64(des,src,nlen,na16,nm16);
}
// 按双字翻转
void memcpy_rev_dword(OUT void* des,	// 目标
					  IN void* src,		// 源
					  IN int nlen,		// 宽度（用以 seek 到最后一段起始点）
					  IN int na16,		// 宽度除以 16 的商
					  IN int nm16)		// 宽度除以 16 的余数
{
	_memcpy_rev_dword_x64(des,src,nlen,na16,nm16);
}

//////////////////////////////////////////////////////////////////////////
// 拷贝一列 Buffer
// 目标指针的前进方向由 npitch 的符号决定
//////////////////////////////////////////////////////////////////////////
void memcpy_dword_col(OUT void* des,	// 目标起点
					  IN void* src,		// 源
					  IN int npitch,	// 目标一行的宽度（符号决定目标指针的前进方向）
					  IN int na16,		// 拷贝列高度乘于 4 后除以 16 的商
					  IN int nm16)		// 拷贝列高度乘于 4 后除以 16 的余数
{
	_memcpy_dword_col_x64(des,src,npitch,na16,nm16);
}

//////////////////////////////////////////////////////////////////////////
// 以字节为单位设置一行 Buffer
//////////////////////////////////////////////////////////////////////////
void memset_char_line(OUT void *des,	// 目标
					  IN char value,	// 设置值
					  IN int na16,		// 宽度除以 16 的商
					  IN int nm16)		// 宽度除以 16 的余数
{
	_memset_char_line_x64(des,value,na16,nm16);
}

//////////////////////////////////////////////////////////////////////////
// 以双字为单位设置一行 Buffer
//////////////////////////////////////////////////////////////////////////
void memset_dword_line(OUT void *des,	// 目标
					   IN DWORD value,	// 设置值
					   IN int na16,		// 宽度除以 16 的商
					   IN int nm16)		// 宽度除以 16 的余数
{
	_memset_dword_line_x64( des, value, na16,nm16);
}
void copy_dword_line_rgb_mula(OUT void *des,	// 目标
								IN void *src,	// 设置值
								IN int na16,		// 宽度除以 16 的商
								IN int nm16)		// 宽度除以 16 的余数
{
	copy_dword_line_rgb_mula_x64(des,src,na16,nm16);
}


void copy_dword_line_mula(OUT void *des,	// 目标
							  IN void *src,	// 设置值
							  IN int na16,		// 宽度除以 16 的商
							  IN int nm16)		// 宽度除以 16 的余数
{
	copy_dword_line_mula_x64(des,src,na16,nm16);
}

void copy_dword_line_mula_invert(OUT void *des,	// 目标
						  IN void *src,	// 设置值
						  IN int na16,		// 宽度除以 16 的商
						  IN int nm16)		// 宽度除以 16 的余数
{
	copy_dword_line_mula_invert_x64(des,src,na16,nm16);
}

void memset_dword_line_rgb_mula(OUT void *des,	// 目标
						   IN DWORD value,	// 设置值
						   IN int na16,		// 宽度除以 16 的商
						   IN int nm16)		// 宽度除以 16 的余数
{
	memset_dword_line_rgb_mula_x64(des,value,na16,nm16);
}



//////////////////////////////////////////////////////////////////////////
// 以双字为单位设置一行 Buffer
//////////////////////////////////////////////////////////////////////////
void memset_dword_line_rgb(OUT void *des,	// 目标
					   IN DWORD value,	// 设置值
					   IN int na16,		// 宽度除以 16 的商
					   IN int nm16)		// 宽度除以 16 的余数
{
	memset_dword_line_rgb_x64(des,value,na16,nm16);
}

//////////////////////////////////////////////////////////////////////////
// 一行 Buffer 的Alpha值替换操作（包含掩码颜色的使用）
//////////////////////////////////////////////////////////////////////////
void memset_alpha_line_all(IN OUT void *des,	// 目标
						   IN char alpha,		// 填充值
						   IN int na16,			// 宽度除以 16 的商
						   IN int nm16)			// 宽度除以 16 的余数
{
	memset_alpha_line_all_x64( des,alpha,na16,nm16);
}
void memset_alpha_line_mask(IN OUT void *des,	// 目标
							IN char alpha,		// 填充值
							IN DWORD color,		// 只填充颜色值
							IN int na16,		// 宽度除以 16 的商
							IN int nm16)		// 宽度除以 16 的余数
{
	memset_alpha_line_mask_x64(des,	alpha,color,na16,nm16);
}
void memset_alpha_line_notmask(IN OUT void *des,	// 目标
							   IN char alpha,		// 填充值
							   IN DWORD notcolor,	// 不填充颜色值
							   IN int na16,			// 宽度除以 16 的商
							   IN int nm16)			// 宽度除以 16 的余数
{
	memset_alpha_line_notmask_x64(des,alpha,notcolor,na16,nm16);
}

//////////////////////////////////////////////////////////////////////////
// 以字节为单位翻转一行 Buffer
//////////////////////////////////////////////////////////////////////////
void memrev_char_line(IN OUT void *head,	// 缓存起始位置
					  IN OUT void *end,		// 缓存结束位置（end = head + nlen）
					  IN int na16,			// 一半宽度除以 16 的商
					  IN int nm16)			// 一半宽度除以 16 的余数
{
	memrev_char_line_x64(head,end,na16,nm16);
}

//////////////////////////////////////////////////////////////////////////
// 以双字为单位翻转一行 Buffer
//////////////////////////////////////////////////////////////////////////
void memrev_dword_line(IN OUT void *head,	// 缓存起始位置
					   IN OUT void *end,	// 缓存结束位置（end = head + nlen）
					   IN int na16,			// 一半像素宽度的字节宽度除以 16 的商
					   IN int nm16)			// 一半像素宽度的字节宽度除以 16 的余数
{
	memrev_dword_line_x64(head,end,na16,nm16);
}

//////////////////////////////////////////////////////////////////////////
// 对一行 BGRA32 Buffer 进行 Alpha 混色
// 优化计算透明度 MA = SA + DA - SA * DA / 255 = SA + DA - (DA * SA + 255) >> 8
// 查表得到增益 KA = SA / MA = SA * 255 / (SA * 255 + DA * 255 - SA * DA)
// 优化计算颜色值 MC = (SC * SA + DC * (1 - SA / 255) * DA) / MA = (SC - DC) * KA + DC
// 增益表的偏移地址 Offset = (SA * 256 + DA) * 4
//////////////////////////////////////////////////////////////////////////
void bgra32_line_mix(IN OUT void *des,		// 目标
					 IN void *src,			// 源
					 IN int na16,			// 宽度除以 16 的商
					 IN int nm16,			// 宽度除以 16 的余数
					 IN void *ka_table)		// 256 * 256 * sizeof(DWORD) 的 KA 表
{
	bgra32_line_mix_x64(des,src,na16,nm16,ka_table);		
}

//////////////////////////////////////////////////////////////////////////
// 对一行 BGRA32 Buffer 进行 Alpha 反混色
// 优化计算透明度 MA = (255 - SA) + DA - (255 - SA) * DA / 255 = 255 - SA + (DA * SA + 255) >> 8
// 查表得到增益 KA = (255 - SA) / MA = (65535 - SA * 255) / (65535 - SA * 255 + SA * DA)
// 优化计算颜色值 MC = (SC * (255 - SA) + DC * (1 - (255 - SA) / 255) * DA) / MA = (SC - DC) * KA + DC
// 增益表的偏移地址 Offset = ((255 - SA) * 256 + DA) * 4
//////////////////////////////////////////////////////////////////////////
void bgra32_line_mix_notsrc(IN OUT void *des,		// 目标
							IN void *src,			// 源
							IN int na16,			// 宽度除以 16 的商
							IN int nm16,			// 宽度除以 16 的余数
							IN void *ka_table)		// 256 * 256 * sizeof(DWORD) 的 KA 表
{
	bgra32_line_mix_notsrc_x64(des,src,na16,nm16,ka_table);
}

//////////////////////////////////////////////////////////////////////////
// 对一行 BGRA32 Buffer 进行快速 Alpha 混色
// 优化计算
//	SRC * SA + DES * (1 - SA) = (SRC - DES) * SA + DES
//	SRC * (1 - SA) + DES * SA = SRC + (DES - SRC) * SA
//////////////////////////////////////////////////////////////////////////
void bgra32_line_alpha_blend(IN OUT void *des,		// 目标
							 IN void *src,			// 源
							 IN int na16,			// 宽度除以 16 的商
							 IN int nm16)			// 宽度除以 16 的余数
{
	bgra32_line_alpha_blend_x64(des,src,na16,nm16);
}
void bgra32_line_alpha_blend_not(IN OUT void *des,		// 目标
								 IN void *src,			// 源
								 IN int na16,			// 宽度除以 16 的商
								 IN int nm16)			// 宽度除以 16 的余数
{
	bgra32_line_alpha_blend_not_x64(des,src,na16,nm16);
}

//////////////////////////////////////////////////////////////////////////
// 对一行 BGRA32 Buffer 进行线性减淡与加深（颜色通道加入 Alpha 通道的影响）
// 优化计算透明度 MA = SA + DA - SA * DA / 255 = SA + DA - (DA * SA + 255) >> 8
// 优化计算颜色值
//	减淡：MC = (SC * SA + DC * DA) / MA = SC * SA / MA + DC * DA / MA = SC * SKA + DC * DKA
//	加深：MC -= 255
// 查表得到增益 SKA = SA / MA = 255 * SA / (SA * 255 + DA * 255 - SA * DA)
// 查表得到增益 DKA = DA / MA = 255 * DA / (SA * 255 + DA * 255 - SA * DA)
// 增益表的偏移地址 Offset_S = (SA * 256 + DA) * 4    Offset_D = (DA * 256 + SA) * 4
//////////////////////////////////////////////////////////////////////////
void bgra32_line_linear_dodge(IN OUT void *des,		// 目标
							  IN void *src,			// 源
							  IN int na16,			// 宽度除以 16 的商
							  IN int nm16,			// 宽度除以 16 的余数
							  IN void *ka_table)	// 256 * 256 * sizeof(DWORD) 的 KA 表
{
	bgra32_line_linear_dodge_x64(des,src,na16,nm16,ka_table);
}
void bgra32_line_linear_burn(IN OUT void *des,		// 目标
							 IN void *src,			// 源
							 IN int na16,			// 宽度除以 16 的商
							 IN int nm16,			// 宽度除以 16 的余数
							 IN void *ka_table)		// 256 * 256 * sizeof(DWORD) 的 KA 表
{
	bgra32_line_linear_burn_x64(des,src,na16,nm16,ka_table);
}

//////////////////////////////////////////////////////////////////////////
// 对一行 BGRA32 Buffer 进行带权重的填充，每个像素点的权重都不一样
// 权重被放大 14 位，差被放大 7 位，相乘之后还放大 5 位，则分量也放大 5 位
// 计算公式为：des = c0 * f + c1 * (1 - f) = (c0 - c1) * f + c1
//////////////////////////////////////////////////////////////////////////
void bgra32_powerfill(OUT void* des,		// 目标
					  IN DWORD c0,			// 起点颜色
					  IN DWORD c1,			// 终点颜色
					  IN short* power,		// 权重
					  IN int na32,			// 宽度除以 32 的商
					  IN int nm32)			// 宽度除以 32 的余数
{
	bgra32_powerfill_x64(des,c0,c1,power,na32,nm32);
}
// 不需要权重表的快速填充，实时计算权重
void bgra32_powerfill_q(OUT void* des,		// 目标
						IN DWORD c0,		// 起点颜色
						IN DWORD c1,		// 终点颜色
						IN float fPrev,		// 起点前一个权重
						IN float fDelta,	// 像素间的权重增量
						IN int na16,		// 宽度除以 16 的商（一次做 4 个像素）
						IN int nm16)		// 宽度除以 16 的余数
{
	bgra32_powerfill_q_x64(des,c0,c1,fPrev,fDelta,na16,nm16);
}
// 采用递归法对浮点颜色进行插值
// 公式 f(x) = A * (c1 - c0) * (x - B) + C
// f(x + 1) = f(x) + A * (c1 - c0) = f(x) + Delta
void bgra32_linear_recursive(OUT void* des,			// 目标
							 IN float* pfCr0,		// 起点颜色
							 IN float* pfCr1,		// 终点颜色
							 IN float fX,			// X 的起始点（像素索引）
							 IN float fCoefA,		// 系数 A
							 IN float fCoefB,		// 系数 B
							 IN float* pfCoefC,		// 系数 C
							 IN int na16,			// 宽度除以 16 的商（一次做 4 个像素）
							 IN int nm16)			// 宽度除以 16 的余数
{
	bgra32_linear_recursive_x64(des,pfCr0,pfCr1,fX,fCoefA,fCoefB,pfCoefC,na16,nm16);
}

//////////////////////////////////////////////////////////////////////////
// 采用双线性插值法对 BGRA32 Buffer 进行缩放，使用权重与索引映射表进行逐行逐个像素处
// 通过索引映射表得到目标行像素映射到源行的起始位置，利用以它为首的四个相邻像素进行加权插值
// 计算公式		S00 * x * y				S10 * (1 - x) * y
//				S01 * x * (1 - y)		S11 * (1 - x) * (1 - y)
// D = S11 + (S00 - S10 - S01 + S11) * x * y + (S01 - S11) * x + (S10 - S11) * y
//////////////////////////////////////////////////////////////////////////
void bgra32_bl_copy_line(OUT void* des,		// 目标拷贝行
						 IN int dcw,		// 目标拷贝行的宽度
						 IN void* src,		// 源拷贝行
						 IN int src_pitch,	// 源图一行的字节数
						 IN void* pPowerX,	// 目标映射到源的 X 方向上的列权重
						 IN void* pMapX,	// 目标映射到源的 X 方向上的列索引
						 IN DWORD powerY)	// 目标行映射到源行的行权重
{
	bgra32_bl_copy_line_x64(des,dcw,src,src_pitch,pPowerX,pMapX,powerY)	;
}

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
							IN int nMapY)			// 目标一行映射到源的行数
{
	bgra32_mean_copy_add_y_x64(des,src,scw,src_pitch,pPowerY,pCoefH,nMapY);
}

// （缩放倍数小于255的高效算法）
void bgra32_mean_copy_add_y_quick(OUT void* des,		// 临时缓存行（是源行拷贝宽度的四倍）
								  IN void* src,			// 源拷贝起始行
								  IN int scw,			// 源拷贝宽度
								  IN int src_pitch,		// 源行字节数
								  IN void* pPowerY,		// 目标某行映射的垂直方向上的权重表
								  IN void* pCoefH,		// 垂直方向上的缩放系数
								  IN int nMapY)			// 目标一行映射到源的行数
{
	bgra32_mean_copy_add_y_quick_x64(des,src,scw,src_pitch,pPowerY,pCoefH,nMapY);
}

void bgra32_mean_copy_add_x(OUT void* des,
							IN int dcw,
							IN void* src,
							IN void* pPowerX,
							IN void* pMapX,
							IN void* pCoefW)
{
	bgra32_mean_copy_add_x_x64(des,dcw,src,pPowerX,pMapX,pCoefW);
}

//////////////////////////////////////////////////////////////////////////
// 对一行 32 位内存的 Alpha 通道进行缩放
//////////////////////////////////////////////////////////////////////////
void bgra32_scale_alpha_line(IN OUT void *des,	// 目标
							 IN char alpha,		// 缩放值
							 IN int na16,		// 宽度除以 16 的商
							 IN int nm16)		// 宽度除以 16 的余数
{
	bgra32_scale_alpha_line_x64(des,alpha,na16,nm16);
}

//////////////////////////////////////////////////////////////////////////
// 对一行 32 位内存进行 Alpha 通道替换操作
//////////////////////////////////////////////////////////////////////////
void bgra32_replace_alpha_line(IN OUT void *des,	// 目标
							   IN void* src,		// 一行 8 位的键值
							   IN int na16,			// 宽度除以 16 的商
							   IN int nm16)			// 宽度除以 16 的余数
{
	bgra32_replace_alpha_line_x64(des,src,na16,nm16);
}

//////////////////////////////////////////////////////////////////////////
// 对一行 32 位内存进行键值操作
//////////////////////////////////////////////////////////////////////////
void bgra32_key_mul_line(IN OUT void* des,	// 目标
						 IN void* src,		// 一行 8 位的键值
						 IN int na16,		// 宽度除以 16 的商
						 IN int nm16)		// 宽度除以 16 的余数
{
	bgra32_key_mul_line_x64(des,src,na16,nm16);
}
void bgra32_key_mul_notsrc_line(IN OUT void* des,	// 目标
								IN void* src,		// 一行 8 位的键值
								IN int na16,		// 宽度除以 16 的商
								IN int nm16)		// 宽度除以 16 的余数
{
	bgra32_key_mul_notsrc_line_x64(des,src,na16,nm16);
}
void bgra32_key_mul_notdes_line(IN OUT void* des,	// 目标
								IN void* src,		// 一行 8 位的键值
								IN int na16,		// 宽度除以 16 的商
								IN int nm16)		// 宽度除以 16 的余数
{
	bgra32_key_mul_notdes_line_x64(des,src,na16,nm16);
}
void bgra32_key_mix_line(IN OUT void* des,	// 目标
						 IN void* src,		// 一行 8 位的键值
						 IN int na16,		// 宽度除以 16 的商
						 IN int nm16)		// 宽度除以 16 的余数
{
	bgra32_key_mix_line_x64(des,src,na16,nm16);
}
void bgra32_key_mix_notsrc_line(IN OUT void* des,	// 目标
								IN void* src,		// 一行 8 位的键值
								IN int na16,		// 宽度除以 16 的商
								IN int nm16)		// 宽度除以 16 的余数
{
	bgra32_key_mix_notsrc_line_x64(des,src,na16,nm16);
}
void bgra32_key_sat_sub_line(IN OUT void* des,	// 目标
							 IN void* src,		// 一行 8 位的键值
							 IN int na16,		// 宽度除以 16 的商
							 IN int nm16)		// 宽度除以 16 的余数
{
	bgra32_key_sat_sub_line_x64(des,src,na16,nm16);
}

//////////////////////////////////////////////////////////////////////////
// 对一行 8 位键内存进行标量操作
//////////////////////////////////////////////////////////////////////////
void keybuf_mul_char_line(IN OUT void* des,		// 一行 8 位键目标
						  IN char alpha,		// 标量值
						  IN int na16,			// 宽度除以 16 的商
						  IN int nm16)			// 宽度除以 16 的余数
{
	keybuf_mul_char_line_x64(des,alpha,na16,nm16);
}
void keybuf_mul_char_line_notdes(IN OUT void* des,		// 一行 8 位键目标
								 IN char alpha,			// 标量值
								 IN int na16,			// 宽度除以 16 的商
								 IN int nm16)			// 宽度除以 16 的余数
{
	keybuf_mul_char_line_notdes_x64(des,alpha,na16,nm16);
}
void keybuf_mix_char_line(IN OUT void* des,		// 一行 8 位键目标
						  IN char alpha,		// 标量值
						  IN int na16,			// 宽度除以 16 的商
						  IN int nm16)			// 宽度除以 16 的余数
{
	keybuf_mix_char_line_x64(des,alpha,na16,nm16);
}

//////////////////////////////////////////////////////////////////////////
// 对两行 8 位键内存进行操作
//////////////////////////////////////////////////////////////////////////
void keybuf_mul_buf_line(IN OUT void* des,		// 一行 8 位目标键
						 IN void* src,			// 一行 8 位源键
						 IN int na16,			// 宽度除以 16 的商
						 IN int nm16)			// 宽度除以 16 的余数
{
	keybuf_mul_buf_line_x64(des,src,na16,nm16);
}
void keybuf_mul_buf_line_notsrc(IN OUT void* des,		// 一行 8 位目标键
								IN void* src,			// 一行 8 位源键
								IN int na16,			// 宽度除以 16 的商
								IN int nm16)			// 宽度除以 16 的余数
{
	keybuf_mul_buf_line_notsrc_x64(des,src,na16,nm16);
}
void keybuf_mul_buf_line_notdes(IN OUT void* des,		// 一行 8 位目标键
								IN void* src,			// 一行 8 位源键
								IN int na16,			// 宽度除以 16 的商
								IN int nm16)			// 宽度除以 16 的余数
{
	keybuf_mul_buf_line_notdes_x64(des,src,na16,nm16);
}
void keybuf_mix_buf_line(IN OUT void* des,		// 一行 8 位目标键
						 IN void* src,			// 一行 8 位源键
						 IN int na16,			// 宽度除以 16 的商
						 IN int nm16)			// 宽度除以 16 的余数
{
	keybuf_mix_buf_line_x64(des,src,na16,nm16);
}
void keybuf_mix_buf_line_notsrc(IN OUT void* des,		// 一行 8 位目标键
								IN void* src,			// 一行 8 位源键
								IN int na16,			// 宽度除以 16 的商
								IN int nm16)			// 宽度除以 16 的余数
{
	keybuf_mix_buf_line_notsrc_x64(des,src,na16,nm16);
}
void keybuf_sat_sub_buf_line(IN OUT void* des,		// 一行 8 位目标键
							 IN void* src,			// 一行 8 位源键
							 IN int na16,			// 宽度除以 16 的商
							 IN int nm16)			// 宽度除以 16 的余数
{
	keybuf_sat_sub_buf_line_x64(des,src,na16,nm16);
}

//////////////////////////////////////////////////////////////////////////
// 对一行 8 位键内存进行双线性插值算法操作
// 通过索引映射表得到目标行像素映射到源行的起始位置，利用以它为首的四个相邻像素进行加权插值
// 计算公式		S00 * x * y				S10 * (1 - x) * y
//				S01 * x * (1 - y)		S11 * (1 - x) * (1 - y)
// D = S11 + (S00 - S10 - S01 + S11) * x * y + (S01 - S11) * x + (S10 - S11) * y
//////////////////////////////////////////////////////////////////////////
void keybuf_bl_copy_line(OUT void* des,		// 目标拷贝行
						 IN int na4,		// 目标拷贝行的宽度除以 4 的商
						 IN int nm4,		// 目标拷贝行的宽度除以 4 的余数
						 IN void* src,		// 源拷贝行
						 IN int src_pitch,	// 源图一行的字节数
						 IN void* pPowerX,	// 目标映射到源的 X 方向上的列权重
						 IN void* pMapX,	// 目标映射到源的 X 方向上的列索引
						 IN DWORD powerY)	// 目标行映射到源行的行权重
{
	keybuf_bl_copy_line_x64(des,na4,nm4,src,src_pitch,pPowerX,pMapX,powerY);
}

//////////////////////////////////////////////////////////////////////////
// 计算一等差序列数与某一特定数的完全平方和（特定数为负则计算完全平方差）
//////////////////////////////////////////////////////////////////////////
void math_serial_perfect_square(OUT void* des,		// 结果行
								IN float fStart,	// 序列起始数
								IN float fStep,		// 等差
								IN float fGiven,	// 特定数
								IN int na16,		// 行字节宽度除以 16 的商与余数（一次做四个元素）
								IN int nm16)
{
	math_serial_perfect_square_x64(des,fStart,fStep,fGiven,na16,nm16);
}
// 增加额外功能，对和进行线性缩放，并计算四次方结果
void math_serial_perfect_square_ex(OUT void* square,		// 完全平方和结果行
								   OUT void* quartic,		// 四次方结果行
								   OUT void* sum,			// 和的线性缩放结果行
								   IN float fStart,			// 序列起始数
								   IN float fStep,			// 等差
								   IN float fGiven,			// 特定数
								   IN float fScaling,		// 和的线性缩放比
								   IN int na16,				// 行字节宽度除以 16 的商与余数（一次做四个元素）
								   IN int nm16)
{
	math_serial_perfect_square_ex_x64(square,quartic,sum,fStart,fStep,fGiven,fScaling,na16,nm16);
}

//////////////////////////////////////////////////////////////////////////
// 计算一非负序列进行特定偏移后的平方根，根据表被放大的系数计算出索引，并查表
//////////////////////////////////////////////////////////////////////////
void math_offset_sqrt_index_lookup(OUT void* des,		// 查表结果（32 位表，要求与表的类型相同）
								   IN float* pSerial,	// 序列
								   IN float fOffset,	// 特定偏移
								   IN void* pTable,		// 表（32 位表）
								   IN float fScaling,	// 表的大小相对于序列长度的放大倍数
								   IN int na16,			// 行字节宽度除以 16 的商与余数（一次做四个元素）
								   IN int nm16)
{
	math_offset_sqrt_index_lookup_x64(des,pSerial,fOffset,pTable,fScaling,na16,nm16);
}

//////////////////////////////////////////////////////////////////////////
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
// (315°,360°) 范围内则令 Cy = 2 * PI * R 并使用 (y - yc)
// 为了最大化加速对 (x - xc)^4、(x - xc)^2、(x - xc) * R、(y - yc)^4、(y - yc)^2、(y - yc) * R 进行缓存
//////////////////////////////////////////////////////////////////////////
void math_tyler_arc_index_lookup_x(OUT void* des,			// 查表结果（32 位表，要求与表的类型相同）
								   IN float* pQuartic,		// 四次方序列
								   IN float* pSquare,		// 完全平方和序列
								   IN float* pSum,			// 和序列
								   IN float fOffsetQ,		// (y - yc)^4
								   IN float fOffsetSq,		// (y - yc)^2
								   IN void* pTable,			// 表（32 位表）
								   IN float fConst,			// 计算式最后添加的常量（定积分的起点值）
								   IN int na16,				// 行字节宽度除以 16 的商与余数（一次做四个元素）
								   IN int nm16)
{
	math_tyler_arc_index_lookup_x_x64(des,pQuartic,pSquare,pSum,fOffsetQ,fOffsetSq,pTable,fConst,na16,nm16);
}
void math_tyler_arc_index_lookup_y(OUT void* des,			// 查表结果（32 位表，要求与表的类型相同）
								   IN float* pQuartic,		// 四次方序列
								   IN float* pSquare,		// 完全平方和序列
								   IN float fOffsetQ,		// (y - yc)^4
								   IN float fOffsetSq,		// (y - yc)^2
								   IN float fSum,			// R * (y - yc)
								   IN void* pTable,			// 表（32 位表）
								   IN float fConst,			// 计算式最后添加的常量（定积分的起点值）
								   IN int na16,				// 行字节宽度除以 16 的商与余数（一次做四个元素）
								   IN int nm16)
{
	math_tyler_arc_index_lookup_y_x64(des,pQuartic,pSquare,fOffsetQ,fOffsetSq,fSum,pTable,fConst,na16,nm16);
}

//////////////////////////////////////////////////////////////////////////
// 翻转符号，将负值变正值或反过来（当输入与输出指针相同时，认为直接翻转）
//////////////////////////////////////////////////////////////////////////
void math_negtive_float(OUT float* des,		// 目标
						IN float* src,		// 源
						IN int na16,		// 行字节宽度除以 16 的商与余数（一次做四个元素）
						IN int nm16)
{
	math_negtive_float_x64(des,src,na16,nm16);
}

//////////////////////////////////////////////////////////////////////////
// 求解一整型等差序列经过两种偏移后的新序列 offset_1 + a 与 offset_2 - a
// 特殊情况：通过指定圆心的直线 y = x 与 y = -x 在指定大小区域内的点的 X 序列值，序列以 y 为索引
//////////////////////////////////////////////////////////////////////////
void math_serial_offset_2(OUT int* des1,		// offset_1 + a
						  OUT int* des2,		// offset_2 - a
						  IN int nmin,			// 最小值
						  IN int nmax,			// 最大值
						  IN float fOffset1,	// 对于 y = x 为 xc - yc
						  IN float fOffset2,	// 对于 y = -x 为 xc + yc
						  IN int nStart,		// 序列起点
						  IN int nStep,			// 序列步长
						  IN int na16,			// 缓存字节宽度除以 16 的商与余数（一次做四个元素）
						  IN int nm16)
{
	math_serial_offset_2_x64(des1,des2,nmin,nmax,fOffset1,fOffset2,nStart,nStep,na16,nm16);
}

//////////////////////////////////////////////////////////////////////////
// Implement texture coordinates mapping of perspective projection, the algorithm is
// same as GPU, named Hyperbolic Interpolation or Rational Linear Interpolation.
// Use bilinear interpolation for color rendering.
// The parameter "pvfXYZW" is an array pointer, it indicates the coordinates of
// the four vertexes of the surface after a series of tranformations such as
// rotation, transfer, zoom and so on, the order of the vertexes must be LeftTop(0),
// RightTop(1), RightBottom(2), LeftBottom(3).
//
// Bilinear algorithm:		S00 * x * y				S01 * (1 - x) * y
//							S10 * x * (1 - y)		S11 * (1 - x) * (1 - y)
// D = S11 + (S00 - S01 - S10 + S11) * x * y + (S10 - S11) * x + (S01 - S11) * y
//////////////////////////////////////////////////////////////////////////
// Calculate the weight and mapping indexes for bilinear interpolation.
void trans_power_index_bl(OUT DWORD* pdwSrcIndex,			// 双线性纹理映射时源的四个索引（S11, S10, S01, S00）
						  OUT float* pfPowerX,				// 水平方向的权重
						  OUT float* pfPowerY,				// 垂直方向的权重
						  OUT float* pfPowerXY,				// 权重之积
						  IN void* pvLeftEdgeUVW,			// 行左边缘的 UVW 值
						  IN void* pvDeltaUVW,				// 行内的 UVW 增量
						  IN int nSrcWidth,					// 源的大小
						  IN int nSrcHeight,
						  IN int nSrcBufWidth,				// 源缓存的真实宽度
						  IN int nLinePixels)				// 一行要处理的像素个数
{
	trans_power_index_bl_x64(pdwSrcIndex,pfPowerX,pfPowerY,pfPowerXY,pvLeftEdgeUVW,
		pvDeltaUVW,nSrcWidth,nSrcHeight,nSrcBufWidth,nLinePixels);
}
// Bilinear interpolation.
void trans_interpo_bl(OUT DWORD* pdwDesLeftEdge,	// 目标行的起始位置
					  IN DWORD* *ppdwSrcStart,		// 源缓存的起始位置
					  IN DWORD* pdwSrcIndex,		// 双线性纹理映射时源的四个索引
					  IN float* pfPowerX,			// 水平方向的权重
					  IN float* pfPowerY,			// 垂直方向的权重
					  IN float* pfPowerXY,			// 权重之积
					  IN int nLinePixels)			// 一行要处理的像素个数
{
	trans_interpo_bl_x64(pdwDesLeftEdge,ppdwSrcStart,pdwSrcIndex,pfPowerX,pfPowerY,pfPowerXY,nLinePixels);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// 颜色变换

// 颜色变换并拆场（不做边界判断）
void color32_to_yuyva_frame(OUT BYTE* pDesYUYV,         // YUYV 输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
							IN int nYUYVPitch,
							OUT BYTE* pDesAlpha,        // Alpha 输出（存储方式 A0 A1 A2 A3 A4 ……）
							IN int nAlphaPitch,
							IN BYTE* pSrc32,            // 源 *g@a 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
							IN int nSrcPitch,           // 源一行的字节宽度
							IN int nWidth,		        // 源图大小
							IN int nHeight,
							IN BOOL bHD,                // 高标清标志
							IN BOOL bSrcRevert,         // 图像是否行翻转标志
							IN BOOL bBGRA)              // 像素内颜色分量的存储顺序，为 TRUE 表示低 B 在低位
{
	int na16 = (nWidth >> 2), nm16 = ((nWidth & 0x03) << 2);
	BYTE* pSrcLine = (bSrcRevert) ? (pSrc32 + (nHeight - 1) * nSrcPitch) : pSrc32;
	int nRealSrcPitch = (bSrcRevert) ? -nSrcPitch : nSrcPitch;

	if (bHD)    // 高清
	{
		if (bBGRA)      // B 在低位
		{
			for (int i=0; i<nHeight; i++)
			{
				color32_to_yuyva_inner(pDesYUYV, pDesAlpha, pSrcLine, na16, nm16, g_nY32_RB_HD, g_nY32_G_HD, g_nY32_RGB_HD, g_nUV_RGB_HD, g_nU_RGB_HD, g_nV_RGB_HD);
				pDesYUYV += nYUYVPitch;
				pDesAlpha += nAlphaPitch;
				pSrcLine += nRealSrcPitch;
			}
		}
		else    // R 在低位
		{
			for (int i=0; i<nHeight; i++)
			{
				color32_to_yuyva_inner(pDesYUYV, pDesAlpha, pSrcLine, na16, nm16, g_nY32_BR_HD, g_nY32_G_HD, g_nY32_BGR_HD, g_nUV_BGR_HD, g_nU_BGR_HD, g_nV_BGR_HD);
				pDesYUYV += nYUYVPitch;
				pDesAlpha += nAlphaPitch;
				pSrcLine += nRealSrcPitch;
			}
		}
	}
	else    // 标清
	{
		if (bBGRA)      // B 在低位
		{
			for (int i=0; i<nHeight; i++)
			{
				color32_to_yuyva_inner(pDesYUYV, pDesAlpha, pSrcLine, na16, nm16, g_nY32_RB_SD, g_nY32_G_SD, g_nY32_RGB_SD, g_nUV_RGB_SD, g_nU_RGB_SD, g_nV_RGB_SD);
				pDesYUYV += nYUYVPitch;
				pDesAlpha += nAlphaPitch;
				pSrcLine += nRealSrcPitch;
			}
		}
		else    // R 在低位
		{
			for (int i=0; i<nHeight; i++)
			{
				color32_to_yuyva_inner(pDesYUYV, pDesAlpha, pSrcLine, na16, nm16, g_nY32_BR_SD, g_nY32_G_SD, g_nY32_BGR_SD, g_nUV_BGR_SD, g_nU_BGR_SD, g_nV_BGR_SD);
				pDesYUYV += nYUYVPitch;
				pDesAlpha += nAlphaPitch;
				pSrcLine += nRealSrcPitch;
			}
		}
	}
}
void color32_to_yuyva_split(OUT BYTE* pDesYUYVF,        // YUYV 第一场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
							IN int nYUYVPitchF,
							OUT BYTE* pDesAlphaF,       // Alpha 第一场输出（存储方式 A0 A1 A2 A3 A4 ……）
							IN int nAlphaPitchF,
							OUT BYTE* pDesYUYVS,        // YUYV 第二场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
							IN int nYUYVPitchS,
							OUT BYTE* pDesAlphaS,       // Alpha 第二场输出（存储方式 A0 A1 A2 A3 A4 ……）
							IN int nAlphaPitchS,
							IN BYTE* pSrc32,            // 源 *g@a 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
							IN int nSrcPitch,           // 源一行的字节宽度
							IN int nWidth,              // 源图大小
							IN int nHeight,
							IN BOOL bHD,                // 高标清标志
							IN BOOL bSrcRevert,         // 图像是否行翻转标志
							IN BOOL bBGRA)              // 像素内颜色分量的存储顺序，为 TRUE 表示低 B 在低位
{
	int na16 = (nWidth >> 2), nm16 = ((nWidth & 0x03) << 2);
	BYTE* pSrcLine = (bSrcRevert) ? (pSrc32 + (nHeight - 1) * nSrcPitch) : pSrc32;
	int nRealSrcPitch = (bSrcRevert) ? -nSrcPitch : nSrcPitch;
	int nHalfHeight = (nHeight >> 1);

	if (bHD)    // 高清
	{
		if (bBGRA)      // B 在低位
		{
			for (int i=0; i<nHalfHeight; i++)
			{
				// 第一场
				color32_to_yuyva_inner(pDesYUYVF, pDesAlphaF, pSrcLine, na16, nm16, g_nY32_RB_HD, g_nY32_G_HD, g_nY32_RGB_HD, g_nUV_RGB_HD, g_nU_RGB_HD, g_nV_RGB_HD);
				pDesYUYVF += nYUYVPitchF;
				pDesAlphaF += nAlphaPitchF;
				pSrcLine += nRealSrcPitch;
				// 第二场
				color32_to_yuyva_inner(pDesYUYVS, pDesAlphaS, pSrcLine, na16, nm16, g_nY32_RB_HD, g_nY32_G_HD, g_nY32_RGB_HD, g_nUV_RGB_HD, g_nU_RGB_HD, g_nV_RGB_HD);
				pDesYUYVS += nYUYVPitchS;
				pDesAlphaS += nAlphaPitchS;
				pSrcLine += nRealSrcPitch;
			}
			if (nHeight & 0x01)     // 如果为奇数行，需要特殊处理，第一场照旧，第二场清黑或拷贝
			{
				// 第一场
				color32_to_yuyva_inner(pDesYUYVF, pDesAlphaF, pSrcLine, na16, nm16, g_nY32_RB_HD, g_nY32_G_HD, g_nY32_RGB_HD, g_nUV_RGB_HD, g_nU_RGB_HD, g_nV_RGB_HD);
				// 第二场
				nYUYVPitchS = min(nYUYVPitchF, nYUYVPitchS);      // 拷贝用
				na16 = (nYUYVPitchS >> 4), nm16 = (nYUYVPitchS & 0x0f);
				//memset_dword_line(pDesSecond, CS_YUYV_BLACK_VALUE, na12, nmp);
				memcpy_line(pDesYUYVS, pDesYUYVF, na16, nm16);
				nAlphaPitchS = min(nAlphaPitchF, nAlphaPitchS);      // 拷贝用
				na16 = (nAlphaPitchS >> 4), nm16 = (nAlphaPitchS & 0x0f);
				memcpy_line(pDesAlphaS, pDesAlphaF, na16, nm16);
			}
		}
		else    // R 在低位
		{
			for (int i=0; i<nHeight; i++)
			{
				// 第一场
				color32_to_yuyva_inner(pDesYUYVF, pDesAlphaF, pSrcLine, na16, nm16, g_nY32_BR_HD, g_nY32_G_HD, g_nY32_BGR_HD, g_nUV_BGR_HD, g_nU_BGR_HD, g_nV_BGR_HD);
				pDesYUYVF += nYUYVPitchF;
				pDesAlphaF += nAlphaPitchF;
				pSrcLine += nRealSrcPitch;
				// 第二场
				color32_to_yuyva_inner(pDesYUYVS, pDesAlphaS, pSrcLine, na16, nm16, g_nY32_BR_HD, g_nY32_G_HD, g_nY32_BGR_HD, g_nUV_BGR_HD, g_nU_BGR_HD, g_nV_BGR_HD);
				pDesYUYVS += nYUYVPitchS;
				pDesAlphaS += nAlphaPitchS;
				pSrcLine += nRealSrcPitch;
			}
			if (nHeight & 0x01)     // 如果为奇数行，需要特殊处理，第一场照旧，第二场清黑或拷贝
			{
				// 第一场
				color32_to_yuyva_inner(pDesYUYVF, pDesAlphaF, pSrcLine, na16, nm16, g_nY32_BR_HD, g_nY32_G_HD, g_nY32_BGR_HD, g_nUV_BGR_HD, g_nU_BGR_HD, g_nV_BGR_HD);
				// 第二场
				nYUYVPitchS = min(nYUYVPitchF, nYUYVPitchS);      // 拷贝用
				na16 = (nYUYVPitchS >> 4), nm16 = (nYUYVPitchS & 0x0f);
				//memset_dword_line(pDesSecond, CS_YUYV_BLACK_VALUE, na12, nmp);
				memcpy_line(pDesYUYVS, pDesYUYVF, na16, nm16);
				nAlphaPitchS = min(nAlphaPitchF, nAlphaPitchS);      // 拷贝用
				na16 = (nAlphaPitchS >> 4), nm16 = (nAlphaPitchS & 0x0f);
				memcpy_line(pDesAlphaS, pDesAlphaF, na16, nm16);
			}
		}
	}
	else    // 标清
	{
		if (bBGRA)      // B 在低位
		{
			for (int i=0; i<nHeight; i++)
			{
				// 第一场
				color32_to_yuyva_inner(pDesYUYVF, pDesAlphaF, pSrcLine, na16, nm16, g_nY32_RB_SD, g_nY32_G_SD, g_nY32_RGB_SD, g_nUV_RGB_SD, g_nU_RGB_SD, g_nV_RGB_SD);
				pDesYUYVF += nYUYVPitchF;
				pDesAlphaF += nAlphaPitchF;
				pSrcLine += nRealSrcPitch;
				// 第二场
				color32_to_yuyva_inner(pDesYUYVS, pDesAlphaS, pSrcLine, na16, nm16, g_nY32_RB_SD, g_nY32_G_SD, g_nY32_RGB_SD, g_nUV_RGB_SD, g_nU_RGB_SD, g_nV_RGB_SD);
				pDesYUYVS += nYUYVPitchS;
				pDesAlphaS += nAlphaPitchS;
				pSrcLine += nRealSrcPitch;
			}
			if (nHeight & 0x01)     // 如果为奇数行，需要特殊处理，第一场照旧，第二场清黑或拷贝
			{
				// 第一场
				color32_to_yuyva_inner(pDesYUYVF, pDesAlphaF, pSrcLine, na16, nm16, g_nY32_RB_SD, g_nY32_G_SD, g_nY32_RGB_SD, g_nUV_RGB_SD, g_nU_RGB_SD, g_nV_RGB_SD);
				// 第二场
				nYUYVPitchS = min(nYUYVPitchF, nYUYVPitchS);      // 拷贝用
				na16 = (nYUYVPitchS >> 4), nm16 = (nYUYVPitchS & 0x0f);
				//memset_dword_line(pDesSecond, CS_YUYV_BLACK_VALUE, na12, nmp);
				memcpy_line(pDesYUYVS, pDesYUYVF, na16, nm16);
				nAlphaPitchS = min(nAlphaPitchF, nAlphaPitchS);      // 拷贝用
				na16 = (nAlphaPitchS >> 4), nm16 = (nAlphaPitchS & 0x0f);
				memcpy_line(pDesAlphaS, pDesAlphaF, na16, nm16);
			}
		}
		else    // R 在低位
		{
			for (int i=0; i<nHeight; i++)
			{
				// 第一场
				color32_to_yuyva_inner(pDesYUYVF, pDesAlphaF, pSrcLine, na16, nm16, g_nY32_BR_SD, g_nY32_G_SD, g_nY32_BGR_SD, g_nUV_BGR_SD, g_nU_BGR_SD, g_nV_BGR_SD);
				pDesYUYVF += nYUYVPitchF;
				pDesAlphaF += nAlphaPitchF;
				pSrcLine += nRealSrcPitch;
				// 第二场
				color32_to_yuyva_inner(pDesYUYVS, pDesAlphaS, pSrcLine, na16, nm16, g_nY32_BR_SD, g_nY32_G_SD, g_nY32_BGR_SD, g_nUV_BGR_SD, g_nU_BGR_SD, g_nV_BGR_SD);
				pDesYUYVS += nYUYVPitchS;
				pDesAlphaS += nAlphaPitchS;
				pSrcLine += nRealSrcPitch;
			}
			if (nHeight & 0x01)     // 如果为奇数行，需要特殊处理，第一场照旧，第二场清黑或拷贝
			{
				// 第一场
				color32_to_yuyva_inner(pDesYUYVF, pDesAlphaF, pSrcLine, na16, nm16, g_nY32_BR_SD, g_nY32_G_SD, g_nY32_BGR_SD, g_nUV_BGR_SD, g_nU_BGR_SD, g_nV_BGR_SD);
				// 第二场
				nYUYVPitchS = min(nYUYVPitchF, nYUYVPitchS);      // 拷贝用
				na16 = (nYUYVPitchS >> 4), nm16 = (nYUYVPitchS & 0x0f);
				//memset_dword_line(pDesSecond, CS_YUYV_BLACK_VALUE, na12, nmp);
				memcpy_line(pDesYUYVS, pDesYUYVF, na16, nm16);
				nAlphaPitchS = min(nAlphaPitchF, nAlphaPitchS);      // 拷贝用
				na16 = (nAlphaPitchS >> 4), nm16 = (nAlphaPitchS & 0x0f);
				memcpy_line(pDesAlphaS, pDesAlphaF, na16, nm16);
			}
		}
	}
}

void color32_to_yuyv_frame(OUT BYTE* pDesYUYV,         // YUYV 输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
						   IN int nYUYVPitch,
						   IN BYTE* pSrc32,            // 源 *g@a 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
						   IN int nSrcPitch,           // 源一行的字节宽度
						   IN int nWidth,		        // 源图大小
						   IN int nHeight,
						   IN BOOL bHD,                // 高标清标志
						   IN BOOL bSrcRevert,         // 图像是否行翻转标志
						   IN BOOL bBGRA)              // 像素内颜色分量的存储顺序，为 TRUE 表示低 B 在低位
{
	int na16 = (nWidth >> 2), nm16 = ((nWidth & 0x03) << 2);
	BYTE* pSrcLine = (bSrcRevert) ? (pSrc32 + (nHeight - 1) * nSrcPitch) : pSrc32;
	int nRealSrcPitch = (bSrcRevert) ? -nSrcPitch : nSrcPitch;

	if (bHD)    // 高清
	{
		if (bBGRA)      // B 在低位
		{
			for (int i=0; i<nHeight; i++)
			{
				color32_to_yuyv_inner(pDesYUYV, pSrcLine, na16, nm16, g_nY32_RB_HD, g_nY32_G_HD, g_nY32_RGB_HD, g_nUV_RGB_HD, g_nU_RGB_HD, g_nV_RGB_HD);
				pDesYUYV += nYUYVPitch;
				pSrcLine += nRealSrcPitch;
			}
		}
		else    // R 在低位
		{
			for (int i=0; i<nHeight; i++)
			{
				color32_to_yuyv_inner(pDesYUYV, pSrcLine, na16, nm16, g_nY32_BR_HD, g_nY32_G_HD, g_nY32_BGR_HD, g_nUV_BGR_HD, g_nU_BGR_HD, g_nV_BGR_HD);
				pDesYUYV += nYUYVPitch;
				pSrcLine += nRealSrcPitch;
			}
		}
	}
	else    // 标清
	{
		if (bBGRA)      // B 在低位
		{
			for (int i=0; i<nHeight; i++)
			{
				color32_to_yuyv_inner(pDesYUYV, pSrcLine, na16, nm16, g_nY32_RB_SD, g_nY32_G_SD, g_nY32_RGB_SD, g_nUV_RGB_SD, g_nU_RGB_SD, g_nV_RGB_SD);
				pDesYUYV += nYUYVPitch;
				pSrcLine += nRealSrcPitch;
			}
		}
		else    // R 在低位
		{
			for (int i=0; i<nHeight; i++)
			{
				color32_to_yuyv_inner(pDesYUYV, pSrcLine, na16, nm16, g_nY32_BR_SD, g_nY32_G_SD, g_nY32_BGR_SD, g_nUV_BGR_SD, g_nU_BGR_SD, g_nV_BGR_SD);
				pDesYUYV += nYUYVPitch;
				pSrcLine += nRealSrcPitch;
			}
		}
	}
}
void color32_to_yuyv_split(OUT BYTE* pDesYUYVF,        // YUYV 第一场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
						   IN int nYUYVPitchF,
						   OUT BYTE* pDesYUYVS,        // YUYV 第二场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
						   IN int nYUYVPitchS,
						   IN BYTE* pSrc32,            // 源 *g@a 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
						   IN int nSrcPitch,           // 源一行的字节宽度
						   IN int nWidth,              // 源图大小
						   IN int nHeight,
						   IN BOOL bHD,                // 高标清标志
						   IN BOOL bSrcRevert,         // 图像是否行翻转标志
						   IN BOOL bBGRA)              // 像素内颜色分量的存储顺序，为 TRUE 表示低 B 在低位
{
	int na16 = (nWidth >> 2), nm16 = ((nWidth & 0x03) << 2);
	BYTE* pSrcLine = (bSrcRevert) ? (pSrc32 + (nHeight - 1) * nSrcPitch) : pSrc32;
	int nRealSrcPitch = (bSrcRevert) ? -nSrcPitch : nSrcPitch;
	int nHalfHeight = (nHeight >> 1);

	if (bHD)    // 高清
	{
		if (bBGRA)      // B 在低位
		{
			for (int i=0; i<nHalfHeight; i++)
			{
				// 第一场
				color32_to_yuyv_inner(pDesYUYVF, pSrcLine, na16, nm16, g_nY32_RB_HD, g_nY32_G_HD, g_nY32_RGB_HD, g_nUV_RGB_HD, g_nU_RGB_HD, g_nV_RGB_HD);
				pDesYUYVF += nYUYVPitchF;
				pSrcLine += nRealSrcPitch;
				// 第二场
				color32_to_yuyv_inner(pDesYUYVS, pSrcLine, na16, nm16, g_nY32_RB_HD, g_nY32_G_HD, g_nY32_RGB_HD, g_nUV_RGB_HD, g_nU_RGB_HD, g_nV_RGB_HD);
				pDesYUYVS += nYUYVPitchS;
				pSrcLine += nRealSrcPitch;
			}
			if (nHeight & 0x01)     // 如果为奇数行，需要特殊处理，第一场照旧，第二场清黑或拷贝
			{
				// 第一场
				color32_to_yuyv_inner(pDesYUYVF, pSrcLine, na16, nm16, g_nY32_RB_HD, g_nY32_G_HD, g_nY32_RGB_HD, g_nUV_RGB_HD, g_nU_RGB_HD, g_nV_RGB_HD);
				// 第二场
				nYUYVPitchS = min(nYUYVPitchF, nYUYVPitchS);      // 拷贝用
				na16 = (nYUYVPitchS >> 4), nm16 = (nYUYVPitchS & 0x0f);
				//memset_dword_line(pDesSecond, CS_YUYV_BLACK_VALUE, na12, nmp);
				memcpy_line(pDesYUYVS, pDesYUYVF, na16, nm16);
			}
		}
		else    // R 在低位
		{
			for (int i=0; i<nHeight; i++)
			{
				// 第一场
				color32_to_yuyv_inner(pDesYUYVF, pSrcLine, na16, nm16, g_nY32_BR_HD, g_nY32_G_HD, g_nY32_BGR_HD, g_nUV_BGR_HD, g_nU_BGR_HD, g_nV_BGR_HD);
				pDesYUYVF += nYUYVPitchF;
				pSrcLine += nRealSrcPitch;
				// 第二场
				color32_to_yuyv_inner(pDesYUYVS, pSrcLine, na16, nm16, g_nY32_BR_HD, g_nY32_G_HD, g_nY32_BGR_HD, g_nUV_BGR_HD, g_nU_BGR_HD, g_nV_BGR_HD);
				pDesYUYVS += nYUYVPitchS;
				pSrcLine += nRealSrcPitch;
			}
			if (nHeight & 0x01)     // 如果为奇数行，需要特殊处理，第一场照旧，第二场清黑或拷贝
			{
				// 第一场
				color32_to_yuyv_inner(pDesYUYVF, pSrcLine, na16, nm16, g_nY32_BR_HD, g_nY32_G_HD, g_nY32_BGR_HD, g_nUV_BGR_HD, g_nU_BGR_HD, g_nV_BGR_HD);
				// 第二场
				nYUYVPitchS = min(nYUYVPitchF, nYUYVPitchS);      // 拷贝用
				na16 = (nYUYVPitchS >> 4), nm16 = (nYUYVPitchS & 0x0f);
				//memset_dword_line(pDesSecond, CS_YUYV_BLACK_VALUE, na12, nmp);
				memcpy_line(pDesYUYVS, pDesYUYVF, na16, nm16);
			}
		}
	}
	else    // 标清
	{
		if (bBGRA)      // B 在低位
		{
			for (int i=0; i<nHeight; i++)
			{
				// 第一场
				color32_to_yuyv_inner(pDesYUYVF, pSrcLine, na16, nm16, g_nY32_RB_SD, g_nY32_G_SD, g_nY32_RGB_SD, g_nUV_RGB_SD, g_nU_RGB_SD, g_nV_RGB_SD);
				pDesYUYVF += nYUYVPitchF;
				pSrcLine += nRealSrcPitch;
				// 第二场
				color32_to_yuyv_inner(pDesYUYVS, pSrcLine, na16, nm16, g_nY32_RB_SD, g_nY32_G_SD, g_nY32_RGB_SD, g_nUV_RGB_SD, g_nU_RGB_SD, g_nV_RGB_SD);
				pDesYUYVS += nYUYVPitchS;
				pSrcLine += nRealSrcPitch;
			}
			if (nHeight & 0x01)     // 如果为奇数行，需要特殊处理，第一场照旧，第二场清黑或拷贝
			{
				// 第一场
				color32_to_yuyv_inner(pDesYUYVF, pSrcLine, na16, nm16, g_nY32_RB_SD, g_nY32_G_SD, g_nY32_RGB_SD, g_nUV_RGB_SD, g_nU_RGB_SD, g_nV_RGB_SD);
				// 第二场
				nYUYVPitchS = min(nYUYVPitchF, nYUYVPitchS);      // 拷贝用
				na16 = (nYUYVPitchS >> 4), nm16 = (nYUYVPitchS & 0x0f);
				//memset_dword_line(pDesSecond, CS_YUYV_BLACK_VALUE, na12, nmp);
				memcpy_line(pDesYUYVS, pDesYUYVF, na16, nm16);
			}
		}
		else    // R 在低位
		{
			for (int i=0; i<nHeight; i++)
			{
				// 第一场
				color32_to_yuyv_inner(pDesYUYVF, pSrcLine, na16, nm16, g_nY32_BR_SD, g_nY32_G_SD, g_nY32_BGR_SD, g_nUV_BGR_SD, g_nU_BGR_SD, g_nV_BGR_SD);
				pDesYUYVF += nYUYVPitchF;
				pSrcLine += nRealSrcPitch;
				// 第二场
				color32_to_yuyv_inner(pDesYUYVS, pSrcLine, na16, nm16, g_nY32_BR_SD, g_nY32_G_SD, g_nY32_BGR_SD, g_nUV_BGR_SD, g_nU_BGR_SD, g_nV_BGR_SD);
				pDesYUYVS += nYUYVPitchS;
				pSrcLine += nRealSrcPitch;
			}
			if (nHeight & 0x01)     // 如果为奇数行，需要特殊处理，第一场照旧，第二场清黑或拷贝
			{
				// 第一场
				color32_to_yuyv_inner(pDesYUYVF, pSrcLine, na16, nm16, g_nY32_BR_SD, g_nY32_G_SD, g_nY32_BGR_SD, g_nUV_BGR_SD, g_nU_BGR_SD, g_nV_BGR_SD);
				// 第二场
				nYUYVPitchS = min(nYUYVPitchF, nYUYVPitchS);      // 拷贝用
				na16 = (nYUYVPitchS >> 4), nm16 = (nYUYVPitchS & 0x0f);
				//memset_dword_line(pDesSecond, CS_YUYV_BLACK_VALUE, na12, nmp);
				memcpy_line(pDesYUYVS, pDesYUYVF, na16, nm16);
			}
		}
	}
}

void yuyv_to_color32_compose(OUT BYTE* pDes32,          // *G@A 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）（Alpha 直接填 0xff）
							 IN int nDesPitch,
							 IN BYTE* pSrcYUYVF,        // YUYV 第一场输入（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
							 IN int nFirstPitch,
							 IN BYTE* pSrcYUYVS,        // YUYV 第二场输入（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
							 IN int nSecondPitch,
							 IN int nWidth,             // 目标大小
							 IN int nHeight,
							 IN BOOL bHD,               // 高标清标志
							 IN BOOL bBGRA)             // 像素内颜色分量的存储顺序，为 TRUE 表示低 B 在低位
{
	int nRealYUYVPitch = ((nWidth + (nWidth & 0x01)) << 1);
	int na16 = ((nRealYUYVPitch - 4) >> 4), nm16 = (nRealYUYVPitch - (na16 << 4));
	int nHalfHeight = (nHeight >> 1);

	if (bHD)        // 高清
	{
		if (bBGRA)      // B 在低位
		{
			for (int i=0; i<nHalfHeight; i++)
			{
				// 第一场
				yuyv_to_bgra_inner(pDes32, pSrcYUYVF, na16, nm16, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
				pDes32 += nDesPitch;
				pSrcYUYVF += nFirstPitch;
				// 第二场
				yuyv_to_bgra_inner(pDes32, pSrcYUYVS, na16, nm16, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
				pDes32 += nDesPitch;
				pSrcYUYVF += nFirstPitch;
			}
			if (nHeight & 0x01)     // 如果为奇数行，需要特殊处理，只采用第一场
			{
				yuyv_to_bgra_inner(pDes32, pSrcYUYVF, na16, nm16, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
			}
		}
		else    // R 在低位
		{
			for (int i=0; i<nHalfHeight; i++)
			{
				// 第一场
				yuyv_to_rgba_inner(pDes32, pSrcYUYVF, na16, nm16, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
				pDes32 += nDesPitch;
				pSrcYUYVF += nFirstPitch;
				// 第二场
				yuyv_to_rgba_inner(pDes32, pSrcYUYVS, na16, nm16, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
				pDes32 += nDesPitch;
				pSrcYUYVF += nFirstPitch;
			}
			if (nHeight & 0x01)     // 如果为奇数行，需要特殊处理，只采用第一场
			{
				yuyv_to_rgba_inner(pDes32, pSrcYUYVF, na16, nm16, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
			}
		}
	}
	else            // 标清
	{
		if (bBGRA)      // B 在低位
		{
			for (int i=0; i<nHalfHeight; i++)
			{
				// 第一场
				yuyv_to_bgra_inner(pDes32, pSrcYUYVF, na16, nm16, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
				pDes32 += nDesPitch;
				pSrcYUYVF += nFirstPitch;
				// 第二场
				yuyv_to_bgra_inner(pDes32, pSrcYUYVS, na16, nm16, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
				pDes32 += nDesPitch;
				pSrcYUYVF += nFirstPitch;
			}
			if (nHeight & 0x01)     // 如果为奇数行，需要特殊处理，只采用第一场
			{
				yuyv_to_bgra_inner(pDes32, pSrcYUYVF, na16, nm16, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
			}
		}
		else    // R 在低位
		{
			for (int i=0; i<nHalfHeight; i++)
			{
				// 第一场
				yuyv_to_rgba_inner(pDes32, pSrcYUYVF, na16, nm16, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
				pDes32 += nDesPitch;
				pSrcYUYVF += nFirstPitch;
				// 第二场
				yuyv_to_rgba_inner(pDes32, pSrcYUYVS, na16, nm16, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
				pDes32 += nDesPitch;
				pSrcYUYVF += nFirstPitch;
			}
			if (nHeight & 0x01)     // 如果为奇数行，需要特殊处理，只采用第一场
			{
				yuyv_to_rgba_inner(pDes32, pSrcYUYVF, na16, nm16, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
			}
		}
	}
}

void color24_to_yuyv_frame(OUT BYTE* pDesYUYV,         // YUYV 输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
						   IN int nYUYVPitch,
						   IN BYTE* pSrc24,            // *G@ 输入，存放方式 *0 G0 @0 *1 G1 @1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
						   IN int nSrcPitch,           // 源一行的字节宽度
						   IN int nWidth,		       // 源图大小
						   IN int nHeight,
						   IN BOOL bHD,                // 高标清标志
						   IN BOOL bSrcRevert,         // 图像是否行翻转标志
						   IN BOOL bBGR)               // 像素内颜色分量的存储顺序，为 TRUE 表示低 B 在低位
{
	int na12 = ((nSrcPitch - 4) / 12), nmp = (nWidth - (na12 << 2));
	BYTE* pSrcLine = (bSrcRevert) ? (pSrc24 + (nHeight - 1) * nSrcPitch) : pSrc24;
	int nRealSrcPitch = (bSrcRevert) ? -nSrcPitch : nSrcPitch;

	if (bHD)       // 高清
	{
		if (bBGR)      // B 在低位
		{
			for (int i=0; i<nHeight; i++)
			{
				color24_to_yuyv_inner(pDesYUYV, pSrcLine, na12, nmp, g_nY24_BGR_HD, g_nY32_RGB_HD, g_nU_RGB_HD, g_nV_RGB_HD, g_nUV_RGB_HD);
				pSrcLine += nRealSrcPitch;
				pDesYUYV += nYUYVPitch;
			}
		}
		else        // R 在低位
		{
			for (int i=0; i<nHeight; i++)
			{
				color24_to_yuyv_inner(pDesYUYV, pSrcLine, na12, nmp, g_nY24_RGB_HD, g_nY32_BGR_HD, g_nU_BGR_HD, g_nV_BGR_HD, g_nUV_BGR_HD);
				pSrcLine += nRealSrcPitch;
				pDesYUYV += nYUYVPitch;
			}
		}
	}
	else        // 标清
	{
		if (bBGR)      // B 在低位
		{
			for (int i=0; i<nHeight; i++)
			{
				color24_to_yuyv_inner(pDesYUYV, pSrcLine, na12, nmp, g_nY24_BGR_SD, g_nY32_RGB_SD, g_nU_RGB_SD, g_nV_RGB_SD, g_nUV_RGB_SD);
				pSrcLine += nRealSrcPitch;
				pDesYUYV += nYUYVPitch;
			}
		}
		else        // R 在低位
		{
			for (int i=0; i<nHeight; i++)
			{
				color24_to_yuyv_inner(pDesYUYV, pSrcLine, na12, nmp, g_nY24_RGB_SD, g_nY32_BGR_SD, g_nU_BGR_SD, g_nV_BGR_SD, g_nUV_BGR_SD);
				pSrcLine += nRealSrcPitch;
				pDesYUYV += nYUYVPitch;
			}
		}
	}
}
void color24_to_yuyv_split(OUT BYTE* pDesFirst,          // 第一场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
						   IN int nFirstPitch,
						   OUT BYTE* pDesSecond,         // 第二场输出（存储方式 Y0 U0 Y1 V0 Y2 U2 Y3 V3 ……）
						   IN int nSecondPitch,
						   IN BYTE* pSrc24,              // *G@ 输入，存放方式 *0 G0 @0 *1 G1 @1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
						   IN int nSrcPitch,             // 源一行的字节宽度（必须为 4 的倍数）
						   IN int nWidth,			     // 源图大小
						   IN int nHeight,
						   IN BOOL bHD,                  // 高标清标志
						   IN BOOL bSrcRevert,           // 图像是否行翻转标志
						   IN BOOL bBGR)                 // 像素内颜色分量的存储顺序，为 TRUE 表示低 B 在低位
{
	int na12 = ((nSrcPitch - 4) / 12), nmp = (nWidth - (na12 << 2));
	BYTE* pSrcLine = (bSrcRevert) ? (pSrc24 + (nHeight - 1) * nSrcPitch) : pSrc24;
	int nRealSrcPitch = (bSrcRevert) ? -nSrcPitch : nSrcPitch;
	int nHalfHeight = (nHeight >> 1);

	if (bHD)       // 高清
	{
		if (bBGR)      // B 在低位
		{
			for (int i=0; i<nHalfHeight; i++)
			{
				// 第一场
				color24_to_yuyv_inner(pDesFirst, pSrcLine, na12, nmp, g_nY24_BGR_HD, g_nY32_RGB_HD, g_nU_RGB_HD, g_nV_RGB_HD, g_nUV_RGB_HD);
				pSrcLine += nRealSrcPitch;
				pDesFirst += nFirstPitch;
				// 第二场
				color24_to_yuyv_inner(pDesSecond, pSrcLine, na12, nmp, g_nY24_BGR_HD, g_nY32_RGB_HD, g_nU_RGB_HD, g_nV_RGB_HD, g_nUV_RGB_HD);
				pSrcLine += nRealSrcPitch;
				pDesSecond += nSecondPitch;
			}
			if (nHeight & 0x01)     // 如果为奇数行，需要特殊处理，第一场照旧，第二场清黑或拷贝
			{
				// 第一场
				color24_to_yuyv_inner(pDesFirst, pSrcLine, na12, nmp, g_nY24_BGR_HD, g_nY32_RGB_HD, g_nU_RGB_HD, g_nV_RGB_HD, g_nUV_RGB_HD);
				// 第二场
				nSecondPitch = min(nFirstPitch, nSecondPitch);      // 拷贝用
				na12 = (nSecondPitch >> 4), nmp = (nSecondPitch & 0x0f);
				//memset_dword_line(pDesSecond, CS_YUYV_BLACK_VALUE, na12, nmp);
				memcpy_line(pDesSecond, pDesFirst, na12, nmp);
			}
		}
		else        // R 在低位
		{
			for (int i=0; i<nHalfHeight; i++)
			{
				// 第一场
				color24_to_yuyv_inner(pDesFirst, pSrcLine, na12, nmp, g_nY24_RGB_HD, g_nY32_BGR_HD, g_nU_BGR_HD, g_nV_BGR_HD, g_nUV_BGR_HD);
				pSrcLine += nRealSrcPitch;
				pDesFirst += nFirstPitch;
				// 第二场
				color24_to_yuyv_inner(pDesSecond, pSrcLine, na12, nmp, g_nY24_RGB_HD, g_nY32_BGR_HD, g_nU_BGR_HD, g_nV_BGR_HD, g_nUV_BGR_HD);
				pSrcLine += nRealSrcPitch;
				pDesSecond += nSecondPitch;
			}
			if (nHeight & 0x01)     // 如果为奇数行，需要特殊处理，第一场照旧，第二场清黑或拷贝
			{
				// 第一场
				color24_to_yuyv_inner(pDesFirst, pSrcLine, na12, nmp, g_nY24_RGB_HD, g_nY32_BGR_HD, g_nU_BGR_HD, g_nV_BGR_HD, g_nUV_BGR_HD);
				// 第二场
				nSecondPitch = min(nFirstPitch, nSecondPitch);      // 拷贝用
				na12 = (nSecondPitch >> 4), nmp = (nSecondPitch & 0x0f);
				//memset_dword_line(pDesSecond, CS_YUYV_BLACK_VALUE, na12, nmp);
				memcpy_line(pDesSecond, pDesFirst, na12, nmp);
			}
		}
	}
	else        // 标清
	{
		if (bBGR)      // B 在低位
		{
			for (int i=0; i<nHalfHeight; i++)
			{
				// 第一场
				color24_to_yuyv_inner(pDesFirst, pSrcLine, na12, nmp, g_nY24_BGR_SD, g_nY32_RGB_SD, g_nU_RGB_SD, g_nV_RGB_SD, g_nUV_RGB_SD);
				pSrcLine += nRealSrcPitch;
				pDesFirst += nFirstPitch;
				// 第二场
				color24_to_yuyv_inner(pDesSecond, pSrcLine, na12, nmp, g_nY24_BGR_SD, g_nY32_RGB_SD, g_nU_RGB_SD, g_nV_RGB_SD, g_nUV_RGB_SD);
				pSrcLine += nRealSrcPitch;
				pDesSecond += nSecondPitch;
			}
			if (nHeight & 0x01)     // 如果为奇数行，需要特殊处理，第一场照旧，第二场清黑或拷贝
			{
				// 第一场
				color24_to_yuyv_inner(pDesFirst, pSrcLine, na12, nmp, g_nY24_BGR_SD, g_nY32_RGB_SD, g_nU_RGB_SD, g_nV_RGB_SD, g_nUV_RGB_SD);
				// 第二场
				nSecondPitch = min(nFirstPitch, nSecondPitch);      // 拷贝用
				na12 = (nSecondPitch >> 4), nmp = (nSecondPitch & 0x0f);
				//memset_dword_line(pDesSecond, CS_YUYV_BLACK_VALUE, na12, nmp);
				memcpy_line(pDesSecond, pDesFirst, na12, nmp);
			}
		}
		else        // R 在低位
		{
			for (int i=0; i<nHalfHeight; i++)
			{
				// 第一场
				color24_to_yuyv_inner(pDesFirst, pSrcLine, na12, nmp, g_nY24_RGB_SD, g_nY32_BGR_SD, g_nU_BGR_SD, g_nV_BGR_SD, g_nUV_BGR_SD);
				pSrcLine += nRealSrcPitch;
				pDesFirst += nFirstPitch;
				// 第二场
				color24_to_yuyv_inner(pDesSecond, pSrcLine, na12, nmp, g_nY24_RGB_SD, g_nY32_BGR_SD, g_nU_BGR_SD, g_nV_BGR_SD, g_nUV_BGR_SD);
				pSrcLine += nRealSrcPitch;
				pDesSecond += nSecondPitch;
			}
			if (nHeight & 0x01)     // 如果为奇数行，需要特殊处理，第一场照旧，第二场清黑或拷贝
			{
				// 第一场
				color24_to_yuyv_inner(pDesFirst, pSrcLine, na12, nmp, g_nY24_RGB_SD, g_nY32_BGR_SD, g_nU_BGR_SD, g_nV_BGR_SD, g_nUV_BGR_SD);
				// 第二场
				nSecondPitch = min(nFirstPitch, nSecondPitch);      // 拷贝用
				na12 = (nSecondPitch >> 4), nmp = (nSecondPitch & 0x0f);
				//memset_dword_line(pDesSecond, CS_YUYV_BLACK_VALUE, na12, nmp);
				memcpy_line(pDesSecond, pDesFirst, na12, nmp);
			}
		}
	}
}

void yuv420_to_color32_frame(OUT BYTE* pDes32,           // *G@A 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）（Alpha 直接填 0xff）
							 IN int nDesPitch,
							 IN BYTE* pSrcY,             // Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
							 IN int nSrcYPitch,
							 IN BYTE* pSrcU,             // U 输入（存储方式 UU0 UU2 UU4 ……）（两行共用一个）
							 IN int nSrcUPitch,
							 IN BYTE* pSrcV,             // V 输入（存储方式 VV0 VV2 VV4 ……）（两行共用一个）
							 IN int nSrcVPitch,
							 IN int nWidth,              // 目标大小
							 IN int nHeight,
							 IN BOOL bHD,                // 高标清标志
							 IN BOOL bDesRevert,         // 图像是否行翻转标志
							 IN BOOL bBGRA)              // 像素内颜色分量的存储顺序，为 TRUE 表示低 B 在低位
{
	int nHalfHeight = (nHeight >> 1), nHalfHalf = ((nHalfHeight + 1) >> 1);
	int nUVPitch = min(nSrcUPitch, nSrcVPitch);
	int na16 = (nWidth >> 5), nm16 = ((nWidth >> 1) & 0x0f);	// 一行像素宽度的一半（因为一行内的 UV 与 Y 为 422 的关系）
	int na8 = ((nWidth - 1) >> 3), nm8 = (nWidth - (na8 << 3));
	int nYP2 = (nSrcYPitch << 1), nUP2 = (nSrcUPitch << 1), nVP2 = (nSrcVPitch << 1);
	// 采用缓存，先插值计算出 422 的 UV 信号（两行），再使用 422 到 color32 的接口
	BYTE* pTmpU = (BYTE*)_mm_malloc(sizeof(BYTE) * (nUVPitch << 1), 16);
	BYTE* pTmpV = (BYTE*)_mm_malloc(sizeof(BYTE) * (nUVPitch << 1), 16);
	BYTE *pU1 = pTmpU, *pU2 = pTmpU + nUVPitch, *pV1 = pTmpV, *pV2 = pTmpV + nUVPitch;
	// 翻转
	BYTE* pDesF = (bDesRevert) ? (pDes32 + (nHeight - 1) * nDesPitch) : pDes32;
	int nRealDesPitch = (bDesRevert) ? -nDesPitch : nDesPitch;
	BYTE* pDesS = pDesF + nRealDesPitch;
	int nRealDP2 = (bDesRevert) ? -(nDesPitch << 1) : (nDesPitch << 1);
	BYTE *pYF = pSrcY, *pYS = pSrcY + nSrcYPitch;

	if (bHD)    // 高清
	{
		if (bBGRA)      // B 在低位
		{
			// 前两行
			// 第一场
			uv20_to_uv22_line_2_F(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
			yuv422_to_color32_inner(pDesF, pYF, pV1, pU1, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
			pDesF += nRealDP2; pYF += nYP2;
			yuv422_to_color32_inner(pDesF, pYF, pV2, pU2, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
			pDesF += nRealDP2; pYF += nYP2;
			pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
			// 第二场
			uv20_to_uv22_line_2_F(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
			yuv422_to_color32_inner(pDesS, pYS, pV1, pU1, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
			pDesS += nRealDP2; pYS += nYP2;
			yuv422_to_color32_inner(pDesS, pYS, pV2, pU2, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
			pDesS += nRealDP2; pYS += nYP2;
			pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
			// 中间行
			for (int i=2; i<nHalfHalf; i++)
			{
				// 第一场
				uv20_to_uv22_line_2(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesF, pYF, pV1, pU1, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
				pDesF += nRealDP2; pYF += nYP2;
				yuv422_to_color32_inner(pDesF, pYF, pV2, pU2, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
				pDesF += nRealDP2; pYF += nYP2;
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
				// 第二场
				uv20_to_uv22_line_2(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesS, pYS, pV1, pU1, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
				pDesS += nRealDP2; pYS += nYP2;
				yuv422_to_color32_inner(pDesS, pYS, pV2, pU2, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
				pDesS += nRealDP2; pYS += nYP2;
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
			}
			// 最后两行或一行（场为奇数行时）
			if (nHalfHalf & 0x01)       // 一行
			{
				// 第一场
				uv20_to_uv22_line_1_L(pTmpU, pTmpV, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesF, pYF, pV1, pU1, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
				// 第二场
				uv20_to_uv22_line_1_L(pTmpU, pTmpV, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesS, pYS, pV1, pU1, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
			}
			else    // 两行
			{
				// 第一场
				uv20_to_uv22_line_2_L(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesF, pYF, pV1, pU1, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
				pDesF += nRealDP2; pYF += nYP2;
				yuv422_to_color32_inner(pDesF, pYF, pV2, pU2, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
				// 第二场
				uv20_to_uv22_line_2_L(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesS, pYS, pV1, pU1, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
				pDesS += nRealDP2; pYS += nYP2;
				yuv422_to_color32_inner(pDesS, pYS, pV2, pU2, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
			}
		}
		else        // R 在低位
		{
			// 前两行
			// 第一场
			uv20_to_uv22_line_2_F(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
			yuv422_to_color32_inner(pDesF, pYF, pU1, pV1, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
			pDesF += nRealDP2; pYF += nYP2;
			yuv422_to_color32_inner(pDesF, pYF, pU2, pV2, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
			pDesF += nRealDP2; pYF += nYP2;
			pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
			// 第二场
			uv20_to_uv22_line_2_F(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
			yuv422_to_color32_inner(pDesS, pYS, pU1, pV1, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
			pDesS += nRealDP2; pYS += nYP2;
			yuv422_to_color32_inner(pDesS, pYS, pU2, pV2, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
			pDesS += nRealDP2; pYS += nYP2;
			pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
			// 中间行
			for (int i=2; i<nHalfHalf; i++)
			{
				// 第一场
				uv20_to_uv22_line_2(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesF, pYF, pU1, pV1, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
				pDesF += nRealDP2; pYF += nYP2;
				yuv422_to_color32_inner(pDesF, pYF, pU2, pV2, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
				pDesF += nRealDP2; pYF += nYP2;
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
				// 第二场
				uv20_to_uv22_line_2(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesS, pYS, pU1, pV1, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
				pDesS += nRealDP2; pYS += nYP2;
				yuv422_to_color32_inner(pDesS, pYS, pU2, pV2, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
				pDesS += nRealDP2; pYS += nYP2;
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
			}
			// 最后两行或一行（场为奇数行时）
			if (nHalfHalf & 0x01)       // 一行
			{
				// 第一场
				uv20_to_uv22_line_1_L(pTmpU, pTmpV, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesF, pYF, pU1, pV1, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
				// 第二场
				uv20_to_uv22_line_1_L(pTmpU, pTmpV, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesS, pYS, pU1, pV1, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
			}
			else    // 两行
			{
				// 第一场
				uv20_to_uv22_line_2_L(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesF, pYF, pU1, pV1, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
				pDesF += nRealDP2; pYF += nYP2;
				yuv422_to_color32_inner(pDesF, pYF, pU2, pV2, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
				// 第二场
				uv20_to_uv22_line_2_L(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesS, pYS, pU1, pV1, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
				pDesS += nRealDP2; pYS += nYP2;
				yuv422_to_color32_inner(pDesS, pYS, pU2, pV2, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
			}
		}
	}
	else        // 标清
	{
		if (bBGRA)      // B 在低位
		{
			// 前两行
			// 第一场
			uv20_to_uv22_line_2_F(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
			yuv422_to_color32_inner(pDesF, pYF, pV1, pU1, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
			pDesF += nRealDP2; pYF += nYP2;
			yuv422_to_color32_inner(pDesF, pYF, pV2, pU2, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
			pDesF += nRealDP2; pYF += nYP2;
			pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
			// 第二场
			uv20_to_uv22_line_2_F(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
			yuv422_to_color32_inner(pDesS, pYS, pV1, pU1, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
			pDesS += nRealDP2; pYS += nYP2;
			yuv422_to_color32_inner(pDesS, pYS, pV2, pU2, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
			pDesS += nRealDP2; pYS += nYP2;
			pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
			// 中间行
			for (int i=2; i<nHalfHalf; i++)
			{
				// 第一场
				uv20_to_uv22_line_2(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesF, pYF, pV1, pU1, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
				pDesF += nRealDP2; pYF += nYP2;
				yuv422_to_color32_inner(pDesF, pYF, pV2, pU2, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
				pDesF += nRealDP2; pYF += nYP2;
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
				// 第二场
				uv20_to_uv22_line_2(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesS, pYS, pV1, pU1, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
				pDesS += nRealDP2; pYS += nYP2;
				yuv422_to_color32_inner(pDesS, pYS, pV2, pU2, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
				pDesS += nRealDP2; pYS += nYP2;
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
			}
			// 最后两行或一行（场为奇数行时）
			if (nHalfHalf & 0x01)       // 一行
			{
				// 第一场
				uv20_to_uv22_line_1_L(pTmpU, pTmpV, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesF, pYF, pV1, pU1, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
				// 第二场
				uv20_to_uv22_line_1_L(pTmpU, pTmpV, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesS, pYS, pV1, pU1, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
			}
			else    // 两行
			{
				// 第一场
				uv20_to_uv22_line_2_L(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesF, pYF, pV1, pU1, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
				pDesF += nRealDP2; pYF += nYP2;
				yuv422_to_color32_inner(pDesF, pYF, pV2, pU2, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
				// 第二场
				uv20_to_uv22_line_2_L(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesS, pYS, pV1, pU1, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
				pDesS += nRealDP2; pYS += nYP2;
				yuv422_to_color32_inner(pDesS, pYS, pV2, pU2, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
			}
		}
		else        // R 在低位
		{
			// 前两行
			// 第一场
			uv20_to_uv22_line_2_F(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
			yuv422_to_color32_inner(pDesF, pYF, pU1, pV1, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
			pDesF += nRealDP2; pYF += nYP2;
			yuv422_to_color32_inner(pDesF, pYF, pU2, pV2, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
			pDesF += nRealDP2; pYF += nYP2;
			pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
			// 第二场
			uv20_to_uv22_line_2_F(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
			yuv422_to_color32_inner(pDesS, pYS, pU1, pV1, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
			pDesS += nRealDP2; pYS += nYP2;
			yuv422_to_color32_inner(pDesS, pYS, pU2, pV2, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
			pDesS += nRealDP2; pYS += nYP2;
			pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
			// 中间行
			for (int i=2; i<nHalfHalf; i++)
			{
				// 第一场
				uv20_to_uv22_line_2(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesF, pYF, pU1, pV1, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
				pDesF += nRealDP2; pYF += nYP2;
				yuv422_to_color32_inner(pDesF, pYF, pU2, pV2, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
				pDesF += nRealDP2; pYF += nYP2;
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
				// 第二场
				uv20_to_uv22_line_2(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesS, pYS, pU1, pV1, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
				pDesS += nRealDP2; pYS += nYP2;
				yuv422_to_color32_inner(pDesS, pYS, pU2, pV2, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
				pDesS += nRealDP2; pYS += nYP2;
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
			}
			// 最后两行或一行（场为奇数行时）
			if (nHalfHalf & 0x01)       // 一行
			{
				// 第一场
				uv20_to_uv22_line_1_L(pTmpU, pTmpV, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesF, pYF, pU1, pV1, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
				// 第二场
				uv20_to_uv22_line_1_L(pTmpU, pTmpV, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesS, pYS, pU1, pV1, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
			}
			else    // 两行
			{
				// 第一场
				uv20_to_uv22_line_2_L(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesF, pYF, pU1, pV1, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
				pDesF += nRealDP2; pYF += nYP2;
				yuv422_to_color32_inner(pDesF, pYF, pU2, pV2, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
				// 第二场
				uv20_to_uv22_line_2_L(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesS, pYS, pU1, pV1, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
				pDesS += nRealDP2; pYS += nYP2;
				yuv422_to_color32_inner(pDesS, pYS, pU2, pV2, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
			}
		}
	}
	// 释放缓存
	_mm_free(pTmpU);
	_mm_free(pTmpV);
}
void yuv420_to_color32_split(OUT BYTE* pDesFirst,        // 第一场 *G@A 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）（Alpha 直接填 0xff）
							 IN int nFirstPitch,
							 OUT BYTE* pDesSecond,       // 第二场 *G@A 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）（Alpha 直接填 0xff）
							 IN int nSecondPitch,
							 IN BYTE* pSrcY,             // Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
							 IN int nSrcYPitch,
							 IN BYTE* pSrcU,             // U 输入（存储方式 UU0 UU2 UU4 ……）（两行共用一个）
							 IN int nSrcUPitch,
							 IN BYTE* pSrcV,             // V 输入（存储方式 VV0 VV2 VV4 ……）（两行共用一个）
							 IN int nSrcVPitch,
							 IN int nWidth,              // 目标大小
							 IN int nHeight,
							 IN BOOL bHD,                // 高标清标志
							 IN BOOL bDesRevert,         // 图像是否行翻转标志
							 IN BOOL bBGRA)              // 像素内颜色分量的存储顺序，为 TRUE 表示低 B 在低位
{
	int nHalfHeight = (nHeight >> 1), nHalfHalf = ((nHalfHeight + 1) >> 1);
	int nUVPitch = min(nSrcUPitch, nSrcVPitch);
	int na16 = (nWidth >> 4), nm16 = (nWidth & 0x0f);
	int na8 = ((nWidth - 1) >> 3), nm8 = (nWidth - (na8 << 3));
	int nYP2 = (nSrcYPitch << 1), nUP2 = (nSrcUPitch << 1), nVP2 = (nSrcVPitch << 1);
	// 采用缓存，先插值计算出 422 的 UV 信号（两行），再使用 422 到 color32 的接口
	BYTE* pTmpU = (BYTE*)_mm_malloc(sizeof(BYTE) * (nUVPitch << 1), 16);
	BYTE* pTmpV = (BYTE*)_mm_malloc(sizeof(BYTE) * (nUVPitch << 1), 16);
	BYTE *pU1 = pTmpU, *pU2 = pTmpU + nUVPitch, *pV1 = pTmpV, *pV2 = pTmpV + nUVPitch;
	// 翻转
	BYTE* pDesF = (bDesRevert) ? (pDesFirst + (nHalfHeight - 1) * nFirstPitch) : pDesFirst;
	int nRealFirstPitch = (bDesRevert) ? -nFirstPitch : nFirstPitch;
	BYTE* pDesS = (bDesRevert) ? (pDesSecond + (nHalfHeight - 1) * nSecondPitch) : pDesSecond;
	int nRealSecondPitch = (bDesRevert) ? -nSecondPitch : nSecondPitch;
	BYTE *pYF = pSrcY, *pYS = pSrcY + nSrcYPitch;

	if (bHD)    // 高清
	{
		if (bBGRA)      // B 在低位
		{
			// 前两行
			// 第一场
			uv20_to_uv22_line_2_F(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
			yuv422_to_color32_inner(pDesF, pYF, pV1, pU1, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
			pDesF += nRealFirstPitch; pYF += nYP2;
			yuv422_to_color32_inner(pDesF, pYF, pV2, pU2, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
			pDesF += nRealFirstPitch; pYF += nYP2;
			pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
			// 第二场
			uv20_to_uv22_line_2_F(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
			yuv422_to_color32_inner(pDesS, pYS, pV1, pU1, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
			pDesS += nRealSecondPitch; pYS += nYP2;
			yuv422_to_color32_inner(pDesS, pYS, pV2, pU2, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
			pDesS += nRealSecondPitch; pYS += nYP2;
			pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
			// 中间行
			for (int i=2; i<nHalfHalf; i++)
			{
				// 第一场
				uv20_to_uv22_line_2(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesF, pYF, pV1, pU1, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
				pDesF += nRealFirstPitch; pYF += nYP2;
				yuv422_to_color32_inner(pDesF, pYF, pV2, pU2, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
				pDesF += nRealFirstPitch; pYF += nYP2;
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
				// 第二场
				uv20_to_uv22_line_2(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesS, pYS, pV1, pU1, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
				pDesS += nRealSecondPitch; pYS += nYP2;
				yuv422_to_color32_inner(pDesS, pYS, pV2, pU2, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
				pDesS += nRealSecondPitch; pYS += nYP2;
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
			}
			// 最后两行或一行（场为奇数行时）
			if (nHalfHalf & 0x01)       // 一行
			{
				// 第一场
				uv20_to_uv22_line_1_L(pTmpU, pTmpV, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesF, pYF, pV1, pU1, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
				// 第二场
				uv20_to_uv22_line_1_L(pTmpU, pTmpV, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesS, pYS, pV1, pU1, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
			}
			else    // 两行
			{
				// 第一场
				uv20_to_uv22_line_2_L(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesF, pYF, pV1, pU1, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
				pDesF += nRealFirstPitch; pYF += nYP2;
				yuv422_to_color32_inner(pDesF, pYF, pV2, pU2, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
				// 第二场
				uv20_to_uv22_line_2_L(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesS, pYS, pV1, pU1, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
				pDesS += nRealSecondPitch; pYS += nYP2;
				yuv422_to_color32_inner(pDesS, pYS, pV2, pU2, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
			}
		}
		else        // R 在低位
		{
			// 前两行
			// 第一场
			uv20_to_uv22_line_2_F(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
			yuv422_to_color32_inner(pDesF, pYF, pU1, pV1, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
			pDesF += nRealFirstPitch; pYF += nYP2;
			yuv422_to_color32_inner(pDesF, pYF, pU2, pV2, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
			pDesF += nRealFirstPitch; pYF += nYP2;
			pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
			// 第二场
			uv20_to_uv22_line_2_F(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
			yuv422_to_color32_inner(pDesS, pYS, pU1, pV1, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
			pDesS += nRealSecondPitch; pYS += nYP2;
			yuv422_to_color32_inner(pDesS, pYS, pU2, pV2, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
			pDesS += nRealSecondPitch; pYS += nYP2;
			pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
			// 中间行
			for (int i=2; i<nHalfHalf; i++)
			{
				// 第一场
				uv20_to_uv22_line_2(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesF, pYF, pU1, pV1, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
				pDesF += nRealFirstPitch; pYF += nYP2;
				yuv422_to_color32_inner(pDesF, pYF, pU2, pV2, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
				pDesF += nRealFirstPitch; pYF += nYP2;
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
				// 第二场
				uv20_to_uv22_line_2(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesS, pYS, pU1, pV1, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
				pDesS += nRealSecondPitch; pYS += nYP2;
				yuv422_to_color32_inner(pDesS, pYS, pU2, pV2, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
				pDesS += nRealSecondPitch; pYS += nYP2;
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
			}
			// 最后两行或一行（场为奇数行时）
			if (nHalfHalf & 0x01)       // 一行
			{
				// 第一场
				uv20_to_uv22_line_1_L(pTmpU, pTmpV, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesF, pYF, pU1, pV1, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
				// 第二场
				uv20_to_uv22_line_1_L(pTmpU, pTmpV, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesS, pYS, pU1, pV1, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
			}
			else    // 两行
			{
				// 第一场
				uv20_to_uv22_line_2_L(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesF, pYF, pU1, pV1, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
				pDesF += nRealFirstPitch; pYF += nYP2;
				yuv422_to_color32_inner(pDesF, pYF, pU2, pV2, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
				// 第二场
				uv20_to_uv22_line_2_L(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesS, pYS, pU1, pV1, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
				pDesS += nRealSecondPitch; pYS += nYP2;
				yuv422_to_color32_inner(pDesS, pYS, pU2, pV2, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
			}
		}
	}
	else        // 标清
	{
		if (bBGRA)      // B 在低位
		{
			// 前两行
			// 第一场
			uv20_to_uv22_line_2_F(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
			yuv422_to_color32_inner(pDesF, pYF, pV1, pU1, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
			pDesF += nRealFirstPitch; pYF += nYP2;
			yuv422_to_color32_inner(pDesF, pYF, pV2, pU2, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
			pDesF += nRealFirstPitch; pYF += nYP2;
			pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
			// 第二场
			uv20_to_uv22_line_2_F(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
			yuv422_to_color32_inner(pDesS, pYS, pV1, pU1, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
			pDesS += nRealSecondPitch; pYS += nYP2;
			yuv422_to_color32_inner(pDesS, pYS, pV2, pU2, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
			pDesS += nRealSecondPitch; pYS += nYP2;
			pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
			// 中间行
			for (int i=2; i<nHalfHalf; i++)
			{
				// 第一场
				uv20_to_uv22_line_2(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesF, pYF, pV1, pU1, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
				pDesF += nRealFirstPitch; pYF += nYP2;
				yuv422_to_color32_inner(pDesF, pYF, pV2, pU2, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
				pDesF += nRealFirstPitch; pYF += nYP2;
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
				// 第二场
				uv20_to_uv22_line_2(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesS, pYS, pV1, pU1, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
				pDesS += nRealSecondPitch; pYS += nYP2;
				yuv422_to_color32_inner(pDesS, pYS, pV2, pU2, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
				pDesS += nRealSecondPitch; pYS += nYP2;
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
			}
			// 最后两行或一行（场为奇数行时）
			if (nHalfHalf & 0x01)       // 一行
			{
				// 第一场
				uv20_to_uv22_line_1_L(pTmpU, pTmpV, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesF, pYF, pV1, pU1, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
				// 第二场
				uv20_to_uv22_line_1_L(pTmpU, pTmpV, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesS, pYS, pV1, pU1, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
			}
			else    // 两行
			{
				// 第一场
				uv20_to_uv22_line_2_L(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesF, pYF, pV1, pU1, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
				pDesF += nRealFirstPitch; pYF += nYP2;
				yuv422_to_color32_inner(pDesF, pYF, pV2, pU2, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
				// 第二场
				uv20_to_uv22_line_2_L(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesS, pYS, pV1, pU1, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
				pDesS += nRealSecondPitch; pYS += nYP2;
				yuv422_to_color32_inner(pDesS, pYS, pV2, pU2, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
			}
		}
		else        // R 在低位
		{
			// 前两行
			// 第一场
			uv20_to_uv22_line_2_F(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
			yuv422_to_color32_inner(pDesF, pYF, pU1, pV1, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
			pDesF += nRealFirstPitch; pYF += nYP2;
			yuv422_to_color32_inner(pDesF, pYF, pU2, pV2, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
			pDesF += nRealFirstPitch; pYF += nYP2;
			pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
			// 第二场
			uv20_to_uv22_line_2_F(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
			yuv422_to_color32_inner(pDesS, pYS, pU1, pV1, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
			pDesS += nRealSecondPitch; pYS += nYP2;
			yuv422_to_color32_inner(pDesS, pYS, pU2, pV2, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
			pDesS += nRealSecondPitch; pYS += nYP2;
			pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
			// 中间行
			for (int i=2; i<nHalfHalf; i++)
			{
				// 第一场
				uv20_to_uv22_line_2(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesF, pYF, pU1, pV1, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
				pDesF += nRealFirstPitch; pYF += nYP2;
				yuv422_to_color32_inner(pDesF, pYF, pU2, pV2, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
				pDesF += nRealFirstPitch; pYF += nYP2;
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
				// 第二场
				uv20_to_uv22_line_2(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesS, pYS, pU1, pV1, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
				pDesS += nRealSecondPitch; pYS += nYP2;
				yuv422_to_color32_inner(pDesS, pYS, pU2, pV2, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
				pDesS += nRealSecondPitch; pYS += nYP2;
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
			}
			// 最后两行或一行（场为奇数行时）
			if (nHalfHalf & 0x01)       // 一行
			{
				// 第一场
				uv20_to_uv22_line_1_L(pTmpU, pTmpV, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesF, pYF, pU1, pV1, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
				// 第二场
				uv20_to_uv22_line_1_L(pTmpU, pTmpV, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesS, pYS, pU1, pV1, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
			}
			else    // 两行
			{
				// 第一场
				uv20_to_uv22_line_2_L(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesF, pYF, pU1, pV1, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
				pDesF += nRealFirstPitch; pYF += nYP2;
				yuv422_to_color32_inner(pDesF, pYF, pU2, pV2, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
				pSrcU += nSrcUPitch; pSrcV += nSrcVPitch;
				// 第二场
				uv20_to_uv22_line_2_L(pTmpU, nUVPitch, pTmpV, nUVPitch, pSrcU, nUP2, pSrcV, nVP2, na16, nm16);
				yuv422_to_color32_inner(pDesS, pYS, pU1, pV1, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
				pDesS += nRealSecondPitch; pYS += nYP2;
				yuv422_to_color32_inner(pDesS, pYS, pU2, pV2, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
			}
		}
	}
	// 释放缓存
	_mm_free(pTmpU);
	_mm_free(pTmpV);
}

void yuv422_to_color32_frame(OUT BYTE* pDes32,           // *G@A 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）（Alpha 直接填 0xff）
							 IN int nDesPitch,
							 IN BYTE* pSrcY,             // Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
							 IN int nSrcYPitch,
							 IN BYTE* pSrcU,             // U 输入（存储方式 U0 U2 U4 ……）
							 IN int nSrcUPitch,
							 IN BYTE* pSrcV,             // V 输入（存储方式 V0 V2 V4 ……）
							 IN int nSrcVPitch,
							 IN int nWidth,              // 目标大小
							 IN int nHeight,
							 IN BOOL bHD,                // 高标清标志
							 IN BOOL bDesRevert,         // 图像是否行翻转标志
							 IN BOOL bBGRA)              // 像素内颜色分量的存储顺序，为 TRUE 表示低 B 在低位
{
	int na8 = ((nWidth - 1) >> 3), nm8 = (nWidth - (na8 << 3));
	BYTE* pDesLine = (bDesRevert) ? (pDes32 + (nHeight - 1) * nDesPitch) : pDes32;
	int nRealDesPitch = (bDesRevert) ? -nDesPitch : nDesPitch;

	if (bHD)        // 高清
	{
		if (bBGRA)      // B 在低位
		{
			for (int i=0; i<nHeight; i++)
			{
				yuv422_to_color32_inner(pDesLine, pSrcY, pSrcV, pSrcU, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
				pDesLine += nRealDesPitch;
				pSrcY += nSrcYPitch;
				pSrcU += nSrcUPitch;
				pSrcV += nSrcVPitch;
			}
		}
		else    // R 在低位
		{
			for (int i=0; i<nHeight; i++)
			{
				yuv422_to_color32_inner(pDesLine, pSrcY, pSrcU, pSrcV, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
				pDesLine += nRealDesPitch;
				pSrcY += nSrcYPitch;
				pSrcU += nSrcUPitch;
				pSrcV += nSrcVPitch;
			}
		}
	}
	else            // 标清
	{
		if (bBGRA)      // B 在低位
		{
			for (int i=0; i<nHeight; i++)
			{
				yuv422_to_color32_inner(pDesLine, pSrcY, pSrcV, pSrcU, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
				pDesLine += nRealDesPitch;
				pSrcY += nSrcYPitch;
				pSrcU += nSrcUPitch;
				pSrcV += nSrcVPitch;
			}
		}
		else    // R 在低位
		{
			for (int i=0; i<nHeight; i++)
			{
				yuv422_to_color32_inner(pDesLine, pSrcY, pSrcU, pSrcV, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
				pDesLine += nRealDesPitch;
				pSrcY += nSrcYPitch;
				pSrcU += nSrcUPitch;
				pSrcV += nSrcVPitch;
			}
		}
	}
}
void yuv422_to_color32_split(OUT BYTE* pDesFirst,        // 第一场 *G@A 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）（Alpha 直接填 0xff）
							 IN int nFirstPitch,
							 OUT BYTE* pDesSecond,       // 第二场 *G@A 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）（Alpha 直接填 0xff）
							 IN int nSecondPitch,
							 IN BYTE* pSrcY,             // Y 输入（存储方式 Y0 Y1 Y2 Y3 Y4 ……）
							 IN int nSrcYPitch,
							 IN BYTE* pSrcU,             // U 输入（存储方式 U0 U2 U4 ……）
							 IN int nSrcUPitch,
							 IN BYTE* pSrcV,             // V 输入（存储方式 V0 V2 V4 ……）
							 IN int nSrcVPitch,
							 IN int nWidth,              // 目标大小
							 IN int nHeight,
							 IN BOOL bHD,                // 高标清标志
							 IN BOOL bDesRevert,         // 图像是否行翻转标志
							 IN BOOL bBGRA)              // 像素内颜色分量的存储顺序，为 TRUE 表示低 B 在低位
{
	int na8 = ((nWidth - 1) >> 3), nm8 = (nWidth - (na8 << 3));
	int nHalfHeight = (nHeight >> 1);
	BYTE* pDesLineF = (bDesRevert) ? (pDesFirst + (nHalfHeight - 1) * nFirstPitch) : pDesFirst;
	BYTE* pDesLineS = (bDesRevert) ? (pDesSecond + (nHalfHeight - 1) * nSecondPitch) : pDesSecond;
	int nRealFirstPitch = (bDesRevert) ? -nFirstPitch : nFirstPitch;
	int nRealSecondPitch = (bDesRevert) ? -nSecondPitch : nSecondPitch;

	if (bHD)        // 高清
	{
		if (bBGRA)      // B 在低位
		{
			for (int i=0; i<nHalfHeight; i++)
			{
				// 第一场
				yuv422_to_color32_inner(pDesLineF, pSrcY, pSrcV, pSrcU, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
				pDesLineF += nRealFirstPitch;
				pSrcY += nSrcYPitch;
				pSrcU += nSrcUPitch;
				pSrcV += nSrcVPitch;
				// 第二场
				yuv422_to_color32_inner(pDesLineS, pSrcY, pSrcV, pSrcU, na8, nm8, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
				pDesLineS += nRealSecondPitch;
				pSrcY += nSrcYPitch;
				pSrcU += nSrcUPitch;
				pSrcV += nSrcVPitch;
			}
		}
		else    // R 在低位
		{
			for (int i=0; i<nHalfHeight; i++)
			{
				// 第一场
				yuv422_to_color32_inner(pDesLineF, pSrcY, pSrcU, pSrcV, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
				pDesLineF += nRealFirstPitch;
				pSrcY += nSrcYPitch;
				pSrcU += nSrcUPitch;
				pSrcV += nSrcVPitch;
				// 第二场
				yuv422_to_color32_inner(pDesLineS, pSrcY, pSrcU, pSrcV, na8, nm8, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
				pDesLineS += nRealSecondPitch;
				pSrcY += nSrcYPitch;
				pSrcU += nSrcUPitch;
				pSrcV += nSrcVPitch;
			}
		}
	}
	else            // 标清
	{
		if (bBGRA)      // B 在低位
		{
			for (int i=0; i<nHalfHeight; i++)
			{
				// 第一场
				yuv422_to_color32_inner(pDesLineF, pSrcY, pSrcV, pSrcU, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
				pDesLineF += nRealFirstPitch;
				pSrcY += nSrcYPitch;
				pSrcU += nSrcUPitch;
				pSrcV += nSrcVPitch;
				// 第二场
				yuv422_to_color32_inner(pDesLineS, pSrcY, pSrcV, pSrcU, na8, nm8, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
				pDesLineS += nRealSecondPitch;
				pSrcY += nSrcYPitch;
				pSrcU += nSrcUPitch;
				pSrcV += nSrcVPitch;
			}
		}
		else    // R 在低位
		{
			for (int i=0; i<nHalfHeight; i++)
			{
				// 第一场
				yuv422_to_color32_inner(pDesLineF, pSrcY, pSrcU, pSrcV, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
				pDesLineF += nRealFirstPitch;
				pSrcY += nSrcYPitch;
				pSrcU += nSrcUPitch;
				pSrcV += nSrcVPitch;
				// 第二场
				yuv422_to_color32_inner(pDesLineS, pSrcY, pSrcU, pSrcV, na8, nm8, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
				pDesLineS += nRealSecondPitch;
				pSrcY += nSrcYPitch;
				pSrcU += nSrcUPitch;
				pSrcV += nSrcVPitch;
			}
		}
	}
}

// 重组数据

// 需要按场处理，在场内隔行插值，每次输出两行
// 前两行
void yuv420_to_yuyv_line_2_F(OUT BYTE* yuyv,        // yuyv 输出，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
							 IN int yuyv_p,
							 IN BYTE* y,            // y 输入，存放方式 Y0 Y1 Y2 Y3 ……
							 IN int y_p_2,
							 IN BYTE* u,            // u 输入，存放方式 U0 U2 U4 U6 ……
							 IN int u_p_2,
							 IN BYTE* v,            // v 输入，存放方式 V0 V2 V4 V6 ……
							 IN int v_p_2,
							 IN int na16,           // 一行像素宽度除以 16 的商与余数（注意目标为 16 位的缓存）
							 IN int nm16)
{
}
// 中间行
void yuv420_to_yuyv_line_2(OUT BYTE* yuyv,        // yuyv 输出，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
						   IN int yuyv_p,
						   IN BYTE* y,            // y 输入，存放方式 Y0 Y1 Y2 Y3 ……
						   IN int y_p_2,
						   IN BYTE* u,            // u 输入，存放方式 UU0 UU2 UU4 UU6 ……
						   IN int u_p_2,
						   IN BYTE* v,            // v 输入，存放方式 VV0 VV2 VV4 VV6 ……
						   IN int v_p_2,
						   IN int na16,           // 一行像素宽度除以 16 的商与余数（注意目标为 16 位的缓存）
						   IN int nm16)
{
}
// 最后两行
void yuv420_to_yuyv_line_2_L(OUT BYTE* yuyv,        // yuyv 输出，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
							 IN int yuyv_p,
							 IN BYTE* y,            // y 输入，存放方式 Y0 Y1 Y2 Y3 ……
							 IN int y_p_2,
							 IN BYTE* u,            // u 输入，存放方式 UU0 UU2 UU4 UU6 ……
							 IN int u_p_2,
							 IN BYTE* v,            // v 输入，存放方式 VV0 VV2 VV4 VV6 ……
							 IN int v_p_2,
							 IN int na16,           // 一行像素宽度除以 16 的商与余数（注意目标为 16 位的缓存）
							 IN int nm16)
{
}
// 最后一行
void yuv420_to_yuyv_line_1_L(OUT BYTE* yuyv,        // yuyv 输出，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
							 IN BYTE* y,            // y 输入，存放方式 Y0 Y1 Y2 Y3 ……
							 IN BYTE* u,            // u 输入，存放方式 UU0 UU2 UU4 UU6 ……
							 IN int u_p_2,
							 IN BYTE* v,            // v 输入，存放方式 VV0 VV2 VV4 VV6 ……
							 IN int v_p_2,
							 IN int na16,           // 一行像素宽度除以 16 的商与余数（注意目标为 16 位的缓存）
							 IN int nm16)
{
	assert(false);
}

void yuv422_to_yuyv_line(OUT BYTE* yuyv,        // yuyv 输出，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
						 IN BYTE* y,            // y 输入，存放方式 Y0 Y1 Y2 Y3 ……
						 IN BYTE* u,            // u 输入，存放方式 U0 U2 U4 U6 ……
						 IN BYTE* v,            // v 输入，存放方式 V0 V2 V4 V6 ……
						 IN int na16,           // 一行像素宽度除以 16 的商与余数（注意目标为 16 位的缓存）
						 IN int nm16)
{
	yuv422_to_yuyv_line_x64(yuyv, y, u, v, na16, nm16);
}

// 一行 Buffer 颜色空间转换

// 分片存储的源 YUV422 或 YUV420

// 前两行
void uv20_to_uv22_line_2_F(OUT void* u_o,         // 422 u 输出，存储方式 U0 U2 U4 U6 ……（两行）
						   IN int u_o_p,
						   OUT void* v_o,         // 422 v 输出，存储方式 V0 V2 V4 V6 ……（两行）
						   IN int v_o_p,
						   IN void* u_i,          // 420 u 输入，存储方式 UU0 UU2 UU4 UU6 ……
						   IN int u_i_p_2,
						   IN void* v_i,          // 420 v 输入，存储方式 VV0 VV2 VV4 VV6 ……
						   IN int v_i_p_2,
						   IN int na16,           // 一行像素宽度的一半（因为一行内的 UV 与 Y 为 422 的关系）除以 16 的商与余数
						   IN int nm16)
{
	uv20_to_uv22_line_2_F_x64(u_o, u_o_p, v_o, v_o_p, u_i, u_i_p_2, v_i, v_i_p_2, na16, nm16);
}
// 中间行
void uv20_to_uv22_line_2(OUT void* u_o,         // 422 u 输出，存储方式 U0 U2 U4 U6 ……（两行）
						 IN int u_o_p,
						 OUT void* v_o,         // 422 v 输出，存储方式 V0 V2 V4 V6 ……（两行）
						 IN int v_o_p,
						 IN void* u_i,          // 420 u 输入，存储方式 UU0 UU2 UU4 UU6 ……
						 IN int u_i_p_2,
						 IN void* v_i,          // 420 v 输入，存储方式 VV0 VV2 VV4 VV6 ……
						 IN int v_i_p_2,
						 IN int na16,           // 一行像素宽度的一半（因为一行内的 UV 与 Y 为 422 的关系）除以 16 的商与余数
						 IN int nm16)
{
	uv20_to_uv22_line_2_x64(u_o, u_o_p, v_o, v_o_p, u_i, u_i_p_2, v_i, v_i_p_2, na16, nm16);
}
// 最后两行
void uv20_to_uv22_line_2_L(OUT void* u_o,         // 422 u 输出，存储方式 U0 U2 U4 U6 ……（两行）
						   IN int u_o_p,
						   OUT void* v_o,         // 422 v 输出，存储方式 V0 V2 V4 V6 ……（两行）
						   IN int v_o_p,
						   IN void* u_i,          // 420 u 输入，存储方式 UU0 UU2 UU4 UU6 ……
						   IN int u_i_p_2,
						   IN void* v_i,          // 420 v 输入，存储方式 VV0 VV2 VV4 VV6 ……
						   IN int v_i_p_2,
						   IN int na16,           // 一行像素宽度的一半（因为一行内的 UV 与 Y 为 422 的关系）除以 16 的商与余数
						   IN int nm16)
{
	uv20_to_uv22_line_2_L_x64(u_o, u_o_p, v_o, v_o_p, u_i, u_i_p_2, v_i, v_i_p_2, na16, nm16);
}
// 最后一行
void uv20_to_uv22_line_1_L(OUT void* u_o,         // 422 u 输出，存储方式 U0 U2 U4 U6 ……（两行）
						   OUT void* v_o,         // 422 v 输出，存储方式 V0 V2 V4 V6 ……（两行）
						   IN void* u_i,          // 420 u 输入，存储方式 UU0 UU2 UU4 UU6 ……
						   IN int u_i_p_2,
						   IN void* v_i,          // 420 v 输入，存储方式 VV0 VV2 VV4 VV6 ……
						   IN int v_i_p_2,
						   IN int na16,           // 一行像素宽度的一半（因为一行内的 UV 与 Y 为 422 的关系）除以 16 的商与余数
						   IN int nm16)
{
	uv20_to_uv22_line_1_L_x64(u_o, v_o, u_i, u_i_p_2, v_i, v_i_p_2, na16, nm16);
}

void yuv422_to_color32_line(OUT void* color32,      // *G@A 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）（Alpha 直接填 0xff）
							IN void* y,             // y 输入，存放方式 Y0 Y1 Y2 Y3 ……
							IN void* u,             // u 输入，存放方式 UU0 UU2 UU4 UU6 ……
							IN void* v,             // v 输入，存放方式 VV0 VV2 VV4 VV6 ……
							IN int na16,            // 源字节宽度减 4 后除以 16 的商与余数
							IN int nm16,
							IN int bHD,             // 高标清标志
							IN BOOL bBGRA)          // 像素内颜色分量的存储顺序，为 TRUE 表示低 B 在低位
{
	if (bHD)    // 高清
	{
		if (bBGRA)      // B 在低位
		{
			yuv422_to_color32_inner(color32, y, v, u, na16, nm16, g_nB_U_HD, g_nG_V_HD, g_nG_U_HD, g_nR_V_HD);
		}
		else       // R 在低位
		{
			yuv422_to_color32_inner(color32, y, u, v, na16, nm16, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
		}
	}
	else    // 标清
	{
		if (bBGRA)      // B 在低位
		{
			yuv422_to_color32_inner(color32, y, v, u, na16, nm16, g_nB_U_SD, g_nG_V_SD, g_nG_U_SD, g_nR_V_SD);
		}
		else       // R 在低位
		{
			yuv422_to_color32_inner(color32, y, u, v, na16, nm16, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
		}
	}
}
void yuv422_to_color32_inner(OUT void* color32,     // *G@A 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
							 IN void* y,            // y 输入，存放方式 Y0 Y1 Y2 Y3 ……
							 IN void* u,            // u 输入，存放方式 UU0 UU2 UU4 UU6 ……
							 IN void* v,            // v 输入，存放方式 VV0 VV2 VV4 VV6 ……
							 IN int na8,		    // 源像素宽度减 1 后除以 8 的商与余数
							 IN int nm8,
							 IN int* pnR_V,         // 系数
							 IN int* pnG_U,         // （通过交换 U 与 V 可以做到 RGBA 与 BGRA 的兼容）
							 IN int* pnG_V,         // （输入 u 与 v 交换，pnR_V 与 pnB_U 交换，pnG_V 与 pnG_U 交换即可实现）
							 IN int* pnB_U)
{
	yuv422_to_color32_inner_x64(color32, y, u, v, na8, nm8, pnR_V, pnG_U, pnG_V, pnB_U);
}

// 保留 Alpha 通道，直接丢弃第二个 UV
void color32_to_yuyva_line(OUT void* yuyv,		// 目标 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
						   OUT void* alpha,	    // 目标 alpha 缓存，存放方式 A0 A1 A2 A3 ……
						   IN void* color32,	// 源 *g@a 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
						   IN int na16,	    	// 源字节宽度除以 16 的商与余数
						   IN int nm16,
						   IN BOOL bHD,         // 高标清标志
						   IN BOOL bBGRA)       // 像素内颜色分量的存储顺序，为 TRUE 表示低 B 在低位
{
	if (bHD)    // 高清
	{
		if (bBGRA)      // B 在低位
		{
			color32_to_yuyva_inner(yuyv, alpha, color32, na16, nm16, g_nY32_RB_HD, g_nY32_G_HD, g_nY32_RGB_HD, g_nUV_RGB_HD, g_nU_RGB_HD, g_nV_RGB_HD);
		}
		else    // R 在低位
		{
			color32_to_yuyva_inner(yuyv, alpha, color32, na16, nm16, g_nY32_BR_HD, g_nY32_G_HD, g_nY32_BGR_HD, g_nUV_BGR_HD, g_nU_BGR_HD, g_nV_BGR_HD);
		}
	}
	else    // 标清
	{
		if (bBGRA)      // B 在低位
		{
			color32_to_yuyva_inner(yuyv, alpha, color32, na16, nm16, g_nY32_RB_SD, g_nY32_G_SD, g_nY32_RGB_SD, g_nUV_RGB_SD, g_nU_RGB_SD, g_nV_RGB_SD);
		}
		else    // R 在低位
		{
			color32_to_yuyva_inner(yuyv, alpha, color32, na16, nm16, g_nY32_BR_SD, g_nY32_G_SD, g_nY32_BGR_SD, g_nUV_BGR_SD, g_nU_BGR_SD, g_nV_BGR_SD);
		}
	}
}
void color32_to_yuyva_inner(OUT void* yuyv,	        // 目标 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
							OUT void* alpha,    	// 目标 alpha 缓存，存放方式 A0 A1 A2 A3 ……
							IN void* color32,	    // 源 *g@a 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
							IN int na16,    		// 源字节宽度除以 16 的商与余数
							IN int nm16,
							IN int* pnY32_BR,       // 系数（通过变更 R 分量与 B 分量的系数位置可以做到 BGRA 与 RGBA 的兼容）
							IN int* pnY32_G,        // BR 表示 R 在低位
							IN int* pnY32_BGR,
							IN int* pnUV_BGR,
							IN int* pnU_BGR,
							IN int* pnV_BGR)
{
	color32_to_yuyva_inner_x64(yuyv, alpha, color32, na16, nm16, pnY32_BR, pnY32_G, pnY32_BGR, pnUV_BGR, pnU_BGR, pnV_BGR);
}
// 丢弃 Alpha 通道，直接丢弃第二个 UV
void color32_to_yuyv_line(OUT void* yuyv,		// 目标 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
						  IN void* color32,	    // 源 *g@a 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
						  IN int na16,	    	// 源字节宽度除以 16 的商与余数
						  IN int nm16,
						  IN BOOL bHD,          // 高标清标志
						  IN BOOL bBGRA)        // 像素内颜色分量的存储顺序，为 TRUE 表示低 B 在低位
{
	if (bHD)    // 高清
	{
		if (bBGRA)      // B 在低位
		{
			color32_to_yuyv_inner(yuyv, color32, na16, nm16, g_nY32_RB_HD, g_nY32_G_HD, g_nY32_RGB_HD, g_nUV_RGB_HD, g_nU_RGB_HD, g_nV_RGB_HD);
		}
		else        // R 在低位
		{
			color32_to_yuyv_inner(yuyv, color32, na16, nm16, g_nY32_BR_HD, g_nY32_G_HD, g_nY32_BGR_HD, g_nUV_BGR_HD, g_nU_BGR_HD, g_nV_BGR_HD);
		}
	}
	else
	{
		if (bBGRA)      // B 在低位
		{
			color32_to_yuyv_inner(yuyv, color32, na16, nm16, g_nY32_RB_SD, g_nY32_G_SD, g_nY32_RGB_SD, g_nUV_RGB_SD, g_nU_RGB_SD, g_nV_RGB_SD);
		}
		else        // R 在低位
		{
			color32_to_yuyv_inner(yuyv, color32, na16, nm16, g_nY32_BR_SD, g_nY32_G_SD, g_nY32_BGR_SD, g_nUV_BGR_SD, g_nU_BGR_SD, g_nV_BGR_SD);
		}
	}
}
void color32_to_yuyv_inner(OUT void* yuyv, 	    // 目标 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
						   IN void* color32,	// 源 *g@a 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
						   IN int na16,		    // 源字节宽度除以 16 的商与余数
						   IN int nm16,
						   IN int* pnY32_BR,    // 系数（通过变更 R 分量与 B 分量的系数位置可以做到 BGRA 与 RGBA 的兼容）
						   IN int* pnY32_G,     // BR 表示 R 在低位
						   IN int* pnY32_BGR,
						   IN int* pnUV_BGR,
						   IN int* pnU_BGR,
						   IN int* pnV_BGR)
{
	color32_to_yuyv_inner_x64(yuyv, color32, na16, nm16, pnY32_BR, pnY32_G, pnY32_BGR, pnUV_BGR, pnU_BGR, pnV_BGR);
}
// 使用 Alpha 通道，前后插值获得第二个 UV
void yuyva_to_color32_line(OUT void* color32,    // *G@A 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
						   IN void* yuyv,	 	 // 源 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
						   IN void* alpha,		 // 源 alpha 缓存，存放方式 A0 A1 A2 A3 ……
						   IN int na16,		     // 源字节宽度减 4 后除以 16 的商与余数
						   IN int nm16,
						   IN BOOL bHD,          // 高标清标志
						   IN BOOL bBGRA)        // 像素内颜色分量的存储顺序，为 TRUE 表示低 B 在低位
{
	if (bHD)    // 高清
	{
		if (bBGRA)      // B 在低位
		{
			yuyva_to_bgra_inner(color32, yuyv, alpha, na16, nm16, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
		}
		else       // R 在低位
		{
			yuyva_to_rgba_inner(color32, yuyv, alpha, na16, nm16, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
		}
	}
	else    // 标清
	{
		if (bBGRA)      // B 在低位
		{
			yuyva_to_bgra_inner(color32, yuyv, alpha, na16, nm16, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
		}
		else       // R 在低位
		{
			yuyva_to_rgba_inner(color32, yuyv, alpha, na16, nm16, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
		}
	}
}
void yuyva_to_rgba_inner(OUT void* rgba,        // RGBA 缓存，存放方式 R0 G0 B0 A0 R1 G1 B1 A1 ……
						 IN void* yuyv, 		// 源 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
						 IN void* alpha,		// 源 alpha 缓存，存放方式 A0 A1 A2 A3 ……
						 IN int na16,		    // 源字节宽度减 4 后除以 16 的商与余数
						 IN int nm16,
						 IN int* pnR_V,         // 系数（无法做到 RGBA 与 BGRA 的兼容）
						 IN int* pnG_U,       
						 IN int* pnG_V,
						 IN int* pnB_U)
{
	yuyva_to_rgba_inner_x64(rgba, yuyv, alpha, na16, nm16, pnR_V, pnG_U, pnG_V, pnB_U);
}
void yuyva_to_bgra_inner(OUT void* bgra,        // BGRA 缓存，存放方式 B0 G0 R0 A0 B1 G1 R1 A1 ……
						 IN void* yuyv, 		// 源 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
						 IN void* alpha,		// 源 alpha 缓存，存放方式 A0 A1 A2 A3 ……
						 IN int na16,		    // 源字节宽度减 4 后除以 16 的商与余数
						 IN int nm16,
						 IN int* pnR_V,         // 系数（无法做到 RGBA 与 BGRA 的兼容）
						 IN int* pnG_U,       
						 IN int* pnG_V,
						 IN int* pnB_U)
{
	yuyva_to_bgra_inner_x64(bgra, yuyv, alpha, na16, nm16, pnR_V, pnG_U, pnG_V, pnB_U);
}
// 补充 Alpha 通道为 255，前后插值获得第二个 UV
void yuyv_to_color32_line(OUT void* color32,    // *G@A 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）（Alpha 直接填 0xff）
						  IN void* yuyv,		// 源 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
						  IN int na16,			// 源字节宽度减 4 后除以 16 的商与余数
						  IN int nm16,
						  IN int bHD,           // 高标清标志
						  IN BOOL bBGRA)        // 像素内颜色分量的存储顺序，为 TRUE 表示低 B 在低位
{
	if (bHD)    // 高清
	{
		if (bBGRA)      // B 在低位
		{
			yuyv_to_bgra_inner(color32, yuyv, na16, nm16, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
		}
		else        // R 在低位
		{
			yuyv_to_rgba_inner(color32, yuyv, na16, nm16, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
		}
	}
	else    // 标清
	{
		if (bBGRA)      // B 在低位
		{
			yuyv_to_bgra_inner(color32, yuyv, na16, nm16, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
		}
		else        // R 在低位
		{
			yuyv_to_rgba_inner(color32, yuyv, na16, nm16, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
		}
	}
}
void yuyv_to_rgba_inner(OUT void* rgba,	        // RGBA 缓存，存放方式 R0 G0 B0 A0 R1 G1 B1 A1 ……
						IN void* yuyv, 		// 源 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
						IN int na16,		    // 源字节宽度减 4 后除以 16 的商与余数
						IN int nm16,
						IN int* pnR_V,        // 系数（无法做到 RGBA 与 BGRA 的兼容）
						IN int* pnG_U,
						IN int* pnG_V,
						IN int* pnB_U)
{
	yuyv_to_rgba_inner_x64(rgba, yuyv, na16, nm16, pnR_V, pnG_U, pnG_V, pnB_U);
};
void yuyv_to_bgra_inner(OUT void* bgra,     // BGRA 缓存，存放方式 B0 G0 R0 A0 B1 G1 R1 A1 ……
						IN void* yuyv, 		// 源 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
						IN int na16,		// 源字节宽度减 4 后除以 16 的商与余数
						IN int nm16,
						IN int* pnR_V,      // 系数（无法做到 RGBA 与 BGRA 的兼容）
						IN int* pnG_U,
						IN int* pnG_V,
						IN int* pnB_U)
{
	yuyv_to_bgra_inner_x64(bgra, yuyv, na16, nm16, pnR_V, pnG_U, pnG_V, pnB_U);
}
// 直接丢弃第二个 UV
void color24_to_yuyv_line(OUT void* yuyv,		// 目标 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
						  IN void* color24,		// *G@ 输入，存放方式 *0 G0 @0 *1 G1 @1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
						  IN int na12,			// 源字节宽度减 4 后除以 12 的商
						  IN int nmp,			// 做完 na12 循环后剩余的像素
						  IN int bHD,           // 高标清标志
						  IN BOOL bBGR)         // 像素内颜色分量的存储顺序，为 TRUE 表示低 B 在低位
{
	if (bHD)    // 高清
	{
		if (bBGR)      // B 在低位
		{
			color24_to_yuyv_inner(yuyv, color24, na12, nmp, g_nY24_BGR_HD, g_nY32_RGB_HD, g_nU_RGB_HD, g_nV_RGB_HD, g_nUV_RGB_HD);
		}
		else    // R 在低位
		{
			color24_to_yuyv_inner(yuyv, color24, na12, nmp, g_nY24_RGB_HD, g_nY32_BGR_HD, g_nU_BGR_HD, g_nV_BGR_HD, g_nUV_BGR_HD);
		}
	}
	else    // 标清
	{
		if (bBGR)      // B 在低位
		{
			color24_to_yuyv_inner(yuyv, color24, na12, nmp, g_nY24_BGR_SD, g_nY32_RGB_SD, g_nU_RGB_SD, g_nV_RGB_SD, g_nUV_RGB_SD);
		}
		else    // R 在低位
		{
			color24_to_yuyv_inner(yuyv, color24, na12, nmp, g_nY24_RGB_SD, g_nY32_BGR_SD, g_nU_BGR_SD, g_nV_BGR_SD, g_nUV_BGR_SD);
		}
	}
}
void color24_to_yuyv_inner(OUT void* yuyv,		// 目标 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
						   IN void* color24,	// *G@ 输入，存放方式 *0 G0 @0 *1 G1 @1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
						   IN int na12,			// 源字节宽度减 4 后除以 12 的商
						   IN int nmp,			// 做完 na12 循环后剩余的像素'
						   IN int* pnY24_RGB,   // 系数（通过变更 R 分量与 B 分量的系数位置可以做到 BGRA 与 RGBA 的兼容）
						   IN int* pnY32_BGR,
						   IN int* pnU_BGR,
						   IN int* pnV_BGR,
						   IN int* pnUV_BGR)
{
	color24_to_yuyv_inner_x64(yuyv, color24, na12, nmp, pnY24_RGB, pnY32_BGR, pnU_BGR, pnV_BGR, pnUV_BGR);
}
// 前后插值获得第二个 UV
void yuyv_to_color24_line(OUT void* color24,	// *G@ 缓存，存放方式 *0 G0 @0 *1 G1 @1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
						  IN void* yuyv,		// 源 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
						  IN int na16,			// 源字节宽度减 4 后除以 16 的商与余数
						  IN int nm16,
						  IN int bHD,           // 高标清标志
						  IN BOOL bBGR)         // 像素内颜色分量的存储顺序，为 TRUE 表示低 B 在低位
{
	if (bHD)    // 高清
	{
		if (bBGR)       // B 在低位
		{
			yuyv_to_color24_inner(color24, yuyv, na16, nm16, g_nB_U_HD, g_nG_U_HD, g_nG_V_HD, g_nR_V_HD);
		}
		else        // R 在低位
		{
			yuyv_to_color24_inner(color24, yuyv, na16, nm16, g_nR_V_HD, g_nG_U_HD, g_nG_V_HD, g_nB_U_HD);
		}
	}
	else    // 标清
	{
		if (bBGR)       // B 在低位
		{
			yuyv_to_color24_inner(color24, yuyv, na16, nm16, g_nB_U_SD, g_nG_U_SD, g_nG_V_SD, g_nR_V_SD);
		}
		else        // R 在低位
		{
			yuyv_to_color24_inner(color24, yuyv, na16, nm16, g_nR_V_SD, g_nG_U_SD, g_nG_V_SD, g_nB_U_SD);
		}
	}
}
void yuyv_to_color24_inner(OUT void* color24,   // *G@ 缓存，存放方式 *0 G0 @0 *1 G1 @1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
						   IN void* yuyv,		// 源 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
						   IN int na16,			// 源字节宽度减 4 后除以 16 的商与余数
						   IN int nm16,
						   IN int* pnR_V,       // 系数（通过变更 R 分量与 B 分量的系数位置可以做到 BGRA 与 RGBA 的兼容）
						   IN int* pnG_U,       // （此处只需要将 pnR_V 与 pnB_U 的输入交换即可实现）
						   IN int* pnG_V,
						   IN int* pnB_U)
{
	yuyv_to_color24_inner_x64(color24, yuyv, na16, nm16, pnR_V, pnG_U, pnG_V, pnB_U);
}

// 一行 24 位缓存到 32 位缓存的转换，填充统一的 Alpha 通道
// 很显然 24 位缓存会四字节对齐，因此在处理最后几个像素时要充分利用这个特点
void color24_to_color32(OUT void* color32,	// *G@A 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……
						IN void* color24,	// *G@ 输入，存放方式 *0 G0 @0 *1 G1 @1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
						IN DWORD dwAlpha,	// Alpha 通道值，要求以这种方式提供：0xAA000000
						IN int nap5,		// 源像素宽度除以 5 的商与余数（正好满足十六字节对齐）
						IN int nmp5)
{
	color24_to_color32_x64(color32, color24, dwAlpha, nap5, nmp5);
}

void _memset_dword_line_align( OUT void *des, /* 目标 */ IN DWORD value, /* 设置值 */ IN int na16, /* 宽度除以 16 的商 */ IN int nm16 )
{
	_memset_dword_line_align_x64( des, value, na16,nm16);
}
//////////////////////////////////////////////////////////////////////////
// file end
