/* 带法线的顶点shader*/

cbuffer CBuf
{
    matrix modelView;
    matrix modelViewProj;
};

struct VSOut
{
    float3 worldPos : Position;
    float3 normal : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 tc : Texcoord;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 tc : Texcoord)
{
    VSOut vso;
    vso.worldPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.normal = mul(n, (float3x3) modelView);
    
    // 将T切线和B切线也带入视图空间
    vso.tan = mul(tan, (float3x3) modelView);
    vso.bitan = mul(bitan, (float3x3) modelView);
    
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.tc = tc;
    return vso;
}