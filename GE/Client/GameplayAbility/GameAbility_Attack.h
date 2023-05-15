#pragma once

#include <Assets/GameplayAbility/GameplayAbility.h>

class GameAbility_Attack : public GameplayAbility
{
	CLASS(GameAbility_Attack, GameplayAbility)

protected:
	GameAbility_Attack();
	GameAbility_Attack(const GameAbility_Attack& Other);
	virtual ~GameAbility_Attack();


	virtual void OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec) override;


private:
	virtual void OnActivateAbility(const SGameplayAbilitySpec& InSpec) override;

	virtual void OnCancelAbility();
	virtual void OnEndAbility();

	virtual void InputPressed(const SGameplayAbilitySpec& InSpec) override;
	virtual void InputReleased(const SGameplayAbilitySpec& InSpec) override;

protected:
	PROP(std::vector<TSharedPtr<class AnimationSequence>>, Attack, DetailsEdit)
		std::vector<TSharedPtr<class AnimationSequence>> Attack;

	PROP(TSubclassOf<GameplayEffect>, AttackSet, DetailsEdit)
		TSubclassOf<GameplayEffect> AttackSet;


};

