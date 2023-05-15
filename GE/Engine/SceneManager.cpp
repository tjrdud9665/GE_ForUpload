#include "EnginePCH.h"
#include "SceneManager.h"
#include "Path/PathManager.h"
#include "AssetManager.h"

SceneManager::SceneManager()
	:m_NextScene(nullptr)
	, m_CurrentScene(nullptr)
	, m_AutoPlay(false)
{

}

SceneManager::~SceneManager()
{	
}

void SceneManager::Initialize()
{
	
	const SPathInfo* ScenePath = g_Engine->Get<PathManager>()->FindPath(SCENE_PATH);
	fs::path untitled = ScenePath->aPath;
	untitled = untitled.parent_path();
	untitled /= "untitled.meta";
	
	m_DefaultUntitled = (Scene*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(untitled.string()), Scene::StaticClass());
	//m_DefaultUntitled = Globals::CreateX<Scene>(this,"untitled");	
	CreateNextScene(m_DefaultUntitled);
}

void SceneManager::PreUpdate(float DeltaSeconds)
{
	if (m_CurrentScene)
		m_CurrentScene->PreUpdate(DeltaSeconds);
}

void SceneManager::Update(float DeltaSeconds)
{
	Scene* prev = nullptr;
	if (m_NextScene)
	{
		if (m_NextScene->m_Change)
		{
			if (m_CurrentScene)
			{
				m_CurrentScene->EndScene();
			}


			prev = m_CurrentScene;
			m_CurrentScene = m_NextScene;
			m_NextScene->BeginScene(prev);		

			m_NextScene = nullptr;

			//m_CurrentScene->m_SceneViewport
			//g_Engine->Get<UIManager>()->SetSceneViewport(m_CurrentScene->m_SceneViewport);


			if (m_OnSceneChanged)
				m_OnSceneChanged(m_CurrentScene);
		}

	}

	if (m_CurrentScene)
	{
		if (!m_CurrentScene->m_Began && m_AutoPlay)
		{
			m_CurrentScene->BeginPlay();
		}

		m_CurrentScene->Update(DeltaSeconds);
	}
}

void SceneManager::PostUpdate(float DeltaSeconds)
{
	if (m_CurrentScene)
		m_CurrentScene->PostUpdate(DeltaSeconds);
}

void SceneManager::CreateNextScene(Scene* InScene, bool AutoChange /*= true*/)
{
	if (m_DefaultUntitled != m_NextScene)
		SAFE_RELEASE(m_NextScene);

	m_NextScene = InScene;

	if (!m_NextScene)
	{
		m_NextScene = Globals::CreateX<Scene>(this, "untitled");	
		m_NextScene->Initialize();
	}


	m_NextScene->m_Change = AutoChange;
}

void SceneManager::ChangeNextScene()
{
	m_NextScene->m_Change = true;
}

void SceneManager::SetAutoPlay(bool AutoPlay)
{
	m_AutoPlay = AutoPlay;
}

class Scene* SceneManager::GetCurrentScene() const
{
	return m_CurrentScene;
}

void SceneManager::BindSceneChanged(std::function<void(class Scene*)> OnChangedFunc)
{
	m_OnSceneChanged = OnChangedFunc;
}

void SceneManager::ClearBinds()
{
	m_OnSceneChanged = nullptr;
}
