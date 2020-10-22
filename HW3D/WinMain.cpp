#include <Windows.h>
#include <winuser.h>
#include "WindowsMessageMap.h"


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
	case WM_KEYDOWN:
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
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);//调用标准库里的默认函数DefWindowsProc;
}

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	
	//注册窗口类
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = ::cusWndProc; //定义windowsProcedure指针
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = nullptr;
	wc.hIconSm = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	const auto pClassName = "hw3d_className";
	wc.lpszClassName = pClassName;
	RegisterClassEx(&wc);
	//创建窗口实例
	HWND hWnd = CreateWindowEx(
		0,pClassName,
		"babababa,this is a chuangkou",
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		200,200,
		640,480,
		nullptr,nullptr, hInstance,nullptr
	);
	ShowWindow(hWnd, SW_SHOW);
	//while (true)
	//	;
	
	///消息结构体构造
	MSG msg;
	BOOL gResult;
	while ( (gResult=GetMessage(&msg,nullptr,0,0)) > 0 )//大于0就是收到消息，=0是退出,-1是出错
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (gResult==-1)
	{
		return -1;
	}
	else if (gResult == 0)
	{
		return msg.wParam;//msg.wParam是PostQuitMessage退出时候显示的code通知
	}

	/*return 0;*/
}

