#include "EnginePCH.h"
#include "PrimitiveComponent.h"
#include "../../Renderer/Renderer.h"

#include <Physics/BulletPhysicsHelper.h>
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "BulletCollision/CollisionShapes/btCollisionShape.h"
#include <Physics/PhysicsSystem.h>
#include <CollisionManager.h>
#include <Assets/Scene/Scene.h>
#include <Physics/PhysicsDebugDraw.h>




/**
 * MeshAsset 이 할당되었다면 1+
 * Gravity;
 * Position_lock;
 * Rotation_lock;
 * Center_of_mass;
 */
PrimitiveComponent::PrimitiveComponent()
	:m_InstanceID(INDEX_NONE)
	, Gravity(SVector3(0.0f, -9.8f, 0.0f))
	, Rotation_lock(SVector3(0.0f))
	, Center_of_mass(SVector3(0.0f))
	, UsePhysicsSimulation(false)
	, Mass(100.0)
	, m_InteranlCallback(nullptr)
	, m_DirtyShape(true)
	, CastShadow(true)
	, m_meshInterface(nullptr)
{
	m_Result.Src = this;
	m_OverlapCount = 0;
	m_CollisionReportMode = ECollisionReportMode::Report;


}

PrimitiveComponent::PrimitiveComponent(const PrimitiveComponent& Other)
	:Super(Other)
	, m_MeshAsset(Other.m_MeshAsset)
	, m_prevMeshAsset(nullptr)
	, m_CollisionReportMode(Other.m_CollisionReportMode)
	, Gravity(Other.Gravity)
	, Rotation_lock(Other.Rotation_lock)
	, Center_of_mass(Other.Center_of_mass)
	, UsePhysicsSimulation(Other.UsePhysicsSimulation)
	, Mass(Other.Mass)
	, m_DirtyShape(true)
	, CastShadow(Other.CastShadow)
	, m_meshInterface(nullptr)
{
	m_InteranlCallback = new btGhostPairCallback;
	m_OverlapCount = 0;
	//Mesh Enterence
	if (m_MeshAsset)
	{
		//m_MeshAsset->AddRenderCount();
		m_MeshAsset->GetMaterials(OUT m_vecMaterial);
	}
	m_cachedPhsics = g_Engine->Get<PhysicsSystem>();

	if (Other.IsConstructed())
	{
		if (Other.m_Profile->Name == "Custom")
		{
			CollisionManager* colMgr = g_Engine->Get<CollisionManager>();
			m_Profile = colMgr->CreateCustomProfile(this, Other.m_Profile);
		}
		else
		{
			m_Profile = Other.m_Profile;
		}

		ReInitCollisionAndPhysics();
	}
}

PrimitiveComponent::~PrimitiveComponent()
{
	if (m_MeshAsset)
		m_MeshAsset->AddRenderCount(false);

	if (m_cachedPhsics)
	{
		DeInitCollisionAndPhysics();
	}

	SAFE_DELETE(m_GhostBody);

	SAFE_DELETE(m_Body);
	SAFE_DELETE(m_Shape);
	SAFE_DELETE(m_InteranlCallback);


	if (g_Engine)
	{
		CollisionManager* colMgr = g_Engine->Get<CollisionManager>();
		if (colMgr)
		{
			colMgr->ReturnProfile(this);
		}
	}
}

void PrimitiveComponent::BeginOverlap(const SCollisionResult& collisionData)
{
	m_Result = collisionData;

	m_OverlapCount++;

	OnBeginOverlap(m_Result);
	size_t beinOverlapFuncCnt = m_OnBeginOverlaps.size();
	for (size_t i = 0; i < beinOverlapFuncCnt; i++)
	{
		m_OnBeginOverlaps[i](m_Result);
	}

}

void PrimitiveComponent::EndOverlap(const SCollisionResult& collisionData)
{
	m_Result = collisionData;

	m_OverlapCount--;

	OnEndOverlap(m_Result);

	size_t endOverlapFuncCnt = m_OnEndOverlaps.size();
	for (size_t i = 0; i < endOverlapFuncCnt; i++)
	{
		m_OnEndOverlaps[i](m_Result);
	}


}

