#include "App.h"
#include <iomanip>
#include <iosfwd>
#include <sstream>
#include "Box.h"
#include <memory>
#include <DirectXCollision.h>

#include "Melon.h"
#include "Pyramid.h"
#include <algorithm>
#include "ChiliMath.h"

App::App()
	:
	wnd(800,600, "App::APP()::Defalut Window For U")
{
	#pragma region ver1.0.22弃用
	//std::mt19937 rng(std::random_device{}());
	//std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);
	//std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 1.0f);
	//std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.08f);
	//std::uniform_real_distribution<float> rdist(6.0f, 20.0f);
	//// 创建80个Box对象
	//for (auto i = 0; i < 180; i++)
	//{
	//	boxes.push_back(std::make_unique<Box>(
	//		wnd.Gfx(), rng, adist,
	//		ddist, odist, rdist
	//		));
	//}
	#pragma endregion ver1.0.22弃用

	// 工厂类 容器，负责创建图像和 根据Drawable实例类型传递随即参数来创建几何体
	class Factory
	{
	public:
		// 构造体就是创建图像类
		Factory(Graphics& gfx)
			:
			gfx(gfx)
		{}

		// 重载操作符() 根据实例类型创建几何体,并将随即参数传递过去
		std::unique_ptr<Drawable> operator()()
		{
			switch (typedist(rng))
			{
			case 0:
				return std::make_unique<Pyramid>(
					gfx, rng, adist, ddist,
					odist, rdist
					);
			case 1:
				return std::make_unique<Box>(
					gfx, rng, adist, ddist,
					odist, rdist, bdist
					);
			case 2:
				return std::make_unique<Melon>(
					gfx, rng, adist, ddist,
					odist, rdist, longdist, latdist
					);
			default:
				assert(false && "bad drawable type in factory");
				return {};
			}
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
		std::uniform_int_distribution<int> typedist{ 0,2 };
	};

	Factory f(wnd.Gfx());// 创建工厂实例
	drawables.reserve(nDrawables);// 为集合分配空间

	//使用Generate_n来产生drawables子类, 第2个参数是数量，第三个是工厂类容器
	std::generate_n(std::back_inserter(drawables), nDrawables, f);

	/*构建左手透视投影矩阵*/
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));

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

	for (auto& d : drawables)
	{
		d->Update(dt);
		d->Draw(wnd.Gfx());
	}
	//让交换链内缓存上屏
	wnd.Gfx().EndFrame();
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


