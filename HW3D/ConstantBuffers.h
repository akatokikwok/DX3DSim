#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"

/* 基类型,继承自Bindable*/
template<typename C>
class ConstantBuffer : public Bindable
{
public:
	/* Update方法; 通常cbuffer在每一帧更新*/
	void Update(Graphics& gfx, const C& consts)
	{
		INFOMAN(gfx);
		// 要操作的常熟缓存写入目标 msr
		D3D11_MAPPED_SUBRESOURCE msr;
		// 若需要更新常量缓存上下文里有1个Map方法,映射并锁定资源内存块
		GFX_THROW_INFO(GetContext(gfx)->Map(
			pConstantBuffer.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&msr
		));
		// 把常量写入内存
		memcpy(msr.pData, &consts, sizeof(consts));
		// 写完之后接触锁定内存
		GetContext(gfx)->Unmap(pConstantBuffer.Get(), 0u);
	}
	/* 带const参数的构造,用初始化数据创建常量缓存*/
	ConstantBuffer(Graphics& gfx, const C& consts)
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
	/* 不带const参数的构造,不用数据初始化创建常量缓存*/
	ConstantBuffer(Graphics& gfx)
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
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;//常量缓存
};

/* 2种常量缓存,此处是顶点的CBuffer.VS阶段绑定到管线*/
template<typename C>
class VertexConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::pConstantBuffer;//使用常量基类的字段 pConstantBuffer
	using Bindable::GetContext;//使用 Bindable的上下文访问方法
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphics& gfx) noexcept override
	{
		GetContext(gfx)->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());
	}
};

/* 2种常量缓存,此处是像素的CBuffer,PS阶段绑定到管线*/
template<typename C>
class PixelConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::pConstantBuffer;
	using Bindable::GetContext;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphics& gfx) noexcept override
	{
		GetContext(gfx)->PSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());
	}
};