#pragma once
#include "GameplayAbility_ChargeAttack.h"

class GameplayAbility_ChargeAttack_HitCancel : public GameplayAbility_ChargeAttack
{
	CLASS(GameplayAbility_ChargeAttack_HitCancel, GameplayAbility_ChargeAttack)

protected:
	GameplayAbility_ChargeAttack_HitCancel();
	GameplayAbility_ChargeAttack_HitCancel(const GameplayAbility_ChargeAttack_HitCancel& Other);
	virtual ~GameplayAbility_ChargeAttack_HitCancel();


	virtual void OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec) override;

public:
	virtual void OnCancelAbility();
	virtual void OnEndAbility();

protected:
	PROP(int32, MaxHitCount, DetailsEdit)
	int32 MaxHitCount;

	int32 m_HitCount;
};

