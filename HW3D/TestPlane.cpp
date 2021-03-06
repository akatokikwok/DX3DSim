﻿#include "TestPlane.h"
#include "Plane.h"
#include "BindableCommon.h"
#include "imgui/imgui.h"
#include "TransformCbufDoubleboi.h"

TestPlane::TestPlane( Graphics& gfx,float size,DirectX::XMFLOAT4 color )
	:
	pmc({ color })
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Plane::Make();//构造Plane种类模型
	model.Transform(dx::XMMatrixScaling(size, size, 1.0f));

	const auto geometryTag = "$plane." + std::to_string(size);
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));//创建顶点缓存
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));//创建索引缓存

	//AddBind(Texture::Resolve(gfx, "Images\\brickwall.jpg",		  0u));//创建漫反射纹理
	
	/* 
	* PS!!!  这里读取的是brickwall_normal_obj.png
	* 原因是因为PSNormalMapObject.hlsl里使用的是逻辑是先采样object space里的图片（objectNormal = normalSample * 2.0f - 1.0f）
	* 之后通过与modelView矩阵相乘才得到view space的法线
	* 它是切线空间的法线贴图;由于对应shader里Texture2D nmap : register(t2);
	* 所以这里插槽也要匹配，设为2号槽位		
	*/
	//AddBind(Texture::Resolve(gfx, "Images\\brickwall_normal_obj.png", 2u));

	//auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
	
	auto pvs = VertexShader::Resolve( gfx,"SolidVS.cso" );
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));//创建顶点着色器

	//AddBind(PixelShader::Resolve(gfx, "PhongPSNormalMapObject.cso"));//创建像素shader--(带法线但不带切线)
	AddBind( PixelShader::Resolve( gfx,"SolidPS.cso" ) );

	/*struct PSMaterialConstant
	{
		float specularIntensity = 0.1f;
		float specularPower = 20.0f;
		float padding[2];
	} pmc;*///转移到头文件去
	AddBind( std::make_shared<PixelConstantBuffer<PSMaterialConstant>>( gfx,pmc,1u ) );//利用自定义的材质常量创建像素常量缓存;注意这里用的是makeshared,因为之后场景里放一块红玻璃板一块蓝色玻璃板,所以要给各自独立的材质常数

	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));//创建输入布局

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));//创建图元

	//AddBind(std::make_shared<TransformCbuf>(gfx, *this));//创建顶点常量缓存
	
	//创建双常数缓存; 顶点shader绑到插槽0，像素shader绑到插槽2(因为PhongPSNormalMapObject.hlsl里像素常量缓存cbuffer TransformCBuf位于[2]位置)
	//AddBind(std::make_shared<TransformCbufDoubleboi>(gfx, *this, 0u, 2u));

	AddBind(std::make_shared<Bind::TransformCbuf>(gfx, *this, 0u));

	AddBind(Bind::Blender::Resolve(gfx, true, 0.5f)); //让TestPlane开启阿尔法混合,混合因子暂设为0.5

	AddBind(Bind::Rasterizer::Resolve(gfx, true));
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

void TestPlane::SpawnControlWindow( Graphics& gfx,const std::string& name ) noexcept
{
	if (ImGui::Begin(name.c_str()))
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
		//bool changed0 = ImGui::SliderFloat("Spec. Intensity.", &pmc.specularIntensity, 0.0f, 1.0f);
		//bool changed1 = ImGui::SliderFloat("Spec. Power", &pmc.specularPower, 0.0f, 100.0f);
		//bool checkState = pmc.normalMappingEnabled == TRUE;
		//bool changed2 = ImGui::Checkbox("Enable Normal Map", &checkState);
		//pmc.normalMappingEnabled = checkState ? TRUE : FALSE;
		//if (changed0 || changed1 || changed2)
		//{
		//	Drawable::QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update(gfx, pmc);//只要满足任一条件 都应该及时应用材质常数的变更
		//}
		auto pBlender = QueryBindable<Bind::Blender>();
		float factor = pBlender->GetFactor();
		ImGui::SliderFloat("Translucency", &factor, 0.0f, 1.0f);
		pBlender->SetFactor(factor);
	}
	ImGui::End();
}