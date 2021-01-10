#include "TransformCbuf.h"

TransformCbuf::TransformCbuf( Graphics& gfx,const Drawable& parent )
	:
	parent( parent )
{
	if( !pVcbuf )
	{
		pVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>( gfx );
	}
}

void TransformCbuf::Bind( Graphics& gfx ) noexcept
{
	// �õ�ģ�;��� 
	const auto model = parent.GetTransformXM();
	// �Զ���1���ṹ��;ģ�;����ת�á���MVP��ת��
	const Transforms tf =
	{
		DirectX::XMMatrixTranspose( model ),
		DirectX::XMMatrixTranspose(
			model *
			gfx.GetCamera() *
			gfx.GetProjection()
		)
	};
	pVcbuf->Update( gfx,tf ); // �þ���ṹ�峣�����¶��㳣������
	pVcbuf->Bind( gfx );	  // �����ϰ󶨳�������	
}

std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::pVcbuf;