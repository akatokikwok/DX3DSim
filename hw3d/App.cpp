#include "App.h"
#include "Melon.h"
#include "Pyramid.h"
#include "Box.h"
#include "Sheet.h"
#include "SkinnedBox.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include <iterator>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

namespace dx = DirectX;

// 管理GDI+的变量
GDIPlusManager gdipm;

App::App()
	:
	wnd( 800,600,"The Window's Title of Renbin" )
	,light(wnd.Gfx())
{
	// 工厂类,含有构造持有图形,另外重载()操作符来达到动态选择哪个模型被创建实例化
	class Factory
	{
	public:
		Factory( Graphics& gfx )
			:
			gfx( gfx )
		{}

		std::unique_ptr<Drawable> operator()()
		{
			switch( typedist( rng ) )
			{
			case 0:
				return std::make_unique<Pyramid>(
					gfx,rng,adist,ddist,
					odist,rdist
				);
			case 1:
				return std::make_unique<Box>(
					gfx,rng,adist,ddist,
					odist,rdist,bdist
				);
			case 2:
				return std::make_unique<Melon>(
					gfx,rng,adist,ddist,
					odist,rdist,longdist,latdist
				);
			case 3:
				return std::make_unique<Sheet>(
					gfx,rng,adist,ddist,
					odist,rdist
				);
			case 4:
				return std::make_unique<SkinnedBox>(
					gfx,rng,adist,ddist,
					odist,rdist
				);
			default:
				assert( false && "bad drawable type in factory" );
				return {};
			}

			return std::make_unique<Box>(
				gfx, rng, adist, ddist,
				odist, rdist, bdist
				);

		}
	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_int_distribution<int> latdist{ 5,20 };
		std::uniform_int_distribution<int> longdist{ 10,40 };
		std::uniform_int_distribution<int> typedist{ 0,4 };//供给给switch 切不同模型枚举用
	};

	Factory f(wnd.Gfx());
	// reserve是容器预留空间，但在空间内不真正创建元素对象，所以在没有添加新的对象之前，不能引用容器内的元素。加入新的元素时，要调用push_back()/insert()函数。
	drawables.reserve( nDrawables );
	// 使用Generate_n来产生drawables子类, 第2个参数是数量，第三个是工厂类容器
	// generate_n() 功能:一个函数对象产生的值给一定的范围内指定数目的容器元素赋值
	std::generate_n( std::back_inserter( drawables ),nDrawables,Factory{ wnd.Gfx() } );

	//const auto s = Surface::FromFile("Images\\kappa50.png");
	// 构建左手透视投影矩阵
	wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
	//// 构建观察矩阵
	//wnd.Gfx().SetCamera(dx::XMMatrixTranslation(0.0f, 0.0f, 20.0f));
}

void App::DoFrame()
{
	const auto dt = timer.Mark() * speed_factor;
	//wnd.Gfx().ClearBuffer( 0.07f,0.0f,0.12f );
	
	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	// 每帧设置视图观察矩阵
	wnd.Gfx().SetCamera(cam.GetMatrix());

	// 以按空格切换启用\禁用imgui
	/*if (wnd.kbd.KeyIsPressed(VK_SPACE))
	{
		wnd.Gfx().DisableImgui();
	}
	else
	{
		wnd.Gfx().EnableImgui();
	}*/
	
	light.Bind(wnd.Gfx());

	for( auto& d : drawables )
	{
		// 若按空格键就暂停,否则继续让时长运行
		d->Update( wnd.kbd.KeyIsPressed( VK_SPACE ) ? 0.0f : dt );
		// 绑定各个Bindable实例并按索引绘制
		d->Draw( wnd.Gfx() );
	}	
	light.Draw(wnd.Gfx());

	#pragma region imgui绘制实例过程
	// imgui 实例; 若放在渲染d3d之前,则imgui位于背景,而D3D在前; Imgui没有z缓冲
	//ImGui_ImplDX11_NewFrame();
	//ImGui_ImplWin32_NewFrame();
	//ImGui::NewFrame();
	//// 此bool用于决定是否展示demo
	//static bool show_demo_window = true;
	
	// 显示imgui内容
	/*if (show_demo_window)
	{
		ImGui::ShowDemoWindow(&show_demo_window);
	}*/
	//ImGui::Render();// render方法吧所有设置转换成imgui内部格式
	//ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());// 把内部格式数据转换到dx11平台上
	#pragma endregion imgui绘制实例过程,弃用

	//static char buffer[1024];
	/// "速度创建"窗口
	if (ImGui::Begin("the windos's Title : Simulation Speed"))
	{		
		ImGui::SliderFloat("Speed Factor", &speed_factor, 0.0f, 4.0f);//增加1个滑块控制浮点值speed_factor

		// 1000除以帧率得到每帧毫秒数
		ImGui::Text("This Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		////ImGui::InputText("Butts", buffer, sizeof(buffer));
		ImGui::Text("Status: %s", wnd.kbd.KeyIsPressed(VK_SPACE) ? "PAUSED" : "RUNNING (hold 长按 spacebar to pause)");
	}
	ImGui::End();
	// 必须在速度创建窗口被绘制出来之后 才允许绘制 这个控制器窗口
	cam.SpawnControlWindow();

	light.SpawnControlWindow();

	// 使用交换链拿到后台缓存上屏,同时把imgui内部格式数据转换到dx11平台上
	wnd.Gfx().EndFrame();
}

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