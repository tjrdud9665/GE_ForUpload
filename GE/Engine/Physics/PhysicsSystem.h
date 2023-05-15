#pragma once
#include "../XObject/XObject.h"

#include "BulletCollision/NarrowPhaseCollision/btManifoldPoint.h"
#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"
#include "BulletCollision/CollisionDispatch/btCollisionObjectWrapper.h"
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "BulletCollision/BroadphaseCollision/btOverlappingPairCache.h"
#include "BulletCollision/CollisionDispatch/btCollisionDispatcher.h"









/** Type of contacts reported by PhysX simulation. */
enum class ContactEventType
{
	ContactBegin,
	ContactStay,
	ContactEnd
};





struct ContactEvent
{	
	TSharedPtr<class PrimitiveComponent> BodyA;
	TSharedPtr<class PrimitiveComponent> BodyB;
	ContactEventType Type = ContactEventType::ContactBegin;	
	std::vector<ContactPoint> Points;	
};


struct SContactEventKeyHash
{
	template <class T1, class T2>
	std::size_t operator() (const std::pair<T1, T2>& pair) const
	{
		return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
	}
};






class PhysicsSystem :public XObject
{
	friend class Globals;
	friend class Engine;
	friend class PhysicsDebugDraw;	
	friend class RayTestCallback;
	using Super = XObject;

	

	using FContactEventKey = std::pair<const btCollisionObject*, const btCollisionObject*>;	
	using FContactEventsMap = std::unordered_map<FContactEventKey, ContactEvent*, SContactEventKeyHash>;


private:
	PhysicsSystem();
	virtual ~PhysicsSystem();

	void Initialize(class RenderManager* renderManger ,class CollisionManager* ColMgr);

	virtual void Update(float DeltaSeconds) override;
		
	

public:	
	class btDynamicsWorld* GetWorld() const;	
	class PhysicsDebugDraw* GetDebugDraw() const;


	class btCollisionWorld* GetEditorWorld() const;

	void ClearEditorWorld();

	
	void DrawEditorWorld();

public:
	SHitResult rayTest(const SVector3& Origin,const SVector3& Direction, float distance);
	
	

private:	

	void TriggerCollision();
	void ReportCollision();

	void ClearEvents();

private:
	bool										m_Updating;
	//Phy Bullet Objects
	class btBroadphaseInterface*				m_OverlappingPairCache;
	class btDefaultCollisionConfiguration*		m_CollisionConfiguration;
	class btCollisionDispatcher*				m_Dispatcher;
	class btSequentialImpulseConstraintSolver*	m_Solver;
	class btDynamicsWorld*						m_World;		
	class PhysicsDebugDraw*						m_Debug_draw;
	class btGhostPairCallback*					m_GhostPairCallback;
	
	SVector3									m_Gravity;


	//Don't Simulate					Only Draw
	class btCollisionWorld*							m_EditorWorld;
	class btBroadphaseInterface*					m_editorOnly;
	class btCollisionDispatcher*					m_editorOnlyDisptcher;

	INT32 _maxSubSteps = 1;
	UINT32 _maxSolveIterations = 256;
	float _internalFps = 60.0f;	
	float _deltaTimeSec = 1.0f;

private:
	FContactEventsMap m_BeginContactEvents;
	FContactEventsMap m_StayContactEvents;
	FContactEventsMap m_EndContactEvents;

	class CollisionFilterCallback* m_FilterCallback;

	class CollisionManager* m_cachedCollisionManager;

};

