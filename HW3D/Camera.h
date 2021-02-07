#pragma once
#include "Graphics.h"

class Camera
{
public:
	Camera() noexcept;
	// 拿到相机变换矩阵
	DirectX::XMMATRIX GetMatrix() const noexcept;
	// 构建相机的IMGUI控制窗口
	void SpawnControlWindow() noexcept;
	// 重置所有相机参数
	void Reset() noexcept;
	// 设置摄像机的旋转朝向
	void Rotate(float dx, float dy) noexcept;
	// 
	void Translate(DirectX::XMFLOAT3 translation) noexcept;

private:
	DirectX::XMFLOAT3 pos; //相机位置
	float pitch;// 当前相机pitch角度
	float yaw;// 当前相机yaw角度
	static constexpr float travelSpeed = 12.0f; // 相机行进速率,默认12
	static constexpr float rotationSpeed = 0.004f;// 相机旋转速率,默认0.004
};