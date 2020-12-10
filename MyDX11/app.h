#pragma once
#include "Window.h"
#include "myTimer.h"
#include "imguiManager.h"

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

	//Drawable
	std::vector<std::unique_ptr<class Drawable>> drawables;
	static constexpr size_t nDrawables = 180;
};