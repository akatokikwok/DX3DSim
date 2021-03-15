#pragma once

#include "Surface.h"
#include <string>
#include <DirectXMath.h>


class TexturePreprocessor
{
public:
	/* 按Y轴翻转入参模型里所有的法线贴图*/
	static void FlipYAllNormalMapsInObj(const std::string& objPath);
	/* FlipYAllNormalMapsInObj调用的助手函数;*/
	static void FlipYNormalMap(const std::string& pathIn, const std::string& pathOut);
private:
	/* */
	template<typename F>
	static void TransformFile(const std::string& pathIn, const std::string& pathOut, F&& func);
	/* */
	static DirectX::XMVECTOR ColorToVector(Surface::Color c) noexcept;
	/* */
	static Surface::Color VectorToColor(DirectX::FXMVECTOR n) noexcept;
};