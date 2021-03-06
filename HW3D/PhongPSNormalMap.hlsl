/* 带法线的像素shader*/

cbuffer LightCBuf// 光源常量;位于[0]
{
    float3 lightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

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


float4 main(float3 worldPos : Position, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    // sample normal from map if normal mapping enabled
    // 说明：DirectX坐标是以左上角为0，而OPENGL是以左下角为0
    if (normalMapEnabled)
    {
        // build the tranform (rotation) into tangent space;切线空间转视图空间的3X3旋转矩阵(利用TBN)
        const float3x3 tanToView = float3x3(
            normalize(tan),
            normalize(bitan),
            normalize(n)
        );        
        
        const float3 normalSample = nmap.Sample(splr, tc).xyz;//先对法线贴图采样
        n.x = normalSample.x * 2.0f - 1.0f; //映射采样品的x到HLSL并归一化
        n.y = -normalSample.y * 2.0f + 1.0f; //映射采样品的y到HLSL并归一化
        n.z = normalSample.z;              //反方向指向摄像机
        
        // 把法线从切线空间转移到视图空间;
        n = mul(n, tanToView);
        
        //n = mul(n, (float3x3) modelView); //让n与MV矩阵相乘
    }
    
    // 主逻辑 ===============================================================================
    
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