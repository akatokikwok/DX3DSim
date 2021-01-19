#include "App.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include <iterator>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "AssTest.h"
#include "Vertex.h"

namespace dx = DirectX;

GDIPlusManager gdipm;

//void f()
//{
//	//// 给布局附着几个元素,比如法线和位置
//	//VertexLayout vl;
//	//vl.Append<VertexLayout::Position3D>()
//	//	.Append<VertexLayout::Normal>();
//	//// 基于该顶点布局创建1个顶点缓冲
//	//VertexBuffer vb(std::move(vl));
//	//// 使用EmplaceBack添加1个顶点
//	//vb.EmplaceBack(dx::XMFLOAT3{ 1.0f,1.0f,5.0f }, dx::XMFLOAT3{ 2.0f,1.0f,4.0f });
//	//// 按重载的[]来按索引再访问顶点缓存的属性
//	//auto pos = vb[0].Attr<VertexLayout::Position3D>();
//
//	VertexBuffer vb(
//		std::move(
//			VertexLayout{}
//			.Append<VertexLayout::Position3D>()
//			.Append<VertexLayout::Normal>()
//			.Append<VertexLayout::Texture2D>()
//		));
//	vb.EmplaceBack(
//		dx::XMFLOAT3{ 1.0f,1.0f,5.0f },
//		dx::XMFLOAT3{ 2.0f,1.0f,4.0f },
//		dx::XMFLOAT2{ 6.0f,9.0f }
//	);
//	vb.EmplaceBack(
//		dx::XMFLOAT3{ 6.0f,9.0f,6.0f },
//		dx::XMFLOAT3{ 9.0f,6.0f,9.0f },
//		dx::XMFLOAT2{ 4.2f,0.0f }
//	);
//
//	auto pos = vb[0].Attr<VertexLayout::Position3D>();
//	auto nor = vb[0].Attr<VertexLayout::Normal>();
//	auto tex = vb[1].Attr<VertexLayout::Texture2D>();
//	vb.Back().Attr<VertexLayout::Position3D>().z = 420.0f;
//	pos = vb.Back().Attr<VertexLayout::Position3D>();
//
//	const auto& cvb = vb;
//	pos = cvb[1].Attr<VertexLayout::Position3D>();
//}

App::App()
	:
	wnd( 800,600,"Renbin's Demo Window" ),
	light( wnd.Gfx() )
{
	//f();

	//class Factory
	//{
	//public:
	//	Factory( Graphics& gfx )
	//		:
	//		gfx( gfx )
	//	{}
	//	std::unique_ptr<Drawable> operator()()
	//	{
	//		//  随机构造一个模型材质参数
	//		const DirectX::XMFLOAT3 mat = { cdist(rng),cdist(rng),cdist(rng) };
	//		
	//		switch (sdist(rng))
	//		{
	//		case 0:
	//			return std::make_unique<Box>(
	//				gfx, rng, adist, ddist,
	//				odist, rdist, bdist, mat
	//				);
	//		case 1:
	//			return std::make_unique<Cylinder>(
	//				gfx, rng, adist, ddist, odist,
	//				rdist, bdist, tdist
	//				);
	//		case 2:
	//			return std::make_unique<Pyramid>(
	//				gfx, rng, adist, ddist, odist,
	//				rdist, tdist
	//				);
	//		case 3:
	//			return std::make_unique<SkinnedBox>(
	//				gfx, rng, adist, ddist,
	//				odist, rdist
	//				);
	//		case 4:
	//			return std::make_unique<AssTest>(
	//				gfx, rng, adist, ddist,
	//				odist, rdist, mat/*这里随机给一个材质颜色*/, 1.5f
	//				);
	//		default:
	//			assert(false && "impossible drawable option in factory");
	//			return {};
	//		}
	//	}
	//private:
	//	Graphics& gfx;
	//	std::mt19937 rng{ std::random_device{}() };// rng是一个随机数

	//	std::uniform_int_distribution<int> sdist{ 0,4 };// 控制上述Switch种类的case数量
	//	std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
	//	std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
	//	std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
	//	std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
	//	std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
	//	std::uniform_real_distribution<float> cdist{ 0.0f,1.0f };// 新增1个变量用于初始化材质
	//	std::uniform_int_distribution<int> tdist{ 3,30 };
	//};

	//drawables.reserve( nDrawables );
	//std::generate_n( std::back_inserter( drawables ),nDrawables,Factory{ wnd.Gfx() } );

	//// 遍历所有的绘制物,如果绘制物是盒子实例,就把该盒子添加进渲染box集合
	//for (auto& pd : drawables)
	//{
	//	if (auto pb = dynamic_cast<Box*>(pd.get()))
	//	{
	//		boxes.push_back(pb);
	//	}
	//}

	wnd.Gfx().SetProjection( dx::XMMatrixPerspectiveLH( 1.0f,3.0f / 4.0f,0.5f,40.0f ) );
}

