#include "SolidSphere.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"
#include "Sphere.h"
#include "Vertex.h"


SolidSphere::SolidSphere( Graphics& gfx,float radius )
{
	using namespace Bind;
	namespace dx = DirectX;

	//if( !IsStaticInitialized() )

	/*struct Vertex
	{*/
		#pragma region ver1.0.39弃用
		//auto model = Sphere::Make<Vertex>();
		//model.Transform(dx::XMMatrixScaling(radius, radius, radius));
		//AddBind(std::make_unique<VertexBuffer>(gfx, model.vertices));
		//AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

		//auto pvs = std::make_unique<VertexShader>(gfx, L"SolidVS.cso");
		//auto pvsbc = pvs->GetBytecode();
		//AddStaticBind(std::move(pvs));

		//AddStaticBind(std::make_unique<PixelShader>(gfx, L"SolidPS.cso"));

		//struct PSColorConstant
		//{
		//	dx::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
		//	float padding;
		//} colorConst;
		//AddStaticBind(std::make_unique<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst));

		//const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		//{
		//	{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		//};
		//AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

		//AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
		#pragma endregion ver1.0.39弃用

		/*dx::XMFLOAT3 pos;
	};*/


	auto model = Sphere::Make(); //构造一个球形物
	model.Transform(dx::XMMatrixScaling(radius, radius, radius));

	const auto geometryTag = "$sphere." + std::to_string(radius);
	AddBind(Bind::VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
	AddBind(Bind::IndexBuffer::Resolve(gfx, geometryTag, model.indices));


	auto pvs = Bind::VertexShader::Resolve(gfx, "SolidVS.cso");
	auto pvsbc = static_cast<Bind::VertexShader&>(*pvs).GetBytecode();
	AddBind(std::move(pvs));

	AddBind(Bind::PixelShader::Resolve(gfx, "SolidPS.cso"));

	struct PSColorConstant
	{		
		dx::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
		float padding;
	} colorConst;
	AddBind(Bind::PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst));

	//const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	//{
	//	{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
	//};
	//AddBind(std::make_shared<InputLayout>(gfx, ied, pvsbc));

	AddBind(Bind::InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

	AddBind(Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}

void SolidSphere::SetPos( DirectX::XMFLOAT3 pos ) noexcept
{
	this->pos = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( pos.x,pos.y,pos.z );
}
