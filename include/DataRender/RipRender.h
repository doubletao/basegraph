#ifndef __DSBUFFEROP_ASM_H__
#define __DSBUFFEROP_ASM_H__
extern "C" void	_memset_dword_line_x64(OUT void *des,	// 目标
							IN DWORD value,				// 设置值
							IN int na16,				// 宽度除以 16 的商
							IN int nm16);				// 宽度除以 16 的余数

extern "C" void _memset_dword_line_align_x64(OUT void *des,	// 目标
							IN DWORD value,			// 设置值
							IN int na16,			// 宽度除以 16 的商
							IN int nm16);			// 宽度除以 16 的余数

extern "C" void memset_alpha_line_all_x64(IN OUT void *des,	// 目标
							IN char alpha,			// 填充值
							IN int na16,			// 宽度除以 16 的商
							IN int nm16);			// 宽度除以 16 的余数

extern "C" void _memcpy_line_align_x64(OUT void *des,		// 目标
							IN const void *src,		// 源
							IN int na16,			// 宽度除以 16 的商
							IN int nm16);			// 宽度除以 16 的余数

extern "C" void _memcpy_line_x64(OUT void *des,			// 目标
							IN const void *src,		// 源
							IN int na16,			// 宽度除以 16 的商
							IN int nm16);			// 宽度除以 16 的余数

extern "C" void memset_alpha_line_mask_x64(IN OUT void *des,	// 目标
							IN char alpha,			// 填充值
							IN DWORD color,			// 只填充颜色值
							IN int na16,			// 宽度除以 16 的商
							IN int nm16);			// 宽度除以 16 的余数
	
extern "C" void memset_alpha_line_notmask_x64(IN OUT void *des,	// 目标
							IN char alpha,			// 填充值
							IN DWORD notcolor,		// 不填充颜色值
							IN int na16,			// 宽度除以 16 的商
							IN int nm16);			// 宽度除以 16 的余数

extern "C" void bgra32_scale_alpha_line_x64(IN OUT void *des,	// 目标
							IN char alpha,			// 缩放值
							IN int na16,			// 宽度除以 16 的商
							IN int nm16);			// 宽度除以 16 的余数

extern "C" void bgra32_line_mix_x64(IN OUT void *des,	// 目标
							IN void *src,			// 源
							IN int na16,			// 宽度除以 16 的商
							IN int nm16,			// 宽度除以 16 的余数
							IN void *ka_table);		// 256 * 256 * sizeof(DWORD) 的 KA 表

extern "C" void bgra32_bl_copy_line_x64(OUT void* des,		// 目标拷贝行
							IN int dcw,				// 目标拷贝行的宽度
							IN void* src,			// 源拷贝行
							IN int src_pitch,		// 源图一行的字节数
							IN void* pPowerX,		// 目标映射到源的 X 方向上的列权重
							IN void* pMapX,			// 目标映射到源的 X 方向上的列索引
							IN DWORD powerY);		// 目标行映射到源行的行权重
extern "C" void color24_to_color32_x64(OUT void* color32,	// *G@A 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……
						IN void* color24,	// *G@ 输入，存放方式 *0 G0 @0 *1 G1 @1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
						IN DWORD dwAlpha,	// Alpha 通道值，要求以这种方式提供：0xAA000000
						IN int nap5,		// 源像素宽度除以 5 的商与余数（正好满足十六字节对齐）
						IN int nmp5);
extern "C" void yuyv_to_color24_inner_x64(OUT void* color24,   // *G@ 缓存，存放方式 *0 G0 @0 *1 G1 @1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
						   IN void* yuyv,		// 源 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
						   IN int na16,			// 源字节宽度减 4 后除以 16 的商与余数
						   IN int nm16,
						   IN int* pnR_V,       // 系数（通过变更 R 分量与 B 分量的系数位置可以做到 BGRA 与 RGBA 的兼容）
						   IN int* pnG_U,       // （此处只需要将 pnR_V 与 pnB_U 的输入交换即可实现）
						   IN int* pnG_V,
						   IN int* pnB_U);

extern "C" void _memcpy_rev_char_x64(OUT void* des,		// 目标
							 IN void* src,		// 源
							 IN int nlen,		// 宽度（用以 seek 到最后一段起始点）
							 IN int na16,		// 宽度除以 16 的商
							 IN int nm16);		// 宽度除以 16 的余数

