#pragma once
#include "Timer.h"

#define INVALID_TIMER_ID 0

struct STimeData
{
	uint64						TimerId;
	float						Interval;
	bool						Loop;
	std::function<void(void)>	Func;

	//Only runningtime
	float	current;
	bool	Pause;

};
//직접 참조하여 타이머를 설정하지 말고 TimerHandle을 이용할것

class TimerManager : public XObject
{
	
	friend class Globals;	
	friend class Engine;
	friend class TimerHandle;

	using TimerList = std::list<STimeData>;

private:
	TimerManager();
	~TimerManager();

	void Initialize();

	virtual void Update(float DeltaSeconds)				override;

private:
	void SetTimer(uint64& outTimerId, float interval, bool Loop, function<void(void)> func);
	//In Out
	void RemoveTimer(uint64& timerID);
	void PauseTimer(uint64 timerID);
	void ResumeTimer(uint64 timerID);
	void ClearAllTimer();
	void UpdateDeltaSeconds(float& OutDeltaTime);

	void UpdateBoundedTimer();



	float GetRemainTime(uint64 timerID)			const;
	float GetRemainTimeRate(uint64 timerID)		const;
	float GetDeltaTime()						const;
private:
	Timer								m_FrameTimer;

	float								m_DeltaTime;
	TimerList							m_TimerList;

	//UID
	static uint64 timerUID;
};

