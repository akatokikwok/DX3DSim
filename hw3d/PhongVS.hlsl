cbuffer CBuf
{
	matrix model;
	matrix modelViewProj;
};

struct VSOut
{
	float3 worldPos : Position;
	float3 normal : Normal;
	float4 pos : SV_Position;
};

VSOut main( float3 pos : Position,float3 n : Normal )
{
	VSOut vso;
	vso.worldPos = (float3)mul( float4(pos,1.0f),model );// 世界位置等于 参数pos乘以模型矩阵
	vso.normal = mul( n,(float3x3)model );				 // 法线位置等于 参数noraml乘以模型矩阵
	vso.pos = mul( float4(pos,1.0f),modelViewProj );	 // 位置    等于 参数pos乘以MVP矩阵	
	return vso;
}