#include "EnginePCH.h"
#include "TimerManager.h"
#include "Globals.h"


TimerManager::TimerManager()
{
	m_DeltaTime = 0;	
	
}

TimerManager::~TimerManager()
{

}

void TimerManager::Initialize()
{
	m_DeltaTime = m_FrameTimer.Mark();
}

void TimerManager::Update(float DeltaSeconds)
{	
	UpdateBoundedTimer();	
}

void TimerManager::SetTimer(uint64& outTimerId, float interval, bool Loop, function<void(void)> func)
{
	outTimerId = ++timerUID;
	STimeData timedata;
	timedata.TimerId = outTimerId;
	timedata.Interval = interval;
	timedata.Loop = Loop;
	timedata.Func = func;
	timedata.current = 0.0f;
	timedata.Pause = false;


	m_TimerList.push_back(timedata);
}

void TimerManager::RemoveTimer(uint64& timerID)
{
	if (!timerID)
		return;

	TimerList::iterator iter = m_TimerList.begin();
	TimerList::iterator End = m_TimerList.end();

	for (; iter != End; iter++)
	{
		if (iter->TimerId == timerID)
		{
			m_TimerList.erase(iter);
			timerID = 0;
			return;
		}
	}



}

void TimerManager::PauseTimer(uint64 timerID)
{
	if (!timerID)
		return;

	TimerList::iterator iter = m_TimerList.begin();
	TimerList::iterator End = m_TimerList.end();

	for (; iter != End; iter++)
	{
		if (iter->TimerId == timerID)
		{
			iter->Pause = true;
			return;
		}
	}
}

void TimerManager::ResumeTimer(uint64 timerID)
{
	if (!timerID)
		return;

	TimerList::iterator iter = m_TimerList.begin();
	TimerList::iterator End = m_TimerList.end();

	for (; iter != End; iter++)
	{
		if (iter->TimerId == timerID)
		{
			iter->Pause = false;
			return;
		}
	}
}

void TimerManager::ClearAllTimer()
{
	m_TimerList.clear();
	timerUID = 0;
}

void TimerManager::UpdateDeltaSeconds(float& OutDeltaTime)
{
	m_DeltaTime = m_FrameTimer.Mark();
	OutDeltaTime = m_DeltaTime;
}

void TimerManager::UpdateBoundedTimer()
{
	TimerList::iterator iter = m_TimerList.begin();
	TimerList::iterator End = m_TimerList.end();

	for (; iter != End;)
	{
		if (iter->Pause)
		{
			iter++;
			continue;
		}

		iter->current += m_DeltaTime;

		if (iter->current >= iter->Interval)
		{

			iter->Func();
			iter->current = 0;
			if (!iter->Loop)
			{

				iter = m_TimerList.erase(iter);
				End = m_TimerList.end();
				continue;
			}

		}

		iter++;
	}
}

float TimerManager::GetRemainTime(uint64 timerID) const
{
	TimerList::const_iterator findIter = find_if(m_TimerList.begin(), m_TimerList.end(), [=](const STimeData& e) { return e.TimerId == timerID; });

	if (findIter != m_TimerList.end())
	{
		return findIter->Interval - findIter->current;
	}

	return -1.0f;
}

float TimerManager::GetRemainTimeRate(uint64 timerID) const
{
	TimerList::const_iterator findIter = find_if(m_TimerList.begin(), m_TimerList.end(), [=](const STimeData& e) { return e.TimerId == timerID; });

	if (findIter != m_TimerList.end())
	{
		return findIter->current / findIter->Interval;
	}
	return -1.0f;
}

float TimerManager::GetDeltaTime() const
{
	return m_DeltaTime;
}

//0 == INVALID_ID
uint64 TimerManager::timerUID = 0;
