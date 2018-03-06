cbuffer cb0
{
	float4x4 viewMat;  
};

struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float4 color		 : COLOR0;
};

struct VS_INPUT
{
	float3 Position : POSITION0;
	float4 color		 : COLOR0;
	float2 tex      : TEXCOORD0;
};

VS_OUTPUT main(in VS_INPUT vIN) 
{
	VS_OUTPUT OutPut;
	OutPut.Position = mul(viewMat,float4(vIN.Position,1));
	OutPut.color = vIN.color;
	return OutPut;
}
