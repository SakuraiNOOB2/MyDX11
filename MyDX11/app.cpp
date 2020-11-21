#include "app.h"
#include "Box.h"
#include <memory>

#define windowLenth (1024)
#define windowWidth (768)

App::App()
	:
	wnd(windowLenth, windowWidth, "Age of Banana")
{

	std::mt19937 rng(std::random_device{}());

	std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);

	std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 2.0f);

	std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.3f);

	std::uniform_real_distribution<float> rdist(6.0f, 20.0f);

	//create boxes
	for (auto i = 0; i < 80; i++) {

		boxes.push_back(std::make_unique<Box>(wnd.Gfx(), rng, adist, ddist, odist, rdist));
	}

	//set the projection matrix for the graphics
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));

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

App::~App()
{
}

void App::DoFrame() {

	//add deltatime
	auto dt = timer.Mark();

	//buffer clearing
	wnd.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f);

	for (auto& b : boxes) {

		//update all the boxes
		b->Update(dt);

		//draw all the boxes
		b->Draw(wnd.Gfx());
	}

	wnd.Gfx().EndFrame();
} 