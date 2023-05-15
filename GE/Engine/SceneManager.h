#pragma once
#include "Assets/Scene/Scene.h"

class SceneManager : public XObject
{
	friend class Globals;
	friend class Engine;

private:
	SceneManager();
	virtual ~SceneManager();

private:
	void Initialize();

	virtual void PreUpdate(float DeltaSeconds)		override;

	virtual void Update(float DeltaSeconds)			override;

	virtual void PostUpdate(float DeltaSeconds)		override;



public:
	/*InScene 은 다음장면으로 하기를 원하는 장면이다.*/
	void CreateNextScene(Scene* InScene, bool AutoChange = true);
	void ChangeNextScene();

public:
	/* Call BeginPlay automatically when SceneChanged */
	void SetAutoPlay(bool AutoPlay);



public:
	class Scene* GetCurrentScene() const;
	void BindSceneChanged(std::function<void(class Scene*)> OnChangedFunc);
	void ClearBinds();

private:
	TSharedPtr<Scene>	m_CurrentScene;
	TSharedPtr<Scene>	m_NextScene;

	TSharedPtr<Scene>	m_DefaultUntitled;

	std::function<void(class Scene*)>	m_OnSceneChanged;

	bool				m_AutoPlay;
};

