#pragma once
#include "EditorWindow.h"
class SceneInfoEditor : public EditorWindow
{
	CLASS(SceneInfoEditor, EditorWindow)

public:
	SceneInfoEditor();
	virtual ~SceneInfoEditor();

	virtual void Build() override;

	void SetEditSceneInfo(class SceneInfo* InSceneInfo);

	virtual void Update(float DeltaSeconds) override;

private:
	void ShowMenu();



private:
	class SceneInfo* m_TargetSceneInfo;

	class PropertySet* m_PropertySet;
};

