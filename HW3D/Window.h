﻿#pragma once
#include "ChiliWin.h"
#include "ChiliException.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include <optional>
#include <memory>


class Window
{
public:
	class Exception : public ChiliException
	{
		using ChiliException::ChiliException;
	public:
		static std::string TranslateErrorCode( HRESULT hr ) noexcept;
	};
	class HrException : public Exception
	{
	public:
		HrException( int line,const char* file,HRESULT hr ) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
	};
	class NoGfxException : public Exception
	{
	public:
		using Exception::Exception;
		const char* GetType() const noexcept override;
	};
private:
	// singleton manages registration/cleanup of window class
	class WindowClass
	{
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass( const WindowClass& ) = delete;
		WindowClass& operator=( const WindowClass& ) = delete;
		static constexpr const char* wndClassName = "Chili Direct3D Engine Window";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	Window( int width,int height,const char* name );
	~Window();
	Window( const Window& ) = delete;
	Window& operator=( const Window& ) = delete;
	void SetTitle( const std::string& title );
	// 以复合形式启用/关闭鼠标
	void EnableCursor() noexcept;
	void DisableCursor() noexcept;
	// 获取窗口内光标的点状态
	bool CursorEnabled() const noexcept;
	static std::optional<int> ProcessMessages() noexcept;
	Graphics& Gfx();
private:
	// 限制光标
	void ConfineCursor() noexcept;
	// 释放光标
	void FreeCursor() noexcept;
	// 以WINAPI方式开启或关闭光标
	void HideCursor() noexcept;
	void ShowCursor() noexcept;
	// 开启/关闭 悬浮在IMGUI窗口上的光标
	void EnableImGuiMouse() noexcept;
	void DisableImGuiMouse() noexcept;

	static LRESULT CALLBACK HandleMsgSetup( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam ) noexcept;
	static LRESULT CALLBACK HandleMsgThunk( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam ) noexcept;
	LRESULT HandleMsg( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam ) noexcept;
public:
	Keyboard kbd;
	Mouse mouse;
private:
	bool cursorEnabled = true;//光标点状态，默认为启用
	std::vector<BYTE> rawBuffer;

	int width;
	int height;
	HWND hWnd;
	std::unique_ptr<Graphics> pGfx;
	std::string commandLine;
};
