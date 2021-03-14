/* 带法线的像素shader*/

//cbuffer LightCBuf// 光源常量;位于[0]
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

cbuffer ObjectCBuf//模型里一些参数常量;位于[1]
{
    float specularIntensity;
    float specularPower;
    bool normalMapEnabled;//法线贴图采样开关
    float padding[1];
};

//cbuffer TransformCBuf // 新增一个T有关模型的常数缓存; 位于[2]
//{
//    matrix modelView;
//    matrix modelViewProj;
//};

Texture2D tex;                  // 约定俗成情况下，一般像素shader里关于纹理插槽[0]放漫反射
/*Texture2D nmap;*/             // 注释掉;这里暂时不使用插槽[1];约定俗成情况下，一般像素shader里关于纹理插槽[1]可能放带镜面光的着色器   
Texture2D nmap : register(t2);  // 约定俗成情况下，一般像素shader里关于纹理插槽[2]可能放法线

SamplerState splr;


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, 
    float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    // normalize the mesh normal
    viewNormal = normalize(viewNormal);
    // replace normal with mapped if normal mapping enabled
    // 说明：DirectX坐标是以左上角为0，而OPENGL是以左下角为0
    if (normalMapEnabled)
    {
        // build the tranform (rotation) into tangent space;切线空间转视图空间的3X3旋转矩阵(利用TBN)
        //const float3x3 tanToView = float3x3(
        //    normalize(tan),
        //    normalize(bitan),
        //    normalize(viewNormal)
        //);        
        
        // sample and unpack the normal from texture into tangent space        
        //const float3 normalSample = nmap.Sample(splr, tc).xyz;//先对法线贴图采样
        
        /* |-- T --|                    |-- X --|
           |-- B --|   <==对应关系==>    |-- Z --|             
           |-- N --|                    |-- Y --|
           由于XYZ坐标转换成 TBN坐标是 TNB的顺序，所以n是第二个分量
        */              
        //float3 tanNormal;
        //tanNormal = normalSample * 2.0f - 1.0f;
        //tanNormal.y = -tanNormal.y;//对第二个分量进行处理
        
        // 把法线从切线空间转移到视图空间;注意重新归一化
        viewNormal = MapNormal(normalize(viewTan), normalize(viewBitan), viewNormal, tc, nmap, splr);
        
    }
    
    // 主逻辑 ===============================================================================
    
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
	// attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
	// diffuse
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
    // specular
    const float3 specular = Speculate(
        diffuseColor, diffuseIntensity, viewNormal,
        lv.vToL, viewFragPos, att, specularPower
    );
    // final color
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}