#pragma once

#include <Assets/GameplayAbility/GameplayAbility.h>

class PlayerSkillAbilliy : public GameplayAbility
{

	CLASS(PlayerSkillAbilliy, GameplayAbility)

protected:
	PlayerSkillAbilliy();
	PlayerSkillAbilliy(const PlayerSkillAbilliy& Other);
	virtual ~PlayerSkillAbilliy();


	virtual void OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec) override;


private:
	virtual void OnActivateAbility(const SGameplayAbilitySpec& InSpec) override;

	virtual void OnCancelAbility();
	virtual void OnEndAbility();

	virtual void InputPressed(const SGameplayAbilitySpec& InSpec);


protected:
	PROP(TSubclassOf<GameplayEffect>, BuffTest, DetailsEdit)
		TSubclassOf<GameplayEffect> BuffTest;

	PROP(std::vector<TSharedPtr<class AnimationSequence>>, SkillAnims, DetailsEdit)
		std::vector<TSharedPtr<class AnimationSequence>> SkillAnims;

};

