#include "App.h"
#include <iomanip>
#include <iosfwd>
#include <sstream>

App::App()
	:
	wnd(800,600, "App::APP()::Defalut Window For U")
{

}

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
		if (const auto ecode = Window::ProcessMessage())
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

	//构造一个变化的范围数
	const float c = sin(timer.Peek()) / 2.0f + 0.5f;
	wnd.Gfx().ClearBuffer(c, c, 1.0f);
	//让交换链内缓存上屏
	wnd.Gfx().EndFrame();

}

