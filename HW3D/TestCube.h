#pragma once
#include "Drawable.h"
#include "Bindable.h"
#include "IndexBuffer.h"

/* */
class TestCube : public Drawable
{
public:
	TestCube(Graphics& gfx, float size);
	/* 设置模型位置*/
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	/* 设置模型朝向*/
	void SetRotation(float roll, float pitch, float yaw) noexcept;
	/* 拿到模型的Transform*/
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	/* 生成TestCube模型 的IMGUI窗口*/
	void SpawnControlWindow(Graphics& gfx, const char* name) noexcept;

private:
	std::vector<std::shared_ptr<Bind::Bindable>> outlineEffect;// 一组管线绑定物,它们有别于普通的绑定物,它们是描边效果

	struct PSMaterialConstant
	{
		float specularIntensity = 0.1f;
		float specularPower = 20.0f;
		BOOL normalMappingEnabled = TRUE;
		float padding[1];
	} pmc;

	DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
};