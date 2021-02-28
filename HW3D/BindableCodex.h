#pragma once

#include "Bindable.h"
#include "BindableCodex.h"
#include <memory>
#include <unordered_map>

namespace Bind
{
	class Codex
	{
	public:
		/* 单例模式下按给定字符串查找解析相对应的资源*/
		static std::shared_ptr<Bindable> Resolve(const std::string& key) noxnd
		{
			return Get().Resolve_(key);
		}
		/* 单例模式下以UID的形式存储指定绑定物资源*/
		static void Store(std::shared_ptr<Bindable> bind)
		{
			Get().Store_(std::move(bind));
		}
	private:
		/* 按给定字符串查找解析相对应的资源*/
		std::shared_ptr<Bindable> Resolve_(const std::string& key) const noxnd
		{
			auto i = binds.find(key);//以关键字查到相对应的绑定物资源
			if (i == binds.end())
			{
				return {};
			}
			else
			{
				return i->second;//持续查找下一个资源
			}
		}

		// 以UID的形式存储指定绑定物资源
		void Store_(std::shared_ptr<Bindable> bind)
		{
			binds[bind->GetUID()] = std::move(bind);
		}
		// 接口：拿取单例模式里Codex本类引用
		static Codex& Get()
		{
			static Codex codex;
			return codex;
		}
	private:
		std::unordered_map<std::string, std::shared_ptr<Bindable>> binds; // 以名称-绑定物资源 一一对应的 无序map
	};
}