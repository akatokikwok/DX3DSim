#pragma once
#include "GrbWin.h"

class Window
{
private:///单例类WindowClass
	//单例，管理window 类的注册和清理
	class WindowClass
	{
	public:
		/*获取类名*/
		static const char* GetName() noexcept;
		/*获取类实例的handle*/
		static HINSTANCE GetInstance() noexcept;
	protected:
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator= (const WindowClass&) = delete;

		static constexpr const char* wndClassName = "GRB Direct113D Engine Window";
		static WindowClass wndClass;//WindowClass类 实例,打开程序时需要创建该静态类		
		HINSTANCE hInst;//WindowClass类 句柄
	};

public:
	/*构造函数*/
	Window(int width, int height, const char* argname) noexcept;
	/*析构函数*/
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
private:
	/**2个静态回调函数，为了解决winapi不能直接调用成员函数*********/
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	/*调用这个成员函数来处理消息*/
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
private:
	int width;
	int height;
	HWND hWnd;

};