#pragma once

//二维向量
typedef struct tag_int2 {		// 整数型
	int	x;
	int	y;
}int2, point2, vector2, *LPPoint2;

typedef struct tag_float2 {		// 浮点型
	float	x;
	float	y;
}float2, point2f, vector2f, *LPPoint2F;

//三维向量
typedef struct tag_int3 {		// 整数型
	int	x;
	int	y;
	int	z;
}int3, point3, vector3, *LPPoint3;

typedef struct tag_float3 {		// 浮点型
	float	x;
	float	y;
	float	z;
}float3, point3f, vector3f, *LPPoint3F;

//四维向量
typedef struct tag_int4 {		// 整数型
	float	x;
	float	y;
	float	z;
	float	w;
}int4, point4, vector4, *LPPoint4;

typedef struct tag_float4 {		// 浮点型
	float	x;
	float	y;
	float	z;
	float	w;
}float4, point4f, vector4f, *LPPoint4F;