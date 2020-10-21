#include <Windows.h>
#include <winuser.h>


//自定义1个消息处理机制
LRESULT CALLBACK cusWndProc(HWND hWnd,/*处理消息的窗口句柄*/ UINT msg,/*消息ID号*/ WPARAM wParam, LPARAM lParam)
{
	switch (msg)//根据消息ID来抉择
	{
	case WM_CLOSE:
		PostQuitMessage(69);
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
	const auto pClassName = "hw3dbutts";
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
	
	//消息结构体构造
	MSG msg;
	BOOL gResult;
	while ( (gResult=GetMessage( &msg, nullptr, 0 , 0)) > 0 )//大于0就是收到消息，=0是退出,-1是出错
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

