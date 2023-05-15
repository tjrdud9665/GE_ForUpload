#include "PCH.h"
#include "TestObject.h"
#include "GameFramework/Components/SceneComponent.h"
#include "GameFramework/Components/PrimitiveComponent.h"
#include "GameFramework/Components/SkeletalMeshComponent.h"
#include "GameFramework/Components/PointLightComponent.h"
#include "GameFramework/Components/SpotLightComponent.h"
#include "GameFramework/Components/StaticMeshComponent.h"
#include "GameFramework/Components/ShapeComponent.h"
#include <GameFramework/Components/BoxComponent.h>
#include <GameFramework/Components/SphereComponent.h>
#include <GameFramework/Components/ParticleComponent.h>

TestObject::TestObject()
{

}

TestObject::TestObject(const TestObject& Other)
	:Super(Other)
	, m_TestSceneRoot(Other.m_TestSceneRoot)
{
	m_TestSceneRoot = FindComponent<SkeletalMeshComponent>("RootTest");
	m_TestChild = FindComponent<StaticMeshComponent>("TestChild");
	m_LightTest = FindComponent<PointLightComponent>("TestLight");
	m_SpotTest = FindComponent<SpotLightComponent>("SpotTest");
	m_TestShape = FindComponent<BoxComponent>("ShapeTest");
	m_TestParticle = FindComponent<ParticleComponent>("testParticle");
}

TestObject::~TestObject()
{
	if (m_TestChild)
	{
		int32 refTest = m_TestChild->GetRefCount();
		int32 t = 0;
	}
}

void TestObject::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);

	m_TestSceneRoot = CreateComponent<SkeletalMeshComponent>("RootTest");
	m_TestChild = CreateComponent<StaticMeshComponent>("TestChild");
	m_LightTest = CreateComponent<PointLightComponent>("TestLight");
	m_SpotTest = CreateComponent<SpotLightComponent>("SpotTest");
	m_TestShape = CreateComponent<BoxComponent>("ShapeTest");
	m_TestParticle = CreateComponent<ParticleComponent>("testParticle");

	SetRootComponent(m_TestSceneRoot);
	m_TestSceneRoot->AddChild(m_TestChild,"Weapon_R");
	m_TestSceneRoot->AddChild(m_LightTest);
	m_TestSceneRoot->AddChild(m_SpotTest);
	m_TestSceneRoot->AddChild(m_TestShape);
	m_TestSceneRoot->AddChild(m_TestParticle);

	m_TestSceneRoot->SetRelativeLocation(SVector3(0, 0, 5));
	//m_TestSceneRoot->SetRelativeRotation(SVector3(0, 0, 35.0f));
	m_TestSceneRoot->SetRelativeScale(SVector3(1, 1, 1));

	m_TestChild->SetRelativeLocation(SVector3(0, 0, 0));

	
	//m_TestSceneChild->SetWorldRotation(SVector3(15, 25.0f, 35.0f));
	//SVector3 test =  m_TestSceneChild->GetWorldRotation();

	//TestSceneChild->SetAbsoluteRotation(true);

	//m_TestSceneChild->SetRelativeRotation(SVector3(0, 0, 35.0f));
}

void TestObject::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);

	//static float t = 35.0f;
	//m_TestSceneRoot->SetRelativeRotation(SVector3(0, 0,t));	
	//t += DeltaSeconds * 50.0f;

}
