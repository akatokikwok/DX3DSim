#pragma once
#include "BindableCommon.h"
#include "Vertex.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ConditionalNoexcept.h"
#include <optional>
#include "Drawable.h"
#include "ConstantBuffers.h"

/* 用于捕获异常的模型异常类*/
class ModelException : public ChiliException
{
public:
	ModelException(int line, const char* file, std::string note) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	const std::string& GetNote() const noexcept;
private:
	std::string note;
};

/* Mesh类继承自Drawable,其构造函数,需要图形对象＼管线绑定物集合;//绑定图元、若符合索引缓存则添加并最后构造顶点shader常量缓存*/
class Mesh : public Drawable
{
public:
	// 构造函数,需要图形对象＼管线绑定物集合
	Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs);
	/* 存累计变化矩阵并绘制被绘制物*/
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noxnd;
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
	//friend class ModelWindow;// 定义在源文件里

public:
	/* 自定义1个材质常量shader,持有法线、高光纹理开关、高光alpha通道、高光功率、高光颜色、高光权重*/
	struct PSMaterialConstantFullmonte
	{
		BOOL  normalMapEnabled = TRUE;	//法线贴图开关
		BOOL  specularMapEnabled = TRUE;	//高光贴图开关
		BOOL  hasGlossMap = FALSE;	// 检查alpha通道开关
		float specularPower = 3.1f;	//高光功率
		DirectX::XMFLOAT3 specularColor = { 0.75f,0.75f,0.75f };	//高光颜色，默认为{ 1.0f,1.0f,1.0f };
		float specularMapWeight = 0.671f;	//高光贴图权重
	};

public:
	// 构造函数,用参数Mesh集合初始化自己的Mesh集合,并保存参数变换
	//Node(std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noxnd;
	Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noxnd;
	/* 用于递归绘制单节点的Meshes*/
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noxnd;
	/* 用来设置最终被应用的变换*/
	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
	// 接口，拿取每个节点的独有ID	
	int GetId() const noexcept;

	/* 用于渲染树上的节点, 同一时间仅允许1个子节点被选中;
	   要求提供 传进节点的引用
	*/
	void ShowTree(/*std::optional<int>& selectedIndex, */Node*& pSelectedNode) const noexcept;
	/* 查询到管线上绑定的 材质常数缓存构建IMGUI 并更新这个常数缓存*/
	void ControlMeDaddy(Graphics& gfx, PSMaterialConstantFullmonte& c);

private:
	// 添加子节点,仅供Model类实例使用,因为Model类是Node类的友元
	void AddChild(std::unique_ptr<Node> pChild) noxnd;	

private:
	std::string name;							// 每个节点的名字
	int id;										// 每个节点对应的独一无二ID
	std::vector<std::unique_ptr<Node>> childPtrs;//子节点集合
	std::vector<Mesh*> meshPtrs;				//节点上挂载的Mesh集合
	//DirectX::XMFLOAT4X4 transform;				// 相对于父节点的变换

	DirectX::XMFLOAT4X4 transform;		// 从文件里加载出来的变换
	DirectX::XMFLOAT4X4 appliedTransform;	// 最终被应用的变换
};

/// //////////////////////////////////////////////////////////////////////////
/* 模型类 // 构造函数;需要图形实例和指定模型名*/
class Model
{
public:
	// 构造函数;需要图形实例和指定模型名
	Model(Graphics& gfx, const std::string fileName);
	/* 为模型根节点绘制所有网格*/
	//void Draw(Graphics& gfx, DirectX::FXMMATRIX transform) const;
	void Draw(Graphics& gfx) const noxnd;
	/* 用于展示Model的IMGUI窗口*/
	void ShowWindow(Graphics& gfx, const char* windowName = nullptr) noexcept;
	/* 设置模型根节点的变换*/
	void SetRootTransform(DirectX::FXMMATRIX tf) noexcept;
	~Model() noexcept;
private:
	/* 用于解析加载单个Mesh;静态方法ParseMesh(); 需要材质数组参数*/
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials);

	/* 解析加载单个节点，指定1个参数节点,PS:其实每次加载树时候，树的结构并没有变化*/
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node) noexcept;

	/*void Draw(Graphics& gfx) const
	{
		pRoot->Draw(gfx, DirectX::XMMatrixIdentity());
	}*/
private:
	std::unique_ptr<Node> pRoot;// 根节点
	std::vector<std::unique_ptr<Mesh>> meshPtrs;// 总体网格集合

	// ModelWindow定义在源文件里,是模型的IMGUI控制窗口;放在cpp里为了保护它的实现不被访问
	std::unique_ptr<class ModelWindow> pWindow;
};