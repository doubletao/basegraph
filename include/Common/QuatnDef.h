#pragma once

#include "Common\MatrixDef.h"
#include "Common\VectorDef.h"


typedef struct tag_quatn {
	float	x;				// i
	float	y;				// j
	float	z;				// k
	float	r;				// 纯量
}BaseGraphQuatn;

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_Set(BaseGraphQuatn* pQ,			// 四元数指针
						 float r,				// 纯量
						 float x,				// i
						 float y,				// j
						 float z);				// k

//////////////////////////////////////////////////////////////////////////
// Description:		identity quaternion
//
// Return Value:	None
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_Identity(BaseGraphQuatn* pQ);	// destination pointer

///////////////////////////////////////////////////////////////////////////////
// 描述：	比较
//
// 返回值:	TRUE		相等
//			FALSE		不相等
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API BOOL __cdecl BaseGraphQuatn_Equal(const BaseGraphQuatn* pQ1,		// 四元数指针
						   const BaseGraphQuatn* pQ2);		// 四元数指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	负四元数
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_Negtive(BaseGraphQuatn* pQ);		// 四元数指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	加减法
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_Add(BaseGraphQuatn* pDesQ,			// 目标四元数指针
						 const BaseGraphQuatn* pSrcQL,			// 左源四元数指针
						 const BaseGraphQuatn* pSrcQR);			// 右源四元数指针

BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_Minus(BaseGraphQuatn* pDesQ,			// 目标四元数指针
						   const BaseGraphQuatn* pSrcQL,		// 左源四元数指针
						   const BaseGraphQuatn* pSrcQR);		// 右源四元数指针

BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_Sub(BaseGraphQuatn* pDesQ,			// 目标四元数指针
						 const BaseGraphQuatn* pSrcQL,			// 左源四元数指针
						 const BaseGraphQuatn* pSrcQR);			// 右源四元数指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	乘法
//			由于本系统使用左手坐标系，以及行向量乘以矩阵，矩阵的级联方式为：旧矩阵 *= 新矩阵
//			为了使四元数也满足这种级联方式，定义与标准乘法不同的乘法，系统采用自定义乘法。
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////

// 标准乘法定义：q1 * q2 = [ r1*r2 - v1*v2		r1*v2 + r2*v1 + v1 x v2 ]
// 旋转公式为：p2 = q * p1 * q^(-1)
//	r = ┌ r1*r2 - x1*x2 - y1*y2 - z1*z2 ┐
//	x = │ r1*x2 + r2*x1 + y1*z2 - z1*y2 │
//	y = │ r1*y2 + r2*y1 + z1*x2 - x1*z2 │
//	z = ┕ r1*z2 + r2*z1 + x1*y2 - y1*x2 ┘
BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_StandardMul(BaseGraphQuatn* pDesQ,		// 目标四元数指针
								 const BaseGraphQuatn* pSrcQL,		// 左源四元数指针
								 const BaseGraphQuatn* pSrcQR);		// 右源四元数指针

// 自定义乘法定义：q1 * q2 = [ r1*r2 - v1*v2	r1*v2 + r2*v1 + v2 x v1 ]
// 旋转公式为：p2 = q^(-1) * p1 * q
//	r = ┌ r1*r2 - x1*x2 - y1*y2 - z1*z2 ┐
//	x = │ r1*x2 + r2*x1 + y2*z1 - z2*y1 │
//	y = │ r1*y2 + r2*y1 + z2*x1 - x2*z1 │
//	z = ┕ r1*z2 + r2*z1 + x2*y1 - y2*x1 ┘
BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_CustomMul(BaseGraphQuatn* pDesQ,			// 目标四元数指针
							   const BaseGraphQuatn* pSrcQL,		// 左源四元数指针
							   const BaseGraphQuatn* pSrcQR);		// 右源四元数指针

#define quatnMul			BaseGraphQuatn_CustomMul

