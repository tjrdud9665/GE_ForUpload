#include "EnginePCH.h"
#include "GameObject.h"
#include "RenderManager.h"
#include <FileSystem/FileSystem.h>


GameObject::GameObject()
	:m_DestroyTimerID(0)
	,m_OwningScene(nullptr)
	,m_LifeTime(0.0f)
	,m_IsBegun(false)
{

}

GameObject::GameObject(const GameObject& Other)
	:Super(Other)
	, m_LifeTime(Other.m_LifeTime)
	,m_DestroyTimerID(0)
	, m_IsBegun(false)
{
	if (Other.IsConstructed())
	{
		
		if(Other.m_RootComponent)
		{
			m_RootComponent = Other.m_RootComponent->Clone();			

			m_RootComponent->SetOwner(this);

			m_RootComponent->AddGameObjectsList();

		}

		{
			FObjectComponentList::const_iterator	iter = Other.m_vecObjectComponent.begin();
			FObjectComponentList::const_iterator	iterEnd = Other.m_vecObjectComponent.end();

			for (; iter != iterEnd; ++iter)
			{
				ObjectComponent* Component = (*iter)->Clone();
				Component->SetOwner(this);
				m_vecObjectComponent.push_back(Component);
			}
		}
	}
}

GameObject::~GameObject()
{
	if (!m_OwningScene)
	{
		std::string sPath = GetInfo().GetSPath();
		if (!sPath.empty())
		{
			std::string CDOName = FileSystem::GetRelativePath(FileSystem::FromString(sPath)) + GetName();
			GetClass()->DeleteCDO(CDOName);
		}
	}
	OnDestroy();
	m_vecObjectComponent.clear();
}

void GameObject::Construct(const std::string& InName /*= ""*/)
{
	Super::Construct(InName);
}

bool GameObject::PostLoad()
{
	bool result = Super::PostLoad();

	std::string CDOName = FileSystem::GetRelativePath(FileSystem::FromString(GetInfo().GetSPath())) + GetName();

	GetClass()->RegisterCDO(CDOName, this, GAMEOBJECT_PATH);

	return result;
}

void GameObject::OnDestroy()
{
	Super::OnDestroy();	

	if (m_RootComponent)
		m_RootComponent->Destroy();

	size_t	Size = m_vecObjectComponent.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecObjectComponent[i]->Destroy();
	}

}

void GameObject::BeginPlay()
{
	if(!m_IsBegun)
	{
		if (m_RootComponent)
		{
			m_RootComponent->BeginPlay();
		}

		size_t Size = m_vecObjectComponent.size();

		for (size_t i = 0; i < Size; ++i)
		{
			m_vecObjectComponent[i]->BeginPlay();
		}
		m_IsBegun = true;
	}

}

void GameObject::EndPlay()
{
	size_t Size = m_vecObjectComponent.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecObjectComponent[i]->EndPlay();
	}
	if (m_RootComponent)
	{
		m_RootComponent->EndPlay();
	}
	m_IsBegun = false;
}

void GameObject::PreUpdate(float DeltaSeconds)
{
	size_t Size = m_vecObjectComponent.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecObjectComponent[i]->PreUpdate(DeltaSeconds);
	}

	if (m_RootComponent)
	{
		m_RootComponent->PreUpdate(DeltaSeconds);
	}
}

void GameObject::Update(float DeltaSeconds)
{
	size_t Size = m_vecObjectComponent.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecObjectComponent[i]->Update(DeltaSeconds);
	}

	if (m_RootComponent)
	{
		m_RootComponent->Update(DeltaSeconds);
	}
}

void GameObject::PostUpdate(float DeltaSeconds)
{
	size_t Size = m_vecObjectComponent.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecObjectComponent[i]->PostUpdate(DeltaSeconds);
	}

	if (m_RootComponent)
	{
		m_RootComponent->PostUpdate(DeltaSeconds);
	}
}

void GameObject::PushRenderList(class RenderManager* pRenderManager)
{
	if(m_RootComponent)
	{
		m_RootComponent->PushRenderList(pRenderManager);
	}
}


void GameObject::SetLifeTime(const float InLifeTime)
{

	m_LifeTime = InLifeTime;

	if (m_LifeTime <= 0)
	{
		Kill();
		return;
	}

	//m_TimerHandle.RemoveTimer(m_DestroyTimerHandle);
	m_TimerHandle.SetTimer(OUT m_DestroyTimerID, m_LifeTime, false, std::bind(&GameObject::Kill, this));

}

void GameObject::Destroy(float TimeDelay /*= -1.0f*/)
{
	if (TimeDelay > 0)
	{
		SetLifeTime(TimeDelay);
	} else
	{
		Kill();
	}

}

void GameObject::SetScene(class Scene* InOwningScene)
{
	m_OwningScene = InOwningScene;

	size_t Size = m_vecObjectComponent.size();

	for (size_t i = 0; i < Size; ++i)
	{
		m_vecObjectComponent[i]->SetOwningScene(m_OwningScene);
	}

	if (m_RootComponent)
	{
		m_RootComponent->SetOwningScene(m_OwningScene);
	}
}

bool GameObject::HasChildGameObject() const
{
	return false;
	//TODO : Make GameObject Hierarchy
}

class Scene* GameObject::GetOwningScene() const
{
	return m_OwningScene;
}

void GameObject::SetObjectLocation(const SVector3& InLocation)
{
	if(m_RootComponent)
	{
		m_RootComponent->m_RelativeLocation = InLocation;
	}
}

void GameObject::SetObjectRotation(const SVector3& InRotation)
{
	if (m_RootComponent)
	{
		m_RootComponent->m_RelativeRotation = InRotation;
	}
}

