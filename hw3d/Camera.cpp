#include "Camera.h"
#include "imgui/imgui.h"

namespace dx = DirectX;

// 拿到//摄像机的位置乘上摄像机旋转自身的方向的矩阵
DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	// 摄像机位置存成1个向量
	const auto pos = dx::XMVector3Transform(
		dx::XMVectorSet(0.0f, 0.0f, -r, 0.0f),
		dx::XMMatrixRotationRollPitchYaw(phi, -theta, 0.0f)
	);
	return dx::XMMatrixLookAtLH(
		pos, //摄像机的位置
		dx::XMVectorZero(),//对准目标的位置
		dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)//摄像机的向上分量
	)	// 乘上摄像机旋转自身的方向
		* dx::XMMatrixRotationRollPitchYaw(pitch, -yaw, roll);
}

void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("R", &r, 0.0f, 80.0f, "%.1f");
		ImGui::SliderAngle("Theta", &theta, -180.0f, 180.0f);
		ImGui::SliderAngle("Phi", &phi, -89.0f, 89.0f);
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void Camera::Reset() noexcept
{
	r = 20.0f;
	theta = 0.0f;
	phi = 0.0f;
	pitch = 0.0f;
	yaw = 0.0f;
	roll = 0.0f;
}