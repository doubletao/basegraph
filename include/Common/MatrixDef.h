#pragma once
#include "Common\VectorDef.h"

typedef struct tag_matrixf4x4
{
	union{
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;
  		};
		float m[4][4];
	};
}matrix4f,*pmatrix4f;

typedef float  SYFMAT4[4][4];
#define SYSIZE_OF_FMAT4 (sizeof(float)*16)

typedef struct tag_matrixf3x3
{
	union{
		struct {
			float        _11, _12, _13;
			float        _21, _22, _23;
			float        _31, _32, _33;
		};
		float m[3][3];
	};
}matrix3f,*pmatrix3f;

typedef float  SYFMAT3[3][3];
#define SYSIZE_OF_FMAT3 (sizeof(float)*9)


struct tag_quatn;
typedef struct tag_quatn SYQuatn;
//struct tag_float3;
//typedef struct tag_float3 float3,vector3f;
///////////////////////////////////////////////////////////////////////////////
// 描述：	零化矩阵
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl matrix4f_zero(pmatrix4f pM);		// 矩阵指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	单位矩阵
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl matrix4f_identity(pmatrix4f pM);	// 矩阵指针

BASEGRAPH_COMMON_API void __cdecl matrix4f_eye(pmatrix4f pM);			// 矩阵指针

BASEGRAPH_COMMON_API void __cdecl matrix4f_debug(const matrix4f * pM);	// 矩阵Debug输出

///////////////////////////////////////////////////////////////////////////////
// 描述：	负矩阵
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl matrix4f_negative(pmatrix4f pDesM,		// 目标矩阵指针
								  pmatrix4f pSrcM);		// 源矩阵指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩阵的某个元素（下标从 1 开始）
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API float __cdecl matrix4f_getat(pmatrix4f pM,	// 矩阵指针
								int r,				// 行
								int c);				// 列

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置矩阵的某个元素（下标从 1 开始）
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl matrix4f_setat(pmatrix4f pM,		// 矩阵指针
							   int r,					// 行
							   int c,					// 列
							   float v);				// 值

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩阵与数的加减乘法
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl matrix4f_addnum(pmatrix4f pDesM,		// 目标矩阵指针
								const matrix4f*  pSrcM,		// 源矩阵指针
								float k);					// 数

BASEGRAPH_COMMON_API void __cdecl matrix4f_minusnum(pmatrix4f pDesM,		// 目标矩阵指针
								  pmatrix4f pSrcM,		// 源矩阵指针
								  float k);					// 数

BASEGRAPH_COMMON_API void __cdecl matrix4f_subnum(pmatrix4f pDesM,		// 目标矩阵指针
								pmatrix4f pSrcM,		// 源矩阵指针
								float k);					// 数

BASEGRAPH_COMMON_API void __cdecl matrix4f_mulnum(pmatrix4f pDesM,		// 目标矩阵指针
								pmatrix4f pSrcM,		// 源矩阵指针
								float k);					// 数

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩阵的加减乘法
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl matrix4f_add(pmatrix4f pDesM,		// 目标矩阵指针
							 pmatrix4f pSrcML,		// 左矩阵指针
							 pmatrix4f pSrcMR);		// 右矩阵指针

BASEGRAPH_COMMON_API void __cdecl matrix4f_minus(pmatrix4f pDesM,		// 目标矩阵指针
							   pmatrix4f pSrcML,	// 左矩阵指针
							   pmatrix4f pSrcMR);	// 右矩阵指针

BASEGRAPH_COMMON_API void __cdecl matrix4f_sub(pmatrix4f pDesM,		// 目标矩阵指针
							 pmatrix4f pSrcML,		// 左矩阵指针
							 pmatrix4f pSrcMR);		// 右矩阵指针

BASEGRAPH_COMMON_API void __cdecl matrix4f_mul(pmatrix4f pDesM,		// 目标矩阵指针
							 const matrix4f*  pSrcML,		// 左矩阵指针
							 const matrix4f*  pSrcMR);		// 右矩阵指针

//////////////////////////////////////////////////////////////////////////
//将一个顶点矩阵转换成法线矩阵
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl matrix4f_to_normal_matrix(matrix4f* pDesM,
									  const matrix4f* pSrc);
