#pragma once
#include <chrono>

class myTimer{

public:
	myTimer();
	float Mark();
	float Peek() const;

private:
	std::chrono::steady_clock::time_point last;

};
