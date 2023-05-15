#include "EnginePCH.h"
#include "Timer.h"

Timer::Timer()
{
	m_last = std::chrono::steady_clock::now();
	
}

Timer::~Timer()
{

}

float Timer::Mark()
{
	const auto old = m_last;
	m_last = std::chrono::steady_clock::now();
	std::chrono::duration<float> delta = m_last - old;

	return delta.count();
}

float Timer::Peek()
{
	return std::chrono::duration<float>(std::chrono::steady_clock::now() - m_last).count();
}