#include "EnginePCH.h"
#include "SphereComponent.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"

SphereComponent::SphereComponent()
	:Radius(1.0f)
{

}

SphereComponent::SphereComponent(const SphereComponent& Other)
	:Super(Other)
	, Radius(Other.Radius)
{

}

SphereComponent::~SphereComponent()
{

}

float SphereComponent::GetOriginRadius() const
{
	return Radius;
}

void SphereComponent::CreateShape()
{
	if (m_DirtyShape)
	{
		SAFE_DELETE(m_Shape);
	}

	if (m_Shape)
	{
		return;
	}

	m_Shape = new btSphereShape(Radius);
	m_DirtyShape = false;

	

}
