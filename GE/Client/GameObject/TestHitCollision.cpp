#include "PCH.h"
#include "TestHitCollision.h"
#include <GameFramework/Components/BoxComponent.h>

TestHitCollision::TestHitCollision()
{

}

TestHitCollision::TestHitCollision(const TestHitCollision& Other)
	:Super(Other)
	, m_HitEventName(Other.m_HitEventName)
{
	m_HitBox = FindComponent<BoxComponent>("HitBoxComp");

}

TestHitCollision::~TestHitCollision()
{

}

void TestHitCollision::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);

	m_HitBox = CreateComponent<BoxComponent>("HitBoxComp");

	SetRootComponent(m_HitBox);

}

void TestHitCollision::OnCollision(const SCollisionResult& CollsionResult)
{
	if (CollsionResult.Dest)
	{
		SGameplayEventData* data = new SGameplayEventData();
		data->EventName = m_HitEventName;
		data->Instigator = this;
		data->Target = CollsionResult.Dest->GetOwner<GameObject>();
		GameplaySystem::SendGameplayEvent(this, data->EventName, data);
	}
}

void TestHitCollision::BeginPlay()
{
	Super::BeginPlay();

	m_HitBox->BindOnBeginOverlap(std::bind(&TestHitCollision::OnCollision, this, std::placeholders::_1));
}
