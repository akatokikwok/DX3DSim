﻿#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	template<typename C>
	class ConstantBuffer : public Bindable
	{
	public:
		// 以MAP映射方式把入参 泛型变量consts写入常量缓存以此达到更新常数缓存目的
		void Update(Graphics& gfx, const C& consts)
		{
			INFOMAN(gfx);

			D3D11_MAPPED_SUBRESOURCE msr;
			GFX_THROW_INFO(GetContext(gfx)->Map(//把常数缓存映射到一块内存空间上
				pConstantBuffer.Get(), 0u,
				D3D11_MAP_WRITE_DISCARD, 0u,
				&msr
			));
			memcpy(msr.pData, &consts, sizeof(consts));
			GetContext(gfx)->Unmap(pConstantBuffer.Get(), 0u);
		}

		/* 构造器：带常数, 插槽的构造;负责创建出常量缓存*/
		ConstantBuffer(Graphics& gfx, const C& consts, UINT slot = 0u)
			:
			slot(slot)// 初始化插槽槽位
		{
			INFOMAN(gfx);

			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof(consts);
			cbd.StructureByteStride = 0u;

			D3D11_SUBRESOURCE_DATA csd = {};
			csd.pSysMem = &consts;
			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, &csd, &pConstantBuffer));
		}

		/* 构造器:不带常数,只带插槽的构造;负责创建出常量缓存*/
		ConstantBuffer(Graphics& gfx, UINT slot = 0u)
			:
			slot(slot)// 初始化插槽槽位
		{
			INFOMAN(gfx);

			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof(C);
			cbd.StructureByteStride = 0u;
			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &pConstantBuffer));
		}
	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;// 常数缓存
		UINT slot;											 // 插槽
	};

	/// <summary>
	/// 常量缓存的派生类: 顶点shader常数缓存和像素shader常数缓存
	/// </summary>
	/// <typeparam name="C"></typeparam>

	template<typename C>
	class VertexConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::pConstantBuffer; // 复用基类常数缓存
		using ConstantBuffer<C>::slot;			  // 复用基类插槽	
		using Bindable::GetContext;
	public:
		using ConstantBuffer<C>::ConstantBuffer;
		// 管线上绑定 顶点着色器常数缓存
		void Bind(Graphics& gfx) noexcept override
		{
			GetContext(gfx)->VSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
		}
		/// =============================================三件套
		static std::shared_ptr<VertexConstantBuffer> Resolve(Graphics& gfx, const C& consts, UINT slot = 0) 
		{
			return Codex::Resolve<VertexConstantBuffer>(gfx, consts, slot);
		}
		static std::shared_ptr<VertexConstantBuffer> Resolve(Graphics& gfx, UINT slot = 0)
		{
			return Codex::Resolve<VertexConstantBuffer>(gfx, slot);
		}
		// 三件套之GenerateUID,2个参数,分别是const C&, UINT slot
		static std::string GenerateUID(const C&, UINT slot)
		{
			return GenerateUID(slot);
		}
		static std::string GenerateUID(UINT slot = 0)
		{
			using namespace std::string_literals;
			return typeid(VertexConstantBuffer).name() + "#"s + std::to_string(slot);
		}
		std::string GetUID() const noexcept override
		{
			return GenerateUID( slot );
		}
	};

	template<typename C>
	class PixelConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::pConstantBuffer;// 复用基类常数缓存
		using ConstantBuffer<C>::slot;			 // 复用基类插槽	
		using Bindable::GetContext;
	public:
		using ConstantBuffer<C>::ConstantBuffer;
		// 管线上绑定 像素着色器常数缓存
		void Bind(Graphics& gfx) noexcept override
		{
			GetContext(gfx)->PSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
		}

		static std::shared_ptr<PixelConstantBuffer> Resolve(Graphics& gfx, const C& consts, UINT slot = 0)
		{
			return Codex::Resolve<PixelConstantBuffer>(gfx, consts, slot);
		}
		static std::shared_ptr<PixelConstantBuffer> Resolve(Graphics& gfx, UINT slot = 0)
		{
			return Codex::Resolve<PixelConstantBuffer>(gfx, slot);
		}
		// 三件套之GenerateUID,2个参数,分别是const C&, UINT slot
		static std::string GenerateUID(const C&, UINT slot)
		{
			return GenerateUID(slot);
		}
		static std::string GenerateUID(UINT slot = 0)
		{
			using namespace std::string_literals;
			return typeid(PixelConstantBuffer).name() + "#"s + std::to_string(slot);
		}		
		std::string GetUID() const noexcept override
		{
			return GenerateUID(slot);
		}
	};
}
