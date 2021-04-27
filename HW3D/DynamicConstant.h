﻿#pragma once
#include "ConditionalNoexcept.h"
#include <cassert>
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <type_traits>
#include <numeric>

#define RESOLVE_BASE(eltype) \
virtual size_t Resolve ## eltype() const noxnd \
{ \
	assert( false && "Cannot resolve to" #eltype ); return 0u; \
}

#define LEAF_ELEMENT(eltype, systype) \
class eltype : public LayoutElement \
{ \
public: \
	using SystemType = systype; \
	size_t Resolve ## eltype() const noxnd override final \
	{ \
		return GetOffsetBegin(); \
	} \
	size_t GetOffsetEnd() const noexcept override final \
	{ \
		return GetOffsetBegin() + ComputeSize(); \
	} \
protected: \
	size_t Finalize( size_t offset_in ) override final \
	{ \
		offset = offset_in; \
		return offset_in + ComputeSize(); \
	} \
	size_t ComputeSize() const noxnd override final \
	{ \
		return sizeof( SystemType ); \
	} \
};

#define REF_CONVERSION(eltype) \
operator eltype::SystemType&() noxnd \
{ \
	return *reinterpret_cast<eltype::SystemType*>(pBytes + offset + pLayout->Resolve ## eltype()); \
} \
eltype::SystemType& operator=( const eltype::SystemType& rhs ) noxnd \
{ \
	return static_cast<eltype::SystemType&>(*this) = rhs; \
}

#define PTR_CONVERSION(eltype) \
operator eltype::SystemType*() noxnd \
{ \
	return &static_cast<eltype::SystemType&>(ref); \
}


namespace Dcb
{
	class Struct;
	class Array;
	class Layout;
	namespace dx = DirectX;

	class LayoutElement
	{
		friend class Layout;
		friend class Array;
		friend class Struct;
	public:
		virtual ~LayoutElement()
		{}

		// [] only works for Structs; access member by name
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
		// T() only works for Arrays; gets the array type layout object
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

		// offset based- functions only work after finalization!
		size_t GetOffsetBegin() const noexcept
		{
			return offset;
		}
		virtual size_t GetOffsetEnd() const noexcept = 0;
		// get size in bytes derived from offsets
		size_t GetSizeInBytes() const noexcept
		{
			return GetOffsetEnd() - GetOffsetBegin();
		}

		// only works for Structs; add LayoutElement
		template<typename T>
		Struct& Add(const std::string& key) noxnd;
		// only works for Arrays; set the type and the # of elements
		template<typename T>
		Array& Set(size_t size) noxnd;

		// returns the value of offset bumped up to the next 16-byte boundary (if not already on one)
		static size_t GetNextBoundaryOffset(size_t offset)
		{
			return offset + (16u - offset % 16u) % 16u;
		}

		RESOLVE_BASE(Matrix)
			RESOLVE_BASE(Float4)
			RESOLVE_BASE(Float3)
			RESOLVE_BASE(Float2)
			RESOLVE_BASE(Float)
			RESOLVE_BASE(Bool)
	protected:
		// sets all offsets for element and subelements, returns offset directly after this element
		virtual size_t Finalize(size_t offset) = 0;
		// computes the size of this element in bytes, considering padding on Arrays and Structs
		virtual size_t ComputeSize() const noxnd = 0;
	protected:
		size_t offset = 0u;
	};


	LEAF_ELEMENT(Matrix, dx::XMFLOAT4X4)
		LEAF_ELEMENT(Float4, dx::XMFLOAT4)
		LEAF_ELEMENT(Float3, dx::XMFLOAT3)
		LEAF_ELEMENT(Float2, dx::XMFLOAT2)
		LEAF_ELEMENT(Float, float)
		LEAF_ELEMENT(Bool, BOOL)


		class Struct : public LayoutElement
	{
	public:
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
			// bump up to next boundary (because structs are multiple of 16 in size)
			return LayoutElement::GetNextBoundaryOffset(elements.back()->GetOffsetEnd());
		}
		template<typename T>
		Struct& Add(const std::string& name) noxnd
		{
			elements.push_back(std::make_unique<T>());
			if (!map.emplace(name, elements.back().get()).second)
			{
				assert(false);
			}
			return *this;
		}
	protected:
		size_t Finalize(size_t offset_in) override final
		{
			assert(elements.size() != 0u);
			offset = offset_in;
			auto offsetNext = offset;
			for (auto& el : elements)
			{
				offsetNext = (*el).Finalize(offsetNext);
			}
			return GetOffsetEnd();
		}
		size_t ComputeSize() const noxnd override final
		{
			// compute offsets of all elements by summing size+padding
			size_t offsetNext = 0u;
			for (auto& el : elements)
			{
				const auto elSize = el->ComputeSize();
				offsetNext += CalculatePaddingBeforeElement(offsetNext, elSize) + elSize;
			}
			// struct size must be multiple of 16 bytes
			return GetNextBoundaryOffset(offsetNext);
		}
	private:
		static size_t CalculatePaddingBeforeElement(size_t offset, size_t size) noexcept
		{
			// advance to next boundary if straddling 16-byte boundary
			if (offset / 16u != (offset + size - 1) / 16u)
			{
				return GetNextBoundaryOffset(offset) - offset;
			}
			return offset;
		}
	private:
		std::unordered_map<std::string, LayoutElement*> map;
		std::vector<std::unique_ptr<LayoutElement>> elements;
	};

	class Array : public LayoutElement
	{
	public:
		size_t GetOffsetEnd() const noexcept override final
		{
			// arrays are not packed in hlsl
			return GetOffsetBegin() + LayoutElement::GetNextBoundaryOffset(pElement->GetSizeInBytes()) * size;
		}
		template<typename T>
		Array& Set(size_t size_in) noxnd
		{
			pElement = std::make_unique<T>();
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
	protected:
		size_t Finalize(size_t offset_in) override final
		{
			assert(size != 0u && pElement);
			offset = offset_in;
			pElement->Finalize(offset_in);
			return GetOffsetEnd();
		}
		size_t ComputeSize() const noxnd override final
		{
			// arrays are not packed in hlsl
			return LayoutElement::GetNextBoundaryOffset(pElement->ComputeSize()) * size;
		}
	private:
		size_t size = 0u;
		std::unique_ptr<LayoutElement> pElement;
	};



	class Layout
	{
	public:
		Layout()
			:
			pLayout(std::make_shared<Struct>())
		{}
		Layout(std::shared_ptr<LayoutElement> pLayout)
			:
			pLayout(std::move(pLayout))
		{}
		LayoutElement& operator[](const char* key)
		{
			assert(!finalized && "cannot modify finalized layout");
			return (*pLayout)[key];
		}
		size_t GetSizeInBytes() const noexcept
		{
			return pLayout->GetSizeInBytes();
		}
		template<typename T>
		LayoutElement& Add(const std::string& key) noxnd
		{
			assert(!finalized && "cannot modify finalized layout");
			return pLayout->Add<T>(key);
		}
		std::shared_ptr<LayoutElement> Finalize()
		{
			pLayout->Finalize(0);
			finalized = true;
			return pLayout;
		}
	private:
		bool finalized = false;
		std::shared_ptr<LayoutElement> pLayout;
	};

	class ElementRef
	{
	public:
		class Ptr
		{
		public:
			Ptr(ElementRef& ref)
				:
				ref(ref)
			{}

			PTR_CONVERSION(Matrix)
				PTR_CONVERSION(Float4)
				PTR_CONVERSION(Float3)
				PTR_CONVERSION(Float2)
				PTR_CONVERSION(Float)
				PTR_CONVERSION(Bool)
		private:
			ElementRef& ref;
		};
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
			// arrays are not packed in hlsl
			const auto elementSize = LayoutElement::GetNextBoundaryOffset(t.GetSizeInBytes());
			return { &t,pBytes,offset + elementSize * index };
		}
		Ptr operator&() noxnd
		{
			return { *this };
		}

		REF_CONVERSION(Matrix)
			REF_CONVERSION(Float4)
			REF_CONVERSION(Float3)
			REF_CONVERSION(Float2)
			REF_CONVERSION(Float)
			REF_CONVERSION(Bool)
	private:
		size_t offset;
		const class LayoutElement* pLayout;
		char* pBytes;
	};

	class Buffer
	{
	public:
		Buffer(Layout& lay)
			:
			pLayout(std::static_pointer_cast<Struct>(lay.Finalize())),
			bytes(pLayout->GetOffsetEnd())
		{}
		ElementRef operator[](const char* key) noxnd
		{
			return { &(*pLayout)[key],bytes.data(),0u };
		}
		const char* GetData() const noexcept
		{
			return bytes.data();
		}
		size_t GetSizeInBytes() const noexcept
		{
			return bytes.size();
		}
		const LayoutElement& GetLayout() const noexcept
		{
			return *pLayout;
		}
		std::shared_ptr<LayoutElement> CloneLayout() const
		{
			return pLayout;
		}
	private:
		std::shared_ptr<Struct> pLayout;
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