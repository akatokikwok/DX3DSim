#include "Mesh.h"
#include <memory>
#include "imgui/imgui.h"

namespace dx = DirectX;

// 绑定图元、若符合索引缓存则添加并最后构造顶点shader常量缓存
Mesh::Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bind::Bindable>> bindPtrs)
{
	if (!IsStaticInitialized())
	{
		// 没初始化就绑定三角形列表图元
		AddStaticBind(std::make_unique<Bind::Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}

	for (auto& pb : bindPtrs)
	{
		if (auto pi = dynamic_cast<Bind::IndexBuffer*>(pb.get()))
		{
			// 如果绑定物是索引缓存,就添加索引缓存并释放管线绑定物；
			AddIndexBuffer(std::unique_ptr<Bind::IndexBuffer>{ pi });
			// 这里对Pi指针又使用了一次unique_ptr，故现存2个指针指向了同一个对象，需要释放
			pb.release();
		}
		else
		{
			AddBind(std::move(pb));
		}
	}

	// 每个Mesh构造的时候就需要添加1个顶点着色器常量缓存,因为每一个Mesh需要的顶点变换都不同
	AddBind(std::make_unique<Bind::TransformCbuf>(gfx, *this));
}

void Mesh::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform/*累计的矩阵变换*/) const noxnd
{
	DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);// 把参数矩阵存起来
	Drawable::Draw(gfx);// 绑定到管线并按索引绘制 
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&transform);
}

