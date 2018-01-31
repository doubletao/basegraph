#pragma once

// 操作模式
enum BUFFER_MODE
{
	BUFFER_COPY = 0,		// 复制
	BUFFER_MIX,			// Alpha 混叠（ma = sa + da - sa * da）
	BUFFER_MIX_NOTSRC,	// Alpha 反向源混叠（ma = (1 - sa) + da - (1 - sa) * da）

	BUFFER_ALPHA_BLEND,	// 快速 Alpha 混合（src * sa + des * (1 - sa)）
	BUFFER_ALPHA_BLEND_NOT,// 快速 Alpha 反向源混合（src * (1 - sa) + des * sa）

	BUFFER_LINEAR_DODGE,	// 线性减淡（src + des）
	BUFFER_LINEAR_BURN,	// 线性加深（src + des - 1）
	BUFFER_COLOR_DODGE,	// 颜色减淡（des / (1 - src)）
	BUFFER_COLOR_BURN,	// 颜色加深（(src + des - 1) / src）
	BUFFER_MULTIPLY,		// 正片叠底（src * des）
	BUFFER_SCREEN,		// 屏幕滤色（src + des - src * des）
	BUFFER_LIGHTEN,		// 变亮（max(src, des)）
	BUFFER_DARKEN,		// 变暗（min(src, des)）
	BUFFER_DIFFERENCE,	// 差值（|src - des|）
	BUFFER_EXCLUSION,	// 排除（src + des - src * des * 2）
	BUFFER_HARD_LIGHT,	// 强光（(src < 0.5) ? (src * des * 2) : (2 * (src + des - src * des) - 1)）
	BUFFER_SOFT_LIGHT,	// 柔光（des * (2 * src + des - 2 * src * des)）
	BUFFER_PIN_LIGHT,	// 点光（(src < 0.5) ? min(src, des) : max(src, des)）
	BUFFER_OVERLAY,		// 叠加（(des < 0.5) ? (src * des * 2) : (2 * (src + des - src * des) - 1)）
	BUFFER_HUE,			// 相位（desHue = srcHue）
	BUFFER_LUM,			// 亮度（desLum = srcLum）
	BUFFER_SAT,			// 饱和度（desSat = srcSat）
	BUFFER_CHROMA,		// 色度 = 相位 + 饱和度（desHue = srcHue, desSat = srcSat）
	BUFFER_SUB,			// 差值（|src - des|）
};

// 扫描方式定义
enum BUFFER_PARITY_STYLE
{
	BUFFER_PARITY_BOTH = 0,		// 奇偶场
	BUFFER_PARITY_ODD,			// 奇场
	BUFFER_PARITY_EVEN,			// 偶场
	BUFFER_PARITY_FOUR_0,		// 4
	BUFFER_PARITY_FOUR_1,
	BUFFER_PARITY_FOUR_2,
	BUFFER_PARITY_FOUR_3,
	BUFFER_PARITY_N_K,
};

// 颜色变换中的制式定义
enum BUFFER_DEFINITION_TYPE
{
	BUFFER_DEFINITION_UNKNOWN = 0,		// 未知定义
	BUFFER_DEFINITION_HIGH,				// 高清
	BUFFER_DEFINITION_STANDARD,			// 标清
};

const int COEF_SCALE_16384 = 16384;			// 14 位放大，不需要补偿