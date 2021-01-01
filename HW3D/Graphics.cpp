#include "Graphics.h"

Graphics::Graphics(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;//�˴�������ȫ��ģʽ�ʲ���ˢ��֡��
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//���˻���������ȾĿ��
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;//�������
	sd.Flags = 0;

	D3D11CreateDeviceAndSwapChain(
		nullptr,//Ĭ���Կ�
		D3D_DRIVER_TYPE_HARDWARE,//ʹ��Ӳ���豸
		nullptr,//�������
		0,//��չ��ǩ
		nullptr,//���Լ���
		0,
		D3D11_SDK_VERSION,
		&sd,//������������Descriptor��ָ��
		&pSwap,//������
		&pDevice,//ָ���豸��ָ��
		nullptr,//������Լ���
		&pContext	//������
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

