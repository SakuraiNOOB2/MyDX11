#include "app.h"
#include "Box.h"
#include "Cylinder.h"
#include "Pyramid.h"
#include "SkinnedBox.h"
#include "ModelTest.h"
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
	m_wnd(windowLenth, windowWidth, "Banana Engine"),
	m_light(m_wnd.Gfx())
{
	
	
	class Factory {

	public:

		//constructor
		Factory(Graphics& gfx)
			:
			gfx(gfx){}

		std::unique_ptr<Drawable> operator()() {

			const DirectX::XMFLOAT3 mat = { cdist(rng),cdist(rng),cdist(rng) };


			switch (sdist(rng)) {

			case 0:

				return std::make_unique<Box>(
					gfx,
					rng,
					adist,
					ddist,
					odist,
					rdist,
					bdist,
					mat);

			case 1:

				return std::make_unique<Cylinder>(
					gfx,
					rng,
					adist,
					ddist,
					odist,
					rdist,
					bdist,
					tdist);

			case 2:
				return std::make_unique<Pyramid>(
					gfx,
					rng,
					adist,
					ddist,
					odist,
					rdist,
					tdist);

			case 3:

				return std::make_unique<SkinnedBox>(
					gfx,
					rng,
					adist,
					ddist,
					odist,
					rdist
					);

			default:
				assert(false && "Impossible drawable option in factory");
				return {};

			}


			

		}

	private:

		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_int_distribution<int> sdist{ 0,3 };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_real_distribution<float> cdist{ 0.0f,1.0f };
		std::uniform_int_distribution<int> tdist{ 3,30 };

	};

	//create boxes
	m_drawables.reserve(m_nDrawables);
	std::generate_n(std::back_inserter(m_drawables), m_nDrawables, Factory{ m_wnd.Gfx() });

	//init box pointers for editing instance parameters
	for (auto& pd : m_drawables) {

		if (auto pb = dynamic_cast<Box*>(pd.get())) {

			m_boxes.push_back(pb);
		}

	}

	//set the projection matrix for the graphics
	m_wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 40.0f));

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
	auto dt = m_timer.Mark() * m_speedFactor;

	//buffer clearing
	m_wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	m_wnd.Gfx().SetCamera(m_camera.GetMatrix());
	m_light.Bind(m_wnd.Gfx(),m_camera.GetMatrix());

	//render
	for (auto& d:m_drawables) {

		//update all the boxes
		d->Update(m_wnd.kbd.KeyIsPressed(VK_SPACE) ? 0.0f : dt);

		//draw all the boxes
		d->Draw(m_wnd.Gfx());
	}
	

	//nano boi model
	m_nano.Draw(m_wnd.Gfx());

	//point light
	m_light.Draw(m_wnd.Gfx());

	//raw input stuffs
	while (const auto e = m_wnd.kbd.ReadKey()){

		if (e->IsPress() && e->GetCode() == VK_INSERT)
		{
			if (m_wnd.GetCursorEnabled())
			{
				m_wnd.DisableCursor();
				m_wnd.mouse.EnableRaw();
			}
			else
			{
				m_wnd.EnableCursor();
				m_wnd.mouse.DisableRaw();
			}
		}
	}


	/// <summary>
	/// imgui stuff
	/// </summary>


	// 1st imgui window (showing simulation speed)
	static char buffer[1024];
	SpawnSimulationWindow();

	//imgui windows for controling stuffs
	m_camera.SpawnControlWindow();	//camera
	m_light.SpawnControlWindow();	//point light
	//ShowImguiDemoWindow();
	m_nano.ShowWindow();		//nano boi
	ShowRawInputWindow();


	SpawnBoxWindowManagerWindow();	 //boxes manager
	SpawnBoxWindows();			//boxes
	
	
	ImGui::End();

	
	//present
	m_wnd.Gfx().EndFrame();
}
void App::SpawnSimulationWindow() noexcept
{

	if (ImGui::Begin("Simulation Speed")) {

		ImGui::SliderFloat("Speed Factor", &m_speedFactor, 0.0f, 6.0f, "%.4f", 3.2f);
		ImGui::Text("%.3f ms/frame (%.1f fps)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("StatusÅF%s", m_wnd.kbd.KeyIsPressed(VK_SPACE) ? "Pause" : "Running(hold spacebar to pause)");

	}
	ImGui::End();

}

void App::SpawnBoxWindowManagerWindow() noexcept
{
	//imgui windows to control box instance parameters
	if (ImGui::Begin("Boxes")) {

		const auto preview = m_comboBoxIndex ? std::to_string(*m_comboBoxIndex) : "Choose Box";

		if (ImGui::BeginCombo("Box Number", preview.c_str())) {

			for (int i = 0; i < m_boxes.size(); i++) {

				const bool isSelected = *m_comboBoxIndex == i;

				if (ImGui::Selectable(std::to_string(i).c_str(), isSelected)) {

					m_comboBoxIndex = i;
				}

				if (isSelected) {

					ImGui::SetItemDefaultFocus();
				}

			}

			ImGui::EndCombo();
		}

		if (ImGui::Button("Spawn Control Window") && m_comboBoxIndex) {

			m_boxControlIDs.insert(*m_comboBoxIndex);

			m_comboBoxIndex.reset();
		}


	}

}

void App::SpawnBoxWindows() noexcept
{
	//imgui box attribute control windows
	for (auto i = m_boxControlIDs.begin(); i != m_boxControlIDs.end();) {

		if (!m_boxes[*i]->SpawnControlWindow(*i, m_wnd.Gfx())) {

			i = m_boxControlIDs.erase(i);
		}
		else {
			i++;
		}
		
	}
}

void App::ShowImguiDemoWindow()
{
	static bool isShowDemoWindow = true;

	if (isShowDemoWindow) {

		ImGui::ShowDemoWindow(&isShowDemoWindow);
	}


}

void App::ShowRawInputWindow()
{
	while (const auto d = m_wnd.mouse.ReadRawDelta())
	{
		x += d->x;
		y += d->y;
	}
	if (ImGui::Begin("Raw Input"))
	{
		ImGui::Text("Tally: (%d,%d)", x, y);
		ImGui::Text("Cursor: %s", m_wnd.GetCursorEnabled() ? "enabled" : "disabled");
	}
	ImGui::End();

}


App::~App()
{
}