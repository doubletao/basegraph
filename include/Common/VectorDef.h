#pragma once
#include <math.h>
#include "Common\MathDef.h"
struct tag_matrixf4x4;
typedef struct tag_matrixf4x4 matrix4f;
//////////////////////////////// 二维向量 和 SYPoint/////////////////////////////////////

typedef struct tag_long2{		// 整型
	long	x;
	long	y;
}long2,point2,vector2,SYPoint, *LPCommonPOINTL;

typedef struct tag_float2{		// 浮点型
	float	x;
	float	y;
}float2,pointf,point2f,vector2f,SYPoint2f, *LPCommonPOINTF;


typedef struct tag_double2{		// 双精度型
	double	x;
	double	y;
}double2,point2lf,vector2lf,SYPoint2lf, *LPCommonPOINTD;


///////////////////////////////////////////////////////////////////////////////
// 描述：	设置向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl long2Set(long2* pV,			// 向量指针
						   long x,
						   long y);

///////////////////////////////////////////////////////////////////////////////
// 描述：	比较向量
//
// 返回值:	true		相等
//			false		不等
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API bool __cdecl long2Equal(const long2* pV1,		// 向量指针
							 const long2* pV2);		// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	零化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl long2Zero(long2* pV);			// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	缩放向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl long2Scale(long2* pV,			// 向量指针
							 long k);					// 缩放倍数

///////////////////////////////////////////////////////////////////////////////
// 描述：	归一化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl long2Normalize(float2* pDesV,			// 目标向量指针
								 const long2* pSrcV);		// 源向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	取向量的模
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API float __cdecl long2Mode(const long2* pV);			// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量点乘
//
// 返回值:	long
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API long __cdecl long2DotProduct(const long2* pV1,		// 向量指针
								  const long2* pV2);		// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量的距离
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API float __cdecl long2Distance(const long2* pV1,			// 向量指针
								 const long2* pV2);			// 向量指针


// 浮点型实现

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl float2Set(float2* pV,			// 向量指针
						   float x,
						   float y);

///////////////////////////////////////////////////////////////////////////////
// 描述：	比较向量
//
// 返回值:	true		相等
//			false		不等
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API bool __cdecl float2Equal(const float2* pV1,		// 向量指针
							 const float2* pV2);		// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	零化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl float2Zero(float2* pV);			// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	缩放向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl float2Scale(float2* pV,			// 向量指针
							 float k);					// 缩放倍数

///////////////////////////////////////////////////////////////////////////////
// 描述：	归一化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl float2Normalize(float2* pDesV,			// 目标向量指针
								 const float2* pSrcV);		// 源向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	取向量的模
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API float __cdecl float2Mode(const float2* pV);			// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量点乘
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API float __cdecl float2DotProduct(const float2* pV1,		// 向量指针
								   const float2* pV2);		// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量的距离
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API float __cdecl float2Distance(const float2* pV1,			// 向量指针
								 const float2* pV2);			// 向量指针


///////////////////////////////////////////////////////////////////////////////
// 描述：	向量和矩阵相乘
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl float2MulMatQ(float2* pDesV,			// 目标向量指针
						   const float2* pSrcV,			// 源向量指针
						   const matrix4f* pSrcM);			// 矩阵指针


BASEGRAPH_COMMON_API void __cdecl float2Cross(pointf &ptresult,pointf pt1,pointf pt2);
// 双精度型实现

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl double2Set(double2* pV,			// 向量指针
						   double x,
						   double y);

///////////////////////////////////////////////////////////////////////////////
// 描述：	比较向量
//
// 返回值:	true		相等
//			false		不等
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API bool __cdecl double2Equal(const double2* pV1,		// 向量指针
							 const double2* pV2);		// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	零化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl double2Zero(double2* pV);			// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	缩放向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl double2Scale(double2* pV,			// 向量指针
							 double k);					// 缩放倍数

///////////////////////////////////////////////////////////////////////////////
// 描述：	归一化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl double2Normalize(double2* pDesV,			// 目标向量指针
								 const double2* pSrcV);		// 源向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	取向量的模
//
// 返回值:	double
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API double __cdecl double2Mode(const double2* pV);			// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量点乘
//
// 返回值:	double
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API double __cdecl double2DotProduct(const double2* pV1,		// 向量指针
									const double2* pV2);		// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量的距离
//
// 返回值:	double
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API double __cdecl double2Distance(const double2* pV1,			// 向量指针
								  const double2* pV2);			// 向量指针

