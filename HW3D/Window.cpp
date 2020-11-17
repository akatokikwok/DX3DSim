#include "Window.h"
#include <sstream>

Window::WindowClass Window::WindowClass::wndClass;

const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

//Window::WindowClass::WindowClass() noexcept

Window::WindowClass::WindowClass() noexcept
	:
	hInst( GetModuleHandle(nullptr) )//����GetModuleHandle������ȡ��ʵ����handle�������ڳ�Ա��
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = Window::HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = Window::WindowClass::GetInstance();
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = Window::WindowClass::GetName();
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(Window::WindowClass::wndClassName, Window::WindowClass::GetInstance());
}

Window::Window(int width, int height, const char* argname) noexcept
{
	//���㴰�ڳߴ�
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	//���ڴ��ݾ��Ρ�������ʽ�������Ƿ��в˵�֮�������Ӧ���ں���
	AdjustWindowRect(&wr, 
		(WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU), 
		FALSE);
	//�������ڻ�����ȡ����
	hWnd = CreateWindow(
		WindowClass::GetName(),
		argname,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wr.right-wr.left,
		wr.bottom-wr.top,
		nullptr,nullptr,WindowClass::GetInstance(),
		this
	);
	//չʾ����
	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

//������̬�ص���������ʹ����WINAPI����Լ��Ϊ�˶�pWnd������Ϣ������ӳ��Ϊ����ָ��
LRESULT WINAPI/*WINAPI*/ Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (msg == WM_NCCREATE/*�ǿͻ����Ͱ��������������ڱ߿������С��ť����������;��WM_NCCREATE���Եõ�WM_CREATE������Ӧ�Ľṹ������ϸ�������this����*/)
	{
		//�ȰѲ���ת��CREATESTRUCTW�ṹ��,֮����������lpCreateParams��Ա�����Լ�ӳ��Ϊ����ʵ����ָ��
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);

		// set WinAPI-managed user data to store ptr to window class
		//������winapi�˴洢����
		SetWindowLongPtr(hWnd, GWLP_USERDATA/*�����ض��Ĵ��������û�����*/, reinterpret_cast<LONG_PTR>(pWnd));
		// set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		// forward message to window class handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	// if we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//������̬�ص���������Ϊ�˶�pWnd������Ϣ������ӳ��Ϊ����ָ��
LRESULT WINAPI/*WINAPI*/ Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	//����Window���ָ�룬���ض��� ����ָ�� ָ��Window��
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
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

// Window Exception Stuff
Window::Exception::Exception(int line, const char* file, HRESULT hr) noexcept
	:
	GrbException(line, file),
	hr(hr)
{}

const char* Window::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::Exception::GetType() const noexcept
{
	return "Grb Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	//ָ���·���Ļ����ָ��,���ڳ��ش�������ַ���
	char* pMsgBuf = nullptr;
	//�������ĳ���
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hr, 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 
		0, nullptr
	);
	//���˴�Ҳ����0,��ֱ�ӷ���δ�������
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);
	return errorString;
}

HRESULT Window::Exception::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}