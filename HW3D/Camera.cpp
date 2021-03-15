#include "Camera.h"
#include "imgui/imgui.h"
#include "ChiliMath.h"

namespace dx = DirectX;

Camera::Camera() noexcept
{
	// 所有成员设默认值
	Reset();
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	/*const auto pos = dx::XMVector3Transform(
		dx::XMVectorSet(0.0f, 0.0f, -r, 0.0f),
		dx::XMMatrixRotationRollPitchYaw(phi, -theta, 0.0f)
	);
	return dx::XMMatrixLookAtLH(
		pos, dx::XMVectorZero(),
		dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	) * dx::XMMatrixRotationRollPitchYaw(
		pitch, -yaw, roll
	);*/
//	// 拿到位移矩阵 * 旋转矩阵
//	return dx::XMMatrixTranslation(-pos.x, -pos.y, -pos.z) * dx::XMMatrixRotationRollPitchYaw(-pitch, -yaw, 0.0f);

	using namespace dx;

	const dx::XMVECTOR forwardBaseVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	// 把相机的旋转矩阵存进已有的(0,0,1)向量里得到一个新向量：摄像机的观察方向
	const auto lookVector = XMVector3Transform(forwardBaseVector, XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f)
	);
	// generate camera transform (applied to all objects to arrange them relative
	// to camera position/orientation in world) from cam position and direction
	// camera "top" always faces towards +Y (cannot do a barrel roll)
	const auto camPosition = XMLoadFloat3(&pos);	//相机位置
	const auto camTarget = camPosition + lookVector;//观察目标位置==相机位置+观察normal向量

	return XMMatrixLookAtLH(camPosition, camTarget, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));// 拿取左手系相机观察矩阵
}

void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Position");

		ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		
		ImGui::SliderAngle("Pitch", &pitch, 0.995f * -90.0f, 0.995f * 90.0f);
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
	/*r = 20.0f;
	theta = 0.0f;
	phi = 0.0f;*/
	pos = { -13.5f,6.0f,3.5f };
	pitch = 0.0f;
	yaw = PI / 2.0f;
	//roll = 0.0f;
}

void Camera::Rotate(float dx, float dy) noexcept
{
	yaw = wrap_angle(yaw + dx * rotationSpeed);//yaw值越界处理
	pitch = std::clamp(pitch + dy * rotationSpeed, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f);// pitch角度限定在-90~90之间
}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
	dx::XMStoreFloat3(&translation, 
		// 读取参数XMFLOAT3型变量, 将旋转矩阵*指定缩放矩阵存结果存到参数里(此处是从矩阵里拿到数据存成向量) ,并使用得到的新向量值更新参数
		dx::XMVector3Transform(
			dx::XMLoadFloat3(&translation),
			dx::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f) * dx::XMMatrixScaling(travelSpeed, travelSpeed, travelSpeed)
	));
	// 将更新的矩阵系数的各分量与pos各分量结合
	pos = {
		pos.x + translation.x,
		pos.y + translation.y,
		pos.z + translation.z
	};
}
