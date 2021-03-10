#include "PointLight.h"
#include "imgui/imgui.h"

PointLight::PointLight( Graphics& gfx,float radius )
	:
	mesh( gfx,radius ),
	cbuf( gfx )
{
	Reset();
}

void PointLight::SpawnControlWindow() noexcept
{
	if( ImGui::Begin( "Light" ) )
	{
		ImGui::Text( "Position" );
		
		ImGui::SliderFloat("X", &cbData.pos.x, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Y", &cbData.pos.y, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Z", &cbData.pos.z, -60.0f, 60.0f, "%.1f");

		ImGui::Text("Intensity/Color");
		ImGui::SliderFloat("Intensity", &cbData.diffuseIntensity, 0.01f, 20.0f, "%.2f", 2);
		// 给各项参数调色面板
		ImGui::ColorEdit3("Diffuse Color", &cbData.diffuseColor.x);
		ImGui::ColorEdit3("Ambient", &cbData.ambient.x);
		//ImGui::ColorEdit3("Material", &cbData.materialColor.x);

		// 光照衰减参数
		ImGui::Text("Falloff");
		ImGui::SliderFloat("Constant", &cbData.attConst, 0.05f, 10.0f, "%.2f", 4);
		ImGui::SliderFloat("Linear", &cbData.attLin, 0.0001f, 4.0f, "%.4f", 8);
		ImGui::SliderFloat("Quadratic", &cbData.attQuad, 0.0000001f, 10.0f, "%.7f", 10);

		if( ImGui::Button( "Reset" ) )
		{
			Reset();
		}
	}
	ImGui::End();
}

void PointLight::Reset() noexcept
{
	cbData = {
		{ 2.0f,9.0f,-17.0f },
		{ 0.4f,0.4f,0.4f },
		{ 1.0f,1.0f,1.0f },
		1.0f,
		1.0f,
		0.045f,
		0.0075f,
	};
}

void PointLight::Draw( Graphics& gfx ) const noxnd
{
	mesh.SetPos(cbData.pos);
	mesh.Draw( gfx );
}

void PointLight::Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept
{

	/// 目的就是把灯光所有参数更正为 相机坐标系而非原先的世界坐标系
	//cbuf.Update(gfx, cbData);

	auto dataCopy = cbData;// 拿到一份常量结构体
	const auto pos = DirectX::XMLoadFloat3(&cbData.pos);// 取出pos
	DirectX::XMStoreFloat3(&dataCopy.pos, DirectX::XMVector3Transform(pos, view));// 乘以参数视图矩阵后被更新的pos存进结构体里

	cbuf.Update(gfx, dataCopy);
	cbuf.Bind( gfx );
}
