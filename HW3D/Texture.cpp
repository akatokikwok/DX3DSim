#include "Texture.h"
#include "Surface.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"
#include <d3d11.h>

namespace Bind
{
	namespace wrl = Microsoft::WRL;

	Texture::Texture(Graphics& gfx, const std::string& path, UINT slot)
		:
		path(path),
		slot(slot)
	{
		INFOMAN(gfx);

		// 加载Surface型的图片
		const auto s = Surface::FromFile(path);

		hasAlpha = s.AlphaLoaded();// 检查是否加载了透明通道

		// create texture resource
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = s.GetWidth();
		textureDesc.Height = s.GetHeight();
		// 设置成0是表明使用一系列chain直到逐步查询到我们所需要的那一层纹理大小
		textureDesc.MipLevels = 0;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		// 绑定标志不仅要设置成着色器资源;而且还需要被绑定成渲染目标,因为要使用MIPMAP的时候GPU会回写到此纹理
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		// 很明显MiscFlags要设置成生成MIPMAPS
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = s.GetBufferPtr();
		sd.SysMemPitch = s.GetWidth() * sizeof(Surface::Color);
		wrl::ComPtr<ID3D11Texture2D> pTexture;
		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(
			&textureDesc, &sd, &pTexture
		));

		// 创建这个纹理的资源视图
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		// 设置成-1表明要是用所有的MIP
		srvDesc.Texture2D.MipLevels = -1;
		GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView
		));

		// 通过使用GPU渲染管线来自动生成MIP chain of 上述纹理
		GetContext(gfx)->GenerateMips(pTextureView.Get());
	}

	void Texture::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
	}

	//========================================================
	std::shared_ptr<Texture> Texture::Resolve(Graphics& gfx, const std::string& path, UINT slot)
	{
		return Codex::Resolve<Texture>(gfx, path, slot);
	}
	std::string Texture::GenerateUID(const std::string& path, UINT slot)
	{
		using namespace std::string_literals;
		return typeid(Texture).name() + "#"s + path + "#" + std::to_string(slot);
	}
	std::string Texture::GetUID() const noexcept
	{
		return GenerateUID(path, slot);
	}

	bool Texture::HasAlpha() const noexcept
	{
		return hasAlpha;
	}

}
