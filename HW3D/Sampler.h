﻿#pragma once
#include "Bindable.h"

namespace Bind
{
	class Sampler : public Bindable
	{
	public:
		Sampler(Graphics& gfx);
		void Bind(Graphics& gfx) noexcept override;

		static std::shared_ptr<Sampler> Resolve(Graphics& gfx);//解析并拿到采样器
		static std::string GenerateUID();
		std::string GetUID() const noexcept override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	};

}

