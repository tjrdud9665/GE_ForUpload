#pragma once
#include <GameFramework/GameObject/Character.h>
#include <GameFramework/GameObject/AIController.h>

class Monster : public Character
{
	CLASS(Monster, Character)
protected:
	Monster();
	Monster(const Monster& Other);
	virtual ~Monster();

public:
	virtual void BeginPlay() override;

	virtual void EndPlay() override;


	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void PostUpdate(float DeltaSeconds) override;

	virtual void CheckTagStatement() override;

	virtual void OnDamaged(float TotalDamage, class GameObject* DamageCauser) override;


	virtual void OnDie() override;

	virtual bool IsDie() const override;

protected:
	PROP(TSubclassOf<AIController>, AIControllerClass, DetailsEdit)
	TSubclassOf<AIController> AIControllerClass;


protected:
	class StaticMeshComponent* m_MonsterWeapon;

	class CharacterAttributeSet* m_CharacterAttributeSet;

//�ʱⰪ �����ų��
	PROP(TSubclassOf<class GameplayEffect>, m_InitStatEffect, DetailsEdit, "Gameplay")
	TSubclassOf<class GameplayEffect>	m_InitStatEffect;

// Monster Abilities
protected:
	PROP(TSubclassOf<class GameplayAbility>, m_DefaultAttack, DetailsEdit, "Gameplay")
	TSubclassOf<class GameplayAbility>	m_DefaultAttack;

	// �⺻���ݹ���
	PROP(float, m_DefaultAttackRange, DetailsEdit)
	float m_DefaultAttackRange;

// Monster Sequences
protected:
	// �ĸ�
	PROP(TSharedPtr<class AnimationSequence>, Hit, DetailsEdit, "Animation")
	TSharedPtr<class AnimationSequence>		Hit;

	// ���� �ĸ�(��� Loop X)
	PROP(TSharedPtr<class AnimationSequence>, KnockDown, DetailsEdit, "Animation")
	TSharedPtr<class AnimationSequence>		KnockDown;

	// ���� �ĸ°� �Ͼ(���� �ĸ°� �����ִ� ���¿��� �Է� �ƹ��ų� ������ �Ͼ)
	PROP(TSharedPtr<class AnimationSequence>, Getup, DetailsEdit, "Animation")
	TSharedPtr<class AnimationSequence>		Getup;

	// ���
	PROP(TSharedPtr<class AnimationSequence>, Death, DetailsEdit, "Animation")
	TSharedPtr<class AnimationSequence>		Death;

protected:
	// Player Ž������
	PROP(float, m_DetectRange, DetailsEdit)
	float m_DetectRange;

	// BeginPlay�� ��������
	SVector3	m_StartedLocation;
};

DECLARE_REFLECTION_TYPE(Monster)