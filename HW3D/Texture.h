#pragma once
#include "Bindable.h"

class Surface;

namespace Bind
{	// 构造方法是--Texture(Graphics& gfx, const std::string& path, UINT slot = 0);
	class Texture : public Bindable
	{
	public:
		/*Texture( Graphics& gfx,const class Surface& s , unsigned int slot = 0);*/

		Texture(Graphics& gfx, const std::string& path, UINT slot = 0);
		void Bind( Graphics& gfx ) noexcept override;

		//============================================
		static std::shared_ptr<Texture> Resolve(Graphics& gfx, const std::string& path, UINT slot = 0);
		static std::string GenerateUID(const std::string& path, UINT slot = 0);
		std::string GetUID() const noexcept override;
	private:
		unsigned int slot;// 插槽用于计算使用多少次纹理
	protected:
		std::string path;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	};
}
