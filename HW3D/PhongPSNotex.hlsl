/* 不带纹理的像素shader*/

//cbuffer LightCBuf
//{
//    float3 lightPos;
//    float3 ambient;
//    float3 diffuseColor;
//    float diffuseIntensity;
//    float attConst;
//    float attLin;
//    float attQuad;
//};

#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

#include "PointLight.hlsl"

cbuffer ObjectCBuf
{
    float4 materialColor; //漫反射材质颜色;用float4，因为方便与CPU对齐
    //float specularIntensity;
    float4 specularColor; // 镜面反射颜色;用float4，因为方便与CPU对齐
    float specularPower;
    //float padding[2];
};


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal) : SV_Target
{
    // renormalize the mesh normal
    viewNormal = normalize(viewNormal);
    
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
    //const float3 vToL = lightPos - viewPos;
    //const float distToL = length(vToL);
    //const float3 dirToL = vToL / distToL;
    
    // ==============================================================================
    
    // attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
	// diffuse
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
    // specular
    const float3 specular = Speculate(
        specularColor.rgb, 1.0f, viewNormal,
        lv.vToL, viewFragPos, att, specularPower
    );
    
	// final color
    //return float4(saturate((diffuse + ambient) * materialColor.rgb + specular), 1.0f);
    //return saturate( float4(diffuse + ambient, 1.0f) * materialColor + specular);
    return float4(saturate((diffuse + ambient) * materialColor.rgb + specular), 1.0f);
    // 即 (漫反射光+环境光) * 漫反射贴图(或是漫反射材质颜色)  + 高光
}