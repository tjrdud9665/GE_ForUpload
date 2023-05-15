#include "EnginePCH.h"
#include "PhysicsMisc.h"
#include "BulletCollision/CollisionDispatch/btCollisionObjectWrapper.h"
#include "PhysicsSystem.h"
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "GameFramework/Components/PrimitiveComponent.h"



btScalar CollisionCallback::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* a, int partIdA, int indexA, const btCollisionObjectWrapper* b, int partIdB, int indexB)
{
	bCollision = cp.getDistance() < m_closestDistanceThreshold ? true : false;
	return bCollision;
}


bool CollisionFilterCallback::needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const
{
	btCollisionObject* body0 = (btCollisionObject*)proxy0->m_clientObject;
	btCollisionObject* body1 = (btCollisionObject*)proxy1->m_clientObject;

	PrimitiveComponent* comp0 = (PrimitiveComponent*)body0->getUserPointer();
	PrimitiveComponent* comp1 = (PrimitiveComponent*)body1->getUserPointer();	


	SCollisionProfile* profile0 = comp0->GetCollisionProfile();
	SCollisionProfile* profile1 = comp1->GetCollisionProfile();
	
	if(!profile0 || !profile1)
		return false;

	//Is Enable pair?
	if (!profile0->Enable || !profile1->Enable)
		return false;

	//Is Ignore each other ??
	if (profile0->vecCollisionInteraction[(int32)profile1->Channel->Channel] == ECollision_Interaction::Ignore ||
		profile1->vecCollisionInteraction[(int32)profile0->Channel->Channel] == ECollision_Interaction::Ignore)
	{
		return false;
	}

	return true;
}

bool CustomDispathcer::needsResponse(const btCollisionObject* body0, const btCollisionObject* body1)
{
	//bool bResult = btCollisionDispatcher::needsResponse(body0, body1);

	PrimitiveComponent* comp0 = (PrimitiveComponent*)body0->getUserPointer();
	PrimitiveComponent* comp1 = (PrimitiveComponent*)body1->getUserPointer();

	//Block 
	SCollisionProfile* profile0 = comp0->GetCollisionProfile();
	SCollisionProfile* profile1 = comp1->GetCollisionProfile();


	//Is Response each other ??
	//Only Respondse Block to Block.
	if (profile0->vecCollisionInteraction[(int32)profile1->Channel->Channel] == ECollision_Interaction::Block &&
		profile1->vecCollisionInteraction[(int32)profile0->Channel->Channel] == ECollision_Interaction::Block)
	{
		return true;
	}

	return false;
}

bool RayTestCallback::needsCollision(btBroadphaseProxy* proxy0) const
{
	btCollisionObject* body0 = (btCollisionObject*)proxy0->m_clientObject;
	PrimitiveComponent* comp0 = (PrimitiveComponent*)body0->getUserPointer();


	//충돌 할지말지를 결정
	SCollisionProfile* profile0 = comp0->GetCollisionProfile();
	SCollisionProfile* profileMouse = m_physicsSystem->m_cachedCollisionManager->FindProfile("Mouse");


	//Is Enable pair?
	if (!profile0->Enable || !profileMouse->Enable)
		return false;

	//Is Ignore each other ??
	if (profile0->vecCollisionInteraction[(int32)profileMouse->Channel->Channel] == ECollision_Interaction::Ignore)
	{
		return false;
	}

	return true;
}


bool CustomDispathcer::needsCollision(const btCollisionObject* body0, const btCollisionObject* body1)
{
	//bool bResult = btCollisionDispatcher::needsCollision(body0, body1);

	PrimitiveComponent* comp0 = (PrimitiveComponent*)body0->getUserPointer();
	PrimitiveComponent* comp1 = (PrimitiveComponent*)body1->getUserPointer();

	//충돌 할지말지를 결정
	SCollisionProfile* profile0 = comp0->GetCollisionProfile();
	SCollisionProfile* profile1 = comp1->GetCollisionProfile();

	//Is Enable pair?
	if (!profile0->Enable || !profile1->Enable)
		return false;

	//Is Ignore each other ??
	if (profile0->vecCollisionInteraction[(int32)profile1->Channel->Channel] == ECollision_Interaction::Ignore ||
		profile1->vecCollisionInteraction[(int32)profile0->Channel->Channel] == ECollision_Interaction::Ignore)
	{
		return false;
	}

	return true;
}

