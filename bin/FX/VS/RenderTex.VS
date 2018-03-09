struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float2 tex : TEXCOORD0;
};

cbuffer cb0
{
	float4x4 worldMat;
};

VS_OUTPUT main(in float3 pos: POSITION0) 
{
	VS_OUTPUT OutPut;
	OutPut.Position = mul(worldMat,float4(pos,1));
	OutPut.tex = float2(pos.x + 0.5f,0.5f-pos.y);
	
	return OutPut;
}
