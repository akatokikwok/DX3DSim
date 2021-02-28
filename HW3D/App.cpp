#include "App.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include "VertexBuffer.h"

namespace dx = DirectX;

GDIPlusManager gdipm;

App::App()
	:
	wnd(1280, 720, "The GRB'S Rending Box"),
	light(wnd.Gfx())
{
	wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 40.0f));

	auto a = Bind::VertexShader::Resolve(wnd.Gfx(), "PhongVS.cso");// 解析并拿到名为PhongVS.cso的顶点着色器
	auto b = Bind::Sampler::Resolve(wnd.Gfx());	//解析并拿到采样器
	auto c = Bind::Sampler::Resolve(wnd.Gfx());

	//wnd.DisableCursor();//应用初始化的时候默认关闭光标
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


	while (const auto e = wnd.kbd.ReadKey())
	{
		// 每帧检测是否按下了INSERT键位;若处于光标启用状态就关闭光标并更新状态为禁用;若处于光标禁用状态就启用光标并更新状态为启用
		/*if (e->IsPress() && e->GetCode() == VK_INSERT)*/
		if (!e->IsPress())
		{
			continue;
		}
		// e->GetCode()根据按下的键位来判定所作处理
		switch( e->GetCode()) 
		{
		// 使用Escape来控制光标显隐
		case VK_ESCAPE:
			if (wnd.CursorEnabled())
			{
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();//启用鼠标的原生输入
			}
			else
			{
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();//禁用鼠标的原生输入
			}
			break;
		// 使用F1键来控制是否显示Imgui窗口
		case VK_F1:
			showDemoWindow = true;
			break;
		}
	}

	/* 鼠标光标禁用(按了F1)情况下的一些键位操作:可以控制摄像机移动*/
	if (!wnd.CursorEnabled())
	{
		if (wnd.kbd.KeyIsPressed('W'))
		{
			cam.Translate({ 0.0f,0.0f,dt });
		}
		if (wnd.kbd.KeyIsPressed('A'))
		{
			cam.Translate({ -dt,0.0f,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('S'))
		{
			cam.Translate({ 0.0f,0.0f,-dt });
		}
		if (wnd.kbd.KeyIsPressed('D'))
		{
			cam.Translate({ dt,0.0f,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('R'))
		{
			cam.Translate({ 0.0f,dt,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('F'))
		{
			cam.Translate({ 0.0f,-dt,0.0f });
		}
	}

	// 只要读到了鼠标数据
	while (const auto delta = wnd.mouse.ReadRawDelta())
	{	// 且光标禁用(F1状态)
		if (!wnd.CursorEnabled())
		{	
			cam.Rotate( (float)delta->x, (float)delta->y);// 对摄像机执行按鼠标输入分量的旋转
		}
	}

	// imgui窗口
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	// 根据助手开关来动态显隐Imgui助手DEMO窗口
	ShowImguiDemoWindow();
	// 展示模型IMGUI窗口
	nano.ShowWindow();
	//ShowRawInputWindow();

	// present
	wnd.Gfx().EndFrame();
}

void App::ShowImguiDemoWindow()
{
	if (showDemoWindow)
	{
		ImGui::ShowDemoWindow(&showDemoWindow);
	}
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