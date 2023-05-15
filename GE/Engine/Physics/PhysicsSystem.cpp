#include "EnginePCH.h"
#include "PhysicsSystem.h"
#include "Renderer/Renderer.h"
#include "PhysicsDebugDraw.h"
#include "Assets/Scene/Scene.h"
#include "RenderManager.h"
#include "GameFramework/Components/ShapeComponent.h"
#include <GameFramework/Components/PrimitiveComponent.h>
#include <SceneManager.h>
#include "CollisionManager.h"
#include "PhysicsMisc.h"

#pragma warning( push )
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )

#include "BulletCollision/BroadphaseCollision/btDbvtBroadphase.h"
#include "BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"
#include "BulletCollision/CollisionDispatch/btCollisionDispatcher.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletPhysicsHelper.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h"
#include "BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h"


#pragma warning( pop )


void PhysicsSystem::TriggerCollision()
{
	PrimitiveComponent* bodyA = nullptr;
	PrimitiveComponent* bodyB = nullptr;
	ContactEvent* currentContactEvent = nullptr;

	if (m_World)
		m_World->performDiscreteCollisionDetection();

	int32 numManifolds = m_World->getDispatcher()->getNumManifolds();
	for (int32 i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = m_World->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();


		bodyA = nullptr;
		bodyB = nullptr;

		int32 numContacts = contactManifold->getNumContacts();

		FContactEventsMap::iterator stayContactEvent = m_StayContactEvents.find(std::make_pair(obA, obB));
		if (stayContactEvent != m_StayContactEvents.end())
		{
			stayContactEvent->second->Type =ContactEventType::ContactStay;
			stayContactEvent->second->Points.clear();

			currentContactEvent = stayContactEvent->second;
		} 
		else
		{
			bodyA = (obA->getUserPointer()) ? static_cast<PrimitiveComponent*>(obA->getUserPointer()) : nullptr;
			bodyB = (obB->getUserPointer()) ? static_cast<PrimitiveComponent*>(obB->getUserPointer()) : nullptr;

			if (!bodyA || !bodyB)
				continue;

			if (!bodyA->IsValidate() || !bodyB->IsValidate())
				continue;

			if (bodyA->GetCollisionReportMode() == ECollisionReportMode::None
				&& bodyB->GetCollisionReportMode() == ECollisionReportMode::None)
				continue;

			ContactEvent* beginEvent = new ContactEvent();
			beginEvent->Type = ContactEventType::ContactBegin;
			beginEvent->BodyA = bodyA;
			beginEvent->BodyB = bodyB;
			FContactEventsMap::iterator find_iter = m_BeginContactEvents.find(std::make_pair(obA, obB));

			if(find_iter == m_BeginContactEvents.end())
			{
				m_BeginContactEvents.insert(std::pair<FContactEventKey, ContactEvent*>(std::make_pair(obA, obB), beginEvent));
				currentContactEvent = beginEvent;
			}
			else
			{
				delete beginEvent;
				currentContactEvent = (*find_iter).second;
			}
			
			
		}

		
		for (int32 j = 0; j < numContacts; j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);

			if (currentContactEvent)
			{
				ContactPoint point;

				point.PositionA = ToVector3(pt.getPositionWorldOnA());
				point.PositionB = ToVector3(pt.getPositionWorldOnB());
				point.Normal = ToVector3(pt.m_normalWorldOnB);
				point.Impulse = (float)pt.getAppliedImpulse();
				point.Distance = (float)pt.getDistance();

				currentContactEvent->Points.push_back(point);
			}
		}
	}

	for (auto it = m_StayContactEvents.begin(); it != m_StayContactEvents.end(); )
	{
		if (!it->second->BodyA->IsValidate() || !it->second->BodyB->IsValidate())
			it->second->Type = ContactEventType::ContactBegin;

		if (it->second->Type == ContactEventType::ContactBegin)
		{
			m_EndContactEvents.insert(std::pair<FContactEventKey, ContactEvent*>(std::make_pair(it->first.first, it->first.second), it->second));
			it->second->Type = ContactEventType::ContactEnd;		

			it = m_StayContactEvents.erase(it);
			continue;
		}

		it++;
	}
}

