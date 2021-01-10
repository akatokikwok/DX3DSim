// ���ճ���Ӧ���ڲ��0
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
// �����ﳣ��Ӧ���ڲ��1
cbuffer ObjectCBuf
{
	// ����Ӧ�ö���������,������ɫ�������屾�������
    float3 materialColor;
	
    float specularIntensity;// �����
    float specularPower;// ���漶��
};

//static const float3 materialColor = { 0.7f,0.7f,0.9f };
//static const float3 ambient = { 0.05f,0.05f,0.05f };
//static const float3 diffuseColor = { 1.0f,1.0f,1.0f };
//static const float diffuseIntensity = 1.0f;

//// ����˥����Χ��������Щ����
//static const float attConst = 1.0f;
//static const float attLin = 0.045f;
////static const float attLin = 0.001f;

//static const float attQuad = 0.0075f;
////static const float attQuad = 0.0002f;

// �����ߵ���ɫ��,���ڽ��ܹ�Դ������;��������Щ���ܹ��յĻ�����ʹ��
float4 main( float3 worldPos : Position,float3 n : Normal ) : SV_Target
{
	// fragment to light vector data
	const float3 vToL = lightPos - worldPos;
	const float distToL = length( vToL );
	const float3 dirToL = vToL / distToL;
	// ˥��
	const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
	// ���������ճ���Ч��
	const float3 diffuse = att * diffuseColor * diffuseIntensity *  max( 0.0f,dot( dirToL,n ) );
	
	// reflected light vector
    const float3 w = n * dot(vToL, n);
	 // �ⷴ���ĵ�λ��������
    const float3 r = w * 2.0f - vToL;
	// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
	// �����== (��������ɫ*������ǿ��)* �����ǿ�� * ()�ľ��漶��
	// ��ʱ�Ϳ���ʵ���þ��벻ͬ�Ļ��ƶ�����ʾ���ĸ߹�ǿ���в���
    const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);
	
	// final color
    //return float4(saturate((diffuse + ambient) * materialColor), 1.0f);
    return float4(saturate((diffuse + ambient + specular) * materialColor), 1.0f); // (������+������+���侵���)*������ɫ
}