#pragma once

#include "../../Assets/AssetData.h"
#include "../Components/SceneComponent.h"
#include "../Components/ObjectComponent.h"


using FSceneComponentList = std::vector<SceneComponent*>;
using FObjectComponentList = std::vector<TSharedPtr<ObjectComponent>>;

class GameObject : public AssetData
{
	CLASS(GameObject, AssetData)
	//using FSceneComponentList = std::list<SceneComponent*>;
	//using FObjectComponentList = std::vector<TSharedPtr<ObjectComponent>>;

protected:
	GameObject();
	GameObject(const GameObject& Other);
	virtual ~GameObject();


protected:
	virtual void Construct(const std::string& InName = "") override;

	virtual bool PostLoad() override;
	
	virtual void OnDestroy();

public:	
	virtual void BeginPlay();

	virtual void EndPlay();


	virtual void PreUpdate(float DeltaSeconds)		override;
	
	virtual void Update(float DeltaSeconds)			override;
	
	virtual void PostUpdate(float DeltaSeconds)		override;

	virtual void PushRenderList(class RenderManager* pRenderManager);	


	void SetLifeTime(const float InLifeTime);

	void Destroy(float TimeDelay = -1.0f);

	virtual void SetScene(class Scene* InOwningScene);

	bool HasChildGameObject() const;

	class Scene* GetOwningScene() const;

public:
	void SetObjectLocation(const SVector3& InLocation);

	void SetObjectRotation(const SVector3& InRotation);

	void SetObjectScale(const SVector3& InScale);


public:
	//Quick Access Root
	SVector3 GetWorldLocation() const;	 
	SVector3 GetWorldRotation() const;
	SVector3 GetWorldScale3D() const;

	SVector3 GetObjectForwardVector() const;
	SVector3 GetObjectBackwardVector() const;
	
	SVector3 GetObjectRightVector() const;
	SVector3 GetObjectLeftVector() const;

	SVector3 GetObjectUpVector() const;
	SVector3 GetObjectDownVector() const;

	STransform GetObjectTransform() const;


	virtual void SetWorldLocation(const SVector3& InLocation);
	virtual void SetWorldRotation(const SVector3& InRotation);
	virtual void SetWorldScale(const SVector3& InScale);
	
	
public:	
	template<typename ComponentType, typename = std::enable_if_t<std::is_base_of<ComponentBase, ComponentType>::value>>
	ComponentType* CreateComponent(const std::string& InName, TSubclassOf<ComponentType> CompClass = ComponentType::StaticClass())
	{
		ComponentType* Comp = Globals::CreateX<ComponentType>(CompClass, this, InName);

		if (CompClass.GetClass()->IsChildOf(ObjectComponent::StaticClass()))
		{
			m_vecObjectComponent.push_back((ObjectComponent*)Comp);
		}
		else
		{
			m_SceneComponentList.push_back((SceneComponent*)Comp);

			if (!m_RootComponent)
			{
				m_RootComponent = (SceneComponent*)Comp;
			}
		}

		return Comp;
	}


	template<typename ComponentType, typename = std::enable_if_t<std::is_base_of<ComponentBase, ComponentType>::value>>
	ComponentType* FindComponentByClass()
	{
		{
			FSceneComponentList::iterator	iter = m_SceneComponentList.begin();
			FSceneComponentList::iterator	iterEnd = m_SceneComponentList.end();

			for (; iter != iterEnd; ++iter)
			{
				if ((*iter)->GetClass()->IsA(ComponentType::StaticClass()))
				{
					return (ComponentType*)(*iter);
				}
			}
		}

		{
			FObjectComponentList::iterator iter = m_vecObjectComponent.begin();
			FObjectComponentList::iterator iterEnd = m_vecObjectComponent.end();

			for (; iter != iterEnd; ++iter)
			{
				if ((*iter)->GetClass()->IsA(ComponentType::StaticClass()) ||
					(*iter)->GetClass()->IsChildOf(ComponentType::StaticClass()))
				{
					return (ComponentType*)((*iter).Get());
				}
			}
		}

		return nullptr;
	}

	template<typename ComponentType, typename = std::enable_if_t<std::is_base_of<ComponentBase, ComponentType>::value>>
	size_t FindComponentsByClass(std::vector<ComponentType*>& OutFounds)
	{
		{
			FSceneComponentList::const_iterator	iter = m_SceneComponentList.begin();
			FSceneComponentList::const_iterator	iterEnd = m_SceneComponentList.end();

			for (; iter != iterEnd; ++iter)
			{
				if ((*iter)->GetClass()->IsA(ComponentType::StaticClass()))
				{
					OutFounds.push_back(*iter);
				}
			}
		}

		{
			FObjectComponentList::const_iterator iter = m_vecObjectComponent.begin();
			FObjectComponentList::const_iterator iterEnd = m_vecObjectComponent.end();

			for (; iter != iterEnd; ++iter)
			{
				if ((*iter)->GetClass()->IsA(ComponentType::StaticClass()))
				{
					OutFounds.push_back(*iter);
				}
			}
		}

		return OutFounds.size();
	}

public:
	SceneComponent* GetRootComponent() const;

	void SetRootComponent(SceneComponent* SceneComp);

	void AddSceneComponent(SceneComponent* SceneComp);

	void DeleteSceneComponent(SceneComponent* Component);

	void DeleteObjectComponent(ObjectComponent* Component);

	const FSceneComponentList& GetSceneComponents() const;
	
	const FObjectComponentList& GetObjectComponents() const;

	SceneComponent* FindSceneComponent(const std::string& InName) const;

	ObjectComponent* FindObjectComponent(const std::string& InName) const;


	template<typename ComponentType, typename = std::enable_if_t<std::is_base_of<ComponentBase, ComponentType>::value>>
	ComponentType* FindComponent(const std::string& InName) const
	{		
		if (ComponentType::StaticClass()->IsChildOf(ObjectComponent::StaticClass()))
		{
			return (ComponentType*)FindObjectComponent(InName);
		}
		else
			return (ComponentType*)FindSceneComponent(InName);

		return nullptr;
	}


private:
	class Scene* m_OwningScene;

	PROP(TSharedPtr<SceneComponent>, m_RootComponent, DetailsEdit, "RootComponent")
	TSharedPtr<SceneComponent>	m_RootComponent;

	FSceneComponentList			m_SceneComponentList;

	PROP(FObjectComponentList, m_vecObjectComponent, DetailsEdit)
	FObjectComponentList		m_vecObjectComponent;


	class GameObject*			m_Parent;

	std::vector<TSharedPtr<class SceneComponent>>		m_ChildGameObject;
		



protected:
	TimerHandle			m_TimerHandle;

	uint64				m_DestroyTimerID;

	float				m_LifeTime;


	bool				m_IsBegun;
	
	//Quick tagging
public:
	/*
	* Return : Found?
	* OutValue : val
	*/
	bool FindTag(const std::string& InTag, bool& OutValue);

	void RemoveTag(const std::string& InTag);

	void AddTag(const std::string& InTag, bool Value);


protected:
	std::unordered_map<std::string, bool>		m_StateTag;
};

DECLARE_REFLECTION_TYPE(GameObject);
