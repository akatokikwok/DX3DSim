#pragma once
#include "Drawable.h"

// 灯泡网格类;继承自 drawablebase<T>
class SolidSphere : public Drawable
{
public:
	SolidSphere( Graphics& gfx,float radius );
	//void Update( float dt ) noexcept override;
	
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	// 设置灯泡位置
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
private:
	DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };
};