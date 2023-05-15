#include "EnginePCH.h"
#include "MovementComponent.h"
#include "GameFramework/GameObject/GameObject.h"
#include "GameFramework/Components/SceneComponent.h"
#include "PrimitiveComponent.h"
#include <Physics/PhysicsSystem.h>
#include <Physics/BulletPhysicsHelper.h>

#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Dynamics/btDynamicsWorld.h"



MovementComponent::MovementComponent()
	:m_btActionController(nullptr)
{
	
}

MovementComponent::MovementComponent(const MovementComponent& Other)
	:Super(Other)
	, m_btActionController(nullptr)
	, m_cachedPhysics(Other.m_cachedPhysics)
{

}

MovementComponent::~MovementComponent()
{
	if(m_btActionController)
	{
		m_cachedPhysics->GetWorld()->removeAction(m_btActionController);		
		SAFE_DELETE(m_btActionController);
	}
}

void MovementComponent::BeginPlay()
{
	Super::BeginPlay();

	m_btActionController = InitActionInterface();
	m_cachedPhysics->GetWorld()->addAction(m_btActionController);
	//World 에 추가하는 코드가 PostUpdate 에 있음..	

}

void MovementComponent::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);
	m_cachedPhysics = g_Engine->Get<PhysicsSystem>();

}

void MovementComponent::EndPlay()
{
	Super::EndPlay();
	m_cachedPhysics->GetWorld()->removeAction(m_btActionController);
	SAFE_DELETE(m_btActionController);
}

void MovementComponent::PostUpdate(float DeltaSeconds)
{	
	Super::PostUpdate(DeltaSeconds);
	if(IsPlaying())
	{
		if (m_UpdateComponent->GetClass()->IsChildOf(PrimitiveComponent::StaticClass()))
		{
			PrimitiveComponent* prim = (PrimitiveComponent*)(m_UpdateComponent.Get());

			btTransform btTr = prim->GetGhostBody()->getWorldTransform();

			prim->SetWorldLocation(ToVector3(btTr.getOrigin()));
			prim->SetWorldRotation(ToQuaternion(btTr.getRotation()).ToEulerAngles());				

			//Root Motion에의한 적용 처리 
		}
	}	


}

class SceneComponent* MovementComponent::GetUpdateComponent() const
{
	return m_UpdateComponent;
}

void MovementComponent::SetUpdateComponent(class SceneComponent* UpdateTarget)
{
	m_UpdateComponent = UpdateTarget;
}


void MovementComponent::SetOwner(XObject* InOwner)
{
	Super::SetOwner(InOwner);

	if (InOwner->GetClass()->IsA(GameObject::StaticClass()) ||
		InOwner->GetClass()->IsChildOf(GameObject::StaticClass()))
	{
		SetUpdateComponent(((GameObject*)InOwner)->GetRootComponent());
	} 
	else if (InOwner->GetClass()->IsChildOf(SceneComponent::StaticClass()) ||
			   InOwner->GetClass()->IsA(SceneComponent::StaticClass()))
	{
		SetUpdateComponent((SceneComponent*)InOwner);
	}

}

void MovementComponent::ApplyToWorld()
{
	
}

class btActionInterface* MovementComponent::InitActionInterface()
{
	return m_btActionController;
}


