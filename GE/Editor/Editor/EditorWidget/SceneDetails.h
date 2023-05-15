#pragma once

#include "EditorWindow.h"

class SceneDetails :public EditorWindow
{
	CLASS(SceneDetails, EditorWindow)
	
protected:
	SceneDetails();
	SceneDetails(const SceneDetails& Other);
	virtual ~SceneDetails();
	
	virtual void Construct(const std::string& InName /* = "" */) override;

public:
	void SetDetailTarget(class XObject* InTarget);
	void ClearDetailTarget();

	class ComponentDetails* GetComponentDetail() const;

	class SceneComponent* GetGuizmoTarget();


private:
	class SceneDetailWidget* m_SceneDetailWidget;
	
	
};

