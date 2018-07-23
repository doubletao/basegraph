cbuffer cb0
{
	float4x4 worldViewProjMat;  
	float4x4 worldMat;  
	float4x4 worldMatInvT;
	float 	fNormalFactor;
};

struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float3 Normal   : NORMAL;
	float2 tex		: TEXCOORD0;
	float3 posWorld	: TEXCOORD1;
	float3 ItemPos  : TEXCOORD2;
	float3 tangent  : TEXCOORD3;
	float3 binormal : TEXCOORD4;
};

struct VS_INPUT
{
	float3 Position : POSITION0;
	float3 Normal   : NORMAL;
	float2 tex		: TEXCOORD0;
	float3 tangent  : TEXCOORD1;
	float3 binormal : TEXCOORD2;
};

VS_OUTPUT main(in VS_INPUT vIN) 
{
	VS_OUTPUT OutPut;
	OutPut.Position = mul(worldViewProjMat,float4(vIN.Position,1));
	OutPut.posWorld = mul(worldMat,float4(vIN.Position,1)).xyz;
	OutPut.Normal = normalize(mul((float3x3)worldMatInvT,vIN.Normal * fNormalFactor));
	OutPut.tangent = normalize(mul((float3x3)worldMatInvT,vIN.tangent));
	OutPut.binormal = normalize(mul((float3x3)worldMatInvT,vIN.binormal));
	OutPut.tex = vIN.tex;
	OutPut.ItemPos = vIN.Position;
	return OutPut;
}

