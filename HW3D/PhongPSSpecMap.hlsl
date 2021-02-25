// 模型高光贴图像素着色器

// 常数--光照
cbuffer LightCBuf   
{
    float3 lightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    
    float attConst;
    float attLin;
    float attQuad;
};

Texture2D tex;  // 纹理
Texture2D spec; // 镜面光纹理

SamplerState splr; // 采样器


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
    const float4 specularSample = spec.Sample(splr, tc); // 首先对高光贴图采样
    const float3 specularColorIntensity = specularSample.rgb;//获取其rgb通道 
    const float specularPower = specularSample.a;            //获取其a通道
    const float3 specular = att * specularColorIntensity * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);
	// final color
    return float4(saturate(diffuse + ambient + specular), 1.0f) * tex.Sample(splr, tc);
}