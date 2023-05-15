#include "EnginePCH.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "CameraManager.h"
#include "../../SceneViewport.h"

#include "GameFramework/Components/PrimitiveComponent.h"
#include "Navigation/NavigationSystem.h"
#include <Physics/BulletPhysicsHelper.h>


Scene::Scene()
	:m_Change(false)
	,m_Began(false)
	,m_Ready(false)
{
	
}

Scene::Scene(const Scene& Other)
	:Super(Other)
	, m_Ready(false)
	, m_Change(false)
	, m_Began(false)
{

}

Scene::~Scene()
{
	if (g_Engine)
	{
		if(g_Engine->Get<ObjectManager>() == m_ObjectManager)
			g_Engine->Set<ObjectManager>(nullptr);
		if (g_Engine->Get<CameraManager>() == m_CameraManager)
			g_Engine->Set<CameraManager>(nullptr);		
		if (g_Engine->Get<SceneViewport>() == m_Viewport)
			g_Engine->Set<SceneViewport>(nullptr);
	}
	SAFE_RELEASE(m_ObjectManager);	
	SAFE_RELEASE(m_CameraManager);
	SAFE_RELEASE(m_Viewport);
	

}

bool Scene::Serialize(Archive& ar)
{
	bool bResult =Super::Serialize(ar);

	if (m_ObjectManager)
	{
		bResult &= m_ObjectManager->Serialize(ar);
	}
	Json data = GetData();
	ar.Serialize("scene", data);

	return bResult;
}

void Scene::Construct(const std::string& InName /*= ""*/)
{
	Super::Construct(InName);

	if (!m_Ready)
	{
		m_ObjectManager = Globals::CreateEngineObject<ObjectManager>();
		m_ObjectManager->AddRef();

		//m_SceneResource = Globals::CreateEngineObject<SceneResource>();
		//m_SceneResource->AddRef();

		m_CameraManager = Globals::CreateEngineObject<CameraManager>();
		m_CameraManager->AddRef();	

		m_Viewport = Globals::CreateEngineObject<SceneViewport>();
		m_Viewport->AddRef();

		//m_NavManager = Globals::CreateEngineObject<NavigationManager>();
		//m_NavManager->AddRef();


		Initialize();
		m_Ready = true;
	}
}

class SceneInfo* Scene::GetSceneInfo() const
{
	return m_SceneInfo;
}

void Scene::Initialize()
{
	m_ObjectManager->Initialize(this);
	m_Viewport->Initialize(this);
	m_CameraManager->Initialize();
	
}

void Scene::BeginScene(Scene* prevScene)
{
	m_Began = false;

	if (!m_Ready)
	{			
		
		SAFE_RELEASE(m_ObjectManager);
		SAFE_RELEASE(m_CameraManager);
		SAFE_RELEASE(m_Viewport);

		m_ObjectManager = Globals::CreateEngineObject<ObjectManager>();
		m_ObjectManager->AddRef();

		m_CameraManager = Globals::CreateEngineObject<CameraManager>();
		m_CameraManager->AddRef();

		m_Viewport = Globals::CreateEngineObject<SceneViewport>();
		m_Viewport->AddRef();

		m_Ready = true;
		Initialize();
	}	

	g_Engine->Set<ObjectManager>(m_ObjectManager);
	g_Engine->Set<CameraManager>(m_CameraManager);	
	g_Engine->Set<SceneViewport>(m_Viewport);

	Json loadData = GetData();
	JsonArchive ar(loadData);
	Serialize(ar);

	//이전 장면의 오브젝트를 
	if (prevScene)
	{
		if (prevScene != this)
		{
			SAFE_RELEASE(prevScene->m_ObjectManager);		
			SAFE_RELEASE(prevScene->m_CameraManager);			
			SAFE_RELEASE(prevScene->m_Viewport);
			prevScene->m_Ready = false;
		}
	}

}

void Scene::BeginPlay()
{
	m_Began = true;

	m_ObjectManager->BeginPlay();
	m_CameraManager->BeginPlay();
	m_Viewport->BeginPlay();
	g_Engine->Get<NavigationSystem>()->BeginPlay();


	if (m_SceneInfoClass)
		m_SceneInfo = Globals::CreateX<SceneInfo>(m_SceneInfoClass, this);


	if (m_SceneInfo)
		m_SceneInfo->BeginPlay();
}

void Scene::EndPlay()
{
	m_Began = false;
	m_ObjectManager->EndPlay();
	m_CameraManager->EndPlay();
	m_Viewport->EndPlay();

	if (m_SceneInfo)
		m_SceneInfo->EndPlay();

	g_Engine->Get<NavigationSystem>()->EndPlay();
}

void Scene::EndScene()
{
	EndPlay();
	g_Engine->Set<ObjectManager>(nullptr);	
	g_Engine->Set<CameraManager>(nullptr);

	g_Engine->Set<SceneViewport>(nullptr);
	//g_Engine->Set<NavigationManager>(nullptr);
	SAFE_RELEASE(m_ObjectManager);	
	SAFE_RELEASE(m_CameraManager);	
	SAFE_RELEASE(m_Viewport);

	//m_SceneInfo = nullptr;
	m_Ready = false;
	//
	
}

void Scene::Update(float DeltaSeconds)
{
	/* Maybe Run Only SceneInfo Class */

	if (m_SceneInfo)
		m_SceneInfo->Update(DeltaSeconds);
}

void Scene::PostUpdate(float DeltaSeconds)
{
	/* Maybe Run Only SceneInfo Class */
	if (m_SceneInfo)
		m_SceneInfo->PostUpdate(DeltaSeconds);
}

void Scene::PreUpdate(float DeltaSeconds)
{
	/* Maybe Run Only SceneInfo Class */
	if (m_SceneInfo)
		m_SceneInfo->PreUpdate(DeltaSeconds);
}

bool Scene::IsBegan() const
{
	return m_Began;
}



