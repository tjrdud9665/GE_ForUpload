#pragma once

#include "EditorWindow.h"

//SceneViewport �� �غ�� Scene �� SceneInfo �� Prop �� ǥ��

class SceneInfoSetting :public EditorWindow
{
	CLASS(SceneInfoSetting, EditorWindow)
	
	friend class SceneEditor;
	

protected:
	SceneInfoSetting();	
	virtual ~SceneInfoSetting();

	virtual void Construct(const std::string& InName /* = "" */) override;

	void SetCurrentScene(class Scene* InScene);

private:
	class PropertySet* m_ScenePropSet;
	class Scene* m_CurrentScene;
};

