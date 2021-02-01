﻿#include "Window.h"
#include <sstream>
#include "resource.h"
#include "WindowsThrowMacros.h"
#include "imgui/imgui_impl_win32.h"


// Window Class Stuff
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	:
	hInst( GetModuleHandle( nullptr ) )
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof( wc );
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(LoadImage( 
		GetInstance(),MAKEINTRESOURCE( IDI_ICON1 ),
		IMAGE_ICON,32,32,0
	));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage(
		GetInstance(),MAKEINTRESOURCE( IDI_ICON1 ),
		IMAGE_ICON,16,16,0
	));
	RegisterClassEx( &wc );
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass( wndClassName,GetInstance() );
}

const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}


// Window Stuff
Window::Window( int width,int height,const char* name )
	:
	width( width ),
	height( height )
{
	// calculate window size based on desired client region size
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	if( AdjustWindowRect( &wr,WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,FALSE ) == 0 )
	{
		throw CHWND_LAST_EXCEPT();
	}
	// create window & get hWnd
	hWnd = CreateWindow(
		WindowClass::GetName(),name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,CW_USEDEFAULT,wr.right - wr.left,wr.bottom - wr.top,
		nullptr,nullptr,WindowClass::GetInstance(),this
	);
	// check for error
	if( hWnd == nullptr )
	{
		throw CHWND_LAST_EXCEPT();
	}
	// newly created windows start off as hidden
	ShowWindow( hWnd,SW_SHOWDEFAULT );
	// Init ImGui Win32 Impl
	ImGui_ImplWin32_Init( hWnd );
	// create graphics object
	pGfx = std::make_unique<Graphics>(hWnd, width, height);

	// 注册鼠标原生输入的持有设备
	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01; // mouse page
	rid.usUsage = 0x02; // mouse usage
	rid.dwFlags = 0;
	rid.hwndTarget = nullptr;	
	if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)// 检查raw data数据来自哪个设备
	{
		throw CHWND_LAST_EXCEPT();
	}
}

Window::~Window()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow( hWnd );
}

void Window::SetTitle( const std::string& title )
{
	if( SetWindowText( hWnd,title.c_str() ) == 0 )
	{
		throw CHWND_LAST_EXCEPT();
	}
}

void Window::EnableCursor() noexcept
{
	cursorEnabled = true;
	ShowCursor();
	EnableImGuiMouse();
	FreeCursor();
}

void Window::DisableCursor() noexcept
{
	cursorEnabled = false;
	HideCursor();
	DisableImGuiMouse();
	ConfineCursor();
}

bool Window::CursorEnabled() const noexcept
{
	return cursorEnabled;
}

