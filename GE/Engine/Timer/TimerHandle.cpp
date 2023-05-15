#include "EnginePCH.h"
#include "TimerHandle.h"
#include "TimerManager.h"

TimerHandle::TimerHandle()
{

}

TimerHandle::~TimerHandle()
{
	if (!g_Engine)
		return;

	TimerManager* Mgr = g_Engine->Get<TimerManager>();
	if(Mgr)
	{
		TimerIDList::iterator iter = m_Handles.begin();
		TimerIDList::iterator End = m_Handles.end();


		for (; iter != End; iter++)
		{
			Mgr->RemoveTimer(*iter);
		}
	}

}

void TimerHandle::SetTimer(uint64& OutHandle,float Interval, bool Loop, std::function<void(void)> func)
{
	if (!g_Engine)
		return;
	TimerManager* Mgr= g_Engine->Get<TimerManager>();
	if(Mgr)
	{
		Mgr->RemoveTimer(OutHandle);
		Mgr->SetTimer(OUT OutHandle, Interval, Loop, func);
		m_Handles.push_back(OutHandle);
	}
	
}

void TimerHandle::RemoveTimer(uint64& InOutHandle)
{
	if (!g_Engine)
		return;
	TimerManager* Mgr = g_Engine->Get<TimerManager>();
	if (Mgr)
	{
		uint64 temp = InOutHandle;
		Mgr->RemoveTimer(OUT InOutHandle);
		m_Handles.remove(temp);
	}
}

void TimerHandle::PauseTimer(uint64 timerID)
{
	TimerManager* Mgr = g_Engine->Get<TimerManager>();
	if (Mgr)
	{
		Mgr->PauseTimer(timerID);		
	}
}

void TimerHandle::ResumeTimer(uint64 timerID)
{
	TimerManager* Mgr = g_Engine->Get<TimerManager>();
	if (Mgr)
	{
		Mgr->ResumeTimer(timerID);
	}
}

float TimerHandle::GetRemainTime(uint64 timerID) const
{
	TimerManager* Mgr = g_Engine->Get<TimerManager>();
	if (Mgr)
	{
		return Mgr->GetRemainTime(timerID);
	}

	return -1.0f;
}

float TimerHandle::GetRemainTimeRate(uint64 timerID) const
{
	TimerManager* Mgr = g_Engine->Get<TimerManager>();
	if (Mgr)
	{
		return Mgr->GetRemainTimeRate(timerID);
	}

	return -1.0f;
}
