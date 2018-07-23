// SSE2 优化算法中用到的静态全局变量，只能被 SYSSE2.cpp 调用
// 严格限制被其它任何文件调用，会造成内存冗余
#pragma once
//////////////////////////////////////////////////////////////////////////
// 常量掩码
static const __declspec(align(16)) DWORD gc_dwMaskDLW[4] = { 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff };
static const __declspec(align(16)) DWORD gc_dwMaskDHW[4] = { 0xffff0000, 0xffff0000, 0xffff0000, 0xffff0000 };
static const __declspec(align(16)) DWORD gc_dwMaskDLB[4] = { 0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff };
static const __declspec(align(16)) DWORD gc_dwMaskWHB[4] = { 0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00 };
static const __declspec(align(16)) DWORD gc_dwMaskWLB[4] = { 0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff };
static const __declspec(align(16)) DWORD gc_dwMaskRGB[4] = { 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff };
static const __declspec(align(16)) DWORD gc_dwMaskG[4] = { 0x0000ff00, 0x0000ff00, 0x0000ff00, 0x0000ff00 };
static const __declspec(align(16)) DWORD gc_dwMaskSpecUV[4] = { 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000 };
static const __declspec(align(16)) DWORD gc_dwMaskAlpha[4] = { 0xff000000, 0xff000000, 0xff000000, 0xff000000 };

