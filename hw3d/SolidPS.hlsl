cbuffer CBuf
{
    float4 color;
};
// ������ɫ��ֻ����ɫ
float4 main() : SV_Target
{
    return color;
}