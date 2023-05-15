#pragma once
class IInteractableInterface
{
public:
	/* Interface of Interactable ~~*/
	virtual bool IsInteractable() const PURE;

	virtual void OnBeginInteractable(class IInteractableInterface* InteractionObject)	PURE;

	virtual void OnEndInteractable()	PURE;

	virtual void OnInteraction()		PURE;
};

