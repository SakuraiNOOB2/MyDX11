#pragma once
#include "myWin.h"
#include "myException.h"
#include "keyboard.h"
#include "mouse.h"
#include "graphics.h"
#include <optional>
#include <memory>

class Window {

public:

	//Window exception
	//error handling (myException class)

	//Windows have error code refer to HRESULT
	class Exception :public myException {
		
		using myException::myException;
	
	public:

		//give a description string to error code
		static std::string TranslateErrorCode(HRESULT hr) noexcept;

	};

	class HrException :public Exception {

	public:

		HrException(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;

		//get error from the HRESULT
		HRESULT GetErrorCode() const noexcept;

		//take the HRESULT store to Exception
		std::string GetErrorDescription() const noexcept;

	private:
		HRESULT hr;

	};


	class NoGfxException :public Exception {

	public:
		using Exception::Exception;
		const char* GetType() const noexcept override;

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
	void SetTitle(const std::string& title);
	static std::optional<int> ProcessMessages() noexcept;
	Graphics& Gfx();


	//cursor handeling
	void EnableCursor() noexcept;
	void DisableCursor() noexcept;
	bool GetCursorEnabled() const noexcept;

private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	//cursor handeling
	void ConfineCursor() noexcept;
	void FreeCursor() noexcept;

	void HideCursor() noexcept;
	void ShowCursor() noexcept;

	//imgui mouse handeling
	void EnableImGuiMouse() noexcept;
	void DisableImGuiMouse() noexcept;

public:
	Keyboard kbd;
	Mouse mouse;
private:

	//mouse stuffs
	bool isCursorEnabled = true;
	std::vector<BYTE> rawBuffer;

	//window stuffs
	int width;
	int height;
	HWND hWnd;
	std::unique_ptr<Graphics> pGfx;
};

