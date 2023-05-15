#pragma once

#include "XObject/XObject.h"
#include "Assets/Scene/Scene.h"
#include "GameFramework/GameObject/GameObject.h"

/*
* Scene 이 없는 상태의 GameObject의 WorldPostion은 변경이 불가능하며 원점으로 고정
* 
*/

using FObjectList = std::list<TSharedPtr<GameObject>>;

class ObjectManager : public XObject
{
	friend class Globals;
	friend class Scene;


private:
	ObjectManager();
	virtual ~ObjectManager();

public:
	void Initialize(class Scene* OwningScene);	

	void BeginPlay();

	void EndPlay();

	virtual void PreUpdate(float DeltaSeconds)				override;

	virtual void Update(float DeltaSeconds)					override;

	virtual void PostUpdate(float DeltaSeconds)				override;


public:
	const FObjectList& GetObjectList();

	/* Create Only Not Spawn */
	template<typename UserGameObject, typename = std::enable_if_t<std::is_base_of<GameObject, UserGameObject>::value>>
	static UserGameObject* CreateGameObject(TSubclassOf<GameObject> ObjectClass = UserGameObject::StaticClass())
	{
		UserGameObject* Obj = nullptr;

		if (!ObjectClass)
		{
			Obj = Globals::CreateX<UserGameObject>(nullptr, ObjectClass.TemplatedClass->GetName());

		} else
		{
			Obj = Globals::CreateX<UserGameObject>(ObjectClass, nullptr, ObjectClass.Class->GetName());
		}
		return Obj;
	}

	/* Spawn Only */
	void SpawnCreatedGameObject(class GameObject* CreatedObject)
	{
		//Check Already Spawn
		{
			FObjectList::const_iterator iter = m_ObjectList.begin();
			FObjectList::const_iterator iterEnd = m_ObjectList.end();

			for (; iter != iterEnd; iter++)
			{
				if ((*iter) == CreatedObject)
					return;
			}
		}
	

		std::string ObjName = CreatedObject->GetName();
		size_t pos = ObjName.rfind("_");
		if(pos != std::string::npos)
			ObjName = ObjName.substr(0, pos);

		stringstream ss;

		auto iter = m_InstanceCounter.find(ObjName);

		if (iter == m_InstanceCounter.end())
			m_InstanceCounter.insert(std::make_pair(ObjName, 0));

		ss << ObjName << "_" << m_InstanceCounter[ObjName]++;

		m_ObjectList.push_back(CreatedObject);
		CreatedObject->SetOwner(this);
		CreatedObject->SetScene(m_OwningScene);
		CreatedObject->SetName(ss.str());
		if (m_OwningScene)
		{
			if (m_OwningScene->m_Began)
				CreatedObject->BeginPlay();
		}

	
	}


	template<typename UserGameObject, typename = std::enable_if_t<std::is_base_of<GameObject, UserGameObject>::value>>
	UserGameObject* SpawnGameObject(const std::string& InName, TSubclassOf<GameObject> ObjectClass = UserGameObject::StaticClass())
	{
		UserGameObject* Obj = nullptr;

		stringstream ss;
		auto iter = m_InstanceCounter.find(InName);

		if (iter == m_InstanceCounter.end())
			m_InstanceCounter.insert(std::make_pair(InName, 0));

		ss << InName << "_" << m_InstanceCounter[InName]++;


		if (!ObjectClass)
		{
			Obj = Globals::CreateX<UserGameObject>(this, ss.str());

		} else
		{
			Obj = Globals::CreateX<UserGameObject>(ObjectClass, this, ss.str());
		}

		Obj->SetScene(m_OwningScene);

		m_ObjectList.push_back(Obj);

		if (m_OwningScene->m_Began)
			Obj->BeginPlay();

		Obj->PushRenderList(m_cachedRenderManager);
		return Obj;
	}

	template<typename UserGameObject, typename = std::enable_if_t<std::is_base_of<GameObject, UserGameObject>::value>>
	size_t FindGameObjectsByClass(std::vector<UserGameObject*>& Objects)
	{
		FObjectList::iterator iter = m_ObjectList.begin();
		FObjectList::iterator iterEnd = m_ObjectList.end();

		for (; iter != iterEnd; iter++)
		{
			if ((*iter)->GetClass()->IsA(UserGameObject::StaticClass()))
			{
				Objects.push_back((UserGameObject*)((*iter).Get()));
			}
		}

		return Objects.size();
	}

	template<typename UserGameObject, typename = std::enable_if_t<std::is_base_of<GameObject, UserGameObject>::value>>
	UserGameObject* FindGameObjectByClass()
	{
		FObjectList::iterator iter = m_ObjectList.begin();
		FObjectList::iterator iterEnd = m_ObjectList.end();

		for (; iter != iterEnd; iter++)
		{
			if ((*iter)->GetClass()->IsA(UserGameObject::StaticClass()))
			{
				return (UserGameObject*)((*iter).Get());
			}
		}

		return nullptr;
	}




	virtual bool Serialize(Archive& ar) override;

private:
	FObjectList			m_ObjectList;
	class Scene*		m_OwningScene;

	std::unordered_map<std::string, uint64>	m_InstanceCounter;

	class RenderManager*				m_cachedRenderManager;
};