//////////////////////////////////////////////////////////////////////////
// 常量补偿
// -1 或 正数最大
static const __declspec(align(16)) int gc_nComp_N1[4] = { (int)0xffffffff, (int)0xffffffff, (int)0xffffffff, (int)0xffffffff };
// 放大位数后的偏移
static const __declspec(align(16)) int gc_nOff128_5[4] = { 0x10001000, 0x10001000, 0x10001000, 0x10001000 };
static const __declspec(align(16)) int gc_nOff128_6[4] = { 0x20002000, 0x20002000, 0x20002000, 0x20002000 };
static const __declspec(align(16)) int gc_nOff128_7[4] = { 0x40004000, 0x40004000, 0x40004000, 0x40004000 };
// 浮点四舍五入
static const __declspec(align(16)) float gc_fRoundComp[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
// 放大位数后的四舍五入
static const __declspec(align(16)) int gc_nRoundComp_1[4] = { 0x00010001, 0x00010001, 0x00010001, 0x00010001 };
static const __declspec(align(16)) int gc_nRoundComp_5[4] = { 0x00100010, 0x00100010, 0x00100010, 0x00100010 };
static const __declspec(align(16)) int gc_nRoundComp_2[4] = { 0x00020002, 0x00020002, 0x00020002, 0x00020002 };

//////////////////////////////////////////////////////////////////////////
// 常量系数

static const __declspec(align(16)) float gc_fCoef_6[4] = { 6.f, 6.f, 6.f, 6.f };
static const __declspec(align(16)) float gc_fCoef_13_6[4] = { 2.1666667f, 2.1666667f, 2.1666667f, 2.1666667f };	// 13/6
static const __declspec(align(16)) float gc_fCoef_149_120[4] = { 1.2416667f, 1.2416667f, 1.2416667f, 1.2416667f };	// 149/120
//////////////////////////////////////////////////////////////////////////
// 变量
// 颜色变换参数定义
// rgba bgra
static __declspec(align(16)) int g_nY32_BR_HD[4];		// YbYr YbYr YbYr YbYr  （rgb）
static __declspec(align(16)) int g_nY32_G_HD[4];		// 0Yg 0Yg 0Yg 0Yg  （rgb or bgr）
static __declspec(align(16)) int g_nY32_RB_HD[4];		// YrYb YrYb YrYb YrYb  （bgr）

static __declspec(align(16)) int g_nY32_BR_SD[4];		// YbYr YbYr YbYr YbYr  （rgb）
static __declspec(align(16)) int g_nY32_G_SD[4];		// 0Yg 0Yg 0Yg 0Yg  （rgb or bgr）
static __declspec(align(16)) int g_nY32_RB_SD[4];		// YrYb YrYb YrYb YrYb  （bgr）

// rgb bgr
static __declspec(align(16)) int g_nY24_RGB_HD[4];		// YrYg YbYr YgYb YrYg  （rgb）
static __declspec(align(16)) int g_nY24_BGR_HD[4];		// YbYg YrYb YgYr YbYg  （bgr）

static __declspec(align(16)) int g_nY24_RGB_SD[4];		// YrYg YbYr YgYb YrYg  （rgb）
static __declspec(align(16)) int g_nY24_BGR_SD[4];		// YbYg YrYb YgYr YbYg  （bgr）

// rgba rgb bgra bgr
static __declspec(align(16)) int g_nY32_BGR_HD[4];		// 0Yb YgYr 0Yb YgYr  （rgb）
static __declspec(align(16)) int g_nUV_BGR_HD[4];		// 0Vb VgVr 0Ub UgUr  （rgb）
static __declspec(align(16)) int g_nU_BGR_HD[4];		// 0Ub UgUr 0Ub UgUr  （rgb）
static __declspec(align(16)) int g_nV_BGR_HD[4];		// 0Vb VgVr 0Vb VgVr  （rgb）
static __declspec(align(16)) int g_nY32_RGB_HD[4];		// 0Yr YgYb 0Yr YgYb  （bgr）
static __declspec(align(16)) int g_nUV_RGB_HD[4];		// 0Vr VgVb 0Ur UgUb  （bgr）
static __declspec(align(16)) int g_nU_RGB_HD[4];		// 0Ur UgUb 0Ur UgUb  （bgr）
static __declspec(align(16)) int g_nV_RGB_HD[4];		// 0Vr VgVb 0Vr VgVb  （bgr）
static __declspec(align(16)) int g_nR_V_HD[4];			// RvRv RvRv RvRv RvRv  （rgb or bgr）
static __declspec(align(16)) int g_nG_U_HD[4];			// GuGu GuGu GuGu GuGu  （rgb or bgr）
static __declspec(align(16)) int g_nG_V_HD[4];			// GvGv GvGv GvGv GvGv  （rgb or bgr）
static __declspec(align(16)) int g_nB_U_HD[4];			// BuBu BuBu BuBu BuBu  （rgb or bgr）

static __declspec(align(16)) int g_nY32_BGR_SD[4];		// 0Yb YgYr 0Yb YgYr  （rgb）
static __declspec(align(16)) int g_nUV_BGR_SD[4];		// 0Vb VgVr 0Ub UgUr  （rgb）
static __declspec(align(16)) int g_nU_BGR_SD[4];		// 0Ub UgUr 0Ub UgUr  （rgb）
static __declspec(align(16)) int g_nV_BGR_SD[4];		// 0Vb VgVr 0Vb VgVr  （rgb）
static __declspec(align(16)) int g_nY32_RGB_SD[4];		// 0Yr YgYb 0Yr YgYb  （bgr）
static __declspec(align(16)) int g_nUV_RGB_SD[4];		// 0Vr VgVb 0Ur UgUb  （bgr）
static __declspec(align(16)) int g_nU_RGB_SD[4];		// 0Ur UgUb 0Ur UgUb  （bgr）
static __declspec(align(16)) int g_nV_RGB_SD[4];		// 0Vr VgVb 0Vr VgVb  （bgr）
static __declspec(align(16)) int g_nR_V_SD[4];			// RvRv RvRv RvRv RvRv  （rgb or bgr）
static __declspec(align(16)) int g_nG_U_SD[4];			// GuGu GuGu GuGu GuGu  （rgb or bgr）
static __declspec(align(16)) int g_nG_V_SD[4];			// GvGv GvGv GvGv GvGv  （rgb or bgr）
static __declspec(align(16)) int g_nB_U_SD[4];			// BuBu BuBu BuBu BuBu  （rgb or bgr）
//////////////////////////////////////////////////////////////////////////
// 文件结束