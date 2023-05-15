#pragma once
#include "GameplayAbility_Attack.h"

class GameplayAbility_ComboAttack : public GameplayAbility_Attack
{
	CLASS(GameplayAbility_ComboAttack, GameplayAbility_Attack)

protected:
	GameplayAbility_ComboAttack();
	GameplayAbility_ComboAttack(const GameplayAbility_ComboAttack& Other);
	virtual ~GameplayAbility_ComboAttack();


	virtual void OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec) override;

private:
	//virtual void OnActivateAbility(const SGameplayAbilitySpec& InSpec) override;

public:
	virtual void OnEndAbility() override;
	virtual void InputPressed(const SGameplayAbilitySpec& InSpec) override;

protected:
	PROP(TSubclassOf<GameplayEffect>, LastHitEffect, DetailsEdit)
	TSubclassOf<GameplayEffect>	LastHitEffect;								//������ ���ݿ� ���ߴ��� Target���� ������ Hitó��


	PROP(TSubclassOf<class HitBoxObject>, LastHitBoxClass, DetailsEdit)
	TSubclassOf<class HitBoxObject>	LastHitBoxClass;						//������ ���� �� ���ݹ����� ��ȯ�� HitBox


	PROP(float, LastSpawnOffset, DetailsEdit)
	float	LastSpawnOffset;												//HitBox�� ��ȯ�� �����ڰ� �ٶ󺸴� ���⿡�� �󸶸�ŭ�� offset�� �ΰ� ��ȯ����?


	int32	m_ComboCount;

	bool	m_ComboEnable;
};

