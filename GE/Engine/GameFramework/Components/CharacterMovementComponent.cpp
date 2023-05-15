#include "EnginePCH.h"
#include "CharacterMovementComponent.h"
#include "BulletDynamics/Dynamics/btDynamicsWorld.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "GameFramework/GameObject/GameObject.h"

CharacterMovementComponent::CharacterMovementComponent()
	:RotationRate(5.0f)
{

}

CharacterMovementComponent::CharacterMovementComponent(const CharacterMovementComponent& Other)
	:Super(Other)
	, MaxWalkSpeed(Other.MaxWalkSpeed)
	, RotationRate(Other.RotationRate)
	, CantMoveTag(Other.CantMoveTag)
{

}

CharacterMovementComponent::~CharacterMovementComponent()
{

}

btActionInterface* CharacterMovementComponent::InitActionInterface()
{
	if (m_cachedPhysics)
	{
		SAFE_DELETE(m_btActionController);
	
		if (m_UpdateComponent->GetClass()->IsChildOf(PrimitiveComponent::StaticClass()))
		{
		PrimitiveComponent* prim = (PrimitiveComponent*)(m_UpdateComponent.Get());
		m_btActionController = new btPlayerAction(prim->GetGhostBody(), (btConvexShape*)prim->GetShape(), stepHeight,btVector3(0,1.0f,0));

		btTransform trans = ToBtTransform(prim->GetComponentTransform());
		
		
		GetActionInterfaceAs<btKinematicCharacterController>()->getGhostObject()->setWorldTransform(trans);
		GetActionInterfaceAs<btKinematicCharacterController>()->getGhostObject()->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
		GetActionInterfaceAs<btKinematicCharacterController>()->setFallSpeed(9800);//MaxFallSpeed						
		GetActionInterfaceAs<btKinematicCharacterController>()->setMaxSlope(btRadians(65.0f));
		GetActionInterfaceAs<btKinematicCharacterController>()->setGravity(btVector3(0, -98, 0));		
		GetActionInterfaceAs<btKinematicCharacterController>()->setMaxPenetrationDepth(5);		

		//prim->GetGhostBody()->forceActivationState(DISABLE_DEACTIVATION);
		//prim->GetGhostBody()->setDeactivationTime(2000);
		}
	}


	return m_btActionController;
}

void CharacterMovementComponent::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);

	const SVector3 fwd = GetUpdateComponent()->GetComponentForwardVector();

	const SVector3 LastInputVector = ConsumeMovementInputVector();

	SVector3 inputDir = LastInputVector.GetSafeNormal();	

	float adjustDelta = std::min(1.0f, DeltaSeconds);

	SVector3 moveVector = inputDir * GetMaxSpeed() * adjustDelta;


	bool CantMove = false;
	GetUpdateComponent()->GetOwner<GameObject>()->FindTag(CantMoveTag,OUT CantMove);

	if (m_btActionController && IsPlaying())
	{
		btKinematicCharacterController* characterController = GetActionInterfaceAs<btKinematicCharacterController>();
		//키보드 이동
		//if (m_AgentID == INDEX_NONE)
		//{	
		//	//DesiredInput Direciton  변위량			
		//	//(fwd ^ inputDir).y 의 값의 부호에 따라회전방향을 결정
		//	int32 sign = SMathUtils::Sign<float>((fwd ^ inputDir).y);

		//	characterController->setAngularVelocity(btVector3(0, sign * (1 - (fwd | inputDir)) * inputDir.Size() * RotationRate, 0));

		//	if (moveVector.Size())
		//	{
		//		m_LastInputDirection = inputDir;
		//	}
		//	float verticalVelocity = characterController->getLinearVelocity().y();
		//	if (!SMathUtils::Equals(verticalVelocity, 0))
		//	{
		//		moveVector.y += verticalVelocity - moveVector.Size();
		//	}

		//	characterController->setLinearVelocity(ToBtVector3(moveVector));
		//} 
		
		{
			btVector3 vel = characterController->getLinearVelocity();			
			SVector3 AgentLocation = GetUpdateComponent()->GetWorldLocation();
			m_CachedNav->GetAgentLocation(m_AgentID, OUT AgentLocation);
			SVector3 compLoc = GetUpdateComponent()->GetWorldLocation();			
			
			
			SVector3 AgentVelocity;
			m_CachedNav->GetAgentVelocity(m_AgentID, OUT AgentVelocity);
			if(AgentVelocity.Size() <= 5 || CantMove)
			{
				m_CachedNav->ResetMoveTarget(m_AgentID);
				m_NavFollowPath = false;			
			}
			if(m_NavFollowPath)
			{		

				SVector3 delta = (AgentLocation - compLoc);
				delta.y = 0;
				SVector3 desiredDir = AgentVelocity.GetSafeNormal();
				desiredDir.y = 0;
				int32 sign = SMathUtils::Sign<float>((fwd ^ desiredDir).y);

				characterController->setAngularVelocity(btVector3(0, sign * (1 - (fwd | desiredDir))  * RotationRate, 0));	

				characterController->setLinearVelocity(ToBtVector3(AgentVelocity)*DeltaSeconds);
			}
			else
			{
				SVector3 reachedVelocity = SVector3(0.0f);
				float verticalVelocity = characterController->getLinearVelocity().y();
				if (!SMathUtils::Equals(verticalVelocity, 0))
				{
					reachedVelocity.y += verticalVelocity;
				}

				characterController->setAngularVelocity(btVector3(0, 0, 0));
				characterController->setLinearVelocity(ToBtVector3(reachedVelocity));
			}			
		}
	}
	



}