void PhysicsSystem::ReportCollision()
{
	SCollisionResult data;

	auto NotifyContact = [&](PrimitiveComponent* body, ContactEvent* evt)
	{
		ECollisionReportMode mode = body->GetCollisionReportMode();
		if (mode == ECollisionReportMode::None)
			return;

		data.Src = evt->BodyA;
		data.Dest = evt->BodyB;
		data.ConstactPoint = evt->Points;			


		switch (evt->Type)
		{
		case ContactEventType::ContactBegin:
			evt->BodyA->BeginOverlap(data);
			data.Src = evt->BodyB;
			data.Dest = evt->BodyA;
			evt->BodyB->BeginOverlap(data);
			break;
		case ContactEventType::ContactStay:
			//if (mode == ECollisionReportMode::ReportPersistent)
				//body->OnCollisionStay(data);
			break;
		case ContactEventType::ContactEnd:
			evt->BodyA->EndOverlap(data);
			data.Src = evt->BodyB;
			data.Dest = evt->BodyA;
			evt->BodyB->EndOverlap(data);
			break;
		}
	};

	for (auto& evt : m_StayContactEvents)
	{
		NotifyContact(evt.second->BodyA, evt.second);
		evt.second->Type = ContactEventType::ContactBegin;
	}

	for (auto& evt : m_BeginContactEvents)
	{
		NotifyContact(evt.second->BodyA, evt.second);
		m_StayContactEvents.insert(std::pair<FContactEventKey, ContactEvent*>(
			std::make_pair(evt.first.first, evt.first.second), evt.second));
	}

	for (auto& evt : m_EndContactEvents)
	{
		NotifyContact(evt.second->BodyA, evt.second);
		SAFE_DELETE(evt.second);
	}

	m_EndContactEvents.clear();
	m_BeginContactEvents.clear();
}

PhysicsSystem::PhysicsSystem()
	:m_OverlappingPairCache(nullptr)
	,m_CollisionConfiguration(nullptr)
	,m_Dispatcher(nullptr)
	,m_Solver(nullptr)
	,m_World(nullptr)
	,m_Debug_draw(nullptr)
	, m_Updating(false)
	, m_Gravity(SVector3(0,-9.8f,0))
{
	
}



PhysicsSystem::~PhysicsSystem()
{	
	ClearEvents();
	SAFE_DELETE(m_World);	
	SAFE_DELETE(m_Solver);
	SAFE_DELETE(m_Dispatcher);
	SAFE_DELETE(m_CollisionConfiguration);	
	SAFE_DELETE(m_OverlappingPairCache);	
	SAFE_DELETE(m_GhostPairCallback);	
	SAFE_DELETE(m_Debug_draw);	
	SAFE_DELETE(m_FilterCallback);

	SAFE_DELETE(m_EditorWorld);
	SAFE_DELETE(m_editorOnly);
	SAFE_DELETE(m_editorOnlyDisptcher);	
		
	
}

void PhysicsSystem::Initialize(class RenderManager* renderManger, class CollisionManager* ColMgr)
{
	m_cachedCollisionManager = ColMgr;
	m_CollisionConfiguration	= new btDefaultCollisionConfiguration();
	m_Dispatcher				= new CustomDispathcer(m_CollisionConfiguration);
	m_OverlappingPairCache		= new btDbvtBroadphase();
	m_Solver					= new btSequentialImpulseConstraintSolver();	

	//btGImpactCollisionAlgorithm::registerAlgorithm(m_Dispatcher);

	// Create the world.
	m_World = new btDiscreteDynamicsWorld(m_Dispatcher, m_OverlappingPairCache, m_Solver, m_CollisionConfiguration);
	m_World->setGravity(ToBtVector3(m_Gravity));

	// Register ghost pair callback so bullet detects collisions with ghost objects (used for character collisions).	
	m_GhostPairCallback = new btGhostPairCallback();
	m_OverlappingPairCache->getOverlappingPairCache()->setInternalGhostPairCallback(m_GhostPairCallback);

	m_World->getDispatchInfo().m_allowedCcdPenetration = 0.0001f;
	// Set up debug drawing.
	m_Debug_draw = new PhysicsDebugDraw(renderManger);
	m_World->setDebugDrawer(m_Debug_draw);	

	m_World->setWorldUserInfo(this);
	m_FilterCallback = new CollisionFilterCallback(this);
	m_World->getPairCache()->setOverlapFilterCallback(m_FilterCallback);	


	m_editorOnly = new btDbvtBroadphase();
	m_editorOnlyDisptcher = new btCollisionDispatcher(m_CollisionConfiguration);
	m_EditorWorld = new btCollisionWorld(m_editorOnlyDisptcher, m_editorOnly, m_CollisionConfiguration);
	m_EditorWorld->setDebugDrawer(m_Debug_draw);
	//Just debugDraw
	
}

