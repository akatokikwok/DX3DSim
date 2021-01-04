#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>
/*
Drawable尽管持有大量的Bindable对象，但不知道哪个匹配TransCbuf;
基类并没有产生变换矩阵所需要的数据,只有其派生的子类才会有
所以让TransformCbuf不从顶点缓存继承,而是直接从Bindable继承
然后将顶点缓存作为TransCbuf的组成字段
并且TransformCbuf还保存1个指向Drawable的引用,	可以从Drawable里获取旋转matrix以Update(带map催动)更新自己内部顶点的着色器缓存
*/



class TransformCbuf : public Bindable
{
public:
	// 拿到gfx的投影矩阵和Drawable派生类的旋转矩阵来更新常量
	TransformCbuf(Graphics& gfx, const Drawable& parent);
	void Bind(Graphics& gfx) noexcept override;
private:
	// 静态的矩阵型 顶点常量缓存指针
	static std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> pVcbuf;
	//字段: 1个Drawable引用
	const Drawable& parent;
};