#include "PCH.h"
#include "PointLightObject.h"
#include "GameFramework/Components/PointLightComponent.h"
#include "GameFramework/Components/PrimitiveComponent.h"

PointLightObject::PointLightObject()
{

}

PointLightObject::PointLightObject(const PointLightObject& Other) 
	:Super(Other)
{
	m_PointLight = FindComponent<PointLightComponent>("PointLight");
}

PointLightObject::~PointLightObject()
{

}

void PointLightObject::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);

	m_PointLight = CreateComponent<PointLightComponent>("PointLight");

	SetRootComponent(m_PointLight);

}

void PointLightObject::PreUpdate(float DeltaSeconds)
{
	Super::PreUpdate(DeltaSeconds);
}

void PointLightObject::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);
}

void PointLightObject::PostUpdate(float DeltaSeconds)
{
	Super::PostUpdate(DeltaSeconds);
}

void PointLightObject::BeginPlay()
{
	Super::BeginPlay();
}
