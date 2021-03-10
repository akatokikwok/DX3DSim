/* 带法线且带镜面光的像素shader*/

// 常数--光照
cbuffer LightCBuf           //[0]
{
    float3 lightPos;        //光源位置
    float3 ambient;         //环境光常数  
    float3 diffuseColor;    //漫反射颜色常数
    float  diffuseIntensity;//漫反射功率常数
    
    float attConst;         
    float attLin;
    float attQuad;
};

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


float4 main(float3 viewPos : Position, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
	// 若开启法线纹理采样
    if (normalMapEnabled)
    {
        // build the tranform (rotation) into tangent space
        const float3x3 tanToView = float3x3(
            normalize(tan),
            normalize(bitan),
            normalize(n)
        );
        // 取法线贴图采样后的分量进行分析
        const float3 normalSample = nmap.Sample(splr, tc).xyz;
        //n.x = normalSample.x * 2.0f - 1.0f;
        //n.y = -normalSample.y * 2.0f + 1.0f;
        //n.z = normalSample.z;
        
        // 由于带切线空间所以按照这条准则
        n = normalSample * 2.0f - 1.0f;
        // bring normal from tanspace into view space
        n = mul(n, tanToView);
    }  
    
    // 主逻辑 ===============================================================================
    // fragment to light vector data
    const float3 vToL = lightPos - viewPos;
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
    //const float4 specularSample = spec.Sample(splr, tc);         //采样后的镜面光
    //const float3 specularReflectionColor = specularSample.rgb;   //剔除透明通道后的高光颜色 
    
    //const float specularPower = specularSample.a * specularPowerFactor; 
    //const float specularPower = pow(2.0f, specularSample.a * 13.0f); //specularPower代表高光贴图的功率; ==保留透明通道的镜面光*系数或者指数
    
    //float specularPower;
    //if (hasGloss)
    
    float3 specularReflectionColor; //表示镜面反射的颜色
    float specularPower = specularPowerConst;  //高光功率更新为一个指定系数
    if (specularMapEnabled)//若开启镜面纹理采样
    {          
        const float4 specularSample = spec.Sample(splr, tc); //采样后的镜面光纹理
        specularReflectionColor = specularSample.rgb * specularMapWeight; //镜面反射颜色 == 剔除alpha通道后的镜面纹理 * 权重
        if (hasGloss)// 若存在透明通道则从高光贴图中加载透明通道
        {
            specularPower = pow(2.0f, specularSample.a * 13.0f); //高光功率不使用原有的固定系数specularPowerConst，而是重新计算;  它 == 保留透明通道的镜面光*系数或者指数
        }
    }
    else//若关闭镜面纹理采样
    {   
        //specularPower = specularPowerConst;   // 若不存在透明通道，则从一个常量中加载
        
        specularReflectionColor = specularColor; //镜面反射颜色更新为一个高光颜色(自定义值)
    }
    
    const float3 specular = att * (diffuseColor * diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);// 镜面光 == 衰减 *(漫反射*其功率) * (反射光-r 和worldpos的点积 pow镜面光功率 )
	// final color
    return float4(
        saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular * specularReflectionColor), //最终颜色== (环境光常数+漫反射强度) * (剔除透明通道后的采样纹理) + 镜面光 * 镜面反射颜色;
    // 简化了就是 漫反射光 * 漫反射贴图(或者漫反射颜色) + 高光 * 高光贴图(或者是高光反射颜色)
    1.0f);
}