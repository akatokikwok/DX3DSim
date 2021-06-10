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

	/* 绘制"具备描边特效"的管线绑定物*/
	void DrawOutline(Graphics& gfx) noxnd
	{
		outlining = true;
		// 把每个描边特效绑定物 绑定到管线 并使用 DrawIndexed进行绘制
		for (auto& b : outlineEffect)
		{
			b->Bind(gfx);
		}
		gfx.DrawIndexed(QueryBindable<Bind::IndexBuffer>()->GetCount());//在这里拿到<索引缓存>型绑定物,并进一步拿取数量执行绘制
		outlining = false;
	}
private:
	std::vector<std::shared_ptr<Bind::Bindable>> outlineEffect;// 一组管线绑定物,它们有别于普通的绑定物,它们是描边效果

	bool outlining = false;//字段, 描边特效启用开关

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