int32 PrimitiveComponent::BindOnBeginOverlap(std::function<void(const SCollisionResult&)> Func)
{
	m_OnBeginOverlaps.push_back(Func);

	return (int32)m_OnBeginOverlaps.size();
}

int32 PrimitiveComponent::BindOnEndOverlap(std::function<void(const SCollisionResult&)> Func)
{
	m_OnEndOverlaps.push_back(Func);

	return (int32)m_OnEndOverlaps.size();
}

void PrimitiveComponent::ClearBeginOverlap()
{
	m_OnBeginOverlaps.clear();
}

void PrimitiveComponent::ClearEndOverlap()
{
	m_OnEndOverlaps.clear();
}

void PrimitiveComponent::SetCollisionProfile(const std::string& InName)
{
	m_Profile = g_Engine->Get<CollisionManager>()->RentalProfile(this, InName);
}

void PrimitiveComponent::OnProfileLost(SCollisionProfile* InDefaultProfile)
{
	m_Profile = InDefaultProfile;
}

void PrimitiveComponent::SetUsePhysicsSimulation(bool bUseSimulation)
{
	UsePhysicsSimulation = bUseSimulation;
}

void PrimitiveComponent::SetCollisionFlags(int32 Flag)
{
	if (m_GhostBody)
		m_GhostBody->setCollisionFlags(m_GhostBody->getCollisionFlags() | Flag);

}


bool PrimitiveComponent::GetAABB(SVector3& outBMin, SVector3& outBMax)
{
	if (m_Shape)
	{
		btTransform tr;
		tr.setIdentity();
		tr.setRotation(ToBtQuaternion(m_ComponentToWorld.Rotation));
		tr.setOrigin(ToBtVector3(m_ComponentToWorld.Translation));
		btVector3 aabbMin;
		btVector3 aabbMax;

		m_Shape->getAabb(tr, OUT aabbMin, OUT aabbMax);
		outBMin = ToVector3(aabbMin);
		outBMax = ToVector3(aabbMax);


		return true;
	}
	else
	{
		if (m_MeshAsset)
		{
			m_MeshAsset->GetAABB(outBMin, outBMax);
			outBMin = outBMin.TransformCoord(m_ComponentToWorld.ToMatrixWithScale());
			outBMax = outBMax.TransformCoord(m_ComponentToWorld.ToMatrixWithScale());


		}

		return true;
	}


	return false;
}

void PrimitiveComponent::BeginPlay()
{
	Super::BeginPlay();
	ReInitCollisionAndPhysics();

	PhysicsBeginPlay();

}

void PrimitiveComponent::PhysicsBeginPlay()
{
	btTransform PhysicsTransform;
	PhysicsTransform.setIdentity();
	PhysicsTransform.setRotation(ToBtQuaternion(m_ComponentToWorld.Rotation));
	PhysicsTransform.setOrigin(ToBtVector3(m_ComponentToWorld.Translation));
	bool ExistRigidBody = false;
	bool ExistGhostBody = false;

	if (m_Body)
	{
		ExistRigidBody = true;
		m_Body->setWorldTransform(PhysicsTransform);
	}
	if (m_GhostBody)
	{
		ExistGhostBody = true;
		m_GhostBody->setWorldTransform(PhysicsTransform);
	}

	Scene* owningScene = GetOwningScene();

	if (owningScene)
	{
		if (UsePhysicsSimulation)
		{
			//RigidBody 가 World 에 추가 되고 Ghost Body 가 world 에서 빠진다
			if (ExistRigidBody)
			{
				if (!m_Body->isInWorld())
				{
					m_cachedPhsics->GetWorld()->addRigidBody(m_Body);
				}
				if (IsPlaying())
				{
					m_Body->setGravity(ToBtVector3(Gravity));
					m_Body->activate();
				}
				else
				{
					m_Body->setActivationState(WANTS_DEACTIVATION);
				}
			}

			if (ExistGhostBody)
			{
				if (m_GhostBody->getUserIndex() != INDEX_NONE)
				{
					m_cachedPhsics->GetWorld()->removeCollisionObject(m_GhostBody);
					m_GhostBody->setUserIndex(INDEX_NONE);
				}
			}

		}
		else
		{
			if (ExistRigidBody)
			{
				//RigidBody 가 World 에 빠지고 Ghost Body 가 world 에추가된다
				if (m_Body->isInWorld())
				{
					m_cachedPhsics->GetWorld()->removeRigidBody(m_Body);
				}
			}

			if (ExistGhostBody)
			{
				if (m_GhostBody->getUserIndex() == INDEX_NONE)
				{
					m_cachedPhsics->GetWorld()->addCollisionObject(m_GhostBody);
					m_GhostBody->setUserIndex(1);
				}
			}


		}

	}
}

