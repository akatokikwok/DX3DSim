#include <Windows.h>
#include <winuser.h>
#include "WindowsMessageMap.h"
#include <iosfwd>
#include <sstream>

#include "Window.h"

//自定义1个消息处理机制
LRESULT CALLBACK cusWndProc(HWND hWnd,/*处理消息的窗口句柄*/ UINT msg,/*消息ID号*/ WPARAM wParam, LPARAM lParam)
{
	//创建静态窗口消息map
	static WindowsMessageMap mm;
	//对于收到的每一条消息,都用字符串形式打印出来
	OutputDebugString(
		mm(msg, lParam, wParam).c_str()//这里的括号在WindowsMessageMap类里进行的重载，效果是拿到格式化字符串
	);

	switch (msg)//根据消息ID来抉择
	{
	case WM_CLOSE:
		PostQuitMessage(69);
		break;
	case WM_KEYDOWN://对大小写不敏感
		if ( wParam=='F' )
		{
			SetWindowText( hWnd, "i have changed the title of this HWND");
		}
		break;
	case WM_KEYUP:
		if (wParam == 'F')
		{
			SetWindowText(hWnd, "the title of this HWND has recoverd");
		}
		break;
	case WM_CHAR:///大小写敏感,当想输入一段文字的时候，所用到的键就会出现WM_CHAR
		{
			static std::string title;
			title.push_back((char)wParam);
			SetWindowText(hWnd, title.c_str());/*此时窗口标题显示键盘上任意以Keydowm WParam的字*/
		}
		break;
	case WM_LBUTTONDOWN:
		{
			POINTS pt = MAKEPOINTS(lParam);//拿一个Point结构体
			std::ostringstream oss;//这里意思是把鼠标坐标结构体输出为文本
			oss << "(" << pt.x << "," << pt.y << ")";
			SetWindowText(hWnd, oss.str().c_str());
		}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);//调用标准库里的默认函数DefWindowsProc;
}


int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance, 
	_In_ LPSTR lpCmdLine, 
	_In_ int nCmdShow
)
{
	
#pragma region 弃用ver1.0.5
	////注册窗口类
	//WNDCLASSEX wc = { 0 };
	//wc.cbSize = sizeof(wc);
	//wc.style = CS_OWNDC;
	//wc.lpfnWndProc = ::cusWndProc; //定义windowsProcedure指针
	//wc.cbClsExtra = 0;
	//wc.cbWndExtra = 0;
	//wc.hInstance = hInstance;
	//wc.hIcon = nullptr;
	//wc.hIconSm = nullptr;
	//wc.hCursor = nullptr;
	//wc.hbrBackground = nullptr;
	//wc.lpszMenuName = nullptr;
	//const auto pClassName = "hw3d_className";
	//wc.lpszClassName = pClassName;
	//RegisterClassEx(&wc);
	////创建窗口实例
	//HWND hWnd = CreateWindowEx(
	//	0,pClassName,
	//	"babababa,this is a chuangkou",
	//	WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
	//	200,200,
	//	640,480,
	//	nullptr,nullptr, hInstance,nullptr
	//);
	//ShowWindow(hWnd, SW_SHOW);
	////while (true)
	////	;
#pragma endregion 弃用ver1.0.5

	try
	{
		/// 利用Window框架类的带参构造可以创造出1个类的多个窗口实例，均可以工作
		Window wnd(800, 300, "chuang kou wenben");
		//Window wnd2(300, 800, "chuang kou 2 wen ben");

		///消息结构体构造
		MSG msg;
		BOOL gResult;
		while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)//大于0就是收到消息，=0是退出,-1是出错
		{
			TranslateMessage(&msg);//TranslateMessage适当条件下可以把wm_keydown同时转成wm_char
			DispatchMessage(&msg);

			#pragma region 按下alt键事件 测试
//if (wnd.kbd.KeyIsPressed(VK_MENU) )
//			{
//				MessageBox(nullptr, "Something happened", "Space Key Was Pressed", MB_OK|MB_ICONEXCLAMATION);
//			}
#pragma endregion 按下alt键事件 测试

#pragma region 鼠标光标移动事件 测试
			while (!wnd.mouse.IsEmpty())
			{
				const auto e = wnd.mouse.Read();
				if (e.GetType() == Mouse::Event::Type::Move)
				{
					std::ostringstream oss;
					oss << "Mouse Postion: (" << e.GetPosX() << "," << e.GetPosY()<<")";
					wnd.SetTitle(oss.str());
				}
			}
#pragma endregion 鼠标光标移动事件 测试
		}
		if (gResult == -1)
		{
			return -1;
		}
		else if (gResult == 0)
		{
			return msg.wParam;//msg.wParam是PostQuitMessage退出时候显示的code通知
		}
	}
	catch (const GrbException& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;

}

