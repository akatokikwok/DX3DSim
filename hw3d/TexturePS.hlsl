
// ȫ������1������,tex������Ӧ�Ű�0�Ų۵�����
Texture2D tex : register(t0);
// ȫ������1��������
SamplerState splr;

float4 main( float2 tc/*��ά��������*/ : TexCoord ) : SV_Target
{
	return tex.Sample( splr,tc );
}