//////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// 描述：	与四元数的转换（要求输入归一化的四元数）
//			本系统使用行向量与矩阵的乘法，并定义左手坐标系，旋转的正方向定义为：
//			以左手法则，从旋转轴的正端点看向负端点，顺时针方向为正。
//
//	四元数到矩阵的变换公式：
//
//	┌ r^2 + x^2 - y^2 - z^2		  2xy + 2rz				  2xz - 2ry		  ┐
//	│		 2xy - 2rz			r^2 + y^2 - x^2 - z^2		  2yz + 2rx		  │
//	┕		 2xz + 2ry				  2yz - 2rx			r^2 + z^2 - x^2 - y^2 ┘
//
//
//	矩阵到四元数的变换公式：用对角线元素和/差的平方根求解其中一个元素的值（取四个中最大为主），
//		用相对于对角线的对称位置上的元素的和与差解其它三个元素。
//
//	r = sqrt(m11 + m22 + m33 + 1) / 2
//	=> x = (m23 - m32) / (4r)    y = (m31 - m13) / (4r)    z = (m12 - m21) / (4r)
//
//	x = sqrt(m11 - m22 - m33 + 1) / 2
//	=> r = (m23 - m32) / (4x)    y = (m12 + m21) / (4x)    z = (m31 + m13) / (4x)
//
//	y = sqrt(-m11 + m22 - m33 + 1) / 2
//	=> r = (m31 - m13) / (4y)    x = (m12 + m21) / (4y)    z = (m23 + m32) / (4y)
//
//	z = sqrt(m11 + m22 + m33 + 1) / 2
//	=> r = (m12 - m21) / (4z)    y = (m31 + m13) / (4z)    z = (m23 + m32) / (4z)
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////

BASEGRAPH_COMMON_API void __cdecl matrix4f_fromquatn(pmatrix4f pDesM,		// 目标矩阵指针
								   const SYQuatn* pSrcQ);		// 源四元数指针

BASEGRAPH_COMMON_API void __cdecl matrix4f_toquatn(SYQuatn* pDesQ,			// 目标四元数指针
								 const matrix4f* pSrcM);		// 源矩阵指针

//////////////////////////////////////////////////////////////////////////
// 描述：	矩阵级联乘以四元数，结果存入矩阵，Mr = Ms * Qs。
//
// 返回值：	无
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl matrix4f_mulquatn(pmatrix4f pDesM,		// 目标矩阵指针
								  pmatrix4f pSrcML,		// 源矩阵指针（左）
								  const SYQuatn* pSrcQR);		// 源四元数指针（右）

//////////////////////////////////////////////////////////////////////////
// 描述：	绕任意轴旋转某一角度的转换矩阵
//			本系统使用行向量与矩阵的乘法，并定义左手坐标系，旋转的正方向定义为：
//			以左手法则，从旋转轴的正端点看向负端点，顺时针方向为正。
//
//	┌ x^2(1 - cosA) + cosA		xy(1 - cosA) + zsinA	xz(1 - cosA) - ysinA ┐
//	│ xy(1 - cosA) - zsinA		y^2(1 - cosA) + cosA	yz(1 - cosA) + xsinA │
//	┕ xz(1 - cosA) + ysinA		yz(1 - cosA) - xsinA	z^2(1 - cosA) + cosA ┘
//
// 返回值：	无
//////////////////////////////////////////////////////////////////////////
//struct tag_vector3f;
//typedef tag_vector3f vector3f, *vector3f*;

BASEGRAPH_COMMON_API void __cdecl matrix4f_fromrotvector(pmatrix4f pDesM,		// 目标矩阵指针
									   const vector3f* pSrcV,		// 源向量指针
									   float fAngle);			// 旋转角度（弧度制）

//////////////////////////////////////////////////////////////////////////
//计算两个向量间的旋转矩阵
BASEGRAPH_COMMON_API void __cdecl matrix4f_BetweenVectors(pmatrix4f pDesM,		// 目标矩阵指针
									   const vector3f* pSrcV,		// 源向量指针
									   const vector3f* pDstV		// 目标向量指针
									   );			

