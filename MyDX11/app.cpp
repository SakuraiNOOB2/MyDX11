#include "app.h"
#include "Box.h"
#include <memory>
#include <algorithm>
#include "myMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
GDIPlusManager gdipm;

#define windowLenth (1024)
#define windowWidth (768)

App::App()
	:
	wnd(windowLenth, windowWidth, "Age of Banana"),
	m_light(wnd.Gfx())
{

	class Factory {

	public:

		//constructor
		Factory(Graphics& gfx)
			:
			gfx(gfx){}

		std::unique_ptr<Drawable> operator()() {

			const DirectX::XMFLOAT3 mat = { cdist(rng),cdist(rng),cdist(rng) };

			return std::make_unique<Box>(
				gfx, 
				rng, 
				adist, 
				ddist, 
				odist, 
				rdist, 
				bdist,
				mat);

		}

	private:

		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_real_distribution<float> cdist{ 0.0f,1.0f };

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

	//add deltatime and * by speedFactor
	auto dt = timer.Mark() * speedFactor;

	//buffer clearing
	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd.Gfx().SetCamera(m_Camera.GetMatrix());
	m_light.Bind(wnd.Gfx(),m_Camera.GetMatrix());

	for (auto& d:drawables) {

		//update all the boxes
		d->Update(wnd.kbd.KeyIsPressed(VK_SPACE) ? 0.0f : dt);

		//draw all the boxes
		d->Draw(wnd.Gfx());
	}
	
	m_light.Draw(wnd.Gfx());

	/// <summary>
	/// imgui stuff
	/// </summary>


	// 1st imgui window (showing simulation speed)
	static char buffer[1024];
	if (ImGui::Begin("Simulation Speed")) {

		ImGui::SliderFloat("Speed Factor", &speedFactor, 0.0f, 6.0f, "%.4f", 3.2f);
		ImGui::Text("%.3f ms/frame (%.1f fps)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("StatusÅF%s", wnd.kbd.KeyIsPressed(VK_SPACE) ? "Pause" : "Running(hold spacebar to pause)");

	}
	ImGui::End();

	//imgui windows to control camera and light
	m_Camera.SpawnControlWindow();
	m_light.SpawnControlWindow();

	//present
	wnd.Gfx().EndFrame();
} 

App::~App()
{
}