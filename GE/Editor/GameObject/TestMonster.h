#pragma once

#include <GameFramework/GameObject/Character.h>
#include <GameFramework/GameObject/AIController.h>

class TestMonster :public Character
{
	CLASS(TestMonster,Character)

protected:
	TestMonster();
	TestMonster(const TestMonster& Other);
	virtual ~TestMonster();

	virtual void BeginPlay() override;

	virtual void Update(float DeltaSeconds) override;

protected:
	PROP(TSubclassOf<AIController>, AIControllerClass, DetailsEdit)
	TSubclassOf<AIController> AIControllerClass;

	

};

