
// 常数--光照
cbuffer PointLightCBuf           //[0]
{
    float3 viewLightPos; //光源位置
    float3 ambient; //环境光常数  
    float3 diffuseColor; //漫反射颜色常数
    float diffuseIntensity; //漫反射功率常数
    
    float attConst;
    float attLin;
    float attQuad;
};