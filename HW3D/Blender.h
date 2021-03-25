#pragma once
#include "Bindable.h"
#include <array>

namespace Bind
{
	class Blender : public Bindable
	{
	public:
		Blender(Graphics& gfx, bool blending);
		void Bind(Graphics& gfx) noexcept override;
		/* 混合开关,如果有些物体不需要混合,可以关掉以此节省GPU资源*/
		static std::shared_ptr<Blender> Resolve(Graphics& gfx, bool blending);
		static std::string GenerateUID(bool blending);
		std::string GetUID() const noexcept override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11BlendState> pBlender;//管线上的混合状态
		bool blending; //混合启用状态开关
	};
}