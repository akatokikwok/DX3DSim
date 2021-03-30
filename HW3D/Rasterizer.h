#pragma once
#include "Bindable.h"
#include <array>

namespace Bind
{
	class Rasterizer : public Bindable
	{
	public:
		Rasterizer(Graphics& gfx, bool twoSided);
		void Bind(Graphics& gfx) noexcept override;
		/* 使用bool控制是否两面光栅化渲染*/
		static std::shared_ptr<Rasterizer> Resolve(Graphics& gfx, bool twoSided);
		static std::string GenerateUID(bool twoSided);
		std::string GetUID() const noexcept override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;//光栅化状态
		bool twoSided;//纹理两面显示开关
	};
}