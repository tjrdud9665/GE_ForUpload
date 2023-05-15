#pragma once
#include "Editor.h"

class SceneEditor :public Editor
{
	CLASS(SceneEditor,Editor)	
	

protected:
	SceneEditor();
	SceneEditor(const SceneEditor& Other);
	virtual ~SceneEditor();

public:

	virtual void Construct(const std::string& InName /* = "" */) override;

	virtual void ExpandViewsMenu() override;
	
	virtual void Update(float DeltaSeconds) override;

	
	

public:
	class SceneViewportWidget* GetSceneViewport() const { return m_SceneViewport; }

	class SceneOutliner* GetSceneOutliner() const { return m_SceneOutliner; }

	class SceneDetails* GetSceneDetails() const { return m_SceneDetails; }

	
private:
	void OnSceneChanged(class Scene* changedScene);


private:
	TSharedPtr<class SceneViewportWidget>		m_SceneViewport;

	TSharedPtr<class SceneOutliner>		m_SceneOutliner;

	TSharedPtr<class SceneDetails>		m_SceneDetails;

	TSharedPtr<class ObjectPlacement>	m_ObjectPlacement;

	TSharedPtr<class NavTool>			m_NavToolWindow;

	TSharedPtr<class SceneInfoSetting>	m_SceneInfoSettings;




};

