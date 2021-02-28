#pragma once
#include "Graphics.h"
#include "ConditionalNoexcept.h"

namespace Bind
{
	class Bindable
	{
	public:
		// 让绑定物子类各自绑定到各自的流水线
		virtual void Bind(Graphics& gfx) noexcept = 0;
		virtual ~Bindable() = default;
		// 获取UID，若拿不到就返回空字符串
		virtual std::string GetUID() const noexcept
		{
			assert(false);
			return "";
		}
	protected:
		static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept;
		static ID3D11Device* GetDevice(Graphics& gfx) noexcept;
		static DxgiInfoManager& GetInfoManager(Graphics& gfx) ;
	};
}