//////////////////////////////////////////////////////////////////////////
// 描述：	欧拉角与旋转矩阵的变换（必须定义旋转顺序以及坐标系方向）
//
// 返回值：	无
//////////////////////////////////////////////////////////////////////////
#define  SY_ROTATE_ORDER_XYZ 0
#define  SY_ROTATE_ORDER_YXZ 1
#define  SY_ROTATE_ORDER_XZY 2
#define  SY_ROTATE_ORDER_YZX 3
#define  SY_ROTATE_ORDER_ZXY 4
#define  SY_ROTATE_ORDER_ZYX 5

BASEGRAPH_COMMON_API void __cdecl matrix4f_from_euler(matrix4f* pDesM,float fAngleX,float fAngleY,float fAngleZ,int nOrder);
BASEGRAPH_COMMON_API void __cdecl matrix4f_to_euler(matrix4f* pDesM,float* pAngleX,float* pAngleY,float* pAngleZ,int nOrder);

// 本系统使用行向量与矩阵的乘法，并定义左手坐标系，旋转的正方向定义为：
// 以左手法则，从旋转轴的正端点看向负端点，顺时针方向为正。
// 旋转顺序为 Y X Z，XY 组成屏幕平面，X 指向右，Y 指向上，Z 垂直于平面指向屏幕内。
// ┌ cosYcosZ + sinYsinXsinZ		sinYsinXcosZ - cosYsinZ		sinYcosX ┐
// │		 cosXsinZ						cosXcosZ			  -sinX	 │
// ┕ cosYsinXsinZ - sinYcosZ		sinYsinZ + cosYsinXcosZ		cosYcosX ┘
BASEGRAPH_COMMON_API void __cdecl matrix4f_fromeuler_lyxz(pmatrix4f pDesM,	// 目标矩阵指针
										float fAngleY,			// Y 旋转角度（弧度制）
										float fAngleX,			// X 旋转角度（弧度制）
										float fAngleZ);			// Z 旋转角度（弧度制）
// 对任意给定角位移，存在无穷多个欧拉角可以用来表示它，称作“别名”问题。
// 因此，采用“限制欧拉角”，Y 与 Z 的范围为 [-180, 180]，X 的范围为 [-90, 90]。
// X = asin(-m23)
// 当 cosX = 0 时，出现万向锁的情况，可以通过 -m31 和 m11 计算 Y
// Y = atan2(-m31, m11)    Z = 0
// 正常情况则通过 m13 和 m33 计算 Y，通过 m21 和 m22 计算 Z
// Y = atan2(m13, m33)    Z = atan2(m21, m22)
BASEGRAPH_COMMON_API void __cdecl matrix4f_toeuler_lyxz(pmatrix4f pSrcM,	// 源矩阵指针
									  float* pfAngleY,		// Y 旋转角度（弧度制）
									  float* pfAngleX,		// X 旋转角度（弧度制）
									  float* pfAngleZ);		// Z 旋转角度（弧度制）

BASEGRAPH_COMMON_API void __cdecl matrix4f_fromeuler_lxyz(pmatrix4f pDesM,	// 目标矩阵指针
										float fAngleX,			// X 旋转角度（弧度制）
										float fAngleY,			// Y 旋转角度（弧度制）
										float fAngleZ);			// Z 旋转角度（弧度制）
BASEGRAPH_COMMON_API void __cdecl matrix4f_toeuler_lxyz(const matrix4f* pSrcM,	// 源矩阵指针
									  float* pfAngleX,		// X 旋转角度（弧度制）
									  float* pfAngleY,		// Y 旋转角度（弧度制）
									  float* pfAngleZ);	// Z 旋转角度（弧度制）
//////////////////////////////////////////////////////////////////////////
// 描述：	位移与旋转矩阵的变换（必须定义旋转顺序以及坐标系方向）
//
// 返回值：	无
//////////////////////////////////////////////////////////////////////////

BASEGRAPH_COMMON_API void __cdecl matrix4f_from_pos(pmatrix4f pDesM,	// 目标矩阵指针
										float fX,			// X方向位移 
										float fY,			// Y方向位移
										float fZ);			// Z方向位移

BASEGRAPH_COMMON_API void __cdecl matrix4f_to_pos(const matrix4f* pSrcM,	// 源矩阵指针
									  float* pfX,		// X方向位移
									  float* pfY,		// Y方向位移
									  float* pfZ);		// Z方向位移

