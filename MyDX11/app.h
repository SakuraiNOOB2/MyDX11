#pragma once
#include "Window.h"
#include "myTimer.h"

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
	Window wnd;
	myTimer timer;

	//Drawable Box
	std::vector<std::unique_ptr<class Box>> boxes;
};