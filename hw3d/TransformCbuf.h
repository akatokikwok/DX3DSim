#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>

class TransformCbuf : public Bindable
{
public:
	/* */
	TransformCbuf( Graphics& gfx,const Drawable& parent );
	/* 自构造1个常熟结构体 去更新并绑定顶点常数缓存*/
	void Bind( Graphics& gfx ) noexcept override;

private:
	// 1个含有MVP矩阵和模型矩阵的结构体
	struct Transforms
	{
		DirectX::XMMATRIX modelViewProj;
		DirectX::XMMATRIX model;
	};
private:
	static std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;// 字段:顶点常数缓存
	const Drawable& parent;											// 上一级的绘制物实例引用
};