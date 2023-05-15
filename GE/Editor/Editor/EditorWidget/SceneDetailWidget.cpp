#include "PCH.h"
#include "SceneDetailWidget.h"
#include "GameFramework/GameObject/GameObject.h"
#include "ComponentDetails.h"
#include "PropertySet.h"

SceneDetailWidget::SceneDetailWidget()
{
	
}

SceneDetailWidget::SceneDetailWidget(const SceneDetailWidget& Other)
	:Super(Other)
{

}

SceneDetailWidget::~SceneDetailWidget()
{

}

void SceneDetailWidget::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);
	
	m_ComponentDetails = AddNew<ComponentDetails>("ComponentDetails");

	m_PropertSet = AddNew<PropertySet>("PropertySet");
}

void SceneDetailWidget::SetDetailTarget(class XObject* InTarget)
{
	//Notify Null Target
	if (!InTarget)
	{
		//m_PropertSet->SetPropertyTarget(m_ComponentDetails->GetCurrentEditTarget());
		m_PropertSet->SetPropertyTarget(nullptr);

	} else
	{
		if (InTarget->GetClass()->IsChildOf(GameObject::StaticClass()))
		{
			m_ComponentDetails->SetEditTarget((GameObject*)InTarget);
		}
		else if(InTarget->GetOwner()->GetClass()->IsChildOf(GameObject::StaticClass()))
		{
			m_ComponentDetails->SetEditTarget_GameObjectOnly((GameObject*)InTarget->GetOwner(),InTarget);
		}
		m_PropertSet->SetPropertyTarget(InTarget);
	}

}

void SceneDetailWidget::ClearDetailTarget()
{
	m_ComponentDetails->SetEditTarget(nullptr);
	m_PropertSet->SetPropertyTarget(nullptr);
}

class ComponentDetails* SceneDetailWidget::GetComponentDetail() const
{
	return m_ComponentDetails;
}

class SceneComponent* SceneDetailWidget::GetGuizmoTarget()
{
	XObject* CurrentEditTarget = m_PropertSet->GetPropertyTarget();
	if (!CurrentEditTarget)
	{
		CurrentEditTarget = m_ComponentDetails->GetCurrentEditTarget();
		if (!CurrentEditTarget)
			return nullptr;
	}


	if (CurrentEditTarget->GetClass()->IsChildOf(GameObject::StaticClass()) ||
		CurrentEditTarget->GetClass()->IsA(GameObject::StaticClass()))
	{
		GameObject* go = (GameObject*)CurrentEditTarget;
		return go->GetRootComponent();
	} else if (CurrentEditTarget->GetClass()->IsChildOf(SceneComponent::StaticClass()) ||
			   CurrentEditTarget->GetClass()->IsA(SceneComponent::StaticClass()))
	{
		return (SceneComponent*)CurrentEditTarget;
	}

	//Can't Grab SceneComponent
	return nullptr;
}
