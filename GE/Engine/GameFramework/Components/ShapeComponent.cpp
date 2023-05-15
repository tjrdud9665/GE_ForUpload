#include "EnginePCH.h"
#include "Assets/Scene/Scene.h"
#include "ShapeComponent.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "Physics/BulletPhysicsHelper.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include <Physics/PhysicsSystem.h>
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include <Math/MathUtils.h>
#include "BulletCollision/CollisionShapes/btConeShape.h"
#include "BulletCollision/CollisionShapes/btCapsuleShape.h"
#include "BulletCollision/CollisionShapes/btCylinderShape.h"
#include "BulletCollision/CollisionShapes/btStaticPlaneShape.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "LinearMath/btDefaultMotionState.h"
#include "BulletCollision/BroadphaseCollision/btBroadphaseProxy.h"
#include <SceneManager.h>

ShapeComponent::ShapeComponent()	
{
}

ShapeComponent::ShapeComponent(const ShapeComponent& Other)
	:Super(Other)	
{		
	
}

ShapeComponent::~ShapeComponent()
{

}

void ShapeComponent::PreUpdate(float DeltaSeconds)
{
	Super::PreUpdate(DeltaSeconds);
	//Create Shape 함수가 override 가 안됨..
	if (m_DirtyShape && (!m_GhostBody && !m_Body))
	{
		ReInitCollisionAndPhysics();
	}

}




