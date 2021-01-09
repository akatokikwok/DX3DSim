cbuffer CBuf
{
    float4 color;
};
// 像素着色器只拿颜色
float4 main() : SV_Target
{
    return color;
}