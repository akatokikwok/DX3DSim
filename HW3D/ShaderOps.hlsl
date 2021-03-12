/*各种操作纹理和着色器光线的方法文件*/

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
