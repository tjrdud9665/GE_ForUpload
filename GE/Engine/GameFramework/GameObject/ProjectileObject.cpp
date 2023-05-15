#include "EnginePCH.h"
#include "ProjectileObject.h"
#include "GameFramework/Components/BoxComponent.h"

ProjectileObject::ProjectileObject()
{

}

ProjectileObject::ProjectileObject(const ProjectileObject& Other)
{

}

ProjectileObject::~ProjectileObject()
{

}

void ProjectileObject::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);

	m_Box = CreateComponent<BoxComponent>("Box");

	SetRootComponent(m_Box);
}

void ProjectileObject::PreUpdate(float DeltaSeconds)
{
	Super::PreUpdate(DeltaSeconds);
}

void ProjectileObject::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);
}

void ProjectileObject::PostUpdate(float DeltaSeconds)
{
	Super::PostUpdate(DeltaSeconds);

	m_DeadTime += DeltaSeconds;

	if (m_DeadTime > 0.5f)
	{
		Destroy();
		m_DeadTime = 0.f;
	}
}

