#pragma once
#include "GameAbillity_Attack.h"
#include "Timer/TimerHandle.h"

class GameAbillity_RunningAttack :
    public GameAbillity_Attack
{
	CLASS(GameAbillity_RunningAttack, GameAbillity_Attack)

protected:
	GameAbillity_RunningAttack();
	GameAbillity_RunningAttack(const GameAbillity_RunningAttack& Other);
	virtual ~GameAbillity_RunningAttack();


	virtual void OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec) override;


private:
	virtual void OnActivateAbility(const SGameplayAbilitySpec& InSpec) override;
	virtual void OnCancelAbility();
	virtual void OnEndAbility();
	virtual void InputPressed(const SGameplayAbilitySpec& InSpec) override;
	virtual void InputReleased(const SGameplayAbilitySpec& InSpec) override;

protected:
	TimerHandle	m_ChargeTimer;
	uint64		m_TimerID;
	bool		m_KeyDownCheck;
	bool		m_KeepRunning;
	int			m_Count;

};

