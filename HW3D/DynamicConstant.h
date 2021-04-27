﻿#pragma once
#include "ConditionalNoexcept.h"
#include <cassert>
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <type_traits>

#define RESOLVE_BASE(eltype) \
virtual size_t Resolve ## eltype() const noxnd \
{ \
	assert( false && "Cannot resolve to" #eltype ); return 0u; \
}

#define LEAF_ELEMENT(eltype, systype) \
class eltype : public LayoutElement \
{ \
public: \
	using LayoutElement::LayoutElement; \
	size_t Resolve ## eltype() const noxnd override final \
	{ \
		return GetOffsetBegin(); \
	} \
	size_t GetOffsetEnd() const noexcept override final \
	{ \
		return GetOffsetBegin() + sizeof( systype ); \
	} \
};

#define REF_CONVERSION(eltype, systype) \
operator systype&() noxnd \
{ \
	return *reinterpret_cast<systype*>(pBytes + offset + pLayout->Resolve ## eltype()); \
} \
systype& operator=( const systype& rhs ) noxnd \
{ \
	return static_cast<systype&>(*this) = rhs; \
}


namespace Dcb
{
	class Struct;
	class Array;

	namespace dx = DirectX;
	class LayoutElement
	{
	public:
		LayoutElement(size_t offset)
			:
			offset(offset)
		{}

		virtual LayoutElement& operator[](const char*)
		{
			assert(false && "cannot access member on non Struct");
			return *this;
		}
		virtual const LayoutElement& operator[](const char* key) const
		{
			assert(false && "cannot access member on non Struct");
			return *this;
		}
		virtual LayoutElement& T()
		{
			assert(false);
			return *this;
		}
		virtual const LayoutElement& T() const
		{
			assert(false);
			return *this;
		}

		size_t GetOffsetBegin() const noexcept
		{
			return offset;
		}
		virtual size_t GetOffsetEnd() const noexcept = 0;
		size_t GetSizeInBytes() const noexcept
		{
			return GetOffsetEnd() - GetOffsetBegin();
		}

		template<typename T>
		Struct& Add(const std::string& key) noxnd;
		template<typename T>
		Array& Set(size_t size) noxnd;

		RESOLVE_BASE(Float3)
		RESOLVE_BASE(Float)
	private:
		size_t offset;
	};

	LEAF_ELEMENT(Float3, dx::XMFLOAT3)
		LEAF_ELEMENT(Float, float)

		class Struct : public LayoutElement
	{
	public:
		using LayoutElement::LayoutElement;
		LayoutElement& operator[](const char* key) override final
		{
			return *map.at(key);
		}
		const LayoutElement& operator[](const char* key) const override final
		{
			return *map.at(key);
		}
		size_t GetOffsetEnd() const noexcept override final
		{
			return elements.empty() ? GetOffsetBegin() : elements.back()->GetOffsetEnd();
		}
		template<typename T>
		Struct& Add(const std::string& name) noxnd
		{
			elements.push_back(std::make_unique<T>(GetOffsetEnd()));
			if (!map.emplace(name, elements.back().get()).second)
			{
				assert(false);
			}
			return *this;
		}
	private:
		std::unordered_map<std::string, LayoutElement*> map;
		std::vector<std::unique_ptr<LayoutElement>> elements;
	};

	class Array : public LayoutElement
	{
	public:
		using LayoutElement::LayoutElement;
		size_t GetOffsetEnd() const noexcept override final
		{
			assert(pElement);
			return GetOffsetBegin() + pElement->GetSizeInBytes() * size;
		}
		template<typename T>
		Array& Set(size_t size_in) noxnd
		{
			pElement = std::make_unique<T>(GetOffsetBegin());
			size = size_in;
			return *this;
		}
		LayoutElement& T() override final
		{
			return *pElement;
		}
		const LayoutElement& T() const override final
		{
			return *pElement;
		}

	private:
		size_t size = 0u;
		std::unique_ptr<LayoutElement> pElement;
	};

	class ElementRef
	{
	public:
		ElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset)
			:
			offset(offset),
			pLayout(pLayout),
			pBytes(pBytes)
		{}
		ElementRef operator[](const char* key) noxnd
		{
			return { &(*pLayout)[key],pBytes,offset };
		}
		ElementRef operator[](size_t index) noxnd
		{
			const auto& t = pLayout->T();
			return { &t,pBytes,offset + t.GetSizeInBytes() * index };
		}

		REF_CONVERSION(Float3, dx::XMFLOAT3)
		REF_CONVERSION(Float, float)
	private:
		size_t offset;
		const class LayoutElement* pLayout;
		char* pBytes;
	};

	class Buffer
	{
	public:
		Buffer(const Struct& pLayout)
			:
			pLayout(&pLayout),
			bytes(pLayout.GetOffsetEnd())
		{}
		ElementRef operator[](const char* key) noxnd
		{
			return { &(*pLayout)[key],bytes.data(),0u };
		}
	private:
		const class Struct* pLayout;
		std::vector<char> bytes;
	};



    // must come after Definitions of Struct and Array
	template<typename T>
	Struct& LayoutElement::Add(const std::string& key) noxnd
	{
		auto ps = dynamic_cast<Struct*>(this);
		assert(ps != nullptr);
		return ps->Add<T>(key);
	}
	template<typename T>
	Array& LayoutElement::Set(size_t size) noxnd
	{
		auto pa = dynamic_cast<Array*>(this);
		assert(pa != nullptr);
		return pa->Set<T>(size);
	}
}