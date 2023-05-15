#pragma once

#include "../AssetData.h"
#include "../SceneInfo/SceneInfo.h"

class Scene :public AssetData
{
	CLASS(Scene,AssetData)
	friend class SceneManager;
	friend class ObjectManager;
	friend class EditorManager;
	

private:
	Scene();
	Scene(const Scene& Other);
	virtual ~Scene();


	virtual bool Serialize(Archive& ar) override;


	virtual void Construct(const std::string& InName = "") override;

public:
	class SceneInfo* GetSceneInfo() const;


public:
	void Initialize();

	void BeginScene(Scene* prevScene);

	void BeginPlay();

	void EndPlay();


	void EndScene();

	virtual void Update(float DeltaSeconds)					override;

	virtual void PostUpdate(float DeltaSeconds)				override;

	virtual void PreUpdate(float DeltaSeconds)				override;


	class CameraManager* GetCameraManager() const
	{
		return m_CameraManager;
	}

	class ObjectManager* GetObjectManager() const
	{
		return m_ObjectManager;
	}

	class SceneViewport* GetSceneViewport() const
	{
		return m_Viewport;
	}


	bool IsBegan() const;




private:
	bool	m_Change;
	bool	m_Began;
	bool	m_Ready;

	PROP(TSubclassOf<SceneInfo>, m_SceneInfoClass, DetailsEdit)
		TSubclassOf<SceneInfo>		m_SceneInfoClass;

	PROP(TSharedPtr<SceneInfo>, m_SceneInfo, Transient)
		TSharedPtr<SceneInfo>	m_SceneInfo;


private:
	class ObjectManager* m_ObjectManager;	
	class CameraManager* m_CameraManager;

	class SceneViewport* m_Viewport;

	
};

