#pragma once
#include "EditorWidget.h"
/**
 * SceneComponent Hierarchy 및 ObjectComponent를 ListBox로 나열합니다.
 * 
 */
class ComponentDetails :public EditorWidget
{
	CLASS(ComponentDetails,EditorWidget)

protected:
	ComponentDetails();
	ComponentDetails(const ComponentDetails& Other);
	virtual ~ComponentDetails();

	virtual void Update(float DeltaSeconds) override;

private:
	void BuildComponentHierarchy(class SceneComponent* ParentComp);

	void BuildComponentNonHierarchy(class ObjectComponent* ObjectComp);


	void HandleClick();


public:
	void SetEditTarget(class GameObject* EditTarget);
	void SetEditTarget_GameObjectOnly(class GameObject* EditTarget ,  XObject* clickedComp);
	XObject* GetCurrentEditTarget() const;

private:
	TSharedPtr<class GameObject>	m_EditTarget;

	class XObject* m_Hovered;
	class XObject* m_Clicked;
};


