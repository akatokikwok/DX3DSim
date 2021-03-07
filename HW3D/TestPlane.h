#pragma once
#include "Drawable.h"

/* 构造器: TestPlane(Graphics& gfx, float size);*/
class TestPlane : public Drawable
{
public:
	TestPlane(Graphics& gfx, float size);
	/* 设置Testplane模型位置*/
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	/* 设置Testplane模型朝向*/
	void SetRotation(float roll, float pitch, float yaw) noexcept;
	/* 获取Testplane模型Transform*/
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

	void SpawnControlWindow(Graphics& gfx) noexcept;//生成TestPlane模型的IMGUI窗口
private:
	struct PSMaterialConstant
	{
		float specularIntensity = 0.18f;
		float specularPower = 18.0f;
		BOOL normalMappingEnabled = TRUE;
		float padding[1];
	} pmc;//结构体:自定义一些材质常量系数

	DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
};