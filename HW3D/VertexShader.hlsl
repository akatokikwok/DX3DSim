struct VSOut
{
    /* PS �������Ǵ��ڼĴ����ϵ���˳��,��������Color��0��,����������HLSL��Ҳ����0�Ŷ���*/
    
    float3 color : Color;//��ά�������ɫ
    float4 pos : SV_Position;//��ά�����λ��
    
};


VSOut main(float2 pos : Position, float3 color : Color) 
{
    VSOut vso;
    vso.pos = float4(pos.x, pos.y, 0.0f, 1.0f);
    vso.color = color;
    
    return vso;
}