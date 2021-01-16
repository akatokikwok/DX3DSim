#include "Pyramid.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include "Cone.h"
#include <array>

Pyramid::Pyramid(Graphics& gfx, std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_int_distribution<int>& tdist)
	:
	TestObject(gfx, rng, adist, ddist, odist, rdist)
{
	namespace dx = DirectX;

	if (!IsStaticInitialized())
	{
		//struct Vertex
		//{
		//	dx::XMFLOAT3 pos;// 位置
		//	dx::XMFLOAT3 n;  // 法线
		//	std::array<char, 4> color;// 四个顶点每个顶点都有自己颜色
		//	char padding;
		//};

		///*auto model = Cone::MakeTesselatedIndependentFaces<Vertex>(tdist(rng));*/

		//// 首先拿到模型的细分;tesselation是细分程度,它是个随机数
		//const auto tesselation = tdist(rng);
		//// 生成不带法线的Cone模型
		//auto model = Cone::MakeTesselatedIndependentFaces<Vertex>(tesselation);
		//// 对每个顶点取颜色
		//for (auto& v : model.vertices)
		//{
		//	v.color = { (char)10,(char)10,(char)255 };
		//}		
		//for (int i = 0; i < tesselation; i++)
		//{
		//	model.vertices[i * 3].color = { (char)255,(char)10,(char)10 };
		//}

		////// 拿到顶点集合里起始元素的引用的颜色(非迭代器)
		////model.vertices.front().color = { (char)255,(char)20,(char)20 }; 
		//// 沿z方向挤压网格
		//model.Transform(dx::XMMatrixScaling(1.0f, 1.0f, 0.7f));
		//// 为模型构造法线
		//model.SetNormalsIndependentFlat();

		//AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

		auto pvs = std::make_unique<VertexShader>(gfx, L"BlendedPhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));
		AddStaticBind(std::make_unique<PixelShader>(gfx, L"BlendedPhongPS.cso"));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		struct PSMaterialConstant
		{
			float specularIntensity = 0.6f;
			float specularPower = 30.0f;
			float padding[2];
		} colorConst;
		AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, colorConst, 1u));
	}
	
	struct Vertex
	{
		dx::XMFLOAT3 pos;
		dx::XMFLOAT3 n;
		std::array<char, 4> color;
		char padding;
	};
	// 创建细分圆锥模型
	const auto tesselation = tdist(rng);
	auto model = Cone::MakeTesselatedIndependentFaces<Vertex>(tesselation);
	// 给每个顶点颜色赋值
	for (auto& v : model.vertices)
	{
		v.color = { (char)10,(char)10,(char)255 };
	}
	// 给每一层细分的颜色进行赋值
	for (int i = 0; i < tesselation; i++)
	{
		model.vertices[i * 3].color = { (char)255,(char)10,(char)10 };
	}
	// 压缩1下z轴
	model.Transform(dx::XMMatrixScaling(1.0f, 1.0f, 0.7f));
	// 设置法线
	model.SetNormalsIndependentFlat();

	AddBind(std::make_unique<VertexBuffer>(gfx, model.vertices));
	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));



	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}
