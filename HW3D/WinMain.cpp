#include <Windows.h>
#include <winuser.h>
#include "WindowsMessageMap.h"
#include <iosfwd>
#include <sstream>

#include "Window.h"

//�Զ���1����Ϣ�������
LRESULT CALLBACK cusWndProc(HWND hWnd,/*������Ϣ�Ĵ��ھ��*/ UINT msg,/*��ϢID��*/ WPARAM wParam, LPARAM lParam)
{
	//������̬������Ϣmap
	static WindowsMessageMap mm;
	//�����յ���ÿһ����Ϣ,�����ַ�����ʽ��ӡ����
	OutputDebugString(
		mm(msg, lParam, wParam).c_str()//�����������WindowsMessageMap������е����أ�Ч�����õ���ʽ���ַ���
	);

	switch (msg)//������ϢID������
	{
	case WM_CLOSE:
		PostQuitMessage(69);
		break;
	case WM_KEYDOWN://�Դ�Сд������
		if ( wParam=='F' )
		{
			SetWindowText( hWnd, "i have changed the title of this HWND");
		}
		break;
	case WM_KEYUP:
		if (wParam == 'F')
		{
			SetWindowText(hWnd, "the title of this HWND has recoverd");
		}
		break;
	case WM_CHAR:///��Сд����,��������һ�����ֵ�ʱ�����õ��ļ��ͻ����WM_CHAR
		{
			static std::string title;
			title.push_back((char)wParam);
			SetWindowText(hWnd, title.c_str());/*��ʱ���ڱ�����ʾ������������Keydowm WParam����*/
		}
		break;
	case WM_LBUTTONDOWN:
		{
			POINTS pt = MAKEPOINTS(lParam);//��һ��Point�ṹ��
			std::ostringstream oss;//������˼�ǰ��������ṹ�����Ϊ�ı�
			oss << "(" << pt.x << "," << pt.y << ")";
			SetWindowText(hWnd, oss.str().c_str());
		}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);//���ñ�׼�����Ĭ�Ϻ���DefWindowsProc;
}


int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance, 
	_In_ LPSTR lpCmdLine, 
	_In_ int nCmdShow
)
{
	
#pragma region ����ver1.0.5
	////ע�ᴰ����
	//WNDCLASSEX wc = { 0 };
	//wc.cbSize = sizeof(wc);
	//wc.style = CS_OWNDC;
	//wc.lpfnWndProc = ::cusWndProc; //����windowsProcedureָ��
	//wc.cbClsExtra = 0;
	//wc.cbWndExtra = 0;
	//wc.hInstance = hInstance;
	//wc.hIcon = nullptr;
	//wc.hIconSm = nullptr;
	//wc.hCursor = nullptr;
	//wc.hbrBackground = nullptr;
	//wc.lpszMenuName = nullptr;
	//const auto pClassName = "hw3d_className";
	//wc.lpszClassName = pClassName;
	//RegisterClassEx(&wc);
	////��������ʵ��
	//HWND hWnd = CreateWindowEx(
	//	0,pClassName,
	//	"babababa,this is a chuangkou",
	//	WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
	//	200,200,
	//	640,480,
	//	nullptr,nullptr, hInstance,nullptr
	//);
	//ShowWindow(hWnd, SW_SHOW);
	////while (true)
	////	;
#pragma endregion ����ver1.0.5

	try
	{
		/// ����Window�����Ĵ��ι�����Դ����1����Ķ������ʵ���������Թ���
		Window wnd(800, 300, "chuang kou wenben");
		//Window wnd2(300, 800, "chuang kou 2 wen ben");

		///��Ϣ�ṹ�幹��
		MSG msg;
		BOOL gResult;
		while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)//����0�����յ���Ϣ��=0���˳�,-1�ǳ���
		{
			TranslateMessage(&msg);//TranslateMessage�ʵ������¿��԰�wm_keydownͬʱת��wm_char
			DispatchMessage(&msg);

			if (wnd.kbd.KeyIsPressed(VK_MENU) )
			{
				MessageBox(nullptr, "Something happened", "Space Key Was Pressed", MB_OK|MB_ICONEXCLAMATION);
			}
		}
		if (gResult == -1)
		{
			return -1;
		}
		else if (gResult == 0)
		{
			return msg.wParam;//msg.wParam��PostQuitMessage�˳�ʱ����ʾ��code֪ͨ
		}
	}
	catch (const GrbException& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;

}

