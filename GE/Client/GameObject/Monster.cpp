#include "PCH.h"
#include "Monster.h"
#include "../GameplayAbility/CharacterAttributeSet.h"
#include <GameFramework/Components/CharacterMovementComponent.h>
#include <GameFramework/Components/SkeletalMeshComponent.h>
#include <GameFramework/Components/StaticMeshComponent.h>
#include <ObjectManager.h>
#include "../SceneInfo/CustomSceneInfo.h"

Monster::Monster()
	: m_DefaultAttackRange(150.f)
	, m_DetectRange(500.f)
	//, m_PatrolRange(300.f)
{
}

Monster::Monster(const Monster& Other)
	: Super(Other)
	, AIControllerClass(Other.AIControllerClass)
	, m_InitStatEffect(Other.m_InitStatEffect)
	, m_DefaultAttack(Other.m_DefaultAttack)
	, Hit(Other.Hit)
	, KnockDown(Other.KnockDown)
	, Getup(Other.Getup)
	, Death(Other.Death)
	//, m_MonsterType(Other.m_MonsterType)
	, m_DefaultAttackRange(Other.m_DefaultAttackRange)
	, m_DetectRange(Other.m_DetectRange)
	//, m_PatrolRange(Other.m_PatrolRange)
{
	m_MonsterWeapon = FindComponent<StaticMeshComponent>("MonsterWeapon");
}

Monster::~Monster()
{
}

void Monster::BeginPlay()
{
	Super::BeginPlay();

	// ���͸��� AIController�� ������.
	m_Controller = g_Engine->Get<ObjectManager>()->SpawnGameObject<AIController>("AIController", AIControllerClass);
	m_Controller->Possess(this);

	if (!m_CharacterAttributeSet)
		m_CharacterAttributeSet = (CharacterAttributeSet*)(m_AttributeInstance.Get());

	m_AbilityComponent->ApplyGameplayEffectToTarget(m_InitStatEffect, m_AbilityComponent);

	m_AbilityComponent->GiveAbility(m_DefaultAttack);

	m_StartedLocation = GetWorldLocationAsBottom();

	AIController* aiCon = GetControllerAs<AIController>();

	aiCon->SetBlackboardValueAsFloat("DetectRange", m_DetectRange);
	aiCon->SetBlackboardValueAsAbility("DefaultAttack", m_DefaultAttack);
	aiCon->SetBlackboardValueAsFloat("DefaultAttackRange", m_DefaultAttackRange);
	aiCon->SetBlackboardValueAsVector("StartedLocation", m_StartedLocation);
}

void Monster::EndPlay()
{
}

void Monster::Construct(const std::string& InName)
{
	Super::Construct(InName);

	m_MonsterWeapon = CreateComponent<StaticMeshComponent>("MonsterWeapon");

	GetSkeletalMeshComponent()->AddChild(m_MonsterWeapon);
}

void Monster::PostUpdate(float DeltaSeconds)
{
	Super::PostUpdate(DeltaSeconds);
}

void Monster::CheckTagStatement()
{
	bool isDeath = false;
	FindTag("Death", OUT isDeath);

	// �׾��� ���(Tag�� �� Character�� ��ӹ��� Ŭ�������� �������� IsDie�� �˻�)
	if (isDeath && IsDie())
	{
		AIController* aiCon = GetControllerAs<AIController>();

		if(aiCon)
			aiCon->SetBlackboardValueAsBool("Die", isDeath);


		// ���� �� ���
		AddTag("Death", true);
		AddTag("CantMove", true);
		AddTag("Idle", false);
		AddTag("Walk", false);
		AddTag("CanAbility", false);
		AddTag("Hit", false);
		AddTag("KnockDown", false);
		AddTag("Getup", false);
		return;
	}
	else
	{
		AddTag("Death", false);
	}


	bool isHit = false;
	bool isKnockDown = false;
	bool isGetup = false;
	bool wantIdle = false;
	bool wantWalk = false;

	// ���� Ȱ��ȭ�� Ability�� �����?
	int32 AbilityCount = GetAbilityComponent()->GetActivatingAbility();


	// ���� ������ ���ߴ���?
	FindTag("Hit", OUT isHit);

	// ���� �� ������ ���ؼ� ��������?
	FindTag("KnockDown", OUT isKnockDown);

	// �����ٰ� �Ͼ�� �ϴ���?
	FindTag("Getup", OUT isGetup);



	AddTag("Idle", false);
	AddTag("Walk", false);

	// KnockDown�� ���ߴٸ�
	if (isKnockDown)
	{
		AddTag("KnockDown", true);
		AddTag("CantMove", true);
		AddTag("CanAbility", false);
		AddTag("Hit", false);
		AddTag("SuperArmor", false);
		AddTag("Getup", false);
	}

	// ���� ���¿��� �Ͼ�� �Ѵٸ�
	else if (isGetup)
	{
		AddTag("CantMove", true);
		AddTag("KnockDown", false);
		AddTag("CanAbility", false);
		AddTag("Hit", false);
		AddTag("SuperArmor", false);

		GetSkeletalMeshComponent()->SetAnimationSequence(Getup);
		GetSkeletalMeshComponent()->SetLooping(false);
		GetSkeletalMeshComponent()->SetEnableRootMotion(false);
		GetSkeletalMeshComponent()->Play();

		// Getup Animation���� �������� Getup Tag false�� �ּ���!
	}

	else if (isHit)
	{
		AddTag("CanAbility", false);
		AddTag("CantMove", true);
	}

	// Ȱ��ȭ�� Ability�� �ִٸ�
	else if (AbilityCount)
	{
		AddTag("CanAbility", false);
		AddTag("CantMove", true);
		AddTag("Hit", false);
	}

	// �����Ƽ�� �Ⱦ��� �������µ� �ƴϰ� �ƹ��͵� �ƴ϶��
	else
	{
		AddTag("CantMove", false);
		AddTag("CanAbility", true);


		bool DesiredMovement = GetMovementComponent()->IsPathFollowing();

		// �̵��ϰ��� �Ѵٸ�
		if (DesiredMovement)
		{
			AddTag("Walk", true);
			AddTag("Idle", false);
		}
		else
		{
			AddTag("Walk", false);
			AddTag("Idle", true);
		}
	}


	// ���������� Idle�̳� Walk ������ ���
	FindTag("Idle", OUT wantIdle);
	FindTag("Walk", OUT wantWalk);

	if (wantWalk)
	{
		GetSkeletalMeshComponent()->SetAnimationSequence(Walk);
		GetSkeletalMeshComponent()->SetLooping(true);
		GetSkeletalMeshComponent()->SetPlayRate(1.f);
		GetSkeletalMeshComponent()->SetEnableRootMotion(false);
		GetSkeletalMeshComponent()->Play();
	}

	else if (wantIdle)
	{
		GetSkeletalMeshComponent()->SetAnimationSequence(Idle);
		GetSkeletalMeshComponent()->SetLooping(true);
		GetSkeletalMeshComponent()->SetPlayRate(1.f);
		GetSkeletalMeshComponent()->SetEnableRootMotion(false);
		GetSkeletalMeshComponent()->Play();
	}
}

