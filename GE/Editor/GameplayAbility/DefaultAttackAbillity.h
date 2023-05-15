#pragma once

#include <Assets/GameplayAbility/GameplayAbility.h>

class DefaultAttackAbillity : public GameplayAbility
{
	CLASS(DefaultAttackAbillity, GameplayAbility)

protected:
	DefaultAttackAbillity();
	DefaultAttackAbillity(const DefaultAttackAbillity& Other);
	virtual ~DefaultAttackAbillity();


	virtual void OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec) override;


private:
	virtual void OnActivateAbility(const SGameplayAbilitySpec& InSpec) override;

	virtual void OnCancelAbility();
	virtual void OnEndAbility();

	virtual void InputPressed(const SGameplayAbilitySpec& InSpec);


protected:
	PROP(TSubclassOf<GameplayEffect>, BuffTest, DetailsEdit)
		TSubclassOf<GameplayEffect> BuffTest;

	PROP(std::vector<TSharedPtr<class AnimationSequence>>, DefaultAttack, DetailsEdit)
		std::vector<TSharedPtr<class AnimationSequence>> DefaultAttack;


	int m_ComboCount;
	bool m_CombeEnable;

	int m_TestCount;


};

