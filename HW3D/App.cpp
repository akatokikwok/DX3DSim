#include "App.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"

namespace dx = DirectX;

GDIPlusManager gdipm;

App::App()
	:
	wnd(1280, 900, "The GRB'S Rending Box"),
	light(wnd.Gfx())
{
	wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 40.0f));

	wnd.DisableCursor();//应用初始化的时候默认关闭光标
}

void App::DoFrame()
{
	const auto dt = timer.Mark() * speed_factor;
	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd.Gfx().SetCamera(cam.GetMatrix());
	light.Bind(wnd.Gfx(), cam.GetMatrix());// 光源每帧更新pixel常量缓存并同时设置到管线 

	/*for( auto& d : drawables )
	{
		d->Update( wnd.kbd.KeyIsPressed( VK_SPACE ) ? 0.0f : dt );
		d->Draw( wnd.Gfx() );
	}*/

	//const auto transform = dx::XMMatrixRotationRollPitchYaw(pos.roll, pos.pitch, pos.yaw) *
	//	dx::XMMatrixTranslation(pos.x, pos.y, pos.z);// 自定义变换==旋转矩阵*移动矩阵
	//nano.Draw(wnd.Gfx(), transform);// 绘制指定的模型

	nano.Draw(wnd.Gfx());
	light.Draw(wnd.Gfx());

	cam.SpawnControlWindow();
	light.SpawnControlWindow();

	ShowImguiDemoWindow();
	nano.ShowWindow();
	ShowRawInputWindow();

	// present
	wnd.Gfx().EndFrame();
}

void App::ShowImguiDemoWindow()
{
	static bool show_demo_window = true;
	if (show_demo_window)
	{
		ImGui::ShowDemoWindow(&show_demo_window);
	}
}

void App::ShowRawInputWindow()
{
	// 累加光标的值
	while (const auto d = wnd.mouse.ReadRawDelta())
	{
		x += d->x;
		y += d->y;
	}
	if (ImGui::Begin("Raw Input"))
	{
		ImGui::Text("Tally: (%d,%d)", x, y);
	}
	ImGui::End();
}

App::~App()
{}


int App::Go()
{
	while (true)
	{
		// process all messages pending, but to not block for new messages
		if (const auto ecode = Window::ProcessMessages())
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		DoFrame();
	}
}