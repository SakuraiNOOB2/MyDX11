#include "app.h"

#define windowLenth (1024)
#define windowWidth (768)

App::App()
	:
	wnd(windowLenth, windowWidth, "Age of Banana")
{
}

int App::Go() {

	MSG msg;
	BOOL gResult;

	while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0) {

		//TranslateMessage will post auxilliary WM_ messages from key msgs
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		DoFrame();
	}

	//check if GetMessage call itself borked
	if (gResult == -1) {

		throw CHWND_LAST_EXCEPT();
	}

	//wParam here is the value passed to PostQuitMessage
	return msg.wParam;
}

void App::DoFrame() {


}