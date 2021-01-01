#include "Graphics.h"

#pragma comment(lib, "d3d11.lib")//链接到库

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

	//用来保存交换链里后缓存
	ID3D11Resource* pBackBuffer = nullptr;
	//用交换链的方法访问后台缓存纹理
	pSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
	//用获取到的纹理来创建渲染目标视图
	pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pTarget);
	//具备了渲染视图之后可以释放后台缓存
	pBackBuffer->Release();
}

Graphics::~Graphics()
{
	if (pTarget!=nullptr)
	{
		pTarget->Release();
	}
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

void Graphics::EndFrame()
{
	pSwap->Present(/*同步间隔*/1u, 0u);
}

