#pragma once
#include "Bindable.h"

class Surface;

namespace Bind
{	// 构造方法是Texture( Graphics& gfx,const class Surface& s , unsigned int slot = 0);
	class Texture : public Bindable
	{
	public:
		Texture( Graphics& gfx,const class Surface& s , unsigned int slot = 0);
		void Bind( Graphics& gfx ) noexcept override;
	private:
		unsigned int slot;// 插槽用于计算使用多少次纹理
	protected:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	};
}
