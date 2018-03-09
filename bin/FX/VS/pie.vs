
cbuffer cb0: register(b0)
{
	float4x4	g_world;
	float4x4	g_view_proj;
	float4x4	g_world_InvT;
	float		g_progress;
	float		g_thick;
	float		g_normalFactor;
	float		undefine;
};

float ABS_COLOR_0 = 0.0001f;

struct VS_INPUT_PIE
{
	float3 pos			: POSITION;
	float3 normal		: NORMAL;
	float2 tex_uv		: TEXCOORD0;
	float3 thick		: TEXCOORD1;
	float3 radius		: TEXCOORD2;
};

struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float3 Normal   : NORMAL;
	float2 tex_uv	: TEXCOORD0;
	float3 posWorld	: TEXCOORD1;
	float3 ItemPos  : TEXCOORD2;
	float3 tangent  : TEXCOORD3;
	float3 binormal : TEXCOORD4;
};

/*
VS_OUTPUT main(VS_INPUT_PIE Input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0;

	
	float4 pos_os	= float4(Input.pos.xy*200,Input.pos.z,1);
	float3 normal_os = Input.normal;
	float4 pos_ws = mul(g_world,pos_os);

	Output.Position		= mul(g_view_proj,pos_ws);
	Output.Normal		= normal_os;
	Output.tex_uv		= Input.tex_uv;
	Output.posWorld		= pos_ws.rgb;
	Output.ItemPos		= pos_os.rgb;
	Output.tangent  	= float3(1,0,0);
	Output.binormal 	= float3(0,1,0);
		
	return Output;
}
*/


VS_OUTPUT main(VS_INPUT_PIE Input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0;
	
	float r,a,c,s;

	r = Input.radius.x;
	a = Input.pos.x + (Input.pos.y - Input.pos.x)*g_progress;
	
	sincos(a,s,c);
	
	float4 pos_os = float4(r*c,r*s,Input.pos.z,1);

	if(Input.thick.x > ABS_COLOR_0) //厚度会变化
	{
		pos_os.z = -g_thick + Input.thick.y;
	}
	
	//pos_os.z = 0;
	
	float3 normal_os = Input.normal;

	if(abs(Input.radius.y - 6)<ABS_COLOR_0) //变化法线计算
	{
		
		matrix<float, 3, 3> rot_matrix  = { c, -s,0, // row 1
											s, c, 0, // row 2
											0, 0, 1.0f, // row 3
										  };
	
		normal_os = mul(normal_os,rot_matrix);
		
	}
	
	float4	 pos_ws	= mul(g_world,pos_os);
	float3	 normal_ws;

	normal_os  = normal_os *g_normalFactor;
	normal_ws  = normalize(mul((float3x3)g_world_InvT,normal_os)); 

	Output.Position		= mul(g_view_proj,pos_ws);
	
	Output.Normal		= normal_ws;
	Output.tex_uv		= Input.tex_uv;
	Output.posWorld		= pos_ws.rgb;
	Output.ItemPos		= pos_os.rgb;
	Output.tangent  	= float3(1,0,0);
	Output.binormal 	= float3(0,1,0);
		
	return Output;
}

