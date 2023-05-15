#include "EnginePCH.h"
#include "CapsuleComponent.h"
#include "BulletCollision/CollisionShapes/btCapsuleShape.h"

CapsuleComponent::CapsuleComponent()
	:Radius(34.0f)
	, HalfHeight(88.0f)
{

}

CapsuleComponent::CapsuleComponent(const CapsuleComponent& Other)
	:Super(Other)
	,Radius(Other.Radius)
	,HalfHeight(Other.HalfHeight)
	,prevHeight(Other.prevHeight)
	,prevRadius(Other.prevRadius)
{

}

CapsuleComponent::~CapsuleComponent()
{

}

void CapsuleComponent::BeginPlay()
{
	Super::BeginPlay();
	prevRadius = Radius;
	prevHeight = HalfHeight;

}

void CapsuleComponent::PreUpdate(float DeltaSeconds)
{
	if (Radius != prevRadius || HalfHeight != prevHeight)
	{
		prevRadius = Radius;
		prevHeight = HalfHeight;
		m_DirtyShape = true;
		ReInitCollisionAndPhysics();

	}
	Super::PreUpdate(DeltaSeconds);
}



float CapsuleComponent::GetHalfHeight() const
{
	return HalfHeight;
}

float CapsuleComponent::GetRadius() const
{
	return Radius;
}

void CapsuleComponent::CreateShape()
{
	if (m_DirtyShape)
	{
		SAFE_DELETE(m_Shape);
	}

	m_Shape = new btCapsuleShape(Radius,HalfHeight*2.0f);
	m_DirtyShape = false;

	
}

