#pragma once
#include "ComponentBase.h"

/*
* 
	SetUsingAbsoluteLocation(bNewAbsoluteLocation);
	SetUsingAbsoluteRotation(bNewAbsoluteRotation);
	SetUsingAbsoluteScale(bNewAbsoluteScale);
*/

/**
 * https://kr.mathworks.com/discovery/affine-transformation.html			아핀 변환
 */



class SceneComponent :public ComponentBase
{
	CLASS(SceneComponent,ComponentBase)
	friend class GameObject;	
	friend class TransformPropertyWidget;
	friend class NavMeshBoundsVolume;

	using FChildComponents = std::vector<TSharedPtr<class SceneComponent>>;

protected:
	SceneComponent();
	SceneComponent(const SceneComponent& Other);
	virtual ~SceneComponent();

	virtual void Construct(const std::string& InName /* = "" */)		override;


public:
	virtual bool Serialize(Archive& ar) override;

	virtual void SetOwner(XObject* InOwner) override;
	
	

public:
	virtual bool GetAABB(SVector3& outBMin, SVector3& outBMax);	


protected:																												//Make Quat
	void UpdateComponentToWorldWithParent(SceneComponent* Parent, const std::string& SocketName, const SVector4& RelativeRoationQuat);

	//UpdateChildren's Transform..
	void PropagateTransformUpdate(bool TransformChanged);

	virtual void OnUpdateTransform();

	virtual void UpdateComponentToWorld();

	void UpdateChildTransforms();
	

protected:
	STransform CalcNewComponentToWorld(const STransform& NewRelativeTransform , const SceneComponent* InParent = nullptr,const std::string& SocketName = "") const
	{		
		if(InParent)
		{
			InParent = GetAttachParent();			

			const bool bGeneral = IsUsingAbsoluteLocation() || IsUsingAbsoluteRotation() || IsUsingAbsoluteScale();
			if (!bGeneral)
			{			
				return NewRelativeTransform * InParent->GetSocketTransform(SocketName);
			}

			return CalcNewComponentToWorld_GeneralCase(NewRelativeTransform, InParent, SocketName);
		}
		else
		{
			return NewRelativeTransform;
		}

	}

	STransform	CalcNewComponentToWorld_GeneralCase(const STransform& NewRelativeTransform, const SceneComponent* InParent, const std::string& SocketName) const;


public:
	/* GameFramework Component's Interface */
	virtual void BeginPlay()							override;

	virtual void EndPlay()								override;

	/* Inherit  XObject  */
	virtual void PreUpdate(float DeltaSeconds)			override;

	virtual void Update(float DeltaSeconds)				override;

	virtual void PostUpdate(float DeltaSeconds)			override;

	virtual void OnDestroy()							override;

	virtual void SetOwningScene(class Scene* InScene)	override;	

	virtual void Destroy(float TimeDelay = -1.0f)		override;

	//////////////////////////////////////////////////////////////////////////
	virtual void Render(class Renderer* pRenderer);

	virtual void RenderDepth(class Renderer* pRenderer,const Matrix& ViewProj);

	virtual void GatherRenderDeptthInstance(class Renderer* pRenderer, const Matrix& ViewProj, std::vector<SDepthInstanceInfo>& InstanceInfos, int32 InstanceID);

	virtual void GatherRenderInstance(class Renderer* pRenderer, std::vector<SInstanceInfo>& InstanceInfos, int32 InstanceID);

	//Slot Size 
	virtual size_t GetIAPair(std::vector<FIAPair>& OutvecIAPairs) const;

	virtual class MeshAsset* GetMeshAsset() const;

	//INDEX_NONE == Not Instancing
	virtual int32 GetInstanceID() const;

	virtual void AdditionalBindPerIA(class Renderer* pRenderer);

public:
	/* Management Of Hierarchy */	
	virtual void AddChild(SceneComponent* InChild, const std::string& SocketName = "");

	/* GameObject */
	//virtual void AddChild(class GameObject* Child, const std::string& SocketName = "");

	void SetAttachedSocketName(const std::string& InSocketName);

	bool DeleteChild(SceneComponent* Child);

	bool DeleteChild(const std::string& Name);

	SceneComponent* FindComponent(const std::string& Name);	

	bool HasChild() const;

	bool HasParent() const;

	void AddGameObjectsList();

	SceneComponent* GetAttachParent() const;
	
	const std::string& GetAttachSocketName() const;

	size_t GetAttachChildren(std::vector<class SceneComponent*>& OutChild) const;

	virtual STransform GetSocketTransform(const std::string& InSocketName) const;

	void ConditionalUpdateComponentToWorld();