void PrimitiveComponent::EndPlay()
{
	Super::EndPlay();

	DeInitCollisionAndPhysics();

	ClearBeginOverlap();
	ClearEndOverlap();



}

void PrimitiveComponent::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);

	//if (m_MeshAsset)
	//{
	////	m_MeshAsset->AddRenderCount();
	//}

	m_cachedPhsics = g_Engine->Get<PhysicsSystem>();
	SetCollisionProfile("Default");
	//CreateShape();
	InitCollisionAndPhysics();

}

void PrimitiveComponent::Render(class Renderer* pRenderer)
{
	Super::Render(pRenderer);
	//TODO :이곳에서 랜더링과 바인딩을하는것이 아닌
	//Context 또는 PerFrame 형태로 Renderer 에 전달하는방식으로 처리할것.

	m_InstanceID = INDEX_NONE;

	if (m_MeshAsset)
	{
		int32 MeshCount = m_MeshAsset->GetMeshCount();

		for (int32 i = 0; i < MeshCount; i++)
		{
			size_t pairCount = m_MeshAsset->GetMesh(i).GetIAPairSize();

			for (size_t a = 0; a < pairCount; a++)
			{
				FIAPair iaPair = m_MeshAsset->GetMesh(i).GetIAPair((int32)a);
				pRenderer->SetVertexBuffer(iaPair.first);
				pRenderer->SetIndexBuffer(iaPair.second);

				Material* slotMaterial = m_MeshAsset->GetMaterial((int32)a);

				if (m_vecMaterial.size() > a)
				{
					if (m_vecMaterial[a])
						slotMaterial = m_vecMaterial[a];
				}

				if (slotMaterial)
				{
					slotMaterial->SetMaterialConstants(pRenderer, EShaders::FORWARD_PHONG);
				}
				AdditionalBindPerIA(pRenderer);
				pRenderer->Apply();
				pRenderer->DrawIndexed();
			}
		}
	}


}


void PrimitiveComponent::GatherRenderInstance(class Renderer* pRenderer, std::vector<SInstanceInfo>& InstanceInfos, int32 InstanceID)
{
	Super::GatherRenderInstance(pRenderer, InstanceInfos, InstanceID);

	if (m_MeshAsset)
	{
		int32 MeshCount = m_MeshAsset->GetMeshCount();

		m_InstanceID = InstanceID;

		for (int32 i = 0; i < MeshCount; i++)
		{
			size_t pairCount = m_MeshAsset->GetMesh(i).GetIAPairSize();
			for (size_t a = 0; a < pairCount; a++)
			{

				//FIAPair iaPair = m_MeshAsset->GetMesh(i).GetIAPair(a);
				//pRenderer->SetVertexBuffer(iaPair.first);
				//pRenderer->SetIndexBuffer(iaPair.second);
				Material* slotMaterial = m_MeshAsset->GetMaterial((int32)a);

				slotMaterial = m_vecMaterial[a];

				if (slotMaterial)
				{			

					InstanceInfos[InstanceID].objSurface = slotMaterial->GetCBufferData();

					//slotMaterial->SetMaterialConstants(pRenderer, EShaders::FORWARD_PHONG);
				}
			}

			//pRenderer->Apply();

		}


	}
}

