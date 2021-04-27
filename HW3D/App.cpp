#include "App.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
//#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include "VertexBuffer.h"
//#include "TexturePreprocessor.h"
//#include <shellapi.h>
//#include <dxtex/DirectXTex.h>
#include "ChiliUtil.h"
#include "DynamicConstant.h"

namespace dx = DirectX;

//GDIPlusManager gdipm;

App::App(const std::string& commandLine)
	:
	commandLine(commandLine),
	wnd(1280, 720, "The GRB'S Rending Box"),
	scriptCommander(TokenizeQuoted(commandLine)), //放在初始之列表示为了在初始化一大堆模型之前就可以做这步逻辑
	light(wnd.Gfx())	
{
	/*auto scratch = DirectX::ScratchImage{};
	DirectX::LoadFromWICFile(L"Images\\brickwall.jpg", DirectX::WIC_FLAGS_NONE, nullptr, scratch);
	auto image = scratch.GetImage(0, 0, 0);
	auto a = image->pixels[0];
	auto b = image->pixels[1];
	auto c = image->pixels[2];
	auto d = image->pixels[3];*/

	//// makeshift cli for doing some preprocessing 
	//if (this->commandLine != "")
	//{
	//	int nArgs;
	//	const auto pLineW = GetCommandLineW();
	//	const auto pArgs = CommandLineToArgvW(pLineW, &nArgs);//解析命令行字符并获得指向命令行的一串参数指针数组
	//	if (nArgs >= 3 && std::wstring(pArgs[1]) == L"--twerk-objnorm")
	//	{
	//		const std::wstring pathInWide = pArgs[2];
	//		//按Y轴翻转入参模型里所有的法线贴图
	//		TexturePreprocessor::FlipYAllNormalMapsInObj(
	//			std::string(pathInWide.begin(), pathInWide.end())
	//		);
	//		throw std::runtime_error("Normal maps all processed successfully. Just kidding about that whole runtime error thing.");
	//	}
	//	else if (nArgs >= 3 && std::wstring(pArgs[1]) == L"--twerk-flipy")
	//	{
	//		const std::wstring pathInWide = pArgs[2];
	//		const std::wstring pathOutWide = pArgs[3];
	//		TexturePreprocessor::FlipYNormalMap(
	//			std::string(pathInWide.begin(), pathInWide.end()),
	//			std::string(pathOutWide.begin(), pathOutWide.end())
	//		);
	//		throw std::runtime_error("Normal map processed successfully. ");//在上一步执行过操作之后本步抛出一个通知
	//	}
	//	else if (nArgs >= 4 && std::wstring(pArgs[1]) == L"--twerk-validate")///检索检查法线贴图
	//	{
	//		const std::wstring minWide = pArgs[2];
	//		const std::wstring maxWide = pArgs[3];
	//		const std::wstring pathWide = pArgs[4];
	//		TexturePreprocessor::ValidateNormalMap(
	//			std::string(pathWide.begin(), pathWide.end()), std::stof(minWide), std::stof(maxWide)
	//		);
	//		throw std::runtime_error("Normal map validated successfully. Just kidding about that whole runtime error thing.");
	//	}
	//}

	Dcb::Layout s;
	s.Add<Dcb::Struct>("butts");
	s["butts"].Add<Dcb::Float3>("pubes");
	s["butts"].Add<Dcb::Float>("dank");
	s.Add<Dcb::Float>("woot");
	s.Add<Dcb::Array>("arr");
	s["arr"].Set<Dcb::Struct>(4);
	s["arr"].T().Add<Dcb::Float3>("twerk");
	s["arr"].T().Add<Dcb::Array>("werk");
	s["arr"].T()["werk"].Set<Dcb::Float>(6);
	s["arr"].T().Add<Dcb::Array>("meta");
	s["arr"].T()["meta"].Set<Dcb::Array>(6);
	s["arr"].T()["meta"].T().Set<Dcb::Matrix>(4);
	Dcb::Buffer b(s);
	b["butts"]["pubes"] = DirectX::XMFLOAT3{ 69.0f,0.0f,0.0f };
	b["butts"]["dank"] = 420.0f;
	b["woot"] = 42.0f;
	b["arr"][2]["werk"][5] = 111.0f;
	dx::XMStoreFloat4x4(
		&b["arr"][2]["meta"][5][3],
		dx::XMMatrixIdentity()
	);
	float k = b["woot"];
	dx::XMFLOAT3 v = b["butts"]["pubes"];
	float u = b["butts"]["dank"];
	float er = b["arr"][2]["werk"][5];
	dx::XMFLOAT4X4 eq = b["arr"][2]["meta"][5][3];

	//wall.SetRootTransform(dx::XMMatrixTranslation(-12.0f, 0.0f, 0.0f));//设置墙模型的根节点Transform
	//tp.SetPos({ 12.0f,0.0f,0.0f });									//设置TestPlane绘制物的位置
	//gobber.SetRootTransform(dx::XMMatrixTranslation(0.0f, 0.0f, -4.0f));
	//nano.SetRootTransform(dx::XMMatrixTranslation(0.0f, -7.0f, 6.0f));

	//auto a = Bind::VertexShader::Resolve(wnd.Gfx(), "PhongVS.cso");// 解析并拿到名为PhongVS.cso的顶点着色器
	//auto b = Bind::Sampler::Resolve(wnd.Gfx());	//解析并拿到采样器
	//auto c = Bind::Sampler::Resolve(wnd.Gfx());
	//wnd.DisableCursor();//应用初始化的时候默认关闭光标

	bluePlane.SetPos( cam.GetPos() );
	redPlane.SetPos(cam.GetPos());
	wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 400.0f));
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

	//wall.Draw(wnd.Gfx());
	//tp.Draw(wnd.Gfx());
	//nano.Draw(wnd.Gfx());
	//gobber.Draw(wnd.Gfx());

	//nano2.Draw(wnd.Gfx());
	light.Draw(wnd.Gfx());//注意此处灯光被覆写，所以下一步绘制plane模型时候保留了插槽0，所以会渲染失败，故要在着色器里将灯光着色器绑定至下一个插槽1
	//plane.Draw(wnd.Gfx());
	//cube.Draw(wnd.Gfx());
	sponza.Draw(wnd.Gfx());
	bluePlane.Draw(wnd.Gfx());
	redPlane.Draw( wnd.Gfx() );

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
	//nano.ShowWindow("Model 1");
	//nano2.ShowWindow("Model 2");
	//plane.SpawnControlWindow(wnd.Gfx());	
	
	//gobber.ShowWindow(wnd.Gfx(), "gobber");
	//wall.ShowWindow(wnd.Gfx(), "Wall");
	//tp.SpawnControlWindow(wnd.Gfx());
	//nano.ShowWindow(wnd.Gfx(), "Nano");

	sponza.ShowWindow(wnd.Gfx(), "Sponza");
	bluePlane.SpawnControlWindow(wnd.Gfx(), "Blue Plane");
	redPlane.SpawnControlWindow(wnd.Gfx(), "Red Plane");
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