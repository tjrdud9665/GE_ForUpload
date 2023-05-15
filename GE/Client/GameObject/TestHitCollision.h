#pragma once
#include "GameFramework\GameObject\GameObject.h"

class TestHitCollision : public GameObject
{
	CLASS(TestHitCollision, GameObject)

protected:
	TestHitCollision();
	TestHitCollision(const TestHitCollision& Other);
	virtual ~TestHitCollision();

	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void OnCollision(const SCollisionResult& CollsionResult);

	virtual void BeginPlay() override;

private:
	class BoxComponent* m_HitBox;

	std::string m_HitEventName;
};

