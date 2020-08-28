#include "Window.h"

#define windowLenth (1024)
#define windowWidth (768)


int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow) {

	try {
		Window wnd(windowLenth, windowWidth, "Age of Banana");

		//message pump
		MSG msg;
		BOOL gResult;
		while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0) {

			TranslateMessage(&msg);
			DispatchMessage(&msg);

			//Test Area


		}

		if (gResult == -1) {

			return -1;
		}

		return msg.wParam;
	}

	//error handling
	//catch myException
	catch (const myException& e) {

		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	//catch Standard Exception if there is no myException
	catch (const std::exception& e) {

		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...) {

		MessageBox(nullptr, "No details avaliable", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}

	return -1;
}



//
//LRESULT CALLBACK WndProc(HWND hWnd, 
//						 UINT msg, 
//						 WPARAM wParam, 
//					 	 LPARAM lParam) {
//
//	static WindowsMessageMap messageMap;
//	OutputDebugString(messageMap(msg, lParam, wParam).c_str());
//
//	switch (msg) {
//		case WM_CLOSE:
//
//			PostQuitMessage(69);
//			break;
//
//		//WM_KEYDOWN doesn't have any notion on lowercase or uppercase letters
//		//used when looking for key stroll inputs
//		case WM_KEYDOWN:
//			if (wParam == 'F') {
//
//				SetWindowText(hWnd, "Respects");
//			}
//			break;
//
//		case WM_KEYUP:
//			if (wParam == 'F') {
//
//				SetWindowText(hWnd, "OOF");
//			}
//			break;
//
//		//WM_CHAR is a message for text input
//		//and also have transformation on uppercase and lowercase letters
//		//used when looking for text inputs in words and sentenses
//		case WM_CHAR:
//		{
//			static std::string title;
//			title.push_back((char)wParam);
//			SetWindowText(hWnd, title.c_str());
//		}
//
//			break;
//
//		case WM_LBUTTONDOWN:
//		{
//			POINTS pt = MAKEPOINTS(lParam);
//			std::ostringstream oss;
//			oss << "(" << pt.x << "," << pt.y << ")";
//			SetWindowText(hWnd, oss.str().c_str());
//		}
//
//			break;
//	}
//
//	return DefWindowProc(hWnd, msg, wParam, lParam);
//}
//
//
//int CALLBACK WinMain(
//	HINSTANCE hInstance,
//	HINSTANCE hPrevInstance,
//	LPSTR     lpCmdLine,
//	int		  nCmdShow) {
//
//	//class name
//	const auto pClassName = "ageofbananas";
//	
//	//register window class
//	WNDCLASSEX wc = { 0 };
//	
//	wc.cbSize = sizeof(wc);
//	wc.style = CS_OWNDC;
//	wc.lpfnWndProc = WndProc;
//	wc.cbClsExtra = 0;
//	wc.cbWndExtra = 0;
//	wc.hInstance = hInstance;
//	wc.hIcon = nullptr;
//	wc.hCursor = nullptr;
//	wc.hbrBackground = nullptr;
//	wc.lpszMenuName = nullptr;
//	wc.lpszClassName = pClassName;
//	wc.hIconSm = nullptr;
//
//	RegisterClassEx(&wc);
//
//	//create window
//	HWND hWnd = CreateWindowEx(
//		0,
//		pClassName,
//		"Age of Bananas",
//		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
//		0,
//		0,
//		windowLenth,
//		windowWidth,
//		nullptr,
//		nullptr,
//		hInstance,
//		nullptr
//	);
//
//	//show the window
//	ShowWindow(hWnd, SW_SHOW);
//
//	//message pump
//	MSG msg;
//	BOOL gResult;
//
//	while ((gResult=GetMessage(&msg, nullptr, 0, 0)) > 0) {
//
//		TranslateMessage(&msg);
//		DispatchMessage(&msg);
//
//	}
//
//	if (gResult == -1) {
//
//		return -1;
//	}
//	else {
//
//		return msg.wParam;
//	}
//
//}