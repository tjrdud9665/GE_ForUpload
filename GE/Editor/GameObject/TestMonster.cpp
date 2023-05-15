#include "PCH.h"
#include "TestMonster.h"
#include "ObjectManager.h"
#include "Assets/AI/BehaviorTree.h"
#include "GameFramework/Components/CharacterMovementComponent.h"

TestMonster::TestMonster()
{

}

TestMonster::TestMonster(const TestMonster& Other)
	:Super(Other)
	, AIControllerClass(Other.AIControllerClass)
{

}

TestMonster::~TestMonster()
{

}

void TestMonster::BeginPlay()
{
	Super::BeginPlay();

	AIController* aiCon = g_Engine->Get<ObjectManager>()->SpawnGameObject<AIController>("AIController", AIControllerClass);		

	aiCon->Possess(this);

}

void TestMonster::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);	
	//Test -> 앞으로 가게끔함
	AIController* aiCon = GetControllerAs<AIController>();
	if(aiCon)
	{
		//Test -> Destination 을 제거해서 BlackboardSelector가 더이상 성공하지 못하게 막기
		if (GetMovementComponent()->GetMaxSpeed() > 150.0f)
		{
			aiCon->RemoveBlackboardValue("Destination", EBlackBoardValType::Vector);
			GetMovementComponent()->StopPathFollow();
		}
		else
		{
			SVector3 dest = GetWorldLocationAsBottom() + GetObjectForwardVector() * 100.0f;
			aiCon->SetBlackboardValueAsVector("Destination", dest);
		}
	}
	

	
	//스킬 써보기
	
	
}
