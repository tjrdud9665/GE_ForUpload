#pragma once

#include "GameFramework/GameObject/GameObject.h"

class TestHitCollison :public GameObject
{
	CLASS(TestHitCollison, GameObject)

protected:
	TestHitCollison();
	TestHitCollison(const TestHitCollison& Other);
	virtual ~TestHitCollison();

	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void OnCollision(const SCollisionResult& CollsionResult);

	virtual void BeginPlay() override;

private:
	class BoxComponent* m_HitBox;

	std::string m_HitEventName;

};

