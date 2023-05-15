#include "EnginePCH.h"
#include "SceneComponent.h"
#include "../GameObject/GameObject.h"
#include "RenderManager.h"
#include "Renderer/Renderer.h"
#include "../../D3D/D3DManager.h"
#include "Assets/Mesh/Mesh.h"
#include "../../Assets/Scene/Scene.h"
#include "Assets/Material/Material.h"
#include "Path/PathManager.h"

#include "Renderer/GeometryGenerator.h"
#include "CameraComponent.h"
#include "CameraManager.h"


SceneComponent::SceneComponent()
	:m_Parent(nullptr)
	, m_ComponentToWorldUpdated(false)
	, m_AbsoulteLocation(false)
	, m_AbsoluteRotation(false)
	, m_AbsoluteScale(false)
	, m_RelativeLocation(SVector3(0.0f))
	, m_RelativeRotation(SVector3(0.0f))
	, m_RelativeScale3D(SVector3(1.0f))	
{
	Transform = this;
}

SceneComponent::SceneComponent(const SceneComponent& Other)
	:Super(Other)
	, m_Parent(nullptr)	
	, m_ComponentToWorldUpdated(Other.m_ComponentToWorldUpdated)
	, m_AbsoulteLocation(Other.m_AbsoulteLocation)
	, m_AbsoluteRotation(Other.m_AbsoluteRotation)
	, m_AbsoluteScale(Other.m_AbsoluteScale)
	, m_RelativeLocation(Other.m_RelativeLocation)
	, m_RelativeRotation(Other.m_RelativeRotation)
	, m_RelativeScale3D(Other.m_RelativeScale3D)
	, AttachSocketName(Other.AttachSocketName)
{
	Transform = this;
	if (Other.IsConstructed())
	{		
		m_Children.clear();	
		

		size_t	Size = Other.m_Children.size();

		for (size_t i = 0; i < Size; ++i)
		{
			SceneComponent* ChildCom = Other.m_Children[i]->Clone();

			AddChild(ChildCom, ChildCom->GetAttachSocketName());
		}
	}

}

SceneComponent::~SceneComponent()
{
	OnDestroy();
}

void SceneComponent::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);	

	
	
	//DefaultUI.png
	Renderer* rdr = g_Engine->Get<Renderer>();
	
	PathManager* pathMgr = g_Engine->Get<PathManager>();	
	const SPathInfo* pathInfo = pathMgr->FindPath(TEXTURE_PATH);	




}

bool SceneComponent::Serialize(Archive& ar)
{
	ar.BeginObject(GetName().c_str());	

	bool bResult = Super::Serialize(ar);
	size_t childSize = m_Children.size();

	for (size_t i = 0; i < childSize; i++)
	{
		bResult &= m_Children[i]->Serialize(ar);
	}

	ar.EndObject();
	return bResult;
}

void SceneComponent::SetOwner(XObject* InOwner)
{
	Super::SetOwner(InOwner);

	size_t ChildSize = m_Children.size();
	for (size_t i = 0; i < ChildSize; i++)
	{
		m_Children[i]->SetOwner(InOwner);
	}
}

bool SceneComponent::GetAABB(SVector3& outBMin, SVector3& outBMax)
{
	return false;
}

void SceneComponent::UpdateComponentToWorldWithParent(SceneComponent* Parent, const std::string& SocketName, const SVector4& RelativeRoationQuat)
{
	if(Parent == this)
	{
		//Circler Exception!!
		return;
	}

	if(Parent && !Parent->m_ComponentToWorldUpdated)
	{		
		Parent->UpdateComponentToWorld();

		if (m_ComponentToWorldUpdated)
		{
			return;
		}
	}

	m_ComponentToWorldUpdated = true;

	STransform NewTransform;

	const STransform RelativeTransform(RelativeRoationQuat, GetRelativeLocation(), GetRelativeScale3D());
	NewTransform = CalcNewComponentToWorld(RelativeTransform, Parent, SocketName);


	// If transform has changed..
	bool bHasChanged = !GetComponentTransform().Equals(NewTransform, KINDA_SMALL_NUMBER);

	
	if (bHasChanged)
	{		
		m_ComponentToWorld = NewTransform;
		PropagateTransformUpdate(true);
	} 
	else
	{			
		PropagateTransformUpdate(false);
	}

}

