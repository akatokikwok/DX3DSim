#pragma once
#include "Drawable.h"

/* 继承自Drawable的Box类*/
class Box : public Drawable
{
public:
	Box(Graphics& gfx, 
		std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,//随机浮点数
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist);
	/* 负责更新一大堆浮点参数*/
	void Update(float dt) noexcept override;
	/* 负责处理这些参数为DX矩阵格式*/
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	// positional
	float r;
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float theta;
	float phi;
	float chi;

	// speed (delta/s),与box运动相关
	float droll;
	float dpitch;
	float dyaw;
	float dtheta;
	float dphi;
	float dchi;
};