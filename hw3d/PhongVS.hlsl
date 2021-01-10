cbuffer CBuf
{
	//matrix model;
	
    matrix modelView; // ���ڸ���Ϊmodelview,�Ա�֮���VSOut�ṹ��worldpos���������������������������������
	matrix modelViewProj;
};

struct VSOut
{
	float3 worldPos : Position;
	float3 normal : Normal;
	float4 pos : SV_Position;
};

VSOut main( float3 pos : Position,float3 n : Normal )
{
	VSOut vso;
    vso.worldPos = (float3) mul(float4(pos, 1.0f), modelView);	// (���������������������) ����λ�õ��� ����pos����modelView
    vso.normal = mul(n, (float3x3) modelView);					// (���������������������) ����λ�õ��� ����noraml����modelView
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);			//  λ��    ���� ����pos����MVP����	
	return vso;
}