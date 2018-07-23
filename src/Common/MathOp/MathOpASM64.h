#ifndef __SYCOMMON_ASM_64_H__
#define __SYCOMMON_ASM_64_H__

#include "Common\MatrixDef.h"
#include "Common\PlaneDef.h"
extern "C" void rectf_scale_x64(rectf* rt,float fscale);
//matrix
extern "C" void matrix4_negative_x64(pmatrix4f pDesM, pmatrix4f pSrcM);
extern "C" void matrix4_addnum_x64(pmatrix4f pDesM, const matrix4f* pSrcM, float k);
extern "C" void matrix4_mulnum_x64(pmatrix4f pDesM, pmatrix4f pSrcM, float k);
extern "C" void matrix4_add_x64(pmatrix4f pDesM, pmatrix4f pSrcML, pmatrix4f pSrcMR);
extern "C" void matrix4_minus_x64(pmatrix4f pDesM, pmatrix4f pSrcML, pmatrix4f pSrcMR);
extern "C" void matrix4_mul_x64(pmatrix4f pDesM, const matrix4f* pSrcML, const matrix4f*  pSrcMR);
extern "C" void matrix4_fromquatn_x64(pmatrix4f pDesM, const SYQuatn* pSrcQ, float fOne);
extern "C" void matrix4_fromrotvector_x64(pmatrix4f pDesM, const float3* pSrcV, float fAngle, float fCoefCos, float fCoefSin, float fOne);
extern "C" void matrix4_fromeuler_lyxz_x64(pmatrix4f pDesM,  float fCosX, float fSinX, float fCosY, float fSinY, float fCosZ, float fSinZ, float fOne);
extern "C" void  matrix4_transpose_x64(pmatrix4f pDesM, const matrix4f* pSrcM);

//quatn
extern "C" void			_quatnNegtive_x64(SYQuatn* pQ);
extern "C" void			_quatnAdd_x64(SYQuatn* pDesQ,	
						 const SYQuatn* pSrcQL,	
						 const SYQuatn* pSrcQR);
extern "C" void _quatnMinus_x64(SYQuatn* pDesQ,		
						   const SYQuatn* pSrcQL,	
						   const SYQuatn* pSrcQR);		

extern "C" void _quatnStandardMul_x64(SYQuatn* pDesQ,		
								 const SYQuatn* pSrcQL,	
								 const SYQuatn* pSrcQR);
extern "C" void _quatnCustomMul_x64(SYQuatn* pDesQ,		
							   const SYQuatn* pSrcQL,	
							   const SYQuatn* pSrcQR);	
extern "C" void _quatnNormalize_x64(SYQuatn* pDesQ,	
							   const SYQuatn* pSrcQ);	
extern "C" void _quatnScale_x64(SYQuatn* pQ,	
						   float k);		
extern "C" void _quatnMulNum_x64(SYQuatn* pDesQ,		
							const SYQuatn* pSrcQ,			
							float k);				
extern "C" void _quatnMode_x64(float* pfRes,const SYQuatn* pQ);			
extern "C" void _quatnDotProduct_x64(float* pfRes, const SYQuatn* pQ1,	
								 const SYQuatn* pQ2);	
extern "C" void _quatnSlerp_x64(SYQuatn* pDesQ,const SYQuatn* pSrcQ0,const SYQuatn* pSrcQ1,float k0,float k1);	
extern "C" void _quatnFromAngleAxis_x64(SYQuatn* pDesQ, const float3* pSrcV, float fCosCoef,float fSinCoef);
extern "C" void _quatnFromEuler_LYXZ_x64(SYQuatn* pDesQ,
									float fCosX,
									float fSinX,
									float fCosY,
									float fSinY,
									float fCosZ,
									float fSinZ);
extern "C" void _quatnMulMatQ_x64(SYQuatn* pDesQ,
							 const SYQuatn* pSrcQ,
							 const matrix4f* pSrcM);
extern "C" void			float2MulMatQ_x64(float2* pDesV,			// 目标向量指针
										  const float2* pSrcV,			// 源向量指针
										  const matrix4f* pSrcM);			// 矩阵指针
extern "C" void			float3Add_x64(float3* pDesV,		// 目标向量
									  const float3* pSrcV1,		// 向量 1
									  const float3* pSrcV2);		// 向量 2

extern "C" void			float3Sub_x64(float3* pDesV,		// 目标向量
									  const float3* pSrcVL,		// 左向量
									  const float3* pSrcVR);		// 右向量

extern "C" void			float3AddNum_x64(const float3* pDesV,	// 目标向量
										 const float3* pSrcV,	// 向量
										 float k);				// 数
extern "C" void			float3Offset_x64(const float3* pV,	// 向量
										 float k);			// 数

extern "C" void			float3Scale_x64(float3* pV,			// 向量指针
										float k);						// 缩放倍数

extern "C" void			float3MulNum_x64(float3* pDesV,		// 目标向量指针
										 const float3* pSrcV,		// 源向量指针
										 float k);					// 缩放倍数
extern "C" void			float3Normalize_x64(float3* pDesV,			// 目标向量指针
											const float3* pSrcV);			// 源向量指针

extern "C" float		float3Mode_x64(const float3* pV);			// 向量指针

extern "C" float		float3DotProduct_x64(const float3* pV1,		// 向量指针
											 const float3* pV2);		// 向量指针

extern "C" void			float3CrossProduct_x64(float3* pDesV,		// 目标向量指针
											   const float3* pSrcV1,		// 源向量指针
											   const float3* pSrcV2);		// 源向量指针

extern "C" void			float3NormalMulMat_x64(float3* pDesV,			// 目标向量指针
											   const float3* pSrcV,			// 源向量指针
											   const matrix4f* pSrcM);				// 矩阵指针

extern "C" void			float3MulMatQ_x64(float3* pDesV,			// 目标向量指针
										  const float3* pSrcV,			// 源向量指针
										  const matrix4f* pSrcM);			// 矩阵指针

extern "C" void			float4Add_x64(float4* pDesV,		// 目标向量
									  const float4* pSrcV1,		// 向量 1
									  const float4* pSrcV2);		// 向量 2

extern "C" void			float4Sub_x64(float4* pDesV,		// 目标向量
									  const float4* pSrcVL,		// 左向量
									  const float4* pSrcVR);		// 右向量

extern "C" void			float4AddNum_x64(float4* pDesV,	// 目标向量
										 const float4* pSrcV,	// 向量
										 float k);				// 数
extern "C" void			float4Offset_x64(float4* pV,	// 向量
										 float k);			// 数
extern "C" void			float4Scale_x64(float4* pV,			// 向量指针
										float k);						// 缩放倍数

extern "C" void			float4MulNum_x64(float4* pDesV,		// 目标向量指针
										 const float4* pSrcV,		// 源向量指针
										 float k);					// 缩放倍数
extern "C" void			float4Normalize_x64(float4* pDesV,			// 目标向量指针
											const float4* pSrcV);			// 源向量指针
extern "C" float		float4Mode_x64(const float4* pV);			// 向量指针

extern "C" float		float4DotProduct_x64(const float4* pV1,		// 向量指针
											 const float4* pV2);		// 向量指针
extern "C" void			float4MulMatQ_x64(float4* pDesV,			// 目标向量指针
										  const float4* pSrcV,			// 源向量指针
										  const matrix4f* pSrcM);			// 矩阵指针

extern "C" void			float4MatQMul_x64(float4* pDesV,			// 目标向量指针
										  const matrix4f* pSrcM,			// 矩阵指针
										  const float4* pSrcV);			// 源向量指针
#endif
