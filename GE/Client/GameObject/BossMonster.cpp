#include "PCH.h"
#include "BossMonster.h"
#include <GameFramework/Components/CharacterMovementComponent.h>
#include <GameFramework/Components/SkeletalMeshComponent.h>
#include <GameFramework/GameObject/AIController.h>
#include <SceneViewport.h>
#include <Assets/Widget/UserWidget.h>
#include "SceneManager.h"

BossMonster::BossMonster()
{
}

BossMonster::BossMonster(const BossMonster& Other)
	: Super(Other)
	, Groggy(Other.Groggy)
	, Run(Other.Run)
	, m_Skill1(Other.m_Skill1)
	, m_Skill2(Other.m_Skill2)
	, m_Skill3(Other.m_Skill3)
	, m_Skill1Range(Other.m_Skill1Range)
	, m_Skill2Range(Other.m_Skill2Range)
	, m_Skill3Range(Other.m_Skill3Range)
	, BossHPWidgetClass(Other.BossHPWidgetClass)
	, EndingScene(Other.EndingScene)
{
}

BossMonster::~BossMonster()
{
}

void BossMonster::BeginPlay()
{
	Super::BeginPlay();

	m_AbilityComponent->GiveAbility(m_Skill1);
	m_AbilityComponent->GiveAbility(m_Skill2);
	m_AbilityComponent->GiveAbility(m_Skill3);

	AIController* aiCon = GetControllerAs<AIController>();

	aiCon->SetBlackboardValueAsAbility("Skill1", m_Skill1);
	aiCon->SetBlackboardValueAsAbility("Skill2", m_Skill2);
	aiCon->SetBlackboardValueAsAbility("Skill3", m_Skill3);
	aiCon->SetBlackboardValueAsFloat("Skill1Range", m_Skill1Range);
	aiCon->SetBlackboardValueAsFloat("Skill2Range", m_Skill2Range);
	aiCon->SetBlackboardValueAsFloat("Skill3Range", m_Skill3Range);

	if (!BossHPWidgetClass.CustomCDOName.empty())
		m_BossHPWidgetInstance = g_Engine->Get<SceneViewport>()->CreateUserWidget<UserWidget>(BossHPWidgetClass);
}

void BossMonster::CheckTagStatement()
{
	bool isDeath = false;
	FindTag("Death", OUT isDeath);

	// �׾��� ���(Tag�� �� Character�� ��ӹ��� Ŭ�������� �������� IsDie�� �˻�)
	if (isDeath && IsDie())
	{
		AIController* aiCon = GetControllerAs<AIController>();

		if (aiCon)
			aiCon->SetBlackboardValueAsBool("Die", isDeath);


		// ���� �� ���
		AddTag("Death", true);
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

	bool isGroggy = false;
	FindTag("Groggy", OUT isGroggy);

	bool wantIdle = false;
	bool wantWalk = false;
	int32 AbilityCount = GetAbilityComponent()->GetActivatingAbility();

	AddTag("Idle", false);
	AddTag("Walk", false);

	if (isGroggy)
	{
		AddTag("CantMove", true);
		AddTag("CanAbility", false);
		AddTag("Ultimate", false);

		GetSkeletalMeshComponent()->SetAnimationSequence(Groggy);
		GetSkeletalMeshComponent()->SetLooping(false);
		GetSkeletalMeshComponent()->SetPlayRate(1.f);
		GetSkeletalMeshComponent()->SetEnableRootMotion(true);
		GetSkeletalMeshComponent()->Play();
	}




	// Ȱ��ȭ�� Ability�� �ִٸ�
	else if (AbilityCount)
	{
		AddTag("CantMove", true);
		AddTag("CanAbility", false);
	}

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
		if(GetMovementComponent()->GetMaxSpeed() >= 200.f)
			GetSkeletalMeshComponent()->SetAnimationSequence(Run);

		else
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

void BossMonster::OnDamaged(float TotalDamage, GameObject* DamageCauser)
{
	// �������¸� ó�� ����
	bool IsDeath = false;
	FindTag("Death", OUT IsDeath);

	if (IsDeath || IsDie())
		return;


	bool isUltimate = false;
	FindTag("Ultimate", OUT isUltimate);

	
	if (isUltimate)
	{
		// �����Ƽ�鿡�� Hurt Event ����
		SGameplayEventData* data = nullptr;
		m_AbilityComponent->HandleGameplayEvent("Hurt", data);
	}
}

void BossMonster::OnDie()
{
	Super::OnDie();

	m_TimerHandle.SetTimer(OUT m_ToEndingSceneID, 5.0f, false, std::bind(&BossMonster::ToEndingScene, this));
}

void BossMonster::ToEndingScene()
{
	g_Engine->Get<SceneManager>()->CreateNextScene(EndingScene);
	g_Engine->Get<SceneManager>()->SetAutoPlay(true);
}
