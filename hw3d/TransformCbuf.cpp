#include "TransformCbuf.h"

TransformCbuf::TransformCbuf( Graphics& gfx,const Drawable& parent )
	:
	parent( parent )
{
	if( !pVcbuf )
	{
		pVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(gfx);
	}
}

void TransformCbuf::Bind( Graphics& gfx ) noexcept
{
	const auto ModelMatrix = parent.GetTransformXM();// 拿到模型矩阵
	const Transforms tf =
	{
		// 以上述模型矩阵的转置去填充MVP
		DirectX::XMMatrixTranspose(ModelMatrix),
		// 以模型矩阵*图形类的摄像机*投影矩阵的转置更新 M
		DirectX::XMMatrixTranspose(
			ModelMatrix *
			gfx.GetCamera() *
			gfx.GetProjection()
		)
	};

	pVcbuf->Update(gfx, tf);// 以Transform结构体常量更新顶点常量缓存
	pVcbuf->Bind(gfx);		// 顶点常数缓存绑定到管线上
	
}

std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::pVcbuf;