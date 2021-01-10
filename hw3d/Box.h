﻿#pragma once
#include "DrawableBase.h"
#include "TestObject.h"

class Box : public TestObject<Box>
{
public:
	Box( Graphics& gfx,std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_real_distribution<float>& bdist,
		DirectX::XMFLOAT3 material
		);
	//// 按时长更新一堆欧拉常数
	//// void Update( float dt ) noexcept override;
	// 拿取模型的变换 
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	//// positional
	//float r;
	//float roll = 0.0f;
	//float pitch = 0.0f;
	//float yaw = 0.0f;
	//float theta;
	//float phi;
	//float chi;
	//// speed (delta/s)
	//float droll;
	//float dpitch;
	//float dyaw;
	//float dtheta;
	//float dphi;
	//float dchi;
	// model transform
	DirectX::XMFLOAT3X3 mt;
};