BASEGRAPH_COMMON_API void __cdecl double2Cross(double2 &ptresult,double2 pt1,double2 pt2);
//////////////////////////////// 二维向量End //////////////////////////////////


//////////////////////////////// 三维向量 /////////////////////////////////////

typedef struct tag_long3{		// 整型
	long	x;
	long	y;
	long	z;
}long3,point3,vector3;


typedef struct tag_float3{		// 浮点型
	float	x;
	float	y;
	float	z;
}float3,point3f,vector3f;

typedef struct tag_double3{		// 双精度浮点型
	double	x;
	double	y;
	double	z;
}double3,point3lf,vector3lf;

enum SYPlane_Type
{
	Plane_XY = 0,
	Plane_XZ,
	Plane_YZ,
};
// 整型实现

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl long3Set(long3* pV,			// 向量指针
							 long x,
							 long y,
							 long z);

///////////////////////////////////////////////////////////////////////////////
// 描述：	比较向量
//
// 返回值:	true		相等
//			false		不等
///////////////////////////////////////////////////////////////////////////////
bool __cdecl long3Equal(const long3* pV1,		// 向量指针
							   const long3* pV2);		// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	零化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl long3Zero(const long3* pV);		// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	缩放向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl long3Scale(const long3* pV,		// 向量指针
							   long k);					// 缩放倍数

///////////////////////////////////////////////////////////////////////////////
// 描述：	归一化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl long3Normalize(const float3* pDesV,			// 目标向量指针
								   const long3* pSrcV);		// 源向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	取向量的模
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API float __cdecl long3Mode(const long3* pV);			// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量点乘
//
// 返回值:	long
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API long __cdecl long3DotProduct(const long3* pV1,		// 向量指针
									const long3* pV2);		// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量叉乘
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl long3CrossProduct(const long3* pDesV,		// 目标向量指针
									  const long3* pSrcV1,		// 源向量指针
									  const long3* pSrcV2);	// 源向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量的距离
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API float __cdecl long3Distance(const long3* pV1,			// 向量指针
								   const long3* pV2);			// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩阵与向量的乘积
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl long3MulMat(const long3* pDesV,			// 目标向量指针
								const long3* pSrcV,			// 源向量指针
								const matrix4f* pSrcM);				// 矩阵指针


// 浮点型实现

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取三维向量的某一维值
//
// 返回值:	浮点值
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API float __cdecl float3GetElement(const float3 &v1, const unsigned int nDim);

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl float3Set(float3* pV,			// 向量指针
							 float x,
							 float y,
							 float z);

///////////////////////////////////////////////////////////////////////////////
// 描述：	比较向量
//
// 返回值:	true		相等
//			false		不等
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API bool __cdecl float3Equal(const float3* pV1,		// 向量指针
							   const float3* pV2);		// 向量指针

BASEGRAPH_COMMON_API bool __cdecl float3Equal(const float3* pV1,		// 向量指针
	const float3* pV2, float fPre);		// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	零化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl float3Zero(float3* pV);			// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	负向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl float3Negtive(float3* pV);			// 向量指针

//////////////////////////////////////////////////////////////////////////
// 描述：	加减法
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl float3Add(float3* pDesV,		// 目标向量
							 const float3* pSrcV1,		// 向量 1
							 const float3* pSrcV2);		// 向量 2

BASEGRAPH_COMMON_API void __cdecl float3Sub(float3* pDesV,		// 目标向量
							 const float3* pSrcVL,		// 左向量
							 const float3* pSrcVR);		// 右向量

BASEGRAPH_COMMON_API void __cdecl float3AddNum(const float3* pDesV,	// 目标向量
								const float3* pSrcV,	// 向量
								float k);				// 数

BASEGRAPH_COMMON_API void __cdecl float3Offset(const float3* pV,		// 目标向量
								float k);				// 数

BASEGRAPH_COMMON_API void __cdecl float3Mul(float3* pDesV,const float3* pSrcV1,const float3* pSrcV2);
BASEGRAPH_COMMON_API void __cdecl float3Div(float3* pDesV,const float3* pSrcV1,const float3* pSrcV2);
///////////////////////////////////////////////////////////////////////////////
// 描述：	缩放向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl float3Scale(float3* pV,			// 向量指针
							   float k);					// 缩放倍数

