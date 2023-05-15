#pragma once
#include "Assets\GameplayAbility\GameplayAbility.h"

class GameplayAbility_DeafaultAttack : public GameplayAbility
{
	CLASS(GameplayAbility_DeafaultAttack, GameplayAbility)

protected:
	GameplayAbility_DeafaultAttack();
	GameplayAbility_DeafaultAttack(const GameplayAbility_DeafaultAttack& Other);
	virtual ~GameplayAbility_DeafaultAttack();


	virtual void OnReceivedGameplayEvent(const std::string& EventName, SGameplayEventData*& EventData, const SGameplayAbilitySpec& InSpec) override;


private:
	virtual void OnActivateAbility(const SGameplayAbilitySpec& InSpec) override;


	virtual void OnEndAbility() override;
	virtual void InputPressed(const SGameplayAbilitySpec& InSpec) override;



protected:
	PROP(std::vector<TSharedPtr<class AnimationSequence>>, Combos, DetailsEdit)
		std::vector < TSharedPtr<class AnimationSequence>> Combos;				//�⺻���� �޺��� ���� Animation��

	int32	m_ComboCount;

	bool	m_ComboEnable;


	PROP(TSharedPtr<class AnimationSequence>, NormalCollisionObject, DetailsEdit)
		TSharedPtr<class AnimationSequence> NormalCollisionObject;				//�⺻���� �� ������ �浹ü Object

	PROP(TSharedPtr<class AnimationSequence>, FinalCollisionObject, DetailsEdit)
		TSharedPtr<class AnimationSequence> FinalCollisionObject;				//�⺻������ ������ �޺��� ������ �浹ü Object

};

