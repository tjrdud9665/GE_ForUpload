#pragma once
#include "GameplayAbility_Attack.h"
#include "Timer/TimerHandle.h"

class GameplayAbility_ChargeAttack : public GameplayAbility_Attack
{
	CLASS(GameplayAbility_ChargeAttack, GameplayAbility_Attack)

protected:
	GameplayAbility_ChargeAttack();
	GameplayAbility_ChargeAttack(const GameplayAbility_ChargeAttack& Other);
	virtual ~GameplayAbility_ChargeAttack();


	virtual void OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec) override;


private:
	virtual void OnActivateAbility(const SGameplayAbilitySpec& InSpec) override;


public:
	virtual void OnCancelAbility();
	virtual void OnEndAbility();


private:
	virtual void InputReleased(const SGameplayAbilitySpec& InSpec) override;
	void	MaxChargeTime();
	void	AddChargeLevel();

protected:
	PROP(TSubclassOf<GameplayEffect>, ChargeHitEffect, DetailsEdit)
	TSubclassOf<GameplayEffect>	ChargeHitEffect;					//공격에 적중당한 Target에게 적용할 Hit처리

	PROP(float, MaxChargingTime, DetailsEdit)
	float	MaxChargingTime;										//최대 차징 타임, 1초마다 차징레벨이 올라가고 최대차징 시 데미지 다르게 처리


	float			m_ChargeStartTime;	//차징을 시작하는 지점
	bool			m_ChargeState;		//차징 중인지?
	int32			m_ChargeLevel;		// 차징단계별 레벨 , 데미지계산하기위함 
	TimerHandle		m_ChargeTimer;
	uint64			m_ChargeMaxTimerID;
	uint64			m_ChargeLevelTimerID;
};

