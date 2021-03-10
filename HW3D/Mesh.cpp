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
Mesh::Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs)
{
	//if (!IsStaticInitialized())
	//{
	//	// 没初始化就绑定三角形列表图元
	//	AddStaticBind(std::make_unique<Bind::Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	//}

	// 绑定图元:三角形列表至资源集合
	AddBind(Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	for (auto& pb : bindPtrs)
	{
		#pragma region ver1.0.39弃用
       //if (auto pi = dynamic_cast<Bind::IndexBuffer*>(pb.get()))
		//{
		//	// 如果绑定物是索引缓存,就添加索引缓存并释放管线绑定物；
		//	AddIndexBuffer(std::unique_ptr<Bind::IndexBuffer>{ pi });
		//	// 这里对Pi指针又使用了一次unique_ptr，故现存2个指针指向了同一个对象，需要释放
		//	pb.release();
		//}
		//else
		//{
		//	AddBind(std::move(pb));
		//}
        #pragma endregion ver1.0.39弃用

		AddBind(std::move(pb));
	}

	// 每个Mesh构造的时候就需要添加1个顶点着色器常量缓存,因为每一个Mesh需要的顶点变换都不同
	AddBind(std::make_shared<Bind::TransformCbuf>(gfx, *this));	//创建出顶点常数缓存
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

//void Node::ControlMeDaddy(Graphics& gfx, PSMaterialConstantFullmonte& c)
//{
//	if (meshPtrs.empty())// 没mesh就直接错误返回
//	{
//		return;
//	}
//
//	if (auto pcb = meshPtrs.front()->QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstantFullmonte>>())//查询到管线上绑定的 材质常数缓存
//	{
//		ImGui::Text("Material");
//
//		bool normalMapEnabled = (bool)c.normalMapEnabled;
//		ImGui::Checkbox("Norm Map", &normalMapEnabled);
//		c.normalMapEnabled = normalMapEnabled ? TRUE : FALSE;
//
//		bool specularMapEnabled = (bool)c.specularMapEnabled;
//		ImGui::Checkbox("Spec Map", &specularMapEnabled);
//		c.specularMapEnabled = specularMapEnabled ? TRUE : FALSE;
//
//		bool hasGlossMap = (bool)c.hasGlossMap;
//		ImGui::Checkbox("Gloss Alpha", &hasGlossMap);
//		c.hasGlossMap = hasGlossMap ? TRUE : FALSE;
//
//		ImGui::SliderFloat("Spec Weight", &c.specularMapWeight, 0.0f, 2.0f);
//
//		ImGui::SliderFloat("Spec Pow", &c.specularPower, 0.0f, 1000.0f, "%f", 5.0f);
//
//		ImGui::ColorPicker3("Spec Color", reinterpret_cast<float*>(&c.specularColor));
//
//		pcb->Update(gfx, c); //更新常数缓存
//	}
//}

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
	void Show(Graphics& gfx, const char* windowName, const Node& root) noexcept
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
				//pSelectedNode->ControlMeDaddy(gfx, mc);	//控制且更新材质常数缓存

				/// pSelectedNode->ControlMeDaddy(gfx, skinMaterial)返回真表明当前选中的节点为皮肤;返回假则表面它是耳环
				if ( !( pSelectedNode->ControlMeDaddy(gfx, skinMaterial)) )
				{
					pSelectedNode->ControlMeDaddy(gfx, ringMaterial);//对耳环应用材质ringMaterial
				}
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
	
	Node::PSMaterialConstantFullmonte skinMaterial; //用定义在头文件的这个材质常数结构体表示 "哥布林皮肤材质"
	Node::PSMaterialConstantNotex ringMaterial;// 用定义在头文件的这个材质常数结构体表示 "耳环材质"
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
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace //也计算切线空间
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

void Model::ShowWindow(Graphics& gfx, const char* windowName) noexcept
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
	pWindow->Show(gfx, windowName, *pRoot);
}

void Model::SetRootTransform(DirectX::FXMMATRIX tf) noexcept
{
	pRoot->SetAppliedTransform(tf);
}

Model::~Model() noexcept
{

}

// 解析加载单片mesh
std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials)
{
	using namespace std::string_literals;
	using Dvtx::VertexLayout;
	using namespace Bind;
	//// material变量是单片mesh的材质; 一个模型有多个mesh和仅1个材质数组，单片mesh有自己的材质索引，材质索引负责从材质数组里取材质
	//auto& material = *pMaterials[mesh.mMaterialIndex];
	//// 遍历单片mesh的材质里的所有属性，并拿取这个属性的引用
	//for (int i=0; i<material.mNumProperties; i++)
	//{
	//	auto& prop = *material.mProperties[i];
	//	int qqq = 90;
	//}

	std::vector<std::shared_ptr<Bind::Bindable>> bindablePtrs;/* 声明管线上所有绑定物的集合*/
	
	//const auto base = "Models\\brick_wall\\"s;	//自定义一个具体路径base = "Models\\brick_wall\\"s;存储贴图的路径
	const auto base = "Models\\gobber\\"s;	//自定义一个具体路径"Models\\gobber\\"s;存储哥布林模型的纹理的路径

	bool hasSpecularMap = false;//高光纹理开关;默认不带有高光贴图
	bool hasAlphaGloss = false;	//高光纹理的透明通道开关
	bool hasNormalMap = false;	//法线纹理开关,默认关闭
	bool hasDiffuseMap = false;	//漫反射纹理开关,默认关闭
	float shininess = 2.0f;	//自定义一个高光功率系数，默认为2.0f
	dx::XMFLOAT4 specularColor = { 0.18f,0.18f,0.18f,1.0f };//自定义镜面光颜色
	dx::XMFLOAT4 diffuseColor = { 0.45f,0.45f,0.85f,1.0f };//自定义漫反射光颜色

	/// 从硬盘里读各种贴图并创建出Texture绑定物，同时更新各类纹理开关为打开，最后创建采样器
	if (mesh.mMaterialIndex >= 0)
	{
		auto& material = *pMaterials[mesh.mMaterialIndex];	// 拿到当前mesh的材质		
		aiString texFileName;								// 创建一个aiString变量用于存储纹理文件的路径

		//material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName); // 拿到第一张漫反射纹理存到上面那个字符串里
		//bindablePtrs.push_back(Bind::Texture::Resolve(gfx,     /*Surface::FromFile*/(base + texFileName.C_Str()), 0 )); // 创建(实际上是Reslove泛型方法按给定参数查找并获得了)1个漫反射纹理, 位于插槽0 ，表示第[0]个纹理
		
		/// 读硬盘里漫反射纹理
		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)	//检查是否在硬盘上持有漫反射纹理
		{
			bindablePtrs.push_back(Bind::Texture::Resolve(gfx, (base + texFileName.C_Str()), 0 ));	// 创建(实际上是Reslove泛型方法按给定参数查找并获得了)1个漫反射纹理, 位于插槽0 ，表示第[0]个纹理
			hasDiffuseMap = true;																// 若查到漫反射纹理就打开漫反射开关
		}		
		else/* 若硬盘里不存在漫反射贴图*/
		{
			material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(diffuseColor));// 就使用自定义的漫反射光颜色
		}


		/// 读硬盘里镜面光纹理(有可能存在读不到的情况)
		if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS) //若该mesh确实在硬盘里持有高光贴图资源,拿一张镜面光纹理存到字符串里
		{
			//bindablePtrs.push_back(Bind::Texture::Resolve(gfx, (base + texFileName.C_Str()), 1 )); // 创建(实际上是Reslove泛型方法按给定参数查找并获得了)1个镜面光纹理，位于插槽1， 表示第[1]个纹理
			auto tex = Bind::Texture::Resolve(gfx, base + texFileName.C_Str(), 1); 
			hasAlphaGloss = tex->HasAlpha();			// 透明通道开关由解析出来的纹理决定
			bindablePtrs.push_back(std::move(tex));		//创建(实际上是Reslove泛型方法按给定参数查找并获得了)1个镜面光纹理，位于插槽1， 表示第[1]个纹理
			
			hasSpecularMap = true;																// 若能在硬盘里读到高光贴图，就开启高光开关
		}
		else/* 若硬盘里不存在高光贴图*/
		{
			material.Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor3D&>(specularColor));// 就使用自定义的高光颜色
		}


		if (!hasAlphaGloss) //若硬盘里没读到高光贴图资源且未开启alpha通道
		{
			material.Get(AI_MATKEY_SHININESS, shininess); //若没查找到高光贴图，就让当面材质读取上面自定义的高光参数
		}
		//material.GetTexture(aiTextureType_NORMALS, 0, &texFileName);  //读法线贴图
		//bindablePtrs.push_back(Texture::Resolve(gfx, (base + texFileName.C_Str()), 2)); 
		/// 读硬盘里的法线纹理
		if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
		{
			//bindablePtrs.push_back(Texture::Resolve(gfx, base + texFileName.C_Str(), 2));	// 创建(实际上是Reslove泛型方法按给定参数查找并获得了)1个镜面光纹理，位于插槽1， 表示第[2]个纹理
			auto tex = Bind::Texture::Resolve(gfx, base + texFileName.C_Str(), 2);
			hasAlphaGloss = tex->HasAlpha();
			bindablePtrs.push_back(std::move(tex));

			hasNormalMap = true;
		}

		///只有在三种贴图至少持有一种的时候才会创建采样器
		if (hasDiffuseMap || hasSpecularMap || hasNormalMap)	
		{
			bindablePtrs.push_back(Bind::Sampler::Resolve(gfx));// 创建采样器
		}
	}

	const auto meshTag = base + "%" + mesh.mName.C_Str();	
	const float scale = 6.0f;

	/// 依次开启漫反射纹理、高光纹理、法线纹理，并加载它 高光的纹理像素着色器(带法线版本)
	if (hasDiffuseMap && hasNormalMap && hasSpecularMap)
	{		
		// 为了加载动态布局系统并拿到真实的顶点数据; 要动态创建(即Append)顶点布局 、指定带位置、法线的顶点缓存、切线T、切线B、纹理
		Dvtx::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
			.Append(VertexLayout::Tangent)
			.Append(VertexLayout::Bitangent)
			.Append(VertexLayout::Texture2D)
		));

		// 遍历参数网格的所有顶点,存储 顶点位置和法线、T切线、B切线、纹理
		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			/* 使用EmplaceBack方法往顶点缓存末端构建一个顶点*/
			vbuf.EmplaceBack(
				dx::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mTangents[i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mBitangents[i]),
				*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i]) //由于模型顶点对于不同的纹理可能显示出不同的纹理坐标;所以这里理解为顶点i的[0]号坐标
			);
		}

		// 确定索引数组三角面;为每个三角面存储顶点索引
		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];	//拿到mesh的每个面
			assert(face.mNumIndices == 3);		// 确保每个面都是三角面
			// 用每个面的索引[0][1][2]填充 索引数组
			indices.push_back(face.mIndices[0]);	
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		bindablePtrs.push_back(Bind::VertexBuffer::Resolve(gfx, meshTag, vbuf));	//创建顶点缓存

		bindablePtrs.push_back(Bind::IndexBuffer::Resolve(gfx, meshTag, indices));	//创建索引缓存

		auto pvs = VertexShader::Resolve(gfx, "PhongVSNormalMap.cso");
		//auto pvsbc = static_cast<VertexShader&>(*pvs).GetBytecode();
		auto pvsbc = pvs->GetBytecode();
		bindablePtrs.push_back(std::move(pvs));	 // 创建顶点shader
	
		bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPSSpecNormalMap.cso"));	//创建像素shader，指定以"带法线贴图、高光贴图的像素着色器"

		bindablePtrs.push_back(Bind::InputLayout::Resolve(gfx, vbuf.GetLayout()/*.GetD3DLayout()*/, pvsbc)); // 创建输入布局
		
		Node::PSMaterialConstantFullmonte pmc;	//PSMaterialConstantFullmonte结构体数据位于头文件里
		pmc.specularPower = shininess;	//材质里高光由外部高光参数更新									
		pmc.hasGlossMap = hasAlphaGloss ? TRUE : FALSE;		//材质里透明通道由外部透明更新
		bindablePtrs.push_back(PixelConstantBuffer<Node::PSMaterialConstantFullmonte>::Resolve(gfx, pmc, 1u));			//创建像素常量缓存<材质>
	
	}
	/// 只开启漫反射、法线贴图，没有高光贴图
	else if(hasDiffuseMap && hasNormalMap)// 不存在高光纹理就加载默认的漫反射纹理像素着色器(带法线版本)
	{
		Dvtx::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
			.Append(VertexLayout::Tangent)
			.Append(VertexLayout::Bitangent)
			.Append(VertexLayout::Texture2D)
		));

		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				dx::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mTangents[i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mBitangents[i]),
				*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}

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

		bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));

		bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

		auto pvs = VertexShader::Resolve(gfx, "PhongVSNormalMap.cso");
		auto pvsbc = pvs->GetBytecode();
		bindablePtrs.push_back(std::move(pvs));		
		
		bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPSNormalMap.cso"));	////创建像素shader，指定以PhongPSNormalMap
		bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc)); // 创建输入布局
		
		struct PSMaterialConstantDiffnorm// 自定义 材质常量struct 并添加进绑定物集合
		{
			//DirectX::XMFLOAT3 color = { 0.6f,0.6f,0.8f }; // 由于模型已经有漫反射纹理了，所以这里不再使用自定义的颜色
			float specularIntensity /*= 0.18f*/; //高光强度
			float specularPower;			//高光功率

			BOOL  normalMapEnabled = TRUE;
			float padding[1];
		} pmc;
		//pmc.specularPower = shininess; // 注意这里结构体的成员高光功率由之前定义好的高光参数决定
		pmc.specularIntensity = (specularColor.x + specularColor.y + specularColor.z) / 3.0f; //高光强度等于高光颜色各分量和的三分之一
		bindablePtrs.push_back(Bind::PixelConstantBuffer<PSMaterialConstantDiffnorm>::Resolve(gfx, pmc, 1u));//创建出像素常数缓存<材质>
	}
	/// 若只开启漫反射
	else if (hasDiffuseMap)
	{
		Dvtx::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
			.Append(VertexLayout::Texture2D)
		));

		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				dx::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}

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

		bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));
		bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));
		auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		bindablePtrs.push_back(std::move(pvs));
		bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPS.cso"));
		bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

		struct PSMaterialConstantDiffuse
		{
			float specularIntensity /*= 0.18f*/;
			float specularPower;
			float padding[2];
		} pmc;
		pmc.specularPower = shininess;
		pmc.specularIntensity = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;
		// this is CLEARLY an issue... all meshes will share same mat const, but may have different
		// Ns (specular power) specified for each in the material properties... bad conflict
		bindablePtrs.push_back(PixelConstantBuffer<PSMaterialConstantDiffuse>::Resolve(gfx, pmc, 1u));
	}
	/// 若没在硬盘里读到任何纹理
	else if (!hasDiffuseMap && !hasNormalMap && !hasSpecularMap)
	{
		Dvtx::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
		));

		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				dx::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i])
			);
		}

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

		bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));

		bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

		auto pvs = VertexShader::Resolve(gfx, "PhongVSNotex.cso");
		auto pvsbc = pvs->GetBytecode();
		bindablePtrs.push_back(std::move(pvs));

		bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPSNotex.cso"));

		bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

		Node::PSMaterialConstantNotex pmc;//定义在头文件Node类下
		pmc.specularPower = shininess;		
		pmc.specularColor = specularColor;//结构体成员更新为自定义的镜面光颜色
		pmc.materialColor = diffuseColor;
		bindablePtrs.push_back(PixelConstantBuffer<Node::PSMaterialConstantNotex>::Resolve(gfx, pmc, 1u));
	}
	else
	{
		throw std::runtime_error("terrible combination of textures in material smh");
	}
	

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
