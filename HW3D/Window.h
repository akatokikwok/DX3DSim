#pragma once
#include "GrbWin.h"
#include "GrbException.h"
#include "Keyboard.h"

class Window
{
public:///�̳��쳣������GrbException
	class Exception : public GrbException
	{
	public:
		Exception(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		/*��ȡ������Ϣ*/
		HRESULT GetErrorCode() const noexcept;
		/**/
		std::string GetErrorString() const noexcept;
	private:
		HRESULT hr;
	};

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
	/**��Ϣ���������;2����̬�ص�������Ϊ�˽��winapi����ֱ�ӵ��ó�Ա����*********/
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	/*���������Ա������������Ϣ*/
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

public:
	Keyboard kbd;//������ʵ��

private:
	int width;
	int height;
	HWND hWnd;
};
//���ߺ�
#define CHWND_EXCEPT( hr ) Window::Exception( __LINE__,__FILE__,hr ) 
/*GetLastError���Բ�׽һЩwindows�޷����ֵĴ���*/
#define CHWND_LAST_EXCEPT() Window::Exception(__LINE__, __FILE__, GetLastError())