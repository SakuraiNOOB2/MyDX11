#include "Window.h"
#include <sstream>
#include "resource.h"

//Window Class Stuff
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	:
	hInst(GetModuleHandle(nullptr))
{
	
	//register window class
	WNDCLASSEX wc = { 0 };

	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));

	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass() {

	UnregisterClass(wndClassName, GetInstance());
}

const char* Window::WindowClass::GetName() noexcept {

	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept {

	return wndClass.hInst;
}

//Window Stuff

//constructor
//width and height should be stored into class
Window::Window(int width, int height, const char* name)
	:
	width(width),
	height(height)
{
	//calculate window size based on desired client region size
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	
	if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE)==0) {

		throw CHWND_LAST_EXCEPT();
	}

	//create window & get hWnd handling
	hWnd = CreateWindow(
		WindowClass::GetName(),
		name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wr.right - wr.left,
		wr.bottom - wr.top,
		nullptr,
		nullptr,
		WindowClass::GetInstance(),
		this
	);

	//check for error
	if (hWnd == nullptr) {

		throw CHWND_LAST_EXCEPT();
	}

	//show window handling
	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

Window::~Window() {

	DestroyWindow(hWnd);
}

void Window::SetTitle(const std::string& title) {

	if (SetWindowText(hWnd, title.c_str()) == 0) {

		throw CHWND_LAST_EXCEPT();
	}
	
}

//Installation: Setup pointers to instances in windows 32 side
LRESULT WINAPI Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{

	//use create parameter passed in from CreateWindow() to store window class pointer
	if (msg == WM_NCCREATE) {

		//extract ptr to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);


		//Store data to windows API side
		//set WinAPI-managed user data to store ptr to window class
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));

		//set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));

		//forward message to window class handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	//if we get a message before the WM_NCCREATE message,handle with default handler
	return DefWindowProc(hWnd, msg, wParam, lParam);

}

//Adapter: Adapted from win32 call convention
//Invoke the member funtcion
LRESULT WINAPI Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept 
{
	//retrieve ptr to window class
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	//forward message to window class handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

//
LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{

	static WindowsMessageMap messageMap;
	OutputDebugString(messageMap(msg, lParam, wParam).c_str());

	switch (msg) {

	case WM_CLOSE:

		PostQuitMessage(0);
		return 0;

		//clear keystate when window loses focus to prevent input getting focus
	case WM_KILLFOCUS:
		kbd.ClearState();
		break;

		//********** Keyboard Messages **********//

	case WM_KEYDOWN:

		//syskey commands need to be handle to track ALT key(VK_MENU)
	case WM_SYSKEYDOWN:
		if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnabled()) {

			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		kbd.OnChar(static_cast<unsigned char>(wParam));
		break;

		//********** Mouse Messages **********//
	case WM_MOUSEMOVE:
	{
		const POINTS pt = MAKEPOINTS(lParam);

		//in client region -> log move, and log enter + capture mouse (if not 
		if (pt.x >= 0 && pt.x < width&&pt.y >= 0 && pt.y < height) {
			
			mouse.OnMouseMove(pt.x, pt.y);

			// the mouse isnt in the client window
			if (!mouse.IsInWindow()) {

				//capture the mouse even the mouse leaves the window
				SetCapture(hWnd);

				//tell the mouse component enter the window
				mouse.OnMouseEnter();
			}
		}
		// not in client -> log move / maintain capture if button down
		else {

			//check the left button or right button is currently down
			//even outside the region
			//could have also used LeftIsPressed() etc. instead of wParam
			if (wParam&(MK_LBUTTON | MK_RBUTTON)) {

				mouse.OnMouseMove(pt.x, pt.y);
			}
			// button up -> release capture / log event for leaving
			else {

				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		const POINTS pt = MAKEPOINTS(lParam);

		//wParam contains mouse wheel data in this suitation
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(pt.x, pt.y, delta);

		break;
	}

	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


//Window Exception Stuff

//constructor
//call for base exception
Window::Exception::Exception(int line,const char* file,HRESULT hr) noexcept
	:
	myException(line,file),
	hr(hr)
{}

const char* Window::Exception::what() const noexcept {

	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code]" << GetErrorCode() << std::endl
		<< "[Description]" << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();

}

const char* Window::Exception::GetType() const noexcept {

	return "SupaHotFire Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept {

	char* pMsgBuff = nullptr;

	//take the HRESULT and give you description string for specific error code
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuff),
		0,
		nullptr);

	if (nMsgLen == 0) {

		return "Unidentified error code";
	}

	std::string errorString = pMsgBuff;
	LocalFree(pMsgBuff);
	return errorString;
}

HRESULT Window::Exception::GetErrorCode() const noexcept {

	return hr;
}

std::string Window::Exception::GetErrorString() const noexcept {

	return TranslateErrorCode(hr);
}