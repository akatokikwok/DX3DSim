#include "TestPlane.h"
#include "Plane.h"
#include "BindableCommon.h"
#include "imgui/imgui.h"
#include "TransformCbufDoubleboi.h"

TestPlane::TestPlane(Graphics& gfx, float size)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Plane::Make();//构造Plane种类模型
	model.Transform(dx::XMMatrixScaling(size, size, 1.0f));

	const auto geometryTag = "$plane." + std::to_string(size);
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));//创建顶点缓存
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));//创建索引缓存

	AddBind(Texture::Resolve(gfx, "Images\\brickwall.jpg",		  0u));//创建漫反射纹理
	AddBind(Texture::Resolve(gfx, "Images\\brickwall_normal.jpg", 1u));//创建法线纹理

	auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));//创建顶点着色器

	AddBind(PixelShader::Resolve(gfx, "PhongPSNormalMap.cso"));//创建像素着色器--法线贴图的

	/*struct PSMaterialConstant
	{
		float specularIntensity = 0.1f;
		float specularPower = 20.0f;
		float padding[2];
	} pmc;*///转移到头文件去
	AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));//利用自定义的材质常量创建像素常量缓存

	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));//创建输入布局

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));//创建图元

	AddBind(std::make_shared<TransformCbuf>(gfx, *this));//创建顶点常量缓存
	
	// 创建常数缓存; 顶点shader绑到插槽0，像素shader绑到插槽2(因为PhongPSNormalMap.hlsl里像素常量缓存cbuffer TransformCBuf位于[2]位置)
	//AddBind(std::make_shared<TransformCbufDoubleboi>(gfx, *this, 0u, 2u));
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

void TestPlane::SpawnControlWindow(Graphics& gfx) noexcept
{
	if (ImGui::Begin("Plane"))
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
		bool changed0 = ImGui::SliderFloat("Spec. Intensity.", &pmc.specularIntensity, 0.0f, 1.0f);
		bool changed1 = ImGui::SliderFloat("Spec. Power", &pmc.specularPower, 0.0f, 100.0f);
		bool checkState = pmc.normalMappingEnabled == TRUE;
		bool changed2 = ImGui::Checkbox("Enable Normal Map", &checkState);
		pmc.normalMappingEnabled = checkState ? TRUE : FALSE;
		if (changed0 || changed1 || changed2)
		{
			Drawable::QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);//只要满足任一条件 都应该及时应用材质常数的变更
		}
	}
	ImGui::End();
}