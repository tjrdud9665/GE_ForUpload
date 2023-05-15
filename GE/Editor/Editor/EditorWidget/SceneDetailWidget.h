#pragma once
#include "EditorWidget.h"

class SceneDetailWidget :public EditorWidget
{
	CLASS(SceneDetailWidget,EditorWidget)


protected:
	SceneDetailWidget();
	SceneDetailWidget(const SceneDetailWidget& Other);
	virtual ~SceneDetailWidget();

	virtual void Construct(const std::string& InName /* = "" */) override;

public:
	void SetDetailTarget(class XObject* InTarget);
	void ClearDetailTarget();

	class ComponentDetails* GetComponentDetail() const;

	class SceneComponent* GetGuizmoTarget();


private:
	class ComponentDetails* m_ComponentDetails;

	class PropertySet* m_PropertSet;
};

