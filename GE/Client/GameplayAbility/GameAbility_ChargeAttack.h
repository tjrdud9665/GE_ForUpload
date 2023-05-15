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
	bool		m_ChargeOn; // Charge ���� 
	bool		m_KeyPushCheck; // Ű�Է� Ȯ�� 
	bool		m_KeyUpCheck; // Ű�Է��� �´��� Ȯ��
	bool		m_Max; // Charge Time �ִ�ġ ���� Ȯ�� 


	bool		m_Charge; // ���� ��¡������ Ȯ�� 
	int			m_ChargeLevel; // ��¡�ܰ躰 ���� , ����������ϱ����� 

	TimerHandle	m_ChargeTimer;
	uint64		m_ChargeMaxTimerID;
	uint64		m_ChargeLevelTimerID;

};

