#include "Box.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include "Cube.h"
#include "Sampler.h"


Box::Box( Graphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist )
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
		struct Vertex
		{
			dx::XMFLOAT3 pos;// 持有位置
			dx::XMFLOAT3 n;	// 持有法线	
		};

		// 构造cube模型并添加法线
		auto model = Cube::MakeIndependent<Vertex>();
		model.SetNormalsIndependentFlat();
		// 创建顶点缓存
		AddStaticBind( std::make_unique<VertexBuffer>( gfx,model.vertices ) );
		// 创建采样器
		AddStaticBind(std::make_unique<Sampler>(gfx));
		// 创建顶点着色器
		auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind( std::move( pvs ) );		
		// 创建像素着色器
		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));
		// 创建索引缓存
		AddStaticIndexBuffer( std::make_unique<IndexBuffer>( gfx,model.indices ) );

		#pragma region ver1.24.1弃用
		/*struct PixelShaderConstants
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
		};*/
		#pragma endregion ver1.24.1弃用
		// 自定义灯泡像素常数缓存并创建像素常数缓存
		struct PSLightConstants
		{
			dx::XMVECTOR pos;
		};
		AddStaticBind(std::make_unique<PixelConstantBuffer<PSLightConstants>>(gfx));

		// 创建输入布局;持有位置和法线
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind( std::make_unique<InputLayout>( gfx,ied,pvsbc ) );
		// 创建输入图元
		AddStaticBind( std::make_unique<Topology>( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	}
	else
	{
		SetIndexFromStatic();
	}
	// 创建顶点常数缓存
	AddBind( std::make_unique<TransformCbuf>( gfx,*this ) );
	
	// model deformation transform (per instance, not stored as bind)
	dx::XMStoreFloat3x3(
		&mt,
		dx::XMMatrixScaling( 1.0f,1.0f,bdist( rng ) )
	);
}

void Box::Update( float dt ) noexcept
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
	namespace dx = DirectX;
	return dx::XMLoadFloat3x3( &mt ) *
		dx::XMMatrixRotationRollPitchYaw( pitch,yaw,roll ) *
		dx::XMMatrixTranslation( r,0.0f,0.0f ) *
		dx::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}
