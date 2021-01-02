﻿#pragma once

#include "GrbWin.h"
#include <d3d11.h>
#include "GrbException.h"

#include <d3d11.h>
#include <vector>
#include "DxgiInfoManager.h"

class Graphics
{
public:
	//添加1个异常类
	class Exception :public GrbException
	{
		using GrbException::GrbException;
	};
	//添加1个HR异常类
	class HrException :public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs={} ) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		//HrException类字段 hr
		HRESULT hr;
		//消息
		std::string info;
	};
	//设备删除异常类
	class DeviceRemovedException :public HrException
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
	~Graphics();
	/*每帧结束瞬间做的事*/
	void EndFrame();
	/*清除渲染视图的进一步封装方法*/
	void ClearBuffer(float red, float green, float blue) noexcept;

private:
	//仅在Debug模式下包含1个InfoManager
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif

	ID3D11Device* pDevice = nullptr;//设备
	IDXGISwapChain* pSwap = nullptr;//交换链
	ID3D11DeviceContext* pContext = nullptr;//上下文
	ID3D11RenderTargetView* pTarget = nullptr;//渲染视图
};