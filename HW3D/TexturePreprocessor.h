#pragma once

#include "Surface.h"
#include <string>
#include <DirectXMath.h>

/*
负责处理法线贴图翻转的类
*/
class TexturePreprocessor
{
public:
	/* 按Y轴翻转入参模型里所有的法线贴图*/
	static void FlipYAllNormalMapsInObj(const std::string& objPath);
	/* FlipYAllNormalMapsInObj调用的助手函数;*/
	static void FlipYNormalMap(const std::string& pathIn, const std::string& pathOut);
	/* 将入参纹理进行在两个阈值之间的检查*/
	static void ValidateNormalMap(const std::string& pathIn, float thresholdMin, float thresholdMax);
	/* */
	static void MakeStripes(const std::string& pathOut, int size, int stripeWidth);
private:
	/*
	* 此方法以指定的泛型fun函数指针处理入参纹理，并存到出参传出这张纹理
	*/
	template<typename F>
	static void TransformFile(const std::string& pathIn, const std::string& pathOut, F&& func);
	/*
	* 助手方法,仅供其他接口调用;
	* 这个方法就是拿到每个像素并以泛型特殊函数func来处理每一个像素
	*/
	template<typename F>
	static void TransformSurface(Surface& surf, F&& func);	
	/* 图片纹理转换成向量*/
	static DirectX::XMVECTOR ColorToVector(Surface::Color c) noexcept;
	/* 向量转换成颜色纹理*/
	static Surface::Color VectorToColor(DirectX::FXMVECTOR n) noexcept;
};