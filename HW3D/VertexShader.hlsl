//struct VSOut
//{
//    /* PS 变量都是存在寄存器上的有顺序,比如这里Color是0号,所以在像素HLSL里也能与0号对上*/
    
//    float3 color : Color;//三维浮点的颜色
//    float4 pos : SV_Position;//四维浮点的位置
    
//};

//常量缓存要保持和C++的一致
cbuffer CBuf
{
    //row_major关键字告诉HLSL这是个行矩阵,所以在此一步来进行转置;因为
    //矩阵在CPU里按二维数组进行存储而在GPU里是按列存储的； 所以旋转矩阵必须使用转置
    
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