void GameObject::SetObjectScale(const SVector3& InScale)
{
	if (m_RootComponent)
	{
		m_RootComponent->m_RelativeScale3D = InScale;
	}
}

SVector3 GameObject::GetWorldLocation() const
{
	if(m_RootComponent)
	{
		return m_RootComponent->GetWorldLocation();
	}
	return SVector3(0.0f);
}

SVector3 GameObject::GetWorldRotation() const
{
	if (m_RootComponent)
	{
		return m_RootComponent->GetWorldRotation();
	}
	return SVector3(0.0f);
}

SVector3 GameObject::GetWorldScale3D() const
{
	if (m_RootComponent)
	{
		return m_RootComponent->GetWorldScale();
	}

	return SVector3(1.0f);
}

SVector3 GameObject::GetObjectForwardVector() const
{
	if (m_RootComponent)
	{
		return m_RootComponent->GetComponentForwardVector();
	}

	return SVector3::ForwardVector;
}

SVector3 GameObject::GetObjectBackwardVector() const
{
	if (m_RootComponent)
	{
		return m_RootComponent->GetComponentBackwardVector();
	}

	return SVector3::BackwardVector;
}

SVector3 GameObject::GetObjectRightVector() const
{
	if (m_RootComponent)
	{
		return m_RootComponent->GetComponentRightVector();
	}

	return SVector3::RightVector;
}

SVector3 GameObject::GetObjectLeftVector() const
{
	if (m_RootComponent)
	{
		return m_RootComponent->GetComponentLeftVector();
	}

	return SVector3::LeftVector;
}

SVector3 GameObject::GetObjectUpVector() const
{
	if (m_RootComponent)
	{
		return m_RootComponent->GetComponentUpVector();
	}

	return SVector3::UpVector;
}

SVector3 GameObject::GetObjectDownVector() const
{
	if (m_RootComponent)
	{
		return m_RootComponent->GetComponentDownVector();
	}

	return SVector3::DownVector;
}

STransform GameObject::GetObjectTransform() const
{
	if(m_RootComponent)
	{
		return m_RootComponent->GetComponentTransform();
	}

	return STransform::Identity;
}

void GameObject::SetWorldLocation(const SVector3& InLocation)
{
	if(m_RootComponent)
	{
		m_RootComponent->SetWorldLocation(InLocation);
	}
}

void GameObject::SetWorldRotation(const SVector3& InRotation)
{
	if (m_RootComponent)
	{
		m_RootComponent->SetWorldRotation(InRotation);
	}
}

void GameObject::SetWorldScale(const SVector3& InScale)
{
	if (m_RootComponent)
	{
		m_RootComponent->SetWorldScale(InScale);
	}
}

SceneComponent* GameObject::GetRootComponent() const
{
	return m_RootComponent;
}

void GameObject::SetRootComponent(SceneComponent* SceneComp)
{
	m_RootComponent = SceneComp;
}

void GameObject::AddSceneComponent(SceneComponent* SceneComp)
{
	m_SceneComponentList.push_back(SceneComp);
}

void GameObject::DeleteSceneComponent(SceneComponent* Component)
{
	FSceneComponentList::iterator	iter = m_SceneComponentList.begin();
	FSceneComponentList::iterator	iterEnd = m_SceneComponentList.end();

	for (; iter != iterEnd; ++iter)
	{
		if (*iter == Component)
		{
			m_SceneComponentList.erase(iter);
			return;
		}
	}
}

void GameObject::DeleteObjectComponent(ObjectComponent* Component)
{
	{
		FObjectComponentList::const_iterator iter = m_vecObjectComponent.begin();
		FObjectComponentList::const_iterator iterEnd = m_vecObjectComponent.end();

		for (; iter != iterEnd; ++iter)
		{
			if ((*iter) == Component)
			{
				m_vecObjectComponent.erase(iter);
				break;
			}
		}
	}
}

const FSceneComponentList& GameObject::GetSceneComponents() const
{
	return m_SceneComponentList;
}

const FObjectComponentList& GameObject::GetObjectComponents() const
{
	return m_vecObjectComponent;
}

SceneComponent* GameObject::FindSceneComponent(const std::string& InName) const
{
	{
		FSceneComponentList::const_iterator	iter = m_SceneComponentList.begin();
		FSceneComponentList::const_iterator	iterEnd = m_SceneComponentList.end();

		for (; iter != iterEnd; ++iter)
		{
			if ((*iter)->GetName() == InName)
			{
				return *iter;
			}
		}
	}
	return nullptr;
}

ObjectComponent* GameObject::FindObjectComponent(const std::string& InName) const
{
	{
		FObjectComponentList::const_iterator iter = m_vecObjectComponent.begin();
		FObjectComponentList::const_iterator iterEnd = m_vecObjectComponent.end();

		for (; iter != iterEnd; ++iter)
		{
			if ((*iter)->GetName() == InName)
			{
				return *iter;
			}
		}
	}

	return nullptr;
}

bool GameObject::FindTag(const std::string& InTag, bool& OutValue)
{
	std::unordered_map<std::string, bool>::iterator iter = m_StateTag.find(InTag);

	if (iter != m_StateTag.end())
	{
		OutValue = (*iter).second;
		return true;
	}

	return false;
}

void GameObject::RemoveTag(const std::string& InTag)
{
	std::unordered_map<std::string, bool>::iterator iter = m_StateTag.find(InTag);
	if (iter != m_StateTag.end())
	{
		m_StateTag.erase(iter);
	}
}

void GameObject::AddTag(const std::string& InTag, bool Value)
{
	std::unordered_map<std::string, bool>::iterator iter = m_StateTag.find(InTag);

	if (iter == m_StateTag.end())
	{
		m_StateTag.insert(std::make_pair(InTag, Value));
	} else
	{
		m_StateTag[InTag] = Value;

	}
}