Node::Node(const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noxnd
	:
	meshPtrs(std::move(meshPtrs)),
	name(name)
{
	DirectX::XMStoreFloat4x4(&this->transform, transform);
}

void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noxnd
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

void Node::AddChild(std::unique_ptr<Node> pChild) noxnd
{
	assert(pChild);
	childPtrs.push_back(std::move(pChild));
}

void Node::ShowTree(int& nodeIndexTracked, std::optional<int>& selectedIndex) const noexcept
{
	// 首先把传进来的索引 视作当前节点的索引
	const int currentNodeIndex = nodeIndexTracked;
	// 增加传进的索引,为下一个节点做准备
	nodeIndexTracked++;
	// 为当前节点定义一些标签,这些标签供TreeNodeEx()使用;TreeNodeEx()方法主要负责叶子节点，而TreeNode()负责树枝节点
	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ((currentNodeIndex == selectedIndex.value_or(-1)) ? ImGuiTreeNodeFlags_Selected : 0)//如果传入节点等于被选中节点,那么该节点就是处于被选中状态
		| ((childPtrs.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);	//如果子节点集合数量为空则证明该节点是叶子节点

	// 如果是树枝节点,就可以展开循环绘制
	if (ImGui::TreeNodeEx((void*)(intptr_t)currentNodeIndex, node_flags, name.c_str()))
	{
		selectedIndex = ImGui::IsItemClicked() ? currentNodeIndex : selectedIndex;// 获取被点中的节点索引

		for (const auto& pChild : childPtrs)
		{
			pChild->ShowTree(nodeIndexTracked, selectedIndex);
		}
		ImGui::TreePop();
	}
}

//////////////////////////////////////////////////////////////////////////
// Model
class ModelWindow // pImpl idiom
{
public:
	// 封装的方法，用于分两列展示各个模型的细节控制
	void Show(const char* windowName, const Node& root) noexcept
	{
		// window name defaults to "Model"
		windowName = windowName ? windowName : "Model";
		// need an ints to track node indices and selected node
		int nodeIndexTracker = 0;
		if (ImGui::Begin(windowName))
		{
			ImGui::Columns(2, nullptr, true);
			root.ShowTree(nodeIndexTracker, selectedIndex);

			ImGui::NextColumn();
			ImGui::Text("Orientation");
			ImGui::SliderAngle("Roll", &pos.roll, -180.0f, 180.0f);
			ImGui::SliderAngle("Pitch", &pos.pitch, -180.0f, 180.0f);
			ImGui::SliderAngle("Yaw", &pos.yaw, -180.0f, 180.0f);
			ImGui::Text("Position");
			ImGui::SliderFloat("X", &pos.x, -20.0f, 20.0f);
			ImGui::SliderFloat("Y", &pos.y, -20.0f, 20.0f);
			ImGui::SliderFloat("Z", &pos.z, -20.0f, 20.0f);
		}
		ImGui::End();
	}
	// 封装的方法，用于获取模型的变换
	dx::XMMATRIX GetTransform() const noexcept
	{
		return dx::XMMatrixRotationRollPitchYaw(pos.roll, pos.pitch, pos.yaw) *
			dx::XMMatrixTranslation(pos.x, pos.y, pos.z);
	}

private:
	std::optional<int> selectedIndex;//准备操作去选中的某个节点索引

	struct
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	} pos;
};
//////////////////////////////////////////////////////////////////////////

Model::Model(Graphics& gfx, const std::string fileName)
	:
	pWindow(std::make_unique<ModelWindow>())
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

void Model::Draw(Graphics& gfx) const noxnd
{
	//const auto transform = DirectX::XMMatrixRotationRollPitchYaw(pos.roll, pos.pitch, pos.yaw) *
	//	DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	pRoot->Node::Draw(gfx, pWindow->GetTransform());
}

void Model::ShowWindow(const char* windowName) noexcept
{
	//windowName = windowName ? windowName : "Model";//若提供参数名就用参数名,不提供参数窗口名字则默认使用"Model"
	//if (ImGui::Begin(windowName))
	//{
	//	ImGui::Columns(2, nullptr, true);//有2列
	//	pRoot->RenderTree();
	//	ImGui::NextColumn();
	//	ImGui::Text("Orientation");
	//	ImGui::SliderAngle("Roll", &pos.roll, -180.0f, 180.0f);
	//	ImGui::SliderAngle("Pitch", &pos.pitch, -180.0f, 180.0f);
	//	ImGui::SliderAngle("Yaw", &pos.yaw, -180.0f, 180.0f);
	//	ImGui::Text("Position");
	//	ImGui::SliderFloat("X", &pos.x, -20.0f, 20.0f);
	//	ImGui::SliderFloat("Y", &pos.y, -20.0f, 20.0f);
	//	ImGui::SliderFloat("Z", &pos.z, -20.0f, 20.0f);
	//}
	//ImGui::End();

	// 展示模型控制窗口
	pWindow->Show(windowName, *pRoot);
}

Model::~Model() noexcept
{

}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh)
{
	namespace dx = DirectX;
	using Dvtx::VertexLayout;

	// 首先动态创建顶点布局 、指定带位置、法线的顶点缓存
	Dvtx::VertexBuffer vbuf(std::move(
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
	std::vector<std::unique_ptr<Bind::Bindable>> bindablePtrs;

	bindablePtrs.push_back(std::make_unique<Bind::VertexBuffer>(gfx, vbuf));

	bindablePtrs.push_back(std::make_unique<Bind::IndexBuffer>(gfx, indices));

	auto pvs = std::make_unique<Bind::VertexShader>(gfx, L"PhongVS.cso");
	auto pvsbc = pvs->GetBytecode();
	bindablePtrs.push_back(std::move(pvs));

	bindablePtrs.push_back(std::make_unique<Bind::PixelShader>(gfx, L"PhongPS.cso"));

	bindablePtrs.push_back(std::make_unique<Bind::InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pvsbc));
	// 自定义材质常量缓存结构体并添加进绑定物集合
	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color = { 0.6f,0.6f,0.8f };
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3];
	} pmc;
	bindablePtrs.push_back(std::make_unique<Bind::PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));

	return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}

std::unique_ptr<Node> Model::ParseNode(const aiNode& node) noexcept
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
	auto pNode = std::make_unique<Node>( node.mName.C_Str(), std::move(curMeshPtrs), transform);
	// 套娃,为单节点也添加子节点
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(*node.mChildren[i]));
	}

	return pNode;
}
