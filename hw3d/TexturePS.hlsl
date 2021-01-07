
// 全局声明1张纹理,tex变量对应着绑定0号槽的纹理
Texture2D tex : register(t0);
// 全局声明1个采样器
SamplerState splr : register(s0);

float4 main( float2 tc/*二维纹理坐标*/ : TexCoord ) : SV_Target
{
	return tex.Sample( splr,tc );
}