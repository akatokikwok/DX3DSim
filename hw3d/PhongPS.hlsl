cbuffer LightCBuf
{
	float3 lightPos;
};

static const float3 materialColor = { 0.7f,0.7f,0.9f };
static const float3 ambient = { 0.05f,0.05f,0.05f };
static const float3 diffuseColor = { 1.0f,1.0f,1.0f };
static const float diffuseIntensity = 1.0f;

// ����˥����Χ��������Щ����
static const float attConst = 1.0f;
static const float attLin = 0.045f;
//static const float attLin = 0.001f;

static const float attQuad = 0.0075f;
//static const float attQuad = 0.0002f;

// �����ߵ���ɫ��,���ڽ��ܹ�Դ������;��������Щ���ܹ��յĻ�����ʹ��
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
    return float4(saturate((diffuse + ambient) * materialColor), 1.0f);// (������+������)*������ɫ
}