extern "C" void _memcpy_rev_dword_x64(OUT void* des,	// 目标
						   IN void* src,		// 源
						   IN int nlen,			// 宽度（用以 seek 到最后一段起始点）
						   IN int na16,			// 宽度除以 16 的商
						   IN int nm16);		// 宽度除以 16 的余数

extern "C" void _memcpy_dword_col_x64(OUT void* des,	// 目标起点
						   IN void* src,		// 源
						   IN int npitch,	// 目标一行的宽度（符号决定目标指针的前进方向）
						   IN int na16,		// 拷贝列高度乘于 4 后除以 16 的商
						   IN int nm16);		// 拷贝列高度乘于 4 后除以 16 的余数

extern "C" void _memset_char_line_x64(OUT void *des,	// 目标
						   IN char value,	// 设置值
						   IN int na16,		// 宽度除以 16 的商
						   IN int nm16);	// 宽度除以 16 的余数

extern "C" void copy_dword_line_rgb_mula_x64(OUT void *des,	// 目标
							  IN void *src,	// 设置值
							  IN int na16,		// 宽度除以 16 的商
							  IN int nm16);		// 宽度除以 16 的余数

extern "C" void memset_dword_line_rgb_mula_x64(OUT void *des,	// 目标
								IN DWORD value,	// 设置值
								IN int na16,		// 宽度除以 16 的商
								IN int nm16);		// 宽度除以 16 的余数

extern "C" void copy_dword_line_mula_x64(OUT void *des,	// 目标
										 IN void *src,	// 设置值
										 IN int na16,		// 宽度除以 16 的商
										 IN int nm16);		// 宽度除以 16 的余数

extern "C" void copy_dword_line_mula_invert_x64(OUT void *des,	// 目标
										 IN void *src,	// 设置值
										 IN int na16,		// 宽度除以 16 的商
										 IN int nm16);		// 宽度除以 16 的余数


extern "C" void memset_dword_line_rgb_x64(OUT void *des,	// 目标
						   IN DWORD value,	// 设置值
						   IN int na16,		// 宽度除以 16 的商
						   IN int nm16);		// 宽度除以 16 的余数

extern "C" void memrev_char_line_x64(IN OUT void *head,	// 缓存起始位置
					  IN OUT void *end,		// 缓存结束位置（end = head + nlen）
					  IN int na16,			// 一半宽度除以 16 的商
					  IN int nm16);			// 一半宽度除以 16 的余数

extern "C" void memrev_dword_line_x64(IN OUT void *head,	// 缓存起始位置
								  IN OUT void *end,	// 缓存结束位置（end = head + nlen）
								  IN int na16,			// 一半像素宽度的字节宽度除以 16 的商
								  IN int nm16);			// 一半像素宽度的字节宽度除以 16 的余数

extern "C" void bgra32_line_mix_notsrc_x64(IN OUT void *des,		// 目标
									   IN void *src,			// 源
									   IN int na16,			// 宽度除以 16 的商
									   IN int nm16,			// 宽度除以 16 的余数
									   IN void *ka_table);		// 256 * 256 * sizeof(DWORD) 的 KA 表

extern "C" void bgra32_line_alpha_blend_x64(IN OUT void *des,		// 目标
										IN void *src,			// 源
										IN int na16,			// 宽度除以 16 的商
										IN int nm16);			// 宽度除以 16 的余数

extern "C" void bgra32_line_alpha_blend_not_x64(IN OUT void *des,		// 目标
											IN void *src,			// 源
											IN int na16,			// 宽度除以 16 的商
											IN int nm16);			// 宽度除以 16 的余数

extern "C" void bgra32_line_linear_dodge_x64(IN OUT void *des,		// 目标
										 IN void *src,			// 源
										 IN int na16,			// 宽度除以 16 的商
										 IN int nm16,			// 宽度除以 16 的余数
										 IN void *ka_table);	// 256 * 256 * sizeof(DWORD) 的 KA 表

extern "C" void bgra32_line_linear_burn_x64(IN OUT void *des,		// 目标
										IN void *src,			// 源
										IN int na16,			// 宽度除以 16 的商
										IN int nm16,			// 宽度除以 16 的余数
										IN void *ka_table);		// 256 * 256 * sizeof(DWORD) 的 KA 表

extern "C" void bgra32_powerfill_x64(OUT void* des,		// 目标
								 IN DWORD c0,			// 起点颜色
								 IN DWORD c1,			// 终点颜色
								 IN short* power,		// 权重
								 IN int na32,			// 宽度除以 32 的商
								 IN int nm32);			// 宽度除以 32 的余数

