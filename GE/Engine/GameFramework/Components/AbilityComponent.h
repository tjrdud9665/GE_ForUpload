#pragma once
#include "ObjectComponent.h"
#include <GameplaySystem/AttributeSet.h>
#include <Assets/GameplayEffect/GameplayEffect.h>
#include <Assets/GameplayAbility/GameplayAbility.h>
#include <Input/InputHandle.h>

#define INFINITE_DURATION -1.0f


struct SGameplayEffectContext
{

	class GameObject* Instigator;//����Ʈ�� ȿ���� �ߵ���Ų AbilityComp�� ����
	class AbilityComponent* InstigatorAbilityComponent;
	class GameObject* EffectCauser;	//�̻��� Ȥ�� ������ ��ų ������Ʈ	
	//class GameplayAbility* AbilityCDO;
	class AbilityComponent* TargetComp;

	int32 AbilityLevel;
	SVector3	WorldOrigin;
};



struct SGameplayEffectSpec
{
	GameplayEffect* Def;
	SGameplayEffectContext Context;

	uint64	Handle;

	float Duration;

	float Period;

	std::vector<SGameplayEffectModifiedAttribute>	AppliedMod;



	SGameplayEffectSpec(GameplayEffect* InDef, const SGameplayEffectContext& _context)
		:Def(InDef)
		, Context(_context)
		, Handle(0)
		, Duration(0.f)
		, Period(0.0f)

	{
		if (Def)
		{
			if (Def->DurationPolicy.Get() == EGameplayEffectDurationType::HasDuration)
			{
				Duration = Def->DurationMagnitude;
				Period = Def->Period;
			} else
			{
				Duration = INFINITE_DURATION;//Infinite
				Period = Def->Period;
			}
		}

	}
	~SGameplayEffectSpec()
	{
		SAFE_DELETE(Def);
	}
	//��Ÿ�ӿ��� ���氡�ɿ���?
	//void SetDuration(float NewDuration);

	float GetDuration() const
	{
		return Duration;
	}
	float GetPeriod() const
	{
		return Period;
	}
};



class AbilityComponent :public ObjectComponent
{
	CLASS(AbilityComponent,ObjectComponent)

protected:
	AbilityComponent();
	AbilityComponent(const AbilityComponent& Other);
	virtual ~AbilityComponent();

protected:
	virtual void BeginPlay() override;

	virtual void Update(float DeltaSeconds) override;

	virtual void PostUpdate(float DeltaSeconds) override;

public:
	bool GiveAbility(TSubclassOf<GameplayAbility> InGiveAbility);

	bool TryActivateAbilityByClass(TSubclassOf<GameplayAbility> InAbilityToActivate);

	class GameplayAbility* GetAbilityInsance(TSubclassOf<GameplayAbility> InAbilityClass);

	void ModifyGameplayAbilitySpecInfo(class GameplayAbility* InAbilityInst, const SGameplayAbilityObjectInfo& Info);


	virtual void SetUpInput(InputHandle& handle);

private:
	void AbilityInputPressed(const std::string& ActionName, uint64 pressedID);
	void AbilityInputReleased(const std::string& ActionName, uint64 releasedID);

public:
	virtual void HandleGameplayEvent(const std::string& EvenName, SGameplayEventData*& EventData);

	bool CanApplyAttributeModifiers(TSubclassOf<GameplayEffect> CostGameplayEffectClass);

public:
	//AttributeHandling.
	float GetAttribute(const std::string& InAttributeName);

public:
	float GetEffectRemainingTime(uint64 InHandle);

	int32 GetActivatingAbility() const;

	AttributeSet* GetAttributeSetFromClass(class AttributeSet* src);

public:
	SGameplayEffectSpec* ApplyGameplayEffectToTarget(TSubclassOf<GameplayEffect> GameplayEffectClass, AbilityComponent* Target);

	SGameplayEffectSpec* ApplyGameplayEffectToSelt(TSubclassOf<GameplayEffect> GameplayEffectClass);



protected:
	std::list<SGameplayEffectSpec*>	m_ActiveGameplayEffectList;

	std::list<AttributeSet*>		m_AttirbuteSets;





protected:
	//��밡���� Abilities..
	std::vector<SGameplayAbilitySpec*> m_ActivatableAbilities;

	std::function<void(const std::string&, const SGameplayEventData&)>	m_OnEventHandled;


	uint64			m_EffectCounter;


	InputHandle* m_InputHandle;
};

