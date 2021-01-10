cbuffer CBuf
{
	//matrix model;
	
    matrix modelView; // 现在更正为modelview,以便之后的VSOut结构体worldpos不再是世界坐标而是相对于摄像机的坐标
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
    vso.worldPos = (float3) mul(float4(pos, 1.0f), modelView);	// (相对于摄像机的坐标情况下) 世界位置等于 参数pos乘以modelView
    vso.normal = mul(n, (float3x3) modelView);					// (相对于摄像机的坐标情况下) 法线位置等于 参数noraml乘以modelView
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);			//  位置    等于 参数pos乘以MVP矩阵	
	return vso;
}