void SceneComponent::PropagateTransformUpdate(bool TransformChanged)
{
	
	if (TransformChanged)
	{
		OnUpdateTransform();

		if (m_Children.size() > 0)
		{			
			UpdateChildTransforms();
		}
	}		
	
}

void SceneComponent::OnUpdateTransform()
{

}

void SceneComponent::UpdateComponentToWorld()
{
	SVector3	ConvertRot = m_RelativeRotation.ConvertAngle();
	
	m_RelativeRotationCache = SMathUtils::EulerToQuat(ConvertRot);
		
	
	UpdateComponentToWorldWithParent(GetAttachParent(), GetAttachSocketName(), m_RelativeRotationCache);
}

void SceneComponent::UpdateChildTransforms()
{
	size_t childSize = m_Children.size();
	if (childSize > 0)
	{
		for(size_t i =0;i<childSize;i++)
		{
			if (m_Children[i] != nullptr)
			{
				// Update Child if it's never been updated.
				if (!m_Children[i]->m_ComponentToWorldUpdated)
				{
					m_Children[i]->UpdateComponentToWorld();
				} 
				else
				{

					// Don't update the child if it uses a completely absolute (world-relative) scheme.
					if (m_Children[i]->IsUsingAbsoluteLocation() && m_Children[i]->IsUsingAbsoluteRotation() && m_Children[i]->IsUsingAbsoluteScale())
					{
						continue;
					}

					m_Children[i]->UpdateComponentToWorld();
				}
			}
		}
	
	}
}

STransform SceneComponent::CalcNewComponentToWorld_GeneralCase(const STransform& NewRelativeTransform, const SceneComponent* InParent, const std::string& SocketName) const
{
	if (InParent != nullptr)
	{
		const STransform ParentToWorld = InParent->GetSocketTransform(SocketName);
		STransform NewCompToWorld = NewRelativeTransform * ParentToWorld;
		
		if (IsUsingAbsoluteLocation())
		{
			NewCompToWorld.Translation = NewRelativeTransform.Translation;
		}

		if (IsUsingAbsoluteRotation())
		{
			NewCompToWorld.Rotation = NewRelativeTransform.Rotation;
		}

		if (IsUsingAbsoluteScale())
		{
			NewCompToWorld.Scale3D = NewRelativeTransform.Scale3D;
		}

		return NewCompToWorld;

	} 
	else
	{
		return NewRelativeTransform;
	}
}

void SceneComponent::BeginPlay()
{
	Super::BeginPlay();
	
	FChildComponents::iterator iter = m_Children.begin();
	FChildComponents::iterator iter_end = m_Children.end();

	for (; iter != iter_end; iter++)
	{
		(*iter)->BeginPlay();
	}
}

void SceneComponent::PreUpdate(float DeltaSeconds)
{
	FChildComponents::iterator iter = m_Children.begin();
	FChildComponents::iterator iter_end = m_Children.end();

	for (; iter != iter_end; iter++)
	{
		(*iter)->PreUpdate(DeltaSeconds);
	}
}

void SceneComponent::Update(float DeltaSeconds)
{
	FChildComponents::iterator iter = m_Children.begin();
	FChildComponents::iterator iter_end = m_Children.end();

	for (; iter != iter_end; iter++)
	{
		(*iter)->Update(DeltaSeconds);
	}	
	
}

void SceneComponent::PostUpdate(float DeltaSeconds)
{
	UpdateComponentToWorld();
	FChildComponents::iterator iter = m_Children.begin();
	FChildComponents::iterator iter_end = m_Children.end();

	for (; iter != iter_end; iter++)
	{
		(*iter)->PostUpdate(DeltaSeconds);
	}	
}

void SceneComponent::OnDestroy()
{
	Super::OnDestroy();
	//Delete All Child
	size_t childs = m_Children.size();
	for (size_t i = 0; i < childs; i++)
	{
		m_Children[i]->Destroy();
	}
}

void SceneComponent::SetOwningScene(class Scene* InScene)
{
	Super::SetOwningScene(InScene);	

	//TODO :Make RenderList.
	//RenderManager* renderMgr = g_Engine->Get<RenderManager>();
	//renderMgr->AddRenderList(this);
	//SceneRenderer에서 Rendering 오브젝트의 프레임웤 코드 

	size_t	Size = m_Children.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_Children[i]->SetOwningScene(InScene);
	}
}

