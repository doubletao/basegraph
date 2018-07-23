cbuffer cb0
{
	float4x4 worldViewProjMat;  
	float4x4 worldMat;  
};

struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float3 Normal   : NORMAL;
	float2 tex		: TEXCOORD0;
	float3 posWorld	: TEXCOORD1;
	float3 ItemPos  : TEXCOORD2;
};

struct VS_INPUT
{
	float3 Position : POSITION0;
	float3 Normal   : NORMAL;
	float2 tex		: TEXCOORD0;
};

VS_OUTPUT main(in VS_INPUT vIN) 
{
	VS_OUTPUT OutPut;
	OutPut.Position = mul(worldViewProjMat,float4(vIN.Position,1));
	OutPut.posWorld = mul(worldMat,float4(vIN.Position,1)).xyz;
	OutPut.Normal = normalize(mul((float3x3)worldMat,vIN.Normal));
	OutPut.tex = vIN.tex;
	OutPut.ItemPos = vIN.Position;
	return OutPut;
}

