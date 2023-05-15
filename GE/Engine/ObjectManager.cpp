#include "EnginePCH.h"
#include "ObjectManager.h"
#include "RenderManager.h"

ObjectManager::ObjectManager()
	:m_cachedRenderManager(nullptr)
{

}

ObjectManager::~ObjectManager()
{
	FObjectList::iterator	iter = m_ObjectList.begin();
	FObjectList::iterator	iterEnd = m_ObjectList.end();

	for (; iter != iterEnd; )
	{		
		(*iter)->Destroy();
		iter = m_ObjectList.erase(iter);
		iterEnd = m_ObjectList.end();		
	}	
	m_ObjectList.clear();
}

void ObjectManager::Initialize(class Scene* OwningScene)
{
	m_OwningScene = OwningScene;
	m_cachedRenderManager = g_Engine->Get<RenderManager>();

}

void ObjectManager::BeginPlay()
{
	FObjectList::iterator	iter = m_ObjectList.begin();
	FObjectList::iterator	iterEnd = m_ObjectList.end();

	for (; iter != iterEnd; ++iter)
	{
		if (!(*iter)->IsEnable())
		{
			continue;
		}

		(*iter)->BeginPlay();
	}
}

void ObjectManager::EndPlay()
{
	FObjectList::iterator	iter = m_ObjectList.begin();
	FObjectList::iterator	iterEnd = m_ObjectList.end();

	for (; iter != iterEnd; ++iter)
	{
		if (!(*iter)->IsEnable())
		{
			continue;
		}

		(*iter)->EndPlay();
	}
}

void ObjectManager::PreUpdate(float DeltaSeconds)
{
	FObjectList::iterator	iter = m_ObjectList.begin();
	FObjectList::iterator	iterEnd = m_ObjectList.end();

	for (; iter != iterEnd; ++iter)
	{
		if (!(*iter)->IsEnable())
		{			
			continue;
		}
		(*iter)->PreUpdate(DeltaSeconds);
	
	}
	
	
}

void ObjectManager::Update(float DeltaSeconds)
{
	FObjectList::iterator	iter = m_ObjectList.begin();
	FObjectList::iterator	iterEnd = m_ObjectList.end();

	for (; iter != iterEnd;)
	{
		if (!(*iter)->IsValidate())
		{
			iter = m_ObjectList.erase(iter);
			iterEnd = m_ObjectList.end();
			continue;
		}

		else if (!(*iter)->IsEnable())
		{
			++iter;
			continue;
		}

		(*iter)->Update(DeltaSeconds);

		++iter;
	}
}

void ObjectManager::PostUpdate(float DeltaSeconds)
{
	
	FObjectList::iterator	iter = m_ObjectList.begin();
	FObjectList::iterator	iterEnd = m_ObjectList.end();

	for (; iter != iterEnd; ++iter)
	{

		if (!(*iter)->IsEnable())
		{
			continue;
		}

		(*iter)->PostUpdate(DeltaSeconds);
		(*iter)->PushRenderList(m_cachedRenderManager);
	}

	
}



const FObjectList& ObjectManager::GetObjectList()
{
	return m_ObjectList;
}

bool ObjectManager::Serialize(Archive& ar)
{
	std::vector<GameObject*> objects;

	if (ar.IsLoading())
	{
		m_ObjectList.clear();
		ar.Serialize("GameObjects", objects);
		size_t LoadedObjSize = objects.size();
		for (size_t i = 0; i < LoadedObjSize; i++)
		{
			SpawnCreatedGameObject(objects[i]);
		}
	} else
	{
		FObjectList::iterator	iter = m_ObjectList.begin();
		FObjectList::iterator	iterEnd = m_ObjectList.end();

		for (; iter != iterEnd; iter++)
		{
			objects.push_back((*iter));
		}

		ar.Serialize("GameObjects", objects);
	}


	return true;
}
