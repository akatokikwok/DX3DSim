#pragma once
#include "Bindable.h"
#include "Vertex.h"

namespace Bind
{
	/* 构造方法--InputLayout(Graphics& gfx, Dvtx::VertexLayout layout,  ID3DBlob* pVertexShaderBytecode);*/
	class InputLayout : public Bindable
	{
	public:
		InputLayout(Graphics& gfx, Dvtx::VertexLayout layout,  ID3DBlob* pVertexShaderBytecode);
		void Bind(Graphics& gfx) noexcept override;
		// 三件套================================
		static std::shared_ptr<InputLayout> Resolve(Graphics& gfx, const Dvtx::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode);
		static std::string GenerateUID(const Dvtx::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode = nullptr);
		std::string GetUID() const noexcept override;
	protected:
		Dvtx::VertexLayout layout;//反射系统输入布局
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	};
}