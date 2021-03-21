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

		//D3D11_SUBRESOURCE_DATA sd = {};
		//sd.pSysMem = s.GetBufferPtr();
		//sd.SysMemPitch = s.GetWidth() * sizeof(Surface::Color);

		// 创建2D纹理 pTexture;
		wrl::ComPtr<ID3D11Texture2D> pTexture;
		// 注意!! 第二个参数实际上一个指针指向 一个用来描述SubResource的数组;这里设置为空,是为了下一步让s写入SubResource
		// 目前的情况是我仅持有原始纹理,而并没有一堆subresource;所以这里先填充第一个完整的layer层;
		// 之后再使用下文的GenerateMips()方法获取剩余的mipmap;所以假如用Subresource去初始这张纹理,则会达不成预计效果
		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(
			&textureDesc, /*&sd,*/ nullptr, &pTexture
		));
		// 将图片s数据 写入 top mip level
		GetContext(gfx)->UpdateSubresource(
			pTexture.Get(), 0u, nullptr, s.GetBufferPtrConst(), s.GetWidth() * sizeof(Surface::Color), 0u
		);

		// 创建这个纹理的资源视图
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		// 设置成-1表明要是用所有的MIP
		srvDesc.Texture2D.MipLevels = -1;
		// 创建与pTexture2d相关联的视图堆
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

	UINT Texture::CalculateNumberOfMipLevels(UINT width, UINT height) noexcept
	{
		const float xSteps = std::ceil(log2((float)width));
		const float ySteps = std::ceil(log2((float)height));
		return (UINT)std::max(xSteps, ySteps);
	}

}
