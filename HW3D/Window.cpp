#include "Window.h"
#include <sstream>
#include "resource.h"

Window::WindowClass Window::WindowClass::wndClass;

const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}


Window::WindowClass::WindowClass() noexcept
	:
	hInst( GetModuleHandle(nullptr) )//调用GetModuleHandle（）获取该实例的handle并保存在成员里
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = Window::HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = Window::WindowClass::GetInstance();
	wc.hIcon = static_cast<HICON>(
		LoadImage(GetInstance()/*哪个程序需要图标*/,
			MAKEINTRESOURCE(IDI_ICON1)/*图标资源的标志*/,
			IMAGE_ICON, 32, 32, 0)
	);
	wc.hIconSm = static_cast<HICON>(
		LoadImage(GetInstance()/*哪个程序需要图标*/,
			MAKEINTRESOURCE(IDI_ICON1)/*图标资源的标志*/,
			IMAGE_ICON, 16, 16, 0)
		);
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = Window::WindowClass::GetName();
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(Window::WindowClass::wndClassName, Window::WindowClass::GetInstance());
}

Window::Window(int width, int height, const char* argname) noexcept
	:
	width(width),
	height(height)
{
	//计算窗口尺寸
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;

	if (
			/*检查窗口大小调整是否失败,失败则抛出异常*/
			//用于传递矩形、传递样式、传递是否有菜单之后的自适应窗口函数	
			AdjustWindowRect( &wr, (WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU), FALSE ) == 0
		)
	{
		throw CHWND_LAST_EXCEPT();
	};

	
	//创建窗口或者拿取窗口
	hWnd = CreateWindow(
		WindowClass::GetName(),
		argname,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wr.right-wr.left,
		wr.bottom-wr.top,
		nullptr,nullptr,WindowClass::GetInstance(),this		
	);
	if (hWnd==nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}

	//展示窗口
	ShowWindow(hWnd, SW_SHOWDEFAULT);

	//利用窗口句柄创建Graphics对象,它是1个unique指针,当窗口销毁时也会自动销毁此graphics对象
	pGfx = std::make_unique<Graphics>(hWnd);
}

void Window::SetTitle(const std::string& title)
{
	if (SetWindowText(hWnd, title.c_str()) == 0)
	{
		throw CHWND_LAST_EXCEPT();
	}
}

std::optional<int> Window::ProcessMessages() noexcept
{
	MSG msg;
	//若队列里存在消息,就移除并且派发消息,但是不阻塞
	while ( PeekMessage(&msg, nullptr,0,0, PM_REMOVE))
	{
		//手动检查队列里的消息是不是wm_quit
		if (msg.wParam == WM_QUIT)
		{
			return (int)msg.wParam;
		}

		/*一直循环下去,直至队列中再也无消息,返回空的optional库*/
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	/*一直循环下去,直至队列中再也无消息,返回空的optional库*/
	return {};
}

Graphics& Window::Gfx()
{	
	if (!pGfx)
	{
		throw CHWND_NOGFX_EXCEPT();
	}
	return *pGfx;
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

//两个静态回调函数都是使用了WINAPI调用约定为了对pWnd进行消息处理后映射为窗口指针
LRESULT WINAPI/*WINAPI*/ Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (msg == WM_NCCREATE/*非客户区就包括标题栏、窗口边框、最大、最小按钮、滚动条等;从WM_NCCREATE可以得到WM_CREATE，它对应的结构体就是上个函数的this参数*/)
	{
		//先把参数转成CREATESTRUCTW结构体,之后再利用其lpCreateParams成员将它自己映射为窗口实例的指针
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);

		// set WinAPI-managed user data to store ptr to window class
		//允许在winapi端存储数据
		SetWindowLongPtr(hWnd, GWLP_USERDATA/*根据特定的窗口设置用户数据*/, reinterpret_cast<LONG_PTR>(pWnd));
		// set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		// forward message to window class handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	// if we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//两个静态回调函数都是为了对pWnd进行消息处理后映射为窗口指针
LRESULT WINAPI/*WINAPI*/ Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	//接受Window类的指针，并重定向 窗口指针 指向Window类
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
		// 当焦点不在主窗口时,清除所有键的状态
		case WM_KILLFOCUS:
			kbd.ClearState();
			break;

			/*********** KEYBOARD MESSAGES ***********/				
		case WM_KEYDOWN://多键位同时按下的处理
		case WM_SYSKEYDOWN://系统键 按下也是同一套逻辑
			if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnabled())
			{
				kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
			}
			break;

		case WM_KEYUP:
		case WM_SYSKEYUP:
			kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
			break;
		case WM_CHAR:
			kbd.OnChar(static_cast<unsigned char>(wParam));
			break;
			/*********** END KEYBOARD MESSAGES ***********/

			/************* MOUSE MESSAGES ****************/
		case WM_MOUSEMOVE:
		{
			//lParam储存坐标
			const POINTS pt = MAKEPOINTS(lParam);
			if (pt.x >= 0 && pt.x< width && pt.y >=0 && pt.y <height )
			{
				mouse.OnMouseMove(pt.x, pt.y);

				if (!mouse.IsInWindow())
				{
					SetCapture(hWnd);
					mouse.OnMouseEnter();
				}
			}
			else
			{
				if (wParam & (MK_LBUTTON | MK_RBUTTON))
				{
					mouse.OnMouseMove(pt.x, pt.y);
				}
				else
				{
					ReleaseCapture();
					mouse.OnMouseLeave();
				}
			}
			break;
		}
		case WM_LBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnLeftPressed(pt.x, pt.y);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnRightPressed(pt.x, pt.y);
			break;
		}
		case WM_LBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnLeftReleased(pt.x, pt.y);
			// release mouse if outside of window
			// 在窗口内按下鼠标左键 即使光标出了窗口仍然可以捕捉并在标题绘制光标位置
			if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height)
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
			break;
		}
		case WM_RBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnRightReleased(pt.x, pt.y);
			// release mouse if outside of window
			// 在窗口内按下鼠标左键 即使光标出了窗口仍然可以捕捉并在标题绘制光标位置
			if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height)
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}

			break;
		}
		
		case WM_MOUSEWHEEL:/******************滑轮事件********************/
		{
			const POINTS pt = MAKEPOINTS(lParam);
#pragma region 弃用
			/*if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			{
				mouse.OnWheelUp(pt.x, pt.y);
			}
			else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
			{
				mouse.OnWheelDown(pt.x, pt.y);
			}*/
#pragma endregion		弃用
			const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
			mouse.OnWheelDelta(pt.x, pt.y, delta);
			break;
		}
		/************** END MOUSE MESSAGES **************/
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

Window::HrException::HrException(int line, const char* file, HRESULT hr) noexcept
	:
	Exception(line, file),
	hr(hr)
{

}

const char* Window::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::HrException::GetType() const noexcept
{
	return "Grb Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	//指向新分配的缓存的指针,用于承载错误代码字符串
	char* pMsgBuf = nullptr;
	//错误代码的长度
	const DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hr, 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 
		0, nullptr
	);
	//若此处也返回0,则直接返回未定义错误
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);
	return errorString;
}

HRESULT Window::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::HrException::GetErrorDescription() const noexcept
{
	return Exception::TranslateErrorCode(hr);
}

const char* Window::NoGfxException::GetType() const noexcept
{
	return "Chili Window Exception [No Graphics]";
}