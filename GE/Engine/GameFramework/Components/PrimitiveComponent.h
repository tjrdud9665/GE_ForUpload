#pragma once
#include "SceneComponent.h"
#include "../../Assets/Mesh/Mesh.h"
#include "../../Assets/Material/Material.h"
#include "../../Assets/Mesh/MeshAsset.h"

#pragma warning( push )
#pragma warning( disable : 5033 )
#include "LinearMath/btMotionState.h"
#pragma warning( pop )
#include <CollisionManager.h>

/*
* MeshAsset의 랜더링 카운트가 올라가야하는 경우
* 현재 장면을 구성하고있는 SceneComponent인 경우
*/

class MotionState : public btMotionState
{
public:
	MotionState(class PrimitiveComponent* comp)
	{
		m_BodyOwner = comp;
	}

	// Update from engine, ENGINE -> BULLET
	virtual void	getWorldTransform(btTransform& worldTrans) const override;

	// Update from bullet, BULLET -> ENGINE
	virtual void  setWorldTransform(const btTransform& worldTrans) override;
private:
	class PrimitiveComponent* m_BodyOwner;
};

enum class ECollisionPreset
{
	PHYSICS, // NO COLLISIONs  Only Ph
	COLLISIONS, // OVERLAP ALL	Only Col 
	COLLISIONS_AND_PHYSICS // Ghost / Phy
};

enum class RigidBodyType
{
	STATIC,
	DYNAMIC,
	KINEMATIC
};

//충돌하고자 하는 다른 객체가 해당객체와 Overlap인가?


class PrimitiveComponent :public SceneComponent
{
	CLASS(PrimitiveComponent, SceneComponent)
	friend class MotionState;
	friend class PhysicsSystem;
	
protected:
	PrimitiveComponent();
	PrimitiveComponent(const PrimitiveComponent& Other);
	virtual ~PrimitiveComponent();

public:
	//Not  virtual 
	void BeginOverlap(const SCollisionResult& collisionData);
	void EndOverlap(const SCollisionResult& collisionData);


	virtual void OnBeginOverlap(const SCollisionResult& Result) 
	{
	
	}

	virtual void OnEndOverlap(const SCollisionResult& Result)
	{
	
	}
	/*
	* INDEX_NONE Invalid
	*/
	int32 BindOnBeginOverlap(std::function<void(const SCollisionResult&)> Func);
	int32 BindOnEndOverlap(std::function<void(const SCollisionResult&)> Func);

	void ClearBeginOverlap();
	void ClearEndOverlap();

	

	int32 GetOverlapingCount() const
	{
		return m_OverlapCount;
	}

	virtual class btRigidBody* GetRigidBody() const
	{
		return m_Body;
	}
	virtual class btPairCachingGhostObject* GetGhostBody() const
	{
		return m_GhostBody;
	}

	class btCollisionShape* GetShape() const 
	{
		return m_Shape; 
	}

	void SetCollisionProfile(const std::string& InName);

	void OnProfileLost(SCollisionProfile* InDefaultProfile);

	SCollisionProfile* GetCollisionProfile()  const
	{
		return m_Profile;
	}



	void SetCollisionReportMode(ECollisionReportMode mode) 
	{
		m_CollisionReportMode = mode;
	}

	bool IsUsePhysicsSimulation() const
	{
		return UsePhysicsSimulation;
	}
	
	ECollisionReportMode GetCollisionReportMode() const 
	{
		return m_CollisionReportMode;
	}

	void SetUsePhysicsSimulation(bool bUseSimulation);

	void SetCollisionFlags(int32 Flag);


	virtual bool GetAABB(SVector3& outBMin, SVector3& outBMax) override;


public:
	//Shadow
	virtual bool IsCastShadow() const override
	{
		return CastShadow;
	}


public:
	virtual void BeginPlay() override;

	void PhysicsBeginPlay();

	virtual void EndPlay() override;

	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void Render(class Renderer* pRenderer) override;

	virtual void GatherRenderInstance(class Renderer* pRenderer, std::vector<SInstanceInfo>& InstanceInfos, int32 InstanceID) override;

	virtual void RenderDepth(class Renderer* pRenderer, const Matrix& ViewProj) override;

	virtual void OnUpdateTransform() override;

	virtual void PreUpdate(float DeltaSeconds) override;

	virtual void PostUpdate(float DeltaSeconds) override;

	virtual size_t GetIAPair(std::vector<FIAPair>& OutvecIAPairs) const override;	

	virtual class MeshAsset* GetMeshAsset() const override;	


	virtual int32 GetInstanceID() const override;

	template<typename MeshAssetType, typename = std::enable_if_t<std::is_base_of<MeshAsset, MeshAssetType>::value>>
	MeshAssetType* GetMeshAssetAs()
	{
		return dynamic_cast<MeshAssetType*>(m_MeshAsset.Get());
		//	return (MeshAssetType*)(m_MeshAsset.Get());
	}

	Material* GetMaterial(int32 idx = 0);

	virtual bool Serialize(Archive& ar) override;

public:
	virtual void SetMeshAsset(class MeshAsset* InMeshAsset);
	
private:
	//PROP(TSharedPtr<MeshAsset>, m_MeshAsset, DetailsEdit | Transient, "Primitive")
	//Property 로 노출은 상위 클래스에서 할것 Static Or Skeletal 
	TSharedPtr<MeshAsset> m_MeshAsset;

	MeshAsset* m_prevMeshAsset;

	//Material Override
	PROP(std::vector<TSharedPtr<class Material>>, m_vecMaterial, DetailsView| Transient, "Material")
	std::vector<TSharedPtr<class Material>> m_vecMaterial;	
	
	int32					m_InstanceID;


	PROP(bool, CastShadow, DetailsEdit, "Shadow")
	bool CastShadow;


private:
	std::vector<std::function<void(const SCollisionResult&)>> m_OnBeginOverlaps;
	std::vector<std::function<void(const SCollisionResult&)>> m_OnEndOverlaps;


protected:
	void InitCollisionAndPhysics();
	void DeInitCollisionAndPhysics();
	void ReInitCollisionAndPhysics();
	
	
	virtual void CreateShape() {}



	

protected:
	SCollisionResult			m_Result;	

	ECollisionReportMode		m_CollisionReportMode;	
	int32						m_OverlapCount;	

	
	SVector3					Position_lock;
	SVector3					Rotation_lock;
	SVector3					Center_of_mass;

protected:
	class PhysicsSystem* m_cachedPhsics;	

	PROP(bool , UsePhysicsSimulation, DetailsEdit,"Physics")
	bool UsePhysicsSimulation;

	PROP(SCollisionProfile*, m_Profile, DetailsEdit, "Collision")
	SCollisionProfile* m_Profile;

	PROP(float, Mass, DetailsEdit, "Physics")
	float			Mass;

	PROP(SVector3, Gravity, DetailsEdit, "Physics")
	SVector3		Gravity;


	class btRigidBody* m_Body;
	class btPairCachingGhostObject* m_GhostBody;
	class btCollisionShape* m_Shape;
	class btTriangleIndexVertexArray* m_meshInterface; 

	class btGhostPairCallback* m_InteranlCallback;
	
	bool m_DirtyShape;
	

};

