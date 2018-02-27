#include "stdafx.h"
#include "Common\QuatnDef.h"
#include "Common\MatrixDef.h"
#include "Common\VectorDef.h"
#include <assert.h>
#include "MathOpASM64.h"

#define RADIAN_HALF_PI 1.5707963267948966192313216916398f 
///////////////////////////////////////////////////////////////////////////////
// 描述：	设置
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYQuatn_Set(SYQuatn* pQ,			// 四元数指针
						 float r,				// 纯量
						 float x,				// i
						 float y,				// j
						 float z)				// k
{
	assert(pQ != NULL);

	pQ->r = r;
	pQ->x = x;
	pQ->y = y;
	pQ->z = z;
}

//////////////////////////////////////////////////////////////////////////
// Description:		identity quaternion
//
// Return Value:	None
//////////////////////////////////////////////////////////////////////////
void __cdecl SYQuatn_Identity(SYQuatn* pQ)		// destination pointer
{
	assert(pQ != NULL);

	pQ->r = 1.f;
	pQ->x = pQ->y = pQ->z = 0.f;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	比较
//
// 返回值:	TRUE		相等
//			FALSE		不相等
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl SYQuatn_Equal(const SYQuatn* pQ1,		// 四元数指针
						   const SYQuatn* pQ2)		// 四元数指针
{
	assert((pQ1 != NULL) && (pQ2 != NULL));

	return ((pQ1->r == pQ2->r) && (pQ1->x == pQ2->x) && (pQ1->y == pQ2->y) && (pQ1->z == pQ2->z));
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	负四元数
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYQuatn_Negtive(SYQuatn* pQ)		// 四元数指针
{
	assert(pQ != NULL);

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	_quatnNegtive_x64(pQ);
#else
	_asm{
		mov			esi, pQ;
		xorps		xmm0, xmm0;
		movups		xmm1, [esi];
		subps		xmm0, xmm1;
		movups		[esi], xmm0;
	}
#endif

#else
	pQ->r = -pQ->r;
	pQ->x = -pQ->x;
	pQ->y = -pQ->y;
	pQ->z = -pQ->z;
#endif
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	加减法
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYQuatn_Add(SYQuatn* pDesQ,			// 目标四元数指针
						 const SYQuatn* pSrcQL,			// 左源四元数指针
						 const SYQuatn* pSrcQR)			// 右源四元数指针
{
	assert((pDesQ != NULL) && (pSrcQL != NULL) && (pSrcQR != NULL));

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	_quatnAdd_x64(pDesQ, pSrcQL, pSrcQR);
#else
	__asm{
		mov			esi, pSrcQL;
		mov			eax, pSrcQR;
		mov			edi, pDesQ;
		movups		xmm0, [esi];		// ql
		movups		xmm1, [eax];		// qr
		addps		xmm0, xmm1;			// ql + qr
		movups		[edi], xmm0;
	}
#endif

#else
	pDesQ->r = pSrcQL->r + pSrcQR->r;
	pDesQ->x = pSrcQR->x + pSrcQL->x;
	pDesQ->y = pSrcQL->y + pSrcQR->y;
	pDesQ->z = pSrcQR->z + pSrcQL->z;
#endif
}

void __cdecl SYQuatn_Minus(SYQuatn* pDesQ,			// 目标四元数指针
						   const SYQuatn* pSrcQL,		// 左源四元数指针
						   const SYQuatn* pSrcQR)		// 右源四元数指针
{
	assert((pDesQ != NULL) && (pSrcQL != NULL) && (pSrcQR != NULL));

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64

	_quatnMinus_x64(pDesQ,pSrcQL,pSrcQR);		
#else
	__asm{
		mov			esi, pSrcQL;
		mov			eax, pSrcQR;
		mov			edi, pDesQ;
		movups		xmm0, [esi];		// ql
		movups		xmm1, [eax];		// qr
		subps		xmm0, xmm1;			// ql - qr
		movups		[edi], xmm0;
	}
#endif

#else
	pDesQ->r = pSrcQL->r - pSrcQR->r;
	pDesQ->x = pSrcQL->x - pSrcQR->x;
	pDesQ->y = pSrcQL->y - pSrcQR->y;
	pDesQ->z = pSrcQL->z - pSrcQR->z;
#endif
}

void __cdecl SYQuatn_Sub(SYQuatn* pDesQ,		// 目标四元数指针
						 const SYQuatn* pSrcQL,		// 左源四元数指针
						 const SYQuatn* pSrcQR)		// 右源四元数指针
{
	SYQuatn_Minus(pDesQ, pSrcQL, pSrcQR);
}

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
void __cdecl SYQuatn_StandardMul(SYQuatn* pDesQ,		// 目标四元数指针
								 const SYQuatn* pSrcQL,		// 左源四元数指针
								 const SYQuatn* pSrcQR)		// 右源四元数指针
{
	assert((pDesQ != NULL) && (pSrcQL != NULL) && (pSrcQR != NULL));

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64

	_quatnStandardMul_x64(pDesQ,		// 目标四元数指???
						pSrcQL,		// 左源四元数指???
						pSrcQR);		// 右源四元数指???
#else
	_asm{
		mov			esi, pSrcQL;
		mov			eax, pSrcQR;
		mov			edi, pDesQ;
		movups		xmm1, [esi];		// r1 z1 y1 x1
		movups		xmm2, [eax];		// r2 z2 y2 x2
		movdqa		xmm3, xmm1;
		movdqa		xmm5, xmm1;
		movdqa		xmm4, xmm2;
		movdqa		xmm6, xmm2;
		shufps		xmm3, xmm3, 0xff;	// r1 r1 r1 r1
		shufps		xmm5, xmm5, 0x92;	// z1 y1 x1 z1
		shufps		xmm4, xmm4, 0xff;	// r2 r2 r2 r2
		shufps		xmm6, xmm6, 0xcd;	// z2 x2 z2 y2
		xorps		xmm0, xmm0;
		mulps		xmm3, xmm2;			// r1*r2  r1*zz  r1*y2  r1*x2
		mulps		xmm4, xmm1;			// r2*r1  r2*z1  r2*y1  r2*x1
		mulps		xmm2, xmm1;			// r1*r2  z1*z2  y1*y2  z1*z2
		mulps		xmm5, xmm6;			// z1*z2  y1*x2  x1*z2  z1*y2
		mulps		xmm1, xmm6;			// r1*z2  z1*x2  y1*z2  x1*y2
		subps		xmm0, xmm2;			// -r1*r2  -z1*z2  -y1*y2  -x1*x2
		subps		xmm3, xmm5;			// r1*r2-z1*z2  r1*z2-y1*x2  r1*y2-x1*z2  r1*x1-z1*y2
		shufps		xmm1, xmm1, 0xc9;	// r1*z2  x1*y2  z1*x2  y1*z2
		movhlps		xmm6, xmm4;			// @  @  r2*r1  r2*z1
		movhlps		xmm7, xmm1;			// @  @  r1*z2  x1*y2
		movlhps		xmm6, xmm0;			// -y1*y2  -x1*x2  r2*r1  r2*z1
		movlhps		xmm7, xmm0;			// -y1*y2  -x1*x2  r1*z2  x1*y2
		shufps		xmm6, xmm6, 0x88;	// -x1*x2  r2*z1  -x1*x2  r2*z1
		shufps		xmm7, xmm7, 0xcc;	// -y1*y2  x1*y2  -y1*y2  x1*y2
		movlhps		xmm4, xmm6;			// -x1*x2  r2*z1  r2*y1  r2*x1
		movlhps		xmm1, xmm7;			// -y1*y2  x1*y2  z2*x1  y1*z2
		addps		xmm3, xmm4;			// r1*r2-x1*x2-z1*z2  r1*z2+r2*z1-y1*x2  r1*y2+r2*y1-x1*z2  r1*x2+r2*x1-z1*y2
		addps		xmm3, xmm1;			// r z y x
		movups		[edi], xmm3;
	}
#endif

#else
	assert((pDesQ != pSrcQL) && (pDesQ != pSrcQR));

	pDesQ->r = pSrcQL->r*pSrcQR->r - pSrcQL->x*pSrcQR->x - pSrcQL->y*pSrcQR->y - pSrcQL->z*pSrcQR->z;
	pDesQ->x = pSrcQL->r*pSrcQR->x + pSrcQL->x*pSrcQR->r + pSrcQL->y*pSrcQR->z - pSrcQL->z*pSrcQR->y;
	pDesQ->y = pSrcQL->r*pSrcQR->y - pSrcQL->x*pSrcQR->z + pSrcQL->y*pSrcQR->r + pSrcQL->z*pSrcQR->x;
	pDesQ->z = pSrcQL->r*pSrcQR->z + pSrcQL->x*pSrcQR->y - pSrcQL->y*pSrcQR->x + pSrcQL->z*pSrcQR->r;
#endif
}

// 自定义乘法定义：q1 * q2 = [ r1*r2 - v1*v2	r1*v2 + r2*v1 + v2 x v1 ]
// 旋转公式为：p2 = q^(-1) * p1 * q
//	r = ┌ r1*r2 - x1*x2 - y1*y2 - z1*z2 ┐
//	x = │ r1*x2 + r2*x1 + y2*z1 - z2*y1 │
//	y = │ r1*y2 + r2*y1 + z2*x1 - x2*z1 │
//	z = ┕ r1*z2 + r2*z1 + x2*y1 - y2*x1 ┘
void __cdecl SYQuatn_CustomMul(SYQuatn* pDesQ,			// 目标四元数指针
							   const SYQuatn* pSrcQL,		// 左源四元数指针
							   const SYQuatn* pSrcQR)		// 右源四元数指针
{
	assert((pDesQ != NULL) && (pSrcQL != NULL) && (pSrcQR != NULL));

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64

	_quatnCustomMul_x64(pDesQ,			// 目标四元数指???
						pSrcQL,		// 左源四元数指???
						pSrcQR);		// 右源四元数指???
#else
	_asm{
		mov			esi, pSrcQL;
		mov			eax, pSrcQR;
		mov			edi, pDesQ;
		movups		xmm1, [esi];		// r1 z1 y1 x1
		movups		xmm2, [eax];		// r2 z2 y2 x2
		movdqa		xmm3, xmm1;
		movdqa		xmm5, xmm1;
		movdqa		xmm4, xmm2;
		movdqa		xmm6, xmm2;
		shufps		xmm3, xmm3, 0xff;	// r1 r1 r1 r1
		shufps		xmm5, xmm5, 0x89;	// z1 x1 z1 y1
		shufps		xmm4, xmm4, 0xff;	// r2 r2 r2 r2
		shufps		xmm6, xmm6, 0x92;	// z2 y2 x2 z2
		xorps		xmm0, xmm0;
		mulps		xmm3, xmm2;			// r1*r2  r1*zz  r1*y2  r1*x2
		mulps		xmm4, xmm1;			// r2*r1  r2*z1  r2*y1  r2*x1
		mulps		xmm2, xmm1;			// r1*r2  z1*z2  y1*y2  z1*z2
		mulps		xmm5, xmm6;			// z1*z2  x1*y2  z1*x2  y1*z2
		mulps		xmm1, xmm6;			// r1*z2  z1*y2  y1*x2  x1*z2
		subps		xmm0, xmm2;			// -r1*r2  -z1*z2  -y1*y2  -x1*x2
		subps		xmm3, xmm5;			// r1*r2-z1*z2  r1*z2-x1*y2  r1*y2-z1*x2  r1*x1-y1*z2
		shufps		xmm1, xmm1, 0xd2;	// r1*z2  y1*x2  x1*z2  z1*y2
		movhlps		xmm6, xmm4;			// @  @  r2*r1  r2*z1
		movhlps		xmm7, xmm1;			// @  @  r1*z2  y1*x2
		movlhps		xmm6, xmm0;			// -y1*y2  -x1*x2  r2*r1  r2*z1
		movlhps		xmm7, xmm0;			// -y1*y2  -x1*x2  r1*z2  y1*x2
		shufps		xmm6, xmm6, 0x88;	// -x1*x2  r2*z1  -x1*x2  r2*z1
		shufps		xmm7, xmm7, 0xcc;	// -y1*y2  y1*x2  -y1*y2  y1*x2
		movlhps		xmm4, xmm6;			// -x1*x2  r2*z1  r2*y1  r2*x1
		movlhps		xmm1, xmm7;			// -y1*y2  y1*x2  x2*z1  z1*y2
		addps		xmm3, xmm4;			// r1*r2-x1*x2-z1*z2  r1*z2+r2*z1-x1*y2  r1*y2+r2*y1-z1*x2  r1*x2+r2*x1-y1*z2
		addps		xmm3, xmm1;			// r z y x
		movups		[edi], xmm3;
	}
#endif

#else
	assert((pDesQ != pSrcQL) && (pDesQ != pSrcQR));

	pDesQ->r = pSrcQL->r*pSrcQR->r - pSrcQL->x*pSrcQR->x - pSrcQL->y*pSrcQR->y - pSrcQL->z*pSrcQR->z;
	pDesQ->x = pSrcQL->r*pSrcQR->x + pSrcQL->x*pSrcQR->r - pSrcQL->y*pSrcQR->z + pSrcQL->z*pSrcQR->y;
	pDesQ->y = pSrcQL->r*pSrcQR->y + pSrcQL->x*pSrcQR->z + pSrcQL->y*pSrcQR->r - pSrcQL->z*pSrcQR->x;
	pDesQ->z = pSrcQL->r*pSrcQR->z - pSrcQL->x*pSrcQR->y + pSrcQL->y*pSrcQR->x + pSrcQL->z*pSrcQR->r;
#endif
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	共轭
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYQuatn_Conjugate(SYQuatn* pDesQ,			// 目标四元数指针
							   const SYQuatn* pSrcQ)			// 源四元数指针
{
	assert((pDesQ != NULL) && (pSrcQ != NULL));

	pDesQ->r = pSrcQ->r;
	pDesQ->x = -pSrcQ->x;
	pDesQ->y = -pSrcQ->y;
	pDesQ->z = -pSrcQ->z;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	归一化
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYQuatn_Normalize(SYQuatn* pDesQ,			// 目标四元数指针
							   const SYQuatn* pSrcQ)			// 源四元数指针
{
	assert((pDesQ != NULL) && (pSrcQ != NULL));

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64

	_quatnNormalize_x64(pDesQ,			// 目标四元数指???
						pSrcQ);			// 源四元数指针
#else
	__asm{
		mov			esi, pSrcQ;
		mov			edi, pDesQ;
		movups		xmm0, [esi];
		movdqa		xmm7, xmm0;
		mulps		xmm0, xmm0;		// 平方
		movhlps		xmm1, xmm0;
		addps		xmm0, xmm1;
		pshufd		xmm2, xmm0, 0x01;	// 求模
		addps		xmm0, xmm2;
		rsqrtss		xmm0, xmm0;
		shufps		xmm0, xmm0, 0x00;
		mulps		xmm0, xmm7;		// 归一化
		movups		[edi], xmm0;
	}
#endif

#else
	float d = 1.f / SYQuatn_Mode(pSrcQ);

	pDesQ->r = pSrcQ->r * d;
	pDesQ->x = pSrcQ->x * d;
	pDesQ->y = pSrcQ->y * d;
	pDesQ->z = pSrcQ->z * d;
#endif
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	缩放
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYQuatn_Scale(SYQuatn* pQ,			// 源四元数指针
						   float k)					// 系数
{
	assert(pQ != NULL);

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64

	_quatnScale_x64(pQ, k);					// 系数
#else
	__asm{
		mov			eax, pQ;
		movss		xmm1, k;
		movups		xmm0, [eax];		// r z y x
		shufps		xmm1, xmm1, 0x00;	// k k k k
		mulps		xmm0, xmm1;
		movups		[eax], xmm0;
	}
#endif

#else
	pQ->r *= k;
	pQ->x *= k;
	pQ->y *= k;
	pQ->z *= k;
#endif
}

void __cdecl SYQuatn_MulNum(SYQuatn* pDesQ,			// 目标四元数指针
							const SYQuatn* pSrcQ,			// 源四元数指针
							float k)					// 系数
{
	assert((pDesQ != NULL) && (pSrcQ != NULL));

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64

	_quatnMulNum_x64(pDesQ,			// 目标四元数指???
					pSrcQ,			// 源四元数指针
					k);					// 系数
#else
	__asm{
		mov			esi, pSrcQ;
		mov			edi, pDesQ;
		movss		xmm1, k;
		movups		xmm0, [esi];
		shufps		xmm1, xmm1, 0x00;
		mulps		xmm0, xmm1;
		movups		[edi], xmm0;
	}
#endif

#else
	pDesQ->r = pSrcQ->r * k;
	pDesQ->x = pSrcQ->x * k;
	pDesQ->y = pSrcQ->y * k;
	pDesQ->z = pSrcQ->z * k;
#endif
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	取模
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
float __cdecl SYQuatn_Mode(const SYQuatn* pQ)			// 四元数指针
{
	assert(pQ != NULL);

	float fRes(0.f);

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64

	_quatnMode_x64(&fRes,pQ);			// 四元数指???
#else
	__asm{
		mov			esi, pQ;
		movups		xmm0, [esi];
		mulps		xmm0, xmm0;
		movhlps		xmm1, xmm0;
		addps		xmm0, xmm1;
		pshufd		xmm2, xmm0, 0x01;
		addps		xmm0, xmm2;
		rsqrtss		xmm0, xmm0;
		rcpss		xmm0, xmm0;				// 平方根（会比直接使用 sqrtss 快 5 倍，但会出现奇异值）
		movss		fRes, xmm0;
	}
#endif

#else
	fRes = sqrtf(pQ->r*pQ->r + pQ->x*pQ->x + pQ->y*pQ->y + pQ->z*pQ->z);
#endif

	return fRes;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	距离
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
float __cdecl SYQuatn_Distance(const SYQuatn* pQ1,			// 四元数指针
							   const SYQuatn* pQ2)			// 四元数指针
{
	assert((pQ1 != NULL) && (pQ2 != NULL));

	SYQuatn q;

	SYQuatn_Minus(&q, pQ1, pQ2);

	return SYQuatn_Mode(&q);
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	点积（内积）
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
float __cdecl SYQuatn_DotProduct(const SYQuatn* pQ1,			// 四元数指针
								 const SYQuatn* pQ2)			// 四元数指针
{
	assert((pQ1 != NULL) && (pQ2 != NULL));

	float fRes(0.f);

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64

	_quatnDotProduct_x64(&fRes, pQ1,		// 四元数指???
								 pQ2);			// 四元数指???
#else
	__asm{
		mov			esi, pQ1;
		mov			edi, pQ2;
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
	fRes = (pQ1->r*pQ2->r + pQ1->x*pQ2->x + pQ1->y*pQ2->y + pQ1->z*pQ2->z);
#endif

	return fRes;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	外积
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYQuatn_OutterProduct(SYQuatn* pDesQ,			// 目标四元数指针
								   const SYQuatn* pSrcQL,		// 左源四元数指针
								   const SYQuatn* pSrcQR)		// 右源四元数指针
{
	assert((pDesQ != NULL) && (pSrcQR != NULL) && (pSrcQL != NULL));
	assert((pDesQ != pSrcQR) && (pDesQ != pSrcQL));

	pDesQ->r = 0.f;
	pDesQ->x = pSrcQL->r*pSrcQR->x - pSrcQL->x*pSrcQR->r - pSrcQL->y*pSrcQR->z + pSrcQL->z*pSrcQR->y;
	pDesQ->y = pSrcQL->r*pSrcQR->y + pSrcQL->x*pSrcQR->z - pSrcQL->y*pSrcQR->r - pSrcQL->z*pSrcQR->x;
	pDesQ->z = pSrcQL->r*pSrcQR->z - pSrcQL->x*pSrcQR->y + pSrcQL->y*pSrcQR->x - pSrcQL->z*pSrcQR->r;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	偶积
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYQuatn_EvenProduct(SYQuatn* pDesQ,		// 目标四元数指针
								 const SYQuatn* pSrcQL,		// 左源四元数指针
								 const SYQuatn* pSrcQR)		// 右源四元数指针
{
	assert((pDesQ != NULL) && (pSrcQR != NULL) && (pSrcQL != NULL));
	assert((pDesQ != pSrcQR) && (pDesQ != pSrcQL));

	pDesQ->r = pSrcQL->r*pSrcQR->r - pSrcQL->x*pSrcQR->x - pSrcQL->y*pSrcQR->y - pSrcQL->z*pSrcQR->z;
	pDesQ->x = pSrcQL->r*pSrcQR->x + pSrcQL->x*pSrcQR->r;
	pDesQ->y = pSrcQL->r*pSrcQR->y + pSrcQL->y*pSrcQR->r;
	pDesQ->z = pSrcQL->r*pSrcQR->z + pSrcQL->z*pSrcQR->r;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	叉积
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYQuatn_CrossProduct(float3* pDesV,	// 目标向量指针
								  const SYQuatn* pSrcQL,		// 左源四元数指针
								  const SYQuatn* pSrcQR)		// 右源四元数指针
{
	assert((pDesV != NULL) && (pSrcQR != NULL) && (pSrcQL != NULL));

	pDesV->x = pSrcQL->y*pSrcQR->z - pSrcQL->z*pSrcQR->y;
	pDesV->y = pSrcQL->z*pSrcQR->x - pSrcQL->x*pSrcQR->z;
	pDesV->z = pSrcQL->x*pSrcQR->y - pSrcQL->y*pSrcQR->x;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	求逆
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYQuatn_Inverse(SYQuatn* pDesQ,		// 目标四元数指针
							 const SYQuatn* pSrcQ)		// 源四元数指针
{
	assert((pDesQ != NULL) && (pSrcQ != NULL) && (pDesQ != pSrcQ));

	SYQuatn_Conjugate(pDesQ, pSrcQ);
	SYQuatn_Scale(pDesQ, 1.f / SYQuatn_DotProduct(pSrcQ, pSrcQ));
}


void __cdecl SYQuatn_Slerp_AxisAngle( vector4f* vresult,vector4f* v1,vector4f* v2 ,float f)
{
	if ( v1->x == v2->x && v1->y == v2->y && v1->z == v2->z )
	{
		vresult->x = v1->x;
		vresult->y = v1->y;
		vresult->z = v1->z;
		vresult->w = (float)(v1->w * (1.0 - f)  + v2->w * f);

	}
	else
	{
		SYQuatn sk,ek,qresult;
		SYQuatn_FromVector4f(&sk, v1 );
		SYQuatn_FromVector4f(&ek, v2 );
		SYQuatn_Slerp(&qresult, &sk, &ek ,f);
		SYQuatn_ToAxisAngle( vresult,&qresult);
	}
}

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
void __cdecl SYQuatn_Slerp(SYQuatn* pDesQ,			// 目标四元数指针
						   const SYQuatn* pSrcQ0,		// 源起始四元数
						   const SYQuatn* pSrcQ1,		// 源终端四元数
						   float t)					// 插值系数 [0～1]
{
	assert((pDesQ != NULL) && (pSrcQ0 != NULL) && (pSrcQ1 != NULL));

	float fCosOmega = SYQuatn_DotProduct(pSrcQ0, pSrcQ1);
	float k0 = 1.f - t, k1 = t;

	if (fCosOmega < 0.f)	// 由于最短弧路径的要求
	{
		fCosOmega = -fCosOmega;
		k1 = -k1;		// 翻转 k1 的符号即可实现反转一个四元数的效果
	}
	// 如果两个四元数非常接近则直接使线性插值
	if (fCosOmega <= 0.9999f)		// 使用三角公式求解其它系数
	{
		float fSinOmega = sqrtf(1.f - fCosOmega * fCosOmega);
		float fOmega = atan2f(fSinOmega, fCosOmega);
		float fSinOmega_Inv = 1.f / fSinOmega;

		k0 = sinf(k0 * fOmega) * fSinOmega_Inv;
		k1 = sinf(k1 * fOmega) * fSinOmega_Inv;
	}
#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64

	_quatnSlerp_x64(pDesQ,pSrcQ0,pSrcQ1,k0,k1);					// 插值系???[0???]
#else
	_asm{
		mov			esi, pSrcQ0;
		mov			eax, pSrcQ1;
		mov			edi, pDesQ;
		movups		xmm0, [esi];
		movups		xmm1, [eax];
		movss		xmm2, k0;
		movss		xmm3, k1;
		shufps		xmm2, xmm2, 0x00;
		shufps		xmm3, xmm3, 0x00;
		mulps		xmm0, xmm2;
		mulps		xmm1, xmm3;
		addps		xmm0, xmm1;
		movups		[edi], xmm0;
	}
#endif

#else
	pDesQ->r = pSrcQ0->r * k0 + pSrcQ1->r * k1;
	pDesQ->x = pSrcQ0->x * k0 + pSrcQ1->x * k1;
	pDesQ->y = pSrcQ0->y * k0 + pSrcQ1->y * k1;
	pDesQ->z = pSrcQ0->z * k0 + pSrcQ1->z * k1;
#endif
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	转化为矩阵
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYQuatn_ToMatrix4(matrix4f* pDesM,		// 目标矩阵指针
							  const SYQuatn* pSrcQ)			// 源四元数指针
{
	assert((pDesM != NULL) && (pSrcQ != NULL));

	float fTmp1(2.f * pSrcQ->x * pSrcQ->x);
	float fTmp2(2.f * pSrcQ->y * pSrcQ->y);
	float fTmp3(2.f * pSrcQ->z * pSrcQ->z);

	memset(pDesM, 0, sizeof(matrix4f) );
	//pDesM->element[0] = 1.f - (fTmp2 + fTmp3);
	//pDesM->element[5] = 1.f - (fTmp1 + fTmp3);
	//pDesM->element[10] = 1.f - (fTmp1 + fTmp2);
	pDesM->m[0][0] = 1.f - (fTmp2 + fTmp3);
	pDesM->m[1][1] = 1.f - (fTmp1 + fTmp3);
	pDesM->m[2][2] = 1.f - (fTmp1 + fTmp2);
	fTmp1 = 2.f * pSrcQ->x * pSrcQ->y;
	fTmp2 = 2.f * pSrcQ->r * pSrcQ->z;
	//pDesM->element[1] = fTmp1 + fTmp2;
	//pDesM->element[4] = fTmp1 - fTmp2;
	pDesM->m[0][1] = fTmp1 + fTmp2;
	pDesM->m[1][0] = fTmp1 - fTmp2;
	fTmp1 = 2.f * pSrcQ->x * pSrcQ->z;
	fTmp2 = 2.f * pSrcQ->r * pSrcQ->y;
	//pDesM->element[2] = fTmp1 - fTmp2;
	//pDesM->element[8] = fTmp1 + fTmp2;
	pDesM->m[0][2] = fTmp1 - fTmp2;
	pDesM->m[2][0] = fTmp1 + fTmp2;
	fTmp1 = 2.f * pSrcQ->y * pSrcQ->z;
	fTmp2 = 2.f * pSrcQ->r * pSrcQ->x;
	//pDesM->element[6] = fTmp1 + fTmp2;
	//pDesM->element[9] = fTmp1 - fTmp2;
	//pDesM->element[15] = 1.f;
	pDesM->m[1][2] = fTmp1 + fTmp2;
	pDesM->m[2][1] = fTmp1 - fTmp2;
	pDesM->m[3][3] = 1.f;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	转化矩阵为四元数
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYQuatn_FromMatrix(SYQuatn* pDesQ,		// 目标四元数指针
								const matrix4f* pSrcM)		// 源矩阵指针
{
	assert((pDesQ != NULL) && (pSrcM != NULL));

	//float max_s = pSrcM->element[0] + pSrcM->element[5] + pSrcM->element[10];	// r
	//float i_s = pSrcM->element[0] - pSrcM->element[5] - pSrcM->element[10];		// x
	float max_s = pSrcM->m[0][0] + pSrcM->m[1][1] + pSrcM->m[2][2];	// r
	float i_s = pSrcM->m[0][0] - pSrcM->m[1][1] - pSrcM->m[2][2];		// x
	int nMaxIndex = 0;		// 最大值表示 0 : r, 1 : x, 2 : y, 3 : x

	// 探测 r, x, y, z 中的最大绝对值
	if (i_s > max_s)		// x
	{
		max_s = i_s;
		nMaxIndex = 1;
	}
	//i_s = pSrcM->element[5] - pSrcM->element[0] - pSrcM->element[10];		// y
	i_s = pSrcM->m[1][1] - pSrcM->m[0][0] - pSrcM->m[2][2];		// y
	if (i_s > max_s)
	{
		max_s = i_s;
		nMaxIndex = 2;
	}
	//i_s = pSrcM->element[10] - pSrcM->element[0] - pSrcM->element[5];		// z
	i_s = pSrcM->m[2][2] - pSrcM->m[0][0] - pSrcM->m[1][1];		// z
	if (i_s > max_s)
	{
		max_s = i_s;
		nMaxIndex = 3;
	}
	// 计算平方根和除数因子
	max_s = sqrtf(max_s + 1.0f) * 0.5f;
	i_s = 0.25f / max_s;
	// 计算四元数的值
	switch (nMaxIndex)
	{
	case 0:			// r 最大
		pDesQ->r = max_s;
		//pDesQ->x = (pSrcM->element[6] - pSrcM->element[9]) * i_s;
		//pDesQ->y = (pSrcM->element[8] - pSrcM->element[2]) * i_s;
		//pDesQ->z = (pSrcM->element[1] - pSrcM->element[4]) * i_s;
		pDesQ->x = (pSrcM->m[1][2] - pSrcM->m[2][1]) * i_s;
		pDesQ->y = (pSrcM->m[2][0] - pSrcM->m[0][2]) * i_s;
		pDesQ->z = (pSrcM->m[0][1] - pSrcM->m[1][0]) * i_s;
		break;
	case 1:			// x 最大
		pDesQ->x = max_s;
		//pDesQ->r = (pSrcM->element[6] - pSrcM->element[9]) * i_s;
		//pDesQ->z = (pSrcM->element[8] + pSrcM->element[2]) * i_s;
		//pDesQ->y = (pSrcM->element[1] + pSrcM->element[4]) * i_s;
		pDesQ->r = (pSrcM->m[1][2] - pSrcM->m[2][1]) * i_s;
		pDesQ->z = (pSrcM->m[2][0] + pSrcM->m[0][2]) * i_s;
		pDesQ->y = (pSrcM->m[0][1] + pSrcM->m[1][0]) * i_s;
		break;
	case 2:			// y 最大
		pDesQ->y = max_s;
		//pDesQ->z = (pSrcM->element[6] + pSrcM->element[9]) * i_s;
		//pDesQ->r = (pSrcM->element[8] - pSrcM->element[2]) * i_s;
		//pDesQ->x = (pSrcM->element[1] + pSrcM->element[4]) * i_s;
		pDesQ->z = (pSrcM->m[1][2] + pSrcM->m[2][1]) * i_s;
		pDesQ->r = (pSrcM->m[2][0] - pSrcM->m[0][2]) * i_s;
		pDesQ->x = (pSrcM->m[0][1] + pSrcM->m[1][0]) * i_s;
		break;
	case 3:			// z 最大
		pDesQ->z = max_s;
		//pDesQ->y = (pSrcM->element[6] + pSrcM->element[9]) * i_s;
		//pDesQ->x = (pSrcM->element[8] + pSrcM->element[2]) * i_s;
		//pDesQ->r = (pSrcM->element[1] - pSrcM->element[4]) * i_s;
		pDesQ->y = (pSrcM->m[1][2] + pSrcM->m[2][1]) * i_s;
		pDesQ->x = (pSrcM->m[2][0] + pSrcM->m[0][2]) * i_s;
		pDesQ->r = (pSrcM->m[0][1] - pSrcM->m[1][0]) * i_s;
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	与矩阵的转换
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
void __cdecl quatnToMatrixQ4F(matrix4f* pDesM,		// 目标矩阵指针
								 const SYQuatn* pSrcQ)			// 源四元数指针
{
	assert((pDesM != NULL) && (pSrcQ != NULL));

	matrix4f_fromquatn(pDesM, pSrcQ);
}

void __cdecl quatnFromMatrix4(SYQuatn* pDesQ,			// 四元数指针
								   const matrix4f* pSrcM)		// 源矩阵指针
{
	assert((pDesQ != NULL) && (pSrcM != NULL));

	matrix4f_toquatn(pDesQ, pSrcM);
}
void __cdecl SYQuatn_FromVector4f(SYQuatn* pDesQ,			// 目标四元数指针
								   const float4* pSrcV)// 角度（弧度）
{
	SYQuatn_FromAngleAxis(pDesQ,			// 目标四元数指针
						(float3*)pSrcV,		// 源向量指针
						pSrcV->w);			// 角度（弧度）
}



void __cdecl SYQuatn_ToAxisAngle(vector4f* pAxisAngle,SYQuatn* pQ4 )
{
	pAxisAngle->w = float(2 * acos( pQ4->r ));
	if(pAxisAngle->w != 0)
	{
		pAxisAngle->x = float(pQ4->x /sin( pAxisAngle->w/2 ));
		pAxisAngle->y = float(pQ4->y /sin( pAxisAngle->w/2 ));
		pAxisAngle->z = float(pQ4->z /sin( pAxisAngle->w/2 ));
	}
	else
	{
		pAxisAngle->x = 0;
		pAxisAngle->y = 0;
		pAxisAngle->z = 0;
	}
}
///////////////////////////////////////////////////////////////////////////////
// 描述：	绕任意轴旋转的四元数（为了保证与矩阵的旋转统一，将在内部使用负角度，
//			即得到从旧坐标系到新坐标系的变换。）
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYQuatn_FromAngleAxis(SYQuatn* pDesQ,			// 目标四元数指针
								   const float3* pSrcV,		// 源向量指针
								   float fAngle)			// 角度（弧度）
{
	assert((pDesQ != NULL) && (pSrcV != NULL));

	float fHalfA = 0.5f * fAngle, fSinCoef = sinf(fHalfA);

#ifdef _USE_SSE_OPTIMAL_

	float fCosCoef = cosf(fHalfA);

#ifdef WIN64
	_quatnFromAngleAxis_x64(pDesQ,pSrcV,fCosCoef,fSinCoef);			// 角度（弧度）
#else
	_asm{
		mov			esi, pSrcV;
		mov			eax, pSrcV;
		mov			edi, pDesQ;
		add			eax, 0x08;
		movlps		xmm0, [esi];
		movss		xmm1, [eax];
		movss		xmm7, fCosCoef;		// 0 0 0 fCos
		movss		xmm6, fSinCoef;
		movlhps		xmm0, xmm1;			// 0 z y x
		shufps		xmm6, xmm6, 0x40;	// 0 fSin fSin fSin
		shufps		xmm7, xmm7, 0x15;	// fCos 0 0 0
		mulps		xmm0, xmm6;			// 必须 0 * 0 否则作或的时候会出现负数
		orps		xmm0, xmm7;
		movups		[edi], xmm0;
	}
#endif

#else
	pDesQ->r = cosf(fHalfA);
	pDesQ->x = pSrcV->x * fSinCoef;
	pDesQ->y = pSrcV->y * fSinCoef;
	pDesQ->z = pSrcV->z * fSinCoef;
#endif
}
// Rotate about special axes, such as X(1, 0, 0)、Y(0, 1, 0)、Z(0, 0, 1).
void __cdecl SYQuatn_FromAngleX(SYQuatn* pDesQ,	// destination pointer
								float fAngle)		// radian of ratation
{
	assert(pDesQ != NULL);

	float fHalfA = -0.5f * fAngle;

	pDesQ->x = sinf(fHalfA);
	pDesQ->r = cosf(fHalfA);
	pDesQ->y = pDesQ->z = 0.f;
}
void __cdecl SYQuatn_FromAngleY(SYQuatn* pDesQ,	// destination pointer
								float fAngle)		// radian of ratation
{
	assert(pDesQ != NULL);

	float fHalfA = -0.5f * fAngle;

	pDesQ->y = sinf(fHalfA);
	pDesQ->r = cosf(fHalfA);
	pDesQ->x = pDesQ->z = 0.f;
}
void __cdecl SYQuatn_FromAngleZ(SYQuatn* pDesQ,	// destination pointer
								float fAngle)		// radian of ratation
{
	assert(pDesQ != NULL);

	float fHalfA = -0.5f * fAngle;

	pDesQ->z = sinf(fHalfA);
	pDesQ->r = cosf(fHalfA);
	pDesQ->x = pDesQ->y = 0.f;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	将欧拉角转化为四元数
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
void __cdecl SYQuatn_FromEuler_LYXZ(SYQuatn* pDesQ,	// 目标四元数指针
									float fAngleY,		// 绕 Y 轴旋转角度
									float fAngleX,		// 绕 X 轴旋转角度
									float fAngleZ)		// 绕 Z 轴旋转角度
{
	assert(pDesQ != NULL);

	float fHalfY = fAngleY * 0.5f, fHalfX = fAngleX * 0.5f, fHalfZ = fAngleZ * 0.5f;
	float fCosY = cosf(fHalfY), fSinY = sinf(-fHalfY);
	float fCosX = cosf(fHalfX), fSinX = sinf(-fHalfX);
	float fCosZ = cosf(fHalfZ), fSinZ = sinf(-fHalfZ);

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64

	_quatnFromEuler_LYXZ_x64(pDesQ,	// 目标四元数指???
									fCosX,		// ???Y 轴旋转角???
									fSinX,		// ???X 轴旋转角???
									fCosY,		// ???Y 轴旋转角???
									fSinY,		// ???X 轴旋转角???
									fCosZ,		// ???Y 轴旋转角???
									fSinZ);		// ???X 轴旋转角???
#else
	_asm{
		mov			edi, pDesQ;
		movss		xmm3, fSinZ;		// 0 0 0 sinZ
		movss		xmm4, fCosZ;		// 0 0 0 cosZ
		movss		xmm1, fSinY;		// 0 0 0 sinY
		movss		xmm2, fCosY;		// 0 0 0 cosY
		movss		xmm5, fSinX;		// 0 0 0 sinX
		movss		xmm6, fCosX;		// 0 0 0 cosX
		xorps		xmm0, xmm0;
		shufps		xmm3, xmm4, 0x00;	// cosZ cosZ sinZ sinZ
		shufps		xmm1, xmm2, 0x00;	// cosY cosY sinY sinY
		shufps		xmm3, xmm3, 0x88;	// cosZ sinZ cosZ sinZ
		shufps		xmm5, xmm5, 0x00;	// sinX sinX sinX sinX
		shufps		xmm6, xmm6, 0x00;	// cosX cosX cosX cosX
		mulps		xmm1, xmm3;			// cosYcosZ  cosYsinZ  sinYcosZ  sinYsinZ
		subps		xmm0, xmm1;			// -cosYcosZ  -cosYsinZ  -sinYcosZ  -sinYsinZ
		movdqa		xmm2, xmm1;			// cosYcosZ  cosYsinZ  sinYcosZ  sinYsinZ
		shufps		xmm1, xmm0, 0xee;	// -cosYcosZ  -cosYsinZ  cosYcosZ  cosYsinZ
		shufps		xmm2, xmm0, 0x44;	// -sinYcosZ  -sinYsinZ  sinYcosZ  sinYsinZ
		movdqa		xmm3, xmm1;			// -cosYcosZ  -cosYsinZ  cosYcosZ  cosYsinZ
		shufps		xmm1, xmm2, 0x13;	// sinYsinZ  sinYcosZ  cosYsinZ  -cosYcosZ
		shufps		xmm2, xmm3, 0x6e;	// cosYcosZ  -cosYsinZ  -sinYcosZ  -sinYsinZ
		mulps		xmm1, xmm5;			// sinYsinXsinZ  sinYsinXcosZ  cosYsinXsinZ  -cosYsinXcosZ
		mulps		xmm2, xmm6;			// cosYcosXcosZ  -cosYcosXsinZ  -sinYcosXcosZ  -sinYcosXsinZ
		addps		xmm1, xmm2;			// r z y x
		movups		[edi], xmm1;
	}
#endif

#else
	float fCosYCosZ = fCosY * fCosZ, fCosYSinZ = fCosY * fSinZ;
	float fSinYCosZ = fSinY * fCosZ, fSinYSinZ = fSinY * fSinZ;

	pDesQ->r = fCosYCosZ * fCosX + fSinYSinZ * fSinX;
	pDesQ->x = -fSinYSinZ * fCosX - fCosYCosZ * fSinX;
	pDesQ->y = -fSinYCosZ * fCosX - fCosYSinZ * fSinX;
	pDesQ->z = -fCosYSinZ * fCosX - fSinYCosZ * fSinX;
#endif
}
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
void __cdecl SYQuatn_ToEuler_LYXZ(const SYQuatn* pSrcQ,		// 源四元数指针
								  float* pfAngleY,		// Y 旋转角度（弧度制）
								  float* pfAngleX,		// X 旋转角度（弧度制）
								  float* pfAngleZ)		// Z 旋转角度（弧度制）
{
	assert((pSrcQ != NULL) && (pfAngleY != NULL) && (pfAngleX != NULL) && (pfAngleZ != NULL));

	SYQuatn tmpQX, tmpQY, tmpQZ, tmpQR;
	float fSinX(0.f);

	SYQuatn_MulNum(&tmpQR, pSrcQ, pSrcQ->r);	// r^2  rz  ry  rx
	SYQuatn_MulNum(&tmpQZ, pSrcQ, pSrcQ->z);	// rz  z^2  yz  xz
	SYQuatn_MulNum(&tmpQY, pSrcQ, pSrcQ->y);	// ry  yz  y^2  xy
	SYQuatn_MulNum(&tmpQX, pSrcQ, pSrcQ->x);	// rx  xz  xy  x^2
	fSinX = -2.f * (tmpQZ.y + tmpQR.x);

	// 检查万向锁（万向锁出现时，绕 Y 与 Z 的旋转将会退化为按同一根轴旋转）
	if (fabsf(fSinX) > 0.9999f)		// 将 Z 置零，认为所有的旋转都绕 Y 轴进行
	{
		*pfAngleX = RADIAN_HALF_PI * fSinX;		// 考虑 X 的符号
		// 通过 -m31 和 m11 来计算
		*pfAngleY = atan2f(-2.f * (tmpQZ.x + tmpQR.y), (tmpQR.r + tmpQX.x - tmpQY.y - tmpQZ.z));
		*pfAngleZ = 0.f;
	}
	else	// 通过 m13 和 m33 来计算 Y，通过 m21 和 m22 来计算 Z。
	{
		*pfAngleX = asinf(fSinX);
		*pfAngleY = atan2f(2.f * (tmpQZ.x - tmpQR.y), (tmpQR.r + tmpQZ.z - tmpQY.y - tmpQX.x));
		*pfAngleZ = atan2f(2.f * (tmpQY.x - tmpQR.z), (tmpQR.r + tmpQY.y - tmpQX.x - tmpQZ.z));
	}
	*pfAngleX = -*pfAngleX;
	*pfAngleY = -*pfAngleY;
	*pfAngleZ = -*pfAngleZ;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	旋转向量，转换为矩阵再使用行向量乘以矩阵。
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl SYQuatn_RotVectorL(long3* pDesV,		// 目标向量指针
								const long3* pSrcV,		// 源向量指针
								const SYQuatn* pUQ)				// 单位轴四元数指针
{
	assert((pDesV != NULL) && (pUQ != NULL) && (pSrcV != NULL));

	matrix4f mat;
	float3 vs, vd;

	float3Set(&vs, (float)pSrcV->x, (float)pSrcV->y, (float)pSrcV->z);
	matrix4f_fromquatn(&mat, pUQ);
	float3MulMatQ(&vd, &vs, &mat);
	long3Set(pDesV, (LONG)vd.x, (LONG)vd.y, (LONG)vd.z);
}

void __cdecl SYQuatn_RotVector3f(float3* pDesV,		// 目标向量指针
								const float3* pSrcV,		// 源向量指针
								const SYQuatn* pUQ)				// 单位轴四元数指针
{
	assert((pDesV != NULL) && (pUQ != NULL) && (pSrcV != NULL));

	matrix4f mat;

	matrix4f_fromquatn(&mat, pUQ);
	float3MulMatQ(pDesV, pSrcV, &mat);
}

void __cdecl SYQuatn_RotVector3lf(double3* pDesV,		// 目标向量指针
								const double3* pSrcV,		// 源向量指针
								const SYQuatn* pUQ)				// 单位轴四元数指针
{
	assert((pDesV != NULL) && (pUQ != NULL) && (pSrcV != NULL));

	matrix4f mat;
	float3 vs, vd;

	float3Set(&vs, (float)pSrcV->x, (float)pSrcV->y, (float)pSrcV->z);
	matrix4f_fromquatn(&mat, pUQ);
	float3MulMatQ(&vd, &vs, &mat);
	double3Set(pDesV, (double)vd.x, (double)vd.y, (double)vd.z);
}

//////////////////////////////////////////////////////////////////////////
// 描述：	与矩阵的乘积
//
// 返回值：	无
//////////////////////////////////////////////////////////////////////////
void __cdecl SYQuatn_MulMatQ(SYQuatn* pDesQ,			// 目标四元数指针
							 const SYQuatn* pSrcQ,			// 源四元数指针
							 const matrix4f* pSrcM)		// 源矩阵指针
{
	assert((pDesQ != NULL) && (pSrcQ != NULL) && (pSrcM != NULL));

#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64

	_quatnMulMatQ_x64(pDesQ,			// 目标四元数指???
							 pSrcQ,			// 源四元数指针
							 pSrcM);		// 源矩阵指???
#else
	__asm{
		mov			esi, pSrcQ;
		mov			edi, pDesQ;
		mov			eax, pSrcM;
		mov			ebx, pSrcM;
		mov			ecx, pSrcM;
		mov			edx, pSrcM;
		add			ebx, 0x10;
		add			ecx, 0x20;
		add			edx, 0x30;
		movups		xmm0, [esi];		// r z y x
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
		shufps		xmm3, xmm3, 0xff;	// r r r r
		mulps		xmm4, xmm0;			// x * m1
		mulps		xmm5, xmm1;			// y * m2
		mulps		xmm6, xmm2;			// z * m3
		mulps		xmm7, xmm3;			// r * m4
		addps		xmm4, xmm5;
		addps		xmm6, xmm7;
		addps		xmm4, xmm6;
		movups		[edi], xmm4;
	}
#endif

#else
	assert(pDesQ != pSrcQ);

	pDesQ->x = pSrcQ->x * pSrcM->_11 + pSrcQ->y * pSrcM->_21 + pSrcQ->z * pSrcM->_31 + pSrcQ->r * pSrcM->_41;
	pDesQ->y = pSrcQ->x * pSrcM->_12 + pSrcQ->y * pSrcM->_22 + pSrcQ->z * pSrcM->_32 + pSrcQ->r * pSrcM->_42;
	pDesQ->z = pSrcQ->x * pSrcM->_13 + pSrcQ->y * pSrcM->_23 + pSrcQ->z * pSrcM->_33 + pSrcQ->r * pSrcM->_43;
	pDesQ->r = pSrcQ->x * pSrcM->_14 + pSrcQ->y * pSrcM->_24 + pSrcQ->z * pSrcM->_34 + pSrcQ->r * pSrcM->_44;
#endif
}

//////////////////////////////////////////////////////////////////////////
// 描述：	获取从一个向量旋转到另一个向量的四元数
//
// 返回值：	无
//////////////////////////////////////////////////////////////////////////
void __cdecl SYQuatn_In2Vector(SYQuatn* pDesQ,			// 目标四元数指针
							   const float3* pStartV,	// 起始向量指针
							   const float3* pEndV)		// 结束向量指针
{
	assert((pDesQ != NULL) && (pStartV != NULL) && (pEndV != NULL));

	double3 vnStart, vnEnd, vAxis;
	float3 vFAxis;
	vnStart.x = pStartV->x;
	vnStart.y = pStartV->y;
	vnStart.z = pStartV->z;

	vnEnd.x = pEndV->x;
	vnEnd.y = pEndV->y;
	vnEnd.z = pEndV->z;

	//float3Normalize(&vnStart, pStartV);
	//float3Normalize(&vnEnd, pEndV);
	double3CrossProduct(&vAxis, &vnEnd,&vnStart);
	if(vAxis.x == 0 && vAxis.y == 0 && vAxis.z == 0)
	{
		// 共线
		if ( double3DotProduct(&vnEnd, &vnStart) > 0 )
		{
			// 同向
			pDesQ->x = 0;
			pDesQ->y = 0;
			pDesQ->z = 0;
			pDesQ->r = 1;
		}
		else
		{
			// 异向
			pDesQ->x = 0;
			pDesQ->y = 0;
			pDesQ->z = 1;
			pDesQ->r = 0;
		}			
		return;
	}
	double3Normalize(&vAxis, &vAxis);
	float fangle = (float)acos(double3DotProduct(&vnStart, &vnEnd)/(double3Mode(&vnStart) * double3Mode(&vnEnd)));
	vFAxis.x = (float)vAxis.x;
	vFAxis.y = (float)vAxis.y;
	vFAxis.z = (float)vAxis.z;
	SYQuatn_FromAngleAxis(pDesQ, &vFAxis, fangle);
}


void __cdecl SYQuatn_In2Vector3lf(SYQuatn* pDesQ,			// 目标四元数指针
							 const double3* pStartV,	// 起始向量指针
							 const double3* pEndV)		// 结束向量指针
{
	assert((pDesQ != NULL) && (pStartV != NULL) && (pEndV != NULL));

	double3 vnStart, vnEnd, vAxis;
	float3 vFAxis;
	vnStart.x = pStartV->x;
	vnStart.y = pStartV->y;
	vnStart.z = pStartV->z;

	vnEnd.x = pEndV->x;
	vnEnd.y = pEndV->y;
	vnEnd.z = pEndV->z;

	//float3Normalize(&vnStart, pStartV);
	//float3Normalize(&vnEnd, pEndV);
	double3CrossProduct(&vAxis, &vnEnd,&vnStart);
	if(vAxis.x == 0 && vAxis.y == 0 && vAxis.z == 0)
	{
		// 共线
		if ( double3DotProduct(&vnEnd, &vnStart) > 0 )
		{
			// 同向
			pDesQ->x = 0;
			pDesQ->y = 0;
			pDesQ->z = 0;
			pDesQ->r = 1;
		}
		else
		{
			// 异向
			pDesQ->x = 0;
			pDesQ->y = 0;
			pDesQ->z = 1;
			pDesQ->r = 0;
		}			
		return;
	}
	double3Normalize(&vAxis, &vAxis);
	float fangle = (float)acos(double3DotProduct(&vnStart, &vnEnd)/(double3Mode(&vnStart) * double3Mode(&vnEnd)));
	vFAxis.x = (float)vAxis.x;
	vFAxis.y = (float)vAxis.y;
	vFAxis.z = (float)vAxis.z;
	SYQuatn_FromAngleAxis(pDesQ, &vFAxis, fangle);
}
//------------------------------- End of File ---------------------------------
