#include "Sheet.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include "Plane.h"
#include "Surface.h"
#include "Texture.h"
#include "Sampler.h"


Sheet::Sheet( Graphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist )
	:
	r( rdist( rng ) ),
	droll( ddist( rng ) ),
	dpitch( ddist( rng ) ),
	dyaw( ddist( rng ) ),
	dphi( odist( rng ) ),
	dtheta( odist( rng ) ),
	dchi( odist( rng ) ),
	chi( adist( rng ) ),
	theta( adist( rng ) ),
	phi( adist( rng ) )
{
	namespace dx = DirectX;

	if( !IsStaticInitialized() )
	{
		// 顶点类型里含有位置和纹理坐标
		struct Vertex
		{
			dx::XMFLOAT3 pos;
			struct
			{
				float u;
				float v;
			} tex;
		};
		// 生成模型并自定义填充所有顶点的纹理UV坐标
		auto model = Plane::Make<Vertex>();
		model.vertices[0].tex = { -0.5,-0.5 };//左上角{0,0}
		model.vertices[1].tex = { 1.5,-0.5 };
		model.vertices[2].tex = { -0.5,1.5 };
		model.vertices[3].tex = { 1.5,1.5 };

		// 构造2D纹理和SRV,使用静态的方法是因为希望所有的sheet对象共享1个纹理
		//AddStaticBind( std::make_unique<Texture>( gfx,Surface::FromFile( "Images\\kappa50.png" ) ) );
		AddStaticBind(std::make_unique<Texture>(gfx, Surface::FromFile("Images\\Zouxiaobo.jpg")));

		// 创建顶点缓存
		AddStaticBind( std::make_unique<VertexBuffer>( gfx,model.vertices ) );
		// 创建采样器
		AddStaticBind( std::make_unique<Sampler>( gfx ) );
		// 创造顶点shader
		auto pvs = std::make_unique<VertexShader>( gfx,L"TextureVS.cso" );
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind( std::move( pvs ) );
		// 创建像素shader
		AddStaticBind( std::make_unique<PixelShader>( gfx,L"TexturePS.cso" ) );
		// 创建索引缓存
		AddStaticIndexBuffer( std::make_unique<IndexBuffer>( gfx,model.indices ) );
		// 创建输入布局
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind( std::make_unique<InputLayout>( gfx,ied,pvsbc ) );
		// 创建图元
		AddStaticBind( std::make_unique<Topology>( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind( std::make_unique<TransformCbuf>( gfx,*this ) );
}

void Sheet::Update( float dt ) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX Sheet::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMMatrixRotationRollPitchYaw( pitch,yaw,roll ) *
		dx::XMMatrixTranslation( r,0.0f,0.0f ) *
		dx::XMMatrixRotationRollPitchYaw( theta,phi,chi ) *
		dx::XMMatrixTranslation( 0.0f,0.0f,20.0f );
}
