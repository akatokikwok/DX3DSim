#include "Mesh.h"
#include <memory>
#include "imgui/imgui.h"
#include <unordered_map>
#include <sstream>
#include "Surface.h"

namespace dx = DirectX;

ModelException::ModelException(int line, const char* file, std::string note) noexcept
	:
	ChiliException(line, file),
	note(std::move(note))
{}

const char* ModelException::what() const noexcept
{
	std::ostringstream oss;
	oss << ChiliException::what() << std::endl
		<< "[Note] " << GetNote();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* ModelException::GetType() const noexcept
{
	return "Grb Model Exception";
}

const std::string& ModelException::GetNote() const noexcept
{
	return note;
}

/// //////////////////////////////////////////////////////////////////////////

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

Node::Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform_in) noxnd
	:
	id(id),
	meshPtrs(std::move(meshPtrs)),
	name(name)
{
	//DirectX::XMStoreFloat4x4(&this->transform, transform);

	dx::XMStoreFloat4x4(&transform, transform_in);
	dx::XMStoreFloat4x4(&appliedTransform, dx::XMMatrixIdentity());
}

void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noxnd
{
	
	// 取得 从imgui配置文件里的变换*最终应用的变换*从父节点传过来的变换
	const auto built =
		dx::XMLoadFloat4x4( &appliedTransform ) *
		dx::XMLoadFloat4x4( &transform ) *
		accumulatedTransform;

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

int Node::GetId() const noexcept
{
	return id;
}

void Node::ShowTree(/*std::optional<int>& selectedIndex, */Node*& pSelectedNode) const noexcept
{
	//// 首先把传进来的索引 视作当前节点的索引
	//const int currentNodeIndex = nodeIndexTracked;
	//// 增加传进的索引,为下一个节点做准备
	//nodeIndexTracked++;

	// if there is no selected node, set selectedId to an impossible value;
	// 这一步首先查询是否点击了，如果不存在有节点被点中则 被选中ID设置为-1，若存在有节点被选中则拿它的序号
	// selectedId 是被选中的节点序号,若没有被选中则默认设置为-1
	const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();

	// 为当前节点定义一些标签,这些标签供TreeNodeEx()使用;TreeNodeEx()方法主要负责叶子节点，而TreeNode()负责树枝节点
	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		//| ((GetId() == selectedIndex.value_or(-1)) ? ImGuiTreeNodeFlags_Selected : 0)
		| ((GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0)				 //如果传入节点等于被选中节点,那么该节点就是处于被选中状态
		| ((childPtrs.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0					 //如果子节点集合数量为空则证明该节点是叶子节点
	);	

	// render this node;TreeNode functions return true when the node is open, in which case you need to also call TreePop() when you are finished displaying the tree node contents.
	const auto expanded = ImGui::TreeNodeEx(
		(void*)(intptr_t)GetId(), node_flags, name.c_str()
	);
	// processing for selecting node// 设置被选中的节点;放到下面的if外面是为了确保即使非树枝节点仍然也可以被选中
	if (ImGui::IsItemClicked())
	{
		pSelectedNode = const_cast<Node*>(this);
	}
	//// 如果是树枝节点,就可以展开循环绘制 
	if (expanded)
	{
		// 对于所有树枝节点循环渲染子节点
		for (const auto& pChild : childPtrs)
		{
			pChild->ShowTree( pSelectedNode);
		}
		ImGui::TreePop();
	}
}

void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{
	dx::XMStoreFloat4x4(&appliedTransform, transform);
}



//////////////////////////////////////////////////////////////////////////
// ModelWindow IMGUI控制窗口
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
			root.ShowTree( pSelectedNode);

			ImGui::NextColumn();
			
			// 如果存在有被选中状态的节点,就可以使用IMGUI去控制
			if (pSelectedNode != nullptr)
			{
				auto& transform = transforms[ pSelectedNode->GetId() ];//根据被选中的索引从无序map里取出对应的数据组
				ImGui::Text("Orientation");
				ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
				ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
				ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);
				ImGui::Text("Position");
				ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
				ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
				ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);
			}
		}
		ImGui::End();
	}
	// 封装的方法，用于获取模型的变换
	dx::XMMATRIX GetTransform() const noexcept
	{
		//const auto& transform = transforms.at(*selectedIndex);

		assert(pSelectedNode != nullptr);
		const auto& transform = transforms.at(pSelectedNode->GetId());// 取对应序号节点的变换

		return	dx::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) * dx::XMMatrixTranslation(transform.x, transform.y, transform.z);//取单节点旋转和位移矩阵乘积
	}

	Node* GetSelectedNode() const noexcept
	{
		return pSelectedNode;
	}

