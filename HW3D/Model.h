#pragma once
#include "DrawableBase.h"
#include "BindableBase.h"
#include "Vertex.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/* Mesh类继承自DrawableBasel,其构造函数,需要图形对象＼管线绑定物集合*/
class Mesh : public DrawableBase<Mesh>
{
public:
	// 构造函数,需要图形对象＼管线绑定物集合
	Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bind::Bindable>> bindPtrs)
	{
		if (!IsStaticInitialized())
		{
			// 没初始化就绑定三角形列表图元
			AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
		}

		for (auto& pb : bindPtrs)
		{
			if (auto pi = dynamic_cast<IndexBuffer*>(pb.get()))
			{
				// 如果绑定物是索引缓存,就添加索引缓存并释放管线绑定物；
				AddIndexBuffer(std::unique_ptr<IndexBuffer>{ pi });
				// 这里对Pi指针又使用了一次unique_ptr，故现存2个指针指向了同一个对象，需要释放
				pb.release();
			}
			else
			{
				AddBind(std::move(pb));
			}
		}

		// 每个Mesh构造的时候就需要添加1个顶点着色器常量缓存,因为每一个Mesh需要的顶点变换都不同
		AddBind(std::make_unique<TransformCbuf>(gfx, *this));
	}
	/* 存累计变化矩阵并绘制被绘制物*/
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform/*累计的矩阵变换*/) const noexcept(!IS_DEBUG)
	{
		DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);// 把参数矩阵存起来
		Drawable::Draw(gfx);// 绑定到管线并按索引绘制
	}
	/* 获得累计变化矩阵*/
	DirectX::XMMATRIX GetTransformXM() const noexcept override
	{
		return DirectX::XMLoadFloat4x4(&transform);
	}
private:
	mutable DirectX::XMFLOAT4X4 transform;// 用于统计"累计矩阵变化"的4x4
};
/// //////////////////////////////////////////////////////////////////////////
/* 模型节点类*/
class Node
{
	friend class Model;
public:
	// 构造函数,用参数Mesh集合初始化自己的Mesh集合,并保存参数变换
	Node(std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept(!IS_DEBUG)
		:
		meshPtrs(std::move(meshPtrs))
	{
		DirectX::XMStoreFloat4x4(&this->transform, transform);
	}
	/* 用于递归绘制单节点的Meshes*/
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
	{
		// 本节点的相对变换乘以父节点的累计变换
		const auto built = DirectX::XMLoadFloat4x4(&transform) * accumulatedTransform;
		
		// 将上述应用到自己的Meshes上,然后对自己的子节点也是如此
		for (const auto pm : meshPtrs)
		{
			pm->Mesh::Draw(gfx, built);
		}
		for (const auto& pc : childPtrs)
		{
			// 套娃
			pc->Node::Draw(gfx, built);
		}
	}
private:
	// 添加子节点,仅供Model类实例使用,因为Model类是Node类的友元
	void AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG)
	{
		assert(pChild);
		childPtrs.push_back(std::move(pChild));
	}
private:
	std::vector<std::unique_ptr<Node>> childPtrs;//子节点集合
	std::vector<Mesh*> meshPtrs;				//节点上挂载的Mesh集合
	DirectX::XMFLOAT4X4 transform;				// 相对于父节点的变换
};
/// //////////////////////////////////////////////////////////////////////////
/* 模型类 // 构造函数;需要图形实例和指定模型名*/
class Model
{
public:
	// 构造函数;需要图形实例和指定模型名
	Model(Graphics& gfx, const std::string fileName)
	{
		// 指定模型名导入
		Assimp::Importer imp;
		const auto pScene = imp.ReadFile(fileName.c_str(),
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices
		);
		// 加载整个模型
		for (size_t i = 0; i < pScene->mNumMeshes; i++)
		{
			meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i]));
		}
		// 使用ParseNode方法存储模型根节点
		pRoot = ParseNode(*pScene->mRootNode);
	}
	/* 为模型根节点绘制所有网格*/
	void Draw(Graphics& gfx, DirectX::FXMMATRIX transform) const
	{
		pRoot->Draw(gfx, transform);
	}
private:
	/* 用于解析加载单个Mesh;静态方法ParseMesh();*/
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh)
	{
		namespace dx = DirectX;
		using hw3dexp::VertexLayout;

		// 首先动态创建顶点布局 、指定带位置、法线的顶点缓存
		hw3dexp::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
		));

		// 遍历参数网格的所有顶点,存储顶点位置和法线
		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i])
			);
		}

		// 确定索引数组三角面;为每个三角面存储顶点索引
		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}
		
		// 把所有的顶点缓存、索引缓存、着色器、输入布局填进绑定物集合
		std::vector<std::unique_ptr<Bindable>> bindablePtrs;

		bindablePtrs.push_back(std::make_unique<VertexBuffer>(gfx, vbuf));

		bindablePtrs.push_back(std::make_unique<IndexBuffer>(gfx, indices));

		auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		bindablePtrs.push_back(std::move(pvs));

		bindablePtrs.push_back(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

		bindablePtrs.push_back(std::make_unique<InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pvsbc));
		// 自定义材质常量缓存结构体并添加进绑定物集合
		struct PSMaterialConstant
		{
			DirectX::XMFLOAT3 color = { 0.6f,0.6f,0.8f };
			float specularIntensity = 0.6f;
			float specularPower = 30.0f;
			float padding[3];
		} pmc;
		bindablePtrs.push_back(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));

		return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
	}

	/* 解析加载单个节点，指定1个参数节点*/
	std::unique_ptr<Node> ParseNode(const aiNode& node)
	{
		namespace dx = DirectX;
		// 拿到参数单节点的转置矩阵
		const auto transform = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
			reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
		));

		// 所有单节点的网格集合
		std::vector<Mesh*> curMeshPtrs;
		curMeshPtrs.reserve(node.mNumMeshes);
		for (size_t i = 0; i < node.mNumMeshes; i++)
		{
			const auto meshIdx = node.mMeshes[i];//当前这组队列的索引
			curMeshPtrs.push_back(meshPtrs.at(meshIdx).get());//因为网格集合此前已在构造里处理过,故用at()取值
		}

		// 使用参数单节点的转置矩阵和所有Mesh的指针构造一个单节点
		auto pNode = std::make_unique<Node>(std::move(curMeshPtrs), transform);
		// 套娃,为单节点也添加子节点
		for (size_t i = 0; i < node.mNumChildren; i++)
		{
			pNode->AddChild(ParseNode(*node.mChildren[i]));
		}

		return pNode;
	}

	/*void Draw(Graphics& gfx) const
	{
		pRoot->Draw(gfx, DirectX::XMMatrixIdentity());
	}*/
private:
	std::unique_ptr<Node> pRoot;// 根节点
	std::vector<std::unique_ptr<Mesh>> meshPtrs;// 总体网格集合
};