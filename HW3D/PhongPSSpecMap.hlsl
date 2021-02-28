// 模型高光贴图像素着色器

// 常数--光照
cbuffer LightCBuf   
{
    float3 lightPos;        //光源位置
    float3 ambient;         //环境光常数  
    float3 diffuseColor;    //漫反射颜色常数
    float  diffuseIntensity;//漫反射功率常数
    
    float attConst;         
    float attLin;
    float attQuad;
};

Texture2D tex;  // 纹理
Texture2D spec; // 镜面光纹理

SamplerState splr; // 采样器

//static const float specularPowerFactor = 100.0f;//自定义个系数，用于控制镜面光功率


float4 main(float3 worldPos : Position, float3 n : Normal, float2 tc : Texcoord) : SV_Target
{
	// fragment to light vector data
    const float3 vToL = lightPos - worldPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
	// attenuation
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
	// diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));
	// reflected light vector
    const float3 w = n * dot(vToL, n);
    const float3 r = w * 2.0f - vToL;
    
	// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
    //// 基于观察方向和光反射方向的夹角、系数来计算 镜面光强度
    const float4 specularSample = spec.Sample(splr, tc);         //采样后的镜面光
    const float3 specularReflectionColor = specularSample.rgb;   //剔除透明通道后的高光颜色 
    
    //const float specularPower = specularSample.a * specularPowerFactor; 
    const float specularPower = pow(2.0f, specularSample.a * 13.0f); //specularPower代表高光贴图的功率; ==保留透明通道的镜面光*系数或者指数
    const float3 specular = att * (diffuseColor * diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);// 镜面光 == 衰减 *(漫反射*其功率) * (反射光-r 和worldpos的点积 pow镜面光功率 )
	// final color
    return float4(
        saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular * specularReflectionColor), //最终颜色== (环境光常数+漫反射强度) * (剔除透明通道后的采样纹理) + 镜面光 * 剔除透明通道后的高光颜色;
    // 简化了就是 漫反射光 * 漫反射贴图 + 高光 * 高光贴图
    1.0f);
}