BASEGRAPH_COMMON_API void __cdecl matrix4f_from_scale(pmatrix4f pDesM,	// 目标矩阵指针
										float fX,			// X方向放缩 
										float fY,			// Y方向放缩
										float fZ);			// Z方向放缩

//////////////////////////////////////////////////////////////////////////
//创建view矩阵
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl matrix4f_create_view(matrix4f* pDesM,
								 const float3* pEye,
								 const float3* pAt,
								 const float3* pUp);
///////////////////////////////////////////////////////////////////////////////
// 描述：	矩阵的转置
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl matrix4f_transpose(pmatrix4f pDesM,		// 目标矩阵指针
								   const matrix4f* pSrcM);	// 源矩阵指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩阵的求逆（采用初等行变换法，O(n^3)）
//
// 返回值:	TRUE		求解成功
//			FALSE		求解失败
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl matrix4f_inverse(pmatrix4f pDesM,		// 目标矩阵指针
								 const matrix4f* pSrcM);		// 源矩阵指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩阵的相等判断
//
// 返回值:	TRUE		相等
//			FALSE		不等
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl matrix4f_isequal(matrix4f* pM1,matrix4f* pM2);

//////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩阵的单位矩阵判断
//
// 返回值:	TRUE		相等
//			FALSE		不等
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl matrix4f_isidentity(matrix4f* pM);

//////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩阵M=SRP。知M逆算S（放缩矩阵）、R（旋转矩阵）、P（位移矩阵）
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////

BASEGRAPH_COMMON_API void __cdecl matrix4f_issolvesrp(const matrix4f* pM,matrix4f* pS,matrix4f* pR,matrix4f* pP);
BASEGRAPH_COMMON_API void __cdecl matrix4f_decompose(const matrix4f* pSrcMat,float3* pScale,float3* pAngle,float3* pPos);
//////////////////////////////////////////////////////////////////////////

// C++ 代码

#ifdef __cplusplus

#define USE_MATRIXQ4F_C_FUNC			// 使用 C 函数标志

//////////////////////////////////////////////////////////////////////////
// 结构重载操作符

inline void matrix4f_to_pos_vector(matrix4f* mat,float3* vPos)
{
	matrix4f_to_pos(mat,&vPos->x,&vPos->y,&vPos->z);
}

inline matrix4f& operator - (matrix4f& m1)
{
#ifdef USE_MATRIXQ4F_C_FUNC
	matrix4f_negative(&m1, &m1);
#else
	m1._11 = -m1._11; m1._12 = -m1._12; m1._13 = -m1._13; m1._14 = -m1._14;
	m1._21 = -m1._21; m1._22 = -m1._22; m1._23 = -m1._23; m1._24 = -m1._24;
	m1._31 = -m1._31; m1._32 = -m1._32; m1._33 = -m1._33; m1._34 = -m1._34;
	m1._41 = -m1._41; m1._42 = -m1._42; m1._43 = -m1._43; m1._44 = -m1._44;
#endif

	return m1;
}

inline matrix4f operator - (matrix4f& m1, matrix4f& m2)
{
	matrix4f mre;

#ifdef USE_MATRIXQ4F_C_FUNC
	matrix4f_minus(&mre, &m1, &m2);
#else
	mre._11 = m1._11 - m2._11; mre._12 = m1._12 - m2._12; mre._13 = m1._13 - m2._13; mre._14 = m1._14 - m2._14;
	mre._21 = m1._21 - m2._21; mre._22 = m1._22 - m2._22; mre._23 = m1._23 - m2._23; mre._24 = m1._24 - m2._24;
	mre._31 = m1._31 - m2._31; mre._32 = m1._32 - m2._32; mre._33 = m1._33 - m2._33; mre._34 = m1._34 - m2._34;
	mre._41 = m1._41 - m2._41; mre._42 = m1._42 - m2._42; mre._43 = m1._43 - m2._43; mre._44 = m1._44 - m2._44;
#endif

	return mre;
}

