cbuffer CBuf
{
	matrix transform;
};

struct VSOut
{
	float2 tex : TexCoord;
	float4 pos : SV_Position;
};

// 为了处理纹理,参数里新增1个纹理坐标
VSOut main( float3 pos : Position,float2 tex : TexCoord )
{
	VSOut vso;
	vso.pos = mul( float4(pos,1.0f),transform );
	vso.tex = tex;
	return vso;
}