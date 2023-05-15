#include "EnginePCH.h"
#include "Character.h"
#include "../Components/MovementComponent.h"
#include "../Components/CapsuleComponent.h"
#include "../Components/SkeletalMeshComponent.h"
#include "Controller.h"
#include "../Components/NavMovementComponent.h"
#include "../Components/AbilityComponent.h"
#include "../Components/CharacterMovementComponent.h"


Character::Character()
	:m_RootCapsule(nullptr)
	,m_SkeletalMeshComponent(nullptr)
	,m_CharacterMovement(nullptr)
{
	
}

Character::Character(const Character& Other)
	:Super(Other)
	, m_AttributeSetClass(Other.m_AttributeSetClass)
	, m_AttributeInstance(nullptr)
	, AbilityComponentClass(Other.AbilityComponentClass)
	, Idle(Other.Idle)
	, Walk(Other.Walk)
{
	m_RootCapsule = FindComponent<CapsuleComponent>("CapsuleComp");
	m_SkeletalMeshComponent = FindComponent<SkeletalMeshComponent>("CharacterMesh");
	m_CharacterMovement = FindComponent<CharacterMovementComponent>("CharMoveComp");

	m_AbilityComponent = FindComponent<AbilityComponent>("AbilityComp");
}

Character::~Character()
{

}

void Character::BeginPlay()
{
	m_AttributeInstance = Globals::CreateX<AttributeSet>(m_AttributeSetClass, this);
	Super::BeginPlay();
	
}

void Character::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);
	m_RootCapsule = CreateComponent<CapsuleComponent>("CapsuleComp");
	m_SkeletalMeshComponent = CreateComponent<SkeletalMeshComponent>("CharacterMesh");
	m_CharacterMovement = CreateComponent<CharacterMovementComponent>("CharMoveComp");

	m_AbilityComponent = CreateComponent<AbilityComponent>("AbilityComp", AbilityComponentClass);

	SetRootComponent(m_RootCapsule);
	m_RootCapsule->AddChild(m_SkeletalMeshComponent);
	m_SkeletalMeshComponent->SetRelativeLocation(SVector3(0, -122.0f, 0));
	m_SkeletalMeshComponent->SetRelativeRotation(SVector3(0, 0, -180));

	//														Default Capsule -> 88 + 34
}

void Character::Update(float DeltaSeconds)
{	
	Super::Update(DeltaSeconds);

	if (m_SkeletalMeshComponent->IsEnableRootMotion())
	{
		m_CharacterMovement->ApplyRootMotionAcc(m_RootCapsule->ConsumeRootMotionAcc());
	}

}

void Character::PostUpdate(float DeltaSeconds)
{
	//Character Capsule Don't Simulation cause byKinematicController
	m_RootCapsule->SetUsePhysicsSimulation(false);
	Super::PostUpdate(DeltaSeconds);

	CheckTagStatement();
}

void Character::CheckTagStatement()
{
	//Tag 에 따른 기본 상태 정의

}

void Character::MoveToLocation(const SVector3& InLocation)
{
	GetMovementComponent()->MoveToTargetLocation(InLocation);
}

void Character::OnDamaged(float TotalDamage, class GameObject* DamageCauser)
{

}

void Character::Die()
{
	//사망처리시 기본적인 처리

	//개별 객체처리
	OnDie();
}

void Character::OnDie()
{
	//개별 객체처리(Overridable)
}

bool Character::IsDie() const
{
	//사망여부
	return true;
}

void Character::PossessedBy(class Controller* NewController)
{
	SetOwner(NewController);

	Controller* const OldController = m_Controller;

	m_Controller = NewController;
}

void Character::UnPossessed()
{
	Controller* const OldController = m_Controller;
	SetOwner(nullptr);
	m_Controller = nullptr;
}

void Character::SetUpInput(InputHandle& handle)
{
	m_AbilityComponent->SetUpInput(handle);
}

class Controller* Character::GetController() const
{
	return m_Controller;
}

bool Character::PostLoad()
{
	bool Result = Super::PostLoad();

	if (!AbilityComponentClass.CustomCDOName.empty())
	{
		DeleteObjectComponent(m_AbilityComponent);
		m_AbilityComponent = CreateComponent<AbilityComponent>("AbilityComp", AbilityComponentClass);
	}


	return Result;
}

class SkeletalMeshComponent* Character::GetSkeletalMeshComponent() const
{
	return m_SkeletalMeshComponent;
}

class CharacterMovementComponent* Character::GetMovementComponent() const
{
	return m_CharacterMovement;
}

void Character::SetWorldLocation(const SVector3& InLocation)
{
	Super::SetWorldLocation(InLocation);

	m_CharacterMovement->ApplyToWorld();
}

void Character::SetWorldRotation(const SVector3& InRotation)
{
	Super::SetWorldRotation(InRotation);
	
	m_CharacterMovement->ApplyToWorld();
}

void Character::SetWorldScale(const SVector3& InScale)
{
	Super::SetWorldScale(InScale);

	m_CharacterMovement->ApplyToWorld();
}

SVector3 Character::GetWorldLocationAsBottom()
{
	SVector3 worldLocaiton = GetWorldLocation();

	worldLocaiton.y -= (m_RootCapsule->GetHalfHeight() + m_RootCapsule->GetRadius());

	return worldLocaiton;
	
}

float Character::GetCapsuleHalfLength() const
{
	return m_RootCapsule->GetHalfHeight() + m_RootCapsule->GetRadius();
}
