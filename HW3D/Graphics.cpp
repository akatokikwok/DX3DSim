#include "Graphics.h"
#include "dxerr.h"
#include <sstream>

//链接到库
#pragma comment(lib, "d3d11.lib")

//定义几个宏,让负责异常抛出的代码更加简洁
#define GFX_THROW_FAILED(hrcall) if(FAILED( hr = (hrcall))) throw Graphics::HrException( __LINE__, __FILE__, hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )

/// //////////////////////////////////////////////////////////////////////////
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

	//用于检查D3D方法的结果
	HRESULT hr;

	GFX_THROW_FAILED(D3D11CreateDeviceAndSwapChain(
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
	));

	//用来保存交换链里后缓存
	ID3D11Resource* pBackBuffer = nullptr;
	//用交换链的方法访问后台缓存纹理
	GFX_THROW_FAILED(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer)));
	//用获取到的纹理来创建渲染目标视图
	GFX_THROW_FAILED(pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pTarget));
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
	HRESULT hr;
	if (FAILED( hr = pSwap->Present(/*同步间隔*/1u, 0u) ))
	{
		//hr 可能会返回已移除设备的错误代码，这是一个特殊的windows内置情况;大概率是超频显卡驱动崩溃
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			GFX_THROW_FAILED(hr);
		}
	}
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept 
{	
	//给一个颜色
	const float color[] = { red, green, blue, 1.0f };
	//在上下文以指定的颜色来填充清除渲染视图
	pContext->ClearRenderTargetView(pTarget, color);
}


/// <summary>
/// Graphics类中各种异常类的异常方法实现
/// </summary>
/// <param name="line"></param>
/// <param name="file"></param>
/// <param name="hr"></param>
/// <returns></returns>
Graphics::HrException::HrException(int line, const char* file, HRESULT hr) noexcept
	:
	Exception(line, file),
	hr(hr)
{

}

const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
	return "Grb Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString(hr);
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	//把错误放进512位的数组即可
	char buf[512];
	DXGetErrorDescription(hr, buf, sizeof(buf));
	return buf;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Grb Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}