inline matrix4f& operator -= (matrix4f& m1, matrix4f& m2)
{
#ifdef USE_MATRIXQ4F_C_FUNC
	matrix4f_minus(&m1, &m1, &m2);
#else
	m1._11 -= m2._11; m1._12 -= m2._12; m1._13 -= m2._13; m1._14 -= m2._14;
	m1._21 -= m2._21; m1._22 -= m2._22; m1._23 -= m2._23; m1._24 -= m2._24;
	m1._31 -= m2._31; m1._32 -= m2._32; m1._33 -= m2._33; m1._34 -= m2._34;
	m1._41 -= m2._41; m1._42 -= m2._42; m1._43 -= m2._43; m1._44 -= m2._44;
#endif

	return m1;
}

inline matrix4f operator + (matrix4f& m1, matrix4f& m2)
{
	matrix4f mre;

#ifdef USE_MATRIXQ4F_C_FUNC
	matrix4f_add(&mre, &m1, &m2);
#else
	mre._11 = m1._11 + m2._11; mre._12 = m1._12 + m2._12; mre._13 = m1._13 + m2._13; mre._14 = m1._14 + m2._14;
	mre._21 = m1._21 + m2._21; mre._22 = m1._22 + m2._22; mre._23 = m1._23 + m2._23; mre._24 = m1._24 + m2._24;
	mre._31 = m1._31 + m2._31; mre._32 = m1._32 + m2._32; mre._33 = m1._33 + m2._33; mre._34 = m1._34 + m2._34;
	mre._41 = m1._41 + m2._41; mre._42 = m1._42 + m2._42; mre._43 = m1._43 + m2._43; mre._44 = m1._44 + m2._44;
#endif

	return mre;
}

inline matrix4f& operator += (matrix4f& m1, matrix4f& m2)
{
#ifdef USE_MATRIXQ4F_C_FUNC
	matrix4f_add(&m1, &m1, &m2);
#else
	m1._11 += m2._11; m1._12 += m2._12; m1._13 += m2._13; m1._14 += m2._14;
	m1._21 += m2._21; m1._22 += m2._22; m1._23 += m2._23; m1._24 += m2._24;
	m1._31 += m2._31; m1._32 += m2._32; m1._33 += m2._33; m1._34 += m2._34;
	m1._41 += m2._41; m1._42 += m2._42; m1._43 += m2._43; m1._44 += m2._44;
#endif

	return m1;
}

inline matrix4f operator + (matrix4f& m1, float k)
{
	matrix4f mre;

#ifdef USE_MATRIXQ4F_C_FUNC
	matrix4f_addnum(&mre, &m1, k);
#else
	memcpy(&mre, &m1, sizeof(matrix4f));
	mre._11 += k; mre._22 += k; mre._33 += k; mre._44 += k;
#endif

	return mre;
}

inline matrix4f& operator += (matrix4f& m1, float k)
{
#ifdef USE_MATRIXQ4F_C_FUNC
	matrix4f_addnum(&m1, &m1, k);
#else
	m1._11 += k; m1._22 += k; m1._33 += k; m1._44 += k;
#endif

	return m1;
}

inline matrix4f operator - (matrix4f& m1, float k)
{
	matrix4f mre;

#ifdef USE_MATRIXQ4F_C_FUNC
	matrix4f_addnum(&mre, &m1, -k);
#else
	memcpy(&mre, &m1, sizeof(matrix4f));
	mre._11 -= k; mre._22 -= k; mre._33 -= k; mre._44 -= k;
#endif

	return mre;
}

inline matrix4f& operator -= (matrix4f& m1, float k)
{
#ifdef USE_MATRIXQ4F_C_FUNC
	matrix4f_addnum(&m1, &m1, -k);
#else
	m1._11 -= k; m1._22 -= k; m1._33 -= k; m1._44 -= k;
#endif

	return m1;
}

inline matrix4f operator * (matrix4f& m1, float k)
{
	matrix4f mre;

#ifdef USE_MATRIXQ4F_C_FUNC
	matrix4f_mulnum(&mre, &m1, k);
#else
	mre._11 = m1._11 * k; mre._12 = m1._12 * k; mre._13 = m1._13 * k; mre._14 = m1._14 * k;
	mre._21 = m1._21 * k; mre._22 = m1._22 * k; mre._23 = m1._23 * k; mre._24 = m1._24 * k;
	mre._31 = m1._31 * k; mre._32 = m1._32 * k; mre._33 = m1._33 * k; mre._34 = m1._34 * k;
	mre._41 = m1._41 * k; mre._42 = m1._42 * k; mre._43 = m1._43 * k; mre._44 = m1._44 * k;
#endif

	return mre;
}

