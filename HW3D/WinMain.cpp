#include <Windows.h>
#include <winuser.h>


//�Զ���1����Ϣ�������
LRESULT CALLBACK cusWndProc(HWND hWnd,/*������Ϣ�Ĵ��ھ��*/ UINT msg,/*��ϢID��*/ WPARAM wParam, LPARAM lParam)
{
	switch (msg)//������ϢID������
	{
	case WM_CLOSE:
		PostQuitMessage(69);
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);//���ñ�׼�����Ĭ�Ϻ���DefWindowsProc;
}

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	
	//ע�ᴰ����
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = ::cusWndProc; //����windowsProcedureָ��
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = nullptr;
	wc.hIconSm = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	const auto pClassName = "hw3dbutts";
	wc.lpszClassName = pClassName;
	RegisterClassEx(&wc);
	//��������ʵ��
	HWND hWnd = CreateWindowEx(
		0,pClassName,
		"babababa,this is a chuangkou",
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		200,200,
		640,480,
		nullptr,nullptr, hInstance,nullptr
	);
	ShowWindow(hWnd, SW_SHOW);
	//while (true)
	//	;
	
	//��Ϣ�ṹ�幹��
	MSG msg;
	BOOL gResult;
	while ( (gResult=GetMessage( &msg, nullptr, 0 , 0)) > 0 )//����0�����յ���Ϣ��=0���˳�,-1�ǳ���
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (gResult==-1)
	{
		return -1;
	}
	else if (gResult == 0)
	{
		return msg.wParam;//msg.wParam��PostQuitMessage�˳�ʱ����ʾ��code֪ͨ
	}

	/*return 0;*/
}

