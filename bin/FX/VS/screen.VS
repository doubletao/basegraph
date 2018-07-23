cbuffer cb0
{
	//x u方向偏移
	//y v方向偏移
	//z u方向缩放
	//w v方向缩放
	float4 texOffset;  
	float4x4 worldMat;
};

struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float2 tex : TEXCOORD0;
	float2 OrginTex : TEXCOORD1;
};

struct VS_INPUT
{
	float3 Position : POSITION0;
	float2 tex		 : TEXCOORD0;
};

VS_OUTPUT main(in VS_INPUT vIN) 
{
	VS_OUTPUT OutPut;
	OutPut.Position = mul(worldMat,float4(vIN.Position,1));
	OutPut.tex = (vIN.tex * texOffset.zw) + texOffset.xy;
	OutPut.OrginTex = vIN.tex;
	return OutPut;
}
