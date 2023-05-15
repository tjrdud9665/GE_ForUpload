#include "PCH.h"
#include "Player.h"
#include <Assets/Mesh/Skeleton.h>
#include <Assets/Animation/AnimationSequence.h>
#include <GameFramework/Components/CharacterMovementComponent.h>
#include <GameFramework/Components/SkeletalMeshComponent.h>
#include <GameFramework/Components/StaticMeshComponent.h>
#include <GameFramework/Components/AbilityComponent.h>
#include "../GameplayAbility/CharacterAttributeSet.h"
#include <GameFramework/Components/PrimitiveComponent.h>
#include <CameraManager.h>
#include <GameFramework/Components/CameraComponent.h>
#include <GameplaySystem/GameInstance.h>

Player::Player()
	: m_IsInteraction(false)
	, m_InteractionObject(nullptr)
{
}

Player::Player(const Player& Other)
	:Super(Other)
	, m_InitStatEffect(Other.m_InitStatEffect)
	, m_DefaultAttack(Other.m_DefaultAttack)
	, m_Skill1(Other.m_Skill1)
	, m_Skill2(Other.m_Skill2)
	, m_Skill3(Other.m_Skill3)
	, m_Skill4(Other.m_Skill4)
	, m_Evade(Other.m_Evade)
	, Hit(Other.Hit)
	, KnockDown(Other.KnockDown)
	, Getup(Other.Getup)
	, Death(Other.Death)
	, m_IsInteraction(false)
	, m_InteractionObject(nullptr)
{

	m_PlayerWeapon = FindComponent<StaticMeshComponent>("PlayerWeapon");
}

Player::~Player()
{
}

void Player::Construct(const std::string& InName)
{
	Super::Construct(InName);

	m_PlayerWeapon = CreateComponent<StaticMeshComponent>("PlayerWeapon");

	GetSkeletalMeshComponent()->AddChild(m_PlayerWeapon);
}

void Player::SetUpInput(InputHandle& handle)
{
	Super::SetUpInput(handle);

	//AxisBind
	//handle.AddAxisBind("MoveForward", std::bind(&Player::MoveForward, this, std::placeholders::_1));

	//handle.AddAxisBind("MoveRight", std::bind(&Player::MoveRight, this, std::placeholders::_1));

	//ActionBind
	// 클릭이동(마우스 우측)
	handle.AddActionBind("MRB", EInputEventType::IE_Pressed, std::bind(&Player::ClickToMove, this, std::placeholders::_1, std::placeholders::_2));
	// 상호작용(F)
	handle.AddActionBind("Interaction", EInputEventType::IE_Pressed, std::bind(&Player::OnInteraction, this));
}

void Player::CheckTagStatement()
{
	bool isDeath = false;
	FindTag("Death", OUT isDeath);

	// 죽었을 경우(Tag와 각 Character를 상속받은 클래스에서 재정의한 IsDie를 검사)

	if (isDeath && IsDie())
	{
		// 전부 다 취소
		AddTag("Death", true);
		AddTag("CantMove", true);
		AddTag("Evade", false);
		AddTag("Idle", false);
		AddTag("Walk", false);
		AddTag("CanAbility", false);
		AddTag("Hit", false);
		AddTag("SuperArmor", false);
		AddTag("KnockDown", false);
		AddTag("Getup", false);
		return;
	}
	//else
	//{
	//	AddTag("Death", false);
	//}


	bool isEvade = false;
	bool isHit = false;
	bool isKnockDown = false;
	bool isGetup = false;
	bool isSuperArmor = false;
	bool wantIdle = false;
	bool wantWalk = false;

	// 현재 활성화된 Ability가 몇개인지?
	int32 AbilityCount = GetAbilityComponent()->GetActivatingAbility();

	// 현재 회피중인지?
	FindTag("Evade", OUT isEvade);

	// 회피 중이라면
	if (isEvade)
	{
		AddTag("Hit", false);
		AddTag("KnockDown", false);
		AddTag("Getup", false);
		AddTag("SuperArmor", false);
		AddTag("CantMove", true);
		AddTag("CanAbility", false);
		// Ability 상에서 애니메이션은 실행해주고 애니메이션이 실행, 취소될 때 Evade Tag를 처리해줌
	}




	// 현재 공격을 당했는지?
	FindTag("Hit", OUT isHit);

	// 현재 쎈 공격을 당해서 누웠는지?
	FindTag("KnockDown", OUT isKnockDown);

	// 누웠다가 일어나야 하는지?
	FindTag("Getup", OUT isGetup);

	// 현재 슈퍼아머 상태인지?
	FindTag("SuperArmor", OUT isSuperArmor);


	AddTag("Idle", false);
	AddTag("Walk", false);

	// KnockDown을 당했다면
	if (isKnockDown)
	{
		AddTag("KnockDown", true);
		AddTag("CanAbility", false);
		AddTag("Hit", false);
		AddTag("SuperArmor", false);
		AddTag("Getup", false);
		AddTag("CantMove", true);
	}

	// 누운 상태에서 일어나야 한다면
	else if (isGetup)
	{
		AddTag("KnockDown", false);
		AddTag("CanAbility", false);
		AddTag("Hit", false);
		AddTag("SuperArmor", false);
		AddTag("CantMove", true);

		GetSkeletalMeshComponent()->SetAnimationSequence(Getup);
		GetSkeletalMeshComponent()->SetLooping(false);
		GetSkeletalMeshComponent()->SetEnableRootMotion(true);
		GetSkeletalMeshComponent()->Play();

		// Getup Animation에서 마지막에 Getup Tag false를 주세요!
	}

	// 맞은 상태라면
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
		AddTag("Hit", false);
		AddTag("KnockDown", false);
		AddTag("Getup", false);
		AddTag("SuperArmor", false);

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

void Player::OnDamaged(float TotalDamage, GameObject* DamageCauser)
{
	// 죽은상태면 처리 ㄴㄴ
	bool IsDeath = false;
	FindTag("Death", OUT IsDeath);

	if (IsDeath || IsDie())
		return;

	// KnockDown 공격이라면?
	bool isKnockDown = false;
	FindTag("KnockDown", OUT isKnockDown);


	// 슈퍼아머 상태라면?
	bool isSuperArmor = false;
	FindTag("SuperArmor", OUT isSuperArmor);




	// 데미지를 주는 대상이 있으면 대상을 바라보는 방향으로 돌려준다.
	if (DamageCauser && !isSuperArmor)
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
		GetSkeletalMeshComponent()->SetPlayRate(1.5f);
		m_SkeletalMeshComponent->SetLooping(false);
		m_SkeletalMeshComponent->SetEnableRootMotion(true);
		m_SkeletalMeshComponent->PlayFromStart();
	}
}

