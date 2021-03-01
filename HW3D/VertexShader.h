#pragma once
#include "Bindable.h"

namespace Bind
{
	class VertexShader : public Bindable
	{
	public:
		VertexShader(Graphics& gfx, const std::string& path);
		void Bind(Graphics& gfx) noexcept override;
		ID3DBlob* GetBytecode() const noexcept;
		// 以路径名解析顶点着色器
		static std::shared_ptr<VertexShader> Resolve(Graphics& gfx, const std::string& path);
		//运行时候获取路径名并拼接字符串
		static std::string GenerateUID(const std::string& path);
		//拿到拼接字符串之后的path名
		std::string GetUID() const noexcept override;
	
	protected:
		std::string path;
		Microsoft::WRL::ComPtr<ID3DBlob> pBytecodeBlob;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	};
}
