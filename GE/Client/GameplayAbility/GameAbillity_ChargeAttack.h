#pragma once

#include "GameAbillity_Attack.h"
#include "Timer/TimerHandle.h"

class GameAbillity_ChargeAttack : public GameAbillity_Attack
{
	CLASS(GameAbillity_ChargeAttack, GameAbillity_Attack)

protected:
	GameAbillity_ChargeAttack();
	GameAbillity_ChargeAttack(const GameAbillity_ChargeAttack& Other);
	virtual ~GameAbillity_ChargeAttack();


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

