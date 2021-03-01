#pragma once

#include "Bindable.h"
#include "BindableCodex.h"
#include <memory>
#include <unordered_map>
#include <type_traits>

namespace Bind
{
	class Codex
	{
	public:
		/* 
		单例模式下按给定字符串查找解析相对应的资源;
		由于绑定物可能是着色器、采样器、缓存之类的，参数数量不一定一致;所以采用多参数params		 			
		*/
		template<class T, typename...Params>
		static std::shared_ptr<T> Resolve(Graphics& gfx, Params&&...p) noxnd
		{
			static_assert(std::is_base_of<Bindable, T>::value, "Can only resolve classes derived from Bindable");
			return Get().Resolve_<T>(gfx, std::forward<Params>(p)...);
		}
		
		//* 单例模式下以UID的形式存储指定绑定物资源*/
		//static void Store(std::shared_ptr<Bindable> bind)
		//{
		//	Get().Store_(std::move(bind));
		//}

	private:
		/* 
		按给定字符串查找解析相对应的资源
		由于绑定物可能是着色器、采样器、缓存之类的，参数数量不一定一致;所以采用多参数params	
		*/
		template<class T, typename...Params>
		std::shared_ptr<T> Resolve_(Graphics& gfx, Params&&...p) noxnd
		{
			const auto key = T::GenerateUID(std::forward<Params>(p)...);

			const auto i = binds.find(key);//以关键字查到相对应的绑定物资源
			if (i == binds.end())
			{
				auto bind = std::make_shared<T>(gfx, std::forward<Params>(p)...);// 为可能的绑定物起用构造方法，生成特定的绑定物
				binds[key] = bind;//用生成的绑定物更新相对应的无序MAP
				return bind;
			}
			else
			{
				return std::static_pointer_cast<T>( i->second );//持续查找下一个绑定物资源
			}
		}

		//// 以UID的形式存储指定绑定物资源
		//void Store_(std::shared_ptr<Bindable> bind)
		//{
		//	binds[bind->GetUID()] = std::move(bind);
		//}

		/* 接口：拿取单例模式里Codex本类引用*/
		static Codex& Get()
		{
			static Codex codex;
			return codex;
		}
	private:
		std::unordered_map<std::string, std::shared_ptr<Bindable>> binds; // 以名称-绑定物资源 一一对应的 无序map
	};
}