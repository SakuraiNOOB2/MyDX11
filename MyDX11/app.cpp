#include "app.h"
#include <sstream>
#include <iomanip>

#define windowLenth (1024)
#define windowWidth (768)

App::App()
	:
	wnd(windowLenth, windowWidth, "Age of Banana")
{
}

int App::Go() {

	while (true) {

		//Process all messages pending,but to not block for new messages
		if (const auto ecode = Window::ProcessMessages()) {

			//if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		DoFrame();
	}
}

void App::DoFrame() {

	const float c = sin(timer.Peek() / 2.0f + 0.5f);
	wnd.Gfx().ClearBuffer(c, c, 1.0f);
	wnd.Gfx().EndFrame();
} 