#pragma once

#include "Surface.h"
#include "ChiliMath.h"
#include <string>
#include <DirectXMath.h>

/// 只是一个集合，并不是传统意义上的类，它包含一些静态方法
class NormalMapTwerker
{
public:
	/* 把指定图片的像素按照x轴都旋转180度*/
	static void RotateXAxis180(const std::string& pathIn, const std::string& pathOut)
	{
		using namespace DirectX;
		const auto rotation = XMMatrixRotationX(PI);// 绕X轴旋转180°
		auto sIn = Surface::FromFile(pathIn); //读指定路径下的图片

		const auto nPixels = sIn.GetWidth() * sIn.GetHeight();//拿到指定图片像素
		const auto pBegin = sIn.GetBufferPtr();
		const auto pEnd = sIn.GetBufferPtrConst() + nPixels;
		for (auto pCurrent = pBegin; pCurrent < pEnd; pCurrent++)
		{
			auto n = ColorToVector(*pCurrent);
			n = XMVector3Transform(n, rotation);
			*pCurrent = VectorToColor(n);
		}

		sIn.Save(pathOut);
	}
	/* 把指定图片的像素按照x轴都旋转180度*/
	static void RotateXAxis180(const std::string& pathIn)
	{
		return RotateXAxis180(pathIn, pathIn);
	}
private:
	/* Color型转换为VECTOR型*/
	static DirectX::XMVECTOR ColorToVector(Surface::Color c)
	{
		using namespace DirectX;
		auto n = XMVectorSet((float)c.GetR(), (float)c.GetG(), (float)c.GetB(), 0.0f);
		const auto all255 = XMVectorReplicate(2.0f / 255.0f);
		n = XMVectorMultiply(n, all255);
		const auto all1 = XMVectorReplicate(1.0f);
		n = XMVectorSubtract(n, all1);
		return n;
	}
	/* XMVECTOR型转为Color型*/
	static Surface::Color VectorToColor(DirectX::FXMVECTOR n)
	{
		using namespace DirectX;
		const auto all1 = XMVectorReplicate(1.0f);
		XMVECTOR nOut = XMVectorAdd(n, all1);
		const auto all255 = XMVectorReplicate(255.0f / 2.0f);
		nOut = XMVectorMultiply(nOut, all255);
		XMFLOAT3 floats;
		XMStoreFloat3(&floats, nOut);
		return {
			(unsigned char)round(floats.x),
			(unsigned char)round(floats.y),
			(unsigned char)round(floats.z),
		};
	}
};