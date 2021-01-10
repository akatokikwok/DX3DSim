cbuffer CBuf
{
	float4 color;
};
// 灯泡着色器
float4 main() : SV_Target
{
	return color;
}