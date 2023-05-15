#pragma once

#include <Assets/GameplayAbility/GameplayAbility.h>

class GameAbility_Evade : public GameplayAbility
{
	CLASS(GameAbility_Evade, GameplayAbility)

protected:
	GameAbility_Evade();
	GameAbility_Evade(const GameAbility_Evade& Other);
	virtual ~GameAbility_Evade();


	virtual void OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec) override;


private:
	virtual void OnActivateAbility(const SGameplayAbilitySpec& InSpec) override;

	virtual void OnCancelAbility();
	virtual void OnEndAbility();

	virtual void InputPressed(const SGameplayAbilitySpec& InSpec) override;
	virtual void InputReleased(const SGameplayAbilitySpec& InSpec) override;

protected:
	PROP(std::vector<TSharedPtr<class AnimationSequence>>, EvadeAnims, DetailsEdit)
		std::vector<TSharedPtr<class AnimationSequence>> EvadeAnims;

};

