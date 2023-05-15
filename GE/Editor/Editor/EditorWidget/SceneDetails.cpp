#include "PCH.h"
#include "SceneDetails.h"
#include "ComponentDetails.h"
#include "GameFramework/GameObject/GameObject.h"
#include "PropertySet.h"
#include "GameFramework/Components/SceneComponent.h"
#include "SceneDetailWidget.h"

SceneDetails::SceneDetails()
{

}

SceneDetails::SceneDetails(const SceneDetails& Other)
	:Super(Other)
{

}

SceneDetails::~SceneDetails()
{

}

void SceneDetails::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);

	m_SceneDetailWidget= AddNew<SceneDetailWidget>("SceneDetailWidget");

	//m_ComponentDetials = AddNew<ComponentDetails>("ComponentDetails");

	//m_PropertSet = AddNew<PropertySet>("PropertySet");
}

void SceneDetails::SetDetailTarget(class XObject* InTarget)
{	
	m_SceneDetailWidget->SetDetailTarget(InTarget);
}

void SceneDetails::ClearDetailTarget()
{
	m_SceneDetailWidget->ClearDetailTarget();
}

class ComponentDetails* SceneDetails::GetComponentDetail() const
{
	return m_SceneDetailWidget->GetComponentDetail();
}

class SceneComponent* SceneDetails::GetGuizmoTarget()
{
	return m_SceneDetailWidget->GetGuizmoTarget();
}
	
