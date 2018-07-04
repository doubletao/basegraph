#include "stdafx.h"
#include <assert.h>
#include "Common\MatrixDef.h"
#include "Common\VectorDef.h"
#include "Common\QuatnDef.h"
#include "MathOpASM64.h"

BOOL iszero(float value)
{
	if(fabs(value) < ABS_FLOAT_0)
		return TRUE;
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	零化矩阵
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl matrix4f_zero(pmatrix4f pM)			// 矩阵指针
{
	assert(pM != NULL);
	memset(pM->m, 0, sizeof(matrix4f));
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	单位矩阵
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl matrix4f_identity(pmatrix4f pM)			// 矩阵指针
{
	memset(pM, 0, sizeof(matrix4f));
	pM->_11 = pM->_22 = pM->_33 = pM->_44 = 1.0f;
}

void __cdecl matrix4f_eye(pmatrix4f pM)			// 矩阵指针
{
	matrix4f_identity(pM);
}

BASEGRAPH_COMMON_API void __cdecl matrix4f_debug(const matrix4f * pM)
{
	if (pM)
	{
		CString strOut;
		strOut.Format(_T("matrix %p:\n"), pM);
		OutputDebugString(strOut);
		for (int i = 0; i < 4; i++)
		{
			for(int j = 0; j < 4; j++)
			{
				CString strOut;
				strOut.Format(_T("m[%d][%d]=%f "), i, j, pM->m[i][j]);
				OutputDebugString(strOut);
			}
			OutputDebugString(_T("\n"));
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	负矩阵
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl matrix4f_negative(pmatrix4f pDesM,		// 目标矩阵指针
								  pmatrix4f pSrcM)		// 源矩阵指针
{
#ifdef _USE_SSE_OPTIMAL_
	
#ifdef WIN64
	matrix4_negative_x64(pDesM, pSrcM);
#else
	__asm{
		mov			esi, pSrcM;
		mov			eax, pSrcM;
		mov			ecx, pSrcM;
		mov			edx, pSrcM;
		add			eax, 0x10;
		add			ecx, 0x20;
		add			edx, 0x30;
		movups		xmm0, [esi];		// 第一行
		movups		xmm1, [eax];		// 第二行
		movups		xmm2, [ecx];		// 第三行
		movups		xmm3, [edx];		// 第四行
		xorps		xmm4, xmm4;
		xorps		xmm5, xmm5;
		xorps		xmm6, xmm6;
		xorps		xmm7, xmm7;
		subps		xmm4, xmm0;
		subps		xmm5, xmm1;
		subps		xmm6, xmm2;
		subps		xmm7, xmm3;
		mov			edi, pDesM;
		mov			eax, pDesM;
		mov			ecx, pDesM;
		mov			edx, pDesM;
		add			eax, 0x10;
		add			ecx, 0x20;
		add			edx, 0x30;
		movups		[edi], xmm4;
		movups		[eax], xmm5;
		movups		[ecx], xmm6;
		movups		[edx], xmm7;
	}
#endif

#else
	for (int r=0; r<4; r++)
	{
		for (int c=0; c<4; c++)
		{
			pDesM->m[r][c] = -pSrcM->m[r][c];
		}
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	获取矩阵的某个元素（下标从 1 开始）
//
// 返回值:	float
///////////////////////////////////////////////////////////////////////////////
float __cdecl matrix4f_getat(pmatrix4f pM,	// 矩阵指针
								int r,				// 行
								int c)				// 列
{
	return pM->m[r][c];
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	设置矩阵的某个元素（下标从 1 开始）
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl matrix4f_setat(pmatrix4f pM,		// 矩阵指针
							   int r,					// 行
							   int c,					// 列
							   float v)					// 值
{
	pM->m[r][c] = v;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩阵与数的加减乘法
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl matrix4f_addnum(pmatrix4f pDesM,		// 目标矩阵指针
								const matrix4f* pSrcM,		// 源矩阵指针
								float k)					// 数
{
#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	matrix4_addnum_x64(pDesM, pSrcM, k);
#else
	_asm{
		mov			esi, pSrcM;			// 源
		mov			eax, pSrcM;
		mov			ecx, pSrcM;
		mov			edx, pSrcM;
		add			eax, 0x10;
		add			ecx, 0x20;
		add			edx, 0x30;
		movss		xmm4, k;			// 0 0 0 k
		movups		xmm0, [esi];		// 第一行
		movups		xmm1, [eax];		// 第二行
		movups		xmm2, [ecx];		// 第三行
		movups		xmm3, [edx];		// 第四行
		movdqa		xmm5, xmm4;
		movdqa		xmm6, xmm4;
		movdqa		xmm7, xmm4;
		shufps		xmm5, xmm5, 0x51;	// 0 0 k 0
		shufps		xmm6, xmm6, 0x45;	// 0 k 0 0
		shufps		xmm7, xmm7, 0x15;	// k 0 0 0
		addps		xmm0, xmm4;
		addps		xmm1, xmm5;
		addps		xmm2, xmm6;
		addps		xmm3, xmm7;
		mov			edi, pDesM;			// 目的
		mov			eax, pDesM;
		mov			ecx, pDesM;
		mov			edx, pDesM;
		add			eax, 0x10;
		add			ecx, 0x20;
		add			edx, 0x30;
		movups		[edi], xmm0;
		movups		[eax], xmm1;
		movups		[ecx], xmm2;
		movups		[edx], xmm3;
	}
#endif

#else
	if (pDesM != pSrcM)
	{
		memcpy(pDesM->m, pSrcM->m, sizeof(matrix4f));
	}
	for (int i=0; i<4; i++)
	{
		pDesM->m[i][i] += k;
	}
#endif
}

void __cdecl matrix4f_minusnum(pmatrix4f pDesM,		// 目标矩阵指针
								  pmatrix4f pSrcM,		// 源矩阵指针
								  float k)					// 数
{
	matrix4f_addnum(pDesM, pSrcM, -k);
}

void __cdecl matrix4f_subnum(pmatrix4f pDesM,		// 目标矩阵指针
								pmatrix4f pSrcM,		// 源矩阵指针
								float k)					// 数
{
	matrix4f_addnum(pDesM, pSrcM, -k);
}

void __cdecl matrix4f_mulnum(pmatrix4f pDesM,		// 目标矩阵指针
								pmatrix4f pSrcM,		// 源矩阵指针
								float k)					// 数
{
#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	matrix4_mulnum_x64(pDesM, pSrcM, k);
#else
	_asm{
		mov			esi, pSrcM;			// 源
		mov			eax, pSrcM;
		mov			ecx, pSrcM;
		mov			edx, pSrcM;
		movss		xmm7, k;			// 0 0 0 k
		add			eax, 0x10;
		add			ecx, 0x20;
		add			edx, 0x30;
		shufps		xmm7, xmm7, 0x00;	// k k k k
		movups		xmm0, [esi];		// 第一行
		movups		xmm1, [eax];		// 第二行
		movups		xmm2, [ecx];		// 第三行
		movups		xmm3, [edx];		// 第四行
		mulps		xmm0, xmm7;
		mulps		xmm1, xmm7;
		mulps		xmm2, xmm7;
		mulps		xmm3, xmm7;
		mov			edi, pDesM;			// 目的
		mov			eax, pDesM;
		mov			ecx, pDesM;
		mov			edx, pDesM;
		add			eax, 0x10;
		add			ecx, 0x20;
		add			edx, 0x30;
		movups		[edi], xmm0;
		movups		[eax], xmm1;
		movups		[ecx], xmm2;
		movups		[edx], xmm3;
	}
#endif

#else
	for (int r=0; r<4; r++)
	{
		for (int c=0; c<4; c++)
		{
			pDesM->m[r][c] = pSrcM->m[r][c] * k;
		}
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩阵的加减乘法
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
void __cdecl matrix4f_add(pmatrix4f pDesM,		// 目标矩阵指针
							 pmatrix4f pSrcML,		// 左矩阵指针
							 pmatrix4f pSrcMR)		// 右矩阵指针
{
#ifdef _USE_SSE_OPTIMAL_
	
#ifdef WIN64
	matrix4_add_x64(pDesM, pSrcML, pSrcMR);
#else
	_asm{
		mov			esi, pSrcML;		// 左矩阵
		mov			eax, pSrcML;
		mov			ecx, pSrcML;
		mov			edx, pSrcML;
		add			eax, 0x10;
		add			ecx, 0x20;
		add			edx, 0x30;
		movups		xmm0, [esi];		// 第一行
		movups		xmm1, [eax];		// 第二行
		movups		xmm2, [ecx];		// 第三行
		movups		xmm3, [edx];		// 第四行
		mov			esi, pSrcMR;		// 右矩阵
		mov			eax, pSrcMR;
		mov			ecx, pSrcMR;
		mov			edx, pSrcMR;
		add			eax, 0x10;
		add			ecx, 0x20;
		add			edx, 0x30;
		movups		xmm4, [esi];		// 第一行
		movups		xmm5, [eax];		// 第二行
		movups		xmm6, [ecx];		// 第三行
		movups		xmm7, [edx];		// 第四行
		// 求和
		addps		xmm0, xmm4;			// 第一行
		addps		xmm1, xmm5;			// 第二行
		addps		xmm2, xmm6;			// 第三行
		addps		xmm3, xmm7;			// 第四行
		mov			edi, pDesM;			// 目标矩阵
		mov			eax, pDesM;
		mov			ecx, pDesM;
		mov			edx, pDesM;
		add			eax, 0x10;
		add			ecx, 0x20;
		add			edx, 0x30;
		movups		[edi], xmm0;		// 第一行
		movups		[eax], xmm1;		// 第二行
		movups		[ecx], xmm2;		// 第三行
		movups		[edx], xmm3;		// 第四行
	}
#endif

#else
	for (int r=0; r<4; r++)
	{
		for (int c=0; c<4; c++)
		{
			pDesM->m[r][c] = pSrcML->m[r][c] + pSrcMR->m[r][c];
		}
	}
#endif
}

void __cdecl matrix4f_minus(pmatrix4f pDesM,			// 目标矩阵指针
								pmatrix4f pSrcML,		// 左矩阵指针
								pmatrix4f pSrcMR)		// 右矩阵指针
{
#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	matrix4_minus_x64(pDesM, pSrcML, pSrcMR);
#else
	_asm{
		mov			esi, pSrcML;		// 左矩阵
		mov			eax, pSrcML;
		mov			ecx, pSrcML;
		mov			edx, pSrcML;
		add			eax, 0x10;
		add			ecx, 0x20;
		add			edx, 0x30;
		movups		xmm0, [esi];		// 第一行
		movups		xmm1, [eax];		// 第二行
		movups		xmm2, [ecx];		// 第三行
		movups		xmm3, [edx];		// 第四行
		mov			esi, pSrcMR;		// 右矩阵
		mov			eax, pSrcMR;
		mov			ecx, pSrcMR;
		mov			edx, pSrcMR;
		add			eax, 0x10;
		add			ecx, 0x20;
		add			edx, 0x30;
		movups		xmm4, [esi];		// 第一行
		movups		xmm5, [eax];		// 第二行
		movups		xmm6, [ecx];		// 第三行
		movups		xmm7, [edx];		// 第四行
		// 求差
		subps		xmm0, xmm4;			// 第一行
		subps		xmm1, xmm5;			// 第二行
		subps		xmm2, xmm6;			// 第三行
		subps		xmm3, xmm7;			// 第四行
		mov			edi, pDesM;			// 目标矩阵
		mov			eax, pDesM;
		mov			ecx, pDesM;
		mov			edx, pDesM;
		add			eax, 0x10;
		add			ecx, 0x20;
		add			edx, 0x30;
		movups		[edi], xmm0;		// 第一行
		movups		[eax], xmm1;		// 第二行
		movups		[ecx], xmm2;		// 第三行
		movups		[edx], xmm3;		// 第四行
	}
#endif

#else
	for (int r=0; r<4; r++)
	{
		for (int c=0; c<4; c++)
		{
			pDesM->m[r][c] = pSrcML->m[r][c] - pSrcMR->m[r][c];
		}
	}
#endif
}

void __cdecl matrix4f_sub(pmatrix4f pDesM,		// 目标矩阵指针
							 pmatrix4f pSrcML,		// 左矩阵指针
							 pmatrix4f pSrcMR)		// 右矩阵指针
{
	matrix4f_minus(pDesM, pSrcML, pSrcMR);
}

void __cdecl matrix4f_mul(pmatrix4f pDesM,		// 目标矩阵指针
							 const matrix4f* pSrcML,		// 左矩阵指针
							 const matrix4f*  pSrcMR)		// 右矩阵指针
{
#ifdef _USE_SSE_OPTIMAL_		// 如果使用 SSE 则不需要考虑内存重复使用的问题

#ifdef WIN64
	matrix4_mul_x64(pDesM, pSrcML, pSrcMR);
#else
	_asm{
		// 先将右矩阵完全读出
		mov			esi, pSrcMR;		// 右矩阵
		mov			eax, pSrcMR;
		mov			ebx, pSrcMR;
		mov			edx, pSrcMR;
		add			eax, 0x10;
		add			ebx, 0x20;
		add			edx, 0x30;
		movups		xmm4, [esi];		// 第一行
		movups		xmm5, [eax];		// 第二行
		movups		xmm6, [ebx];		// 第三行
		movups		xmm7, [edx];		// 第四行
		mov			ecx, 0x04;
		mov			esi, pSrcML;		// 左矩阵
		mov			edi, pDesM;			// 目标矩阵
loop_line:
		movups		xmm0, [esi];		// 第一行
		movdqa		xmm1, xmm0;
		movdqa		xmm2, xmm0;
		movdqa		xmm3, xmm0;
		shufps		xmm0, xmm0, 0x00;	// 第一列
		shufps		xmm1, xmm1, 0x55;	// 第二列
		shufps		xmm2, xmm2, 0xaa;	// 第三列
		shufps		xmm3, xmm3, 0xff;	// 第四列
		mulps		xmm0, xmm4;			// 第一列 * 第一行
		mulps		xmm1, xmm5;			// 第二列 * 第二行
		mulps		xmm2, xmm6;			// 第三列 * 第三行
		mulps		xmm3, xmm7;			// 第四列 * 第四行
		addps		xmm0, xmm1;
		addps		xmm2, xmm3;
		addps		xmm0, xmm2;			// 求和得到第一行
		movups		[edi], xmm0;
		add			esi, 0x10;
		add			edi, 0x10;
		sub			ecx, 0x01;
		jnz			loop_line;
	}
#endif

#else
	assert((pDesM != pSrcML) && (pDesM != pSrcMR));

	memset(pDesM->m, 0, sizeof(matrix4f));
	for (int r=0; r<4; r++)
	{
		for (int c=0; c<4; c++)
		{
			for (int k=0; k<4; k++)
			{
				pDesM->m[r][c] += pSrcML->m[r][k] * pSrcMR->m[k][c];
			}
		}
	}
#endif
}

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
void __cdecl matrix4f_fromquatn(pmatrix4f pDesM,		// 目标矩阵指针
								   const SYQuatn* pSrcQ)			// 源四元数指针
{
#ifdef _USE_SSE_OPTIMAL_
	float fOne(1.f);

#ifdef WIN64
	matrix4_fromquatn_x64(pDesM, pSrcQ, fOne);
#else
	_asm{
		mov			esi, pSrcQ;
		mov			edi, pDesM;
		mov			eax, pDesM;
		mov			ecx, pDesM;
		mov			edx, pDesM;
		add			eax, 0x10;
		add			ecx, 0x20;
		add			edx, 0x30;
		movups		xmm7, [esi];		// r z y x
		movss		xmm1, fOne;			// 0 0 0 1
		movdqa		xmm0, xmm7;
		movdqa		xmm2, xmm7;
		movdqa		xmm5, xmm7;
		movdqa		xmm6, xmm7;
		shufps		xmm2, xmm2, 0xff;	// r r r r
		shufps		xmm5, xmm5, 0xd2;	// r y x z
		shufps		xmm6, xmm6, 0xc9;	// r x z y
		shufps		xmm1, xmm1, 0x15;	// 1 0 0 0
		mulps		xmm0, xmm0;			// r^2 z^2 y^2 x^2
		mulps		xmm7, xmm2;			// r^2 rz ry rx
		mulps		xmm5, xmm6;			// r^2 xy xz yz
		movdqa		xmm2, xmm0;
		movdqa		xmm3, xmm0;
		movdqa		xmm4, xmm0;
		shufps		xmm2, xmm2, 0xff;	// r^2 r^2 r^2 r^2
		shufps		xmm3, xmm3, 0xc9;	// r^2 x^2 z^2 y^2
		shufps		xmm4, xmm4, 0xd2;	// r^2 y^2 x^2 z^2
		addps		xmm5, xmm5;			// 2r^2  2xy  2xz  2yz
		addps		xmm7, xmm7;			// 2r^2  2rz  2ry  2rx
		addps		xmm0, xmm2;			// 2r^2  r^2+z^2  r^2+y^2  r^2+x^2
		addps		xmm3, xmm4;			// 2r^2  x^2+y^2  x^2+z^2  y^2+z^2
		movdqa		xmm6, xmm5;
		subps		xmm5, xmm7;			// 0  _21  _13  _32
		subps		xmm0, xmm3;			// 0  _33  _22  _11
		addps		xmm6, xmm7;			// *  _12  _31  _23
		movdqa		xmm2, xmm5;
		movdqa		xmm4, xmm0;
		shufps		xmm5, xmm6, 0x50;	// _31  _31  _32  _32
		shufps		xmm0, xmm6, 0xa0;	// _12  _12  _11  _11
		shufps		xmm6, xmm2, 0xd0;	// 0  _13  _23  _23
		shufps		xmm2, xmm4, 0x5a;	// _22  _22  _21  _21
		shufps		xmm5, xmm4, 0xe2;	// 0  _33  _32  _31
		shufps		xmm0, xmm6, 0xe8;	// 0  _13  _12  _11
		shufps		xmm2, xmm6, 0xc8;	// 0  _23  _22  _21
		movups		[edx], xmm1;		// 第四行
		movups		[edi], xmm0;		// 第一行
		movups		[eax], xmm2;		// 第二行
		movups		[ecx], xmm5;		// 第三行
	}
#endif

#else
	float fTmp0 = pSrcQ->r * pSrcQ->r;
	float fTmp1 = pSrcQ->x * pSrcQ->x;
	float fTmp2 = pSrcQ->y * pSrcQ->y;
	float fTmp3 = pSrcQ->z * pSrcQ->z;

	pDesM->_14 = pDesM->_24 = pDesM->_34 = pDesM->_41 = pDesM->_42 = pDesM->_43 = 0.f;
	pDesM->_11 = fTmp0 + fTmp1 - fTmp2 - fTmp3;
	pDesM->_22 = fTmp0 + fTmp2 - fTmp3 - fTmp1;
	pDesM->_33 = fTmp0 + fTmp3 - fTmp1 - fTmp2;
	pDesM->_44 = 1.f;
	fTmp1 = 2.f * pSrcQ->x * pSrcQ->y;
	fTmp2 = 2.f * pSrcQ->r * pSrcQ->z;
	pDesM->_12 = fTmp1 + fTmp2;
	pDesM->_21 = fTmp1 - fTmp2;
	fTmp1 = 2.f * pSrcQ->x * pSrcQ->z;
	fTmp2 = 2.f * pSrcQ->r * pSrcQ->y;
	pDesM->_13 = fTmp1 - fTmp2;
	pDesM->_31 = fTmp1 + fTmp2;
	fTmp1 = 2.f * pSrcQ->y * pSrcQ->z;
	fTmp2 = 2.f * pSrcQ->r * pSrcQ->x;
	pDesM->_23 = fTmp1 + fTmp2;
	pDesM->_32 = fTmp1 - fTmp2;
#endif
}

void __cdecl matrix4f_toquatn(SYQuatn* pDesQ,			// 目标四元数指针
								 const matrix4f* pSrcM)		// 源矩阵指针
{
	float max_s = pSrcM->_11 + pSrcM->_22 + pSrcM->_33;		// r
	float i_s = pSrcM->_11 - pSrcM->_22 - pSrcM->_33;		// x
	int nMaxIndex(0);		// 最大值表示 0 : r, 1 : x, 2 : y, 3 : x

	// 探测 r, x, y, z 中的最大绝对值
	if (i_s > max_s)		// x
	{
		max_s = i_s;
		nMaxIndex = 1;
	}
	i_s = pSrcM->_22 - pSrcM->_11 - pSrcM->_33;		// y
	if (i_s > max_s)
	{
		max_s = i_s;
		nMaxIndex = 2;
	}
	i_s = pSrcM->_33 - pSrcM->_22 - pSrcM->_11;		// z
	if (i_s > max_s)
	{
		max_s = i_s;
		nMaxIndex = 3;
	}
	// 计算平方根和除数因子
	max_s = sqrtf(max_s + 1.f) * 0.5f;
	i_s = 0.25f / max_s;
	// 计算四元数的值
	switch (nMaxIndex)
	{
	case 0:			// r 最大
		pDesQ->r = max_s;
		pDesQ->x = (pSrcM->_23 - pSrcM->_32) * i_s;
		pDesQ->y = (pSrcM->_31 - pSrcM->_13) * i_s;
		pDesQ->z = (pSrcM->_12 - pSrcM->_21) * i_s;
		break;
	case 1:			// x 最大
		pDesQ->x = max_s;
		pDesQ->r = (pSrcM->_23 - pSrcM->_32) * i_s;
		pDesQ->z = (pSrcM->_31 + pSrcM->_13) * i_s;
		pDesQ->y = (pSrcM->_12 + pSrcM->_21) * i_s;
		break;
	case 2:			// y 最大
		pDesQ->y = max_s;
		pDesQ->z = (pSrcM->_23 + pSrcM->_32) * i_s;
		pDesQ->r = (pSrcM->_31 - pSrcM->_13) * i_s;
		pDesQ->x = (pSrcM->_12 + pSrcM->_21) * i_s;
		break;
	case 3:			// z 最大
		pDesQ->z = max_s;
		pDesQ->y = (pSrcM->_23 + pSrcM->_32) * i_s;
		pDesQ->x = (pSrcM->_31 + pSrcM->_13) * i_s;
		pDesQ->r = (pSrcM->_12 - pSrcM->_21) * i_s;
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
// 描述：	矩阵级联乘以四元数，结果存入矩阵，Mr = Ms * Qs。
//
// 返回值：	无
//////////////////////////////////////////////////////////////////////////
void __cdecl matrix4f_mulquatn(pmatrix4f pDesM,		// 目标矩阵指针
								  pmatrix4f pSrcML,		// 源矩阵指针（左）
								  const SYQuatn* pSrcQR)			// 源四元数指针（右）
{
	matrix4f tmpMR;

	matrix4f_fromquatn(&tmpMR, pSrcQR);
	matrix4f_mul(pDesM, pSrcML, &tmpMR);
}

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
void __cdecl matrix4f_fromrotvector(pmatrix4f pDesM,		// 目标矩阵指针
									   const float3* pSrcV,		// 源向量指针
									   float fAngle)			// 旋转角度（弧度制）
{
	float fCoefCos = cosf(fAngle), fCoefSin = sinf(fAngle);

#ifdef _USE_SSE_OPTIMAL_
	float fOne(1.f);

#ifdef WIN64
	matrix4_fromrotvector_x64(pDesM, pSrcV, fAngle, fCoefCos, fCoefSin, fOne);
#else
	_asm{
		mov			esi, pSrcV;
		mov			ebx, pSrcV;
		mov			edi, pDesM;
		mov			eax, pDesM;
		mov			ecx, pDesM;
		mov			edx, pDesM;
		add			ebx, 0x08;
		add			eax, 0x10;
		add			ecx, 0x20;
		add			edx, 0x30;
		movlps		xmm7, [esi];		// * * y x
		movss		xmm6, [ebx];		// 0 0 0 z
		movss		xmm1, fOne;			// 0 0 0 1
		movss		xmm2, fCoefCos;		// 0 0 0 cosA
		movss		xmm3, fCoefSin;		// 0 0 0 sinA
		movlhps		xmm7, xmm6;			// 0 z y x
		movdqa		xmm0, xmm1;
		movdqa		xmm5, xmm7;
		movdqa		xmm6, xmm7;
		subps		xmm1, xmm2;			// 0 0 0 1-cosA
		shufps		xmm3, xmm3, 0x40;	// 0 sinA sinA sinA
		shufps		xmm5, xmm5, 0xc9;	// 0 x z y
		shufps		xmm1, xmm1, 0x40;	// 0 1-cosA 1-cosA 1-cosA
		shufps		xmm2, xmm2, 0x40;	// 0 cosA cosA cosA
		shufps		xmm6, xmm6, 0xd2;	// 0 y x z
		mulps		xmm1, xmm7;			// 0 z(1-cosA) y(1-cosA) x(1-cosA)
		mulps		xmm3, xmm5;			// 0 xsinA zsinA ysinA
		mulps		xmm6, xmm1;			// 0 yz(1-cosA) xy(1-cosA) zx(1-cosA)
		mulps		xmm7, xmm1;			// 0 z^2(1-cosA) y^2(1-cosA) x^2(1-cosA)
		movdqa		xmm4, xmm6;
		subps		xmm6, xmm3;			// 0  _32  _21  _13
		addps		xmm7, xmm2;			// 0  _33  _22  _11
		addps		xmm4, xmm3;			// 0  _23  _12  _31
		movdqa		xmm5, xmm6;
		movdqa		xmm1, xmm7;
		shufps		xmm5, xmm7, 0x55;	// _22  _22  _21  _21
		shufps		xmm1, xmm4, 0x50;	// _12  _12  _11  _11
		shufps		xmm5, xmm4, 0xe8;	// 0  _23  _22  _21
		shufps		xmm4, xmm6, 0xa0;	// _32  _32  _31  _31
		shufps		xmm1, xmm6, 0xc8;	// 0  _13  _12  _11
		shufps		xmm4, xmm7, 0xe8;	// 0  _33  _32  _31
		shufps		xmm0, xmm0, 0x15;	// _44 0 0 0
		movups		[edx], xmm0;		// 第四行
		movups		[eax], xmm5;		// 第二行
		movups		[edi], xmm1;		// 第一行
		movups		[ecx], xmm4;		// 第三行
	}
#endif

#else
	float fCosComp = 1.f - fCoefCos;
	float fCoefX = pSrcV->x * fCosComp;
	float fCoefY = pSrcV->y * fCosComp;
	float fCoefZ = pSrcV->z * fCosComp;
	float fTmp1(0.f), fTmp2(0.f);

	pDesM->_14 = pDesM->_24 = pDesM->_34 = pDesM->_41 = pDesM->_42 = pDesM->_43 = 0.f;
	pDesM->_11 = fCoefX * pSrcV->x + fCoefCos;
	pDesM->_22 = fCoefY * pSrcV->y + fCoefCos;
	pDesM->_33 = fCoefZ * pSrcV->z + fCoefCos;
	pDesM->_44 = 1.f;
	fTmp1 = fCoefX * pSrcV->y;
	fTmp2 = pSrcV->z * fCoefSin;
	pDesM->_12 = fTmp1 + fTmp2;
	pDesM->_21 = fTmp1 - fTmp2;
	fTmp1 = fCoefX * pSrcV->z;
	fTmp2 = pSrcV->y * fCoefSin;
	pDesM->_13 = fTmp1 - fTmp2;
	pDesM->_31 = fTmp1 + fTmp2;
	fTmp1 = fCoefZ * pSrcV->y;
	fTmp2 = pSrcV->x * fCoefSin;
	pDesM->_23 = fTmp1 + fTmp2;
	pDesM->_32 = fTmp1 - fTmp2;
#endif
}

//////////////////////////////////////////////////////////////////////////
// 描述：	欧拉角到旋转矩阵的变换（必须定义旋转顺序以及坐标系方向）
//
// 返回值：	无
//////////////////////////////////////////////////////////////////////////

// 本系统使用行向量与矩阵的乘法，并定义左手坐标系，旋转的正方向定义为：
// 以左手法则，从旋转轴的正端点看向负端点，顺时针方向为正。
// 旋转顺序为 Y X Z，XY 组成屏幕平面，X 指向右，Y 指向上，Z 垂直于平面指向屏幕内。
// ┌ cosYcosZ + sinYsinXsinZ		sinYsinXcosZ - cosYsinZ		sinYcosX ┐
// │		 cosXsinZ						cosXcosZ			  -sinX	 │
// ┕ cosYsinXsinZ - sinYcosZ		sinYsinZ + cosYsinXcosZ		cosYcosX ┘
void __cdecl matrix4f_fromeuler_lyxz(pmatrix4f pDesM,	// 目标矩阵指针
										float fAngleY,			// Y 旋转角度（弧度制）
										float fAngleX,			// X 旋转角度（弧度制）
										float fAngleZ)			// Z 旋转角度（弧度制）
{
	matrix4f_fromeuler_lxyz(pDesM, fAngleX, fAngleY, fAngleZ);
	return;
	float fCosY = cosf(fAngleY), fSinY = -sinf(fAngleY);
	float fCosX = cosf(fAngleX), fSinX = -sinf(fAngleX);
	float fCosZ = cosf(fAngleZ), fSinZ = -sinf(fAngleZ);

#ifdef _USE_SSE_OPTIMAL_
	float fOne(1.f);

#ifdef WIN64
	matrix4_fromeuler_lyxz_x64(pDesM,  fCosX, fSinX, fCosY, fSinY, fCosZ, fSinZ, fOne);
#else
	_asm{
		mov			edi, pDesM;
		mov			eax, pDesM;
		mov			ecx, pDesM;
		mov			edx, pDesM;
		add			eax, 0x10;
		add			ecx, 0x20;
		add			edx, 0x30;
		xorps		xmm0, xmm0;			// 0 0 0 0
		movss		xmm1, fCosX;		// 0 0 0 cosX
		movss		xmm2, fSinX;		// 0 0 0 sinX
		movss		xmm3, fCosY;		// 0 0 0 cosY
		movss		xmm4, fSinY;		// 0 0 0 sinY
		movss		xmm5, fCosZ;		// 0 0 0 cosZ
		movss		xmm6, fSinZ;		// 0 0 0 sinZ
		movss		xmm7, fOne;			// 0 0 0 1
		subps		xmm0, xmm2;			// 0 0 0 _23
		shufps		xmm4, xmm3, 0x00;	// cosY cosY sinY sinY
		shufps		xmm6, xmm5, 0x00;	// cosZ cosZ sinZ sinZ
		shufps		xmm1, xmm1, 0x00;	// cosX cosX cosX cosX
		shufps		xmm2, xmm2, 0x00;	// sinX sinX sinX sinX
		shufps		xmm7, xmm7, 0x15;	// _44 0 0 0
		movdqa		xmm3, xmm4;
		movdqa		xmm5, xmm6;
		shufps		xmm6, xmm6, 0x88;	// cosZ sinZ cosZ sinZ
		shufps		xmm4, xmm5, 0x88;	// cosZ sinZ cosY sinY
		mulps		xmm3, xmm6;			// cosYcosZ  cosYsinZ  sinYcosZ  sinYsinZ
		mulps		xmm1, xmm4;			// cosXcosZ  cosXsinZ  cosYcosX  sinYcosX ( _22  _21  _33  _13 )
		xorps		xmm5, xmm5;
		subps		xmm5, xmm3;			// -cosYcosZ  -cosYsinZ  -sinYcosZ  -sinYsinZ
		mulps		xmm2, xmm3;			// cosYsinXcosZ  cosYsinXsinZ  sinYsinXcosZ  sinYsinXsinZ
		shufps		xmm5, xmm3, 0xc9;	// cosYcosZ  sinYsinZ  -cosYsinZ  -sinYcosZ
		shufps		xmm0, xmm1, 0x44;	// _33  _13  0  _23
		shufps		xmm2, xmm2, 0x36;	// sinYsinXsinZ  cosYsinXcosZ  sinYsinXcosZ  cosYsinXsinZ
		addps		xmm2, xmm5;			// _11  _32  _12  _31
		movdqa		xmm4, xmm2;
		shufps		xmm1, xmm0, 0x4e;	// 0  _23  _22  _21
		shufps		xmm2, xmm0, 0x67;	// 0  _13  _12  _11
		shufps		xmm4, xmm0, 0x78;	// 0  _33  _32  _31
		movups		[edi], xmm2;		// 第一行
		movups		[eax], xmm1;		// 第二行
		movups		[ecx], xmm4;		// 第三行
		movups		[edx], xmm7;		// 第四行
	}
#endif

#else
	float fSinYSinX = fSinY * fSinX, fCosYSinZ = fCosY * fSinZ, fCosYCosZ = fCosY * fCosZ;

	pDesM->_14 = pDesM->_24 = pDesM->_34 = pDesM->_41 = pDesM->_42 = pDesM->_43 = 0.f;
	pDesM->_44 = 1.f;
	pDesM->_11 = fCosY * fCosZ + fSinYSinX * fSinZ;
	pDesM->_12 = fSinYSinX * fCosZ - fCosYSinZ;
	pDesM->_13 = fSinY * fCosX;
	pDesM->_21 = fCosX * fSinZ;
	pDesM->_22 = fCosX * fCosZ;
	pDesM->_23 = -fSinX;
	pDesM->_31 = fCosYSinZ * fSinX - fSinY * fCosZ;
	pDesM->_32 = fSinY * fSinZ + fCosYCosZ * fSinX;
	pDesM->_33 = fCosY * fCosX;
#endif
}
// 对任意给定角位移，存在无穷多个欧拉角可以用来表示它，称作“别名”问题。
// 因此，采用“限制欧拉角”，Y 与 Z 的范围为 [-180, 180]，X 的范围为 [-90, 90]。
// X = asin(-m23)
// 当 cosX = 0 时，出现万向锁的情况，可以通过 -m31 和 m11 计算 Y
// Y = atan2(-m31, m11)    Z = 0
// 正常情况则通过 m13 和 m33 计算 Y，通过 m21 和 m22 计算 Z
// Y = atan2(m13, m33)    Z = atan2(m21, m22)
void __cdecl matrix4f_toeuler_lyxz(pmatrix4f pSrcM,	// 源矩阵指针
									  float* pfAngleY,		// Y 旋转角度（弧度制）
									  float* pfAngleX,		// X 旋转角度（弧度制）
									  float* pfAngleZ)		// Z 旋转角度（弧度制）
{
	matrix4f_toeuler_lxyz(pSrcM, pfAngleX, pfAngleY, pfAngleZ);
	return;
	// 通过 -m23 计算 X，确定数值的安全性，asin 的自变量域为 [-1, 1]
	if (pSrcM->_23 >= 1.f)		// -PI / 2
	{
		*pfAngleX = RADIAN_HALF_PI;
	}
	else if (pSrcM->_23 <= -1.f)	// PI / 2
	{
		*pfAngleX = -RADIAN_HALF_PI;
	}
	else
	{
		*pfAngleX = asinf(pSrcM->_23);
	}
	// 检查万向锁（万向锁出现时，绕 Y 与 Z 的旋转将会退化为按同一根轴旋转）
	if (fabsf(pSrcM->_23) > 0.9999f)		// 将 Z 置零，认为所有的旋转都绕 Y 轴进行
	{
		*pfAngleY = atan2f(pSrcM->_31, pSrcM->_11);	// 通过 -m31 和 m11 来计算
		*pfAngleZ = 0.f;
	}
	else	// 通过 m13 和 m33 来计算 Y，通过 m21 和 m22 来计算 Z。
	{
		*pfAngleY = atan2f(-pSrcM->_13, pSrcM->_33);
		*pfAngleZ = atan2f(-pSrcM->_21, pSrcM->_22);
	}
	//if ((int)*pfAngleX!=0 || (int)*pfAngleY!=0 || (int)*pfAngleZ!=0)
	//{
		//TRACE(_T("to:*pfAngleX=%f*,pfAngleY=%f,*pfAngleZ=%f\n"), *pfAngleX, *pfAngleY, *pfAngleZ);
	//}
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩阵的转置
//
// 返回值:	无
///////////////////////////////////////////////////////////////////////////////
#include <emmintrin.h>
void __cdecl matrix4f_transpose(pmatrix4f pDesM,		// 目标矩阵指针
								   const matrix4f* pSrcM)		// 源矩阵指针
{
#ifdef _USE_SSE_OPTIMAL_

#ifdef WIN64
	matrix4_transpose_x64(pDesM, pSrcM);
#else
	__asm{
		mov			esi, pSrcM;			// 源
		mov			eax, pSrcM;
		mov			ecx, pSrcM;
		mov			edx, pSrcM;
		mov			edi, pDesM;
		add			eax, 0x10;
		add			ecx, 0x20;
		add			edx, 0x30;
		movups		xmm0, [esi];		// 第一行
		movups		xmm1, [eax];		// 第二行
		movups		xmm2, [ecx];		// 第三行
		movups		xmm3, [edx];		// 第四行
		movdqa		xmm6, xmm0;
		movdqa		xmm7, xmm2;
		shufps		xmm0, xmm1, 0x44;	// m22 m21 m12 m11
		shufps		xmm2, xmm3, 0x44;	// m42 m41 m32 m31
		shufps		xmm6, xmm1, 0xee;	// m24 m23 m14 m13
		shufps		xmm7, xmm3, 0xee;	// m44 m43 m34 m33
		movdqa		xmm1, xmm0;			// m22 m21 m12 m11
		movdqa		xmm3, xmm6;			// m24 m23 m14 m13
		shufps		xmm0, xmm2, 0x88;	// m41 m31 m21 m11
		shufps		xmm1, xmm2, 0xdd;	// m42 m32 m22 m12
		shufps		xmm3, xmm7, 0x88;	// m43 m33 m23 m13
		shufps		xmm6, xmm7, 0xdd;	// m44 m34 m24 m14
		mov			eax, pDesM;
		mov			ecx, pDesM;
		mov			edx, pDesM;
		add			eax, 0x10;
		add			ecx, 0x20;
		add			edx, 0x30;
		movups		[edi], xmm0;
		movups		[eax], xmm1;
		movups		[ecx], xmm3;
		movups		[edx], xmm6;
	}
#endif

#else
	for (int r=0; r<4; r++)
	{
		for (int c=0; c<4; c++)
		{
			pDesM->m[r][c] = pSrcM->m[c][r];
		}
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩阵的求逆（采用初等行变换法，O(n^3)）
//
// 返回值:	TRUE		求解成功
//			FALSE		求解失败
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl matrix4f_inverse(pmatrix4f pDesM,		// 目标矩阵指针
								 const matrix4f* pSrcM)		// 源矩阵指针
{
	//D3DXMatrixInverse((D3DXMATRIX*)(pDesM),0,(D3DXMATRIX*)pSrcM);
	//return TRUE;

	float fScale(0.f), fMax(0.f);
	int nCheckCount(0);			// 处理到第几列
	int nRow[4] = { 0, 1, 2, 3 };	// 存储新的行索引
	BOOL bRow[4] = { FALSE };		// 行是否归一化（已为主导行）
	matrix4f mTmp;				// 缓存

	// 确保源矩阵不被改变
	matrix4f_addnum(pDesM, pSrcM, 0.f);
	matrix4f_identity(&mTmp);
	// 处理
	while (nCheckCount < 4)
	{
		fMax = 0.f;
		// 选取某列中非主导变换行的最大行
		for (int r=0; r<4; r++)
		{
			if (bRow[r])
				continue;

			if (fabsf(pDesM->m[r][nCheckCount]) > fabsf(fMax))
			{
				nRow[nCheckCount] = r;
				fMax = pDesM->m[r][nCheckCount];
			}
		}
		// 防止由于数值过小而出现奇异
		if (fMax == 0)
			return FALSE;

		fMax = 1.f / fMax;
		// 逐行处理
		for (int r=0; r<4; r++)
		{
			if (r == nRow[nCheckCount])
			{
				// 归一化本行
				if (fMax != 1.f)
				{
					for (int c=0; c<4; c++)
					{
						if (c > nCheckCount)			// 归一化右边
						{
							pDesM->m[r][c] *= fMax;
						}
						mTmp.m[r][c] *= fMax;
					}
				}
				// 设置主导行及已归一标置
				bRow[r] = TRUE;
			}
			else		// 将其他行的该列归 0
			{
				fScale = pDesM->m[r][nCheckCount];
				if (fScale != 0.f)
				{
					// 如果主导行未归一则系数要多除主导行的主导列值
					if (!bRow[nRow[nCheckCount]])
					{
						fScale *= fMax;
					}
					for (int c=0; c<4; c++)
					{
						if (c > nCheckCount)		// 合并右边
						{
							pDesM->m[r][c] -= pDesM->m[nRow[nCheckCount]][c] * fScale;
						}
						mTmp.m[r][c] -= mTmp.m[nRow[nCheckCount]][c] * fScale;
						// 当值足够小则清空
						//if (iszero(mTmp.m[r][c]))
						//{
						//	mTmp.m[r][c] = 0.f;
						//}
					}
				}
			}
		}
		nCheckCount++;
	}
	// 交换已变换的矩阵
	for (int r=0; r<4; r++)
	{
		memcpy(pDesM->m[r], mTmp.m[nRow[r]], sizeof(float) << 2);
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 描述：	矩阵的相等判断
//
// 返回值:	TRUE		相等
//			FALSE		不等
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl matrix4f_isequal(matrix4f* pM1,matrix4f* pM2)
{
	for(int i = 0; i < 4; i++)
	{
		__m128 m1 = _mm_loadu_ps(pM1->m[i]);
		__m128 m2 = _mm_loadu_ps(pM2->m[i]);

		__m128 eq = _mm_cmpeq_ps(m1,m2);
		if(_mm_movemask_ps(eq) != 0xf)
			return FALSE;
	}
	return TRUE;
	//if(pM1->_11 != pM2->_11 || 
	//	pM1->_12 != pM2->_12 || 
	//	pM1->_13 != pM2->_13 || 
	//	pM1->_14 != pM2->_14 || 
	//	pM1->_21 != pM2->_21 || 
	//	pM1->_22 != pM2->_22 || 
	//	pM1->_23 != pM2->_23 || 
	//	pM1->_24 != pM2->_24 || 
	//	pM1->_31 != pM2->_31 || 
	//	pM1->_32 != pM2->_32 || 
	//	pM1->_33 != pM2->_33 || 
	//	pM1->_34 != pM2->_34 || 
	//	pM1->_41 != pM2->_41 || 
	//	pM1->_42 != pM2->_42 || 
	//	pM1->_43 != pM2->_43 || 
	//	pM1->_44 != pM2->_44 )
	//	return FALSE;
	//return TRUE;
}
//////////////////////////////////////////////////////////////////////////

BOOL __cdecl matrix4f_isidentity(matrix4f* pM)
{
	if(pM->_11 != 1.0f||
		pM->_22 != 1.0f||
		pM->_33 != 1.0f||
		pM->_44 != 1.0f||
		pM->_12 != 0.0f||
		pM->_13 != 0.0f||
		pM->_14 != 0.0f||
		pM->_21 != 0.0f||
		pM->_23 != 0.0f||
		pM->_24 != 0.0f||
		pM->_31 != 0.0f||
		pM->_32 != 0.0f||
		pM->_34 != 0.0f||
		pM->_41 != 0.0f||
		pM->_42 != 0.0f||
		pM->_43 != 0.0f)
		return FALSE;
	return TRUE;
}

void __cdecl matrix4f_from_pos( pmatrix4f pDesM, /* 目标矩阵指针 */ float fX, /* X 较蛭 移 */ float fY, /* Y 较蛭 移 */ float fZ )
{
	matrix4f_identity(pDesM);
	pDesM->_41 = fX;
	pDesM->_42 = fY;
	pDesM->_43 = fZ;
}

void __cdecl matrix4f_to_pos( const matrix4f* pSrcM, /* 源矩阵指针 */ float* pfX, /* X 较蛭 移 */ float* pfY, /* Y 较蛭 移 */ float* pfZ )
{
	*pfX = pSrcM->_41;
	*pfY = pSrcM->_42;
	*pfZ = pSrcM->_43;
}

void __cdecl matrix4f_create_view( matrix4f* pDesM, const float3* pEye, const float3* pAt, const float3* pUp )
{
	float3 zaxis = *pAt - *pEye;
	float3Normalize(&zaxis,&zaxis);
	float3 xaxis,yaxis;
	float3CrossProduct(&xaxis,pUp,&zaxis);
	float3Normalize(&xaxis,&xaxis);
	float3CrossProduct(&yaxis,&zaxis,&xaxis);
	pDesM->_11 = xaxis.x;
	pDesM->_21 = xaxis.y;
	pDesM->_31 = xaxis.z;

	pDesM->_12 = yaxis.x;
	pDesM->_22 = yaxis.y;
	pDesM->_32 = yaxis.z;

	pDesM->_13 = zaxis.x;
	pDesM->_23 = zaxis.y;
	pDesM->_33 = zaxis.z;

	pDesM->_14 = 0;
	pDesM->_24 = 0;
	pDesM->_34 = 0;

	pDesM->_41 = -float3DotProduct(&xaxis,pEye);
	pDesM->_42 = -float3DotProduct(&yaxis,pEye);
	pDesM->_43 = -float3DotProduct(&zaxis,pEye);
	pDesM->_44 = 1;
}

void __cdecl matrix4f_from_scale( pmatrix4f pDesM, /* 目标矩阵指针 */ float fX, /* X 较?潘?*/ float fY, /* Y 较?潘?*/ float fZ )
{
	matrix4f_identity(pDesM);
	pDesM->_11 = fX;
	pDesM->_22 = fY;
	pDesM->_33 = fZ;
}

void __cdecl matrix4f_to_normal_matrix( matrix4f* pDesM, const matrix4f* pSrc )
{
	memcpy(pDesM,pSrc,sizeof(matrix4f));
	pDesM->_41 = 0;
	pDesM->_42 = 0;
	pDesM->_43 = 0;
}

// 对任意给定角位移，存在无穷多个欧拉角可以用来表示它，称作“别名”问题。
// 因此，采用“限制欧拉角”，X 与 Z 的范围为 [-180, 180]，Y 的范围为 [-90, 90]。
// Y = asin(-m13)
// 当 cosY = 0 时，出现万向锁的情况，可以通过 m31 和 m21 计算 X
// X = atan2(m21, m31)    Z = 0
// 正常情况则通过 m23 和 m33 计算 X，通过 m11 和 m12 计算 Z
// X = atan2(m23, m33)    Z = atan2(m12, m11)
void __cdecl matrix4f_toeuler_lxyz(const matrix4f* pSrcM,	// 源矩阵指针
									  float* pfAngleX,		// X 旋转角度（弧度制）
									  float* pfAngleY,		// Y 旋转角度（弧度制）
									  float* pfAngleZ)		// Z 旋转角度（弧度制）
{
	// 通过 -m23 计算 X，确定数值的安全性，asin 的自变量域为 [-1, 1]
	if (pSrcM->_13 >= 1.f)		// -PI / 2
	{
		*pfAngleY = -RADIAN_HALF_PI;
	}
	else if (pSrcM->_13 <= -1.f)	// PI / 2
	{
		*pfAngleY = RADIAN_HALF_PI;
	}
	else
	{
		*pfAngleY = asinf(-pSrcM->_13);
	}
	// 检查万向锁（万向锁出现时，绕 Y 与 Z 的旋转将会退化为按同一根轴旋转）
	if (fabsf(pSrcM->_13) > 0.9999f)		// 将 Z 置零，认为所有的旋转都绕 Y 轴进行
	{
		*pfAngleX = atan2f(pSrcM->_21, pSrcM->_31);	// 通过 -m31 和 m11 来计算
		*pfAngleZ = 0.f;
	}
	else	// 通过 m13 和 m33 来计算 Y，通过 m21 和 m22 来计算 Z。
	{
		*pfAngleX = atan2f(pSrcM->_23, pSrcM->_33);
		*pfAngleZ = atan2f(pSrcM->_12, pSrcM->_11);
	}
}

void __cdecl matrix4f_fromeuler_lxyz(pmatrix4f pDesM,	// 目标矩阵指针
										float fAngleX,			// X 旋转角度（弧度制）
										float fAngleY,			// Y 旋转角度（弧度制）
										float fAngleZ)			// Z 旋转角度（弧度制）
{
	float fCosX = cosf(fAngleX), fSinX = sinf(fAngleX);
	float fCosY = cosf(fAngleY), fSinY = sinf(fAngleY);
	float fCosZ = cosf(fAngleZ), fSinZ = sinf(fAngleZ);
	float fSinYSinX = fSinY * fSinX, fCosXSinZ = fCosX * fSinZ, fCosXCosZ = fCosX * fCosZ;

	pDesM->_14 = pDesM->_24 = pDesM->_34 = pDesM->_41 = pDesM->_42 = pDesM->_43 = 0.f;
	pDesM->_44 = 1.f;

	pDesM->_11 = fCosY * fCosZ;
	pDesM->_12 = fCosY * fSinZ;
	pDesM->_13 = -fSinY;
	pDesM->_21 = fSinYSinX * fCosZ - fCosXSinZ;
	pDesM->_22 = fSinYSinX * fSinZ + fCosXCosZ;
	pDesM->_23 = fSinX * fCosY;
	pDesM->_31 = fCosXCosZ * fSinY + fSinX * fSinZ;
	pDesM->_32 = fCosXSinZ * fSinY - fCosZ * fSinX;
	pDesM->_33 = fCosY * fCosX;
}

void __cdecl matrix4f_from_euler( matrix4f* pDesM,float fAngleX,float fAngleY,float fAngleZ,int nOrder )
{
	//if ((int)fAngleX!=0 || (int)fAngleY!=0 || (int)fAngleZ!=0)
	//{
		//TRACE(_T("fAngleX=%f, fAngleY=%f, fAngleZ=%f\n"), fAngleX, fAngleY, fAngleZ);
	//}
	switch(nOrder)
	{

	case ROTATE_ORDER_YXZ:
		matrix4f_fromeuler_lyxz(pDesM,fAngleY,fAngleX,fAngleZ);
		break;
	case ROTATE_ORDER_XYZ:
		matrix4f_fromeuler_lxyz(pDesM,fAngleX,fAngleY,fAngleZ);
		break;

	case  ROTATE_ORDER_XZY:
		{
			matrix4f xzRot,yRot;
			matrix4f_fromeuler_lyxz(&xzRot,0,fAngleX,fAngleZ);
			matrix4f_fromeuler_lyxz(&yRot,fAngleY,0,0);
			*pDesM = xzRot * yRot;
		}
		break;
	case ROTATE_ORDER_YZX:
		{
			matrix4f yzRot,xRot;
			matrix4f_fromeuler_lxyz(&yzRot,0,fAngleY,fAngleZ);
			matrix4f_fromeuler_lxyz(&xRot,fAngleX,0,0);
			*pDesM = yzRot*xRot;
		}
		break;
	case ROTATE_ORDER_ZXY:
		{
			matrix4f xyRot,zRot;
			matrix4f_fromeuler_lxyz(&xyRot,fAngleX,fAngleY,0);
			matrix4f_fromeuler_lxyz(&zRot,0,0,fAngleZ);
			*pDesM = zRot*xyRot;			
		}
		break;
	case ROTATE_ORDER_ZYX:
		{
			matrix4f yxRot,zRot;
			matrix4f_fromeuler_lyxz(&yxRot,fAngleY,fAngleX,0);
			matrix4f_fromeuler_lyxz(&zRot,0,0,fAngleZ);
			*pDesM = zRot*yxRot;
		}
		break;
	}
}

#include <smmintrin.h>
//////////////////////////////////////////////////////////////////////////
//采用QR分解法，将3x3矩阵分解为旋转和scale矩阵
// 假设 M = [ m0 | m1 | m2 ]， Q = [ q0 | q1 | q2 ]，则
// q0 = m0/|m0|
// q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
// q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
//
//s0 = q0*m0
//s1 = q1*m1
//s2 = q2*m2

void GetMatrixScaleRot(const matrix4f* pSrcMat,matrix4f* pRotMat,float3* pScale,matrix4f* pMatScale)
{
	if(!pRotMat && !pScale && !pMatScale)
		return;
	__m128 m0 = _mm_loadu_ps(pSrcMat->m[0]);
	__m128 m1 = _mm_loadu_ps(pSrcMat->m[1]);
	__m128 m2 = _mm_loadu_ps(pSrcMat->m[2]);

	__m128 q0 = _mm_div_ps(m0,_mm_sqrt_ps(_mm_dp_ps(m0,m0,0xff)));
	__m128 q1 = _mm_sub_ps(m1 , _mm_dp_ps(q0,m1,0xff));
	q1 = _mm_div_ps(q1,_mm_sqrt_ps(_mm_dp_ps(q1,q1,0xff)));

	__m128 q2 = _mm_sub_ps(_mm_sub_ps(m2 , _mm_mul_ps(q0,_mm_dp_ps(q0,m2,0xff))),_mm_mul_ps(q1,_mm_dp_ps(q1,m2,0xff)));
	q2 = _mm_div_ps(q2,_mm_sqrt_ps(_mm_dp_ps(q2,q2,0xff)));

	//cal det
	__m128 detq0 = q0;
	__m128 detq1 = _mm_shuffle_ps(q1,q1,_MM_SHUFFLE(3,0,2,1));
	__m128 detq2 = _mm_shuffle_ps(q2,q2,_MM_SHUFFLE(3,1,0,2));
	__m128 detq3 = _mm_shuffle_ps(q1,q1,_MM_SHUFFLE(3,1,0,2));
	__m128 detq4 = _mm_shuffle_ps(q2,q2,_MM_SHUFFLE(3,0,2,1));

	__m128 detAdd = _mm_mul_ps(detq0,_mm_mul_ps(detq1,detq2));
	__m128 detSub = _mm_mul_ps(detq0,_mm_mul_ps(detq3,detq4));

	__m128 ret1 = _mm_sub_ps(detAdd , detSub);
	__m128 ret2 = _mm_shuffle_ps(ret1,ret1,_MM_SHUFFLE(3,3,3,1));
	__m128 ret3 = _mm_shuffle_ps(ret1,ret1,_MM_SHUFFLE(3,3,3,2));
	ret1 = _mm_add_ss(ret1,_mm_add_ss(ret2,ret3));
	//float fDet = 1;
	//_mm_store_ss(&fDet,ret1);
	if(_mm_comilt_ss(ret1,_mm_setzero_ps()))
	{
		__m128 sign = _mm_set_ps1(-1);
		q0 = _mm_mul_ps(q0,sign);
		q1 = _mm_mul_ps(q1,sign);
		q2 = _mm_mul_ps(q2,sign);
	}

	if(pScale)
	{
		_mm_store_ss(&pScale->x,_mm_dp_ps(q0,m0,0xff));
		_mm_store_ss(&pScale->y,_mm_dp_ps(q1,m1,0xff));
		_mm_store_ss(&pScale->z,_mm_dp_ps(q2,m2,0xff));
	}
	if(pMatScale)
	{
		__m128 q3 = _mm_set_ps(1,0,0,0);
		_mm_storeu_ps(pMatScale->m[0],_mm_dp_ps(q0,m0,0xf1));
		_mm_storeu_ps(pMatScale->m[1],_mm_dp_ps(q1,m1,0xf2));
		_mm_storeu_ps(pMatScale->m[2],_mm_dp_ps(q2,m2,0xf4));
		_mm_storeu_ps(pMatScale->m[3],q3);
	}
	if(pRotMat)
	{
		__m128 q3 = _mm_set_ps(1,0,0,0);
		_mm_storeu_ps(pRotMat->m[0],q0);
		_mm_storeu_ps(pRotMat->m[1],q1);
		_mm_storeu_ps(pRotMat->m[2],q2);
		_mm_storeu_ps(pRotMat->m[3],q3);
	}
}

void __cdecl matrix4f_decompose( const matrix4f* pSrcMat,float3* pScale,float3* pAngle,float3* pPos )
{
	//int i,j;  
	matrix4f matRot;
	GetMatrixScaleRot(pSrcMat,pAngle?&matRot:NULL,pScale,NULL);
	if(pAngle)
		matrix4f_toeuler_lyxz(&matRot,&pAngle->y,&pAngle->x,&pAngle->z);
	if (pPos)
		float3Set(pPos,pSrcMat->_41,pSrcMat->_42,pSrcMat->_43);
}

BASEGRAPH_COMMON_API void calc_trans_vector_tomatrix(matrix4f & pvmatrix, const float3 & vCenter, const float3 & vScale, const float3 & vAngle, const float3 & vPos, int nRotateOrder)
{
	calc_trans_vector_tomatrix(&pvmatrix, &vPos, &vScale, &vAngle, &vCenter, NULL, NULL, nRotateOrder);
}

BASEGRAPH_COMMON_API void calc_trans_vector_tomatrix(matrix4f * pvmatrix, const vector3f * pvpos, const vector3f * pvscale, const vector3f * pqangle, const vector3f * pvcenter, const matrix4f * pbasevmatrix, const matrix4f * prot_adjust, int nRotateOrder)
{
	matrix4f mattrans, matrot, matscale, matcenterneg;
	matrix4f_from_pos(&matcenterneg, -pvcenter->x, -pvcenter->y, -pvcenter->z);
	float3ToScaleMatQ(&matscale, pvscale);
	matrix4f_from_euler(&matrot, pqangle->x, pqangle->y, pqangle->z, nRotateOrder);
	float3ToTransMatQ(&mattrans, pvpos);

	*pvmatrix = matcenterneg * matscale * matrot;
	if (prot_adjust)
		*pvmatrix *= *prot_adjust;

	if (pbasevmatrix)
		*pvmatrix *= *pbasevmatrix;
	*pvmatrix *= mattrans;
}

BASEGRAPH_COMMON_API void calc_trans_vector_tomatrix(matrix4f * pvmatrix, vector3f * pvpos, vector3f * pvscale, SYQuatn * pqrotate, vector3f * pvcenter, matrix4f * pbasevmatrix, matrix4f * prot_adjust)
{
	matrix4f mattrans, matrot, matscale, matcenterneg;
	matrix4f_identity(&matcenterneg);
	matcenterneg._41 = -pvcenter->x;
	matcenterneg._42 = -pvcenter->y;
	matcenterneg._43 = -pvcenter->z;
	float3ToScaleMatQ(&matscale, pvscale);
	SYQuatn_ToMatrix4(&matrot, pqrotate);
	float3ToTransMatQ(&mattrans, pvpos);
	*pvmatrix = matcenterneg * matscale * matrot;
	if (prot_adjust)
		*pvmatrix *= *prot_adjust;

	if (pbasevmatrix)
		*pvmatrix *= *pbasevmatrix;
	*pvmatrix *= mattrans;
}

void __cdecl matrix4f_issolvesrp(const matrix4f* pM,matrix4f* pScale,matrix4f* pRotate,matrix4f* pTranslate)
{
	if (pTranslate != 0)
		matrix4f_from_pos(pTranslate,pM->_41,pM->_42,pM->_43);
	GetMatrixScaleRot(pM,pRotate,NULL,pScale);
}

void __cdecl matrix4f_BetweenVectors( pmatrix4f pDesM, /* 目标矩阵指针 */ const vector3f* pSrcV, /* 源向量指针 */ const vector3f* pDstV/* 源向量指针 */ )
{
	float3 vAxis;
	float fSign = float3DotProduct(pSrcV,pDstV);
	if(fSign - 1 < 0.0001 && fSign -1 > -0.0001 )
	{
		matrix4f_identity(pDesM);
		return;
	}
	else if(fSign < 0.0001 -1 && fSign > -0.0001+1 )
	{
		vAxis.x = pSrcV->y;
		vAxis.y = -pSrcV->x;
		vAxis.z = pSrcV->z;
		matrix4f_fromrotvector(pDesM,&vAxis,PAIF);
		return;
	}

	float3CrossProduct(&vAxis,pSrcV,pDstV);
	float3Normalize(&vAxis,&vAxis);
	float fAngle = (float)float3AngleBetVec(pSrcV,pDstV,&vAxis);
	matrix4f_fromrotvector(pDesM,&vAxis,fAngle);
}

void __cdecl matrix4f_to_euler( matrix4f* pDesM,float* pAngleX,float* pAngleY,float* pAngleZ,int nOrder )
{
	switch(nOrder)
	{
	case ROTATE_ORDER_XYZ:
		matrix4f_toeuler_lxyz(pDesM,pAngleX,pAngleY,pAngleZ);
		break;
	case ROTATE_ORDER_YXZ:
		matrix4f_toeuler_lyxz(pDesM,pAngleY,pAngleX,pAngleZ);
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
// 文件结束
