#include "myTimer.h"

using namespace std::chrono;

myTimer::myTimer() {

	last = steady_clock::now();
}

float myTimer::Mark() {

	const auto old = last;
	last = steady_clock::now();
	const duration<float> frameTime = last - old;
	return frameTime.count();
}

float myTimer::Peek() const {

	return duration<float>(steady_clock::now() - last).count();
}