#pragma once
#include "GrbWin.h"

class Window
{
private:///������WindowClass
	//����������window ���ע�������
	class WindowClass
	{
	public:
		/*��ȡ����*/
		static const char* GetName() noexcept;
		/*��ȡ��ʵ����handle*/
		static HINSTANCE GetInstance() noexcept;
	protected:
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator= (const WindowClass&) = delete;

		static constexpr const char* wndClassName = "GRB Direct113D Engine Window";
		static WindowClass wndClass;//WindowClass�� ʵ��,�򿪳���ʱ��Ҫ�����þ�̬��		
		HINSTANCE hInst;//WindowClass�� ���
	};

public:
	/*���캯��*/
	Window(int width, int height, const char* argname) noexcept;
	/*��������*/
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
private:
	/**2����̬�ص�������Ϊ�˽��winapi����ֱ�ӵ��ó�Ա����*********/
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	/*���������Ա������������Ϣ*/
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
private:
	int width;
	int height;
	HWND hWnd;

};