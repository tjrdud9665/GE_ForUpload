#include "PCH.h"
#include "HitBoxObject.h"
#include "GameFramework/Components/BoxComponent.h"
#include "GameFramework/Components/AbilityComponent.h"
#include "GameFramework/GameObject/Character.h"
#include "GameFramework/Components/PrimitiveComponent.h"

HitBoxObject::HitBoxObject()
	: m_DeadTimerID(0)
	, m_DeadTime(0.f)
	, m_Shooter(nullptr)
{

}

HitBoxObject::HitBoxObject(const HitBoxObject& Other)
	:Super(Other)
	, m_DeadTimerID(0)
	, m_DeadTime(Other.m_DeadTime)
	, m_HitEventName(Other.m_HitEventName)
	, m_Shooter(Other.m_Shooter)
{
	m_Box = FindComponent<BoxComponent>("Box");
}

HitBoxObject::~HitBoxObject()
{
}

void HitBoxObject::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);

	m_Box = CreateComponent<BoxComponent>("Box");

	SetRootComponent(m_Box);

}

void HitBoxObject::BeginPlay()
{
	Super::BeginPlay();

	m_Box->BindOnBeginOverlap(std::bind(&HitBoxObject::CollisionOn, this, std::placeholders::_1));

	m_TimerHandle.SetTimer(OUT m_DestroyTimerID, m_DeadTime, false, std::bind(&HitBoxObject::Kill, this));

}

void HitBoxObject::SetShooter(class GameObject* InShooter)
{
	m_Shooter = InShooter;
}

void HitBoxObject::CollisionOn(const SCollisionResult& Result)
{

	if (Result.Dest->IsValidate())
	{
		GameObject* targetObj = Result.Dest->GetOwner<GameObject>();

		if (targetObj && m_Shooter)
		{
			SGameplayEventData* eventData = new SGameplayEventData();

			eventData->Instigator = Result.Src->GetOwner<GameObject>();
			eventData->Target = targetObj;
			eventData->EventName = m_HitEventName;

			GameplaySystem::SendGameplayEvent(m_Shooter, m_HitEventName, eventData);
		}
	}
}
