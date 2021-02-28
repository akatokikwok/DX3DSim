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

		// 可选构造方法--VertexBuffer(Graphics& gfx, const std::string& tag, const Dvtx::VertexBuffer& vbuf);
		VertexBuffer(Graphics& gfx, const std::string& tag, const Dvtx::VertexBuffer& vbuf);

		// 利用动态反射顶点可选的构造函数，需要 Vertex.h里的VertexBuffer类实例作参数;
		VertexBuffer(Graphics& gfx, const Dvtx::VertexBuffer& vbuf);

		void Bind(Graphics& gfx) noexcept override;

		// 三件套=================================================
		static std::shared_ptr<Bindable> Resolve(Graphics& gfx, const std::string& tag, const Dvtx::VertexBuffer& vbuf);
		template<typename...Ignore>
		static std::string GenerateUID(const std::string& tag, Ignore&&...ignore)
		{
			return GenerateUID_(tag);
		}
		std::string GetUID() const noexcept override;

	private:
		static std::string GenerateUID_(const std::string& tag);

	protected:
		std::string tag;
		UINT stride;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	};

}
