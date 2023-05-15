#pragma once
#include "BulletCollision/BroadphaseCollision/btOverlappingPairCache.h"
#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"


class CollisionCallback : public btCollisionWorld::ContactResultCallback
{
	friend class PhysicsSystem;
public:

	CollisionCallback(class PhysicsSystem* Psystem)
		: m_System(Psystem)
		, bCollision(false)
		, m_closestDistanceThreshold(0.01f)
	{

	}

protected:
	/**
		* Internal function used for Bullet integration (do not use or override).
		*/
	btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* a, int partIdA, int indexA, const btCollisionObjectWrapper* b, int partIdB, int indexB);

private:
	float m_closestDistanceThreshold;
	bool bCollision;
	class PhysicsSystem* m_System;
};


class CollisionFilterCallback : public btOverlapFilterCallback
{
public:
	CollisionFilterCallback(class PhysicsSystem* _system)
		:m_physicsSystem(_system)
	{

	}

	virtual ~CollisionFilterCallback()
	{}
	// return true when pairs need collision
	virtual bool	needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const override;

private:
	class PhysicsSystem* m_physicsSystem;
};

class CustomDispathcer : public btCollisionDispatcher
{
public:
	CustomDispathcer(class btCollisionConfiguration* collisionConfiguration)
		:btCollisionDispatcher(collisionConfiguration)
	{}


	virtual bool	needsCollision(const btCollisionObject* body0, const btCollisionObject* body1) override;

	virtual bool	needsResponse(const btCollisionObject* body0, const btCollisionObject* body1)	override;
};



class RayTestCallback : public btCollisionWorld::ClosestRayResultCallback
{

public:
	RayTestCallback(class PhysicsSystem* _phySys, const btVector3& rayFromWorld, const btVector3& rayToWorld)
		: btCollisionWorld::ClosestRayResultCallback(rayFromWorld, rayToWorld)
		, m_physicsSystem(_phySys)
	{}

	virtual bool needsCollision(btBroadphaseProxy* proxy0) const override;

private:
	class PhysicsSystem* m_physicsSystem;
};



