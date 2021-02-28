#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"
#include "Vertex.h"

namespace Bind
{
	class VertexBuffer : public Bindable
	{
	public:
		//template<class V>
		//VertexBuffer(Graphics& gfx, const std::vector<V>& vertices)
		//	:
		//	stride(sizeof(V))
		//{
		//	INFOMAN(gfx);

		//	D3D11_BUFFER_DESC bd = {};
		//	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		//	bd.Usage = D3D11_USAGE_DEFAULT;
		//	bd.CPUAccessFlags = 0u;
		//	bd.MiscFlags = 0u;
		//	bd.ByteWidth = UINT(sizeof(V) * vertices.size());
		//	bd.StructureByteStride = sizeof(V);
		//	D3D11_SUBRESOURCE_DATA sd = {};
		//	sd.pSysMem = vertices.data();
		//	GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bd, &sd, &pVertexBuffer));
		//}

		// 利用动态反射顶点可选的构造函数，需要 Vertex.h里的VertexBuffer类实例作参数;
		VertexBuffer(Graphics& gfx, const Dvtx::VertexBuffer& vbuf)
			:
			stride((UINT)vbuf.GetLayout().Size())
		{
			INFOMAN(gfx);

			D3D11_BUFFER_DESC bd = {};
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.CPUAccessFlags = 0u;
			bd.MiscFlags = 0u;
			bd.ByteWidth = UINT(vbuf.SizeBytes());// 步长等于 (字节缓存数组大小)
			bd.StructureByteStride = stride;
			D3D11_SUBRESOURCE_DATA sd = {};
			sd.pSysMem = vbuf.GetData();
			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bd, &sd, &pVertexBuffer));
		}

		void Bind(Graphics& gfx) noexcept override;
	protected:
		UINT stride;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	};

}
