#include "Box.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include "DrawableBase.h"
#include "Sphere.h"
#include "Cube.h"

Box::Box(Graphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist
	, std::uniform_real_distribution<float>& bdist
	)
	:
	r(rdist(rng)),
	droll(ddist(rng)),
	dpitch(ddist(rng)),
	dyaw(ddist(rng)),
	dphi(odist(rng)),
	dtheta(odist(rng)),
	dchi(odist(rng)),
	chi(adist(rng)),
	theta(adist(rng)),
	phi(adist(rng))
{
	// 使用简写dx
	namespace dx = DirectX;

	if (!IsStaticInitialized())//若不初始化,则初始化
	{
		// 顶点型
		struct Vertex
		{
			#pragma region ver1.0.22弃用
			/*struct
			{
				float x;
				float y;
				float z;
			} pos;*/
			#pragma endregion ver1.0.22弃用
			dx::XMFLOAT3 pos;
		};
		#pragma region ver1.0.22顶点数组及其绑定弃用
		//// 顶点数组
		//const std::vector<Vertex> vertices =
		//{
		//	{ -1.0f,-1.0f,-1.0f },
		//	{ 1.0f,-1.0f,-1.0f },
		//	{ -1.0f,1.0f,-1.0f },
		//	{ 1.0f,1.0f,-1.0f },
		//	{ -1.0f,-1.0f,1.0f },
		//	{ 1.0f,-1.0f,1.0f },
		//	{ -1.0f,1.0f,1.0f },
		//	{ 1.0f,1.0f,1.0f },
		//};
		//DrawableBase::AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));	
		
		// 构造顶点缓存并添加至binds对象数组
		//AddBind(std::make_unique<VertexBuffer>(gfx, vertices));
		#pragma endregion ver1.0.22顶点数组及其绑定弃用

		// 构造1个几何体，使用该几何模型的make方法创建出顶点数据和索引数据
		const auto model = Cube::Make<Vertex>();
		//创造出顶点缓存并添加至binds对象数组
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));
		#pragma region ver1.0.22弃用
/* 并且同时更改它的缩放,*/
		//auto model = Sphere::Make<Vertex>();
		//model.Transform(dx::XMMatrixScaling(1.0f, 1.0f, 1.2f));
		//AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));
#pragma endregion ver1.0.22弃用

	
		// 构造顶点着色器并添加至binds对象数组
		auto pvs = std::make_unique<VertexShader>(gfx, L"ColorIndexVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));
		// 构造像素着色器并添加至binds对象数组
		AddStaticBind(std::make_unique<PixelShader>(gfx, L"ColorIndexPS.cso"));

		#pragma region ver1.0.22 索引数组及其绑定弃用
		//// 创建索引数组并构造索引缓存,添加至binds对象数组
		//const std::vector<unsigned short> indices =
		//{
		//	0,2,1, 2,3,1,
		//	1,3,5, 3,7,5,
		//	2,6,3, 3,6,7,
		//	4,5,7, 4,7,6,
		//	0,4,2, 2,4,6,
		//	0,1,4, 1,5,4
		//};
		//AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));
		#pragma endregion ver1.0.22 索引数组及其绑定弃用
		// 创造索引缓存并添加至binds对象数组
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

		//  利用PixelShaderConstants的结构体来自定义1个像素shader常量数组
		struct PixelShaderConstants
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			} face_colors[8];
		};
		const PixelShaderConstants cb2 =
		{
			{
				{ 1.0f,1.0f,1.0f },
				{ 1.0f,0.0f,0.0f },
				{ 0.0f,1.0f,0.0f },
				{ 1.0f,1.0f,0.0f },
				{ 0.0f,0.0f,1.0f },
				{ 1.0f,0.0f,1.0f },
				{ 0.0f,1.0f,1.0f },
				{ 0.0f,0.0f,0.0f },
			}
		};
		// 构造Pixel常量缓存
		AddStaticBind(std::make_unique<PixelConstantBuffer<PixelShaderConstants>>(gfx, cb2));
		// 创建输入布局数组并创建输入布局
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			//{ "Color",0,DXGI_FORMAT_R8G8B8A8_UNORM/*UNORM可以让数归一化*/,0,8u,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		// 为了创建布局,需要顶点shader的字节码
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));
		// 构造输入图元
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else//若已经初始化就使用该方法确保Indexbuffer存在值
	{
		DrawableBase::SetIndexFromStatic();
	}

	// 以box本类数据填充构造顶点常量缓存(因为最终调用的还是VertexConstantBuffer构造方法)
	Drawable::AddBind(std::make_unique<TransformCbuf>(gfx, *this));

	// 在这里把实例对象的变换矩阵进行了自定义缩放,以便变化的mt供其他矩阵调用
	dx::XMStoreFloat3x3(
		&mt,
		dx::XMMatrixScaling(1.0f, 1.0f, bdist(rng))
	);
}

void Box::Update(float dt) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	//return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll)/*拿到旋转四元数*/ *
	//	DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
	//	DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
	//	DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f);

	namespace dx = DirectX;
	// 先把mt加载成XMVECTOR,然后就可以乘其他矩阵并最终传递到顶点shader
	return dx::XMLoadFloat3x3(&mt) *
		dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
		dx::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
		dx::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}