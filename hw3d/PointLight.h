#pragma once
#include "Graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffers.h"

// 点光源类
class PointLight
{
public:
	PointLight(Graphics& gfx, float radius = 0.5f);
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	// 设置灯泡位置并绘制灯泡网格
	void Draw(Graphics& gfx) const noexcept(!IS_DEBUG);
	// 更新点光源的常数缓存
	void Bind(Graphics& gfx) const noexcept;
private:
	// 灯的像素常数缓存结构体
	struct PointLightCBuf
	{
		DirectX::XMFLOAT3 pos;
		float padding;
	};
private:
	DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };;	// 灯位置
	mutable SolidSphere mesh;						// 仿灯泡形状的网格
	mutable PixelConstantBuffer<PointLightCBuf> cbuf;// 灯的像素常数缓存
};