#pragma once
#include "Graphics.h"
#include <DirectXMath.h>

class Bindable;

class Drawable
{
	template<class T>
	friend class DrawableBase;
public:
	Drawable() = default;
	Drawable( const Drawable& ) = delete;
	virtual ~Drawable() = default;
	// 让绘制物 拿取以模型矩阵乘以各自定义的旋转矩阵\或者变换矩阵
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	// 让绘制物 绑定到流水线同时按索引绘制
	void Draw( Graphics& gfx ) const noexcept(!IS_DEBUG);
	// 让绘制物 按时长更新各自的yaw\pith\roll\theta 
	virtual void Update( float dt ) noexcept = 0;
	
protected:

	// 此方法允许查询拿到所需Bindable对象
	template<class T>
	T* QueryBindable() noexcept
	{
		for (auto& pb : binds)
		{
			if (auto pt = dynamic_cast<T*>(pb.get()))
			{
				return pt;
			}
		}
		return nullptr;
	}

	void AddBind( std::unique_ptr<Bindable> bind ) noexcept(!IS_DEBUG);
	void AddIndexBuffer( std::unique_ptr<class IndexBuffer> ibuf ) noexcept(!IS_DEBUG);
private:
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;
private:
	const class IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> binds;
};