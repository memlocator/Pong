#include"Timer.h"

using namespace std::chrono;
Timer::Timer()
{
	this->startTime = high_resolution_clock::now();
}
double Timer::timeElapsed() const
{
	auto currentTime = high_resolution_clock::now();
	return (double)std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
}
void Timer::start()
{
	startTime = high_resolution_clock::now();
}

void Timer::restart()
{
	startTime = std::chrono::high_resolution_clock::now();
}
