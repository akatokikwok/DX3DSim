#include "TransformCbuf.h"

namespace Bind
{
	TransformCbuf::TransformCbuf(Graphics& gfx, UINT slot)
	{
		if (!pVcbuf)
		{
			pVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
		}
	}

	/* 执行UpdateBindImpl(gfx, GetTransforms(gfx));*/
	void TransformCbuf::Bind(Graphics& gfx) noexcept
	{
		UpdateBindImpl(gfx, GetTransforms(gfx));
	}

	void TransformCbuf::InitializeParentReference(const Drawable& parent) noexcept
	{
		pParent = &parent;
	}
		
	void TransformCbuf::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept
	{
		assert(pParent != nullptr);

		pVcbuf->Update(gfx, tf); // 用入参tf以映射形式更新顶点常数缓存
		pVcbuf->Bind(gfx); // 管线上绑定 顶点shader常数缓存
	}

	/* 在模型常数缓存里返回拿到一个结构体，它里面存有转置模型视图矩阵、转置MVP矩阵*/
	TransformCbuf::Transforms TransformCbuf::GetTransforms(Graphics& gfx) noexcept
	{
		assert(pParent != nullptr);
		// 拿到ModelView矩阵 == 绘制物模型矩阵  * 视图矩阵
		const auto modelView = pParent->GetTransformXM() * gfx.GetCamera();
		// 返回拿到1个Transforms型的结构体;模型矩阵的转置、和MVP的转置
		return
		{
			DirectX::XMMatrixTranspose(modelView),
			DirectX::XMMatrixTranspose(modelView * gfx.GetProjection())
		};
	}

	std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::pVcbuf;
}