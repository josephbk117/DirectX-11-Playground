#pragma once
#include <chrono>
class Timer
{
private:
	bool isRunning = false;
	std::chrono::time_point<std::chrono::steady_clock> startPoint;
	std::chrono::time_point<std::chrono::steady_clock> stopPoint;
public:
	Timer();
	double getMillisecondsElapsed();
	void restart();
	bool stop();
	bool start();	
};

