cbuffer CBuf
{
    float4 color;
};
// ����shader
float4 main() : SV_Target
{
    return color;
}
