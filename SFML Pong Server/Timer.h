#pragma once
#include<chrono>

class Timer
{
private:
	std::chrono::time_point<std::chrono::steady_clock> startTime;
public:
	Timer();
	double timeElapsed() const;
	void start();
	void restart();

};