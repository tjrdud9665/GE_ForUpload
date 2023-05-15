#pragma once

#include "GameFramework/GameObject/GameObject.h"


class TestObject :public GameObject
{
	CLASS(TestObject, GameObject)

protected:
	TestObject();
	TestObject(const TestObject& Other);
	virtual ~TestObject();


	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void Update(float DeltaSeconds) override;

private:
	class SkeletalMeshComponent*		m_TestSceneRoot;

	class StaticMeshComponent* m_TestChild;

	class PointLightComponent* m_LightTest;

	class SpotLightComponent* m_SpotTest;

	class BoxComponent* m_TestShape;
	
	class ParticleComponent* m_TestParticle;

	
	PROP(std::vector<TSubclassOf<GameObject>>, m_testArray, DetailsEdit | Transient, "Test")
	std::vector<TSubclassOf<GameObject>> m_testArray;

	PROP(bool, TestBool, DetailsView, "Test")
	bool TestBool;
		
};

