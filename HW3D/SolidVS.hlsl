//cbuffer CBuf
//{
//	matrix model;
//	matrix modelViewProj;
//};
#include "Transform.hlsl"

float4 main( float3 pos : Position ) : SV_Position
{
	return mul( float4(pos,1.0f),modelViewProj );// 参数pos乘以mvp位置
}