#pragma once
#include "DrawableBase.h"
#include "BindableCommon.h"
#include "Vertex.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ConditionalNoexcept.h"

/* Mesh类继承自DrawableBasel,其构造函数,需要图形对象＼管线绑定物集合*/
class Mesh : public DrawableBase<Mesh>
{
public:
	// 构造函数,需要图形对象＼管线绑定物集合
	Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bind::Bindable>> bindPtrs);
	/* 存累计变化矩阵并绘制被绘制物*/
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform/*累计的矩阵变换*/) const noxnd;
	/* 获得累计变化矩阵*/
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
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
	Node(std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noxnd;
	/* 用于递归绘制单节点的Meshes*/
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noxnd;
private:
	// 添加子节点,仅供Model类实例使用,因为Model类是Node类的友元
	void AddChild(std::unique_ptr<Node> pChild) noxnd;
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
	Model(Graphics& gfx, const std::string fileName);
	/* 为模型根节点绘制所有网格*/
	void Draw(Graphics& gfx, DirectX::FXMMATRIX transform) const;
private:
	/* 用于解析加载单个Mesh;静态方法ParseMesh();*/
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh);

	/* 解析加载单个节点，指定1个参数节点*/
	std::unique_ptr<Node> ParseNode(const aiNode& node);

	/*void Draw(Graphics& gfx) const
	{
		pRoot->Draw(gfx, DirectX::XMMatrixIdentity());
	}*/
private:
	std::unique_ptr<Node> pRoot;// 根节点
	std::vector<std::unique_ptr<Mesh>> meshPtrs;// 总体网格集合
};