void CharacterMovementComponent::PostUpdate(float DeltaSeconds)
{
	Super::PostUpdate(DeltaSeconds);	
}


float CharacterMovementComponent::GetMaxSpeed() const
{
	return MaxWalkSpeed;
}

void CharacterMovementComponent::SetMaxSpeed(float InSpeed)
{
	MaxWalkSpeed = InSpeed;
}

void CharacterMovementComponent::AddInputVector(const SVector3& WorldVector)
{
	m_ControllInputVector += WorldVector * GetMaxSpeed();
}

const SVector3& CharacterMovementComponent::ConsumeMovementInputVector()
{
	m_LastControllInputVector = m_ControllInputVector;
	m_ControllInputVector = SVector3::ZeroVector;
	return m_LastControllInputVector;
}

const SVector3& CharacterMovementComponent::GetLastControllInputVector() const
{
	return m_LastControllInputVector;
}

float CharacterMovementComponent::GetCurrentVelocity() 
{
	btKinematicCharacterController* characterController = GetActionInterfaceAs<btKinematicCharacterController>();
	if(characterController)
	{
		return characterController->getLinearVelocity().length();
	}

	return 0.0f;
}

void CharacterMovementComponent::ApplyToWorld()
{
	btKinematicCharacterController* characterController = GetActionInterfaceAs<btKinematicCharacterController>();
	if(characterController && m_UpdateComponent)
	{
		m_UpdateComponent->ConditionalUpdateComponentToWorld();
		btTransform btTr;
		btTr.setIdentity();
		btTr.setOrigin(ToBtVector3(m_UpdateComponent->GetWorldLocation()));
		btTr.setRotation(ToBtQuaternion(SMathUtils::EulerToQuat(m_UpdateComponent->GetWorldRotation().ConvertAngle())));
		
		characterController->getGhostObject()->setWorldTransform(btTr);
	}
}

void CharacterMovementComponent::ApplyRootMotionAcc(const SVector3& RootAcc)
{
	btKinematicCharacterController* characterController = GetActionInterfaceAs<btKinematicCharacterController>();
	if (characterController && m_UpdateComponent)
	{
		/*	btTransform btTr = characterController->getGhostObject()->getWorldTransform();
			btTr.setOrigin(btTr.getOrigin() + ToBtVector3(RootAcc));
			characterController->getGhostObject()->setWorldTransform(btTr);*/

		//RootAcc 는 이동해야될 양		
		
		characterController->setLinearVelocity(characterController->getLinearVelocity() + ToBtVector3(RootAcc));
		
	}
}

bool CharacterMovementComponent::IsPathFollowing() const
{
	return m_NavFollowPath;
}

btPlayerAction::btPlayerAction(class btPairCachingGhostObject* ghostObject, btConvexShape* convexShape, btScalar stepHeight, const btVector3& up)
	:btKinematicCharacterController(ghostObject, convexShape, stepHeight, up)
{

}

bool btPlayerAction::needsCollision(const btCollisionObject* body0, const btCollisionObject* body1)
{

	PrimitiveComponent* comp0 = (PrimitiveComponent*)body0->getUserPointer();
	PrimitiveComponent* comp1 = (PrimitiveComponent*)body1->getUserPointer();


	SCollisionProfile* profile0 = comp0->GetCollisionProfile();
	SCollisionProfile* profile1 = comp1->GetCollisionProfile();

	if (!profile0 || !profile1)
		return false;

	//Is Enable pair?
	if (!profile0->Enable || !profile1->Enable)
		return false;

	//Is Ignore each other ??
	if (profile0->vecCollisionInteraction[(int32)profile1->Channel->Channel] == ECollision_Interaction::Block &&
		profile1->vecCollisionInteraction[(int32)profile0->Channel->Channel] == ECollision_Interaction::Block)
	{
		return true;
	}

	return false;
}
