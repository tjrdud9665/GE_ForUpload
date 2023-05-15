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
	std::vector<TSharedPtr<AnimationSequence>> Attacks;						//�⺻���ݿ� ���� Animation��

	
	PROP(float, AnimPlayRate, DetailsEdit)
	float AnimPlayRate;														//���� �������


	PROP(bool, RootMotion, DetailsEdit)
	bool RootMotion;														//RootMotion�� ����� ������?


	PROP(TSubclassOf<GameplayEffect>, HitEffect, DetailsEdit)
	TSubclassOf<GameplayEffect>	HitEffect;									//���ݿ� ���ߴ��� Target���� ������ Hitó��


	PROP(TSubclassOf<HitBoxObject>, HitBoxClass, DetailsEdit)
	TSubclassOf<HitBoxObject>	HitBoxClass;								//���ݽ� ���ݹ����� ��ȯ�� HitBox


	PROP(TSubclassOf<ParticleObject>, ParticleClass, DetailsEdit)
	TSubclassOf<ParticleObject>	ParticleClass;


	PROP(float, SpawnOffset, DetailsEdit)
	float	SpawnOffset;													//HitBox�� ��ȯ�� �����ڰ� �ٶ󺸴� ���⿡�� �󸶸�ŭ�� offset�� �ΰ� ��ȯ����?


	PROP(bool, ChasingMouse, DetailsEdit)
	bool	ChasingMouse;
};

