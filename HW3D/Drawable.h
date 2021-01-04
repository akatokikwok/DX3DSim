#pragma once
#include "Graphics.h"
#include <DirectXMath.h>

class Bindable;

class Drawable
{
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual ~Drawable() = default;

	/* GetTransformXM() 获取父类的转换Matrix*/
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	/* Draw(gfx)*/
	void Draw(Graphics& gfx) const noexcept/*(!IS_DEBUG)*/;
	/* */
	virtual void Update(float dt) noexcept = 0;
	/* 为binds添加参数bindable对象*/
	void AddBind(std::unique_ptr<Bindable> bind) noexcept/*(!IS_DEBUG)*/;
	/* 不会重复添加缓存索引的方法*/
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept;
private:
	const IndexBuffer* pIndexBuffer = nullptr;	 //字段： 索引缓存;因为考虑到有些bindable型对象会是索引,所以需要索引缓存
	std::vector<std::unique_ptr<Bindable>> binds;//字段: unique_ptr型bindable的绑定数组
};