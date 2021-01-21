#pragma once
#include "Window.h"
#include "myTimer.h"
#include "imguiManager.h"
#include "camera.h"
#include "PointLight.h"

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

private:
	ImguiManager imgui;

	Window wnd;
	myTimer timer;

	//speed factor
	float speedFactor = 1.0f;

	//camera
	Camera m_Camera;

	//point light
	PointLight m_light;

	//Drawable
	std::vector<std::unique_ptr<class Drawable>> drawables;
	static constexpr size_t nDrawables = 180;
};