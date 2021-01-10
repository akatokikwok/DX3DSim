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
	// �õ�ģ����ͼ���� 
	const auto modelView = parent.GetTransformXM() * gfx.GetCamera();
	// �Զ���1���ṹ��;ģ�;����ת�á���MVP��ת��
	const Transforms tf =
	{
		DirectX::XMMatrixTranspose(modelView),
		DirectX::XMMatrixTranspose(
			modelView *
			
			gfx.GetProjection()
		)
	};
	pVcbuf->Update( gfx,tf ); // �þ���ṹ�峣�����¶��㳣������
	pVcbuf->Bind( gfx );	  // �����ϰ󶨳�������	
}

std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::pVcbuf;