extern "C" void bgra32_powerfill_q_x64(OUT void* des,		// 目标
								   IN DWORD c0,		// 起点颜色
								   IN DWORD c1,		// 终点颜色
								   IN float fPrev,		// 起点前一个权重
								   IN float fDelta,	// 像素间的权重增量
								   IN int na16,		// 宽度除以 16 的商（一次做 4 个像素）
								   IN int nm16);		// 宽度除以 16 的余数

extern "C" void bgra32_linear_recursive_x64(OUT void* des,			// 目标
										IN float* pfCr0,		// 起点颜色
										IN float* pfCr1,		// 终点颜色
										IN float fX,			// X 的起始点（像素索引）
										IN float fCoefA,		// 系数 A
										IN float fCoefB,		// 系数 B
										IN float* pfCoefC,		// 系数 C
										IN int na16,			// 宽度除以 16 的商（一次做 4 个像素）
										IN int nm16);			// 宽度除以 16 的余数

extern "C" void bgra32_mean_copy_add_y_x64(OUT void* des,			// 临时缓存行（是源行拷贝宽度的四倍）
									   IN void* src,			// 源拷贝起始行
									   IN int scw,				// 源拷贝宽度
									   IN int src_pitch,		// 源行字节数
									   IN void* pPowerY,		// 目标某行映射的垂直方向上的权重表
									   IN void* pCoefH,		// 垂直方向上的缩放系数
									   IN int nMapY);			// 目标一行映射到源的行数

extern "C" void bgra32_mean_copy_add_y_quick_x64(OUT void* des,		// 临时缓存行（是源行拷贝宽度的四倍）
											 IN void* src,			// 源拷贝起始行
											 IN int scw,			// 源拷贝宽度
											 IN int src_pitch,		// 源行字节数
											 IN void* pPowerY,		// 目标某行映射的垂直方向上的权重表
											 IN void* pCoefH,		// 垂直方向上的缩放系数
											 IN int nMapY);			// 目标一行映射到源的行数

extern "C" void bgra32_mean_copy_add_x_x64(OUT void* des,
									   IN int dcw,
									   IN void* src,
									   IN void* pPowerX,
									   IN void* pMapX,
									   IN void* pCoefW);

extern "C" void bgra32_replace_alpha_line_x64(IN OUT void *des,	// 目标
										  IN void* src,		// 一行 8 位的键值
										  IN int na16,			// 宽度除以 16 的商
										  IN int nm16);			// 宽度除以 16 的余数

extern "C" void bgra32_key_mul_line_x64(IN OUT void* des,	// 目标
									IN void* src,		// 一行 8 位的键值
									IN int na16,		// 宽度除以 16 的商
									IN int nm16);		// 宽度除以 16 的余数

extern "C" void bgra32_key_mul_notsrc_line_x64(IN OUT void* des,	// 目标
										   IN void* src,		// 一行 8 位的键值
										   IN int na16,		// 宽度除以 16 的商
										   IN int nm16);	// 宽度除以 16 的余数

extern "C" void bgra32_key_mul_notdes_line_x64(IN OUT void* des,	// 目标
										   IN void* src,		// 一行 8 位的键值
										   IN int na16,		// 宽度除以 16 的商
										   IN int nm16);		// 宽度除以 16 的余数

extern "C" void bgra32_key_mix_line_x64(IN OUT void* des,	// 目标
									IN void* src,		// 一行 8 位的键值
									IN int na16,		// 宽度除以 16 的商
									IN int nm16);		// 宽度除以 16 的余数

extern "C" void bgra32_key_mix_notsrc_line_x64(IN OUT void* des,	// 目标
										   IN void* src,		// 一行 8 位的键值
										   IN int na16,		// 宽度除以 16 的商
										   IN int nm16);		// 宽度除以 16 的余数

extern "C" void bgra32_key_sat_sub_line_x64(IN OUT void* des,	// 目标
										IN void* src,		// 一行 8 位的键值
										IN int na16,		// 宽度除以 16 的商
										IN int nm16);		// 宽度除以 16 的余数

extern "C" void keybuf_mul_char_line_x64(IN OUT void* des,		// 一行 8 位键目标
									 IN char alpha,		// 标量值
									 IN int na16,			// 宽度除以 16 的商
									 IN int nm16);			// 宽度除以 16 的余数