///////////////////////////////////////////////////////////////////////////////
// 描述：	共轭
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_Conjugate(BaseGraphQuatn* pDesQ,			// 目标四元数指针
							   const BaseGraphQuatn* pSrcQ);		// 源四元数指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	归一化
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_Normalize(BaseGraphQuatn* pDesQ,			// 目标四元数指针
							   const BaseGraphQuatn* pSrcQ);		// 源四元数指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	缩放
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_Scale(BaseGraphQuatn* pQ,			// 源四元数指针
						   float k);				// 系数

BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_MulNum(BaseGraphQuatn* pDesQ,		// 目标四元数指针
							const BaseGraphQuatn* pSrcQ,		// 源四元数指针
							float k);				// 系数

///////////////////////////////////////////////////////////////////////////////
// 描述：	取模
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API float __cdecl BaseGraphQuatn_Mode(const BaseGraphQuatn* pQ);			// 四元数指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	距离
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API float __cdecl BaseGraphQuatn_Distance(const BaseGraphQuatn* pQ1,			// 四元数指针
							   const BaseGraphQuatn* pQ2);			// 四元数指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	点积（内积）
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API float __cdecl BaseGraphQuatn_DotProduct(const BaseGraphQuatn* pQ1,			// 四元数指针
								 const BaseGraphQuatn* pQ2);		// 四元数指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	外积
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_OutterProduct(BaseGraphQuatn* pDesQ,			// 目标四元数指针
								   const BaseGraphQuatn* pSrcQL,		// 左源四元数指针
								   const BaseGraphQuatn* pSrcQR);		// 右源四元数指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	偶积
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_EvenProduct(BaseGraphQuatn* pDesQ,		// 目标四元数指针
								 const BaseGraphQuatn* pSrcQL,		// 左源四元数指针
								 const BaseGraphQuatn* pSrcQR);		// 右源四元数指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	叉积
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_CrossProduct(float3* pDesV,	// 目标向量指针
								  const BaseGraphQuatn* pSrcQL,		// 左源四元数指针
								  const BaseGraphQuatn* pSrcQR);	// 右源四元数指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	求逆
//
// 返回值：	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_Inverse(BaseGraphQuatn* pDesQ,		// 目标四元数指针
							 const BaseGraphQuatn* pSrcQ);		// 源四元数指针

//////////////////////////////////////////////////////////////////////////
// 描述：	球面插值，以最短弧路径进行插值，则要求两个四元数的点积必须为正值，
//			如果求出为负，则翻转其中一个四元数的符号。
//
//						  sin((1-t)W)		   sin(tW)
//		slerp(q0,q1,t) = ------------- * q0 + --------- * q1
//						    sin(W)				sin(W)
//
//		其中：cos(W) = q0·q1（点积）
//
// 返回值：	无
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_Slerp(BaseGraphQuatn* pDesQ,			// 目标四元数指针
						   const BaseGraphQuatn* pSrcQ0,		// 源起始四元数
						   const BaseGraphQuatn* pSrcQ1,		// 源终端四元数
						   float t);				// 插值系数 [0～1]

BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_Slerp_AxisAngle( vector4f* vresult,vector4f* v1,vector4f* v2 ,float f);
///////////////////////////////////////////////////////////////////////////////
// 描述：	与矩阵的转换
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_ToMatrix4(matrix4f* pDesM,		// 目标矩阵指针
							  const BaseGraphQuatn* pSrcQ);			// 源四元数指针

BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_FromMatrix(BaseGraphQuatn* pDesQ,		// 目标四元数指针
								const matrix4f* pSrcM);		// 源矩阵指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	与矩阵的转换（要求输入归一化的四元数）
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
//void __cdecl quatnToMatrixQ4F(const matrix4f* pDesM,		// 目标矩阵指针
//								 const BaseGraphQuatn* pSrcQ);			// 源四元数指针
//
//void __cdecl quatnFromMatrix4(BaseGraphQuatn* pDesQ,			// 目标四元数指针
//								   const matrix4f* pSrcM);	// 源矩阵指针

BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_FromVector4f(BaseGraphQuatn* pDesQ,			// 目标四元数指针
								   const float4* pSrcV);// 角度（弧度）
///////////////////////////////////////////////////////////////////////////////
// 描述：	绕任意轴旋转的四元数（为了保证与矩阵的旋转统一，将在内部使用负角度，
//			即得到从旧坐标系到新坐标系的变换。）
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_FromAngleAxis(BaseGraphQuatn* pDesQ,			// 目标四元数指针
								   const float3* pSrcV,		// 源向量指针
								   float fAngle);			// 角度（弧度）
// Rotate about special axes, such as X(1, 0, 0)、Y(0, 1, 0)、Z(0, 0, 1).
BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_FromAngleX(BaseGraphQuatn* pDesQ,	// destination pointer
								float fAngle);		// radian of ratation
BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_FromAngleY(BaseGraphQuatn* pDesQ,	// destination pointer
								float fAngle);		// radian of ratation
BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_FromAngleZ(BaseGraphQuatn* pDesQ,	// destination pointer
								float fAngle);		// radian of ratation

BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_ToAxisAngle(vector4f* pAxisAngle,BaseGraphQuatn* pQ4 );
///////////////////////////////////////////////////////////////////////////////
// 描述：	欧拉角与四元数的转换（要求从四元数到欧拉角变换中，必须输入归一化式）
//			本系统使用行向量与矩阵的乘法，并定义左手坐标系，旋转的正方向定义为：
//			以左手法则，从旋转轴的正端点看向负端点，顺时针方向为正。
//			根据自定义的乘法以及级联方式，得到的四元数将等效于从旧坐标系到新坐标系的变换矩阵。
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////

// 左手坐标系，旋转顺序为 Y X Z，自定义乘法，旧 *= 新 的级联方式。
// r = ┌  cos(y/2)cos(x/2)cos(z/2) + sin(y/2)sin(x/2)sin(z/2) ┐
// x = │ -sin(y/2)cos(x/2)sin(z/2) - cos(y/2)sin(x/2)cos(z/2) │
// y = │ -sin(y/2)cos(x/2)cos(z/2) + cos(y/2)sin(x/2)sin(z/2) │
// z = └ -cos(y/2)cos(x/2)sin(z/2) + sin(y/2)sin(x/2)cos(z/2) ┘
BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_FromEuler_LYXZ(BaseGraphQuatn* pDesQ,	// 目标四元数指针
									float fAngleY,		// 绕 Y 轴旋转角度
									float fAngleX,		// 绕 X 轴旋转角度
									float fAngleZ);		// 绕 Z 轴旋转角度
// 根据四元数到矩阵与矩阵到欧拉角的公式，可以推导出四元数到欧拉角的公式。
// 根据四元数到矩阵变换的要求，输入四元数必须为归一化式。
// m23 = -sinX = 2yz + 2rx
// m21 = cosXsinZ = 2xy - 2rz    m22 = cosXcosZ = r^2 + y^2 - x^2 - z^2
// m13 = sinYcosX = 2xz - 2ry    m33 = cosYcosX = r^2 + z^2 - x^2 - y^2
// m11 = cosYcosZ + sinYsinXsinZ = r^2 + x^2 - y^2 - z^2
// m31 = cosYsinXsinZ - sinYcosZ = 2xz + 2ry
//
// X = asin(-m23) = asin(-2yz - 2rx)
// 当 cosX = 0 时，出现万向锁的情况，可以通过 -m31 和 m11 计算 Y
// Z = 0    Y = atan2(-m31, m11) = atan2(-2xz - 2ry, r^2 + x^2 - y^2 - z^2)
// 正常情况则通过 m13 和 m33 计算 Y，通过 m21 和 m22 计算 Z
// Y = atan2(m13, m33) = atan2(2xz - 2ry, r^2 + z^2 - x^2 - y^2)
// Z = atan2(m21, m22) = atan2(2xy - 2rz, r^2 + y^2 - x^2 - z^2)
BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_ToEuler_LYXZ(const BaseGraphQuatn* pSrcQ,		// 源四元数指针
								  float* pfAngleY,		// Y 旋转角度（弧度制）
								  float* pfAngleX,		// X 旋转角度（弧度制）
								  float* pfAngleZ);		// Z 旋转角度（弧度制）

