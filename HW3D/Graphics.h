#pragma once

#include "GrbWin.h"
#include <d3d11.h>

class Graphics
{
public:
	Graphics(HWND hWnd);
	/* 禁用拷贝和复制带参构造*/
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	/*每帧结束瞬间做的事*/
	void EndFrame();
	/*清除渲染视图的进一步封装方法*/
	void ClearBuffer(float red, float green, float blue) noexcept
	{
		//给一个颜色
		const float color[] = {red, green, blue, 1.0f};
		//在上下文以指定的颜色来填充清除渲染视图
		pContext->ClearRenderTargetView(pTarget, color);
	};

protected:
private:
	ID3D11Device* pDevice = nullptr;//设备
	IDXGISwapChain* pSwap = nullptr;//交换链
	ID3D11DeviceContext* pContext = nullptr;//上下文
	ID3D11RenderTargetView* pTarget = nullptr;//渲染视图
};