extern "C" void keybuf_mul_char_line_notdes_x64(IN OUT void* des,		// 一行 8 位键目标
											IN char alpha,			// 标量值
											IN int na16,			// 宽度除以 16 的商
											IN int nm16);			// 宽度除以 16 的余数

extern "C" void keybuf_mix_char_line_x64(IN OUT void* des,		// 一行 8 位键目标
									 IN char alpha,		// 标量值
									 IN int na16,			// 宽度除以 16 的商
									 IN int nm16);			// 宽度除以 16 的余数

extern "C" void keybuf_mul_buf_line_x64(IN OUT void* des,		// 一行 8 位目标键
									IN void* src,			// 一行 8 位源键
									IN int na16,			// 宽度除以 16 的商
									IN int nm16);			// 宽度除以 16 的余数

extern "C" void keybuf_or_buf_line_x64(IN OUT void* des,		// 一行 8 位目标键
								   IN void* src,			// 一行 8 位源键
								   IN int na16,			// 宽度除以 16 的商
								   IN int nm16);			// 宽度除以 16 的余数

extern "C" void keybuf_mul_buf_line_notsrc_x64(IN OUT void* des,		// 一行 8 位目标键
										   IN void* src,			// 一行 8 位源键
										   IN int na16,			// 宽度除以 16 的商
										   IN int nm16);			// 宽度除以 16 的余数

extern "C" void keybuf_mul_buf_line_notdes_x64(IN OUT void* des,		// 一行 8 位目标键
										   IN void* src,			// 一行 8 位源键
										   IN int na16,			// 宽度除以 16 的商
										   IN int nm16);			// 宽度除以 16 的余数

extern "C" void keybuf_mix_buf_line_x64(IN OUT void* des,		// 一行 8 位目标键
									IN void* src,			// 一行 8 位源键
									IN int na16,			// 宽度除以 16 的商
									IN int nm16);			// 宽度除以 16 的余数

extern "C" void keybuf_mix_buf_line_notsrc_x64(IN OUT void* des,		// 一行 8 位目标键
										   IN void* src,			// 一行 8 位源键
										   IN int na16,			// 宽度除以 16 的商
										   IN int nm16);			// 宽度除以 16 的余数

extern "C" void keybuf_sat_sub_buf_line_x64(IN OUT void* des,		// 一行 8 位目标键
										IN void* src,			// 一行 8 位源键
										IN int na16,			// 宽度除以 16 的商
										IN int nm16);			// 宽度除以 16 的余数



extern "C" void keybuf_bl_copy_line_x64(OUT void* des,		// 目标拷贝行
									IN int na4,		// 目标拷贝行的宽度除以 4 的商
									IN int nm4,		// 目标拷贝行的宽度除以 4 的余数
									IN void* src,		// 源拷贝行
									IN int src_pitch,	// 源图一行的字节数
									IN void* pPowerX,	// 目标映射到源的 X 方向上的列权重
									IN void* pMapX,	// 目标映射到源的 X 方向上的列索引
									IN DWORD powerY);	// 目标行映射到源行的行权重

extern "C" void math_serial_perfect_square_x64(OUT void* des,		// 结果行
										   IN float fStart,	// 序列起始数
										   IN float fStep,		// 等差
										   IN float fGiven,	// 特定数
										   IN int na16,		// 行字节宽度除以 16 的商与余数（一次做四个元素）
										   IN int nm16);

extern "C" void math_serial_perfect_square_ex_x64(OUT void* square,		// 完全平方和结果行
											  OUT void* quartic,		// 四次方结果行
											  OUT void* sum,			// 和的线性缩放结果行
											  IN float fStart,			// 序列起始数
											  IN float fStep,			// 等差
											  IN float fGiven,			// 特定数
											  IN float fScaling,		// 和的线性缩放比
											  IN int na16,				// 行字节宽度除以 16 的商与余数（一次做四个元素）
											  IN int nm16);

extern "C" void math_offset_sqrt_index_lookup_x64(OUT void* des,		// 查表结果（32 位表，要求与表的类型相同）
											  IN float* pSerial,	// 序列
											  IN float fOffset,	// 特定偏移
											  IN void* pTable,		// 表（32 位表）
											  IN float fScaling,	// 表的大小相对于序列长度的放大倍数
											  IN int na16,			// 行字节宽度除以 16 的商与余数（一次做四个元素）
											  IN int nm16);

