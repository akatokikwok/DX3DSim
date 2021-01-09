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
// 顶点着色器负责更新单顶点的世界位置\法线\位置
VSOut main(float3 pos : Position, float3 n : Normal)
{
    VSOut vso;
    vso.worldPos = (float3) mul(float4(pos, 1.0f), model);// 顶点的世界位置等于 参数pos向量*模型矩阵
    vso.normal = mul(n, (float3x3) model);                // 顶点的法线位置等于 参数法线n向量*模型矩阵 
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);      // 顶点的位置    等于  参数pos*MVP矩阵
    return vso;
}