#include "PCH.h"
#include "TestHitCollison.h"
#include <GameFramework/Components/BoxComponent.h>

TestHitCollison::TestHitCollison()
{

}

TestHitCollison::TestHitCollison(const TestHitCollison& Other)
	:Super(Other)
	,m_HitEventName(Other.m_HitEventName)
{
	m_HitBox = FindComponent<BoxComponent>("HitBoxComp");

}

TestHitCollison::~TestHitCollison()
{

}

void TestHitCollison::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);

	m_HitBox = CreateComponent<BoxComponent>("HitBoxComp");

	SetRootComponent(m_HitBox);

}

void TestHitCollison::OnCollision(const SCollisionResult& CollsionResult)
{
	if(CollsionResult.Dest)
	{
		SGameplayEventData* data = new SGameplayEventData();
		data->EventName = m_HitEventName;
		data->Instigator = this;
		data->Target = CollsionResult.Dest->GetOwner<GameObject>();
		GameplaySystem::SendGameplayEvent(this, data->EventName, data);
	}
}

void TestHitCollison::BeginPlay()
{
	Super::BeginPlay();

	m_HitBox->BindOnBeginOverlap(std::bind(&TestHitCollison::OnCollision,this,std::placeholders::_1));
}
