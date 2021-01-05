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

// 管理GDI+的变量
GDIPlusManager gdipm;

App::App()
	:
	wnd( 800,600,"The Window's Title of Renbin" )
{
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
	drawables.reserve( nDrawables );
	// 使用Generate_n来产生drawables子类, 第2个参数是数量，第三个是工厂类容器
	std::generate_n( std::back_inserter( drawables ),nDrawables,Factory{ wnd.Gfx() } );

	const auto s = Surface::FromFile("Images\\kappa50.png");
	// 构建左手透视投影矩阵
	wnd.Gfx().SetProjection( DirectX::XMMatrixPerspectiveLH( 1.0f,3.0f / 4.0f,0.5f,40.0f ) );
}

void App::DoFrame()
{
	const auto dt = timer.Mark();
	wnd.Gfx().ClearBuffer( 0.07f,0.0f,0.12f );
	for( auto& d : drawables )
	{
		// 若按空格键就暂停,否则继续让时长运行
		d->Update( wnd.kbd.KeyIsPressed( VK_SPACE ) ? 0.0f : dt );
		// 绑定各个Bindable实例并按索引绘制
		d->Draw( wnd.Gfx() );
	}
	// 使用交换链拿到后台缓存上屏
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