void SceneComponent::Destroy(float TimeDelay /*= -1.0f*/)
{
	Super::Destroy(TimeDelay);

	size_t	Size = m_Children.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_Children[i]->Destroy(TimeDelay);
	}
}

void SceneComponent::Render(class Renderer* pRenderer)
{
	//메쉬를그리기 직전단계까지만 합니다.
	// 테스트Quad를 그립니다.
	CameraComponent* currentCamera = nullptr;
	if(m_OwningScene)	
		currentCamera = m_OwningScene->GetCameraManager()->GetCurrentCamera();	
	else
		currentCamera =g_Engine->Get<CameraManager>()->GetCurrentCamera();

	Matrix matProj = currentCamera->GetProjMatrix();

	Matrix matView = currentCamera->GetViewMatrix();

	SObjectMatrices objMat;


	objMat.World = m_ComponentToWorld.ToMatrixWithScale();
	objMat.Normal = Matrix::NormalMatrix(objMat.World) * matView;
	objMat.WorldViewProj = (objMat.World * matView) * matProj;

	
	pRenderer->SetConstantStruct("ObjMatrices", (void*)(&objMat));


}


void SceneComponent::RenderDepth(class Renderer* pRenderer, const Matrix& ViewProj)
{
	SObjectShadowMatrices objMat;

	objMat.World = m_ComponentToWorld.ToMatrixWithScale();	
	objMat.Wvp = (objMat.World * ViewProj);

	pRenderer->SetConstantStruct("ObjMatrices", (void*)(&objMat));
}


void SceneComponent::GatherRenderDeptthInstance(class Renderer* pRenderer, const Matrix& ViewProj, std::vector<SDepthInstanceInfo>& InstanceInfos, int32 InstanceID)
{
	SObjectShadowMatrices objMat;

	objMat.World = m_ComponentToWorld.ToMatrixWithScale();
	
	objMat.Wvp = (objMat.World * ViewProj);

	InstanceInfos[InstanceID].objMatrics = objMat;
	
}

void SceneComponent::GatherRenderInstance(class Renderer* pRenderer, std::vector<SInstanceInfo>& InstanceInfos,int32 InstanceID)
{
	CameraComponent* currentCamera = nullptr;

	if (m_OwningScene)
		currentCamera = m_OwningScene->GetCameraManager()->GetCurrentCamera();
	else
		currentCamera = g_Engine->Get<CameraManager>()->GetCurrentCamera();

	Matrix matProj = currentCamera->GetProjMatrix();

	Matrix matView = currentCamera->GetViewMatrix();

	SObjectMatrices objMat;

	objMat.World = m_ComponentToWorld.ToMatrixWithScale();
	objMat.Normal = Matrix::NormalMatrix(objMat.World) * matView;
	objMat.WorldViewProj = (objMat.World * matView) * matProj;
	
	InstanceInfos[InstanceID].objMatrics = objMat;


}

size_t SceneComponent::GetIAPair(std::vector<FIAPair>& OutvecIAPairs) const
{
	return 0;
}

class MeshAsset* SceneComponent::GetMeshAsset() const
{
	return nullptr;
}

int32 SceneComponent::GetInstanceID() const
{
	return INDEX_NONE;
}

void SceneComponent::AdditionalBindPerIA(class Renderer* pRenderer)
{
	
}

void SceneComponent::AddChild(SceneComponent* InChild, const std::string& SocketName /*= ""*/)
{
	InChild->m_Parent = this;
	InChild->AttachSocketName= SocketName;
	m_Children.push_back(InChild);
}


void SceneComponent::SetAttachedSocketName(const std::string& InSocketName)
{
	AttachSocketName = InSocketName;
}

bool SceneComponent::DeleteChild(SceneComponent* InChild)
{
	size_t	Size = m_Children.size();

	for (size_t i = 0; i < Size; ++i)
	{
		if (m_Children[i] == InChild)
		{
			auto	iter = m_Children.begin() + i;

			(*iter)->m_Parent = nullptr;

			GameObject* ownerGameObject = GetOwner<GameObject>();
			if(ownerGameObject)
			{
				ownerGameObject->DeleteSceneComponent(*iter);
			}
			

			m_Children.erase(iter);		
			

			return true;
		}
	}

	return false;
}

