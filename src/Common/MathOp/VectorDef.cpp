#include "stdafx.h"
#include "Common\VectorDef.h"
#include "Common\MatrixDef.h"
#include <assert.h>
#include "MathOpASM64.h"

//////////////////////////////// 二维向量 /////////////////////////////////////

// 整型实现

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl long2Set(long2* pV,			// 向量指针
						   long x,
						   long y)
{
	assert(pV != NULL);

	pV->x = x;
	pV->y = y;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	比较向量
//
// 返回值:	true		相等
//			false		不等
///////////////////////////////////////////////////////////////////////////////
bool __cdecl long2Equal(const long2* pV1,		// 向量指针
							 const long2* pV2)		// 向量指针
{
	assert((pV1 != NULL) && (pV2 != NULL));

	return (pV1->x == pV2->x && pV1->y == pV2->y);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	零化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl long2Zero(long2* pV)			// 向量指针
{
	assert(pV != NULL);

	pV->x = pV->y = 0;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	缩放向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl long2Scale(long2* pV,			// 向量指针
							 long k)					// 缩放倍数
{
	assert(pV != NULL);

	pV->x *= k;
	pV->y *= k;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	归一化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl long2Normalize(float2* pDesV,		// 目标向量指针
								 const long2* pSrcV)		// 源向量指针
{
	assert((pDesV != NULL) && (pSrcV != NULL));

	// 增加微量以防止奇异计算
	float k = 1.f / (long2Mode(pSrcV) + 0.0000011f);

	pDesV->x = pSrcV->x * k;
	pDesV->y = pSrcV->y * k;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	取向量的模
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
float __cdecl long2Mode(const long2* pV)			// 向量指针
{
	assert(pV != NULL);

	return float(sqrt(float(pV->x * pV->x + pV->y * pV->y)));
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量点乘
//
// 返回值:	long
///////////////////////////////////////////////////////////////////////////////
long __cdecl long2DotProduct(const long2* pV1,		// 向量指针
								  const long2* pV2)		// 向量指针
{
	assert((pV1 != NULL) && (pV2 != NULL));

	return (pV1->x * pV2->x + pV1->y * pV2->y);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量的距离
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
float __cdecl long2Distance(const long2* pV1,			// 向量指针
								 const long2* pV2)			// 向量指针
{
	assert((pV1 != NULL) && (pV2 != NULL));

	long dx = pV1->x - pV2->x;
	long dy = pV1->y - pV2->y;

	return float(sqrt(float(dx * dx + dy * dy)));
}


// 浮点型实现

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl float2Set(float2* pV,			// 向量指针
						   float x,
						   float y)
{
	assert(pV != NULL);

	pV->x = x;
	pV->y = y;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	比较向量
//
// 返回值:	true		相等
//			false		不等
///////////////////////////////////////////////////////////////////////////////
bool __cdecl float2Equal(const float2* pV1,		// 向量指针
							 const float2* pV2)		// 向量指针
{
	assert((pV1 != NULL) && (pV2 != NULL));

	return (fabs(pV1->x - pV2->x) < ABS_FLOAT_0 && fabs(pV1->y - pV2->y) < ABS_FLOAT_0 );
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	零化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl float2Zero(float2* pV)			// 向量指针
{
	assert(pV != NULL);

	pV->x = pV->y = 0.f;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	缩放向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl float2Scale(float2* pV,			// 向量指针
							 float k)					// 缩放倍数
{
	assert(pV != NULL);

	pV->x *= k;
	pV->y *= k;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	归一化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl float2Normalize(float2* pDesV,			// 目标向量指针
								 const float2* pSrcV)			// 源向量指针
{
	assert((pDesV != NULL) && (pSrcV != NULL));

	float fMod = float2Mode(pSrcV);
	if(fMod == 0)
		return;
	float k = 1.f / fMod;

	pDesV->x = pSrcV->x * k;
	pDesV->y = pSrcV->y * k;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	取向量的模
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
float __cdecl float2Mode(const float2* pV)			// 向量指针
{
	assert(pV != NULL);

	return float(sqrt(pV->x * pV->x + pV->y * pV->y));
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量点乘
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
float __cdecl float2DotProduct(const float2* pV1,		// 向量指针
								   const float2* pV2)		// 向量指针
{
	assert((pV1 != NULL) && (pV2 != NULL));

	return (pV1->x * pV2->x + pV1->y * pV2->y);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量的距离
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
float __cdecl float2Distance(const float2* pV1,			// 向量指针
								 const float2* pV2)			// 向量指针
{
	assert((pV1 != NULL) && (pV2 != NULL));

	float dx = pV1->x - pV2->x;
	float dy = pV1->y - pV2->y;
	
	return float(sqrt(dx * dx + dy * dy));
}

void __cdecl float2MulMatQ(float2* pDesV,			// 目标向量指针
						   const float2* pSrcV,			// 源向量指针
						   const matrix4f* pSrcM)			// 矩阵指针
{
	assert((pDesV != NULL) && (pSrcV != NULL) && (pSrcM != NULL));

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	float2MulMatQ_x64(pDesV,pSrcV,pSrcM);
#else
	__asm{
		mov			esi, pSrcV;
		mov			edi, pSrcV;
		mov			eax, pSrcM;
		mov			ebx, pSrcM;
		mov			ecx, pSrcM;
		mov			edx, pSrcM;
		add			edi, 0x08;
		add			ebx, 0x10;
		add			ecx, 0x20;
		add			edx, 0x30;
		movlps		xmm1, [esi];	// @ @ y x
		movups		xmm4, [eax];	// 第一行
		movups		xmm5, [ebx];	// 第二行
		movups		xmm6, [ecx];	// 第三行
		movups		xmm7, [edx];	// 第四行
		movdqa		xmm0, xmm1;
		shufps		xmm0, xmm0, 0x00;	// x
		shufps		xmm1, xmm1, 0x55;	// y
		shufps		xmm2, xmm2, 0x00;	// z
		mulps		xmm4, xmm0;
		mulps		xmm5, xmm1;
		addps		xmm4, xmm5;
		addps		xmm4, xmm7;		// * z y x
		mov			edx, pDesV;
		mov			edi, pDesV;
		add			edx, 0x08;
		movlps		[edi], xmm4;
	}
#endif

#else
	assert(pDesV != pSrcV);
	pDesV->x = pSrcV->x * pSrcM->_11 + pSrcV->y * pSrcM->_21 + 1.f * pSrcM->_41;
	pDesV->y = pSrcV->x * pSrcM->_12 + pSrcV->y * pSrcM->_22 + 1.f * pSrcM->_42;
#endif
}

void __cdecl float2Cross(float2 &ptresult,float2 pt1,float2 pt2)
{
	ptresult.x = 0;
	ptresult.y = pt1.x * pt2.y - pt1.y * pt2.x;
}

// 双精度型实现

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl double2Set(double2* pV,			// 向量指针
						   double x,
						   double y)
{
	assert(pV != NULL);

	pV->x = x;
	pV->y = y;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	比较向量
//
// 返回值:	true		相等
//			false		不等
///////////////////////////////////////////////////////////////////////////////
bool __cdecl double2Equal(const double2* pV1,		// 向量指针
							 const double2* pV2)		// 向量指针
{
	assert((pV1 != NULL) && (pV2 != NULL));

	return (pV1->x == pV2->x && pV1->y == pV2->y);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	零化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl double2Zero(double2* pV)			// 向量指针
{
	assert(pV != NULL);

	pV->x = pV->y = 0.0;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	缩放向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl double2Scale(double2* pV,			// 向量指针
							 double k)					// 缩放倍数
{
	assert(pV != NULL);

	pV->x *= k;
	pV->y *= k;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	归一化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl double2Normalize(double2* pDesV,		// 目标向量指针
								 const double2* pSrcV)		// 源向量指针
{
	assert((pDesV != NULL) && (pSrcV != NULL));

	// 增加微量以防止奇异计算
	double k = 1.0 / (double2Mode(pSrcV) + 0.000000000000011);

	pDesV->x = pSrcV->x * k;
	pDesV->y = pSrcV->y * k;
}

void __cdecl double2Cross(double2 &ptresult,double2 pt1,double2 pt2)
{
	ptresult.x = 0;
	ptresult.y = pt1.x * pt2.y - pt1.y * pt2.x;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	取向量的模
//
// 返回值:	double
///////////////////////////////////////////////////////////////////////////////
double __cdecl double2Mode(const double2* pV)			// 向量指针
{
	assert(pV != NULL);

	return sqrt(pV->x * pV->x + pV->y * pV->y);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量点乘
//
// 返回值:	double
///////////////////////////////////////////////////////////////////////////////
double __cdecl double2DotProduct(const double2* pV1,		// 向量指针
									const double2* pV2)		// 向量指针
{
	assert((pV1 != NULL) && (pV2 != NULL));

	return (pV1->x * pV2->x + pV1->y * pV2->y);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量的距离
//
// 返回值:	double
///////////////////////////////////////////////////////////////////////////////
double __cdecl double2Distance(const double2* pV1,			// 向量指针
								  const double2* pV2)			// 向量指针
{
	assert((pV1 != NULL) && (pV2 != NULL));

	double dx = pV1->x - pV2->x;
	double dy = pV1->y - pV2->y;

	return sqrt(dx * dx + dy * dy);
}

//////////////////////////////// 二维向量End //////////////////////////////////

//////////////////////////////// 三维向量 /////////////////////////////////////

// 整型实现

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl long3Set(long3* pV,			// 向量指针
							 long x,
							 long y,
							 long z)
{
	assert(pV != NULL);

	pV->x = x;
	pV->y = y;
	pV->z = z;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	比较向量
//
// 返回值:	true		相等
//			false		不等
///////////////////////////////////////////////////////////////////////////////
bool __cdecl long3Equal(const long3* pV1,		// 向量指针
							   const long3* pV2)		// 向量指针
{
	assert((pV1 != NULL) && (pV2 != NULL));

	return (pV1->x == pV2->x && pV1->y == pV2->y && pV1->z == pV2->z);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	零化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl long3Zero(long3* pV)		// 向量指针
{
	assert(pV != NULL);

	pV->x = pV->y = pV->z = 0;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	缩放向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl long3Scale(long3* pV,		// 向量指针
							   long k)					// 缩放倍数
{
	assert(pV != NULL);

	pV->x *= k;
	pV->y *= k;
	pV->z *= k;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	归一化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl long3Normalize(float3* pDesV,			// 目标向量指针
								   const long3* pSrcV)			// 源向量指针
{
	assert((pDesV != NULL) && (pSrcV != NULL));

	float lMod = long3Mode(pSrcV);
	if(lMod == 0)
	{
		pDesV->x = pDesV->y = pDesV->z = 0;
		return;
	}
	float k = 1.f / lMod;

	pDesV->x = (float)pSrcV->x * k;
	pDesV->y = (float)pSrcV->y * k;
	pDesV->z = (float)pSrcV->z * k;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	取向量的模
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
float __cdecl long3Mode(const long3* pV)			// 向量指针
{
	assert(pV != NULL);

	return float(sqrt(float(pV->x * pV->x + pV->y * pV->y + pV->z * pV->z)));
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量点乘
//
// 返回值:	long
///////////////////////////////////////////////////////////////////////////////
long __cdecl long3DotProduct(const long3* pV1,		// 向量指针
									const long3* pV2)		// 向量指针
{
	assert((pV1 != NULL) && (pV2 != NULL));

	return (pV1->x * pV2->x + pV1->y * pV2->y + pV1->z * pV2->z);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量叉乘
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl long3CrossProduct(long3* pDesV,		// 目标向量指针
									  const long3* pSrcV1,		// 源向量指针
									  const long3* pSrcV2)		// 源向量指针
{
	assert((pDesV != NULL) && (pSrcV1 != NULL) && (pSrcV2 != NULL));
	assert((pDesV != pSrcV1) && (pDesV != pSrcV2));

	pDesV->x = pSrcV1->y * pSrcV2->z - pSrcV1->z * pSrcV2->y;
	pDesV->y = pSrcV1->z * pSrcV2->x - pSrcV1->x * pSrcV2->z;
	pDesV->z = pSrcV1->x * pSrcV2->y - pSrcV1->y * pSrcV2->x;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量的距离
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
float __cdecl long3Distance(const long3* pV1,			// 向量指针
								   const long3* pV2)			// 向量指针
{
	assert((pV1 != NULL) && (pV2 != NULL));

	long a = pV1->x - pV2->x;
	long b = pV1->y - pV2->y;
	long c = pV1->z - pV2->z;

	return float(sqrt(float(a * a + b * b + c * c)));
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩阵与向量的乘积
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl long3MulMat(long3* pDesV,			// 目标向量指针
								const long3* pSrcV,			// 源向量指针
								const matrix4f* pSrcM)				// 矩阵指针
{
	assert((pDesV != NULL) && (pSrcV != NULL) && (pSrcM != NULL));
	assert(pDesV != pSrcV);

	memset(pDesV, 0, sizeof(long3));
	//pDesV->x = pSrcM->element[0]*pSrcV->x+pSrcM->element[1]*pSrcV->y+pSrcM->element[2]*pSrcV->z+pSrcM->element[3];
	//pDesV->y = pSrcM->element[4]*pSrcV->x+pSrcM->element[5]*pSrcV->y+pSrcM->element[6]*pSrcV->z+pSrcM->element[7];
	//pDesV->z = pSrcM->element[8]*pSrcV->x+pSrcM->element[9]*pSrcV->y+pSrcM->element[10]*pSrcV->z+pSrcM->element[11];
	pDesV->x = (long)(pSrcM->m[0][0]*pSrcV->x+pSrcM->m[0][1]*pSrcV->y+pSrcM->m[0][2]*pSrcV->z+pSrcM->m[0][3]);
	pDesV->y = (long)(pSrcM->m[1][0]*pSrcV->x+pSrcM->m[1][1]*pSrcV->y+pSrcM->m[1][2]*pSrcV->z+pSrcM->m[1][3]);
	pDesV->z = (long)(pSrcM->m[2][0]*pSrcV->x+pSrcM->m[2][1]*pSrcV->y+pSrcM->m[2][2]*pSrcV->z+pSrcM->m[2][3]);
}


// 浮点型实现
///////////////////////////////////////////////////////////////////////////////
// 描述：	获取三维向量的某一维值
// 返回值:	浮点值
///////////////////////////////////////////////////////////////////////////////
float __cdecl float3GetElement(const float3 &v1, const unsigned int nDim)
{
	float fRet = 0;
	switch(nDim)
	{
	case 0:
		fRet = v1.x;
		break;
	case 1:
		fRet = v1.y;
		break;
	case 2:
		fRet = v1.z;
		break;
	}

	return fRet;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl float3Set(float3* pV,			// 向量指针
							 float x,
							 float y,
							 float z)
{
	assert(pV != NULL);

	pV->x = x;
	pV->y = y;
	pV->z = z;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	比较向量
//
// 返回值:	true		相等
//			false		不等
///////////////////////////////////////////////////////////////////////////////
#include <emmintrin.h>
bool __cdecl float3Equal(const float3* pV1,		// 向量指针
							   const float3* pV2)		// 向量指针
{
	assert((pV1 != NULL) && (pV2 != NULL));
	return (pV1->x == pV2->x && pV1->y == pV2->y && pV1->z == pV2->z);
}

bool __cdecl float3Equal(const float3* pV1,		// 向量指针
	const float3* pV2, float fPre)		// 向量指针
{
	assert((pV1 != NULL) && (pV2 != NULL));
	return (fabs(pV1->x - pV2->x) < fPre && fabs(pV1->y - pV2->y) < fPre && fabs(pV1->z - pV2->z) < fPre);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	零化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl float3Zero(float3* pV)			// 向量指针
{
	assert(pV != NULL);

	pV->x = pV->y = pV->z = 0.f;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	负向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl float3Negtive(float3* pV)			// 向量指针
{
	assert(pV != NULL);

	pV->x = -pV->x;
	pV->y = -pV->y;
	pV->z = -pV->z;
}

//////////////////////////////////////////////////////////////////////////
// 描述：	加减法
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl float3Add(float3* pDesV,		// 目标向量
							 const float3* pSrcV1,		// 向量 1
							 const float3* pSrcV2)		// 向量 2
{
	assert((pDesV != NULL) && (pSrcV1 != NULL) && (pSrcV2 != NULL));

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	float3Add_x64(pDesV,pSrcV1,pSrcV2);
#else
	__asm{
		mov			edi, pDesV;
		mov			edx, pDesV;
		mov			esi, pSrcV1;
		mov			eax, pSrcV1;
		mov			ebx, pSrcV2;
		mov			ecx, pSrcV2;
		add			eax, 0x08;
		add			ecx, 0x08;
		add			edx, 0x08;
		movlps		xmm1, [esi];		// @ @ y1 x1
		movlps		xmm3, [ebx];		// @ @ y2 x2
		movss		xmm2, [eax];		// 0 0 0 z1
		movss		xmm4, [ecx];		// 0 0 0 z2
		movlhps		xmm1, xmm2;			// 0 z1 y1 x1
		movlhps		xmm3, xmm4;			// 0 z2 y2 x2
		addps		xmm1, xmm3;			// 0 z y x
		movhlps		xmm2, xmm1;			// 0 0 0 z
		movlps		[edi], xmm1;
		movss		[edx], xmm2;
	}
#endif

#else
	pDesV->x = pSrcV1->x + pSrcV2->x;
	pDesV->y = pSrcV1->y + pSrcV2->y;
	pDesV->z = pSrcV1->z + pSrcV2->z;
#endif
}

void __cdecl float3Sub(float3* pDesV,		// 目标向量
							 const float3* pSrcVL,		// 左向量
							 const float3* pSrcVR)		// 右向量
{
	assert((pDesV != NULL) && (pSrcVL != NULL) && (pSrcVR != NULL));

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	float3Sub_x64(pDesV,pSrcVL,pSrcVR);
#else
	__asm{
		mov			edi, pDesV;
		mov			edx, pDesV;
		mov			esi, pSrcVL;
		mov			eax, pSrcVL;
		mov			ebx, pSrcVR;
		mov			ecx, pSrcVR;
		add			eax, 0x08;
		add			ecx, 0x08;
		add			edx, 0x08;
		movlps		xmm1, [esi];		// @ @ yL xL
		movlps		xmm3, [ebx];		// @ @ yR xR
		movss		xmm2, [eax];		// 0 0 0 zL
		movss		xmm4, [ecx];		// 0 0 0 zR
		movlhps		xmm1, xmm2;			// 0 zL yL xL
		movlhps		xmm3, xmm4;			// 0 zR yR xR
		subps		xmm1, xmm3;			// 0 z y z
		movhlps		xmm2, xmm1;			// 0 0 0 z
		movlps		[edi], xmm1;
		movss		[edx], xmm2;
	}
#endif

#else
	pDesV->x = pSrcVL->x - pSrcVR->x;
	pDesV->y = pSrcVL->y - pSrcVR->y;
	pDesV->z = pSrcVL->z - pSrcVR->z;
#endif
}

void __cdecl float3AddNum(float3* pDesV,	// 目标向量
								const float3* pSrcV,	// 向量
								float k)				// 数
{
	assert((pDesV != NULL) && (pSrcV != NULL));

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	float3AddNum_x64(pDesV,pSrcV,k);
#else
	__asm{
		mov			esi, pSrcV;
		mov			eax, pSrcV;
		mov			edi, pDesV;
		mov			edx, pDesV;
		add			eax, 0x08;
		add			edx, 0x08;
		movlps		xmm1, [esi];		// @ @ y x
		movss		xmm2, [eax];		// 0 0 0 z
		movss		xmm7, k;			// 0 0 0 k
		movlhps		xmm1, xmm2;			// 0 z y x
		shufps		xmm7, xmm7, 0x00;	// k k k k
		addps		xmm1, xmm7;			// @ k+z k+y k+x
		movhlps		xmm2, xmm1;			// 0 0 @ k+z
		movlps		[edi], xmm1;
		movss		[edx], xmm2;
	}
#endif

#else
	pDesV->x = pSrcV->x + k;
	pDesV->y = pSrcV->y + k;
	pDesV->z = pSrcV->z + k;
#endif
}

void __cdecl float3Offset(float3* pV,	// 向量
								float k)			// 数
{
	assert(pV != NULL);

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	float3Offset_x64(pV,k);
#else
	__asm{
		mov			eax, pV;
		mov			edx, pV;
		add			eax, 0x08;
		movlps		xmm1, [edx];		// @ @ y x
		movss		xmm2, [eax];		// 0 0 0 z
		movss		xmm7, k;			// 0 0 0 k
		movlhps		xmm1, xmm2;			// 0 z y x
		shufps		xmm7, xmm7, 0x00;	// k k k k
		addps		xmm1, xmm7;			// @ k+z k+y k+x
		movhlps		xmm2, xmm1;			// 0 0 @ k+z
		movlps		[edx], xmm1;
		movss		[eax], xmm2;
	}
#endif

#else
	pV->x += k;
	pV->y += k;
	pV->z += k;
#endif
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	缩放向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl float3Scale(float3* pV,			// 向量指针
							   float k)						// 缩放倍数
{
	assert(pV != NULL);

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	float3Scale_x64(pV,k);
#else
	__asm{
		mov			eax, pV;
		mov			edx, pV;
		add			eax, 0x08;
		movlps		xmm1, [edx];		// @ @ y x
		movss		xmm2, [eax];		// 0 0 0 z
		movss		xmm7, k;			// 0 0 0 k
		movlhps		xmm1, xmm2;			// 0 z y x
		shufps		xmm7, xmm7, 0x00;	// k k k k
		mulps		xmm1, xmm7;			// 0 k*z k*y k*x
		movhlps		xmm2, xmm1;			// 0 0 0 k*z
		movlps		[edx], xmm1;
		movss		[eax], xmm2;
	}
#endif

#else
	pV->x *= k;
	pV->y *= k;
	pV->z *= k;
#endif
}

void __cdecl float3MulNum(float3* pDesV,		// 目标向量指针
								const float3* pSrcV,		// 源向量指针
								float k)					// 缩放倍数
{
	assert((pDesV != NULL) && (pSrcV != NULL));

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	float3MulNum_x64(pDesV,pSrcV,k);
#else
	__asm{
		mov			esi, pSrcV;
		mov			eax, pSrcV;
		mov			edi, pDesV;
		mov			edx, pDesV;
		add			eax, 0x08;
		add			edx, 0x08;
		movlps		xmm1, [esi];		// @ @ y x
		movss		xmm2, [eax];		// 0 0 0 z
		movss		xmm7, k;			// 0 0 0 k
		movlhps		xmm1, xmm2;			// 0 z y x
		shufps		xmm7, xmm7, 0x00;	// k k k k
		mulps		xmm1, xmm7;			// 0 k*z k*y k*x
		movhlps		xmm2, xmm1;			// 0 0 0 k*z
		movlps		[edi], xmm1;
		movss		[edx], xmm2;
	}
#endif

#else
	pDesV->x = pSrcV->x * k;
	pDesV->y = pSrcV->y * k;
	pDesV->z = pSrcV->z * k;
#endif
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	归一化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
//static __declspec(align(16)) float gc_detal[4] = { 0.0000011f, 0.0000011f, 0.0000011f, 0.0000011f};
void __cdecl float3Normalize(float3* pDesV,			// 目标向量指针
								   const float3* pSrcV)			// 源向量指针
{
	assert((pDesV != NULL) && (pSrcV != NULL));
	//如果源为全0，则目标也为全0
	if (pSrcV->x > -ABS_DOUBLE_0 && pSrcV->x < ABS_DOUBLE_0
		&& pSrcV->y > -ABS_DOUBLE_0 && pSrcV->y < ABS_DOUBLE_0
		&& pSrcV->z > -ABS_DOUBLE_0 && pSrcV->z < ABS_DOUBLE_0)
	{
		memset(pDesV, 0, sizeof(float3));
		return;
	}

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	float3Normalize_x64(pDesV,pSrcV);
#else
	__asm{
		mov			esi, pSrcV;
		mov			eax, pSrcV;
		mov			edi, pDesV;
		mov			edx, pDesV;
		add			eax, 0x08;
		add			edx, 0x08;
//		movdqa		xmm7,gc_detal;
		movlps		xmm0, [esi];		// @ @ y x
		movss		xmm1, [eax];		// 0 0 0 z
		movlhps		xmm0, xmm1;			// 0 z y x
		movdqa		xmm2, xmm0;
		mulps		xmm0, xmm0;			// 0 z^2 y^2 x^2
		movhlps		xmm1, xmm0;			// 0 0 0 z^2
		addps		xmm1, xmm0;			// 0 z^2 y^2 x^2+z^2
		shufps		xmm0, xmm0, 0x01;	// * * * y^2
		addps		xmm0, xmm1;			// * * * x^2+y^2+z^2
//		addps		xmm0, xmm7;			//添加微量，防止奇异计算
		//rsqrtss		xmm0, xmm0;			// 求模的倒数 a
		sqrtss		xmm0, xmm0;			// 求模的倒数 a
		shufps		xmm0, xmm0, 0x00;	// a a a a
		divps 		xmm2, xmm0;			// 0 z*a y*a x*a
		movhlps		xmm1, xmm2;			// 0 0 0 z*a
		movlps		[edi], xmm2;
		movss		[edx], xmm1;
	}
#endif

#else
	// 增加微量以防止奇异计算
	float fMod = float3Mode(pSrcV);
	if(fMod == 0)
	{
		pDesV->x = pDesV->y = pDesV->z = 0;
		return;
	}
	float k = 1.f / (float3Mode(pSrcV));

	pDesV->x = pSrcV->x * k;
	pDesV->y = pSrcV->y * k;
	pDesV->z = pSrcV->z * k;
#endif
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	取向量的模
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
float __cdecl float3Mode(const float3* pV)			// 向量指针
{
	assert(pV != NULL);

//float版本：【注】这个版本使用了rsqrtss和rcpss来代替sqrtss，提高了速度但是也比较大的损失了精度，大约只有千分之一的精度
	//float fRes(0.f);

//#ifdef _USE_SSE_OPTIMAL_
//	_asm{
//		mov			eax, pV;
//		mov			esi, pV;
//		add			eax, 0x08;
//		movlps		xmm1, [esi];		// @ @ y x
//		movss		xmm2, [eax];		// 0 0 0 z
//		movlhps		xmm1, xmm2;			// 0 z y x
//		mulps		xmm1, xmm1;			// 0 z^2 y^2 x^2
//		movhlps		xmm2, xmm1;			// 0 0 0 z^2
//		pshufd		xmm3, xmm1, 0x01;	// * * * y^2
//		addps		xmm1, xmm2;			// * * * x^2+z^2
//		addps		xmm1, xmm3;			// * * * x^2+y^2+z^2
//		rsqrtss		xmm1, xmm1;
//		rcpss		xmm1, xmm1;			// 平方根（会比直接使用 sqrtss 快 5 倍，但会出现奇异值）
//		movss		fRes, xmm1;
//	}
//#else
//	fRes = sqrtf(pV->x * pV->x + pV->y * pV->y + pV->z * pV->z);
//#endif
	//return fRes;

//double版本
#ifdef _USE_SSE_OPTIMAL_
	float fRes = 0.f;

#ifdef WIN64
	return float3Mode_x64(pV);
#else
	_asm{
		mov			eax, pV;
		movups		xmm1, [eax];		// xmm1 [?, z, y, x]
		movhlps		xmm2, xmm1;			// xmm2 [?, ?, ?, z]
		cvtps2pd	xmm3, xmm1;			// xmm3 [yd, xd]
		cvtps2pd	xmm4, xmm2;			// xmm4 [??, zd]
		mulpd		xmm3, xmm3;			// xmm3 [yd*yd, xd*xd]
		mulpd		xmm4, xmm4;			// xmm4 [??, zd*zd]
		addpd		xmm4, xmm3;			// xmm4 [??, zd*zd + xd*xd]
		movhlps		xmm3, xmm3;			// xmm3 [??, yd*yd]
		addsd		xmm3, xmm4;			// xmm3 [??, zd*zd + xd*xd + yd*yd]
		sqrtsd		xmm3, xmm3;			// xmm3 [??, sqrt(zd*zd + xd*xd + yd*yd)]
		cvtsd2ss	xmm3, xmm3;			// xmm3 [?, ?, ?, (float)sqrt(zd*zd + xd*xd + yd*yd)]
		movss		fRes, xmm3;
	}
#endif

	return fRes;
#else
	return (float)sqrt(pV->x * (double)pV->x + pV->y * (double)pV->y + pV->z * (double)pV->z);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量点乘
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
float __cdecl float3DotProduct(const float3* pV1,		// 向量指针
									 const float3* pV2)		// 向量指针
{
	assert((pV1 != NULL) && (pV2 != NULL));

	float fRes = 0.f;

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	return float3DotProduct_x64(pV1,pV2);		// 向量指针
#else
	__asm{
		mov			esi, pV1;
		mov			eax, pV1;
		mov			edi, pV2;
		mov			edx, pV2;
		add			eax, 0x08;
		add			edx, 0x08;
		movlps		xmm0, [esi];		// @ @ y1 x1
		movlps		xmm2, [edi];		// @ @ y2 x2
		movss		xmm1, [eax];		// 0 0 0 z1
		movss		xmm3, [edx];		// 0 0 0 z2
		movlhps		xmm0, xmm1;			// 0 z1 y1 x1
		movlhps		xmm2, xmm3;			// 0 z2 y2 x2
		mulps		xmm0, xmm2;			// 0 z1*z2 y1*y2 x1*x2
		movhlps		xmm1, xmm0;			// 0 0 0 z1*z2
		addps		xmm1, xmm0;			// * * * x1*x2+z1*z2
		shufps		xmm0, xmm0, 0x01;	// * * * y1*y2
		addps		xmm0, xmm1;			// * * * x1*x2+y1*y2+z1*z2
		movss		fRes, xmm0;
	}
#endif

#else
	fRes = pV1->x * pV2->x + pV1->y * pV2->y + pV1->z * pV2->z;
#endif
	return fRes;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量叉乘
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl float3CrossProduct(float3* pDesV,		// 目标向量指针
									  const float3* pSrcV1,		// 源向量指针
									  const float3* pSrcV2)		// 源向量指针
{
	assert((pDesV != NULL) && (pSrcV1 != NULL) && (pSrcV2 != NULL));

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	float3CrossProduct_x64(pDesV,		// 目标向量指针
		pSrcV1,		// 源向量指针
		pSrcV2);		// 源向量指针
#else
	__asm{
		mov			esi, pSrcV1;
		mov			eax, pSrcV1;
		mov			ebx, pSrcV2;
		mov			ecx, pSrcV2;
		mov			edi, pDesV;
		mov			edx, pDesV;
		add			eax, 0x08;
		add			ecx, 0x08;
		add			edx, 0x08;
		movlps		xmm0, [esi];		// @ @ y1 x1
		movlps		xmm2, [ebx];		// @ @ y2 x2
		movss		xmm1, [eax];		// 0 0 0 z1
		movss		xmm3, [ecx];		// 0 0 0 z2
		movlhps		xmm0, xmm1;		// 0 z1 y1 x1
		movlhps		xmm2, xmm3;		// 0 z2 y2 x2
		pshufd		xmm1, xmm0, 0x12;	// x1 y1 x1 z1
		shufps		xmm0, xmm0, 0x09;	// x1 x1 z1 y1
		mulps		xmm1, xmm2;		// 0 y1*z2 x1*y2 z1*x2
		mulps		xmm0, xmm2;		// 0 x1*z2 z1*y2 y1*x2
		shufps		xmm1, xmm1, 0x12;	// z1*x2 x1*y2 z1*x2 y1*z2
		shufps		xmm0, xmm0, 0x09;	// y1*x2 y1*x2 x1*z2 z1*y2
		subps		xmm1, xmm0;		// @ x1*y2-y1*x2 z1*x2-x1*z2 y1*z2-z1*y2
		movhlps		xmm0, xmm1;
		movlps		[edi], xmm1;
		movss		[edx], xmm0;
	}
#endif

#else
	assert((pDesV != pSrcV1) && (pDesV != pSrcV2));

	pDesV->x = pSrcV1->y * pSrcV2->z - pSrcV1->z * pSrcV2->y;
	pDesV->y = pSrcV1->z * pSrcV2->x - pSrcV1->x * pSrcV2->z;
	pDesV->z = pSrcV1->x * pSrcV2->y - pSrcV1->y * pSrcV2->x;
#endif
}


double __cdecl float3Angle(const float3* pSrcV1,	// 源向量指针
								const float3* pSrcV2)	// 源向量指针
{
	double ddot = float3DotProduct(pSrcV1,pSrcV2);
	double dis1 = float3Mode(pSrcV1);
	double dis2 = float3Mode(pSrcV2);

	return acos(ddot/(dis1 * dis2));
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量的距离
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
float __cdecl float3Distance(const float3* pV1,			// 向量指针
								   const float3* pV2)			// 向量指针
{
	assert((pV1 != NULL) && (pV2 != NULL));

	float3 v = {pV1->x - pV2->x, pV1->y - pV2->y, pV1->z - pV2->z};
	return float3Mode(&v);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩阵与向量的乘积
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl float3NormalMulMat(float3* pDesV,			// 目标向量指针
								const float3* pSrcV,			// 源向量指针
								const matrix4f* pSrcM)				// 矩阵指针
{
	assert((pDesV != NULL) && (pSrcV != NULL) && (pSrcM != NULL));

#ifdef _USE_SSE_OPTIMAL_
	float* pElement = (float*)pSrcM->m;

#ifdef WIN64
	float3NormalMulMat_x64(pDesV,			// 目标向量指针
		pSrcV,			// 源向量指针
		pSrcM);
#else
	__asm{
		mov			esi, pSrcV;
		mov			edi, pSrcV;
		mov			eax, pElement;
		mov			ebx, pElement;
		mov			ecx, pElement;
		mov			edx, pElement;
		add			edi, 0x08;
		add			ebx, 0x10;
		add			ecx, 0x20;
		add			edx, 0x30;
		movlps		xmm1, [esi];	// @ @ y x
		movss		xmm2, [edi];	// 0 0 0 z
		movups		xmm4, [eax];	// 第一行
		movups		xmm5, [ebx];	// 第二行
		movups		xmm6, [ecx];	// 第三行
		movups		xmm7, [edx];	// 第四行
		pshufd		xmm0, xmm1, 0x00;	// x
		shufps		xmm1, xmm1, 0x55;	// y
		shufps		xmm2, xmm2, 0x00;	// z
		mulps		xmm4, xmm0;
		mulps		xmm5, xmm1;
		mulps		xmm6, xmm2;
		addps		xmm4, xmm5;
		addps		xmm4, xmm6;		// * z y x
		movhlps		xmm1, xmm4;		// * * * z
		mov			edx, pDesV;
		mov			edi, pDesV;
		add			edx, 0x08;
		movlps		[edi], xmm4;
		movss		[edx], xmm1;
	}
#endif

#else
	//assert(pDesV != pSrcV);

	//pDesV->x = pSrcM->element[0]*pSrcV->x+pSrcM->element[1]*pSrcV->y+pSrcM->element[2]*pSrcV->z+pSrcM->element[3];
	//pDesV->y = pSrcM->element[4]*pSrcV->x+pSrcM->element[5]*pSrcV->y+pSrcM->element[6]*pSrcV->z+pSrcM->element[7];
	//pDesV->z = pSrcM->element[8]*pSrcV->x+pSrcM->element[9]*pSrcV->y+pSrcM->element[10]*pSrcV->z+pSrcM->element[11];
	float3 temp;
	temp.x = pSrcM->m[0][0]*pSrcV->x+pSrcM->m[1][0]*pSrcV->y+pSrcM->m[2][0]*pSrcV->z;
	temp.y = pSrcM->m[0][1]*pSrcV->x+pSrcM->m[1][1]*pSrcV->y+pSrcM->m[2][1]*pSrcV->z;
	temp.z = pSrcM->m[0][2]*pSrcV->x+pSrcM->m[1][2]*pSrcV->y+pSrcM->m[2][2]*pSrcV->z;
	*pDesV = temp;
#endif
}

void __cdecl float3MatMulNormal(float3* pDesV,			// 目标向量指针
								const matrix4f* pSrcM,				// 矩阵指针
								const float3* pSrcV)			// 源向量指针
{
	assert((pDesV != NULL) && (pSrcV != NULL) && (pSrcM != NULL));

//#ifdef _USE_SSE_OPTIMAL_
//	float* pElement = pSrcM->element;
//
//	__asm{
//		mov			esi, pSrcV;
//		mov			edi, pSrcV;
//		mov			eax, pElement;
//		mov			ebx, pElement;
//		mov			ecx, pElement;
//		mov			edx, pElement;
//		add			edi, 0x08;
//		add			ebx, 0x10;
//		add			ecx, 0x20;
//		add			edx, 0x30;
//		movlps		xmm1, [esi];	// @ @ y x
//		movss		xmm2, [edi];	// 0 0 0 z
//		movups		xmm4, [eax];	// 第一行
//		movups		xmm5, [ebx];	// 第二行
//		movups		xmm6, [ecx];	// 第三行
//		movups		xmm7, [edx];	// 第四行
//		pshufd		xmm0, xmm1, 0x00;	// x
//		shufps		xmm1, xmm1, 0x55;	// y
//		shufps		xmm2, xmm2, 0x00;	// z
//		mulps		xmm4, xmm0;
//		mulps		xmm5, xmm1;
//		mulps		xmm6, xmm2;
//		addps		xmm4, xmm5;
//		addps		xmm6, xmm7;
//		addps		xmm4, xmm6;		// * z y x
//		movhlps		xmm1, xmm4;		// * * * z
//		mov			edx, pDesV;
//		mov			edi, pDesV;
//		add			edx, 0x08;
//		movlps		[edi], xmm4;
//		movss		[edx], xmm1;
//	}
//#else
	assert(pDesV != pSrcV);

	pDesV->x = pSrcM->m[0][0]*pSrcV->x+pSrcM->m[0][1]*pSrcV->y+pSrcM->m[0][2]*pSrcV->z+pSrcM->m[0][3];
	pDesV->y = pSrcM->m[1][0]*pSrcV->x+pSrcM->m[1][1]*pSrcV->y+pSrcM->m[1][2]*pSrcV->z+pSrcM->m[1][3];
	pDesV->z = pSrcM->m[2][0]*pSrcV->x+pSrcM->m[2][1]*pSrcV->y+pSrcM->m[2][2]*pSrcV->z+pSrcM->m[2][3];
//#endif
}

//////////////////////////////////////////////////////////////////////////
// 描述：	四维向量与矩阵的乘积（补最后一维为单位值）
//			本系统采用行向量与矩阵的乘法定义
//
// 返回值：	无
//////////////////////////////////////////////////////////////////////////
void __cdecl float3MulMatQ(float3* pDesV,			// 目标向量指针
								 const float3* pSrcV,			// 源向量指针
								 const matrix4f* pSrcM)			// 矩阵指针
{
	assert((pDesV != NULL) && (pSrcV != NULL) && (pSrcM != NULL));

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	float3MulMatQ_x64(pDesV,			// 目标向量指针
		pSrcV,			// 源向量指针
		pSrcM);			// 矩阵指针
#else
	__asm{
		mov			esi, pSrcV;
		mov			edi, pSrcV;
		mov			eax, pSrcM;
		mov			ebx, pSrcM;
		mov			ecx, pSrcM;
		mov			edx, pSrcM;
		add			edi, 0x08;
		add			ebx, 0x10;
		add			ecx, 0x20;
		add			edx, 0x30;
		movlps		xmm1, [esi];	// @ @ y x
		movss		xmm2, [edi];	// 0 0 0 z
		movups		xmm4, [eax];	// 第一行
		movups		xmm5, [ebx];	// 第二行
		movups		xmm6, [ecx];	// 第三行
		movups		xmm7, [edx];	// 第四行
		movdqa		xmm0, xmm1;
		shufps		xmm0, xmm0, 0x00;	// x
		shufps		xmm1, xmm1, 0x55;	// y
		shufps		xmm2, xmm2, 0x00;	// z
		mulps		xmm4, xmm0;
		mulps		xmm5, xmm1;
		mulps		xmm6, xmm2;
		addps		xmm4, xmm5;
		addps		xmm6, xmm7;
		addps		xmm4, xmm6;		// * z y x
		movhlps		xmm1, xmm4;		// * * * z
		mov			edx, pDesV;
		mov			edi, pDesV;
		add			edx, 0x08;
		movlps		[edi], xmm4;
		movss		[edx], xmm1;
	}
#endif

#else
	assert(pDesV != pSrcV);

	pDesV->x = pSrcV->x * pSrcM->_11 + pSrcV->y * pSrcM->_21 + pSrcV->z * pSrcM->_31 + 1.f * pSrcM->_41;
	pDesV->y = pSrcV->x * pSrcM->_12 + pSrcV->y * pSrcM->_22 + pSrcV->z * pSrcM->_32 + 1.f * pSrcM->_42;
	pDesV->z = pSrcV->x * pSrcM->_13 + pSrcV->y * pSrcM->_23 + pSrcV->z * pSrcM->_33 + 1.f * pSrcM->_43;
#endif
}

void __cdecl float3MatQMul(float3* pDesV,			// 目标向量指针
								 const matrix4f* pSrcM,			// 矩阵指针
								 const float3* pSrcV)			// 源向量指针
{
	assert((pDesV != NULL) && (pSrcV != NULL) && (pSrcM != NULL));

//#ifdef _USE_SSE_OPTIMAL_
//	__asm{
//		mov			esi, pSrcV;
//		mov			edi, pSrcV;
//		mov			eax, pSrcM;
//		mov			ecx, pSrcM;
//		mov			edx, pSrcM;
//		add			edi, 0x08;
//		add			ecx, 0x10;
//		add			edx, 0x20;
//		movlps		xmm0, [esi];		// @ @ y x
//		movss		xmm1, [edi];		// 0 0 0 z
//		movups		xmm4, [eax];		// 第一行
//		movups		xmm5, [ecx];		// 第二行
//		movups		xmm6, [edx];		// 第三行
//		movlhps		xmm0, xmm1;			// 0 z y x
//		mulps		xmm4, xmm0;
//		mulps		xmm5, xmm0;
//		mulps		xmm6, xmm0;
//		movhlps		xmm1, xmm4;
//		movhlps		xmm2, xmm5;
//		movhlps		xmm3, xmm6;
//		addps		xmm1, xmm4;			// z + x
//		addps		xmm2, xmm5;
//		addps		xmm3, xmm6;
//		shufps		xmm4, xmm4, 0x01;	// y
//		shufps		xmm5, xmm5, 0x01;
//		shufps		xmm6, xmm6, 0x01;
//		addps		xmm1, xmm4;
//		addps		xmm2, xmm5;
//		addps		xmm3, xmm6;
//		mov			edi, pDesV;
//		mov			eax, pDesV;
//		mov			edx, pDesV;
//		add			eax, 0x04;
//		add			edx, 0x08;
//		movss		[edi], xmm1;
//		movss		[eax], xmm2;
//		movss		[edx], xmm3;
//	}
//#else
	assert(pDesV != pSrcV);

	pDesV->x = pSrcV->x * pSrcM->_11 + pSrcV->y * pSrcM->_12 + pSrcV->z * pSrcM->_13 + pSrcM->_14;
	pDesV->y = pSrcV->x * pSrcM->_21 + pSrcV->y * pSrcM->_22 + pSrcV->z * pSrcM->_23 + pSrcM->_24;
	pDesV->z = pSrcV->x * pSrcM->_31 + pSrcV->y * pSrcM->_32 + pSrcV->z * pSrcM->_33 + pSrcM->_34;
//#endif
}

//////////////////////////////////////////////////////////////////////////
// 描述：	向量到初等变换矩阵的转换
//			由于本系统采用行向量与矩阵的乘法定义，因此偏移量将在矩阵最后一行
//
// 返回值：	无
//////////////////////////////////////////////////////////////////////////
void __cdecl float3ToScaleMatQ(matrix4f* pDesM,		// 目标矩阵指针
									 const float3* pSrcV)		// 源向量指针
{
	assert((pDesM != NULL) && (pSrcV != NULL));

	memset(pDesM, 0, sizeof(matrix4f));
	pDesM->_11 = pSrcV->x;
	pDesM->_22 = pSrcV->y;
	pDesM->_33 = pSrcV->z;
	pDesM->_44 = 1.f;
}

void __cdecl float3ToTransMatQ(matrix4f* pDesM,		// 目标矩阵指针
									 const float3* pSrcV)		// 源向量指针
{
	assert((pDesM != NULL) && (pSrcV != NULL));

	memset(pDesM, 0, sizeof(matrix4f));
	pDesM->_11 = pDesM->_22 = pDesM->_33 = pDesM->_44 = 1.f;
	pDesM->_41 = pSrcV->x;
	pDesM->_42 = pSrcV->y;
	pDesM->_43 = pSrcV->z;
}

void __cdecl float3ToRotateZMatQ(matrix4f* pDesM,		// 目标矩阵指针
									 const float3* pSrcV)		// 源向量指针
{
	assert((pDesM != NULL) && (pSrcV != NULL));

	memset(pDesM, 0, sizeof(matrix4f));
	float dr = 1.f / sqrtf( pSrcV->x * pSrcV->x + pSrcV->y * pSrcV->y );
	pDesM->_33 = pDesM->_44 = 1.f;
	pDesM->_11 = pDesM->_22 = dr * pSrcV->y;//dr * pSrcV->x;//
	pDesM->_12 = -dr * pSrcV->x;//-dr * pSrcV->y;//
	pDesM->_21 = -pDesM->_12;
}
void __cdecl float2Fromfloat3(float2* des,float3* src)
{
	des->x = src->x;
	des->y = src->y;
}

void __cdecl float3fFromfloat2fEx(float3* pV,		// 目标向量
								  float2* pt,
								  float fvalue,
								  int nPlaneType)
{
	if( nPlaneType == Plane_XZ)
	{
		pV->x = pt->x;
		pV->y = fvalue;
		pV->z = pt->y;
	}
	else if( nPlaneType == Plane_YZ)
	{
		pV->x = fvalue;
		pV->y = pt->y;
		pV->z = pt->x;
	}
	else
	{
		pV->x = pt->x;
		pV->y = pt->y;
		pV->z = fvalue;
	}
};

void __cdecl float3fFromfloat2f(float3* pV,		// 目标向量
								float2* pt,
								int nPlaneType)		// 面
{
	if( nPlaneType == Plane_XZ)
	{
		pV->x = pt->x;
		pV->y = 0.0f;
		pV->z = pt->y;
	}
	else if( nPlaneType == Plane_YZ)
	{
		pV->x = 0.0f;
		pV->y = pt->y;
		pV->z = pt->x;
	}
	else
	{
		pV->x = pt->x;
		pV->y = pt->y;
		pV->z = 0.0f;
	}
};

// 双精度型实现

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl double3Set(double3* pV,			// 向量指针
							 double x,
							 double y,
							 double z)
{
	assert(pV != NULL);

	pV->x = x;
	pV->y = y;
	pV->z = z;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl double3SetSingle(double3* pV,			// 向量指针
							  double dvalue)
{
	assert(pV != NULL);

	pV->x = dvalue;
	pV->y = dvalue;
	pV->z = dvalue;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	比较向量
//
// 返回值:	true		相等
//			false		不等
///////////////////////////////////////////////////////////////////////////////
bool __cdecl double3Equal(const double3* pV1,		// 向量指针
							   const double3* pV2)		// 向量指针
{
	assert((pV1 != NULL) && (pV2 != NULL));

	return (pV1->x == pV2->x && pV1->y == pV2->y && pV1->z == pV2->z);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	零化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl double3Zero(double3* pV)			// 向量指针
{
	assert(pV != NULL);

	pV->x = pV->y = pV->z = 0.0;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	缩放向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl double3Scale(double3* pV,			// 向量指针
							   double k)					// 缩放倍数
{
	assert(pV != NULL);

	pV->x *= k;
	pV->y *= k;
	pV->z *= k;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	各项异性缩放向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl double3ScaleV(double3* pDesV,double3* pScaleV)
{
	pDesV->x = pDesV->x * pScaleV->x;
	pDesV->y = pDesV->y * pScaleV->y;
	pDesV->z = pDesV->z * pScaleV->z;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	归一化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl double3Normalize(double3* pDesV,			// 目标向量指针
								   const double3* pSrcV)			// 源向量指针
{
	assert((pDesV != NULL) && (pSrcV != NULL));

	double k = 1.0 / (double3Mode(pSrcV) + 0.000000000000011);

	pDesV->x = pSrcV->x * k;
	pDesV->y = pSrcV->y * k;
	pDesV->z = pSrcV->z * k;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	取向量的模
//
// 返回值:	double
///////////////////////////////////////////////////////////////////////////////
double __cdecl double3Mode(const double3* pV)			// 向量指针
{
	assert(pV != NULL);

	return sqrt(pV->x * pV->x + pV->y * pV->y + pV->z * pV->z);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量点乘
//
// 返回值:	double
///////////////////////////////////////////////////////////////////////////////
double __cdecl double3DotProduct(const double3* pV1,		// 向量指针
									  const double3* pV2)		// 向量指针
{
	assert((pV1 != NULL) && (pV2 != NULL));

	return (pV1->x * pV2->x + pV1->y * pV2->y + pV1->z * pV2->z);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量叉乘
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl double3CrossProduct(double3* pDesV,		// 目标向量指针
									  const double3* pSrcV1,		// 源向量指针
									  const double3* pSrcV2)		// 源向量指针
{
	assert((pDesV != NULL) && (pSrcV1 != NULL) && (pSrcV2 != NULL));
	assert((pDesV != pSrcV1) && (pDesV != pSrcV2));

	pDesV->x = pSrcV1->y * pSrcV2->z - pSrcV1->z * pSrcV2->y;
	pDesV->y = pSrcV1->z * pSrcV2->x - pSrcV1->x * pSrcV2->z;
	pDesV->z = pSrcV1->x * pSrcV2->y - pSrcV1->y * pSrcV2->x;
}


double __cdecl double3Angle(const double3* pSrcV1,	// 源向量指针
								const double3* pSrcV2)	// 源向量指针
{
	double ddot = double3DotProduct(pSrcV1,pSrcV2);
	double dis1 = double3Mode(pSrcV1);
	double dis2 = double3Mode(pSrcV2);

	return acos(ddot/(dis1 * dis2));
}

double __cdecl double3AngleBetVec( const double3* pSrcV1, const double3* pSrcV2, const double3* pUp)
{
	double3 crossPuct;
	double3CrossProduct(&crossPuct,pSrcV1,pSrcV2);
	double forward;
	forward = double3DotProduct(pUp,&crossPuct) > 0 ? 1.0 :-1.0;
	double x,y;
	x = double3DotProduct(pSrcV1,pSrcV2);
	y = forward * double3Mode(&crossPuct);

	return atan2(y,x);	
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量的距离
//
// 返回值:	double
///////////////////////////////////////////////////////////////////////////////
double __cdecl double3Distance(const double3* pV1,			// 向量指针
									const double3* pV2)			// 向量指针
{
	assert((pV1 != NULL) && (pV2 != NULL));

	double a = pV1->x - pV2->x;
	double b = pV1->y - pV2->y;
	double c = pV1->z - pV2->z;

	return sqrt(a * a + b * b + c * c);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩阵与向量的乘积
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl double3MulMat(double3* pDesV,			// 目标向量指针
								const double3* pSrcV,			// 源向量指针
								const matrix4f* pSrcM)				// 矩阵指针
{
	assert((pDesV != NULL) && (pSrcV != NULL) && (pSrcM != NULL));
	assert(pDesV != pSrcV);

	memset(pDesV, 0, sizeof(matrix4f));
	//pDesV->x = pSrcM->element[0]*pSrcV->x+pSrcM->element[1]*pSrcV->y+pSrcM->element[2]*pSrcV->z+pSrcM->element[3];
	//pDesV->y = pSrcM->element[4]*pSrcV->x+pSrcM->element[5]*pSrcV->y+pSrcM->element[6]*pSrcV->z+pSrcM->element[7];
	//pDesV->z = pSrcM->element[8]*pSrcV->x+pSrcM->element[9]*pSrcV->y+pSrcM->element[10]*pSrcV->z+pSrcM->element[11];
	pDesV->x = pSrcM->m[0][0]*pSrcV->x+pSrcM->m[0][1]*pSrcV->y+pSrcM->m[0][2]*pSrcV->z+pSrcM->m[0][3];
	pDesV->y = pSrcM->m[1][0]*pSrcV->x+pSrcM->m[1][1]*pSrcV->y+pSrcM->m[1][2]*pSrcV->z+pSrcM->m[1][3];
	pDesV->z = pSrcM->m[2][0]*pSrcV->x+pSrcM->m[2][1]*pSrcV->y+pSrcM->m[2][2]*pSrcV->z+pSrcM->m[2][3];
}

//////////////////////////////// 三维向量End //////////////////////////////////


//////////////////////////////// 四维向量 /////////////////////////////////////

// 浮点型实现

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl float4Set(float4* pV,			// 向量指针
							 float x,
							 float y,
							 float z,
							 float w)
{
	assert(pV != NULL);

	pV->x = x;
	pV->y = y;
	pV->z = z;
	pV->w = w;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	比较向量
//
// 返回值:	true		相等
//			false		不等
///////////////////////////////////////////////////////////////////////////////
bool __cdecl float4Equal(const float4* pV1,		// 向量指针
							   const float4* pV2)		// 向量指针
{
	assert((pV1 != NULL) && (pV2 != NULL));
	__m128 m1 = _mm_loadu_ps((const float*)pV1);
	__m128 m2 = _mm_loadu_ps((const float*)pV2);

	__m128 eq = _mm_cmpeq_ps(m1,m2);
	return _mm_movemask_ps(eq) == 0xf;
	//return (pV1->x == pV2->x && pV1->y == pV2->y && pV1->z == pV2->z && pV1->w == pV2->w);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	零化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl float4Zero(float4* pV)			// 向量指针
{
	assert(pV != NULL);

	pV->x = pV->y = pV->z = pV->w = 0.f;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	负向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl float4Negtive(float4* pV)			// 向量指针
{
	assert(pV != NULL);

	pV->x = -pV->x;
	pV->y = -pV->y;
	pV->z = -pV->z;
	pV->w = -pV->w;
}

//////////////////////////////////////////////////////////////////////////
// 描述：	加减法
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl float4Add(float4* pDesV,		// 目标向量
							 const float4* pSrcV1,		// 向量 1
							 const float4* pSrcV2)		// 向量 2
{
	assert((pDesV != NULL) && (pSrcV1 != NULL) && (pSrcV2 != NULL));

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	float4Add_x64(pDesV,		// 目标向量
		pSrcV1,		// 向量 1
		pSrcV2);		// 向量 2
#else
	__asm{
		mov			edi, pDesV;
		mov			esi, pSrcV1;
		mov			eax, pSrcV2;
		movups		xmm1, [esi];		// w1 z1 y1 x1
		movups		xmm2, [eax];		// w2 z2 y2 x2
		addps		xmm1, xmm2;			// w z y x
		movups		[edi], xmm1;
	}
#endif

#else
	pDesV->x = pSrcV1->x + pSrcV2->x;
	pDesV->y = pSrcV1->y + pSrcV2->y;
	pDesV->z = pSrcV1->z + pSrcV2->z;
	pDesV->z = pSrcV1->w + pSrcV2->w;
#endif
}

void __cdecl float4Sub(float4* pDesV,		// 目标向量
							 const float4* pSrcVL,		// 左向量
							 const float4* pSrcVR)		// 右向量
{
	assert((pDesV != NULL) && (pSrcVL != NULL) && (pSrcVR != NULL));

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	float4Sub_x64(pDesV,		// 目标向量
		pSrcVL,		// 左向量
		pSrcVR);		// 右向量
#else
	__asm{
		mov			edi, pDesV;
		mov			esi, pSrcVL;
		mov			eax, pSrcVR;
		movups		xmm1, [esi];		// wL zL yL xL
		movups		xmm2, [eax];		// wR zR yR xR
		subps		xmm1, xmm2;			// w z y x
		movups		[edi], xmm1;
	}
#endif

#else
	pDesV->x = pSrcVL->x - pSrcVR->x;
	pDesV->y = pSrcVL->y - pSrcVR->y;
	pDesV->z = pSrcVL->z - pSrcVR->z;
	pDesV->z = pSrcVL->w - pSrcVR->w;
#endif
}

void __cdecl float4AddNum(float4* pDesV,	// 目标向量
								const float4* pSrcV,	// 向量
								float k)				// 数
{
	assert((pDesV != NULL) && (pSrcV != NULL));

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	float4AddNum_x64(pDesV,	// 目标向量
		pSrcV,	// 向量
		k);
#else
	__asm{
		mov			esi, pSrcV;
		mov			edi, pDesV;
		movss		xmm7, k;			// 0 0 0 k
		movups		xmm1, [esi];		// w z y x
		shufps		xmm7, xmm7, 0x00;	// k k k k
		addps		xmm1, xmm7;			// w+k k+z k+y k+x
		movups		[edi], xmm1;
	}
#endif

#else
	pDesV->x = pSrcV->x + k;
	pDesV->y = pSrcV->y + k;
	pDesV->z = pSrcV->z + k;
	pDesV->z = pSrcV->w + k;
#endif
}

void __cdecl float4Offset(float4* pV,	// 向量
								float k)			// 数
{
	assert(pV != NULL);

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
		float4Offset_x64(pV,k);
#else
	__asm{
		mov			eax, pV;
		movss		xmm7, k;			// 0 0 0 k
		movups		xmm1, [eax];		// w z y x
		shufps		xmm7, xmm7, 0x00;	// k k k k
		addps		xmm1, xmm7;			// w+k k+z k+y k+x
		movups		[eax], xmm1;
	}
#endif

#else
	pV->x += k;
	pV->y += k;
	pV->z += k;
	pV->w += k;
#endif
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	缩放向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl float4Scale(float4* pV,			// 向量指针
							   float k)						// 缩放倍数
{
	assert(pV != NULL);

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	float4Scale_x64(pV,k);						// 缩放倍数
#else
	__asm{
		mov			eax, pV;
		movss		xmm7, k;			// 0 0 0 k
		movups		xmm1, [eax];		// w z y x
		shufps		xmm7, xmm7, 0x00;	// k k k k
		mulps		xmm1, xmm7;			// w*k k*z k*y k*x
		movups		[eax], xmm1;
	}
#endif

#else
	pV->x *= k;
	pV->y *= k;
	pV->z *= k;
	pV->w *= k;
#endif
}

void __cdecl float4MulNum(float4* pDesV,		// 目标向量指针
								const float4* pSrcV,		// 源向量指针
								float k)					// 缩放倍数
{
	assert((pDesV != NULL) && (pSrcV != NULL));

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	float4MulNum_x64(pDesV,		// 目标向量指针
		pSrcV,		// 源向量指针
		k);					// 缩放倍数
#else
	__asm{
		mov			esi, pSrcV;
		mov			edi, pDesV;
		movss		xmm7, k;			// 0 0 0 k
		movups		xmm1, [esi];		// w z y x
		shufps		xmm7, xmm7, 0x00;	// k k k k
		mulps		xmm1, xmm7;			// w*k k*z k*y k*x
		movups		[edi], xmm1;
	}
#endif

#else
	pDesV->x = pSrcV->x * k;
	pDesV->y = pSrcV->y * k;
	pDesV->z = pSrcV->z * k;
	pDesV->w = pSrcV->w * k;
#endif
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	归一化向量
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl float4Normalize(float4* pDesV,			// 目标向量指针
								   const float4* pSrcV)			// 源向量指针
{
	assert((pDesV != NULL) && (pSrcV != NULL));

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	float4Normalize_x64(pDesV,pSrcV);			// 源向量指针
#else
	__asm{
		mov			esi, pSrcV;
		mov			edi, pDesV;
		movups		xmm0, [esi];		// w z y x
		movdqa		xmm7, xmm0;
		mulps		xmm0, xmm0;			// w^2 z^2 y^2 x^2
		movhlps		xmm1, xmm0;			// 0 0 w^2 z^2
		addps		xmm0, xmm1;
		pshufd		xmm2, xmm0, 0x01;	// 求模
		addps		xmm0, xmm2;
		rsqrtss		xmm0, xmm0;
		shufps		xmm0, xmm0, 0x00;	// a a a a
		mulps		xmm0, xmm7;			// w*a z*a y*a x*a
		movups		[edi], xmm0;
	}
#endif

#else
	// 增加微量以防止奇异计算
	float k = 1.f / (float4Mode(pSrcV) + 0.0000011f);

	pDesV->x = pSrcV->x * k;
	pDesV->y = pSrcV->y * k;
	pDesV->z = pSrcV->z * k;
	pDesV->w = pSrcV->w * k;
#endif
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	取向量的模
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
float __cdecl float4Mode(const float4* pV)			// 向量指针
{
	assert(pV != NULL);

	float fRes(0.f);

//float版本：【注】这个版本使用了rsqrtss和rcpss来代替sqrtss，提高了速度但是也比较大的损失了精度，大约只有千分之一的精度
//#ifdef _USE_SSE_OPTIMAL_
//	_asm{
//		mov			esi, pV;
//		movups		xmm0, [esi];		// w z y x
//		mulps		xmm0, xmm0;			// w^2 z^2 y^2 x^2
//		movhlps		xmm1, xmm0;			// 0 0 w^2 z^2
//		addps		xmm0, xmm1;
//		pshufd		xmm2, xmm0, 0x01;	// 求模
//		addps		xmm0, xmm2;
//		rsqrtss		xmm0, xmm0;
//		rcpss		xmm0, xmm0;			// 平方根（会比直接使用 sqrtss 快 5 倍，但会出现奇异值）
//		movss		fRes, xmm0;
//	}
//#else
//	fRes = sqrtf(pV->x * pV->x + pV->y * pV->y + pV->z * pV->z + pV->w * pV->w);
//#endif

//double版本
#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	return float4Mode_x64(pV);			// 向量指针
#else
	_asm{
		mov			eax, pV;
		movups		xmm1, [eax];		// xmm1 [w, z, y, x]
		movhlps		xmm2, xmm1;			// xmm2 [?, ?, w, z]
		cvtps2pd	xmm3, xmm1;			// xmm3 [yd, xd]
		cvtps2pd	xmm4, xmm2;			// xmm4 [wd, zd]
		mulpd		xmm3, xmm3;			// xmm3 [yd*yd, xd*xd]
		mulpd		xmm4, xmm4;			// xmm4 [wd*wd, zd*zd]
		addpd		xmm4, xmm3;			// xmm4 [wd*wd+yd*yd, zd*zd + xd*xd]
		movhlps		xmm5, xmm4;			// xmm5 [??, wd*wd+yd*yd]
		addsd		xmm5, xmm4;			// xmm5 [??, wd*wd + zd*zd + xd*xd + yd*yd]
		sqrtsd		xmm5, xmm5;			// xmm5 [??, sqrt(wd*wd + zd*zd + xd*xd + yd*yd)]
		cvtsd2ss	xmm5, xmm5;			// xmm3 [?, ?, ?, (float)sqrt(wd*wd + zd*zd + xd*xd + yd*yd)]
		movss		fRes, xmm5;
	}
#endif

#else
	fRes = (float)sqrt(pV->x * (double)pV->x + (double)pV->y * pV->y + (double)pV->z * pV->z + (double)pV->w * pV->w);
#endif

	return fRes;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量点乘
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
float __cdecl float4DotProduct(const float4* pV1,		// 向量指针
									 const float4* pV2)		// 向量指针
{
	assert((pV1 != NULL) && (pV2 != NULL));

	float fRes = 0.f;

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	return float4DotProduct_x64(pV1,		// 向量指针
		pV2);		// 向量指针
#else
	__asm{
		mov			esi, pV1;
		mov			edi, pV2;
		movups		xmm0, [esi];
		movups		xmm1, [edi];
		mulps		xmm0, xmm1;
		movhlps		xmm2, xmm0;
		addps		xmm0, xmm2;
		pshufd		xmm3, xmm0, 0x01;
		addps		xmm0, xmm3;
		movss		fRes, xmm0;
	}
#endif

#else
	fRes = pV1->x * pV2->x + pV1->y * pV2->y + pV1->z * pV2->z + pV1->w * pV2->w;
#endif

	return fRes;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	向量的距离
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
float __cdecl float4Distance(const float4* pV1,			// 向量指针
								   const float4* pV2)			// 向量指针
{
	assert((pV1 != NULL) && (pV2 != NULL));
	float4 v = {pV1->x - pV2->x, pV1->y - pV2->y, pV1->z - pV2->z, pV1->w - pV2->w};
	return float4Mode(&v);
}

//////////////////////////////////////////////////////////////////////////
// 描述：	向量与矩阵的乘积，本系统采用行向量与矩阵的乘法定义
//
// 返回值：	无
//////////////////////////////////////////////////////////////////////////
void __cdecl float4MulMatQ(float4* pDesV,			// 目标向量指针
								 const float4* pSrcV,			// 源向量指针
								 const matrix4f* pSrcM)			// 矩阵指针
{
	assert((pDesV != NULL) && (pSrcV != NULL) && (pSrcM != NULL));

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	float4MulMatQ_x64(pDesV,			// 目标向量指针
		pSrcV,			// 源向量指针
		pSrcM);			// 矩阵指针
	//pDesV->x = pSrcV->x * pSrcM->_11 + pSrcV->y * pSrcM->_21 + pSrcV->z * pSrcM->_31 + pSrcV->w * pSrcM->_41;
	//pDesV->y = pSrcV->x * pSrcM->_12 + pSrcV->y * pSrcM->_22 + pSrcV->z * pSrcM->_32 + pSrcV->w * pSrcM->_42;
	//pDesV->z = pSrcV->x * pSrcM->_13 + pSrcV->y * pSrcM->_23 + pSrcV->z * pSrcM->_33 + pSrcV->w * pSrcM->_43;
	//pDesV->w = pSrcV->x * pSrcM->_14 + pSrcV->y * pSrcM->_24 + pSrcV->z * pSrcM->_34 + pSrcV->w * pSrcM->_44;

#else
	__asm{
		mov			esi, pSrcV;
		mov			edi, pDesV;
		mov			eax, pSrcM;
		mov			ebx, pSrcM;
		mov			ecx, pSrcM;
		mov			edx, pSrcM;
		add			ebx, 0x10;
		add			ecx, 0x20;
		add			edx, 0x30;
		movups		xmm0, [esi];		// w z y x
		movups		xmm4, [eax];		// 第一行
		movups		xmm5, [ebx];		// 第二行
		movups		xmm6, [ecx];		// 第三行
		movups		xmm7, [edx];		// 第四行
		movdqa		xmm1, xmm0;
		movdqa		xmm2, xmm0;
		movdqa		xmm3, xmm0;
		shufps		xmm0, xmm0, 0x00;	// x x x x
		shufps		xmm1, xmm1, 0x55;	// y y y y
		shufps		xmm2, xmm2, 0xaa;	// z z z z
		shufps		xmm3, xmm3, 0xff;	// w w w w
		mulps		xmm4, xmm0;			// x * m1
		mulps		xmm5, xmm1;			// y * m2
		mulps		xmm6, xmm2;			// z * m3
		mulps		xmm7, xmm3;			// w * m4
		addps		xmm4, xmm5;
		addps		xmm6, xmm7;
		addps		xmm4, xmm6;
		movups		[edi], xmm4;
	}
#endif

#else
	assert(pDesV != pSrcV);

	pDesV->x = pSrcV->x * pSrcM->_11 + pSrcV->y * pSrcM->_21 + pSrcV->z * pSrcM->_31 + pSrcV->w * pSrcM->_41;
	pDesV->y = pSrcV->x * pSrcM->_12 + pSrcV->y * pSrcM->_22 + pSrcV->z * pSrcM->_32 + pSrcV->w * pSrcM->_42;
	pDesV->z = pSrcV->x * pSrcM->_13 + pSrcV->y * pSrcM->_23 + pSrcV->z * pSrcM->_33 + pSrcV->w * pSrcM->_43;
	pDesV->w = pSrcV->x * pSrcM->_14 + pSrcV->y * pSrcM->_24 + pSrcV->z * pSrcM->_34 + pSrcV->w * pSrcM->_44;
#endif
}

void __cdecl float4MatQMul(float4* pDesV,			// 目标向量指针
								 const matrix4f* pSrcM,			// 矩阵指针
								 const float4* pSrcV)			// 源向量指针
{
	assert((pDesV != NULL) && (pSrcV != NULL) && (pSrcM != NULL));

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	float4MatQMul_x64(pDesV,			// 目标向量指针
		pSrcM,			// 矩阵指针
		pSrcV);			// 源向量指针;
#else
	__asm{
		mov			esi, pSrcV;
		mov			edi, pDesV;
		mov			eax, pSrcM;
		mov			ebx, pSrcM;
		mov			ecx, pSrcM;
		mov			edx, pSrcM;
		add			ebx, 0x10;
		add			ecx, 0x20;
		add			edx, 0x30;
		movups		xmm0, [esi];		// w z y x
		movups		xmm4, [eax];		// 第一行
		movups		xmm5, [ebx];		// 第二行
		movups		xmm6, [ecx];		// 第三行
		movups		xmm7, [edx];		// 第四行
		mulps		xmm4, xmm0;
		mulps		xmm5, xmm0;
		mulps		xmm6, xmm0;
		mulps		xmm7, xmm0;
		movhlps		xmm0, xmm4;
		movhlps		xmm1, xmm5;
		movhlps		xmm2, xmm6;
		movhlps		xmm3, xmm7;
		addps		xmm0, xmm4;			// w + y  z + x
		addps		xmm1, xmm5;
		addps		xmm2, xmm6;
		addps		xmm3, xmm7;
		movlhps		xmm0, xmm1;			// yR  xR
		movlhps		xmm2, xmm3;			// wR  zR
		movdqa		xmm4, xmm0;
		movdqa		xmm6, xmm2;
		shufps		xmm4, xmm4, 0xb1;
		shufps		xmm6, xmm6, 0xb1;
		addps		xmm0, xmm4;
		addps		xmm2, xmm6;
		shufps		xmm0, xmm2, 0x88;
		movups		[edi], xmm4;
	}
#endif

#else
	assert(pDesV != pSrcV);

	pDesV->x = pSrcV->x * pSrcM->_11 + pSrcV->y * pSrcM->_12 + pSrcV->z * pSrcM->_13 + pSrcV->w * pSrcM->_14;
	pDesV->y = pSrcV->x * pSrcM->_21 + pSrcV->y * pSrcM->_22 + pSrcV->z * pSrcM->_23 + pSrcV->w * pSrcM->_24;
	pDesV->z = pSrcV->x * pSrcM->_31 + pSrcV->y * pSrcM->_32 + pSrcV->z * pSrcM->_33 + pSrcV->w * pSrcM->_34;
	pDesV->w = pSrcV->x * pSrcM->_41 + pSrcV->y * pSrcM->_42 + pSrcV->z * pSrcM->_43 + pSrcV->w * pSrcM->_44;
#endif
}

double __cdecl float3AngleBetVec(const float3* pSrcV1,
								 const float3* pSrcV2,
								 const float3* pUp)
{
	float3 crossPuct;
	float3CrossProduct(&crossPuct,pSrcV1,pSrcV2);
	float forward;
	//float3CrossProduct(&forward,pSrcV1,pUp);
	forward = float3DotProduct(pUp,&crossPuct) > 0 ? 1.f :-1.f;
	float x,y;
	x = float3DotProduct(pSrcV1,pSrcV2);
	y = forward * float3Mode(&crossPuct);

	return atan2(y,x);	
}

#include <emmintrin.h>

inline void LoadFloat3(__m128& v1,const float3* pV)
{
	v1 = _mm_loadl_pi(v1,(__m64*)pV);
	__m128 v = _mm_set_ss(pV->z);
	v1 = _mm_movelh_ps(v1,v);
}

inline void StoreFloat3(__m128& data, float3* pDstV)
{
	_mm_storel_pi((__m64*)pDstV,data);
	__m128 v = _mm_setzero_ps();
	v = _mm_movehl_ps(v,data);
	_mm_store_ss(&pDstV->z,v);
}

void __cdecl float3GetMin( float3* pV,const float3* pV1,const float3* pV2 )
{
	pV->x = min(pV1->x,pV2->x);
	pV->y = min(pV1->y,pV2->y);
	pV->z = min(pV1->z,pV2->z);
}

void __cdecl float3GetMax( float3* pV,const float3* pV1,const float3* pV2 )
{
	pV->x = max(pV1->x,pV2->x);
	pV->y = max(pV1->y,pV2->y);
	pV->z = max(pV1->z,pV2->z);
}

void __cdecl float3GetMinMax(float3* pMin,float3* pMax,const float3* pV1,const float3* pV2)
{
	float3GetMin(pMin,pV1,pV2);
	float3GetMax(pMax,pV1,pV2);
}

void __cdecl float3Mul( float3* pDesV,const float3* pSrcV1,const float3* pSrcV2 )
{
	__m128 v1,v2;
	LoadFloat3(v1,pSrcV1);
	LoadFloat3(v2,pSrcV2);
	v1 = _mm_mul_ps(v1,v2);
	StoreFloat3(v1,pDesV);
}

void __cdecl float3Div( float3* pDesV,const float3* pSrcV1,const float3* pSrcV2 )
{
	__m128 v1,v2;
	LoadFloat3(v1,pSrcV1);
	LoadFloat3(v2,pSrcV2);
	v1 = _mm_div_ps(v1,v2);
	StoreFloat3(v1,pDesV);

}

void __cdecl float4Clamp( float4* pDev,float fMin ,float fMax  )
{
	__m128 v1 = _mm_loadu_ps(&pDev->x);
	__m128 vMin = _mm_set1_ps(fMin);
	__m128 vMax = _mm_set1_ps(fMax);
	v1 = _mm_min_ps(v1,vMax);
	v1 = _mm_max_ps(v1,vMin);
	_mm_storeu_ps(&pDev->x,v1);
}

//////////////////////////////// 四维向量End //////////////////////////////////

//------------------------------- End of File ---------------------------------
