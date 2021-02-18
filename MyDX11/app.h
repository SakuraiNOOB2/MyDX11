#pragma once
#include "Window.h"
#include "myTimer.h"
#include "imguiManager.h"
#include "camera.h"
#include "PointLight.h"
#include "Model.h"
#include <set>

class App {

public:

	//Constructor
	App();
	
	//master frame / message loop
	int Go();

	//Destructor
	~App();

private:
	void DoFrame();

	//imgui windows management
	void SpawnSimulationWindow() noexcept;
	void SpawnBoxWindowManagerWindow() noexcept;
	void SpawnBoxWindows() noexcept;
	void ShowImguiDemoWindow();
	void ShowRawInputWindow();

private:
	ImguiManager imgui;

	Window m_wnd;
	myTimer m_timer;

	//speed factor
	float m_speedFactor = 1.0f;

	//camera
	Camera m_camera;

	//point light
	PointLight m_light;

	//Drawable
	std::vector<std::unique_ptr<class Drawable>> m_drawables;
	std::vector<class Box*> m_boxes;
	static constexpr size_t m_nDrawables = 45;

	Model m_nano{ m_wnd.Gfx(),"asset\\model\\nano.gltf" };


	//Combo Box control 
	std::optional<int> m_comboBoxIndex;
	std::set<int> m_boxControlIDs;

	//raw input data
	int x = 0, y = 0;

};