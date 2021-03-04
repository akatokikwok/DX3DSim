#include "TransformCbufDoubleboi.h"

namespace Bind
{
	TransformCbufDoubleboi::TransformCbufDoubleboi(Graphics& gfx, const Drawable& parent, UINT slotV, UINT slotP)
		:
		TransformCbuf(gfx, parent, slotV)
	{
		if (!pPcbuf)
		{
			pPcbuf = std::make_unique<PixelConstantBuffer<Transforms>>(gfx, slotP);
		}
	}

	void Bind::TransformCbufDoubleboi::Bind(Graphics& gfx) noexcept
	{
		const auto tf = GetTransforms(gfx);		// 拿取MV和MVP矩阵的转置
		TransformCbuf::UpdateBindImpl(gfx, tf);// 使用基类的UpdateBindImpl更新顶点常数缓存并绑到管线
		UpdateBindImpl(gfx, tf);			 	//再使用自己的UpdateBindImpl更新像素常数
	}
	
	/* 更新像素常数缓存并绑定到管线*/
	void TransformCbufDoubleboi::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept
	{
		pPcbuf->Update(gfx, tf);
		pPcbuf->Bind(gfx);
	}

	std::unique_ptr<PixelConstantBuffer<TransformCbuf::Transforms>> TransformCbufDoubleboi::pPcbuf;
}