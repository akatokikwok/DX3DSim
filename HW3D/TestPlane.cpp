#include "TestPlane.h"
#include "Plane.h"
#include "BindableCommon.h"

TestPlane::TestPlane(Graphics& gfx, float size)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Plane::Make();//构造模型
	model.Transform(dx::XMMatrixScaling(size, size, 1.0f));

	const auto geometryTag = "$plane." + std::to_string(size);
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));//创建顶点缓存
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));//创建索引缓存

	AddBind(Texture::Resolve(gfx, "Images\\brickwall.jpg"));//创建纹理

	auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));//创建顶点着色器

	AddBind(PixelShader::Resolve(gfx, "PhongPS.cso"));//创建像素着色器

	struct PSMaterialConstant
	{
		float specularIntensity = 0.8f;
		float specularPower = 45.0f;
		float padding[2];
	} pmc;
	AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));//利用自定义的材质常量创建像素常量缓存

	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));//创建输入布局

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));//创建图元

	AddBind(std::make_shared<TransformCbuf>(gfx, *this));//创建顶点常量缓存
}

void TestPlane::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}

void TestPlane::SetRotation(float roll, float pitch, float yaw) noexcept
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX TestPlane::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw) * DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}