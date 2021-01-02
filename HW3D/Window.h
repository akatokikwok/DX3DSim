﻿#pragma once
#include "GrbWin.h"
#include "GrbException.h"
#include "Keyboard.h"
#include "Mouse.h"

#include <optional>/*ver1.0.10*/
#include "Graphics.h"//ver1.0.13
#include <memory>//ver1.0.13

class Window
{
public:
	/* 继承异常处理类GrbException*/
	class Exception : public GrbException
	{
		using GrbException::GrbException;
	public:
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
	};

	/* HrException异常类*/
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept override;
		/*拿取错误消息*/
		HRESULT GetErrorCode() const noexcept;
		/*报错描述*/
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
	};

	/* 图形显示异常类*/
	class NoGfxException : public Exception
	{
	public:
		using Exception::Exception;
		const char* GetType() const noexcept override;
	};

	/// //////////////////////////////////////////////////////////////////////////
private:
	///单例类WindowClass
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

	//测试方法,把消息拍到窗口标题上
	void SetTitle(const std::string& title); 
	/*把消息处理的应用逻辑，置入此方法,该函数负责处理所有窗口的消息,所以要设置成static型*/
	static std::optional<int> ProcessMessages() noexcept;
	/* 访问Graphics的方法*/
	Graphics& Gfx();

private:
	/**消息处理程序函数;2个静态回调函数，为了解决winapi不能直接调用成员函数*********/
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	/*调用这个成员函数来处理消息*/
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

public:
	Keyboard kbd;//键盘类实例
	Mouse mouse;//鼠标类实例

private:
	int width;
	int height;
	HWND hWnd;

	std::unique_ptr<Graphics> pGfx;//Graphics类实例
};
//工具宏
#define CHWND_EXCEPT( hr ) Window::HrException( __LINE__,__FILE__,(hr) )
#define CHWND_LAST_EXCEPT() Window::HrException( __LINE__,__FILE__,GetLastError() )
#define CHWND_NOGFX_EXCEPT() Window::NoGfxException( __LINE__,__FILE__ ) 