std::optional<int> Window::ProcessMessages() noexcept
{
	MSG msg;
	// while queue has messages, remove and dispatch them (but do not block on empty queue)
	while( PeekMessage( &msg,nullptr,0,0,PM_REMOVE ) )
	{
		// check for quit because peekmessage does not signal this via return val
		if( msg.message == WM_QUIT )
		{
			// return optional wrapping int (arg to PostQuitMessage is in wparam) signals quit
			return (int)msg.wParam;
		}

		// TranslateMessage will post auxilliary WM_CHAR messages from key msgs
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	// return empty optional when not quitting app
	return {};
}

Graphics& Window::Gfx()
{
	if( !pGfx )
	{
		throw CHWND_NOGFX_EXCEPT();
	}
	return *pGfx;
}

void Window::ConfineCursor() noexcept
{
	// 首先获得本客户端窗口存在一个矩形里
	RECT rect;
	GetClientRect(hWnd, &rect);
	// 从窗口空间映射到控件空间
	MapWindowPoints(hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
	// 之后利用WINAPI方法就可以把光标限制在一个矩形里
	ClipCursor(&rect);
}

void Window::FreeCursor() noexcept
{
	ClipCursor(nullptr);
}

void Window::HideCursor() noexcept
{
	while (::ShowCursor(FALSE) >= 0);//这里使用的是WINAPI函数ShowCursor
}

void Window::ShowCursor() noexcept
{
	while (::ShowCursor(TRUE) < 0);
}

void Window::EnableImGuiMouse() noexcept
{
	ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
}

void Window::DisableImGuiMouse() noexcept
{
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
}

LRESULT CALLBACK Window::HandleMsgSetup( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam ) noexcept
{
	// use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
	if( msg == WM_NCCREATE )
	{
		// extract ptr to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// set WinAPI-managed user data to store ptr to window instance
		SetWindowLongPtr( hWnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(pWnd) );
		// set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr( hWnd,GWLP_WNDPROC,reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk) );
		// forward message to window instance handler
		return pWnd->HandleMsg( hWnd,msg,wParam,lParam );
	}
	// if we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProc( hWnd,msg,wParam,lParam );
}

LRESULT CALLBACK Window::HandleMsgThunk( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam ) noexcept
{
	// retrieve ptr to window instance
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr( hWnd,GWLP_USERDATA ));
	// forward message to window instance handler
	return pWnd->HandleMsg( hWnd,msg,wParam,lParam );
}

LRESULT Window::HandleMsg( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam ) noexcept
{
	if( ImGui_ImplWin32_WndProcHandler( hWnd,msg,wParam,lParam ) )
	{
		return true;
	}
	const auto& imio = ImGui::GetIO();

	switch( msg )
	{
	// we don't want the DefProc to handle this message because
	// we want our destructor to destroy the window, so return 0 instead of break
	case WM_CLOSE:
		PostQuitMessage( 0 );
		return 0;
	// clear keystate when window loses focus to prevent input getting "stuck"
	case WM_KILLFOCUS:
		kbd.ClearState();
		break;

	case WM_ACTIVATE:
		OutputDebugString("activeate\n");
		// confine/free cursor on window to foreground/background if cursor disabled
		// 无论何种操作都应该使光标被限制在窗口里

		// 此处当鼠标被禁用时候,同时也要求光标被限制在窗口里
		if (!cursorEnabled)
		{
			if (wParam & WA_ACTIVE/*此宏表示以非点击方式激活窗口*/ || wParam & WA_CLICKACTIVE /*此宏表示以点击方式激活窗口*/)
			{
				OutputDebugString("activeate => confine\n");
				ConfineCursor();
				HideCursor();
			}
			else
			{
				OutputDebugString("activeate => free\n");
				FreeCursor();
			}
		}
		break;

	/*********** KEYBOARD MESSAGES ***********/
	case WM_KEYDOWN:
	// syskey commands need to be handled to track ALT key (VK_MENU) and F10
	case WM_SYSKEYDOWN:
		// stifle this keyboard message if imgui wants to capture
		if( imio.WantCaptureKeyboard )
		{
			break;
		}
		if( !(lParam & 0x40000000) || kbd.AutorepeatIsEnabled() ) // filter autorepeat
		{
			kbd.OnKeyPressed( static_cast<unsigned char>(wParam) );
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		// stifle this keyboard message if imgui wants to capture
		if( imio.WantCaptureKeyboard )
		{
			break;
		}
		kbd.OnKeyReleased( static_cast<unsigned char>(wParam) );
		break;
	case WM_CHAR:
		// stifle this keyboard message if imgui wants to capture
		if( imio.WantCaptureKeyboard )
		{
			break;
		}
		kbd.OnChar( static_cast<unsigned char>(wParam) );
		break;
	/*********** END KEYBOARD MESSAGES ***********/

	/************* MOUSE MESSAGES ****************/
	case WM_MOUSEMOVE:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		// cursorless exclusive gets first dibs
		if (!cursorEnabled)
		{
			if (!mouse.IsInWindow())
			{
				SetCapture(hWnd);
				mouse.OnMouseEnter();
				HideCursor();
			}
			break;
		}

		// stifle this mouse message if imgui wants to capture
		if( imio.WantCaptureMouse )
		{
			break;
		}
		//const POINTS pt = MAKEPOINTS( lParam );
		// in client region -> log move, and log enter + capture mouse (if not previously in window)
		if( pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height )
		{
			mouse.OnMouseMove( pt.x,pt.y );
			if( !mouse.IsInWindow() )
			{
				SetCapture( hWnd );
				mouse.OnMouseEnter();
			}
		}
		// not in client -> log move / maintain capture if button down
		else
		{
			if( wParam & (MK_LBUTTON | MK_RBUTTON) )
			{
				mouse.OnMouseMove( pt.x,pt.y );
			}
			// button up -> release capture / log event for leaving
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
		SetForegroundWindow( hWnd );

		// 若接收到鼠标左键点击且光标被禁用,则执行隐藏并限制光标在窗口内;假设用户点击窗口标题栏是想拖动窗口;
		if (!cursorEnabled)
		{
			OutputDebugString("lclick => recapture\n");
			ConfineCursor();
			HideCursor();
		}
		// stifle this mouse message if imgui wants to capture
		if( imio.WantCaptureMouse )
		{
			break;
		}
		const POINTS pt = MAKEPOINTS( lParam );
		mouse.OnLeftPressed( pt.x,pt.y );
		break;
	}
	case WM_RBUTTONDOWN:
	{
		// stifle this mouse message if imgui wants to capture
		if( imio.WantCaptureMouse )
		{
			break;
		}
		const POINTS pt = MAKEPOINTS( lParam );
		mouse.OnRightPressed( pt.x,pt.y );
		break;
	}
	case WM_LBUTTONUP:
	{
		// stifle this mouse message if imgui wants to capture
		if( imio.WantCaptureMouse )
		{
			break;
		}
		const POINTS pt = MAKEPOINTS( lParam );
		mouse.OnLeftReleased( pt.x,pt.y );
		// release mouse if outside of window
		if( pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height )
		{
			ReleaseCapture();
			mouse.OnMouseLeave();
		}
		break;
	}
	case WM_RBUTTONUP:
	{
		// stifle this mouse message if imgui wants to capture
		if( imio.WantCaptureMouse )
		{
			break;
		}
		const POINTS pt = MAKEPOINTS( lParam );
		mouse.OnRightReleased( pt.x,pt.y );
		// release mouse if outside of window
		if( pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height )
		{
			ReleaseCapture();
			mouse.OnMouseLeave();
		}
		break;
	}
	case WM_MOUSEWHEEL:
	{
		// stifle this mouse message if imgui wants to capture
		if( imio.WantCaptureMouse )
		{
			break;
		}
		const POINTS pt = MAKEPOINTS( lParam );
		const int delta = GET_WHEEL_DELTA_WPARAM( wParam );
		mouse.OnWheelDelta( pt.x,pt.y,delta );
		break;
	}
	/************** END MOUSE MESSAGES **************/

	/************** RAW MOUSE MESSAGES **************/
	case WM_INPUT:
	{
		if (!mouse.RawEnabled())
		{
			break;
		}

		// 输入数据
		UINT size;
		// first get the size of the input data;
		// 当有设备输入的时候就会发送这些消息,使用GetrawInputData()获得收到的数据
		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam),//lParam是那个raw Input的句柄
			RID_INPUT,
			nullptr,
			&size,
			sizeof(RAWINPUTHEADER)) == -1)
		{
			// 若数据出错就直接抛异常
			break;
		}
		rawBuffer.resize(size);
		
		// 重新再执行一次GetRawInputData(),将数据写入
		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			rawBuffer.data(),
			&size,
			sizeof(RAWINPUTHEADER)) != size)
		{
			// 如果这个第二次检测查出来的size和第一次的不一致,也将其视为出错,并break掉此次输入
			break;
		}
		// process the raw input data
		auto& ri = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
		if (ri.header.dwType == RIM_TYPEMOUSE /*首先只需要鼠标的数据,其他数据将被忽视掉*/&&
			(ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0)/*只处理光标移动的情况*/)
		{
			mouse.OnRawDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
		}
		break;
	}
	/************** END RAW MOUSE MESSAGES **************/


	}

	return DefWindowProc( hWnd,msg,wParam,lParam );
}


// Window Exception Stuff
std::string Window::Exception::TranslateErrorCode( HRESULT hr ) noexcept
{
	char* pMsgBuf = nullptr;
	// windows will allocate memory for err string and make our pointer point to it
	const DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,hr,MAKELANGID( LANG_NEUTRAL,SUBLANG_DEFAULT ),
		reinterpret_cast<LPSTR>(&pMsgBuf),0,nullptr
	);
	// 0 string length returned indicates a failure
	if( nMsgLen == 0 )
	{
		return "Unidentified error code";
	}
	// copy error string from windows-allocated buffer to std::string
	std::string errorString = pMsgBuf;
	// free windows buffer
	LocalFree( pMsgBuf );
	return errorString;
}


Window::HrException::HrException( int line,const char* file,HRESULT hr ) noexcept
	:
	Exception( line,file ),
	hr( hr )
{}

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
	return "Chili Window Exception";
}

HRESULT Window::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::HrException::GetErrorDescription() const noexcept
{
	return Exception::TranslateErrorCode( hr );
}


const char* Window::NoGfxException::GetType() const noexcept
{
	return "Chili Window Exception [No Graphics]";
}