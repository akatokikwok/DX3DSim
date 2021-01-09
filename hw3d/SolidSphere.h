#pragma once
#include "DrawableBase.h"
 
// 灯的绘制物
class SolidSphere : public DrawableBase<SolidSphere>
{
public:
	SolidSphere(Graphics& gfx, float radius);
	void Update(float dt) noexcept override;	
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	// 以参数更新点光源位置
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
private:
	DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };
};