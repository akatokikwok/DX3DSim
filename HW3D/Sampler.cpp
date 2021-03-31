#include "Sampler.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	Sampler::Sampler(Graphics& gfx)
	{
		INFOMAN(gfx);

		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
		//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		//samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;

		/// 为了使用MIPMAP技术,下面采样要设置LOD
		//samplerDesc.MipLODBias = 0.0f;//添加一个LOD偏差系数因子,它负责当进行LOD进行计算的时候可以使用它抵消一些
		//samplerDesc.MinLOD = 0.0f;//LOD最小级别
		//samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;//LOD最大级别

		GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&samplerDesc, &pSampler));
	}

	void Sampler::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetSamplers(0, 1, pSampler.GetAddressOf());
	}

	std::shared_ptr<Sampler> Sampler::Resolve(Graphics& gfx)
	{
		return Codex::Resolve<Sampler>(gfx);
	}
	std::string Sampler::GenerateUID()
	{
		return typeid(Sampler).name();
	}
	std::string Sampler::GetUID() const noexcept
	{
		return GenerateUID();
	}
}