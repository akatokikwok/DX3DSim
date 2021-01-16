#include "AssTest.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

AssTest::AssTest(Graphics& gfx, std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	DirectX::XMFLOAT3 material,
	float scale)
	:
	TestObject(gfx, rng, adist, ddist, odist, rdist)
{
	namespace dx = DirectX;

	if (!IsStaticInitialized())
	{
		// 顶点含有位置pos和法线n
		struct Vertex
		{
			dx::XMFLOAT3 pos;
			dx::XMFLOAT3 n;
		};
		// 读入指定路径的3D模型
		Assimp::Importer imp;
		const auto pModel = imp.ReadFile("models\\suzanne.obj",
			aiProcess_Triangulate |// 把加载模型选项里设置为"包括三角形部分-把所有四边形或者多边形分解成简单三角形"
			aiProcess_JoinIdenticalVertices
		);
		// 拿到模型的0号网格
		const auto pMesh = pModel->mMeshes[0];

		/* 加载用于承载数据的顶点数组 */
		std::vector<Vertex> vertices;
		// 给顶点数组预留模型顶点数数量
		vertices.reserve(pMesh->mNumVertices);
		// 遍历模型每一个顶点 都把它的位置和法线统计进来
		for (unsigned int i = 0; i < pMesh->mNumVertices; i++)
		{
			vertices.push_back(
				{				
					{ pMesh->mVertices[i].x * scale,pMesh->mVertices[i].y * scale,pMesh->mVertices[i].z * scale },// 每个顶点位置取x,y,z并乘以参数scale构成顶点位置
					*reinterpret_cast<dx::XMFLOAT3*>(&pMesh->mNormals[i])
				});
		}

		/* 加载用于承载数据的索引数组 */
		/* 注:网格则由多个顶点\面\索引构成*/
		std::vector<unsigned short> indices;
		// Mesh被分解成许多面之后每个面就只有3个顶点3个索引,mesh数量乘3就是需要的索引总数量
		indices.reserve(pMesh->mNumFaces * 3);
		// 遍历所有的三角面 把每个三角面索引都添加进数组
		for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
		{
			const auto& face = pMesh->mFaces[i];// 拿到每一个面
			assert(face.mNumIndices == 3); // 确保每个单独面都是三角形
			// 把每个单三角面的索引添加进 indices
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		// 利用上述顶点数组创建顶点缓存
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));
		// 利用上述索引数组创建索引缓存
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));
		// 利用带法线的shader创建顶点着色器
		auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));
		// 利用带法线的shader创建像素着色器
		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));
		// 自定义输入布局并创建
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));
		// 创建输入图元
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
		// 自定义材质结构体(带颜色、镜面光)并由参数材质决定颜色;同时创建出像素shader常量缓存并绑在1号插槽
		struct PSMaterialConstant
		{
			DirectX::XMFLOAT3 color;
			float specularIntensity = 0.6f;
			float specularPower = 30.0f;
			float padding[3];
		} pmc;
		pmc.color = material;
		AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));
	}
	else
	{
		// 否则解决重复添加索引
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}