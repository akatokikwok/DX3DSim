#pragma once
#include "DrawableBase.h"
#include "TestObject.h"
#include "ConstantBuffers.h"

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
	// 生成Box的控制窗口
	void SpawnControlWindow(int id, Graphics& gfx) noexcept;
private:
	// 允许将每个盒子的材质参数保持一致
	void SyncMaterial(Graphics& gfx) noexcept(!IS_DEBUG);
private:
	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color;
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3];
	} materialConstants; 
	// 材质常量缓存
	using MaterialCbuf = PixelConstantBuffer<PSMaterialConstant>;
	// 模型三维
	DirectX::XMFLOAT3X3 mt;
};