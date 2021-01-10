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
	// diffuse attenuation
	const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
	// diffuse intensity
	const float3 diffuse = diffuseColor * diffuseIntensity * att * max( 0.0f,dot( dirToL,n ) );
	// final color
    return float4(saturate((diffuse + ambient) * materialColor), 1.0f);// (环境光+漫反射)*材质颜色
}