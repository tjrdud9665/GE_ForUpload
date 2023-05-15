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
	TSubclassOf<GameplayEffect>	ChargeHitEffect;					//���ݿ� ���ߴ��� Target���� ������ Hitó��

	PROP(float, MaxChargingTime, DetailsEdit)
	float	MaxChargingTime;										//�ִ� ��¡ Ÿ��, 1�ʸ��� ��¡������ �ö󰡰� �ִ���¡ �� ������ �ٸ��� ó��


	float			m_ChargeStartTime;	//��¡�� �����ϴ� ����
	bool			m_ChargeState;		//��¡ ������?
	int32			m_ChargeLevel;		// ��¡�ܰ躰 ���� , ����������ϱ����� 
	TimerHandle		m_ChargeTimer;
	uint64			m_ChargeMaxTimerID;
	uint64			m_ChargeLevelTimerID;
};