void PrimitiveComponent::RenderDepth(class Renderer* pRenderer, const Matrix& ViewProj)
{
	Super::RenderDepth(pRenderer, ViewProj);

	if (m_MeshAsset)
	{
		int32 MeshCount = m_MeshAsset->GetMeshCount();

		for (int32 i = 0; i < MeshCount; i++)
		{
			size_t pairCount = m_MeshAsset->GetMesh(i).GetIAPairSize();

			for (size_t a = 0; a < pairCount; a++)
			{
				FIAPair iaPair = m_MeshAsset->GetMesh(i).GetIAPair((int32)a);
				pRenderer->SetVertexBuffer(iaPair.first);
				pRenderer->SetIndexBuffer(iaPair.second);

				Material* slotMaterial = m_MeshAsset->GetMaterial((int32)a);

				AdditionalBindPerIA(pRenderer);
				pRenderer->Apply();
				pRenderer->DrawIndexed();
			}
		}
	}
}



void PrimitiveComponent::OnUpdateTransform()
{
	if (!m_Profile)
		return;

	if (!m_Profile->Enable)
		return;

	
	btTransform PhysicsTransform;
	PhysicsTransform.setIdentity();
	PhysicsTransform.setRotation(ToBtQuaternion(m_ComponentToWorld.Rotation));
	PhysicsTransform.setOrigin(ToBtVector3(m_ComponentToWorld.Translation));
	bool ExistRigidBody = false;
	bool ExistGhostBody = false;

	if (m_Body)
	{
		ExistRigidBody = true;
		m_Body->setWorldTransform(PhysicsTransform);
	}
	if (m_GhostBody)
	{
		ExistGhostBody = true;
		m_GhostBody->setWorldTransform(PhysicsTransform);
	}

	if (m_Shape)
	{
		m_Shape->setLocalScaling(ToBtVector3(m_ComponentToWorld.Scale3D));
	}

}

void PrimitiveComponent::PreUpdate(float DeltaSeconds)
{
	Super::PreUpdate(DeltaSeconds);
	//Mesh Enterence
	if (m_MeshAsset != m_prevMeshAsset)
	{
		m_vecMaterial.clear();

// 		if (m_prevMeshAsset)
// 			m_prevMeshAsset->AddRenderCount(false);

		//if(m_MeshAsset)
		//	m_MeshAsset->AddRenderCount(true);

		m_prevMeshAsset = m_MeshAsset;

		if (m_MeshAsset)
		{
			m_MeshAsset->GetMaterials(OUT m_vecMaterial);
		}

		//OnChangedMeshAsset();
		m_DirtyShape = true;
		ReInitCollisionAndPhysics();
	}
}