void Monster::OnDamaged(float TotalDamage, GameObject* DamageCauser)
{
	// �������¸� ó�� ����
	bool IsDeath = false;
	FindTag("Death", OUT IsDeath);

	if (IsDeath || IsDie())
		return;


	// �������� �ִ� ����� ������ ����� �ٶ󺸴� �������� �����ش�.
	if (DamageCauser)
	{
		// ������ ���ϰ� ó�´³��ϰ� �����̼��� ���� ���밪 180�̴� 
		// ������ �� Rot.z �� +�� 180�� ���ָ� �ǰ� - �� 180�� �����ָ� �ȴ� .
		// �׷� ó�´³� Rotation�� ��������.

		float DamageCauserRotZ = DamageCauser->GetWorldRotation().y;
		float HitRotZ = 0.f;


		if (0 < DamageCauserRotZ)
		{
			HitRotZ = DamageCauserRotZ - 180.f;
		}

		else if (0 > DamageCauserRotZ)
		{
			HitRotZ = DamageCauserRotZ + 180.f;
		}

		SetWorldRotation(SVector3(0.f, 0.f, HitRotZ));
	}


	// KnockDown �����̶��?
	bool isKnockDown = false;
	FindTag("KnockDown", OUT isKnockDown);


	// ���۾Ƹ� ���¶��?
	bool isSuperArmor = false;
	FindTag("SuperArmor", OUT isSuperArmor);


	// KnockDown ���¶�� SuperArmor���ο� ������� KnockDown �ִϸ��̼� ����
	if (isKnockDown)
	{
		// �����Ƽ�鿡�� Hurt Event ����
		SGameplayEventData* data = nullptr;
		m_AbilityComponent->HandleGameplayEvent("Hurt", data);

		AddTag("KnockDown", true);
		m_SkeletalMeshComponent->SetAnimationSequence(KnockDown);
		m_SkeletalMeshComponent->SetLooping(false);
		m_SkeletalMeshComponent->SetEnableRootMotion(true);
		m_SkeletalMeshComponent->PlayFromStart();

		// KnockDown Animation���� �������� KnockDown Tag false�� Getup Tag true�� �ּ���!
	}

	// SuperArmor�� �ƴ� ��� Hit �ִϸ��̼� ����
	else if (isSuperArmor == false)
	{
		// �����Ƽ�鿡�� Hurt Event ����
		SGameplayEventData* data = nullptr;
		m_AbilityComponent->HandleGameplayEvent("Hurt", data);

		AddTag("Hit", true);
		m_SkeletalMeshComponent->SetAnimationSequence(Hit);
		m_SkeletalMeshComponent->SetLooping(false);
		m_SkeletalMeshComponent->SetEnableRootMotion(true);
		m_SkeletalMeshComponent->PlayFromStart();
	}
}

void Monster::OnDie()
{
	AddTag("Death", true);
	AddTag("Idle", false);
	AddTag("Walk", false);
	AddTag("CanAbility", false);
	AddTag("Hit", false);
	AddTag("SuperArmor", false);
	AddTag("KnockDown", false);

	// Capsule �� �浹�� ���δ� ����
	((PrimitiveComponent*)GetRootComponent())->SetCollisionProfile("MonsterDeath");

	SceneInfo* scnInfo = GetOwningScene()->GetSceneInfo();
	CustomSceneInfo* CustomScnInfo = dynamic_cast<CustomSceneInfo*>(scnInfo);
	if (CustomScnInfo)
		CustomScnInfo->AddChaosDungeonKillCount();

	// ���⼭�� �������ִ� DeathAnimation�� ���� ó��
	if (Death)
	{
		GetSkeletalMeshComponent()->SetAnimationSequence(Death);
		GetSkeletalMeshComponent()->SetLooping(false);
		GetSkeletalMeshComponent()->SetPlayRate(1.0f);
		GetSkeletalMeshComponent()->PlayFromStart();
	}
}

bool Monster::IsDie() const
{
	if (m_CharacterAttributeSet)
	{
		return m_CharacterAttributeSet->GetHealth() <= 0.0f;
	}

	return true;
}
