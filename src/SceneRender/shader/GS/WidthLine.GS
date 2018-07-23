cbuffer cb0
{
	float fLineWidth;
	float fWHRadio;
	float2 noUse;
};

struct GS_OUTPUT
{
	float4 Position : SV_Position;
	float4 color		 : COLOR0;
	float2	tex		: TEXCOORD0;
};

struct GS_INPUT
{
	float4 Position : POSITION0;
	float4 color		 : COLOR0;
};
#define COUNT 7

[maxvertexcount(44)]
void main(line GS_INPUT vIN[2],inout TriangleStream<GS_OUTPUT> TriangleOutputStream) 
{
	float2 lineRadio = vIN[1].Position.xy - vIN[0].Position.xy;
	lineRadio.y /= fWHRadio;
	lineRadio = normalize(lineRadio);
	float2 delt;
	delt.x = lineRadio.y * fLineWidth;
	delt.y = -lineRadio.x * fLineWidth * fWHRadio ;

	GS_OUTPUT newVertex;
	newVertex.color = vIN[0].color;
	newVertex.Position = vIN[0].Position ;
	newVertex.Position.xy -= delt * vIN[0].Position.w;
	newVertex.tex.x = 0;
	newVertex.tex.y = 0;
	TriangleOutputStream.Append(newVertex);

	newVertex.color = vIN[1].color;
	newVertex.Position = vIN[1].Position ;
	newVertex.Position.xy -= delt* vIN[1].Position.w;
	newVertex.tex.x = 1;
	newVertex.tex.y = 0;
	TriangleOutputStream.Append(newVertex);
	
	newVertex.color = vIN[0].color;
	newVertex.Position = vIN[0].Position ;
	newVertex.Position.xy += delt* vIN[0].Position.w;
	newVertex.tex.x = 0;
	newVertex.tex.y = 1;
	TriangleOutputStream.Append(newVertex);

	newVertex.color = vIN[1].color;
	newVertex.Position = vIN[1].Position ;
	newVertex.Position.xy += delt* vIN[1].Position.w;
	newVertex.tex.x = 1;
	newVertex.tex.y = 1;
	TriangleOutputStream.Append(newVertex);
	TriangleOutputStream.RestartStrip(); 

	for(int j = 0; j < 1; j++)
	{
		newVertex.color = vIN[j].color;
		newVertex.tex.x = j;
		newVertex.tex.y = 0;

		GS_OUTPUT Center;
		Center.Position = vIN[j].Position;
		Center.color = vIN[j].color;
		Center.tex = float2(0,0.5);
		lineRadio = -lineRadio;
		for(int i = 0; i < COUNT; i++)
		{
			float alpha = -1.57079 + i * 3.1415926 /(COUNT -1)  ;
			float4 circle = 0;
			circle.x =  fLineWidth * ( lineRadio.x * cos(alpha) - lineRadio.y * sin(alpha) );
			circle.y =  fLineWidth * ( lineRadio.x * sin(alpha) + lineRadio.y * cos(alpha))  * fWHRadio;
			circle *= vIN[j].Position.w;
			circle = vIN[j].Position + circle;
			newVertex.Position = circle;
			
			newVertex.tex.x = 0;

			TriangleOutputStream.Append(newVertex);
			TriangleOutputStream.Append(Center);
		}
		TriangleOutputStream.RestartStrip();
	}
}
