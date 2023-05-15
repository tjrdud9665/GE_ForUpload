#pragma once
#include <Assets/GameplayAbility/GameplayAbility.h>
#include <Assets/Animation/AnimationSequence.h>
#include "../GameObject/HitBoxObject.h"
#include "../GameObject/ParticleObject.h"

class GameplayAbility_Attack : public GameplayAbility
{
	CLASS(GameplayAbility_Attack, GameplayAbility)

protected:
	GameplayAbility_Attack();
	GameplayAbility_Attack(const GameplayAbility_Attack& Other);
	virtual ~GameplayAbility_Attack();


	virtual void OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec) override;


private:
	virtual void OnActivateAbility(const SGameplayAbilitySpec& InSpec) override;

public:
	virtual void OnEndAbility();
	virtual void OnCancelAbility();

protected:
	void ChangeDirection();



protected:
	PROP(std::vector<TSharedPtr<AnimationSequence>>, Attacks, DetailsEdit)
	std::vector<TSharedPtr<AnimationSequence>> Attacks;						//기본공격에 사용될 Animation들

	
	PROP(float, AnimPlayRate, DetailsEdit)
	float AnimPlayRate;														//공격 재생비율


	PROP(bool, RootMotion, DetailsEdit)
	bool RootMotion;														//RootMotion을 사용할 것인지?


	PROP(TSubclassOf<GameplayEffect>, HitEffect, DetailsEdit)
	TSubclassOf<GameplayEffect>	HitEffect;									//공격에 적중당한 Target에게 적용할 Hit처리


	PROP(TSubclassOf<HitBoxObject>, HitBoxClass, DetailsEdit)
	TSubclassOf<HitBoxObject>	HitBoxClass;								//공격시 공격범위에 소환할 HitBox


	PROP(TSubclassOf<ParticleObject>, ParticleClass, DetailsEdit)
	TSubclassOf<ParticleObject>	ParticleClass;


	PROP(float, SpawnOffset, DetailsEdit)
	float	SpawnOffset;													//HitBox를 소환시 시전자가 바라보는 방향에서 얼마만큼의 offset을 두고 소환할지?


	PROP(bool, ChasingMouse, DetailsEdit)
	bool	ChasingMouse;
};

