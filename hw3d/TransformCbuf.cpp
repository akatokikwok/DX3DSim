#include "TransformCbuf.h"

TransformCbuf::TransformCbuf( Graphics& gfx,const Drawable& parent )
	:
	parent( parent )
{
	if( !pVcbuf )
	{
		pVcbuf = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>( gfx );
	}
}

void TransformCbuf::Bind( Graphics& gfx ) noexcept
{
	pVcbuf->Update( gfx,
		/*从模型实例上拿模型矩阵,从图形里拿摄像机视图矩阵和投影矩阵*/
		DirectX::XMMatrixTranspose(
			parent.GetTransformXM() * 
			gfx.GetCamera() *
			gfx.GetProjection()
		)
	);
	pVcbuf->Bind( gfx );
}

std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformCbuf::pVcbuf;