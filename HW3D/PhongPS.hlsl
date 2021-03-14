/// 只有漫反射的 pixel shader

// 光照常数应该在插槽0
//cbuffer LightCBuf
//{
//	float3 lightPos;
	
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

// 绘制物常数应该在插槽1
cbuffer ObjectCBuf
{
	// 材质应该独立出光照,材质颜色是由物体本身决定的
    //float3 materialColor;
	
    float specularIntensity;// 镜面光
    float specularPower;// 镜面级数
	
	float padding[2];
};

Texture2D tex;
SamplerState splr;


// 带法线的着色器,用于接受光源的照射;供给给那些承受光照的绘制物使用
float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord) : SV_Target
{
	// renormalize interpolated normal
    viewNormal = normalize(viewNormal);
	
	// fragment to light vector data
	//const float3 vToL = lightPos - viewPos;/* 注意这里的入参是视图空间*/
	//const float distToL = length( vToL );
	//const float3 dirToL = vToL / distToL;
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
	
	// 衰减
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
	// 漫反射最终呈现效果
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
	// specular
    const float3 specular = Speculate(diffuseColor, diffuseIntensity, viewNormal, lv.vToL, viewFragPos, att, specularPower);
	
	// final color
    //return float4(saturate((diffuse + ambient) * materialColor), 1.0f);
	//return float4(saturate( diffuse + ambient + specular), 1.0f) * tex.Sample(splr, tc); // (环境光+漫反射+反射镜面光)*材质颜色
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f); // 漫反射+环境光 乘以 普通纹理的rgb通道 在加上镜面光
}
