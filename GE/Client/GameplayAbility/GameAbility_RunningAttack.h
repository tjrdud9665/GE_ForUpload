#pragma once
#include "GameAbility_Attack.h"
#include "Timer/TimerHandle.h"

class GameAbility_RunningAttack : public GameAbility_Attack
{
	CLASS(GameAbility_RunningAttack, GameAbility_Attack)

protected:
	GameAbility_RunningAttack();
	GameAbility_RunningAttack(const GameAbility_RunningAttack& Other);
	virtual ~GameAbility_RunningAttack();


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
	int			m_RunningCount;

	PROP(int , m_MaxCount, DetailsEdit)
	int			m_MaxCount;


	PROP(TSubclassOf<GameplayEffect>, AttackSet, DetailsEdit)
		TSubclassOf<GameplayEffect> AttackSet;

};