void PrimitiveComponent::PostUpdate(float DeltaSeconds)
{
	Super::PostUpdate(DeltaSeconds);
	bool ExistRigidBody = false;
	bool ExistGhostBody = false;

	if (m_Body)
	{
		ExistRigidBody = true;
	}
	if (m_GhostBody)
	{
		ExistGhostBody = true;
	}

	Scene* owningScene = GetOwningScene();

	if (owningScene)
	{
		if (UsePhysicsSimulation)
		{
			//RigidBody 가 World 에 추가 되고 Ghost Body 가 world 에서 빠진다
			if (ExistRigidBody)
			{
				if (!m_Body->isInWorld())
				{
					m_cachedPhsics->GetWorld()->addRigidBody(m_Body);
				}
				if (IsPlaying())
				{
					m_Body->setGravity(ToBtVector3(Gravity));
					m_Body->activate();
				}
				else
				{
					m_Body->setActivationState(WANTS_DEACTIVATION);
				}
			}

			if (ExistGhostBody)
			{
				if (m_GhostBody->getUserIndex() != INDEX_NONE)
				{
					m_cachedPhsics->GetWorld()->removeCollisionObject(m_GhostBody);
					m_GhostBody->setUserIndex(INDEX_NONE);
				}
			}

		}
		else
		{
			if (ExistRigidBody)
			{
				//RigidBody 가 World 에 빠지고 Ghost Body 가 world 에추가된다
				if (m_Body->isInWorld())
				{
					m_cachedPhsics->GetWorld()->removeRigidBody(m_Body);
				}
			}

			if (ExistGhostBody)
			{
				if (m_GhostBody->getUserIndex() == INDEX_NONE)
				{
					m_cachedPhsics->GetWorld()->addCollisionObject(m_GhostBody);
					m_GhostBody->setUserIndex(1);
				}
			}
		}
	}
	//Editor Only Flow
	else
	{
		if (ExistGhostBody)
		{
			if (m_GhostBody->getUserIndex() == INDEX_NONE)
			{
				m_cachedPhsics->GetEditorWorld()->addCollisionObject(m_GhostBody);
				m_GhostBody->setActivationState(WANTS_DEACTIVATION);
				m_GhostBody->setUserIndex(1);
			}
		}
	}

}

size_t PrimitiveComponent::GetIAPair(std::vector<FIAPair>& OutvecIAPairs) const
{
	if (m_MeshAsset)
	{
		int32 count = m_MeshAsset->GetMeshCount();
		for (int32 i = 0; i < count; i++)
		{
			size_t pairSize = m_MeshAsset->GetMesh(i).GetIAPairSize();
			for (int32 j = 0; j < pairSize; j++)
			{
				OutvecIAPairs.push_back(m_MeshAsset->GetMesh(i).GetIAPair(j));
			}
		}
	}

	return OutvecIAPairs.size();
}

class MeshAsset* PrimitiveComponent::GetMeshAsset() const
{
	return m_MeshAsset;
}



Material* PrimitiveComponent::GetMaterial(int32 idx)
{
	if (m_vecMaterial.size() <= idx)
		return nullptr;
	
	return m_vecMaterial[idx];
}

bool PrimitiveComponent::Serialize(Archive& ar)
{
	bool Result = Super::Serialize(ar);
	ar.BeginObject(GetName().c_str());
	bool IsCustomProfile = false;
	if (ar.IsLoading())
	{
		ar.Serialize("IsCustomProfile", IsCustomProfile);
		if (IsCustomProfile)
		{
			m_Profile = g_Engine->Get<CollisionManager>()->CreateCustomProfile(this);
			ar.Serialize("CustomProfile", m_Profile);
		}
	}
	else
	{
		bool IsCustomProfile = (g_Engine->Get<CollisionManager>()->FindCustomProfile(this) != nullptr);
		ar.Serialize("IsCustomProfile", IsCustomProfile);

		if (IsCustomProfile)
		{
			ar.Serialize("CustomProfile", m_Profile);
		}

	}
	ar.EndObject();
	return Result;
}

void PrimitiveComponent::SetMeshAsset(class MeshAsset* InMeshAsset)
{
	//PreUpdate 에서 RenderCount 를 수정

	if (m_MeshAsset != InMeshAsset)
	{
		m_vecMaterial.clear();

		if (m_MeshAsset)
			m_prevMeshAsset->AddRenderCount(false);

		if (InMeshAsset)
			InMeshAsset->AddRenderCount(true);

		m_MeshAsset = InMeshAsset;

		if (m_MeshAsset)
		{
			m_MeshAsset->GetMaterials(OUT m_vecMaterial);
		}

		//OnChangedMeshAsset();
		m_DirtyShape = true;
		ReInitCollisionAndPhysics();
	}

	//m_MeshAsset = InMeshAsset;

	//if (m_MeshAsset)
	//{		
	//	m_MeshAsset->GetMaterials(OUT m_vecMaterial);
	//}

}

