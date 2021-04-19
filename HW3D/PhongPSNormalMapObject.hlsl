/* 不带切线TB的 像素着色器(法线贴图版)*/

#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

#include "PointLight.hlsl" //注意这里的PointLight.hlsl对应的就是常量缓存灯泡
//cbuffer LightCBuf       //---[0]
//{
//    float3 lightPos;
//    float3 ambient;
//    float3 diffuseColor;
//    float diffuseIntensity;
//    float attConst;
//    float attLin;
//    float attQuad;
//};

cbuffer ObjectCBuf      //---[1]
{
    float specularIntensity;
    float specularPower;
    bool normalMapEnabled;
    float padding[1];
};

#include "Transform.hlsl" //注意这个头文件不能放到上面，不然着色器识别常数缓存的顺序会出错！导致不能调试！！！！
//cbuffer TransformCBuf   //---[2]
//{
//    matrix modelView;
//    matrix modelViewProj;
//};

///=================================================
///所以上述所有Pixel ConstantBuffer 顺序是:
//  光源---[0]
//  渲染对象上的材质---[1]
//  Transform---[2]


Texture2D tex;
Texture2D nmap : register(t2);

SamplerState splr;


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord) : SV_Target
{
	// sample normal from map if normal mapping enabled
    if (normalMapEnabled)//注意！！若启用法线贴图,则会将法线一直在z方向映射，所以要在z分量上做一些处理
    {
        // unpack normal data
        const float3 normalSample = nmap.Sample(splr, tc).xyz;  
        
        /*
        因为一个向量每个维度的取值范围在(-1, 1)，而纹理每个通道的值范围在(0, 1)，
        因此我们需要做一个映射，即pixel = (normal + 1) / 2。即 对象空间的法线 == 2 * 采样后的纹理 - 1
        这样，之前的法线值(0, 0, 1)实际上对应了法线纹理中RGB的值为(0.5, 0.5, 1)，而这个颜色也就是法线纹理中那大片的蓝色。
        这些蓝色实际上说明顶点的大部分法线是和模型本身法线一样的，不需要改变。
        总结一下就是，
        法线纹理的RGB通道存储了在每个顶点各自的Tangent Space中的法线方向的映射值。
        */
        
        const float3 objectNormal = normalSample * 2.0f - 1.0f;// 拿到对象空间的法线
        viewNormal = normalize(mul(objectNormal, (float3x3) modelView)); //把法线从object space 转换到 view space同时归一化
    }    
    
    // fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);    
	// attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
    // diffuse intensity
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
	// specular
    const float3 specular = Speculate(
        specularIntensity.rrr, 1.0f, viewNormal, lv.vToL,
        viewFragPos, att, specularPower
    );
    
    // final color
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}