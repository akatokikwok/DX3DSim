#include "App.h"
#include <iomanip>
#include <iosfwd>
#include <sstream>
#include "Box.h"
#include <memory>
#include <DirectXCollision.h>

App::App()
	:
	wnd(800,600, "App::APP()::Defalut Window For U")
{
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.3f);
	std::uniform_real_distribution<float> rdist(6.0f, 20.0f);
	for (auto i = 0; i < 80; i++)
	{
		boxes.push_back(std::make_unique<Box>(
			wnd.Gfx(), rng, adist,
			ddist, odist, rdist
			));
	}
	/*构建左手透视投影矩阵*/
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));

}

App::~App()
{}

int App::Go()
{
	#pragma region ver1.0.10弃用
	/*
	MSG msg;
	bool gResult;
	while ( (gResult = GetMessage(&msg, nullptr, 0, 0)) > 0) 此处的GetMessage方法已经不能满足需求，改用PcocesssMessage()里的PeekMessage方法
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		App::DoFrame();
	}
	if (gResult == -1)
		throw CHWND_LAST_EXCEPT();

	return msg.wParam;
	*/
#pragma endregion ver1.0.10弃用

	while (true)
	{
		/*ecode是optional型，optional型会重载BOOL型,其不为空就使ecode返回真*/
		if (const auto ecode = Window::ProcessMessages())
		{
			//拿取消息并返回ecode
			return *ecode;
		}
		//否则若上述BOOL为空（返回假）,执行每帧逻辑函数
		App::DoFrame();
	}
}

void App::DoFrame()
{	
#pragma region 于ver1.0.13弃用
	///*获取从窗口创立以来经过的累计时长*/
	//const float t = timer.Peek();
	//std::ostringstream oss;
	////对时长变量t进行四舍五入1位
	//oss << "Time elapsed:::::::" << std::setprecision(1) << std::fixed << t << "s";
	//wnd.SetTitle(oss.str());
#pragma endregion 于ver1.0.13弃用

#pragma region ver1.0.20弃用
	////构造一个变化的范围数
	//const float c = sin(timer.Peek()) / 2.0f + 0.5f;
	///* 以填充色清屏先*/
	//wnd.Gfx().ClearBuffer(c, c, 1.0f);
	///* 调用绘制三角形方法*/
	////此处timer.peek动态修改角度即矩阵的值达到每一帧矩阵变化以催动顶点移动
	//wnd.Gfx().DrawTestTriangle( timer.Peek() );
#pragma endregion ver1.0.20弃用

	auto dt = timer.Mark();
	wnd.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f);

	for (auto& b : boxes)
	{
		b->Update(dt);
		b->Draw(wnd.Gfx());
	}
	//让交换链内缓存上屏
	wnd.Gfx().EndFrame();

}

