cbuffer CBuf
{
    matrix model;
    matrix modelViewProj;
};

// 顶点shader等于灯泡位置*MVP
float4 main(float3 pos : Position) : SV_Position
{
    return mul(float4(pos, 1.0f), modelViewProj);
}