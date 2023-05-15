#pragma once

//Framework 내에서 타이머의 등록을 관리합니다.
//RAII 형태로 구현
class TimerHandle
{
	using TimerIDList = std::list<uint64>;
public:
	TimerHandle();
	~TimerHandle();

public:
	void SetTimer(uint64& OutHandle , float Interval, bool Loop, std::function<void(void)> func);
	void RemoveTimer(uint64& InOutHandle);
	
	void PauseTimer(uint64 timerID);
	void ResumeTimer(uint64 timerID);
	float GetRemainTime(uint64 timerID)	const;
	float GetRemainTimeRate(uint64 timerID)	const;	

private:
	TimerIDList			m_Handles;
};