inline matrix4f& operator *= (matrix4f& m1, float k)
{
#ifdef USE_MATRIXQ4F_C_FUNC
	matrix4f_mulnum(&m1, &m1, k);
#else
	m1._11 *= k; m1._12 *= k; m1._13 *= k; m1._14 *= k;
	m1._21 *= k; m1._22 *= k; m1._23 *= k; m1._24 *= k;
	m1._31 *= k; m1._32 *= k; m1._33 *= k; m1._34 *= k;
	m1._41 *= k; m1._42 *= k; m1._43 *= k; m1._44 *= k;
#endif

	return m1;
}

inline matrix4f operator * (const matrix4f& m1,const matrix4f& m2)
{
	matrix4f mre;

	matrix4f_mul(&mre, &m1, &m2);

	return mre;
}

inline matrix4f& operator *= (matrix4f& m1,const matrix4f& m2)
{
#ifdef _USE_SSE_OPTIMAL_
	matrix4f_mul(&m1, &m1, &m2);
#else
	matrix4f mre;

	matrix4f_mul(&mre, &m1, &m2);
	memcpy(&m1, &mre, sizeof(matrix4f));
#endif

	return m1;
}

inline matrix4f operator * (matrix4f& m1, SYQuatn& q)
{
	matrix4f mre;

	matrix4f_mulquatn(&mre, &m1, &q);

	return mre;
}

inline matrix4f& operator *= (matrix4f& m1, SYQuatn& q)
{
#ifdef _USE_SSE_OPTIMAL_
	matrix4f_mulquatn(&m1, &m1, &q);
#else
	matrix4f mre;

	matrix4f_mulquatn(&mre, &m1, &q);
	memcpy(&m1, &mre, sizeof(matrix4f));
#endif
	
	return m1;
}

inline matrix4f operator / (matrix4f& m1, float k)
{
	matrix4f mre;

#ifdef USE_MATRIXQ4F_C_FUNC
	matrix4f_mulnum(&mre, &m1, 1.f / k);
#else
	float kTmp = 1.f / k;

	mre._11 = m1._11 * kTmp; mre._12 = m1._12 * kTmp; mre._13 = m1._13 * kTmp; mre._14 = m1._14 * kTmp;
	mre._21 = m1._21 * kTmp; mre._22 = m1._22 * kTmp; mre._23 = m1._23 * kTmp; mre._24 = m1._24 * kTmp;
	mre._31 = m1._31 * kTmp; mre._32 = m1._32 * kTmp; mre._33 = m1._33 * kTmp; mre._34 = m1._34 * kTmp;
	mre._41 = m1._41 * kTmp; mre._42 = m1._42 * kTmp; mre._43 = m1._43 * kTmp; mre._44 = m1._44 * kTmp;
#endif

	return mre;
}

inline matrix4f& operator /= (matrix4f& m1, float k)
{
#ifdef USE_MATRIXQ4F_C_FUNC
	matrix4f_mulnum(&m1, &m1, 1.f / k);
#else
	float kTmp = 1.f / k;

	m1._11 *= kTmp; m1._12 *= kTmp; m1._13 *= kTmp; m1._14 *= kTmp;
	m1._21 *= kTmp; m1._22 *= kTmp; m1._23 *= kTmp; m1._24 *= kTmp;
	m1._31 *= kTmp; m1._32 *= kTmp; m1._33 *= kTmp; m1._34 *= kTmp;
	m1._41 *= kTmp; m1._42 *= kTmp; m1._43 *= kTmp; m1._44 *= kTmp;
#endif

	return m1;
}
inline	BOOL operator == (matrix4f& m1,matrix4f& m2)
{
	return matrix4f_isequal(&m1,&m2);
}
inline	BOOL operator != (matrix4f& m1,matrix4f& m2)
{
	return !matrix4f_isequal(&m1,&m2);
}
#endif
//////////////////////////////////////////////////////////////////////////
// End of file.