BASEGRAPH_COMMON_API void __cdecl float3MulNum(float3* pDesV,		// 目标向量指针
								const float3* pSrcV,		// 源向量指针
                                float k);					// 缩放倍数

///////////////////////////////////////////////////////////////////////////////
// 描述：	归一化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl float3Normalize(float3* pDesV,			// 目标向量指针
								   const float3* pSrcV);		// 源向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	取向量的模
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API float __cdecl float3Mode(const float3* pV);			// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量点乘
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API float __cdecl float3DotProduct(const float3* pV1,		// 向量指针
									 const float3* pV2);	// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量叉乘
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl float3CrossProduct(float3* pDesV,		// 目标向量指针
									  const float3* pSrcV1,		// 源向量指针
									  const float3* pSrcV2);	// 源向量指针


///////////////////////////////////////////////////////////////////////////////
// 描述：	向量夹角
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API double __cdecl float3Angle(const float3* pSrcV1,	// 源向量指针
								const float3* pSrcV2);	// 源向量指针
//////////////////////////////////////////////////////////////////////////
//得到两向量间的夹角，得到的角度范围为-180，180
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API double __cdecl float3AngleBetVec(const float3* pSrcV1,
								 const float3* pSrcV2,
								 const float3* pUp);
///////////////////////////////////////////////////////////////////////////////
// 描述：	向量的距离
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API float __cdecl float3Distance(const float3* pV1,			// 向量指针
								   const float3* pV2);			// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩阵与向量的乘积
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
//V2 = V1 * M
BASEGRAPH_COMMON_API void __cdecl float3NormalMulMat(float3* pDesV,			// 目标向量指针
								const float3* pSrcV,			// 源向量指针
								const matrix4f* pSrcM);				// 矩阵指针
////V2 = M * V1
BASEGRAPH_COMMON_API void __cdecl float3MatMulNormal(float3* pDesV,			// 目标向量指针
								const matrix4f* pSrcM,				// 矩阵指针
								const float3* pSrcV);			// 源向量指针

//////////////////////////////////////////////////////////////////////////
// 描述：	三维向量与矩阵的乘积（补最后一维为单位值）
//			本系统采用行向量与矩阵的乘法定义
//
// 返回值：	无
//////////////////////////////////////////////////////////////////////////
//V2 = (V1,1) * M
BASEGRAPH_COMMON_API void __cdecl float3MulMatQ(float3* pDesV,			// 目标向量指针
								 const float3* pSrcV,			// 源向量指针
								 const matrix4f* pSrcM);			// 矩阵指针
////V2 = M * (V1,1)
BASEGRAPH_COMMON_API void __cdecl float3MatQMul(float3* pDesV,			// 目标向量指针
								 const matrix4f* pSrcM,			// 矩阵指针
								 const float3* pSrcV);			// 源向量指针

//////////////////////////////////////////////////////////////////////////
// 描述：	向量到初等变换矩阵的转换
//			由于本系统采用行向量与矩阵的乘法定义，因此偏移量将在矩阵最后一行
//
// 返回值：	无
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl float3ToScaleMatQ(matrix4f* pDesM,		// 目标矩阵指针
									 const float3* pSrcV);		// 源向量指针

BASEGRAPH_COMMON_API void __cdecl float3ToTransMatQ(matrix4f* pDesM,		// 目标矩阵指针
									 const float3* pSrcV);		// 源向量指针

BASEGRAPH_COMMON_API void __cdecl float3ToRotateZMatQ(matrix4f* pDesM,		// 目标矩阵指针
									 const float3* pSrcV);		// 源向量指针


BASEGRAPH_COMMON_API void __cdecl float3fFromfloat2f(float3* pV,		// 目标向量
								  float2* pt,
								  int nPlaneType = Plane_XY);

BASEGRAPH_COMMON_API void __cdecl float3fFromfloat2fEx(float3* pV,		// 目标向量
								float2* pt,
								float fvalue,
								int nPlaneType = Plane_XY);

BASEGRAPH_COMMON_API void __cdecl float3GetMin(float3* pV,const float3* pV1,const float3* pV2);
BASEGRAPH_COMMON_API void __cdecl float3GetMax(float3* pV,const float3* pV1,const float3* pV2);


