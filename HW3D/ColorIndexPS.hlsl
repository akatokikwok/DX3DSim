//��ɫҲ����ά������
//float4 main(float3 color : Color) : SV_Target
//{
//    //return float4(1.0f, 1.0f, 1.0f, 1.0f);
//    return float4(color,1);
//}

cbuffer CBuf
{
    float4 face_colors[8];
};

float4 main(uint tid : SV_PrimitiveID) : SV_Target
{
    return face_colors[(tid / 2) % 8];//��8ȡ��, �Ͳ��ᳬ��8λ��ɫ����

}