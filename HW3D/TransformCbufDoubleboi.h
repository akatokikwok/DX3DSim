#pragma once
#include "TransformCbuf.h"

namespace Bind
{
	/* 继承自TransformCbuf,*/
	class TransformCbufDoubleboi : public TransformCbuf
	{
	public:
		/* 构造器:带两个插槽*/
		TransformCbufDoubleboi(Graphics& gfx, const Drawable& parent, UINT slotV = 0u, UINT slotP = 0u);
		/* 重写Bind；执行一次基类的UpdateBindImpl和自己的UpdateBindImpl*/
		void Bind(Graphics& gfx) noexcept override;
	protected:
		/* 效仿基类的UpdateBindImpl；更新像素常数缓存并绑定到管线*/
		void UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept;
	private:
		static std::unique_ptr<PixelConstantBuffer<Transforms>> pPcbuf;//与基类相比，它还额外持有1个像素常数缓存
	};
}