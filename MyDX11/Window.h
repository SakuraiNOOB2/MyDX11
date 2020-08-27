#pragma once
#include "myWin.h"
#include "myException.h"

class Window {

public:

	//Window exception
	//error handling (myException class)

	//Windows have error code refer to HRESULT
	class Exception :public myException {
	public:
		Exception(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;

		//give a description string to error code
		static std::string TranslateErrorCode(HRESULT hr) noexcept;

		//get error from the HRESULT
		HRESULT GetErrorCode() const noexcept;

		//take the HRESULT store to Exception
		std::string GetErrorString() const noexcept;

	private:
		HRESULT hr;

	};

private:

	//singleton manages registration/cleanup of window class
	class WindowClass {

	public:

		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;

	private:

		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const char* wndClassName = "Banana DX3D Engine Window";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};

public:
	Window(int width, int height, const char* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

private:
	int width;
	int height;
	HWND hWnd;

};

//error exception helper macro
#define CHWND_EXCEPT(hr) Window::Exception(__LINE__,__FILE__,hr)
#define CHWND_LAST_EXCEPT() Window::Exception(__LINE__,__FILE__,GetLastError())