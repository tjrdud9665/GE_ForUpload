#pragma once

#include "GameAbility_Attack.h"
#include "Timer/TimerHandle.h"

class GameAbility_ChargeAttack : public GameAbility_Attack
{
	CLASS(GameAbility_ChargeAttack, GameAbility_Attack)

protected:
	GameAbility_ChargeAttack();
	GameAbility_ChargeAttack(const GameAbility_ChargeAttack& Other);
	virtual ~GameAbility_ChargeAttack();


	virtual void OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec) override;


private:
	virtual void OnActivateAbility(const SGameplayAbilitySpec& InSpec) override;
	virtual void OnCancelAbility();
	virtual void OnEndAbility();
	virtual void InputPressed(const SGameplayAbilitySpec& InSpec) override;
	virtual void InputReleased(const SGameplayAbilitySpec& InSpec) override;
	void	MaxChargeTime();
	void	CheckChargeLevel();
	

protected:
	bool		m_ChargeOn; // Charge 시작 
	bool		m_KeyPushCheck; // 키입력 확인 
	bool		m_KeyUpCheck; // 키입력을 뗏는지 확인
	bool		m_Max; // Charge Time 최대치 도달 확인 


	bool		m_Charge; // 현재 차징중인지 확인 
	int			m_ChargeLevel; // 차징단계별 레벨 , 데미지계산하기위함 

	TimerHandle	m_ChargeTimer;
	uint64		m_ChargeMaxTimerID;
	uint64		m_ChargeLevelTimerID;

};