void App::DoFrame()
{
	const auto dt = timer.Mark() * speed_factor;
	wnd.Gfx().BeginFrame( 0.07f,0.0f,0.12f );
	wnd.Gfx().SetCamera( cam.GetMatrix() );
	// 光源每帧更新pixel常量缓存并同时设置到管线 
	light.Bind( wnd.Gfx(), cam.GetMatrix());

	/*for( auto& d : drawables )
	{
		d->Update( wnd.kbd.KeyIsPressed( VK_SPACE ) ? 0.0f : dt );
		d->Draw( wnd.Gfx() );
	}*/

	// 自定义变换==旋转矩阵*移动矩阵
	const auto transform = dx::XMMatrixRotationRollPitchYaw(pos.roll, pos.pitch, pos.yaw) *
		dx::XMMatrixTranslation(pos.x, pos.y, pos.z);
	// 为模型根节点绘制所有Mesh
	nano.Draw(wnd.Gfx(), transform);	

	// 按索引绘制光源
	light.Draw( wnd.Gfx() );

	// imgui windows
	/*SpawnSimulationWindow();*/
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	//SpawnBoxWindowManagerWindow();	// 生成窗口集中管理窗口
	//SpawnBoxWindows();				// 为每个单独生成1个窗口
	
	// 显示模型控制窗口
	ShowModelWindow();

	//// 
	//if (ImGui::Begin("Boxes"))
	//{
	//	using namespace std::string_literals;
	//	const auto preview = comboBoxIndex ? std::to_string(*comboBoxIndex) : "Choose a box..."s;
	//	if (ImGui::BeginCombo("Box Number", preview.c_str()))
	//	{
	//		// 为所有的box实例创建1个Combo框
	//		for (int i = 0; i < boxes.size(); i++)
	//		{
	//			const bool selected = *comboBoxIndex == i;
	//			if (ImGui::Selectable(std::to_string(i).c_str(), selected))
	//			{
	//				comboBoxIndex = i;
	//			}
	//			if (selected)
	//			{
	//				ImGui::SetItemDefaultFocus();
	//			}
	//		}
	//		ImGui::EndCombo();
	//	}
	//	if (ImGui::Button("Spawn Control Window") && comboBoxIndex)
	//	{
	//		boxControlIds.insert(*comboBoxIndex);
	//		comboBoxIndex.reset();
	//	}
	//}
	//ImGui::End();
	//// 用ID为标准遍历所有box实例,为其生成控制窗口
	//for (auto id : boxControlIds)
	//{
	//	boxes[id]->SpawnControlWindow(id, wnd.Gfx());
	//}

	// present
	wnd.Gfx().EndFrame();
}

void App::ShowModelWindow()
{
	if (ImGui::Begin("Model"))
	{
		using namespace std::string_literals;

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


//void App::SpawnSimulationWindow() noexcept
//{
//	if (ImGui::Begin("Simulation Speed"))
//	{
//		ImGui::SliderFloat("Speed Factor", &speed_factor, 0.0f, 6.0f, "%.4f", 3.2f);
//		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
//		ImGui::Text("Status: %s", wnd.kbd.KeyIsPressed(VK_SPACE) ? "PAUSED" : "RUNNING (hold spacebar to pause)");
//	}
//	ImGui::End();
//}
//
//void App::SpawnBoxWindowManagerWindow() noexcept
//{
//	if (ImGui::Begin("Boxes"))
//	{
//		using namespace std::string_literals;
//		// 提交框里显示的文字
//		const auto preview = comboBoxIndex ? std::to_string(*comboBoxIndex) : "Choose a box..."s;
//
//		if (ImGui::BeginCombo("Box Number", preview.c_str()))
//		{
//			for (int i = 0; i < boxes.size(); i++)
//			{
//				const bool selected = *comboBoxIndex == i;
//				if (ImGui::Selectable(std::to_string(i).c_str(), selected))
//				{
//					comboBoxIndex = i;
//				}
//				if (selected)
//				{
//					ImGui::SetItemDefaultFocus();
//				}
//			}
//			ImGui::EndCombo();
//		}
//
//		if (ImGui::Button("Spawn Control Window") && comboBoxIndex)
//		{
//			boxControlIds.insert(*comboBoxIndex);
//			comboBoxIndex.reset();
//		}
//	}
//	ImGui::End();
//}
//
//void App::SpawnBoxWindows() noexcept
//{
//	for (auto i = boxControlIds.begin(); i != boxControlIds.end(); )
//	{
//		// 检查每一个盒子是否生成过了,若没有生成,则擦除这个ID;否则继续遍历
//		if (!boxes[*i]->SpawnControlWindow(*i, wnd.Gfx()))
//		{
//			i = boxControlIds.erase(i);
//		}
//		else
//		{
//			i++;
//		}
//	}
//}


App::~App()
{}


int App::Go()
{
	while( true )
	{
		// process all messages pending, but to not block for new messages
		if( const auto ecode = Window::ProcessMessages() )
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		DoFrame();
	}
}