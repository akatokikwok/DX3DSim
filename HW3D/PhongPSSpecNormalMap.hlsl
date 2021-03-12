/* 带法线且带镜面光的像素shader*/

// 常数--光照
cbuffer LightCBuf           //[0]
{
    float3 viewLightPos;        //光源位置
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

/* 操作法线纹理(需要切线空间)*/
float3 MapNormal(
    const in float3 tan, 
    const in float3 bitan, 
    const in float3 normal,
    const in float2 tc, 
    uniform Texture2D nmap, //如果变量以uniform关键字为前缀，就意味着此变量在着色器外面被初始化，比如被C++应用程序初始化，然后再输入进着色器。
    uniform SamplerState splr)
{
    // build the tranform (rotation) into same space as tan/bitan/normal (target space);构建TBN旋转矩阵
    const float3x3 tanToTarget = float3x3(
       tan, bitan, normal
    );
    // 对入参法线贴图采样并取分量
    const float3 normalSample = nmap.Sample(splr, tc).xyz;
    // 样本*2-1 拿到切线空间法线
    const float3 tanNormal = normalSample * 2.0f - 1.0f;
    // 与TBN矩阵相乘,切线空间转化到视图空间
    return normalize(mul(tanNormal, tanToTarget));
}

/* 操作光衰减*/
float Attenuate(uniform float attConst, uniform float attLin, uniform float attQuad, const in float distFragToL)
{
    return 1.0f / (attConst + attLin * distFragToL + attQuad * (distFragToL * distFragToL));
}

/* 操作漫反射*/
float3 Diffuse(
    uniform float3 diffuseColor,
    uniform float diffuseIntensity,
    const in float att,
    const in float3 viewDirFragToL,
    const in float3 viewNormal)
{
    return diffuseColor * diffuseIntensity * att * max(0.0f, dot(viewDirFragToL, viewNormal));
}

/* 操作镜面反射*/
float3 Speculate(
    const in float3 specularColor,
    uniform float specularIntensity,
    const in float3 viewNormal,
    const in float3 viewFragToL,
    const in float3 viewPos,
    const in float att,
    const in float specularPower)
{
    // 计算反射后的光向量(标准化)
    const float3 w = viewNormal * dot(viewFragToL, viewNormal);
    const float3 r = normalize(w * 2.0f - viewFragToL);
    // vector from camera to fragment (in view space); 相机到视图空间里片元的向量(标准化)
    const float3 viewCamToFrag = normalize(viewPos);
    // calculate specular component color based on angle between
    // viewing vector and reflection vector, narrow with power function
    return att * specularColor * specularIntensity * pow(max(0.0f, dot(-r, viewCamToFrag)), specularPower);
}

/* 主shader*/
float4 main(float3 viewPos : Position /*相机观察位置*/, float3 viewNormal : Normal, 
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
    // fragment to light vector data
    const float3 viewFragToL = viewLightPos - viewPos;  /* 视图空间里 模型位置到光源位置的vector*/
    const float distFragToL = length(viewFragToL);      /* 物体到光源的距离, 浮点数*/
    const float3 viewDirFragToL = viewFragToL / distFragToL; /*归一化的 '点到光源向量'*/
	
    
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
    const float att = Attenuate(attConst, attLin, attQuad, distFragToL);
    // diffuse light;拿到漫反射光
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, viewDirFragToL, viewNormal);    
    // specular reflected; 拿到镜面反射结果
    const float3 specularReflected = Speculate(specularReflectionColor, 1.0f, viewNormal,
        viewFragToL, viewPos, att, specularPower
    );
    
	// final color attenuate diffuse & ambient by diffuse texture color and add specular reflected
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specularReflected), 1.0f);
    
    
    
 //   const float3 specular = att * (diffuseColor * diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);// 镜面光 == 衰减 *(漫反射*其功率) * (反射光-r 和worldpos的点积 pow镜面光功率 )
	    //// final color
 //   return float4(
 //       saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular * specularReflectionColor), //最终颜色== (环境光常数+漫反射强度) * (剔除透明通道后的采样纹理) + 镜面光 * 镜面反射颜色;
 //   // 简化了就是 漫反射光 * 漫反射贴图(或者漫反射颜色) + 高光 * 高光贴图(或者是高光反射颜色)
 //   1.0f);
}