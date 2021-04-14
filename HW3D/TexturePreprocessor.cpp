#include "TexturePreprocessor.h"
#include <filesystem>
#include <sstream>
#include "Mesh.h"
#include "ChiliMath.h"

/* 
* 助手方法,仅供其他接口调用; 
* 这个方法就是拿到每个像素并以泛型特殊函数func来处理每一个像素
*/
template<typename F>
inline void TexturePreprocessor::TransformSurface(Surface& surf, F&& func)
{
	const auto width = surf.GetWidth();//拿到图片的宽
	const auto height = surf.GetHeight();//拿到图片的高
	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			const auto n = ColorToVector(surf.GetPixel(x, y));//把每一个像素从颜色转成xmvector型
			surf.PutPixel(x, y, VectorToColor(func(n, x, y)));//给每个像素设置颜色,而颜色则从指定的泛型fun处理法线获得
		}
	}
}

/* 
* 此方法以指定的泛型fun函数指针处理入参纹理，并存到出参传出这张纹理
*/
template<typename F>
inline void TexturePreprocessor::TransformFile(const std::string& pathIn, const std::string& pathOut, F&& func)
{
	auto surf = Surface::FromFile(pathIn);//读入参pathIn路径下的图片

	//const auto nPixels = surf.GetWidth() * surf.GetHeight();
	//const auto pBegin = surf.GetBufferPtr();
	//const auto pEnd = surf.GetBufferPtrConst() + nPixels;
	//for (auto pCurrent = pBegin; pCurrent < pEnd; pCurrent++)
	//{
	//	const auto n = ColorToVector(*pCurrent);
	//	*pCurrent = VectorToColor(func(n));
	//}

	TransformSurface(surf, func);//调用TransformSurface方法，以读到的纹理和入参func来处理这张纹理读每一个像素
	surf.Save(pathOut);//把处理后的纹理当作出参传出去
}

/* 按Y轴翻转入参模型里所有的法线贴图*/
void TexturePreprocessor::FlipYAllNormalMapsInObj(const std::string& objPath)
{
	const auto rootPath = std::filesystem::path{ objPath }.parent_path().string() + "\\";//拿到图片文件的根目录rootPath

	// load scene from .obj file to get our list of normal maps in the materials
	// 使用Assimp加载上述路径模型mesh
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(objPath.c_str(), 0u);
	if (pScene == nullptr)
	{
		throw ModelException(__LINE__, __FILE__, imp.GetErrorString());
	}

	// 遍历mesh里的所有材质并且翻转所有的法线贴图
	for (auto i = 0u; i < pScene->mNumMaterials; i++)
	{
		const auto& mat = *pScene->mMaterials[i];//遍历mesh里的所有材质
		aiString texFileName;
		if (mat.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)//如果能从硬盘里找到法线贴图
		{
			const auto path = rootPath + texFileName.C_Str();//拿到这张法线纹理的名字path
			FlipYNormalMap(path, path);//把path这张法线纹理按与与{1,-1,1,1}相乘
		}
	}
}

/* 
* 助手方法
* 拿到纹理里每个像素与{1,-1,1,1}的分量乘积
*/
void TexturePreprocessor::FlipYNormalMap(const std::string& pathIn, const std::string& pathOut)
{
	
	// function for processing each normal in texture
	using namespace DirectX;
	const auto flipY = XMVectorSet(1.0f, -1.0f, 1.0f, 1.0f);
	// 函数指针ProceessNormal; Lambda表达式 计算任意入参向量与{1,-1,1,1}的分量乘积
	const auto ProcessNormal = [flipY](FXMVECTOR n, int x, int y ) -> XMVECTOR
	{
		return XMVectorMultiply(n, flipY);
	};
	// 指定用ProcessNormal函数指针去操作入参纹理pathIn，并传出处理过的纹理
	TransformFile(pathIn, pathOut, ProcessNormal);
}

void TexturePreprocessor::ValidateNormalMap(const std::string& pathIn, float thresholdMin, float thresholdMax)
{
	OutputDebugStringA(("Validating normal map [" + pathIn + "]\n").c_str());
	// function for processing each normal in texture
	using namespace DirectX;
	auto sum = XMVectorZero();
	//先建立一个函数指针
	const auto ProcessNormal = [thresholdMin, thresholdMax, &sum](FXMVECTOR n, int x, int y) -> XMVECTOR
	{
		const float len = XMVectorGetX(XMVector3Length(n));//拿到向量n长度的x分量;存到浮点数len里
		const float z = XMVectorGetZ(n);//拿到向量n的z分量，存到浮点数z里
		if (len < thresholdMin || len > thresholdMax)//检查len是否在两个入参阈值之外;判定为错误的法线
		{
			XMFLOAT3 vec;
			XMStoreFloat3(&vec, n);
			std::ostringstream oss;
			oss << "Bad normal length: " << len << " at: (" << x << "," << y << ") normal: (" << vec.x << "," << vec.y << "," << vec.z << ")\n";
			OutputDebugStringA(oss.str().c_str());
		}
		if (z < 0.0f)//若向量n的z分量小于0；也输出一些判定消息；即法线n应该指向正方向的z
		{
			XMFLOAT3 vec;
			XMStoreFloat3(&vec, n);
			std::ostringstream oss;
			oss << "Bad normal Z direction at: (" << x << "," << y << ") normal: (" << vec.x << "," << vec.y << "," << vec.z << ")\n";
			OutputDebugStringA(oss.str().c_str());
		}
		sum = XMVectorAdd(sum, n);
		return n;
	};
	// execute the validation for each texel
	auto surf = Surface::FromFile(pathIn);
	TransformSurface(surf, ProcessNormal);
	// output bias
	{
		XMFLOAT2 sumv;
		XMStoreFloat2(&sumv, sum);
		std::ostringstream oss;
		oss << "Normal map biases: (" << sumv.x << "," << sumv.y << ")\n";//拿分量偏差
		OutputDebugStringA(oss.str().c_str());
	}
}

void TexturePreprocessor::MakeStripes(const std::string& pathOut, int size, int stripeWidth)
{
	// 确保纹理维度是2的幂次方,达到mipmap效果
	auto power = log2(size);
	assert(modf(power, &power) == 0.0);
	// make sure stripe width enables at least 2 stripes
	assert(stripeWidth < size / 2);

	Surface s(size, size);
	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			Surface::Color c = { 0,0,0 };
			if ((x / stripeWidth) % 2 == 0)
			{
				c = { 255,255,255 };
			}
			s.PutPixel(x, y, c);
		}
	}
	s.Save(pathOut);
}

DirectX::XMVECTOR TexturePreprocessor::ColorToVector(Surface::Color c) noexcept
{
	using namespace DirectX;
	auto n = XMVectorSet((float)c.GetR(), (float)c.GetG(), (float)c.GetB(), 0.0f);
	const auto all255 = XMVectorReplicate(2.0f / 255.0f);
	n = XMVectorMultiply(n, all255);
	const auto all1 = XMVectorReplicate(1.0f);
	n = XMVectorSubtract(n, all1);
	return n;
}

Surface::Color TexturePreprocessor::VectorToColor(DirectX::FXMVECTOR n) noexcept
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