#include "app.h"
#include "Melon.h"
#include "Pyramid.h"
#include "Box.h"
#include "Sheet.h"
#include "SkinnedBox.h"
#include <memory>
#include <algorithm>
#include "myMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

GDIPlusManager gdipm;

#define windowLenth (1024)
#define windowWidth (768)

App::App()
	:
	wnd(windowLenth, windowWidth, "Age of Banana")
{

	class Factory {

	public:

		//constructor
		Factory(Graphics& gfx)
			:
			gfx(gfx){}

		std::unique_ptr<Drawable> operator()() {

			switch (typedist(rng)) {

			case 0:

				return std::make_unique<Pyramid>(
					gfx, rng, adist, ddist, odist, rdist
					);

			case 1:

				return std::make_unique<Box>(
					gfx, rng, adist, ddist, odist, rdist, bdist
					);

			case 2:
				return std::make_unique<Melon>(
					gfx, rng, adist, ddist, odist, rdist, longdist, latdist
					);

			case 3:
				return std::make_unique<Sheet>(
					gfx, rng, adist, ddist, odist, rdist
					);

			case 4:
				return std::make_unique<SkinnedBox>(
					gfx, rng, adist, ddist, odist, rdist
					);


			default:

				assert(false && "Bad drawable type in factory");
				return {};

			}

		}

	private:

		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_int_distribution<int> latdist{ 5,20 };
		std::uniform_int_distribution<int> longdist{ 10,40 };
		std::uniform_int_distribution<int> typedist{ 0,4 };
	};

	//create boxes
	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, Factory{ wnd.Gfx() });

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



void App::DoFrame() {

	//add deltatime
	auto dt = timer.Mark();

	//buffer clearing
	wnd.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f);

	for (auto& d:drawables) {

		//update all the boxes
		d->Update(wnd.kbd.KeyIsPressed(VK_SPACE) ? 0.0f : dt);

		//draw all the boxes
		d->Draw(wnd.Gfx());
	}

	//imgui stuff
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	static bool show_demo_window = true;
	if (show_demo_window) {

		ImGui::ShowDemoWindow(&show_demo_window);
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	//present
	wnd.Gfx().EndFrame();
} 

App::~App()
{
}