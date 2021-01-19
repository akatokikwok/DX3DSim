#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>

namespace Bind
{
	// 模型变换常量缓存类
	class TransformCbuf : public Bindable
	{
	private:
		// 顶点常数缓存结构体
		struct Transforms
		{
			DirectX::XMMATRIX modelViewProj;
			DirectX::XMMATRIX model;
		};
	public:
		TransformCbuf(Graphics& gfx, const Drawable& parent, UINT slot = 0u);// 新增插槽的构造
		void Bind(Graphics& gfx) noexcept override;
	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;// 顶点常数缓存
		const Drawable& parent;// 绘制物引用
	};
}
