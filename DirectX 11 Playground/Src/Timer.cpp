#include "Timer.h"

Timer::Timer()
{
	startPoint = std::chrono::high_resolution_clock::now();
	stopPoint = std::chrono::high_resolution_clock::now();
}

double Timer::getMillisecondsElapsed()
{
	if (isRunning)
	{
		auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - startPoint);
		return elapsed.count();
	}
	else
	{
		auto elapsed = std::chrono::duration<double, std::milli>(stopPoint - startPoint);
		return elapsed.count();
	}
}

void Timer::restart()
{
	isRunning = true;
	startPoint = std::chrono::high_resolution_clock::now();
}

bool Timer::stop()
{
	if (!isRunning)
		return false;
	else
	{
		stopPoint = std::chrono::high_resolution_clock::now();
		isRunning = false;
		return true;
	}
}

bool Timer::start()
{
	if (isRunning)
		return false;
	else
	{
		startPoint = std::chrono::high_resolution_clock::now();
		isRunning = true;
		return true;
	}
}