	SVector3 GetWorldAxis(EAxis Axis) const;

public:
	/* 트랜스폼 컨트롤 */
	const SVector3& GetRelativeLocation() const
	{
		return m_RelativeLocation;
	}
	const SVector3& GetRelativeRotation() const
	{
		return m_RelativeRotation;
	}
	const SVector3& GetRelativeScale3D() const
	{
		return m_RelativeScale3D;
	}
	const STransform& GetComponentTransform() const
	{
		return m_ComponentToWorld;
	}

	bool IsUsingAbsoluteLocation() const
	{
		return m_AbsoulteLocation;
	}

	bool IsUsingAbsoluteRotation() const
	{
		return m_AbsoluteRotation;
	}

	bool IsUsingAbsoluteScale() const
	{
		return m_AbsoluteScale;
	}

	void SetAbsoluteLocation(bool AbsoluteLoaction)
	{
		m_AbsoulteLocation = AbsoluteLoaction;
	}

	void SetAbsoluteRotation(bool AbsoluteRotation)
	{
		m_AbsoluteRotation = AbsoluteRotation;
	}

	void SetAbsoluteScale(bool AbsoluteScale)
	{
		m_AbsoluteScale = AbsoluteScale;
	}

	void SetRelativeLocation(const SVector3& InLocation)
	{
		m_RelativeLocation = InLocation;
		m_ComponentToWorldUpdated = false;
	}

	void SetRelativeRotation(const SVector3& InRotation)
	{
		m_RelativeRotation = InRotation;
		m_ComponentToWorldUpdated = false;
	}

	void SetRelativeScale(const SVector3& InScale)
	{
		m_RelativeScale3D = InScale;
		m_ComponentToWorldUpdated = false;
	}


	void AddWorldLocation(const SVector3& Delta);
	void SetWorldLocation(const SVector3& NewLocation);
	const SVector3& GetWorldLocation() const;


	void AddWorldRotation(const SVector3& EulerDelta);
	void SetWorldRotation(const SVector3& EulerRotation);
	SVector3 GetWorldRotation() const;
	

	void SetWorldScale(const SVector3& NewScale);
	SVector3 GetWorldScale() const;	

	

	SVector4 GetRelativeRotationFromWorld(const SVector4& NewRotationQuat);

	virtual void SetTransformFromMatrix(const Matrix& InMatrix);

	SVector3 GetComponentForwardVector() const;

	SVector3 GetComponentBackwardVector() const;

	SVector3 GetComponentRightVector() const;

	SVector3 GetComponentLeftVector() const;

	SVector3 GetComponentUpVector() const;

	SVector3 GetComponentDownVector() const;

public:	

	void AddRootMotaionAcc(const SVector3& rootMoationAcc);

	SVector3 ConsumeRootMotionAcc();

	

public:
	//Every Frame Push SceneComponentRenderData - hierarchy
	//계속해서 g_Engine->Get 으로 호출할경우 쓸모없는 오버해드.
	virtual void PushRenderList(class RenderManager* pRenderManager);	

	//Shdow
	virtual bool IsCastShadow() const 
	{
		return false;
	}


protected:
	class SceneComponent*	m_Parent;

	FChildComponents		m_Children;	

	//1. SceneComponent의 스스로의 PropertyHandle을 가지고와서 하는방법 (Self Prop)
	//

	PROP(SVector3 , m_RelativeLocation, SaveGame)
	SVector3				m_RelativeLocation;

	PROP(SVector3, m_RelativeRotation, SaveGame)
	SVector3				m_RelativeRotation;		

	PROP(SVector3, m_RelativeScale3D, SaveGame)
	SVector3				m_RelativeScale3D;


	STransform				m_ComponentToWorld;
		
	SVector4				m_WorldRotationCache;	
		
	SVector4				m_RelativeRotationCache;


	//Transform..
	bool					m_ComponentToWorldUpdated;

	//Mode 
	bool					m_AbsoulteLocation;

	bool					m_AbsoluteRotation;

	bool					m_AbsoluteScale;

	PROP(std::string, AttachSocketName, DetailsEdit, "Transform")
	std::string				AttachSocketName;

	//메쉬및 ID로 쓰는 오브젝트들 저장하고나서
	// 불러올떄는 ID가아닌 ID에 해당하는 실값을 다시가져와야함.
	// 5번에 AMesh를쓰다가 저장 -> AMesh's NameWithRelPath
	// 다시로드할떄 AMesh's NameWithRelPath ->MeshID	
	

	//Editor Transform 을 만들기위함. PropertSet에서 Transform 을 컨트롤하기위한.
	//*순환걸림으로 절대 쉐어드로 만들지말것 
	PROP(SceneComponent*, Transform, DetailsEdit | Transient, "Transform")
	SceneComponent* Transform;

	SVector3				m_RootMotionAcc;


};