///////////////////////////////////////////////////////////////////////////////
// 描述：	旋转向量，转换为矩阵再使用行向量乘以矩阵。
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_RotVectorL(long3* pDesV,		// 目标向量指针
								const long3* pSrcV,		// 源向量指针
								const BaseGraphQuatn* pUQ);				// 单位轴四元数指针

BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_RotVector3f(float3* pDesV,		// 目标向量指针
								const float3* pSrcV,		// 源向量指针
								const BaseGraphQuatn* pUQ);				// 单位轴四元数指针

BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_RotVector3lf(double3* pDesV,		// 目标向量指针
								const double3* pSrcV,		// 源向量指针
								const BaseGraphQuatn* pUQ);				// 单位轴四元数指针

//////////////////////////////////////////////////////////////////////////
// 描述：	与矩阵的乘积
//
// 返回值：	无
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_MulMatQ(BaseGraphQuatn* pDesQ,			// 目标四元数指针
							 const BaseGraphQuatn* pSrcQ,			// 源四元数指针
							 const matrix4f* pSrcM);		// 源矩阵指针

//////////////////////////////////////////////////////////////////////////
// 描述：	获取从一个向量旋转到另一个向量的四元数
//
// 返回值：	无
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_In2Vector(BaseGraphQuatn* pDesQ,			// 目标四元数指针
							   const float3* pStartV,	// 起始向量指针
							   const float3* pEndV);	// 结束向量指针

BASEGRAPH_COMMON_API void __cdecl BaseGraphQuatn_In2Vector3lf(BaseGraphQuatn* pDesQ,			// 目标四元数指针
							 const double3* pStartV,	// 起始向量指针
							 const double3* pEndV);		// 结束向量指针

//////////////////////////////////////////////////////////////////////////
// C++ 代码

#ifdef __cplusplus

#define USE_QUATN_C_FUNC		// 使用 C 接口

//////////////////////////////////////////////////////////////////////////
// 结构操作符重载

inline BaseGraphQuatn& operator - (BaseGraphQuatn& q1)
{
#ifdef USE_QUATN_C_FUNC
	BaseGraphQuatn_Negtive(&q1);
#else
	q1.x = -q1.x;
	q1.y = -q1.y;
	q1.z = -q1.z;
	q1.r = -q1.r;
#endif

	return q1;
}

inline BaseGraphQuatn operator + (const BaseGraphQuatn& q1,const BaseGraphQuatn& q2)
{
	BaseGraphQuatn qre;

#ifdef USE_QUATN_C_FUNC
	BaseGraphQuatn_Add(&qre, &q1, &q2);
#else
	qre.x = q1.x + q2.x;
	qre.y = q1.y + q2.y;
	qre.z = q1.z + q2.z;
	qre.r = q1.r + q2.r;
#endif

	return qre;
}

inline BaseGraphQuatn& operator += (BaseGraphQuatn& q1,const BaseGraphQuatn& q2)
{
#ifdef USE_QUATN_C_FUNC
	BaseGraphQuatn_Add(&q1, &q1, &q2);
#else
	q1.x += q2.x;
	q1.y += q2.y;
	q1.z += q2.z;
	q1.r += q2.r;
#endif

	return q1;
}

