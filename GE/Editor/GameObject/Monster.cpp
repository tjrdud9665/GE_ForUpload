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

	// 몬스터마다 AIController를 가진다.
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

	// 죽었을 경우(Tag와 각 Character를 상속받은 클래스에서 재정의한 IsDie를 검사)
	if (isDeath && IsDie())
	{
		AIController* aiCon = GetControllerAs<AIController>();

		if(aiCon)
			aiCon->SetBlackboardValueAsBool("Die", isDeath);


		// 전부 다 취소
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

	// 현재 활성화된 Ability가 몇개인지?
	int32 AbilityCount = GetAbilityComponent()->GetActivatingAbility();


	// 현재 공격을 당했는지?
	FindTag("Hit", OUT isHit);

	// 현재 쎈 공격을 당해서 누웠는지?
	FindTag("KnockDown", OUT isKnockDown);

	// 누웠다가 일어나야 하는지?
	FindTag("Getup", OUT isGetup);



	AddTag("Idle", false);
	AddTag("Walk", false);

	// KnockDown을 당했다면
	if (isKnockDown)
	{
		AddTag("KnockDown", true);
		AddTag("CantMove", true);
		AddTag("CanAbility", false);
		AddTag("Hit", false);
		AddTag("SuperArmor", false);
		AddTag("Getup", false);
	}

	// 누운 상태에서 일어나야 한다면
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

		// Getup Animation에서 마지막에 Getup Tag false를 주세요!
	}

	else if (isHit)
	{
		AddTag("CanAbility", false);
		AddTag("CantMove", true);
	}

	// 활성화된 Ability가 있다면
	else if (AbilityCount)
	{
		AddTag("CanAbility", false);
		AddTag("CantMove", true);
		AddTag("Hit", false);
	}

	// 어빌리티도 안쓰고 맞은상태도 아니고 아무것도 아니라면
	else
	{
		AddTag("CantMove", false);
		AddTag("CanAbility", true);


		bool DesiredMovement = GetMovementComponent()->IsPathFollowing();

		// 이동하고자 한다면
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


	// 최종적으로 Idle이나 Walk 상태일 경우
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
	// 죽은상태면 처리 ㄴㄴ
	bool IsDeath = false;
	FindTag("Death", OUT IsDeath);

	if (IsDeath || IsDie())
		return;


	// 데미지를 주는 대상이 있으면 대상을 바라보는 방향으로 돌려준다.
	if (DamageCauser)
	{
		// 때리는 놈하고 처맞는놈하고 로테이션의 합은 절대값 180이다 
		// 때리는 놈 Rot.z 가 +면 180을 빼주면 되고 - 면 180을 더해주면 된다 .
		// 그럼 처맞는놈 Rotation이 구해진다.

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


	// KnockDown 공격이라면?
	bool isKnockDown = false;
	FindTag("KnockDown", OUT isKnockDown);


	// 슈퍼아머 상태라면?
	bool isSuperArmor = false;
	FindTag("SuperArmor", OUT isSuperArmor);


	// KnockDown 상태라면 SuperArmor여부에 상관없이 KnockDown 애니메이션 수행
	if (isKnockDown)
	{
		// 어빌리티들에게 Hurt Event 전달
		SGameplayEventData* data = nullptr;
		m_AbilityComponent->HandleGameplayEvent("Hurt", data);

		AddTag("KnockDown", true);
		m_SkeletalMeshComponent->SetAnimationSequence(KnockDown);
		m_SkeletalMeshComponent->SetLooping(false);
		m_SkeletalMeshComponent->SetEnableRootMotion(true);
		m_SkeletalMeshComponent->PlayFromStart();

		// KnockDown Animation에서 마지막에 KnockDown Tag false랑 Getup Tag true를 주세요!
	}

	// SuperArmor가 아닐 경우 Hit 애니메이션 수행
	else if (isSuperArmor == false)
	{
		// 어빌리티들에게 Hurt Event 전달
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

	// Capsule 이 충돌을 전부다 무시
	((PrimitiveComponent*)GetRootComponent())->SetCollisionProfile("MonsterDeath");

	SceneInfo* scnInfo = GetOwningScene()->GetSceneInfo();
	CustomSceneInfo* CustomScnInfo = dynamic_cast<CustomSceneInfo*>(scnInfo);
	if (CustomScnInfo)
		CustomScnInfo->AddChaosDungeonKillCount();

	// 여기서는 가지고있는 DeathAnimation에 대한 처리
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
