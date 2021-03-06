#pragma once
#include "Vertex.h"
#include <vector>
#include <DirectXMath.h>

//template<class T>
class IndexedTriangleList
{
public:
	IndexedTriangleList() = default;

	/* 构造器: 入参顶点数据和入参顶点索引数据更新成员;
	   并确保 具备3个以上的顶点*/
	IndexedTriangleList( Dvtx::VertexBuffer verts_in,std::vector<unsigned short> indices_in )
		:
		vertices( std::move( verts_in ) ),
		indices( std::move( indices_in ) )
	{
		assert( vertices.Size() > 2 );
		assert( indices.size() % 3 == 0 );
	}

	/* 以参数矩阵设定模型的三围*/
	void Transform( DirectX::FXMMATRIX matrix )
	{
		/*for( auto& v : vertices )*/

		using Elements = Dvtx::VertexLayout::ElementType;
		for (int i = 0; i < vertices.Size(); i++)
		{			
			auto& pos = vertices[i].Attr<Elements::Position3D>();
			// 用参数矩阵更新pos值
			DirectX::XMStoreFloat3(
				&pos,
				DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&pos), matrix) //把参数矩阵存到pos向量里
			);
		}
	}
	

	/* 获取每个顶点的法向量; 
	   即利用叉积把每隔三个顶点一组计算出来的v1,v2,v3计算出各自的法向量再更新到v1、v2、v3里去*/
	void SetNormalsIndependentFlat() noxnd
	{
		using namespace DirectX;
		using Type = Dvtx::VertexLayout::ElementType;
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			auto v0 = vertices[indices[i]];
			auto v1 = vertices[indices[i + 1]];
			auto v2 = vertices[indices[i + 2]];

			// 从第xxx个带POSTION型的顶点里加载出各自的向量
			const auto p0 = XMLoadFloat3(&v0.Attr<Type::Position3D>());
			const auto p1 = XMLoadFloat3(&v1.Attr<Type::Position3D>());
			const auto p2 = XMLoadFloat3(&v2.Attr<Type::Position3D>());
			// 叉积算出单位法向量
			const auto n = XMVector3Normalize(XMVector3Cross((p1 - p0), (p2 - p0)));

			// 再把各自的法向量更新到每个顶点里面去
			XMStoreFloat3(&v0.Attr<Type::Normal>(), n);
			XMStoreFloat3(&v1.Attr<Type::Normal>(), n);
			XMStoreFloat3(&v2.Attr<Type::Normal>(), n);
		}
	}

public:
	Dvtx::VertexBuffer vertices;		//在DVTX(动态反射系统)里真正的顶点数据
	std::vector<unsigned short> indices;	//顶点索引
};