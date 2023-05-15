#pragma once
#include <chrono>

class Timer 
{
	friend class TimerManager;
	friend class Engine;


private:
	Timer();
	~Timer();

	float Mark();
	float Peek();

private:
	std::chrono::steady_clock::time_point m_last;
};

