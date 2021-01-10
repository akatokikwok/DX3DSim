cbuffer CBuf
{
    float4 color;
};
// µÆÅÝshader
float4 main() : SV_Target
{
    return color;
}
