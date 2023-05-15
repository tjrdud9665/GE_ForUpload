#include "PCH.h"
#include "InteractableObject.h"
#include "GameFramework/Components/SphereComponent.h"
#include "GameFramework/Components/PrimitiveComponent.h"
#include "GameFramework/Components/ShapeComponent.h"
#include "GameFramework/Components/BoxComponent.h"


InteractableObject::InteractableObject()
	:m_IsBeganInteraction(false)
{
	m_InteractionObject = nullptr;
}

InteractableObject::InteractableObject(const InteractableObject& Other)
	: Super(Other)
	, m_IsBeganInteraction(false)
	, m_InteractionObject(nullptr)
{
	m_InteractionBounds = FindComponent<BoxComponent>("InteractionBounds");

}

InteractableObject::~InteractableObject()
{
}

void InteractableObject::Construct(const std::string& InName /*= "" */)
{
	Super::Construct(InName);

	m_InteractionBounds = CreateComponent<BoxComponent>("InteractionBounds");

	SetRootComponent(m_InteractionBounds);

}

void InteractableObject::BeginPlay()
{
	Super::BeginPlay();

	m_InteractionBounds->BindOnBeginOverlap(std::bind(&InteractableObject::OnBeginOverlapInteractionBoudns, this, std::placeholders::_1));
	m_InteractionBounds->BindOnEndOverlap(std::bind(&InteractableObject::OnEndOverlapInteractionBoudns, this, std::placeholders::_1));
}

void InteractableObject::OnDestroy()
{
	//Remove Binds
	m_IsBeganInteraction = false;

	IInteractableInterface* Interactable = (IInteractableInterface*)(m_InteractionObject.Get());
	if (Interactable)
	{
		Interactable->OnEndInteractable();
	}
	m_InteractionObject = nullptr;
	m_InteractionBounds->ClearBeginOverlap();
	m_InteractionBounds->ClearEndOverlap();

	Super::OnDestroy();
}

bool InteractableObject::IsInteractable() const
{
	return !m_IsBeganInteraction;
}

void InteractableObject::OnBeginInteractable(class IInteractableInterface* InteractionObject)
{
	//Some Work
}

void InteractableObject::OnEndInteractable()
{
	//Some Work
}

void InteractableObject::OnInteraction()
{

}

void InteractableObject::OnBeginOverlapInteractionBoudns(const SCollisionResult& InResult)
{
	//이미 인터렉션중이라면	
	IInteractableInterface* Interactable = InResult.Dest->GetOwner<IInteractableInterface>();

	if (Interactable)
	{
		if (Interactable->IsInteractable())
		{
			Interactable->OnBeginInteractable(this);
			OnBeginInteractable(Interactable);
			m_InteractionObject = InResult.Dest->GetOwner();
			m_IsBeganInteraction = true;
		}
	}


}

void InteractableObject::OnEndOverlapInteractionBoudns(const SCollisionResult& InResult)
{
	m_IsBeganInteraction = false;
	OnEndInteractable();
	IInteractableInterface* Interactable = dynamic_cast<IInteractableInterface*>(m_InteractionObject.Get());
	if (Interactable)
	{
		Interactable->OnEndInteractable();
	}

	m_InteractionObject = nullptr;

}
