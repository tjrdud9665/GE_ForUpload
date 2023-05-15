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
	// Ŭ���̵�(���콺 ����)
	handle.AddActionBind("MRB", EInputEventType::IE_Pressed, std::bind(&Player::ClickToMove, this, std::placeholders::_1, std::placeholders::_2));
	// ��ȣ�ۿ�(F)
	handle.AddActionBind("Interaction", EInputEventType::IE_Pressed, std::bind(&Player::OnInteraction, this));
}

void Player::CheckTagStatement()
{
	bool isDeath = false;
	FindTag("Death", OUT isDeath);

	// �׾��� ���(Tag�� �� Character�� ��ӹ��� Ŭ�������� �������� IsDie�� �˻�)

	if (isDeath && IsDie())
	{
		// ���� �� ���
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

	// ���� Ȱ��ȭ�� Ability�� �����?
	int32 AbilityCount = GetAbilityComponent()->GetActivatingAbility();

	// ���� ȸ��������?
	FindTag("Evade", OUT isEvade);

	// ȸ�� ���̶��
	if (isEvade)
	{
		AddTag("Hit", false);
		AddTag("KnockDown", false);
		AddTag("Getup", false);
		AddTag("SuperArmor", false);
		AddTag("CantMove", true);
		AddTag("CanAbility", false);
		// Ability �󿡼� �ִϸ��̼��� �������ְ� �ִϸ��̼��� ����, ��ҵ� �� Evade Tag�� ó������
	}




	// ���� ������ ���ߴ���?
	FindTag("Hit", OUT isHit);

	// ���� �� ������ ���ؼ� ��������?
	FindTag("KnockDown", OUT isKnockDown);

	// �����ٰ� �Ͼ�� �ϴ���?
	FindTag("Getup", OUT isGetup);

	// ���� ���۾Ƹ� ��������?
	FindTag("SuperArmor", OUT isSuperArmor);


	AddTag("Idle", false);
	AddTag("Walk", false);

	// KnockDown�� ���ߴٸ�
	if (isKnockDown)
	{
		AddTag("KnockDown", true);
		AddTag("CanAbility", false);
		AddTag("Hit", false);
		AddTag("SuperArmor", false);
		AddTag("Getup", false);
		AddTag("CantMove", true);
	}

	// ���� ���¿��� �Ͼ�� �Ѵٸ�
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

		// Getup Animation���� �������� Getup Tag false�� �ּ���!
	}

	// ���� ���¶��
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
		AddTag("Hit", false);
		AddTag("KnockDown", false);
		AddTag("Getup", false);
		AddTag("SuperArmor", false);

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

void Player::OnDamaged(float TotalDamage, GameObject* DamageCauser)
{
	// �������¸� ó�� ����
	bool IsDeath = false;
	FindTag("Death", OUT IsDeath);

	if (IsDeath || IsDie())
		return;

	// KnockDown �����̶��?
	bool isKnockDown = false;
	FindTag("KnockDown", OUT isKnockDown);


	// ���۾Ƹ� ���¶��?
	bool isSuperArmor = false;
	FindTag("SuperArmor", OUT isSuperArmor);




	// �������� �ִ� ����� ������ ����� �ٶ󺸴� �������� �����ش�.
	if (DamageCauser && !isSuperArmor)
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
		GetSkeletalMeshComponent()->SetPlayRate(1.5f);
		m_SkeletalMeshComponent->SetLooping(false);
		m_SkeletalMeshComponent->SetEnableRootMotion(true);
		m_SkeletalMeshComponent->PlayFromStart();
	}
}

void Player::BeginPlay()
{
	Super::BeginPlay();

	//� Ability���� ������ ������ AbilityComponent�� ����
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

	// Capsule �� �浹�� ���δ� ����
	((PrimitiveComponent*)GetRootComponent())->SetCollisionProfile("PlayerDeath");


	// ���⼭�� �������ִ� DeathAnimation�� ���� ó��
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
