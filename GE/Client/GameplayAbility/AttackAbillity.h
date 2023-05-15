#pragma once

#include <Assets/GameplayAbility/GameplayAbility.h>

class AttackAbillity : public GameplayAbility
{
	CLASS(AttackAbillity, GameplayAbility)


protected:
	AttackAbillity();
	AttackAbillity(const AttackAbillity& Other);
	virtual ~AttackAbillity();


	virtual void OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec) override;


private:
	virtual void OnActivateAbility(const SGameplayAbilitySpec& InSpec) override;

	virtual void OnCancelAbility();
	virtual void OnEndAbility();

protected:
	PROP(std::vector<TSharedPtr<class AnimationSequence>>, Attack, DetailsEdit)
		std::vector<TSharedPtr<class AnimationSequence>> Attack;

	PROP(TSubclassOf<GameplayEffect>, BuffTest, DetailsEdit)
		TSubclassOf<GameplayEffect> BuffTest;


};