void PhysicsSystem::Update(float DeltaSeconds)
{
	//if (g_Engine->IsEditMode())
		//m_World->debugDrawWorld();

	m_World->setWorldUserInfo(this);

	Scene* currentScene = g_Engine->Get<SceneManager>()->GetCurrentScene();

	if (!currentScene)
		return;
	if (!currentScene->IsBegan())
	{
		m_World->updateAabbs();
		m_EditorWorld->updateAabbs();
		return;
	}
	

	float internalTimeStep = 1.0f / _internalFps;
	INT32 maxSubsteps = static_cast<INT32>(DeltaSeconds * _internalFps) + 1;
	if (_maxSubSteps < 0)
	{
		internalTimeStep = DeltaSeconds;
		maxSubsteps = 1;
	}else if(_maxSubSteps > 0)
	{
		maxSubsteps = std::min<INT32>(maxSubsteps, _maxSubSteps);
	}


	m_Updating = true;
	m_World->stepSimulation(internalTimeStep, maxSubsteps, internalTimeStep);
	
	m_Updating = false;

	_deltaTimeSec += DeltaSeconds;
	if (_deltaTimeSec > 1.0f / _internalFps)
	{
		TriggerCollision();
		TriggerCollision();
		ReportCollision();
		_deltaTimeSec = 0.0f;
	}


}

class btDynamicsWorld* PhysicsSystem::GetWorld() const
{
	return m_World;
}



class btCollisionWorld* PhysicsSystem::GetEditorWorld() const
{
	return m_EditorWorld;
}

void PhysicsSystem::ClearEditorWorld()
{
	int32 collisonObjectSize = m_EditorWorld->getNumCollisionObjects();
	btCollisionObjectArray& arr =  m_EditorWorld->getCollisionObjectArray();
	for(int32 i=0;i<arr.size();i++)
	{		
		arr[i]->setUserIndex(INDEX_NONE);
		m_EditorWorld->removeCollisionObject(arr[i]);
	}
	
}

class PhysicsDebugDraw* PhysicsSystem::GetDebugDraw() const
{
	return m_Debug_draw;
}

void PhysicsSystem::DrawEditorWorld()
{
	m_EditorWorld->debugDrawWorld();
}

SHitResult PhysicsSystem::rayTest(const SVector3& Origin, const SVector3& Direction, float distance)
{
	SHitResult result;
	btVector3 rayFromWorld = ToBtVector3(Origin);
	btVector3 rayToWorld(rayFromWorld + ToBtVector3(Direction * distance));					 

	RayTestCallback callback(this,rayFromWorld, rayToWorld);
	m_World->rayTest(rayFromWorld, rayToWorld, callback);
	if (callback.hasHit())
	{
	
		result.HitComponent = (PrimitiveComponent*)callback.m_collisionObject->getUserPointer();
		result.point = SVector3(callback.m_hitPointWorld.x(), callback.m_hitPointWorld.y(), callback.m_hitPointWorld.z());
		result.fraction = callback.m_closestHitFraction;
		result.normal = SVector3(callback.m_hitNormalWorld.x(), callback.m_hitNormalWorld.y(), callback.m_hitNormalWorld.z());
		result.Hit = true;
	}

	return result;

}


void PhysicsSystem::ClearEvents()
{
	//FContactEventsMap m_StayContactEvents;
	//FContactEventsMap m_EndContactEvents;
	{
		FContactEventsMap::iterator iter = m_BeginContactEvents.begin();
		FContactEventsMap::iterator iter_end = m_BeginContactEvents.end();
		for (; iter != iter_end; iter++)
		{
			SAFE_DELETE(iter->second);
		}
	}
	{
		FContactEventsMap::iterator iter = m_StayContactEvents.begin();
		FContactEventsMap::iterator iter_end = m_StayContactEvents.end();
		for (; iter != iter_end; iter++)
		{
			SAFE_DELETE(iter->second);
		}
	}


	{
		FContactEventsMap::iterator iter = m_EndContactEvents.begin();
		FContactEventsMap::iterator iter_end = m_EndContactEvents.end();
		for (; iter != iter_end; iter++)
		{
			SAFE_DELETE(iter->second);
		}
	}


	m_StayContactEvents.clear();
	m_BeginContactEvents.clear();
	m_EndContactEvents.clear();
}
