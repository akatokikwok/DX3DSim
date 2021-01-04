﻿#include "Box.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include "DrawableBase.h"

Box::Box(Graphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist)
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
	if (!IsStaticInitialized())//若不初始化,则初始化
	{
		// 顶点型
		struct Vertex
		{
			struct
			{
				float x;
				float y;
				float z;
			} pos;
		};
		// 顶点数组
		const std::vector<Vertex> vertices =
		{
			{ -1.0f,-1.0f,-1.0f },
			{ 1.0f,-1.0f,-1.0f },
			{ -1.0f,1.0f,-1.0f },
			{ 1.0f,1.0f,-1.0f },
			{ -1.0f,-1.0f,1.0f },
			{ 1.0f,-1.0f,1.0f },
			{ -1.0f,1.0f,1.0f },
			{ 1.0f,1.0f,1.0f },
		};
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));
		// 构造顶点缓存并添加至binds对象数组
		AddBind(std::make_unique<VertexBuffer>(gfx, vertices));
		// 构造顶点着色器并添加至binds对象数组
		auto pvs = std::make_unique<VertexShader>(gfx, L"VertexShader.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));
		// 构造像素着色器并添加至binds对象数组
		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PixelShader.cso"));
		// 创建索引数组并构造索引缓存,添加至binds对象数组
		const std::vector<unsigned short> indices =
		{
			0,2,1, 2,3,1,
			1,3,5, 3,7,5,
			2,6,3, 3,6,7,
			4,5,7, 4,7,6,
			0,4,2, 2,4,6,
			0,1,4, 1,5,4
		};
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));
		//  像素常量数组和 顶点常量数组
		struct ConstantBuffer2
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			} face_colors[6];
		};
		const ConstantBuffer2 cb2 =
		{
			{
				{ 1.0f,0.0f,1.0f },
				{ 1.0f,0.0f,0.0f },
				{ 0.0f,1.0f,0.0f },
				{ 0.0f,0.0f,1.0f },
				{ 1.0f,1.0f,0.0f },
				{ 0.0f,1.0f,1.0f },
			}
		};
		// 构造像素常量缓存
		AddStaticBind(std::make_unique<PixelConstantBuffer<ConstantBuffer2>>(gfx, cb2));
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

	// 以box本类数据填充构造常量缓存
	Drawable::AddBind(std::make_unique<TransformCbuf>(gfx, *this));
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
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll)/*拿到旋转四元数*/ *
		DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
		DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}