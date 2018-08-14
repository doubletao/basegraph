#pragma once
//数学定义/////////////////////////////////////////////////////////////////////
// 浮点数定义
const float Common_ABS_FLOAT_0 = 1.1e-6f;		// float 精度为 6 到 7 位
const float Common_ABS_DOUBLE_0 = 1.1e-8f;		// float 精度为 6 到 7 位
#define PAI				3.14159265358979
#define PAIF			3.14159265358979f
#define PAI_2			6.28318530717958
#define PAI_2F			6.28318530717958f
#define PAI_HALF		1.570796326794897
#define PAI_HALFF		1.570796326794897f
#define RADIAN_HALF_PI	1.5707963267948966192313216916398f 
#define ABS_FLOAT_0		5.0e-5f
#define ABS_DOUBLE_0	1.0e-3f
#define SYMAXUINT		((UINT)~((UINT)0))
#define SYMAXINT		((INT)(SYMAXUINT >> 1))
#define MBaseARAMMAX		65535
#define MBaseARAMMIN		-65535
											// 数据类型取值范围、精度等
#define Common_INT_MAX		INT_MAX		/* maximum (signed) int value */
#define Common_INT_MIN		INT_MIN		/* minimum (signed) int value */

#define Common_UINT_MAX		UINT_MAX		/* maximum unsigned int value */

#define Common_FLT_MAX		FLT_MAX		/* max value */
#define Common_FLT_MIN		FLT_MIN		/* min positive value */
#define Common_FLT_MIN_NEG	(-FLT_MAX)		/* min negative value */

#define Common_DBL_MAX		DBL_MAX		/* max value */
#define Common_DBL_MIN		DBL_MIN		/* min positive value */
#define Common_DBL_MIN_NEG	(-DBL_MAX)	/* min negative value */
											//asin与acos定义域：[-1, 1]
#define ASIN_DOMAINF(a)	\
	if ((a) < -1.0f)		\
		(a) = -1.0f;		\
	else if ((a) > 1.0f)	\
		(a) = 1.0f;

											//Callback parameter created, playout data higher 8bit represents player status, lower 24bit represents current frame number
#define MAKEOUTDATA(nStatus,dwFrame)	((((((DWORD)nStatus) & 0x000000ff ) << 24 ) | (dwFrame & 0x00ffffff)))

const int ANGLE_HALF_PI = 90;
const int ANGLE_PI = 180;
const int ANGLE_2_PI = 360;
const float RADIAN_PI = 3.1415927f;
const float RADIAN_2_PI = 6.2831853f;

const float RADIAN_2_PI_INV = 0.1591549f;		// 0.5f / PI
const float ANGLE_2_PI_INV = 0.0027778f;		// 1.f / 360
const float ANGLE_TO_RADIAN = 0.0174533f;		// PI / 180
const float RADIAN_TO_ANGLE = 57.2957795f;		// 180 / PI

const float POUND_TO_PIXELS = 1.3333f;           // 4.0 / 3.0
const float PIXELS_TO_POUND = 0.75f;             // 3.0 / 4.0
												 // 比较
inline BOOL Common_IsZero_F(IN float x)
{
	return (x <= Common_ABS_FLOAT_0 && x >= -Common_ABS_FLOAT_0);
}
inline BOOL Common_IsEqual_F(IN float x, IN float y)
{
	return Common_IsZero_F(x - y);
}
inline int Common_ClampAngle_PI_2(IN int nAngle)	// [0, 360)
{
	return (nAngle - (int)floorf(nAngle * ANGLE_2_PI_INV) * ANGLE_2_PI);
}
//////////////////////////////////////////////////////////////////////////
// 取绝对值、最大值、最小值
template <class TYPE>
inline TYPE Common_Abs(IN TYPE x1, IN TYPE x2)
{
	return ((x1 > x2) ? (x1 - x2) : (x2 - x1));
}
template <class TYPE>
inline TYPE Common_Max(IN TYPE x1, IN TYPE x2)
{
	return ((x1 > x2) ? x1 : x2);
}
template <class TYPE>
inline TYPE Common_Min(IN TYPE x1, IN TYPE x2)
{
	return ((x1 > x2) ? x2 : x1);
}
// 取整
inline int Common_FToI(IN float x)
{
	return (int)(x + ((x < 0.f) ? -0.5f : 0.5f));
}
inline int Common_DToI(IN double x)
{
	return (int)(x + ((x < 0.f) ? -0.5f : 0.5f));
}
// 角度与弧度之间的变换
inline int Common_RadianToAngle(IN float fRad)
{
	return Common_FToI(fRad * RADIAN_TO_ANGLE);
}
inline BOOL Common_IsZero(IN double x)
{
	return (x <= Common_ABS_DOUBLE_0 && x >= -Common_ABS_DOUBLE_0);
}
inline BOOL Common_IsEqual(IN double x, IN double y)
{
	return Common_IsZero(x - y);
}