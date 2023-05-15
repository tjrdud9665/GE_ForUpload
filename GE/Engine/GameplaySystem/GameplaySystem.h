#pragma once
#include "../Reflection/ReflectionTypeTraits.h"

ENUM(EGameplayModOp, Additive = 0, Multiplicitive, Division, Override, Max);

ENUM(EGameplayEffectDurationType, Instant = 0, Infinite, HasDuration);

ENUM(EGameplayModCalcType, StaticFloat = 0, AttributeBased);

ENUM(EAttributeSource, Source = 0, Target);

ENUM(EGameplayAbilityInstancingPolicy, NonInstanced = 0, InstancedPerObject, InstancedPerExecution);


struct SGameplayAttributeData
{
	float BaseValue;
	float CurrentValue;

	SGameplayAttributeData()
		: BaseValue(0.f)
		, CurrentValue(0.f)
	{}
	SGameplayAttributeData(float DefaultValue)
		: BaseValue(DefaultValue)
		, CurrentValue(DefaultValue)
	{

	}
	float GetCurrentValue() const
	{
		return CurrentValue;
	}
	void SetCurrentValue(float NewValue)
	{
		CurrentValue = NewValue;
	}
	float GetBaseValue() const
	{
		return BaseValue;
	}
	void SetBaseValue(float NewValue)
	{
		BaseValue = NewValue;
	}
};

DECLARE_REFLECTION_TYPE(SGameplayAttributeData);
DECLARE_REFLECTION_TYPE(struct SGameplayAttribute);

DECLARE_REFLECTION_TYPE(class AttributeModifer*);

DECLARE_SHARED_REFLECTION_TYPE(class AttributeSet);

struct SGameplayEventData
{
	class GameObject* Instigator;
	class GameObject* Target;

	SVector3		Position;
	//float			FloatData;
	std::string		EventName;

};

class IAbilitySystem
{
public:
	virtual class AbilityComponent* GetAbilityComponent() const PURE;

};


class GameplaySystem
{
public:
	static void SendGameplayEvent(class GameObject* ToObject, const std::string& EventName, SGameplayEventData*& Data);

	static float GetAttributeValue(class GameObject* AttributeOwner, const std::string& AttributeName);


};