extern "C" void math_tyler_arc_index_lookup_x_x64(OUT void* des,			// 查表结果（32 位表，要求与表的类型相同）
											  IN float* pQuartic,		// 四次方序列
											  IN float* pSquare,		// 完全平方和序列
											  IN float* pSum,			// 和序列
											  IN float fOffsetQ,		// (y - yc)^4
											  IN float fOffsetSq,		// (y - yc)^2
											  IN void* pTable,			// 表（32 位表）
											  IN float fConst,			// 计算式最后添加的常量（定积分的起点值）
											  IN int na16,				// 行字节宽度除以 16 的商与余数（一次做四个元素）
											  IN int nm16);

extern "C" void math_tyler_arc_index_lookup_y_x64(OUT void* des,			// 查表结果（32 位表，要求与表的类型相同）
											  IN float* pQuartic,		// 四次方序列
											  IN float* pSquare,		// 完全平方和序列
											  IN float fOffsetQ,		// (y - yc)^4
											  IN float fOffsetSq,		// (y - yc)^2
											  IN float fSum,			// R * (y - yc)
											  IN void* pTable,			// 表（32 位表）
											  IN float fConst,			// 计算式最后添加的常量（定积分的起点值）
											  IN int na16,				// 行字节宽度除以 16 的商与余数（一次做四个元素）
											  IN int nm16);

extern "C" void math_negtive_float_x64(OUT float* des,		// 目标
								   IN float* src,		// 源
								   IN int na16,		// 行字节宽度除以 16 的商与余数（一次做四个元素）
								   IN int nm16);

extern "C" void math_serial_offset_2_x64(OUT int* des1,		// offset_1 + a
									 OUT int* des2,		// offset_2 - a
									 IN int nmin,			// 最小值
									 IN int nmax,			// 最大值
									 IN float fOffset1,	// 对于 y = x 为 xc - yc
									 IN float fOffset2,	// 对于 y = -x 为 xc + yc
									 IN int nStart,		// 序列起点
									 IN int nStep,			// 序列步长
									 IN int na16,			// 缓存字节宽度除以 16 的商与余数（一次做四个元素）
									 IN int nm16);

extern "C" void trans_power_index_bl_x64(OUT DWORD* pdwSrcIndex,			// 双线性纹理映射时源的四个索引（S11, S10, S01, S00）
									 OUT float* pfPowerX,				// 水平方向的权重
									 OUT float* pfPowerY,				// 垂直方向的权重
									 OUT float* pfPowerXY,				// 权重之积
									 IN void* pvLeftEdgeUVW,			// 行左边缘的 UVW 值
									 IN void* pvDeltaUVW,				// 行内的 UVW 增量
									 IN int nSrcWidth,					// 源的大小
									 IN int nSrcHeight,
									 IN int nSrcBufWidth,				// 源缓存的真实宽度
									 IN int nLinePixels);				// 一行要处理的像素个数

extern "C" void trans_interpo_bl_x64(OUT DWORD* pdwDesLeftEdge,	// 目标行的起始位置
								 IN DWORD* *ppdwSrcStart,		// 源缓存的起始位置
								 IN DWORD* pdwSrcIndex,		// 双线性纹理映射时源的四个索引
								 IN float* pfPowerX,			// 水平方向的权重
								 IN float* pfPowerY,			// 垂直方向的权重
								 IN float* pfPowerXY,			// 权重之积
								 IN int nLinePixels);			// 一行要处理的像素个数

extern "C" void _yuv420_to_yuyv_line_2_F_x64(OUT BYTE* yuyv,        // yuyv 输出，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
							 IN int yuyv_p,
							 IN BYTE* y,            // y 输入，存放方式 Y0 Y1 Y2 Y3 ……
							 IN int y_p_2,
							 IN BYTE* u,            // u 输入，存放方式 U0 U2 U4 U6 ……
							 IN int u_p_2,
							 IN BYTE* v,            // v 输入，存放方式 V0 V2 V4 V6 ……
							 IN int v_p_2,
							 IN int na16,           // 一行像素宽度除以 16 的商与余数（注意目标为 16 位的缓存）
							 IN int nm16,
							int* gc_nRoundComp_2); 
