#pragma once
#include "Graphics.h"
#include <DirectXMath.h>

class Bindable;

class Drawable
{
	// 让其成为Drawable的友元
	template<class T>
	friend class DrawableBase;

public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual ~Drawable() = default;

	/* GetTransformXM() 获取父类的转换Matrix*/
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	/* Draw(gfx),在流水线上绑定各Bindable实例,同时按照索引数量来绘制他们*/
	void Draw(Graphics& gfx) const noexcept/*(!IS_DEBUG)*/;
	/* */
	virtual void Update(float dt) noexcept = 0;

protected:
	/* 为binds添加参数bindable对象*/
	void AddBind(std::unique_ptr<Bindable> bind) noexcept/*(!IS_DEBUG)*/;
	/* 不会重复添加缓存索引的方法*/
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept;
private:
	// Drawable类需要访问这些Bindable对象,并交由子类重写实现,可以交由派生类DrawableBase去重写
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;
private:
	const IndexBuffer* pIndexBuffer = nullptr;	 //字段： 索引缓存;因为考虑到有些bindable型对象会是索引,所以需要索引缓存
	std::vector<std::unique_ptr<Bindable>> binds;//字段: unique_ptr型bindable的绑定数组
};