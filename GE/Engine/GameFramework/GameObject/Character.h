#pragma once
#include "GameObject.h"
#include <Input/InputHandle.h>
#include <GameplaySystem/GameplaySystem.h>
#include <Assets/Animation/AnimationSequence.h>
#include <Assets/Mesh/Skeleton.h>
#include "../Components/AbilityComponent.h"
#include <GameplaySystem/AttributeSet.h>


class Character : public GameObject, public IAbilitySystem
{
	CLASS(Character, GameObject)
	friend class Controller;

protected:
	Character();
	Character(const Character& Other);
	virtual ~Character();

public:
	virtual void BeginPlay() override;


	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void Update(float DeltaSeconds) override;

	virtual void PostUpdate(float DeltaSeconds);

	virtual void CheckTagStatement();

	void MoveToLocation(const SVector3& InLocation);	

	virtual void OnDamaged(float TotalDamage, class GameObject* DamageCauser);

	void Die();
	virtual void OnDie();

	virtual bool IsDie() const;


	template<typename ControllerType, typename = std::enable_if_t<std::is_base_of<Controller, ControllerType>::value>>
	ControllerType* GetControllerAs()
	{		
		return (ControllerType*)m_Controller;
	}


public:
	virtual void PossessedBy(class Controller* NewController);
	virtual void UnPossessed();

	virtual void SetUpInput(InputHandle& handle);

	class Controller* GetController() const;

	/*Sample Of UserOverride Component(하위클래스에서 컴포넌트를 상속시킨 오브젝트로 확장한다면)*/
	virtual bool PostLoad() override;

public:
	class SkeletalMeshComponent* GetSkeletalMeshComponent() const;

	class CharacterMovementComponent* GetMovementComponent() const;

	virtual void SetWorldLocation(const SVector3& InLocation) override;
	virtual void SetWorldRotation(const SVector3& InRotation) override;
	virtual void SetWorldScale(const SVector3& InScale) override;

	SVector3 GetWorldLocationAsBottom();

	float GetCapsuleHalfLength()	const;

public:
	virtual class AbilityComponent* GetAbilityComponent() const override
	{
		return m_AbilityComponent;
	}
	template<typename AttributeSetType, typename = std::enable_if_t<std::is_base_of<AttributeSet, AttributeSetType>::value>>
	AttributeSetType* GetAttributeSetAs()
	{
		return (AttributeSetType*)m_AttributeInstance.Get();
	}

protected:
	class CapsuleComponent* m_RootCapsule;

	class SkeletalMeshComponent* m_SkeletalMeshComponent;

	class CharacterMovementComponent* m_CharacterMovement;

	class AbilityComponent*	m_AbilityComponent;

protected:
	class Controller* m_Controller;		//If Monster -> AIController
										//if Player - >PlayerController



protected:
	PROP(TSubclassOf<AttributeSet>, m_AttributeSetClass, DetailsEdit,"Gameplay")
		TSubclassOf<AttributeSet>	m_AttributeSetClass;

	//RuntimeOnly.
	PROP(TSharedPtr<AttributeSet>, m_AttributeInstance, Transient, "Gameplay")
		TSharedPtr<AttributeSet>	m_AttributeInstance;


	PROP(TSubclassOf<class AbilityComponent>, AbilityComponentClass, DetailsEdit, "Gameplay")
		TSubclassOf<class AbilityComponent> AbilityComponentClass;



protected:
	//* 몬/플 공통 동작 
	// 
	PROP(TSharedPtr<class AnimationSequence>, Idle, DetailsEdit, "Animation")
		TSharedPtr<class AnimationSequence>		Idle;

	PROP(TSharedPtr<class AnimationSequence>, Walk, DetailsEdit, "Animation")
		TSharedPtr<class AnimationSequence>		Walk;



};

