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
	TSubclassOf<GameplayEffect>	LastHitEffect;								//마지막 공격에 적중당한 Target에게 적용할 Hit처리


	PROP(TSubclassOf<class HitBoxObject>, LastHitBoxClass, DetailsEdit)
	TSubclassOf<class HitBoxObject>	LastHitBoxClass;						//마지막 공격 시 공격범위에 소환할 HitBox


	PROP(float, LastSpawnOffset, DetailsEdit)
	float	LastSpawnOffset;												//HitBox를 소환시 시전자가 바라보는 방향에서 얼마만큼의 offset을 두고 소환할지?


	int32	m_ComboCount;

	bool	m_ComboEnable;
};