extern "C" void _yuv420_to_yuyv_line_2_x64(OUT BYTE* yuyv,        // yuyv 输出，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
						   IN int yuyv_p,
						   IN BYTE* y,            // y 输入，存放方式 Y0 Y1 Y2 Y3 ……
						   IN int y_p_2,
						   IN BYTE* u,            // u 输入，存放方式 UU0 UU2 UU4 UU6 ……
						   IN int u_p_2,
						   IN BYTE* v,            // v 输入，存放方式 VV0 VV2 VV4 VV6 ……
						   IN int v_p_2,
						   IN int na16,           // 一行像素宽度除以 16 的商与余数（注意目标为 16 位的缓存）
						   IN int nm16);
extern "C" void _yuv420_to_yuyv_line_2_L_x64(OUT BYTE* yuyv,        // yuyv 输出，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
							 IN int yuyv_p,
							 IN BYTE* y,            // y 输入，存放方式 Y0 Y1 Y2 Y3 ……
							 IN int y_p_2,
							 IN BYTE* u,            // u 输入，存放方式 UU0 UU2 UU4 UU6 ……
							 IN int u_p_2,
							 IN BYTE* v,            // v 输入，存放方式 VV0 VV2 VV4 VV6 ……
							 IN int v_p_2,
							 IN int na16,           // 一行像素宽度除以 16 的商与余数（注意目标为 16 位的缓存）
							 IN int nm16);
extern "C" void _yuv420_to_yuyv_line_1_L_x64(OUT BYTE* yuyv,        // yuyv 输出，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
							 IN BYTE* y,            // y 输入，存放方式 Y0 Y1 Y2 Y3 ……
							 IN BYTE* u,            // u 输入，存放方式 UU0 UU2 UU4 UU6 ……
							 IN int u_p_2,
							 IN BYTE* v,            // v 输入，存放方式 VV0 VV2 VV4 VV6 ……
							 IN int v_p_2,
							 IN int na16,           // 一行像素宽度除以 16 的商与余数（注意目标为 16 位的缓存）
							 IN int nm16);

extern "C" void color24_to_yuyv_inner_x64(OUT void* yuyv,		// 目标 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
						   IN void* color24,	// *G@ 输入，存放方式 *0 G0 @0 *1 G1 @1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
						   IN int na12,			// 源字节宽度减 4 后除以 12 的商
						   IN int nmp,			// 做完 na12 循环后剩余的像素'
						   IN int* pnY24_RGB,   // 系数（通过变更 R 分量与 B 分量的系数位置可以做到 BGRA 与 RGBA 的兼容）
						   IN int* pnY32_BGR,
						   IN int* pnU_BGR,
						   IN int* pnV_BGR,
						   IN int* pnUV_BGR);

extern "C" void yuyv_to_bgra_inner_x64(OUT void* bgra,     // BGRA 缓存，存放方式 B0 G0 R0 A0 B1 G1 R1 A1 ……
						IN void* yuyv, 		// 源 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
						IN int na16,		// 源字节宽度减 4 后除以 16 的商与余数
						IN int nm16,
						IN int* pnR_V,      // 系数（无法做到 RGBA 与 BGRA 的兼容）
						IN int* pnG_U,
						IN int* pnG_V,
						IN int* pnB_U);

extern "C" void yuyv_to_rgba_inner_x64(OUT void* rgba,	        // RGBA 缓存，存放方式 R0 G0 B0 A0 R1 G1 B1 A1 ……
						IN void* yuyv, 		// 源 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
						IN int na16,		    // 源字节宽度减 4 后除以 16 的商与余数
						IN int nm16,
						IN int* pnR_V,        // 系数（无法做到 RGBA 与 BGRA 的兼容）
						IN int* pnG_U,
						IN int* pnG_V,
						IN int* pnB_U);

extern "C" void yuyva_to_bgra_inner_x64(OUT void* bgra,        // BGRA 缓存，存放方式 B0 G0 R0 A0 B1 G1 R1 A1 ……
						 IN void* yuyv, 		// 源 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
						 IN void* alpha,		// 源 alpha 缓存，存放方式 A0 A1 A2 A3 ……
						 IN int na16,		    // 源字节宽度减 4 后除以 16 的商与余数
						 IN int nm16,
						 IN int* pnR_V,         // 系数（无法做到 RGBA 与 BGRA 的兼容）
						 IN int* pnG_U,       
						 IN int* pnG_V,
						 IN int* pnB_U);

