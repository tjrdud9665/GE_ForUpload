#include "EnginePCH.h"
#include "BoxComponent.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"


BoxComponent::BoxComponent()
	:BoxExtent(1.0f)
{

}

BoxComponent::BoxComponent(const BoxComponent& Other)
	:Super(Other)
	, BoxExtent(Other.BoxExtent)
{

}

BoxComponent::~BoxComponent()
{	
}

const SVector3& BoxComponent::GetOriginBoxExtent() const
{
	return BoxExtent;
}

void BoxComponent::CreateShape()
{
	if(m_DirtyShape)
	{
		SAFE_DELETE(m_Shape);		
	}
	
	if(m_Shape)
	{
		return;
	}

	m_Shape = new btBoxShape(ToBtVector3(BoxExtent));
	m_DirtyShape = false;
	
}