bool SceneComponent::DeleteChild(const std::string& Name)
{
	size_t	Size = m_Children.size();

	for (size_t i = 0; i < Size; ++i)
	{
		if (m_Children[i]->GetName() == Name)
		{
			FChildComponents::iterator	iter = m_Children.begin() + i;

			(*iter)->m_Parent = nullptr;

			GameObject* ownerGameObject = GetOwner<GameObject>();
			if (ownerGameObject)
			{
				ownerGameObject->DeleteSceneComponent(*iter);
			}			

			m_Children.erase(iter);			

			return true;
		}
	}

	return false;
}

SceneComponent* SceneComponent::FindComponent(const std::string& Name)
{
	if (GetName() == Name)
		return this;

	size_t	Size = m_Children.size();

	for (size_t i = 0; i < Size; ++i)
	{
		SceneComponent* found = m_Children[i]->FindComponent(Name);

		if (found)
			return found;
	}

	return nullptr;
}


bool SceneComponent::HasChild() const
{
	return m_Children.size() != 0;
}

bool SceneComponent::HasParent() const
{
	return m_Parent != nullptr;
}

void SceneComponent::AddGameObjectsList()
{
	GetOwner<GameObject>()->AddSceneComponent(this);

	size_t	Size = m_Children.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_Children[i]->AddGameObjectsList();
	}
}

SceneComponent* SceneComponent::GetAttachParent() const
{
	return m_Parent;
}

const std::string& SceneComponent::GetAttachSocketName() const
{
	return AttachSocketName;
}



size_t SceneComponent::GetAttachChildren(std::vector<class SceneComponent*>& OutChild) const
{
	size_t childSize = m_Children.size();

	for(size_t i =0;i<childSize;i++)
	{
		OutChild.push_back(m_Children[i]);
	}
	return childSize;
}

STransform SceneComponent::GetSocketTransform(const std::string& InSocketName) const
{
	//TODO: Make SocketTransform Map
	
	//return GetComponentTransform().GetRelativeTransform(m_ComponentToWorld);	

	return GetComponentTransform();
}

void SceneComponent::ConditionalUpdateComponentToWorld()
{
	if (!m_ComponentToWorldUpdated)
	{
		UpdateComponentToWorld();
	}
}

SVector3 SceneComponent::GetWorldAxis(EAxis Axis) const
{
	Matrix compToWorldMat = m_ComponentToWorld.ToMatrixNoScale();

	return compToWorldMat.v[Axis];
}

void SceneComponent::AddWorldLocation(const SVector3& Delta)
{
	SetWorldLocation(GetWorldLocation() + Delta);
}

void SceneComponent::SetWorldLocation(const SVector3& NewLocation)
{
	SVector3 NewRelLocation = NewLocation;

	//World To Relative
	if (GetAttachParent() != nullptr && !IsUsingAbsoluteLocation())
	{		
		ConditionalUpdateComponentToWorld();
		STransform ParentToWorld = GetAttachParent()->GetSocketTransform(GetAttachSocketName());
		NewRelLocation = ParentToWorld.InverseTransformPosition(NewLocation);
	}

	SetRelativeLocation(NewRelLocation);

}

const SVector3& SceneComponent::GetWorldLocation() const
{	
	return m_ComponentToWorld.Translation;
}

void SceneComponent::AddWorldRotation(const SVector3& EulerDelta)
{
	SVector3 val = GetWorldRotation() + EulerDelta;
	SetWorldRotation(val);
}

void SceneComponent::SetWorldRotation(const SVector3& EulerRotation)
{
	//X,Y,Z 회전에 따른 Pitch Yaw Roll
	//y->X , z ->y , x- >z;
	SVector3 converted = SVector3(EulerRotation.z, EulerRotation.x, EulerRotation.y);
	

	if(GetAttachParent() == nullptr )
	{
		SetRelativeRotation(converted);
	}
	else
	{		
		SVector4 NewRelRotation = GetRelativeRotationFromWorld(SMathUtils::EulerToQuat(converted.ConvertAngle()));
		SVector3 rt = NewRelRotation.ToEulerAngles();
		rt = SVector3(rt.z, rt.x, rt.y);;
		SetRelativeRotation(rt);
	}
	
	ConditionalUpdateComponentToWorld();
}

