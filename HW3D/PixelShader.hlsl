//颜色也是四维浮点数
float4 main(float3 color : Color) : SV_Target
{
    //return float4(1.0f, 1.0f, 1.0f, 1.0f);
    return float4(color,1);
}