BASEGRAPH_COMMON_API void __cdecl float2Fromfloat3(float2* des,float3* src);
template<typename t1,typename t2>
void __cdecl v2tov3(t1* des,
	 t2* src,
	 float fvalue,
	 int nPlaneType = Plane_XY)
{
	if( nPlaneType == Plane_XZ)
	{
		des->x = src->x;
		des->y = fvalue;
		des->z = src->y;
	}
	else if( nPlaneType == Plane_YZ)
	{
		des->x = fvalue;
		des->y = src->y;
		des->z = src->x;
	}
	else
	{
		des->x = src->x;
		des->y = src->y;
		des->z = fvalue;
	}
}

// 双精度型实现

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl double3Set(double3* pV,			// 向量指针
							 double x,
							 double y,
							 double z);

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl double3SetSingle(double3* pV,			// 向量指针
							  double dvalue);

///////////////////////////////////////////////////////////////////////////////
// 描述：	比较向量
//
// 返回值:	true		相等
//			false		不等
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API bool __cdecl double3Equal(const double3* pV1,		// 向量指针
							   const double3* pV2);		// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	零化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl double3Zero(double3* pV);			// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	缩放向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl double3Scale(double3* pV,			// 向量指针
							   double k);					// 缩放倍数

///////////////////////////////////////////////////////////////////////////////
// 描述：	各项异性缩放向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl double3ScaleV(double3* pDesV,double3* pScaleV);

///////////////////////////////////////////////////////////////////////////////
// 描述：	归一化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl double3Normalize(double3* pDesV,			// 目标向量指针
								   const double3* pSrcV);		// 源向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	取向量的模
//
// 返回值:	double
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API double __cdecl double3Mode(const double3* pV);			// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量点乘
//
// 返回值:	double
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API double __cdecl double3DotProduct(const double3* pV1,		// 向量指针
									  const double3* pV2);		// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量叉乘
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl double3CrossProduct(double3* pDesV,		// 目标向量指针
									  const double3* pSrcV1,		// 源向量指针
									  const double3* pSrcV2);	// 源向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：两向量的夹角
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API double __cdecl double3Angle(const double3* pSrcV1,	// 源向量指针
								 const double3* pSrcV2);	// 源向量指针

BASEGRAPH_COMMON_API double __cdecl double3AngleBetVec(const double3* pSrcV1,
									const double3* pSrcV2,
									const double3* pUp );

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量的距离
//
// 返回值:	double
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API double __cdecl double3Distance(const double3* pV1,			// 向量指针
									const double3* pV2);			// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩阵与向量的乘积
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl double3MulMat(double3* pDesV,			// 目标向量指针
								const double3* pSrcV,			// 源向量指针
								const matrix4f* pSrcM);				// 矩阵指针

//////////////////////////////// 三维向量End //////////////////////////////////


//////////////////////////////// 四维向量 /////////////////////////////////////

typedef struct tag_DSVectorF4D{		// 浮点型
	float	x;
	float	y;
	float	z;
	float	w;
}float4,point4f,vector4f;

// 浮点型实现

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl float4Set(float4* pV,			// 向量指针
							 float x,
							 float y,
							 float z,
							 float w);
///////////////////////////////////////////////////////////////////////////////
// 描述：	比较向量
//
// 返回值:	true		相等
//			false		不等
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API bool __cdecl float4Equal(const float4* pV1,		// 向量指针
							   const float4* pV2);		// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	零化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl float4Zero(float4* pV);			// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	负向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl float4Negtive(const float4* pV);			// 向量指针

//////////////////////////////////////////////////////////////////////////
// 描述：	加减法
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl float4Add(float4* pDesV,		// 目标向量
							 const float4* pSrcV1,		// 向量 1
							 const float4* pSrcV2);		// 向量 2

BASEGRAPH_COMMON_API void __cdecl float4Sub(float4* pDesV,		// 目标向量
							 const float4* pSrcVL,		// 左向量
							 const float4* pSrcVR);		// 右向量

BASEGRAPH_COMMON_API void __cdecl float4AddNum(float4* pDesV,	// 目标向量
								const float4* pSrcV,	// 向量
								float k);				// 数

BASEGRAPH_COMMON_API void __cdecl float4Offset(float4* pV,		// 目标向量
								float k);				// 数

///////////////////////////////////////////////////////////////////////////////
// 描述：	缩放向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl float4Scale(float4* pV,			// 向量指针
							   float k);					// 缩放倍数

BASEGRAPH_COMMON_API void __cdecl float4MulNum(float4* pDesV,		// 目标向量指针
								const float4* pSrcV,		// 源向量指针
								float k);					// 缩放倍数