extern "C" void yuyva_to_rgba_inner_x64(OUT void* rgba,        // RGBA 缓存，存放方式 R0 G0 B0 A0 R1 G1 B1 A1 ……
						 IN void* yuyv, 		// 源 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
						 IN void* alpha,		// 源 alpha 缓存，存放方式 A0 A1 A2 A3 ……
						 IN int na16,		    // 源字节宽度减 4 后除以 16 的商与余数
						 IN int nm16,
						 IN int* pnR_V,         // 系数（无法做到 RGBA 与 BGRA 的兼容）
						 IN int* pnG_U,       
						 IN int* pnG_V,
						 IN int* pnB_U);

extern "C" void color32_to_yuyv_inner_x64(OUT void* yuyv, 	    // 目标 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
						   IN void* color32,	// 源 *g@a 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
						   IN int na16,		    // 源字节宽度除以 16 的商与余数
						   IN int nm16,
						   IN int* pnY32_BR,    // 系数（通过变更 R 分量与 B 分量的系数位置可以做到 BGRA 与 RGBA 的兼容）
						   IN int* pnY32_G,     // BR 表示 R 在低位
						   IN int* pnY32_BGR,
						   IN int* pnUV_BGR,
						   IN int* pnU_BGR,
						   IN int* pnV_BGR);

extern "C" void color32_to_yuyva_inner_x64(OUT void* yuyv,	        // 目标 yuyv 缓存，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
							OUT void* alpha,    	// 目标 alpha 缓存，存放方式 A0 A1 A2 A3 ……
							IN void* color32,	    // 源 *g@a 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
							IN int na16,    		// 源字节宽度除以 16 的商与余数
							IN int nm16,
							IN int* pnY32_BR,       // 系数（通过变更 R 分量与 B 分量的系数位置可以做到 BGRA 与 RGBA 的兼容）
							IN int* pnY32_G,        // BR 表示 R 在低位
							IN int* pnY32_BGR,
							IN int* pnUV_BGR,
							IN int* pnU_BGR,
							IN int* pnV_BGR);

extern "C" extern "C" void yuv422_to_color32_inner_x64(OUT void* color32,     // *G@A 缓存，存放方式 *0 G0 @0 A0 *1 G1 @1 A1 ……（* 与 @ 表示 R 或 B，由像素的存储方式确定）
							 IN void* y,            // y 输入，存放方式 Y0 Y1 Y2 Y3 ……
							 IN void* u,            // u 输入，存放方式 UU0 UU2 UU4 UU6 ……
							 IN void* v,            // v 输入，存放方式 VV0 VV2 VV4 VV6 ……
							 IN int na8,		    // 源像素宽度减 1 后除以 8 的商与余数
							 IN int nm8,
							 IN int* pnR_V,         // 系数
							 IN int* pnG_U,         // （通过交换 U 与 V 可以做到 RGBA 与 BGRA 的兼容）
							 IN int* pnG_V,         // （输入 u 与 v 交换，pnR_V 与 pnB_U 交换，pnG_V 与 pnG_U 交换即可实现）
							 IN int* pnB_U);

extern "C" void uv20_to_uv22_line_1_L_x64(OUT void* u_o,         // 422 u 输出，存储方式 U0 U2 U4 U6 ……（两行）
						   OUT void* v_o,         // 422 v 输出，存储方式 V0 V2 V4 V6 ……（两行）
						   IN void* u_i,          // 420 u 输入，存储方式 UU0 UU2 UU4 UU6 ……
						   IN int u_i_p_2,
						   IN void* v_i,          // 420 v 输入，存储方式 VV0 VV2 VV4 VV6 ……
						   IN int v_i_p_2,
						   IN int na16,           // 一行像素宽度的一半（因为一行内的 UV 与 Y 为 422 的关系）除以 16 的商与余数
						   IN int nm16);
extern "C" void uv20_to_uv22_line_2_L_x64(OUT void* u_o,         // 422 u 输出，存储方式 U0 U2 U4 U6 ……（两行）
						   IN int u_o_p,
						   OUT void* v_o,         // 422 v 输出，存储方式 V0 V2 V4 V6 ……（两行）
						   IN int v_o_p,
						   IN void* u_i,          // 420 u 输入，存储方式 UU0 UU2 UU4 UU6 ……
						   IN int u_i_p_2,
						   IN void* v_i,          // 420 v 输入，存储方式 VV0 VV2 VV4 VV6 ……
						   IN int v_i_p_2,
						   IN int na16,           // 一行像素宽度的一半（因为一行内的 UV 与 Y 为 422 的关系）除以 16 的商与余数
						   IN int nm16);

