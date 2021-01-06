#pragma once
#include "Graphics.h"

class Camera
{
public:
	拿到//摄像机的位置乘上摄像机旋转自身的方向的矩阵
	DirectX::XMMATRIX GetMatrix() const noexcept;
	// 摄像机的控制菜单
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
private:
	float r = 20.0f; // 离观察中心的距离
	float theta = 0.0f;// 绕原点yaw旋转的角度
	float phi = 0.0f;// 绕原点pitch旋转的角度
	float pitch = 0.0f;// 相机自身pitch观察外界
	float yaw = 0.0f;// 相机自身yaw观察外界
	float roll = 0.0f;// 相机自身roll观察外界
};