private:
	//std::optional<int> selectedIndex;//准备操作去选中的某个节点索引

	Node* pSelectedNode; // 当前被选中的节点及索引

	struct TransformParameters
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	} ;
	
	// 无序map负责把索引映射到数据参数结构体TransformParameters上;目的是追踪每个骨骼节点的变换
	std::unordered_map<int, TransformParameters> transforms;
};
//////////////////////////////////////////////////////////////////////////

Model::Model(Graphics& gfx, const std::string fileName)
	:
	pWindow(std::make_unique<ModelWindow>())
{
	// 指定模型名导入
	Assimp::Importer imp;
	// pScene是被读取到的模型的aiScene型网格
	const auto pScene = imp.ReadFile(fileName.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals
	);
	// 若模型加载不正确,就抛出提示信息
	if (pScene == nullptr)
	{
		throw ModelException(__LINE__, __FILE__, imp.GetErrorString());
	}

	// 加载整个模型网格
	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials));
	}
	// 使用ParseNode方法存储模型根节点
	int nextId = 0;
	pRoot = ParseNode(nextId, *pScene->mRootNode);
}

void Model::Draw(Graphics& gfx) const noxnd
{
	//const auto transform = DirectX::XMMatrixRotationRollPitchYaw(pos.roll, pos.pitch, pos.yaw) *
	//	DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	//pRoot->Node::Draw(gfx, pWindow->GetTransform());

	if (auto node = pWindow->GetSelectedNode())
	{
		node->SetAppliedTransform(pWindow->GetTransform());
	}
	pRoot->Node::Draw(gfx, dx::XMMatrixIdentity());
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

// 解析加载单片mesh
std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials)
{
	using Dvtx::VertexLayout;

	// 为了加载动态布局系统; 要动态创建(即Append)顶点布局 、指定带位置、法线的顶点缓存、纹理
	Dvtx::VertexBuffer vbuf(std::move(
		VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)
		.Append(VertexLayout::Texture2D)
	));

	//// material变量是单片mesh的材质; 一个模型有多个mesh和仅1个材质数组，单片mesh有自己的材质索引，材质索引负责从材质数组里取材质
	//auto& material = *pMaterials[mesh.mMaterialIndex];
	//// 遍历单片mesh的材质里的所有属性，并拿取这个属性的引用
	//for (int i=0; i<material.mNumProperties; i++)
	//{
	//	auto& prop = *material.mProperties[i];
	//	int qqq = 90;
	//}

	// 遍历参数网格的所有顶点,存储顶点位置和法线
	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		vbuf.EmplaceBack(
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
			*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i]) //由于模型顶点对于不同的纹理可能显示出不同的纹理坐标;所以这里理解为顶点i的[0]号坐标
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
	// 检查单片mesh是否持有材质纹理
	if (mesh.mMaterialIndex >= 0)
	{
		auto& material = *pMaterials[mesh.mMaterialIndex]; // 拿到当前mesh的材质

		using namespace std::string_literals;
		const auto base = "Models\\nano_textured\\"s;
		aiString texFileName; // 创建一个aiString变量用于存储纹理文件的路径

		material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName); // 拿到第一张漫反射纹理存到上面那个字符串里
		bindablePtrs.push_back(std::make_unique<Bind::Texture>(gfx, Surface::FromFile(base + texFileName.C_Str()), 0)); // 创建1个漫反射纹理, 位于插槽0 ，表示第一个纹理
		material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName); //拿一张镜面光纹理存到字符串里
		bindablePtrs.push_back(std::make_unique<Bind::Texture>(gfx, Surface::FromFile(base + texFileName.C_Str()), 1)); // 创建1个镜面光纹理，位于插槽1， 表示第二个纹理


		bindablePtrs.push_back(std::make_unique<Bind::Sampler>(gfx)); // 创建采样器
	}

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
		//DirectX::XMFLOAT3 color = { 0.6f,0.6f,0.8f }; // 由于模型已经有漫反射纹理了，所以这里不再使用自定义的颜色
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[2];
	} pmc;
	bindablePtrs.push_back(std::make_unique<Bind::PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));

	return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}

std::unique_ptr<Node> Model::ParseNode(int& nextId, const aiNode& node) noexcept
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

	// 使用参数单节点的转置矩阵和所有Mesh的指针构造一个单节点;每次建立节点时候，把id传进去，然后增加id序数，以此建立下一个节点
	auto pNode = std::make_unique<Node>( nextId++, node.mName.C_Str(), std::move(curMeshPtrs), transform);
	// 套娃,为单节点也添加子节点
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(nextId, *node.mChildren[i]));
	}

	return pNode;
}
