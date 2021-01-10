#pragma once
#include "Graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffers.h"

// 点光源类
class PointLight
{
public:
	// 灯泡网格构造; 默认常参为0.5f
	PointLight( Graphics& gfx,float radius = 0.5f );
	// 生成灯泡网格的IMGUI
	void SpawnControlWindow() noexcept;
	// 重置灯泡网格位置
	void Reset() noexcept;
	// 设置灯泡网格位置并按索引绘制该网格
	void Draw( Graphics& gfx ) const noexcept(!IS_DEBUG);
	// 更新像素常量缓存(此处为灯泡位置)并同时绑定到管线;
	// 注意要此方法内实现的时候也要供调用者每帧绑定到管线
	void Bind( Graphics& gfx ) const noexcept;
private:
	// 像素着色器常量缓存结构体
	struct PointLightCBuf
	{
		DirectX::XMFLOAT3 pos;
		float padding;
	};
private:
	DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };;	 // 灯泡网格位置
	mutable SolidSphere mesh;						 // 灯泡网格体
	mutable PixelConstantBuffer<PointLightCBuf> cbuf;// 像素常数缓存
};