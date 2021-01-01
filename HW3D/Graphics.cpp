#include "Graphics.h"

Graphics::Graphics(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;//此处不用于全屏模式故不用刷新帧率
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//将此缓存用作渲染目标
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;//性能最高
	sd.Flags = 0;

	D3D11CreateDeviceAndSwapChain(
		nullptr,//默认显卡
		D3D_DRIVER_TYPE_HARDWARE,//使用硬件设备
		nullptr,//软件驱动
		0,//扩展标签
		nullptr,//特性级别
		0,
		D3D11_SDK_VERSION,
		&sd,//交换链描述符Descriptor的指针
		&pSwap,//交换链
		&pDevice,//指向设备的指针
		nullptr,//输出特性级别
		&pContext	//上下文
	);
}

Graphics::~Graphics()
{
	if (pContext!=nullptr)
	{
		pContext->Release();
	}
	if (pSwap!=nullptr)
	{
		pSwap->Release();
	}
	if (pDevice!=nullptr)
	{
		pDevice->Release();
	}

}

