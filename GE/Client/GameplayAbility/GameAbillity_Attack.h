#pragma once

#include <Assets/GameplayAbility/GameplayAbility.h>

class GameAbillity_Attack : public GameplayAbility
{
	CLASS(GameAbillity_Attack, GameplayAbility)


protected:
	GameAbillity_Attack();
	GameAbillity_Attack(const GameAbillity_Attack& Other);
	virtual ~GameAbillity_Attack();


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

	PROP(TSubclassOf<GameplayEffect>, BuffTest, DetailsEdit)
		TSubclassOf<GameplayEffect> BuffTest;


};

