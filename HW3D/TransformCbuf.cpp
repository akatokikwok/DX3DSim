#include "TransformCbuf.h"

TransformCbuf::TransformCbuf(Graphics& gfx, const Drawable& parent)
	:
	//vcbuf(gfx),
	parent(parent)
{
	if (!pVcbuf)
	{
		pVcbuf = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(gfx);
	}
}


void TransformCbuf::Bind(Graphics& gfx) noexcept
{
	// 更新常量缓存
	pVcbuf->Update(gfx,
		DirectX::XMMatrixTranspose(
			parent.GetTransformXM() * gfx.GetProjection()
		)
	);	
	pVcbuf->Bind(gfx);
}

// 静态的矩阵型 顶点常量缓存指针
std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformCbuf::pVcbuf;//类外声明此变量