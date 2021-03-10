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
#include <type_traits>
#include "imgui/imgui.h"

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
	/* 自定义1个材质常量PSMaterialConstantFullmonte
	持有法线、高光纹理开关、高光alpha通道、高光功率、高光颜色、高光权重; 当三种纹理都启用时使用这个结构体*/
	struct PSMaterialConstantFullmonte
	{
		BOOL  normalMapEnabled = TRUE;	//法线贴图开关
		BOOL  specularMapEnabled = TRUE;	//高光贴图开关
		BOOL  hasGlossMap = FALSE;	// 检查alpha通道开关
		float specularPower = 3.1f;	//高光功率
		DirectX::XMFLOAT3 specularColor = { 0.75f,0.75f,0.75f };	//高光颜色，默认为{ 1.0f,1.0f,1.0f };
		float specularMapWeight = 0.671f;	//高光贴图权重
	};

	/* 自定义PSMaterialConstantNotex;
	一个材质常数缓存shader;包含材质颜色、高光强度、高光功率;当三种纹理都不启用的时候使用这个结构体 */
	struct PSMaterialConstantNotex
	{
		DirectX::XMFLOAT4 materialColor = { 0.447970f,0.327254f,0.176283f,1.0f };
		DirectX::XMFLOAT4 specularColor = { 0.65f,0.65f,0.65f,1.0f };
		float specularPower = 120.0f;
		float padding[3];
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

	/* 泛型方法ControlMeDaddy; 负责查询到管线上绑定的 材质常数缓存T; 顺便构建IMGUI 并更新这个常数缓存*/
	template<class T>
	bool ControlMeDaddy(Graphics& gfx, T& c)
	{
		if (meshPtrs.empty())
		{
			return false;
		}

		/* PS!! 这里只匹配PSMaterialConstantFullmonte型的常数缓存 */
		if constexpr (std::is_same<T, PSMaterialConstantFullmonte>::value)
		{
			/* 若查询到管线上绑定的 材质常数缓存T 对象 */
			if (auto pcb = meshPtrs.front()->QueryBindable<Bind::PixelConstantBuffer<T>>())
			{
				ImGui::Text("Material");//标题叫Material

				bool normalMapEnabled = (bool)c.normalMapEnabled;
				ImGui::Checkbox("Norm Map", &normalMapEnabled);
				c.normalMapEnabled = normalMapEnabled ? TRUE : FALSE;

				bool specularMapEnabled = (bool)c.specularMapEnabled;
				ImGui::Checkbox("Spec Map", &specularMapEnabled);
				c.specularMapEnabled = specularMapEnabled ? TRUE : FALSE;

				bool hasGlossMap = (bool)c.hasGlossMap;
				ImGui::Checkbox("Gloss Alpha", &hasGlossMap);
				c.hasGlossMap = hasGlossMap ? TRUE : FALSE;

				ImGui::SliderFloat("Spec Weight", &c.specularMapWeight, 0.0f, 2.0f);

				ImGui::SliderFloat("Spec Pow", &c.specularPower, 0.0f, 1000.0f, "%f", 5.0f);

				ImGui::ColorPicker3("Spec Color", reinterpret_cast<float*>(&c.specularColor));

				pcb->Update(gfx, c);//更新常数缓存
				return true;
			}
		}
		/* PS!! 这里只匹配PSMaterialConstantNotex型的常数缓存 */
		else if constexpr (std::is_same<T, PSMaterialConstantNotex>::value)
		{
			if (auto pcb = meshPtrs.front()->QueryBindable<Bind::PixelConstantBuffer<T>>())
			{
				ImGui::Text("Material");

				ImGui::ColorPicker3("Spec Color", reinterpret_cast<float*>(&c.specularColor));

				ImGui::SliderFloat("Spec Pow", &c.specularPower, 0.0f, 1000.0f, "%f", 5.0f);

				ImGui::ColorPicker3("Diff Color", reinterpret_cast<float*>(&c.materialColor));

				pcb->Update(gfx, c);
				return true;
			}
		}
		return false;
	}

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