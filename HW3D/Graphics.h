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


protected:
private:
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwap = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
};