SVector3 SceneComponent::GetWorldRotation() const
{	
	return m_ComponentToWorld.Rotation.ToEulerAngles();		

}

void SceneComponent::SetWorldScale(const SVector3& NewScale)
{
	SVector3 NewRelScale = NewScale;

	//World To Relative
	if (GetAttachParent() != nullptr && !IsUsingAbsoluteScale())
	{
		ConditionalUpdateComponentToWorld();
		STransform ParentToWorld = GetAttachParent()->GetSocketTransform(GetAttachSocketName());
		NewRelScale = NewScale * ParentToWorld.GetSafeScaleReciprocal(ParentToWorld.Scale3D);
		
	}

	SetRelativeScale(NewRelScale);
}

SVector3 SceneComponent::GetWorldScale() const
{
	return m_ComponentToWorld.Scale3D;
}

SVector4 SceneComponent::GetRelativeRotationFromWorld(const SVector4& NewRotationQuat)
{
	SVector4 NewRelRotation = NewRotationQuat;

	// If already attached to something, transform into local space
	if (GetAttachParent() != nullptr && !IsUsingAbsoluteRotation())
	{
		const STransform  ParentToWorld = GetAttachParent()->GetSocketTransform(GetAttachSocketName());	
		{
			const SVector4 ParentToWorldQuat = ParentToWorld.Rotation;
		
			const SVector4 NewRelQuat = SVector4::VectorQuaternionMultiply2(ParentToWorldQuat.Inverse(), NewRelRotation);
			
			
			NewRelRotation = NewRelQuat;
			SVector3 myTest = NewRelQuat.ToEulerAngles();
			
		}
	}
	return NewRelRotation;
}

void SceneComponent::SetTransformFromMatrix(const Matrix& InMatrix)
{
	Matrix matrix = InMatrix;
	SVector3 scale3D;
	SVector4 rotQuat;
	SVector3 translation;
	matrix.DecomposeMatrix(OUT scale3D, OUT rotQuat, OUT translation);
	
		
	SVector3 EnulerAngle = rotQuat.ToEulerAngles();	
	//SVector3 converted = SVector3(EnulerAngle.z, EnulerAngle.x, EnulerAngle.y);
	SetWorldScale(scale3D);		
	SetWorldRotation(EnulerAngle);
	SetWorldLocation(translation);	
	
	
}

//라 업 룩 
SVector3 SceneComponent::GetComponentForwardVector() const
{
	return m_ComponentToWorld.ToMatrixNoScale().v[2];
}

SVector3 SceneComponent::GetComponentBackwardVector() const
{
	return -m_ComponentToWorld.ToMatrixNoScale().v[2];
}

SVector3 SceneComponent::GetComponentRightVector() const
{
	return m_ComponentToWorld.ToMatrixNoScale().v[0];
}

SVector3 SceneComponent::GetComponentLeftVector() const
{
	return -m_ComponentToWorld.ToMatrixNoScale().v[0];
}

SVector3 SceneComponent::GetComponentUpVector() const
{
	return m_ComponentToWorld.ToMatrixNoScale().v[1];
}

SVector3 SceneComponent::GetComponentDownVector() const
{
	return -m_ComponentToWorld.ToMatrixNoScale().v[1];
}


void SceneComponent::AddRootMotaionAcc(const SVector3& rootMoationAcc)
{
	m_RootMotionAcc += rootMoationAcc;
}

SVector3 SceneComponent::ConsumeRootMotionAcc()
{
	SVector3 temp = m_RootMotionAcc;
	m_RootMotionAcc = SVector3::ZeroVector;

	return temp;
}

void SceneComponent::PushRenderList(class RenderManager* pRenderManager)
{	
	pRenderManager->AddRenderList(this);

	size_t childSize = m_Children.size();

	for(size_t i =0;i< childSize;i++)
	{
		m_Children[i]->PushRenderList(pRenderManager);
	}
}



void SceneComponent::EndPlay()
{
	Super::EndPlay();

	size_t childSize = m_Children.size();

	for (size_t i = 0; i < childSize; i++)
	{
		m_Children[i]->EndPlay();
	}
}
