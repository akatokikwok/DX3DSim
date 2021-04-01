#pragma once
#include "Bindable.h"
#include <array>
#include <optional>

namespace Bind
{
	// 构造器Blender( Graphics& gfx,bool blending,std::optional<float> factor = {} );
	class Blender : public Bindable
	{
	public:
		Blender( Graphics& gfx,bool blending,std::optional<float> factor = {} );
		void Bind(Graphics& gfx) noexcept override;
		/* 混合开关,如果有些物体不需要混合,可以关掉以此节省GPU资源*/
		static std::shared_ptr<Blender> Resolve( Graphics& gfx,bool blending,std::optional<float> factor = {} );
		
		void SetFactor(float factor) noxnd;
		float GetFactor() const noxnd;

		static std::string GenerateUID( bool blending,std::optional<float> factor );
		std::string GetUID() const noexcept override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11BlendState> pBlender;//管线上的混合状态
		bool blending; //混合启用状态开关
		std::optional<std::array<float,4>> factors; //代表RGBA通道的混合因子
	};
}