#pragma once

#include "InteractableObject.h"
#include "Assets/Widget/UserWidget.h"



class UIInteractableObject :public InteractableObject
{
	CLASS(UIInteractableObject, InteractableObject)

	friend class Scene;
	friend class Globals;

protected:
	UIInteractableObject();
	UIInteractableObject(const UIInteractableObject& Other);
	virtual ~UIInteractableObject();

public:
	virtual void Construct(const std::string& InName = "") override;

	virtual void Update(float DeltaSeconds) override;

	virtual void BeginPlay() override;

public:
	/* Interface of Interactables */
	virtual void OnBeginInteractable(class IInteractableInterface* InteractionObject)  override;

	virtual void OnEndInteractable()													override;

	virtual void OnInteraction()														override;

protected:
	class StaticMeshComponent* m_StaticMeshCom;

	bool			m_Interaction;



	bool			m_ShinyReverse;
	float			m_ShinyTime;
	float			m_ShinyValue;

	PROP(bool, m_Shiny, DetailsEdit)
	bool	m_Shiny;

	PROP(TSubclassOf<UserWidget>, m_UIWidget, DetailsEdit)
	TSubclassOf<UserWidget>	m_UIWidget;

	class UserWidget* m_UI;

private:
	void Shiny();

};

