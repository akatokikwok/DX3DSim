#pragma once
#include "Graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffers.h"
#include "ConditionalNoexcept.h"

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
	void Draw( Graphics& gfx ) const noxnd;
	// 更新像素常量缓存(此处为灯泡位置)并同时绑定到管线;
	// 注意要此方法内实现的时候也要供调用者每帧绑定到管线
	void Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept;
private:
	// 点光源的 像素着色器常量缓存结构体
	struct PointLightCBuf
	{
		//// 使用16字节对齐来驱使 编译时候缓存充足不报错
		//alignas(16) DirectX::XMFLOAT3 pos;
		///*float padding;*/

		//alignas(16) DirectX::XMFLOAT3 materialColor;
		//alignas(16) DirectX::XMFLOAT3 ambient;
		//alignas(16) DirectX::XMFLOAT3 diffuseColor;
		//float diffuseIntensity;
		//float attConst;
		//float attLin;
		//float attQuad;

		alignas(16) DirectX::XMFLOAT3 pos;
		alignas(16) DirectX::XMFLOAT3 ambient;
		alignas(16) DirectX::XMFLOAT3 diffuseColor;
		float diffuseIntensity;
		float attConst;
		float attLin;
		float attQuad;
	};
private:
	PointLightCBuf cbData;							 // 常量结构体实例;依次是pos,ambient.diffuse,diffuseIntensity,attconst,attLin,attQuad

	mutable SolidSphere mesh;						 // 灯泡网格体
	mutable Bind::PixelConstantBuffer<PointLightCBuf> cbuf;// 像素常数缓存
};