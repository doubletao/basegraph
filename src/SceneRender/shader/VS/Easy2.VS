cbuffer cb0
{
	float4x4 viewMat;
	float4x4 projMat;
};

struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float4 color	: COLOR0;
	float  fog_param: TEXCOORD0;
};

struct VS_INPUT
{
	float3 Position : POSITION0;
	float4 color	: COLOR0;
	float2 tex      : TEXCOORD0;
};

VS_OUTPUT main(in VS_INPUT vIN) 
{
	VS_OUTPUT OutPut;
	
	OutPut.Position = mul(viewMat,float4(vIN.Position,1));
	OutPut.fog_param = OutPut.Position.z;
	
	OutPut.Position = mul(projMat,OutPut.Position);
	OutPut.color = vIN.color;
		
	return OutPut;
}
