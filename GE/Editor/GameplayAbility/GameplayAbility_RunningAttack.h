#pragma once
#include "GameplayAbility_Attack.h"
#include "Timer/TimerHandle.h"

class GameplayAbility_RunningAttack : public GameplayAbility_Attack
{
	CLASS(GameplayAbility_RunningAttack, GameplayAbility_Attack)

protected:
	GameplayAbility_RunningAttack();
	GameplayAbility_RunningAttack(const GameplayAbility_RunningAttack& Other);
	virtual ~GameplayAbility_RunningAttack();


	virtual void OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec) override;


public:
	virtual void OnCancelAbility();
	virtual void OnEndAbility();
	virtual void InputPressed(const SGameplayAbilitySpec& InSpec) override;

protected:
	TimerHandle	m_ChargeTimer;
	uint64		m_TimerID;
	bool		m_KeyDownCheck;
	bool		m_KeepRunning;
	int			m_RunningCount;

	PROP(int32, MaxCount, DetailsEdit)
	int32	MaxCount;

};