///////////////////////////////////////////////////////////////////////////////
// 描述：	归一化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl float4Normalize(float4* pDesV,			// 目标向量指针
								   const float4* pSrcV);		// 源向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	取向量的模
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API float __cdecl float4Mode(const float4* pV);			// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量点乘
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API float __cdecl float4DotProduct(const float4* pV1,		// 向量指针
									 const float4* pV2);	// 向量指针

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量的距离
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API float __cdecl float4Distance(const float4* pV1,			// 向量指针
								   const float4* pV2);			// 向量指针

//////////////////////////////////////////////////////////////////////////
// 描述：	向量与矩阵的乘积
//
// 返回值：	无
//////////////////////////////////////////////////////////////////////////
BASEGRAPH_COMMON_API void __cdecl float4MulMatQ(float4* pDesV,			// 目标向量指针
								 const float4* pSrcV,			// 源向量指针
								 const matrix4f* pSrcM);			// 矩阵指针

BASEGRAPH_COMMON_API void __cdecl float4MatQMul(float4* pDesV,			// 目标向量指针
								 const matrix4f* pSrcM,			// 矩阵指针
								 const float4* pSrcV);			// 源向量指针


BASEGRAPH_COMMON_API void __cdecl float4Clamp(float4* pDev,float fMin,float fMax );
//////////////////////////////// 四维向量End //////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// C++ 代码

#ifdef __cplusplus

#define USE_VECTOR_C_FUNC			// 使用 C 函数标志

//////////////////////////////////////////////////////////////////////////
// 结构重载操作符

//////////////////////////////// 三维向量 //////////////////////////////////

inline double2& operator += (double2& v1, const double2& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	return v1;
}

inline float2 operator - (const float2& v1)
{
	float2 vre;

	vre.x = -v1.x;
	vre.y = -v1.y;

	return vre;
}

inline float2 operator - (const float2& v1, const float2& v2)
{
	float2 vre;
	vre.x = v1.x - v2.x;
	vre.y = v1.y - v2.y;
	return vre;
}


inline float2& operator -= (float2& v1, const float2& v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;

	return v1;
}

inline float2 operator + (const float2& v1, const float2& v2)
{
	float2 vre;
	vre.x = v1.x + v2.x;
	vre.y = v1.y + v2.y;
	return vre;
}

inline float2& operator += (float2& v1, const float2& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	return v1;
}

inline float2 operator * (const float2& v1, float f)
{
	float2 vre;

	vre.x = v1.x * f;
	vre.y = v1.y * f;
	return vre;
}

inline float2& operator *= (float2& v1, float f)
{
	v1.x *= f;
	v1.y *= f;
	return v1;
}

inline float2 operator / (const float2& v1, float f)
{
	float2 vre;

	float fTmp = 1.f / f;

	vre.x = v1.x * fTmp;
	vre.y = v1.y * fTmp;
	return vre;
}

inline float2& operator /= (float2& v1, float f)
{
	float fTmp = 1.f / f;
	v1.x *= fTmp;
	v1.y *= fTmp;
	return v1;
}

inline float operator * (const float2& v1, const float2& v2)
{
#ifdef USE_VECTOR_C_FUNC
	return float2DotProduct(&v1, &v2);
#else
	return (v1.x * v2.x + v1.y * v2.y);
#endif
}

inline float2 operator * (const float2& v, const matrix4f& m)
{
	float2 vre;
	float2MulMatQ(&vre, &v, &m);

	return vre;
}

inline float2& operator *= (float2& v, const matrix4f& m)
{
#ifdef _USE_SSE_OPTIMAL_
	float2MulMatQ(&v, &v, &m);
#else
	float2 vt;
	float2MulMatQ(&vt, &v, &m);
	v.x = vt.x;
	v.y = vt.y;
#endif

	return v;
}

inline bool operator == (const float2& v1,const float2& v2)
{
	return (v1.x == v2.x && v1.y == v2.y);
}
inline bool operator != (const float2& v1,const  float2& v2)
{
	return (v1.x != v2.x || v1.y != v2.y );
}


// 长整型

inline bool operator == (const long3& v1,const long3& v2)
{
	return (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z);
}

inline bool operator != (const long3& v1,const  long3& v2)
{
	return (v1.x != v2.x || v1.y != v2.y || v1.z != v2.z);
}

inline long3 operator - (const long3& v1)
{
	long3 vre;

	vre.x = -v1.x;
	vre.y = -v1.y;
	vre.z = -v1.z;

	return vre;
}

