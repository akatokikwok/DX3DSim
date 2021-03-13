/* 带法线且带镜面光的像素shader*/

#include "ShaderOps.hlsl"
#include "PointLight.hlsl"
#include "LightVectorData.hlsl"
//// 常数--光照
//cbuffer LightCBuf           //[0]
//{
//    float3 viewLightPos;        //光源位置
//    float3 ambient;         //环境光常数  
//    float3 diffuseColor;    //漫反射颜色常数
//    float  diffuseIntensity;//漫反射功率常数
    
//    float attConst;         
//    float attLin;
//    float attQuad;
//};

// 常数--模型上的
cbuffer ObjectCBuf          //[1]
{
    bool normalMapEnabled;//法线贴图采样开关
    bool specularMapEnabled; //镜面纹理采样开关
    bool hasGloss; //透明通道;通知shader是否存在透明通道
    float specularPowerConst;//自定义一个高光功率系数

    float3 specularColor;       //高光颜色
    float specularMapWeight;    //高光纹理占比权重
};

Texture2D tex;  // 漫反射纹理
Texture2D spec; // 镜面光纹理
Texture2D nmap; // 法线贴图

SamplerState splr; // 采样器

//static const float specularPowerFactor = 100.0f;//自定义个系数，用于控制镜面光功率


/* 主shader*/
float4 main(float3 viewFragPos : Position /*相机观察位置*/, float3 viewNormal : Normal,
    float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    // normalize the mesh normal
    viewNormal = normalize(viewNormal);
    
	// 若开启法线纹理采样
    if (normalMapEnabled)
    {
        viewNormal = MapNormal(normalize(viewTan), normalize(viewBitan), viewNormal, tc, nmap, splr); // 执行外部方法:操作法线贴图;
    }  
    
    // 主逻辑 ===============================================================================
   
    //const float3 viewFragToL = viewLightPos - viewPos;  /* 视图空间里 模型位置到光源位置的vector*/
    //const float distFragToL = length(viewFragToL);      /* 物体到光源的距离, 浮点数*/
    //const float3 viewDirFragToL = viewFragToL / distFragToL; /*归一化的 '点到光源向量'*/
    
    // fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
    
    // specular parameter determination (mapped or uniform) ;先按不同情形执行镜面参数配置以获得不同情况下的镜面参数(在这里是镜面反射颜色和功率)
    float3 specularReflectionColor; //表示镜面反射的颜色
    float specularPower = specularPowerConst;  //高光功率更新为一个指定系数  
    //若开启镜面纹理采样
    if (specularMapEnabled)
    {          
        const float4 specularSample = spec.Sample(splr, tc); //采样后的镜面光纹理
        specularReflectionColor = specularSample.rgb * specularMapWeight; //镜面反射颜色 == 剔除alpha通道后的镜面纹理 * 权重
        if (hasGloss)// 只有高光贴图具备阿尔法通道才会更新镜面功率
        {
            specularPower = pow(2.0f, specularSample.a * 13.0f); //高光功率不使用原有的固定系数specularPowerConst，而是重新计算;  它 == 保留透明通道的镜面光*系数或者指数
        }
    }
    else//若关闭镜面纹理采样
    {   
        specularReflectionColor = specularColor; //镜面反射颜色更新为一个高光颜色(自定义值)
    }    
    
    // attenuation;拿到衰减
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
    // diffuse light;拿到漫反射光
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
    // specular reflected; 拿到镜面反射结果
    const float3 specularReflected = Speculate(specularReflectionColor, 1.0f, viewNormal,
        lv.vToL, viewFragPos, att, specularPower
    );
    
	// final color = attenuate diffuse & ambient by diffuse texture color and add specular reflected
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specularReflected), 1.0f);
    
    
    
 //   const float3 specular = att * (diffuseColor * diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);// 镜面光 == 衰减 *(漫反射*其功率) * (反射光-r 和worldpos的点积 pow镜面光功率 )
	    //// final color
 //   return float4(
 //       saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular * specularReflectionColor), //最终颜色== (环境光常数+漫反射强度) * (剔除透明通道后的采样纹理) + 镜面光 * 镜面反射颜色;
 //   // 简化了就是 漫反射光 * 漫反射贴图(或者漫反射颜色) + 高光 * 高光贴图(或者是高光反射颜色)
 //   1.0f);
}