void Player::BeginPlay()
{
	Super::BeginPlay();

	//어떤 Ability들을 가지고 있을지 AbilityComponent에 전달
	m_AbilityComponent->GiveAbility(m_DefaultAttack);
	m_AbilityComponent->GiveAbility(m_Skill1);
	m_AbilityComponent->GiveAbility(m_Skill2);
	m_AbilityComponent->GiveAbility(m_Skill3);
	m_AbilityComponent->GiveAbility(m_Skill4);
	m_AbilityComponent->GiveAbility(m_Evade);

	if (!m_CharacterAttributeSet)
		m_CharacterAttributeSet = (CharacterAttributeSet*)(m_AttributeInstance.Get());


	Json statusData;
	GameInstance* gInst = g_Engine->Get<GameInstance>();
	if (gInst)
	{
		if (gInst->GetData("Status", OUT statusData))
		{
			//LoadStatus data..			
			JsonArchive ar(statusData);
			m_CharacterAttributeSet->Serialize(ar);
		}
	}

	m_AbilityComponent->ApplyGameplayEffectToTarget(m_InitStatEffect, m_AbilityComponent);

	AddTag("Idle", true);
}

void Player::EndPlay()
{
	Super::EndPlay();

	GameInstance* gInst = g_Engine->Get<GameInstance>();
	if (gInst)
	{
		JsonArchive ar{};
		m_CharacterAttributeSet->Serialize(ar);
		gInst->SaveData("Status", ar.GetData());
	}
}

void Player::OnDie()
{
	AddTag("Death", true);
	AddTag("Idle", false);
	AddTag("Walk", false);
	AddTag("CanAbility", false);
	AddTag("Hit", false);
	AddTag("SuperArmor", false);
	AddTag("KnockDown", false);

	// Capsule 이 충돌을 전부다 무시
	((PrimitiveComponent*)GetRootComponent())->SetCollisionProfile("PlayerDeath");


	// 여기서는 가지고있는 DeathAnimation에 대한 처리
	if (Death)
	{
		GetSkeletalMeshComponent()->SetAnimationSequence(Death);
		GetSkeletalMeshComponent()->SetLooping(false);
		GetSkeletalMeshComponent()->SetPlayRate(1.0f);
		GetSkeletalMeshComponent()->PlayFromStart();
	}
}

bool Player::IsDie() const
{
	if (m_CharacterAttributeSet)
	{
		return m_CharacterAttributeSet->GetHealth() <= 0.0f;
	}

	return true;
}

bool Player::IsInteractable() const
{
	return !m_IsInteraction;
}

void Player::OnBeginInteractable(IInteractableInterface* InteractionObject)
{
	m_IsInteraction = true;
	m_InteractionObject = InteractionObject;


}

void Player::OnEndInteractable()
{
	m_IsInteraction = false;
	m_InteractionObject = nullptr;

	
}

void Player::OnInteraction()
{
	if (m_InteractionObject)
	{
		m_InteractionObject->OnInteraction();
	}
}

void Player::ClickToMove(const std::string& BindName, uint64 ID)
{
	D3D11_VIEWPORT viewport = g_Engine->Get<Renderer>()->GetCurrentViewport();

	if (g_Engine->IsEditMode())
	{
		viewport = g_Engine->GetEditViewport();
	}

	SVector2 mousePos = g_Engine->Get<Input>()->GetMousePosition();

	SVector3 origin;
	SVector3 direction;

	// Ray Picking
	SHitResult hit = g_Engine->Get<CameraManager>()->GetCurrentCamera()->pickRay(viewport, mousePos.x, mousePos.y, OUT origin, OUT direction);

	if (hit.Hit)
	{
		SVector3 hitLoc = hit.point;

		GetMovementComponent()->MoveToTargetLocation(hitLoc);
	}
}
