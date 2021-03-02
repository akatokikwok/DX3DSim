cbuffer LightCBuf//光源常量
{
    float3 lightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

cbuffer ObjectCBuf//模型里一些参数常量
{
    float specularIntensity;
    float specularPower;
    bool normalMapEnabled;//法线贴图采样开关
    float padding[1];
};

Texture2D tex;
Texture2D nmap;

SamplerState splr;


float4 main(float3 worldPos : Position, float3 n : Normal, float2 tc : Texcoord) : SV_Target
{
    // sample normal from map if normal mapping enabled
    if (normalMapEnabled)
    {
        const float3 normalSample = nmap.Sample(splr, tc).xyz;//先对法线贴图采样
        n.x = normalSample.x * 2.0f - 1.0f; //映射采样品的x到HLSL并归一化
        n.y = normalSample.y * 2.0f - 1.0f; //映射采样品的y到HLSL并归一化
        n.z = -normalSample.z;              //取负并指向摄像机
    }
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
    const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);
	// final color
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}