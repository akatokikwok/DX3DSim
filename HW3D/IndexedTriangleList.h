#pragma once
#include <vector>
#include <DirectXMath.h>

template<class T>
class IndexedTriangleList
{
public:
	IndexedTriangleList() = default;
	IndexedTriangleList(std::vector<T> verts_in, std::vector<unsigned short> indices_in)
		:
		vertices(std::move(verts_in)),
		indices(std::move(indices_in))
	{
		assert(vertices.size() > 2);
		assert(indices.size() % 3 == 0);
	}
	// IndexedTriangleList类自带的三维变换方法
	void Transform(DirectX::FXMMATRIX matrix)
	{
		for (auto& v : vertices)//拿到单个顶点
		{
			// 拿取单顶点的位置 加载成1个四维向量pos
			const DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&v.pos);
			// 把结果向量存到三维数里,此处实现了顶点位置按照参数矩阵催动而变化
			DirectX::XMStoreFloat3(
				&v.pos,
				DirectX::XMVector3Transform(pos, matrix)/*向量pos乘参数矩阵返回1个结果向量*/
			);
		}
	}

public:
	std::vector<T> vertices;			// 顶点数组
	std::vector<unsigned short> indices;// 索引数组
};