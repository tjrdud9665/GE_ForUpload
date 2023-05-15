#pragma once

#include "GameFramework/GameObject/GameObject.h"
#include "IInteractableInterface.h"


//Don't Spawn this Object.

class InteractableObject :public GameObject, public IInteractableInterface
{
	CLASS(InteractableObject, GameObject, Abstract)

protected:
	InteractableObject();
	InteractableObject(const InteractableObject& Other);
	virtual ~InteractableObject();

protected:
	/* Interface of	GameObject */
	virtual void Construct(const std::string& InName = "") override;

	virtual void BeginPlay() override;

	virtual void OnDestroy() override;



public:
	/* Interface of Interactable ~~*/
	virtual bool IsInteractable()												  const override;

	virtual void OnBeginInteractable(class IInteractableInterface* InteractionObject)	override;

	virtual void OnEndInteractable()													override;

	virtual void OnInteraction()														override;

private:
	/*Collision Bind Functions */
	void OnBeginOverlapInteractionBoudns(const SCollisionResult& InResult);

	void OnEndOverlapInteractionBoudns(const SCollisionResult& InResult);



protected:
	TSharedPtr<class BoxComponent>	m_InteractionBounds;

	bool								m_IsBeganInteraction;

	TSharedPtr<XObject>					m_InteractionObject;

};

