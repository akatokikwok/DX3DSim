cbuffer CBuf
{
	float4 color;
};
// µÆÅÝ×ÅÉ«Æ÷
float4 main() : SV_Target
{
	return color;
}