extern "C" void uv20_to_uv22_line_2_x64(OUT void* u_o,         // 422 u 输出，存储方式 U0 U2 U4 U6 ……（两行）
						 IN int u_o_p,
						 OUT void* v_o,         // 422 v 输出，存储方式 V0 V2 V4 V6 ……（两行）
						 IN int v_o_p,
						 IN void* u_i,          // 420 u 输入，存储方式 UU0 UU2 UU4 UU6 ……
						 IN int u_i_p_2,
						 IN void* v_i,          // 420 v 输入，存储方式 VV0 VV2 VV4 VV6 ……
						 IN int v_i_p_2,
						 IN int na16,           // 一行像素宽度的一半（因为一行内的 UV 与 Y 为 422 的关系）除以 16 的商与余数
						 IN int nm16);

extern "C" void uv20_to_uv22_line_2_F_x64(OUT void* u_o,         // 422 u 输出，存储方式 U0 U2 U4 U6 ……（两行）
						   IN int u_o_p,
						   OUT void* v_o,         // 422 v 输出，存储方式 V0 V2 V4 V6 ……（两行）
						   IN int v_o_p,
						   IN void* u_i,          // 420 u 输入，存储方式 UU0 UU2 UU4 UU6 ……
						   IN int u_i_p_2,
						   IN void* v_i,          // 420 v 输入，存储方式 VV0 VV2 VV4 VV6 ……
						   IN int v_i_p_2,
						   IN int na16,           // 一行像素宽度的一半（因为一行内的 UV 与 Y 为 422 的关系）除以 16 的商与余数
						   IN int nm16);

extern "C" void yuv422_to_yuyv_line_x64(OUT BYTE* yuyv,        // yuyv 输出，存放方式 Y0 U0 Y1 V0 Y2 U2 Y3 V2 ……
						 IN BYTE* y,            // y 输入，存放方式 Y0 Y1 Y2 Y3 ……
						 IN BYTE* u,            // u 输入，存放方式 U0 U2 U4 U6 ……
						 IN BYTE* v,            // v 输入，存放方式 V0 V2 V4 V6 ……
						 IN int na16,           // 一行像素宽度除以 16 的商与余数（注意目标为 16 位的缓存）
						 IN int nm16);
extern "C" void _Fill_inner_pixel(DWORD* t_pSrc,int src_buff_width,DWORD* cur_dst,int num_pix,        
					 	float cur_v_u[4],float dxv_dxu[4],short src_offest[8]        ,
						int	alpha_mask[4],int	alpha_one_mask[4],int alpha_ps_mask[4],float one_ps_mask2[4],float one_ps_mask[4]);
extern "C" void _rip_x_filllinebuf_x64(int* ptempsubtable, //RGBA
										DWORD* plinebuf,	//DWORD count
										int acount,
										int mcount);		//RGBA
extern "C" void _rip_update_buf_byte_x64(int  g_nTotalErea,
										DWORD* psrcbuf,
										BYTE*  pdesbuf,
										const DWORD* gc_dwScale_temp,
										int acount,
										int mcount);

extern "C" BOOL _yuv_yyuuvv2yuv_x64(BYTE* pDesBuffer,BYTE* pSrcField,int nA16,int nMod16,DWORD* pMask);
extern "C" BOOL _yuv_yuv2yyuuvv_x64(BYTE* pDesBuffer,BYTE* pSrcField,int nA8,int nMod8);
extern "C" void _yuv_uyvy2yuv8_x64(BYTE* puyvy,BYTE* py,BYTE* pu,BYTE* pv,int nA16,int nM16);
extern "C" void _yuv_yuyv2yuv8_x64(BYTE* puyvy,BYTE* py,BYTE* pu,BYTE* pv,int nA16,int nM16);
extern "C" void _bgra32_mean_copy_add_y_quick_field_x64(OUT void* des,	
														IN void* src,	
														IN int scw,
														IN int src_pitch,
														IN void* pPowerY,
														IN void* pCoefH,
														IN int nMapY,
														IN int nFirst);	
extern "C" void _asmX64_SSE2_FastMemset(LPDWORD lpdwDes,DWORD	dwColor,int nSize);

inline  void	MMX_FastMemset( IN	LPDWORD	lpdwDes,	// 目标内存指针
								IN	DWORD	dwColor,	// 欲设色值
								IN	int		nSize)			// 点个数(以 DWORD 为单位)
{
	_asmX64_SSE2_FastMemset(lpdwDes,dwColor,nSize);
}

#endif

