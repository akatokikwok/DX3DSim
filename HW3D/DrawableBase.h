#pragma once
#include "Drawable.h"
#include "IndexBuffer.h"
#include "Bindable.h"

template<class T>
class DrawableBase : public Drawable
{
protected:
	 static bool IsStaticInitialized()  noexcept
	{
		return !staticBinds.empty();
	}

	// 更改为静态添加绑定
	 static void AddStaticBind(std::unique_ptr<Bindable> bind) noexcept
	{
		assert("*Must* use AddStaticIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
		staticBinds.push_back(std::move(bind));
	}

	void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noexcept
	{
		assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
		pIndexBuffer = ibuf.get();
		staticBinds.push_back(std::move(ibuf));
	}

	void SetIndexFromStatic() noexcept/*(!IS_DEBUG)*/
	{
		assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
		for (const auto& b : staticBinds)//先拿到所有绑定对象(基本上都是从Bindable实例)
		{
			if (const auto p = dynamic_cast<IndexBuffer*>(b.get()))//若是索引缓存指针
			{
				pIndexBuffer = p;
				return;
			}
		}
		assert("Failed to find index buffer in static binds" && pIndexBuffer != nullptr);
	}

private:
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override
	{
		return staticBinds;
	}
private:
	static std::vector<std::unique_ptr<Bindable>> staticBinds;
};

// 需要在外部再声明一下静态绑定对象
template<class T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::staticBinds;