#include "Texture.h"
#include "Surface.h"
#include "GraphicsThrowMacros.h"

namespace wrl = Microsoft::WRL;

Texture::Texture( Graphics& gfx,const Surface& s )
{
	INFOMAN( gfx );

	// 创建贴图资源
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = s.GetWidth();
	textureDesc.Height = s.GetHeight();
	textureDesc.MipLevels = 1;//创建纹理时允许创建一组多个纹理,但是此处只创建1个
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;// 类似于后缓存,也是四位填充
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA sd = {};
	// 子资源的系统内存需要 surface实例的缓存
	sd.pSysMem = s.GetBufferPtr();
	// SysMemPitch指代第一行第一个像素和第二行第一个像素之间的距离
	sd.SysMemPitch = s.GetWidth() * sizeof( Surface::Color );
	wrl::ComPtr<ID3D11Texture2D> pTexture;
	GFX_THROW_INFO( GetDevice( gfx )->CreateTexture2D( 
		&textureDesc,&sd,&pTexture
	) );

	// 创建在纹理上的着色器资源视图;(因为需要上述的纹理先创建视图,才可以把"视图"绑定至渲染管线)
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;//视图的格式应与纹理的格式保持一致
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;//不使用mipmap
	GFX_THROW_INFO( GetDevice( gfx )->CreateShaderResourceView( 
		pTexture.Get(),&srvDesc,&pTextureView
	) );
}

void Texture::Bind( Graphics& gfx ) noexcept
{
	GetContext( gfx )->PSSetShaderResources( 0u,1u,pTextureView.GetAddressOf() );
}