void PrimitiveComponent::InitCollisionAndPhysics()
{
	if (m_DirtyShape)
	{
		CreateShape();
	}

	if (!m_Shape)
		return;

	if (m_Body || m_GhostBody)
		return;

	UpdateComponentToWorld();
	btTransform PhysicsTransform;
	PhysicsTransform.setIdentity();
	PhysicsTransform.setOrigin(ToBtVector3(m_ComponentToWorld.Translation));
	PhysicsTransform.setRotation(ToBtQuaternion(m_ComponentToWorld.Rotation));


	btVector3 localInertia = btVector3(0, 0, 0);
	MotionState* myMotionState = new MotionState(this);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(Mass, myMotionState, m_Shape, localInertia);
	m_Body = new btRigidBody(rbInfo);
	m_Body->setUserPointer(this);
	m_Body->setRestitution(0.8f);
	//m_Body->setWorldTransform(PhysicsTransform);
	m_Body->setInterpolationWorldTransform(PhysicsTransform);
	m_Body->clearForces();
	m_Body->setCcdMotionThreshold(1.0f);

	m_GhostBody = new btPairCachingGhostObject();
	m_GhostBody->setUserPointer(this);
	m_GhostBody->setCollisionShape(m_Shape);

}

void PrimitiveComponent::DeInitCollisionAndPhysics()
{
	if (GetOwningScene())
	{

		m_DirtyShape = true;

		btDynamicsWorld* world = m_cachedPhsics->GetWorld();
		if (m_Body != nullptr)
		{
			world->removeRigidBody(m_Body);
			delete m_Body->getMotionState();
			SAFE_DELETE(m_Body);
		}
		if (m_GhostBody != nullptr)
		{
			world->removeCollisionObject(m_GhostBody);
			SAFE_DELETE(m_GhostBody);
		}
	}
	else
	{
		btCollisionWorld* world = m_cachedPhsics->GetEditorWorld();

		if (m_GhostBody != nullptr)
		{
			world->removeCollisionObject(m_GhostBody);
			m_GhostBody->setUserIndex(INDEX_NONE);
			SAFE_DELETE(m_GhostBody);
		}

		if (m_Body)
		{
			delete m_Body->getMotionState();
			SAFE_DELETE(m_Body);
		}

	}
}

void PrimitiveComponent::ReInitCollisionAndPhysics()
{
	DeInitCollisionAndPhysics();
	InitCollisionAndPhysics();
	OnUpdateTransform();
}

int32 PrimitiveComponent::GetInstanceID() const
{
	return m_InstanceID;
}

void MotionState::getWorldTransform(btTransform& worldTrans) const
{
	PrimitiveComponent* owner = m_BodyOwner;
	if (!owner->GetRigidBody() && !owner->GetGhostBody())
		return;
	if (!owner->IsValidate())
		return;

	const SVector3 lastPos = owner->GetWorldLocation();
	const SVector4 lastRot = SVector4::VectorQuaternionRotateVector(owner->GetComponentTransform().Rotation, owner->Center_of_mass);

	//
	//
	worldTrans.setOrigin(ToBtVector3(lastPos + lastRot));
	worldTrans.setRotation(ToBtQuaternion(lastRot));
}

void MotionState::setWorldTransform(const btTransform& worldTrans)
{
	PrimitiveComponent* owner = m_BodyOwner;
	if (!owner->GetRigidBody() && !owner->GetGhostBody())
		return;
	if (!owner->IsValidate())
		return;



	const SVector4 newWorldRot = ToQuaternion(worldTrans.getRotation());
	const SVector3 newWorldPos = ToVector3(worldTrans.getOrigin()) - SVector4::VectorQuaternionRotateVector(newWorldRot, owner->Center_of_mass);

	//if(owner->IsUsePhysicsSimulation())	
	{
		owner->SetWorldLocation(newWorldPos);
		owner->SetWorldRotation(newWorldRot.ToEulerAngles());
	}

}
