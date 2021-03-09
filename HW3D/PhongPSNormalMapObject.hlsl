/* 不带切线TB的 像素着色器(法线贴图版)*/

cbuffer LightCBuf       //---[0]
{
    float3 lightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

cbuffer ObjectCBuf      //---[1]
{
    float specularIntensity;
    float specularPower;
    bool normalMapEnabled;
    float padding[1];
};

cbuffer TransformCBuf   //---[2]
{
    matrix modelView;
    matrix modelViewProj;
};

Texture2D tex;
Texture2D nmap : register(t2);

SamplerState splr;


float4 main(float3 viewPos : Position, float3 n : Normal, float2 tc : Texcoord) : SV_Target
{
	// sample normal from map if normal mapping enabled
    if (normalMapEnabled)//注意！！若启用法线贴图,则会将法线一直在z方向映射，所以要在z分量上做一些处理
    //{
    //    // unpack normal data
    //    const float3 normalSample = nmap.Sample(splr, tc).xyz;
    //    n.x = normalSample.x * 2.0f - 1.0f;
    //    n.y = -normalSample.y * 2.0f + 1.0f;
    //    n.z = -normalSample.z;
        
    //    n = mul(n, (float3x3) modelView);
    //}
    //else// 若没开启法线贴图
    {
        // unpack normal data
        const float3 normalSample = nmap.Sample(splr, tc).xyz;
        
        //由于不带切线空间，所以就按照正常的来
        n.x = normalSample.x * 2.0f - 1.0f;
        n.y = -normalSample.y * 2.0f + 1.0f;
        n.z = -normalSample.z * 2.0f + 1.0f;
        n = mul(n, (float3x3) modelView);
    }
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
    const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);
	// final color
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}