inline BaseGraphQuatn operator - (const BaseGraphQuatn& q1, const BaseGraphQuatn& q2)
{
	BaseGraphQuatn qre;

#ifdef USE_QUATN_C_FUNC
	BaseGraphQuatn_Minus(&qre, &q1, &q2);
#else
	qre.x = q1.x - q2.x;
	qre.y = q1.y - q2.y;
	qre.z = q1.z - q2.z;
	qre.r = q1.r - q2.r;
#endif

	return qre;
}

inline BaseGraphQuatn& operator -= (BaseGraphQuatn& q1,const BaseGraphQuatn& q2)
{
#ifdef USE_QUATN_C_FUNC
	BaseGraphQuatn_Minus(&q1, &q1, &q2);
#else
	q1.x -= q2.x;
	q1.y -= q2.y;
	q1.z -= q2.z;
	q1.r -= q2.r;
#endif

	return q1;
}

inline BaseGraphQuatn operator * (const BaseGraphQuatn& q1, float f)
{
	BaseGraphQuatn qre;

#ifdef USE_QUATN_C_FUNC
	BaseGraphQuatn_MulNum(&qre, &q1, f);
#else
	qre.x = q1.x * f;
	qre.y = q1.y * f;
	qre.z = q1.z * f;
	qre.r = q1.r * f;
#endif

	return qre;
}

inline BaseGraphQuatn& operator *= (BaseGraphQuatn& q1, float f)
{
#ifdef USE_QUATN_C_FUNC
	BaseGraphQuatn_Scale(&q1, f);
#else
	q1.x *= f;
	q1.y *= f;
	q1.z *= f;
	q1.r *= f;
#endif

	return q1;
}

inline BaseGraphQuatn operator / (const BaseGraphQuatn& q1, float f)
{
	BaseGraphQuatn qre;

#ifdef USE_QUATN_C_FUNC
	BaseGraphQuatn_MulNum(&qre, &q1, 1.f / f);
#else
	float fTmp = 1.f / f;

	qre.x = q1.x * fTmp;
	qre.y = q1.y * fTmp;
	qre.z = q1.z * fTmp;
	qre.r = q1.r * fTmp;
#endif

	return qre;
}

inline BaseGraphQuatn& operator /= (BaseGraphQuatn& q1, float f)
{
#ifdef USE_QUATN_C_FUNC
	BaseGraphQuatn_Scale(&q1, 1.f / f);
#else
	float fTmp = 1.f / f;
	q1.x *= fTmp;
	q1.y *= fTmp;
	q1.z *= fTmp;
	q1.r *= fTmp;
#endif

	return q1;
}

inline BaseGraphQuatn operator * (const BaseGraphQuatn& q,const matrix4f& m)
{
	BaseGraphQuatn qre;

	BaseGraphQuatn_MulMatQ(&qre, &q, &m);

	return qre;
}

inline BaseGraphQuatn& operator *= (BaseGraphQuatn& q,const matrix4f& m)
{
#ifdef _USE_SSE_OPTIMAL_
	BaseGraphQuatn_MulMatQ(&q, &q, &m);
#else
	BaseGraphQuatn qre;
	
	BaseGraphQuatn_MulMatQ(&qre, &q, &m);
	memcpy(&q, &qre, sizeof(BaseGraphQuatn));
#endif

	return q;
}

inline BaseGraphQuatn operator * (const BaseGraphQuatn& q1,const BaseGraphQuatn& q2)
{
	BaseGraphQuatn qre;

	BaseGraphQuatn_CustomMul(&qre, &q1, &q2);

	return qre;
}

inline BaseGraphQuatn& operator *= (BaseGraphQuatn& q1,const BaseGraphQuatn& q2)
{
	BaseGraphQuatn qre;

	BaseGraphQuatn_CustomMul(&qre, &q1, &q2);
	memcpy(&q1, &qre, sizeof(BaseGraphQuatn));

	return q1;
}

#endif

//------------------------------- End of File ---------------------------------
