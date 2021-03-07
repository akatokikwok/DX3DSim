#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>

namespace Bind
{
	// 模型变换常量缓存类(即顶点常数缓存)
	/// TransformCbuf是一个特殊的Bindable类，因为它内部还有一个VertexConstantBuffer
	/// TransformCbuf是沟通绘制物和顶点常数缓存之前的桥梁
	class TransformCbuf : public Bindable
	{
	protected:
		// 自定义一个Transforms结构体;含MV和MVP矩阵
		struct Transforms
		{
			DirectX::XMMATRIX modelView;
			DirectX::XMMATRIX modelViewProj;
		};
	public:
		/* 设置成员绘制物,并构造一个顶点常数缓存指针*/
		TransformCbuf(Graphics& gfx, const Drawable& parent, UINT slot = 0u);

		/* 调用UpdateBindImpl函数;*/
		void Bind(Graphics& gfx) noexcept override;

	protected:
		/* 以入参tf 按Map映射方式更新VSConstantBuffer,再把VSConstantBuffer设置到管线上 */
		void UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept;
		/* 返回一个Transforms结构体, 里面存"转置MV"和"转置MVP矩阵"; */
		Transforms GetTransforms(Graphics& gfx) noexcept;
	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;// 顶点常数缓存
		const Drawable& parent;// 绘制物引用
	};
}
