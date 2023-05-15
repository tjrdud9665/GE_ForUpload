#pragma once
#include "Monster.h"

class BossMonster : public Monster
{
	CLASS(BossMonster, Monster)

protected:
	BossMonster();
	BossMonster(const BossMonster& Other);
	virtual ~BossMonster();

public:
	virtual void BeginPlay() override;

	virtual void CheckTagStatement() override;

	virtual void OnDamaged(float TotalDamage, class GameObject* DamageCauser) override;

	virtual void OnDie() override;

protected:
	void ToEndingScene();

// Monster Sequences
protected:
	// ÃÄ¸Â
	PROP(TSharedPtr<class AnimationSequence>, Groggy, DetailsEdit, "Animation")
	TSharedPtr<class AnimationSequence>	Groggy;

	// ¶Ù´ó
	PROP(TSharedPtr<class AnimationSequence>, Run, DetailsEdit, "Animation")
	TSharedPtr<class AnimationSequence>	Run;

// BossMonster Abilities
protected:
	PROP(TSubclassOf<class GameplayAbility>, m_Skill1, DetailsEdit, "Gameplay")
	TSubclassOf<class GameplayAbility>	m_Skill1;
		
	PROP(float, m_Skill1Range, DetailsEdit)
	float m_Skill1Range;

	PROP(TSubclassOf<class GameplayAbility>, m_Skill2, DetailsEdit, "Gameplay")
	TSubclassOf<class GameplayAbility>	m_Skill2;

	PROP(float, m_Skill2Range, DetailsEdit)
	float m_Skill2Range;

	PROP(TSubclassOf<class GameplayAbility>, m_Skill3, DetailsEdit, "Gameplay")
	TSubclassOf<class GameplayAbility>	m_Skill3;

	PROP(float, m_Skill3Range, DetailsEdit)
	float m_Skill3Range;

protected:
	PROP(TSubclassOf<class UserWidget>, BossHPWidgetClass, DetailsEdit)
	TSubclassOf<class UserWidget>		BossHPWidgetClass;

	class UserWidget* m_BossHPWidgetInstance;

protected:
	PROP(TSharedPtr<class Scene>, EndingScene, DetailsEdit)
	TSharedPtr<class Scene>	EndingScene;


	uint64	m_ToEndingSceneID;
};

DECLARE_REFLECTION_TYPE(BossMonster)