inline long3 operator - (const long3& v1,const  long3& v2)
{
	long3 vre;

	vre.x = v1.x - v2.x;
	vre.y = v1.y - v2.y;
	vre.z = v1.z - v2.z;

	return vre;
}

inline long3& operator -= (long3& v1, const long3& v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;

	return v1;
}

inline long3 operator + (const long3& v1,const  long3& v2)
{
	long3 vre;

	vre.x = v1.x + v2.x;
	vre.y = v1.y + v2.y;
	vre.z = v1.z + v2.z;

	return vre;
}

inline long3& operator += (long3& v1,const  long3& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;

	return v1;
}

inline long3 operator * (const long3& v1, long f)
{
	long3 vre;

	vre.x = v1.x * f;
	vre.y = v1.y * f;
	vre.z = v1.z * f;

	return vre;
}

inline long3& operator *= (long3& v1, long f)
{
	v1.x *= f;
	v1.y *= f;
	v1.z *= f;

	return v1;
}

inline long3 operator / (const long3& v1, long f)
{
	long3 vre;
	double fTmp = 1.0 / f;

	vre.x = (long)(v1.x * fTmp);
	vre.y = (long)(v1.y * fTmp);
	vre.z = (long)(v1.z * fTmp);

	return vre;
}

inline long3& operator /= (long3& v1, long f)
{
	double fTmp = 1.0 / f;

	v1.x = (long)(v1.x * fTmp);
	v1.y = (long)(v1.y * fTmp);
	v1.z = (long)(v1.z * fTmp);

	return v1;
}

