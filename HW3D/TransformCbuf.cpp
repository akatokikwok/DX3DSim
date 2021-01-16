#include "TransformCbuf.h"

TransformCbuf::TransformCbuf( Graphics& gfx,const Drawable& parent, UINT slot)
	:
	parent( parent )
{
	if( !pVcbuf )
	{
		pVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>( gfx, slot);
	}
}

void TransformCbuf::Bind( Graphics& gfx ) noexcept
{
	// 拿到模型视图矩阵 
	const auto modelView = parent.GetTransformXM() * gfx.GetCamera();
	// 自定义1个结构体;模型矩阵的转置、和MVP的转置
	const Transforms tf =
	{
		DirectX::XMMatrixTranspose(modelView),
		DirectX::XMMatrixTranspose(
			modelView *
			
			gfx.GetProjection()
		)
	};
	pVcbuf->Update( gfx,tf ); // 用矩阵结构体常量更新顶点常数缓存
	pVcbuf->Bind( gfx );	  // 管线上绑定常数缓存	
}

std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::pVcbuf;