#pragma once
#include <GameplaySystem/AttributeSet.h>


//�����Դϴ�. �����ؼ� �������� ���÷μ��� ������

class SampleAttributeSet :public AttributeSet
{
	CLASS(SampleAttributeSet,AttributeSet)

protected:
	SampleAttributeSet();
	SampleAttributeSet(const SampleAttributeSet& Other);
	virtual ~SampleAttributeSet();


protected:
	PROP(SGameplayAttributeData, Health, DetailsEdit)
		SGameplayAttributeData	Health;

	PROP(SGameplayAttributeData, MaxHealth, DetailsEdit)
		SGameplayAttributeData	MaxHealth;

	PROP(SGameplayAttributeData, MoveSpeed, DetailsEdit)
		SGameplayAttributeData	MoveSpeed;

	PROP(SGameplayAttributeData, Damage, Transient)
		SGameplayAttributeData	Damage;

	PROP(SGameplayAttributeData, AttackPower, DetailsEdit)
		SGameplayAttributeData	AttackPower;
	//����
	PROP(SGameplayAttributeData, DefensePower, DetailsEdit)
		SGameplayAttributeData	DefensePower;

public:
	virtual void PreAttributeChange(const SGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(class GameplayEffect* InDef, class AbilityComponent* Src, class AbilityComponent* Target) override;
};