inline long operator * (const long3& v1,const  long3& v2)
{
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

// 单精度型

inline bool operator == (const float3& v1, const float3& v2)
{
	return (fabs(v1.x - v2.x) < ABS_FLOAT_0 
		&& fabs(v1.y - v2.y) < ABS_FLOAT_0 
		&& fabs(v1.z - v2.z) < ABS_FLOAT_0 );
}

inline bool operator == (const float3& v1,const  float2& v2)
{
	return (fabs(v1.x - v2.x) < ABS_FLOAT_0 
		&& fabs(v1.y - v2.y) < ABS_FLOAT_0 );
}

inline bool operator == (const float2& v1,const  float3& v2)
{
	return (fabs(v1.x - v2.x) < ABS_FLOAT_0 
		&& fabs(v1.y - v2.y) < ABS_FLOAT_0 );
}
inline bool operator != (const float3& v1, const float3& v2)
{
	return (fabs(v1.x - v2.x) > ABS_FLOAT_0 
		|| fabs(v1.y - v2.y) > ABS_FLOAT_0 
		|| fabs(v1.z - v2.z) > ABS_FLOAT_0 );
}


inline float3 operator - (const float3& v1)
{
	float3 vre;

	vre.x = -v1.x;
	vre.y = -v1.y;
	vre.z = -v1.z;

	return vre;
}

inline float3 operator - (const float3& v1, const float3& v2)
{
	float3 vre;

#ifdef USE_VECTOR_C_FUNC
	float3Sub(&vre, &v1, &v2);
#else
	vre.x = v1.x - v2.x;
	vre.y = v1.y - v2.y;
	vre.z = v1.z - v2.z;
#endif

	return vre;
}

inline float3& operator -= (float3& v1, const float3& v2)
{
#ifdef USE_VECTOR_C_FUNC
	float3Sub(&v1, &v1, &v2);
#else
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
#endif

	return v1;
}

inline float3 operator + (const float3& v1, const float3& v2)
{
	float3 vre;

#ifdef USE_VECTOR_C_FUNC
	float3Add(&vre, &v1, &v2);
#else
	vre.x = v1.x + v2.x;
	vre.y = v1.y + v2.y;
	vre.z = v1.z + v2.z;
#endif

	return vre;
}

inline float3& operator += (float3& v1, const float3& v2)
{
#ifdef USE_VECTOR_C_FUNC
	float3Add(&v1, &v1, &v2);
#else
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
#endif

	return v1;
}

inline float3 operator * (const float3& v1, float f)
{
	float3 vre;

#ifdef USE_VECTOR_C_FUNC
	float3MulNum(&vre, &v1, f);
#else
	vre.x = v1.x * f;
	vre.y = v1.y * f;
	vre.z = v1.z * f;
#endif

	return vre;
}

inline float3 operator * (float f,const float3& v1 )
{
	float3 vre;

#ifdef USE_VECTOR_C_FUNC
	float3MulNum(&vre, &v1, f);
#else
	vre.x = v1.x * f;
	vre.y = v1.y * f;
	vre.z = v1.z * f;
#endif

	return vre;
}
inline float3& operator *= (float3& v1, float f)
{
#ifdef USE_VECTOR_C_FUNC
	float3Scale(&v1, f);
#else
	v1.x *= f;
	v1.y *= f;
	v1.z *= f;
#endif

	return v1;
}

inline float3& operator *= (float3& v1,const float3& v2)
{
	float3Mul(&v1,&v1,&v2);
	return v1;
}

inline float3 operator / (const float3& v1, float f)
{
	float3 vre;

#ifdef USE_VECTOR_C_FUNC
	float3MulNum(&vre, &v1, 1.f / f);
#else
	float fTmp = 1.f / f;

	vre.x = v1.x * fTmp;
	vre.y = v1.y * fTmp;
	vre.z = v1.z * fTmp;
#endif

	return vre;
}

inline float3 operator / (const float3& v1,const float3& v2)
{
	float3 ret;
	float3Div(&ret,&v1,&v2);
	return ret;
}

inline float3& operator /= (float3& v1,const float3& v2)
{
	float3Div(&v1,&v1,&v2);
	return v1;
}

inline float3& operator /= (float3& v1, float f)
{
#ifdef USE_VECTOR_C_FUNC
	float3Scale(&v1, 1.f / f);
#else
	float fTmp = 1.f / f;

	v1.x *= fTmp;
	v1.y *= fTmp;
	v1.z *= fTmp;
#endif

	return v1;
}

inline float operator * (const float3& v1, const float3& v2)
{
#ifdef USE_VECTOR_C_FUNC
	return float3DotProduct(&v1, &v2);
#else
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
#endif
}

inline float3 operator * (const float3& v, const matrix4f& m)
{
	float3 vre;

	float3MulMatQ(&vre, &v, &m);

	return vre;
}

inline float3& operator *= (float3& v, const matrix4f& m)
{
#ifdef _USE_SSE_OPTIMAL_
	float3MulMatQ(&v, &v, &m);
#else
	float3 vt;

	float3MulMatQ(&vt, &v, &m);
	v.x = vt.x;
	v.y = vt.y;
	v.z = vt.z;
#endif

	return v;
}

inline float3 operator * (const matrix4f& m, const float3& v)
{
	float3 vre;

	float3MatQMul(&vre, &m, &v);

	return vre;
}

// 双精度型

inline bool operator == (const double3& v1,const  double3& v2)
{
	return (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z);
}

inline bool operator != (const double3& v1,const  double3& v2)
{
	return (v1.x != v2.x || v1.y != v2.y || v1.z != v2.z);
}

inline double3 operator - (const double3& v1)
{
	double3 vre;
	vre.x = -v1.x;
	vre.y = -v1.y;
	vre.z = -v1.z;

	return vre;
}

inline double3 operator - (const double3& v1,const  double3& v2)
{
	double3 vre;

	vre.x = v1.x - v2.x;
	vre.y = v1.y - v2.y;
	vre.z = v1.z - v2.z;

	return vre;
}

inline double3& operator -= (double3& v1,const  double3& v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;

	return v1;
}

inline double3 operator + (const double3& v1, const double3& v2)
{
	double3 vre;

	vre.x = v1.x + v2.x;
	vre.y = v1.y + v2.y;
	vre.z = v1.z + v2.z;

	return vre;
}

inline double3& operator += (double3& v1,const  double3& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;

	return v1;
}

inline double3 operator * (const double3& v1, double f)
{
	double3 vre;

	vre.x = v1.x * f;
	vre.y = v1.y * f;
	vre.z = v1.z * f;

	return vre;
}

inline double3& operator *= (double3& v1, double f)
{
	v1.x *= f;
	v1.y *= f;
	v1.z *= f;

	return v1;
}

inline double3 operator / (const double3& v1, double f)
{
	double3 vre;
	double fTmp = 1.0 / f;
	
	vre.x = v1.x * fTmp;
	vre.y = v1.y * fTmp;
	vre.z = v1.z * fTmp;

	return vre;
}

inline double3& operator /= (double3& v1, double f)
{
	double fTmp = 1.0 / f;

	v1.x *= fTmp;
	v1.y *= fTmp;
	v1.z *= fTmp;

	return v1;
}

inline double operator * (const double3& v1,const  double3& v2)
{
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

//////////////////////////////// 三维向量End //////////////////////////////////

//////////////////////////////// 四维向量 /////////////////////////////////////

// 单精度型

inline bool operator == (const float4& v1,const  float4& v2)
{
	return float4Equal(&v1,&v2);
}


inline bool operator != (const float4& v1,const  float4& v2)
{
	return (v1.x != v2.x || v1.y != v2.y || v1.z != v2.z || v1.w != v2.w);
}

inline float4 operator - (const float4& v1)
{
	float4 vre;
	vre.x = -v1.x;
	vre.y = -v1.y;
	vre.z = -v1.z;
	vre.w = -v1.w;

	return vre;
}

inline float4 operator - (const float4& v1,const  float4& v2)
{
	float4 vre;

#ifdef USE_VECTOR_C_FUNC
	float4Sub(&vre, &v1, &v2);
#else
	vre.x = v1.x - v2.x;
	vre.y = v1.y - v2.y;
	vre.z = v1.z - v2.z;
	vre.w = v1.w - v2.w;
#endif

	return vre;
}

inline float4& operator -= (float4& v1,const  float4& v2)
{
#ifdef USE_VECTOR_C_FUNC
	float4Sub(&v1, &v1, &v2);
#else
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	v1.w -= v2.w;
#endif

	return v1;
}

inline float4 operator + (const float4& v1,const  float4& v2)
{
	float4 vre;

#ifdef USE_VECTOR_C_FUNC
	float4Add(&vre, &v1, &v2);
#else
	vre.x = v1.x + v2.x;
	vre.y = v1.y + v2.y;
	vre.z = v1.z + v2.z;
	vre.w = v1.w + v2.w;
#endif

	return vre;
}

inline float4& operator += (float4& v1,const  float4& v2)
{
#ifdef USE_VECTOR_C_FUNC
	float4Add(&v1, &v1, &v2);
#else
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	v1.w += v2.w;
#endif

	return v1;
}

inline float4 operator * (const float4& v1, float f)
{
	float4 vre;

#ifdef USE_VECTOR_C_FUNC
	float4MulNum(&vre, &v1, f);
#else
	vre.x = v1.x * f;
	vre.y = v1.y * f;
	vre.z = v1.z * f;
	vre.w = v1.w * f;
#endif

	return vre;
}

inline float4& operator *= (float4& v1, float f)
{
#ifdef USE_VECTOR_C_FUNC
	float4Scale(&v1, f);
#else
	v1.x *= f;
	v1.y *= f;
	v1.z *= f;
	v1.w *= f;
#endif

	return v1;
}

inline float4 operator / (const float4& v1, float f)
{
	float4 vre;

#ifdef USE_VECTOR_C_FUNC
	float4MulNum(&vre, &v1, 1.f / f);
#else
	float fTmp = 1.f / f;

	vre.x = v1.x * fTmp;
	vre.y = v1.y * fTmp;
	vre.z = v1.z * fTmp;
	vre.w = v1.w * fTmp;
#endif

	return vre;
}

inline float4& operator /= (float4& v1, float f)
{
#ifdef USE_VECTOR_C_FUNC
	float4Scale(&v1, 1.f / f);
#else
	float fTmp = 1.f / f;

	v1.x *= fTmp;
	v1.y *= fTmp;
	v1.z *= fTmp;
	v1.w *= fTmp;
#endif

	return v1;
}

inline float operator * (const float4& v1,const  float4& v2)
{
#ifdef USE_VECTOR_C_FUNC
	return float4DotProduct(&v1, &v2);
#else
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w);
#endif
}

inline float4 operator * (const float4& v,const  matrix4f& m)
{
	float4 vre;

	float4MulMatQ(&vre, &v, &m);

	return vre;
}

inline float4& operator *= (float4& v, const matrix4f& m)
{
#ifdef _USE_SSE_OPTIMAL_
	float4MulMatQ(&v, &v, &m);
#else
	float4 vt;

	float4MulMatQ(&vt, &v, &m);
	v.x = vt.x;
	v.y = vt.y;
	v.z = vt.z;
	v.w = vt.w;
#endif

	return v;
}

inline float4 operator * (const matrix4f& m, const float4& v)
{
	float4 vre;

	float4MatQMul(&vre, &m, &v);

	return vre;
}

//////////////////////////////// 四维向量End //////////////////////////////////

#endif

//------------------------------- End of File ---------------------------------
