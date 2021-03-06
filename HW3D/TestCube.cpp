#include "TestCube.h"
#include "Cube.h"
#include "BindableCommon.h"
#include "TransformCbufDoubleboi.h"
#include "imgui/imgui.h"

TestCube::TestCube(Graphics& gfx, float size)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Cube::MakeIndependentTextured();//构造带贴图的Cube种类模型
	model.Transform(dx::XMMatrixScaling(size, size, size));//设置模型三围为入参
	model.SetNormalsIndependentFlat();//构造模型每个顶点的法向量

	const auto geometryTag = "$cube." + std::to_string(size);
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));//顶点缓存
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));//索引缓存

	AddBind(Texture::Resolve(gfx, "Images\\brickwall.jpg"));//创建漫反射纹理
	AddBind(Texture::Resolve(gfx, "Images\\brickwall_normal.jpg", 1u));//创建法线纹理

	auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));//创建顶点着色器

	AddBind(PixelShader::Resolve(gfx, "PhongPSNormalMap.cso"));//创建像素着色器--法线贴图的

	AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));//利用自定义的材质常量创建像素常量缓存

	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));//创建输入布局

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));//创建图元

	// 创建双常数缓存; 顶点shader绑到插槽0，像素shader绑到插槽2(因为PhongPSNormalMap.hlsl里像素常量缓存cbuffer TransformCBuf位于[2]位置)
	AddBind(std::make_shared<TransformCbufDoubleboi>(gfx, *this, 0u, 2u));
}

void TestCube::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}

void TestCube::SetRotation(float roll, float pitch, float yaw) noexcept
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX TestCube::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw) * DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}

void TestCube::SpawnControlWindow(Graphics& gfx) noexcept
{
	if (ImGui::Begin("Cube"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
		ImGui::Text("Shading");
		bool changed0 = ImGui::SliderFloat("Spec. Int.", &pmc.specularIntensity, 0.0f, 1.0f);
		bool changed1 = ImGui::SliderFloat("Spec. Power", &pmc.specularPower, 0.0f, 100.0f);
		bool checkState = pmc.normalMappingEnabled == TRUE;
		bool changed2 = ImGui::Checkbox("Enable Normal Map", &checkState);
		pmc.normalMappingEnabled = checkState ? TRUE : FALSE;
		if (changed0 || changed1 || changed2)
		{
			QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);
		}
	}
	ImGui::End();
}