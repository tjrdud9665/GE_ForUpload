#pragma once
#include "InteractableObject.h"


class TeleportObject :public InteractableObject
{
	CLASS(TeleportObject, InteractableObject)

	friend class Scene;
	friend class Globals;

protected:
	TeleportObject();
	TeleportObject(const TeleportObject& Other);
	virtual ~TeleportObject();

public:
	virtual void Construct(const std::string& InName = "") override;

	virtual void Update(float DeltaSeconds) override;

	virtual void BeginPlay() override;

public:
	/* Interface of Interactables */
	virtual void OnBeginInteractable(class IInteractableInterface* InteractionObject)  override;

	virtual void OnEndInteractable()													override;

	virtual void OnInteraction()														override;


public:
	void ActivateTeleport();

private:
	void TeleportScene();

	//void MakeLoadingGameObjectList(Archive& ar , class LoadingThread* loadingThread);



protected:
	class StaticMeshComponent* m_StaticMeshCom;

	PROP(TSharedPtr<class Scene>,  m_Destionation,  DetailsEdit)
	TSharedPtr<class Scene>	 m_Destionation;

	bool			m_Interaction;
	bool			m_ShinyReverse;
	float			m_ShinyTime;

	PROP(float, m_ShinyValue, DetailsEdit)
	float			m_ShinyValue;

	bool			m_Activate;

protected:
	PROP(TSubclassOf<class UserWidget>, m_InteractionWidgetClass, DetailsEdit)
	TSubclassOf<class UserWidget>		m_InteractionWidgetClass;

	class UserWidget* m_WidgetInstance;

private:
	void Shiny();

};


