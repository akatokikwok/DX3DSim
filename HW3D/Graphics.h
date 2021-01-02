﻿#pragma once
#include "GrbException.h"
#include "GrbWin.h"
#include <d3d11.h>
#include <vector>
#include "DxgiInfoManager.h"
#include <wrl.h>
#include <wrl\internal.h>

class Graphics
{
public:
	/* 异常类*/
	class Exception : public GrbException
	{
		using GrbException::GrbException;
	};
	/* HR异常类*/
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	/* 信息通知 异常类*/
	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};
	/* 设备被意外移除异常类*/
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};
/// //////////////////////////////////////////////////////////////////////////
public:
	Graphics(HWND hWnd);
	/* 禁用拷贝和复制带参构造*/
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics()=default;
	
	//////////////////////////////////////////////////////////////////////////

	/*每帧结束瞬间做的事*/
	void EndFrame();
	/*清除渲染视图的进一步封装方法*/
	void ClearBuffer(float red, float green, float blue) noexcept;
	/* 与绘制三角形有关的代码*/
	void DrawTestTriangle(float angle);

private:
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif

	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;//设备,用于分配内存\创建资源
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;//交换链
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;//上下文,用于发送渲染命令
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;//渲染视图
};

