//struct VSOut
//{
//    /* PS �������Ǵ��ڼĴ����ϵ���˳��,��������Color��0��,����������HLSL��Ҳ����0�Ŷ���*/
    
//    float3 color : Color;//��ά�������ɫ
//    float4 pos : SV_Position;//��ά�����λ��
    
//};

//��������Ҫ���ֺ�C++��һ��
cbuffer CBuf
{
    //row_major�ؼ��ָ���HLSL���Ǹ��о���,�����ڴ�һ��������ת��;��Ϊ
    //������CPU�ﰴ��ά������д洢����GPU���ǰ��д洢�ģ� ������ת�������ʹ��ת��
    
    //row_major matrix transform;
    matrix transform;
};

//VSOut main(float2 pos : Position, float3 color : Color) 
//{
//    VSOut vso;
//    vso.pos = mul(float4(pos.x, pos.y, 0.0f, 1.0f), transform);
//    vso.color = color;
    
//    return vso;
//}

float4 main(float3 pos : Position) : SV_Position
{
    return mul(float4(pos, 1.0f), transform);
}