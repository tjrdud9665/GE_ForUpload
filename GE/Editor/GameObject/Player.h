#pragma once
#include "GameFramework\GameObject\PlayerCharacter.h"
#include "IInteractableInterface.h"

class Player : public PlayerCharacter, public IInteractableInterface
{
	CLASS(Player, PlayerCharacter)
protected:
	Player();
	Player(const Player& Other);
	virtual ~Player();


protected:
	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void SetUpInput(InputHandle& handle) override;

	virtual void CheckTagStatement() override;

	virtual void OnDamaged(float TotalDamage, class GameObject* DamageCauser) override;

	virtual void BeginPlay() override;

	virtual void EndPlay() override;

public:
	virtual void OnDie() override;

	virtual bool IsDie() const override;


public:
	/* Interface of Interactable ~~*/
	virtual bool IsInteractable()												  const override;

	virtual void OnBeginInteractable(class IInteractableInterface* InteractionObject)	override;

	virtual void OnEndInteractable()													override;

	virtual void OnInteraction()														override;

private:
	void ClickToMove(const std::string& BindName, uint64 ID);	// 마우스 이동
	

public:
	TSubclassOf<class GameplayAbility> GetSkill1()	const { return m_Skill1; }
	TSubclassOf<class GameplayAbility> GetSkill2()	const { return m_Skill2; }
	TSubclassOf<class GameplayAbility> GetSkill3()	const { return m_Skill3; }
	TSubclassOf<class GameplayAbility> GetSkill4()	const { return m_Skill4; }

protected:
	class StaticMeshComponent* m_PlayerWeapon;

	class CharacterAttributeSet* m_CharacterAttributeSet;

	//초기값 적용시킬거
	PROP(TSubclassOf<class GameplayEffect>, m_InitStatEffect, DetailsEdit, "Gameplay")
		TSubclassOf<class GameplayEffect>	m_InitStatEffect;


// Player Abilities
protected:
	PROP(TSubclassOf<class GameplayAbility>, m_DefaultAttack, DetailsEdit, "Gameplay")
		TSubclassOf<class GameplayAbility>	m_DefaultAttack;

	PROP(TSubclassOf<class GameplayAbility>, m_Skill1, DetailsEdit, "Gameplay")
		TSubclassOf<class GameplayAbility>	m_Skill1;

	PROP(TSubclassOf<class GameplayAbility>, m_Skill2, DetailsEdit, "Gameplay")
		TSubclassOf<class GameplayAbility>	m_Skill2;

	PROP(TSubclassOf<class GameplayAbility>, m_Skill3, DetailsEdit, "Gameplay")
		TSubclassOf<class GameplayAbility>	m_Skill3;

	PROP(TSubclassOf<class GameplayAbility>, m_Skill4, DetailsEdit, "Gameplay")
		TSubclassOf<class GameplayAbility>	m_Skill4;

	PROP(TSubclassOf<class GameplayAbility>, m_Evade, DetailsEdit, "Gameplay")
		TSubclassOf<class GameplayAbility>	m_Evade;

// Player Sequences
protected:
	// 쳐맞
	PROP(TSharedPtr<class AnimationSequence>, Hit, DetailsEdit, "Animation")
		TSharedPtr<class AnimationSequence>		Hit;

	// 씨게 쳐맞(얘는 Loop X)
	PROP(TSharedPtr<class AnimationSequence>, KnockDown, DetailsEdit, "Animation")
		TSharedPtr<class AnimationSequence>		KnockDown;

	// 씨게 쳐맞고 일어남(씨게 쳐맞고 누워있는 상태에서 입력 아무거나 받으면 일어남)
	PROP(TSharedPtr<class AnimationSequence>, Getup, DetailsEdit, "Animation")
		TSharedPtr<class AnimationSequence>		Getup;

	// 사망
	PROP(TSharedPtr<class AnimationSequence>, Death, DetailsEdit, "Animation")
		TSharedPtr<class AnimationSequence>		Death;

protected:
	bool					m_IsInteraction;
	IInteractableInterface* m_InteractionObject;
};

