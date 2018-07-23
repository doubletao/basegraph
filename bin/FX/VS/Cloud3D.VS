cbuffer cb0
{
	float2 xyScale;//点象素和经纬度的换算，一个象素点对应经纬度多少
	float2 xyOffset;//经纬度偏移值
	float2 widthHeight;
	float Radius;
	float CloudHigh;
	float CloudDepth;
	float4x4 WorldViewProjMat; 
};

struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float4 color    : COLOR0;
};
struct VS_INPUT
{
	float2 tex      : TEXCOORD0;
};
Texture2D <float4> tex;

VS_OUTPUT main(in VS_INPUT vIN) 
{
	VS_OUTPUT OutPut;
	float4 vPos;
	float2 fLonLat = vIN.tex * xyScale + xyOffset;
	
	OutPut.color = tex.Load(int3(vIN.tex*widthHeight,0));

//	float constA = 3.14159265358979 / 180;
//	float dLaAngle = fLonLat.y * constA;
//	float dLoAngle = fLonLat.x * constA + 3.14159265358979; 
	float dLaAngle = fLonLat.y ;
	float dLoAngle = fLonLat.x + 3.14159265358979;
	float cosTh = cos(dLaAngle);
	int r = (OutPut.color.x * 255 - 180) * CloudDepth + CloudHigh + Radius;
	//int r = Radius + CloudHigh;
	int R = r * cosTh;
	vPos.x = R * cos(dLoAngle);
    vPos.y = r * sin(dLaAngle);
	vPos.z = R * sin(dLoAngle);
	vPos.w = 1.0;

	OutPut.Position = mul(WorldViewProjMat,vPos);
	return OutPut;
}
