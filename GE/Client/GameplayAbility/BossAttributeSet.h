#pragma once
#include "CharacterAttributeSet.h"

class BossAttributeSet : public CharacterAttributeSet
{
	CLASS(BossAttributeSet, CharacterAttributeSet)

protected:
	BossAttributeSet();
	BossAttributeSet(const BossAttributeSet& Other);
	virtual ~BossAttributeSet();

public:
	virtual void PostGameplayEffectExecute(class GameplayEffect* InDef, class AbilityComponent* Src, class AbilityComponent* Target) override;

private:
	bool m_isPhase1;
	bool m_isPhase2;
	bool m_isPhase3;
};

