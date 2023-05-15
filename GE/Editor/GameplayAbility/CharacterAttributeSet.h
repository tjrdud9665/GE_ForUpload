#pragma once
#include "GameplaySystem\AttributeSet.h"

class CharacterAttributeSet : public AttributeSet
{
	CLASS(CharacterAttributeSet, AttributeSet)

protected:
	CharacterAttributeSet();
	CharacterAttributeSet(const CharacterAttributeSet& Other);
	virtual ~CharacterAttributeSet();


protected:
	PROP(SGameplayAttributeData, Health, DetailsEdit)
		SGameplayAttributeData	Health;

	PROP(SGameplayAttributeData, MaxHealth, DetailsEdit)
		SGameplayAttributeData	MaxHealth;

	PROP(SGameplayAttributeData, MoveSpeed, DetailsEdit)
		SGameplayAttributeData	MoveSpeed;

	//���ݷ�
	PROP(SGameplayAttributeData, AttackPower, DetailsEdit)
		SGameplayAttributeData	AttackPower;

	//����
	PROP(SGameplayAttributeData, DefensePower, DetailsEdit)
		SGameplayAttributeData	DefensePower;

	//���� ������
	PROP(SGameplayAttributeData, Damage, Transient)
		SGameplayAttributeData	Damage;


public:
	float GetHealth() const;

	float GetMaxHealth() const;

	float GetMoveSpeed() const;

	float GetAttackPower() const;

	float GetDamage()	const;

	float GetDefensePower()	const;

public:
	virtual void PostGameplayEffectExecute(class GameplayEffect* InDef, class AbilityComponent* Src, class AbilityComponent* Target) override;

	virtual void PreAttributeChange(const SGameplayAttribute& Attribute, float& NewValue) override;
};

