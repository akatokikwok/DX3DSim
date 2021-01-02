struct VSOut
{
    /* PS 变量都是存在寄存器上的有顺序,比如这里Color是0号,所以在像素HLSL里也能与0号对上*/
    
    float3 color : Color;//三维浮点的颜色
    float4 pos : SV_Position;//四维浮点的位置
    
};


VSOut main(float2 pos : Position, float3 color : Color) 
{
    VSOut vso;
    vso.pos = float4(pos.x, pos.y, 0.0f, 1.0f);
    vso.color = color;
    
    return vso;
}