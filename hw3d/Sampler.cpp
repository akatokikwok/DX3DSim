#include "Sampler.h"
#include "GraphicsThrowMacros.h"

Sampler::Sampler( Graphics& gfx )
{
	INFOMAN( gfx );

	D3D11_SAMPLER_DESC samplerDesc = {};
	/* 2d纹理的过滤方式,包括nearst neighbour和bilinear*/
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;// 线性插值
	//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;// 点插值

	/* 有3种枚举可供选择,负责定义纹理超出边界时的处理, Wrap\Clamp\Border 
		假如是Clamp模式的话,那么纹理图的像素将延长并覆盖表面
	*/
	samplerDesc.AddressU =  D3D11_TEXTURE_ADDRESS_BORDER ;
	samplerDesc.AddressV =   D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	// 若选择Border模式,则可以细分填充颜色,这里设置成绿色
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;


	GFX_THROW_INFO( GetDevice( gfx )->CreateSamplerState( &samplerDesc,&pSampler ) );
}

void Sampler::Bind( Graphics& gfx ) noexcept
{
	GetContext( gfx )->PSSetSamplers( 0,1,pSampler.GetAddressOf() );
}