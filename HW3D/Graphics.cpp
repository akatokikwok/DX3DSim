#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include <wrl.h>
#include <wrl\client.h>
#include <d3dcompiler.h>

//为了使用智能指针
namespace wrl = Microsoft::WRL;

#pragma comment(lib, "d3d11.lib")//链接到库
#pragma comment(lib, "D3DCompiler.lib")//运行时编译着色器

// graphics exception checking/throwing macros (some with dxgi infos)
#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
//不返回hr型的消息宏
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Graphics::InfoException( __LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

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

	/* 设备层开启调试*/
	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// for checking results of d3d functions
	HRESULT hr;

	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,//默认显卡 
		D3D_DRIVER_TYPE_HARDWARE,//使用硬件设备 
		nullptr,//软件驱动 
		swapCreateFlags,//扩展标签 
		nullptr,//特性级别 
		0,
		D3D11_SDK_VERSION,
		&sd,//交换链描述符Descriptor的指针
		&pSwap,//交换链
		&pDevice,//指向设备的指针
		nullptr,//输出特性级别
		&pContext	//上下文
	));

	////用来保存交换链里后缓存
	//ID3D11Resource* pBackBuffer = nullptr;
	////用交换链的方法访问后台缓存纹理
	//GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer)));
	////用获取到的纹理来创建渲染目标视图
	//GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pTarget));
	////具备了渲染视图之后可以释放后台缓存
	//pBackBuffer->Release();

	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO( pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget));
}


void Graphics::EndFrame()
{
	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif

	if (FAILED(hr = pSwap->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			throw GFX_EXCEPT(hr);
		}

		//pSwap->Present(/*同步间隔*/1u, 0u);
	}
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	//给一个颜色
	const float color[] = { red, green, blue, 1.0f };
	//在上下文以指定的颜色来填充清除渲染视图
	pContext->ClearRenderTargetView(pTarget.Get(), color);
}

void Graphics::DrawTestTriangle(float angle)
{
	namespace wrl = Microsoft::WRL;
	HRESULT hr;

	/* 创建1个顶点结构体型*/
	struct Vertex
	{
		struct  
		{
			float x;
			float y;
		} pos;
		
		struct 
		{
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		} color;		
	};

	/* 创建1个顶点数组,此处为顺时针*/
	/*const*/ Vertex vertices[] =
	{
		{ 0.0f, 0.5f,	255,0,0,0},
		{ 0.5f, -0.5f,  0,255,0,0},
		{ -0.5f, -0.5f,	0,0,255,0},
		{-0.3, 0.3,0,255,0,0},
		{0.3,0.3,0,0,255,0},
		{0,-0.8,255,0,0,0},

	};
	vertices[0].color.g = 255;

	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;//声明1个顶点缓存

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;//缓存类型
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(vertices);//顶点数组的尺寸
	bd.StructureByteStride = sizeof(Vertex);//顶点型的大小
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;//顶点数组指针

	/* 在设备上创建出顶点缓冲*/
	GFX_THROW_INFO(pDevice->CreateBuffer( /*Buffer描述*/&bd, /*SubResourceData*/&sd, /*ppBuffer*/&pVertexBuffer));
	/* 顶点缓存绑定到管线上*/
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	pContext->IASetVertexBuffers(/*起始槽位*/0u, /*缓存数*/1u ,  pVertexBuffer.GetAddressOf(),  /*单顶点型数据大小*/&stride, /*需求中的数据处于顶点里第几位*/&offset);

	/* 创建索引缓存以便以特定顺序节省顶点并调用DrawIndexed()*///索引缓冲一般为16位，可用unsigned short
	const unsigned short indices[] =
	{
		0,1,2,
		0,2,3,
		0,4,1,
		2,1,5,
	};
	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indices);
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;
	GFX_THROW_INFO(pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));
	pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	/* 创建常量缓存来使用变换矩阵*/
	struct ConstantBuffer
	{
		struct
		{
			float element[4][4];
		} transformation;
	};
	const ConstantBuffer cb =
	{
		//绕Z轴转,此处考虑到800*600的屏幕图形会被拉伸,所以乘3/4来保持不拉伸,在第一列体现
		{
			0.75f*(std::cos(angle)),	std::sin(angle),	0,	0,
			0.75f*(-std::sin(angle)),	std::cos(angle),	0,	0,
			0,					0,					1,	0,
			0,					0,					0,	1,		
		}
	};
	wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;//每帧更新一次,需要一个动态的
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//允许CPU每帧传顶点数据过来
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(cb);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &cb;
	GFX_THROW_INFO(pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer));
	//管线上绑定常量缓存(在VS阶段)
	pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

	/// 此后过程是利用Blob先创建像素shader再创建顶点shader,最后再顶点输入布局*///////////////////////////////////////////////////////////////////////////

	/* 利用Blob字节码创建像素shader*/
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	wrl::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO( D3DReadFileToBlob( L"PixelShader.cso" , &pBlob));
	GFX_THROW_INFO( pDevice->CreatePixelShader(pBlob->GetBufferPointer(),pBlob->GetBufferSize(),nullptr, &pPixelShader));
	/* 管线上绑定像素shader*/
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	/* 创建顶点shader*/
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	/* 创建顶点shader*/
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
	GFX_THROW_INFO(pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));
	/* 管线上绑定顶点shader*/
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);


	/* 绑定顶点输入布局*/
	//但是在像素shader认为,三维和二维都只是向量,除了位置外可能还有颜色\法向量
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		//{ /*该值必须要与顶点hlsl里的第二参数保持一致*/"Position", /*索引*/0, DXGI_FORMAT_R32G32_FLOAT,/*Slot*/0,/*相比于原组的偏移量*/0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "Position",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Color",0,DXGI_FORMAT_R8G8B8A8_UNORM/*UNORM可以让数归一化*/,0,8u,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	GFX_THROW_INFO(
		pDevice->CreateInputLayout(/*输入布局数组*/ied, /*数组里元素数量*/(UINT)std::size(ied),
			/*着色器字节码*/pBlob->GetBufferPointer(), /*字节码长度*/pBlob->GetBufferSize(), &pInputLayout)
	);
	pContext->IASetInputLayout(pInputLayout.Get());

	/* 管线上绑定渲染目标,不然像素shader不知道渲染输出到哪个目的地*/
	// GetAddressof的好处是会获取到智能指针的指针,而不用释放对象;getaddressof就是单纯取指针地址。
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr/*此处未用到深度模板*/); 

	/* 管线上绑定图元装备形式,三角形列表,3个顶点一组*/
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/* 顺带设置一下视口*/
	D3D11_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);

	GFX_THROW_INFO_ONLY(pContext->DrawIndexed(/*顶点数*/ (UINT)std::size(indices), /*起始顶点位置*/0u,0u));//3个顶点,从0号开始

}

/// 各异常类实现 //////////////////////////////////////////////////////////////////////////
Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file),
	hr(hr)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	if (!info.empty())
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
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
	char buf[512];
	DXGetErrorDescription(hr, buf, sizeof(buf));
	return buf;
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Grb Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}


Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}


const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "Grb Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}