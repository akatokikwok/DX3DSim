// 光照常数应该在插槽0
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
// 绘制物常数应该在插槽1
cbuffer ObjectCBuf
{
	// 材质应该独立出光照,材质颜色是由物体本身决定的
    float3 materialColor;
	
    float specularIntensity;// 镜面光
    float specularPower;// 镜面级数
};

//static const float3 materialColor = { 0.7f,0.7f,0.9f };
//static const float3 ambient = { 0.05f,0.05f,0.05f };
//static const float3 diffuseColor = { 1.0f,1.0f,1.0f };
//static const float diffuseIntensity = 1.0f;

//// 光照衰减范围由下面这些决定
//static const float attConst = 1.0f;
//static const float attLin = 0.045f;
////static const float attLin = 0.001f;

//static const float attQuad = 0.0075f;
////static const float attQuad = 0.0002f;

// 带法线的着色器,用于接受光源的照射;供给给那些承受光照的绘制物使用
float4 main( float3 worldPos : Position,float3 n : Normal ) : SV_Target
{
	// fragment to light vector data
	const float3 vToL = lightPos - worldPos;
	const float distToL = length( vToL );
	const float3 dirToL = vToL / distToL;
	// 衰减
	const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
	// 漫反射最终呈现效果
	const float3 diffuse = att * diffuseColor * diffuseIntensity *  max( 0.0f,dot( dirToL,n ) );
	
	// reflected light vector
    const float3 w = n * dot(vToL, n);
	 // 光反射后的单位方向向量
    const float3 r = w * 2.0f - vToL;
	// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
	// 镜面光== (漫反射颜色*漫反射强度)* 镜面光强度 * ()的镜面级数
	// 此时就可以实现让距离不同的绘制对象显示出的高光强弱有差异
    const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);
	
	// final color
    //return float4(saturate((diffuse + ambient) * materialColor), 1.0f);
    return float4(saturate((diffuse + ambient + specular) * materialColor), 1.0f); // (环境光+漫反射+反射镜面光)*材质颜色
}