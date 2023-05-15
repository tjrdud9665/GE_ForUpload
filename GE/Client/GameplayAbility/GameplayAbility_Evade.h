#pragma once
#include "Assets\GameplayAbility\GameplayAbility.h"

class GameplayAbility_Evade : public GameplayAbility
{
	CLASS(GameplayAbility_Evade, GameplayAbility)

protected:
	GameplayAbility_Evade();
	GameplayAbility_Evade(const GameplayAbility_Evade& Other);
	virtual ~GameplayAbility_Evade();


	virtual void OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec) override;


private:
	virtual void OnActivateAbility(const SGameplayAbilitySpec& InSpec) override;

public:
	virtual void OnEndAbility();


protected:
	void ChangeDirection();

protected:
	PROP(TSharedPtr<class AnimationSequence>, Evade, DetailsEdit)
	TSharedPtr<class AnimationSequence> Evade;


	PROP(float, AnimPlayRate, DetailsEdit)
	float AnimPlayRate;


	PROP(bool, ChasingMouse, DetailsEdit)
	bool	ChasingMouse;
};

