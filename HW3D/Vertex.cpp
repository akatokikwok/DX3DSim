﻿#include "Vertex.h"

namespace Dvtx
{
	Dvtx::VertexLayout::Element::Element(ElementType type, size_t offset) :
		type(type),
		offset(offset)
	{

	}


	size_t VertexLayout::Element::GetOffsetAfter() const noxnd
	{
		return offset + Size();
	}

	size_t VertexLayout::Element::GetOffset() const
	{
		return offset;
	}

	size_t VertexLayout::Element::Size() const noxnd
	{
		return SizeOf(type);
	}

	constexpr size_t VertexLayout::Element::SizeOf(ElementType type) noxnd
	{
		using namespace DirectX;
		// 使用查找表进行映射内部的成员SysType
		switch (type)
		{
		case Position2D:
			//return sizeof(XMFLOAT2);
			return sizeof(Map<Position2D>::SysType);
		case Position3D:
			//return sizeof(XMFLOAT3);
			return sizeof(Map<Position3D>::SysType);
		case Texture2D:
			//return sizeof(XMFLOAT2);
			return sizeof(Map<Texture2D>::SysType);
		case Normal:
			//return sizeof(XMFLOAT3);
			return sizeof(Map<Normal>::SysType);
		case Float3Color:
			//return sizeof(XMFLOAT3);
			return sizeof(Map<Float3Color>::SysType);
		case Float4Color:
			//return sizeof(XMFLOAT3);
			return sizeof(Map<Float4Color>::SysType);
		case BGRAColor:
			//return sizeof(hw3dexp::BGRAColor);
			return sizeof(Map<BGRAColor>::SysType);
		}
		assert("Invalid element type" && false);
		return 0u;
	}

	Dvtx::VertexLayout::ElementType VertexLayout::Element::GetType() const noexcept
	{
		return type;
	}

	D3D11_INPUT_ELEMENT_DESC VertexLayout::Element::GetDesc() const noxnd
	{
		switch (type)
		{
		case Position2D:
			return GenerateDesc<Position2D>(GetOffset());
		case Position3D:
			return GenerateDesc<Position3D>(GetOffset());
		case Texture2D:
			return GenerateDesc<Texture2D>(GetOffset());
		case Normal:
			return GenerateDesc<Normal>(GetOffset());
		case Float3Color:
			return GenerateDesc<Float3Color>(GetOffset());
		case Float4Color:
			return GenerateDesc<Float4Color>(GetOffset());
		case BGRAColor:
			return GenerateDesc<BGRAColor>(GetOffset());
		}
		assert("Invalid element type" && false);
		return { "INVALID",0,DXGI_FORMAT_UNKNOWN,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 };
	}

	const Dvtx::VertexLayout::Element& VertexLayout::ResolveByIndex(size_t i) const noxnd
	{
		return elements[i];
	}

	Dvtx::VertexLayout& VertexLayout::Append(ElementType type) noxnd
	{
		elements.emplace_back(type, Size());
		return *this;
	}

	size_t VertexLayout::Size() const noxnd
	{
		return elements.empty() ? 0u : elements.back().GetOffsetAfter();
	}

	size_t VertexLayout::GetElementCount() const noexcept
	{
		return elements.size();
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> VertexLayout::GetD3DLayout() const noxnd
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
		desc.reserve(GetElementCount());
		for (const auto& e : elements)
		{
			desc.push_back(e.GetDesc());
		}
		return desc;
	}

	Vertex::Vertex(char* pData, const VertexLayout& layout) :
		pData(pData),
		layout(layout)
	{
		assert(pData != nullptr);
	}

	ConstVertex::ConstVertex(const Vertex& v) :
		vertex(v)
	{

	}

	VertexBuffer::VertexBuffer(VertexLayout layout) :
		layout(std::move(layout))
	{

	}

	const char* VertexBuffer::GetData() const noxnd
	{
		return buffer.data();
	}

	const Dvtx::VertexLayout& VertexBuffer::GetLayout() const noexcept
	{
		return layout;
	}

	size_t VertexBuffer::Size() const noxnd
	{
		return buffer.size() / layout.Size();
	}

	size_t VertexBuffer::SizeBytes() const noxnd
	{
		return buffer.size();
	}

	Dvtx::Vertex VertexBuffer::Back() noxnd
	{
		assert(buffer.size() != 0u);
		return Vertex{ buffer.data() + buffer.size() - layout.Size(),layout };
	}

	Dvtx::ConstVertex VertexBuffer::Back() const noxnd
	{
		return const_cast<VertexBuffer*>(this)->Back();
	}

	Vertex VertexBuffer::operator[](size_t i) noxnd
	{
		assert(i < Size());
		return Vertex{ buffer.data() + layout.Size() * i,layout };
	}

	Dvtx::Vertex VertexBuffer::Front() noxnd
	{
		assert(buffer.size() != 0u);
		return Vertex{ buffer.data(),layout };
	}

	Dvtx::ConstVertex VertexBuffer::Front() const noxnd
	{
		return const_cast<VertexBuffer*>(this)->Front();
	}

	ConstVertex VertexBuffer::operator[](size_t i) const noxnd
	{
		return const_cast<VertexBuffer&>(*this)[i];
	}


}


